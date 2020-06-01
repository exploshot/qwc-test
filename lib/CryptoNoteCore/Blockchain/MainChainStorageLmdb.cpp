// Copyright (c) 2019, The TurtleCoin Developers
// Copyright (c) 2018-2020, The Qwertycoin Project
//
// This file is part of Qwertycoin.
//
// Qwertycoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Qwertycoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Qwertycoin.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <fstream>
#include <string_view>
#include <exception>

#include <lmdb/lmdbpp.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <Common/CryptoNoteTools.h>
#include <Common/FileSystemShim.h>

#include <CryptoNoteCore/Blockchain/MainChainStorageLmdb.h>

#include <Global/Constants.h>
#include <Global/LMDBConfig.h>

using namespace rapidjson;
using namespace CryptoNote;
using namespace LMDB;

namespace CryptoNote {
    MainChainStorageLmdb::MainChainStorageLmdb(const std::string &blocksFilename,
                                               const std::string &indexesFilename)
    {
        /*!
            store db filename, will be used later for checking/resizing mapsize
        */
        m_dbpath = fs::path (blocksFilename);

        /*!
            create db file if not already exists
        */
        if (!fs::ifstream (m_dbpath)) {
            fs::ofstream file (m_dbpath);
            if (!file) {
                throw std::runtime_error ("Failed to create db file");
            }
            file.close ();
        }

        /*!
            set initial mapsize
        */
        size_t mapsize = fs::file_size (m_dbpath);
        if (mapsize == 0) {
            /*!
                starts with 64M
            */
            mapsize += MAPSIZE_MIN_AVAIL;
        }

        m_db.set_mapsize (mapsize);

        /*!
            open database
        */
        try {
            m_db.open (blocksFilename.c_str (),
                       MDB_NOSUBDIR | MDB_NOMETASYNC | MDB_WRITEMAP | MDB_MAPASYNC | MDB_NORDAHEAD,
                       0664);
        } catch (std::exception &e) {
            throw std::runtime_error ("Failed to create database: " + std::string (e.what ()));
        }

        /*!
            prepare tx handle
        */
        lmdb::txn_begin (m_db, nullptr, MDB_RDONLY, &rtxn);
        lmdb::txn_begin (m_db, nullptr, 0, &wtxn);

        /*!
            initialize blockcount cache counter
        */
        initializeBlockCount ();
        m_dirty = 0;
    }

    MainChainStorageLmdb::~MainChainStorageLmdb()
    {
        if (rtxn != nullptr) {
            lmdb::txn_abort (rtxn);
        }

        if (wtxn != nullptr) {
            lmdb::txn_commit (wtxn);
        }

        m_db.sync ();
    }

    void MainChainStorageLmdb::pushBlock(const RawBlock &rawBlock)
    {

        /*!
            only commit every max_insert
        */
        if (m_dirty == MAX_DIRTY) {
            /*!
                reset commit counter
            */
            m_dirty = 0;

            /*!
                commit all pending transactions
            */
            lmdb::txn_commit (wtxn);

            /*!
                flush to disk (only when using MDB_NOSYNC)
                m_db.sync();
            */

            /*!
                resize when needed
            */
            checkResize ();

            /*!
                recreate write tx handle
            */
            lmdb::txn_begin (m_db, nullptr, 0, &wtxn);
        }

        /*!
            stringify RawBlock for storage 
        */
        StringBuffer rblock;
        Writer<StringBuffer> writer (rblock);
        rawBlock.toJSON (writer);

        { // open lmdb cursor
            lmdb::dbi dbi = lmdb::dbi::open (wtxn, nullptr);

            dbi.put (wtxn, std::to_string (m_blockcount), rblock.GetString ());

            if (m_blockcount == 0) {
                lmdb::txn_commit (wtxn);
                m_db.sync ();
                lmdb::txn_begin (m_db, nullptr, 0, &wtxn);
            }

            /*!
                increment cached block count couter
            */
            m_blockcount++;
        } // close lmdb cursor

        m_dirty++;
    }

    void MainChainStorageLmdb::popBlock()
    {
        lmdb::dbi dbi;

        renewRoTxn ();

        { // open lmdb cursor
            lmdb::dbi dbi = lmdb::dbi::open (rtxn, nullptr);
            auto cursor = lmdb::cursor::open (wtxn, dbi);
            std::string_view key, val;

            if (cursor.get (key, val, MDB_LAST)) {
                cursor.del ();
                cursor.close ();

                lmdb::txn_commit (wtxn);
                lmdb::txn_begin (m_db, nullptr, 0, &wtxn);
            } else {
                cursor.close ();
            }
        } // close lmdb cursor
    }

    RawBlock MainChainStorageLmdb::getBlockByIndex(const uint32_t index)
    {
        bool found = false;

        renewRoTxn ();

        lmdb::dbi dbi;
        RawBlock rawBlock;

        { // open lmdb cursor
            try {
                dbi = lmdb::dbi::open (rtxn, nullptr);
            } catch (const std::exception &e) {
                try {
                    lmdb::txn_commit (wtxn);
                } catch (...) {

                }

                throw std::runtime_error ("Could not find block in cache for given blockIndex: " +
                                          std::string (e.what ()));
            }

            std::string_view val;
            if (dbi.get (rtxn, std::to_string (index), val)) {
                Document doc;
                if (!doc.Parse<0> (std::string (val)).HasParseError ()) {
                    rawBlock.fromJSON (doc);
                    found = true;
                } else {
                    std::cout
                        << doc.GetParseError ()
                        << std::endl;
                }
            }
        } // close lmdb cursor

        if (!found) {
            try {
                lmdb::txn_commit (wtxn);
            } catch (...) {
            }
            throw std::runtime_error ("Could not find block in cache for given blockIndex: " +
                                      std::to_string (index));
        }

        return rawBlock;
    }

    uint32_t MainChainStorageLmdb::getBlockCount() const
    {
        return m_blockcount;
    }

    void MainChainStorageLmdb::initializeBlockCount()
    {
        m_blockcount = 0;

        renewRoTxn ();

        { // open lmdb cursor
            lmdb::dbi dbi = lmdb::dbi::open (rtxn, nullptr);
            MDB_stat stat = dbi.stat (rtxn);
            m_blockcount = stat.ms_entries;
        } // close lmdb cursor
    }

    void MainChainStorageLmdb::clear()
    {
        throw std::runtime_error ("NotImplemented");

        { // open lmdb cursor
            lmdb::dbi dbi = lmdb::dbi::open (wtxn, nullptr);
            dbi.drop (wtxn, 0);
            lmdb::txn_commit (wtxn);
            lmdb::txn_begin (m_db, nullptr, 0, &wtxn);
        } // close lmdb cursor
    }

    void MainChainStorageLmdb::renewRoTxn()
    {
        try {
            lmdb::txn_reset (rtxn);
        } catch (...) {

        }

        try {
            lmdb::txn_renew (rtxn);
        } catch (...) {

        }
    }

    void MainChainStorageLmdb::renewRwTxn(bool sync)
    {
        if (wtxn == nullptr) {
            lmdb::txn_begin (m_db, nullptr, 0, &wtxn);
            return;
        }

        try {
            lmdb::txn_commit (wtxn);
        } catch (...) {
        }

        if (sync) {
            m_db.sync (false);
        }

        try {
            lmdb::txn_begin (m_db, nullptr, 0, &wtxn);
        } catch (...) {
        }
    }

    void MainChainStorageLmdb::checkResize()
    {
        /*!
            assumed to be called after all tx has been commited
        */
        size_t size_avail = 0;
        size_t mapsize = 0;

        /*!
            reset/renew ro cursor
        */
        renewRoTxn ();

        { // open lmdb cursor
            lmdb::dbi dbi = lmdb::dbi::open (rtxn, nullptr);
            MDB_stat stat = dbi.stat (rtxn);
            MDB_envinfo info;
            lmdb::env_info (m_db, &info);
            mapsize = info.me_mapsize;
            size_avail = mapsize - (stat.ms_psize * info.me_last_pgno);
        } // close lmdb cursor

        if (size_avail > MAPSIZE_MIN_AVAIL) {
            return;
        }

        /*!
            flush to disk (only when NOT using MDB_NOSYNC flag)
        */
        m_db.sync (true);

        mapsize += 1ULL
            << SHIFTING_VAL;
        m_db.set_mapsize (mapsize);
    }

    std::unique_ptr<IMainChainStorage> createMainChainStorageLmdb(const std::string &dataDir,
                                                                         const Currency &currency)
    {
        fs::path blocksFilename = fs::path (dataDir) / currency.blocksFileName ();
        fs::path indexesFilename = fs::path (dataDir) / currency.blockIndexesFileName ();

        auto storage = std::make_unique<MainChainStorageLmdb> (blocksFilename.string () +
                                                               ".lmdb",
                                                               indexesFilename.string ());

        if (storage->getBlockCount () == 0) {
            RawBlock genesisBlock;
            genesisBlock.block = toBinaryArray (currency.genesisBlock ());
            storage->pushBlock (genesisBlock);
        }

        return storage;
    }
} // namespace CryptoNote
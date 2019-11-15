// Copyright (c) 2019, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

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

namespace CryptoNote
{
MainChainStorageLmdb::MainChainStorageLmdb(const std::string &blocksFilename, const std::string &indexesFilename)
{
    // store db filename, will be used later for checking/resizing mapsize
    m_dbpath = fs::path(blocksFilename);

    // create db file if not already exists
    if (!std::ifstream(m_dbpath))
    {
        std::ofstream file(m_dbpath);
        if (!file)
        {
            throw std::runtime_error("Failed to create db file");
        }
        file.close();
    }

    // set initial mapsize
    size_t mapsize = fs::file_size(m_dbpath);
    if (mapsize == 0)
    {
        // starts with 128M
        mapsize += MAPSIZE_MIN_AVAIL;
    }

    m_db.set_mapsize(mapsize);
    // open database
    try
    {
        //m_db.open(blocksFilename.c_str(), MDB_NOSUBDIR|MDB_NORDAHEAD|MDB_NOMETASYNC|MDB_WRITEMAP|MDB_MAPASYNC, 0664);
        //m_db.open(blocksFilename.c_str(), MDB_NOSUBDIR|MDB_NOSYNC|MDB_WRITEMAP|MDB_MAPASYNC|MDB_NORDAHEAD, 0664);
        m_db.open(blocksFilename.c_str(), MDB_NOSUBDIR|MDB_NOMETASYNC|MDB_WRITEMAP|MDB_MAPASYNC|MDB_NORDAHEAD, 0664);
    }
    catch (std::exception &e)
    {
        throw std::runtime_error("Failed to create database: " + std::string(e.what()));
    }

    // prepare tx handle
    lmdb::txn_begin(m_db, nullptr, MDB_RDONLY, &rtxn);
    lmdb::txn_begin(m_db, nullptr, 0, &wtxn);

    // initialize blockcount cache counter
    initializeBlockCount();
    m_dirty = 0;
}

MainChainStorageLmdb::~MainChainStorageLmdb()
{
    //std::cout << "Closing blockchain db." << std::endl;
    if(rtxn != nullptr)
    {
        lmdb::txn_abort(rtxn);
    }

    if(wtxn != nullptr){
        lmdb::txn_commit(wtxn);
    }

    m_db.sync();
}

void MainChainStorageLmdb::pushBlock(const RawBlock &rawBlock)
{

    // only commit every max_insert
    if (m_dirty == MAX_DIRTY)
    {
        // reset commit counter
        m_dirty = 0;

        // commit all pending transactions
        lmdb::txn_commit(wtxn);

        // flush to disk (only when using MDB_NOSYNC)
        // m_db.sync();

        // resize when needed
        checkResize();

        // recreate write tx handle
        lmdb::txn_begin(m_db, nullptr, 0, &wtxn);
    }

    /* stringify RawBlock for storage **/
    StringBuffer rblock;
    Writer<StringBuffer> writer(rblock);
    rawBlock.toJSON(writer);

    {
        lmdb::dbi dbi = lmdb::dbi::open(wtxn, nullptr);

        dbi.put(wtxn, std::to_string(m_blockcount), rblock.GetString());

        if (m_blockcount == 0)
        {
            // renewRwTxn(true);
            lmdb::txn_commit(wtxn);
            m_db.sync();
            lmdb::txn_begin(m_db, nullptr, 0, &wtxn);
        }

        // increment cached block count couter
        m_blockcount++;
    }

    m_dirty++;
}

void MainChainStorageLmdb::popBlock()
{
    lmdb::dbi dbi;
    
    renewRoTxn();

    {
        lmdb::dbi dbi = lmdb::dbi::open(rtxn, nullptr);
        auto cursor = lmdb::cursor::open(wtxn, dbi);
        std::string_view key, val;
        if (cursor.get(key, val, MDB_LAST))
        {
            cursor.del();
            cursor.close();
            lmdb::txn_commit(wtxn);
            lmdb::txn_begin(m_db, nullptr, 0, &wtxn);
        }
        else
        {
            cursor.close();
        }
    }
}

RawBlock MainChainStorageLmdb::getBlockByIndex(const uint32_t index)
{
    bool found = false;

    renewRoTxn();

    lmdb::dbi dbi;
    RawBlock rawBlock;

    {
        try
        {
            dbi = lmdb::dbi::open(rtxn, nullptr);
        }
        catch(const std::exception& e)
        {
            try { lmdb::txn_commit(wtxn); } catch (...){}
            throw std::runtime_error("Could not find block in cache for given blockIndex: " + std::string(e.what()));
        }
            
        std::string_view val;
        if (dbi.get(rtxn, std::to_string(index), val))
        {
            Document doc;
            if (!doc.Parse<0>(std::string(val)).HasParseError() ) {
                rawBlock.fromJSON(doc);
                found = true;
            }else{
                std::cout << doc.GetParseError() << std::endl;
            }
        }
    }

    if (!found)
    {
        try
        {
            lmdb::txn_commit(wtxn);
        }
        catch (...)
        {
        }
        throw std::runtime_error("Could not find block in cache for given blockIndex: " + std::to_string(index));
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

    renewRoTxn();
    {
        lmdb::dbi dbi = lmdb::dbi::open(rtxn, nullptr);
        MDB_stat stat = dbi.stat(rtxn);
        m_blockcount = stat.ms_entries;
    }
}

void MainChainStorageLmdb::clear()
{
    throw std::runtime_error("NotImplemented");

    {
        lmdb::dbi dbi = lmdb::dbi::open(wtxn, nullptr);
        dbi.drop(wtxn, 0);
        lmdb::txn_commit(wtxn);
        lmdb::txn_begin(m_db, nullptr, 0, &wtxn);
    }
}

void MainChainStorageLmdb::renewRoTxn()
{
    /*
    if(rtxn == nullptr)
    {
        lmdb::txn_begin(m_db, nullptr, MDB_RDONLY, &rtxn);
        return;
    }

    try
    {
        lmdb::txn_reset(rtxn);
    }
    catch (...)
    {
    }

    try
    {
        lmdb::txn_renew(rtxn);
    }
    catch (...)
    {
    }
    */
    try{ lmdb::txn_reset(rtxn); } catch(...){}
    try{ lmdb::txn_renew(rtxn); } catch(...){}
}

void MainChainStorageLmdb::renewRwTxn(bool sync)
{
    if(wtxn == nullptr)
    {
        lmdb::txn_begin(m_db, nullptr, 0, &wtxn);
        return;
    }

    try
    {
        lmdb::txn_commit(wtxn);
    }
    catch (...)
    {
    }

    if (sync)
    {
        m_db.sync(false);
    }

    try
    {
        lmdb::txn_begin(m_db, nullptr, 0, &wtxn);
    }
    catch (...)
    {
    }
}

void MainChainStorageLmdb::checkResize()
{
    /** assumed to be called after all tx has been commited **/
    size_t size_avail = 0;
    size_t mapsize = 0;

    // reset/renew ro cursor
    renewRoTxn();

    {
        lmdb::dbi dbi = lmdb::dbi::open(rtxn, nullptr);
        MDB_stat stat = dbi.stat(rtxn);
        MDB_envinfo info;
        lmdb::env_info(m_db, &info);
        mapsize = info.me_mapsize;
        size_avail = mapsize - (stat.ms_psize * info.me_last_pgno);
    }

    if (size_avail > MAPSIZE_MIN_AVAIL)
    {
        return;
    }

    // flush to disk (only when NOT using MDB_NOSYNC flag)
        m_db.sync(true);

        mapsize += 1ULL << SHIFTING_VAL;
        m_db.set_mapsize(mapsize); 
}

std::unique_ptr<IMainChainStorage> createSwappedMainChainStorageLmdb(const std::string &dataDir, const Currency &currency)
{
    fs::path blocksFilename = fs::path(dataDir) / currency.blocksFileName();
    fs::path indexesFilename = fs::path(dataDir) / currency.blockIndexesFileName();

    auto storage = std::make_unique<MainChainStorageLmdb>(blocksFilename.string() + ".lmdb", indexesFilename.string());

    if (storage->getBlockCount() == 0)
    {
        RawBlock genesisBlock;
        genesisBlock.block = toBinaryArray(currency.genesisBlock());
        storage->pushBlock(genesisBlock);
    }

    return storage;
}
} // namespace CryptoNote
// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include <iostream>
#include <fstream>
#include <string_view>

#include <lmdb/lmdbpp.h>

#include <Common/FileSystemShim.h>

#include <CryptoNoteCore/Database/DatabaseConfig.h>
#include <CryptoNoteCore/Database/DatabaseErrors.h>
#include <CryptoNoteCore/Database/LmDBWrapper.h>

#include <Global/Constants.h>
#include <Global/LMDBConfig.h>

using namespace CryptoNote;
using namespace Logging;
using namespace LMDB;

LmDBWrapper::LmDBWrapper(std::shared_ptr<Logging::ILogger> logger)
    : logger (logger, "LmDBWrapper"),
      state (NOT_INITIALIZED)
{
}

LmDBWrapper::~LmDBWrapper()
{
    try {
        m_db.sync ();
        m_db.close ();
    } catch (...) {

    }
}

void LmDBWrapper::init(const DataBaseConfig &config)
{
    if (state.load () != NOT_INITIALIZED) {
        throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::ALREADY_INITIALIZED));
    }

    logger (INFO)
        << "Initializing DB using lmdb backend";

    /*!
        set m_dbDir & m_dbFile fore easy reuse
    */
    setDataDir (config);

    /*!
        create db dir if  not already exists (lmbd not automatically create it) 
    */
    if (!fs::exists (m_dbDir) || !fs::is_directory (m_dbDir)) {
        if (!fs::create_directory (m_dbDir)) {
            logger (ERROR)
                << "Failed to create db directory";
            throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::INTERNAL_ERROR));
        }
    }

    if (!std::ifstream (m_dbFile)) {
        std::ofstream file (m_dbFile);
        if (!file) {
            logger (ERROR)
                << "Failed to create db file";
            throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::INTERNAL_ERROR));
        }
        file.close ();
    }

    /*!
        set initial mapsize
    */
    uint64_t mapsize = 0;
    try {
        mapsize = fs::file_size (m_dbFile);
    } catch (...) {
    }

    if (mapsize == 0) {
        /*!
            starts with 64M
        */
        mapsize += MAPSIZE_MIN_AVAIL;
    }

    m_db.set_mapsize (mapsize);

    logger (DEBUGGING)
        << "Initial DB mapsize: "
        << mapsize
        << " bytes";

    logger (INFO)
        << "Opening DB in "
        << m_dbDir;
    try {
        m_db.open (m_dbDir.c_str (), MDB_NOSYNC | MDB_WRITEMAP | MDB_MAPASYNC | MDB_NORDAHEAD, 0664);
    } catch (const std::exception &e) {
        logger (ERROR)
            << "Failed to open database: "
            << e.what ();
        throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::INTERNAL_ERROR));
    }

    /*!
        resize mapsize if needed
    */
    checkResize ();

    logger (INFO)
        << "DB opened in "
        << m_dbDir;

    m_dirty = 0;
    state.store (INITIALIZED);
}

void LmDBWrapper::shutdown()
{
    if (state.load () != INITIALIZED) {
        throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::NOT_INITIALIZED));
    }

    logger (INFO)
        << "Closing DB.";
    m_db.sync ();
    state.store (NOT_INITIALIZED);
}

void LmDBWrapper::destroy(const DataBaseConfig &config)
{
    if (state.load () != NOT_INITIALIZED) {
        throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::ALREADY_INITIALIZED));
    }

    logger (WARNING)
        << "Destroying DB in "
        << m_dbDir;
    lmdb::dbi dbi;

    {
        auto txn = lmdb::txn::begin (m_db);
        dbi = lmdb::dbi::open (txn, nullptr);

        try {
            dbi.drop (txn, 0);
        } catch (const std::exception &e) {
            logger (ERROR)
                << "DB Error. DB can't be destroyed in "
                << m_dbDir
                << ". Error: "
                << e.what ();
            throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::INTERNAL_ERROR));
        }

        txn.commit ();
    }

    m_db.sync ();
}

std::error_code LmDBWrapper::write(IWriteBatch &batch)
{
    if (state.load () != INITIALIZED) {
        throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::NOT_INITIALIZED));
    }

    /*!
        resize if needed
    */
    checkResize ();

    MDB_txn *wtxn;
    lmdb::dbi dbi;
    std::error_code errCode;

    try {
        lmdb::txn_begin (m_db, nullptr, 0, &wtxn);
    } catch (const std::exception &e) {
        logger (ERROR)
            << "Failed to prepare db write transaction: "
            << e.what ();
        throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::INTERNAL_ERROR));
    }

    dbi = lmdb::dbi::open (wtxn, nullptr);

    {
        // insert
        const std::vector<std::pair<std::string, std::string>> rawData (batch.extractRawDataToInsert ());
        logger (TRACE)
            << "Writing rawdata, len: "
            << rawData.size ();

        for (const std::pair<std::string, std::string> &kvPair : rawData) {
            if (dbi.put (wtxn, kvPair.first, kvPair.second)) {
                m_dirty++;
            } else {
                logger (ERROR)
                    << "dbi.put failed";
                errCode = make_error_code (CryptoNote::error::DataBaseErrorCodes::INTERNAL_ERROR);
            }
        }
    }

    {
        // delete
        const std::vector<std::string> rawKeys (batch.extractRawKeysToRemove ());
        logger (TRACE)
            << "Removing rawKeys, len: "
            << rawKeys.size ();
        for (const std::string &key : rawKeys) {
            if (dbi.del (wtxn, key)) {
                m_dirty++;
            } else {
                logger (ERROR)
                    << "dbi.del failed";
                errCode = make_error_code (CryptoNote::error::DataBaseErrorCodes::INTERNAL_ERROR);
            }
        }
    }

    try {
        lmdb::txn_commit (wtxn);
    } catch (const std::exception &e) {
        logger (ERROR)
            << "Failed to commit db transactions: "
            << e.what ();
        throw std::system_error (make_error_code (CryptoNote::error::DataBaseErrorCodes::INTERNAL_ERROR));
    }


    if (m_dirty >= MAX_DIRTY) {
        m_dirty = 0;
        logger (TRACE)
            << "Flushing dirty commits to disk";
        m_db.sync (0);
    }

    return errCode;
}

void LmDBWrapper::checkResize()
{
    uint64_t size_avail;
    uint64_t mapsize;

    {
        auto rtxn = lmdb::txn::begin (m_db, nullptr, MDB_RDONLY);
        lmdb::dbi dbi = lmdb::dbi::open (rtxn, nullptr);
        MDB_stat stat = dbi.stat (rtxn);

        MDB_envinfo info;
        lmdb::env_info (m_db, &info);

        mapsize = info.me_mapsize;
        const uint64_t size_used = stat.ms_psize * info.me_last_pgno;
        size_avail = mapsize - size_used;
    }

    if (size_avail > MAPSIZE_MIN_AVAIL) {
        logger (TRACE)
            << "DB Resize: no resize required, size avail: "
            << size_avail
            << " bytes.";
        return;
    }

    m_db.sync ();

    const uint64_t extra = 1ULL
        << SHIFTING_VAL;
    mapsize += extra;

    logger (DEBUGGING)
        << "Resizing database. New mapsize: "
        << mapsize
        << " bytes.";
    m_db.set_mapsize (mapsize);
}

std::error_code LmDBWrapper::read(IReadBatch &batch)
{
    if (state.load () != INITIALIZED) {
        throw std::runtime_error ("Not initialized.");
    }

    const std::vector<std::string> rawKeys (batch.getRawKeys ());
    logger (TRACE)
        << "Batch reading rawKeys, len: "
        << rawKeys.size ();
    std::vector<bool> resultStates;
    std::vector<std::string> values;
    lmdb::dbi dbi;

    {
        auto rtxn = lmdb::txn::begin (m_db, nullptr, MDB_RDONLY);
        dbi = lmdb::dbi::open (rtxn, nullptr);

        for (const std::string &key : rawKeys) {
            std::string_view val;
            if (dbi.get (rtxn, key, val)) {
                values.push_back (std::string (val));
                resultStates.push_back (true);
            } else {
                /*!
                    TODO: get rid of this
                    rocksdb MultiGet compat: pass empty string if the key wasn't found
                */
                values.push_back (std::string (""));
                resultStates.push_back (false);
            }
        }
    }
    /*!
        rtxn will be aborted/dropped here
    */

    batch.submitRawResult (values, resultStates);
    return std::error_code ();
}

void LmDBWrapper::setDataDir(const DataBaseConfig &config)
{
    if (config.getTestnet ()) {
        m_dbDir = fs::path (config.getDataDir () + '/' + TESTNET_DB_NAME);
    } else {
        m_dbDir = fs::path (config.getDataDir () + '/' + DB_NAME);
    }
    m_dbFile = fs::path (m_dbDir / "data.mdb");
}

fs::path LmDBWrapper::getDataDir(const DataBaseConfig &config)
{
    return m_dbDir;
}

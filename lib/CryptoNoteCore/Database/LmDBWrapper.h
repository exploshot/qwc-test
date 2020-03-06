// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
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

#pragma once

#include <atomic>
#include <memory>
#include <string>

#include <lmdb/lmdbpp.h>

#include <IDataBase.h>

#include <Common/FileSystemShim.h>

#include <CryptoNoteCore/Database/DatabaseConfig.h>

#include <Logging/LoggerRef.h>

namespace CryptoNote {
    class LmDBWrapper: public IDataBase
    {
    public:
        LmDBWrapper(std::shared_ptr<Logging::ILogger> logger);
        virtual ~LmDBWrapper();

        LmDBWrapper(const LmDBWrapper &) = delete;
        LmDBWrapper(LmDBWrapper &&) = delete;

        LmDBWrapper &operator=(const LmDBWrapper &) = delete;
        LmDBWrapper &operator=(LmDBWrapper &&) = delete;

        void init(const DataBaseConfig &config);
        void shutdown();

        void destroy(const DataBaseConfig &config); //Be careful with this method!

        std::error_code write(IWriteBatch &batch) override;
        std::error_code read(IReadBatch &batch) override;

    private:
        void checkResize();
        void setDataDir(const DataBaseConfig &config);
        fs::path getDataDir(const DataBaseConfig &config);

        enum State
        {
            NOT_INITIALIZED,
            INITIALIZED
        };

        Logging::LoggerRef logger;
        std::atomic<State> state;
        fs::path m_dbDir;
        fs::path m_dbFile;
        lmdb::env m_db = lmdb::env::create ();
        std::atomic_uint m_dirty;
    };
} // namespace CryptoNote

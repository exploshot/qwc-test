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

#pragma once

#include <atomic>

#include <lmdb/lmdb.h>
#include <lmdb/lmdbpp.h>

#include <Common/FileSystemShim.h>

#include <CryptoNoteCore/Blockchain/IMainChainStorage.h>
#include <CryptoNoteCore/Currency.h>

namespace CryptoNote {
    class MainChainStorageLmdb: public IMainChainStorage
    {
    public:
        MainChainStorageLmdb(const std::string &blocksFilename,
                             const std::string &indexesFilename);

        virtual ~MainChainStorageLmdb();

        virtual void pushBlock(const RawBlock &rawBlock) override;
        virtual void popBlock() override;

        virtual RawBlock getBlockByIndex(const uint32_t index) override;
        virtual uint32_t getBlockCount() const override;

        virtual void clear() override;

    private:
        void initializeBlockCount();
        void checkResize();
        void renewRoTxn();
        void renewRwTxn(bool sync);

        lmdb::env m_db = lmdb::env::create ();
        mutable MDB_txn *rtxn;
        mutable MDB_txn *wtxn;
        mutable std::atomic_int m_blockcount;
        mutable std::atomic_int m_dirty;
        fs::path m_dbpath;
    };

    std::unique_ptr<IMainChainStorage> createMainChainStorageLmdb(const std::string &dataDir,
                                                                         const Currency &currency);
} // namespace CryptoNote

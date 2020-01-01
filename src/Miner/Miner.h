// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
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
#include <thread>
#include <mutex>

#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/RemoteContext.h>

#include <CryptoNote.h>

namespace CryptoNote {

    struct BlockMiningParameters
    {
        BlockTemplate blockTemplate;
        uint64_t difficulty;
    };

    class Miner
    {
    public:
        Miner(System::Dispatcher &dispatcher);

        BlockTemplate mine(const BlockMiningParameters &blockMiningParameters, size_t threadCount);
        uint64_t getHashCount();

        /*!
         * NOTE! this is blocking method
         */
        void stop();

    private:
        System::Dispatcher &m_dispatcher;
        System::Event m_miningStopped;

        enum class MiningState: uint8_t
        {
            MINING_STOPPED, BLOCK_FOUND, MINING_IN_PROGRESS
        };
        std::atomic<MiningState> m_state;

        std::vector<std::unique_ptr<System::RemoteContext<void>>> m_workers;

        BlockTemplate m_block;
        std::atomic<uint64_t> m_hash_count = 0;
        std::mutex m_hashes_mutex;

        void runWorkers(BlockMiningParameters blockMiningParameters, size_t threadCount);
        void workerFunc(const BlockTemplate &blockTemplate, uint64_t difficulty, uint32_t nonceStep);
        bool setStateBlockFound();
        void incrementHashCount();
    };

} //namespace CryptoNote

// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include <Serialization/ISerializer.h>

namespace CryptoNote {

    struct CoreStatistics
    {
        uint64_t transactionPoolSize;
        uint64_t blockchainHeight;
        uint64_t miningSpeed;
        uint64_t alternativeBlockCount;
        std::string topBlockHashString;

        void serialize(ISerializer &s)
        {
            s (transactionPoolSize, "tx_pool_size");
            s (blockchainHeight, "blockchain_height");
            s (miningSpeed, "mining_speed");
            s (alternativeBlockCount, "alternative_blocks");
            s (topBlockHashString, "top_block_id_str");
        }
    };
} // namespace CryptoNote

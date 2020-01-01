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

#include <string>

#include <Rpc/CoreRpcServerCommandsDefinitions.h>

namespace Utilities {
    std::string getMiningSpeed(const uint64_t hashrate);

    std::string getSyncPercentage(uint64_t height, const uint64_t target_height);

    std::string getUpgradeTime(const uint64_t height, const uint64_t upgrade_height);

    std::string getStatusString(CryptoNote::COMMAND_RPC_GET_INFO::response iresp);

    std::string formatAmount(const uint64_t amount);

    std::string formatAmountBasic(const uint64_t amount);

    std::string prettyPrintBytes(const uint64_t numBytes);

    std::string unixTimeToDate(const uint64_t timestamp);
} // namespace Utilities

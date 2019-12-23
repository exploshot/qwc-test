// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

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

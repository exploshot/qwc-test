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

namespace Config {
    class WalletConfig
    {
    public:
        /*!
         * Pretty self explanatory, this configures whether we process
         * coinbase transactions in the wallet. Most wallets have not received
         * coinbase transactions.
         */
        bool skipCoinbaseTransactions = true;
    };

    class DaemonConfig
    {
    public:
    };

    class GlobalConfig
    {
    public:
    };

    /*!
     * Global config, exposed as `config`.
     * Example: `if (Config::config.wallet.scanCoinbaseTransactions)`
     */
    class Config
    {
    public:
        Config()
        {
        };

        /*!
         * Configuration for wallets
         */
        WalletConfig wallet;

        /*!
         * Configuration for the daemon
         */
        DaemonConfig daemon;

        /*!
         * Configuration for throughout the codebase
         */
        GlobalConfig global;
    };

    extern Config config;
} // namespace Config

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

#include <rapidjson/document.h>

#include <Global/CryptoNoteConfig.h>

#include <Logging/ILogger.h>

using namespace rapidjson;

namespace PaymentService {
    struct WalletServiceConfiguration
    {
        WalletServiceConfiguration()
        {
        };

        /*!
         * Address for the daemon RPC
         */
        std::string daemonAddress = "127.0.0.1";

        /*!
         * Address to run the API on (0.0.0.0 for all interfaces)
         */
        std::string bindAddress = "127.0.0.1";

        /*!
         * Password to access the API
         */
        std::string rpcPassword;

        /*!
         * Location of wallet file on disk
         */
        std::string containerFile;

        /*!
         * Password for the wallet file
         */
        std::string containerPassword;

        std::string serverRoot;

        /*!
         * Value to set for Access-Control-Allow-Origin (* for all)
         */
        std::string corsHeader;

        /*!
         * File to log to
         */
        std::string logFile = "service.log";

        /*!
         * Port to use for the daemon RPC
         */
        int daemonPort = CryptoNote::RPC_DEFAULT_PORT;

        /*!
         * Port for the API to listen on
         */
        int bindPort = CryptoNote::SERVICE_DEFAULT_PORT;

        /*!
         * Default log level
         */
        int logLevel = Logging::INFO;

        /*!
         * Timeout for daemon connection in seconds
         */
        int initTimeout = 10;

        /*!
         * Should we disable RPC connection
         */
        bool legacySecurity = false;

        /*!
         * Should we display the help message
         */
        bool help = false;

        /*!
         * Should we display the version message
         */
        bool version = false;

        /*!
         * Should we dump the provided config
         */
        bool dumpConfig = false;

        /*!
         * File to load config from
         */
        std::string configFile;

        /*!
         * File to dump the provided config to
         */
        std::string outputFile;

        /*!
         * Private view key to import
         */
        std::string secretViewKey;

        /*!
         * Private spend key to import
         */
        std::string secretSpendKey;

        /*!
         * Mnemonic seed to import
         */
        std::string mnemonicSeed;

        /*!
         * Should we create a new wallet
         */
        bool generateNewContainer = false;

        bool daemonize = false;
        bool registerService = false;
        bool unregisterService = false;

        /*!
         * Print all the addresses and exit (Why is this a thing?)
         */
        bool printAddresses = false;

        bool syncFromZero = false;

        /*!
         * Height to begin scanning at (on initial import)
         */
        uint64_t scanHeight;
    };

    bool updateConfigFormat(const std::string configFile, WalletServiceConfiguration &config);

    void handleSettings(int argc, char *argv[], WalletServiceConfiguration &config);

    void handleSettings(const std::string configFile, WalletServiceConfiguration &config);

    Document asJSON(const WalletServiceConfiguration &config);

    std::string asString(const WalletServiceConfiguration &config);

    void asFile(const WalletServiceConfiguration &config, const std::string &filename);
} // namespace PaymentService
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

/////////////////////////////////////
#include <WalletApi/ParseArguments.h>
/////////////////////////////////////

#include <cxxopts.hpp>

#include <Global/CliHeader.h>
#include <Global/Config.h>
#include <Global/CryptoNoteConfig.h>
#include <Global/Constants.h>

#include <thread>

#include <version.h>

ApiConfig parseArguments(int argc, char **argv)
{
    ApiConfig config;

    cxxopts::Options options (argv[0], CryptoNote::getProjectCLIHeader ());

    bool help, version, scanCoinbaseTransactions;

    int logLevel;

    unsigned int threads;

    options.add_options ("Core")
               ("h,help",
                "Display this help message",
                cxxopts::value<bool> (help)->implicit_value ("true"))

               ("log-level",
                "Specify log level",
                cxxopts::value<int> (logLevel)->default_value (std::to_string (config.logLevel)), "#")

               ("scan-coinbase-transactions",
                "Scan miner/coinbase transactions",
                cxxopts::value<bool> (scanCoinbaseTransactions)->default_value ("false")->implicit_value ("true"))

               ("threads",
                "Specify number of wallet sync threads",
                cxxopts::value<unsigned int> (threads)->default_value (std::to_string (std::max (1u,
                                                                                                 std::thread::hardware_concurrency ()))),
                "#")

               ("v,version",
                "Output software version information",
                cxxopts::value<bool> (version)->default_value ("false")->implicit_value ("true"));

    options.add_options ("Network")
               ("p,port",
                "The port to listen on for http requests",
                cxxopts::value<uint16_t> (config.port)->default_value (std::to_string (CryptoNote::SERVICE_DEFAULT_PORT)),
                "<port>")

               ("rpc-bind-ip",
                "Interface IP address for the RPC service",
                cxxopts::value<std::string> (config.rpcBindIp)->default_value ("127.0.0.1"));

    options.add_options ("RPC")
               ("enable-cors",
                "Adds header 'Access-Control-Allow-Origin' to the RPC responses. Uses the value specified as the domain. Use * for all.",
                cxxopts::value<std::string> (config.corsHeader),
                "<domain>")

               ("r,rpc-password",
                "Specify the <password> to access the RPC server.",
                cxxopts::value<std::string> (config.rpcPassword), "<password>");

    try {
        const auto result = options.parse (argc, argv);

        /*!
         * Rpc password must be specified if not --help or --version
         */
        if (result.count ("rpc-password") == 0 && !(help || version)) {
            std::cout
                << "You must specify an rpc-password!"
                << std::endl;
            std::cout
                << options.help ({})
                << std::endl;
            exit (1);
        }
    } catch (const cxxopts::OptionException &e) {
        std::cout
            << "Error: Unable to parse command line argument options: "
            << e.what ()
            << std::endl
            << std::endl;
        std::cout
            << options.help ({})
            << std::endl;
        exit (1);
    }

    /*!
     * Do we want to display the help message?
     */
    if (help) {
        std::cout
            << options.help ({})
            << std::endl;
        exit (0);
    }
    /*!
     * Do we want to display the software version?
     */
    else if (version)
    {
        std::cout
            << CryptoNote::getProjectCLIHeader ()
            << std::endl;
        exit (0);
    }

    if (logLevel < Logger::DISABLED || logLevel > Logger::DEBUG) {
        std::cout
            << "Log level must be between "
            << Logger::DISABLED
            << " and "
            << Logger::DEBUG
            << "!"
            << std::endl;
        exit (1);
    } else {
        config.logLevel = static_cast<Logger::LogLevel>(logLevel);
    }

    if (threads == 0) {
        std::cout
            << "Thread count must be at least 1"
            << std::endl;
        exit (1);
    } else {
        config.threads = threads;
    }

    if (scanCoinbaseTransactions) {
        Config::config.wallet.skipCoinbaseTransactions = false;
    }

    return config;
}

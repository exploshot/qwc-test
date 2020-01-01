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

#include <Miner/MinerManager.h>

#include <System/Dispatcher.h>

int main(int argc, char **argv)
{
    while (true) {
        CryptoNote::MiningConfig config;
        config.parse (argc, argv);

        try {
            System::Dispatcher dispatcher;

            auto httpClient = std::make_shared<httplib::Client> (
                config.daemonHost.c_str (), config.daemonPort, 10 /* 10 second timeout */
            );

            Miner::MinerManager app (dispatcher, config, httpClient);

            app.start ();
        } catch (const std::exception &e) {
            std::cout
                << "Unhandled exception caught: "
                << e.what ()
                << "\nAttempting to relaunch..."
                << std::endl;
        }
    }
}

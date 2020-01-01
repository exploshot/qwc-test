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

#include <Global/CryptoNoteConfig.h>

#include <Logging/Logger.h>

struct ApiConfig
{
    /*!
     * The IP to listen for requests on
     */
    std::string rpcBindIp;

    /*!
     * What port should we listen on
     */
    uint16_t port;

    /*!
     * Password the user must supply with each request
     */
    std::string rpcPassword;

    /*!
     * The value to use with the 'Access-Control-Allow-Origin' header
     */
    std::string corsHeader;

    /*!
     * Controls what level of messages to log
     */
    Logger::LogLevel logLevel = Logger::DISABLED;

    unsigned int threads;
};

ApiConfig parseArguments(int argc, char **argv);

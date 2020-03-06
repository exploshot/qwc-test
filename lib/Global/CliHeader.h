// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
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

#include <sstream>

#include <Global/Constants.h>
#include <Global/CryptoNoteConfig.h>

#include <version.h>

namespace CryptoNote {
    inline std::string getProjectCLIHeader()
    {
        std::stringstream programHeader;
        programHeader
            << std::endl
            << Constants::asciiArt
            << std::endl
            << " "
            << CryptoNote::CRYPTONOTE_NAME
            << " v"
            << PROJECT_VERSION_LONG
            << std::endl
            << " This software is distributed under the General Public License v3.0"
            << std::endl
            << std::endl
            << " "
            << PROJECT_COPYRIGHT
            << std::endl
            << std::endl
            << " Additional Copyright(s) may apply, please see the included LICENSE file for more information."
            << std::endl
            << " If you did not receive a copy of the LICENSE, please visit:"
            << std::endl
            << " "
            << CryptoNote::LICENSE_URL
            << std::endl
            << std::endl;

        return programHeader.str ();
    }
} // namespace CryptoNote
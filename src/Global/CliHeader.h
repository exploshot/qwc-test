// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
//
// Please see the included LICENSE file for more information.

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
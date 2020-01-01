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

#include <Global/Constants.h>

namespace ApiConstants {
    /*!
     * The number of iterations of PBKDF2 to perform on the wallet
     * password.
     */
    const uint64_t PBKDF2_ITERATIONS = 10000;

    /*!
     * The length of the address after removing the prefix
     */
    const uint16_t addressBodyLength = WalletConfig::standardAddressLength
                                       - WalletConfig::addressPrefix.length ();

    /*!
     * This is the equivalent of QWC[a-zA-Z0-9]{95} but working for all coins
     */
    const std::string addressRegex = std::string (WalletConfig::addressPrefix) + "[a-zA-Z0-9]{"
                                     + std::to_string (addressBodyLength) + "}";

    /*!
     * 64 char, hex
     */
    const std::string hashRegex = "[a-fA-F0-9]{64}";
}

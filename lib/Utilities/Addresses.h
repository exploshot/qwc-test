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
#include <vector>

#include <CryptoNote.h>

#include <Utilities/Errors.h>

namespace Utilities {
    std::vector <Crypto::PublicKey> addressesToSpendKeys(const std::vector <std::string> addresses);

    std::tuple <Crypto::PublicKey, Crypto::PublicKey> addressToKeys(const std::string address);

    std::tuple <std::string, std::string> extractIntegratedAddressData(const std::string address);

    std::string publicKeysToAddress(const Crypto::PublicKey publicSpendKey,
                                    const Crypto::PublicKey publicViewKey);

    std::string privateKeysToAddress(const Crypto::SecretKey privateSpendKey,
                                     const Crypto::SecretKey privateViewKey);

    std::tuple <Error, std::string> createIntegratedAddress(const std::string address,
                                                            const std::string paymentID);

    std::string getAccountAddressAsStr(const uint64_t prefix,
                                       const CryptoNote::AccountPublicAddress &adr);

    bool parseAccountAddressString(uint64_t &prefix,
                                   CryptoNote::AccountPublicAddress &adr,
                                   const std::string &str);
} // namespace Utilities

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

#include <CryptoTypes.h>
#include <json.hpp>

#include <Common/StringTools.h>

#include <SubWallets/SubWallet.h>

#include <WalletBackend/WalletBackend.h>

using nlohmann::json;

/*!
 * Tmp struct just used in serialization (See cpp for justification)
 */
struct Transfer
{
    Crypto::PublicKey publicKey;
    int64_t amount;
};

/*!
 * As above
 */
struct TxPrivateKey
{
    Crypto::Hash txHash;
    Crypto::SecretKey txPrivateKey;
};

/*!
 * Transfer
 */
void to_json(json &j, const Transfer &t);
void from_json(const json &j, Transfer &t);

void to_json(json &j, const TxPrivateKey &t);
void from_json(const json &j, TxPrivateKey &t);

namespace WalletTypes {
    /*!
     * WalletTypes::Transaction
     */
    void to_json(json &j, const WalletTypes::Transaction &t);
    void from_json(const json &j, WalletTypes::Transaction &t);
} // namespace WalletTypes

std::vector <Transfer> transfersToVector(const std::unordered_map <Crypto::PublicKey, int64_t> transfers);

std::unordered_map <Crypto::PublicKey, int64_t> vectorToTransfers(const std::vector <Transfer> vector);

std::vector <TxPrivateKey>
txPrivateKeysToVector(const std::unordered_map <Crypto::Hash, Crypto::SecretKey> txPrivateKeys);

std::unordered_map <Crypto::Hash, Crypto::SecretKey> vectorToTxPrivateKeys(const std::vector <TxPrivateKey> vector);

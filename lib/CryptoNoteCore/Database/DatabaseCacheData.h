// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include <map>

#include <CryptoNoteCore/Blockchain/BlockchainCache.h>

namespace CryptoNote {

    struct KeyOutputInfo
    {
        Crypto::PublicKey publicKey;
        Crypto::Hash transactionHash;
        uint64_t unlockTime;
        uint16_t outputIndex;

        void serialize(CryptoNote::ISerializer &s);
    };

    /*!
        inherit here to avoid breaking IBlockchainCache interface
    */
    struct ExtendedTransactionInfo: CachedTransactionInfo
    {
        /*!
            CachedTransactionInfo tx;

            global key output indexes spawned in this transaction
        */
        std::map<IBlockchainCache::Amount,
                 std::vector<IBlockchainCache::GlobalOutputIndex>> amountToKeyIndexes;
        void serialize(ISerializer &s);
    };
} // namespace CryptoNote

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

#include <Common/CryptoNoteTools.h>

#include <CryptoNoteCore/Database/DatabaseCacheData.h>

#include <Serialization/CryptoNoteSerialization.h>
#include <Serialization/SerializationOverloads.h>

namespace CryptoNote {

    void ExtendedTransactionInfo::serialize(CryptoNote::ISerializer &s)
    {
        s (static_cast<CachedTransactionInfo &>(*this), "cached_transaction");
        s (amountToKeyIndexes, "key_indexes");
    }

    void KeyOutputInfo::serialize(ISerializer &s)
    {
        s (publicKey, "public_key");
        s (transactionHash, "transaction_hash");
        s (unlockTime, "unlock_time");
        s (outputIndex, "outputIndex");
    }

}

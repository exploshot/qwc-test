// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
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

#include <set>
#include <unordered_set>

#include <CryptoNote.h>

#include <Crypto/Crypto.h>

#include <CryptoNoteCore/Transactions/CachedTransaction.h>

namespace CryptoNote {

    struct TransactionValidatorState
    {
        std::unordered_set<Crypto::KeyImage> spentKeyImages;
    };

    void mergeStates(TransactionValidatorState &destination, const TransactionValidatorState &source);
    bool hasIntersections(const TransactionValidatorState &destination, const TransactionValidatorState &source);
    void excludeFromState(TransactionValidatorState &state, const CachedTransaction &transaction);
} // namespace CryptoNote

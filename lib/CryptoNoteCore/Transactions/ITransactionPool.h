// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
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

#include <CryptoNoteCore/Transactions/CachedTransaction.h>

namespace CryptoNote {

    struct TransactionValidatorState;

    class ITransactionPool
    {
    public:
        virtual ~ITransactionPool()
        {
        };

        virtual bool pushTransaction(CachedTransaction &&tx, TransactionValidatorState &&transactionState) = 0;
        virtual const CachedTransaction &getTransaction(const Crypto::Hash &hash) const = 0;
        virtual bool removeTransaction(const Crypto::Hash &hash) = 0;

        virtual size_t getTransactionCount() const = 0;
        virtual std::vector<Crypto::Hash> getTransactionHashes() const = 0;
        virtual bool checkIfTransactionPresent(const Crypto::Hash &hash) const = 0;

        virtual const TransactionValidatorState &getPoolTransactionValidationState() const = 0;
        virtual std::vector<CachedTransaction> getPoolTransactions() const = 0;
        virtual std::tuple<std::vector<CachedTransaction>, std::vector<CachedTransaction>>
        getPoolTransactionsForBlockTemplate() const = 0;

        virtual uint64_t getTransactionReceiveTime(const Crypto::Hash &hash) const = 0;
        virtual std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const Crypto::Hash &paymentId) const = 0;
    };

} // namespace CryptoNote

// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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


#include <chrono>
#include <unordered_map>

#include <Crypto/Crypto.h>

#include <CryptoNoteCore/Transactions/ITransactionPool.h>
#include <CryptoNoteCore/Transactions/ITransactionPoolCleaner.h>
#include <CryptoNoteCore/ITimeProvider.h>

#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>

#include <System/ContextGroup.h>

namespace CryptoNote {

    class TransactionPoolCleanWrapper: public ITransactionPoolCleanWrapper
    {
    public:
        TransactionPoolCleanWrapper(
            std::unique_ptr<ITransactionPool> &&transactionPool,
            std::unique_ptr<ITimeProvider> &&timeProvider,
            std::shared_ptr<Logging::ILogger> logger,
            uint64_t timeout);

        TransactionPoolCleanWrapper(const TransactionPoolCleanWrapper &) = delete;
        TransactionPoolCleanWrapper(TransactionPoolCleanWrapper &&other) = delete;

        TransactionPoolCleanWrapper &operator=(const TransactionPoolCleanWrapper &) = delete;
        TransactionPoolCleanWrapper &operator=(TransactionPoolCleanWrapper &&) = delete;

        virtual ~TransactionPoolCleanWrapper();

        virtual bool pushTransaction(CachedTransaction &&tx, TransactionValidatorState &&transactionState) override;
        virtual const CachedTransaction &getTransaction(const Crypto::Hash &hash) const override;
        virtual bool removeTransaction(const Crypto::Hash &hash) override;

        virtual size_t getTransactionCount() const override;
        virtual std::vector<Crypto::Hash> getTransactionHashes() const override;
        virtual bool checkIfTransactionPresent(const Crypto::Hash &hash) const override;

        virtual const TransactionValidatorState &getPoolTransactionValidationState() const override;
        virtual std::vector<CachedTransaction> getPoolTransactions() const override;
        virtual std::tuple<std::vector<CachedTransaction>, std::vector<CachedTransaction>>
        getPoolTransactionsForBlockTemplate() const override;

        virtual uint64_t getTransactionReceiveTime(const Crypto::Hash &hash) const override;
        virtual std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const Crypto::Hash &paymentId) const override;

        virtual std::vector<Crypto::Hash> clean(const uint32_t height) override;

    private:
        std::unique_ptr<ITransactionPool> transactionPool;
        std::unique_ptr<ITimeProvider> timeProvider;
        Logging::LoggerRef logger;
        std::unordered_map<Crypto::Hash, uint64_t> recentlyDeletedTransactions;
        uint64_t timeout;

        bool isTransactionRecentlyDeleted(const Crypto::Hash &hash) const;
        void cleanRecentlyDeletedTransactions(uint64_t currentTime);
    };
} //namespace CryptoNote

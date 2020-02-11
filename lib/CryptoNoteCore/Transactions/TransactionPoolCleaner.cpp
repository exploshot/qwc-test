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

#include <Common/StringTools.h>

#include <CryptoNoteCore/Transactions/TransactionPoolCleaner.h>
#include <CryptoNoteCore/Core.h>
#include <CryptoNoteCore/Mixins.h>

#include <System/InterruptedException.h>
#include <System/Timer.h>

namespace CryptoNote {

    TransactionPoolCleanWrapper::TransactionPoolCleanWrapper(
        std::unique_ptr<ITransactionPool> &&transactionPool,
        std::unique_ptr<ITimeProvider> &&timeProvider,
        std::shared_ptr<Logging::ILogger> logger,
        uint64_t timeout)
        : transactionPool (std::move (transactionPool)),
          timeProvider (std::move (timeProvider)),
          logger (logger, "TransactionPoolCleanWrapper"),
          timeout (timeout)
    {
        assert(this->timeProvider);
    }

    TransactionPoolCleanWrapper::~TransactionPoolCleanWrapper()
    {
    }

    bool TransactionPoolCleanWrapper::pushTransaction(CachedTransaction &&tx,
                                                      TransactionValidatorState &&transactionState)
    {
        return !isTransactionRecentlyDeleted (tx.getTransactionHash ()) &&
               transactionPool->pushTransaction (std::move (tx),
                                                 std::move (transactionState));
    }

    const CachedTransaction &TransactionPoolCleanWrapper::getTransaction(const Crypto::Hash &hash) const
    {
        return transactionPool->getTransaction (hash);
    }

    bool TransactionPoolCleanWrapper::removeTransaction(const Crypto::Hash &hash)
    {
        return transactionPool->removeTransaction (hash);
    }

    size_t TransactionPoolCleanWrapper::getTransactionCount() const
    {
        return transactionPool->getTransactionCount ();
    }

    std::vector<Crypto::Hash> TransactionPoolCleanWrapper::getTransactionHashes() const
    {
        return transactionPool->getTransactionHashes ();
    }

    bool TransactionPoolCleanWrapper::checkIfTransactionPresent(const Crypto::Hash &hash) const
    {
        return transactionPool->checkIfTransactionPresent (hash);
    }

    const TransactionValidatorState &TransactionPoolCleanWrapper::getPoolTransactionValidationState() const
    {
        return transactionPool->getPoolTransactionValidationState ();
    }

    std::vector<CachedTransaction> TransactionPoolCleanWrapper::getPoolTransactions() const
    {
        return transactionPool->getPoolTransactions ();
    }

    std::tuple<std::vector<CachedTransaction>, std::vector<CachedTransaction>>
    TransactionPoolCleanWrapper::getPoolTransactionsForBlockTemplate() const
    {
        return transactionPool->getPoolTransactionsForBlockTemplate ();
    }

    uint64_t TransactionPoolCleanWrapper::getTransactionReceiveTime(const Crypto::Hash &hash) const
    {
        return transactionPool->getTransactionReceiveTime (hash);
    }

    std::vector<Crypto::Hash>
    TransactionPoolCleanWrapper::getTransactionHashesByPaymentId(const Crypto::Hash &paymentId) const
    {
        return transactionPool->getTransactionHashesByPaymentId (paymentId);
    }

    std::vector<Crypto::Hash> TransactionPoolCleanWrapper::clean(const uint32_t height)
    {
        try {
            uint64_t currentTime = timeProvider->now ();
            auto transactionHashes = transactionPool->getTransactionHashes ();

            std::vector<Crypto::Hash> deletedTransactions;
            for (const auto &hash: transactionHashes) {
                uint64_t transactionAge = currentTime - transactionPool->getTransactionReceiveTime (hash);
                if (transactionAge >= timeout) {
                    logger (Logging::DEBUGGING)
                        << "Deleting transaction "
                        << Common::podToHex (hash)
                        << " from pool";
                    recentlyDeletedTransactions.emplace (hash, currentTime);
                    transactionPool->removeTransaction (hash);
                    deletedTransactions.emplace_back (std::move (hash));
                }

                CachedTransaction transaction = transactionPool->getTransaction (hash);
                std::vector<CachedTransaction> transactions;
                transactions.emplace_back (transaction);

                auto[success, error] = Mixins::validate (transactions, height);

                if (!success) {
                    logger (Logging::DEBUGGING)
                        << "Deleting invalid transaction "
                        << Common::podToHex (hash)
                        << " from pool."
                        << error;
                    recentlyDeletedTransactions.emplace (hash, currentTime);
                    transactionPool->removeTransaction (hash);
                    deletedTransactions.emplace_back (std::move (hash));
                }
            }

            cleanRecentlyDeletedTransactions (currentTime);
            return deletedTransactions;
        } catch (System::InterruptedException &) {
            throw;
        } catch (std::exception &e) {
            logger (Logging::WARNING)
                << "Caught an exception: "
                << e.what ()
                << ", stopping cleaning procedure cycle";
            throw;
        }
    }

    bool TransactionPoolCleanWrapper::isTransactionRecentlyDeleted(const Crypto::Hash &hash) const
    {
        auto it = recentlyDeletedTransactions.find (hash);
        return it != recentlyDeletedTransactions.end () && it->second >= timeout;
    }

    void TransactionPoolCleanWrapper::cleanRecentlyDeletedTransactions(uint64_t currentTime)
    {
        for (auto it = recentlyDeletedTransactions.begin (); it != recentlyDeletedTransactions.end ();) {
            if (currentTime - it->second >= timeout) {
                it = recentlyDeletedTransactions.erase (it);
            } else {
                ++it;
            }
        }
    }
} // namespace CryptoNote

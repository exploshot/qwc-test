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

#include <unordered_map>
#include <unordered_set>

#include <boost/utility.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <Common/Util.h>
#include <Common/IIntUtil.h>

#include <Crypto/Crypto.h>
#include <Crypto/Hash.h>

#include <CryptoNoteCore/Blockchain/BlockchainIndices.h>
#include <CryptoNoteCore/Blockchain/LMDB/BlockchainDB.h>
#include <CryptoNoteCore/Transactions/ITransactionPool.h>
#include <CryptoNoteCore/Transactions/ITransactionValidator.h>
#include <CryptoNoteCore/Transactions/ITxPoolObserver.h>
#include <CryptoNoteCore/Transactions/TransactionExtra.h>
#include <CryptoNoteCore/Transactions/TransactionValidationState.h>
#include <CryptoNoteCore/CryptoNoteBasic.h>
#include <CryptoNoteCore/CryptoNoteBasicImpl.h>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/ICore.h>
#include <CryptoNoteCore/ITimeProvider.h>
#include <CryptoNoteCore/VerificationContext.h>

#include <Logging/LoggerRef.h>

namespace CryptoNote {

    class ISerializer;
    using CryptoNote::BlockInfo;
    using namespace boost::multi_index;

    class OnceInTimeInterval
    {
    public:
        OnceInTimeInterval(unsigned interval, ITimeProvider &timeProvider)
            : mInterval(interval),
              mTimeProvider(timeProvider)
        {
            mLastWorkedTime = 0;
        }

        template<class functor_t>
        bool call(functor_t functr)
        {
            time_t now;

            if (now - mLastWorkedTime > mInterval) {
                bool res = functr();
                mLastWorkedTime = mTimeProvider.now ();

                return res;
            }

            return true;
        }

    private:
        time_t mLastWorkedTime;
        unsigned mInterval;
        ITimeProvider &mTimeProvider;
    };

    class TxMemoryPool : boost::noncopyable
    {
    public:
        TxMemoryPool(
            std::unique_ptr<BlockchainDB> &mDb,
            const CryptoNote::Currency &currency,
            CryptoNote::ITransactionValidator &validator,
            CryptoNote::ICore &core,
            CryptoNote::ITimeProvider &timeProvider,
            std::shared_ptr<Logging::ILogger> &log,
            bool blockchainIndicesEnabled);

        bool addObserver(ITxPoolObserver *observer);
        bool removeObserver(ITxPoolObserver *observer);

        // load / store operations
        bool init(const std::string &configFolder);
        bool deinit();

        bool haveTx(const Crypto::Hash &id) const;
        bool addTx(const Transaction &tx,
                   TxVerificationContext &txVerificationContext,
                   bool keptByBlock,
                   BlockchainDB &db);
        bool addTx(const Transaction &tx,
                   const Crypto::Hash &id,
                   size_t blobSize,
                   TxVerificationContext &txVerificationContext,
                   bool keptByBlock,
                   BlockchainDB &db);

        // gets tx and remove it from pool
        bool takeTx(const Crypto::Hash &id,
                    Transaction &tx,
                    size_t &blobSize,
                    uint64_t &fee);

        bool onBlockchainInc(uint64_t newBlockHeight, const Crypto::Hash &topBlockId);
        bool onBlockchainDec(uint64_t newBlockHeight, const Crypto::Hash &topBlockId);

        void lock() const;
        void unlock() const;
        std::unique_lock<std::recursive_mutex> obtainGuard() const;

        bool fillBlockTemplate(Block &block,
                               size_t medianSize,
                               size_t maxCumulativeSize,
                               uint64_t alreadyGeneratedCoins,
                               size_t &totalSize,
                               uint64_t &fee);

        void getTransactions(std::list<Transaction> &txs) const;
        void getDifference(const std::vector<Crypto::Hash>& knownTxIds,
                           std::vector<Crypto::Hash> &newTxIds,
                           std::vector<Crypto::Hash> &deletedTxIds) const;
        size_t getTransactionsCount() const;
        std::string printPool(bool shortFormat) const;

        void onIdle();

        bool getTransactionIdsByPaymentId(const Crypto::Hash &paymentId,
                                          std::vector<Crypto::Hash> &txIds);
        bool getTransactionIdsByTimestamp(uint64_t timestampBegin,
                                          uint64_t timestampEnd,
                                          uint32_t txNumLimit,
                                          std::vector<Crypto::Hash> &hashes,
                                          uint64_t &txNumWithinTimestamps);

        template<class TIdsContainer, class TTxContainer, class TMissedContainer>
        void getTransactions(const TIdsContainer &txIds,
                             TTxContainer &txs,
                             TMissedContainer &missedTxs)
        {
            std::lock_guard<std::recursive_mutex> lock(mTxLock);

            for (const auto &id : txIds) {
                auto it = mTransactions.find(id);
                if (it == mTransactions.end()) {
                    missedTxs.push_back(id);
                } else {
                    txs.push_back(it->tx);
                }
            }
        }

        void serialize(ISerializer &s);
        void getTransactions(std::list<Transaction> &txs,
                             bool includeUnrelayedTxes,
                             BlockchainDB &db) const;

        struct TransactionCheckInfo
        {
            BlockInfo maxUsedBlock;
            BlockInfo lastFailedBlock;
        };

        struct TransactionDetails : public TransactionCheckInfo
        {
            Crypto::Hash id;
            Transaction tx;
            size_t blobSize;
            uint64_t fee;
            bool keptByBlock;
            time_t receiveTime;
        };

        void getMemoryPool(std::list<CryptoNote::TxMemoryPool::TransactionDetails> txs) const;
        std::list<CryptoNote::TxMemoryPool::TransactionDetails> getMemoryPool() const;

    private:
        struct TransactionPriorityComparator
        {
            bool operator()(const TransactionDetails &lHs,
                            const TransactionDetails &rHs) const
            {
                uint64_t lHsHi, lHsLo = mul128(lHs.fee, rHs.blobSize, &lHsHi);
                uint64_t rHsHi, rHsLo = mul128(rHs.fee, lHs.blobSize, &rHsHi);

                return
                    // prefer more profitable transactions
                    (lHsHi > rHsHi) ||
                    (lHsHi == rHsHi && lHsLo > rHsLo) ||
                    // prefer smaller
                    (lHsHi == rHsHi && lHsLo == rHsLo && lHs.blobSize < rHs.blobSize) ||
                    // prefer older
                    (lHsHi == rHsHi && lHsLo == rHsLo && lHs.blobSize == rHs.blobSize &&
                    lHs.receiveTime < rHs.receiveTime);
            }
        };

        typedef hashed_unique<BOOST_MULTI_INDEX_MEMBER(TransactionDetails,
                                                       Crypto::Hash, id)> MainIndexT;

        typedef ordered_non_unique<identity<TransactionDetails>,
                                            TransactionPriorityComparator> FeeIndexT;

        typedef multi_index_container<TransactionDetails,
                                      indexed_by<MainIndexT, FeeIndexT>> TxContainerT;

        typedef std::pair<uint64_t, uint64_t> GlobalOutput;
        typedef std::set<GlobalOutput> GlobalOutputsContainer;
        typedef std::unordered_map<Crypto::KeyImage,
                                   std::unordered_set<Crypto::Hash>> KeyImagesContainer;

        // double spending checking
        bool addTransactionInputs(const Crypto::Hash &id,
                                  const Transaction &tx,
                                  bool keptByBlock);
        bool haveSpentInputs(const Transaction &tx) const;
        bool removeTransactionInputs(const Crypto::Hash &id,
                                     const Transaction &tx,
                                     bool keptByBlock);

        TxContainerT::iterator removeTransaction(TxContainerT::iterator i);
        bool removeExpiredTransactions();
        bool isTxReadyToGo(const Transaction &tx,
                           TransactionCheckInfo &txD) const;

        void buildIndices();

        Tools::ObserverManager<ITxPoolObserver> mObserverManager;
        const CryptoNote::Currency &mCurrency;
        CryptoNote::ICore &mCore;
        OnceInTimeInterval mTxCheckInterval;
        mutable std::recursive_mutex mTxLock;
        KeyImagesContainer mSpentKeyImages;
        GlobalOutputsContainer mSpentOutputs;

        std::string mConfigFolder;
        CryptoNote::ITransactionValidator &mValidator;
        CryptoNote::ITimeProvider &mTimeProvider;

        TxContainerT mTransactions;
        TxContainerT::nth_index<1>::type &mFeeIndex;
        std::unordered_map<Crypto::Hash, uint64_t> mRecentlyDeletedTransactions;

        Logging::LoggerRef logger;
        std::unique_ptr<BlockchainDB> &mDb;
        PaymentIdIndex mPaymentIdIndex;
        TimestampTransactionsIndex mTimestampIndex;
        std::unordered_map<Crypto::Hash, uint64_t> mTtlIndex;
    };

    class TransactionPool: public ITransactionPool
    {
    public:
        TransactionPool(std::shared_ptr<Logging::ILogger> logger);

        virtual bool
        pushTransaction(CachedTransaction &&transaction, TransactionValidatorState &&transactionState) override;
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
    private:
        TransactionValidatorState poolState;

        struct PendingTransactionInfo
        {
            uint64_t receiveTime;
            CachedTransaction cachedTransaction;
            boost::optional<Crypto::Hash> paymentId;

            const Crypto::Hash &getTransactionHash() const;
        };

        struct TransactionPriorityComparator
        {
            /*!
                lhs > hrs
            */
            bool operator()(const PendingTransactionInfo &lhs,
                            const PendingTransactionInfo &rhs) const;
        };

        struct TransactionHashTag
        {
        };
        struct TransactionCostTag
        {
        };
        struct PaymentIdTag
        {
        };

        typedef boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<TransactionCostTag>,
            boost::multi_index::identity<PendingTransactionInfo>,
            TransactionPriorityComparator
        > TransactionCostIndex;

        typedef boost::multi_index::hashed_unique<
            boost::multi_index::tag<TransactionHashTag>,
            boost::multi_index::const_mem_fun<
                PendingTransactionInfo,
                const Crypto::Hash &,
                &PendingTransactionInfo::getTransactionHash
            >
        > TransactionHashIndex;

        struct PaymentIdHasher
        {
            size_t operator()(const boost::optional<Crypto::Hash> &paymentId) const;
        };

        typedef boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<PaymentIdTag>,
            BOOST_MULTI_INDEX_MEMBER(PendingTransactionInfo,
                                     boost::optional<Crypto::Hash>,
                                     paymentId),
            PaymentIdHasher
        > PaymentIdIndex;

        typedef boost::multi_index_container<
            PendingTransactionInfo,
            boost::multi_index::indexed_by<
                TransactionHashIndex,
                TransactionCostIndex,
                PaymentIdIndex
            >
        > TransactionsContainer;

        TransactionsContainer transactions;
        TransactionsContainer::index<TransactionHashTag>::type &transactionHashIndex;
        TransactionsContainer::index<TransactionCostTag>::type &transactionCostIndex;
        TransactionsContainer::index<PaymentIdTag>::type &paymentIdIndex;

        Logging::LoggerRef logger;
    };
} // namespace CryptoNote

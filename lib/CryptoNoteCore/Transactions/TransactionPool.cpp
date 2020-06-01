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

#include <Common/IIntUtil.h>
#include <CryptoNoteCore/Transactions/TransactionExtra.h>

#include <CryptoNoteCore/Blockchain/LMDB/BlockchainDB.h>
#include <CryptoNoteCore/Transactions/TransactionPool.h>
#include <CryptoNoteCore/CryptoNoteBasicImpl.h>

namespace CryptoNote {

    class BlockTemplate
    {
    public:
        bool addTransaction(const Crypto::Hash &txId, const Transaction &tx)
        {
            if (!canAdd(tx)) {
                return false;
            }

            for (const auto &in : tx.inputs) {
                if (in.type() == typeid(KeyInput)) {
                    auto r = mKeyImages.insert(boost::get<KeyInput>(in).keyImage);
                } else if (in.type() == typeid(MultisignatureInput)) {
                    const auto& mSig = boost::get<MultisignatureInput>(in);
                    auto r = mUsedOutputs.count(std::make_pair(mSig.amount,
                                                               mSig.outputIndex));
                }
            }

            mTxHashes.push_back(txId);
            return true;
        }

        const std::vector<Crypto::Hash> &getTransactions() const
        {
            return mTxHashes;
        }
    private:
        bool canAdd(const Transaction &tx)
        {
            for (const auto &in : tx.inputs) {
                if (in.type() == typeid(KeyInput)) {
                    if (mKeyImages.count(boost::get<KeyInput>(in).keyImage)) {
                        return false;
                    }
                } else if (in.type() == typeid(MultisignatureInput)) {
                    const auto& mSig = boost::get<MultisignatureInput>(in);
                    if (mUsedOutputs.count(std::make_pair(mSig.amount,
                                                          mSig.outputIndex))) {
                        return false;
                    }
                }
            }

            return true;
        }

        std::unordered_set<Crypto::KeyImage> mKeyImages;
        std::set<std::pair<uint64_t, uint64_t>> mUsedOutputs;
        std::vector<Crypto::Hash> mTxHashes;
    };

    using CryptoNote::BlockInfo;
    std::unordered_set<Crypto::Hash> mValidatedTransactions;

    // TxMemoryPool

    TxMemoryPool::TxMemoryPool(std::unique_ptr<BlockchainDB> &mDb,
                               const CryptoNote::Currency &currency,
                               CryptoNote::ITransactionValidator &validator,
                               CryptoNote::ICore &core,
                               CryptoNote::ITimeProvider &timeProvider,
                               std::shared_ptr<Logging::ILogger> &log,
                               bool blockchainIndicesEnabled)
        : mDb(mDb),
          mCurrency(currency),
          mValidator(validator),
          mCore(core),
          mTimeProvider(timeProvider),
          mTxCheckInterval(60, timeProvider),
          mFeeIndex(boost::get<1>(mTransactions)),
          logger(log, "Txpool"),
          mPaymentIdIndex(blockchainIndicesEnabled),
          mTimestampIndex(blockchainIndicesEnabled)
    {
    }

    bool TxMemoryPool::addTx(const Transaction &tx,
                             /* const Crypto::Hash &txPrefixHash, */
                             const Crypto::Hash &id,
                             size_t blobSize,
                             TxVerificationContext &txVerificationContext,
                             bool keptByBlock,
                             BlockchainDB &db)
    {
        if (!checkInputTypesSupported(tx)) {
            txVerificationContext.mVerificationFailed = true;
            return false;
        }

        uint64_t inputsAmount = 0;
        uint64_t outputsAmount = getOutputAmount(tx);

        if (!getInputAmount(tx, inputsAmount)) {
            logger(Logging::ERROR, Logging::BRIGHT_RED)
                << "Failed to get inputs amount of transaction with hash: "
                << getObjectHash(tx);

            txVerificationContext.mVerificationFailed = true;
            return false;
        }

        if (outputsAmount > inputsAmount) {
            logger (Logging::ERROR, Logging::BRIGHT_RED)
                << "transaction use more money then it has: use "
                << mCurrency.formatAmount(outputsAmount)
                << ", have "
                << mCurrency.formatAmount(inputsAmount);

            txVerificationContext.mVerificationFailed = true;
            return false;
        }

        std::vector<TransactionExtraField> txExtraFields;
        parseTransactionExtra(tx.extra, txExtraFields);
        TransactionExtraTTL ttl;
        if (!findTransactionExtraFieldByType(txExtraFields, ttl)) {
            ttl.ttl = 0;
        }

        const uint64_t fee = inputsAmount - outputsAmount;
        bool isFusionTransaction = fee == 0 &&
                                   mCurrency.isFusionTransaction(tx,
                                                                 blobSize,
                                                                 mCore.getTopBlockIndex());

        if (ttl.ttl != 0 && !keptByBlock) {
            uint64_t now = static_cast<uint64_t>(time(nullptr));
            if (ttl.ttl <= now) {
                logger(Logging::WARNING, Logging::BRIGHT_YELLOW)
                    << "Transaction TTL has already expired: tx = "
                    << id
                    << ", ttl = "
                    << ttl.ttl;
                txVerificationContext.mVerificationFailed = true;
                return false;
            } else if (ttl.ttl - now >
                       mCurrency.mempoolTxLiveTime() +
                       mCurrency.blockFutureTimeLimit()) {
                logger(Logging::WARNING, Logging::BRIGHT_YELLOW)
                    << "Transaction TTL is out of range: tx = "
                    << id
                    << ", ttl = "
                    << ttl.ttl;
                txVerificationContext.mVerificationFailed = true;
                return false;
            }

            if (fee != 0) {
                logger(Logging::WARNING, Logging::BRIGHT_YELLOW)
                    << "Transaction with TTL has non-zero fee: tx = "
                    << id
                    << ", fee = "
                    << mCurrency.formatAmount(fee);
                txVerificationContext.mVerificationFailed = true;
                return false;
            }
        }

        /*!
         * check key images for transaction if it is not kept by block
         */
        if (!keptByBlock) {
            std::lock_guard<std::recursive_mutex> lock(mTxLock);
            if (haveSpentInputs(tx)) {
                logger(Logging::ERROR, Logging::BRIGHT_RED)
                    << "Transaction with id= "
                    << id
                    << " used already spent inputs";
                txVerificationContext.mVerificationFailed = true;
                return false;
            }
        }

        BlockInfo maxUsedBlock;

        /*!
         * check Inputs
         */
        bool inputsValid = mValidator.checkTransactionInputs(tx, maxUsedBlock);
        bool r = Tools::isLmdb();
        TxPoolTxMetaT meta;

        if (!inputsValid) {
            if (!keptByBlock) {
                logger(Logging::INFO) << "tx used wrong inputs, rejected";
                txVerificationContext.mVerificationFailed = true;
                return false;
            } else if (r && keptByBlock) {
                meta.blobSize = blobSize;
                meta.fee = fee;
                meta.maxUsedBlockId = Constants::NULL_HASH;
                meta.maxUsedBlockHeight = 0;
                meta.lastFailedHeight = 0;
                meta.lastFailedId = Constants::NULL_HASH;
                meta.keptByBlock = keptByBlock;
                meta.receiveTime = mTimeProvider.now();
                meta.lastRelayedTime = mTimeProvider.now();
                meta.doubleSpendSeen = haveSpentInputs(tx);
                memset(meta.padding, 0, sizeof(meta.padding));

                try {
                    mDb->blockTxnStart(false);
                    CryptoNote::Transaction txC = tx;
                    mDb->addTxPoolTx(txC, meta);
                    mDb->blockTxnStop();
                    mTtlIndex.emplace(std::make_pair(id, ttl.ttl));
                } catch (const std::exception &e) {
                    logger (Logging::ERROR, Logging::BRIGHT_RED)
                        << "transaction already exists at inserting in memory pool: " << e.what();
                    return false;
                }

                txVerificationContext.mAddedToPool = true;
                txVerificationContext.mVerificationFailed = false;
            }
            maxUsedBlock.clear();
        }

        if (!keptByBlock) {
            bool sizeValid = mValidator.checkTransactionSize(blobSize);
            if (!sizeValid) {
                logger(Logging::ERROR, Logging::BRIGHT_RED) << "tx too big, rejected";
                txVerificationContext.mVerificationFailed = true;
                return false;
            }
        }
        std::lock_guard<std::recursive_mutex> lock(mTxLock);

        if (!keptByBlock && mRecentlyDeletedTransactions.find(id)
                         != mRecentlyDeletedTransactions.end()) {
            logger (Logging::ERROR, Logging::BRIGHT_RED)
                << "Trying to add recently deleted transaction. Ignore: " << id;
            txVerificationContext.mVerificationFailed = true;
            txVerificationContext.mShouldBeRelayed = false;
            txVerificationContext.mAddedToPool = false;

            return true;
        }

        /*!
         * add to pool
         */
        if (!r) {
            TransactionDetails txD;

            txD.id = id;
            txD.blobSize = blobSize;
            txD.tx = tx;
            txD.fee = fee;
            txD.keptByBlock = keptByBlock;
            txD.receiveTime = mTimeProvider.now();

            txD.maxUsedBlock = maxUsedBlock;
            txD.lastFailedBlock.clear();

            auto txDP = mTransactions.insert(txD);
            if (!(txDP.second)) {
                logger (Logging::ERROR, Logging::BRIGHT_RED)
                    << "transaction already exists at inserting in memory pool";
                return false;
            }

            mPaymentIdIndex.add(tx);
            mTimestampIndex.add(txD.receiveTime, txD.id);

            if (ttl.ttl != 0) {
                mTtlIndex.emplace(std::make_pair(id, ttl.ttl));
            }
        } else {
            meta.blobSize = blobSize;
            meta.keptByBlock = keptByBlock;
            meta.fee = fee;
            meta.maxUsedBlockId = maxUsedBlock.id;
            meta.lastFailedHeight = 0;
            meta.lastFailedId = Constants::NULL_HASH;
            meta.receiveTime = mTimeProvider.now();
            meta.lastRelayedTime = mTimeProvider.now();
            meta.doubleSpendSeen = false;
            memset(meta.padding, 0, sizeof(meta.padding));

            try {
                mDb->blockTxnStart(false);
                mDb->removeTxPoolTx(getObjectHash(tx));
                mDb->blockTxnStop();
                CryptoNote::Transaction txC = tx;
                mDb->blockTxnStart(false);
                mDb->addTxPoolTx(txC, meta);
                mDb->blockTxnStop();
            } catch (const std::exception &e) {
                logger (Logging::ERROR, Logging::BRIGHT_RED)
                    << "internal error: transaction already exists at inserting in memory pool: "
                    << e.what();
                return false;
            }
        }

        txVerificationContext.mAddedToPool = true;
        txVerificationContext.mShouldBeRelayed = inputsValid && (fee > 0 ||
                                                                 isFusionTransaction ||
                                                                 ttl.ttl != 0);
        txVerificationContext.mVerificationFailed = false;

        if (!addTransactionInputs(id, tx, keptByBlock)) {
            return false;
        }

        txVerificationContext.mVerificationFailed = false;

        /*!
         * succed
         */
        return true;
    }

    bool TxMemoryPool::addTx(const Transaction &tx,
                             TxVerificationContext &txVerificationContext,
                             bool keptByBlock,
                             BlockchainDB &db)
    {
        Crypto::Hash h = Constants::NULL_HASH;
        size_t blobSize = 0;
        getObjectHash(tx, h, blobSize);
        return addTx(tx, h, blobSize, txVerificationContext, keptByBlock, db);
    }

    bool TxMemoryPool::takeTx(const Crypto::Hash &id,
                              Transaction &tx,
                              size_t &blobSize,
                              uint64_t &fee)
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        auto it = mTransactions.find(id);
        if (it == mTransactions.end()) {
            return false;
        }

        auto &txD = *it;

        tx = txD.tx;
        blobSize = txD.blobSize;
        fee = txD.fee;

        removeTransaction(it);
        return true;
    }

    size_t TxMemoryPool::getTransactionsCount() const
    {
        bool r = !Tools::isLmdb();
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        size_t size;
        if (r) {
            size = mTransactions.size();
        } else {
            size = mDb->getTxPoolTxCount();
        }

        return size;
    }

    void TxMemoryPool::getTransactions(std::list<Transaction> &txs) const
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        for (const auto &txVt : mTransactions) {
            txs.push_back(txVt.tx);
        }
    }

    void TxMemoryPool::getTransactions(std::list<Transaction> &txs,
                                       bool includeUnrelayedTxes,
                                       BlockchainDB &db) const
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        db.forAllTxPoolTxes([&txs](const Crypto::Hash &txId,
                                   const TxPoolTxMetaT &meta,
                                   const CryptoNote::blobData *bd)
        {
            Transaction tx;
            if (!parseAndValidateTxFromBlob(*bd, tx)) {
                // continue
                return true;
            }

            txs.push_back(tx);
            return true;
        }, true, includeUnrelayedTxes);
    }

    void TxMemoryPool::getMemoryPool(std::list<TxMemoryPool::TransactionDetails> txs) const
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        for (const auto &txD : mFeeIndex) {
            txs.push_back(txD);
        }
    }

    std::list<TxMemoryPool::TransactionDetails> TxMemoryPool::getMemoryPool() const
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        std::list<TxMemoryPool::TransactionDetails> txs;
        for (const auto &txD : mFeeIndex) {
            txs.push_back(txD);
        }

        return txs;
    }

    void TxMemoryPool::getDifference(const std::vector<Crypto::Hash> &knownTxIds,
                                     std::vector<Crypto::Hash> &newTxIds,
                                     std::vector<Crypto::Hash> &deletedTxIds) const
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        std::unordered_set<Crypto::Hash> readyTxIds;
        for (const auto &tx : mTransactions) {
            TransactionCheckInfo checkInfo(tx);
            if (mValidatedTransactions.find(tx.id) != mValidatedTransactions.end()) {
                readyTxIds.insert(tx.id);
                logger(Logging::DEBUGGING)
                    << "MemPool - tx "
                    << tx.id
                    << " loaded from cache";
            } else if (isTxReadyToGo(tx.tx, checkInfo)) {
                readyTxIds.insert(tx.id);
                mValidatedTransactions.insert(tx.id);
                logger(Logging::DEBUGGING)
                    << "MemPool - tx "
                    << tx.id
                    << " added to cache";
            }
        }

        std::unordered_set<Crypto::Hash> knownSet(knownTxIds.begin(), knownTxIds.end());
        for (auto it = readyTxIds.begin(), e = readyTxIds.end(); it != e;) {
            auto knownIt = knownSet.find(*it);
            if (knownIt != knownSet.end()) {
                knownSet.erase(knownIt);
                it = readyTxIds.erase(it);
            } else {
                ++it;
            }
        }

        newTxIds.assign(readyTxIds.begin(), readyTxIds.end());
        deletedTxIds.assign(knownSet.begin(), knownSet.end());
    }

    bool TxMemoryPool::onBlockchainInc(uint64_t newBlockHeight,
                                       const Crypto::Hash &topBlockId)
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        if (!mValidatedTransactions.empty()) {
            logger (Logging::DEBUGGING)
                << "MemPool - Block height incremented, cleared "
                << mValidatedTransactions.size()
                << " cached transaction hashes. New height: "
                << newBlockHeight << " Top block: "
                << topBlockId;
            mValidatedTransactions.clear();
        }

        return true;
    }

    bool TxMemoryPool::onBlockchainDec(uint64_t newBlockHeight,
                                       const Crypto::Hash &topBlockId)
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        if (!mValidatedTransactions.empty()) {
            logger (Logging::DEBUGGING, Logging::YELLOW)
                << "MemPool - Block height decremented "
                << mValidatedTransactions.size()
                << " cached transaction hashes. New height: "
                << newBlockHeight << " Top block: "
                << topBlockId;
            mValidatedTransactions.clear();
        }

        return true;
    }

    bool TxMemoryPool::haveTx(const Crypto::Hash &id) const
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        if (mTransactions.count(id)) {
            return true;
        }

        return false;
    }

    void TxMemoryPool::lock() const
    {
        mTxLock.lock();
    }

    void TxMemoryPool::unlock() const
    {
        mTxLock.unlock();
    }

    std::unique_lock<std::recursive_mutex> TxMemoryPool::obtainGuard() const
    {
        return std::unique_lock<std::recursive_mutex>(mTxLock);
    }

    bool TxMemoryPool::isTxReadyToGo(const Transaction &tx, TransactionCheckInfo &txD) const
    {
        if (!mValidator.checkTransactionInputs(tx, txD.maxUsedBlock, txD.lastFailedBlock)) {
            return false;
        }

        /*!
         * if we here, transaction seems valid, but, anyway,
         * check for keyImages collisions with blockchain, just to be sure
         */
        if (mValidator.haveSpentKeyImages(tx)) {
            return false;
        }

        /*!
         * transaction is fine
         */
        return true;
    }

    std::string TxMemoryPool::printPool(bool shortFormat) const
    {
        std::stringstream ss;
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        for (const auto & txD : mFeeIndex) {
            ss << "id: " << txD.id << std::endl;

            if (!shortFormat) {
                ss << storeToJson(txD.tx) << std::endl;
            }

            ss  << "blobSize: " << txD.blobSize << std::endl
                << "fee: " << mCurrency.formatAmount(txD.fee) << std::endl
                << "keptByBlock: " << (txD.keptByBlock ? 'T' : 'F') << std::endl
                << "maxUsedBlockHeight: " << txD.maxUsedBlock.height << std::endl
                << "maxUsedBlockId: " << txD.maxUsedBlock.id << std::endl
                << "lastFailedBlockHeight: " << txD.lastFailedBlock.height << std::endl
                << "lastFailedBlockId: " << txD.lastFailedBlock.id << std::endl
                << "amountOut: " << getOutputAmount(txD.tx) << std::endl
                << "feeAtomicUnits: " << txD.fee << std::endl
                << "receivedTimestamp: " << txD.receiveTime << std::endl
                << "received: " << std::ctime(&txD.receiveTime);

            auto ttlIt = mTtlIndex.find(txD.id);
            if (ttlIt != mTtlIndex.end()) {
                ss << "TTL: " << std::ctime(reinterpret_cast<const time_t*>(&ttlIt->second));
            }

            ss << std::endl;
        }

        return ss.str();
    }

    bool TxMemoryPool::fillBlockTemplate(Block &block,
                                         size_t medianSize,
                                         size_t maxCumulativeSize,
                                         uint64_t alreadyGeneratedCoins,
                                         size_t &totalSize,
                                         uint64_t &fee)
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);

        totalSize = 0;
        fee = 0;

        size_t maxTotalSize = (125 * medianSize) / 100;
        maxTotalSize = std::min(maxTotalSize, maxCumulativeSize) -
                       mCurrency.minerTxBlobReservedSize();

        BlockTemplate blockTemplate;

        for (auto it = mFeeIndex.rbegin(); it != mFeeIndex.rend() && it->fee == 0; ++it) {
            const auto &txD = *it;

            if (mTtlIndex.count(txD.id) > 0) {
                continue;
            }

            if (mCurrency.fusionTxMaxSize() < totalSize + txD.blobSize) {
                continue;
            }

            TransactionCheckInfo checkInfo(txD);
            if (isTxReadyToGo(txD.tx, checkInfo) && blockTemplate.addTransaction(txD.id, txD.tx)) {
                totalSize += txD.blobSize;
                logger(Logging::DEBUGGING)
                    << "Fusion transaction "
                    << txD.id
                    << " included to block template";
            }

            TxPoolTxMetaT meta;
            if (!mDb->getTxPoolTxMeta(txD.id, meta)) {
                logger (Logging::ERROR, Logging::BRIGHT_RED) << "failed to find tx meta";
                continue;
            }
        }

        for (auto i = mFeeIndex.begin(); i != mFeeIndex.end(); ++i) {
            const auto &txD = *i;

            if (mTtlIndex.count(txD.id) > 0) {
                continue;
            }

            size_t blockSizeLimit = (txD.fee ==0) ? medianSize : maxTotalSize;
            if (blockSizeLimit < totalSize + txD.blobSize) {
                continue;
            }

            TransactionCheckInfo checkInfo(txD);
            bool ready = false;
            if (mValidatedTransactions.find(txD.id) != mValidatedTransactions.end()) {
                ready = true;
                logger (Logging::DEBUGGING)
                    << "Fill block template - tx added from cache: " << txD.id;
            } else if (isTxReadyToGo(txD.tx, checkInfo)) {
                ready = true;
                mValidatedTransactions.insert(txD.id);
                logger (Logging::DEBUGGING)
                    << "Fill block template - tx added to cache: " << txD.id;
            }

            /*!
             * update item state
             */
            mFeeIndex.modify(i, [&checkInfo](TransactionCheckInfo &item) {
                item = checkInfo;
            });

            if (ready && blockTemplate.addTransaction(txD.id, txD.tx)) {
                totalSize += txD.blobSize;
                fee += txD.fee;
                logger (Logging::DEBUGGING)
                    << "Transaction "
                    << txD.id
                    << " included to block template";
            } else {
                logger (Logging::DEBUGGING)
                    << "Transaction "
                    << txD.id
                    << " is failed to include to block template";
            }
        }

        block.transactionHashes = blockTemplate.getTransactions();
        return true;
    }

    bool TxMemoryPool::init(const std::string &configFolder)
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);

        mConfigFolder = configFolder;
        std::string stateFilePath = configFolder + "/" + mCurrency.txPoolFileName();
        boost::system::error_code ec;
        if (!boost::filesystem::exists(stateFilePath, ec)) {
            return true;
        }

        if (!loadFromBinaryFile(*this, stateFilePath)) {
            logger (Logging::ERROR)
                << "Failed to load memory pool from file: "
                << stateFilePath;

            mTransactions.clear();
            mSpentKeyImages.clear();
            mSpentOutputs.clear();

            mPaymentIdIndex.clear();
            mTimestampIndex.clear();
            mTtlIndex.clear();
        } else {
            buildIndices();
        }

        removeExpiredTransactions();

        /*!
         * Ignore deserialization error
         */
        return true;
    }

    bool TxMemoryPool::deinit()
    {
        if (!Tools::createDirectoriesIfNecessary(mConfigFolder)) {
            logger (Logging::INFO)
                << "Failed to create data directory: " << mConfigFolder;
            return false;
        }

        std::string stateFilePath = mConfigFolder + "/" + mCurrency.txPoolFileName();

        if (!storeToBinaryFile(*this, stateFilePath)) {
            logger(Logging::INFO)
                << "Failed to serialize memory pool to file "
                << stateFilePath;
        }

        mPaymentIdIndex.clear();
        mTimestampIndex.clear();
        mTtlIndex.clear();

        return true;
    }

    #define CURRENT_MEMPOOL_ARCHIVE_VER 1

    void serialize (CryptoNote::TxMemoryPool::TransactionDetails &td, ISerializer &s)
    {
        s(td.id, "id");
        s(td.blobSize, "blobSize");
        s(td.fee, "fee");
        s(td.tx, "tx");
        s(td.maxUsedBlock.height, "maxUsedBlock.height");
        s(td.maxUsedBlock.id, "maxUsedBlock.id");
        s(td.lastFailedBlock.height, "lastFailedBlock.height");
        s(td.lastFailedBlock.id, "lastFailedBlock.id");
        s(td.keptByBlock, "keptByBlock");
        s(reinterpret_cast<uint64_t&>(td.receiveTime), "receiveTime");
    }

    void TxMemoryPool::serialize(ISerializer &s)
    {
        uint8_t version = CURRENT_MEMPOOL_ARCHIVE_VER;

        s(version, "version");
        if (version != CURRENT_MEMPOOL_ARCHIVE_VER) {
            return;
        }

        std::lock_guard<std::recursive_mutex> lock(mTxLock);

        if (s.type() == ISerializer::INPUT) {
            mTransactions.clear();
            readSequence<TransactionDetails>(std::inserter(mTransactions,
                                                           mTransactions.end()),
                                             "transactions",
                                             s);
        } else {
            writeSequence<TransactionDetails>(mTransactions.begin(),
                                              mTransactions.end(),
                                              "transactions",
                                              s);
        }

        KV_MEMBER(mSpentKeyImages);
        KV_MEMBER(mSpentOutputs);
        KV_MEMBER(mRecentlyDeletedTransactions);
    }

    void TxMemoryPool::onIdle()
    {
        mTxCheckInterval.call([this]()
                              {
                                  return removeExpiredTransactions();
                              });
    }

    bool TxMemoryPool::removeExpiredTransactions()
    {
        bool somethingRemoved = false;
        {
            std::lock_guard<std::recursive_mutex> lock(mTxLock);

            uint64_t now = mTimeProvider.now();

            for (auto it = mRecentlyDeletedTransactions.begin();
                 it != mRecentlyDeletedTransactions.end();) {
                uint64_t elapsedTimeSinceDeletion = now - it->second;
                if (elapsedTimeSinceDeletion > mCurrency.numberOfPeriodsToForgetTxDeletedFromPool()
                    * mCurrency.mempoolTxLiveTime()) {
                    it = mRecentlyDeletedTransactions.erase(it);
                } else {
                    ++it;
                }
            }

            for (auto it = mTransactions.begin(); it != mTransactions.end();) {
                uint64_t txAge = now -it->receiveTime;
                bool remove = txAge > (it->keptByBlock ?
                                                       mCurrency.mempoolTxFromAltBlockLiveTime() :
                                       mCurrency.mempoolTxLiveTime());
                auto ttlIt = mTtlIndex.find(it->id);
                bool ttlExpired = (ttlIt != mTtlIndex.end() && ttlIt->second <= now);

                if (remove || ttlExpired) {
                    if (ttlExpired) {
                        logger(Logging::TRACE)
                            << "Tx "
                            << it->id
                            << " removed from tx pool due to expired TTL, TTL : "
                            << ttlIt->second;
                    } else {
                        logger(Logging::TRACE)
                            << "Tx "
                            << it->id
                            << " removed from tx pool due to outdated, age: "
                            << txAge;
                    }

                    mRecentlyDeletedTransactions.emplace(it->id, now);
                    it = removeTransaction(it);
                    somethingRemoved = true;
                } else {
                    ++ it;
                }
            }
        }

        if (somethingRemoved) {
            mObserverManager.notify(&ITxPoolObserver::txDeletedFromPool);
        }

        return true;
    }

    TxMemoryPool::TxContainerT::iterator TxMemoryPool::removeTransaction(TxContainerT::iterator i)
    {
        removeTransactionInputs(i->id, i->tx, i->keptByBlock);
        mPaymentIdIndex.remove(i->tx);
        mTimestampIndex.remove(i->receiveTime, i->id);
        mTtlIndex.erase(i->id);
        if (mValidatedTransactions.find(i->id) != mValidatedTransactions.end()) {
            mValidatedTransactions.erase(i->id);
            logger(Logging::DEBUGGING)
                << "Removing transaction from MemPool cache "
                << i->id
                << ". Cache size: "
                << mValidatedTransactions.size();
        }

        return mTransactions.erase(i);
    }

    bool TxMemoryPool::removeTransactionInputs(const Crypto::Hash &id,
                                               const Transaction &tx,
                                               bool keptByBlock)
    {
        for (const auto &in : tx.inputs) {
            if (in.type() == typeid(KeyInput)) {
                const auto &txIn = boost::get<KeyInput>(in);
                auto it = mSpentKeyImages.find(txIn.keyImage);
                if (!(it != mSpentKeyImages.end())) {
                    logger (Logging::ERROR, Logging::BRIGHT_RED)
                        << "failed to find transaction input in key images. img="
                        << txIn.keyImage
                        << std::endl
                        << "transaction id = " << id;
                    return false;
                }
                std::unordered_set<Crypto::Hash> &keyImageSet = it->second;

                if(!(keyImageSet.empty())) {
                    logger (Logging::ERROR, Logging::BRIGHT_RED)
                        << "empty keyImage set, img="
                        << txIn.keyImage
                        << std::endl
                        << "transaction id = "
                        << id;
                    return false;
                }

                auto itInSet = keyImageSet.find(id);
                if (!(itInSet != keyImageSet.end())) {
                    logger (Logging::ERROR, Logging::BRIGHT_RED)
                        << "transaction id not found in keyImage set, img="
                        << txIn.keyImage
                        << std::endl
                        << "transaction id = "
                        << id;
                    return false;
                }

                keyImageSet.erase(itInSet);
                if (keyImageSet.empty()) {
                    /*!
                     * it is now empty hash container for this keyImage
                     */
                    mSpentKeyImages.erase(it);
                }
            } else if (in.type() == typeid(MultisignatureInput)) {
                if (!keptByBlock) {
                    const auto &mSig = boost::get<MultisignatureInput>(in);
                    auto output = GlobalOutput(mSig.amount, mSig.outputIndex);
                    assert (mSpentOutputs.count(output));
                    mSpentOutputs.erase(output);
                }
            }
        }

        return true;
    }

    bool TxMemoryPool::addTransactionInputs(const Crypto::Hash &id,
                                            const Transaction &tx,
                                            bool keptByBlock)
    {
        /*!
         * should not fail
         */
        for (const auto &in : tx.inputs) {
            if (in.type() == typeid(KeyInput)) {
                const auto  &txIn = boost::get<KeyInput>(in);
                std::unordered_set<Crypto::Hash> &keyImageSet = mSpentKeyImages[txIn.keyImage];
                if (!(keptByBlock || keyImageSet.size())) {
                    logger (Logging::ERROR, Logging::BRIGHT_RED)
                        << "internal error: keptByBlock=" << keptByBlock
                        << ",  kei_image_set.size()=" << keyImageSet.size() << ENDL
                        << "txin.keyImage=" << txIn.keyImage << ENDL << "id=" << id;
                    return false;
                }

                auto insRes = keyImageSet.insert(id);
            } else if (in.type() == typeid(MultisignatureInput)) {
                if (!keptByBlock) {
                    const auto &mSig = boost::get<MultisignatureInput>(in);
                    auto r = mSpentOutputs.insert(GlobalOutput(mSig.amount, mSig.outputIndex));
                    (void)r;
                    assert(r.second);
                }
            }
        }

        return true;
    }

    bool TxMemoryPool::haveSpentInputs(const Transaction &tx) const
    {
        for (const auto &in : tx.inputs) {
            if (in.type() == typeid(KeyInput)) {
                const auto &toKeyIn = boost::get<KeyInput>(in);
                if (mSpentKeyImages.count(toKeyIn.keyImage)) {
                    return true;
                }
            } else if (in.type() == typeid(MultisignatureInput)) {
                const auto &mSig = boost::get<MultisignatureInput>(in);
                if (mSpentOutputs.count(GlobalOutput(mSig.amount, mSig.outputIndex))) {
                    return true;
                }
            }
        }

        return false;
    }

    bool TxMemoryPool::addObserver(ITxPoolObserver *observer)
    {
        return mObserverManager.add(observer);
    }

    bool TxMemoryPool::removeObserver(ITxPoolObserver *observer)
    {
        return mObserverManager.remove(observer);
    }

    void TxMemoryPool::buildIndices()
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        for (auto it = mTransactions.begin(); it != mTransactions.end(); it++) {
            mPaymentIdIndex.add(it->tx);
            mTimestampIndex.add(it->receiveTime, it->id);

            std::vector<TransactionExtraField> txExtraFields;
            parseTransactionExtra(it->tx.extra, txExtraFields);
            TransactionExtraTTL ttl;
            if (findTransactionExtraFieldByType(txExtraFields, ttl)) {
                if (ttl.ttl != 0) {
                    mTtlIndex.emplace(std::make_pair(it->id, ttl.ttl));
                }
            }
        }
    }

    bool TxMemoryPool::getTransactionIdsByPaymentId(const Crypto::Hash &paymentId,
                                                    std::vector<Crypto::Hash> &txIds)
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        txIds = mPaymentIdIndex.find(paymentId);

        return true;
    }

    bool TxMemoryPool::getTransactionIdsByTimestamp(uint64_t timestampBegin,
                                                    uint64_t timestampEnd,
                                                    uint32_t txNumLimit,
                                                    std::vector<Crypto::Hash> &hashes,
                                                    uint64_t &txNumWithinTimestamps)
    {
        std::lock_guard<std::recursive_mutex> lock(mTxLock);
        return mTimestampIndex.find(timestampBegin,
                                    timestampEnd,
                                    txNumLimit,
                                    hashes,
                                    txNumWithinTimestamps);
    }

    // TransactionPool

    /*!
        lhs > hrs
    */
    bool TransactionPool::TransactionPriorityComparator::operator()(const PendingTransactionInfo &lhs,
                                                                    const PendingTransactionInfo &rhs) const
    {
        const CachedTransaction &left = lhs.cachedTransaction;
        const CachedTransaction &right = rhs.cachedTransaction;

        /*!
            price(lhs) = lhs.fee / lhs.blobSize
            price(lhs) > price(rhs) -->
            lhs.fee / lhs.blobSize > rhs.fee / rhs.blobSize -->
            lhs.fee * rhs.blobSize > rhs.fee * lhs.blobSize
        */
        uint64_t lhs_hi, lhs_lo = mul128 (left.getTransactionFee (),
                                          right.getTransactionBinaryArray ().size (),
                                          &lhs_hi);
        uint64_t rhs_hi, rhs_lo = mul128 (right.getTransactionFee (),
                                          left.getTransactionBinaryArray ().size (),
                                          &rhs_hi);

        return
            /*!
                prefer more profitable transactions
            */
            (lhs_hi > rhs_hi) ||
            (lhs_hi == rhs_hi && lhs_lo > rhs_lo) ||

            /*!
                prefer smaller
            */
            (
                lhs_hi == rhs_hi &&
                lhs_lo == rhs_lo &&
                left.getTransactionBinaryArray ().size () <
                right.getTransactionBinaryArray ().size ()) ||

            /*!
                prefer older
            */
            (
                lhs_hi == rhs_hi &&
                lhs_lo == rhs_lo &&
                left.getTransactionBinaryArray ().size () ==
                right.getTransactionBinaryArray ().size () &&
                lhs.receiveTime < rhs.receiveTime
            );
    }

    const Crypto::Hash &TransactionPool::PendingTransactionInfo::getTransactionHash() const
    {
        return cachedTransaction.getTransactionHash ();
    }

    size_t TransactionPool::PaymentIdHasher::operator()(const boost::optional<Crypto::Hash> &paymentId) const
    {
        if (!paymentId) {
            return std::numeric_limits<size_t>::max ();
        }

        return std::hash<Crypto::Hash>{} (*paymentId);
    }

    TransactionPool::TransactionPool(std::shared_ptr<Logging::ILogger> logger)
        : transactionHashIndex (transactions.get<TransactionHashTag> ()),
          transactionCostIndex (transactions.get<TransactionCostTag> ()),
          paymentIdIndex (transactions.get<PaymentIdTag> ()),
          logger (logger, "TransactionPool")
    {
    }

    bool TransactionPool::pushTransaction(CachedTransaction &&transaction,
                                          TransactionValidatorState &&transactionState)
    {
        auto pendingTx = PendingTransactionInfo
            {
                static_cast<uint64_t>(time (nullptr)),
                std::move (transaction)
            };

        Crypto::Hash paymentId;
        if (getPaymentIdFromTxExtra (pendingTx.cachedTransaction.getTransaction ().extra, paymentId)) {
            pendingTx.paymentId = paymentId;
        }

        if (transactionHashIndex.count (pendingTx.getTransactionHash ()) > 0) {
            logger (Logging::DEBUGGING)
                << "pushTransaction: transaction hash already present in index";
            return false;
        }

        if (hasIntersections (poolState, transactionState)) {
            logger (Logging::DEBUGGING)
                << "pushTransaction: failed to merge states, some keys already used";
            return false;
        }

        mergeStates (poolState, transactionState);

        logger (Logging::DEBUGGING)
            << "pushed transaction "
            << pendingTx.getTransactionHash ()
            << " to pool";
        return transactionHashIndex.insert (std::move (pendingTx)).second;
    }

    const CachedTransaction &TransactionPool::getTransaction(const Crypto::Hash &hash) const
    {
        auto it = transactionHashIndex.find (hash);
        assert(it != transactionHashIndex.end ());

        return it->cachedTransaction;
    }

    bool TransactionPool::removeTransaction(const Crypto::Hash &hash)
    {
        auto it = transactionHashIndex.find (hash);
        if (it == transactionHashIndex.end ()) {
            logger (Logging::DEBUGGING)
                << "removeTransaction: transaction not found";
            return false;
        }

        excludeFromState (poolState, it->cachedTransaction);
        transactionHashIndex.erase (it);

        logger (Logging::DEBUGGING)
            << "transaction "
            << hash
            << " removed from pool";
        return true;
    }

    size_t TransactionPool::getTransactionCount() const
    {
        return transactionHashIndex.size ();
    }

    std::vector<Crypto::Hash> TransactionPool::getTransactionHashes() const
    {
        std::vector<Crypto::Hash> hashes;
        for (auto it = transactionCostIndex.begin (); it != transactionCostIndex.end (); ++it) {
            hashes.push_back (it->getTransactionHash ());
        }

        return hashes;
    }

    bool TransactionPool::checkIfTransactionPresent(const Crypto::Hash &hash) const
    {
        return transactionHashIndex.find (hash) != transactionHashIndex.end ();
    }

    const TransactionValidatorState &TransactionPool::getPoolTransactionValidationState() const
    {
        return poolState;
    }

    std::vector<CachedTransaction> TransactionPool::getPoolTransactions() const
    {
        std::vector<CachedTransaction> result;
        result.reserve (transactionCostIndex.size ());

        for (const auto &transactionItem: transactionCostIndex) {
            result.emplace_back (transactionItem.cachedTransaction);
        }

        return result;
    }

    std::tuple<std::vector<CachedTransaction>, std::vector<CachedTransaction>>
    TransactionPool::getPoolTransactionsForBlockTemplate() const
    {
        std::vector<CachedTransaction> paidTransactions;

        std::vector<CachedTransaction> freeTransactions;

        for (const auto &transaction : transactionCostIndex) {

            /*!
                Cannot Identify Fusions based on Fees alone
            */
            uint64_t transactionFee = transaction.cachedTransaction.getTransactionFee ();

            if (transactionFee != 0) {
                paidTransactions.emplace_back (transaction.cachedTransaction);
            } else {
                freeTransactions.emplace_back (transaction.cachedTransaction);
            }
        }

        return
            {
                paidTransactions,
                freeTransactions
            };
    }

    uint64_t TransactionPool::getTransactionReceiveTime(const Crypto::Hash &hash) const
    {
        auto it = transactionHashIndex.find (hash);
        assert(it != transactionHashIndex.end ());

        return it->receiveTime;
    }

    std::vector<Crypto::Hash>
    TransactionPool::getTransactionHashesByPaymentId(const Crypto::Hash &paymentId) const
    {
        boost::optional<Crypto::Hash> p (paymentId);

        auto range = paymentIdIndex.equal_range (p);
        std::vector<Crypto::Hash> transactionHashes;
        transactionHashes.reserve (std::distance (range.first, range.second));
        for (auto it = range.first; it != range.second; ++it) {
            transactionHashes.push_back (it->getTransactionHash ());
        }

        return transactionHashes;
    }

} // namespace CryptoNote

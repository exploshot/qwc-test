// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2020, The Qwertycoin developers
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

#include <CryptoNoteCore/Blockchain/BlockchainIndices.h>
#include <CryptoNoteCore/Transactions/TransactionExtra.h>

namespace CryptoNote {
    namespace {
        const size_t DEFAULT_BUCKET_COUNT = 5;
    } // namespace

    /*!
     * PaymentIdIndex Class
     */

    PaymentIdIndex::PaymentIdIndex(bool _enabled)
        : enabled(_enabled),
          index(DEFAULT_BUCKET_COUNT, paymentIdHash)
    {
    }

    bool PaymentIdIndex::add(const Transaction &transaction)
    {
        if (!enabled) {
            return false;
        }

        Crypto::Hash paymentId;
        Crypto::Hash txHash = CryptoNote::getObjectHash (transaction);

        if (!getPaymentIdFromTxExtra(transaction.extra, paymentId)) {
            return false;
        }

        index.insert (std::make_pair (paymentId, txHash));

        return true;
    }

    bool PaymentIdIndex::remove(const Transaction &transaction)
    {
        if (!enabled) {
            return false;
        }

        Crypto::Hash paymentId;
        Crypto::Hash txHash = getObjectHash (transaction);

        if (!getPaymentIdFromTxExtra(transaction.extra, paymentId)) {
            return false;
        }

        auto range = index.equal_range(paymentId);
        for (auto iter = range.first; iter != range.second; ++iter) {
            if (iter->second == txHash) {
                index.erase(iter);

                return true;
            }
        }

        return false;
    }

    bool PaymentIdIndex::find(const Crypto::Hash &paymentId,
                              std::vector<Crypto::Hash> &txHashes)
    {
        if (!enabled) {
            throw std::runtime_error("Payment id index disabled");
        }

        bool found = false;
        auto range = index.equal_range(paymentId);
        for (auto iter = range.first; iter != range.second; ++iter) {
            found = true;
            txHashes.emplace_back(iter->second);
        }

        return found;
    }

    std::vector<Crypto::Hash> PaymentIdIndex::find(const Crypto::Hash &paymentId)
    {
        if (!enabled) {
            throw std::runtime_error("Payment id index disabled");
        }

        std::vector<Crypto::Hash> txHashes;
        bool found = false;
        auto range = index.equal_range(paymentId);
        for (auto iter = range.first; iter != range.second; ++iter) {
            txHashes.emplace_back(iter->second);
        }

        return txHashes;
    }

    void PaymentIdIndex::clear()
    {
        if (enabled) {
            index.clear();
        }
    }

    void PaymentIdIndex::serialize(ISerializer &s)
    {
        if (!enabled) {
            throw std::runtime_error("Payment id index disabeld");
        }

        s(index, "index");
    }

    /*!
     * TimestampBlockIndex Class
     */
    TimestampBlocksIndex::TimestampBlocksIndex(bool _enabled)
        : enabled(_enabled)
    {
    }

    bool TimestampBlocksIndex::add(uint64_t timestamp, const Crypto::Hash &hash)
    {
        if (!enabled) {
            return false;
        }

        index.emplace(timestamp, hash);
        return true;
    }

    bool TimestampBlocksIndex::remove(uint64_t timestamp, const Crypto::Hash &hash)
    {
        if (!enabled) {
            return false;
        }

        auto range = index.equal_range(timestamp);
        for (auto iter = range.first; iter != range.second; ++iter) {
            if (iter->second == hash) {
                index.erase(iter);

                return true;
            }
        }

        return false;
    }

    bool TimestampBlocksIndex::find(uint64_t timestampBegin,
                                    uint64_t timestampEnd,
                                    uint32_t hashesNumberLimit,
                                    std::vector<Crypto::Hash> &hashes,
                                    uint32_t &hashesNumberWithinTimestamps)
    {
        if (!enabled) {
            throw std::runtime_error("Timestamp block index disabled");
        }

        uint32_t hashesNumber = 0;
        if (timestampBegin > timestampEnd) {
            return false;
        }

        auto begin = index.lower_bound(timestampBegin);
        auto end = index.upper_bound(timestampEnd);

        hashesNumberWithinTimestamps = static_cast<uint32_t>(std::distance(begin, end));

        for (auto iter = begin; iter != end && hashesNumber < hashesNumberLimit; ++iter) {
            ++hashesNumber;
            hashes.emplace_back(iter->second);
        }

        return hashesNumber > 0;
    }

    void TimestampBlocksIndex::clear()
    {
        if (enabled) {
            index.clear();
        }
    }

    void TimestampBlocksIndex::serialize(ISerializer &s)
    {
        if (!enabled) {
            throw std::runtime_error("Timestamp block index disabled");
        }

        s(index, "index");
    }

    /*!
     * TimestampTransactionIndex Class
     */
    TimestampTransactionsIndex::TimestampTransactionsIndex(bool _enabled)
        : enabled(_enabled)
    {
    }

    bool TimestampTransactionsIndex::add(uint64_t timestamp, const Crypto::Hash &hash)
    {
        if (!enabled) {
            return false;
        }

        index.emplace(timestamp, hash);
        return true;
    }

    bool TimestampTransactionsIndex::remove(uint64_t timestamp,
                                            const Crypto::Hash &hash)
    {
        if (!enabled) {
            return false;
        }

        auto range = index.equal_range(timestamp);
        for (auto iter = range.first; iter != range.second; ++iter) {
            if (iter->second == hash) {
                index.erase(iter);
                return true;
            }
        }

        return false;
    }

    bool TimestampTransactionsIndex::find(uint64_t timestampBegin,
                                          uint64_t timestampEnd,
                                          uint64_t hashesNumberLimit,
                                          std::vector<Crypto::Hash> &hashes,
                                          uint64_t &hashesNumberWithinTimestamps)
    {
        if (!enabled) {
            throw std::runtime_error("Timestamp transaction index disabled");
        }

        uint32_t hashesNumber = 0;
        if (timestampBegin > timestampEnd) {
            return false;
        }

        auto begin = index.lower_bound(timestampBegin);
        auto end = index.lower_bound(timestampEnd);

        hashesNumberWithinTimestamps = static_cast<uint32_t>(std::distance(begin, end));

        for (auto iter = begin; iter != end && hashesNumber < hashesNumberLimit; ++iter) {
            ++hashesNumber;
            hashes.emplace_back(iter->second);
        }

        return hashesNumber > 0;
    }

    void TimestampTransactionsIndex::clear()
    {
        if (enabled) {
            index.clear();
        }
    }

    void TimestampTransactionsIndex::serialize(ISerializer &s)
    {
        if (!enabled) {
            throw std::runtime_error("Timestamp transactions index disabled.");
        }

        s(index, "index");
    }

    GeneratedTransactionIndex::GeneratedTransactionIndex(bool _enabled)
        : lastGeneratedTxNumber(0),
          enabled(_enabled)
    {
    }

    bool GeneratedTransactionIndex::add(const BlockTemplate &block)
    {
        if (!enabled) {
            return false;
        }

        uint32_t blockHeight = boost::get<BaseInput>(block
                                                     .baseTransaction
                                                     .inputs
                                                     .front()).blockIndex;

        if (index.size() != blockHeight) {
            return false;
        }

        bool status = index.emplace(blockHeight,
                                    lastGeneratedTxNumber +
                                    block.transactionHashes.size() +
                                    1).second; //Plus miner tx
        if (status) {
            lastGeneratedTxNumber += block.transactionHashes.size() + 1;
        }
        return status;
    }

    bool GeneratedTransactionIndex::remove(const BlockTemplate &block)
    {
        if (!enabled) {
            return false;
        }

        uint32_t blockHeight = boost::get<BaseInput>(block
                                                     .baseTransaction
                                                     .inputs
                                                     .front()).blockIndex;

        if (blockHeight != index.size() - 1) {
            return false;
        }

        auto iter = index.find(blockHeight);
        assert(iter != index.end());
        index.erase(iter);

        if (blockHeight != 0) {
            iter = index.find(blockHeight - 1);
            assert(iter != index.end());
            lastGeneratedTxNumber = iter->second;
        } else {
            lastGeneratedTxNumber = 0;
        }

        return true;
    }

    bool GeneratedTransactionIndex::find(uint32_t height,
                                          uint64_t& generatedTransactions)
    {
        if (!enabled) {
            throw std::runtime_error("Generated transactions index disabled.");
        }

        if (height > std::numeric_limits<uint32_t>::max()) {
            return false;
        }
        auto iter = index.find(height);
        if (iter == index.end()) {
            return false;
        }
        generatedTransactions = iter->second;

        return true;
    }

    void GeneratedTransactionIndex::clear()
    {
        if (enabled) {
            index.clear();
        }
    }

    void GeneratedTransactionIndex::serialize(ISerializer& s)
    {
        if (!enabled) {
            throw std::runtime_error("Generated transactions index disabled.");
        }

        s(index, "index");
        s(lastGeneratedTxNumber, "lastGeneratedTxNumber");
    }

    OrphanBlocksIndex::OrphanBlocksIndex(bool _enabled)
        : enabled(_enabled)
    {
    }

    bool OrphanBlocksIndex::add(const BlockTemplate& block)
    {
        if (!enabled) {
            return false;
        }

        Crypto::Hash blockHash = getBlockHash(block);
        uint32_t blockHeight = boost::get<BaseInput>(block
                                                     .baseTransaction
                                                     .inputs
                                                     .front()).blockIndex;
        index.emplace(blockHeight, blockHash);

        return true;
    }

    bool OrphanBlocksIndex::remove(const BlockTemplate& block)
    {
        if (!enabled) {
            return false;
        }

        Crypto::Hash blockHash = getBlockHash(block);
        uint32_t blockHeight = boost::get<BaseInput>(block
                                                     .baseTransaction
                                                     .inputs
                                                     .front()).blockIndex;
        auto range = index.equal_range(blockHeight);
        for (auto iter = range.first; iter != range.second; ++iter) {
            if (iter->second == blockHash) {
                index.erase(iter);

                return true;
            }
        }

        return false;
    }

    bool OrphanBlocksIndex::find(uint32_t height,
                                 std::vector<Crypto::Hash>& blockHashes)
    {
        if (!enabled) {
            throw std::runtime_error("Orphan blocks index disabled.");
        }

        if (height > std::numeric_limits<uint32_t>::max()) {
            return false;
        }

        bool found = false;
        auto range = index.equal_range(height);

        for (auto iter = range.first; iter != range.second; ++iter) {
            found = true;
            blockHashes.emplace_back(iter->second);
        }

        return found;
    }

    void OrphanBlocksIndex::clear()
    {
        if (enabled) {
            index.clear();
        }
    }

} // namespace CryptoNote
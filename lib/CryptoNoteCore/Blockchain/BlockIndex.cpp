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

#include <boost/utility/value_init.hpp>

#include <Common/Util.h>

#include <CryptoNoteCore/Blockchain/BlockIndex.h>

#include <Serialization/CryptoNoteSerialization.h>
#include <Serialization/SerializationOverloads.h>

namespace CryptoNote {
    Crypto::Hash BlockIndex::getBlockId(uint32_t height) const
    {
        return mContainer[static_cast<size_t>(height)];
    }

    std::vector<Crypto::Hash> BlockIndex::getBlockIds(uint32_t startBlockIndex,
                                                      uint32_t maxCount) const
    {
        std::vector<Crypto::Hash> result;
        if (startBlockIndex >= mContainer.size()) {
            return result;
        }

        size_t count = std::min(static_cast<size_t>(maxCount),
                                mContainer.size() - static_cast<size_t>(startBlockIndex));
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(mContainer[startBlockIndex + i]);
        }

        return result;
    }

    std::vector<Crypto::Hash> BlockIndex::getBlockIds(uint32_t startBlockIndex,
                                                      uint32_t maxCount,
                                                      BlockchainDB &db) const
    {
        std::vector<Crypto::Hash> result;
        if (startBlockIndex >= mContainer.size()) {
            return result;
        }

        size_t count = std::min(static_cast<size_t>(maxCount),
                                mContainer.size() - static_cast<size_t>(startBlockIndex));
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(mContainer[startBlockIndex + i]);
        }

        return result;
    }

    bool BlockIndex::findSupplement(const std::vector<Crypto::Hash> &ids,
                                    uint32_t &offset) const
    {
        for (const auto &id : ids) {
            if (getBlockHeight(id, offset)) {
                return true;
            }
        }

        return false;
    }

    bool BlockIndex::findSupplement(const std::vector<Crypto::Hash> &ids,
                                    uint32_t &offset,
                                    BlockchainDB &db) const
    {
        /*!
         * TODO: Check if this should return a vector by reference for offset
         */
        for (const auto &id : ids) {
            try {
                offset = db.getBlockHeight(id);
            } catch (...) {
                std::exception e;
                throw e;
                return false;
            }

            return true;
        }

        return false;
    }

    std::vector<Crypto::Hash> BlockIndex::buildSparseChain(const Crypto::Hash &startBlockId) const
    {
        uint32_t startBlockHeight;
        getBlockHeight(startBlockId, startBlockHeight);
        std::vector<Crypto::Hash> result;
        size_t sparceChainEnd = static_cast<size_t>(startBlockHeight + 1);
        for (size_t i = 1; i <= sparceChainEnd; i *= 2) {
            result.emplace_back(mContainer[sparceChainEnd - i]);
        }

        if (result.back() != mContainer[0]) {
            result.emplace_back(mContainer[0]);
        }

        return result;
    }

    std::vector<Crypto::Hash> BlockIndex::buildSparseChain(const Crypto::Hash &startBlockId,
                                                           BlockchainDB &db) const
    {
        uint32_t startBlockHeight = db.getBlockHeight(startBlockId);

        std::vector<Crypto::Hash> result;
        size_t sparseChainEnd = static_cast<size_t>(startBlockHeight + 1);
        for (size_t i = 1; i <= sparseChainEnd; i *= 2) {
            result.emplace_back(db.getBlockHashFromHeight(sparseChainEnd - i));
        }

        if (result.back() != mContainer[0]) {
            result.emplace_back(mContainer[0]);
        }

        return result;
    }

    Crypto::Hash BlockIndex::getTailId() const
    {
        assert(!mContainer.empty());
        return mContainer.back();
    }

    void BlockIndex::serialize(ISerializer &s)
    {
        if (s.type() == ISerializer::INPUT) {
            readSequence<Crypto::Hash>(std::back_inserter(mContainer),
                                                          "index",
                                                          s);
        } else {
            writeSequence<Crypto::Hash>(mContainer.begin(),
                                        mContainer.end(),
                                        "index",
                                        s);
        }
    }

} // namespace CryptoNote
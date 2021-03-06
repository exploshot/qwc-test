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

#include <vector>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/random_access_index.hpp>

#include <Crypto/Hash.h>

#include <CryptoNoteCore/Blockchain/LMDB/BlockchainDB.h>

namespace CryptoNote {
    class ISerializer;

    class BlockIndex
    {
    public:
        BlockIndex()
            : mIndex(mContainer.get<1>())
        {
        }

        void pop()
        {
            mContainer.pop_back();
        }

        /*!
         * returns true if new element was inserted, false if already exists
         */
        bool push(const Crypto::Hash &h)
        {
            auto result = mContainer.push_back(h);

            return result.second;
        }

        bool hasBlock(const Crypto::Hash &h) const
        {
            if (mIndex.find(h) != mIndex.end()) {
                return true;
            }

            return false;
        }

        bool getBlockHeight(const Crypto::Hash &h, uint32_t &height) const
        {
            auto hi = mIndex.find(h);
            if (hi == mIndex.end()) {
                return false;
            }

            height = static_cast<uint32_t>(std::distance(
                                                    mContainer.begin(),
                                                    mContainer.project<0>(hi)));

            return true;
        }

        uint32_t size() const
        {
            return static_cast<uint32_t>(mContainer.size());
        }

        void clear()
        {
            mContainer.clear();
        }

        Crypto::Hash getBlockId(uint32_t height) const;
        Crypto::Hash getBlockId(uint32_t height, BlockchainDB &db) const;
        std::vector<Crypto::Hash> getBlockIds(uint32_t startBlockIndex, uint32_t maxCount) const;
        std::vector<Crypto::Hash> getBlockIds(uint32_t startBlockIndex,
                                              uint32_t maxCount,
                                              BlockchainDB &db) const;
        bool findSupplement(const std::vector<Crypto::Hash> &ids, uint32_t &offset) const;
        bool findSupplement(const std::vector<Crypto::Hash> &ids,
                            uint32_t &offset,
                            BlockchainDB &db) const;
        std::vector<Crypto::Hash> buildSparseChain(const Crypto::Hash &startBlockId,
                                                   BlockchainDB &db) const;
        std::vector<Crypto::Hash> buildSparseChain(const Crypto::Hash &startBlockId) const;
        Crypto::Hash getTailId() const;

        void serialize(ISerializer &s);

    private:
        typedef boost::multi_index_container <
            Crypto::Hash,
            boost::multi_index::indexed_by<
                boost::multi_index::random_access<>,
                boost::multi_index::hashed_unique<boost::multi_index::identity<Crypto::Hash>>
            >
        > ContainerT;

        ContainerT mContainer;
        ContainerT::nth_index<1>::type& mIndex;
    };
}


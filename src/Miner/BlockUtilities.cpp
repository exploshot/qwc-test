// Copyright (c) 2019, The TurtleCoin Developers
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
#include <Common/Varint.h>

#include <Miner/BlockUtilities.h>

#include <Serialization/CryptoNoteSerialization.h>
#include <Serialization/SerializationTools.h>

std::vector<uint8_t> getParentBlockHashingBinaryArray(const CryptoNote::BlockTemplate &block, const bool headerOnly)
{
    return getParentBinaryArray (block, true, headerOnly);
}

std::vector<uint8_t> getParentBlockBinaryArray(const CryptoNote::BlockTemplate &block, const bool headerOnly)
{
    return getParentBinaryArray (block, false, headerOnly);
}

std::vector<uint8_t>
getParentBinaryArray(const CryptoNote::BlockTemplate &block, const bool hashTransaction, const bool headerOnly)
{
    std::vector<uint8_t> binaryArray;

    auto serializer = makeParentBlockSerializer (block, hashTransaction, headerOnly);

    if (!toBinaryArray (serializer, binaryArray)) {
        throw std::runtime_error ("Can't serialize parent block");
    }

    return binaryArray;
}

std::vector<uint8_t> getBlockHashingBinaryArray(const CryptoNote::BlockTemplate &block)
{
    std::vector<uint8_t> blockHashingBinaryArray;

    if (!toBinaryArray (static_cast<const CryptoNote::BlockHeader &>(block), blockHashingBinaryArray)) {
        throw std::runtime_error ("Can't serialize BlockHeader");
    }

    std::vector<Crypto::Hash> transactionHashes;
    transactionHashes.reserve (block.transactionHashes.size () + 1);
    transactionHashes.push_back (getObjectHash (block.baseTransaction));
    transactionHashes.insert (transactionHashes.end (),
                              block.transactionHashes.begin (),
                              block.transactionHashes.end ());

    Crypto::Hash treeHash;

    Crypto::treeHash (transactionHashes.data (), transactionHashes.size (), treeHash);

    blockHashingBinaryArray.insert (blockHashingBinaryArray.end (), treeHash.data, treeHash.data + 32);

    auto transactionCount = Common::asBinaryArray (Tools::getVarintData (block.transactionHashes.size () + 1));

    blockHashingBinaryArray.insert (blockHashingBinaryArray.end (), transactionCount.begin (), transactionCount.end ());

    return blockHashingBinaryArray;
}

Crypto::Hash getBlockHash(const CryptoNote::BlockTemplate &block)
{
    auto blockHashingBinaryArray = getBlockHashingBinaryArray (block);

    if (block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_2
        || block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_3) {
        const auto &parentBlock = getParentBlockHashingBinaryArray (block, false);
        blockHashingBinaryArray.insert (blockHashingBinaryArray.end (), parentBlock.begin (), parentBlock.end ());
    }

    return CryptoNote::getObjectHash (blockHashingBinaryArray);
}

Crypto::Hash getMerkleRoot(const CryptoNote::BlockTemplate &block)
{
    return CryptoNote::getObjectHash (getBlockHashingBinaryArray (block));
}

Crypto::Hash getBlockLongHash(const CryptoNote::BlockTemplate &block)
{
    std::vector<uint8_t> bd;
    if (block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_1
        || block.majorVersion >= CryptoNote::BLOCK_MAJOR_VERSION_4) {
        bd = getBlockHashingBinaryArray (block);
    } else if (block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_2
               || block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_3) {
        bd = getParentBlockHashingBinaryArray (block, true);
    }

    Crypto::Hash hash;

    try {
        const auto hashingAlgorithm
            = CryptoNote::HASHING_ALGORITHMS_BY_BLOCK_VERSION.at (block.majorVersion);

        hashingAlgorithm (bd.data (), bd.size (), hash);

        return hash;
    } catch (const std::out_of_range &) {
        throw std::runtime_error ("Unknown block major version.");
    }
}
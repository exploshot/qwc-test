// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
//
// Please see the included LICENSE file for more information.

#include <Common/CryptoNoteTools.h>
#include <Common/Varint.h>

#include <CryptoNoteCore/Blockchain/CachedBlock.h>

#include <Global/CryptoNoteConfig.h>

using namespace Crypto;
using namespace CryptoNote;

CachedBlock::CachedBlock(const BlockTemplate& block) : block(block) {
}

const BlockTemplate& CachedBlock::getBlock() const {
  return block;
}

const Crypto::Hash& CachedBlock::getTransactionTreeHash() const {
  if (!transactionTreeHash.is_initialized()) {
    std::vector<Crypto::Hash> transactionHashes;
    transactionHashes.reserve(block.transactionHashes.size() + 1);
    transactionHashes.push_back(getObjectHash(block.baseTransaction));
    transactionHashes.insert(transactionHashes.end(), block.transactionHashes.begin(), block.transactionHashes.end());
    transactionTreeHash = Crypto::Hash();
    Crypto::tree_hash(transactionHashes.data(), transactionHashes.size(), transactionTreeHash.get());
  }

  return transactionTreeHash.get();
}

const Crypto::Hash& CachedBlock::getBlockHash() const {
  if (!blockHash.is_initialized()) {
    // std::cout << "Blockhash is not initialized" << std::endl;
    BinaryArray blockBinaryArray = getBlockHashingBinaryArray();
    if (BLOCK_MAJOR_VERSION_2 == block.majorVersion || BLOCK_MAJOR_VERSION_3 == block.majorVersion) {
      // std::cout << "Blocktimestamp: " << block.timestamp << std::endl;
      const auto& parentBlock = getParentBlockHashingBinaryArray(false);
      blockBinaryArray.insert(blockBinaryArray.end(), parentBlock.begin(), parentBlock.end());
    }

    Crypto::Hash tempHash = getObjectHash(blockBinaryArray);
    blockHash = tempHash;
    
    // std::cout << "Blockhash: " << tempHash << std::endl;
  }

  return blockHash.get();
}

const Crypto::Hash& CachedBlock::getBlockLongHash() const
{
    if (blockLongHash.is_initialized())
    {
        return blockLongHash.get();
    }

    std::vector<uint8_t> bd;
    if (block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_1 || block.majorVersion >= CryptoNote::BLOCK_MAJOR_VERSION_4) {
      bd = getBlockHashingBinaryArray();
    } else if (block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_2 || block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_3) {
      bd = getParentBlockHashingBinaryArray(true);
    }
    /*
    const std::vector<uint8_t> &rawHashingBlock = block.majorVersion == CryptoNote::BLOCK_MAJOR_VERSION_1
        ? getBlockHashingBinaryArray()
        : getParentBlockHashingBinaryArray(true);
    */
    blockLongHash = Hash();

    try
    {
        const auto hashingAlgorithm
            = CryptoNote::HASHING_ALGORITHMS_BY_BLOCK_VERSION.at(block.majorVersion);

        hashingAlgorithm(bd.data(), bd.size(), blockLongHash.get());

        return blockLongHash.get();
    }
    catch (const std::out_of_range &)
    {
        throw std::runtime_error("Unknown block major version.");
    }
}

const Crypto::Hash& CachedBlock::getAuxiliaryBlockHeaderHash() const {
  if (!auxiliaryBlockHeaderHash.is_initialized()) {
    auxiliaryBlockHeaderHash = getObjectHash(getBlockHashingBinaryArray());
  }

  return auxiliaryBlockHeaderHash.get();
}

const BinaryArray& CachedBlock::getBlockHashingBinaryArray() const {
  if (!blockHashingBinaryArray.is_initialized()) {
    // std::cout << "blockHashingBinaryArray isnt initialized" << std::endl;
    blockHashingBinaryArray = BinaryArray();
    auto& result = blockHashingBinaryArray.get();

    Crypto::Hash tempHash = getObjectHash(result);
    // std::cout << "result: " << tempHash << std::endl;

    if (!toBinaryArray(static_cast<const BlockHeader&>(block), result)) {
      blockHashingBinaryArray.reset();
      throw std::runtime_error("Can't serialize BlockHeader");
    }

    const Crypto::Hash& treeHash = getTransactionTreeHash();
    // std::cout << "TreeHash: " << treeHash << std::endl;
    result.insert(result.end(), treeHash.data, treeHash.data + 32);
    tempHash = getObjectHash(result);
    // std::cout << "result2: " << tempHash << std::endl;
    auto transactionCount = Common::asBinaryArray(Tools::get_varint_data(block.transactionHashes.size() + 1));
    // std::cout << "TransactionCount: " << transactionCount.size() + 1<< std::endl;
    result.insert(result.end(), transactionCount.begin(), transactionCount.end());
    tempHash = getObjectHash(result);
    // std::cout << "result3: " << tempHash << std::endl;
  }

  return blockHashingBinaryArray.get();
}

const BinaryArray& CachedBlock::getParentBlockBinaryArray(bool headerOnly) const {
  if (headerOnly) {
    if (!parentBlockBinaryArrayHeaderOnly.is_initialized()) {
      auto serializer = makeParentBlockSerializer(block, false, true);
      parentBlockBinaryArrayHeaderOnly = BinaryArray();
      if (!toBinaryArray(serializer, parentBlockBinaryArrayHeaderOnly.get())) {
        parentBlockBinaryArrayHeaderOnly.reset();
        throw std::runtime_error("Can't serialize parent block header.");
      }
    }

    return parentBlockBinaryArrayHeaderOnly.get();
  } else {
    if (!parentBlockBinaryArray.is_initialized()) {
      auto serializer = makeParentBlockSerializer(block, false, false);
      parentBlockBinaryArray = BinaryArray();
      if (!toBinaryArray(serializer, parentBlockBinaryArray.get())) {
        parentBlockBinaryArray.reset();
        throw std::runtime_error("Can't serialize parent block.");
      }
    }

    return parentBlockBinaryArray.get();
  }
}

const BinaryArray& CachedBlock::getParentBlockHashingBinaryArray(bool headerOnly) const {
  std::cout << "CachedBlock.cpp getParentBlockHashingBinaryArray L148" << std::endl;
  if (headerOnly) {
    if (!parentBlockHashingBinaryArrayHeaderOnly.is_initialized()) {
      auto serializer = makeParentBlockSerializer(block, true, true);
      parentBlockHashingBinaryArrayHeaderOnly = BinaryArray();
      if (!toBinaryArray(serializer, parentBlockHashingBinaryArrayHeaderOnly.get())) {
        parentBlockHashingBinaryArrayHeaderOnly.reset();
        throw std::runtime_error("Can't serialize parent block header for hashing.");
      }
    }

    return parentBlockHashingBinaryArrayHeaderOnly.get();
  } else {
    if (!parentBlockHashingBinaryArray.is_initialized()) {
      auto serializer = makeParentBlockSerializer(block, true, false);
      parentBlockHashingBinaryArray = BinaryArray();
      if (!toBinaryArray(serializer, parentBlockHashingBinaryArray.get())) {
        parentBlockHashingBinaryArray.reset();
        throw std::runtime_error("Can't serialize parent block for hashing.");
      }
    }

    return parentBlockHashingBinaryArray.get();
  }
}

uint32_t CachedBlock::getBlockIndex() const {
  if (!blockIndex.is_initialized()) {
    if (block.baseTransaction.inputs.size() != 1) {
      blockIndex = 0;
    } else {
      const auto& in = block.baseTransaction.inputs[0];
      if (in.type() != typeid(BaseInput)) {
        blockIndex = 0;
      } else {
        blockIndex = boost::get<BaseInput>(in).blockIndex;
      }
    }
  }

  return blockIndex.get();
}

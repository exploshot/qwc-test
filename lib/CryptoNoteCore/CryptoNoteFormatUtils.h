// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include <boost/utility/value_init.hpp>

#include <CryptoNoteCore/CryptoNoteBasic.h>
#include <CryptoNoteCore/Blockchain/LMDB/BinaryArrayDataType.h>

#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Serialization/BinaryInputStreamSerializer.h>
#include <Serialization/CryptoNoteSerialization.h>

namespace Logging {
    class ILogger;
} // namespace Logging

namespace CryptoNote {

    bool parseAndValidateTransactionFromBinaryArray(const BinaryArray &txBinaryArray,
                                                    Transaction &tx,
                                                    Crypto::Hash &txHash,
                                                    Crypto::Hash &txPrefixHash);

    bool isOutToAcc(const AccountKeys &acc,
                    const KeyOutput &outKey,
                    const Crypto::KeyDerivation &derivation,
                    size_t keyIndex);
    bool lookupAccOuts(const AccountKeys &acc,
                       const Transaction &tx,
                       const Crypto::PublicKey &txPubKey,
                       std::vector<size_t> &outs,
                       uint64_t &moneyTransfered);
    bool lookupAccOuts(const AccountKeys &acc,
                       const Transaction &tx,
                       std::vector<size_t> &outs,
                       uint64_t &moneyTransfered);
    bool getTxFee(const Transaction &tx, uint64_t &fee);
    uint64_t getTxFee(const Transaction &tx);
    bool generateKeyImageHelper(const AccountKeys &ack,
                                const Crypto::PublicKey &txPublicKey,
                                size_t realOutputIndex,
                                KeyPair &inEphemeral,
                                Crypto::KeyImage &ki);
    bool checkInputTypesSupported(const TransactionPrefix &tx);
    bool checkOutsValid(const TransactionPrefix &tx, std::string *error = nullptr);
    bool checkInputsOverflow(const TransactionPrefix &tx);
    bool checkOutsOverflow(const TransactionPrefix &tx);

    bool getBlockHashingBlob(const Block &b, BinaryArray &blob);
    bool getParentBlockHashingBlob(const Block &b, BinaryArray &blob);
    bool getAuxBlockHeaderHash(const Block &b, Crypto::Hash &res);
    bool getBlockHash(const Block &b, Crypto::Hash &res);
    Crypto::Hash getBlockHash(const Block &b);

    std::vector<uint32_t> relativeOutputOffsetsToAbsolute(const std::vector<uint32_t> &off);
    std::vector<uint32_t> absoluteOutputOffsetsToRelative(const std::vector<uint32_t> &off);

    uint64_t getInputAmount(const Transaction &transaction);
    std::vector<uint64_t> getInputsAmounts(const Transaction &transaction);
    uint64_t getOutputAmount(const Transaction &transaction);
    void decomposeAmount(uint64_t amount, uint64_t dustThreshold, std::vector<uint64_t> &decomposedAmounts);

    /*!
        62387455827 -> 455827 + 7000000 + 80000000 + 300000000 + 2000000000 + 60000000000, 
        where 455827 <= dustThreshold
    */
    template<typename chunkHandlerT, typename dustHandlerT>
    void decomposeAmountIntoDigits(uint64_t amount,
                                   uint64_t dustThreshold,
                                   const chunkHandlerT &chunkHandler,
                                   const dustHandlerT &dustHandler)
    {
        if (0 == amount) {
            return;
        }

        bool isDustHandled = false;
        uint64_t dust = 0;
        uint64_t order = 1;
        while (0 != amount) {
            uint64_t chunk = (amount % 10) * order;
            amount /= 10;
            order *= 10;

            if (dust + chunk <= dustThreshold) {
                dust += chunk;
            } else {
                if (!isDustHandled && 0 != dust) {
                    dustHandler (dust);
                    isDustHandled = true;
                }

                if (0 != chunk) {
                    chunkHandler (chunk);
                }
            }
        }

        if (!isDustHandled && 0 != dust) {
            dustHandler (dust);
        }
    }

    void getTxTreeHash(const std::vector<Crypto::Hash> &txHashes, Crypto::Hash &h);
    Crypto::Hash getTxTreeHash(const std::vector<Crypto::Hash> &txHashes);
    Crypto::Hash getTxTreeHash(const Block &b);
    bool isValidDecomposedAmount(uint64_t amount);

    bool parseAndValidateTxFromBlob(const CryptoNote::blobData &txBlob,
                                    CryptoNote::Transaction &tx,
                                    Crypto::Hash &txHash, Crypto::Hash &txPrefixHash);
    bool parseAndValidateTxFromBlob(const CryptoNote::blobData &txBlob,
                                    CryptoNote::Transaction &tx);
    bool parseAndValidateBlockFromBlob(const CryptoNote::blobData &bBlob, CryptoNote::Block &tx);

    CryptoNote::blobData blockToBlob(const CryptoNote::Block &block);
    bool blockToBlob(const CryptoNote::Block &block, CryptoNote::blobData &blob);
    CryptoNote::blobData txToBlob(const CryptoNote::Transaction &tx);
    bool txToBlob(const CryptoNote::Transaction &tx, CryptoNote::blobData &txBlob);

} // namespace CryptoNote

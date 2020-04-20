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

#include <set>

#include <Common/CryptoNoteTools.h>
#include <Common/Varint.h>

#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/CryptoNoteBasicImpl.h>
#include <CryptoNoteCore/CryptoNoteFormatUtils.h>
#include <CryptoNoteCore/Transactions/TransactionExtra.h>

#include <Global/CryptoNoteConfig.h>

#include <Logging/LoggerRef.h>

#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Serialization/BinaryInputStreamSerializer.h>
#include <Serialization/CryptoNoteSerialization.h>

using namespace Logging;
using namespace Crypto;
using namespace Common;

namespace CryptoNote {

    bool parseAndValidateTransactionFromBinaryArray(const BinaryArray &txBinaryArray,
                                                    Transaction &tx,
                                                    Crypto::Hash &txHash,
                                                    Crypto::Hash &txPrefixHash)
    {
        if (!fromBinaryArray (tx, txBinaryArray)) {
            return false;
        }

        /*!
         * TODO: validate Tx
         */
        CnFastHash (txBinaryArray.data(), txBinaryArray.size(), txHash);
        getObjectHash (*static_cast<TransactionPrefix *>(&tx), txPrefixHash);

        return true;
    }

    bool generateKeyImageHelper(const AccountKeys &ack,
                                const PublicKey &txPublicKey,
                                size_t realOutputIndex,
                                KeyPair &inEphemeral,
                                KeyImage &ki)
    {
        KeyDerivation recv_derivation;
        bool r = generateKeyDerivation (txPublicKey,
                                        ack.viewSecretKey,
                                        recv_derivation);

        assert(r && "key image helper: failed to generateKeyDerivation");

        if (!r) {
            return false;
        }

        r = derivePublicKey (recv_derivation,
                             realOutputIndex,
                             ack.address.spendPublicKey,
                             inEphemeral.publicKey);

        assert(r && "key image helper: failed to derivePublicKey");

        if (!r) {
            return false;
        }

        deriveSecretKey (recv_derivation,
                         realOutputIndex,
                         ack.spendSecretKey,
                         inEphemeral.secretKey);
        generateKeyImage (inEphemeral.publicKey, inEphemeral.secretKey, ki);

        return true;
    }

    bool getTxFee(const Transaction &tx, uint64_t &fee)
    {
        uint64_t amount_in = 0;
        uint64_t amount_out = 0;

        for (const auto &in : tx.inputs) {
            if (in.type () == typeid (KeyInput)) {
                amount_in += boost::get<KeyInput> (in).amount;
            }
        }

        for (const auto &o : tx.outputs) {
            amount_out += o.amount;
        }

        if (!(amount_in >= amount_out)) {
            return false;
        }

        fee = amount_in - amount_out;

        return true;
    }

    uint64_t getTxFee(const Transaction &tx)
    {
        uint64_t r = 0;
        if (!getTxFee (tx, r)) {
            return 0;
        }

        return r;
    }

    std::vector<uint32_t> relativeOutputOffsetsToAbsolute(const std::vector<uint32_t> &off)
    {
        std::vector<uint32_t> res = off;
        for (size_t i = 1; i < res.size (); i++) {
            res[i] += res[i - 1];
        }

        return res;
    }

    std::vector<uint32_t> absoluteOutputOffsetsToRelative(const std::vector<uint32_t> &off)
    {
        if (off.empty ()) {
            return {};
        }

        auto copy = off;

        for (size_t i = 1; i < copy.size (); ++i) {
            copy[i] = off[i] - off[i - 1];
        }

        return copy;
    }

    bool checkInputTypesSupported(const TransactionPrefix &tx)
    {
        for (const auto &in : tx.inputs) {
            if (in.type () != typeid (KeyInput)) {
                return false;
            }
        }

        return true;
    }

    bool checkOutsValid(const TransactionPrefix &tx, std::string *error)
    {
        for (const TransactionOutput &out : tx.outputs) {
            if (out.target.type () == typeid (KeyOutput)) {
                if (out.amount == 0) {
                    if (error) {
                        *error = "Zero amount ouput";
                    }

                    return false;
                }

                if (!checkKey (boost::get<KeyOutput> (out.target).key)) {
                    if (error) {
                        *error = "Output with invalid key";
                    }

                    return false;
                }
            } else {
                if (error) {
                    *error = "Output with invalid type";
                }

                return false;
            }
        }

        return true;
    }

    bool checkInputsOverflow(const TransactionPrefix &tx)
    {
        uint64_t money = 0;

        for (const auto &in : tx.inputs) {
            uint64_t amount = 0;

            if (in.type () == typeid (KeyInput)) {
                amount = boost::get<KeyInput> (in).amount;
            }

            if (money > amount + money) {
                return false;
            }

            money += amount;
        }

        return true;
    }

    bool checkOutsOverflow(const TransactionPrefix &tx)
    {
        uint64_t money = 0;
        for (const auto &o : tx.outputs) {
            if (money > o.amount + money) {
                return false;
            }
            money += o.amount;
        }

        return true;
    }

    bool isOutToAcc(const AccountKeys &acc,
                    const KeyOutput &outKey,
                    const KeyDerivation &derivation,
                    size_t keyIndex)
    {
        PublicKey pk;
        derivePublicKey (derivation, keyIndex, acc.address.spendPublicKey, pk);

        return pk == outKey.key;
    }

    bool isOutToAcc(const AccountKeys &acc,
                    const KeyOutput &outKey,
                    const PublicKey &txPubKey,
                    size_t keyIndex)
    {
        KeyDerivation derivation;
        generateKeyDerivation (txPubKey, acc.viewSecretKey, derivation);

        return isOutToAcc (acc, outKey, derivation, keyIndex);
    }

    bool lookupAccOuts(const AccountKeys &acc,
                       const Transaction &tx,
                       std::vector<size_t> &outs,
                       uint64_t &moneyTransfered)
    {
        PublicKey txPubKey = getTransactionPublicKeyFromExtra(tx.extra);
        if (txPubKey == Constants::NULL_PUBLIC_KEY) {
            return false;
        }

        return lookupAccOuts (acc, tx, txPubKey, outs, moneyTransfered);
    }

    bool lookupAccOuts(const AccountKeys &acc,
                       const Transaction &tx,
                       const Crypto::PublicKey &txPubKey,
                       std::vector<size_t> &outs,
                       uint64_t &moneyTransfered)
    {
        moneyTransfered = 0;
        size_t keyIndex = 0;
        size_t outputIndex = 0;

        KeyDerivation derivation;
        generateKeyDerivation(txPubKey, acc.viewSecretKey, derivation);

        for (const TransactionOutput &o : tx.outputs) {
            assert(o.target.type() == typeid (KeyOutput) || o.target.type() == typeid (MultisignatureOutput));
            if (o.target.type() == typeid (KeyOutput)) {
                if (isOutToAcc (acc, boost::get<KeyOutput> (o.target), derivation, keyIndex)) {
                    outs.push_back(outputIndex);
                    moneyTransfered += o.amount;
                }

                ++keyIndex;
            } else if (o.target.type() == typeid (MultisignatureOutput)) {
                keyIndex += boost::get<MultisignatureOutput>(o.target).keys.size();
            }

            ++outputIndex;
        }

        return true;
    }

    bool getBlockHashingBlob(const Block &b, BinaryArray &blob)
    {
        if (!toBinaryArray (static_cast<const BlockHeader &>(b), blob)) {
            return false;
        }

        Hash treeRootHash = getTxTreeHash(b);
        blob.insert(blob.end(), treeRootHash.data, treeRootHash.data + 32);
        auto txCount = asBinaryArray(Tools::getVarintData(b.transactionHashes.size() + 1));
        blob.insert(blob.end(), txCount.begin(), txCount.end());

        return true;
    }

    bool getParentBlockHashingBlob(const Block &b, BinaryArray &blob)
    {
        auto serializer = makeParentBlockSerializer(b, true, true);

        return toBinaryArray (serializer, blob);
    }

    bool getBlockHash(const Block &b, Crypto::Hash &res)
    {
        BinaryArray bA;
        if (!getBlockHashingBlob (b, bA)) {
            return false;
        }

        /*!
         * The header of block version 1 differs from headers of blocks starting from v.2
         */
        if (BLOCK_MAJOR_VERSION_2 == b.majorVersion || BLOCK_MAJOR_VERSION_3 == b.majorVersion) {
            BinaryArray parentBlob;
            auto serializer = makeParentBlockSerializer (b, true, false);
            if (!toBinaryArray (serializer, parentBlob)) {
                return false;
            }

            bA.insert(bA.end(), parentBlob.begin(), parentBlob.end());
        }

        return getObjectHash(bA, res);
    }

    Hash getBlockHash(const Block &b)
    {
        Hash p = Constants::NULL_HASH;
        getBlockHash(b, p);

        return p;
    }

    bool getAuxBlockHeaderHash(const Block &b, Crypto::Hash &res)
    {
        BinaryArray blob;
        if (!getBlockHashingBlob (b, blob)) {
            return false;
        }

        return getObjectHash (blob, res);
    }



    uint64_t getInputAmount(const Transaction &transaction)
    {
        uint64_t amount = 0;
        for (auto &input : transaction.inputs) {
            if (input.type () == typeid (KeyInput)) {
                amount += boost::get<KeyInput> (input).amount;
            }
        }

        return amount;
    }

    std::vector<uint64_t> getInputsAmounts(const Transaction &transaction)
    {
        std::vector<uint64_t> inputsAmounts;
        inputsAmounts.reserve (transaction.inputs.size ());

        for (auto &input: transaction.inputs) {
            if (input.type () == typeid (KeyInput)) {
                inputsAmounts.push_back (boost::get<KeyInput> (input).amount);
            }
        }

        return inputsAmounts;
    }

    uint64_t getOutputAmount(const Transaction &transaction)
    {
        uint64_t amount = 0;
        for (auto &output : transaction.outputs) {
            amount += output.amount;
        }

        return amount;
    }

    void decomposeAmount(uint64_t amount,
                         uint64_t dustThreshold,
                         std::vector<uint64_t> &decomposedAmounts)
    {
        decomposeAmountIntoDigits (amount,
                                   dustThreshold,
                                   [&](uint64_t amount)
                                   {
                                       decomposedAmounts.push_back (amount);
                                   }, [&](uint64_t dust)
                                   {
                                       decomposedAmounts.push_back (dust);
                                   });
    }

    void getTxTreeHash(const std::vector<Crypto::Hash> &txHashes, Crypto::Hash &h)
    {
        treeHash(txHashes.data(), txHashes.size(), h);
    }

    Hash getTxTreeHash(const std::vector<Crypto::Hash> &txHashes)
    {
        Hash h = Constants::NULL_HASH;
        getTxTreeHash(txHashes, h);
        return h;
    }

    Hash getTxTreeHash(const Block &b)
    {
        std::vector<Hash> txIds;
        Hash h = Constants::NULL_HASH;
        getObjectHash(b.baseTransaction, h);
        txIds.push_back(h);
        for (auto &tH : b.transactionHashes) {
            txIds.push_back (tH);
        }

        return getTxTreeHash(txIds);
    }

    bool parseAndValidateTxFromBlob(const CryptoNote::blobData &txBlob,
                                    CryptoNote::Transaction &tx,
                                    Crypto::Hash &txHash,
                                    Crypto::Hash &txPrefixHash)
    {
        std::stringstream ss;
        ss << txBlob;
        BinaryArray bA = fromHex (ss.str().c_str());
        bA.pop_back();
        bool r = parseAndValidateTransactionFromBinaryArray(bA, tx, txHash, txPrefixHash);

        return r;
    }

    bool parseAndValidateTxFromBlob(const CryptoNote::blobData &txBlob,
                                    CryptoNote::Transaction &tx)
    {
        BinaryArray bA = asBinaryArray (txBlob.c_str());
        bA.pop_back();
        Crypto::Hash txHash, txPrefixHash;
        bool r = parseAndValidateTransactionFromBinaryArray(bA, tx, txHash, txPrefixHash);

        return r;
    }

    bool parseAndValidateBlockFromBlob(const CryptoNote::blobData &bBlob,
                                       CryptoNote::Block &tx)
    {
        std::stringstream ss;
        ss << bBlob;
        BinaryArchive<true> bA(ss);
        bool r = Serial::serialize (bA, tx);
        if (!r) {
            return false;
        }

        return true;
    }

    blobData blockToBlob(const CryptoNote::Block &block)
    {
        blobData bD;
        BinaryArray bA = storeToBinary(block);
        bD = Common::asString(bA);
        return bD;
    }

    blobData txToBlob(const CryptoNote::Transaction &tx)
    {
        blobData bD;
        BinaryArray bA = storeToBinary(tx);
        bD = Common::asString(bA);
        return bD;
    }

    bool txToBlob(const CryptoNote::Transaction &tx, CryptoNote::blobData &txBlob)
    {
        BinaryArray bA = storeToBinary(tx);
        txBlob = Common::asString(bA);
        return !txBlob.empty();
    }

} // namespace CryptoNote

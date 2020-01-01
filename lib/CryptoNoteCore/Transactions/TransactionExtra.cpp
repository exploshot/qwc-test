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

#include <Common/IIntUtil.h>
#include <Common/MemoryInputStream.h>
#include <Common/StreamTools.h>
#include <Common/StringTools.h>
#include <Common/Varint.h>

#include <CryptoNoteCore/Transactions/TransactionExtra.h>

#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Serialization/BinaryInputStreamSerializer.h>
#include <Serialization/SerializationTools.h>

using namespace Crypto;
using namespace Common;

namespace CryptoNote {

    bool parseTransactionExtra(const BinaryArray &transactionExtra,
                               std::vector<TransactionExtraField> &transactionExtraFields)
    {
        transactionExtraFields.clear ();

        if (transactionExtra.empty ()) {
            return true;
        }

        bool seenTxExtraTagPadding = false;
        bool seenTxExtraTagPubKey = false;
        bool seenTxExtraNonce = false;
        bool seenTxExtraMergeMiningTag = false;

        try {
            MemoryInputStream iss (transactionExtra.data (), transactionExtra.size ());
            BinaryInputStreamSerializer ar (iss);

            int c = 0;

            while (!iss.endOfStream ()) {
                c = read < uint8_t > (iss);
                switch (c) {
                    case TX_EXTRA_TAG_PADDING: {
                        if (seenTxExtraTagPadding) {
                            return true;
                        }

                        seenTxExtraTagPadding = true;

                        size_t size = 1;
                        for (; !iss.endOfStream () && size <= TX_EXTRA_PADDING_MAX_COUNT; ++size) {
                            if (read < uint8_t > (iss) != 0) {
                                return false; // all bytes should be zero
                            }
                        }

                        if (size > TX_EXTRA_PADDING_MAX_COUNT) {
                            return false;
                        }

                        transactionExtraFields.push_back (TransactionExtraPadding{size});

                        break;
                    }

                    case TX_EXTRA_TAG_PUBKEY: {
                        if (seenTxExtraTagPubKey) {
                            return true;
                        }

                        seenTxExtraTagPubKey = true;

                        TransactionExtraPublicKey extraPk;
                        ar (extraPk.publicKey, "public_key");
                        transactionExtraFields.push_back (extraPk);

                        break;
                    }

                    case TX_EXTRA_NONCE: {
                        if (seenTxExtraNonce) {
                            return true;
                        }

                        seenTxExtraNonce = true;

                        TransactionExtraNonce extraNonce;
                        uint8_t size = read < uint8_t > (iss);

                        if (size > 0) {
                            extraNonce.nonce.resize (size);
                            read (iss, extraNonce.nonce.data (), extraNonce.nonce.size ());
                        }

                        transactionExtraFields.push_back (extraNonce);

                        break;
                    }

                    case TX_EXTRA_MERGE_MINING_TAG: {
                        if (seenTxExtraMergeMiningTag) {
                            break;
                        }

                        seenTxExtraMergeMiningTag = true;

                        TransactionExtraMergeMiningTag mmTag;
                        ar (mmTag, "mm_tag");
                        transactionExtraFields.push_back (mmTag);

                        break;
                    }

                    case TX_EXTRA_MESSAGE_TAG : {
                        TransactionExtraMessage message;
                        ar (message.data, "message");
                        transactionExtraFields.push_back (message);
                        break;
                    }

                    case TX_EXTRA_TTL : {
                        uint8_t size;
                        readVarint (iss, size);
                        TransactionExtraTTL ttl;
                        readVarint (iss, ttl.ttl);
                        transactionExtraFields.push_back (ttl);
                        break;
                    }

                    case TX_EXTRA_SENDER_TAG : {
                        TransactionExtraSender sender;
                        ar (sender.data, "sender");
                        transactionExtraFields.push_back (sender);
                        break;
                    }
                }
            }
        } catch (std::exception &) {
            return false;
        }

        return true;
    }

    struct ExtraSerializerVisitor: public boost::static_visitor<bool>
    {
        BinaryArray &extra;

        ExtraSerializerVisitor(BinaryArray &txExtra)
            : extra (txExtra)
        {
        }

        bool operator()(const TransactionExtraPadding &t)
        {
            if (t.size > TX_EXTRA_PADDING_MAX_COUNT) {
                return false;
            }
            extra.insert (extra.end (), t.size, 0);

            return true;
        }

        bool operator()(const TransactionExtraPublicKey &t)
        {
            return addTransactionPublicKeyToExtra (extra, t.publicKey);
        }

        bool operator()(const TransactionExtraNonce &t)
        {
            return addExtraNonceToTransactionExtra (extra, t.nonce);
        }

        bool operator()(const TransactionExtraMergeMiningTag &t)
        {
            return appendMergeMiningTagToExtra (extra, t);
        }

        bool operator()(const TransactionExtraMessage &t)
        {
            return appendMessageToExtra (extra, t);
        }

        bool operator()(const TransactionExtraTTL &t)
        {
            appendTTLToExtra (extra, t.ttl);
            return true;
        }

        bool operator()(const TransactionExtraSender &t)
        {
            return appendSenderToExtra (extra, t);
        }
    };

    bool writeTransactionExtra(BinaryArray &txExtra,
                               const std::vector<TransactionExtraField> &txExtraFields)
    {
        ExtraSerializerVisitor visitor (txExtra);

        for (const auto &tag : txExtraFields) {
            if (!boost::apply_visitor (visitor, tag)) {
                return false;
            }
        }

        return true;
    }

    PublicKey getTransactionPublicKeyFromExtra(const BinaryArray &txExtra)
    {
        std::vector<TransactionExtraField> txExtraFields;
        parseTransactionExtra (txExtra, txExtraFields);

        TransactionExtraPublicKey pub_key_field;
        if (!findTransactionExtraFieldByType (txExtraFields, pub_key_field)) {
            return Crypto::PublicKey ();
        }

        return pub_key_field.publicKey;
    }

    bool addTransactionPublicKeyToExtra(BinaryArray &txExtra,
                                        const PublicKey &txPubKey)
    {
        txExtra.resize (txExtra.size () + 1 + sizeof (PublicKey));
        txExtra[txExtra.size () - 1 - sizeof (PublicKey)] = TX_EXTRA_TAG_PUBKEY;
        *reinterpret_cast<PublicKey *>(&txExtra[txExtra.size () - sizeof (PublicKey)]) = txPubKey;

        return true;
    }

    bool addExtraNonceToTransactionExtra(BinaryArray &txExtra,
                                         const BinaryArray &extra_nonce)
    {
        if (extra_nonce.size () > TX_EXTRA_NONCE_MAX_COUNT) {
            return false;
        }

        size_t start_pos = txExtra.size ();
        txExtra.resize (txExtra.size () + 2 + extra_nonce.size ());
        //write tag
        txExtra[start_pos] = TX_EXTRA_NONCE;
        //write len
        ++start_pos;
        txExtra[start_pos] = static_cast<uint8_t>(extra_nonce.size ());
        //write data
        ++start_pos;
        memcpy (&txExtra[start_pos], extra_nonce.data (), extra_nonce.size ());

        return true;
    }

    bool appendMergeMiningTagToExtra(BinaryArray &txExtra,
                                     const TransactionExtraMergeMiningTag &mm_tag)
    {
        BinaryArray blob;
        if (!toBinaryArray (mm_tag, blob)) {
            return false;
        }

        txExtra.push_back (TX_EXTRA_MERGE_MINING_TAG);
        std::copy (reinterpret_cast<const uint8_t *>(
                       blob.data ()),
                   reinterpret_cast<const uint8_t *>(blob.data () + blob.size ()), std::back_inserter (txExtra));

        return true;
    }

    bool getMergeMiningTagFromExtra(const BinaryArray &txExtra,
                                    TransactionExtraMergeMiningTag &mm_tag)
    {
        std::vector<TransactionExtraField> txExtraFields;
        parseTransactionExtra (txExtra, txExtraFields);

        return findTransactionExtraFieldByType (txExtraFields, mm_tag);
    }

    bool appendMessageToExtra(BinaryArray &txExtra, const TransactionExtraMessage &message)
    {
        BinaryArray blob;
        if (!toBinaryArray (message, blob)) {
            return false;
        }

        txExtra.reserve (txExtra.size () + 1 + blob.size ());
        txExtra.push_back (TX_EXTRA_MESSAGE_TAG);
        std::copy (reinterpret_cast<const uint8_t *>(blob.data ()),
                   reinterpret_cast<const uint8_t *>(blob.data () + blob.size ()),
                   std::back_inserter (txExtra));

        return true;
    }

    void appendTTLToExtra(BinaryArray &txExtra, uint64_t ttl)
    {
        std::string ttlData = Tools::getVarintData (ttl);
        std::string extraFieldSize = Tools::getVarintData (ttlData.size ());

        txExtra.reserve (txExtra.size () + 1 + extraFieldSize.size () + ttlData.size ());
        txExtra.push_back (TX_EXTRA_TTL);
        std::copy (extraFieldSize.begin (), extraFieldSize.end (), std::back_inserter (txExtra));
        std::copy (ttlData.begin (), ttlData.end (), std::back_inserter (txExtra));
    }

    bool appendSenderToExtra(BinaryArray &txExtra, const TransactionExtraSender &sender)
    {
        BinaryArray blob;
        if (!toBinaryArray (sender, blob)) {
            return false;
        }

        txExtra.reserve (txExtra.size () + 1 + blob.size ());
        txExtra.push_back (TX_EXTRA_SENDER_TAG);
        std::copy (reinterpret_cast<const uint8_t *>(blob.data ()),
                   reinterpret_cast<const uint8_t *>(blob.data () + blob.size ()),
                   std::back_inserter (txExtra)
        );

        return true;
    }

    std::vector<std::string> getMessagesFromExtra(const BinaryArray &extra,
                                                  const Crypto::PublicKey &txKey,
                                                  Crypto::SecretKey *recipientSecretKey)
    {
        std::vector<TransactionExtraField> TxExtraFields;
        std::vector<std::string> result;

        if (!parseTransactionExtra (extra, TxExtraFields)) {
            return result;
        }

        size_t i = 0;

        for (const auto &field : TxExtraFields) {
            if (field.type () != typeid (TransactionExtraMessage)) {
                continue;
            }

            std::string res;
            if (boost::get<TransactionExtraMessage> (field).decrypt (i, txKey, recipientSecretKey, res)) {
                result.push_back (res);
            }
            ++i;
        }

        return result;
    }

    std::vector<std::string> getSendersFromExtra(const BinaryArray &extra,
                                                 const Crypto::PublicKey &txKey,
                                                 Crypto::SecretKey *recipientSecretKey)
    {
        std::vector<TransactionExtraField> TxExtraFields;
        std::vector<std::string> result;

        if (!parseTransactionExtra (extra, TxExtraFields)) {
            return result;
        }

        size_t i = 0;

        for (const auto &field : TxExtraFields) {
            if (field.type () != typeid (TransactionExtraSender)) {
                continue;
            }

            std::string res;
            if (boost::get<TransactionExtraSender> (field).decrypt (i, txKey, recipientSecretKey, res)) {
                result.push_back (res);
            }
            ++i;
        }

        return result;
    }

    void setPaymentIdToTransactionExtraNonce(BinaryArray &extra_nonce,
                                             const Hash &payment_id)
    {
        extra_nonce.clear ();
        extra_nonce.push_back (TX_EXTRA_NONCE_PAYMENT_ID);
        const uint8_t *payment_id_ptr = reinterpret_cast<const uint8_t *>(&payment_id);
        std::copy (payment_id_ptr, payment_id_ptr + sizeof (payment_id),
                   std::back_inserter (extra_nonce));
    }

    bool getPaymentIdFromTransactionExtraNonce(const BinaryArray &extra_nonce,
                                               Hash &payment_id)
    {
        if (sizeof (Hash) + 1 != extra_nonce.size ()) {
            return false;
        }

        if (TX_EXTRA_NONCE_PAYMENT_ID != extra_nonce[0]) {
            return false;
        }

        payment_id = *reinterpret_cast<const Hash *>(extra_nonce.data () + 1);

        return true;
    }

    bool parsePaymentId(const std::string &paymentIdString,
                        Hash &paymentId)
    {
        return Common::podFromHex (paymentIdString, paymentId);
    }

    bool createTxExtraWithPaymentId(const std::string &paymentIdString,
                                    BinaryArray &extra)
    {
        Hash paymentIdBin;

        if (!parsePaymentId (paymentIdString, paymentIdBin)) {
            return false;
        }

        BinaryArray extraNonce;
        CryptoNote::setPaymentIdToTransactionExtraNonce (extraNonce, paymentIdBin);

        if (!CryptoNote::addExtraNonceToTransactionExtra (extra, extraNonce)) {
            return false;
        }

        return true;
    }

    bool getPaymentIdFromTxExtra(const BinaryArray &extra,
                                 Hash &paymentId)
    {
        std::vector<TransactionExtraField> txExtraFields;

        if (!parseTransactionExtra (extra, txExtraFields)) {
            return false;
        }

        TransactionExtraNonce extra_nonce;

        if (findTransactionExtraFieldByType (txExtraFields, extra_nonce)) {
            if (!getPaymentIdFromTransactionExtraNonce (extra_nonce.nonce, paymentId)) {
                return false;
            }
        } else {
            return false;
        }

        return true;
    }

#pragma pack(push, 1)
    struct MessageKeyData
    {
        KeyDerivation derivation;
        uint8_t magic1;
        uint8_t magic2;
    };

    struct SenderKeyData
    {
        KeyDerivation derivation;
        uint8_t magic1;
        uint8_t magic2;
    };
#pragma pop(pop)

    static_assert (sizeof (MessageKeyData) == 34, "Invalid structure size");
    static_assert (sizeof (SenderKeyData) == 34, "Invalid structure size");

    bool TransactionExtraMessage::encrypt(std::size_t index,
                                          const std::string &message,
                                          AccountPublicAddress *recipient,
                                          const KeyPair &txKey)
    {
        size_t messageLength = message.size ();
        std::unique_ptr<char[]> buf (new char[messageLength + TX_EXTRA_MESSAGE_CHECKSUM_SIZE]);
        memcpy (buf.get (), message.data (), messageLength);
        memset (buf.get () + messageLength, 0, TX_EXTRA_MESSAGE_CHECKSUM_SIZE);

        messageLength += TX_EXTRA_MESSAGE_CHECKSUM_SIZE;
        if (recipient) {
            MessageKeyData keyData;
            if (!generateKeyDerivation (recipient->spendPublicKey, txKey.secretKey, keyData.derivation)) {
                return false;
            }

            keyData.magic1 = 0x80;
            keyData.magic2 = 0;

            Hash h = CnFastHash (&keyData, sizeof (MessageKeyData));
            uint64_t nonce = SWAP64LE(index);
            uint8_t *nonce8 = reinterpret_cast<uint8_t *>(&nonce);
            chacha (10, buf.get (), messageLength, reinterpret_cast<uint8_t *>(&h), nonce8, buf.get ());
        }

        data.assign (buf.get (), messageLength);

        return true;
    }

    bool TransactionExtraMessage::decrypt(std::size_t index,
                                          const Crypto::PublicKey &txKey,
                                          Crypto::SecretKey *recipientSecretKey,
                                          std::string &message) const
    {
        size_t messageLength = data.size ();
        if (messageLength < TX_EXTRA_MESSAGE_CHECKSUM_SIZE) {
            return false;
        }
        const char *buf;
        std::unique_ptr<char[]> ptr;
        if (recipientSecretKey != nullptr) {
            ptr.reset (new char[messageLength]);
            assert(ptr);
            MessageKeyData keyData;
            if (!generateKeyDerivation (txKey, *recipientSecretKey, keyData.derivation)) {
                return false;
            }
            keyData.magic1 = 0x80;
            keyData.magic2 = 0;
            Hash h = CnFastHash (&keyData, sizeof (MessageKeyData));
            uint64_t nonce = SWAP64LE(index);
            uint8_t *nonce8 = reinterpret_cast<uint8_t *>(&nonce);
            chacha (10, data.data (), messageLength, reinterpret_cast<uint8_t *>(&h), nonce8, ptr.get ());
            buf = ptr.get ();
        } else {
            buf = data.data ();
        }
        messageLength -= TX_EXTRA_MESSAGE_CHECKSUM_SIZE;
        for (size_t i = 0; i < TX_EXTRA_MESSAGE_CHECKSUM_SIZE; i++) {
            if (buf[messageLength + 1] != 0) {
                return false;
            }
        }

        message.assign (buf, messageLength);

        return true;
    }

    bool TransactionExtraMessage::serialize(ISerializer &serializer)
    {
        serializer (data, "data");

        return true;
    }

    bool TransactionExtraSender::encrypt(std::size_t index,
                                         const std::string &sender,
                                         AccountPublicAddress *recipient,
                                         const KeyPair &txKey)
    {
        size_t senderLength = sender.size ();
        std::unique_ptr<char[]> buf (new char[senderLength + TX_EXTRA_SENDER_CHECKSUM_SIZE]);
        memcpy (buf.get (), sender.data (), senderLength);
        memset (buf.get () + senderLength, 0, TX_EXTRA_SENDER_CHECKSUM_SIZE);

        senderLength += TX_EXTRA_SENDER_CHECKSUM_SIZE;
        if (recipient) {
            SenderKeyData keyData;
            if (!generateKeyDerivation (recipient->spendPublicKey, txKey.secretKey, keyData.derivation)) {
                return false;
            }

            keyData.magic1 = 0x80;
            keyData.magic2 = 0;

            Hash h = CnFastHash (&keyData, sizeof (SenderKeyData));
            uint64_t nonce = SWAP64LE(index);
            uint8_t *nonce8 = reinterpret_cast<uint8_t *>(&nonce);
            chacha (10, buf.get (), senderLength, reinterpret_cast<uint8_t *>(&h), nonce8, buf.get ());
        }

        data.assign (buf.get (), senderLength);

        return true;
    }

    bool TransactionExtraSender::decrypt(std::size_t index,
                                         const Crypto::PublicKey &txKey,
                                         Crypto::SecretKey *recipientSecretKey,
                                         std::string &sender) const
    {
        size_t senderLength = data.size ();
        if (senderLength < TX_EXTRA_SENDER_CHECKSUM_SIZE) {
            return false;
        }
        const char *buf;
        std::unique_ptr<char[]> ptr;
        if (recipientSecretKey != nullptr) {
            ptr.reset (new char[senderLength]);
            assert(ptr);
            SenderKeyData keyData;
            if (!generateKeyDerivation (txKey, *recipientSecretKey, keyData.derivation)) {
                return false;
            }
            keyData.magic1 = 0x80;
            keyData.magic2 = 0;
            Hash h = CnFastHash (&keyData, sizeof (SenderKeyData));
            uint64_t nonce = SWAP64LE(index);
            uint8_t *nonce8 = reinterpret_cast<uint8_t *>(&nonce);
            chacha (10, data.data (), senderLength, reinterpret_cast<uint8_t *>(&h), nonce8, ptr.get ());
            buf = ptr.get ();
        } else {
            buf = data.data ();
        }
        senderLength -= TX_EXTRA_SENDER_CHECKSUM_SIZE;
        for (size_t i = 0; i < TX_EXTRA_SENDER_CHECKSUM_SIZE; i++) {
            if (buf[senderLength + 1] != 0) {
                return false;
            }
        }

        sender.assign (buf, senderLength);

        return true;
    }

    bool TransactionExtraSender::serialize(ISerializer &serializer)
    {
        serializer (data, "data");

        return true;
    }

} // namespace CryptoNote

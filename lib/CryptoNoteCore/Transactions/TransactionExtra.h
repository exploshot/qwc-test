// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <algorithm>
#include <vector>
#include <boost/variant.hpp>

#include <CryptoNote.h>

#define TX_EXTRA_PADDING_MAX_COUNT          255
#define TX_EXTRA_NONCE_MAX_COUNT            255

#define TX_EXTRA_TAG_PADDING                0x00
#define TX_EXTRA_TAG_PUBKEY                 0x01
#define TX_EXTRA_NONCE                      0x02
#define TX_EXTRA_MERGE_MINING_TAG           0x03 // I want to deactivate that feature
#define TX_EXTRA_MESSAGE_TAG                0x04
#define TX_EXTRA_TTL                        0x05
#define TX_EXTRA_SENDER_TAG                 0x06
#define TX_EXTRA_C2F_TAG                    0x07 // Coin 2 Fiat Feature

#define TX_EXTRA_NONCE_PAYMENT_ID           0x00

#define TX_EXTRA_MESSAGE_CHECKSUM_SIZE      4
#define TX_EXTRA_SENDER_CHECKSUM_SIZE       6

namespace CryptoNote {

    class ISerializer;

    struct TransactionExtraPadding
    {
        size_t size;
    };

    struct TransactionExtraPublicKey
    {
        Crypto::PublicKey publicKey;
    };

    struct TransactionExtraNonce
    {
        BinaryArray nonce;
    };

    struct TransactionExtraMergeMiningTag
    {
        size_t depth;
        Crypto::Hash merkleRoot;
    };

    struct TransactionExtraMessage
    {
        bool encrypt(
            std::size_t index,
            const std::string &message,
            AccountPublicAddress *recipient,
            const KeyPair &txKey
        );

        bool decrypt(
            std::size_t index,
            const Crypto::PublicKey &txKey,
            Crypto::SecretKey *recipientSecretKey,
            std::string &message
        ) const;

        bool serialize(ISerializer &serializer);

        std::string data;
    };

    struct TransactionExtraSender
    {
        bool encrypt(
            std::size_t index,
            const std::string &sender,
            AccountPublicAddress *recipient,
            const KeyPair &txKey
        );

        bool decrypt(
            std::size_t index,
            const Crypto::PublicKey &txKey,
            Crypto::SecretKey *recipientSecretKey,
            std::string &sender
        ) const;

        bool serialize(ISerializer &serializer);

        std::string data;
    };

    struct TransactionExtraTTL
    {
        uint64_t ttl;
    };

    /*!
        TransactionExtraField format, except TransactionExtraPadding and TransactionExtraPublicKey:
        varint tag;
        varint size;
        varint data[];
    */
    typedef boost::variant<
        TransactionExtraPadding,
        TransactionExtraPublicKey,
        TransactionExtraNonce,
        TransactionExtraMergeMiningTag,
        TransactionExtraMessage,
        TransactionExtraTTL,
        TransactionExtraSender> TransactionExtraField;

    template<typename T>
    bool findTransactionExtraFieldByType(const std::vector<TransactionExtraField> &txExtraFields, T &field)
    {
        auto it = std::find_if (txExtraFields.begin (), txExtraFields.end (),
                                [](const TransactionExtraField &f)
                                {
                                    return typeid (T) == f.type ();
                                });

        if (txExtraFields.end () == it) {
            return false;
        }

        field = boost::get<T> (*it);

        return true;
    }

    bool parseTransactionExtra(const BinaryArray &txExtra, std::vector<TransactionExtraField> &txExtraFields);
    bool writeTransactionExtra(BinaryArray &txExtra, const std::vector<TransactionExtraField> &txExtraFields);

    Crypto::PublicKey getTransactionPublicKeyFromExtra(const BinaryArray &txExtra);
    bool addTransactionPublicKeyToExtra(BinaryArray &txExtra, const Crypto::PublicKey &txPubKey);
    bool addExtraNonceToTransactionExtra(BinaryArray &txExtra, const BinaryArray &extra_nonce);
    void setPaymentIdToTransactionExtraNonce(BinaryArray &extra_nonce, const Crypto::Hash &payment_id);
    bool getPaymentIdFromTransactionExtraNonce(const BinaryArray &extra_nonce, Crypto::Hash &payment_id);
    bool appendMergeMiningTagToExtra(BinaryArray &txExtra, const TransactionExtraMergeMiningTag &mm_tag);
    bool getMergeMiningTagFromExtra(const BinaryArray &txExtra, TransactionExtraMergeMiningTag &mm_tag);

    bool appendMessageToExtra(BinaryArray &txExtra, const TransactionExtraMessage &message);
    bool appendSenderToExtra(BinaryArray &txExtra, const TransactionExtraSender &sender);
    void appendTTLToExtra(BinaryArray &txExtra, uint64_t ttl);
    std::vector<std::string> getMessagesFromExtra(const BinaryArray &extra,
                                                  const Crypto::PublicKey &txKey,
                                                  Crypto::SecretKey *recipientSecretKey);
    std::vector<std::string> getSendersFromExtra(const BinaryArray &extra,
                                                 const Crypto::PublicKey &txKey,
                                                 Crypto::SecretKey *recipientSecretKey);

    bool createTxExtraWithPaymentId(const std::string &paymentIdString, BinaryArray &extra);
    //returns false if payment id is not found or parse error
    bool getPaymentIdFromTxExtra(const BinaryArray &extra, Crypto::Hash &paymentId);
    bool parsePaymentId(const std::string &paymentIdString, Crypto::Hash &paymentId);

} // namespace CryptoNote

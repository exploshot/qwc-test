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
#define TX_EXTRA_MERGE_MINING_TAG           0x03
#define TX_EXTRA_MESSAGE_TAG                0x04
#define TX_EXTRA_TTL                        0x05
#define TX_EXTRA_SENDER_TAG                 0x06

#define TX_EXTRA_NONCE_PAYMENT_ID           0x00

namespace CryptoNote {

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
        std::vector<uint8_t> nonce;
    };

    struct TransactionExtraMergeMiningTag 
    {
        size_t depth;
        Crypto::Hash merkleRoot;
    };

    /*!
        txExtraField format, except txExtraPadding and txExtraPubKey:
        varint tag;
        varint size;
        varint data[];
    */
    typedef boost::variant<
        TransactionExtraPadding, 
        TransactionExtraPublicKey, 
        TransactionExtraNonce, 
        TransactionExtraMergeMiningTag> TransactionExtraField;



    template<typename T>
    bool findTransactionExtraFieldByType(const std::vector<TransactionExtraField> &txExtraFields, T &field) 
    {
        auto it = std::find_if(txExtraFields.begin(), txExtraFields.end(),
                  [](const TransactionExtraField &f) { 
                      return typeid(T) == f.type(); 
                  });

        if (txExtraFields.end() == it) {
            return false;
        }
          
        field = boost::get<T>(*it);
        
        return true;
    }

    bool parseTransactionExtra(const std::vector<uint8_t> &txExtra, std::vector<TransactionExtraField> &txExtraFields);
    bool writeTransactionExtra(std::vector<uint8_t> &txExtra, const std::vector<TransactionExtraField> &txExtraFields);

    Crypto::PublicKey getTransactionPublicKeyFromExtra(const std::vector<uint8_t> &txExtra);
    bool addTransactionPublicKeyToExtra(std::vector<uint8_t> &txExtra, const Crypto::PublicKey &tx_pub_key);
    bool addExtraNonceToTransactionExtra(std::vector<uint8_t> &txExtra, const BinaryArray &extra_nonce);
    void setPaymentIdToTransactionExtraNonce(BinaryArray &extra_nonce, const Crypto::Hash &payment_id);
    bool getPaymentIdFromTransactionExtraNonce(const BinaryArray &extra_nonce, Crypto::Hash &payment_id);
    bool appendMergeMiningTagToExtra(std::vector<uint8_t> &txExtra, const TransactionExtraMergeMiningTag &mm_tag);
    bool getMergeMiningTagFromExtra(const std::vector<uint8_t> &txExtra, TransactionExtraMergeMiningTag &mm_tag);

    bool createTxExtraWithPaymentId(const std::string &paymentIdString, std::vector<uint8_t> &extra);
    //returns false if payment id is not found or parse error
    bool getPaymentIdFromTxExtra(const std::vector<uint8_t> &extra, Crypto::Hash &paymentId);
    bool parsePaymentId(const std::string &paymentIdString, Crypto::Hash &paymentId);

} // namespace CryptoNote

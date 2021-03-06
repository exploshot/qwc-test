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

#include <CryptoNote.h>

#include <Crypto/Chacha8.h>
#include <Crypto/Crypto.h>

#include <Serialization/ISerializer.h>

namespace Crypto {

    bool serialize(PublicKey &pubKey, Common::StringView name, CryptoNote::ISerializer &serializer);
    bool serialize(SecretKey &secKey, Common::StringView name, CryptoNote::ISerializer &serializer);
    bool serialize(Hash &h, Common::StringView name, CryptoNote::ISerializer &serializer);
    bool serialize(chacha8IV &chacha, Common::StringView name, CryptoNote::ISerializer &serializer);
    bool serialize(KeyImage &keyImage, Common::StringView name, CryptoNote::ISerializer &serializer);
    bool serialize(Signature &sig, Common::StringView name, CryptoNote::ISerializer &serializer);
    bool serialize(EllipticCurveScalar &ecScalar, Common::StringView name, CryptoNote::ISerializer &serializer);
    bool serialize(EllipticCurvePoint &ecPoint, Common::StringView name, CryptoNote::ISerializer &serializer);

} // namespace Crypto

namespace CryptoNote {

    struct ParentBlockSerializer
    {
        ParentBlockSerializer(
            ParentBlock &parentBlock,
            uint64_t &timestamp,
            uint32_t &nonce,
            bool hashingSerialization,
            bool headerOnly)
            : m_parentBlock (parentBlock),
              m_timestamp (timestamp),
              m_nonce (nonce),
              m_hashingSerialization (hashingSerialization),
              m_headerOnly (headerOnly)
        {
        }

        ParentBlock &m_parentBlock;
        uint64_t &m_timestamp;
        uint32_t &m_nonce;
        bool m_hashingSerialization;
        bool m_headerOnly;
    };

    inline ParentBlockSerializer makeParentBlockSerializer(
        const Block &b,
        bool hashingSerialization,
        bool headerOnly)
    {
        auto &blockRef = const_cast<Block &>(b);
        return ParentBlockSerializer{
            blockRef.parentBlock,
            blockRef.timestamp,
            blockRef.nonce,
            hashingSerialization,
            headerOnly
        };
    }

    struct AccountKeys;
    struct TransactionExtraMergeMiningTag;

    enum class SerializationTag: uint8_t
    {
        Base = 0xff,
        Key = 0x2,
        Multisignature = 0x3,
        Transaction = 0xcc,
        Block = 0xbb
    };

    void serialize(TransactionPrefix &txP, ISerializer &serializer);
    void serialize(Transaction &tx, ISerializer &serializer);
    void serialize(TransactionInput &in, ISerializer &serializer);
    void serialize(TransactionOutput &in, ISerializer &serializer);

    void serialize(BaseInput &gen, ISerializer &serializer);
    void serialize(KeyInput &key, ISerializer &serializer);
    void serialize(MultisignatureInput &multisignature, ISerializer &serializer);

    void serialize(TransactionOutput &output, ISerializer &serializer);
    void serialize(TransactionOutputTarget &output, ISerializer &serializer);
    void serialize(KeyOutput &key, ISerializer &serializer);
    void serialize(MultisignatureOutput &multisignature, ISerializer &serializer);

    void serialize(BlockHeader &header, ISerializer &serializer);
    void serialize(Block &block, ISerializer &serializer);
    void serialize(ParentBlockSerializer &pbs, ISerializer &serializer);
    void serialize(TransactionExtraMergeMiningTag &tag, ISerializer &serializer);

    void serialize(AccountPublicAddress &address, ISerializer &serializer);
    void serialize(AccountKeys &keys, ISerializer &s);

    void serialize(KeyPair &keyPair, ISerializer &serializer);
    void serialize(RawBlock &rawBlock, ISerializer &serializer);

} // namespace CryptoNote

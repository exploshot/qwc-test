// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <list>

#include <CryptoNoteCore/CryptoNoteBasic.h>

// ISerializer-based serialization
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/CryptoNoteSerialization.h>

namespace CryptoNote
{

#define BC_COMMANDS_POOL_BASE 2000

    /*!
        just to keep backward compatibility with BlockCompleteEntry serialization
    */
    struct RawBlockLegacy 
    {
        BinaryArray blockTemplate;
        std::vector<BinaryArray> transactions;
    };

    struct NOTIFY_NEW_BLOCK_request
    {
        RawBlockLegacy block;
        uint32_t currentBlockchainHeight;
        uint32_t hop;
    };

    struct NOTIFY_NEW_BLOCK
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 1;
        typedef NOTIFY_NEW_BLOCK_request request;
    };

    struct NOTIFY_NEW_TRANSACTIONS_request
    {
        std::vector<BinaryArray> txs;
    };

    struct NOTIFY_NEW_TRANSACTIONS
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 2;
        typedef NOTIFY_NEW_TRANSACTIONS_request request;
    };

    struct NOTIFY_REQUEST_GET_OBJECTS_request
    {
        std::vector<Crypto::Hash> txs;
        std::vector<Crypto::Hash> blocks;

        void serialize(ISerializer &s) 
        {
            serializeAsBinary(txs, "txs", s);
            serializeAsBinary(blocks, "blocks", s);
        }
    };

    struct NOTIFY_REQUEST_GET_OBJECTS
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 3;
        typedef NOTIFY_REQUEST_GET_OBJECTS_request request;
    };

    struct NOTIFY_RESPONSE_GET_OBJECTS_request
    {
        std::vector<std::string> txs;
        std::vector<RawBlockLegacy> blocks;
        std::vector<Crypto::Hash> missed_ids;
        uint32_t currentBlockchainHeight;
    };

    struct NOTIFY_RESPONSE_GET_OBJECTS
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 4;
        typedef NOTIFY_RESPONSE_GET_OBJECTS_request request;
    };

    struct NOTIFY_REQUEST_CHAIN
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 6;

        struct request
        {
            /*!
                IDs of the first 10 blocks are sequential, next goes with pow(2,n) offset, 
                like 2, 4, 8, 16, 32, 64 and so on, and the last one is always genesis block
            */
            std::vector<Crypto::Hash> blockIds;

            void serialize(ISerializer &s) 
            {
                serializeAsBinary(blockIds, "blockIds", s);
            }
        };
    };

    struct NOTIFY_RESPONSE_CHAIN_ENTRY_request
    {
        uint32_t startHeight;
        uint32_t totalHeight;
        std::vector<Crypto::Hash> m_blockIds;

        void serialize(ISerializer &s) 
        {
            KV_MEMBER(startHeight)
            KV_MEMBER(totalHeight)
            serializeAsBinary(m_blockIds, "m_blockIds", s);
        }
    };

    struct NOTIFY_RESPONSE_CHAIN_ENTRY
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 7;
        typedef NOTIFY_RESPONSE_CHAIN_ENTRY_request request;
    };

    struct NOTIFY_REQUEST_TX_POOL_request 
    {
        std::vector<Crypto::Hash> txs;

        void serialize(ISerializer &s) 
        {
            serializeAsBinary(txs, "txs", s);
        }
    };

    struct NOTIFY_REQUEST_TX_POOL 
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 8;
        typedef NOTIFY_REQUEST_TX_POOL_request request;
    };

    struct NOTIFY_NEW_LITE_BLOCK_request 
    {
        BinaryArray blockTemplate;
        uint32_t currentBlockchainHeight;
        uint32_t hop;
    };

    struct NOTIFY_NEW_LITE_BLOCK 
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 9;
        typedef NOTIFY_NEW_LITE_BLOCK_request request;
    };

    struct NOTIFY_MISSING_TXS_request 
    {
        Crypto::Hash blockHash;
        uint32_t currentBlockchainHeight;
        std::vector<Crypto::Hash> missingTxs;
    };

    struct NOTIFY_MISSING_TXS 
    {
        const static int ID = BC_COMMANDS_POOL_BASE + 10;
        typedef NOTIFY_MISSING_TXS_request request;
    };
} // namespace CryptoNote

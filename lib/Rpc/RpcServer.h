// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
// Copyright (c) 2018, The Karai Developers
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

#include <functional>
#include <unordered_map>

#include <Common/Math.h>

#include <Logging/LoggerRef.h>

#include <Rpc/CoreRpcServerCommandsDefinitions.h>
#include <Rpc/JsonRpc.h>
#include <Rpc/HttpServer.h>

namespace CryptoNote {

    class Core;
    class NodeServer;
    struct ICryptoNoteProtocolHandler;

    class RpcServer: public HttpServer
    {
    public:
        RpcServer(System::Dispatcher &dispatcher,
                  std::shared_ptr <Logging::ILogger> log,
                  Core &c,
                  NodeServer &p2p,
                  ICryptoNoteProtocolHandler &protocol);

        typedef std::function<bool(RpcServer *, const HttpRequest &request, HttpResponse &response)> HandlerFunction;
        bool enableCors(const std::vector <std::string> domains);
        bool setFeeAddress(const std::string fee_address);
        bool setFeeAmount(const uint32_t fee_amount);
        std::vector <std::string> getCorsDomains();

        bool onGetBlockHeadersRange(const COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::request &req,
                                        COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::response &res,
                                        JsonRpc::JsonRpcError &error_resp);
        bool onGetInfo(const COMMAND_RPC_GET_INFO::request &req, COMMAND_RPC_GET_INFO::response &res);

    private:

        template<class Handler>
        struct RpcHandler
        {
            const Handler handler;
            const bool allowBusyCore;
        };

        typedef void (RpcServer::*HandlerPtr)(const HttpRequest &request, HttpResponse &response);
        static std::unordered_map <std::string, RpcHandler<HandlerFunction>> s_handlers;

        virtual void processRequest(const HttpRequest &request, HttpResponse &response) override;
        bool processJsonRpcRequest(const HttpRequest &request, HttpResponse &response);
        bool isCoreReady();

        /*!
         * json handlers
         */
        bool onGetBlocks(const COMMAND_RPC_GET_BLOCKS_FAST::request &req, COMMAND_RPC_GET_BLOCKS_FAST::response &res);
        bool onQueryBlocks(const COMMAND_RPC_QUERY_BLOCKS::request &req, COMMAND_RPC_QUERY_BLOCKS::response &res);
        bool onQueryBlocksLite(const COMMAND_RPC_QUERY_BLOCKS_LITE::request &req,
                               COMMAND_RPC_QUERY_BLOCKS_LITE::response &res);
        bool onQueryBlocksDetailed(const COMMAND_RPC_QUERY_BLOCKS_DETAILED::request &req,
                                   COMMAND_RPC_QUERY_BLOCKS_DETAILED::response &res);
        bool onGetWalletSyncData(const COMMAND_RPC_GET_WALLET_SYNC_DATA::request &req,
                                 COMMAND_RPC_GET_WALLET_SYNC_DATA::response &res);
        bool onGetIndexes(const COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::request &req,
                          COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::response &res);

        bool onGetTransactionsStatus(const COMMAND_RPC_GET_TRANSACTIONS_STATUS::request &req,
                                     COMMAND_RPC_GET_TRANSACTIONS_STATUS::response &res);

        bool onGetGlobalIndexesForRange(const COMMAND_RPC_GET_GLOBAL_INDEXES_FOR_RANGE::request &req,
                                        COMMAND_RPC_GET_GLOBAL_INDEXES_FOR_RANGE::response &res);

        bool onGetRandomOuts(const COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::request &req,
                                COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::response &res);
        bool onGetPoolChanges(const COMMAND_RPC_GET_POOL_CHANGES::request &req,
                              COMMAND_RPC_GET_POOL_CHANGES::response &rsp);
        bool onGetPoolChangesLite(const COMMAND_RPC_GET_POOL_CHANGES_LITE::request &req,
                                  COMMAND_RPC_GET_POOL_CHANGES_LITE::response &rsp);
        bool onGetBlocksDetailsByHeights(const COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS::request &req,
                                         COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS::response &rsp);
        bool onGetBlocksDetailsByHashes(const COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::request &req,
                                        COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::response &rsp);
        bool onGetBlockDetailsByHeight(const COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::request &req,
                                       COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::response &rsp);
        bool onGetBlocksHashesByTimestamps(const COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::request &req,
                                           COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::response &rsp);
        bool onGetTransactionDetailsByHashes(const COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::request &req,
                                             COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::response &rsp);
        bool onGetTransactionHashesByPaymentId(const COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::request &req,
                                               COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::response &rsp);
        bool onGetHeight(const COMMAND_RPC_GET_HEIGHT::request &req, COMMAND_RPC_GET_HEIGHT::response &res);
        bool onGetTransactions(const COMMAND_RPC_GET_TRANSACTIONS::request &req,
                               COMMAND_RPC_GET_TRANSACTIONS::response &res);
        bool onSendRawTx(const COMMAND_RPC_SEND_RAW_TX::request &req, COMMAND_RPC_SEND_RAW_TX::response &res);
        bool onGetFeeInfo(const COMMAND_RPC_GET_FEE_ADDRESS::request &req, COMMAND_RPC_GET_FEE_ADDRESS::response &res);
        bool onGetPeers(const COMMAND_RPC_GET_PEERS::request &req, COMMAND_RPC_GET_PEERS::response &res);

        // json rpc
        bool onGetBlockCount(const COMMAND_RPC_GETBLOCKCOUNT::request &req, COMMAND_RPC_GETBLOCKCOUNT::response &res);
        bool onGetBlockHash(const COMMAND_RPC_GETBLOCKHASH::request &req, COMMAND_RPC_GETBLOCKHASH::response &res);
        bool onGetBlockTemplate(const COMMAND_RPC_GETBLOCKTEMPLATE::request &req,
                                COMMAND_RPC_GETBLOCKTEMPLATE::response &res);
        bool onGetCurrencyId(const COMMAND_RPC_GET_CURRENCY_ID::request &req,
                             COMMAND_RPC_GET_CURRENCY_ID::response &res);
        bool onSubmitBlock(const COMMAND_RPC_SUBMITBLOCK::request &req, COMMAND_RPC_SUBMITBLOCK::response &res);
        bool onGetLastBlockHeader(const COMMAND_RPC_GET_LAST_BLOCK_HEADER::request &req,
                                  COMMAND_RPC_GET_LAST_BLOCK_HEADER::response &res);
        bool onGetBlockHeaderByHash(const COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH::request &req,
                                    COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH::response &res);
        bool onGetBlockHeaderByHeight(const COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT::request &req,
                                      COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT::response &res);

        void fillBlockHeaderResponse(const BlockTemplate &blk,
                                     bool orphan_status,
                                     uint32_t index,
                                     const Crypto::Hash &hash,
                                     BlockHeaderResponse &responce);
        RawBlockLegacy prepareRawBlockLegacy(BinaryArray &&blockBlob);

        bool fOnBlocksListJson(const F_COMMAND_RPC_GET_BLOCKS_LIST::request &req,
                               F_COMMAND_RPC_GET_BLOCKS_LIST::response &res);
        bool fOnBlockJson(const F_COMMAND_RPC_GET_BLOCK_DETAILS::request &req,
                          F_COMMAND_RPC_GET_BLOCK_DETAILS::response &res);
        bool fOnTransactionJson(const F_COMMAND_RPC_GET_TRANSACTION_DETAILS::request &req,
                                F_COMMAND_RPC_GET_TRANSACTION_DETAILS::response &res);
        bool fOnTransactionsPoolJson(const F_COMMAND_RPC_GET_POOL::request &req,
                                     F_COMMAND_RPC_GET_POOL::response &res);
        bool fGetMixin(const Transaction &transaction, uint64_t &mixin);

        Logging::LoggerRef logger;
        Core &m_core;
        NodeServer &m_p2p;
        ICryptoNoteProtocolHandler &m_protocol;
        std::vector <std::string> m_cors_domains;
        std::string m_fee_address;
        uint32_t m_fee_amount;
    };

} // namespace CryptoNote

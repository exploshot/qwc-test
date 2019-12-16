// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <atomic>

#include <Common/ObserverManager.h>

#include <CryptoNoteCore/ICore.h>

#include <CryptoNoteProtocol/CryptoNoteProtocolDefinitions.h>
#include <CryptoNoteProtocol/CryptoNoteProtocolHandlerCommon.h>
#include <CryptoNoteProtocol/ICryptoNoteProtocolObserver.h>
#include <CryptoNoteProtocol/ICryptoNoteProtocolQuery.h>

#include <P2p/P2pProtocolDefinitions.h>
#include <P2p/NetNodeCommon.h>
#include <P2p/ConnectionContext.h>

#include <Logging/LoggerRef.h>

namespace System {
    class Dispatcher;
} // namespace System

namespace CryptoNote {
    class Currency;

    class CryptoNoteProtocolHandler : public ICryptoNoteProtocolHandler
    {
    public:

        CryptoNoteProtocolHandler(const Currency &currency, 
                                  System::Dispatcher &dispatcher,
                                  ICore &rcore, 
                                  IP2pEndpoint *p_net_layout, 
                                  std::shared_ptr<Logging::ILogger> log);

        virtual bool addObserver(ICryptoNoteProtocolObserver *observer) override;
        virtual bool removeObserver(ICryptoNoteProtocolObserver *observer) override;

        void setP2pEndpoint(IP2pEndpoint *p2p);
        virtual bool isSynchronized() const override { return m_synchronized; }
        void logConnections();

        /*!
            Interface t_payload_net_handler, where t_payload_net_handler 
            is template argument of nodetool::node_server
        */
        void stop();
        bool startSync(CryptoNoteConnectionContext &context);
        void onConnectionOpened(CryptoNoteConnectionContext &context);
        void onConnectionClosed(CryptoNoteConnectionContext &context);
        CoreStatistics getStatistics();
        bool getPayloadSyncData(CORE_SYNC_DATA &hshd);
        bool processPayloadSyncData(const CORE_SYNC_DATA &hshd, 
                                    CryptoNoteConnectionContext &context, 
                                    bool is_inital);
        int handleCommand(bool is_notify, int command, 
                          const BinaryArray &in_buff, 
                          BinaryArray &buff_out, 
                          CryptoNoteConnectionContext &context, 
                          bool &handled);
        virtual size_t getPeerCount() const override;
        virtual uint32_t getObservedHeight() const override;
        virtual uint32_t getBlockchainHeight() const override;
        void requestMissingPoolTransactions(const CryptoNoteConnectionContext &context);

    private:
        /*!
            ----------------- commands handlers ----------------------------------------------
        */
        int handleNotifyNewBlock(int command, 
                                 NOTIFY_NEW_BLOCK::request &arg, 
                                 CryptoNoteConnectionContext &context);
        int handleNotifyNewTransactions(int command, 
                                        NOTIFY_NEW_TRANSACTIONS::request &arg, 
                                        CryptoNoteConnectionContext &context);
        int handleRequestGetObjects(int command, 
                                    NOTIFY_REQUEST_GET_OBJECTS::request &arg, 
                                    CryptoNoteConnectionContext &context);
        int handleResponseGetObjects(int command, 
                                     NOTIFY_RESPONSE_GET_OBJECTS::request &arg, 
                                     CryptoNoteConnectionContext &context);
        int handleRequestChain(int command, 
                               NOTIFY_REQUEST_CHAIN::request &arg, 
                               CryptoNoteConnectionContext &context);
        int handleResponseChainEntry(int command, 
                                     NOTIFY_RESPONSE_CHAIN_ENTRY::request &arg, 
                                     CryptoNoteConnectionContext &context);
        int handleRequestTxPool(int command, 
                                NOTIFY_REQUEST_TX_POOL::request &arg, 
                                CryptoNoteConnectionContext &context);
        int handleNotifyNewLiteBlock(int command, 
                                     NOTIFY_NEW_LITE_BLOCK::request &arg, 
                                     CryptoNoteConnectionContext &context);
        int handleNotifyMissingTxs(int command, 
                                   NOTIFY_MISSING_TXS::request &arg, 
                                   CryptoNoteConnectionContext &context);

        /*!
            ----------------- i_cryptonote_protocol ----------------------------------
        */
        virtual void relayBlock(NOTIFY_NEW_BLOCK::request &arg) override;
        virtual void relayTransactions(const std::vector<BinaryArray> &transactions) override;

        /*!
            ----------------------------------------------------------------------------------
        */
        uint32_t getCurrentBlockchainHeight();
        bool requestMissingObjects(CryptoNoteConnectionContext &context, 
                                   bool check_having_blocks);
        bool onConnectionSynchronized();
        void updateObservedHeight(uint32_t peerHeight, 
                                  const CryptoNoteConnectionContext &context);
        void recalculateMaxObservedHeight(const CryptoNoteConnectionContext &context);
        int processObjects(CryptoNoteConnectionContext &context, 
                           std::vector<RawBlock> &&rawBlocks, 
                           const std::vector<CachedBlock> &cachedBlocks);
        Logging::LoggerRef logger;

    public:
        /*!
            ----------------- banning ----------------------------------
        */
        void ban(uint32_t ip);
        void unban(uint32_t ip);
        void unbanAll();
        bool isBanned(CryptoNoteConnectionContext &context) const;

    private:
        mutable std::mutex m_bannedMutex;
        std::set<uint32_t> m_bannedIps;
    
        
    public:
        /*!
            ----------------- tx threshold ----------------------------------
        */
        uint64_t txThresholdInterval() const;
        void setTxThresholdInterval(uint64_t interval);

        size_t txThreshold() const;
        void setTxThreshold(size_t count);
    
    private:
        std::atomic_uint64_t m_transactionsPushedInterval
        { 
            4 * 60
        };

        std::atomic_size_t m_transactionsPushedMaxInInterval{ 15 };

    private:
        int doPushLiteBlock(NOTIFY_NEW_LITE_BLOCK::request block, 
                            CryptoNoteConnectionContext &context, 
                            std::vector<BinaryArray> missingTxs);

    private:
        System::Dispatcher &m_dispatcher;
        ICore &m_core;
        const Currency &m_currency;

        p2p_endpoint_stub m_p2p_stub;
        IP2pEndpoint *m_p2p;
        std::atomic<bool> m_synchronized;
        std::atomic<bool> m_stop;

        mutable std::mutex m_observedHeightMutex;
        uint32_t m_observedHeight;
        
        mutable std::mutex m_blockchainHeightMutex;
        uint32_t m_blockchainHeight;

        std::atomic<size_t> m_peersCount;
        Tools::ObserverManager<ICryptoNoteProtocolObserver> m_observerManager;
    };
}

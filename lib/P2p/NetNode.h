// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2019, The CyprusCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
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

#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>

#include <CryptoNoteProtocol/CryptoNoteProtocolHandler.h>

#include <Logging/LoggerRef.h>

#include <P2p/ConnectionContext.h>
#include <P2p/LevinProtocol.h>
#include <P2p/NetNodeCommon.h>
#include <P2p/NetNodeConfig.h>
#include <P2p/OnceInInterval.h>
#include <P2p/P2pNetworks.h>
#include <P2p/P2pProtocolDefinitions.h>
#include <P2p/PeerListManager.h>

#include <System/Context.h>
#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/Timer.h>
#include <System/TcpConnection.h>
#include <System/TcpListener.h>

#include <version.h>

namespace System {
    class TcpConnection;
} // namespace System

namespace CryptoNote {
    class LevinProtocol;
    class ISerializer;

    struct P2pMessage
    {
        enum Type
        {
            COMMAND,
            REPLY,
            NOTIFY
        };

        P2pMessage(Type type, uint32_t command, const BinaryArray &buffer, int32_t returnCode = 0)
            :
            type (type), command (command), buffer (buffer), returnCode (returnCode)
        {
        }

        P2pMessage(P2pMessage &&msg)
            :
            type (msg.type), command (msg.command), buffer (std::move (msg.buffer)), returnCode (msg.returnCode)
        {
        }

        size_t size()
        {
            return buffer.size ();
        }

        Type type;
        uint32_t command;
        const BinaryArray buffer;
        int32_t returnCode;
    };

    struct P2pConnectionContext: public CryptoNoteConnectionContext
    {
    public:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

        System::Context<void> *context;
        uint64_t peerId;
        System::TcpConnection connection;

        P2pConnectionContext(System::Dispatcher &dispatcher,
                             std::shared_ptr <Logging::ILogger> log,
                             System::TcpConnection &&conn)
            :
            context (nullptr),
            peerId (0),
            connection (std::move (conn)),
            logger (log, "node_server"),
            queueEvent (dispatcher),
            stopped (false)
        {
        }

        P2pConnectionContext(P2pConnectionContext &&ctx)
            :
            CryptoNoteConnectionContext (std::move (ctx)),
            context (ctx.context),
            peerId (ctx.peerId),
            connection (std::move (ctx.connection)),
            logger (ctx.logger.getLogger (), "node_server"),
            queueEvent (std::move (ctx.queueEvent)),
            stopped (std::move (ctx.stopped))
        {
        }

        bool pushMessage(P2pMessage &&msg);
        std::vector <P2pMessage> popBuffer();
        void interrupt();

        uint64_t writeDuration(TimePoint now) const;

    private:
        Logging::LoggerRef logger;
        TimePoint writeOperationStartTime;
        System::Event queueEvent;
        std::vector <P2pMessage> writeQueue;
        size_t writeQueueSize = 0;
        bool stopped;
    };

    class NodeServer: public IP2pEndpoint
    {
    public:
        NodeServer(System::Dispatcher &dispatcher,
                   CryptoNote::CryptoNoteProtocolHandler &payload_handler,
                   std::shared_ptr <Logging::ILogger> log);

        bool run();
        bool init(const NetNodeConfig &config);
        bool deinit();
        bool sendStopSignal();
        uint32_t get_this_peer_port()
        {
            return m_listeningPort;
        }
        CryptoNote::CryptoNoteProtocolHandler &getPayloadObject();

        void serialize(ISerializer &s);

        /*!
         * debug functions
         */
        bool logPeerlist();
        bool logConnections();
        virtual uint64_t getConnectionsCount() override;
        size_t getOutgoingConnectionsCount();

        PeerlistManager &getPeerlistManager()
        {
            return m_peerlist;
        }

    private:

        int handleCommand(const LevinProtocol::Command &cmd,
                          BinaryArray &buff_out,
                          P2pConnectionContext &context,
                          bool &handled);

        /*!
         * commands handlers
         */
        int handleHandshake(int command,
                            COMMAND_HANDSHAKE::request &arg,
                            COMMAND_HANDSHAKE::response &rsp,
                            P2pConnectionContext &context);
        int handleTimedSync(int command,
                            COMMAND_TIMED_SYNC::request &arg,
                            COMMAND_TIMED_SYNC::response &rsp,
                            P2pConnectionContext &context);
        int handlePing(int command,
                       COMMAND_PING::request &arg,
                       COMMAND_PING::response &rsp,
                       P2pConnectionContext &context);
#ifdef ALLOW_DEBUG_COMMANDS
        int handleGetStatInfo(int command,
                              COMMAND_REQUEST_STAT_INFO::request& arg,
                              COMMAND_REQUEST_STAT_INFO::response& rsp,
                              P2pConnectionContext& context);
        int handleGetNetworkState(int command,
                                  COMMAND_REQUEST_NETWORK_STATE::request& arg,
                                  COMMAND_REQUEST_NETWORK_STATE::response& rsp,
                                  P2pConnectionContext& context);
        int handleGetPeerId(int command,
                            COMMAND_REQUEST_PEER_ID::request& arg,
                            COMMAND_REQUEST_PEER_ID::response& rsp,
                            P2pConnectionContext& context);
        bool checkTrust(const ProofOfTrust& tr);
#endif

        bool initConfig();
        bool makeDefaultConfig();
        bool storeConfig();
        void initUpnp();

        bool
        handshake(CryptoNote::LevinProtocol &proto, P2pConnectionContext &context, bool just_take_peerlist = false);
        bool timedSync();
        bool handleTimedSyncResponse(const BinaryArray &in, P2pConnectionContext &context);
        void forEachConnection(std::function<void(P2pConnectionContext &)> action);

        void onConnectionNew(P2pConnectionContext &context);
        void onConnectionClose(P2pConnectionContext &context);

        /*!
         * i_p2p_endpoint
         */
        virtual void relayNotifyToAll(int command,
                                      const BinaryArray &data_buff,
                                      const boost::uuids::uuid *excludeConnection) override;
        virtual bool invokeNotifyToPeer(int command,
                                        const BinaryArray &req_buff,
                                        const CryptoNoteConnectionContext &context) override;
        virtual void forEachConnection(std::function<void(CryptoNote::CryptoNoteConnectionContext &, uint64_t)> f) override;
        virtual void externalRelayNotifyToAll(int command,
                                              const BinaryArray &data_buff,
                                              const boost::uuids::uuid *excludeConnection) override;
        virtual void externalRelayNotifyToList(int command,
                                               const BinaryArray &data_buff,
                                               const std::list <boost::uuids::uuid> relayList) override;

        bool handleConfig(const NetNodeConfig &config);
        bool appendNetAddress(std::vector <NetworkAddress> &nodes, const std::string &addr);
        bool idleWorker();
        bool handleRemotePeerlist(const std::list <PeerlistEntry> &peerlist,
                                    time_t local_time,
                                    const CryptoNoteConnectionContext &context);
        bool getLocalNodeData(BasicNodeData &node_data);

        bool mergePeerlistWithLocal(const std::list <PeerlistEntry> &bs);
        bool fixTimeDelta(std::list <PeerlistEntry> &local_peerlist, time_t local_time, int64_t &delta);

        bool connectionsMaker();
        bool makeNewConnectionFromPeerlist(bool use_white_list);
        bool tryToConnectAndHandshakeWithNewPeer(const NetworkAddress &na,
                                                 bool just_take_peerlist = false,
                                                 uint64_t last_seen_stamp = 0,
                                                 bool white = true);
        bool isPeerUsed(const PeerlistEntry &peer);
        bool isAddressConnected(const NetworkAddress &peer);
        bool tryPing(BasicNodeData &node_data, P2pConnectionContext &context);
        bool makeExpectedConnectionsCount(bool white_list, size_t expected_connections);

        bool connectToPeerlist(const std::vector <NetworkAddress> &peers);

        /*!
         * debug functions
         */
        std::string printConnectionsContainer();

        typedef std::unordered_map <boost::uuids::uuid,
                                    P2pConnectionContext,
                                    boost::hash<boost::uuids::uuid>> ConnectionContainer;
        typedef ConnectionContainer::iterator ConnectionIterator;
        ConnectionContainer m_connections;

        void acceptLoop();
        void connectionHandler(const boost::uuids::uuid &connectionId, P2pConnectionContext &connection);
        void writeHandler(P2pConnectionContext &ctx);
        void onIdle();
        void timedSyncLoop();
        void timeoutLoop();

        template<typename T>
        void safeInterrupt(T &obj);

        struct config
        {
            NetworkConfig m_net_config;
            uint64_t m_peer_id;

            void serialize(ISerializer &s)
            {
                KV_MEMBER (m_net_config)
                KV_MEMBER (m_peer_id)
            }
        };

        config m_config;
        std::string m_config_folder;

        bool m_have_address;
        bool m_first_connection_maker_call;
        uint32_t m_listeningPort;
        uint32_t m_external_port;
        uint32_t m_ip_address;
        bool m_allow_local_ip;
        bool m_hide_my_port;
        std::string m_p2p_state_filename;
        std::string m_node_version;
        bool m_p2p_state_reset;

        System::Dispatcher &m_dispatcher;
        System::ContextGroup m_workingContextGroup;
        System::Event m_stopEvent;
        System::Timer m_idleTimer;
        System::Timer m_timeoutTimer;
        System::TcpListener m_listener;
        Logging::LoggerRef logger;
        std::atomic<bool> m_stop;

        CryptoNoteProtocolHandler &m_payload_handler;
        PeerlistManager m_peerlist;

        // OnceInInterval m_peer_handshake_idle_maker_interval;
        OnceInInterval m_connections_maker_interval;
        OnceInInterval m_peerlist_store_interval;
        System::Timer m_timedSyncTimer;

        std::string m_bind_ip;
        std::string m_port;
#ifdef ALLOW_DEBUG_COMMANDS
        uint64_t m_last_stat_request_time;
#endif
        std::vector <NetworkAddress> m_priority_peers;
        std::vector <NetworkAddress> m_exclusive_peers;
        std::vector <NetworkAddress> m_seed_nodes;
        std::list <PeerlistEntry> m_command_line_peers;
        uint64_t m_peer_livetime;
        boost::uuids::uuid m_network_id;
    };
}

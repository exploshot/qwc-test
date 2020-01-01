// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
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

#include <boost/uuid/uuid.hpp>

#include <Crypto/Crypto.h>

#include <CryptoNoteCore/CoreStatistics.h>

#include <Global/CryptoNoteConfig.h>

#include <P2p/P2pProtocolTypes.h>

/*!
 * new serialization
 */
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/CryptoNoteSerialization.h>
#include <version.h>

namespace CryptoNote {
    inline bool serialize(boost::uuids::uuid &v, Common::StringView name, ISerializer &s)
    {
        return s.binary (&v, sizeof (v), name);
    }

    struct NetworkConfig
    {
        void serialize(ISerializer &s)
        {
            KV_MEMBER(connections_count)
            KV_MEMBER(handshake_interval)
            KV_MEMBER(packet_max_size)
            KV_MEMBER(config_id)
        }

        uint32_t connections_count;
        uint32_t connection_timeout;
        uint32_t ping_connection_timeout;
        uint32_t handshake_interval;
        uint32_t packet_max_size;
        uint32_t config_id;
        uint32_t send_peerlist_sz;
    };

    struct BasicNodeData
    {
        boost::uuids::uuid network_id;
        uint8_t version;
        uint64_t local_time;
        uint32_t my_port;
        uint64_t peer_id;
        std::string node_version = PROJECT_VERSION;

        void serialize(ISerializer &s)
        {
            KV_MEMBER(network_id)
            if (s.type () == ISerializer::INPUT) {
                version = 0;
            }
            KV_MEMBER(version)
            KV_MEMBER(peer_id)
            KV_MEMBER(local_time)
            KV_MEMBER(my_port)
            KV_MEMBER(node_version)
        }
    };

    struct CORE_SYNC_DATA
    {
        uint32_t current_height;
        Crypto::Hash top_id;

        void serialize(ISerializer &s)
        {
            KV_MEMBER(current_height)
            KV_MEMBER(top_id)
        }
    };

    #define P2P_COMMANDS_POOL_BASE 1000

    struct COMMAND_HANDSHAKE
    {
        enum
        {
            ID = P2P_COMMANDS_POOL_BASE + 1
        };

        struct request
        {
            BasicNodeData node_data;
            CORE_SYNC_DATA payload_data;

            void serialize(ISerializer &s)
            {
                KV_MEMBER(node_data)
                KV_MEMBER(payload_data)
            }

        };

        struct response
        {
            BasicNodeData node_data;
            CORE_SYNC_DATA payload_data;
            std::list<PeerlistEntry> local_peerlist;

            void serialize(ISerializer &s)
            {
                KV_MEMBER(node_data)
                KV_MEMBER(payload_data)
                serializeAsBinary (local_peerlist, "local_peerlist", s);
            }
        };
    };

    struct COMMAND_TIMED_SYNC
    {
        enum
        {
            ID = P2P_COMMANDS_POOL_BASE + 2
        };

        struct request
        {
            CORE_SYNC_DATA payload_data;

            void serialize(ISerializer &s)
            {
                KV_MEMBER(payload_data)
            }

        };

        struct response
        {
            uint64_t local_time;
            CORE_SYNC_DATA payload_data;
            std::list<PeerlistEntry> local_peerlist;

            void serialize(ISerializer &s)
            {
                KV_MEMBER(local_time)
                KV_MEMBER(payload_data)
                serializeAsBinary (local_peerlist, "local_peerlist", s);
            }
        };
    };

    struct COMMAND_PING
    {
        /*!
         * Used to make "callback" connection, to be sure that opponent node
         * have accessible connection point. Only other nodes can add peer to peerlist,
         * and ONLY in case when peer has accepted connection and answered to ping.
         */
        enum
        {
            ID = P2P_COMMANDS_POOL_BASE + 3
        };

        #define PING_OK_RESPONSE_STATUS_TEXT "OK"

        struct request
        {
            /*!
             * actually we don't need to send any real data
             * @param s
             */
            void serialize(ISerializer &s)
            {
            }
        };

        struct response
        {
            std::string status;
            uint64_t peer_id;

            void serialize(ISerializer &s)
            {
                KV_MEMBER(status)
                KV_MEMBER(peer_id)
            }
        };
    };
}

#ifdef ALLOW_DEBUG_COMMANDS

/*!
 * These commands are considered as insecure, and made in debug purposes for a limited lifetime.
 * Anyone who feel unsafe with this commands can disable the ALLOW_GET_STAT_COMMAND macro.
 */
struct ProofOfTrust
{
    uint64_t peer_id;
    uint64_t    time;
    Crypto::Signature sign;

    void serialize(ISerializer& s) {
        KV_MEMBER(peer_id)
            KV_MEMBER(time)
            KV_MEMBER(sign)
    }
};

inline Crypto::Hash getProofOfTrustHash(const ProofOfTrust& pot) {
    std::string s;
    s.append(reinterpret_cast<const char*>(&pot.peer_id), sizeof(pot.peer_id));
    s.append(reinterpret_cast<const char*>(&pot.time), sizeof(pot.time));
    return Crypto::CnFastHash(s.data(), s.size());
}

struct COMMAND_REQUEST_STAT_INFO
{
    enum { ID = P2P_COMMANDS_POOL_BASE + 4 };

    struct request
    {
        ProofOfTrust tr;

        void serialize(ISerializer& s) {
            KV_MEMBER(tr)
        }
    };

    struct response
    {
        std::string version;
        std::string os_version;
        uint64_t connections_count;
        uint64_t incoming_connections_count;
        CoreStatistics payload_info;

        void serialize(ISerializer& s) {
            KV_MEMBER(version)
                KV_MEMBER(os_version)
                KV_MEMBER(connections_count)
                KV_MEMBER(incoming_connections_count)
                KV_MEMBER(payload_info)
        }
    };
};

struct COMMAND_REQUEST_NETWORK_STATE
{
    enum { ID = P2P_COMMANDS_POOL_BASE + 5 };

    struct request
    {
        ProofOfTrust tr;

        void serialize(ISerializer& s) {
            KV_MEMBER(tr)
        }
    };

    struct response
    {
        std::list<PeerlistEntry> local_peerlist_white;
        std::list<PeerlistEntry> local_peerlist_gray;
        std::list<ConnectionEntry> connections_list;
        uint64_t my_id;
        uint64_t local_time;

        void serialize(ISerializer& s) {
            serializeAsBinary(local_peerlist_white, "local_peerlist_white", s);
            serializeAsBinary(local_peerlist_gray, "local_peerlist_gray", s);
            serializeAsBinary(connections_list, "connections_list", s);
            KV_MEMBER(my_id)
                KV_MEMBER(local_time)
        }
    };
};

struct COMMAND_REQUEST_PEER_ID
{
    enum { ID = P2P_COMMANDS_POOL_BASE + 6 };

    struct request
    {
        void serialize(ISerializer& s) {}
    };

    struct response
    {
        uint64_t my_id;

        void serialize(ISerializer& s) {
            KV_MEMBER(my_id)
        }
    };
};

#endif
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

#include <deque>
#include <list>
#include <optional>
#include <ostream>
#include <unordered_set>

#include <boost/uuid/uuid.hpp>

#include <Common/StringTools.h>

#include <Crypto/Hash.h>

#include <P2p/PendingLiteBlock.h>

namespace CryptoNote {

    struct CryptoNoteConnectionContext
    {
        uint8_t version;
        boost::uuids::uuid m_connection_id;
        uint32_t m_remote_ip = 0;
        uint32_t m_remote_port = 0;
        bool m_is_income = false;
        time_t m_started = 0;

        enum state
        {
            StateBeforeHandshake = 0, // default state
            StateSynchronizing,
            StateIdle,
            StateNormal,
            StateSyncRequired,
            StatePoolSyncRequired,
            StateShutdown
        };

        state m_state = StateBeforeHandshake;
        std::deque<std::pair<uint64_t, size_t>> m_pushed_transactions;
        std::optional<PendingLiteBlock> m_pending_lite_block;
        std::list<Crypto::Hash> m_needed_objects;
        std::unordered_set<Crypto::Hash> m_requested_objects;
        uint32_t m_remote_blockchain_height = 0;
        uint32_t m_last_response_height = 0;
    };

    inline std::string getProtocolStateString(CryptoNoteConnectionContext::state s)
    {
        switch (s) {
            case CryptoNoteConnectionContext::StateBeforeHandshake:
                return "StateBeforeHandshake";
            case CryptoNoteConnectionContext::StateSynchronizing:
                return "StateSynchronizing";
            case CryptoNoteConnectionContext::StateIdle:
                return "StateIdle";
            case CryptoNoteConnectionContext::StateNormal:
                return "StateNormal";
            case CryptoNoteConnectionContext::StateSyncRequired:
                return "StateSyncRequired";
            case CryptoNoteConnectionContext::StatePoolSyncRequired:
                return "StatePoolSyncRequired";
            case CryptoNoteConnectionContext::StateShutdown:
                return "StateShutdown";
            default:
                return "unknown";
        }
    }

} // namespace CryptoNote

namespace std {
    inline std::ostream &operator<<(std::ostream &s, const CryptoNote::CryptoNoteConnectionContext &context)
    {
        return s
            << "["
            << Common::ipAddressToString (context.m_remote_ip)
            << ":"
            <<
            context.m_remote_port
            << (context.m_is_income ? " INC" : " OUT")
            << "] ";
    }
} // namespace std

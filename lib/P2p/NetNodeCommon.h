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

#include <CryptoNote.h>

#include <P2p/P2pProtocolTypes.h>

namespace CryptoNote {

    struct CryptoNoteConnectionContext;

    struct IP2pEndpoint
    {
        virtual void relayNotifyToAll(int command,
                            const BinaryArray &data_buff,
                            const boost::uuids::uuid *excludeConnection) = 0;
        virtual bool invokeNotifyToPeer(int command,
                                           const BinaryArray &req_buff,
                                           const CryptoNote::CryptoNoteConnectionContext &context) = 0;
        virtual uint64_t getConnectionsCount() = 0;
        virtual void forEachConnection(std::function<void(CryptoNote::CryptoNoteConnectionContext &, uint64_t)> f) = 0;
        /*!
         * can be called from external threads
         */
        virtual void externalRelayNotifyToAll(int command,
                                              const BinaryArray &data_buff,
                                              const boost::uuids::uuid *excludeConnection) = 0;
        virtual void externalRelayNotifyToList(int command,
                                               const BinaryArray &data_buff,
                                               const std::list <boost::uuids::uuid> relayList) = 0;
    };

    struct P2pEndpointStub: public IP2pEndpoint
    {
        virtual void relayNotifyToAll(int command,
                                         const BinaryArray &data_buff,
                                         const boost::uuids::uuid *excludeConnection) override
        {
        }
        virtual bool invokeNotifyToPeer(int command,
                                           const BinaryArray &req_buff,
                                           const CryptoNote::CryptoNoteConnectionContext &context) override
        {
            return true;
        }
        virtual void forEachConnection(std::function<void(CryptoNote::CryptoNoteConnectionContext &, uint64_t)> f) override
        {
        }

        virtual uint64_t getConnectionsCount() override
        {
            return 0;
        }
        virtual void externalRelayNotifyToAll(int command,
                                              const BinaryArray &data_buff,
                                              const boost::uuids::uuid *excludeConnection) override
        {
        }

        virtual void externalRelayNotifyToList(int command,
                                               const BinaryArray &data_buff,
                                               const std::list <boost::uuids::uuid> relayList) override
        {
        }
    };
} // namespace CryptoNote

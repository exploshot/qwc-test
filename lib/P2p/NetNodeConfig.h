// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
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

#include <cstdint>
#include <vector>
#include <string>

#include <P2p/P2pProtocolTypes.h>

namespace CryptoNote {

    class NetNodeConfig
    {
    public:
        NetNodeConfig();
        bool init(
            const std::string interface,
            const int port,
            const int external,
            const bool localIp,
            const bool hidePort,
            const std::string dataDir,
            const std::vector<std::string> addPeers,
            const std::vector<std::string> addExclusiveNodes,
            const std::vector<std::string> addPriorityNodes,
            const std::vector<std::string> addSeedNodes,
            const bool p2pResetPeerState,
            const std::string addExclusiveVersion);

        bool getAllowLocalIp() const;
        bool getHideMyPort() const;
        bool getP2pStateReset() const;
        std::string getBindIp() const;
        std::string getConfigFolder() const;
        std::string getExclusiveVersion() const;
        std::string getP2pStateFilename() const;
        uint16_t getBindPort() const;
        uint16_t getExternalPort() const;
        std::vector<NetworkAddress> getExclusiveNodes() const;
        std::vector<NetworkAddress> getPriorityNodes() const;
        std::vector<NetworkAddress> getSeedNodes() const;
        std::vector<PeerlistEntry> getPeers() const;

    private:
        bool allowLocalIp;
        bool hideMyPort;
        bool p2pStateReset;
        std::string bindIp;
        std::string configFolder;
        std::string exclusiveVersion;
        std::string p2pStateFilename;
        uint16_t bindPort;
        uint16_t externalPort;
        std::vector<PeerlistEntry> peers;
        std::vector<NetworkAddress> priorityNodes;
        std::vector<NetworkAddress> exclusiveNodes;
        std::vector<NetworkAddress> seedNodes;
    };

} //namespace CryptoNote

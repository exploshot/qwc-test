// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2019, The CyprusCoin Developers
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

#include <Common/Util.h>
#include <Common/StringTools.h>

#include <Crypto/Random.h>

#include <Global/CryptoNoteConfig.h>

#include <P2p/NetNodeConfig.h>

namespace CryptoNote {
    namespace {

        bool parsePeerFromString(NetworkAddress &pe, const std::string &node_addr)
        {
            return Common::parseIpAddressAndPort (pe.ip, pe.port, node_addr);
        }

        bool parsePeersAndAddToNetworkContainer(const std::vector <std::string> peerList,
                                                std::vector <NetworkAddress> &container)
        {
            for (const std::string &peer : peerList) {
                NetworkAddress networkAddress = NetworkAddress ();
                if (!parsePeerFromString (networkAddress, peer)) {
                    return false;
                }
                container.push_back (networkAddress);
            }
            return true;
        }

        bool parsePeersAndAddToPeerListContainer(const std::vector <std::string> peerList,
                                                 std::vector <PeerlistEntry> &container)
        {
            for (const std::string &peer : peerList) {
                PeerlistEntry peerListEntry = PeerlistEntry ();
                peerListEntry.id = Random::randomValue<uint64_t> ();
                if (!parsePeerFromString (peerListEntry.adr, peer)) {
                    return false;
                }
                container.push_back (peerListEntry);
            }
            return true;
        }

    } //namespace

    NetNodeConfig::NetNodeConfig()
    {
        bindIp = "";
        bindPort = 0;
        externalPort = 0;
        allowLocalIp = false;
        hideMyPort = false;
        configFolder = Tools::getDefaultDataDirectory ();
        p2pStateReset = false;
        exclusiveVersion = "";
    }

    bool NetNodeConfig::init(const std::string interface,
                             const int port,
                             const int external,
                             const bool localIp,
                             const bool hidePort,
                             const std::string dataDir,
                             const std::vector <std::string> addPeers,
                             const std::vector <std::string> addExclusiveNodes,
                             const std::vector <std::string> addPriorityNodes,
                             const std::vector <std::string> addSeedNodes,
                             const bool p2pResetPeerState,
                             const std::string addExclusiveVersion)
    {
        bindIp = interface;
        bindPort = port;
        externalPort = external;
        allowLocalIp = localIp;
        hideMyPort = hidePort;
        configFolder = dataDir;
        p2pStateFilename = CryptoNote::parameters::P2P_NET_DATA_FILENAME;
        p2pStateReset = p2pResetPeerState;
        exclusiveVersion = addExclusiveVersion;

        if (!addPeers.empty ()) {
            if (!parsePeersAndAddToPeerListContainer (addPeers, peers)) {
                return false;
            }
        }

        if (!addExclusiveNodes.empty ()) {
            if (!parsePeersAndAddToNetworkContainer (addExclusiveNodes, exclusiveNodes)) {
                return false;
            }
        }

        if (!addPriorityNodes.empty ()) {
            if (!parsePeersAndAddToNetworkContainer (addPriorityNodes, priorityNodes)) {
                return false;
            }
        }

        if (!addSeedNodes.empty ()) {
            if (!parsePeersAndAddToNetworkContainer (addSeedNodes, seedNodes)) {
                return false;
            }
        }

        return true;
    }

    bool NetNodeConfig::getAllowLocalIp() const
    {
        return allowLocalIp;
    }

    bool NetNodeConfig::getHideMyPort() const
    {
        return hideMyPort;
    }

    bool NetNodeConfig::getP2pStateReset() const
    {
        return p2pStateReset;
    }

    std::string NetNodeConfig::getBindIp() const
    {
        return bindIp;
    }

    std::string NetNodeConfig::getConfigFolder() const
    {
        return configFolder;
    }

    std::string NetNodeConfig::getExclusiveVersion() const
    {
        return exclusiveVersion;
    }

    std::string NetNodeConfig::getP2pStateFilename() const
    {
        return p2pStateFilename;
    }

    uint16_t NetNodeConfig::getBindPort() const
    {
        return bindPort;
    }

    uint16_t NetNodeConfig::getExternalPort() const
    {
        return externalPort;
    }

    std::vector <NetworkAddress> NetNodeConfig::getExclusiveNodes() const
    {
        return exclusiveNodes;
    }

    std::vector <NetworkAddress> NetNodeConfig::getPriorityNodes() const
    {
        return priorityNodes;
    }

    std::vector <NetworkAddress> NetNodeConfig::getSeedNodes() const
    {
        return seedNodes;
    }

    std::vector <PeerlistEntry> NetNodeConfig::getPeers() const
    {
        return peers;
    }

} //namespace CryptoNote

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

#include <algorithm>
#include <time.h>

#include <P2p/PeerListManager.h>

#include <Serialization/SerializationOverloads.h>

#include <System/Ipv4Address.h>

void PeerlistManager::serialize(CryptoNote::ISerializer &s)
{
    const uint8_t currentVersion = 1;
    uint8_t version = currentVersion;

    s (version, "version");

    if (version != currentVersion) {
        return;
    }

    s (m_peers_white, "whitelist");
    s (m_peers_gray, "graylist");
}

void serialize(NetworkAddress &na, CryptoNote::ISerializer &s)
{
    s (na.ip, "ip");
    s (na.port, "port");
}

void serialize(PeerlistEntry &pe, CryptoNote::ISerializer &s)
{
    s (pe.adr, "adr");
    s (pe.id, "id");
    s (pe.last_seen, "last_seen");
}

PeerlistManager::PeerlistManager()
    :
    m_whitePeerlist (m_peers_white, CryptoNote::P2P_LOCAL_WHITE_PEERLIST_LIMIT),
    m_grayPeerlist (m_peers_gray, CryptoNote::P2P_LOCAL_GRAY_PEERLIST_LIMIT)
{
}

bool PeerlistManager::init(bool allow_local_ip)
{
    m_allow_local_ip = allow_local_ip;
    return true;
}

void PeerlistManager::trimWhitePeerlist()
{
    m_whitePeerlist.trim ();
}

void PeerlistManager::trimGrayPeerlist()
{
    m_grayPeerlist.trim ();
}

bool PeerlistManager::mergePeerlist(const std::list<PeerlistEntry> &outer_bs)
{
    for (const PeerlistEntry &be : outer_bs) {
        appendWithPeerGray (be);
    }

    /*!
     * delete extra elements
     */
    trimGrayPeerlist ();
    return true;
}

bool PeerlistManager::getWhitePeerByIndex(PeerlistEntry &p, size_t i) const
{
    return m_whitePeerlist.get (p, i);
}

bool PeerlistManager::getGrayPeerByIndex(PeerlistEntry &p, size_t i) const
{
    return m_grayPeerlist.get (p, i);
}

bool PeerlistManager::isIpAllowed(uint32_t ip) const
{
    System::Ipv4Address addr (networkToHost (ip));

    /*!
     * never allow loopback ip
     */
    if (addr.isLoopback ()) {
        return false;
    }

    if (!m_allow_local_ip && addr.isPrivate ()) {
        return false;
    }

    return true;
}

bool PeerlistManager::getPeerlistHead(std::list<PeerlistEntry> &bs_head, uint32_t depth)
{
    /*!
     * Sort the peers by last seen [Newer peers come first]
     */
    std::sort (m_peers_white.begin (), m_peers_white.end (), [](const auto &lhs, const auto &rhs)
    {
        return lhs.last_seen > rhs.last_seen;
    });

    uint32_t i = 0;

    for (const auto &peer : m_peers_white) {
        if (!peer.last_seen) {
            continue;
        }

        bs_head.push_back (peer);

        if (i > depth) {
            break;
        }

        i++;
    }

    return true;
}

bool PeerlistManager::getPeerlistFull(std::list<PeerlistEntry> &pl_gray, std::list<PeerlistEntry> &pl_white) const
{
    std::copy (m_peers_gray.begin (), m_peers_gray.end (), std::back_inserter (pl_gray));
    std::copy (m_peers_white.begin (), m_peers_white.end (), std::back_inserter (pl_white));

    return true;
}

bool PeerlistManager::setPeerJustSeen(uint64_t peer, uint32_t ip, uint32_t port)
{
    NetworkAddress addr;
    addr.ip = ip;
    addr.port = port;
    return setPeerJustSeen (peer, addr);
}

bool PeerlistManager::setPeerJustSeen(uint64_t peer, const NetworkAddress &addr)
{
    try {
        /*!
         * find in white list
         */
        PeerlistEntry ple;
        ple.adr = addr;
        ple.id = peer;
        ple.last_seen = time (NULL);
        return appendWithPeerWhite (ple);
    } catch (std::exception &) {
        /*!
         * TODO
         */
    }

    return false;
}

bool PeerlistManager::appendWithPeerWhite(const PeerlistEntry &newPeer)
{
    try {
        if (!isIpAllowed (newPeer.adr.ip)) {
            return true;
        }

        /*!
         * See if the peer already exists
         */
        auto whiteListIterator = std::find_if (m_peers_white.begin (), m_peers_white.end (), [&newPeer](const auto peer)
        {
            return peer.adr == newPeer.adr;
        });

        /*!
         * Peer doesn't exist
         */
        if (whiteListIterator == m_peers_white.end ()) {
            /*!
             * put new record into white list
             */
            m_peers_white.push_back (newPeer);
            trimWhitePeerlist ();
        } else {
            /*!
             * update record in white list
             */
            *whiteListIterator = newPeer;
        }

        /*!
         * remove from gray list, if need
         */
        auto grayListIterator = std::find_if (m_peers_gray.begin (), m_peers_gray.end (), [&newPeer](const auto peer)
        {
            return peer.adr == newPeer.adr;
        });


        if (grayListIterator != m_peers_gray.end ()) {
            m_peers_gray.erase (grayListIterator);
        }

        return true;
    } catch (std::exception &) {
        return false;
    }

    /*!
     * Unreachable Code
     */
    return false;
}

bool PeerlistManager::appendWithPeerGray(const PeerlistEntry &newPeer)
{
    try {
        if (!isIpAllowed (newPeer.adr.ip)) {
            return true;
        }

        /*!
         * find in white list
         */
        auto whiteListIterator = std::find_if (m_peers_white.begin (), m_peers_white.end (), [&newPeer](const auto peer)
        {
            return peer.adr == newPeer.adr;
        });

        if (whiteListIterator != m_peers_white.end ()) {
            return true;
        }

        /*!
         * update gray list
         */
        auto grayListIterator = std::find_if (m_peers_gray.begin (), m_peers_gray.end (), [&newPeer](const auto peer)
        {
            return peer.adr == newPeer.adr;
        });

        if (grayListIterator == m_peers_gray.end ()) {
            /*!
             * put new record into white list
             */
            m_peers_gray.push_back (newPeer);
            trimGrayPeerlist ();
        } else {
            /*!
             * update record in white list
             */
            *grayListIterator = newPeer;
        }

        return true;
    } catch (std::exception &) {
        return false;
    }

    /*!
     * Unreachable Code
     */
    return false;
}

Peerlist &PeerlistManager::getWhite()
{
    return m_whitePeerlist;
}

Peerlist &PeerlistManager::getGray()
{
    return m_grayPeerlist;
}

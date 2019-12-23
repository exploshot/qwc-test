// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <list>

#include <Global/CryptoNoteConfig.h>

#include <P2p/P2pProtocolTypes.h>
#include <P2p/Peerlist.h>

#include <Serialization/ISerializer.h>

class PeerlistManager
{
public:
    PeerlistManager();

    bool init(bool allow_local_ip);

    size_t getWhitePeersCount() const
    {
        return m_peers_white.size ();
    }

    size_t getGrayPeersCount() const
    {
        return m_peers_gray.size ();
    }

    bool mergePeerlist(const std::list <PeerlistEntry> &outer_bs);
    bool getPeerlistHead(std::list <PeerlistEntry> &bs_head, uint32_t depth = CryptoNote::P2P_DEFAULT_PEERS_IN_HANDSHAKE);
    bool getPeerlistFull(std::list <PeerlistEntry> &pl_gray, std::list <PeerlistEntry> &pl_white) const;
    bool getWhitePeerByIndex(PeerlistEntry &p, size_t i) const;
    bool getGrayPeerByIndex(PeerlistEntry &p, size_t i) const;
    bool appendWithPeerWhite(const PeerlistEntry &pr);
    bool appendWithPeerGray(const PeerlistEntry &pr);
    bool setPeerJustSeen(uint64_t peer, uint32_t ip, uint32_t port);
    bool setPeerJustSeen(uint64_t peer, const NetworkAddress &addr);
    bool setPeerUnreachable(const PeerlistEntry &pr);
    bool isIpAllowed(uint32_t ip) const;
    void trimWhitePeerlist();
    void trimGrayPeerlist();

    void serialize(CryptoNote::ISerializer &s);

    Peerlist &getWhite();
    Peerlist &getGray();

private:
    std::string m_config_folder;
    const std::string m_node_version;
    bool m_allow_local_ip;
    std::vector <PeerlistEntry> m_peers_gray;
    std::vector <PeerlistEntry> m_peers_white;
    Peerlist m_whitePeerlist;
    Peerlist m_grayPeerlist;
};

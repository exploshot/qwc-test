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

#include <vector>

#include <P2p/P2pProtocolTypes.h>

class Peerlist
{
public:
    Peerlist(std::vector<PeerlistEntry> &peers, size_t maxSize);

    /*!
     * Gets the size of the peer list
     */
    size_t count() const;

    /*!
     * Gets a peer list entry, indexed by time
     * @param entry
     * @param index
     * @return
     */
    bool get(PeerlistEntry &entry, size_t index) const;

    /*!
     * Trim the peer list, removing the oldest ones
     */
    void trim();

private:
    std::vector<PeerlistEntry> &m_peers;

    const size_t m_maxSize;
};

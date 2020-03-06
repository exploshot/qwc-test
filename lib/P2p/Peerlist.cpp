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

#include <P2p/Peerlist.h>

Peerlist::Peerlist(std::vector<PeerlistEntry> &peers, size_t maxSize)
    :
    m_peers (peers), m_maxSize (maxSize)
{
}

size_t Peerlist::count() const
{
    return m_peers.size ();
}

bool Peerlist::get(PeerlistEntry &entry, size_t i) const
{
    if (i >= m_peers.size ()) {
        return false;
    }

    /*!
     * Sort the peers by last seen [Newer peers come first]
     */
    std::sort (m_peers.begin (), m_peers.end (), [](const auto &lhs, const auto &rhs)
    {
        return lhs.last_seen > rhs.last_seen;
    });

    entry = m_peers[i];

    return true;
}

/*!
 * Remove the oldest peers
 */
void Peerlist::trim()
{
    if (m_peers.size () <= m_maxSize) {
        return;
    }

    /*!
     * Sort the peers by last seen [Newer peers come first]
     */
    std::sort (m_peers.begin (), m_peers.end (), [](const auto &lhs, const auto &rhs)
    {
        return lhs.last_seen > rhs.last_seen;
    });

    /*!
     * Trim to max size
     */
    m_peers.erase (m_peers.begin () + m_maxSize, m_peers.end ());
}

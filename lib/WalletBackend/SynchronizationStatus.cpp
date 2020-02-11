// Copyright (c) 2018, The TurtleCoin Developers
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

#include <Global/Constants.h>
#include <WalletBackend/SynchronizationStatus.h>

uint64_t SynchronizationStatus::getHeight() const
{
    return m_lastKnownBlockHeight;
}

void SynchronizationStatus::storeBlockHash(const Crypto::Hash hash,
                                           const uint64_t height)
{
    m_lastKnownBlockHeight = height;

    /*!
     * Already added this hash
     */
    if (!m_lastKnownBlockHashes.empty () && m_lastKnownBlockHashes.back () == hash) {
        return;
    }

    /*!
     * If we're at a checkpoint height, add the hash to the infrequent
     * checkpoints (at the beginning of the queue)
     */
    if (m_lastSavedCheckpointAt + Constants::BLOCK_HASH_CHECKPOINTS_INTERVAL < height) {
        m_lastSavedCheckpointAt = height;
        m_blockHashCheckpoints.push_front (hash);
    }

    m_lastKnownBlockHashes.push_front (hash);

    /*!
     * If we're exceeding capacity, remove the last (oldest) hash
     */
    if (m_lastKnownBlockHashes.size () > Constants::LAST_KNOWN_BLOCK_HASHES_SIZE) {
        m_lastKnownBlockHashes.pop_back ();
    }
}

std::deque <Crypto::Hash> SynchronizationStatus::getBlockCheckpoints() const
{
    return m_blockHashCheckpoints;
}

std::deque <Crypto::Hash> SynchronizationStatus::getRecentBlockHashes() const
{
    return m_lastKnownBlockHashes;
}

void SynchronizationStatus::fromJSON(const JSONObject &j)
{
    for (const auto &x : getArrayFromJSON (j, "blockHashCheckpoints")) {
        Crypto::Hash h;
        h.fromString (getStringFromJSONString (x));
        m_blockHashCheckpoints.push_back (h);
    }

    for (const auto &x : getArrayFromJSON (j, "lastKnownBlockHashes")) {
        Crypto::Hash h;
        h.fromString (getStringFromJSONString (x));
        m_lastKnownBlockHashes.push_back (h);
    }

    m_lastKnownBlockHeight = getUint64FromJSON (j, "lastKnownBlockHeight");
}

void SynchronizationStatus::toJSON(rapidjson::Writer <rapidjson::StringBuffer> &writer) const
{
    writer.StartObject ();

    writer.Key ("blockHashCheckpoints");
    writer.StartArray ();
    for (const auto hash : m_blockHashCheckpoints) {
        hash.toJSON (writer);
    }
    writer.EndArray ();

    writer.Key ("lastKnownBlockHashes");
    writer.StartArray ();
    for (const auto hash : m_lastKnownBlockHashes) {
        hash.toJSON (writer);
    }
    writer.EndArray ();

    writer.Key ("lastKnownBlockHeight");
    writer.Uint64 (m_lastKnownBlockHeight);

    writer.EndObject ();
}

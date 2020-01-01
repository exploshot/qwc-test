// Copyright (c) 2019, The TurtleCoin Developers
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

#include <atomic>
#include <vector>

#include <WalletTypes.h>

#include <Nigel/Nigel.h>

#include <SubWallets/SubWallets.h>

#include <Utilities/ThreadSafeDeque.h>

#include <WalletBackend/SynchronizationStatus.h>


class BlockDownloader
{
public:
    BlockDownloader()
    {
    };

    /*!
     * Constructor
     */
    BlockDownloader(const std::shared_ptr <Nigel> daemon,
                    const std::shared_ptr <SubWallets> subWallets,
                    const uint64_t startHeight,
                    const uint64_t startTimestamp);

    /*!
     * Move constructor
     */
    BlockDownloader(BlockDownloader &&old);

    /*!
     * Move assignment operator
     */
    BlockDownloader &operator=(BlockDownloader &&old);

    /*!
     * Destructor
     */
    ~BlockDownloader();

    /*!
     * Retrieve blockCount blocks from the internal store. does not remove
     * them. Returns as many as possible if the amount requested is not
     * available. May be empty (this is the norm when synced.)
     */
    std::vector <std::tuple<WalletTypes::WalletBlockInfo, uint32_t>> fetchBlocks(const size_t blockCount);

    /*!
     * Drops the oldest block from the internal queue
     */
    void dropBlock(const uint64_t blockHeight, const Crypto::Hash blockHash);

    /*!
     * Start block downloading process
     */
    void start();

    /*!
     * Stop block downloading process
     */
    void stop();

    /*!
     * Returns height of processed blocks
     */
    uint64_t getHeight() const;

    void fromJSON(const JSONObject &j,
                  const uint64_t startHeight,
                  const uint64_t startTimestamp);

    void toJSON(rapidjson::Writer <rapidjson::StringBuffer> &writer) const;

    void setSubWallets(const std::shared_ptr <SubWallets> subWallets);

    void initializeAfterLoad(const std::shared_ptr <Nigel> daemon);

private:

    /*!
     * Synchronizes pre-fetching blocks
     */
    void downloader();

    /*!
     * Determines if we should prefetch more blocks
     */
    bool shouldFetchMoreBlocks() const;

    /*!
     * Gets checkpoints of stored (not processed) blocks
     */
    std::vector <Crypto::Hash> getStoredBlockCheckpoints() const;

    /*!
     * Gets checkpoints of stored, processed, and infrequent checkpoints
     */
    std::vector <Crypto::Hash> getBlockCheckpoints() const;

    /*!
     * Downloads a set of blocks, if needed
     */
    bool downloadBlocks();

    /*!
     * Cached blocks
     */
    ThreadSafeDeque <std::tuple<WalletTypes::WalletBlockInfo, uint32_t>> m_storedBlocks;

    /*!
     * The daemon connection
     */
    std::shared_ptr <Nigel> m_daemon;

    /*!
     * Timestamp to begin syncing at
     */
    uint64_t m_startTimestamp;

    /*!
     * Height to begin syncing at
     */
    uint64_t m_startHeight;

    /*!
     * Sync progress
     */
    SynchronizationStatus m_synchronizationStatus;

    std::shared_ptr <SubWallets> m_subWallets;

    /*!
     * For synchronizing block downloading
     */
    std::mutex m_mutex;

    /*!
     * Are we ready to go attempt to retrieve more data
     */
    std::atomic<bool> m_consumedData = true;

    /*!
     * Should we try and fetch more data (Used in conjunction with m_consumedData)
     */
    std::condition_variable m_shouldTryFetch;

    /*!
     * Should we stop downloading
     */
    std::atomic<bool> m_shouldStop = false;

    /*!
     * Thread that performs the actual downloading of blocks
     */
    std::thread m_downloadThread;

    uint32_t m_arrivalIndex = 0;
};

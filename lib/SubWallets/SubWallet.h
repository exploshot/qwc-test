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

#pragma once

#include <string>
#include <unordered_set>

#include <CryptoTypes.h>
#include <WalletTypes.h>

#include <Crypto/Crypto.h>

#include <Utilities/Errors.h>

#include <rapidjson/document.h>

class SubWallet
{
public:

    SubWallet() = default;

    SubWallet(const Crypto::PublicKey publicSpendKey,
              const std::string address,
              const uint64_t scanHeight,
              const uint64_t scanTimestamp,
              const bool isPrimaryAddress);

    SubWallet(const Crypto::PublicKey publicSpendKey,
              const Crypto::SecretKey privateSpendKey,
              const std::string address,
              const uint64_t scanHeight,
              const uint64_t scanTimestamp,
              const bool isPrimaryAddress);

    /*!
     * Converts the class to a json object
     *
     * @param writer
     */
    void toJSON(rapidjson::Writer <rapidjson::StringBuffer> &writer) const;

    /*!
     * Initializes the class from a json string
     *
     * @param j
     */
    void fromJSON(const JSONValue &j);

    /*!
     * Generates a key image from the derivation, and stores the
     * transaction input along with the key image filled in
     *
     * @param derivation
     * @param outputIndex
     * @param isViewWallet
     * @return
     */
    Crypto::KeyImage getTxInputKeyImage(const Crypto::KeyDerivation derivation,
                                        const size_t outputIndex,
                                        const bool isViewWallet) const;

    /*!
     * Store a transaction input
     *
     * @param input
     * @param isViewWallet
     */
    void storeTransactionInput(const WalletTypes::TransactionInput input,
                               const bool isViewWallet);

    std::tuple <uint64_t, uint64_t> getBalance(const uint64_t currentHeight) const;

    void reset(const uint64_t scanHeight);
    bool isPrimaryAddress() const;
    std::string address() const;

    Crypto::PublicKey publicSpendKey() const;
    Crypto::SecretKey privateSpendKey() const;

    void markInputAsSpent(const Crypto::KeyImage keyImage,
                          const uint64_t spendHeight);

    void markInputAsLocked(const Crypto::KeyImage keyImage);

    std::vector <Crypto::KeyImage> removeForkedInputs(const uint64_t forkHeight,
                                                      const bool isViewWallet);

    void removeCancelledTransactions(const std::unordered_set <Crypto::Hash> cancelledTransactions);

    /*!
     * Gets inputs that are spendable at the given height
     *
     * @param height
     * @return
     */
    std::vector <WalletTypes::TxInputAndOwner> getSpendableInputs(const uint64_t height) const;

    uint64_t syncStartHeight() const;
    uint64_t syncStartTimestamp() const;

    void storeUnconfirmedIncomingInput(const WalletTypes::UnconfirmedInput input);

    void convertSyncTimestampToHeight(const uint64_t timestamp,
                                      const uint64_t height);

    void pruneSpentInputs(const uint64_t pruneHeight);

    std::vector <Crypto::KeyImage> getKeyImages() const;

private:

    /*!
     * A vector of the stored transaction input data, to be used for
     * sending transactions later
     */
    std::vector <WalletTypes::TransactionInput> m_unspentInputs;

    /*!
     * Inputs which have been used in a transaction, and are waiting to
     * either be put into a block, or return to our wallet
     */
    std::vector <WalletTypes::TransactionInput> m_lockedInputs;

    /*!
     * Inputs which have been spent in a transaction
     */
    std::vector <WalletTypes::TransactionInput> m_spentInputs;

    /*!
     * Inputs which have come in from a transaction we sent - either from
     * change or from sending to ourself - we use this to display unlocked
     * balance correctly
     */
    std::vector <WalletTypes::UnconfirmedInput> m_unconfirmedIncomingAmounts;

    /*!
     * This subwallet's public spend key
     */
    Crypto::PublicKey m_publicSpendKey;

    /*!
     * The subwallet's private spend key
     */
    Crypto::SecretKey m_privateSpendKey;

    /*!
     * The timestamp to begin syncing the wallet at
     * (usually creation time or zero)
     */
    uint64_t m_syncStartTimestamp = 0;

    /*!
     * The height to begin syncing the wallet at
     */
    uint64_t m_syncStartHeight = 0;

    /*!
     * This subwallet's public address
     */
    std::string m_address;

    /*!
     * The wallet has one 'main' address which we will use by default
     * when treating it as a single user wallet
     */
    bool m_isPrimaryAddress;
};

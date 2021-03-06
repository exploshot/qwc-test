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

#include <Common/Base58.h>

#include <Global/CryptoNoteConfig.h>

#include <Utilities/ValidateParameters.h>

#include <Serialization/SerializationTools.h>

#include <Utilities/Addresses.h>

namespace Utilities {

    /*!
     * Will throw an exception if the addresses are invalid. Please check they
     * are valid before calling this function. (e.g. use validateAddresses)
     *
     * Please note this function does not accept integrated addresses. Please
     * extract the payment ID from them before calling this function.
     */
    std::vector <Crypto::PublicKey> addressesToSpendKeys(const std::vector <std::string> addresses)
    {
        std::vector <Crypto::PublicKey> spendKeys;

        for (const auto &address : addresses) {
            const auto[spendKey, viewKey] = addressToKeys (address);
            spendKeys.push_back (spendKey);
        }

        return spendKeys;
    }

    std::tuple <Crypto::PublicKey, Crypto::PublicKey> addressToKeys(const std::string address)
    {
        CryptoNote::AccountPublicAddress parsedAddress;

        uint64_t prefix;

        /*!
         * Failed to parse
         */
        if (!parseAccountAddressString (prefix, parsedAddress, address)) {
            throw std::invalid_argument ("Address is not valid!");
        }

        /*!
         * Incorrect prefix
         */
        if (prefix != CryptoNote::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX) {
            throw std::invalid_argument ("Address is not valid!");
        }

        return {parsedAddress.spendPublicKey, parsedAddress.viewPublicKey};
    }

    /*!
     * Assumes address is valid
     */
    std::tuple <std::string, std::string> extractIntegratedAddressData(const std::string address)
    {
        /*!
         * Don't need this
         */
        uint64_t ignore;
        std::string decoded;

        /*!
         * Decode from base58
         */
        Tools::Base58::decodeAddress (address, ignore, decoded);

        const uint64_t paymentIDLen = 64;

        /*!
         * Grab the payment ID from the decoded address
         */
        std::string paymentID = decoded.substr (0, paymentIDLen);

        /*!
         * The binary array encoded keys are the rest of the address
         */
        std::string keys = decoded.substr (paymentIDLen, std::string::npos);

        /*!
         * Convert keys as string to binary array
         */
        CryptoNote::BinaryArray ba = Common::asBinaryArray (keys);

        CryptoNote::AccountPublicAddress addr;

        /*!
         * Convert from binary array to public keys
         */
        CryptoNote::fromBinaryArray (addr, ba);

        /*!
         * Convert the set of extracted keys back into an address
         */
        const std::string actualAddress = getAccountAddressAsStr (
            CryptoNote::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX,
            addr
        );

        return {actualAddress, paymentID};
    }

    std::string publicKeysToAddress(const Crypto::PublicKey publicSpendKey,
                                    const Crypto::PublicKey publicViewKey)
    {
        return getAccountAddressAsStr (
            CryptoNote::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX,
            {publicSpendKey, publicViewKey}
        );
    }

    /*!
     * Generates a public address from the given private keys
     */
    std::string privateKeysToAddress(const Crypto::SecretKey privateSpendKey,
                                     const Crypto::SecretKey privateViewKey)
    {
        Crypto::PublicKey publicSpendKey;
        Crypto::PublicKey publicViewKey;

        Crypto::secretKeyToPublicKey (privateSpendKey, publicSpendKey);
        Crypto::secretKeyToPublicKey (privateViewKey, publicViewKey);

        return getAccountAddressAsStr (
            CryptoNote::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX,
            {publicSpendKey, publicViewKey}
        );
    }

    std::tuple <Error, std::string> createIntegratedAddress(const std::string address,
                                                            const std::string paymentID)
    {
        if (Error error = validatePaymentID (paymentID); error != SUCCESS) {
            return {error, std::string ()};
        }

        const bool allowIntegratedAddresses = false;

        if (Error error = validateAddresses ({address}, allowIntegratedAddresses); error != SUCCESS) {
            return {error, std::string ()};
        }

        uint64_t prefix;

        CryptoNote::AccountPublicAddress addr;

        /*!
         * Get the private + public key from the address
         */
        parseAccountAddressString (prefix, addr, address);

        /*!
         * Pack as a binary array
         */
        std::vector <uint8_t> ba;
        CryptoNote::toBinaryArray (addr, ba);
        std::string keys = Common::asString (ba);

        /*!
         * Encode prefix + paymentID + keys as an address
         */
        const std::string integratedAddress = Tools::Base58::encodeAddress (
            CryptoNote::parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX,
            paymentID + keys
        );

        return {SUCCESS, integratedAddress};
    }

    std::string getAccountAddressAsStr(const uint64_t prefix,
                                       const CryptoNote::AccountPublicAddress &adr)
    {
        std::vector <uint8_t> ba;
        toBinaryArray (adr, ba);
        return Tools::Base58::encodeAddress (prefix, Common::asString (ba));
    }

    bool parseAccountAddressString(uint64_t &prefix,
                                   CryptoNote::AccountPublicAddress &adr,
                                   const std::string &str)
    {
        std::string data;

        return Tools::Base58::decodeAddress (str, prefix, data) &&
               fromBinaryArray (adr, Common::asBinaryArray (data)) &&
               checkKey (adr.spendPublicKey) &&
               checkKey (adr.viewPublicKey);
    }

} // namespace Utilities

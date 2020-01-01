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

#include <boost/utility/value_init.hpp>

#include <sstream>
#include <string>
#include <vector>

#include <CryptoTypes.h>
#include <version.h>

#include <Global/CryptoNoteConfig.h>


/*!
 * You can change things in this file, but you probably shouldn't. Leastways,
   without knowing what you're doing.
 */
namespace Constants {
    /*!
     * Amounts we make outputs into (Not mandatory, but a good idea)
     */
    const std::vector<uint64_t> PRETTY_AMOUNTS
        {
            1, 2, 3, 4, 5, 6, 7, 8, 9,
            10, 20, 30, 40, 50, 60, 70, 80, 90,
            100, 200, 300, 400, 500, 600, 700, 800, 900,
            1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
            10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000,
            100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000,
            1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000,
            10000000, 20000000, 30000000, 40000000, 50000000, 60000000, 70000000, 80000000, 90000000,
            100000000, 200000000, 300000000, 400000000, 500000000, 600000000, 700000000, 800000000, 900000000,
            1000000000, 2000000000, 3000000000, 4000000000, 5000000000, 6000000000, 7000000000, 8000000000, 9000000000,
            10000000000, 20000000000, 30000000000, 40000000000, 50000000000, 60000000000, 70000000000, 80000000000, 90000000000,
            100000000000, 200000000000, 300000000000, 400000000000, 500000000000, 600000000000, 700000000000, 800000000000, 900000000000,
            1000000000000, 2000000000000, 3000000000000, 4000000000000, 5000000000000, 6000000000000, 7000000000000, 8000000000000, 9000000000000,
            10000000000000, 20000000000000, 30000000000000, 40000000000000, 50000000000000, 60000000000000, 70000000000000, 80000000000000, 90000000000000,
            100000000000000, 200000000000000, 300000000000000, 400000000000000, 500000000000000, 600000000000000, 700000000000000, 800000000000000, 900000000000000,
            1000000000000000, 2000000000000000, 3000000000000000, 4000000000000000, 5000000000000000, 6000000000000000, 7000000000000000, 8000000000000000, 9000000000000000,
            10000000000000000, 20000000000000000, 30000000000000000, 40000000000000000, 50000000000000000, 60000000000000000, 70000000000000000, 80000000000000000, 90000000000000000,
            100000000000000000, 200000000000000000, 300000000000000000, 400000000000000000, 500000000000000000, 600000000000000000, 700000000000000000, 800000000000000000, 900000000000000000,
            1000000000000000000, 2000000000000000000, 3000000000000000000, 4000000000000000000, 5000000000000000000, 6000000000000000000, 7000000000000000000, 8000000000000000000, 9000000000000000000,
            10000000000000000000ull
        };

    /*!
     * Indicates the following data is a payment ID
     */
    const uint8_t TX_EXTRA_PAYMENT_ID_IDENTIFIER = 0x00;

    /*!
     * Indicates the following data is a transaction public key
     */
    const uint8_t TX_EXTRA_PUBKEY_IDENTIFIER = 0x01;

    /*!
     * Indicates the following data is an extra nonce
     */
    const uint8_t TX_EXTRA_NONCE_IDENTIFIER = 0x02;

    /*!
     * Indicates the following data is a merge mine depth+merkle root
     */
    const uint8_t TX_EXTRA_MERGE_MINING_IDENTIFIER = 0x03;


    const std::string windowsAsciiArt =
        "\n                                                              \n"
        "                         _                   _                  \n"
        "                        | |                 (_)                 \n"
        "  __ ___      _____ _ __| |_ _   _  ___ ___  _ _ __             \n"
        " / _` \\ \\ /\\ / / _ \\ '__| __| | | |/ __/ _ \\| | '_  \\     \n"
        "| (_| |\\ V  V /  __/ |  | |_| |_| | (_| (_) | | | | |          \n"
        " \\__, | \\_/\\_/ \\___|_|   \\__|\\__, |\\___\\___/|_|_| |_|   \n"
        "    | |                       __/ |                             \n"
        "    |_|                      |___/                              \n"
        "                                                                \n";

    const std::string nonWindowsAsciiArt =
        "\n                                                                                 \n"
        " ██████╗ ██╗    ██╗███████╗██████╗ ████████╗██╗   ██╗ ██████╗ ██████╗ ██╗███╗   ██╗\n"
        "██╔═══██╗██║    ██║██╔════╝██╔══██╗╚══██╔══╝╚██╗ ██╔╝██╔════╝██╔═══██╗██║████╗  ██║\n"
        "██║   ██║██║ █╗ ██║█████╗  ██████╔╝   ██║    ╚████╔╝ ██║     ██║   ██║██║██╔██╗ ██║\n"
        "██║▄▄ ██║██║███╗██║██╔══╝  ██╔══██╗   ██║     ╚██╔╝  ██║     ██║   ██║██║██║╚██╗██║\n"
        "╚██████╔╝╚███╔███╔╝███████╗██║  ██║   ██║      ██║   ╚██████╗╚██████╔╝██║██║ ╚████║\n"
        " ╚══▀▀═╝  ╚══╝╚══╝ ╚══════╝╚═╝  ╚═╝   ╚═╝      ╚═╝    ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝\n"
        "                                                                                   \n";

    /*!
     * Windows has some characters it won't display in a terminal. If your ascii
     * art works fine on Windows and Linux terminals, just replace 'asciiArt' with
     * the art itself, and remove these two #ifdefs and above ascii arts.
     */
    #ifdef _WIN32
        const std::string asciiArt = windowsAsciiArt;
    #else

    const std::string asciiArt = nonWindowsAsciiArt;

    #endif

    const Crypto::Hash NULL_HASH = Crypto::Hash ({
                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                 });

    const Crypto::PublicKey NULL_PUBLIC_KEY = Crypto::PublicKey ({
                                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                                 });

    const Crypto::SecretKey NULL_SECRET_KEY = Crypto::SecretKey ({
                                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                                     0, 0, 0, 0, 0, 0, 0, 0,
                                                                 });

    /*!
     * We use this to check that the file is a wallet file, this bit does
     * not get encrypted, and we can check if it exists before decrypting.
     * If it isn't, it's not a wallet file.
     */
    const std::array<char, 64> IS_A_WALLET_IDENTIFIER =
        {{
             0x49, 0x66, 0x20, 0x49, 0x20, 0x70, 0x75, 0x6c, 0x6c, 0x20, 0x74,
             0x68, 0x61, 0x74, 0x20, 0x6f, 0x66, 0x66, 0x2c, 0x20, 0x77, 0x69,
             0x6c, 0x6c, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x64, 0x69, 0x65, 0x3f,
             0x0a, 0x49, 0x74, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62,
             0x65, 0x20, 0x65, 0x78, 0x74, 0x72, 0x65, 0x6d, 0x65, 0x6c, 0x79,
             0x20, 0x70, 0x61, 0x69, 0x6e, 0x66, 0x75, 0x6c, 0x2e
         }};

    /*!
     * We use this to check if the file has been correctly decoded, i.e.
     * is the password correct. This gets encrypted into the file, and
     * then when unencrypted the file should start with this - if it
     * doesn't, the password is wrong
     */
    const std::array<char, 26> IS_CORRECT_PASSWORD_IDENTIFIER =
        {{
             0x59, 0x6f, 0x75, 0x27, 0x72, 0x65, 0x20, 0x61, 0x20, 0x62, 0x69,
             0x67, 0x20, 0x67, 0x75, 0x79, 0x2e, 0x0a, 0x46, 0x6f, 0x72, 0x20,
             0x79, 0x6f, 0x75, 0x2e
         }};

    /*!
     * The number of iterations of PBKDF2 to perform on the wallet
     * password.
     */
    const uint64_t PBKDF2_ITERATIONS = 500000;

    /*!
     * What version of the file format are we on (to make it easier to
     * upgrade the wallet format in the future)
     */
    const uint16_t WALLET_FILE_FORMAT_VERSION = 0;

    /*!
     * How large should the m_lastKnownBlockHashes container be
     */
    const size_t LAST_KNOWN_BLOCK_HASHES_SIZE = 50;

    /*!
     * Save a block hash checkpoint every BLOCK_HASH_CHECKPOINTS_INTERVAL
     * blocks
     */
    const uint32_t BLOCK_HASH_CHECKPOINTS_INTERVAL = 5000;

    /*!
     * The amount of blocks since an input has been spent that we remove it
     * from the container
     */
    const uint64_t PRUNE_SPENT_INPUTS_INTERVAL = CryptoNote::parameters::EXPECTED_NUMBER_OF_BLOCKS_PER_DAY * 2;

    /*!
     * When we get the global indexes, we pass in a range of blocks, to obscure
     * which transactions we are interested in - the ones that belong to us.
     * To do this, we get the global indexes for all transactions in a range.
     *
     * For example, if we want the global indexes for a transaction in block
     * 17, we get all the indexes from block 10 to block 20.
     *
     * This value determines how many blocks to take from.
     */
    const uint64_t GLOBAL_INDEXES_OBSCURITY = 10;

    /*!
     * Amount of blocks to take in one chunk from the block downloader, and
     * then split into threads and process. Too large will result in large
     * jumps in the sync height, but should offer better performance from a
     * decrease in locking of data structures.
     */
    const uint64_t BLOCK_PROCESSING_CHUNK = 500;

    const size_t KILOBYTE = 1024;
    const size_t MEGABYTE = 1024 * KILOBYTE;
    const size_t GIGABYTE = 1024 * MEGABYTE;
} // namespace Constants

/*!
 * Make sure everything in here is const - or it won't compile!
 */
namespace WalletConfig {

    /*!
     * The prefix your coins address starts with
     */
    const uint64_t uAddressPrefix = 0x14820c;
    const std::string addressPrefix = "QWC";

    /*!
     * Your coins 'Ticker', e.g. Monero = XMR, Bitcoin = BTC
     */
    const std::string ticker = "QWC";

    /*!
     * The filename to output the CSV to in save_csv
     */
    const std::string csvFilename = "transactions.csv";

    /*!
     * The filename to read+write the address book to - consider starting with
     * a leading '.' to make it hidden under mac+linux
     */
    const std::string addressBookFilename = ".addressBook.json";

    /*!
     * The name of your daemon
     */
    const std::string daemonName = "QDaemon";

    /*!
     * The name to call this wallet
     */
    const std::string walletName = "SimpleWallet";

    /*!
     * The name of service/walletd, the programmatic rpc interface to a wallet
     */
    const std::string walletdName = "qwertycoin-service";

    /*!
     * The full name of your crypto
     */
    const std::string coinName = std::string (CryptoNote::CRYPTONOTE_NAME);

    /*!
     * Where can your users contact you for support? E.g. discord
     */
    const std::string contactLink = "http://chat.qwertycoin.org";

    /*!
     * The number of decimals your coin has
     */
    const uint8_t numDecimalPlaces = CryptoNote::parameters::CRYPTONOTE_DUST_DECIMAL_POINT;
    const uint8_t numDisplayDecimalPlaces = CryptoNote::parameters::CRYPTONOTE_DISPLAY_DECIMAL_POINT;

    /*!
     * The length of a standard address for your coin
     */
    const uint16_t standardAddressLength = 98;

    /*!
     * The length of an integrated address for your coin - It's the same as
     * a normal address, but there is a paymentID included in there - since
     * payment ID's are 64 chars, and base58 encoding is done by encoding
     * chunks of 8 chars at once into blocks of 11 chars, we can calculate
     * this automatically
     */
    const uint16_t integratedAddressLength = standardAddressLength + ((64 * 11) / 8);

    /*!
     * The default fee value to use with transactions (in ATOMIC units!)
     */
    const uint64_t defaultFee = CryptoNote::parameters::MINIMUM_FEE;

    /*!
     * The minimum fee value to allow with transactions (in ATOMIC units!)
     */
    const uint64_t minimumFee = CryptoNote::parameters::MINIMUM_FEE;

    /*!
     * The minimum amount allowed to be sent - usually 1 (in ATOMIC units!)
     */
    const uint64_t minimumSend = 1;

    /*!
     * Is a mixin of zero disabled on your network?
     */
    const bool mixinZeroDisabled = false;

    /*!
     * If a mixin of zero is disabled, at what height was it disabled?
     * E.g. fork height, or 0, if never allowed. This is ignored if a
     * mixin of zero is allowed
     */
    const uint64_t mixinZeroDisabledHeight = CryptoNote::parameters::MIXIN_LIMITS_V2_HEIGHT;

    /*!
     * Should we process coinbase transactions? We can skip them to speed up
     * syncing, as most people don't have solo mined transactions
     */
    const bool processCoinbaseTransactions = true;

    /*!
     * Max size of a post body response - 10MB
     * Will decrease the amount of blocks requested from the daemon if this
     * is exceeded.
     * Note - blockStoreMemoryLimit - maxBodyResponseSize should be greater
     * than zero, or no data will get cached.
     * Further note: Currently blocks request are not decreased if this is
     * exceeded. Needs to be implemented in future?
     */
    const size_t maxBodyResponseSize = 10 * Constants::MEGABYTE;

    /*!
     * The amount of memory to use storing downloaded blocks - 50MB
     */
    const size_t blockStoreMemoryLimit = 50 * Constants::MEGABYTE;

    const size_t TIMESTAMP_MAX_WIDTH = 19;
    const size_t HASH_MAX_WIDTH = 64;
    const size_t TOTAL_AMOUNT_MAX_WIDTH = 22;
    const size_t FEE_MAX_WIDTH = 14;
    const size_t BLOCK_MAX_WIDTH = 7;
    const size_t UNLOCK_TIME_MAX_WIDTH = 11;
    const size_t MESSAGE_MAX_WIDTH = 128;
    const size_t SENDER_MAX_WITH = 16;

} // namespace WalletConfig

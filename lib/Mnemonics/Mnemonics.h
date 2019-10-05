// Copyright 2014-2018 The Monero Developers
// Copyright 2018 The TurtleCoin Developers
// Copyright 2018 The Plenteum Developers
//
// Please see the included LICENSE file for more information.

#include <CryptoNote.h>

#include <tuple>

#include <vector>

#include <Errors/Errors.h>

namespace Mnemonics
{
    std::tuple<Error, Crypto::SecretKey> MnemonicToPrivateKey(const std::string words, std::string &languageName);

    std::string PrivateKeyToMnemonic(const Crypto::SecretKey privateKey, const std::string& languageName);

} // namespace Mnemonics

namespace Crypto {
    namespace ElectrumWords {
        const int seedLength = 24;
        const std::string oldLanguageName = "English";

        /*
         *  Converts seed words to bytes (secret key).
         * 
         *  @param words String containing the words separated by spaces.
         *  @param dst To put the secret key restored from the words.
         *  @param languageName Language of the seed as found gets written here.
         * 
         *  @return false if not a multiple of 3 words, or if word is not in the words list
         */
        bool wordsToBytes(std::string words,
                          Crypto::SecretKey& dst,
                          std::string& languageName);

        /*
         *  Converts bytes (secret key) to seed words.
         * 
         *  @param src Secret key.
         *  @param words Space delimited concatenated words get written here.
         *  @param languageName Seed language name.
         * 
         *  @return true if successful false if not. Unsuccessful if wrong key size.
         */
        bool bytesToWords(const Crypto::SecretKey& src,
                          std::string& words,
                          const std::string& languageName);

        /*
         *  Gets a list of seed languages that are supported.
         * 
         *  @param languages A vector is set to the list of languages.
         */

        void getLanguageList(std::vector<std::string>& languages);

        /*
         *  Tells if the seed passed is an old style seed or not.
         * 
         *  @param seed The seed to check (a space delimited concatenated word list).
         * 
         *  @return true if the seed passed is a old style seed false if not.
         */
        bool getIsOldStyleSeed(std::string seed);

    } // namespace ElectrumWords
} // namespace Crypto
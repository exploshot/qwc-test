// Copyright 2014-2018 The Monero Developers
// Copyright 2018 The TurtleCoin Developers
// Copyright 2018 The Plenteum Developers
//
// Please see the included LICENSE file for more information.

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/crc.hpp>
#include <boost/filesystem.hpp>

#include <Common/Lazy.h>

#include <Mnemonics/CRC32.h>
#include <Mnemonics/Mnemonics.h>
#include <Mnemonics/WordList.h>
#include <Mnemonics/LanguageBase.h>

namespace Language {

    using namespace Mnemonics;

    const std::string WordLists::English           ::cName = "English";
    const std::string WordLists::Ukrainian         ::cName = "українська мова";
    const std::string WordLists::Polish            ::cName = "język polski";
    const std::string WordLists::German            ::cName = "Deutsch";
    const std::string WordLists::French            ::cName = "Français";
    const std::string WordLists::Spanish           ::cName = "Español";
    const std::string WordLists::Italian           ::cName = "Italiano";
    const std::string WordLists::Portugese         ::cName = "Português";
    const std::string WordLists::Dutch             ::cName = "Nederlands";
    const std::string WordLists::Japanese          ::cName = "日本語";
    const std::string WordLists::ChineseSimplified ::cName = "简体中文 (中国)";
    const std::string WordLists::Russian           ::cName = "русский язык";

    typedef std::unordered_map<std::string, Lazy<std::shared_ptr<Base>>> LanguageMap;

    const static LanguageMap cLanguageMap = {
        { WordLists::English   ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::English>();    }) },
        { WordLists::Ukrainian ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::Ukrainian>();  }) },
        { WordLists::Polish    ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::Polish>();     }) },
        { WordLists::German    ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::German>();     }) },
        { WordLists::French    ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::French>();     }) },
        { WordLists::Spanish   ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::Spanish>();    }) },
        { WordLists::Italian   ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::Italian>();    }) },
        { WordLists::Portugese::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::Portugese>(); }) },
        { WordLists::Dutch     ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::Dutch>();      }) },
        { WordLists::Japanese  ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::Japanese>();   }) },
        { WordLists::ChineseSimplified::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::ChineseSimplified>(); }) },
        { WordLists::Russian   ::cName, Lazy<std::shared_ptr<Base>>([](){ return std::make_shared<WordLists::Russian>();    }) },
    };

} // namespace Language

namespace Mnemonics
{
    std::tuple<Error, Crypto::SecretKey> MnemonicToPrivateKey(const std::string words, std::string &languageName)
    {
        const size_t len = words.size();

        /* Mnemonics must be 25 words long */
        if (len != 25)
        {
            /* Write out "word" or "words" to make the grammar of the next sentence
               correct, based on if we have 1 or more words */
            const std::string wordPlural = len == 1 ? "word" : "words";

            Error error(
                MNEMONIC_WRONG_LENGTH,
                "The mnemonic seed given is the wrong length. It should be "
                "25 words long, but it is " + std::to_string(len) + " " +
                wordPlural + " long."
            );

            return {error, Crypto::SecretKey()};
        }

        Crypto::SecretKey key;
        std::string wordls;
        for  (const auto & p : words) wordls += p;
        Crypto::ElectrumWords::wordsToBytes(wordls, key, languageName);

        return {SUCCESS, key};
    }

    std::string PrivateKeyToMnemonic(
        const Crypto::SecretKey privateKey, 
        const std::string &languageName
    ) {
        std::string words;
        
        Crypto::ElectrumWords::bytesToWords(privateKey, words, languageName);

        return words;
    }
}

namespace
{
    uint32_t createChecksumIndex(
        const std::vector<std::string>& wordList,
        uint32_t uniquePrefixLength
    );
    
    bool checksumTest(
        std::vector<std::string> seed, 
        uint32_t uniquePrefixLength
    );

    /*
     *          Finds the word list that contains the seed words and puts the 
     *          indices where matches occured in matched_indices.
     * 
     * @param   seed List of words to match.
     * @param   hasChecksum The seed has a checksum word (maybe not checked).
     * @param   matchedIndices The indices where the seed words were found are added to this.
     * @param   language Language instance pointer to write to after it is found.
     * 
     * @return  true if all the words were present in some language false if not.
     */
    bool findSeedLanguage(
        const std::vector<std::string>& seed,
        bool hasChecksum,
        std::vector<uint32_t>& matchedIndices,
        Language::Base **language
    ) {
        Language::Base* fallback = NULL;

        // Iterate through all the languages and find a match
        for (auto it1 = Language::cLanguageMap.cbegin(); it1 != Language::cLanguageMap.cend(); ++it1) {
            std::shared_ptr<Language::Base>& lang = it1->second;

            const auto& wordMap = lang->getWordMap();
            const auto& trimmedWordMap = lang->getTrimmedWordMap();

            // To iterate through seed words
            std::vector<std::string>::const_iterator it2;
            bool fullMatch = true;
            std::string trimmedWord;

            // Iterate through all the words and see if they're all present
            for (it2 = seed.cbegin(); it2 != seed.cend(); ++it2) {
                if (hasChecksum) {
                    trimmedWord = Language::UTF8Prefix(*it2, lang->getUniquePrefixLength());

                    // Use the trimmed words and map
                    if (trimmedWordMap.count(trimmedWord) == 0) {
                        fullMatch = false;
                        break;
                    }
                    matchedIndices.push_back(trimmedWordMap.at(trimmedWord));
                } else {
                    if (wordMap.count(*it2) == 0) {
                        fullMatch = false;
                        break;
                    }
                    matchedIndices.push_back(wordMap.at(*it2));
                }
            }

            if (fullMatch) {
                // if we were using prefix only, and we have a checksum, check it now
                // to avoid false positives due to prefix set being too common
                if (hasChecksum && !checksumTest(seed, lang->getUniquePrefixLength())) {
                    fallback = lang.get();
                    fullMatch = false;
                }
            }

            if (fullMatch) {
                *language = lang.get();
                return true;
            }

            // Some didn't match. Clear the index array.
            matchedIndices.clear();
        }

        // if we get there, we've not found a good match, but we might have a fallback,
        // if we detected a match which did not fit the checksum, which might be a badly
        // typed/transcribed seed in the right language
        if (fallback) {
            *language = fallback;
            return true;
        }

        return false;
    }

    /*
     *          Creates a checksum index in the word list array on the list of words.
     * 
     * @param   wordList Vector of words
     * @param   uniquePrefixLength the prefix length of each word to use for checksum
     * 
     * @return  Checksum Index
     */
    uint32_t createChecksumIndex(
        const std::vector<std::string>& wordList,
        uint32_t uniquePrefixLength
    ) {
        std::string trimmedWords = "";

        for (auto it = wordList.cbegin(); it != wordList.cend(); ++it) {
            if (it->length() > uniquePrefixLength) {
                trimmedWords += Language::UTF8Prefix(*it, uniquePrefixLength);
            } else {
                trimmedWords += *it;
            }
        }

        boost::crc_32_type result;
        result.process_bytes(trimmedWords.data(), trimmedWords.length());

        return result.checksum() % Crypto::ElectrumWords::seedLength;
    }

    /*
     *          Does the checksum test on the seed passed.
     * 
     * @param   seed Vector of seed words
     * @param   uniquePrefixLength the prefix length of each word to use for checksum
     * 
     * @return  True if the test passed false if not.
     */
    bool checksumTest(
        std::vector<std::string> seed, 
        uint32_t uniquePrefixLength
    ) {
        // The last word is the checksum.
        std::string lastWord = seed.back();
        seed.pop_back();

        std::string checksum = seed[createChecksumIndex(seed, uniquePrefixLength)];
        std::string trimmedChecksum 
            = checksum.length() 
            > uniquePrefixLength 
            ? Language::UTF8Prefix(checksum, uniquePrefixLength) : checksum;
        std::string trimmedLastWord
            = lastWord.length()
            > uniquePrefixLength
            ? Language::UTF8Prefix(lastWord, uniquePrefixLength) : lastWord;
        
        return trimmedChecksum == trimmedLastWord;
    }

} // namespace

namespace Crypto
{
    namespace ElectrumWords
    {
        /*
         *  Converts seed words to bytes (secret key).
         * 
         *  @param words String containing the words separated by spaces.
         *  @param dst To put the secret key restored from the words.
         *  @param languageName Language of the seed as found gets written here.
         * 
         *  @return false if not a multiple of 3 words, or if word is not in the words list
         */
        bool wordsToBytes(
            std::string words,
            Crypto::SecretKey& dst,
            std::string& languageName) 
        {
            std::vector<std::string> seed;

            boost::algorithm::trim(words);
            boost::split(seed, words, boost::is_any_of(" "), boost::token_compress_on);

            // error on non-compliant word list
            if (seed.size() != seedLength/2
                && seed.size() != seedLength
                && seed.size() != seedLength + 1) {
                return false;
            }

            // If it is seed with a checksum
            bool hasChecksum = seed.size() == (seedLength + 1);

            std::vector<uint32_t> matchedIndices;
            Language::Base *language;
            if (!findSeedLanguage(seed, hasChecksum, matchedIndices, &language)) {
                return false;
            }

            languageName = language->getLanguageName();
            uint32_t wordListLength = static_cast<uint32_t>(language->getWordList().size());

            if (hasChecksum) {
                if (!checksumTest(seed, language->getUniquePrefixLength())) {
                    return false; // checksum fail
                }
                seed.pop_back();
            }

            for (unsigned int i = 0; i < seed.size() / 3; i++) {
                uint32_t val;
                uint32_t w1, w2, w3;
                w1 = matchedIndices[i*3];
                w2 = matchedIndices[i*3 + 1];
                w3 = matchedIndices[i*3 + 2];

                val = w1 + wordListLength + (((wordListLength - w1) + w2) % wordListLength) +
                      wordListLength * wordListLength * (((wordListLength - w2) + w3) % wordListLength);
                
                if (val % wordListLength != w1) {
                    return false;
                }

                memcpy(dst.data + i * 4, &val, 4); // copy 4 butes to position
            }

            std::string wListCopy = words;
            if (seed.size() == seedLength / 2) {
                memcpy(dst.data + 16, dst.data, 16); // if electrum 12-word seed, duplicate it
                wListCopy += ' ';
                wListCopy += words;
            }

            return true;
        }

        /*
         *  Converts bytes (secret key) to seed words.
         * 
         *  @param src Secret key.
         *  @param words Space delimited concatenated words get written here.
         *  @param languageName Seed language name.
         * 
         *  @return true if successful false if not. Unsuccessful if wrong key size.
         */
        bool bytesToWords(
            const Crypto::SecretKey& src,
            std::string& words,
            const std::string& languageName) {
            
            if (sizeof(src.data) % 4 != 0 || sizeof(src.data) == 0) {
                return false;
            }

            auto itLanguage = Language::cLanguageMap.find(languageName);
            if (itLanguage == Language::cLanguageMap.cend()) {
                return false;
            }
            std::shared_ptr<Language::Base>& language = itLanguage->second;

            const std::vector<std::string>& wordList = language->getWordList();
            // To store the words for random access to add the checksum word later.
            std::vector<std::string> wordsStore;

            uint32_t wordListLength = static_cast<uint32_t>(wordList.size());
            // 8 bytes -> 3 words.  8 digits base 16 -> 3 digits base 1626

            for (unsigned int i = 0; i < sizeof(src.data) / 4; i++, words += ' ') {
                uint32_t val;
                uint32_t w1, w2, w3;

                memcpy(&val, (src.data) + (i * 4), 4);

                w1 = val % wordListLength;
                w2 = ((val / wordListLength) + w1) % wordListLength;
                w3 = (((val / wordListLength) / wordListLength) + w2) % wordListLength;

                words += wordList[w1];
                words += ' ';
                words += wordList[w2];
                words += ' ';
                words += wordList[w3];

                wordsStore.push_back(wordList[w1]);
                wordsStore.push_back(wordList[w2]);
                wordsStore.push_back(wordList[w3]);
            }

            words.pop_back();
            words += ' ';
            words += wordsStore[createChecksumIndex(wordsStore, language->getUniquePrefixLength())];

            return true;
        }

        /*
         *  Gets a list of seed languages that are supported.
         * 
         *  @param languages A vector is set to the list of languages.
         */

        void getLanguageList(std::vector<std::string>& languages) {
            languages.clear();
            auto itBegin = Language::cLanguageMap.cbegin();
            auto itEnd   = Language::cLanguageMap.cend();
            std::transform(
                itBegin,
                itEnd,
                std::back_inserter(languages),
                [](const Language::LanguageMap::value_type& pair) { return pair.first; }
            );
        }

        /*
         *  Tells if the seed passed is an old style seed or not.
         * 
         *  @param seed The seed to check (a space delimited concatenated word list).
         * 
         *  @return true if the seed passed is a old style seed false if not.
         */
        bool getIsOldStyleSeed(std::string seed) {
            std::vector<std::string> wordList;
            boost::algorithm::trim(seed);
            boost::split(wordList, seed, boost::is_any_of(" "), boost::token_compress_on);

            return wordList.size() != (seedLength + 1);
        }
    } // namespace ElectrumWords
    
} // namespace Crypto

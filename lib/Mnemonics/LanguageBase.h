// Copyright (c) 2014-2017, The Monero Project
// Copyright (c) 2017-2018, Karbo developers
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Language {
    inline std::string UTF8Prefix(const std::string &s, size_t count)
    {
        std::string prefix = "";
        const char *ptr = s.c_str ();
        while (count-- && *ptr) {
            prefix += *ptr++;
            while (((*ptr) & 0xc0) == 0x80) {
                prefix += *ptr++;
            }

        }
        return prefix;
    }

    class Base
    {
    protected:
        enum
        {
            ALLOW_SHORT_WORDS = 1
                << 0,
            ALLOW_DUPLICATE_PREFIXES = 1
                << 1,
        };

        /*!
         * Name of Language
         */
        const std::string &languageName;

        /*!
         * Apointer to the Array of Words
         */
        const std::vector<std::string> wordList;

        /*!
         * Hash Table to find word's Index
         */
        std::unordered_map<std::string, uint32_t> wordMap;

        /*!
         * Hash Table to find word's trimmed Index
         */
        std::unordered_map<std::string, uint32_t> trimmedWordMap;

        /*!
         * Number of unique starting Characters to trim the Wordlist to when matching
         */
        uint32_t uniquePrefixLength;

        /*!
         *          Populate the word maps after the list is ready.
         *
         * @param   flags
         */
        void populateMaps(uint32_t flags = 0)
        {
            int ii;
            std::vector<std::string>::const_iterator it;

            if (wordList.size () != 1626) {
                throw std::runtime_error ("Wrong wordlist length for " + languageName);
            }

            for (it = wordList.begin (), ii = 0; it != wordList.end (); it++, ii++) {
                wordMap[*it] = ii;
                if ((*it).size () < uniquePrefixLength) {
                    if (flags & ALLOW_SHORT_WORDS) {
                        //LOG_PRINT_L0(language_name << " word '" << *it
                        // << "' is shorter than its prefix length, " << unique_prefix_length);
                    } else {
                        throw std::runtime_error ("Too short word in " + languageName + " word list: " + *it);
                    }
                }
                std::string trimmed;
                if (it->length () > uniquePrefixLength) {
                    trimmed = UTF8Prefix (*it, uniquePrefixLength);
                } else {
                    trimmed = *it;
                }
                if (trimmedWordMap.find (trimmed) != trimmedWordMap.end ()) {
                    if (flags & ALLOW_DUPLICATE_PREFIXES) {
                        //LOG_PRINT_L0("Duplicate prefix in " << language_name
                        //<< " word list: " << trimmed);
                    } else {
                        throw std::runtime_error ("Duplicate prefix in " + languageName
                                                  + " word list: " + trimmed);
                    }
                }
                trimmedWordMap[trimmed] = ii;
            }
        }
    public:
        Base(const std::string &languageName,
             const std::vector<std::string> &words,
             uint32_t prefixLength)
            : wordList (words),
              uniquePrefixLength (prefixLength),
              languageName (languageName)
        {
        }

        virtual ~Base() = default;

        /*!
         * Returns a pointer to the word list
         */
        const std::vector<std::string> &getWordList() const
        {
            return wordList;
        }

        /*!
         * Returns a pointer to the word map
         */
        const std::unordered_map<std::string, uint32_t> &getWordMap() const
        {
            return wordMap;
        }

        /*!
         * Returns a pointer to the trimmed word map.
         */
        const std::unordered_map<std::string, uint32_t> &getTrimmedWordMap() const
        {
            return trimmedWordMap;
        }

        /*!
         * Returns the name of the language.
         */
        const std::string &getLanguageName() const
        {
            return languageName;
        }

        /*!
         * Returns the number of unique starting characters to be used for matching.
         */
        uint32_t getUniquePrefixLength() const
        {
            return uniquePrefixLength;
        }
    };
} // namespace Language

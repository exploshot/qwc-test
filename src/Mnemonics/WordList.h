// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Code surrounding the word list is Copyright (c) 2014-2017, The Monero Project
//                                   Copyright (c) 2017-2018, Karbo developers
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

#include <Mnemonics/LanguageBase.h>

#include <Mnemonics/chineseSimpl.h>
#include <Mnemonics/dutch.h>
#include <Mnemonics/english.h>
#include <Mnemonics/french.h>
#include <Mnemonics/german.h>
#include <Mnemonics/italian.h>
#include <Mnemonics/japanese.h>
#include <Mnemonics/polish.h>
#include <Mnemonics/portugese.h>
#include <Mnemonics/russian.h>
#include <Mnemonics/spanish.h>
#include <Mnemonics/ukrainian.h>

namespace Language {
    namespace Mnemonics {
        namespace WordLists {

            class ChineseSimplified: public Base
            {
            public:
                const static std::string cName;

                ChineseSimplified()
                    : Base (cName, chineseSimplified, 1)
                {
                    populateMaps ();
                }
            };

            class Dutch : public Base {
            public:
                const static std::string cName;

                Dutch() : Base(cName, dutch, 4) {
                    populateMaps();
                }
            };

            class English: public Base
            {
            public:
                const static std::string cName;

                English()
                    : Base (cName, english, 3)
                {
                    populateMaps ();
                }
            };

            class French : public Base
            {
            public:
                const static std::string cName;

                French() : Base(cName, french, 4) {
                    populateMaps();
                }
            };

            class German : public Base
            {
            public:
                const static std::string cName;

                German() : Base(cName, german, 4) {
                    populateMaps();
                }
            };

            class Italian : public Base
            {
            public:
                const static std::string cName;

                Italian() : Base(cName, italian, 4) {
                    populateMaps();
                }
            };

            class Japanese : public Base
            {
            public:
                const static std::string cName;

                Japanese() : Base(cName, japanese, 3) {
                    populateMaps();
                }
            };

            class Polish : public Base
            {
            public:
                const static std::string cName;

                Polish() : Base(cName, polish, 4) {
                    populateMaps();
                }
            };

            class Portugese : public Base
            {
            public:
                const static std::string cName;

                Portugese() : Base(cName, portugese, 4) {
                    populateMaps();
                }
            };

            class Russian : public Base
            {
            public:
                const static std::string cName;

                Russian() : Base(cName, russian, 4) {
                    populateMaps();
                }
            };

            class Spanish : public Base
            {
            public:
                const static std::string cName;

                Spanish() : Base(cName, spanish, 4) {
                    populateMaps();
                }
            };

            class Ukrainian : public Base
            {
            public:
                const static std::string cName;

                Ukrainian() : Base(cName, ukrainian, 4) {
                    populateMaps();
                }
            };

        }
    }
} // namespace Language

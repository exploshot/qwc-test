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

#include <iostream>

#include <Utilities/ColouredMsg.h>
#include <Utilities/Input.h>

namespace Utilities {
    bool confirm(const std::string &msg)
    {
        return confirm (msg, true);
    }

    /*!
     * defaultToYes = what value we return on hitting enter, i.e. the "expected"
     * workflow
     */
    bool confirm(const std::string &msg, const bool defaultToYes)
    {
        /*!
         * In unix programs, the upper case letter indicates the default, for
         * example when you hit enter
         */
        const std::string prompt = defaultToYes ? " (Y/n): " : " (y/N): ";

        while (true) {
            std::cout
                << InformationMsg (msg + prompt);

            std::string answer;
            std::getline (std::cin, answer);

            const char c = ::tolower (answer[0]);

            switch (c) {
                /*!
                 * Lets people spam enter / choose default value
                 */
                case '\0':
                    return defaultToYes;
                case 'y':
                    return true;
                case 'n':
                    return false;
            }

            std::cout
                << WarningMsg ("Bad input: ")
                << InformationMsg (answer)
                << WarningMsg (" - please enter either Y or N.")
                << std::endl;
        }
    }
} // namespace Utilities

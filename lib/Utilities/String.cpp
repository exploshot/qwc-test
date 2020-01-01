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

#include <algorithm>
#include <sstream>

#include <Utilities/String.h>

namespace Utilities {
    /*!
     * Erases all instances of c from the string. E.g. 2,000,000 becomes 2000000
     */
    void removeCharFromString(std::string &str, const char c)
    {
        str.erase (std::remove (str.begin (), str.end (), c), str.end ());
    }

    /*!
     * Trims any whitespace from left and right
     */
    void trim(std::string &str)
    {
        rightTrim (str);
        leftTrim (str);
    }

    void leftTrim(std::string &str)
    {
        std::string whitespace = " \t\n\r\f\v";

        str.erase (0, str.find_first_not_of (whitespace));
    }

    void rightTrim(std::string &str)
    {
        std::string whitespace = " \t\n\r\f\v";

        str.erase (str.find_last_not_of (whitespace) + 1);
    }

    /*!
     * Checks if str begins with substring
     */
    bool startsWith(const std::string &str, const std::string &substring)
    {
        return str.rfind (substring, 0) == 0;
    }

    std::vector<std::string> split(const std::string &str, char delim = ' ')
    {
        std::vector<std::string> cont;
        std::stringstream ss (str);
        std::string token;

        while (std::getline (ss, token, delim)) {
            cont.push_back (token);
        }

        return cont;
    }

    std::string removePrefix(const std::string &str, const std::string &prefix)
    {
        const size_t removePos = str.rfind (prefix, 0);

        if (removePos == 0) {
            /* erase is in place */
            std::string copy = str;

            copy.erase (0, prefix.length ());

            return copy;
        }

        return str;
    }

} // namespace Utilities

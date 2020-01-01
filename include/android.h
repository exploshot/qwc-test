// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#ifdef ANDROID

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <string>

namespace std {

template <typename T>
string to_string(T value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

/*
inline unsigned long stoul (std::string const &str, size_t *idx = 0, int base = 10)
{
    char *endp;
    unsigned long value = strtoul(str.c_str(), &endp, base);
    if (endp == str.c_str()) {
        throw std::invalid_argument("my_stoul");
    }

    if (value == ULONG_MAX && errno == ERANGE) {
        throw std::out_of_range("my_stoul");
    }

    if (idx) {
        *idx = endp - str.c_str();
    }

    return value;
}
*/
} // namespace std

#endif

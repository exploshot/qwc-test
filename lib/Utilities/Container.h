// Portions Copyright (c) 2018-2019 Galaxia Project Developers
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
#include <iterator>
#include <unordered_set>
#include <vector>

namespace Utilities {

    template<typename T, typename Function>
    std::vector<T> filter(const std::vector<T> &input, Function predicate)
    {
        std::vector<T> result;

        std::copy_if (
            input.begin (), input.end (), std::back_inserter (result), predicate
        );

        return result;
    }

    /*!
     * Verify that the items in a collection are all unique
     */
    template<typename T>
    bool is_unique(T begin, T end)
    {
        std::unordered_set<typename T::value_type> set{};

        for (; begin != end; ++begin) {
            if (!set.insert (*begin).second) {
                return false;
            }
        }

        return true;
    }

} // namespace Utilities

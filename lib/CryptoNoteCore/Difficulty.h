// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
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

#include <stdint.h>
#include <vector>

uint64_t nextDifficultyV5(std::vector<uint64_t> timestamps,
                          std::vector<uint64_t> cumulativeDifficulties);

uint64_t nextDifficultyV4(std::vector<uint64_t> timestamps,
                          std::vector<uint64_t> cumulativeDifficulties);

uint64_t nextDifficultyV3(std::vector<uint64_t> timestamps,
                          std::vector<uint64_t> cumulativeDifficulties);

template<typename T>
T clamp(const T &n, const T &lower, const T &upper)
{
    return std::max (lower, std::min (n, upper));
}

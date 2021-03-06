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

#include <algorithm>

#include <CryptoNoteCore/Difficulty.h>

#include <Global/CryptoNoteConfig.h>

/*!
	LWMA-2 difficulty algorithm
	Copyright (c) 2017-2018 Zawy, MIT License
	https://github.com/zawy12/difficulty-algorithms/issues/3
*/
uint64_t nextDifficultyV5(std::vector<uint64_t> timestamps,
                          std::vector<uint64_t> cumulativeDifficulties)
{
    int64_t T = CryptoNote::parameters::DIFFICULTY_TARGET;
    int64_t N = CryptoNote::parameters::DIFFICULTY_WINDOW;
    int64_t L (0),
        ST,
        sum3ST (0),
        next_D,
        prev_D;

    /*!
        If we are starting up, returning a difficulty guess. If you are a
           new coin, you might want to set this to a decent estimate of your
           hashrate

        N =60, timeStamp size should also be 60
    */
    if (timestamps.size () < static_cast<uint64_t>(N + 1)) {
        return 50000; //50k
    }

    for (int64_t i = 1; i <= N; i++) {
        ST = static_cast<int64_t>(timestamps[i]) - static_cast<int64_t>(timestamps[i - 1]);

        ST = std::max (-4 * T, std::min (ST, 6 * T));

        L += ST * i;

        if (i > N - 3) {
            sum3ST += ST;
        }
    }

    next_D = (
                 static_cast<int64_t>(cumulativeDifficulties[N] -
                                      cumulativeDifficulties[0])
                 * T *
                 (N + 1) * 99
             ) / (100 * 2 * L);

    prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N - 1];

    next_D = std::max ((prev_D * 67) / 100, std::min (next_D, (prev_D * 150) / 100));

    if (sum3ST < (8 * T) / 10) {
        next_D = std::max (next_D, (prev_D * 108) / 100);
    }

    return static_cast<uint64_t>(next_D);
}

/*!
	LWMA-2 difficulty algorithm 
	Copyright (c) 2017-2018 Zawy, MIT License
	https://github.com/zawy12/difficulty-algorithms/issues/3
*/
uint64_t nextDifficultyV4(std::vector<uint64_t> timestamps,
                          std::vector<uint64_t> cumulativeDifficulties)
{
    int64_t T = CryptoNote::parameters::DIFFICULTY_TARGET;
    int64_t N = CryptoNote::parameters::DIFFICULTY_WINDOW_V1;
    int64_t FTL = CryptoNote::parameters::CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT;
    int64_t L (0), ST, sum3ST (0), next_D, prev_D;

    if (timestamps.size () <= static_cast<uint64_t>(N)) {
        return 1000;
    }

    for (int64_t i = 1; i <= N; i++) {
        ST = std::max (-FTL, std::min (static_cast<int64_t>(timestamps[i]) -
                                       static_cast<int64_t>(timestamps[i - 1]), 6 * T));

        L += ST * i;

        if (i > N - 3) {
            sum3ST += ST;
        }
    }

    next_D = (
                 static_cast<int64_t>(cumulativeDifficulties[N] -
                                      cumulativeDifficulties[0])
                 * T * (N + 1) * 99
             ) / (100 * 2 * L);
    prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N - 1];

    /*!
        Make sure we don't divide by zero if 50x attacker (thanks fireice)
    */
    next_D = std::max ((prev_D * 67) / 100, std::min (next_D, (prev_D * 150) / 100));

    if (sum3ST < (8 * T) / 10) {
        next_D = std::max (next_D, (prev_D * 110) / 100);
    }

    return static_cast<uint64_t>(next_D);
}

/*!
	LWMA-2 difficulty algorithm
	Copyright (c) 2017-2018 Zawy, MIT License
	https://github.com/zawy12/difficulty-algorithms/issues/3
*/
uint64_t nextDifficultyV3(std::vector<uint64_t> timestamps,
                          std::vector<uint64_t> cumulativeDifficulties)
{
    int64_t T = CryptoNote::parameters::DIFFICULTY_TARGET;
    int64_t N = CryptoNote::parameters::DIFFICULTY_WINDOW_V1;
    int64_t FTL = CryptoNote::parameters::CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT;
    int64_t L (0),
        ST,
        sum3ST (0),
        next_D,
        prev_D;

    if (timestamps.size () <= static_cast<uint64_t>(N)) {
        return 1000;
    }

    for (int64_t i = 1; i <= N; i++) {
        ST = std::max (-FTL, std::min (static_cast<int64_t>(timestamps[i]) -
                                       static_cast<int64_t>(timestamps[i - 1]), 6 * T));

        L += ST * i;

        if (i > N - 3) {
            sum3ST += ST;
        }
    }

    next_D = (
                 static_cast<int64_t>(cumulativeDifficulties[N] -
                                      cumulativeDifficulties[0])
                 * T * (N + 1) * 99
             ) / (100 * 2 * L);
    prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N - 1];

    /*!
		Make sure we don't divide by zero if 50x attacker (thanks fireice)
	*/
    next_D = std::max ((prev_D * 70) / 100, std::min (next_D, (prev_D * 107) / 100));

    if (sum3ST < (8 * T) / 10) {
        next_D = (prev_D * 110) / 100;
    }

    return static_cast<uint64_t>(next_D);
}

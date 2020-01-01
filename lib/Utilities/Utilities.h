// Copyright (c) 2018-2019, The TurtleCoin Developers
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

#include <atomic>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>

#include <CryptoNote.h>

#include <Utilities/Errors.h>

namespace Utilities {
    uint64_t getTransactionSum(const std::vector <std::pair<std::string, uint64_t>> destinations);
    uint64_t getUpperBound(const uint64_t val, const uint64_t nearestMultiple);
    uint64_t getLowerBound(const uint64_t val, const uint64_t nearestMultiple);

    bool isInputUnlocked(const uint64_t unlockTime,
                         const uint64_t currentHeight);

    uint64_t getMaxTxSize(const uint64_t currentHeight);

    void sleepUnlessStopping(
        const std::chrono::milliseconds duration,
        std::atomic<bool> &condition);

    uint64_t scanHeightToTimestamp(const uint64_t scanHeight);
    uint64_t timestampToScanHeight(const uint64_t timestamp);
    uint64_t getCurrentTimestampAdjusted();

    bool isSubtractionSafe(int64_t currentValue, uint64_t transferAmount);
    bool parseDaemonAddressFromString(std::string &host, uint16_t &port, std::string address);

    size_t getApproximateMaximumInputCount(const size_t transactionSize,
                                           const size_t outputCount,
                                           const size_t mixinCount);

    /*!
     * Verify that a + b will not overflow when added.
     * 2 positive numbers - should always get greater (or equal) when summed.
     * Any negative numbers - should always get smaller (or equal) when summed.
     */
    template<typename T>
    bool additionWillOverflow(T a, T b)
    {
        static_assert (std::is_integral<T>::value, "additionWillOverflow can only be used for integral inputs!");

        /*!
         * The result of the addition
         */
        T result = a + b;

        /*!
         * The larger number
         */
        T larger = std::max (a, b);

        /*!
         * If either number is negative, the result should be smaller than the
         * larger of the inputs. If not, it has overflowed.
         */
        if (a < 0 || b < 0) {
            return result > larger;
        }

        /*!
         * If both numbers are positive, the result should be greater than the
         * larger of the two inputs. If not, it has overflowed.
         */
        return result < larger;
    }

    template<typename T>
    bool subtractionWillOverflow(T a, T b)
    {
        /*!
         * Subtraction is just addition where the second param is negated
         */
        return additionWillOverflow (a, -b);
    }

    /*!
     * Verify the sum of the vector amounts will not overflow
     */
    template<typename T>
    bool sumWillOverflow(std::vector <T> amounts)
    {
        T sum = 0;

        /*!
         * Add each item to the sum as we go, checking if any of the additions
         * will make the total sum overflow
         */
        for (const auto item : amounts) {
            if (additionWillOverflow (item, sum)) {
                return true;
            }

            sum += item;
        }

        return false;
    }
} // namespace Utilities

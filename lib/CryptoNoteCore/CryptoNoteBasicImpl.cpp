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

#include <Common/Base58.h>
#include <Common/CryptoNoteTools.h>
#include <Common/IIntUtil.h>

#include <Crypto/Hash.h>

#include <CryptoNoteCore/CryptoNoteBasicImpl.h>
#include <CryptoNoteCore/CryptoNoteFormatUtils.h>

#include <Serialization/CryptoNoteSerialization.h>

using namespace Crypto;
using namespace Common;

namespace CryptoNote {

    uint64_t getPenalizedAmount(uint64_t amount, size_t medianSize, size_t currentBlockSize)
    {
        static_assert (sizeof (size_t) >= sizeof (uint32_t), "size_t is too small");
        assert(currentBlockSize <= 2 * medianSize);
        assert(medianSize <= std::numeric_limits<uint32_t>::max ());
        assert(currentBlockSize <= std::numeric_limits<uint32_t>::max ());

        if (amount == 0) {
            return 0;
        }

        if (currentBlockSize <= medianSize) {
            return amount;
        }

        uint64_t productHi;
        /*!
            BUGFIX by Monero Project: 32-bit saturation bug (e.g. ARM7),
            the result was being treated as 32-bit by default
        */
        uint64_t multiplicand = UINT64_C(2) * medianSize - currentBlockSize;
        multiplicand *= currentBlockSize;
        uint64_t productLo = mul128 (amount, multiplicand, &productHi);

        uint64_t penalizedAmountHi;
        uint64_t penalizedAmountLo;
        div128_32 (productHi,
                   productLo,
                   static_cast<uint32_t>(medianSize),
                   &penalizedAmountHi,
                   &penalizedAmountLo);
        div128_32 (penalizedAmountHi,
                   penalizedAmountLo,
                   static_cast<uint32_t>(medianSize),
                   &penalizedAmountHi,
                   &penalizedAmountLo);

        assert(0 == penalizedAmountHi);
        assert(penalizedAmountLo < amount);

        return penalizedAmountLo;
    }
} // namespace CryptoNote

bool parseHash256(const std::string &str_hash, Crypto::Hash &hash)
{
    return Common::podFromHex (str_hash, hash);
}

// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
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

#include <sstream>
#include <tuple>

#include <Global/CryptoNoteConfig.h>

#include <Utilities/Mixins.h>

namespace Utilities {
    /*!
     * Returns {minMixin, maxMixin, defaultMixin}
     */
    std::tuple<uint64_t, uint64_t, uint64_t> getMixinAllowableRange(const uint64_t height)
    {
        uint64_t minMixin = 0;
        uint64_t maxMixin = std::numeric_limits<uint64_t>::max ();
        uint64_t defaultMixin = CryptoNote::parameters::DEFAULT_MIXIN;

        /*!
         * We now limit the mixin allowed in a transaction. However, there have been
         * some transactions outside these limits in the past, so we need to only
         * enforce this on new blocks, otherwise wouldn't be able to sync the chain
         *
         * We also need to ensure that the mixin enforced is for the limit that
         * was correct when the block was formed - i.e. if 0 mixin was allowed at
         * block 100, but is no longer allowed - we should still validate block 100
         */

        if (height >= CryptoNote::parameters::MIXIN_LIMITS_V1_HEIGHT) {
            minMixin = CryptoNote::parameters::MINIMUM_MIXIN;
            maxMixin = CryptoNote::parameters::MAXIMUM_MIXIN;
            defaultMixin = CryptoNote::parameters::DEFAULT_MIXIN;
        } else {
            minMixin = CryptoNote::parameters::MINIMUM_MIXIN_V1;
            maxMixin = CryptoNote::parameters::MAXIMUM_MIXIN_V1;
            defaultMixin = CryptoNote::parameters::DEFAULT_MIXIN_V1;
        }

        return {minMixin, maxMixin, defaultMixin};
    }
} // namespace Utilities

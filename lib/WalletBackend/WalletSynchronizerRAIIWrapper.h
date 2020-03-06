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

#pragma once

#include <WalletBackend/WalletSynchronizer.h>

class WalletSynchronizerRAIIWrapper
{
public:
    WalletSynchronizerRAIIWrapper(const std::shared_ptr <WalletSynchronizer> walletSynchronizer)
        : m_walletSynchronizer (walletSynchronizer)
    {
    };

    template<typename T>
    auto pauseSynchronizerToRunFunction(T func)
    {
        /*!
         * Can only perform one operation with the synchronizer stopped at
         * once
         */
        std::scoped_lock lock (m_mutex);

        /*!
         * Stop the synchronizer
         */
        if (m_walletSynchronizer != nullptr) {
            m_walletSynchronizer->stop ();
        }

        /*!
         * Run the function, now safe
         */
        auto result = func ();

        /*!
         * Start the synchronizer again
         */
        if (m_walletSynchronizer != nullptr) {
            m_walletSynchronizer->start ();
        }

        /*!
         * Return the extracted value
         */
        return result;
    }

private:
    std::shared_ptr <WalletSynchronizer> m_walletSynchronizer;

    std::mutex m_mutex;
};

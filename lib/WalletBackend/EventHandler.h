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

#include <functional>

template<typename T>
class Event
{
public:
    void subscribe(std::function<void(T)> function)
    {
        m_function = function;
    }

    void unsubscribe()
    {
        m_function = {};
    }

    void pause()
    {
        m_paused = true;
    }

    void resume()
    {
        m_paused = false;
    }

    void fire(T args)
    {
        /*!
         * If we have a function to run, and we're not ignoring events
         */
        if (m_function && !m_paused) {
            /*!
             * Launch the function, and return instantly. This way we
             * can have multiple functions running at once.
             *
             * If you use std::cout in your function, you may experience
             * interleaved characters when printing. To resolve this,
             * consider using std::osyncstream.
             * Further reading: https://stackoverflow.com/q/14718124/8737306
             */
            std::thread (m_function, args).detach ();
        }
    }

private:
    std::function<void(T)> m_function;

    bool m_paused = false;
};

class EventHandler
{
public:
    Event<uint64_t> onSynced;

    Event<WalletTypes::Transaction> onTransaction;
};

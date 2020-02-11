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

#include <chrono>

namespace System {

    class Dispatcher;

    class Timer
    {
    public:
        Timer();
        explicit Timer(Dispatcher &dispatcher);
        Timer(const Timer &) = delete;
        Timer(Timer &&other);
        ~Timer();

        void sleep(std::chrono::nanoseconds duration);

        Timer &operator=(const Timer &) = delete;
        Timer &operator=(Timer &&other);

    private:
        Dispatcher *dispatcher;
        void *context;
        int timer;
    };

} // namespace System

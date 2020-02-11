// Please see the included LICENSE file for more information.
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

#include <string>

namespace google_breakpad {
    class ExceptionHandler;
} // namespace google_breakpad

namespace Qwertycoin {

    namespace Breakpad {

        class ExceptionHandler
        {
        public:
            explicit ExceptionHandler(const std::string &dumpPath = std::string());
            virtual ~ExceptionHandler();

            static void dummyCrash();

        private:
            google_breakpad::ExceptionHandler *m_exceptionHandler = nullptr;
        };

    } // namespace Breakpad

} // namespace Qwertycoin

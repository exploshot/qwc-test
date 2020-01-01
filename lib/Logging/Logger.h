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
#include <string>
#include <vector>

namespace Logger {
    enum LogLevel
    {
        DEBUG = 4,
        INFO = 3,
        WARNING = 2,
        FATAL = 1,
        DISABLED = 0,
    };

    enum LogCategory
    {
        SYNC,
        TRANSACTIONS,
        FILESYSTEM,
        SAVE,
        DAEMON,
        SYSTEM
    };

    std::string logLevelToString(const LogLevel level);

    LogLevel stringToLogLevel(std::string level);

    std::string logCategoryToString(const LogCategory category);

    class Logger
    {
    public:
        Logger()
        {
        };

        void log(
            const std::string message,
            const LogLevel level,
            const std::vector<LogCategory> categories) const;

        void setLogLevel(const LogLevel level);

        void setLogCallback(
            std::function<void(
                const std::string prettyMessage,
                const std::string message,
                const LogLevel level,
                const std::vector<LogCategory> categories)> callback);

    private:
        /*!
         * Logging disabled by default
         */
        LogLevel m_logLevel = DISABLED;

        std::function<void(
            const std::string prettyMessage,
            const std::string message,
            const LogLevel level,
            const std::vector<LogCategory> categories)> m_callback;
    };

    /*!
     * Global logger instance
     */
    extern Logger logger;
} // namespace Logging

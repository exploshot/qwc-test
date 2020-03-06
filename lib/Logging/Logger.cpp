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

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <Logging/Logger.h>

namespace Logger {
    Logger logger;

    std::string logLevelToString(const LogLevel level)
    {
        switch (level) {
            case DISABLED: {
                return "Disabled";
            }
            case DEBUG: {
                return "Debug";
            }
            case INFO: {
                return "Info";
            }
            case WARNING: {
                return "Warning";
            }
            case FATAL: {
                return "Fatal";
            }
            default:
                throw std::invalid_argument ("Invalid log level given");
        }
    }

    LogLevel stringToLogLevel(std::string level)
    {
        /*!
         * Convert to lower case
         */
        std::transform (level.begin (), level.end (), level.begin (),
                        ::tolower);

        if (level == "disabled") {
            return DISABLED;
        } else if (level == "debug") {
            return DEBUG;
        } else if (level == "info") {
            return INFO;
        } else if (level == "warning") {
            return WARNING;
        } else if (level == "fatal") {
            return FATAL;
        }

        throw std::invalid_argument ("Invalid log level given");
    }

    std::string logCategoryToString(const LogCategory category)
    {
        switch (category) {
            case SYNC: {
                return "Sync";
            }
            case TRANSACTIONS: {
                return "Transactions";
            }
            case FILESYSTEM: {
                return "Filesystem";
            }
            case SAVE: {
                return "Save";
            }
            case DAEMON: {
                return "Daemon";
            }
            default:
                throw std::invalid_argument ("Invalid log category given");
        }
    }

    void Logger::log(const std::string message,
                     const LogLevel level,
                     const std::vector <LogCategory> categories) const
    {
        if (level == DISABLED) {
            return;
        }

        const std::time_t now = std::time (nullptr);

        std::stringstream output;

        output
            << "["
            << std::put_time (std::localtime (&now), "%H:%M:%S")
            << "] "
            << "["
            << logLevelToString (level)
            << "]";

        for (const auto &category : categories) {
            output
                << " ["
                << logCategoryToString (category)
                << "]";
        }

        output
            << ": "
            << message;

        if (level <= m_logLevel) {
            /*!
             * If the user provides a callback, log to that instead
             */
            if (m_callback) {
                m_callback (output.str (), message, level, categories);
            } else {
                std::cout
                    << output.str ()
                    << std::endl;
            }
        }
    }

    void Logger::setLogLevel(const LogLevel level)
    {
        m_logLevel = level;
    }

    void Logger::setLogCallback(
        std::function<void(
            const std::string prettyMessage,
            const std::string message,
            const LogLevel level,
            const std::vector <LogCategory> categories)> callback)
    {
        m_callback = callback;
    }
} // namespace Logging

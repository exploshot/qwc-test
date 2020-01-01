// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace CryptoNote {

    class DataBaseConfig
    {
    public:
        DataBaseConfig();
        bool init(const std::string dataDirectory,
                  const int backgroundThreads,
                  const int maxOpenFiles,
                  const int writeBufferSizeMB,
                  const int readCacheSizeMB
        );

        bool isConfigFolderDefaulted() const;
        std::string getDataDir() const;
        uint16_t getBackgroundThreadsCount() const;
        uint32_t getMaxOpenFiles() const;
        uint64_t getWriteBufferSize() const; //Bytes
        uint64_t getReadCacheSize() const; //Bytes
        bool getTestnet() const;

    private:
        bool configFolderDefaulted;
        std::string dataDir;
        uint16_t backgroundThreadsCount;
        uint32_t maxOpenFiles;
        uint64_t writeBufferSize;
        uint64_t readCacheSize;
        bool testnet;
    };
} //namespace CryptoNote
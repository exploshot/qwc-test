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

#include <CryptoNoteCore/Blockchain/BlockchainCache.h>
#include <CryptoNoteCore/Blockchain/IBlockchainCacheFactory.h>

namespace CryptoNote {

    class MemoryBlockchainCacheFactory: public IBlockchainCacheFactory
    {
    public:
        MemoryBlockchainCacheFactory(const std::string &filename,
                                     std::shared_ptr<Logging::ILogger> logger);
        virtual ~MemoryBlockchainCacheFactory() override;

        std::unique_ptr<IBlockchainCache> createRootBlockchainCache(const Currency &currency) override;
        std::unique_ptr<IBlockchainCache> createBlockchainCache(const Currency &currency,
                                                                IBlockchainCache *parent,
                                                                uint32_t startIndex = 0) override;

    private:
        std::string filename;
        std::shared_ptr<Logging::ILogger> logger;
    };
} //namespace CryptoNote

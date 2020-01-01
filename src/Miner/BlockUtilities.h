// Copyright (c) 2019, The TurtleCoin Developers
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

#include <CryptoNote.h>
#include <CryptoTypes.h>

#include <cstdint>

#include <vector>

std::vector<uint8_t> getParentBlockBinaryArray(const CryptoNote::BlockTemplate &block, const bool headerOnly);
std::vector<uint8_t> getParentBlockHashingBinaryArray(const CryptoNote::BlockTemplate &block, const bool headerOnly);

std::vector<uint8_t>
getParentBinaryArray(const CryptoNote::BlockTemplate &block, const bool hashTransaction, const bool headerOnly);
std::vector<uint8_t> getBlockHashingBinaryArray(const CryptoNote::BlockTemplate &block);

Crypto::Hash getBlockHash(const CryptoNote::BlockTemplate &block);
Crypto::Hash getMerkleRoot(const CryptoNote::BlockTemplate &block);
Crypto::Hash getBlockLongHash(const CryptoNote::BlockTemplate &block);
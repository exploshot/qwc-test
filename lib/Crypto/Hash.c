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

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <Crypto/HashOps.h>
#include <Crypto/Keccak.h>

void hashPermutation(union hashState *state)
{
    keccakf ((uint64_t *) state, 24);
}

void hashProcess(union hashState *state, const uint8_t *buf, size_t count)
{
    keccak1600 (buf, (int) count, (uint8_t *) state);
}

void CnFastHash(const void *data, size_t length, char *hash)
{
    union hashState state;
    hashProcess (&state, data, length);
    memcpy (hash, &state, HASH_SIZE);
}

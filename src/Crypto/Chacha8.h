// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018-2019, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.#pragma once

#pragma once

#include <string>

#include <Crypto/Hash.h>
#include <Crypto/Random.h>

constexpr inline int CHACHA8_KEY_SIZE = 32;
constexpr inline int CHACHA8_IV_SIZE = 8;

namespace Crypto
{
    void chacha8(const void *data, 
                 size_t length, 
                 const uint8_t *key, 
                 const uint8_t *iv, 
                 char *cipher);

    #pragma pack(push, 1)
    struct chacha8Key
    {
        uint8_t data[CHACHA8_KEY_SIZE];
    };

    struct chacha8IV
    {
        uint8_t data[CHACHA8_IV_SIZE];
    };
    #pragma pack(pop)

    static_assert(sizeof(chacha8Key) == CHACHA8_KEY_SIZE && sizeof(chacha8IV) 
                                      == CHACHA8_IV_SIZE, "Invalid structure size");


    inline void chacha8(const void *data, 
                        size_t length, 
                        const chacha8Key &key, 
                        const chacha8IV &iv, 
                        char *cipher)
    {
        chacha8(data, 
                length, 
                reinterpret_cast<const uint8_t *>(&key), 
                reinterpret_cast<const uint8_t *>(&iv), 
                cipher);
    }

    inline void generateChacha8Key(const std::string &password, chacha8Key &key)
    {
        static_assert(sizeof(chacha8Key) <= sizeof(Hash), "Size of hash must be at least that of chacha8Key");
        Hash pwdHash;
        CnSlowHashV0(password.data(), password.size(), pwdHash);
        memcpy(&key, &pwdHash, sizeof(key));
        memset(&pwdHash, 0, sizeof(pwdHash));
    }

    /**
     * Generates a random chacha8 IV
     */
    inline chacha8IV randomChachaIV()
    {
        chacha8IV result;
        Random::randomBytes(CHACHA8_IV_SIZE, result.data);
        return result;
    }
}

// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2014-2018, The Aeon Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

/* This file contains the portable version of the slow-hash routines
   for the CryptoNight hashing algorithm */

#if !(!defined NO_AES && (defined(__arm__) || defined(__aarch64__))) && \
    !(!defined NO_AES && (defined(__x86_64__) || \
    (defined(_MSC_VER) && defined(_WIN64))))
#pragma message ("info: Using SlowHashPortable.c")

#include <Crypto/SlowHashCommon.h>

void slowHashAllocateState(void)
{
    // Do nothing, this is just to maintain compatibility with the upgraded slow-hash.c
    return;
}

void slowHashFreeState(void)
{
    // As above
    return;
}

    #if defined(__GNUC__)
        #define RDATA_ALIGN16 __attribute__ ((aligned(16)))
        #define STATIC static
        #define INLINE inline
    #else /* defined(__GNUC__) */
        #define RDATA_ALIGN16
        #define STATIC static
        #define INLINE
    #endif /* defined(__GNUC__) */

    #define U64(x) ((uint64_t *) (x))

static void (*const extraHashes[4])(const void *, size_t, char *) =
{
    hashExtraBlake, hashExtraGroestl, hashExtraJh, hashExtraSkein
};

extern void aesbSingleRound(const uint8_t *in, uint8_t*out, const uint8_t *expandedKey);
extern void aesbPseudoRound(const uint8_t *in, uint8_t *out, const uint8_t *expandedKey);

static void mul(const uint8_t* a, const uint8_t* b, uint8_t* res)
{
    uint64_t a0, b0;
    uint64_t hi, lo;

    a0 = SWAP64LE(((uint64_t*)a)[0]);
    b0 = SWAP64LE(((uint64_t*)b)[0]);
    lo = mul128(a0, b0, &hi);
    ((uint64_t*)res)[0] = SWAP64LE(hi);
    ((uint64_t*)res)[1] = SWAP64LE(lo);
}

static void sumHalfBlocks(uint8_t* a, const uint8_t* b)
{
    uint64_t a0, a1, b0, b1;

    a0 = SWAP64LE(((uint64_t*)a)[0]);
    a1 = SWAP64LE(((uint64_t*)a)[1]);
    b0 = SWAP64LE(((uint64_t*)b)[0]);
    b1 = SWAP64LE(((uint64_t*)b)[1]);
    a0 += b0;
    a1 += b1;
    ((uint64_t*)a)[0] = SWAP64LE(a0);
    ((uint64_t*)a)[1] = SWAP64LE(a1);
}

static void copyBlock(uint8_t* dst, const uint8_t* src)
{
    memcpy(dst, src, AES_BLOCK_SIZE);
}

static void swapBlocks(uint8_t *a, uint8_t *b)
{
    uint64_t t[2];
    U64(t)[0] = U64(a)[0];
    U64(t)[1] = U64(a)[1];
    U64(a)[0] = U64(b)[0];
    U64(a)[1] = U64(b)[1];
    U64(b)[0] = U64(t)[0];
    U64(b)[1] = U64(t)[1];
}

static void xorBlocks(uint8_t* a, const uint8_t* b)
{
    size_t i;
    for (i = 0; i < AES_BLOCK_SIZE; i++)
    {
        a[i] ^= b[i];
    }
}

static void xor64(uint8_t* left, const uint8_t* right)
{
    size_t i;
    for (i = 0; i < 8; ++i)
    {
        left[i] ^= right[i];
    }
}

void CnSlowHash(const void *data,
                size_t length,
                char *hash,
                int light,
                int variant,
                int prehashed,
                uint32_t pageSize,
                uint32_t scratchpad,
                uint32_t iterations)
{
    uint32_t initRounds = (scratchpad / INIT_SIZE_BYTE);
    uint32_t aesRounds = (iterations / 2);
    size_t lightFlag = (light ? 2: 1);

    uint8_t text[INIT_SIZE_BYTE];
    uint8_t a[AES_BLOCK_SIZE];
    uint8_t b[AES_BLOCK_SIZE * 2];
    uint8_t c[AES_BLOCK_SIZE];
    uint8_t c1[AES_BLOCK_SIZE];
    uint8_t d[AES_BLOCK_SIZE];
    RDATA_ALIGN16 uint8_t expandedKey[256];

    union cnSlowHashState state;

    size_t i, j;
    uint8_t *p = NULL;
    oAesCtx *aesCtx;

    static void (*const extraHashes[4])(const void *, size_t, char *) =
    {
        hashExtraBlake, hashExtraGroestl, hashExtraJh, hashExtraSkein
    };

    #ifndef FORCE_USE_HEAP
    uint8_t long_state[pageSize];
    #else /* FORCE_USE_HEAP */
        #pragma message ("warning: ACTIVATING FORCE_USE_HEAP IN SlowHashPortable.c")
    uint8_t *long_state = (uint8_t *)malloc(pageSize);
    #endif /* FORCE_USE_HEAP */

    if (prehashed) {
        memcpy(&state.hs, data, length);
    } else {
        hashProcess(&state.hs, data, length);
    }

    memcpy(text, state.init, INIT_SIZE_BYTE);

    aesCtx = (oAesCtx *) oAesAlloc();
    oAesKeyImportData(aesCtx, state.hs.b, AES_KEY_SIZE);

    VARIANT1_PORTABLE_INIT();
    VARIANT2_PORTABLE_INIT();

    // use aligned data
    memcpy(expandedKey, aesCtx->key->expData, aesCtx->key->expDataLen);

    for(i = 0; i < initRounds; i++) {
        for(j = 0; j < INIT_SIZE_BLK; j++) {
            aesbPseudoRound(&text[AES_BLOCK_SIZE * j], 
                            &text[AES_BLOCK_SIZE * j], expandedKey);
        }
        memcpy(&long_state[i * INIT_SIZE_BYTE], text, INIT_SIZE_BYTE);
    }

    U64(a)[0] = U64(&state.k[0])[0] ^ U64(&state.k[32])[0];
    U64(a)[1] = U64(&state.k[0])[1] ^ U64(&state.k[32])[1];
    U64(b)[0] = U64(&state.k[16])[0] ^ U64(&state.k[48])[0];
    U64(b)[1] = U64(&state.k[16])[1] ^ U64(&state.k[48])[1];

    for(i = 0; i < aesRounds; i++) {
    #define MASK(div) ((uint32_t)(((pageSize / AES_BLOCK_SIZE) / (div) - 1) << 4))
    #define state_index(x,div) ((*(uint32_t *) x) & MASK(div))

        // Iteration 1
        j = state_index(a,lightFlag);
        p = &long_state[j];
        aesbSingleRound(p, p, a);
        copyBlock(c1, p);

        VARIANT2_PORTABLE_SHUFFLE_ADD(long_state, j);
        xorBlocks(p, b);
        VARIANT1_1(p);

        // Iteration 2
        j = state_index(c1,lightFlag);
        p = &long_state[j];
        copyBlock(c, p);

        VARIANT2_PORTABLE_INTEGER_MATH(c, c1);
        mul(c1, c, d);
        VARIANT2_2_PORTABLE();
        VARIANT2_PORTABLE_SHUFFLE_ADD(long_state, j);
        sumHalfBlocks(a, d);
        swapBlocks(a, c);
        xorBlocks(a, c);
        VARIANT1_2(c + 8);
        copyBlock(p, c);

        if (variant >= 2) {
            copyBlock(b + AES_BLOCK_SIZE, b);
        }

        copyBlock(b, c1);
    }

    memcpy(text, state.init, INIT_SIZE_BYTE);
    oAesKeyImportData(aesCtx, &state.hs.b[32], AES_KEY_SIZE);
    memcpy(expandedKey, aesCtx->key->expData, aesCtx->key->expDataLen);

    for(i = 0; i < initRounds; i++) {
        for(j = 0; j < INIT_SIZE_BLK; j++) {
            xorBlocks(&text[j * AES_BLOCK_SIZE], &long_state[i * INIT_SIZE_BYTE + j * AES_BLOCK_SIZE]);
            aesbPseudoRound(&text[AES_BLOCK_SIZE * j], &text[AES_BLOCK_SIZE * j], expandedKey);
        }
    }

    oAesFree((OAES_CTX **) &aesCtx);
    memcpy(state.init, text, INIT_SIZE_BYTE);
    hashPermutation(&state.hs);
    extraHashes[state.hs.b[0] & 3](&state, 200, hash);
    oAesFree((OAES_CTX **) &aesCtx);

    #ifdef FORCE_USE_HEAP
    free(long_state);
    #endif /* FORCE_USE_HEAP */
}

#endif

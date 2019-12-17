// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2014-2018, The Aeon Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

/* This file contains common CryptoNight information including
   the definitions of variants, block sizes, etc */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <Common/IIntUtil.h>

#include <Crypto/HashOps.h>
#include <Crypto/OAesLib.h>
#include <Crypto/Variant2IntSqrt.h>

// Standard Crypto Definitions
#define AES_BLOCK_SIZE         16
#define AES_KEY_SIZE           32
#define INIT_SIZE_BLK          8
#define INIT_SIZE_BYTE         (INIT_SIZE_BLK * AES_BLOCK_SIZE)

extern void aesbSingleRound(const uint8_t *in, uint8_t *out, const uint8_t *expandedKey);
extern void aesbPseudoRound(const uint8_t *in, uint8_t *out, const uint8_t *expandedKey);

#pragma pack(push, 1)
union cnSlowHashState
{
    union hashState hs;
    struct
    {
        uint8_t k[64];
        uint8_t init[INIT_SIZE_BYTE];
    };
};
#pragma pack(pop)

#define VARIANT1_1(p) \
    do if (variant == 1) \
    { \
        const uint8_t tmp = ((const uint8_t*)(p))[11]; \
        static const uint32_t table = 0x75310; \
        const uint8_t index = (((tmp >> 3) & 6) | (tmp & 1)) << 1; \
        ((uint8_t*)(p))[11] = tmp ^ ((table >> index) & 0x30); \
    } while(0)

#define VARIANT1_2(p) \
    do if (variant == 1) \
    { \
        xor64(p, tweak12); \
    } while(0)

#define VARIANT1_CHECK() \
    do if (length < 43) \
    { \
        fprintf(stderr, "Cryptonight variant 1 need at least 43 bytes of data"); \
        abort(); \
    } while(0)

#define NONCE_POINTER (((const uint8_t*)data)+35)

#define VARIANT1_PORTABLE_INIT() \
    uint8_t tweak12[8]; \
    do if (variant == 1) \
    { \
        VARIANT1_CHECK(); \
        memcpy(&tweak12, &state.hs.b[192], sizeof(tweak12)); \
        xor64(tweak12, NONCE_POINTER); \
    } while(0)

#define VARIANT1_INIT64() \
    if (variant == 1) \
    { \
        VARIANT1_CHECK(); \
    } \
    const uint64_t tweak12 = (variant == 1) ? (state.hs.w[24] ^ (*((const uint64_t*)NONCE_POINTER))) : 0

#define VARIANT2_INIT64() \
    uint64_t divisionResult = 0; \
    uint64_t sqrtResult = 0; \
    do if (variant == 2) \
    { \
        U64(b)[2] = state.hs.w[8] ^ state.hs.w[10]; \
        U64(b)[3] = state.hs.w[9] ^ state.hs.w[11]; \
        divisionResult = state.hs.w[12]; \
        sqrtResult = state.hs.w[13]; \
    } while (0)

#define VARIANT2_PORTABLE_INIT() \
    uint64_t divisionResult = 0; \
    uint64_t sqrtResult = 0; \
    do if (variant == 2) \
    { \
        memcpy(b + AES_BLOCK_SIZE, state.hs.b + 64, AES_BLOCK_SIZE); \
        xor64(b + AES_BLOCK_SIZE, state.hs.b + 80); \
        xor64(b + AES_BLOCK_SIZE + 8, state.hs.b + 88); \
        divisionResult = state.hs.w[12]; \
        sqrtResult = state.hs.w[13]; \
    } while (0)

#define VARIANT2_SHUFFLE_ADD_SSE2(base_ptr, offset) \
    do if (variant == 2) \
    { \
        const __m128i chunk1 = _mm_load_si128((__m128i *)((base_ptr) + ((offset) ^ 0x10))); \
        const __m128i chunk2 = _mm_load_si128((__m128i *)((base_ptr) + ((offset) ^ 0x20))); \
        const __m128i chunk3 = _mm_load_si128((__m128i *)((base_ptr) + ((offset) ^ 0x30))); \
        _mm_store_si128((__m128i *)((base_ptr) + ((offset) ^ 0x10)), _mm_add_epi64(chunk3, _b1)); \
        _mm_store_si128((__m128i *)((base_ptr) + ((offset) ^ 0x20)), _mm_add_epi64(chunk1, _b)); \
        _mm_store_si128((__m128i *)((base_ptr) + ((offset) ^ 0x30)), _mm_add_epi64(chunk2, _a)); \
    } while (0)

#define VARIANT2_SHUFFLE_ADD_NEON(base_ptr, offset) \
    do if (variant == 2) \
    { \
        const uint64x2_t chunk1 = vld1q_u64(U64((base_ptr) + ((offset) ^ 0x10))); \
        const uint64x2_t chunk2 = vld1q_u64(U64((base_ptr) + ((offset) ^ 0x20))); \
        const uint64x2_t chunk3 = vld1q_u64(U64((base_ptr) + ((offset) ^ 0x30))); \
        vst1q_u64(U64((base_ptr) + ((offset) ^ 0x10)), vaddq_u64(chunk3, vreinterpretq_u64_u8(_b1))); \
        vst1q_u64(U64((base_ptr) + ((offset) ^ 0x20)), vaddq_u64(chunk1, vreinterpretq_u64_u8(_b))); \
        vst1q_u64(U64((base_ptr) + ((offset) ^ 0x30)), vaddq_u64(chunk2, vreinterpretq_u64_u8(_a))); \
    } while (0)

#define VARIANT2_PORTABLE_SHUFFLE_ADD(base_ptr, offset) \
    do if (variant == 2) \
    { \
        uint64_t* chunk1 = U64((base_ptr) + ((offset) ^ 0x10)); \
        uint64_t* chunk2 = U64((base_ptr) + ((offset) ^ 0x20)); \
        uint64_t* chunk3 = U64((base_ptr) + ((offset) ^ 0x30)); \
        \
        const uint64_t chunk1_old[2] = { chunk1[0], chunk1[1] }; \
        \
        uint64_t b1[2]; \
        memcpy(b1, b + 16, 16); \
        chunk1[0] = chunk3[0] + b1[0]; \
        chunk1[1] = chunk3[1] + b1[1]; \
        \
        uint64_t a0[2]; \
        memcpy(a0, a, 16); \
        chunk3[0] = chunk2[0] + a0[0]; \
        chunk3[1] = chunk2[1] + a0[1]; \
        \
        uint64_t b0[2]; \
        memcpy(b0, b, 16); \
        chunk2[0] = chunk1_old[0] + b0[0]; \
        chunk2[1] = chunk1_old[1] + b0[1]; \
    } while (0)

#define VARIANT2_INTEGER_MATH_DIVISION_STEP(b, ptr) \
    ((uint64_t*)(b))[0] ^= divisionResult ^ (sqrtResult << 32); \
    { \
        const uint64_t dividend = ((uint64_t*)(ptr))[1]; \
        const uint32_t divisor = (((uint64_t*)(ptr))[0] + (uint32_t)(sqrtResult << 1)) | 0x80000001UL; \
        divisionResult = ((uint32_t)(dividend / divisor)) + \
                        (((uint64_t)(dividend % divisor)) << 32); \
    } \
    const uint64_t sqrt_input = ((uint64_t*)(ptr))[0] + divisionResult

#define VARIANT2_INTEGER_MATH_SSE2(b, ptr) \
    do if (variant == 2) \
    { \
        VARIANT2_INTEGER_MATH_DIVISION_STEP(b, ptr); \
        VARIANT2_INTEGER_MATH_SQRT_STEP_SSE2(); \
        VARIANT2_INTEGER_MATH_SQRT_FIXUP(sqrtResult); \
    } while(0)

#if defined DBL_MANT_DIG && (DBL_MANT_DIG >= 50)
// double precision floating point type has enough bits of precision on current platform
    #define VARIANT2_PORTABLE_INTEGER_MATH(b, ptr) \
    do if (variant == 2) \
    { \
        VARIANT2_INTEGER_MATH_DIVISION_STEP(b, ptr); \
        VARIANT2_INTEGER_MATH_SQRT_STEP_FP64(); \
        VARIANT2_INTEGER_MATH_SQRT_FIXUP(sqrtResult); \
    } while (0)
#else
// double precision floating point type is not good enough on current platform
// fall back to the reference code (integer only)
    #define VARIANT2_PORTABLE_INTEGER_MATH(b, ptr) \
    do if (variant == 2) \
    { \
        VARIANT2_INTEGER_MATH_DIVISION_STEP(b, ptr); \
        VARIANT2_INTEGER_MATH_SQRT_STEP_REF(); \
    } while (0)
#endif

#define VARIANT2_2_PORTABLE() \
    if (variant == 2) { \
        xorBlocks(long_state + (j ^ 0x10), d); \
        xorBlocks(d, long_state + (j ^ 0x20)); \
    }

#define VARIANT2_2() \
    do if (variant == 2) \
    { \
        *U64(hpState + (j ^ 0x10)) ^= hi; \
        *(U64(hpState + (j ^ 0x10)) + 1) ^= lo; \
        hi ^= *U64(hpState + (j ^ 0x20)); \
        lo ^= *(U64(hpState + (j ^ 0x20)) + 1); \
    } while (0)

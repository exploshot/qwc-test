// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2016-2018, The Karbowanec developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

#include <alloca.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>

#include <Common/Varint.h>

#include <Crypto/Crypto.h>
#include <Crypto/Hash.h>
#include <Crypto/Random.h>

namespace Crypto {

    extern "C" {
        #include <Crypto/Keccak.h>
        #include <Crypto/CryptoOps.h>
    }

    static inline void randomScalar(EllipticCurveScalar &res)
    {
        unsigned char tmp[64];
        Random::randomBytes (64, tmp);
        scReduce (tmp);
        memcpy (&res, tmp, 32);
    }

    static inline void hashToScalar(const void *data,
                                    size_t length,
                                    EllipticCurveScalar &res)
    {
        CnFastHash (data, length, reinterpret_cast<Hash &>(res));
        scReduce32 (reinterpret_cast<unsigned char *>(&res));
    }

    void CryptoOps::generateKeys(PublicKey &pub, SecretKey &sec)
    {
        geP3 point;
        randomScalar (reinterpret_cast<EllipticCurveScalar &>(sec));
        geScalarmultBase (&point, reinterpret_cast<unsigned char *>(&sec));
        geP3ToBytes (reinterpret_cast<unsigned char *>(&pub), &point);
    }

    void CryptoOps::generateDeterministicKeys(PublicKey &pub,
                                              SecretKey &sec,
                                              SecretKey &second)
    {
        geP3 point;
        sec = second;
        // reduce in case second round of keys (sendkeys)
        scReduce32 (reinterpret_cast<unsigned char *>(&sec));
        geScalarmultBase (&point, reinterpret_cast<unsigned char *>(&sec));
        geP3ToBytes (reinterpret_cast<unsigned char *>(&pub), &point);
    }

    SecretKey CryptoOps::generateMKeys(PublicKey &pub,
                                       SecretKey &sec,
                                       const SecretKey &recoveryKey,
                                       bool recover)
    {
        geP3 point;
        SecretKey rng;
        if (recover) {
            rng = recoveryKey;
        } else {
            randomScalar (reinterpret_cast<EllipticCurveScalar &>(rng));
        }

        sec = rng;
        // reduce in case second round of keys (sendkeys)
        scReduce32 (reinterpret_cast<unsigned char *>(&sec));
        geScalarmultBase (&point, reinterpret_cast<unsigned char *>(&sec));
        geP3ToBytes (reinterpret_cast<unsigned char *>(&pub), &point);

        return rng;
    }

    bool CryptoOps::checkKey(const PublicKey &key)
    {
        geP3 point;

        return geFromBytesVartime (&point, reinterpret_cast<const unsigned char *>(&key)) == 0;
    }

    bool CryptoOps::secretKeyToPublicKey(const SecretKey &sec, PublicKey &pub)
    {
        geP3 point;
        if (scCheck (reinterpret_cast<const unsigned char *>(&sec)) != 0) {
            return false;
        }
        geScalarmultBase (&point, reinterpret_cast<const unsigned char *>(&sec));
        geP3ToBytes (reinterpret_cast<unsigned char *>(&pub), &point);

        return true;
    }

    bool CryptoOps::generateKeyDerivation(const PublicKey &key1,
                                          const SecretKey &key2,
                                          KeyDerivation &derivation)
    {
        geP3 point;
        geP2 point2;
        geP1P1 point3;
        assert(scCheck (reinterpret_cast<const unsigned char *>(&key2)) == 0);

        if (geFromBytesVartime (&point, reinterpret_cast<const unsigned char *>(&key1)) != 0) {
            return false;
        }

        geScalarmult (&point2, reinterpret_cast<const unsigned char *>(&key2), &point);
        geMul8 (&point3, &point2);
        geP1P1ToP2 (&point2, &point3);
        geToBytes (reinterpret_cast<unsigned char *>(&derivation), &point2);

        return true;
    }

    static void derivationToScalar(const KeyDerivation &derivation,
                                   size_t outputIndex,
                                   EllipticCurveScalar &res)
    {
        struct
        {
            KeyDerivation derivation;
            char outputIndex[(sizeof (size_t) * 8 + 6) / 7];
        } buf;

        char *end = buf.outputIndex;
        buf.derivation = derivation;
        Tools::writeVarint (end, outputIndex);
        assert(end <= buf.outputIndex + sizeof buf.outputIndex);
        hashToScalar (&buf, end - reinterpret_cast<char *>(&buf), res);
    }

    static void derivationToScalar(const KeyDerivation &derivation,
                                   size_t outputIndex,
                                   const uint8_t *suffix,
                                   size_t suffixLength,
                                   EllipticCurveScalar &res)
    {
        assert(suffixLength <= 32);

        struct
        {
            KeyDerivation derivation;
            char outputIndex[(sizeof (size_t) * 8 + 6) / 7 + 32];
        } buf;

        char *end = buf.outputIndex;
        buf.derivation = derivation;
        Tools::writeVarint (end, outputIndex);
        assert(end <= buf.outputIndex + sizeof buf.outputIndex);
        size_t bufSize = end - reinterpret_cast<char *>(&buf);
        memcpy (end, suffix, suffixLength);
        hashToScalar (&buf, bufSize + suffixLength, res);
    }

    bool CryptoOps::derivePublicKey(const KeyDerivation &derivation,
                                    size_t outputIndex,
                                    const PublicKey &base,
                                    PublicKey &derivedKey)
    {
        EllipticCurveScalar scalar;
        geP3 point1;
        geP3 point2;
        geCached point3;
        geP1P1 point4;
        geP2 point5;

        if (geFromBytesVartime (&point1,
                                reinterpret_cast<const unsigned char *>(&base)) != 0) {
            return false;
        }

        derivationToScalar (derivation, outputIndex, scalar);
        geScalarmultBase (&point2, reinterpret_cast<unsigned char *>(&scalar));
        geP3ToCached (&point3, &point2);
        geAdd (&point4, &point1, &point3);
        geP1P1ToP2 (&point5, &point4);
        geToBytes (reinterpret_cast<unsigned char *>(&derivedKey), &point5);

        return true;
    }

    bool CryptoOps::derivePublicKey(const KeyDerivation &derivation,
                                    size_t outputIndex,
                                    const PublicKey &base,
                                    const uint8_t *suffix,
                                    size_t suffixLength,
                                    PublicKey &derivedKey)
    {
        EllipticCurveScalar scalar;
        geP3 point1;
        geP3 point2;
        geCached point3;
        geP1P1 point4;
        geP2 point5;

        if (geFromBytesVartime (&point1,
                                reinterpret_cast<const unsigned char *>(&base)) != 0) {
            return false;
        }

        derivationToScalar (derivation, outputIndex, suffix, suffixLength, scalar);
        geScalarmultBase (&point2, reinterpret_cast<unsigned char *>(&scalar));
        geP3ToCached (&point3, &point2);
        geAdd (&point4, &point1, &point3);
        geP1P1ToP2 (&point5, &point4);
        geToBytes (reinterpret_cast<unsigned char *>(&derivedKey), &point5);

        return true;
    }

    bool CryptoOps::underivePublicKeyAndGetScalar(const KeyDerivation &derivation,
                                                  size_t outputIndex,
                                                  const PublicKey &derivedKey,
                                                  PublicKey &base,
                                                  EllipticCurveScalar &hashedDerivation)
    {
        geP3 point1;
        geP3 point2;
        geCached point3;
        geP1P1 point4;
        geP2 point5;

        if (geFromBytesVartime (&point1, reinterpret_cast<const unsigned char *>(&derivedKey)) != 0) {
            return false;
        }

        derivationToScalar (derivation, outputIndex, hashedDerivation);
        geScalarmultBase (&point2, reinterpret_cast<unsigned char *>(&hashedDerivation));
        geP3ToCached (&point3, &point2);
        geSub (&point4, &point1, &point3);
        geP1P1ToP2 (&point5, &point4);
        geToBytes (reinterpret_cast<unsigned char *>(&base), &point5);

        return true;
    }

    void CryptoOps::deriveSecretKey(const KeyDerivation &derivation,
                                    size_t outputIndex,
                                    const SecretKey &base,
                                    SecretKey &derivedKey)
    {
        EllipticCurveScalar scalar;
        assert(scCheck (reinterpret_cast<const unsigned char *>(&base)) == 0);
        derivationToScalar (derivation, outputIndex, scalar);
        scAdd (reinterpret_cast<unsigned char *>(&derivedKey),
               reinterpret_cast<const unsigned char *>(&base),
               reinterpret_cast<unsigned char *>(&scalar));
    }

    void CryptoOps::deriveSecretKey(const KeyDerivation &derivation,
                                    size_t outputIndex,
                                    const SecretKey &base,
                                    const uint8_t *suffix,
                                    size_t suffixLength,
                                    SecretKey &derivedKey)
    {
        EllipticCurveScalar scalar;
        assert(scCheck (reinterpret_cast<const unsigned char *>(&base)) == 0);
        derivationToScalar (derivation, outputIndex, suffix, suffixLength, scalar);
        scAdd (reinterpret_cast<unsigned char *>(&derivedKey),
               reinterpret_cast<const unsigned char *>(&base),
               reinterpret_cast<unsigned char *>(&scalar));
    }

    bool CryptoOps::underivePublicKey(const KeyDerivation &derivation,
                                      size_t outputIndex,
                                      const PublicKey &derivedKey,
                                      PublicKey &base)
    {
        EllipticCurveScalar scalar;
        geP3 point1;
        geP3 point2;
        geCached point3;
        geP1P1 point4;
        geP2 point5;

        if (geFromBytesVartime (&point1,
                                reinterpret_cast<const unsigned char *>(&derivedKey)) != 0) {
            return false;
        }

        derivationToScalar (derivation, outputIndex, scalar);
        geScalarmultBase (&point2, reinterpret_cast<unsigned char *>(&scalar));
        geP3ToCached (&point3, &point2);
        geSub (&point4, &point1, &point3);
        geP1P1ToP2 (&point5, &point4);
        geToBytes (reinterpret_cast<unsigned char *>(&base), &point5);

        return true;
    }

    bool CryptoOps::underivePublicKey(const KeyDerivation &derivation,
                                      size_t outputIndex,
                                      const PublicKey &derivedKey,
                                      const uint8_t *suffix,
                                      size_t suffixLength,
                                      PublicKey &base)
    {
        EllipticCurveScalar scalar;
        geP3 point1;
        geP3 point2;
        geCached point3;
        geP1P1 point4;
        geP2 point5;

        if (geFromBytesVartime (&point1,
                                reinterpret_cast<const unsigned char *>(&derivedKey)) != 0) {
            return false;
        }

        derivationToScalar (derivation, outputIndex, suffix, suffixLength, scalar);
        geScalarmultBase (&point2, reinterpret_cast<unsigned char *>(&scalar));
        geP3ToCached (&point3, &point2);
        geSub (&point4, &point1, &point3);
        geP1P1ToP2 (&point5, &point4);
        geToBytes (reinterpret_cast<unsigned char *>(&base), &point5);

        return true;
    }

    struct sComm
    {
        Hash h;
        EllipticCurvePoint key;
        EllipticCurvePoint comm;
    };

    void CryptoOps::generateSignature(const Hash &prefixHash,
                                      const PublicKey &pub,
                                      const SecretKey &sec,
                                      Signature &sig)
    {
        geP3 tmp3;
        EllipticCurveScalar k;
        sComm buf;
#if !defined(NDEBUG)
        {
            geP3 t;
            PublicKey t2;
            assert(scCheck (reinterpret_cast<const unsigned char *>(&sec)) == 0);
            geScalarmultBase (&t, reinterpret_cast<const unsigned char *>(&sec));
            geP3ToBytes (reinterpret_cast<unsigned char *>(&t2), &t);
            assert(pub == t2);
        }
#endif
        buf.h = prefixHash;
        buf.key = reinterpret_cast<const EllipticCurvePoint &>(pub);
        randomScalar (k);
        geScalarmultBase (&tmp3, reinterpret_cast<unsigned char *>(&k));
        geP3ToBytes (reinterpret_cast<unsigned char *>(&buf.comm), &tmp3);
        hashToScalar (&buf, sizeof (sComm), reinterpret_cast<EllipticCurveScalar &>(sig));
        scMulSub (reinterpret_cast<unsigned char *>(&sig) + 32,
                  reinterpret_cast<unsigned char *>(&sig),
                  reinterpret_cast<const unsigned char *>(&sec),
                  reinterpret_cast<unsigned char *>(&k));
    }

    bool CryptoOps::checkSignature(const Hash &prefixHash,
                                   const PublicKey &pub,
                                   const Signature &sig)
    {
        geP2 tmp2;
        geP3 tmp3;
        EllipticCurveScalar c;
        sComm buf;
        assert(checkKey (pub));
        buf.h = prefixHash;
        buf.key = reinterpret_cast<const EllipticCurvePoint &>(pub);

        if (geFromBytesVartime (&tmp3, reinterpret_cast<const unsigned char *>(&pub)) != 0) {
            return false;
        }

        if (scCheck (reinterpret_cast<const unsigned char *>(&sig)) != 0 ||
            scCheck (reinterpret_cast<const unsigned char *>(&sig) + 32) != 0) {
            return false;
        }
        geDoubleScalarmultBaseVartime (&tmp2,
                                       reinterpret_cast<const unsigned char *>(&sig),
                                       &tmp3,
                                       reinterpret_cast<const unsigned char *>(&sig) + 32);
        geToBytes (reinterpret_cast<unsigned char *>(&buf.comm), &tmp2);
        hashToScalar (&buf, sizeof (sComm), c);
        scSub (reinterpret_cast<unsigned char *>(&c),
               reinterpret_cast<unsigned char *>(&c),
               reinterpret_cast<const unsigned char *>(&sig));

        return scIsNonZero (reinterpret_cast<unsigned char *>(&c)) == 0;
    }

    static void hashToEC(const PublicKey &key, geP3 &res)
    {
        Hash h;
        geP2 point;
        geP1P1 point2;
        CnFastHash (std::addressof (key), sizeof (PublicKey), h);
        geFromFeFromBytesVartime (&point,
                                  reinterpret_cast<const unsigned char *>(&h));
        geMul8 (&point2, &point);
        geP1P1ToP3 (&res, &point2);
    }

    KeyImage CryptoOps::scalarmultKey(const KeyImage &P, const KeyImage &a)
    {
        geP3 A;
        geP2 R;
        // maybe use assert instead?
        geFromBytesVartime (&A, reinterpret_cast<const unsigned char *>(&P));
        geScalarmult (&R, reinterpret_cast<const unsigned char *>(&a), &A);
        KeyImage aP;
        geToBytes (reinterpret_cast<unsigned char *>(&aP), &R);

        return aP;
    }

    void CryptoOps::hashDataToEC(const uint8_t *data,
                                 std::size_t len,
                                 PublicKey &key)
    {
        Hash h;
        geP2 point;
        geP1P1 point2;
        CnFastHash (data, len, h);
        geFromFeFromBytesVartime (&point, reinterpret_cast<const unsigned char *>(&h));
        geMul8 (&point2, &point);
        geP1P1ToP2 (&point, &point2);
        geToBytes (reinterpret_cast<unsigned char *>(&key), &point);
    }

    void CryptoOps::generateKeyImage(const PublicKey &pub,
                                     const SecretKey &sec,
                                     KeyImage &image)
    {
        geP3 point;
        geP2 point2;
        assert(scCheck (reinterpret_cast<const unsigned char *>(&sec)) == 0);
        hashToEC (pub, point);
        geScalarmult (&point2,
                      reinterpret_cast<const unsigned char *>(&sec), &point);
        geToBytes (reinterpret_cast<unsigned char *>(&image), &point2);
    }

#ifdef _MSC_VER
#pragma warning(disable: 4200)
#endif

    struct rsComm
    {
        Hash h;
        struct
        {
            EllipticCurvePoint a, b;
        } ab[];
    };

    static inline size_t rsCommSize(size_t pubs_count)
    {
        return sizeof (rsComm) +
               pubs_count *
               sizeof (((rsComm *) 0)->ab[0]);
    }

    std::tuple<bool, std::vector<Signature>> CryptoOps::generateRingSignatures(
        const Hash prefixHash,
        const KeyImage keyImage,
        const std::vector<PublicKey> publicKeys,
        const Crypto::SecretKey transactionSecretKey,
        uint64_t realOutput)
    {
        std::vector<Signature> signatures (publicKeys.size ());

        geP3 imageUnp;
        geDsmp imagePre;
        EllipticCurveScalar sum, k, h;

        rsComm *const buf = reinterpret_cast<rsComm *>(alloca(rsCommSize (publicKeys.size ())));

        if (geFromBytesVartime (&imageUnp,
                                reinterpret_cast<const unsigned char *>(&keyImage)) != 0) {
            return {false, signatures};
        }

        geDsmPrecomp (imagePre, &imageUnp);

        sc0 (reinterpret_cast<unsigned char *>(&sum));

        buf->h = prefixHash;

        for (size_t i = 0; i < publicKeys.size (); i++) {
            geP2 tmp2;
            geP3 tmp3;

            if (i == realOutput) {
                randomScalar (k);
                geScalarmultBase (&tmp3, reinterpret_cast<unsigned char *>(&k));
                geP3ToBytes (reinterpret_cast<unsigned char *>(&buf->ab[i].a), &tmp3);
                hashToEC (publicKeys[i], tmp3);
                geScalarmult (&tmp2, reinterpret_cast<unsigned char *>(&k), &tmp3);
                geToBytes (reinterpret_cast<unsigned char *>(&buf->ab[i].b), &tmp2);
            } else {
                randomScalar (reinterpret_cast<EllipticCurveScalar &>(signatures[i]));
                randomScalar (*reinterpret_cast<EllipticCurveScalar *>(
                    reinterpret_cast<unsigned char *>(&signatures[i]) + 32));

                if (geFromBytesVartime (&tmp3,
                                        reinterpret_cast<const unsigned char *>(&publicKeys[i])) != 0) {
                    return {false, signatures};
                }

                geDoubleScalarmultBaseVartime (
                    &tmp2,
                    reinterpret_cast<unsigned char *>(&signatures[i]),
                    &tmp3,
                    reinterpret_cast<unsigned char *>(&signatures[i]) + 32
                );

                geToBytes (reinterpret_cast<unsigned char *>(&buf->ab[i].a), &tmp2);

                hashToEC (publicKeys[i], tmp3);

                geDoubleScalarmultPrecompVartime (
                    &tmp2,
                    reinterpret_cast<unsigned char *>(&signatures[i]) + 32,
                    &tmp3,
                    reinterpret_cast<unsigned char *>(&signatures[i]),
                    imagePre
                );

                geToBytes (reinterpret_cast<unsigned char *>(&buf->ab[i].b), &tmp2);

                scAdd (
                    reinterpret_cast<unsigned char *>(&sum),
                    reinterpret_cast<unsigned char *>(&sum),
                    reinterpret_cast<unsigned char *>(&signatures[i])
                );
            }
        }

        hashToScalar (buf, rsCommSize (publicKeys.size ()), h);

        scSub (
            reinterpret_cast<unsigned char *>(&signatures[realOutput]),
            reinterpret_cast<unsigned char *>(&h),
            reinterpret_cast<unsigned char *>(&sum)
        );

        scMulSub (
            reinterpret_cast<unsigned char *>(&signatures[realOutput]) + 32,
            reinterpret_cast<unsigned char *>(&signatures[realOutput]),
            reinterpret_cast<const unsigned char *>(&transactionSecretKey),
            reinterpret_cast<unsigned char *>(&k)
        );

        return {true, signatures};
    }

    bool CryptoOps::checkRingSignature(
        const Hash &prefixHash,
        const KeyImage &image,
        const std::vector<PublicKey> pubs,
        const std::vector<Signature> signatures)
    {

        geP3 imageUnp;
        geDsmp imagePre;

        EllipticCurveScalar sum, h;

        rsComm *const buf = reinterpret_cast<rsComm *>(alloca(rsCommSize (pubs.size ())));

        if (geFromBytesVartime (&imageUnp,
                                reinterpret_cast<const unsigned char *>(&image)) != 0) {
            return false;
        }

        geDsmPrecomp (imagePre, &imageUnp);

        if (geCheckSubgroupPrecompVartime (imagePre) != 0) {
            return false;
        }

        sc0 (reinterpret_cast<unsigned char *>(&sum));

        buf->h = prefixHash;

        for (size_t i = 0; i < pubs.size (); i++) {
            geP2 tmp2;
            geP3 tmp3;

            if (scCheck (reinterpret_cast<const unsigned char *>(&signatures[i])) != 0
                || scCheck (reinterpret_cast<const unsigned char *>(&signatures[i]) + 32) != 0) {
                return false;
            }

            if (geFromBytesVartime (&tmp3,
                                    reinterpret_cast<const unsigned char *>(&pubs[i])) != 0) {
                return false;
            }

            geDoubleScalarmultBaseVartime (
                &tmp2,
                reinterpret_cast<const unsigned char *>(&signatures[i]),
                &tmp3,
                reinterpret_cast<const unsigned char *>(&signatures[i]) + 32
            );

            geToBytes (reinterpret_cast<unsigned char *>(&buf->ab[i].a), &tmp2);

            hashToEC (pubs[i], tmp3);

            geDoubleScalarmultPrecompVartime (
                &tmp2,
                reinterpret_cast<const unsigned char *>(&signatures[i]) + 32,
                &tmp3,
                reinterpret_cast<const unsigned char *>(&signatures[i]),
                imagePre
            );

            geToBytes (reinterpret_cast<unsigned char *>(&buf->ab[i].b), &tmp2);

            scAdd (
                reinterpret_cast<unsigned char *>(&sum),
                reinterpret_cast<unsigned char *>(&sum),
                reinterpret_cast<const unsigned char *>(&signatures[i])
            );
        }

        hashToScalar (buf, rsCommSize (pubs.size ()), h);

        scSub (
            reinterpret_cast<unsigned char *>(&h),
            reinterpret_cast<unsigned char *>(&h),
            reinterpret_cast<unsigned char *>(&sum)
        );

        return scIsNonZero (reinterpret_cast<unsigned char *>(&h)) == 0;
    }

    void CryptoOps::generateViewFromSpend(const Crypto::SecretKey &spend,
                                          Crypto::SecretKey &viewSecret)
    {

        /*!
            If we don't need the pub key
        */
        Crypto::PublicKey unusedDummyVariable;
        generateViewFromSpend (spend, viewSecret, unusedDummyVariable);
    }

    void CryptoOps::generateViewFromSpend(const Crypto::SecretKey &spend,
                                          Crypto::SecretKey &viewSecret,
                                          Crypto::PublicKey &viewPublic)
    {

        Crypto::SecretKey viewKeySeed;
        keccak ((uint8_t *) &spend, sizeof (spend), (uint8_t *) &viewKeySeed, sizeof (viewKeySeed));
        Crypto::generateDeterministicKeys (viewPublic, viewSecret, viewKeySeed);
    }
} // namespace Crypto

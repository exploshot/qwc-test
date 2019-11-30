// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2016-2018, The Karbowanec developers
// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <cstddef>
#include <limits>
#include <mutex>
#include <type_traits>
#include <vector>

#include <CryptoTypes.h>
#include <Crypto/Hash.h>

namespace Crypto {

    struct EllipticCurvePoint 
    {
        uint8_t data[32];
    };

    struct EllipticCurveScalar 
    {
        uint8_t data[32];
    };

    class CryptoOps 
    {
        CryptoOps();
        CryptoOps(const CryptoOps &);
        void operator=(const CryptoOps &);
        ~CryptoOps();

        static void generateKeys(PublicKey &, SecretKey &);
        friend void generateKeys(PublicKey &, SecretKey &);
        static void generateDeterministicKeys(PublicKey &pub, SecretKey &sec, SecretKey &second);
        friend void generateDeterministicKeys(PublicKey &pub, SecretKey &sec, SecretKey &second);
        static SecretKey generateMKeys(PublicKey &pub, SecretKey &sec, const SecretKey &recoveryKey = SecretKey(), bool recover = false);
        friend SecretKey generateMKeys(PublicKey &pub, SecretKey &sec, const SecretKey &recoveryKey, bool recover);
        static bool checkKey(const PublicKey &);
        friend bool checkKey(const PublicKey &);
        static bool secretKeyToPublicKey(const SecretKey &, PublicKey &);
        friend bool secretKeyToPublicKey(const SecretKey &, PublicKey &);
        static bool generateKeyDerivation(const PublicKey &, const SecretKey &, KeyDerivation &);
        friend bool generateKeyDerivation(const PublicKey &, const SecretKey &, KeyDerivation &);
        static bool derivePublicKey(const KeyDerivation &, size_t, const PublicKey &, PublicKey &);
        friend bool derivePublicKey(const KeyDerivation &, size_t, const PublicKey &, PublicKey &);
        friend bool derivePublicKey(const KeyDerivation &, size_t, const PublicKey &, const uint8_t*, size_t, PublicKey &);
        static bool derivePublicKey(const KeyDerivation &, size_t, const PublicKey &, const uint8_t*, size_t, PublicKey &);
        //hack for pg
        static bool underivePublicKeyAndGetScalar(const KeyDerivation &, std::size_t, const PublicKey &, PublicKey &, EllipticCurveScalar &);
        friend bool underivePublicKeyAndGetScalar(const KeyDerivation &, std::size_t, const PublicKey &, PublicKey &, EllipticCurveScalar &);
        //
        static void deriveSecretKey(const KeyDerivation &, size_t, const SecretKey &, SecretKey &);
        friend void deriveSecretKey(const KeyDerivation &, size_t, const SecretKey &, SecretKey &);
        static void deriveSecretKey(const KeyDerivation &, size_t, const SecretKey &, const uint8_t*, size_t, SecretKey &);
        friend void deriveSecretKey(const KeyDerivation &, size_t, const SecretKey &, const uint8_t*, size_t, SecretKey &);
        static bool underivePublicKey(const KeyDerivation &, size_t, const PublicKey &, PublicKey &);
        friend bool underivePublicKey(const KeyDerivation &, size_t, const PublicKey &, PublicKey &);
        static bool underivePublicKey(const KeyDerivation &, size_t, const PublicKey &, const uint8_t*, size_t, PublicKey &);
        friend bool underivePublicKey(const KeyDerivation &, size_t, const PublicKey &, const uint8_t*, size_t, PublicKey &);
        static void generateSignature(const Hash &, const PublicKey &, const SecretKey &, Signature &);
        friend void generateSignature(const Hash &, const PublicKey &, const SecretKey &, Signature &);
        static bool checkSignature(const Hash &, const PublicKey &, const Signature &);
        friend bool checkSignature(const Hash &, const PublicKey &, const Signature &);
        static void generateKeyImage(const PublicKey &, const SecretKey &, KeyImage &);
        friend void generateKeyImage(const PublicKey &, const SecretKey &, KeyImage &);
        static KeyImage scalarmultKey(const KeyImage  &P, const KeyImage  &a);
        friend KeyImage scalarmultKey(const KeyImage  &P, const KeyImage  &a);
        static void hashDataToEC(const uint8_t*, std::size_t, PublicKey&);
        friend void hashDataToEC(const uint8_t*, std::size_t, PublicKey&);

    public:

        static std::tuple<bool, std::vector<Signature>> generateRingSignatures(
            const Hash prefixHash,
            const KeyImage keyImage,
            const std::vector<PublicKey> publicKeys,
            const Crypto::SecretKey transactionSecretKey,
            uint64_t realOutput);

        static bool checkRingSignature(
            const Hash &prefixHash,
            const KeyImage &image,
            const std::vector<PublicKey> pubs,
            const std::vector<Signature> signatures);

        static void generateViewFromSpend(
            const Crypto::SecretKey &spend,
            Crypto::SecretKey &viewSecret);

        static void generateViewFromSpend(
            const Crypto::SecretKey &spend,
            Crypto::SecretKey &viewSecret,
            Crypto::PublicKey &viewPublic);
    };

    /*!
        Generate a new key pair
    */
    inline void generateKeys(PublicKey &pub, SecretKey &sec) 
    {
        CryptoOps::generateKeys(pub, sec);
    }

    inline void generateDeterministicKeys(PublicKey &pub, 
                                          SecretKey &sec, 
                                          SecretKey &second) 
    {
        CryptoOps::generateDeterministicKeys(pub, sec, second);
    }

    inline SecretKey generateMKeys(PublicKey &pub, 
                                  SecretKey &sec, 
                                  const SecretKey &recoveryKey = SecretKey(), 
                                  bool recover = false) 
    {
        return CryptoOps::generateMKeys(pub, sec, recoveryKey, recover);
    }

    /*!
        Check a public key. Returns true if it is valid, false otherwise.
    */
    inline bool checkKey(const PublicKey &key) 
    {
        return CryptoOps::checkKey(key);
    }

    /*!
        Checks a private key and computes the corresponding public key.
    */
    inline bool secretKeyToPublicKey(const SecretKey &sec, PublicKey &pub) 
    {
        return CryptoOps::secretKeyToPublicKey(sec, pub);
    }

    /*!
        To generate an ephemeral key used to send money to:
        The sender generates a new key pair, which becomes the transaction key.
        The public transaction key is included in "extra" field.
        Both the sender and the receiver generate key derivation from the
        transaction key and the receivers' "view" key.
        The sender uses key derivation, the output index, and the receivers'
        "spend" key to derive an ephemeral public key.
        The receiver can either derive the public key (to check that the transaction
        is addressed to him) or the private key (to spend the money).
    */
    inline bool generateKeyDerivation(const PublicKey &key1, 
                                      const SecretKey &key2, 
                                      KeyDerivation &derivation) 
    {
        return CryptoOps::generateKeyDerivation(key1, key2, derivation);
    }

    inline bool derivePublicKey(const KeyDerivation &derivation, 
                                size_t outputIndex,
                                const PublicKey &base, 
                                const uint8_t *prefix, 
                                size_t prefixLength, 
                                PublicKey &derivedKey) 
    {
        return CryptoOps::derivePublicKey(derivation, 
                                          outputIndex, 
                                          base, 
                                          prefix, 
                                          prefixLength, 
                                          derivedKey);
    }

    inline bool derivePublicKey(const KeyDerivation &derivation, 
                                size_t outputIndex,
                                const PublicKey &base, 
                                PublicKey &derivedKey)
    {
        return CryptoOps::derivePublicKey(derivation, 
                                          outputIndex, 
                                          base, 
                                          derivedKey);
    }

    inline bool underivePublicKeyAndGetScalar(const KeyDerivation &derivation, 
                                              std::size_t outputIndex,
                                              const PublicKey &derivedKey, 
                                              PublicKey &base, 
                                              EllipticCurveScalar &hashedDerivation) 
    {
        return CryptoOps::underivePublicKeyAndGetScalar(derivation, 
                                                        outputIndex, 
                                                        derivedKey, 
                                                        base, 
                                                        hashedDerivation);
    }
    
    inline void deriveSecretKey(const KeyDerivation &derivation, 
                                std::size_t outputIndex,
                                const SecretKey &base, 
                                const uint8_t *prefix, 
                                size_t prefixLength, 
                                SecretKey &derivedKey) 
    {
        CryptoOps::deriveSecretKey(derivation, 
                                  outputIndex, 
                                  base, 
                                  prefix, 
                                  prefixLength, 
                                  derivedKey);
    }

    inline void deriveSecretKey(const KeyDerivation &derivation, 
                                std::size_t outputIndex,
                                const SecretKey &base, 
                                SecretKey &derivedKey) 
    {
        CryptoOps::deriveSecretKey(derivation, 
                                  outputIndex, 
                                  base, 
                                  derivedKey);
    }


    /*!
        Inverse function of derivePublicKey. It can be used by the receiver to find which 
        "spend" key was used to generate a transaction. This may be useful if the receiver 
        used multiple addresses which only differ in "spend" key.
    */
    inline bool underivePublicKey(const KeyDerivation &derivation, 
                                  size_t outputIndex,
                                  const PublicKey &derivedKey, 
                                  const uint8_t *prefix, 
                                  size_t prefixLength, 
                                  PublicKey &base) 
    {
        return CryptoOps::underivePublicKey(derivation, 
                                            outputIndex, 
                                            derivedKey, 
                                            prefix, 
                                            prefixLength, 
                                            base);
    }

    inline bool underivePublicKey(const KeyDerivation &derivation, 
                                  size_t outputIndex,
                                  const PublicKey &derivedKey, 
                                  PublicKey &base) 
    {
        return CryptoOps::underivePublicKey(derivation, 
                                            outputIndex, 
                                            derivedKey, 
                                            base);
    }

    /*!
        Generation and checking of a standard signature.
    */
    inline void generateSignature(const Hash &prefixHash, 
                                  const PublicKey &pub, 
                                  const SecretKey &sec, 
                                  Signature &sig)
    {
        CryptoOps::generateSignature(prefixHash, pub, sec, sig);
    }

    inline bool checkSignature(const Hash &prefixHash, 
                              const PublicKey &pub, 
                              const Signature &sig) 
    {
        return CryptoOps::checkSignature(prefixHash, pub, sig);
    }

    /*!
        To send money to a key:
        The sender generates an ephemeral key and includes it in transaction output.
        To spend the money, the receiver generates a key image from it.
        Then he selects a bunch of outputs, including the one he spends, 
        and uses them to generate a ring signature.
        To check the signature, it is necessary to collect all the keys that were used to generate it.
        To detect double spends, it is necessary to check that each key image is used at most once.
    */
    inline void generateKeyImage(const PublicKey &pub, 
                                const SecretKey &sec, 
                                KeyImage &image) 
    {
        CryptoOps::generateKeyImage(pub, sec, image);
    }

    inline KeyImage scalarmultKey(const KeyImage  &P, const KeyImage &a) 
    {
        return CryptoOps::scalarmultKey(P, a);
    }

    inline void hashDataToEC(const uint8_t *data, 
                            std::size_t len, 
                            PublicKey &key) 
    {
        CryptoOps::hashDataToEC(data, len, key);
    }
} // namespace Crypto

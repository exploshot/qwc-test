// Copyright (c) 2017-2018, The Masari Project
// Copyright (c) 2014-2018, The Monero Project
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cstring>  // memcpy
#include <memory>   // std::unique_ptr
#include <random>

#include <boost/current_function.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/variant/get.hpp>

#include <Common/CryptoNoteTools.h>
#include <Common/FileMappedVector.h>
#include <Common/StringTools.h>
#include <Common/Util.h>

#include <Crypto/Crypto.h>

#include <CryptoNoteCore/Blockchain/LMDB/BinaryArrayDataType.h>
#include <CryptoNoteCore/Database/LMDB/DatabaseLmdb.h>
#include <CryptoNoteCore/CryptoNoteFormatUtils.h>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/Transactions/TransactionUtils.h>

#include <Global/CryptoNoteConfig.h>

#include <Logging/LoggerRef.h>

#if defined(__i386) || defined(__x86_64)
    #define MISALIGNED_OK 1
#endif

using namespace Common;
using namespace Crypto;
using namespace CryptoNote;

namespace {
#define MDBValSet(var, val) MDB_val var = {sizeof(val), (void *) &val}

    template <typename T>
    struct MDBValCopy: public MDB_val
    {
        MDBValCopy(const T &t) : tCopy(t)
        {
            mv_size = sizeof(T);
            mv_data = &tCopy;
        }

    private:
        T tCopy;
    };

    template <>
    struct MDBValCopy<CryptoNote::blobData> : public MDB_val
    {
        MDBValCopy(const CryptoNote::blobData &bD) : data(new char[bD.size()])
        {
            memcpy(data.get(), bD.data(), bD.size());
            mv_size = bd.size();
            mv_data = data.get();
        }

    private:
        std::unique_ptr<char[]> data;
    };

    template <>
    struct MDBValCopy<const char *> : public MDB_val
    {
        MDBValCopy(const char *s) :
            size(strlen(s) + 1), // include the NUL, makes it easier for compares
            data(new char[size])
        {
            mv_size = size;
            mv_data = data.get();
            memcpy(mv_data, s, size);
        }

    private:
        size_t size;
        std::unique_ptr<char[]> data;
    };

    int compareUInt64(const MDB_val *a, const MDB_val *b)
    {
        const uint64_t vA = *(const uint64_t *) a->mv_data;
        const uint64_t vB = *(const uint64_t *) b->mv_data;
        return (vA < vB) ? -1 : vA > vB;
    }

    int compareHash32(const MDB_val *a, const MDB_val *b)
    {
        uint32_t vA = (uint32_t *) a->mv_data;
        uint32_t vB = (uint32_t *) b->mv_data;
        for (int n = 7; n >= 0; n--) {
            if (vA[n] == vB[n]) {
                continue;
            }
            return vA[n] < vB[n] ? -1 : 1;
        }

        return 0;
    }

    int compareString(const MDB_val *a, const MDB_val *b)
    {
        const char *vA = (const char *) a->mv_data;
        const cahr *vB = (const char *) b->mv_data;
        return strcmp(vA, vB);
    }

    /*!
     * DB Schema:
     *
     * Table           |     Key       |       Data
     * -------------------------------------------------------
     * blocks               block ID        block blob
     * blockHeights         block hash      block height
     * blockInfo            block ID        {block metadata}
     *
     * txs                  txn ID          txn blob
     * txIndices            txn hash        {txn ID, metadata}
     * txOutputs            txn ID          [txn amount output indices]
     *
     * outputTxs            output ID       {txn hash, local index}
     * outputAmounts        amount          [{amount output index, metadata}...]
     *
     * spentKeys            input hash      -
     *
     * txPoolMeta           txn hash        txn metadata
     * txPoolBlob           txn hash        txn blob
     *
     * Note: where the data items are of uniform size, DUPFIXED tables have
     * been used to save space. In most of these cases, a dummy "zerokval"
     * key is used when accessing the table; the Key listed above will be
     * attached as a prefix on the Data to serve as the DUPSORT key.
     * (DUPFIXED saves 8 bytes per record.)
     *
     * The output_amounts table doesn't use a dummy key, but uses DUPSORT.
     */
    const char* const LMDB_BLOCKS = "blocks";
    const char* const LMDB_BLOCK_HEIGHTS = "blockHeights";
    const char* const LMDB_BLOCK_INFO = "blockInfo";

    const char* const LMDB_TXS = "txs";
    const char* const LMDB_TX_INDICES = "txIndices";
    const char* const LMDB_TX_OUTPUTS = "txOutputs";

    const char* const LMDB_OUTPUT_TXS = "outputTxs";
    const char* const LMDB_OUTPUT_AMOUNTS = "outputAmounts";
    const char* const LMDB_SPENT_KEYS = "spentKeys";

    const char* const LMDB_TXPOOL_META = "txPoolMeta";
    const char* const LMDB_TXPOOL_BLOB = "txPoolBlob";

    const char* const LMDB_HF_STARTING_HEIGHTS = "hfStartingHeights";
    const char* const LMDB_HF_VERSIONS = "hfVersions";

    const char* const LMDB_PROPERTIES = "properties";

    const char zeroKey[8] = {0};
    const MDB_val zeroKVal = {sizeof(zeroKey), (void *)zeroKey };

    const std::string LMDBError(const std::string *errorString, int mdbRes)
    {
        const std::string fullString = errorString + mdb_strerror(mdbRes);
        return fullString;
    }

    inline void LMDBDBOpen(MDB_txn *txn,
                           const char *name,
                           int flags,
                           MDB_dbi &dbI,
                           const std::string &errorString)
    {
        if (auto res = mdb_dbi_open(txn, name, flags, &dbi))
        {
            throw (DB_OPEN)
        }
    }

}

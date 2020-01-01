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

/* From fe.h */

typedef int32_t fe[10];

/* From ge.h */

typedef struct
{
    fe X;
    fe Y;
    fe Z;
} geP2;

typedef struct
{
    fe X;
    fe Y;
    fe Z;
    fe T;
} geP3;

typedef struct
{
    fe X;
    fe Y;
    fe Z;
    fe T;
} geP1P1;

typedef struct
{
    fe yplusx;
    fe yminusx;
    fe xy2d;
} gePrecomp;

typedef struct
{
    fe YplusX;
    fe YminusX;
    fe Z;
    fe T2d;
} geCached;

/* From geAdd.c */

void geAdd(geP1P1 *, const geP3 *, const geCached *);

/* From ge_double_scalarmult.c, modified */

typedef geCached geDsmp[8];

extern const gePrecomp ge_Bi[8];

void geDsmPrecomp(geDsmp r, const geP3 *s);
void geDoubleScalarmultBaseVartime(geP2 *,
                                   const unsigned char *,
                                   const geP3 *,
                                   const unsigned char *);

/* From ge_frombytes.c, modified */

extern const fe fe_SqrtM1;

extern const fe feD;

int geFromBytesVartime(geP3 *, const unsigned char *);

/* From geP1P1ToP2.c */

void geP1P1ToP2(geP2 *, const geP1P1 *);

/* From geP1P1ToP3.c */

void geP1P1ToP3(geP3 *, const geP1P1 *);

/* From geP2Dbl.c */

void geP2Dbl(geP1P1 *, const geP2 *);

/* From geP3ToCached.c */

extern const fe feD2;

void geP3ToCached(geCached *, const geP3 *);

/* From geP3ToP2.c */

void geP3ToP2(geP2 *, const geP3 *);

/* From geP3ToBytes.c */

void geP3ToBytes(unsigned char *, const geP3 *);

/* From geScalarmultBase.c */

extern const gePrecomp geBase[32][8];

void geScalarmultBase(geP3 *, const unsigned char *);

/* From geSub.c */

void geSub(geP1P1 *, const geP3 *, const geCached *);

/* From geToBytes.c */

void geToBytes(unsigned char *, const geP2 *);

/* From scReduce.c */

void scReduce(unsigned char *);

/* New code */

void geScalarmult(geP2 *, const unsigned char *, const geP3 *);
void geDoubleScalarmultPrecompVartime(geP2 *, const unsigned char *, const geP3 *, const unsigned char *, const geDsmp);
int geCheckSubgroupPrecompVartime(const geDsmp);
void geMul8(geP1P1 *, const geP2 *);

extern const fe fe_ma2;

extern const fe fe_ma;

extern const fe fe_fffb1;

extern const fe fe_fffb2;

extern const fe fe_fffb3;

extern const fe fe_fffb4;

void geFromFeFromBytesVartime(geP2 *, const unsigned char *);
void sc0(unsigned char *);
void scReduce32(unsigned char *);
void scAdd(unsigned char *, const unsigned char *, const unsigned char *);
void scSub(unsigned char *, const unsigned char *, const unsigned char *);
void scMulSub(unsigned char *, const unsigned char *, const unsigned char *, const unsigned char *);
int scCheck(const unsigned char *);
int scIsNonZero(const unsigned char *); /* Doesn't normalize */

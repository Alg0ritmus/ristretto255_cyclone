#ifndef _HELPERS_H
#define _HELPERS_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define FIELED_ELEM_SIZE 8 // i64 field_elem[16];
#define BYTES_ELEM_SIZE 32 // byte representation of field elements u8[32]
#define HASH_BYTES_SIZE 2*BYTES_ELEM_SIZE // u8[64]

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t i64;

//typedef i64 field_elem[FIELED_ELEM_SIZE];
typedef uint32_t field_elem[FIELED_ELEM_SIZE]; // cyclone repr

typedef struct ge_point25519{
    field_elem x,y,z,t;
}ristretto255_point;
// mine functions

/*-----------------------------------*/

#define FOR_T(i, start, end) for (i = (start); i < (end); i++)
#define FOR(i, start, end)         FOR_T(i, start, end)
#define COPY(i, dst, src, size)    FOR(i, 0, size) (dst)[i] = (src)[i]
#define ZERO(i, buf, size)         FOR(i, 0, size) (buf)[i] = 0

/*-----------------------------------*/


#endif //_HELPERS_H
// ******************************************************************
// ----------------- TECHNICAL UNIVERSITY OF KOSICE -----------------
// ---Department of Electronics and Multimedia Telecommunications ---
// -------- FACULTY OF ELECTRICAL ENGINEERING AND INFORMATICS -------
// ------------ THIS CODE IS A PART OF A MASTER'S THESIS ------------
// ------------------------- Master thesis --------------------------
// -----------------Patrik Zelenak & Milos Drutarovsky --------------
// ---------------------------version 0.1.3 -------------------------
// --------------------------- 30-09-2023 ---------------------------
// ******************************************************************

/**
  * This file contains the structure of 'field_elem,'
  * which serves as the internal representation of field elements 
  * in GF(2^255-19).
  * In our code, 'field_elem' represents a array of eight words,
  * each with a length of 'uint32_t.' This structure is well-suited 
  * for our representation of 'ristretto255' bcs. we do not need to
  * perform packing/unpacking to obtain the internal representation
  * required for internal calculations, unlike other libraries.
  *
  * File also includes repr. of ristretto255_point,
  * which is point that uses twisted endward's curve,
  * extended coordinate representation (X, Y, Z, T).
  *
  * Last but not least, the file provides redefined types,
  * length constants, and macros used throughout the entire codebase.
**/

#ifndef _HELPERS_H
#define _HELPERS_H

#include <stdio.h>
#include <stdint.h>
#include "config.h"

// pokec k tomuto 
#define FIELED_ELEM_SIZE 8 
#define BYTES_ELEM_SIZE 32 
#define HASH_BYTES_SIZE 2*BYTES_ELEM_SIZE 

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;


// cyclone represantation of field_element
typedef u32 field_elem[FIELED_ELEM_SIZE];

typedef struct ge_point25519 {
    field_elem x,y,z,t;
} ristretto255_point;


// Here we implemented some macros to make
// code more readable. 
/*-----------------------------------*/

#define FOR_T(i, start, end) for (i = (start); i < (end); i++)
#define FOR(i, start, end)         FOR_T(i, start, end)
#define COPY(i, dst, src, size)    FOR(i, 0, size) (dst)[i] = (src)[i]
#define ZERO(i, buf, size)         FOR(i, 0, size) (buf)[i] = 0
// The macro WIPE_BUFFER is used throughout this file,
// and it employs the crypto_wipe function (defined below)
// to securely erase input data.
#define WIPE_BUFFER(buffer)        crypto_wipe(buffer, sizeof(buffer))

/**
  * In the context of a crypto library, using volatile to prevent the
  * compiler from optimizing out memory zeroing is a safety 
  * precaution. Crypto libraries often deal with sensitive data,
  * and ensuring that memory is securely wiped (zeroed) is a critical
  * security requirement to prevent data leakage.
  *
  * The compiler's optimization may decide to skip the zeroing
  * operation if it believes the memory is never read afterward.
  * By using volatile, you're essentially telling the compiler
  * not to make that assumption and to perform the zeroing operation
  * regardless of whether the memory is explicitly read afterward.
  * This helps ensure that sensitive data is properly wiped from 
  * memory, which is crucial for security purposes.
  *
  * In other words, we need to use volatile because the compiler
  * might skip zeroing the buffer due to optimization and make the
  * assumption that 'why would I write into the buffer when it
  * is never read.'
  *
  * One more comment on volatile keyword:
  * The 'volatile' keyword is used to indicate to the compiler that
  * a variable or object can change its value at any time without any
  * action being taken by the code the compiler finds nearby. 
  * This is typically used to prevent the compiler from optimizing
  * away certain accesses to memory, especially in cases where 
  * the memory might be modified by hardware, another thread,
  * or some other external entity that the compiler cannot predict.
  *
  * Last but not least, the difference between volatile on a pointer
  * and volatile on a variable is that volatile on a pointer indicates
  * that the data being pointed to may change,
  * while volatile on a variable indicates that the variable itself
  * may change at any time.
  *
  * Note:
  * A pointer of the form volatile volatile u8 *v_secret
  * is a pointer to an u8 that the compiler will treat as volatile.
  * This means that the compiler will assume that it is possible
  * for the variable that v_secret is pointing at to have changed
  * even if there is nothing in the source code to suggest that
  * this might occur. (https://stackoverflow.com/questions/9935190/why-is-a-point-to-volatile-pointer-like-volatile-int-p-useful).
**/
static void crypto_wipe(void *secret, size_t size)
{
    volatile u8 *v_secret = (u8*)secret;
    size_t idx;
    ZERO(idx, v_secret, size);
}


/*-----------------------------------*/


#endif //_HELPERS_H
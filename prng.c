// ******************************************************************
// ----------------- TECHNICAL UNIVERSITY OF KOSICE -----------------
// ---Department of Electronics and Multimedia Telecommunications ---
// -------- FACULTY OF ELECTRICAL ENGINEERING AND INFORMATICS -------
// ------------ THIS CODE IS A PART OF A MASTER'S THESIS ------------
// ------------------------- Master thesis --------------------------
// -----------------Patrik Zelenak & Milos Drutarovsky --------------
// ---------------------------version T.T.5 -------------------------
// --------------------------- 05-02-2024 ---------------------------
// ******************************************************************

/**
  * This file contains PRNG taken from:
  * https://rosettacode.org/wiki/Linear_congruential_generator#C
  * 
  * A rand_32_bytes is a function that we use to generate a 
  * random 32-byte value. We also changed srand() to s_rand()
  *  in naming, and we made it available from external files
  *  by removing the static keyword.
**/

#include "prng.h"

static int32_t rseed = 0; // seed can be changed externally using s_rand()

void s_rand(int32_t x)
{
	rseed = x;
}

#ifndef MS_RAND
#define RAND_MAX ((1U << 31) - 1)

static inline int rand()
{
	return rseed = (rseed * 1103515245 + 12345) & RAND_MAX;
}

#else /* MS rand */

#define RAND_MAX_32 ((1U << 31) - 1)
#define RAND_MAX ((1U << 15) - 1)

static inline int rand()
{
	return (rseed = (rseed * 214013 + 2531011) & RAND_MAX_32) >> 16;
}

#endif/* MS_RAND */



void rand_32_bytes(u8 out[32]){
    for (int i = 0; i < 32; i++){
        out[i] = (rand() >> 24) & 0xFF;
    }
}
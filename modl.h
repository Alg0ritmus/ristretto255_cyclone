// **********************************************************************************
// -----------------------------TECHNICAL UNIVERSITY OF KOSICE-----------------------
// ------------------- FACULTY OF ELECTRICAL ENGINEERING AND INFORMATICS-------------
// --------------------- THIS CODE IS A PART OF A MASTER'S THESIS -------------------
// -------------------------------------Master thesis--------------------------------
// ----------------------------Patrik Zelenak & Milos Drutarovsky--------------------
// --------------------------------------version 0.1---------------------------------
// **********************************************************************************

/**
  * This file includes 3 function taken from crypto library MonoCypher. We extracted and 
  * slightly edited functions that performs modular inverse (arithmetic modulo L). These functions
  * are suitable for creation of other protocols like OPAQUE etc. 
  *
  * Note that mod_l() are build on Barrett reduction which is used later in inverse_mod_l();
  * which can be suboptimal, even though we reuse temp. space on stack. More efficient way is to use Montgomery reduction. Due to efficieny
  * we slightly edited crypto_x25519_inverse() to perform Montgomery reduction. 
  * 
  * NOTE: Extracted and edited code is functional only in Litle Endian architecture (at least for now)!
**/

#ifndef _MODL_H
#define _MODL_H

// Dependencies
#include "helpers.h" // included definition of types like u8, u32... + FOR macros, etc.

void mod_l(u8 reduced[BYTES_ELEM_SIZE], const u32 x[16]);
void inverse_mod_l(u8 out[BYTES_ELEM_SIZE], const u8 in[BYTES_ELEM_SIZE]);
void crypto_x25519_inverse(u8 out[BYTES_ELEM_SIZE], const u8 in[BYTES_ELEM_SIZE]);

#ifdef MONTGOMERY_MODL_INVERSE_FLAG
	#define modl_l_inverse crypto_x25519_inverse
#else
	#define modl_l_inverse inverse_mod_l
#endif

#endif //_MODL_H
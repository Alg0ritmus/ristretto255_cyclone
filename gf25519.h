/**
  * SOME WORDS:
  * This file represents an excerpt of code from the cryptographic
  * library Cyclone. None of this code has been edited;
  * the only change from the original code is the renaming of functions
  * that I extracted. You can see the original naming as well as the URL
  * to the function implementation above each function. We chose the Cyclone
  * crypto library because of its advantages, such as compactness and speed,
  * eliminating the need for I/O conversion. In other words, we do not need to perform
  * any conversion, such as pack/unpack or from_bytes/to_bytes, as required by other
  * libraries like TweetNaCl or MonoCypher. We also appreciate the maintenance of the Cyclone library,
  * so we are using the latest version, 2.3.0, released on June 12, 2023.
**/

/**
 * @file gf25519.h
 * @brief Curve25519 elliptic curve (constant-time implementation)
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2023 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCRYPTO Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 2.3.0
 **/

#ifndef _GF25519_H
#define _GF25519_H

//Dependencies
#include "helpers.h"

typedef unsigned int uint_t;    // MD
typedef int int_t;              // MD

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//Curve25519 related functions

// Original name: curve25519Add
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L79C6-L79C19
void gf25519Add(uint32_t *r, const uint32_t *a, const uint32_t *b);

// Original name: curve25519Sub
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L130
void gf25519Sub(uint32_t *r, const uint32_t *a, const uint32_t *b);
  
// Original name: curve25519Mul
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L191 
void gf25519Mul(uint32_t *r, const uint32_t *a, const uint32_t *b);
 
// Original name: curve25519Red
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L352
void gf25519Red(uint32_t *r, const uint32_t *a);

// Original name: curve25519Sqr
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L317C18-L317C31
void gf25519Sqr(uint32_t *r, const uint32_t *a);

// Original name: curve25519Copy
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L515
void gf25519Copy(uint32_t *a, const uint32_t *b);

// Original name: curve25519Swap
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L534
void gf25519Swap(uint32_t *a, uint32_t *b, uint32_t c);

// Original name: curve25519Select
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L562
void gf25519Select(uint32_t *r, const uint32_t *a, const uint32_t *b, uint32_t c);

// Original name: curve25519Comp
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L587
uint32_t gf25519Comp(const uint32_t *a, const uint32_t *b);

// Original name: curve25519Pwr2
// URL: https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L331
void gf25519Pwr2(uint32_t *r, const uint32_t *a, uint_t n);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif

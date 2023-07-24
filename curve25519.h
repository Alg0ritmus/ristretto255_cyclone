// 20230627 MD, upravene pre samostatnu kompilaciu 

/**
 * @file curve25519.h
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

#ifndef _CURVE25519_H
#define _CURVE25519_H

//Dependencies
//#include "core/crypto.h"
#include "helpers.h"
#include <stdint.h>


typedef unsigned int uint_t;    // MD
typedef int int_t;              // MD


//Length of the elliptic curve
#define CURVE25519_BIT_LEN 255
#define CURVE25519_BYTE_LEN 32
#define CURVE25519_WORD_LEN 8

//A24 constant
#define CURVE25519_A24 121666

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif


// CONSTANTS taken from ristretto draft: https://datatracker.ietf.org/doc/draft-hdevalence-cfrg-ristretto/

static const field_elem F_ZERO = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000},
    F_ONE = {0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000},
    F_MODULUS = {0xFFFFFFED, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF},
    SQRT_M1 = {0x4A0EA0B0,0xC4EE1B27, 0xAD2FE478, 0x2F431806, 0x3DFBD7A7, 0x2B4D0099, 0x4FC1DF0B, 0x2B832480},
    EDWARDS_D = {0x135978A3, 0x75EB4DCA, 0x4141D8AB, 0x00700A4D, 0x7779E898, 0x8CC74079, 0x2B6FFE73, 0x52036CEE},
    EDWARDS_D2 = {0x26B2F159, 0xEBD69B94, 0x8283B156, 0x00E0149A, 0xEEF3D130, 0x198E80F2, 0x56DFFCE7, 0x2406D9DC},
    INVSQRT_A_MINUS_D  = {0x805D40EA, 0x99C8FDAA, 0x5A4172BE, 0x9D2F1617, 0xFE01D840, 0x16C27B91, 0xCFAFFCA2, 0x786C8905},
    ONE_MINUS_D_SQ = {0x945FC176, 0xE27C09C1, 0xCD5E350F, 0x2C81A138, 0xBE70DFE4, 0x9994ABDD, 0xB2B3E0D7, 0x029072A8},
    D_MINUS_ONE_SQ = {0x44ED4D20, 0x31AD5AAA, 0xB01E1999, 0xD29E4A2C, 0x529B4EEB, 0x4CDCD32F, 0xF66C2241, 0x5968B37A},
    SQRT_AD_MINUS_ONE = {0x497B2E1B, 0x7E97F6A0, 0x1B7854BD, 0xAF9D8E0C, 0x31F5D1FD, 0x0F3CFCC9, 0x2B8348AC, 0x376931BF};

//Curve25519 related functions
void curve25519SetInt(uint32_t *a, uint32_t b);
void curve25519Add(uint32_t *r, const uint32_t *a, const uint32_t *b);
void curve25519AddInt(uint32_t *r, const uint32_t *a, uint32_t b);
void curve25519Sub(uint32_t *r, const uint32_t *a, const uint32_t *b);
void curve25519SubInt(uint32_t *r, const uint32_t *a, uint32_t b);
void curve25519Mul(uint32_t *r, const uint32_t *a, const uint32_t *b);
void curve25519MulInt(uint32_t *r, const uint32_t *a, uint32_t b);
void curve25519Red(uint32_t *r, const uint32_t *a);
void curve25519Sqr(uint32_t *r, const uint32_t *a);
void curve25519Pwr2(uint32_t *r, const uint32_t *a, uint_t n);
void curve25519Inv(uint32_t *r, const uint32_t *a);

uint32_t curve25519Sqrt(uint32_t *r, const uint32_t *a, const uint32_t *b);

void curve25519Copy(uint32_t *a, const uint32_t *b);
void curve25519Swap(uint32_t *a, uint32_t *b, uint32_t c);

void curve25519Select(uint32_t *r, const uint32_t *a, const uint32_t *b,
   uint32_t c);

uint32_t curve25519Comp(const uint32_t *a, const uint32_t *b);

void curve25519Import(uint32_t *a, uint8_t *data);
void curve25519Export(uint32_t *a, uint8_t *data);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif

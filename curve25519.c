// 20230627 MD, upravene pre samostatnu kompilaciu 

/**
  * @file curve25519.c
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
  
 //Switch to the appropriate trace level
 #define TRACE_LEVEL CRYPTO_TRACE_LEVEL
  
 #include <string.h>


 //Dependencies
// #include "core/crypto.h"
// #include "ecc/ec_curves.h"
 #include "curve25519.h"
// #include "debug.h"
  

// MD
//# define htole32(x) (x)
//# define letoh32(x) (x)

 /**
  * @brief Modular addition
  * @param[out] r Resulting integer R = (A + B) mod p
  * @param[in] a An integer such as 0 <= A < p
  * @param[in] b An integer such as 0 <= B < p
  **/
  
 void curve25519Add(uint32_t *r, const uint32_t *a, const uint32_t *b)
 {
    uint_t i;
    uint64_t temp;
  
    //Compute R = A + B
    for(temp = 0, i = 0; i < 8; i++)
    {
       temp += a[i];
       temp += b[i];
       r[i] = temp & 0xFFFFFFFF;
       temp >>= 32;
    }
  
    //Perform modular reduction
    curve25519Red(r, r);
 }
   
  
 /**
  * @brief Modular subtraction
  * @param[out] r Resulting integer R = (A - B) mod p
  * @param[in] a An integer such as 0 <= A < p
  * @param[in] b An integer such as 0 <= B < p
  **/
  
 void curve25519Sub(uint32_t *r, const uint32_t *a, const uint32_t *b)
 {
    uint_t i;
    int64_t temp;
  
    //Compute R = A - 19 - B
    for(temp = -19, i = 0; i < 8; i++)
    {
       temp += a[i];
       temp -= b[i];
       r[i] = temp & 0xFFFFFFFF;
       temp >>= 32;
    }
  
    //Compute R = A + (2^255 - 19) - B
    r[7] += 0x80000000;
  
    //Perform modular reduction
    curve25519Red(r, r);
 }
  
  
  
 /**
  * @brief Modular multiplication
  * @param[out] r Resulting integer R = (A * B) mod p
  * @param[in] a An integer such as 0 <= A < p
  * @param[in] b An integer such as 0 <= B < p
  **/
  
 void curve25519Mul(uint32_t *r, const uint32_t *a, const uint32_t *b)
 {
    uint_t i;
    uint_t j;
    uint64_t c;
    uint64_t temp;
    uint32_t u[16];
  
    //Initialize variables
    temp = 0;
    c = 0;
  
    //Comba's method is used to perform multiplication
    for(i = 0; i < 16; i++)
    {
       //The algorithm computes the products, column by column
       if(i < 8)
       {
          //Inner loop
          for(j = 0; j <= i; j++)
          {
             temp += (uint64_t) a[j] * b[i - j];
             c += temp >> 32;
             temp &= 0xFFFFFFFF;
          }
       }
       else
       {
          //Inner loop
          for(j = i - 7; j < 8; j++)
          {
             temp += (uint64_t) a[j] * b[i - j];
             c += temp >> 32;
             temp &= 0xFFFFFFFF;
          }
       }
  
       //At the bottom of each column, the final result is written to memory
       u[i] = temp & 0xFFFFFFFF;
  
       //Propagate the carry upwards
       temp = c & 0xFFFFFFFF;
       c >>= 32;
    }
  
    //Reduce bit 255 (2^255 = 19 mod p)
    temp = (u[7] >> 31) * 19;
    //Mask the most significant bit
    u[7] &= 0x7FFFFFFF;
  
    //Perform fast modular reduction (first pass)
    for(i = 0; i < 8; i++)
    {
       temp += u[i];
       temp += (uint64_t) u[i + 8] * 38;
       u[i] = temp & 0xFFFFFFFF;
       temp >>= 32;
    }
  
    //Reduce bit 256 (2^256 = 38 mod p)
    temp *= 38;
    //Reduce bit 255 (2^255 = 19 mod p)
    temp += (u[7] >> 31) * 19;
    //Mask the most significant bit
    u[7] &= 0x7FFFFFFF;
  
    //Perform fast modular reduction (second pass)
    for(i = 0; i < 8; i++)
    {
       temp += u[i];
       u[i] = temp & 0xFFFFFFFF;
       temp >>= 32;
    }
  
    //Reduce non-canonical values
    curve25519Red(r, u);
 }

  
  
 /**
  * @brief Modular squaring
  * @param[out] r Resulting integer R = (A ^ 2) mod p
  * @param[in] a An integer such as 0 <= A < p
  **/
  
 void curve25519Sqr(uint32_t *r, const uint32_t *a)
 {
    //Compute R = (A ^ 2) mod p
    curve25519Mul(r, a, a);
 }
  
  
 /**
  * @brief Raise an integer to power 2^n
  * @param[out] r Resulting integer R = (A ^ (2^n)) mod p
  * @param[in] a An integer such as 0 <= A < p
  * @param[in] n An integer such as n >= 1
  **/
  
 void curve25519Pwr2(uint32_t *r, const uint32_t *a, uint_t n)
 {
    uint_t i;
  
    //Pre-compute (A ^ 2) mod p
    curve25519Sqr(r, a);
  
    //Compute R = (A ^ (2^n)) mod p
    for(i = 1; i < n; i++)
    {
       curve25519Sqr(r, r);
    }
 }
  
  
 /**
  * @brief Modular reduction
  * @param[out] r Resulting integer R = A mod p
  * @param[in] a An integer such as 0 <= A < (2 * p)
  **/
  
 void curve25519Red(uint32_t *r, const uint32_t *a)
 {
    uint_t i;
    uint64_t temp;
    uint32_t b[8];
  
    //Compute B = A + 19
    for(temp = 19, i = 0; i < 8; i++)
    {
       temp += a[i];
       b[i] = temp & 0xFFFFFFFF;
       temp >>= 32;
    }
  
    //Compute B = A - (2^255 - 19)
    b[7] -= 0x80000000;
  
    //If B < (2^255 - 19) then R = B, else R = A
    curve25519Select(r, b, a, (b[7] & 0x80000000) >> 31);
 }
    
  
 /**
  * @brief Copy an integer
  * @param[out] a Pointer to the destination integer
  * @param[in] b Pointer to the source integer
  **/
  
 void curve25519Copy(uint32_t *a, const uint32_t *b)
 {
    uint_t i;
  
    //Copy the value of the integer
    for(i = 0; i < 8; i++)
    {
       a[i] = b[i];
    }
 }
  
  
 /**
  * @brief Conditional swap
  * @param[in,out] a Pointer to the first integer
  * @param[in,out] b Pointer to the second integer
  * @param[in] c Condition variable
  **/
  
 void curve25519Swap(uint32_t *a, uint32_t *b, uint32_t c)
 {
    uint_t i;
    uint32_t mask;
    uint32_t dummy;
  
    //The mask is the all-1 or all-0 word
    mask = ~c + 1; 
 
    //Conditional swap
    for(i = 0; i < 8; i++)
    {
       //Constant time implementation
       dummy = mask & (a[i] ^ b[i]);
       a[i] ^= dummy;
       b[i] ^= dummy;
    }
 }
  
  
 /**
  * @brief Select an integer
  * @param[out] r Pointer to the destination integer
  * @param[in] a Pointer to the first source integer
  * @param[in] b Pointer to the second source integer
  * @param[in] c Condition variable
  **/
  
 void curve25519Select(uint32_t *r, const uint32_t *a, const uint32_t *b,
    uint32_t c)
 {
    uint_t i;
    uint32_t mask;
  
    //The mask is the all-1 or all-0 word
    mask = c - 1;
  
    //Select between A and B
    for(i = 0; i < 8; i++)
    {
       //Constant time implementation
       r[i] = (a[i] & mask) | (b[i] & ~mask);
    }
 }
  
  
 /**
  * @brief Compare integers
  * @param[in] a Pointer to the first integer
  * @param[in] b Pointer to the second integer
  * @return The function returns 0 if the A = B, else 1
  **/
  
 uint32_t curve25519Comp(const uint32_t *a, const uint32_t *b)
 {
    uint_t i;
    uint32_t mask;
  
    //Initialize mask
    mask = 0;
  
    //Compare A and B
    for(i = 0; i < 8; i++)
    {
       //Constant time implementation
       mask |= a[i] ^ b[i];
    }
  
    //Return 0 if A = B, else 1
    return (((uint32_t) (mask | (~mask + 1))) >> 31); 
 }
  
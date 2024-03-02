// ******************************************************************
// ----------------- TECHNICAL UNIVERSITY OF KOSICE -----------------
// ---Department of Electronics and Multimedia Telecommunications ---
// -------- FACULTY OF ELECTRICAL ENGINEERING AND INFORMATICS -------
// ------------ THIS CODE IS A PART OF A MASTER'S THESIS ------------
// ------------------------- Master thesis --------------------------
// -----------------Patrik Zelenak & Milos Drutarovsky --------------
// ---------------------------version 0.2.1 -------------------------
// --------------------------- 04-02-2024 ---------------------------
// ******************************************************************

/**
  * This file serves as a test file for our implementation of 
  * ristretto255. In our impl. of Ristretto255 we use Cyclone crypto
  * library for internal calculation in GF(2^255-19). While 
  * constructing our code, we followed latest version of ristretto 
  * draft (https://datatracker.ietf.org/doc/draft-hdevalence-cfrg-ristretto/).
  * All ristretto related test vectors are taken from draft above. 
  * Test file also includes test for modular inverse (mod L) 
  * taken from MonoCypher. Note that modular inverse can be performed
  * in 2 ways; 
  * using Barrett reduction - `inverse_mod_l()` or more efficient
  * Montgomery reduction - `crypto_x25519_inverse() `.
  * 
**/


#include "ristretto255.h"
#include "gf25519.h"
#include "modl.h"
#include "test_config.h"

#define pack25519 pack
#define unpack25519 unpack

int main(){
    
// if 1, small endian is choosed
#ifndef BIGENDIAN_FLAG

    // Non-canonical field encodings.
    u8 non_canonical_vectors[4][BYTES_ELEM_SIZE] = {
        {0x00, 0xff, 0xff, 0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
        {0xff, 0xff, 0xff, 0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f},
        {0xf3, 0xff, 0xff, 0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f},
        {0xed, 0xff, 0xff, 0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f}
    };

    // Non-square x^2
    u8 non_square_x2[8][BYTES_ELEM_SIZE] = {
        {0x26,0x94,0x8d,0x35,0xca,0x62,0xe6,0x43,0xe2,0x6a,0x83,0x17,0x73,0x32,0xe6,0xb6,0xaf,0xeb,0x9d,0x08,0xe4,0x26,0x8b,0x65,0x0f,0x1f,0x5b,0xbd,0x8d,0x81,0xd3,0x71},
        {0x4e,0xac,0x07,0x7a,0x71,0x3c,0x57,0xb4,0xf4,0x39,0x76,0x29,0xa4,0x14,0x59,0x82,0xc6,0x61,0xf4,0x80,0x44,0xdd,0x3f,0x96,0x42,0x7d,0x40,0xb1,0x47,0xd9,0x74,0x2f},
        {0xde,0x6a,0x7b,0x00,0xde,0xad,0xc7,0x88,0xeb,0x6b,0x6c,0x8d,0x20,0xc0,0xae,0x96,0xc2,0xf2,0x01,0x90,0x78,0xfa,0x60,0x4f,0xee,0x5b,0x87,0xd6,0xe9,0x89,0xad,0x7b},
        {0xbc,0xab,0x47,0x7b,0xe2,0x08,0x61,0xe0,0x1e,0x4a,0x0e,0x29,0x52,0x84,0x14,0x6a,0x51,0x01,0x50,0xd9,0x81,0x77,0x63,0xca,0xf1,0xa6,0xf4,0xb4,0x22,0xd6,0x70,0x42},
        {0x2a,0x29,0x2d,0xf7,0xe3,0x2c,0xab,0xab,0xbd,0x9d,0xe0,0x88,0xd1,0xd1,0xab,0xec,0x9f,0xc0,0x44,0x0f,0x63,0x7e,0xd2,0xfb,0xa1,0x45,0x09,0x4d,0xc1,0x4b,0xea,0x08},
        {0xf4,0xa9,0xe5,0x34,0xfc,0x0d,0x21,0x6c,0x44,0xb2,0x18,0xfa,0x0c,0x42,0xd9,0x96,0x35,0xa0,0x12,0x7e,0xe2,0xe5,0x3c,0x71,0x2f,0x70,0x60,0x96,0x49,0xfd,0xff,0x22},
        {0x82,0x68,0x43,0x6f,0x8c,0x41,0x26,0x19,0x6c,0xf6,0x4b,0x3c,0x7d,0xdb,0xda,0x90,0x74,0x6a,0x37,0x86,0x25,0xf9,0x81,0x3d,0xd9,0xb8,0x45,0x70,0x77,0x25,0x67,0x31},
        {0x28,0x10,0xe5,0xcb,0xc2,0xcc,0x4d,0x4e,0xec,0xe5,0x4f,0x61,0xc6,0xf6,0x97,0x58,0xe2,0x89,0xaa,0x7a,0xb4,0x40,0xb3,0xcb,0xea,0xa2,0x19,0x95,0xc2,0xf4,0x23,0x2b}
    };

    //  Negative xy value
    u8 negative_xy[8][BYTES_ELEM_SIZE] = {
        {0x3e,0xb8,0x58,0xe7,0x8f,0x5a,0x72,0x54,0xd8,0xc9,0x73,0x11,0x74,0xa9,0x4f,0x76,0x75,0x5f,0xd3,0x94,0x1c,0x0a,0xc9,0x37,0x35,0xc0,0x7b,0xa1,0x45,0x79,0x63,0x0e},
        {0xa4,0x5f,0xdc,0x55,0xc7,0x64,0x48,0xc0,0x49,0xa1,0xab,0x33,0xf1,0x70,0x23,0xed,0xfb,0x2b,0xe3,0x58,0x1e,0x9c,0x7a,0xad,0xe8,0xa6,0x12,0x52,0x15,0xe0,0x42,0x20},
        {0xd4,0x83,0xfe,0x81,0x3c,0x6b,0xa6,0x47,0xeb,0xbf,0xd3,0xec,0x41,0xad,0xca,0x1c,0x61,0x30,0xc2,0xbe,0xee,0xe9,0xd9,0xbf,0x06,0x5c,0x8d,0x15,0x1c,0x5f,0x39,0x6e},
        {0x8a,0x2e,0x1d,0x30,0x05,0x01,0x98,0xc6,0x5a,0x54,0x48,0x31,0x23,0x96,0x0c,0xcc,0x38,0xae,0xf6,0x84,0x8e,0x1e,0xc8,0xf5,0xf7,0x80,0xe8,0x52,0x37,0x69,0xba,0x32},
        {0x32,0x88,0x84,0x62,0xf8,0xb4,0x86,0xc6,0x8a,0xd7,0xdd,0x96,0x10,0xbe,0x51,0x92,0xbb,0xea,0xf3,0xb4,0x43,0x95,0x1a,0xc1,0xa8,0x11,0x84,0x19,0xd9,0xfa,0x09,0x7b},
        {0x22,0x71,0x42,0x50,0x1b,0x9d,0x43,0x55,0xcc,0xba,0x29,0x04,0x04,0xbd,0xe4,0x15,0x75,0xb0,0x37,0x69,0x3c,0xef,0x1f,0x43,0x8c,0x47,0xf8,0xfb,0xf3,0x5d,0x11,0x65},
        {0x5c,0x37,0xcc,0x49,0x1d,0xa8,0x47,0xcf,0xeb,0x92,0x81,0xd4,0x07,0xef,0xc4,0x1e,0x15,0x14,0x4c,0x87,0x6e,0x01,0x70,0xb4,0x99,0xa9,0x6a,0x22,0xed,0x31,0xe0,0x1e},
        {0x44,0x54,0x25,0x11,0x7c,0xb8,0xc9,0x0e,0xdc,0xbc,0x7c,0x1c,0xc0,0xe7,0x4f,0x74,0x7f,0x2c,0x1e,0xfa,0x56,0x30,0xa9,0x67,0xc6,0x4f,0x28,0x77,0x92,0xa4,0x8a,0x4b},

    };
    // s = -1, which causes y = 0.
    u8 s_minus_1[BYTES_ELEM_SIZE] = {0xec,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f};


    // negative vectors
    u8 test_negative_vectors[8][BYTES_ELEM_SIZE] = {
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
        {0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,},
        {0xed, 0x57, 0xff, 0xd8, 0xc9, 0x14, 0xfb, 0x20, 0x14, 0x71, 0xd1, 0xc3, 0xd2, 0x45, 0xce, 0x3c, 0x74, 0x6f, 0xcb, 0xe6, 0x3a, 0x36, 0x79, 0xd5, 0x1b, 0x6a, 0x51, 0x6e, 0xbe, 0xbe, 0x0e, 0x20, },
        {0xc3, 0x4c, 0x4e, 0x18, 0x26, 0xe5, 0xd4, 0x03, 0xb7, 0x8e, 0x24, 0x6e, 0x88, 0xaa, 0x05, 0x1c, 0x36, 0xcc, 0xf0, 0xaa, 0xfe, 0xbf, 0xfe, 0x13, 0x7d, 0x14, 0x8a, 0x2b, 0xf9, 0x10, 0x45, 0x62, },
        {0xc9, 0x40, 0xe5, 0xa4, 0x40, 0x41, 0x57, 0xcf, 0xb1, 0x62, 0x8b, 0x10, 0x8d, 0xb0, 0x51, 0xa8, 0xd4, 0x39, 0xe1, 0xa4, 0x21, 0x39, 0x4e, 0xc4, 0xeb, 0xcc, 0xb9, 0xec, 0x92, 0xa8, 0xac, 0x78, },
        {0x47, 0xcf, 0xc5, 0x49, 0x7c, 0x53, 0xdc, 0x8e, 0x61, 0xc9, 0x1d, 0x17, 0xfd, 0x62, 0x6f, 0xfb, 0x1c, 0x49, 0xe2, 0xbc, 0xa9, 0x4e, 0xed, 0x05, 0x22, 0x81, 0xb5, 0x10, 0xb1, 0x11, 0x7a, 0x24, },
        {0xf1, 0xc6, 0x16, 0x5d, 0x33, 0x36, 0x73, 0x51, 0xb0, 0xda, 0x8f, 0x6e, 0x45, 0x11, 0x01, 0x0c, 0x68, 0x17, 0x4a, 0x03, 0xb6, 0x58, 0x12, 0x12, 0xc7, 0x1c, 0x0e, 0x1d, 0x02, 0x6c, 0x3c, 0x72, },
        {0x87, 0x26, 0x0f, 0x7a, 0x2f, 0x12, 0x49, 0x51, 0x18, 0x36, 0x0f, 0x02, 0xc2, 0x6a, 0x47, 0x0f, 0x45, 0x0d, 0xad, 0xf3, 0x4a, 0x41, 0x3d, 0x21, 0x04, 0x2b, 0x43, 0xb9, 0xd9, 0x3e, 0x13, 0x09,}
    };


    // P - negative vector
    u8 test_negative_vectors_compl[8][BYTES_ELEM_SIZE] = {
        {0xec, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0x7f},
        {0xec, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0},
        {0x0, 0xa8, 0x0, 0x27,0x36, 0xeb, 0x4, 0xdf,0xeb, 0x8e, 0x2e, 0x3c,0x2d, 0xba, 0x31, 0xc3,0x8b, 0x90, 0x34, 0x19,0xc5, 0xc9, 0x86, 0x2a,0xe4, 0x95, 0xae, 0x91,0x41, 0x41, 0xf1, 0x5f},
        {0x2a, 0xb3, 0xb1, 0xe7,0xd9, 0x1a, 0x2b, 0xfc,0x48, 0x71, 0xdb, 0x91,0x77, 0x55, 0xfa, 0xe3,0xc9, 0x33, 0xf, 0x55,0x1, 0x40, 0x1, 0xec,0x82, 0xeb, 0x75, 0xd4,0x6, 0xef, 0xba, 0x1d},
        {0x24, 0xbf, 0x1a, 0x5b,0xbf, 0xbe, 0xa8, 0x30,0x4e, 0x9d, 0x74, 0xef,0x72, 0x4f, 0xae, 0x57,0x2b, 0xc6, 0x1e, 0x5b,0xde, 0xc6, 0xb1, 0x3b,0x14, 0x33, 0x46, 0x13,0x6d, 0x57, 0x53, 0x7},
        {0xa6, 0x30, 0x3a, 0xb6,0x83, 0xac, 0x23, 0x71,0x9e, 0x36, 0xe2, 0xe8,0x2, 0x9d, 0x90, 0x4,0xe3, 0xb6, 0x1d, 0x43,0x56, 0xb1, 0x12, 0xfa,0xdd, 0x7e, 0x4a, 0xef,0x4e, 0xee, 0x85, 0x5b},
        {0xfc, 0x38, 0xe9, 0xa2,0xcc, 0xc9, 0x8c, 0xae,0x4f, 0x25, 0x70, 0x91,0xba, 0xee, 0xfe, 0xf3,0x97, 0xe8, 0xb5, 0xfc,0x49, 0xa7, 0xed, 0xed,0x38, 0xe3, 0xf1, 0xe2,0xfd, 0x93, 0xc3, 0xd},
        {0x66, 0xd9, 0xf0, 0x85,0xd0, 0xed, 0xb6, 0xae,0xe7, 0xc9, 0xf0, 0xfd,0x3d, 0x95, 0xb8, 0xf0,0xba, 0xf2, 0x52, 0xc,0xb5, 0xbe, 0xc2, 0xde,0xfb, 0xd4, 0xbc, 0x46,0x26, 0xc1, 0xec, 0x76},
    };

    u8 MAP_VECTORS[7][HASH_BYTES_SIZE]= {
        {0x5d, 0x1b, 0xe0, 0x9e, 0x3d, 0xc, 0x82, 0xfc, 0x53, 0x81, 0x12, 0x49, 0xe, 0x35, 0x70, 0x19, 0x79, 0xd9, 0x9e, 0x6, 0xca, 0x3e, 0x2b, 0x5b, 0x54, 0xbf, 0xfe, 0x8b, 0x4d, 0xc7, 0x72, 0xc1, 0x4d, 0x98, 0xb6, 0x96, 0xa1, 0xbb, 0xfb, 0x5c, 0xa3, 0x2c, 0x43, 0x6c, 0xc6, 0x1c, 0x16, 0x56, 0x37, 0x90, 0x30, 0x6c, 0x79, 0xea, 0xca, 0x77, 0x5, 0x66, 0x8b, 0x47, 0xdf, 0xfe, 0x5b, 0xb6},
        {0xf1, 0x16, 0xb3, 0x4b, 0x8f, 0x17, 0xce, 0xb5, 0x6e, 0x87, 0x32, 0xa6, 0xd, 0x91, 0x3d, 0xd1, 0xc, 0xce, 0x47, 0xa6, 0xd5, 0x3b, 0xee, 0x92, 0x4, 0xbe, 0x8b, 0x44, 0xf6, 0x67, 0x8b, 0x27, 0x1, 0x2, 0xa5, 0x69, 0x2, 0xe2, 0x48, 0x8c, 0x46, 0x12, 0xe, 0x92, 0x76, 0xcf, 0xe5, 0x46, 0x38, 0x28, 0x6b, 0x9e, 0x4b, 0x3c, 0xdb, 0x47, 0xb, 0x54, 0x2d, 0x46, 0xc2, 0x6, 0x8d, 0x38},
        {0x84, 0x22, 0xe1, 0xbb, 0xda, 0xab, 0x52, 0x93, 0x8b, 0x81, 0xfd, 0x60, 0x2e, 0xff, 0xb6, 0xf8, 0x91, 0x10, 0xe1, 0xe5, 0x72, 0x8, 0xad, 0x12, 0xd9, 0xad, 0x76, 0x7e, 0x2e, 0x25, 0x51, 0xc, 0x27, 0x14, 0x7, 0x75, 0xf9, 0x33, 0x70, 0x88, 0xb9, 0x82, 0xd8, 0x3d, 0x7f, 0xcf, 0xb, 0x2f, 0xa1, 0xed, 0xff, 0xe5, 0x19, 0x52, 0xcb, 0xe7, 0x36, 0x5e, 0x95, 0xc8, 0x6e, 0xaf, 0x32, 0x5c},
        {0xac, 0x22, 0x41, 0x51, 0x29, 0xb6, 0x14, 0x27, 0xbf, 0x46, 0x4e, 0x17, 0xba, 0xee, 0x8d, 0xb6, 0x59, 0x40, 0xc2, 0x33, 0xb9, 0x8a, 0xfc, 0xe8, 0xd1, 0x7c, 0x57, 0xbe, 0xeb, 0x78, 0x76, 0xc2, 0x15, 0xd, 0x15, 0xaf, 0x1c, 0xb1, 0xfb, 0x82, 0x4b, 0xbd, 0x14, 0x95, 0x5f, 0x2b, 0x57, 0xd0, 0x8d, 0x38, 0x8a, 0xab, 0x43, 0x1a, 0x39, 0x1c, 0xfc, 0x33, 0xd5, 0xba, 0xfb, 0x5d, 0xbb, 0xaf},
        {0x16, 0x5d, 0x69, 0x7a, 0x1e, 0xf3, 0xd5, 0xcf, 0x3c, 0x38, 0x56, 0x5b, 0xee, 0xfc, 0xf8, 0x8c, 0xf, 0x28, 0x2b, 0x8e, 0x7d, 0xbd, 0x28, 0x54, 0x4c, 0x48, 0x34, 0x32, 0xf1, 0xce, 0xc7, 0x67, 0x5d, 0xeb, 0xea, 0x8e, 0xbb, 0x4e, 0x5f, 0xe7, 0xd6, 0xf6, 0xe5, 0xdb, 0x15, 0xf1, 0x55, 0x87, 0xac, 0x4d, 0x4d, 0x4a, 0x1d, 0xe7, 0x19, 0x1e, 0xc, 0x1c, 0xa6, 0x66, 0x4a, 0xbc, 0xc4, 0x13},
        {0xa8, 0x36, 0xe6, 0xc9, 0xa9, 0xca, 0x9f, 0x1e, 0x8d, 0x48, 0x62, 0x73, 0xad, 0x56, 0xa7, 0x8c, 0x70, 0xcf, 0x18, 0xf0, 0xce, 0x10, 0xab, 0xb1, 0xc7, 0x17, 0x2d, 0xdd, 0x60, 0x5d, 0x7f, 0xd2, 0x97, 0x98, 0x54, 0xf4, 0x7a, 0xe1, 0xcc, 0xf2, 0x4, 0xa3, 0x31, 0x2, 0x9, 0x5b, 0x42, 0x0, 0xe5, 0xbe, 0xfc, 0x4, 0x65, 0xac, 0xcc, 0x26, 0x31, 0x75, 0x48, 0x5f, 0xe, 0x17, 0xea, 0x5c},
        {0x2c, 0xdc, 0x11, 0xea, 0xeb, 0x95, 0xda, 0xf0, 0x11, 0x89, 0x41, 0x7c, 0xdd, 0xdb, 0xf9, 0x59, 0x52, 0x99, 0x3a, 0xa9, 0xcb, 0x9c, 0x64, 0xe, 0xb5, 0x5, 0x8d, 0x9, 0x70, 0x2c, 0x74, 0x62, 0x2c, 0x99, 0x65, 0xa6, 0x97, 0xa3, 0xb3, 0x45, 0xec, 0x24, 0xee, 0x56, 0x33, 0x5b, 0x55, 0x6e, 0x67, 0x7b, 0x30, 0xe6, 0xf9, 0xa, 0xc7, 0x7d, 0x78, 0x10, 0x64, 0xf8, 0x66, 0xa3, 0xc9, 0x82}
    };

    u8 MAP_VECTORS_RESULT[7][BYTES_ELEM_SIZE]= {
        {0x30, 0x66, 0xf8, 0x2a, 0x1a, 0x74, 0x7d, 0x45, 0x12, 0xd, 0x17, 0x40, 0xf1, 0x43, 0x58, 0x53, 0x1a, 0x8f, 0x4, 0xbb, 0xff, 0xe6, 0xa8, 0x19, 0xf8, 0x6d, 0xfe, 0x50, 0xf4, 0x4a, 0xa, 0x46},
        {0xf2, 0x6e, 0x5b, 0x6f, 0x7d, 0x36, 0x2d, 0x2d, 0x2a, 0x94, 0xc5, 0xd0, 0xe7, 0x60, 0x2c, 0xb4, 0x77, 0x3c, 0x95, 0xa2, 0xe5, 0xc3, 0x1a, 0x64, 0xf1, 0x33, 0x18, 0x9f, 0xa7, 0x6e, 0xd6, 0x1b},
        {0x0, 0x6c, 0xcd, 0x2a, 0x9e, 0x68, 0x67, 0xe6, 0xa2, 0xc5, 0xce, 0xa8, 0x3d, 0x33, 0x2, 0xcc, 0x9d, 0xe1, 0x28, 0xdd, 0x2a, 0x9a, 0x57, 0xdd, 0x8e, 0xe7, 0xb9, 0xd7, 0xff, 0xe0, 0x28, 0x26},
        {0xf8, 0xf0, 0xc8, 0x7c, 0xf2, 0x37, 0x95, 0x3c, 0x58, 0x90, 0xae, 0xc3, 0x99, 0x81, 0x69, 0x0, 0x5d, 0xae, 0x3e, 0xca, 0x1f, 0xbb, 0x4, 0x54, 0x8c, 0x63, 0x59, 0x53, 0xc8, 0x17, 0xf9, 0x2a},
        {0xae, 0x81, 0xe7, 0xde, 0xdf, 0x20, 0xa4, 0x97, 0xe1, 0xc, 0x30, 0x4a, 0x76, 0x5c, 0x17, 0x67, 0xa4, 0x2d, 0x6e, 0x6, 0x2, 0x97, 0x58, 0xd2, 0xd7, 0xe8, 0xef, 0x7c, 0xc4, 0xc4, 0x11, 0x79},
        {0xe2, 0x70, 0x56, 0x52, 0xff, 0x9f, 0x5e, 0x44, 0xd3, 0xe8, 0x41, 0xbf, 0x1c, 0x25, 0x1c, 0xf7, 0xdd, 0xdb, 0x77, 0xd1, 0x40, 0x87, 0xd, 0x1a, 0xb2, 0xed, 0x64, 0xf1, 0xa9, 0xce, 0x86, 0x28},
        {0x80, 0xbd, 0x7, 0x26, 0x25, 0x11, 0xcd, 0xde, 0x48, 0x63, 0xf8, 0xa7, 0x43, 0x4c, 0xef, 0x69, 0x67, 0x50, 0x68, 0x1c, 0xb9, 0x51, 0xe, 0xea, 0x55, 0x70, 0x88, 0xf7, 0x6d, 0x9e, 0x50, 0x65}
    };
    
    u8 RISTRETTO255_BASEPOINT[BYTES_ELEM_SIZE] = {
            0xe2, 0xf2, 0xae, 0xa, 
            0x6a, 0xbc, 0x4e, 0x71, 
            0xa8, 0x84, 0xa9, 0x61,
            0xc5, 0x0, 0x51, 0x5f,
            0x58, 0xe3, 0xb, 0x6a, 
            0xa5, 0x82, 0xdd, 0x8d, 
            0xb6, 0xa6, 0x59, 0x45, 
            0xe0, 0x8d, 0x2d, 0x76
        };
    u8 SMALL_MULTIPLES_OF_GENERATOR_VECTORS[16][BYTES_ELEM_SIZE] = {
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0xe2,0xf2,0xae,0x0a,0x6a,0xbc,0x4e,0x71,0xa8,0x84,0xa9,0x61,0xc5,0x00,0x51,0x5f,0x58,0xe3,0x0b,0x6a,0xa5,0x82,0xdd,0x8d,0xb6,0xa6,0x59,0x45,0xe0,0x8d,0x2d,0x76},
        {0x6a,0x49,0x32,0x10,0xf7,0x49,0x9c,0xd1,0x7f,0xec,0xb5,0x10,0xae,0x0c,0xea,0x23,0xa1,0x10,0xe8,0xd5,0xb9,0x01,0xf8,0xac,0xad,0xd3,0x09,0x5c,0x73,0xa3,0xb9,0x19},
        {0x94,0x74,0x1f,0x5d,0x5d,0x52,0x75,0x5e,0xce,0x4f,0x23,0xf0,0x44,0xee,0x27,0xd5,0xd1,0xea,0x1e,0x2b,0xd1,0x96,0xb4,0x62,0x16,0x6b,0x16,0x15,0x2a,0x9d,0x02,0x59},
        {0xda,0x80,0x86,0x27,0x73,0x35,0x8b,0x46,0x6f,0xfa,0xdf,0xe0,0xb3,0x29,0x3a,0xb3,0xd9,0xfd,0x53,0xc5,0xea,0x6c,0x95,0x53,0x58,0xf5,0x68,0x32,0x2d,0xaf,0x6a,0x57},
        {0xe8,0x82,0xb1,0x31,0x01,0x6b,0x52,0xc1,0xd3,0x33,0x70,0x80,0x18,0x7c,0xf7,0x68,0x42,0x3e,0xfc,0xcb,0xb5,0x17,0xbb,0x49,0x5a,0xb8,0x12,0xc4,0x16,0x0f,0xf4,0x4e},
        {0xf6,0x47,0x46,0xd3,0xc9,0x2b,0x13,0x05,0x0e,0xd8,0xd8,0x02,0x36,0xa7,0xf0,0x00,0x7c,0x3b,0x3f,0x96,0x2f,0x5b,0xa7,0x93,0xd1,0x9a,0x60,0x1e,0xbb,0x1d,0xf4,0x03},
        {0x44,0xf5,0x35,0x20,0x92,0x6e,0xc8,0x1f,0xbd,0x5a,0x38,0x78,0x45,0xbe,0xb7,0xdf,0x85,0xa9,0x6a,0x24,0xec,0xe1,0x87,0x38,0xbd,0xcf,0xa6,0xa7,0x82,0x2a,0x17,0x6d},
        {0x90,0x32,0x93,0xd8,0xf2,0x28,0x7e,0xbe,0x10,0xe2,0x37,0x4d,0xc1,0xa5,0x3e,0x0b,0xc8,0x87,0xe5,0x92,0x69,0x9f,0x02,0xd0,0x77,0xd5,0x26,0x3c,0xdd,0x55,0x60,0x1c},
        {0x02,0x62,0x2a,0xce,0x8f,0x73,0x03,0xa3,0x1c,0xaf,0xc6,0x3f,0x8f,0xc4,0x8f,0xdc,0x16,0xe1,0xc8,0xc8,0xd2,0x34,0xb2,0xf0,0xd6,0x68,0x52,0x82,0xa9,0x07,0x60,0x31},
        {0x20,0x70,0x6f,0xd7,0x88,0xb2,0x72,0x0a,0x1e,0xd2,0xa5,0xda,0xd4,0x95,0x2b,0x01,0xf4,0x13,0xbc,0xf0,0xe7,0x56,0x4d,0xe8,0xcd,0xc8,0x16,0x68,0x9e,0x2d,0xb9,0x5f},
        {0xbc,0xe8,0x3f,0x8b,0xa5,0xdd,0x2f,0xa5,0x72,0x86,0x4c,0x24,0xba,0x18,0x10,0xf9,0x52,0x2b,0xc6,0x00,0x4a,0xfe,0x95,0x87,0x7a,0xc7,0x32,0x41,0xca,0xfd,0xab,0x42},
        {0xe4,0x54,0x9e,0xe1,0x6b,0x9a,0xa0,0x30,0x99,0xca,0x20,0x8c,0x67,0xad,0xaf,0xca,0xfa,0x4c,0x3f,0x3e,0x4e,0x53,0x03,0xde,0x60,0x26,0xe3,0xca,0x8f,0xf8,0x44,0x60},
        {0xaa,0x52,0xe0,0x00,0xdf,0x2e,0x16,0xf5,0x5f,0xb1,0x03,0x2f,0xc3,0x3b,0xc4,0x27,0x42,0xda,0xd6,0xbd,0x5a,0x8f,0xc0,0xbe,0x01,0x67,0x43,0x6c,0x59,0x48,0x50,0x1f},
        {0x46,0x37,0x6b,0x80,0xf4,0x09,0xb2,0x9d,0xc2,0xb5,0xf6,0xf0,0xc5,0x25,0x91,0x99,0x08,0x96,0xe5,0x71,0x6f,0x41,0x47,0x7c,0xd3,0x00,0x85,0xab,0x7f,0x10,0x30,0x1e},
        {0xe0,0xc4,0x18,0xf7,0xc8,0xd9,0xc4,0xcd,0xd7,0x39,0x5b,0x93,0xea,0x12,0x4f,0x3a,0xd9,0x90,0x21,0xbb,0x68,0x1d,0xfc,0x33,0x02,0xa9,0xd9,0x9a,0x2e,0x53,0xe6,0x4e},

    };
    #else
    u8 non_canonical_vectors[4][BYTES_ELEM_SIZE] = {
        {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff, 0xff, 0xff, 0xff,0x00},
        {0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff, 0xff, 0xff, 0xff,0xff},
        {0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff, 0xff, 0xff, 0xff,0xf3},
        {0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff, 0xff, 0xff, 0xff,0xed},
    };

    u8 non_square_x2[8][BYTES_ELEM_SIZE] = {
        {0x71,0xd3,0x81,0x8d,0xbd,0x5b,0x1f,0x0f,0x65,0x8b,0x26,0xe4,0x08,0x9d,0xeb,0xaf,0xb6,0xe6,0x32,0x73,0x17,0x83,0x6a,0xe2,0x43,0xe6,0x62,0xca,0x35,0x8d,0x94,0x26},
        {0x2f,0x74,0xd9,0x47,0xb1,0x40,0x7d,0x42,0x96,0x3f,0xdd,0x44,0x80,0xf4,0x61,0xc6,0x82,0x59,0x14,0xa4,0x29,0x76,0x39,0xf4,0xb4,0x57,0x3c,0x71,0x7a,0x07,0xac,0x4e},
        {0x7b,0xad,0x89,0xe9,0xd6,0x87,0x5b,0xee,0x4f,0x60,0xfa,0x78,0x90,0x01,0xf2,0xc2,0x96,0xae,0xc0,0x20,0x8d,0x6c,0x6b,0xeb,0x88,0xc7,0xad,0xde,0x00,0x7b,0x6a,0xde},
        {0x42,0x70,0xd6,0x22,0xb4,0xf4,0xa6,0xf1,0xca,0x63,0x77,0x81,0xd9,0x50,0x01,0x51,0x6a,0x14,0x84,0x52,0x29,0x0e,0x4a,0x1e,0xe0,0x61,0x08,0xe2,0x7b,0x47,0xab,0xbc},
        {0x08,0xea,0x4b,0xc1,0x4d,0x09,0x45,0xa1,0xfb,0xd2,0x7e,0x63,0x0f,0x44,0xc0,0x9f,0xec,0xab,0xd1,0xd1,0x88,0xe0,0x9d,0xbd,0xab,0xab,0x2c,0xe3,0xf7,0x2d,0x29,0x2a},
        {0x22,0xff,0xfd,0x49,0x96,0x60,0x70,0x2f,0x71,0x3c,0xe5,0xe2,0x7e,0x12,0xa0,0x35,0x96,0xd9,0x42,0x0c,0xfa,0x18,0xb2,0x44,0x6c,0x21,0x0d,0xfc,0x34,0xe5,0xa9,0xf4},
        {0x31,0x67,0x25,0x77,0x70,0x45,0xb8,0xd9,0x3d,0x81,0xf9,0x25,0x86,0x37,0x6a,0x74,0x90,0xda,0xdb,0x7d,0x3c,0x4b,0xf6,0x6c,0x19,0x26,0x41,0x8c,0x6f,0x43,0x68,0x82},
        {0x2b,0x23,0xf4,0xc2,0x95,0x19,0xa2,0xea,0xcb,0xb3,0x40,0xb4,0x7a,0xaa,0x89,0xe2,0x58,0x97,0xf6,0xc6,0x61,0x4f,0xe5,0xec,0x4e,0x4d,0xcc,0xc2,0xcb,0xe5,0x10,0x28}
        };
    
    u8 negative_xy[8][BYTES_ELEM_SIZE] = {
    {0x0e,0x63,0x79,0x45,0xa1,0x7b,0xc0,0x35,0x37,0xc9,0x0a,0x1c,0x94,0xd3,0x5f,0x75,0x76,0x4f,0xa9,0x74,0x11,0x73,0xc9,0xd8,0x54,0x72,0x5a,0x8f,0xe7,0x58,0xb8,0x3e},
    {0x20,0x42,0xe0,0x15,0x52,0x12,0xa6,0xe8,0xad,0x7a,0x9c,0x1e,0x58,0xe3,0x2b,0xfb,0xed,0x23,0x70,0xf1,0x33,0xab,0xa1,0x49,0xc0,0x48,0x64,0xc7,0x55,0xdc,0x5f,0xa4},
    {0x6e,0x39,0x5f,0x1c,0x15,0x8d,0x5c,0x06,0xbf,0xd9,0xe9,0xee,0xbe,0xc2,0x30,0x61,0x1c,0xca,0xad,0x41,0xec,0xd3,0xbf,0xeb,0x47,0xa6,0x6b,0x3c,0x81,0xfe,0x83,0xd4},
    {0x32,0xba,0x69,0x37,0x52,0xe8,0x80,0xf7,0xf5,0xc8,0x1e,0x8e,0x84,0xf6,0xae,0x38,0xcc,0x0c,0x96,0x23,0x31,0x48,0x54,0x5a,0xc6,0x98,0x01,0x05,0x30,0x1d,0x2e,0x8a},
    {0x7b,0x09,0xfa,0xd9,0x19,0x84,0x11,0xa8,0xc1,0x1a,0x95,0x43,0xb4,0xf3,0xea,0xbb,0x92,0x51,0xbe,0x10,0x96,0xdd,0xd7,0x8a,0xc6,0x86,0xb4,0xf8,0x62,0x84,0x88,0x32},
    {0x65,0x11,0x5d,0xf3,0xfb,0xf8,0x47,0x8c,0x43,0x1f,0xef,0x3c,0x69,0x37,0xb0,0x75,0x15,0xe4,0xbd,0x04,0x04,0x29,0xba,0xcc,0x55,0x43,0x9d,0x1b,0x50,0x42,0x71,0x22},
    {0x1e,0xe0,0x31,0xed,0x22,0x6a,0xa9,0x99,0xb4,0x70,0x01,0x6e,0x87,0x4c,0x14,0x15,0x1e,0xc4,0xef,0x07,0xd4,0x81,0x92,0xeb,0xcf,0x47,0xa8,0x1d,0x49,0xcc,0x37,0x5c},
    {0x4b,0x8a,0xa4,0x92,0x77,0x28,0x4f,0xc6,0x67,0xa9,0x30,0x56,0xfa,0x1e,0x2c,0x7f,0x74,0x4f,0xe7,0xc0,0x1c,0x7c,0xbc,0xdc,0x0e,0xc9,0xb8,0x7c,0x11,0x25,0x54,0x44}
    };

    u8 s_minus_1[BYTES_ELEM_SIZE] = {0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xec};

    u8 test_negative_vectors[8][BYTES_ELEM_SIZE] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
    {0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x01},
    {0x20,0x0e,0xbe,0xbe,0x6e,0x51,0x6a,0x1b,0xd5,0x79,0x36,0x3a,0xe6,0xcb,0x6f,0x74,0x3c,0xce,0x45,0xd2,0xc3,0xd1,0x71,0x14,0x20,0xfb,0x14,0xc9,0xd8,0xff,0x57,0xed},
    {0x62,0x45,0x10,0xf9,0x2b,0x8a,0x14,0x7d,0x13,0xfe,0xbf,0xfe,0xaa,0xf0,0xcc,0x36,0x1c,0x05,0xaa,0x88,0x6e,0x24,0x8e,0xb7,0x03,0xd4,0xe5,0x26,0x18,0x4e,0x4c,0xc3},
    {0x78,0xac,0xa8,0x92,0xec,0xb9,0xcc,0xeb,0xc4,0x4e,0x39,0x21,0xa4,0xe1,0x39,0xd4,0xa8,0x51,0xb0,0x8d,0x10,0x8b,0x62,0xb1,0xcf,0x57,0x41,0x40,0xa4,0xe5,0x40,0xc9},
    {0x24,0x7a,0x11,0xb1,0x10,0xb5,0x81,0x22,0x05,0xed,0x4e,0xa9,0xbc,0xe2,0x49,0x1c,0xfb,0x6f,0x62,0xfd,0x17,0x1d,0xc9,0x61,0x8e,0xdc,0x53,0x7c,0x49,0xc5,0xcf,0x47},
    {0x72,0x3c,0x6c,0x02,0x1d,0x0e,0x1c,0xc7,0x12,0x12,0x58,0xb6,0x03,0x4a,0x17,0x68,0x0c,0x01,0x11,0x45,0x6e,0x8f,0xda,0xb0,0x51,0x73,0x36,0x33,0x5d,0x16,0xc6,0xf1},
    {0x09,0x13,0x3e,0xd9,0xb9,0x43,0x2b,0x04,0x21,0x3d,0x41,0x4a,0xf3,0xad,0x0d,0x45,0x0f,0x47,0x6a,0xc2,0x02,0x0f,0x36,0x18,0x51,0x49,0x12,0x2f,0x7a,0x0f,0x26,0x87}
    };
    
    u8 test_negative_vectors_compl[8][BYTES_ELEM_SIZE] = {
    { 0x7f, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff,0xec},
    { 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0,0xec},
    { 0x5f, 0xf1, 0x41,0x41, 0x91, 0xae, 0x95,0xe4, 0x2a, 0x86, 0xc9,0xc5, 0x19, 0x34, 0x90,0x8b, 0xc3, 0x31, 0xba,0x2d, 0x3c, 0x2e, 0x8e,0xeb, 0xdf, 0x4, 0xeb,0x36, 0x27, 0x0, 0xa8,0x0},
    { 0x1d, 0xba, 0xef,0x6, 0xd4, 0x75, 0xeb,0x82, 0xec, 0x1, 0x40,0x1, 0x55, 0xf, 0x33,0xc9, 0xe3, 0xfa, 0x55,0x77, 0x91, 0xdb, 0x71,0x48, 0xfc, 0x2b, 0x1a,0xd9, 0xe7, 0xb1, 0xb3,0x2a},
    { 0x7, 0x53, 0x57,0x6d, 0x13, 0x46, 0x33,0x14, 0x3b, 0xb1, 0xc6,0xde, 0x5b, 0x1e, 0xc6,0x2b, 0x57, 0xae, 0x4f,0x72, 0xef, 0x74, 0x9d,0x4e, 0x30, 0xa8, 0xbe,0xbf, 0x5b, 0x1a, 0xbf,0x24},
    { 0x5b, 0x85, 0xee,0x4e, 0xef, 0x4a, 0x7e,0xdd, 0xfa, 0x12, 0xb1,0x56, 0x43, 0x1d, 0xb6,0xe3, 0x4, 0x90, 0x9d,0x2, 0xe8, 0xe2, 0x36,0x9e, 0x71, 0x23, 0xac,0x83, 0xb6, 0x3a, 0x30,0xa6},
    { 0xd, 0xc3, 0x93,0xfd, 0xe2, 0xf1, 0xe3,0x38, 0xed, 0xed, 0xa7,0x49, 0xfc, 0xb5, 0xe8,0x97, 0xf3, 0xfe, 0xee,0xba, 0x91, 0x70, 0x25,0x4f, 0xae, 0x8c, 0xc9,0xcc, 0xa2, 0xe9, 0x38,0xfc},
    { 0x76, 0xec, 0xc1,0x26, 0x46, 0xbc, 0xd4,0xfb, 0xde, 0xc2, 0xbe,0xb5, 0xc, 0x52, 0xf2,0xba, 0xf0, 0xb8, 0x95,0x3d, 0xfd, 0xf0, 0xc9,0xe7, 0xae, 0xb6, 0xed,0xd0, 0x85, 0xf0, 0xd9,0x66},
    };

    u8 MAP_VECTORS[7][HASH_BYTES_SIZE]= {
    { 0xb6, 0x5b, 0xfe, 0xdf, 0x47, 0x8b, 0x66, 0x5, 0x77, 0xca, 0xea, 0x79, 0x6c, 0x30, 0x90, 0x37, 0x56, 0x16, 0x1c, 0xc6, 0x6c, 0x43, 0x2c, 0xa3, 0x5c, 0xfb, 0xbb, 0xa1, 0x96, 0xb6, 0x98, 0x4d, 0xc1, 0x72, 0xc7, 0x4d, 0x8b, 0xfe, 0xbf, 0x54, 0x5b, 0x2b, 0x3e, 0xca, 0x6, 0x9e, 0xd9, 0x79, 0x19, 0x70, 0x35, 0xe, 0x49, 0x12, 0x81, 0x53, 0xfc, 0x82, 0xc, 0x3d, 0x9e, 0xe0, 0x1b,0x5d},
    { 0x38, 0x8d, 0x6, 0xc2, 0x46, 0x2d, 0x54, 0xb, 0x47, 0xdb, 0x3c, 0x4b, 0x9e, 0x6b, 0x28, 0x38, 0x46, 0xe5, 0xcf, 0x76, 0x92, 0xe, 0x12, 0x46, 0x8c, 0x48, 0xe2, 0x2, 0x69, 0xa5, 0x2, 0x1, 0x27, 0x8b, 0x67, 0xf6, 0x44, 0x8b, 0xbe, 0x4, 0x92, 0xee, 0x3b, 0xd5, 0xa6, 0x47, 0xce, 0xc, 0xd1, 0x3d, 0x91, 0xd, 0xa6, 0x32, 0x87, 0x6e, 0xb5, 0xce, 0x17, 0x8f, 0x4b, 0xb3, 0x16,0xf1},
    { 0x5c, 0x32, 0xaf, 0x6e, 0xc8, 0x95, 0x5e, 0x36, 0xe7, 0xcb, 0x52, 0x19, 0xe5, 0xff, 0xed, 0xa1, 0x2f, 0xb, 0xcf, 0x7f, 0x3d, 0xd8, 0x82, 0xb9, 0x88, 0x70, 0x33, 0xf9, 0x75, 0x7, 0x14, 0x27, 0xc, 0x51, 0x25, 0x2e, 0x7e, 0x76, 0xad, 0xd9, 0x12, 0xad, 0x8, 0x72, 0xe5, 0xe1, 0x10, 0x91, 0xf8, 0xb6, 0xff, 0x2e, 0x60, 0xfd, 0x81, 0x8b, 0x93, 0x52, 0xab, 0xda, 0xbb, 0xe1, 0x22,0x84},
    { 0xaf, 0xbb, 0x5d, 0xfb, 0xba, 0xd5, 0x33, 0xfc, 0x1c, 0x39, 0x1a, 0x43, 0xab, 0x8a, 0x38, 0x8d, 0xd0, 0x57, 0x2b, 0x5f, 0x95, 0x14, 0xbd, 0x4b, 0x82, 0xfb, 0xb1, 0x1c, 0xaf, 0x15, 0xd, 0x15, 0xc2, 0x76, 0x78, 0xeb, 0xbe, 0x57, 0x7c, 0xd1, 0xe8, 0xfc, 0x8a, 0xb9, 0x33, 0xc2, 0x40, 0x59, 0xb6, 0x8d, 0xee, 0xba, 0x17, 0x4e, 0x46, 0xbf, 0x27, 0x14, 0xb6, 0x29, 0x51, 0x41, 0x22,0xac},
    { 0x13, 0xc4, 0xbc, 0x4a, 0x66, 0xa6, 0x1c, 0xc, 0x1e, 0x19, 0xe7, 0x1d, 0x4a, 0x4d, 0x4d, 0xac, 0x87, 0x55, 0xf1, 0x15, 0xdb, 0xe5, 0xf6, 0xd6, 0xe7, 0x5f, 0x4e, 0xbb, 0x8e, 0xea, 0xeb, 0x5d, 0x67, 0xc7, 0xce, 0xf1, 0x32, 0x34, 0x48, 0x4c, 0x54, 0x28, 0xbd, 0x7d, 0x8e, 0x2b, 0x28, 0xf, 0x8c, 0xf8, 0xfc, 0xee, 0x5b, 0x56, 0x38, 0x3c, 0xcf, 0xd5, 0xf3, 0x1e, 0x7a, 0x69, 0x5d,0x16},
    { 0x5c, 0xea, 0x17, 0xe, 0x5f, 0x48, 0x75, 0x31, 0x26, 0xcc, 0xac, 0x65, 0x4, 0xfc, 0xbe, 0xe5, 0x0, 0x42, 0x5b, 0x9, 0x2, 0x31, 0xa3, 0x4, 0xf2, 0xcc, 0xe1, 0x7a, 0xf4, 0x54, 0x98, 0x97, 0xd2, 0x7f, 0x5d, 0x60, 0xdd, 0x2d, 0x17, 0xc7, 0xb1, 0xab, 0x10, 0xce, 0xf0, 0x18, 0xcf, 0x70, 0x8c, 0xa7, 0x56, 0xad, 0x73, 0x62, 0x48, 0x8d, 0x1e, 0x9f, 0xca, 0xa9, 0xc9, 0xe6, 0x36,0xa8},
    { 0x82, 0xc9, 0xa3, 0x66, 0xf8, 0x64, 0x10, 0x78, 0x7d, 0xc7, 0xa, 0xf9, 0xe6, 0x30, 0x7b, 0x67, 0x6e, 0x55, 0x5b, 0x33, 0x56, 0xee, 0x24, 0xec, 0x45, 0xb3, 0xa3, 0x97, 0xa6, 0x65, 0x99, 0x2c, 0x62, 0x74, 0x2c, 0x70, 0x9, 0x8d, 0x5, 0xb5, 0xe, 0x64, 0x9c, 0xcb, 0xa9, 0x3a, 0x99, 0x52, 0x59, 0xf9, 0xdb, 0xdd, 0x7c, 0x41, 0x89, 0x11, 0xf0, 0xda, 0x95, 0xeb, 0xea, 0x11, 0xdc,0x2c},
    };

    u8 MAP_VECTORS_RESULT[7][BYTES_ELEM_SIZE]= {
    { 0x46, 0xa, 0x4a, 0xf4, 0x50, 0xfe, 0x6d, 0xf8, 0x19, 0xa8, 0xe6, 0xff, 0xbb, 0x4, 0x8f, 0x1a, 0x53, 0x58, 0x43, 0xf1, 0x40, 0x17, 0xd, 0x12, 0x45, 0x7d, 0x74, 0x1a, 0x2a, 0xf8, 0x66,0x30},
    { 0x1b, 0xd6, 0x6e, 0xa7, 0x9f, 0x18, 0x33, 0xf1, 0x64, 0x1a, 0xc3, 0xe5, 0xa2, 0x95, 0x3c, 0x77, 0xb4, 0x2c, 0x60, 0xe7, 0xd0, 0xc5, 0x94, 0x2a, 0x2d, 0x2d, 0x36, 0x7d, 0x6f, 0x5b, 0x6e,0xf2},
    { 0x26, 0x28, 0xe0, 0xff, 0xd7, 0xb9, 0xe7, 0x8e, 0xdd, 0x57, 0x9a, 0x2a, 0xdd, 0x28, 0xe1, 0x9d, 0xcc, 0x2, 0x33, 0x3d, 0xa8, 0xce, 0xc5, 0xa2, 0xe6, 0x67, 0x68, 0x9e, 0x2a, 0xcd, 0x6c,0x0},
    { 0x2a, 0xf9, 0x17, 0xc8, 0x53, 0x59, 0x63, 0x8c, 0x54, 0x4, 0xbb, 0x1f, 0xca, 0x3e, 0xae, 0x5d, 0x0, 0x69, 0x81, 0x99, 0xc3, 0xae, 0x90, 0x58, 0x3c, 0x95, 0x37, 0xf2, 0x7c, 0xc8, 0xf0,0xf8},
    { 0x79, 0x11, 0xc4, 0xc4, 0x7c, 0xef, 0xe8, 0xd7, 0xd2, 0x58, 0x97, 0x2, 0x6, 0x6e, 0x2d, 0xa4, 0x67, 0x17, 0x5c, 0x76, 0x4a, 0x30, 0xc, 0xe1, 0x97, 0xa4, 0x20, 0xdf, 0xde, 0xe7, 0x81,0xae},
    { 0x28, 0x86, 0xce, 0xa9, 0xf1, 0x64, 0xed, 0xb2, 0x1a, 0xd, 0x87, 0x40, 0xd1, 0x77, 0xdb, 0xdd, 0xf7, 0x1c, 0x25, 0x1c, 0xbf, 0x41, 0xe8, 0xd3, 0x44, 0x5e, 0x9f, 0xff, 0x52, 0x56, 0x70,0xe2},
    { 0x65, 0x50, 0x9e, 0x6d, 0xf7, 0x88, 0x70, 0x55, 0xea, 0xe, 0x51, 0xb9, 0x1c, 0x68, 0x50, 0x67, 0x69, 0xef, 0x4c, 0x43, 0xa7, 0xf8, 0x63, 0x48, 0xde, 0xcd, 0x11, 0x25, 0x26, 0x7, 0xbd,0x80}
    };  

    u8 RISTRETTO255_BASEPOINT[BYTES_ELEM_SIZE] = {
        0x76, 0x2d, 0x8d, 0xe0, 
        0x45, 0x59, 0xa6, 0xb6,
        0x8d, 0xdd, 0x82, 0xa5, 
        0x6a, 0xb, 0xe3, 0x58, 
        0x5f, 0x51, 0x0, 0xc5,
        0x61, 0xa9, 0x84, 0xa8, 
        0x71, 0x4e, 0xbc, 0x6a, 
        0xa, 0xae, 0xf2, 0xe2
    };
    
    u8 SMALL_MULTIPLES_OF_GENERATOR_VECTORS[16][BYTES_ELEM_SIZE] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x76,0x2d,0x8d,0xe0,0x45,0x59,0xa6,0xb6,0x8d,0xdd,0x82,0xa5,0x6a,0x0b,0xe3,0x58,0x5f,0x51,0x00,0xc5,0x61,0xa9,0x84,0xa8,0x71,0x4e,0xbc,0x6a,0x0a,0xae,0xf2,0xe2},
    {0x19,0xb9,0xa3,0x73,0x5c,0x09,0xd3,0xad,0xac,0xf8,0x01,0xb9,0xd5,0xe8,0x10,0xa1,0x23,0xea,0x0c,0xae,0x10,0xb5,0xec,0x7f,0xd1,0x9c,0x49,0xf7,0x10,0x32,0x49,0x6a},
    {0x59,0x02,0x9d,0x2a,0x15,0x16,0x6b,0x16,0x62,0xb4,0x96,0xd1,0x2b,0x1e,0xea,0xd1,0xd5,0x27,0xee,0x44,0xf0,0x23,0x4f,0xce,0x5e,0x75,0x52,0x5d,0x5d,0x1f,0x74,0x94},
    {0x57,0x6a,0xaf,0x2d,0x32,0x68,0xf5,0x58,0x53,0x95,0x6c,0xea,0xc5,0x53,0xfd,0xd9,0xb3,0x3a,0x29,0xb3,0xe0,0xdf,0xfa,0x6f,0x46,0x8b,0x35,0x73,0x27,0x86,0x80,0xda},
    {0x4e,0xf4,0x0f,0x16,0xc4,0x12,0xb8,0x5a,0x49,0xbb,0x17,0xb5,0xcb,0xfc,0x3e,0x42,0x68,0xf7,0x7c,0x18,0x80,0x70,0x33,0xd3,0xc1,0x52,0x6b,0x01,0x31,0xb1,0x82,0xe8},
    {0x03,0xf4,0x1d,0xbb,0x1e,0x60,0x9a,0xd1,0x93,0xa7,0x5b,0x2f,0x96,0x3f,0x3b,0x7c,0x00,0xf0,0xa7,0x36,0x02,0xd8,0xd8,0x0e,0x05,0x13,0x2b,0xc9,0xd3,0x46,0x47,0xf6},
    {0x6d,0x17,0x2a,0x82,0xa7,0xa6,0xcf,0xbd,0x38,0x87,0xe1,0xec,0x24,0x6a,0xa9,0x85,0xdf,0xb7,0xbe,0x45,0x78,0x38,0x5a,0xbd,0x1f,0xc8,0x6e,0x92,0x20,0x35,0xf5,0x44},
    {0x1c,0x60,0x55,0xdd,0x3c,0x26,0xd5,0x77,0xd0,0x02,0x9f,0x69,0x92,0xe5,0x87,0xc8,0x0b,0x3e,0xa5,0xc1,0x4d,0x37,0xe2,0x10,0xbe,0x7e,0x28,0xf2,0xd8,0x93,0x32,0x90},
    {0x31,0x60,0x07,0xa9,0x82,0x52,0x68,0xd6,0xf0,0xb2,0x34,0xd2,0xc8,0xc8,0xe1,0x16,0xdc,0x8f,0xc4,0x8f,0x3f,0xc6,0xaf,0x1c,0xa3,0x03,0x73,0x8f,0xce,0x2a,0x62,0x02},
    {0x5f,0xb9,0x2d,0x9e,0x68,0x16,0xc8,0xcd,0xe8,0x4d,0x56,0xe7,0xf0,0xbc,0x13,0xf4,0x01,0x2b,0x95,0xd4,0xda,0xa5,0xd2,0x1e,0x0a,0x72,0xb2,0x88,0xd7,0x6f,0x70,0x20},
    {0x42,0xab,0xfd,0xca,0x41,0x32,0xc7,0x7a,0x87,0x95,0xfe,0x4a,0x00,0xc6,0x2b,0x52,0xf9,0x10,0x18,0xba,0x24,0x4c,0x86,0x72,0xa5,0x2f,0xdd,0xa5,0x8b,0x3f,0xe8,0xbc},
    {0x60,0x44,0xf8,0x8f,0xca,0xe3,0x26,0x60,0xde,0x03,0x53,0x4e,0x3e,0x3f,0x4c,0xfa,0xca,0xaf,0xad,0x67,0x8c,0x20,0xca,0x99,0x30,0xa0,0x9a,0x6b,0xe1,0x9e,0x54,0xe4},
    {0x1f,0x50,0x48,0x59,0x6c,0x43,0x67,0x01,0xbe,0xc0,0x8f,0x5a,0xbd,0xd6,0xda,0x42,0x27,0xc4,0x3b,0xc3,0x2f,0x03,0xb1,0x5f,0xf5,0x16,0x2e,0xdf,0x00,0xe0,0x52,0xaa},
    {0x1e,0x30,0x10,0x7f,0xab,0x85,0x00,0xd3,0x7c,0x47,0x41,0x6f,0x71,0xe5,0x96,0x08,0x99,0x91,0x25,0xc5,0xf0,0xf6,0xb5,0xc2,0x9d,0xb2,0x09,0xf4,0x80,0x6b,0x37,0x46},
    {0x4e,0xe6,0x53,0x2e,0x9a,0xd9,0xa9,0x02,0x33,0xfc,0x1d,0x68,0xbb,0x21,0x90,0xd9,0x3a,0x4f,0x12,0xea,0x93,0x5b,0x39,0xd7,0xcd,0xc4,0xd9,0xc8,0xf7,0x18,0xc4,0xe0},
    };

    #endif

    u8 INTG[BYTES_ELEM_SIZE]={
        0x0F, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00
    };
    ///////////////////////////////////////////////////
    //
    //       tttttttt   eeeeeee     ssssss   tttttttt
    //          tt      ee        ss            tt
    //          tt      eeee       sssssss      tt
    //          tt      ee               ss     tt
    //          tt      eeeeee     sssssss      tt
    //

    // printf("testujem endiana!");
    // field_elem test;
    // for (size_t i = 0; i < 16; i++)
    // {
    //     unpack25519(test, SMALL_MULTIPLES_OF_GENERATOR_VECTORS[i]);
    //     print(test);
    //     print_32((u8*)test);
    // }
    // printf("------testujem endiana!");
    
    
    

    
    int result = 1;
    int subresult = 1;
    //temporary variables
    u8 bytes_out_[BYTES_ELEM_SIZE] = {0};
    field_elem in;
    ristretto255_point output_ristretto_point;
    ristretto255_point *out_rist = &output_ristretto_point;
    ristretto255_point output_ristretto_point2;
    ristretto255_point *out_rist2 = &output_ristretto_point2;
    

    // TESTING A.1 -> small multiples of generator(basepoint) and encoding function 

    for (int i = 0; i < 16; ++i)
    {
        INTG[0] = i;
        ristretto255_decode(out_rist,RISTRETTO255_BASEPOINT);
        ristretto255_scalarmult(out_rist2, out_rist,INTG);
        ristretto255_encode(bytes_out_,out_rist2);
        printf("skuska mult. gen:");print_32(bytes_out_);
        subresult = bytes_eq_32(bytes_out_, SMALL_MULTIPLES_OF_GENERATOR_VECTORS[i]);
        result &= subresult;

        #ifdef VERBOSE_FLAG
        if (!subresult){
                printf("SMALL_MULTIPLES_OF_GENERATOR TEST no.%d: FAILED! Error was found when comparing result to SMALL_MULTIPLES_OF_GENERATOR_VECTORS[%d]\n",i,i);
        }
        else{
            printf("SMALL_MULTIPLES_OF_GENERATOR TEST no.%d: SUCCESS!\n",i);
        }
        #endif  
    }
    
#if 1
    // testing hash_to_group
    for (int i = 0; i < 7; ++i){

        hash_to_group(bytes_out_, MAP_VECTORS[i]);
        
        subresult = bytes_eq_32(bytes_out_, MAP_VECTORS_RESULT[i]); // returns 1 if two are eq
        result &= subresult;

        #ifdef VERBOSE_FLAG
        if (!subresult){
                printf("HASH_TO_GROUP TEST no.%d: FAILED! Error was found when testing vector MAP_VECTORS[%d]\n",i,i);
        }
        else{
            printf("HASH_TO_GROUP TEST no.%d: SUCCESS!\n",i);
        }
        #endif      
    }


    //testing if vector is negative 
    for (int i = 0; i < 8; ++i){
        unpack25519(in,test_negative_vectors[i]);

        subresult = is_neg(in);
        result &= subresult; // returns 1 if two are eq

        #ifdef VERBOSE_FLAG
        if (!subresult){
                printf("NEGATIVITY VECTOR TEST no.%d: FAILED! Error was found when testing vector test_negative_vectors[%d]\n",i,i);
        }
        else{
            printf("NEGATIVITY VECTOR TEST no.%d: SUCCESS!\n",i);
        }
        #endif          
    }

    //testing if vector is P-negative is positive 
    for (int i = 0; i < 8; ++i){
        
        unpack25519(in,test_negative_vectors_compl[i]);
        subresult = 1-is_neg(in);
        result &= subresult; // returns 1 if two are eq


        #ifdef VERBOSE_FLAG
        if (!subresult){
                printf("P-NEGATIVE VECTOR TEST no.%d: FAILED! Error was found when testing vector test_negative_vectors_compl[%d]\n",i,i);
        }
        else{
            printf("P-NEGATIVE VECTOR TEST no.%d: SUCCESS!\n",i);
        }
        #endif      
    }


    // testing non-canonical vectors -> should result in error during decoding
    for (int i = 0; i < 4; ++i)
    {
        subresult = ristretto255_decode(out_rist,non_canonical_vectors[i]);
        result &= subresult;

        #ifdef VERBOSE_FLAG
        if (!subresult){
                printf("non-canonical VECTOR TEST no.%d: FAILED! Error was found when testing vector non_canonical_vectors[%d]\n",i,i);
        }
        else{
            printf("non-canonical VECTOR TEST no.%d: SUCCESS!\n",i);
        }
        #endif  
    }

    // testing Non-square x^2 vectors -> should result in error during decoding
    for (int i = 0; i < 8; ++i)
    {
        subresult = ristretto255_decode(out_rist,non_square_x2[i]);
        result &= subresult;

        #ifdef VERBOSE_FLAG
        if (!subresult){
                printf("Non-square x^2 VECTOR TEST no.%d: FAILED! Error was found when testing vector non_square_x2[%d]\n",i,i);
        }
        else{
            printf("Non-square x^2 VECTOR TEST no.%d: SUCCESS!\n",i);
        }
        #endif  
    }

    // testing Negative xy value vectors -> should result in error during decoding
    for (int i = 0; i < 8; ++i)
    {
        subresult = ristretto255_decode(out_rist,negative_xy[i]);
        result &= subresult;

        #ifdef VERBOSE_FLAG
        if (!subresult){
                printf("Negative xy VECTOR TEST no.%d: FAILED! Error was found when testing vector negative_xy[%d]\n",i,i);
        }
        else{
            printf("Negative xy VECTOR TEST no.%d: SUCCESS!\n",i);
        }
        #endif      
    }

    
    /**
      * MOD L test vector
      * Note that variables "rounds,testing_constant,r,iner,modL_test_vec"
      * are variables declared in test_config.h, feel free to change it
      * as you want. Please note that test vector is generated by python
      * script py_modl_inverse.py. So if you want to generate new test
      * vectors, dont forget to change python script accordingly. See 
      * test_config.h for more.
    **/
    // in = 2^256-1;
    int k,i;

   for (k=0; k< rounds; k++) {
    //  calc of inverse
      modl_l_inverse(r, iner);
      for(i=0; i<32; i++) {
        iner[i] = r[i] ^ testing_constant;
      }
   }    

    subresult = bytes_eq_32(iner, modL_test_vec); // returns 1 if two are eq
    result &= subresult;
    #ifdef VERBOSE_FLAG
    if (!subresult){
            printf("ModL VECTOR TEST : FAILED! Error was found when testing vector for mod L\n");
    }
    else{
        printf("ModL VECTOR TEST: SUCCESS!\n");
    }
    #endif   


    // testing s = -1, which causes y = 0.
    result &= ristretto255_decode(out_rist,s_minus_1);

    #ifdef VERBOSE_FLAG
    if (!result){
            printf("s = -1 VECTOR TEST: FAILED!\n");
    }
    else{
        printf("s = -1 VECTOR TEST: SUCCESS!\n");
    }
    #endif  
#endif
    // total result
    if (result != 1)
    {
        printf("\n**********************\n");
        printf("------CONCLUSION------\n\n");
        printf("     TESTS FAILED!    \n");
        printf("**********************\n");
    }
    else{
        printf("\n*************************\n");
        printf("---------CONCLUSION--------\n\n");
        printf("ALL TESTS RAN SUCCESSFULLY!\n");
        printf("***************************\n");
    }

    return 0;
}



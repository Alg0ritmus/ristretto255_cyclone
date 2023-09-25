// ******************************************************************
// ----------------- TECHNICAL UNIVERSITY OF KOSICE -----------------
// ---Department of Electronics and Multimedia Telecommunications ---
// -------- FACULTY OF ELECTRICAL ENGINEERING AND INFORMATICS -------
// ------------ THIS CODE IS A PART OF A MASTER'S THESIS ------------
// ------------------------- Master thesis --------------------------
// -----------------Patrik Zelenak & Milos Drutarovsky --------------
// ---------------------------version 0.1.1 -------------------------
// --------------------------- 21-09-2023 ---------------------------
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

#define pack25519 pack
#define unpack25519 unpack

int main(){
    

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

    u8 GENERATORIK[BYTES_ELEM_SIZE] = {
        0xe2, 0xf2, 0xae, 0xa, 
        0x6a, 0xbc, 0x4e, 0x71, 
        0xa8, 0x84, 0xa9, 0x61,
        0xc5, 0x0, 0x51, 0x5f,
        0x58, 0xe3, 0xb, 0x6a, 
        0xa5, 0x82, 0xdd, 0x8d, 
        0xb6, 0xa6, 0x59, 0x45, 
        0xe0, 0x8d, 0x2d, 0x76
    };

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



    ///////////////////////////////////////////////////
    //
    //       tttttttt   eeeeeee     ssssss   tttttttt
    //          tt      ee        ss            tt
    //          tt      eeee       sssssss      tt
    //          tt      ee               ss     tt
    //          tt      eeeeee     sssssss      tt
    //


    
    int result = 1;
    int subresult = 1;
    //temporary variables
    u8 bytes_out_[BYTES_ELEM_SIZE] = {0};
    field_elem in;
    ristretto255_point output_ristretto_point;
    ristretto255_point *out_rist = &output_ristretto_point;
    ristretto255_point output_ristretto_point2;
    ristretto255_point *out_rist2 = &output_ristretto_point2;
    

    // TESTING A.1 -> small multiples of generator and encoding function 
    ristretto255_decode(out_rist,GENERATORIK);

    for (int i = 0; i < 16; ++i)
    {
        INTG[0] = i;
        ristretto255_decode(out_rist,GENERATORIK);
        ristretto255_scalarmult(out_rist2, out_rist,INTG);
        ristretto255_encode(bytes_out_,out_rist2);
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

    u8 r[32], iner[32];
    u8 modL_test_vec[32] = {
        0x09, 0x78, 0xd0, 0xfb, 0xe4, 0x4d,
        0x7c, 0x69, 0x60, 0xa9, 0xab, 0x59,
        0xf3, 0xad, 0x3c, 0x13, 0xd8, 0x9b,
        0xd1, 0xe6, 0x11, 0x2a, 0x65, 0xeb,
        0x48, 0xc9, 0x9d, 0x7a, 0x18, 0x40,
        0x88, 0x5f,
    };
    int k,i;
    const u8 testing_constant = 0x57;

    // MOD L test vector
    // in = 2^256-1;

       for(k=0; k<32; k++) {
          iner[k]= 255;
       }

       for (k=0; k< 1000; k++) {
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



// **********************************************************************************
// -----------------------------TECHNICAL UNIVERSITY OF KOSICE-----------------------
// ------------------- FACULTY OF ELECTRICAL ENGINEERING AND INFORMATICS-------------
// --------------------- THIS CODE IS A PART OF A MASTER'S THESIS -------------------
// -------------------------------------Master thesis--------------------------------
// ----------------------------Patrik Zelenak & Milos Drutarovsky--------------------
// --------------------------------------version 0.1---------------------------------
// **********************************************************************************

/**
  * This file contains some useful functions for printing, eq. and 
  * wiping field_elem. 
**/

#ifndef _UTILS_H
#define _UTILS_H

#include "helpers.h"

/*      UTILS       */
void print(const field_elem o);

void print_32(const u8* o);

void pack_and_print_32(const field_elem o);

void wipe_field_elem(void *felem, size_t size);

int bytes_eq_32( const u8 a[BYTES_ELEM_SIZE],  const u8 b[BYTES_ELEM_SIZE]);
#endif //_UTILS_H


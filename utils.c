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

// Dependencies
#include "utils.h"

// inspired by Monocypher's crypto_wipe
// URL: https://github.com/LoupVaillant/Monocypher/blob/master/src/monocypher.c#L163
void wipe_field_elem(void *felem, size_t size){
    volatile u8 *to_be_cleared = (u8*)felem;
    for (size_t i = 0; i < size; i++) to_be_cleared[i] = 0;
}

void print(const field_elem o){

    for (int i=0;i<FIELED_ELEM_SIZE;i++){
        printf("%x ", o[i]);
        
    }
    printf("\n");
}


void print_32(const u8* o){

    for (int i=0;i<BYTES_ELEM_SIZE;i++){
        printf("%02hx ", o[i]);
        
    }
    printf("\n");
}

// return 1 if they're equal
// checking if two u8[32] are eq
int bytes_eq_32( const u8 a[BYTES_ELEM_SIZE],  const u8 b[BYTES_ELEM_SIZE]){
    int result = 1;

    for (int i = 0; i < BYTES_ELEM_SIZE; ++i){
        result &= a[i] == b[i];
    }

    return result;
}

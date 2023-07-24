#include "helpers.h"
//#include "tweetNaClLib.h"
#include "utils.h"
#include "curve25519.h"
#define pack25519(out, in) (*(out) = (uint32_t)(*(in))) 

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

void pack_and_print_32(const field_elem o){
    u8 temp[BYTES_ELEM_SIZE];
    pack25519(temp,(uint32_t *)o);
    print_32(temp);
}
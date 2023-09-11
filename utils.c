#include "helpers.h"
//#include "tweetNaClLib.h"
#include "utils.h"
#include "curve25519.h"

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

// NOTE: do we really need this? , mb. just for debugging ...
#if 0 // TODO: figure out how to define/include pack25519 without loop
void pack_and_print_32(const field_elem o){
    u8 temp[BYTES_ELEM_SIZE];
    pack25519(temp,(uint32_t *)o);
    print_32(temp);
}
#endif

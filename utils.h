#ifndef _UTILS_H
#define _UTILS_H


/*      UTILS       */
void print(const field_elem o);

void print_32(const u8* o);

void pack_and_print_32(const field_elem o);

void wipe_field_elem(void *felem, size_t size);

#endif //_UTILS_H


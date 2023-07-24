#ifndef _RISTRETTO255_H
#define _RISTRETTO255_H



int ristretto255_decode(ristretto255_point *ristretto_out, const u8 bytes_in[BYTES_ELEM_SIZE]);
int ristretto255_encode(u8 bytes_out[BYTES_ELEM_SIZE], const ristretto255_point *ristretto_in);
int hash_to_group(u8 bytes_out[BYTES_ELEM_SIZE], const u8 bytes_in[HASH_BYTES_SIZE]);
void ristretto255_scalarmult(ristretto255_point* p, ristretto255_point* q,const u8 *s);
int bytes_eq_32( const u8 a[BYTES_ELEM_SIZE],  const u8 b[BYTES_ELEM_SIZE]);
void fneg(field_elem out, field_elem in);
int is_neg(field_elem in);
int is_neg_bytes(const u8 in[BYTES_ELEM_SIZE]);
void fabsolute(field_elem out, field_elem in);

#endif //_RISTRETTO255_H
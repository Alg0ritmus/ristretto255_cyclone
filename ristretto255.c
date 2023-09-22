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
  * This file is core of our ristretto255 implementation. It is more
  * efficient version of our equivalent TweetNaCl version 
  * of ristretto255 (https://github.com/Alg0ritmus/textbook_ristretto255).
  * This file includes all necessary functions for creation of other
  * protocols that use  eliptic curve Curve25519. 
  *
  * Ristretto255 is a cryptographic construction designed for 
  * efficient and secure elliptic curve cryptography (ECC) 
  * operations. It is based on the Curve25519 elliptic curve,
  * which is a widely used curve in modern cryptography
  * due to its strong security properties and efficient performance.
  *
  * Curve25519, the underlying elliptic curve, has a cofactor of 8,
  * meaning its order is eight times a prime number. This cofactor 
  * introduces the possibility of small subgroup attacks because not
  * all points on the curve are in the main subgroup. The cofactor
  * is a factor by which the order of the elliptic curve group is
  * divisible. In elliptic curve cryptography, it's important to work
  * with points that are on the main subgroup of the curve, as points
  * on small subgroups can introduce vulnerabilities.
  *
  * Ristretto255 addresses this issue by providing a way to map 
  * points from the Curve25519 curve to a unique representative point
  * in a group that eliminates the cofactor-related vulnerabilities.
  * By doing so, it ensures that all points in its representation 
  * belong to the main subgroup, making it resistant to small 
  * subgroup attacks and other potential security issues associated
  * with the cofactor. This mapping is carefully designed to maintain
  * compatibility with Curve25519's arithmetic operations while 
  * addressing the cofactor problem, thus providing a safer
  * and more predictable foundation for cryptographic protocols.
  *
  * In our representation we used Cyclone, a crypto library,
  * that internaly works with 8-word elements. This is why we chose
  * field_elem (our internal representation of GF(2*255-19) 
  * field element) as 8-word elements. We chose Cyclone crypto library
  * because of its advantages, such as compactness and speed,
  * elimination of I/O conversion. In other words, we do not need to
  * perform any conversion, such as pack/unpack or from_bytes/to_bytes
  * as required by other libraries like TweetNaCl or MonoCypher. Note
  * that we are using pack/unpack terminology just for copying data.
  * Most of the functions used from Cyclone are identical with 
  * original code. In fact we just extract and rename all necessary
  * functions.
  *
  * Results from our speed measurements have shown that Cyclone is
  * approximately 5 times faster than TweetNaCl (packing/unpacking
  * not included). Additionally, while MonoCypher is slightly faster
  * (about 1.5 times) than Cyclone, there are highly efficient 
  * implementations written in assembly language that outperform
  * any C implementations. These are particularly suitable for
  * embedded MCUs like the ARM Cortex-M4. This is one of the reasons
  * why we chose Cyclone.
  * Note that measurements were performed on an STM32 MCU with
  * a Cortex-M3 core. 
  * 
  * Besides Cyclone we also implemented necessary functions to 
  * perform scalar multiplication of ristretto255 point.
  * Ristretto255 point has its own representation (see `helpers.h`),
  * that uses extended Edwards point form. Internally, a Ristretto
  * point is represented by an extended Edwards point. 
  * Two extended Edwards points P,Q may represent the same Ristretto
  * point, in the same way that different projective X,Y,Z,T 
  * coordinates may represent the same extended Edwards point.
  * Group operations on Ristretto points are carried out with no 
  * overhead by performing the operations on the representative
  * Edwards points. For example ristretto255_point_addition() performs
  * addition of ristretto255 points, which impl. was largely inspired
  * by TweetNaCl crypto library.
**/

#include <string.h>
#include "ristretto255.h"
#include "ristretto255_constants.h"
#include "gf25519.h"
#include "modl.h"

// Macro WIPE uses crypto_wipe function implemented in modl.h.
// It is wiping function that makes sure we clear memory 
// respomsibly. Function was taken from Monocypher crypto library.
// Note that macro WIPE is then used in WIPE_PT to wipe 
// ristretto255_point which structure is defined in helpers.h
#define WIPE(felem) crypto_wipe(felem, sizeof(felem))
#define WIPE_PT(rpoint) wipe_ristretto255_point(rpoint)
// functions redefined below are "renamed" to fit our equivalent 
// TweetNaCl representation of ristretto255
#define swap25519 gf25519Swap
#define fsub gf25519Sub
#define fadd gf25519Add
#define fmul gf25519Mul
#define pow2 gf25519Sqr
#define pow_xtimes gf25519Pwr2
#define fselect gf25519Select
#define feq !gf25519Comp 
#define carry25519(out, in) gf25519Red(out, in)
#define b_copy gf25519Copy
#define fcopy gf25519Copy

// Setting "order" during pack/unpack w respect to BIGENDIAN_FLAG
// Note that we are using pack/unpack terminology just for copying data.
// See our TweetNaCl implementation (https://github.com/Alg0ritmus/textbook_ristretto255),
// where packing/unpacking takes big role.
#ifdef BIGENDIAN_FLAG
void pack(u8* uint8Array,const u32* uint32ArrayIn) {
    uint32_t uint32Array[8];
    // Note that Cyclone internally uses modulo 2P operations,
    // which are more efficient;
    // so we need to use reduction here
    carry25519(uint32Array, uint32ArrayIn); 
    for (int i = 0; i < 8; ++i) {
        uint8Array[i * 4 + 3] = ((uint32Array[i] >> 0) & 0xFF);
        uint8Array[i * 4 + 2] = ((uint32Array[i] >> 8) & 0xFF);
        uint8Array[i * 4 + 1] = ((uint32Array[i] >> 16) & 0xFF);
        uint8Array[i * 4 + 0] = ((uint32Array[i] >> 24) & 0xFF);
    }
}

void unpack(u32* uint32Array, const u8* uint8Array) {
    for (int i = 0; i < 8; ++i) {
        uint32Array[i] = (uint8Array[i * 4 + 3] << 0) |
                         (uint8Array[i * 4 + 2] << 8) |
                         (uint8Array[i * 4 + 1] << 16)|
                          uint8Array[i * 4 + 0] << 24;
    }
}
#else
void pack(u8* uint8Array,const u32* uint32Array) { 
  memcpy(uint8Array, (u8*) uint32Array, 32);
}

void unpack(u32* uint32Array, const u8* uint8Array) {
  memcpy(uint32Array, uint8Array, 32);
}

#endif

// Wiping ristretto255 point, using WIPE macro
// Note that macro WIPE uses wipe_field_elem() function
// implemented in utils.c
void wipe_ristretto255_point(ristretto255_point* ristretto_in){
  WIPE(ristretto_in->x);
  WIPE(ristretto_in->y);
  WIPE(ristretto_in->z);
  WIPE(ristretto_in->t);

}


#define pack25519 pack
#define unpack25519 unpack

/**
  * @brief Negation of field_elem
  * @param[in]   -> in
  * @param[out]  -> -in
**/
// fneg() => creates negative input and "save" it to variable out
// function uses modular logic where negation of element a -> p-a = -a 
// inspired by: https://github.com/jedisct1/libsodium/blob/master/src/libsodium/include/sodium/private/ed25519_ref10_fe_51.h#L94
void fneg(field_elem out, field_elem in){
    fsub(out, F_MODULUS, in);

};


/**
  * @brief Returns 1 if field_elem is negative 
  * @param[in]   -> in
  * @param[out]  -> Boolean: True/False
**/
// Note that field_elem is negative, only if LSB is set to 1,
// otherwise it is  positive
// This is something that is very important to realize,
// when constructing ristretto255 bcs.
// it is used on multiple places in code
// inspired by: https://github.com/jedisct1/libsodium/blob/master/src/libsodium/include/sodium/private/ed25519_ref10_fe_25_5.h#L302
int is_neg(field_elem in){
    u8 temp[BYTES_ELEM_SIZE];
    pack25519(temp, in);
    return temp[0] & 1;
}


/**
  * @brief Returns 1 if u8[BYTES_ELEM_SIZE] is negative 
  * @param[in]   -> in
  * @param[out]  -> Boolean: True/False
**/
// equivalent for is_neg for 32-word u8[32] element
int is_neg_bytes(const u8 in[BYTES_ELEM_SIZE]){
    return in[0] & 1;
}

/**
  * @brief Returns absolute value of field_elem
  * @param[in]   -> in
  * @param[out]  -> abs(in)
**/
// fabsolute functions gets absolute value of input in constant time 
void fabsolute(field_elem out, field_elem in){
    field_elem temp;
    fcopy(temp,in); // temp=in, so I dont rewrite in
    fneg(out,temp); // out = ~in
    // CT_SWAP if it is neg.
    swap25519(out,temp,is_neg(in));

    WIPE(temp);
}


/**
  * @brief Returns 1 if nonnegative root exists
  * @param[in]   -> a
  * @param[in]   -> b
  * @param[out]  -> nonnegative square root of either (a/b)^1/2 or (ia/b)^1/2
**/
// function that calc ristretto255 inverse square root
// Implementing Ristretto requires computing (inverse) square roots.
// Ed25519 implementations compute square roots as part of the
// Ed25519 decompression functionality,
// but this is often inlined into the decompression formulas,
/// so we need to tune existing Cyclone's curve25519Sqrt
// to fit our needs. Note that our inv_sqrt() computes the
// nonnegative square root of either (a/b)^1/2 or (ia/b)^1/2,
// where i = SQRT_M1 (constant from ristretto255 draft).
// Internal compotation of inversion of B and the square root:
// A * B^3 * (A * B^7)^((p - 5) / 8)
// largely inspired by Cyclone's curve25519Sqrt:
// https://github.com/Oryx-Embedded/CycloneCRYPTO/blob/master/ecc/curve25519.c#L430
// Logic Inspired by: https://ristretto.group/formulas/invsqrt.html
static int inv_sqrt(field_elem out,const field_elem a, const field_elem b){
   int correct_sign_sqrt;
   int flipped_sign_sqrt;
   int flipped_sign_sqrt_i;
   field_elem c; field_elem v;

   // Compute the candidate root (A / B)^((p + 3) / 8). This can be
   // done with the following trick, using a single modular powering 
   // for both the inversion of B and the square root:
   // A * B^3 * (A * B^7)^((p - 5) / 8)
   pow2(v, b);
   fmul(v, v, b);
   pow2(v, v);
   fmul(v, v, b);

   //Compute C = A * B^7
   fmul(c, a, v);

   //Compute U = C^((p - 5) / 8)
   pow2(out, c);
   fmul(out, out, c); //C^(2^2 - 1)
   pow2(out, out);
   fmul(v, out, c); //C^(2^3 - 1)
   pow_xtimes(out, v, 3);
   fmul(out, out, v); //C^(2^6 - 1)
   pow2(out, out);
   fmul(v, out, c); //C^(2^7 - 1)
   pow_xtimes(out, v, 7);
   fmul(out, out, v); //C^(2^14 - 1)
   pow2(out, out);
   fmul(v, out, c); //C^(2^15 - 1)
   pow_xtimes(out, v, 15);
   fmul(out, out, v); //C^(2^30 - 1)
   pow2(out, out);
   fmul(v, out, c); //C^(2^31 - 1)
   pow_xtimes(out, v, 31);
   fmul(v, out, v); //C^(2^62 - 1)
   pow_xtimes(out, v, 62);
   fmul(out, out, v); //C^(2^124 - 1)
   pow2(out, out);
   fmul(v, out, c); //C^(2^125 - 1)
   pow_xtimes(out, v, 125);
   fmul(out, out, v); //C^(2^250 - 1)
   pow2(out, out);
   pow2(out, out);
   fmul(out, out, c); //C^(2^252 - 3)


   //The first candidate root is U = A * B^3 * (A * B^7)^((p - 5) / 8) 
   fmul(out, out, a);
   pow2(v, b);
   fmul(v, v, b);
   fmul(out, out, v); // out == r

   //Calculate C = B * U^2 // CHECK
   pow2(c, out);
   fmul(c, c, b);

   //Conditions:
   correct_sign_sqrt = feq(c, a);

   fneg(v,(uint32_t *)a);
   flipped_sign_sqrt = feq(c, v);


   fmul(v, v, SQRT_M1);
   flipped_sign_sqrt_i = feq(c, v);

   //Calc v = i*r
   fmul(v, SQRT_M1, out);

   // if cond = 1, select first option
   #ifdef USE_GF25519SELECT
   fselect(out, out, v, flipped_sign_sqrt | flipped_sign_sqrt_i ); 
   #else
   swap25519(out, v, flipped_sign_sqrt | flipped_sign_sqrt_i ); 
   #endif

  // calc v = -r
   fneg(v,out);
   // if cond = 1, select first option
   #ifdef USE_GF25519SELECT
   fselect(out, out, v, is_neg(out)); 
   #else
   swap25519(out, v, is_neg(out)); 
   #endif  

   WIPE(c); WIPE(v);
   return correct_sign_sqrt | flipped_sign_sqrt;
}


/**
  * @brief Maps field_elem into ristretto255_point
  * @param[in]   -> t
  * @param[out]  -> ristretto255_point from field_elem
**/
// MAP function from draft
// MAP is used in hash_to_group() fuction to get ristretto point
// from hash
// Input parameter is field_elem "t"
// Output is point on Edward's (interpreted as ristretto255_point)
// curve with coords X,Y,Z,T
// MAP is also called ristretto255_elligator
static void MAP(ristretto255_point* ristretto_out, const field_elem t){ 
    field_elem tmp1, tmp2, tmp3, tmp4, tmp5;
    int was_square, wasnt_square;

    #define _r tmp1
    #define out tmp2
    #define c tmp3
    #define rpd tmp4
    #define v tmp5
    fmul(_r,t,t);

    fmul(_r,SQRT_M1,_r);
    fadd(out,_r,F_ONE);

    fmul(out,out,ONE_MINUS_D_SQ);
    fneg(c,(uint32_t *)F_ONE);  

    fadd(rpd,_r,EDWARDS_D);
    fmul(v,_r,EDWARDS_D);
    fsub(v,c,v);
    fmul(v,v,rpd);
    
    #define s tmp4
    #define s_prime tmp2
    was_square = inv_sqrt(s,out,v);
    wasnt_square = 1 - was_square;

    fmul(s_prime,s,t);
    fabsolute(s_prime,s_prime);
    fneg(s_prime,s_prime);

    swap25519(s,s_prime,wasnt_square);
    #define rc tmp2
    fcopy(rc,_r);
    swap25519(c,rc,wasnt_square);

    #define n tmp2
    fsub(n,_r,F_ONE);
    fmul(n,n,c);
    fmul(n,n,D_MINUS_ONE_SQ);
    fsub(n,n,v);

    #define ss tmp1
    #define w0 tmp3
    fmul(ss,s,s);

    fadd(w0,s,s);
    fmul(w0,w0,v);
    #define w1 tmp4
    fmul(w1,n,SQRT_AD_MINUS_ONE);
    #define w2 tmp2
    #define w3 tmp5
    fsub(w2,F_ONE,ss);
    fadd(w3,F_ONE,ss);

    fmul(ristretto_out->x,w0,w3);
    fmul(ristretto_out->y,w2,w1);
    fmul(ristretto_out->z,w1,w3);
    fmul(ristretto_out->t,w0,w2);

    WIPE(ss); WIPE(w2); WIPE(w0);
    WIPE(w1); WIPE(w3);
}



/**
  * @brief Add two ristretto255_point points
  * @param[in]   -> p ristretto255_point
  * @param[in]   -> q ristretto255_point
  * @param[out]  -> r ristretto255_point
**/
// Ristretto255 point addition (Edward's coords | X,Y,Z,T ) 
// ristretto255_point_addition is identical to 
// tweetNaCl sv add(gf p[4],gf q[4]),
// Basically I changed name to smth more slef-explaining and 
// changed gf[4] to our ristretto255_point representation
// of ristretto points. NOTE: gf is the same as our field_elem
// (just naming is different)
// https://github.com/dominictarr/tweetnacl/blob/master/tweetnacl.c#L590
void ristretto255_point_addition(ristretto255_point* r,const ristretto255_point* p,const ristretto255_point* q){
    field_elem temp_1,temp_2,temp_3, temp_4, temp_5;

    #define a temp_1
    #define _t temp_2
    #define b temp_3
    
    fsub(a, p->y, p->x);
    fsub(_t, q->y, q->x);
    fmul(a, a, _t);
    fadd(b, p->x, p->y);
    fadd(_t, q->x, q->y);
    fmul(b, b, _t);

    #define e temp_5
    #define h temp_2
    fadd(h, b, a);
    fsub(e, b, a);
    
    
    #define d temp_1
    #define _c temp_3
    fmul(d, p->z, q->z);
    fadd(d, d, d);

    fmul(_c, p->t, q->t);
    fmul(_c, _c, EDWARDS_D2);
    #define f temp_4
    fsub(f, d, _c);
    #define g temp_3
    fadd(g, d, _c);

    fmul(r->x, e, f);
    fmul(r->y, h, g);
    fmul(r->z, g, f);
    fmul(r->t, e, h);

    WIPE(d); WIPE(h); WIPE(g);
    WIPE(f); WIPE(e);
}


/**
  * @brief Conditional swap if b is set to 1
  * @param[in/out]   -> p ristretto255_point
  * @param[in/out]   -> q ristretto255_point
  * @param[in]       -> b if set to 1 swap
**/
// cswap() => conditional swap, if b is set to 1,
// then swap 2 ristretto points
// inspired by tweetNaCl: https://github.com/dominictarr/tweetnacl/blob/master/tweetnacl.c#L615
static void cswap(ristretto255_point* p, ristretto255_point* q,u8 b){
    swap25519(p->x,q->x,b);
    swap25519(p->y,q->y,b);
    swap25519(p->z,q->z,b);
    swap25519(p->t,q->t,b);
}



/**
  * @brief Decode input bytes u8[32] to ristretto255_point
  * @param[in]   -> bytes_in[32]
  * @param[out]   -> ristretto_out ristretto255_point
**/
// Inspired by ristretto draft
// generate ristretto point from bytes[32]
int ristretto255_decode(ristretto255_point *ristretto_out, const u8 bytes_in[BYTES_ELEM_SIZE]){
  
  int was_square, is_canonical, is_negative;

  field_elem temp1,temp2,temp3,temp4,temp5,temp6;

  u8 checked_bytes[BYTES_ELEM_SIZE];

  // Step 1: Check that the encoding of the 
  // field element is canonical
  #define _s temp1
  unpack25519(_s, bytes_in);
  pack25519(checked_bytes,_s);

  

  // check if bytes_in == checked_bytes, else abort
  is_canonical = bytes_eq_32(checked_bytes,bytes_in);
  is_negative = is_neg_bytes(bytes_in);

  if (is_canonical == 0 || is_negative==1){
    #ifdef DDEBUG_FLAG
        printf("ristretto255_decode: Bad encoding or neg bytes passed to the ristretto255_decode function! is_canonical=%d, is_negative=%d\n", is_canonical, is_negative);
    #endif
    return 1;
  }

  // Step 2 calc ristretto255/ge25519 point
  // a = ± 1
  #define _ss temp2
  #define u1 temp3
  #define u2 temp4
  

  pow2(_ss,_s);
  fsub(u1,F_ONE,_ss); // 1 + as^2
  fadd(u2,F_ONE,_ss); // 1 - as^2

  
  #define uu1 temp5
  pow2(uu1,u1);
  


  // d == EDWARDS_D-> from ristretto darft
  #define duu1_positive temp6
  fmul(duu1_positive,EDWARDS_D,uu1); // d*u1^2
  #define duu1 temp5
  fneg(duu1,duu1_positive); // -(D * u1^2) 

  #define uu2 temp6
  pow2(uu2,u2);
  #define _v temp2
  fsub(_v,duu1,uu2); // adu1^2 - u2^2
  #define vuu2 temp5
  fmul(vuu2,_v,uu2); // v*u2^2

  #define _I temp6
  was_square = inv_sqrt(_I,F_ONE,vuu2);

  #define Dx temp5
  fmul(Dx,_I,u2); // I*u2
  #define Dxv temp4
  fmul(Dxv, Dx, _v); // Dx * v


  // x: |2sDx|
  #define sDx temp2
  fmul(sDx,_s,Dx);
  fadd(ristretto_out->x,sDx,sDx); // 2sDx
  fabsolute(ristretto_out->x,ristretto_out->x);// |2sDx|

  #define Dy temp5
  fmul(Dy, _I, Dxv); // I*Dx*v

  // y: u1Dy
  fmul(ristretto_out->y,u1,Dy);

  // t: x,y
  fmul(ristretto_out->t,ristretto_out->x,ristretto_out->y);

  fcopy(ristretto_out->z, F_ONE);

  WIPE(_s); WIPE(sDx); WIPE(u1);
  WIPE(Dxv); WIPE(Dy); WIPE(_I);

  if (was_square == 0){
    #ifdef DEBUG_FLAG
      printf("\n\n\n ristretto255_decode: Bad encoding! was_square=%d \n\n\n",was_square);
    #endif
    return 1;
  }
  if (is_neg(ristretto_out->t)){
    #ifdef DEBUG_FLAG
      printf("\n\n\n ristretto255_decode: Bad encoding! is_neg(t)=%d \n\n\n",is_neg(ristretto_out->t));
    #endif
    return 1;
  }
  if (feq(ristretto_out->y,F_ZERO)){
    #ifdef DEBUG_FLAG
      printf("\n\n\n ristretto255_decode: Bad encoding! feq(y,F_ZERO)=%d \n\n\n",feq(ristretto_out->y,F_ZERO));
    #endif
    return 1;
  }
  return 0;
}



/**
  * @brief Encode input ristretto255_point to output bytes u8[32]
  * @param[in]   -> ristretto_out ristretto255_point
  * @param[out]      -> bytes_out[32]
**/
// Inspired by ristretto draft
// encodes ristretto255 point to 32-word u8[32] element
int ristretto255_encode(u8 bytes_out[BYTES_ELEM_SIZE], const ristretto255_point* ristretto_in){

  
  field_elem _temp1,_temp2,_temp3,_temp4,_temp5,_temp6,_temp7;


  #define temp_zy1 _temp1
  #define temp_zy2 _temp2
  #define u1_ _temp3
  fadd(temp_zy1,ristretto_in->z,ristretto_in->y);
  fsub(temp_zy2,ristretto_in->z,ristretto_in->y);
  fmul(u1_,temp_zy1,temp_zy2); // (Z+Y)(Z-Y)

  #define u2_ _temp1
  fmul(u2_,ristretto_in->x,ristretto_in->y); // XY

  #define uu2_ _temp2
  pow2(uu2_,u2_); //u2^2 
  #define u1uu2 _temp4
  fmul(u1uu2,u1_,uu2_); // u1 * u2^2
  #define I_ _temp2
  inv_sqrt(I_,F_ONE,u1uu2);

  #define D1_ _temp4
  fmul(D1_,u1_,I_); // u1 * I
  #define D2_ _temp3
  fmul(D2_,u2_,I_); // u2 * I

  #define D1D2 _temp1
  fmul(D1D2,D1_,D2_); // D1 * D2
  #define Zinv _temp2
  fmul(Zinv,D1D2,ristretto_in->t); // Zinv = D1*D2*T
  

  #define enchanted_denominator _temp1
  fmul(enchanted_denominator,D1_,INVSQRT_A_MINUS_D);
  #define tZinv _temp4
  fmul(tZinv,ristretto_in->t,Zinv);
  
  int is_tZinv_neg = 1-is_neg(tZinv);
  #define _X _temp4
  #define _Y _temp5
  fcopy(_X,ristretto_in->x);
  fcopy(_Y,ristretto_in->y);

  #define iX _temp6
  #define iY _temp7
  fmul(iX, ristretto_in->x, SQRT_M1);
  fmul(iY, ristretto_in->y, SQRT_M1);
  swap25519(iY,_X,is_tZinv_neg);
  swap25519(iX,_Y,is_tZinv_neg);
  swap25519(enchanted_denominator,D2_,is_tZinv_neg);

  

  #define XZ_inv _temp3
  fmul(XZ_inv,iY,Zinv);
  #define n_Y _temp2
  fneg(n_Y,iX);
  fcopy(iX,iX);
  swap25519(iX,n_Y,is_neg(XZ_inv));
  

  #define _Z _temp3
  fcopy(_Z,ristretto_in->z);

  #define Z_Y _temp2
  fsub(Z_Y,_Z,iX);

  #define temp_s _temp3
  fmul(temp_s,enchanted_denominator,Z_Y);


  //fcopy(s,temp_s);
  fabsolute(temp_s,temp_s);
  
  pack25519(bytes_out,temp_s);

  WIPE(enchanted_denominator); WIPE(Z_Y); WIPE(temp_s);
  WIPE(_X); WIPE(_Y); WIPE(iX);
  WIPE(iY);
  return 0;
}



/**
  * @brief Turns hash bytes[32] into valid ristretto point in GF
  * @param[in]   -> bytes_in[32]
  * @param[out]  -> bytes_out[32]
**/

// Inspired by ristretto draft
// hash_to_group or element derivation function takes 
// hash input and turn it into valid ristretto point.
// In this implementation, input is hexa-string (see more below)
// hash_to_group function consists of 3 steps:
// 1) divide input into 2 halves and mask both hlaves
// 2) MAP both halves so out get 2 points represented 
// with X,Y,Z,T coords (Extended edward's coords)
//
// 3) perform addition of 2 edward's point, note that
// we need to add 2 edwards points so fe25519 arithmetics won't 
// fit there we need to use function that adds 2 edwards points

int hash_to_group(u8 bytes_out[BYTES_ELEM_SIZE], const u8 bytes_in[HASH_BYTES_SIZE]){
  ristretto255_point a_point;
  ristretto255_point *a = &a_point;

  ristretto255_point b_point;
  ristretto255_point *b = &b_point;

  ristretto255_point r_point;
  ristretto255_point *r = &r_point;
  

  // make halves
  u8 t1[32], t2[32];
  b_copy((uint32_t *)t1,(uint32_t *)bytes_in);
  b_copy((uint32_t *)t2,(uint32_t *) (bytes_in+32));

  
  // MASK LSB for each half, this is equivalent to modulo 2**255
  // This step is very important, if skipped, hash_to_group 
  // returns invalid elements
  t1[31] &= 0x7F;
  t2[31] &= 0x7F;

  // encode t1,t2 to field_elem

  field_elem ft1,ft2;
  unpack25519(ft1,t1);
  unpack25519(ft2,t2);

  MAP(a,ft1); // map(ristretto_elligator) first half
  MAP(b,ft2); // map(ristretto_elligator) second hal

  ristretto255_point_addition(r,a,b); // addition of 2 Edward's point

  ristretto255_encode(bytes_out, r);

  WIPE_PT(&a_point); WIPE_PT(&b_point); WIPE_PT(&r_point);
  return 0;
}


/**
  * @brief Scalar multiplication of ristretto255_point
  * @param[in]   -> q ristretto255_point 
  * @param[in]   -> s ==> scalar 
  * @param[out]  -> p ristretto255_point
**/
// ristretto255_scalarmult() => scalar multiplication ristretto_point q * scalar s
// Note that scalar "s" is represented as u8[32]
// Inspired by tweetNaCl: https://github.com/dominictarr/tweetnacl/blob/master/tweetnacl.c#L632
void ristretto255_scalarmult(ristretto255_point* p, ristretto255_point* q,const u8 *s){
  fcopy(p->x,F_ZERO);
  fcopy(p->y,F_ONE);
  fcopy(p->z,F_ONE);
  fcopy(p->t,F_ZERO);
  for (int i = 255;i >= 0;--i) {
    u8 b = (s[i/8]>>(i&7))&1;
    cswap(p,q,b);
    ristretto255_point_addition(q,q,p);
    ristretto255_point_addition(p,p,p);
    cswap(p,q,b);
  }
}





































/*
 *
 * motion_comp.c LORES
 *
 */

#include <stdio.h>
#include <inttypes.h>

#include "mpeg2_internal.h"

mc_functions_t mc_functions;

void motion_comp_init (void)
{
     mc_functions = mc_functions_c;
}

/*
 * These routines deserve a little explanation. Basically what we are doing here are
 * two basic calculations, (a + b + 1) / 2 and (a + b + c + d + 2) / 4. These calculations
 * just give the average of 2 or 4 pixles, properly rounded by adding 0.5. However, doing
 * this on a byte basis will not be very fast, so we try to perform the calculation on
 * four bytes at a time. This is how we do it.
 *
 * First we need to know a simple mathematic rule, any positive number can be written as
 * the sum of two other positive numbers. It should be no trouble realizing this, right?
 *
 * So, let's have a look at the first calculation:
 *
 * (a + b + 1) / 2, now lets rewrite this with a and b being the sum of two positive numbers.
 *
 * (a1 + a2 + b1 + b2 + 1) / 2, assume a2 and b2 can have the values [0, 1] while a1 and b1
 *                              can have the values [0, 2, 4, 8, ..., 252, 254].
 *
 * So, basically a2 and b2 are the least significant bit, but why do we need to do this?
 * Simple, adding two 8 bit numbers will require 9 bits of storage, but we only have 8 to
 * work with and must find a way to work around this. Let's rearrange the calculation.
 *
 * (a1 + b1) / 2 + (a2 + b2 + 1) / 2 = (a1 / 2) + (b1 / 2) + (a2 + b2 + 1) / 2
 *
 * As you can see, if we devide a1 and b1 by 2, they never use more than 7 bits and will
 * thus fit within the 8 bits we have. And a2, b2 are just one bit, so no problem.
 * Let's implement this for four calculations at a time.
 *
 * avg = ((a >> 1) & 0x7f7f7f7f) + ((b >> 1) & 0x7f7f7f7f) + 
 *       ((((a & 0x01010101) + (b & 0x01010101) + 0x01010101) >> 1) & 0x01010101)
 *
 * The code above uses 3 shifts, 5 ands, 4 adds, a total of 12 instructions. This is the same
 * as bytewise, 4 * (1 shift, 2 add) = 4 shifts, 8 adds (total 12). The improvement is due to
 * the reduced memory accesses. But wait, we can do a shortcut here. Let's look at the
 * possible outcomes of the (a2 + b2 + 1) / 2 calculation.
 *
 * (0 + 0 + 1) >> 1 = 0, (0 + 1 + 1) >> 1 = 1, (1 + 0 + 1) >> 1 = 1, (1 + 1 + 1) >> 1 = 1
 *
 * If you look at the first two terms you see that as long as atleast one of them is one,
 * the result becomes one. Now, how can we do this? Yes, using an or!
 *
 * (a | b) & 0x01010101 will give the exact same result!
 *
 * avg = ((a >> 1) & 0x7f7f7f7f) + ((b >> 1) & 0x7f7f7f7f) + ((a | b) & 0x01010101)
 *
 * This gives just 2 shifts, 3 ands, 2 adds and 1 or, a total of 8 opcodes (and cycles)
 * to be compared to 12 using the old way. Add this together with the reduced code size and
 * fewer memory accesses and you'll get a neat speed increase!
 *
 * The shortcut can not be performed for the four term average, but we'll have to live with
 * that, and we'll still gain some speed!
 *
 * 2001-08-22 / Mathias "AmiDog" Roslund
 *
 * The average for two terms can actually be calculated using fewer opcodes as has been done
 * below, this obviously has a positive effect on the performace.
 *
 * 2002-09-02 / Mathias "AmiDog" Roslund
 *
 */

/*
 *       shift, and, add, or, xor = opcodes
 * AVG2    1     1    1    1   1      5
 * AVG4    3     3    3    3   3     15
 * AVG4_2  4     4    4    4   4     20
 *
 */

#define MASK7F 0x7f7f7f7f

#define AVG2(a, b) \
  ((a) | (b)) - ((((a) ^ (b)) >> 1) & M7F)

#define AVG4(a, b, c, d) \
  AVG2((AVG2(a, b)), (AVG2(c, d)))

#define AVG4_2(a, b, c, d, e) \
  AVG2((AVG4(a, b, c, d)), e)

/* Long access to byte pointer */
#define LONG(var, offset) (*(unsigned long *)(var + offset))

void MC_put_8_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a;

    do {
     a = LONG(ref,  0);

     LONG(dest,  0) = a;

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_put_16_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  4);

     LONG(dest,  0) = a;
     LONG(dest,  4) = b;

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_avg_8_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(dest,  0);
     LONG(dest,  0) = AVG2(a, b);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_avg_16_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  4);
     c = LONG(dest,  0);
     d = LONG(dest,  4);

     LONG(dest,  0) = AVG2(a, c);
     LONG(dest,  4) = AVG2(b, d);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_put_x8_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  1);

     LONG(dest,  0) = AVG2(a, b);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_put_x16_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  4);

     c = LONG(ref,  1);
     d = LONG(ref,  5);

     LONG(dest,  0) = AVG2(a, c);
     LONG(dest,  4) = AVG2(b, d);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_avg_x8_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  1);

     a = AVG2(a, b);

     b = LONG(dest,  0);

     LONG(dest,  0) = AVG2(a, b);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_avg_x16_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  4);

     c = LONG(ref,  1);
     d = LONG(ref,  5);

     a = AVG2(a, c);
     b = AVG2(b, d);

     c = LONG(dest,  0);
     d = LONG(dest,  4);

     LONG(dest,  0) = AVG2(a, c);
     LONG(dest,  4) = AVG2(b, d);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_put_y8_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref + stride,  0);

     LONG(dest,  0) = AVG2(a, b);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_put_y16_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  4);
     c = LONG(ref + stride,  0);
     d = LONG(ref + stride,  4);

     LONG(dest,  0) = AVG2(a, c);
     LONG(dest,  4) = AVG2(b, d);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_avg_y8_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref + stride,  0);

     a = AVG2(a, b);

     b = LONG(dest,  0);

     LONG(dest,  0) = AVG2(a, b);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_avg_y16_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  4);
     c = LONG(ref + stride,  0);
     d = LONG(ref + stride,  4);

     a = AVG2(a, c);
     b = AVG2(b, d);

     c = LONG(dest,  0);
     d = LONG(dest,  4);

     LONG(dest,  0) = AVG2(a, c);
     LONG(dest,  4) = AVG2(b, d);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_put_xy8_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  1);

     c = LONG(ref + stride,  0);
     d = LONG(ref + stride,  1);

     LONG(dest,  0) = AVG4(a, b, c, d);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_put_xy16_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, e, f, g, h, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  1);
     c = LONG(ref,  4);
     d = LONG(ref,  5);

     e = LONG(ref + stride,  0);
     f = LONG(ref + stride,  1);
     g = LONG(ref + stride,  4);
     h = LONG(ref + stride,  5);

     LONG(dest,  0) = AVG4(a, b, e, f);
     LONG(dest,  4) = AVG4(c, d, g, h);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_avg_xy8_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, e, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  1);

     c = LONG(ref + stride,  0);
     d = LONG(ref + stride,  1);

     e = LONG(dest,  0);

     LONG(dest,  0) = AVG4_2(a, b, c, d, e);

     ref += stride;
     dest += stride;
    } while (--height);
}

void MC_avg_xy16_c (uint8_t * dest, uint8_t * ref, int stride, int height)
{
    unsigned long a, b, c, d, e, f, g, h, i, j, M7F = MASK7F;

    do {
     a = LONG(ref,  0);
     b = LONG(ref,  1);
     c = LONG(ref,  4);
     d = LONG(ref,  5);

     e = LONG(ref + stride,  0);
     f = LONG(ref + stride,  1);
     g = LONG(ref + stride,  4);
     h = LONG(ref + stride,  5);

     i = LONG(dest,  0);
     j = LONG(dest,  4);

     LONG(dest,  0) = AVG4_2(a, b, e, f, i);
     LONG(dest,  4) = AVG4_2(c, d, g, h, j);

     ref += stride;
     dest += stride;
    } while (--height);
}

MOTION_COMP_EXTERN (c)

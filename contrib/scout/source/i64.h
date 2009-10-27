/* i64.h */
/*--------------------------------------------------------------------------*\
  Copyright (C) 1999 Douglas W. Sauder

  This software is provided "as is," without any express or implied
  warranty.  In no event will the author be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  The original distribution can be obtained from www.hunnysoft.com.
  You can email the author at dwsauder@erols.com.

  $RCSfile: i64.h,v $
  $Revision$
  $Date$
\*--------------------------------------------------------------------------*/

#ifndef I64_MATH
#define I64_MATH

typedef struct {
    unsigned hi;  /* Most significant 32 bits */
    unsigned lo;  /* Least significant 32 bits */
} bigint;


/* Compare n1 and n2.
 *    Returns  1 if n1 > n2
 *    Returns -1 if n1 < n2
 *    Returns  0 if n1 = n2
 */
int i64_cmp(bigint n1, bigint n2);

#define I64_GREATER 1
#define I64_LESS -1
#define I64_EQUAL 0

/* Sign of n
 *
 *    Returns  1 if n > 0
 *    Returns -1 if n < 0
 *    Returns  0 if n = 0
 */
int i64_sgn(bigint n);

/* Left shift
 *
 *    Shift n to the left by b bits
 *
 *    Undefined if b < 0 or b > 63
 */
bigint i64_lshift(bigint n, int b);

/* Unsigned right shift
 *
 *    Shift n to the right by b bits.  Zeros are shifted in.
 *
 *    Undefined if b < 0 or b > 63
 */
bigint i64_urshift(bigint n, int b);

/* Signed right shift
 *
 *    Shift n to the right by b bits.  Zeros or ones are shifted in, depending
 *    on the most leftmost bit in n.
 *
 *    Undefined if b < 0 or b > 63
 */
bigint i64_srshift(bigint n, int b);

/* Change sign of n
 *
 *    Returns -n
 */
bigint i64_inv(bigint n);

/* Add n1 and n2
 *
 *    Returns n1 + n2
 */
bigint i64_add(bigint n1, bigint n2);

/* Subtract n2 from n1
 *
 *    Returns n1 - n2
 */
bigint i64_sub(bigint n1, bigint n2);

/* Multiply n1 and n2
 *
 *    Returns n1 * n2
 */
bigint i64_mul(bigint multiplicand, bigint multiplier);

/* Divide dividend by divisor
 *
 *    Returns values quotient and remainder such that
 *
 *      dividend = (divisor * quotient) + remainder
 */
void i64_div(bigint dividend, bigint divisor, bigint *quotient, bigint *remainder);

/* Divide dividend by divisor (unsigned)
 *
 *    Returns values quotient and remainder such that
 *
 *      dividend = (divisor * quotient) + remainder
 *
 * Both dividend and divisor are treated as unsigned integers
 */
void i64_udiv(bigint dividend, bigint divisor, bigint *quotient, bigint *remainder);

/* Converts string to 64 bit integer
 *
 *    Returns 64 bit integer from ASCII representation in buffer str
 *    Returns 0 on error
 */
bigint i64_atoi(const char *str);

/* Converts 64 bit integer to string
 *
 *     Fills buffer str with ASCII representation for n
 */
void i64_itoa(bigint n, char *str, int strSize);


/* Following functions added by Paul Huxham 30/12/2000 */

/* Set the value of a bigint from a long */
bigint i64_set( long n1 );

/* Set the value of a bigint from a ulong */
bigint i64_uset( unsigned long n1 );

/* Multiply two ulongs together and return a bigint */
bigint i64_uumul( unsigned long n1, unsigned long n2 );

#endif

/* i64.c */
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
  You can email the author, Doug Sauder, at dwsauder@erols.com.

  $RCSfile: i64.c,v $
  $Revision$
  $Date$
\*--------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>
#include "i64.h"

static void i64_udiv_impl(bigint dividend, bigint divisor, bigint *quotient,
    bigint *remainder);


/*
 * Compare two values
 *  
 *    n1 < n2  -->  -1
 *    n1 = n2  -->   0
 *    n1 > n2  -->   1
 */
int i64_cmp(bigint n1, bigint n2)
{
    int cmp;

    cmp = 1;
    if ((int) n1.hi < (int) n2.hi) {
        cmp = -1;
    }
    else if (n1.hi == n2.hi) {
        if (n1.lo < n2.lo) {
            cmp = -1;
        }
        else if (n1.lo == n2.lo) {
           cmp = 0;
        }
    }
    return cmp;
}


/*
 * "Sign" operation
 *
 *    n1 < 0  -->  -1
 *    n1 = 0  -->   0
 *    n1 > 0  -->   1
 */
int i64_sgn(bigint n1)
{
    int sgn;

    sgn = 1;
    if (n1.hi & 0x80000000) {
       sgn = -1;
    }
    else if ((n1.hi | n1.lo) == 0) {
       sgn = 0;
    }
    return sgn;
}


/*
 * Left shift n by b bits.  Undefined if b is negative.
 */
bigint i64_lshift(bigint n, int b)
{
    if (b > 0) {
        if (b < 32) {
            n.hi <<= b;
            n.hi += n.lo >> (32 - b);
            n.lo <<= b;
        }
        else if (b < 64) {
            n.hi = n.lo << (b - 32);
            n.lo = 0;
        }
        else {
            n.lo = 0;
            n.hi = 0;
        }
    }
    return n;
}


/*
 * Unsigned right shift n by b bits.  Zeros are always shifted in.
 * Undefined if b is negative.
 */
bigint i64_urshift(bigint n, int b)
{
    if (b > 0) {
        if (b < 32) {
            n.lo >>= b;
            n.lo += n.hi << (32 - b);
            n.hi >>= b;
        }
        else if (b < 64) {
            n.lo = n.hi >> (b - 32);
            n.hi = 0;
        }
        else {
            n.lo = 0;
            n.hi = 0;
        }
    }
    return n;
}


/*
 * Signed right shift n by b bits.  Bit shifted in matches most significant
 * bit in n.  Undefined if b is negative.
 */
bigint i64_srshift(bigint n, int b)
{
    unsigned m;

    if (b > 0) {
        m = 0;
        if (n.hi & 0x80000000) {
            m = 0xffffffff;
        }
        if (b < 32) {
            n.lo >>= b;
            n.lo += n.hi << (32 - b);
            n.hi >>= b;
            n.hi += m << (32 - b);
        }
        else if (b < 64) {
            b -= 32;
            n.lo = n.hi >> b;
            n.lo += m << (32 - b);
            n.hi = m;
        }
        else {
            n.lo = m;
            n.hi = m;
        }
    }
    return n;
}


/*
 * Change sign operation (additive inverse)
 *
 *   n  -->  -n
 */
bigint i64_inv(bigint n)
{
    bigint r;

    r.lo = (unsigned) - (int) n.lo;
    if (r.lo == 0) {
        r.hi = (unsigned) - (int) n.hi;
    }
    else /* if (r.lo != 0) */ {
        r.hi = ~n.hi;
    }
    return r;
}


/*
 * Add n1 and n2
 */
bigint i64_add(bigint n1, bigint n2)
{
    bigint sum;
    unsigned c;

    sum.lo = n1.lo + n2.lo;
    c = (n1.lo & n2.lo) | ((~sum.lo) & (n1.lo | n2.lo));
    c >>= 31;
    sum.hi = n1.hi + n2.hi + c;
    return sum;
}


/*
 * Subtract n2 from n1
 */
bigint i64_sub(bigint n1, bigint n2)
{
    bigint diff;
    unsigned c;

    diff.lo = n1.lo - n2.lo;
    c = (diff.lo & n2.lo) | ((~n1.lo) & (diff.lo | n2.lo));
    c >>= 31;
    diff.hi = n1.hi - n2.hi - c;
    return diff;
}


/*
 * Multiply n1 and n2
 */
bigint i64_mul(bigint n1, bigint n2)
{
    int prodIsMinus;
    bigint prod;
    unsigned m1_0, m1_1, m1_2, m1_3;
    unsigned m2_0, m2_1, m2_2, m2_3;
    unsigned ms;

    prodIsMinus = 0;
    if (n1.hi & 0x80000000) {
        n1 = i64_inv(n1);
        prodIsMinus = ~prodIsMinus;
    }
    if (n2.hi & 0x80000000) {
        n2 = i64_inv(n2);
        prodIsMinus = ~prodIsMinus;
    }

    m1_3 = n1.hi >> 16;
    m1_2 = n1.hi & 0xffff;
    m1_1 = n1.lo >> 16;
    m1_0 = n1.lo & 0xffff;

    m2_3 = n2.hi >> 16;
    m2_2 = n2.hi & 0xffff;
    m2_1 = n2.lo >> 16;
    m2_0 = n2.lo & 0xffff;

    ms = m1_0 * m2_0;
    prod.lo = ms & 0xffff;
    ms >>= 16;

    ms += m1_0 * m2_1;
    prod.hi = ms >> 16;
    ms &= 0xffff;

    ms += m1_1 * m2_0;
    prod.hi += ms >> 16;
    prod.lo += ms << 16;

    prod.hi += (m1_0 * m2_2) + (m1_1 * m2_1) + (m1_2 * m2_0);

    ms = (m1_0 * m2_3) + (m1_1 * m2_2) + (m1_2 * m2_1) + (m1_3 * m2_0);
    prod.hi += ms << 16;

    if (prodIsMinus) {
        prod = i64_inv(prod);
    }

    return prod;
}


static char leftZerosInByte[] = {
/* 00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f */
    8,  7,  6,  6,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,
/* 10  11  12  13  14  15  16  17  18  19  1a  1b  1c  1d  1e  1f */
    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
/* 20  21  22  23  24  25  26  27  28  29  2a  2b  2c  2d  2e  2f */
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
/* 30  31  32  33  34  35  36  37  38  39  3a  3b  3c  3d  3e  3f */
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
/* 40  41  42  43  44  45  46  47  48  49  4a  4b  4c  4d  4e  4f */
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 50  51  52  53  54  55  56  57  58  59  5a  5b  5c  5d  5e  5f */
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 60  61  62  63  64  65  66  67  68  69  6a  6b  6c  6d  6e  6f */
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 70  71  72  73  74  75  76  77  78  79  7a  7b  7c  7d  7e  7f */
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 80  81  82  83  84  78  86  87  88  89  8a  8b  8c  8d  8e  8f */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* 90  91  92  93  94  98  96  97  98  99  9a  9b  9c  9d  9e  9f */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* a0  a1  a2  a3  a4  a8  a6  a7  a8  a9  aa  ab  ac  ad  ae  af */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* b0  b1  b2  b3  b4  b8  b6  b7  b8  b9  ba  bb  bc  bd  be  bf */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* c0  c1  c2  c3  c4  c8  c6  c7  c8  c9  ca  cb  cc  cd  ce  cf */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* d0  d1  d2  d3  d4  d8  d6  d7  d8  d9  da  db  dc  dd  de  df */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* e0  e1  e2  e3  e4  e8  e6  e7  e8  e9  ea  eb  ec  ed  ee  ef */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* f0  f1  f2  f3  f4  f8  f6  f7  f8  f9  fa  fb  fc  fd  fe  ff */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


/*
 * Signed division
 */
void i64_div(bigint dividend, bigint divisor, bigint *quotient, bigint *remainder)
{
    int quotientIsMinus, remainderIsMinus;

    if ((divisor.hi | divisor.lo) == 0) {
        return;
    }
    if ((dividend.hi | dividend.lo) == 0) {
        quotient->hi = 0;
        quotient->lo = 0;
        remainder->hi = 0;
        remainder->lo = 0;
        return;
    }
    quotientIsMinus = 0;
    remainderIsMinus = dividend.hi & 0x80000000;
    if (remainderIsMinus) {
        dividend = i64_inv(dividend);
        quotientIsMinus = ~quotientIsMinus;
    }
    if (divisor.hi & 0x80000000) {
        divisor = i64_inv(divisor);
        quotientIsMinus = ~quotientIsMinus;
    }
    i64_udiv_impl(dividend, divisor, quotient, remainder);
    if (quotientIsMinus) {
        *quotient = i64_inv(*quotient);
    }
    if (remainderIsMinus) {
        *remainder = i64_inv(*remainder);
    }
}


/*
 * Unsigned division
 */
void i64_udiv(bigint dividend, bigint divisor, bigint *quotient, bigint *remainder)
{
    if ((divisor.hi | divisor.lo) == 0) {
        return;
    }
    if ((dividend.hi | dividend.lo) == 0) {
        quotient->hi = 0;
        quotient->lo = 0;
        remainder->hi = 0;
        remainder->lo = 0;
        return;
    }
    i64_udiv_impl(dividend, divisor, quotient, remainder);
}


/*
 * Unsigned division implementation.
 *
 * Note: does not check argument values
 */
static void i64_udiv_impl(bigint dividend, bigint divisor, bigint *quotient,
    bigint *remainder)
{
    int i, j, m, n, d, divisorLeftZeros, dividendLeftZeros;
    unsigned v[4], u[5], q[4], qh, rh, a, b, c;
    bigint mm;

    divisorLeftZeros = 0;
    mm = divisor;
    n = mm.hi >> 24;
    while (n == 0) {
        divisorLeftZeros += leftZerosInByte[n];
        mm.hi <<= 8;
        mm.hi += mm.lo >> 24;
        mm.lo <<= 8;
        n = mm.hi >> 24;
    };
    divisorLeftZeros += leftZerosInByte[n];

    dividendLeftZeros = 0;
    mm = dividend;
    n = mm.hi >> 24;
    while (n == 0) {
        dividendLeftZeros += leftZerosInByte[n];
        mm.hi <<= 8;
        mm.hi += mm.lo >> 24;
        mm.lo <<= 8;
        n = mm.hi >> 24;
    };
    dividendLeftZeros += leftZerosInByte[n];

    n = (79 - divisorLeftZeros) / 16;
    m = (79 - dividendLeftZeros) / 16 - n;

    if (n == 1) {
        quotient->hi = dividend.hi / divisor.lo;
        rh = dividend.hi % divisor.lo;
        a = (rh << 16) + (dividend.lo >> 16);
        quotient->lo = (a / divisor.lo) << 16;
        rh = a % divisor.lo;
        a = (rh << 16) + (dividend.lo & 0xffff);
        quotient->lo += a / divisor.lo;
        remainder->hi = 0;
        remainder->lo = a % divisor.lo;
    }
    else /* if (n > 1) */ {
        d = divisorLeftZeros & 0x0f;

        i = 0;
        u[i++] = (dividend.lo << d) & 0xffff;
        u[i++] = dividend.lo >> (16 - d) & 0xffff;
        u[i++] = ((dividend.hi << d) +
            ((dividend.lo >> 16) >> (16 - d))) & 0xffff;
        u[i++] = (dividend.hi >> (16 - d)) & 0xffff;
        u[i]   = (dividend.hi >> 16) >> (16 - d);

        i = 0;
        v[i++] = (divisor.lo << d) & 0xffff;
        v[i++] = (divisor.lo >> (16 - d)) & 0xffff;
        v[i++] = ((divisor.hi << d) +
            ((divisor.lo >> 16) >> (16 - d))) & 0xffff;
        v[i]   = divisor.hi >> (16 - d);

        memset(q, 0, sizeof(q));

        for (j=m; j >= 0; --j) {
            a = (u[j+n] << 16) + u[j+n-1];
            qh = a / v[n-1];
            rh = a % v[n-1];
            if ((qh & 0x30000) || qh*v[n-2] > (rh << 16) + u[j+n-2]) {
                --qh;
                rh += v[n-1];
                if (! (rh & 0x10000)) {
                    if ((qh & 0x30000) || qh*v[n-2] > (rh << 16) + u[j+n-2]) {
                        --qh;
                    }
                }
            }
            b = 0;
            c = 1;
            for (i=0; i < n; ++i) {
                b = qh * v[i] + b;
                c = u[j+i] + c + ((~b) & 0xffff);
                u[j+i] = c & 0xffff;
                b = b >> 16;
                c = c >> 16;
            }
            c = u[j+i] + c + ((~b) & 0xffff);
            u[j+i] = c & 0xffff;
            if (u[j+n]) {
                c = 0;
                for (i=0; i < n; ++i) {
                    c = u[j+i] + v[i] + c;
                    u[j+i] = c & 0xffff;
                    c = c >> 16;
                }
                c = u[j+i] + c;
                u[j+i] = c & 0xffff;
                --qh;
            }
            q[j] = qh;
        }
        quotient->hi = (q[3] << 16) + q[2];
        quotient->lo = (q[1] << 16) + q[0];
        remainder->hi = (u[3] << (16 - d)) + (u[2] >> d);
        remainder->lo = ((u[2] << 16) << (16 - d)) + (u[1] << (16 - d))
            + (u[0] >> d);
    }
}


/*
 * Convert string to integer
 */
bigint i64_atoi(const char *str)
{
    int i, isNegative;
    bigint ten, digit, n;

    ten.hi = 0;
    ten.lo = 10;
    digit.hi = 0;
    digit.lo = 0;
    n.hi = 0;
    n.lo = 0;
    isNegative = 0;
    i = 0;
    while (isspace(str[i])) {
        ++i;
    }
    if (str[i] == '-') {
        isNegative = 1;
        ++i;
    }
    else if (str[i] == '+') {
        ++i;
    }
    while (isdigit(str[i])) {
        digit.lo = str[i] - '0';
        n = i64_mul(ten, n);
        n = i64_add(digit, n);
        ++i;
    }
    if (isNegative) {
        n = i64_inv(n);
    }
    return n;
}


/*
 * Convert integer to string
 */
void i64_itoa(bigint n, char *str, int strSize)
{
    char s[24];
    int pos;
    bigint ten, digit;

    ten.hi = 0;
    ten.lo = 10;
    pos = 20;
    s[pos] = 0;
    /* positive number */
    if (i64_sgn(n) > 0) {
        while (n.hi != 0 || n.lo != 0) {
            i64_div(n, ten, &n, &digit);
            --pos;
            s[pos] = digit.lo + '0';
        }
    }
    /* negative number */
    else if (i64_sgn(n) < 0) {
        n = i64_inv(n);
        while (n.hi != 0 || n.lo != 0) {
            i64_udiv(n, ten, &n, &digit);
            --pos;
            s[pos] = digit.lo + '0';
        }
        --pos;
        s[pos] = '-';
    }
    /* zero */
    else /* if (i64_sgn(n) == 0) */ {
        --pos;
        s[pos] = '0';
    }
    strncpy(str, &s[pos], strSize);
}

/* Added by Paul Huxham 30/12/2000 */

/* Set the value of a bigint from a long */
bigint i64_set( long n1 )
{
    bigint ret;

    ret.hi = 0;
    ret.lo = n1;

    if ( n1 < 1 ) i64_inv( ret );

    return ret;
}

/* Set the value of a bigint from a ulong */
bigint i64_uset( unsigned long n1 )
{
    bigint ret;

    ret.hi = 0;
    ret.lo = n1;

    return ret;
}

/* Multiply two ulongs together and return a bigint */
bigint i64_uumul( unsigned long n1, unsigned long n2 )
{
    bigint a1, a2;

    a1 = i64_uset( n1 );
    a2 = i64_uset( n2 );

    return i64_mul( a1, a2 );
}

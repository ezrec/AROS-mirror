/*
**
** BITWISE.C    Bit-wise operations on floating point numbers.
**
** Written 11-15-92 in ANSI C
**
** Eval is a floating point expression evaluator.
** This is version 1.12
** Copyright (C) 1993  Will Menninger
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or any
** later version.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 675 Mass Ave, Cambridge, MA 02139, USA.
**
** The author until 9/93 can be contacted at:
** e-mail:     willus@ilm.pfc.mit.edu
** U.S. mail:  Will Menninger, 45 River St., #2, Boston, MA 02108-1124
**
**
*/

#include "eval.h"

#define DATASIZE    ((DBL_MANT_DIG>>3)+2)

typedef unsigned char   uchar;
typedef struct  {
                uchar   data[DATASIZE];
                int     sign;
                int     mask;
                int     numbits;
                int     exponent;
                int     mi;
                } BITARRAY, *BITARRAYPTR;

static void dbl_to_ba(double val,BITARRAYPTR ba);
static double ba_to_dbl(BITARRAYPTR ba);
static void change_exp(BITARRAYPTR ba,int newexp);
static void shift_right(BITARRAYPTR ba,int count);
static void shift_left(BITARRAYPTR ba,int count);
static void ba_negate(BITARRAYPTR ba);
static void ba_not(BITARRAYPTR ba);
#ifdef DEBUG
static void print_ba(BITARRAYPTR ba);
#endif


double not(double val)

    {
    BITARRAY    ba;

    dbl_to_ba(val,&ba);
    ba_not(&ba);
    ba_negate(&ba);
    ba.sign=!ba.sign;
    return(ba_to_dbl(&ba));
    }


double or(double val1,double val2)

    {
    BITARRAY    ba1,ba2;
    int         i;

    dbl_to_ba(val1,&ba1);
    dbl_to_ba(val2,&ba2);
    if (ba1.exponent>ba2.exponent)
        change_exp(&ba2,ba1.exponent);
    else
        change_exp(&ba1,ba2.exponent);
    if (!ba1.sign)
        ba_negate(&ba1);
    if (!ba2.sign)
        ba_negate(&ba2);
    for (i=0;i<=ba1.mi;i++)
        ba1.data[i]|=ba2.data[i];
    ba1.data[ba1.mi]&=ba1.mask;
    if (!ba1.sign || !ba2.sign)
        {
        ba_negate(&ba1);
        ba1.sign=0;
        }
    else
        ba1.sign=1;
    return(ba_to_dbl(&ba1));
    }


double and(double val1,double val2)

    {
    BITARRAY    ba1,ba2;
    int         i;

    dbl_to_ba(val1,&ba1);
    dbl_to_ba(val2,&ba2);
    if (ba1.exponent>ba2.exponent)
        change_exp(&ba2,ba1.exponent);
    else
        change_exp(&ba1,ba2.exponent);
    if (!ba1.sign)
        ba_negate(&ba1);
    if (!ba2.sign)
        ba_negate(&ba2);
    for (i=0;i<=ba1.mi;i++)
        ba1.data[i]&=ba2.data[i];
    ba1.data[ba1.mi]&=ba1.mask;
    if (!ba1.sign && !ba2.sign)
        {
        ba_negate(&ba1);
        ba1.sign=0;
        }
    else
        ba1.sign=1;
    return(ba_to_dbl(&ba1));
    }


double xor(double val1,double val2)

    {
    BITARRAY    ba1,ba2;
    int         i;

    dbl_to_ba(val1,&ba1);
    dbl_to_ba(val2,&ba2);
    if (ba1.exponent>ba2.exponent)
        change_exp(&ba2,ba1.exponent);
    else
        change_exp(&ba1,ba2.exponent);
    if (!ba1.sign)
        ba_negate(&ba1);
    if (!ba2.sign)
        ba_negate(&ba2);
    for (i=0;i<=ba1.mi;i++)
        ba1.data[i]^=ba2.data[i];
    ba1.data[ba1.mi]&=ba1.mask;
    if ((!ba1.sign && ba2.sign)||(ba1.sign && !ba2.sign))
        {
        ba_negate(&ba1);
        ba1.sign=0;
        }
    else
        ba1.sign=1;
    return(ba_to_dbl(&ba1));
    }


static void dbl_to_ba(double val,BITARRAYPTR ba)

    {
    int     i,j,k,m;
    double  f;

    ba->numbits=DBL_MANT_DIG-3;
    ba->mask=0;
    ba->mi=0;
    ba->exponent=0;
    ba->sign=(val>=0.);
    val=fabs(val);
    if (val>0.)
        {
        f=log(val)/log(2.);
        f=floor(f)+1.;
        ba->exponent=f;
        f=val/pow(2.,f);
        f=f+pow(2.,(double)-ba->numbits-1.);
        if (f>=1.)
            {
            ba->exponent++;
            f/=2.;
            }
        }
    else
        f=0.;
    for (i=0;i<DATASIZE;i++)
        ba->data[i]=0;
    for (i=0,j=0,k=128;i<ba->numbits;i++)
        {
        f=f*2.;
        if (f>=1.)
            {
            ba->data[j]|=k;
            f-=1.;
            }
        k>>=1;
        if (!k)
            {
            j++;
            k=128;
            }
        }
    if (k==128)
        {
        ba->mask=255;
        ba->mi=j-1;
        }
    else
        {
        for (k<<=1,m=k;k<128;k<<=1,m|=k);
        ba->mask=m;
        ba->mi=j;
        }
    }


static double ba_to_dbl(BITARRAYPTR ba)

    {
    int     i,j,k;
    double  val;

    val=0.;
    for (i=0,j=0,k=128;i<ba->numbits;i++,k>>=1)
        {
        if (!k)
            {
            k=128;
            j++;
            }
        val=val*2.;
        if (ba->data[j]&k)
            val=val+1.;
        }
    val=val*pow(2.,(double)ba->exponent-ba->numbits);
    return(ba->sign ? val : -val);
    }


static void change_exp(BITARRAYPTR ba,int newexp)

    {
    if (newexp>ba->exponent)
        shift_right(ba,newexp-ba->exponent);
    else
        shift_left(ba,ba->exponent-newexp);
    ba->exponent=newexp;
    }


static void shift_right(BITARRAYPTR ba,int count)

    {
    int     i,j,k,m;
    int     smallshift;
    int     byteshift;

    if (!count)
        return;
    if (count>=ba->numbits)
        {
        for (i=0;i<DATASIZE;i++)
            ba->data[i]=0;
        return;
        }
    smallshift=count&7;
    if (smallshift)
        {
        m=1;
        for (i=1;i<smallshift;i++)
            m=(m<<1)|1;
        k=0;
        for (i=0;i<=ba->mi;i++)
            {
            j=ba->data[i];
            if (i>0)
                ba->data[i]=k|(j>>smallshift);
            else
                ba->data[i]=j>>smallshift;
            k=(j&m)<<(8-smallshift);
            }
        }
    byteshift=(count>>3);
    if (byteshift)
        {
        for (i=ba->mi;i>=byteshift;i--)
            ba->data[i]=ba->data[i-byteshift];
        for (i=byteshift-1;i>=0;i--)
            ba->data[i]=0;
        }
    ba->data[ba->mi]&=ba->mask;
    }


static void shift_left(BITARRAYPTR ba,int count)

    {
    int     i,j,k,m;
    int     smallshift;
    int     byteshift;

    if (!count)
        return;
    if (count>=ba->numbits)
        {
        for (i=0;i<DATASIZE;i++)
            ba->data[i]=0;
        return;
        }
    smallshift=count&7;
    if (smallshift)
        {
        m=128;
        for (i=1;i<smallshift;i++)
            m=(m>>1)|128;
        k=0;
        ba->data[ba->mi]&=ba->mask;
        for (i=ba->mi;i>=0;i--)
            {
            j=ba->data[i];
            if (i<ba->mi)
                ba->data[i]=k|(j<<smallshift);
            else
                ba->data[i]=j<<smallshift;
            k=(j&m)>>(8-smallshift);
            }
        }
    byteshift=(count>>3);
    if (byteshift)
        {
        for (i=0;i<=ba->mi-byteshift;i++)
            ba->data[i]=ba->data[i+byteshift];
        for (i=ba->mi-byteshift+1;i<=ba->mi;i++)
            ba->data[i]=0;
        }
    ba->data[ba->mi]&=ba->mask;
    }


static void ba_negate(BITARRAYPTR ba)

    {
    int     i,j,k,one;

    i=ba->numbits&7;
    if (!i)
        i=8;
    k=128;
    for (j=1;j<i;j++,k>>=1);
    j=ba->mi;
    one=0;
    for (i=ba->numbits-1;i>=0;i--,k<<=1)
        {
        if (k>128)
            {
            k=1;
            j--;
            if (one)
                break;
            }
        if (one)
            ba->data[j]^=k;
        else
            if (ba->data[j]&k)
                {
                one=1;
                if (i==ba->numbits-1)
                    ba->data[j]^=k;
                }
        }
    if (i>0)
        for (;j>=0;j--)
            ba->data[j]=~ba->data[j];
    }



static void ba_not(BITARRAYPTR ba)

    {
    int     i;

    for (i=0;i<=ba->mi;i++)
        ba->data[i]=~ba->data[i];
    ba->data[ba->mi]&=ba->mask;
    }


#ifdef DEBUG
static void print_ba(BITARRAYPTR ba)

    {
    int     i;

    printf(ba->sign ? "+" : "-");
    for (i=0;i<ba->mi;i++)
        printf("%02X",(unsigned)ba->data[i]);
    printf("%02X    %d\n",(unsigned)ba->data[ba->mi]&ba->mask,
            ba->exponent);
    }
#endif

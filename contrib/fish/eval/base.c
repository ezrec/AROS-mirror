/*
** BASE.C   Handles float to ASCII and ASCII to float conversion
**          in any number base.
** Will Menninger, 11-14-92, ANSI C
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
#include <float.h>

#define SCI     1
#define FREE    3

static int  sigfig;
static int  maxexp;
static int  fixed;
static int  sci;
static int  dplace;
static int  maxdigits;
static int  ibase;
static int  obase;

static void sci_out(double val,char *buf);
static void intadd(int x);
static void free_out(double val,char *buf);
static int  digval(char c);
static int  ascii_int(int base,char *s);
static void addtobuf(char *s);
static void initbuf(char *s);
static void incbuf(char c);


void baseconv(double val,char *buf)

    {
    if (sci)
        sci_out(val,buf);
    else
        free_out(val,buf);
    }


void set_scinote(int val)

    {
    sci=val;
    }


void set_sigfig(int val)

    {
    sigfig=val;
    }


void set_dplace(int val)

    {
    dplace=val;
    }


void set_maxexp(int val)

    {
    maxexp=val;
    }


void set_fix(int val)

    {
    fixed=val;
    }


void setobase(int base)

    {
    obase=base;
    maxdigits=precision(base);
    }


void setibase(int base)

    {
    ibase=base;
    }


int getobase(void)

    {
    return(obase);
    }


int getibase(void)

    {
    return(ibase);
    }


int get_scinote(void)

    {
    return(sci);
    }


int get_fix(void)

    {
    return(fixed);
    }


int precision(int base)

    {
    return((int)(DBL_MANT_DIG*log(2.)/log((double)base)));
    }


void print_outtype(void)

    {
    printf("\nScientific notation:       %s\n",sci ? "ON" : "OFF");
    printf("Significant digits:        ");
    if (sigfig>0)
        printf("%d",sigfig);
    else
        printf("max");
    if (!sci && fixed)
        printf(" (ignored)");
    printf("\n");
    printf("Fixed precision:           %s",fixed ? "ON" : "OFF");
    if (sci)
        printf(" (trailing zeroes printed.)");
    printf("\n");
    printf("Digits after the decimal:  %d",dplace);
    if (sci || !fixed)
        printf(" (ignored)");
    printf("\n");
    printf("Exponent limit:            ");
    if (maxexp<0)
        printf("none");
    else
        printf("%d",maxexp);
    if (sci)
        printf(" (ignored)");
    printf("\n\n");
    }


double asciiconv(int base,char *s)

    {
    double  val;
    int     i,div,expc;

    for (i=0;s[i]!=EOS;i++)
        s[i]=tolower((int)s[i]);
    expc= base>14 ? '\\' : 'e';
    div=-1;
    val=0;
    for (i=0;s[i]!=expc && s[i]!='\\' && s[i]!=EOS;i++)
        {
        if (s[i]=='.')
            {
            div++;
            continue;
            }
        if (div>=0)
            div++;
        val=val*base+digval(s[i]);
        }
    if (div<0)
        div=0;
    if (s[i]!=EOS)
        div-=ascii_int(base,&s[i+1]);
    if (div!=0)
        val=val/pow((double)base,(double)div);
    return(val);
    }


static int ascii_int(int base,char *s)

    {
    int     i,val,sign;

    val=0;
    if (s[0]=='-' || s[0]=='+')
        {
        sign=(s[0]=='+');
        i=1;
        }
    else
        {
        sign=1;
        i=0;
        }
    for (;s[i]!=EOS;i++)
        val=base*val+digval(s[i]);
    return(sign ? val : -val);
    }


static int digval(char c)

    {
    c=tolower((int)c);
    return((int)(c<='9' ? c-'0' : c-'a'+10));
    }


static void sci_out(double val,char *buf)

    {
    double   ex,f;
    int      sf,sn,i,j,iex,dec,nz;

    initbuf(buf);
    if (sigfig<=0)
        sf=maxdigits;
    else
        sf=sigfig;
    if (val==0.)
        {
        incbuf((char)'0');
        if (fixed && sf>1)
            {
            incbuf((char)'.');
            for (i=1;i<sf;i++)
                incbuf((char)'0');
            }
        addtobuf(obase>14 ? " x 10^0":"e0");
        return;
        }
    sn=val>0;
    val=fabs(val);
    ex=log(val)/log((double)obase);
    ex=floor(ex)+1.;
    iex=ex;
    val=val+(obase/2.)*pow((double)obase,ex-1.-sf);
    f=val/pow((double)obase,ex);
    if (f>=1.)
        {
        f=f/obase;
        iex++;
        }
    initbuf(buf);
    dec=0;
    nz=0;
    if (!sn)
        incbuf((char)'-');
    for (i=0;i<sf;i++)
        {
        if (i==1)
            dec=1;
        if (i>=maxdigits)
            nz++;
        else
            {
            f=f*obase;
            if (f>=1.)
                {
                if (dec)
                    {
                    incbuf((char)'.');
                    dec=0;
                    }
                if (nz)
                    {
                    for (j=0;j<nz;j++)
                        incbuf((char)'0');
                    nz=0;
                    }
                j=f;
                incbuf((char)(j<10 ? '0'+j : 'A'+j-10));
                f=f-j;
                }
            else
                nz++;
            }
        }
    if (fixed)
        {
        if (dec)
            incbuf((char)'.');
        if (nz)
            for (j=0;j<nz;j++)
                incbuf((char)'0');
        }
    addtobuf(obase>14 ? " x 10^" : "e");
    intadd(iex-1);
    }


static void intadd(int x)

    {
    char    temp[100];
    int     i,j,sn;

    if (!x)
        {
        incbuf((char)'0');
        return;
        }
    sn=(x>=0);
    if (x<0)
        x=-x;
    for (i=0;x;i++)
        {
        j=x%obase;
        temp[i]=j<10 ? j+'0' : 'A'+j-10;
        x/=obase;
        }
    if (!sn)
        incbuf((char)'-');
    for (j=i-1;j>=0;j--)
        incbuf((char)temp[j]);
    }


static void free_out(double val,char *buf)

    {
    double  ex,f;
    int     sf,sn,i,j,k,iex,iex2,nz,dec;

    initbuf(buf);
    if (fixed || sigfig<=0)
        sf=maxdigits;
    else
        sf=sigfig;
    if (val==0.)
        {
        incbuf((char)'0');
        if (fixed)
            {
            if (dplace>0)
                incbuf((char)'.');
            for (i=0;i<dplace;i++)
                incbuf((char)'0');
            }
        return;
        }
    sn=val>0;
    val=fabs(val);
    ex=log(val)/log((double)obase);
    ex=floor(ex)+1.;
    if (maxexp>=0 && fabs(ex)>=maxexp+1)
        {
        sci_out(sn ? val : -val,buf);
        return;
        }
    iex=ex;
    if (fixed)
        iex2=-1-dplace;
    else
        iex2=ex-1-sf;
    val=val+(obase/2.)*pow((double)obase,(double)iex2);
    if (iex-maxdigits-1<iex2)
        val=val+pow((double)obase,(double)iex-maxdigits);
    f=val/pow((double)obase,ex);
    if (f>=1.)
        {
        f=f/obase;
        iex++;
        }
    k=iex;
    if (!sn)
        incbuf((char)'-');
    dec=0;
    nz=0;
    if (fixed && iex<=-dplace)
        {
        incbuf((char)'0');
        if (dplace>0)
            {
            incbuf((char)'.');
            for (j=0;j<dplace;j++)
                incbuf((char)'0');
            }
        return;
        }
    if (iex<=0)
        {
        incbuf((char)'0');
        dec=1;
        nz=-iex;
        }
    for (i=0;fixed || i<sf;i++)
        {
        f=f*obase;
        if (i<maxdigits && f>=1.)
            {
            if (dec)
                {
                incbuf((char)'.');
                dec=0;
                }
            if (nz)
                {
                for (j=0;j<nz;j++)
                    incbuf((char)'0');
                nz=0;
                }
            j=f;
            incbuf((char)(j<10 ? '0'+j : 'A'+j-10));
            f=f-j;
            }
        else
            if (k<=0)
                nz++;
            else
                incbuf((char)'0');
        k--;
        if (fixed && k<=-dplace)
            break;
        if (k==0)
            dec=1;
        }
    if (k>0)
        for (;k>0;k--)
            incbuf((char)'0');
    if (fixed)
        {
        if (dec)
            incbuf((char)'.');
        if (nz)
            for (j=0;j<nz;j++)
                incbuf((char)'0');
        }
    }


static void addtobuf(char *s)

    {
    int     i;

    for (i=0;s[i]!=EOS;i++)
        incbuf((char)s[i]);
    }


static int   bufindex;
static char *bufptr;

static void initbuf(char *s)

    {
    bufindex=0;
    bufptr=s;
    }


static void incbuf(char c)

    {
    if (bufindex>MAXOUTLEN-50)
        return;
    if (bufindex==MAXOUTLEN-50)
        {
        bufindex++;
        strcat(bufptr,"*** BUFFER OVERFLOW!");
        return;
        }
    bufptr[bufindex++]=c;
    bufptr[bufindex]=EOS;
    }

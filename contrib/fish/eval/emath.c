/*
**
** EMATH.C     A module to be included with EVAL.C that supplies
**             math functions not included in ANSI C.
**
** To add/change the functions in Eval, see funcs.c.
**
** NOTICE about this part of the Eval source code:
** ------------------------------------------------------------------------
** If you add source code to this module that is copied from "Numerical
** Recipes in C" or some other book of C math functions, make sure you
** clearly read the license agreement from that book.  In particular,
** "Numerical Recipes in C" forbids you to distribute their source code.
** Because my license agreement requires free distribution of ALL source
** code, you may not add code from a book like "Numerical Recipes in C" and
** then redistribute any of the source or executables.  The upshot of all
** this is that you must write your own versions of whatever math functions
** you add to this code if you plan to redistribute it.
**
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
*/

#include "eval.h"


#define ACCURACY    ((double)1.0e-10)
#define SMALL_PI    3.14
#define MAXRESULT   (DBL_MAX/1.e3)
#define ACC         40.0
#define EXTR        1.0e10
#define MAX         1e14
#define FACTOR      2
#define FIRSTGUESS  .11
#define TINYNUM     1e-6
#define NITER       30

static double x0;

static double bessi0(double x);
static double bessi1(double x);
static double bessj0(double x);
static double bessj1(double x);
static double bessk0(double x);
static double bessk1(double x);
static double hypcos(double x);
static double hypsin(double x);
static double hyptan(double x);

/*
**
**  acosh      Calculates arc-hyperbolic-cosine of a value.
**             Arguments must be >=1.  If not, -1 is returned.
**             Positive values are always returned if the
**             argument is within the correct domain.
**             Abs(Maximum value returned) = 1e14
**
*/
double acosh(double x)

   {
   int     i;
   double  b1,b2;

   if (x<1)
       return(-1.);
   if (x==1.)
       return(0.);
   x0=x;
   if (cosh(.11)>x)
       {
       b1=0.;
       b2=.11;
       }
   else
       {
       for (b2=.1;cosh(b2)<x && b2<MAX;b2*=FACTOR);
       if (b2>MAX)
           return(MAX);
       b1=b2/FACTOR;
       }
   return(root(hypcos,b1,b2,ACCURACY,&i));
   }


static double hypcos(double x)

   {
   return(cosh(x)-x0);
   }


/*
**
**  asinh      Calculates arc-hyperbolic-sine of a value.
**
**             Abs(Maximum value returned) = 1e14
**
*/
double asinh(double x)

   {
   int     i,sign;
   double  b1,b2,acc;

   if (x==0.)
       return(0.);
   if (x>0.)
       sign=1;
   else
       {
       sign=-1;
       x=-x;
       }
   x0=x;
   acc=ACCURACY;
   if (sinh(FIRSTGUESS)>x)
       {
       b1=0.;
       b2=FIRSTGUESS;
       }
   else
       {
       for (b2=.1;sinh(b2)<x && b2<MAX;b2*=FACTOR,acc*=FACTOR);
       if (b2>MAX)
           return(MAX*sign);
       b1=b2/FACTOR;
       }
   return(root(hypsin,b1,b2,acc,&i)*sign);
   }


static double hypsin(double x)

   {
   return(sinh(x)-x0);
   }


/*
**
**  atanh      Calculates arc-hyperbolic-tangent of a value.
**             Value must be between but not including -1 and 1
**             or else 0 is returned.
**
**             Abs(Maximum value returned) = 1e14
**
*/
double atanh(double x)

   {
   int     i,sign;
   double  b1,b2;

   if (x<=-1 || x>=1)
       return(0.);
   if (x==0.)
       return(0.);
   if (x>0.)
       sign=1;
   else
       {
       sign=-1;
       x=-x;
       }
   x0=x;
   if (tanh(.11)>x)
       {
       b1=0.;
       b2=.11;
       }
   else
       {
       for (b2=.1;tanh(b2)<x && b2<MAX;b2*=FACTOR);
       if (b2>MAX)
           return(MAX*sign);
       b1=b2/FACTOR;
       }
   return(root(hyptan,b1,b2,ACCURACY,&i)*sign);
   }


static double hyptan(double x)

   {
   return(tanh(x)-x0);
   }


/*
**
** bessi    Calculates the value of Im(x) using a downward recurrence
**          (Miller's) formula.  Calls bessi0 to normalize.
**
** The idea for Miller's algorithm, and where to start the series,
** was taken from Numerical Recipes in C, but this function is NOT
** a copy of their function, and, I believe, does not infringe on
** their copyright.
**
** Properties:  Im(x) is a solution to the modified (hyperbolic)
**              Bessel equation.
**              Im(x) is monotonically increasing and diverges
**              at infinity.
**
**              I(-m)(x) = Im(x)
**              Im(-x)   = (-1)^m*Im(x)
**              I(m+1)(x) = -m*2/x*Im(x) + I(m-1)(x)
**              I(m-1)(x) = m*2/x*Im(x) + I(m+1)(x)
**
*/
double bessi(int m,double x)

    {
    int     i,negx;
    double  c,ans,nextterm,seed0,seed1;

    if (m<0)
        m=-m;
    if (x==0.)
        return(0.);
    if (m==0)
        return(bessi0(x));
    if (m==1)
        return(bessi1(x));
    negx=(x<0);
    x=fabs(x);
    if (x==0.)
        return(0.);
    c=2./x;
    seed1=1.0;  /* Initial seeds don't matter.  Series converges. */
    seed0=0.0;
    nextterm=0.;
    ans=0.;
    for (i=(int)(m+sqrt(64.*m))&(~1);i>0;i--)
        {
        nextterm=i*c*seed1+seed0;
        if (fabs(nextterm)>EXTR)
            {
            nextterm/=EXTR;
            ans/=EXTR;
            seed1/=EXTR;
            }
        if (i==m)
            ans=seed1;
        seed0=seed1;
        seed1=nextterm;
        }
    ans*=bessi0(x)/nextterm;
    return(negx && (m&1) ? -ans : ans);
    }


/*
** bessi0   Calculate I0(x) from Abromowitz and Stegun, p. 378.
**          Accuracy is better than 2e-7.
*/
static double bessi0(double x)

    {
    double  t,t2,t3,t4,t8,ans;

    x=fabs(x);
    if (x<3.75)
        {
        /* Eq 9.8.1 */
        t=(x/3.75);
        t2=t*t;
        t4=t2*t2;
        t8=t4*t4;
        ans=1+3.5156229*t2+3.0899424*t4+1.2067492*t4*t2+.2659732*t8
             +.0360768*t8*t2+.0045813*t4*t8;
        }
    else
        {
        /* Eq 9.8.2 */
        t=3.75/x;
        t2=t*t;
        t3=t2*t;
        t4=t2*t2;
        ans=.39894228+.01328592*t+.00225319*t2-.00157565*t3
            +.00916281*t4-.02057706*t*t4+.02635537*t2*t4
            -.01647633*t3*t4+.00392377*t4*t4;
        ans=exp(x)*ans/sqrt(x);
        }
    return(ans);
    }


/*
** bessi1   Calculate I1(x) from Abromowitz and Stegun, p. 378.
**          Accuracy is better than 3e-7.
*/
static double bessi1(double x)

    {
    int     negx;
    double  t,t2,t3,t4,t8,ans;

    negx=(x<0.);
    x=fabs(x);
    if (x<3.75)
        {
        /* Eq 9.8.3 */
        t=(x/3.75);
        t2=t*t;
        t4=t2*t2;
        t8=t4*t4;
        ans=.5+.87890594*t2+.51498869*t4+.15084934*t4*t2+.02658733*t8
              +.00301532*t8*t2+.00032411*t8*t4;
        ans*=x;
        }
    else
        {
        /* Eq 9.8.4 */
        t=3.75/x;
        t2=t*t;
        t3=t2*t;
        t4=t2*t2;
        ans=.39894228-.03988024*t-.00362018*t2+.00163801*t3
            -.01031555*t4+.02282967*t*t4-.02895312*t4*t2
            +.01787654*t4*t3-.00420059*t4*t4;
        ans=exp(x)*ans/sqrt(x);
        }
    return(negx ? -ans : ans);
    }


/*
**
** bessj    Calculates the value of Jm(x) using the bessj0 and
**          bessj0 functions and the Bessel function recurrence relation.
**
** The idea for Miller's algorithm, when to use it, and where to start the
** series, was taken from Numerical Recipes in C, but this function is NOT
** a copy of their function, and, I believe, does not infringe on their
** copyright.
**
** Properties:  Jm(x) is a solution to the Bessel function.
**              It is oscillatory in nature and does not diverge at
**              any point.
**
**              J(-m)(x) = (-1)^m*Jm(x)
**              Jm(-x)   = (-1)^m*Jm(x)
**              J(m+1)(x) = m*2/x*Jm(x) - J(m-1)(x)
**              J(m-1)(x) = m*2/x*Jm(x) - J(m+1)(x)
**
**
*/
double bessj(int m,double x)

    {
    int     i,negm,negx;
    double  c,j0,j1,ans,nextterm,seed1,seed0;

    negm=(m<0);
    if (m<0)
        m=-m;
    if (!(m&1))
        negm=0;
    if (m==0)
        return(bessj0(x));
    if (m==1)
        return(negm ? -bessj1(x) : bessj1(x));
    negx=(x<0);
    x=fabs(x);
    if (x==0.)
        return(0.);
    c=2./x;
    if (x>m)
        {
        /* Use forward recurrence relation */
        j0=bessj0(x);
        j1=bessj1(x);
        for (i=1;i<m;i++)
            {
            ans=i*c*j1-j0;
            j0=j1;
            j1=ans;
            }
        }
    else
        {
        /* Use Miller's algorithm of downward recurrence for x<m */
        seed1=1.; /* Start with any seed value--series will converge */
        seed0=0.;
        ans=0.;
        for (i=(int)(m+sqrt(64.*m))&(~1);i>0;i--)
            {
            nextterm=i*c*seed1-seed0;
            if (fabs(nextterm)>EXTR)
                {
                nextterm/=EXTR;
                ans/=EXTR;
                seed1/=EXTR;
                }
            if (i==m)
                ans=seed1;
            seed0=seed1;
            seed1=nextterm;
            }
        /* Normalize with bessj0 */
        ans*=(bessj0(x)/nextterm);
        }
    if (negx && (m&1))
        ans=-ans;
    return(negm ? -ans : ans);
    }


/*
** bessj0   Calculate J0(x) from Abromowitz and Stegun, p. 369.
**          Accuracy is better than 5e-8.
*/
static double bessj0(double x)

    {
    double  ans;
    double  x2,x4,x8;
    double  f0,theta0,thox,thox2,thox4;

    x=fabs(x);
    if (x<3)
        {
        /* Eq 9.4.1 */

        x2=(x/3)*(x/3);
        x4=x2*x2;
        x8=x4*x4;
        ans=1-2.2499997*x2+1.2656208*x4-.3163866*x2*x4
             +.04444479*x8-.0039444*x8*x2+.0002100*x8*x4;
        }
    else
        {
        /* Eq 9.4.3 */

        thox=3./x;
        thox2=thox*thox;
        thox4=thox2*thox2;
        f0=.79788456-.00000077*thox-.00552740*thox2-.00009512*thox2*thox
           +.00137237*thox4-.00072805*thox4*thox+.00014476*thox4*thox2;
        theta0=x-.78539816-.04166397*thox-.00003954*thox2+.00262573*thox2*thox
               -.00054125*thox4-.00029333*thox4*thox+.00013558*thox2*thox4;
        ans=f0*cos(theta0)/sqrt(x);
        }
    return(ans);
    }


/*
** bessj1   Calculate J1(x) from Abromowitz and Stegun, p. 370.
**          Accuracy is better than 5e-8.
*/
static double bessj1(double x)

    {
    int     negx;
    double  ans;
    double  x2,x4,x8;
    double  f1,theta1,thox,thox2,thox4;

    negx=(x<0);
    x=fabs(x);
    if (x<3)
        {
        /* Eq 9.4.4 */

        x2=(x/3)*(x/3);
        x4=x2*x2;
        x8=x4*x4;
        ans=0.5-.56249985*x2+.21093573*x4-.03954289*x4*x2+.00443319*x8
            -.00031761*x8*x2+.00001109*x4*x8;
        ans*=x;
        }
    else
        {
        /* Eq 9.4.6 */

        thox=3./x;
        thox2=thox*thox;
        thox4=thox2*thox2;
        f1=.79788456+.00000156*thox+.01659667*thox2+.00017105*thox*thox2
           -.00249511*thox4+.00113653*thox4*thox-.00020033*thox2*thox4;
        theta1=x-2.35619449+.12499612*thox+.00005650*thox2-.00637879*thox*thox2
               +.00074348*thox4+.00079824*thox4*thox-.00029166*thox4*thox2;
        ans=f1*cos(theta1)/sqrt(x);
        }
    return(negx ? -ans : ans);
    }


/*
**
** bessk   Calculates the value of Km(x) using Bessel function
**         recurrence relations.
**
** Properties:  Km(x) is a solution to the modified
**              (hyperbolic) Bessel equation.  It diverges
**              at zero and is monotonically decreasing.
**
**              Km(x) can also be written as:
**
**                           pi   I(-v)(x) - Iv(x)
**              lim (v->m)  ---- ------------------
**                            2       sin(v*x)
**
**              (only when m is an integer)
**
**
**              K(-m)(x) = Km(x)
**              Km(-x)   = (-1)^m*Km(x)
**              K(m+1)(x) = m*2/x*Km(x) + K(m-1)(x)
**
*/
double bessk(int m,double x)

    {
    int     i,negx;
    double  c,k0,k1,ans;

    if (m<0)
        m=-m;
    if (m==0)
        return(bessk0(x));
    if (m==1)
        return(bessk1(x));
    negx=(x<0.);
    x=fabs(x);
    if (x==0.)
        return(DBL_MAX);
    c=2./x;
    k0=bessk0(x);
    k1=bessk1(x);
    for (i=1;i<m;i++)
        {
        ans=c*i*k1+k0;
        k0=k1;
        k1=ans;
        }
    return(negx && (m&1) ? -ans : ans);
    }


/*
** bessk0   Calculate K0(x) from Abromowitz and Stegun, p. 379.
**          Accuracy is better than 2e-7.
*/
static double bessk0(double x)

    {
    double  ans,t,t2,t4,t8;

    x=fabs(x);
    if (x==0.)
        return(MAXRESULT);
    if (x<2)
        {
        /* Eq 9.8.5 */
        t=x/2.;
        t2=t*t;
        t4=t2*t2;
        t8=t4*t4;
        ans=-.57721566+.42278420*t2+.23069756*t4+.03488590*t4*t2
            +.00262698*t8+.00010750*t8*t2+.00000740*t8*t4;
        ans-=log(x/2)*bessi0(x);
        }
    else
        {
        /* Eq 9.8.6 */
        t=2./x;
        t2=t*t;
        t4=t2*t2;
        ans=1.25331414-.07832358*t+.02189568*t2-.01062446*t2*t
            +.00587872*t4-.00251540*t4*t+.00053208*t4*t2;
        ans*=exp(-x)/sqrt(x);
        }
    return(ans);
    }


/*
** bessk1   Calculate K1(x) from Abromowitz and Stegun, p. 379.
**          Accuracy is better than 3e-7.
*/
static double bessk1(double x)

    {
    int     negx;
    double  ans,t,t2,t4,t8;

    negx=(x<0.);
    x=fabs(x);
    if (x==0.)
        return(MAXRESULT);
    if (x<2)
        {
        /* Eq 9.8.7 */
        t=x/2.;
        t2=t*t;
        t4=t2*t2;
        t8=t4*t4;
        ans=1+.15443144*t2-.67278579*t4-.18156897*t4*t2
             -.01919402*t8-.00110404*t8*t2-.00004686*t8*t4;
        ans=(ans/x)+log(x/2)*bessi1(x);
        }
    else
        {
        /* Eq 9.8.8 */
        t=2./x;
        t2=t*t;
        t4=t2*t2;
        ans=1.25331414+.23498619*t-.03655620*t2+.01504268*t2*t
            -.00780353*t4+.00325614*t4*t-.00068245*t4*t2;
        ans*=exp(-x)/sqrt(x);
        }
    return(negx ? -ans : ans);
    }


/*
**
**  dbessi      Calculates the value of Im'(x) using bessi and
**              Bessel function recurrence relations.
**  Im'(x) = m/x*Im(x) + I(m+1)(x)
**
*/
double dbessi(int m,double x)

    {
    if (fabs(x)<TINYNUM)
        x=x<0 ? -TINYNUM : TINYNUM;
    return(m*bessi(m,x)/x+bessi(m+1,x));
    }


/*
**
**  dbessj      Calculates the value of Jm'(x) using bessj and
**              Bessel function recurrence relations.
**  Jm'(x) = m/x*Jm(x) - J(m+1)(x)
**
*/
double dbessj(int m,double x)

    {
    if (fabs(x)<TINYNUM)
        x=x<0 ? -TINYNUM : TINYNUM;
    return(m*bessj(m,x)/x-bessj(m+1,x));
    }


/*
**
**  dbessk      Calculates the value of Km'(x) using bessk and
**              Bessel function recurrence relations.
**  Km'(x) = m/x*Km(x) - K(m+1)(x)
**
*/
double dbessk(int m,double x)

    {
    if (fabs(x)<TINYNUM)
        x=x<0 ? -TINYNUM : TINYNUM;
    return(m*bessk(m,x)/x-bessk(m+1,x));
    }


/*
**
** djroot       Calculates the nth non-zero root of the mth
**              order bessel function's derivative, Jm'(x)
**
*/
double djroot(int m,int n)

   {
   double  f0,f1,step,x0,x1;
   int     flag,nroot;

   if (n<1)
       return(0.);
   x0=(double)m+.5;
   step= n==1 ? SMALL_PI/4. : SMALL_PI;
   x1=x0+step;
   f0=dbessj(m,x0);
   f1=dbessj(m,x1);
   for (nroot=0 ;1; x0=x1,f0=f1,x1+=step,f1=dbessj(m,x1))
       {
       if (SGN(f0)*SGN(f1)<0)
           {
           nroot++;
           if (nroot==n)
               break;
           if (nroot==n-1)
               step/=4.;
           }
       }
   return(root2(dbessj,m,x0,x1,ACCURACY,&flag));
   }


/*
** factorial
*/
double factorial(int m)

    {
    int     i;
    double  ans;

    if (m<2)
        return(1.);
    ans=1.0;
    for (i=2;i<=m;i++)
        ans*=i;
    return(ans);
    }


/*
**
**  jroot       Calculates the nth non-zero root of the mth
**              order bessel function, Jm(x)
**
*/
double jroot(int m,int n)

   {
   double  f0,f1,step,x0,x1;
   int     flag,nroot;

   if (n<1)
       return(0.);
   x0=(double)m+2.;
   step= n==1 ? SMALL_PI/4. : SMALL_PI;
   x1=x0+step;
   f0=bessj(m,x0);
   f1=bessj(m,x1);
   for (nroot=0 ;1; x0=x1,f0=f1,x1+=step,f1=bessj(m,x1))
       {
       if (SGN(f0)*SGN(f1)<0)
           {
           nroot++;
           if (nroot==n)
               break;
           if (nroot==n-1)
               step/=4.;
           }
       }
   return(root2(bessj,m,x0,x1,ACCURACY,&flag));
   }


/*
**
** root     Finds root of function F(x) between x0 and x1
**          to precision: F(root) = 0. +/- xacc
**
** Returns:  The root of the function.
**
** Also returns status in "flag" variable.
** flag=0 if root found.
** flag=1 if root not found.  This could be due to NITER iterations being
**        exceeded (a diverging solution pehaps?) or x0 and x1 not bracketing
**        the root.
**
*/
double root(double(*F)(double),double x0,double x1,double xacc,int *flag)

   {
   int     n;
   double  f2,f3,f4,x2,x3,x4;

   (*flag)=1;
   if (x0==x1)
       return(0.);
   x2=x0;
   x3=x1;
   f2=(*F)(x2);
   f3=(*F)(x3);
   if (SGN(f2)*SGN(f3)>0)
       return(0.);
   for (n=1;n<=NITER;n++)
       {
       if (n<=4)
           x4=(x2+x3)/2.;
       else
           x4=(f3*x2-f2*x3)/(f3-f2);
       f4=(*F)(x4);
       if (ABS(f4)<xacc)
           {
           (*flag)=0;
           return(x4);
           }
       if (SGN(f2)*SGN(f4)<0)
           {
           if (n>4 && ABS(f4)>ABS(f3))
               return(0.);
           f3=f4;
           x3=x4;
           continue;
           }
       if (n>4 && ABS(f4)>ABS(f2))
           return(0.);
       f2=f4;
       x2=x4;
       }
   return(0.);
   }


/*
**  root2   Finds root of function F(m,x).  Works the same way as root.
*/
double root2(double(*F)(int,double),int m,double x0,double x1,double xacc,
             int *flag)

   {
   int     n;
   double  f2,f3,f4,x2,x3,x4;

   (*flag)=1;
   if (x0==x1)
       return(0.);
   x2=x0;
   x3=x1;
   f2=(*F)(m,x2);
   f3=(*F)(m,x3);
   if (SGN(f2)*SGN(f3)>0)
       return(0.);
   for (n=1;n<=NITER;n++)
       {
       if (n<=4)
           x4=(x2+x3)/2.;
       else
           x4=(f3*x2-f2*x3)/(f3-f2);
       f4=(*F)(m,x4);
       if (ABS(f4)<xacc)
           {
           (*flag)=0;
           return(x4);
           }
       if (SGN(f2)*SGN(f4)<0)
           {
           if (n>4 && ABS(f4)>ABS(f3))
               return(0.);
           f3=f4;
           x3=x4;
           continue;
           }
       if (n>4 && ABS(f4)>ABS(f2))
           return(0.);
       f2=f4;
       x2=x4;
       }
   return(0.);
   }

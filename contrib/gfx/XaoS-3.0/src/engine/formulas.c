
/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996,1997 by
 *
 *      Jan Hubicka          (hubicka@paru.cas.cz)
 *      Thomas Marsh         (tmarsh@austin.ibm.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifdef _plan9_
#include <u.h>
#include <libc.h>
#include <stdio.h>
#else
/* Hello reader!
 * code you are looking at is dangerous for both you and your hardware! PLEASE
 * CLOSE THIS FILE UNLESS YOU REALY KNOW WHAT YOU ARE DOING.
 *
 * Main purpose of this file is to generate optimal caluclation loops for
 * various formulas/algorithms. It heavily includes docalc.c - set of
 * caluclation loops, that then uses macros instad of formulas. This lets me
 * to change calculation loops easily. At the other hand it looks very ugly.
 * You have been warned :)
 */

#ifndef _MAC
#include <aconfig.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <string.h>
#include <math.h>
#ifdef __EMX__
#include <float.h>
#include <sys/cdefs.h>
#endif
#include <stdio.h>
#endif /*plan9 */
#include <archaccel.h>
#include <config.h>
#include <complex.h>
#include <filter.h>
#include <fractal.h>
#include "julia.h"
#ifndef M_PI
#define M_PI 3.1415
#endif

#ifdef SLOWFUNCPTR
#define FUNCTYPE INLINE
#else
#define FUNCTYPE
#endif


char *incolorname[] =
{
  "maxiter",
  "zmag",
  "Decomposition-like",
  "real/imag",
  "abs(abs(c)-abs(r))",
  "cos(mag)",
  "mag*cos(real^2)",
  "sin(real^2-imag^2)",
  "atan(real*imag*creal*cimag)",
  "squares",
  "True-color",
  NULL
};

char *outcolorname[] =
{
  "iter",
  "iter+real",
  "iter+imag",
  "iter+real/imag",
  "iter+real+imag+real/imag",
  "binary decomposition",
  "biomorphs",
  "potential",
  "color decomposition",
  "smooth",
  "True-color",
  NULL
};
char *tcolorname[] =
{
  "black",
  "re*im sin(re^2) angle",
  "sin(re) sin(im) sin(square)",
  "hsv",
  "hsv2",
  "cos(re^c) cos(im^2) cos(square)",
  "abs(re^2) abs(im^2) abs(square)",
  "re*im re*re im*im",
  "abs(im*cim) abs(re*cre) abs(re*cim)",
  "abs(re*im-csqr) abs(re^2-csqr) abs(im^2-csqr)",
  "angle angle2 angle",
  "Disable truecolor colouring",
  NULL
};
#define SHIFT 8
#define SMUL 256



#define PERIINOUTPUT() STAT(nperi++;ninside2++);return(cpalette.pixels[0])

#define OUTOUTPUT() STAT(niter2+=iter);return(!cfractalc.coloringmode?cpalette.pixels[(iter%(cpalette.size-1))+1]:color_output(zre,zim,pre,pim,iter))
#define INOUTPUT() STAT(niter1+=iter;ninside2++);return(cfractalc.incoloringmode?incolor_output(zre,zim,pre,pim,iter):cpalette.pixels[0])

#define OUTPUT() if(iter>=(int)cfractalc.maxiter)\
                { \
                  if(cfractalc.incoloringmode==10) return(truecolor_output(zre,zim,pre,pim,iter,cfractalc.intcolor,1)); \
		  INOUTPUT();  \
                } \
		else  { \
                  if(cfractalc.coloringmode==10) return(truecolor_output(zre,zim,pre,pim,iter,cfractalc.outtcolor,0)); \
		  OUTOUTPUT(); \
		}

#define SMOOTHOUTPUT() {PRESMOOTH;zre+=0.000001;szmag+=0.000001; \
                        iter=((cfractalc.maxiter-iter)*256+log(4/(szmag))/log((zre)/(szmag))*256); \
                        if (iter < 0) {\
                          iter = ((cpalette.size - 1) << 8) - ((-iter) % ((cpalette.size - 1) << 8))-1; \
                          if (iter < 0) iter=0; \
			} \
                        iter %= (cpalette.size - 1) << 8; \
 \
                        if ((cpalette.type & (C256 | LARGEITER | SMALLITER)) || !(iter & 255)) \
                          return (cpalette.pixels[1 + (iter >> 8)]); \
                        { \
                          unsigned int i1, i2; \
                          i1 = cpalette.pixels[1 + (iter >> 8)]; \
                          if ((iter >> 8) == cpalette.size - 2) \
                            i2 = cpalette.pixels[1]; \
                          else \
                            i2 = cpalette.pixels[2 + (iter >> 8)]; \
                          iter &= 255; \
                          return (interpoltype (cpalette.type, i2, i1, iter)); \
                        } \
                      }

// static INLINE void
// hsv_to_rgb (int h, int s, int v, int *red, int *green, int *blue) CONSTF;
static INLINE void
hsv_to_rgb (int h, int s, int v, int *red, int *green, int *blue)
{
  int hue;
  int f, p, q, t;

  if (s == 0)
    {
      *red = v;
      *green = v;
      *blue = v;
    }
  else
    {
      h %= 256;
      if (h < 0)
	h += 256;
      hue = h * 6;

      f = hue & 255;
      p = v * (256 - s) / 256;
      q = v * (256 - (s * f) / 256) >> 8;
      t = v * (256 * 256 - (s * (256 - f))) >> 16;
      /*printf("%i %i\n",hue/256,h); */

      switch ((int) (hue / 256))
	{
	case 0:
	  *red = v;
	  *green = t;
	  *blue = p;
	  break;
	case 1:
	  *red = q;
	  *green = v;
	  *blue = p;
	  break;
	case 2:
	  *red = p;
	  *green = v;
	  *blue = t;
	  break;
	case 3:
	  *red = p;
	  *green = q;
	  *blue = v;
	  break;
	case 4:
	  *red = t;
	  *green = p;
	  *blue = v;
	  break;
	case 5:
	  *red = v;
	  *green = p;
	  *blue = q;
	  break;
	}
    }
}

// static int
// truecolor_output (number_t zre, number_t zim, number_t pre, number_t pim, int iter, int mode, int inset) CONSTF REGISTERS (3);
static int
truecolor_output (number_t zre, number_t zim, number_t pre, number_t pim, int iter, int mode, int inset)
{
  /*WARNING r and b fiends are swapped for HISTORICAL REASONS (BUG :)*/
  int r = 0, g = 0, b = 0, w = 0;

  switch (mode)
    {
    case 0:
      break;
    case 1:
      b = (sin ((double) atan2 (zre, zim) * 20) + 1) * 127;
      /*w=(sin((double)zim/zre)+1)*127; */
      w = (sin ((double) zim / zre)) * 127;
      r = zre * zim;
      g = (sin ((double) (zre * zre) / 2) + 1) * 127;
      /*(int)((atan2(zre,zim)+M_PI)*127/M_PI)*256 */
      break;
    case 2:
      if (!inset)
	{
	  r = (sin ((double) zre * 2) + 1) * 127;
	  g = (sin ((double) zim * 2) + 1) * 127;
	  b = (sin ((double) (zim * zim + zre * zre) / 2) + 1) * 127;
	}
      else
	{
	  r = (sin ((double) zre * 50) + 1) * 127;
	  g = (sin ((double) zim * 50) + 1) * 127;
	  b = (sin ((double) (zim * zim + zre * zre) * 50) + 1) * 127;
	}
      w = (sin ((double) zim / zre)) * 127;
      break;
    case 3:
      if (inset)
	hsv_to_rgb (atan2 (zre, zim) * 256 / M_PI, (sin (zre * 50) + 1) * 128, (sin (zim * 50) + 1) * 128, &r, &g, &b);
      else
	hsv_to_rgb (atan2 (zre, zim) * 256 / M_PI, (sin (zre) + 1) * 128, (sin (zim) + 1) * 128, &r, &g, &b);
      break;
    case 4:
      if (inset)
	hsv_to_rgb (sin ((zre * zre + zim * zim) * 0.1) * 256, sin (atan2 (zre, zim) * 10) * 128 + 128, (sin ((zre + zim) * 10)) * 65 + 128, &r, &g, &b);
      else
	hsv_to_rgb (sin ((zre * zre + zim * zim) * 0.01) * 256, sin (atan2 (zre, zim) * 10) * 128 + 128, (sin ((zre + zim) * 0.3)) * 65 + 128, &r, &g, &b);
      break;
    case 5:
      {
	if (!inset)
	  {
	    r = (int) (cos (myabs (zre * zre)) * 128) + 128;
	    g = (int) (cos (myabs (zre * zim)) * 128) + 128;
	    b = (int) (cos (myabs (zim * zim + zre * zre)) * 128) + 128;
	  }
	else
	  {
	    r = (int) (cos (myabs (zre * zre) * 10) * 128) + 128;
	    g = (int) (cos (myabs (zre * zim) * 10) * 128) + 128;
	    b = (int) (cos (myabs (zim * zim + zre * zre) * 10) * 128) + 128;
	  }
      }
      break;
    case 6:
      {
	if (!inset)
	  {
	    r = (int) (zre * zim * 64);
	    g = (int) (zre * zre * 64);
	    b = (int) (zim * zim * 64);
	  }
	else
	  r = (int) (zre * zim * 256);
	g = (int) (zre * zre * 256);
	b = (int) (zim * zim * 256);
      }
      break;
    case 7:
      {
	if (!inset)
	  {
	    r = (int) ((zre * zre + zim * zim - pre * pre - pim * pim) * 16);
	    g = (int) ((zre * zre * 2 - pre * pre - pim * pim) * 16);
	    b = (int) ((zim * zim * 2 - pre * pre - pim * pim) * 16);
	  }
	else
	  {
	    r = (int) ((zre * zre + zim * zim - pre * pre - pim * pim) * 256);
	    g = (int) ((zre * zre * 2 - pre * pre - pim * pim) * 256);
	    b = (int) ((zim * zim * 2 - pre * pre - pim * pim) * 256);
	  }
      }
      break;
    case 8:
      {
	if (!inset)
	  {
	    r = (int) ((myabs (zim * pim)) * 64);
	    g = (int) ((myabs (zre * pre)) * 64);
	    b = (int) ((myabs (zre * pim)) * 64);
	  }
	else
	  {
	    r = (int) ((myabs (zim * pim)) * 256);
	    g = (int) ((myabs (zre * pre)) * 256);
	    b = (int) ((myabs (zre * pim)) * 256);
	  }
      }
      break;
    case 9:
      {
	if (!inset)
	  {
	    r = (int) ((myabs (zre * zim - pre * pre - pim * pim)) * 64);
	    g = (int) ((myabs (zre * zre - pre * pre - pim * pim)) * 64);
	    b = (int) ((myabs (zim * zim - pre * pre - pim * pim)) * 64);
	  }
	else
	  {
	    r = (int) ((myabs (zre * zim - pre * pre - pim * pim)) * 256);
	    g = (int) ((myabs (zre * zre - pre * pre - pim * pim)) * 256);
	    b = (int) ((myabs (zim * zim - pre * pre - pim * pim)) * 256);
	  }
      }
      break;
    case 10:
      {
	r = (int) (atan2 (zre, zim) * 128 / M_PI) + 128;
	g = (int) (atan2 (zre, zim) * 128 / M_PI) + 128;
	b = (int) (atan2 (zim, zre) * 128 / M_PI) + 128;
      }
      break;
    }

  r += w;
  g += w;
  b += w;
  if (r < 0)
    r = 0;
  else if (r > 255)
    r = 255;
  if (g < 0)
    g = 0;
  else if (g > 255)
    g = 255;
  if (b < 0)
    b = 0;
  else if (b > 255)
    b = 255;

  switch (cpalette.type)
    {
    case GRAYSCALE:
      return ((r * 76 + g * 151 + b * 29) * (cpalette.end - cpalette.start) >> 16) + cpalette.start;
    case TRUECOLOR:
      return (r + (g << 8) + (b << 16));
#ifdef SUPPORTMISSORDERED
    case TRUECOLORMI:
      return ((r<<24) + (g << 16) + (b << 8));
#endif
#ifdef SHICOLOR
    case HICOLOR:
      r >>= 3;
      g >>= 2;
      b >>= 3;
      return (r + (g << 5) + (b << 11));
#endif
#ifdef SREALCOLOR
    case REALCOLOR:
      r >>= 3;
      g >>= 3;
      b >>= 3;
      return (r + (g << 5) + (b << 10));
#endif
    }

  return cpalette.pixels[inset];
}
#ifdef __alpha__
#define __TEST__
#endif
// static int
// color_output (number_t zre, number_t zim, number_t pre, number_t pim, int iter) CONSTF REGISTERS (3);
static int
color_output (number_t zre, number_t zim, number_t pre, number_t pim, int iter)
{
  iter <<= SHIFT;

  switch (cfractalc.coloringmode)
    {
    case 9:
      break;
    case 1:			/* real */
      iter = (iter + zre * SMUL);
      break;
    case 2:			/* imag */
      iter = (iter + zim * SMUL);
      break;
    case 3:			/* real / imag */
#ifdef __TEST__
      if (zim != 0)
#endif
	iter = (iter + (zre / zim) * SMUL);
      break;
    case 4:			/* all of the above */
#ifdef __TEST__
      if (zim != 0)
#endif
	iter = (iter + (zre + zim + zre / zim) * SMUL);
      break;
    case 5:
      if (zim > 0)
	iter = ((cfractalc.maxiter << SHIFT) - iter);
      break;
    case 6:
      if (myabs (zim) < 2.0 || myabs (zre) < 2.0)
	iter = ((cfractalc.maxiter << SHIFT) - iter);
      break;
    case 7:
      zre=zre*zre+zim*zim;
#ifdef __TEST__
      if(zre<1||!iter) iter=0; else
#endif
      iter = (sqrt (log (zre) / iter) * 256 * 256);
      break;
    default:
    case 8:
      iter = ((atan2 (zre, zim) / (M_PI + M_PI) + 0.75) * 20000);
      break;
    }

  if (iter < 0) {
    iter = ((cpalette.size - 1) << 8) - ((-iter) % ((cpalette.size - 1) << 8)) - 1;
    if (iter < 0) iter=0;
  }
  iter %= (cpalette.size - 1) << 8;
  if ((cpalette.type & (C256 | LARGEITER | SMALLITER)) || !(iter & 255))
    return (cpalette.pixels[1 + (iter >> 8)]);
  {
    unsigned int i1, i2;

    i1 = cpalette.pixels[1 + (iter >> 8)];

    if ((iter >> 8) == cpalette.size - 2)
      i2 = cpalette.pixels[1];
    else
      i2 = cpalette.pixels[2 + (iter >> 8)];

    iter &= 255;
    return (interpoltype (cpalette.type, i2, i1, iter));
  }

}

// static int
// incolor_output (number_t zre, number_t zim, number_t pre, number_t pim, int iter) CONSTF REGISTERS (3);
static int
incolor_output (number_t zre, number_t zim, number_t pre, number_t pim, int iter)
{
  switch (cfractalc.incoloringmode)
    {
    case 1:			/* zmag */
      iter = ((zre * zre + zim * zim) * (number_t) (cfractalc.maxiter >> 1) * SMUL + SMUL);
      break;
    case 2:			/* real */
      iter = ((atan2 (zre, zim) / (M_PI + M_PI) + 0.75) * 20000);
      break;
    default:
      break;
    case 3:			/* real / imag */
      iter = (100 + (zre / zim) * SMUL * 10);
      break;
    case 4:
      zre = myabs (zre);
      zim = myabs (zim);
      pre = myabs (pre);
      pre = myabs (pim);
      iter += myabs (pre - zre) * 256 * 64;
      iter += myabs (pim - zim) * 256 * 64;
      break;
    case 5:
      if (((int) ((zre * zre + zim * zim) * 10)) % 2)
	iter = cos (zre * zim * pre * pim) * 256 * 256;
      else
	iter = sin (zre * zim * pre * pim) * 256 * 256;
      break;
    case 6:
      iter = (zre * zre + zim * zim) * cos (zre * zre) * 256 * 256;
      break;
    case 7:
      iter = sin (zre * zre - zim * zim) * 256 * 256;
      break;
    case 8:
      iter = atan (zre * zim * pre * pim) * 256 * 64;
      break;
    case 9:
      if ((abs ((int) (zre * 40)) % 2) ^ (abs ((int) (zim * 40)) % 2))
	iter = ((atan2 (zre, zim) / (M_PI + M_PI) + 0.75) * 20000);
      else
	iter = ((atan2 (zim, zre) / (M_PI + M_PI) + 0.75) * 20000);
      break;
    };

  if (iter < 0) {
    iter = ((cpalette.size - 1) << 8) - ((-iter) % ((cpalette.size - 1) << 8)) - 1;
    if (iter < 0) iter=0;
  }
  iter %= (cpalette.size - 1) << 8;

  if ((cpalette.type & (C256 | LARGEITER | SMALLITER)) || !(iter & 255))
    return (cpalette.pixels[1 + (iter >> 8)]);
  {
    unsigned int i1, i2;
    i1 = cpalette.pixels[1 + (iter >> 8)];
    if ((iter >> 8) == cpalette.size - 2)
      i2 = cpalette.pixels[1];
    else
      i2 = cpalette.pixels[2 + (iter >> 8)];
    iter &= 255;
    return (interpoltype (cpalette.type, i2, i1, iter));
  }

}

#define VARIABLES
#define INIT
#define UNCOMPRESS
#define USEHACKS
#define PRETEST 0
#define FORMULA  \
	    zim=(zim*zre)*2+pim; \
	    zre = rp - ip + pre; \
            ip=zim*zim; \
            rp=zre*zre;

#define BTEST (rp+ip<4)
#define SMOOTH
#define SCALC smand_calc
#define SPERI smand_peri
#define CALC mand_calc
#define PERI mand_peri
#define JULIA mand_julia
#define RANGE 2
#define RPIP
#include "docalc.c"

#define UNCOMPRESS
#define USEHACKS
#define PRETEST 0
#define FORMULA \
	rp = zre * (rp - 3 * ip); \
	zim = zim * (3 * zre * zre - ip) + pim; \
	zre = rp + pre; \
	rp = zre * zre; \
	ip = zim * zim;
#define BTEST (rp+ip<4)
#define SMOOTH
#define SCALC smand3_calc
#define SPERI smand3_peri
#define CALC mand3_calc
#define PERI mand3_peri
#define JULIA mand3_julia
#define RANGE 2
#define RPIP
#include "docalc.c"


#define UNCOMPRESS
#define VARIABLES number_t br,tmp;
#define FORMULA \
	br = zre + zre + pre - 2; \
	tmp = zre * zim; \
	zre = rp - ip + pre - 1; \
	ip = zim + zim + pim; \
	zim = tmp + tmp + pim; \
	tmp = 1 / (br * br + ip * ip); \
	rp = (zre * br + zim * ip) * tmp; \
	ip = (zim * br - zre * ip) * tmp; \
	zre = (rp + ip) * (rp - ip); \
	zim = rp * ip; \
	zim += zim; \
	rp = zre - 1; \
	ip = zim * zim; \
	rp = zre * zre;
#define BTEST (rp+ip<100*100&&(rp-2*zre+ip)>-0.99)
#define POSTCALC if(rp-2*zre+ip>-0.99) zre *= 0.02, zim *= 0.02;
#define CALC magnet_calc
#define PERI magnet_peri
#define SCALC smagnet_calc
#define SPERI smagnet_peri
#define SMOOTH
#define PRESMOOTH szmag/=100*100/4;zre=(rp+ip)/(100*100*4);
#define JULIA magnet_julia
#define RANGE 4
#define RPIP
#include "docalc.c"

#define UNCOMPRESS
#define BTEST (rp+ip<4)
#define FORMULA \
	rp = rp * rp - 6 * rp * ip + ip * ip + pre; \
	zim = 4 * zre * zre * zre * zim - 4 * zre * ip * zim + pim; \
	zre = rp; \
	rp = zre * zre; \
	ip = zim * zim;
#define SMOOTH
#define SCALC smand4_calc
#define SPERI smand4_peri
#define CALC mand4_calc
#define PERI mand4_peri
#define JULIA mand4_julia
#define RANGE 2
#define RPIP
#include "docalc.c"

#define VARIABLES register number_t t;
#define BTEST (rp+ip<4)
#define FORMULA \
	c_pow4(zre, zim, rp, ip); \
	c_mul(zre, zim, rp, ip, t, zim); \
	zre = t + pre; \
	zim += pim; \
	rp = zre * zre; \
	ip = zim * zim;
#define SMOOTH
#define SCALC smand5_calc
#define SPERI smand5_peri
#define CALC mand5_calc
#define PERI mand5_peri
#define JULIA mand5_julia
#define RANGE 2
#define RPIP
#include "docalc.c"


#define VARIABLES register number_t t;
#define BTEST (rp+ip<4)
#define FORMULA \
	c_pow3(zre, zim, rp, ip); \
	c_pow3(rp, ip, t, zim); \
	zre = t + pre; \
	zim += pim; \
	rp = zre * zre; \
	ip = zim * zim;
#define SMOOTH
#define SCALC smand6_calc
#define SPERI smand6_peri
#define CALC mand6_calc
#define PERI mand6_peri
#define JULIA mand6_julia
#define RANGE 2
#define RPIP
#include "docalc.c"

/* plan9 compiler has problem with rest of formulas files. Hope that will be fixed later */

#define VARIABLES
#define BTEST (rp+ip<4)
#define FORMULA \
	if (zre >= 0) { \
	    rp = zre - 1; \
	} else { \
	    rp = zre + 1; \
	} \
	c_mul(rp, zim, pre, pim, zre, zim); \
	rp = zre * zre; \
	ip = zim * zim;
#define SMOOTH
#define SCALC sbarnsley1_calc
#define CALC barnsley1_calc
#define JULIA barnsley1_julia
#define RANGE 2
#define RPIP
#include "docalc.c"


#define VARIABLES register number_t n,sqrr,sqri,zre1,zim1;
#define INIT sqri=zim*zim,n=zre,zre=pre,pre=n,n=zim,zim=pim,pim=n,n=1;
#define BTEST (n>1E-6)
#define FORMULA \
	zre1 = zre; \
	zim1 = zim; \
	n = zim * zim; \
	sqri = zre * zre; \
	sqrr = sqri - n; \
	sqri = n + sqri; \
	n = 0.3333333333 / ((sqri * sqri)); \
	zim = (0.66666666) * zim - (zre + zre) * zim * n + pim; \
	zre = (0.66666666) * zre + (sqrr) * n + pre; \
	zre1 -= zre; \
	zim1 -= zim; \
	n = zre1 * zre1 + zim1 * zim1;
#define CALC newton_calc
#define RPIP
#include "docalc.c"


#define VARIABLES register number_t zpr,zip;
#define SAVEVARIABLES register number_t szpr,szip;
#define SAVE szpr=zpr,szip=zip;
#define RESTORE zpr=szpr,zip=szip;
#define INIT zpr=zip=0;
#define BTEST (rp+ip<4)
#define FORMULA \
	rp = rp - ip + pre + pim * zpr; \
	ip = 2 * zre * zim + pim * zip; \
	zpr = zre, zip = zim; \
	zre = rp; \
	zim = ip; \
	rp = zre * zre, ip = zim * zim;
#define SMOOTH
#define SCALC sphoenix_calc
#define SPERI sphoenix_peri
#define CALC phoenix_calc
#define PERI phoenix_peri
#define RPIP
#include "docalc.c"


#define VARIABLES register number_t tr,ti,zpr,zpm,rp1,ip1;
#define INIT zpr=zpm=0,tr=zre,zre=pre,pre=tr,tr=zim,zim=pim,pim=tr,tr=1;
#define BTEST (zpr*zpr+zpm*zpm<4)
#define FORMULA \
	rp1 = zre; \
	ip1 = zim; \
	c_pow3(zre, zim, tr, ti); \
	c_add(tr, ti, zpr, zpm, zre, zim); \
	zpr = rp1 + pre; \
	zpm = ip1 + pim;
#define CALC octo_calc
#define SCALC socto_calc
#define SMOOTH
#define CUSTOMSAVEZMAG szmag=zpr*zpr+zpm*zpm
#define PRESMOOTH zre=zpr*zpr+zpm*zpm
#include "docalc.c"

symetry sym6[] =
{
  {0, 1.73205080758},
  {0, -1.73205080758}
};

symetry sym8[] =
{
  {0, 1},
  {0, -1}
};

symetry sym16[] =
{
  {0, 1},
  {0, -1},
  {0, 0.414214},
  {0, -0.414214},
  {0, 2.414214},
  {0, -2.414214}
};

struct formula formulas[] =
{
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    mand_calc,
    mand_peri,
    smand_calc,
    smand_peri,
#endif
    mand_julia,
    {"Mandelbrot", "Julia"},
    "mandel",
	/*{0.5, -2.0, 1.25, -1.25}, */
    {-0.75, 0.0, 2.5, 2.5},
    1, 1, 0.0, 0.0,
    {
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    MANDEL_BTRACE,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    mand3_calc,
    mand3_peri,
    smand3_calc,
    smand3_peri,
#endif
    mand3_julia,
    {"Mandelbrot^3", "Julia^3"},
    "mandel3",
	/*{1.25, -1.25, 1.25, -1.25}, */
    {0.0, 0.0, 2.5, 2.5},
    1, 1, 0.0, 0.0,
    {
      {0, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {0, 0, 0, NULL},
      {0, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {0, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {0, 0, 0, NULL},
      {0, 0, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    MANDEL_BTRACE,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    mand4_calc,
    mand4_peri,
    smand4_calc,
    smand4_peri,
#endif
    mand4_julia,
    {"Mandelbrot^4", "Julia^4"},
    "mandel4",
	/*{1.25, -1.25, 1.25, -1.25}, */
    {0.0, 0.0, 2.5, 2.5},
    1, 1, 0.0, 0.0,
    {
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    MANDEL_BTRACE,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    mand5_calc,
    mand5_peri,
    smand5_calc,
    smand5_peri,
#endif
    mand5_julia,
    {"Mandelbrot^5", "Julia^5"},
    "mandel5",
	/*{1.25, -1.25, 1.25, -1.25}, */
    {0.0, 0.0, 2.5, 2.5},
    1, 1, 0.0, 0.0,
    {
      {0, 0, 2, sym8},
      {INT_MAX, 0, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {0, 0, 2, sym8},
      {0, 0, 2, sym8},
      {INT_MAX, INT_MAX, 0, NULL},
      {0, 0, 2, sym8},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {0, 0, 2, sym8},
      {0, 0, 2, sym8},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    MANDEL_BTRACE,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    mand6_calc,
    mand6_peri,
    smand6_calc,
    smand6_peri,
#endif
    mand6_julia,
    {"Mandelbrot^6", "Julia^6"},
    "mandel6",
	/*{1.25, -1.25, 1.25, -1.25}, */
    {0.0, 0.0, 2.5, 2.5},
    1, 1, 0.0, 0.0,
    {
      {0, 0, 6, sym16},
      {INT_MAX, 0, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {0, 0, 0, NULL},
      {0, 0, 6, sym16},
      {INT_MAX, INT_MAX, 0, NULL},
      {0, 0, 6, sym16},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {0, 0, 6, sym16},
      {0, 0, 6, sym16},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    MANDEL_BTRACE,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    octo_calc,
	/*octo_peri, */ NULL,
    socto_calc,
	/*socto_peri, */ NULL,
#endif
    NULL,
    {"Octal", "Octal"},
    "octal",
	/*{1.25, -1.25, 1.25, -1.25}, */
    {0.0, 0.0, 2.5, 2.5},
    0, 1, 0.0, 0.0,
    {
      {0, 0, 6, sym16},
      {INT_MAX, 0, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {0, INT_MAX, 0, NULL},
      {0, 0, 0, NULL},
      {0, 0, 6, sym16},
      {INT_MAX, INT_MAX, 0, NULL},
      {0, 0, 6, sym16},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {0, 0, 6, sym16},
      {0, 0, 6, sym16},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    MANDEL_BTRACE | STARTZERO,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    newton_calc,
    NULL,
    NULL,
    NULL,
#endif
    NULL,
    {"Newton", "Newton julia?"},
    "newton",
	/*{1.25, -1.25, 1.25, -1.25}, */
    {0.0, 0.0, 2.5, 2.5},
    0, 1, 1.0199502202048319698, 0.0,
    {
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, 0, 2, sym6},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    STARTZERO,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    barnsley1_calc,
    NULL,
    sbarnsley1_calc,
    NULL,
#endif
    barnsley1_julia,
    {"Barnsley1 Mandelbrot", "Barnsley1"},
    "barnsley",
	/*{1.25, -1.25, 1.25, -1.25}, */
    {0.0, 0.0, 2.5, 2.5},
    0, 0, -0.6, 1.1,
    {
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    STARTZERO | MANDEL_BTRACE,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    phoenix_calc,
    phoenix_peri,
    sphoenix_calc,
    sphoenix_peri,
#endif
    NULL,
    {"MandPhoenix", "Phoenix"},
    "phoenix",
	/*{1.25, -1.25, 1.25, -1.25}, */
    {0.0, 0.0, 2.5, 2.5},
    1, 0, 0.56667000000000001, -0.5,
    {
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    MANDEL_BTRACE,
  },
  {
    FORMULAMAGIC,
#ifndef SLOWFUNCPTR
    magnet_calc,
    magnet_peri,
    smagnet_calc,
    smagnet_peri,
#endif
    magnet_julia,
    {"Magnet", "Magnet"},
    "magnet",
	/*{3, 0, 2.2, -2.2}, */
    {1.5, 0.0, 3.0, 4.4},
    1, 1, 0.0, 0.0,
    {
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    {
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, 0, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
      {INT_MAX, INT_MAX, 0, NULL},
    },
    STARTZERO,
  },
};
#ifdef SLOWFUNCPTR
unsigned int calculateswitch(register  number_t x1,register number_t y1,register number_t x2,register number_t y2, int periodicity)
{
  if(periodicity&&cfractalc.periodicity) 
    if(cfractalc.coloringmode==9) 
      switch(cfractalc.currentformula-formulas)
      {
	case 0:return(smand_peri(x1,y1,x2,y2));break;
	case 1:return(smand3_peri(x1,y1,x2,y2));break;
	case 2:return(smand4_peri(x1,y1,x2,y2));break;
	case 3:return(smand5_peri(x1,y1,x2,y2));break;
	case 4:return(smand6_peri(x1,y1,x2,y2));break;
	case 5:return(octo_calc(x1,y1,x2,y2));break;
	case 6:return(newton_calc(x1,y1,x2,y2));break;
	case 7:return(sbarnsley1_calc(x1,y1,x2,y2));break;
	case 8:return(sphoenix_peri(x1,y1,x2,y2));break;
	case 9:return(smagnet_peri(x1,y1,x2,y2));break;
      }
    else
      switch(cfractalc.currentformula-formulas)
      {
	case 0:return(mand_peri(x1,y1,x2,y2));break;
	case 1:return(mand3_peri(x1,y1,x2,y2));break;
	case 2:return(mand4_peri(x1,y1,x2,y2));break;
	case 3:return(mand5_peri(x1,y1,x2,y2));break;
	case 4:return(mand6_peri(x1,y1,x2,y2));break;
	case 5:return(octo_calc(x1,y1,x2,y2));break;
	case 6:return(newton_calc(x1,y1,x2,y2));break;
	case 7:return(barnsley1_calc(x1,y1,x2,y2));break;
	case 8:return(phoenix_peri(x1,y1,x2,y2));break;
	case 9:return(magnet_peri(x1,y1,x2,y2));break;
      }
  else
    if(cfractalc.coloringmode==9) 
      switch(cfractalc.currentformula-formulas)
      {
	case 0:return(smand_calc(x1,y1,x2,y2));break;
	case 1:return(smand3_calc(x1,y1,x2,y2));break;
	case 2:return(smand4_calc(x1,y1,x2,y2));break;
	case 3:return(smand5_calc(x1,y1,x2,y2));break;
	case 4:return(smand6_calc(x1,y1,x2,y2));break;
	case 5:return(octo_calc(x1,y1,x2,y2));break;
	case 6:return(newton_calc(x1,y1,x2,y2));break;
	case 7:return(sbarnsley1_calc(x1,y1,x2,y2));break;
	case 8:return(sphoenix_calc(x1,y1,x2,y2));break;
	case 9:return(smagnet_calc(x1,y1,x2,y2));break;
      }
    else
      switch(cfractalc.currentformula-formulas)
      {
	case 0:return(mand_calc(x1,y1,x2,y2));break;
	case 1:return(mand3_calc(x1,y1,x2,y2));break;
	case 2:return(mand4_calc(x1,y1,x2,y2));break;
	case 3:return(mand5_calc(x1,y1,x2,y2));break;
	case 4:return(mand6_calc(x1,y1,x2,y2));break;
	case 5:return(octo_calc(x1,y1,x2,y2));break;
	case 6:return(newton_calc(x1,y1,x2,y2));break;
	case 7:return(barnsley1_calc(x1,y1,x2,y2));break;
	case 8:return(phoenix_calc(x1,y1,x2,y2));break;
	case 9:return(magnet_calc(x1,y1,x2,y2));break;
      }
  return 0;
}
#endif

struct formula *currentformula = formulas;
CONST int nformulas = sizeof (formulas) / sizeof (struct formula);

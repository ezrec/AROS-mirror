
/* Hello reader!

 * Are you sure you want read this? Its very cryptic and strange code. YOU
 * HAVE BEEN WARNED! Its purpose is to genereate as fast as possible
 * calculation loops for various formulas/algorithms. It uses lots of
 * coprocesor magic. It is included from formulas.c
 */
#ifndef VARIABLES		/*supply defaultd values */
#define VARIABLES
#endif
#ifndef PRETEST
#define PRETEST 0
#endif
#ifndef INIT
#define INIT
#endif
#ifndef POSTCALC
#define POSTCALC
#endif
#ifndef PRESMOOTH
#define PRESMOOTH zre=rp+ip
#endif
#ifndef UFORMULA
#define UFORMULA FORMULA
#endif
#ifndef UEND
#define UEND
#endif
#ifndef SAVE
#define SAVE
#endif
#ifndef SAVEVARIABLES
#define SAVEVARIABLES
#endif
#ifndef RESTORE
#define RESTORE
#endif
#ifndef RANGE
#define RANGE 2
#endif
#ifndef __GNUC__
#undef USEHACKS
#endif
#ifndef __i386__
#undef USEHACKS
#endif
#ifdef USEHACKS
#ifdef RPIP
#define I386HACK1 __asm__ ("#HACK1" : \
		: \
		 "m" (szre), \
		 "m" (szim) \
		);
#define I386HACK __asm__ ("#HACK" : \
		: \
		 "f" (rp), \
		 "f" (ip), \
		 "f" (pre), \
		 "f" (pim), \
		 "f" (zre), \
		 "f" (zim) \
		);
#else
#define I386HACK __asm__ ("#HACK" : \
		: \
		 "f" (pre), \
		 "f" (pim), \
		 "f" (zre), \
		 "f" (zim) \
		);
#endif
#else
#define I386HACK
#define I386HACK1
#endif

#ifdef SMOOTHMODE
#ifdef CUSTOMSAVEZMAG
#define SAVEZMAG CUSTOMSAVEZMAG;
#else
#define SAVEZMAG szmag=rp+ip;
#endif
#else
#define SAVEZMAG
#endif

#ifdef UNCOMPRESS
/*uncompressed version of loop */
#ifdef SMOOTHMODE
// static int FUNCTYPE
// SCALC (register number_t zre, register number_t zim, register number_t pre, register number_t pim) CONSTF REGISTERS (3);
static int FUNCTYPE
SCALC (register number_t zre, register number_t zim, register number_t pre, register number_t pim)
#else
// static int FUNCTYPE
// CALC (register number_t zre, register number_t zim, register number_t pre, register number_t pim) CONSTF REGISTERS (3);
static int
CALC (register number_t zre, register number_t zim, register number_t pre, register number_t pim)
#endif
{
  register int iter = cfractalc.maxiter;
  number_t szre = 0, szim = 0;
#ifdef RPIP
  register number_t rp = 0, ip;
#endif
#ifdef SMOOTHMODE
  number_t szmag = 0;
#endif
  SAVEVARIABLES
    VARIABLES;
  INIT;
  if (PRETEST)
    iter = 0;
  else
    {
#ifdef RPIP
      rp = zre * zre;
      ip = zim * zim;
#endif
      if (iter < 16)
	{
	  I386HACK1;
	  I386HACK;
	  while (iter && BTEST)
	    {			/*try first 8 iterations */
	      SAVEZMAG;
	      FORMULA;
	      iter--;
	    }
	}
      else
	{
	  iter = 8 + (cfractalc.maxiter & 7);
	  I386HACK1;
	  I386HACK;
	  while (iter && BTEST)
	    {			/*try first 8 iterations */
	      SAVEZMAG;
	      FORMULA;
	      iter--;
	    }
	  if (BTEST)
	    {
	      iter = (cfractalc.maxiter - 8) & (~7);
	      iter >>= 3;
	      do
		{
		  /*hmm..we are probably in some deep area. */
		  szre = zre;	/*save current possition */
		  szim = zim;
		  SAVE;
		  I386HACK1;
		  I386HACK;	/*do next 8 iteration w/o out of bounds checking */
		  UFORMULA;
		  UFORMULA;
		  UFORMULA;
		  UFORMULA;
		  UFORMULA;
		  UFORMULA;
		  UFORMULA;
		  UFORMULA;
		  UEND;
		  iter--;
		}
	      while (iter && BTEST);
	      if (!(BTEST))
		{		/*we got out of bounds */
		  iter <<= 3;
		  iter += 8;	/*restore saved possition */
		  RESTORE;
		  zre = szre;
		  zim = szim;
#ifdef RPIP
		  rp = zre * zre;
		  ip = zim * zim;
#endif
		  I386HACK1;
		  I386HACK;
		  do
		    {
		      SAVEZMAG
			FORMULA;
		      iter--;
		    }
		  while ((iter) && BTEST);
		}
	    }
	  else
	    iter += cfractalc.maxiter - 8 - (cfractalc.maxiter & 7);
	}
    }
#ifdef SMOOTHMODE
  if (iter)
    SMOOTHOUTPUT ();
  POSTCALC;
  iter = cfractalc.maxiter - iter;
  INOUTPUT ();
#else
  POSTCALC;
  iter = cfractalc.maxiter - iter;
  OUTPUT ();
#endif
}
#else
#ifdef SMOOTHMODE
// static int FUNCTYPE
// SCALC (register number_t zre, register number_t zim, register number_t pre, register number_t pim) CONSTF REGISTERS (3);
static int FUNCTYPE
SCALC (register number_t zre, register number_t zim, register number_t pre, register number_t pim)
#else
// static int FUNCTYPE
// CALC (register number_t zre, register number_t zim, register number_t pre, register number_t pim) CONSTF REGISTERS (3);
static int FUNCTYPE
CALC (register number_t zre, register number_t zim, register number_t pre, register number_t pim)
#endif
{
  register int iter = cfractalc.maxiter /*& (~(int) 3) */ ;
#ifdef RPIP
  register number_t rp, ip;
#endif
#ifdef SMOOTHMODE
  number_t szmag = 0;
#endif
  VARIABLES;
  INIT;
  if (PRETEST)
    iter = 0;
  else
    {
#ifdef RPIP
      rp = zre * zre;
      ip = zim * zim;
#endif
      while ((iter) && BTEST)
	{
	  I386HACK;
	  SAVEZMAG
	    FORMULA;
	  iter--;

	}
    }
#ifdef SMOOTHMODE
  if (iter)
    SMOOTHOUTPUT ();
  POSTCALC;
  iter = cfractalc.maxiter - iter;
  INOUTPUT ();
#else
  POSTCALC;
  iter = cfractalc.maxiter - iter;
  OUTPUT ();
#endif
}
#endif


/*F. : Periodicity checking rountines.          (16-02-97)
   All comments preceded by F. are mine (Fabrice Premel premelfa@etu.utc.fr).
   Tried to make coed as efficient as possible.
   Next to do is convert lim in a variable that would be updated sometimes
   I'll try to make here a short explanation on periodicity checking :
   first, we'll define 2 variables : whentosave and whenincsave, which are, respectively,
   a measure of when we have to update saved values to be checked, and when to increase
   interval between 2 updates, as if they're too close, we'll miss large periods.
   We save Z at the beginning, and then we compare each new iteration with this Z, and if naerly
   equal, we declare the suite to be periodic.
   When ( iter mod whentosave ) == 0, we store a new value, and we repeat.

   UNCOMPRESSed form is just an extension, with careful that if we only check whentosave
   all 8 iterations, number of iterations must be well set at the begining.This is
   done by adding a (iter&7) in the while statement preceeding then uncompressed
   calculation. */

/*F. : This is from then lim factor that depends all periodicity check spped : the bigger it is, the faster we
   can detect periodicity, but the bigger it is, the more we can introduce errors.
   I suggest a value of (maxx-minx)/(double)getmaxx for a classic Mandelbrot Set,
   and maybe a lesser value for an extra power Mandelbrot.
   But this should be calculated outter from here (ie each frame, for example), to avoid
   new calculs */
#ifdef PERI
#define PCHECK (myabs(r1 - zre)<cfractalc.periodicity_limit && myabs(s1 - zim) < cfractalc.periodicity_limit)

#ifndef UNCOMPRESS

#ifdef SMOOTHMODE
// static int FUNCTYPE
// SPERI (register number_t zre, register number_t zim, register number_t pre, register number_t pim) CONSTF REGISTERS (3);
static int FUNCTYPE
SPERI (register number_t zre, register number_t zim, register number_t pre, register number_t pim)
#else
// static int FUNCTYPE
// PERI (register number_t zre, register number_t zim, register number_t pre, register number_t pim) CONSTF REGISTERS (3);
static int FUNCTYPE
PERI (register number_t zre, register number_t zim, register number_t pre, register number_t pim)
#endif
{
  register int iter = cfractalc.maxiter /*& (~(int) 3) */ , iter1 = 8;
  register number_t r1, s1;
  int whensavenew, whenincsave;
#ifdef RPIP
  register number_t rp, ip;
#endif
#ifdef SMOOTHMODE
  number_t szmag = 0;
#endif
  VARIABLES;
  INIT;
  if (PRETEST)
    iter = 0;
  else
    {
#ifdef RPIP
      rp = zre * zre;
      ip = zim * zim;
#endif
      I386HACK;
      if (iter < iter1)
	iter1 = iter, iter = 8;
      while (iter1 && BTEST)
	{			/*H. : do first few iterations w/o checking */
	  SAVEZMAG;
	  FORMULA;
	  iter1--;
	}
      if (iter1)
	{
	  if (iter >= 8)
	    iter -= 8 - iter1;
	  goto end;
	}
      if (iter <= 8)
	{
	  iter = iter1;
	}
      else
	{
	  iter -= 8;
	  r1 = zre;
	  s1 = zim;
	  whensavenew = 3;	/*You should adapt theese values */
	  /*F. : We should always define whensavenew as 2^N-1, so we could use a AND instead of % */

	  whenincsave = 10;
	  /*F. : problem is that after deep zooming, peiodicity is never detected early, cause is is
	     quite slow before going in a periodic loop.
	     So, we should start checking periodicity only after some times */
	  while ((iter) && BTEST)
	    {
	      SAVEZMAG;
	      FORMULA;
	      if ((iter & whensavenew) == 0)
		{		/*F. : changed % to & */
		  r1 = zre;
		  s1 = zim;
		  whenincsave--;
		  if (!whenincsave)
		    {
		      whensavenew = ((whensavenew + 1) << 1) - 1;	/*F. : Changed to define a new AND mask */
		      whenincsave = 10;
		    }
		}
	      else
		{
		  if (PCHECK)
		    {
		      PERIINOUTPUT ();
		    }
		}
	      iter--;

	    }
	}
    }
end:
#ifdef SMOOTHMODE
  if (iter)
    SMOOTHOUTPUT ();
  POSTCALC;
  iter = cfractalc.maxiter - iter;
  INOUTPUT ();
#else
  POSTCALC;
  iter = cfractalc.maxiter - iter;
  OUTPUT ();
#endif
}

#else

/*F. : UNCOMPRESSed version. Note that whensavenew+1 should be a multiple of 8, else periodicity won't be able
   to detect anything. */
/*F. : this macros definitions are really strange, but after a while, it's good */

#ifdef SMOOTHMODE
// static int FUNCTYPE
// SPERI (register number_t zre, register number_t zim, register number_t pre, register number_t pim) CONSTF REGISTERS (3);
static int FUNCTYPE
SPERI (register number_t zre, register number_t zim, register number_t pre, register number_t pim)
#else
// static int FUNCTYPE
// PERI (register number_t zre, register number_t zim, register number_t pre, register number_t pim) CONSTF REGISTERS (3);
static int FUNCTYPE
PERI (register number_t zre, register number_t zim, register number_t pre, register number_t pim)
#endif
{
  register int iter = cfractalc.maxiter /*& (~(int) 3) */ ;
  register number_t r1 = zre, s1 = zim;
  number_t szre = 0, szim = 0;	/*F. : Didn't declared register, cause they are few used */
  int whensavenew, whenincsave;
#ifdef RPIP
  register number_t rp = 0, ip;
#endif
#ifdef SMOOTHMODE
  number_t szmag = 0;
#endif
  SAVEVARIABLES
    VARIABLES;
  INIT;
  if (PRETEST)
    iter = 0;
  else
    {
      if (cfractalc.maxiter <= 16)
	{
	  I386HACK1;
	  /*I386HACK; */
#ifdef RPIP
	  rp = zre * zre;
	  ip = zim * zim;
#endif
	  while (iter && BTEST)
	    {			/*F. : Added iter&7 to be sure we'll be on a 8 multiple */
	      SAVEZMAG
		FORMULA;
	      iter--;
	    }
	}
      else
	{
	  whensavenew = 7;	/*You should adapt theese values */
	  /*F. : We should always define whensavenew as 2^N-1, so we could use a AND instead of % */

	  whenincsave = 10;
#ifdef RPIP
	  rp = zre * zre;
	  ip = zim * zim;
#endif
	  /*F. : problem is that after deep zooming, peiodicity is never detected early, cause is is
	     quite slow before going in a periodic loop.
	     So, we should start checking periodicity only after some times */
	  I386HACK1;
	  /*I386HACK; */
	  iter = 8 + (cfractalc.maxiter & 7);
	  while (iter && BTEST)
	    {			/*F. : Added iter&7 to be sure we'll be on a 8 multiple */
	      SAVEZMAG
		FORMULA;
	      iter--;
	    }
	  if (BTEST)
	    {			/*F. : BTEST is calculed two times here, isn't it ? */
	      /*H. : No gcc is clever and adds test to the end :) */
	      iter = (cfractalc.maxiter - 8) & (~7);
	      do
		{
		  szre = zre, szim = zim;
		  SAVE;
		  SAVEZMAG
		  /*I386HACK; */
		    I386HACK1;
		  FORMULA;	/*F. : Calculate one time */
		  if (PCHECK)
		    goto periodicity;
		  FORMULA;
		  if (PCHECK)
		    goto periodicity;
		  FORMULA;
		  if (PCHECK)
		    goto periodicity;
		  FORMULA;
		  if (PCHECK)
		    goto periodicity;
		  FORMULA;
		  if (PCHECK)
		    goto periodicity;
		  FORMULA;
		  if (PCHECK)
		    goto periodicity;
		  FORMULA;
		  if (PCHECK)
		    goto periodicity;
		  FORMULA;
		  if (PCHECK)
		    goto periodicity;
		  iter -= 8;
		  /*F. : We only test this now, as it can't be true before */
		  if ((iter & whensavenew) == 0)
		    {		/*F. : changed % to & */
		      r1 = zre, s1 = zim;	/*F. : Save new values */
		      whenincsave--;
		      if (!whenincsave)
			{
			  whensavenew = ((whensavenew + 1) << 1) - 1;	/*F. : Changed to define a new AND mask */
			  whenincsave = 10;	/*F. : Start back */
			}
		    }
		}
	      while (iter && BTEST);
	      if (!BTEST)
		{		/*we got out of bounds */
		  iter += 8;	/*restore saved possition */
		  RESTORE;
		  zre = szre;
		  zim = szim;
#ifdef RPIP
		  rp = zre * zre;
		  ip = zim * zim;
#endif
		  I386HACK1;
		  /*I386HACK; */
		  do
		    {
		      SAVEZMAG
			FORMULA;
		      iter--;
		    }
		  while ((iter) && BTEST);
		}
	    }
	  else
	    iter += cfractalc.maxiter - 8 - (cfractalc.maxiter & 7);
	}
    }
#ifdef SMOOTHMODE
  if (iter)
    SMOOTHOUTPUT ();
  POSTCALC;
  iter = cfractalc.maxiter - iter;
  INOUTPUT ();
#else
  POSTCALC;
  iter = cfractalc.maxiter - iter;
  OUTPUT ();
#endif
periodicity:
  PERIINOUTPUT ();
}
/*else uncompress */
#endif

/*endif PERI */
#undef PCHECK
#endif

#ifndef SMOOTHMODE
#ifdef JULIA
static void
JULIA (struct image *image, register number_t pre, register number_t pim, int maxiter)
{
  int i, i1, i2, j, x, y, itmp, itmp2;
  int iter;
  number_t rp = 0, ip = 0;
  register number_t zre, zim, im, xdelta, ydelta, range, rangep;
  number_t xstep, ystep;
  unsigned char *queue[QMAX];
  unsigned char **qptr;
  unsigned char *addr, **addr1 = image->currlines;
#ifdef STATISTICS
  int guessed = 0, unguessed = 0, iters = 0;
#endif
  VARIABLES;
  range = RANGE;
  rangep = range * range;

  xdelta = image->width / (RMAX - RMIN);
  ydelta = image->height / (IMAX - IMIN);
  xstep = (RMAX - RMIN) / image->width;
  ystep = (IMAX - IMIN) / image->height;
  init_julia (image, rangep, range, xdelta, ystep);
  for (i2 = 0; i2 < 2; i2++)
    for (i1 = 0; i1 < image->height; i1++)
      {
	if (i1 % 2)
	  i = image->height / 2 - i1 / 2;
	else
	  i = image->height / 2 + i1 / 2 + 1;
	if (i >= image->height)
	  continue;
	im = IMIN + (i + 0.5) * ystep;
	for (j = (i + i2) & 1; j < image->width; j += 2)
	  {
	    STAT (total2++);
	    addr = addr1[i] + j;
	    if (*addr != NOT_CALCULATED)
	      continue;
	    x = j;
	    y = i;
	    if (y > 0 && y < image->height - 1 && *(addr + 1) &&
		x > 0 && x < image->width - 1)
	      {
		if ((iter = *(addr + 1)) != NOT_CALCULATED && iter == *(addr - 1) && iter ==
		    addr1[y - 1][x] && iter == addr1[y + 1][x])
		  {
		    *addr = *(addr + 1);
		    continue;
		  }
	      }
	    zim = im;
	    zre = RMIN + (j + 0.5) * xstep;
	    iter = 0;
	    qptr = queue;
	    ip = (zim * zim);
	    rp = (zre * zre);
	    INIT;
	    while (1)
	      {
		if (*addr != NOT_CALCULATED
#ifdef SAG
		    && (*addr == INPROCESS || (*addr != 1 &&
				  (itmp2 = *(addr + 1)) != NOT_CALCULATED &&
		((itmp2 != (itmp = *(addr - 1)) && itmp != NOT_CALCULATED) ||
		 (itmp2 != (itmp = *((addr1[y + 1]) + x)) && itmp != NOT_CALCULATED) ||
		 (itmp2 != (itmp = *((addr1[y - 1]) + x)) && itmp != NOT_CALCULATED))))
#endif
		  )
		  {
		    if (*addr == INPROCESS || *addr == INSET)
		      {
			*qptr = addr;
			qptr++;
			STAT (guessed++);
			goto inset;
		      }
		    STAT (guessed++);
		    iter = *addr;
		    goto outset;
		  }
#ifdef STATISTICS
		if (*addr != NOT_CALCULATED)
		  unguessed++;
#endif
		if (*addr != INPROCESS)
		  {
		    *qptr = addr;
		    qptr++;
		    *addr = INPROCESS;
		    if (qptr >= queue + QMAX)
		      goto inset;
		  }
		STAT (iters++);
		FORMULA;
		ip = (zim * zim);
		rp = (zre * zre);
		if (rp + ip > RANGE || !(BTEST))
		  goto outset;
		x = (zre - RMIN) * xdelta;
		y = (zim - IMIN) * ydelta;
		addr = addr1[y] + x;
		if ((itmp = *(addr + 1)) != NOT_CALCULATED && itmp == *(addr - 1) && itmp ==
		    addr1[y - 1][x] && itmp == addr1[y + 1][x])
		  {
		    *addr = *(addr + 1);
		  }
	      }
	  inset:
	    while (qptr > queue)
	      {
		qptr--;
		**qptr = INSET;
	      }
	    continue;
	  outset:
	    y = image->palette->size;
	    while (qptr > queue)
	      {
		qptr--;
		iter++;
		if (iter >= y)
		  iter = 1;
		**qptr = iter;
	      }
	  }
      }
#ifdef STATISTICS
  printf ("guessed %i, unguessed %i, iterations %i\n", guessed, unguessed, iters);
  guessed2 += guessed;
  unguessed2 += unguessed;
  iters2 += iters;
#endif
}
#endif
#endif

#undef PCHECK
#undef I386HACK
#undef I386HACK1
#undef SAVEZMAG
#ifndef SMOOTHMODE
#ifdef SMOOTH
#define SMOOTHMODE
#include "docalc.c"
#endif
#endif

/*cleanup for next formula */
#undef PRESMOOTH
#undef SMOOTH
#undef SMOOTHMODE
#undef RANGE
#undef JULIA
#undef PERI
#undef SPERI
#undef INIT
#undef VARIABLES
#undef PRETEST
#undef BTEST
#undef FORMULA
#undef CALC
#undef SCALC
#undef RPIP
#undef POSTCALC
#undef UNCOMPRESS
#undef SAVE
#undef SAVEVARIABLES
#undef RESTORE
#undef USEHACKS
#undef UFORMULA
#undef UEND

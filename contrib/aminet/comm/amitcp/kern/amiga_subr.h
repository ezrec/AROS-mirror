/*
 * $Id$
 * 
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 1.13  1994/01/23  22:06:26  jraja
 * Fixed bcopy argument cast.
 *
 * Revision 1.12  1994/01/18  02:28:27  jraja
 * Changed some arguments to const.
 *
 * Revision 1.11  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 * Revision 1.10  1993/05/17  01:02:04  ppessi
 * Changed RCS version
 *
 * Revision 1.9  1993/04/24  23:19:22  jraja
 * Changed define NO_NOALIGN to USE_ALIGNED_COPIES.
 *
 * Revision 1.8  93/04/13  22:30:20  22:30:20  jraja (Jarno Tapio Rajahalme)
 * Changed NOALIGNED to NOALIGN.
 * 
 * Revision 1.7  93/03/22  17:04:11  17:04:11  jraja (Jarno Tapio Rajahalme)
 * Added definitions for aligned_bzero() aligned_bzero_const(),
 * aligned_bcopy() aligned_bcopy_const(). These are optimized versions of
 * bzero() and bcopy(). _const ones are to be used with constant size argument.
 * 
 * Revision 1.6  93/03/19  14:14:44  14:14:44  too (Tomi Ollila)
 * Code changes at night 17-18 March 1993
 * 
 * Revision 1.5  93/03/04  09:43:32  09:43:32  jraja (Jarno Tapio Rajahalme)
 * Fixed includes.
 * 
 * Revision 1.4  93/02/27  12:44:25  12:44:25  jraja (Jarno Tapio Rajahalme)
 * added strncpy() for gcc.
 * 
 * Revision 1.3  93/02/24  12:52:22  12:52:22  jraja (Jarno Tapio Rajahalme)
 * Changed uxkern to kern.
 * 
 * Revision 1.2  93/02/04  18:57:47  18:57:47  jraja (Jarno Tapio Rajahalme)
 * Added SASC definitions and ovbcopy().
 * 
 * Revision 1.1  93/01/06  19:05:36  19:05:36  jraja (Jarno Tapio Rajahalme)
 * Initial revision
 * 
 */

#ifndef KERN_AMIGA_SUBR_H
#define KERN_AMIGA_SUBR_H

#ifndef AMIGA_SUBR_H
#define AMIGA_SUBR_H


#if __SASC

#define imin(a,b) min(a,b)

/* #define MIN(a,b) min(a,b) */   /* Conflicts with ixemul definition */
#define lmin(a,b) min(a,b)
#define ulmin(a,b) min(a,b)

#define imax(a,b) max(a,b)
#define MAX(a,b) max(a,b)
#define lmax(a,b) max(a,b)
#define ulmax(a,b) max(a,b)

/*
 * bcopy(), bcmp() and bzero() are defined in string.h
 *
 * NOTE: bcopy is infact ovbcopy(). Optimize this when all other works!
 */

#undef bcopy 
#define bcopy(a,b,c) CopyMem((APTR)(a),b,c)
#define ovbcopy(a,b,c) memmove(b,a,c)

#else

#ifndef _CDEFS_H
#include <sys/cdefs.h>
#endif

#include <string.h>

static inline int 
imin(int a, int b)
{
  return (a < b ? a : b);
}

#define MIN(a,b) imin(a,b)

static inline int 
imax(int a, int b)
{
  return (a > b ? a : b);
}

static inline unsigned int
min(unsigned int a, unsigned int b)
{
  return (a < b ? a : b);
}

static inline unsigned int
max(unsigned int a, unsigned int b)
{
  return (a > b ? a : b);
}

static inline long
lmin(long a, long b)
{
  return (a < b ? a : b);
}

static inline long
lmax(long a, long b)
{
  return (a > b ? a : b);
}

static inline unsigned long
ulmin(unsigned long a, unsigned long b)
{
  return (a < b ? a : b);
}

static inline unsigned long
ulmax(unsigned long a, unsigned long b)
{
  return (a > b ? a : b);
}

static inline void
ovbcopy(const void *v1, void *v2, register unsigned len)
{
  const register u_char *s1 = v1;
  register u_char *s2 = v2;
  
  if (s1 < s2) {
    /*
     * copy possibly destroying s1 (if overlap), copy backwards
     */
    s1 += len;
    s2 += len;
    while (len--)
      *(--s2) = *(--s1); 
  }
  else
    while (len--)
      *s2++ = *s1++;
}

static inline char *
strcpy(register char *s1, register const char *s2)
{
  register char *s = s1;
  while( (*s++ = *s2++) )
    ;
  return (s1);
}
#endif /* __SASC */

/* 
 * These are for both environments
 */

#ifndef USE_ALIGNED_COPIES
#define aligned_bcopy_const bcopy
#define aligned_bcopy bcopy
#define aligned_bzero_const bzero
#define aligned_bzero bzero
#else
/*
 * clear an aligned memory area of constant length to zero
 */ 
static inline void
aligned_bzero_const(void *buf, long size) 
{
  short lcount;
  long *lbuf = (long *)buf;
  short *sbuf;

  lcount = (size >> 2);
  if (lcount--) {
    /*
     * unroll the loop if short enough
     */
    if (lcount < 6) {
      *lbuf++ = 0;
      if (--lcount >= 0)
	*lbuf++ = 0;
      if (--lcount >= 0)
	*lbuf++ = 0;
      if (--lcount >= 0)
	*lbuf++ = 0;
      if (--lcount >= 0)
	*lbuf++ = 0;
      if (--lcount >= 0)
	*lbuf++ = 0;
    }
    else {
      do {
	*lbuf++ = 0;
      } while (--lcount >= 0);
    }
  }

  sbuf = (short *)lbuf;
  if (size & 0x2)
    *sbuf++ = 0;

  if (size & 0x1)
    *(char *)sbuf = 0;
}

static inline void
aligned_bzero(void *buf, long size) 
{
  short lcount;
  long *lbuf = (long *)buf;
  short *sbuf;

  lcount = (size >> 2);
  if (lcount--) {
    do {
      *lbuf++ = 0;
    } while (--lcount >= 0);
  }

  sbuf = (short *)lbuf;
  if (size & 0x2)
    *sbuf++ = 0;

  if (size & 0x1)
    *(char *)sbuf = 0;
}

static inline void
aligned_bcopy_const(const void *src, void *dst, long size) 
{
  short lcount;
  long *ldst = (long *)dst;
  short *sdst;
  long *lsrc = (long *)src;
  short *ssrc;

  lcount = (size >> 2);
  if (lcount--) {
    /*
     * unroll the loop if short enough
     */
    if (lcount < 6) {
      *ldst++ = *lsrc++;
      if (--lcount >= 0)
	*ldst++ = *lsrc++;
      if (--lcount >= 0)
	*ldst++ = *lsrc++;
      if (--lcount >= 0)
	*ldst++ = *lsrc++;
      if (--lcount >= 0)
	*ldst++ = *lsrc++;
      if (--lcount >= 0)
	*ldst++ = *lsrc++;
    }
    else {
      do {
	*ldst++ = *lsrc++;
      } while (--lcount >= 0);
    }
  }

  sdst = (short *)ldst;
  ssrc = (short *)lsrc;
  if (size & 0x2)
    *sdst++ = *ssrc++;

  if (size & 0x1)
    *(char *)sdst = *(char *)ssrc;
}

static inline void
aligned_bcopy(const void *src, void *dst, long size) 
{
  short lcount;
  long *ldst = (long *)dst;
  short *sdst;
  long *lsrc = (long *)src;
  short *ssrc;

  lcount = (size >> 2);
  if (lcount--) {
    do {
      *ldst++ = *lsrc++;
    } while (--lcount >= 0);
  }

  sdst = (short *)ldst;
  ssrc = (short *)lsrc;
  if (size & 0x2)
    *sdst++ = *ssrc++;

  if (size & 0x1)
    *(char *)sdst = *(char *)ssrc;
}
#endif /* USE_ALIGNED_COPIES */
#endif /* AMIGA_SUBR_H */

#endif /* KERN_AMIGA_SUBR_H */

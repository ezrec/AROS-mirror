/*
 * Copyright (C) 1993,1994 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                         Helsinki University of Technology, Finland.
 *                         All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this file; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */

#ifndef SYS_PARAM_H
#define SYS_PARAM_H

/*
 * bsdsocket.library is not BSD4.3 compatible
 */
#define	BSD	199402	/* Feb, 1994 system version (year & month) */
#define BSD4_3	1
#define BSD4_4	1

#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif

#define NOFILE		FD_SETSIZE /* perhaps we shouldn't use FD_SETSIZE */

#if defined(__PPC__) || defined(__m68000)
#define	BIG_ENDIAN	4321	/* MSB first */
#define	BYTE_ORDER	BIG_ENDIAN
#elif defined(__i386__)
#define	LITTLE_ENDIAN	1234	/* LSB first */
#define	BYTE_ORDER	LITTLE_ENDIAN
#else
#error Unknown CPU type
#endif

#define MAXHOSTNAMELEN  64	/* max length of hostname */
#define MAXLOGNAME      32	/* max length of login name */

/*
 * Endianness conversion macros
 */
#define FlipWord(A) \
   ({ \
      UWORD _FlipWord_A = (A); \
      _FlipWord_A = (_FlipWord_A << 8) | (_FlipWord_A >> 8); \
   })

#define FlipLong(A) \
   ({ \
      ULONG _FlipLong_A = (A); \
      _FlipLong_A = \
         (FlipWord(_FlipLong_A) << 16) | FlipWord(_FlipLong_A >> 16); \
   })

#if BYTE_ORDER == BIG_ENDIAN

#define BEWord(A) \
   (A)

#define BELong(A) \
   (A)

#define LEWord(A) \
   FlipWord(A)

#define LELong(A) \
   FlipLong(A)

#else

#define BEWord(A) \
   FlipWord(A)

#define BELong(A) \
   FlipLong(A)

#define LEWord(A) \
   (A)

#define LELong(A) \
   (A)

#endif

#define MakeBEWord(A) \
   BEWord(A)

#define MakeBELong(A) \
   BELong(A)

#define MakeLEWord(A) \
   LEWord(A)

#define MakeLELong(A) \
   LELong(A)

/*
 * Macros for network/external number representation conversion.
 */
#ifndef ntohl
#define	ntohl(x)	BELong(x)
#define	ntohs(x)	BEWord(x)
#define	htonl(x)	MakeBELong(x)
#define	htons(x)	MakeBEWord(x)

#define	NTOHL(x)	((x) = ntohl(x))
#define	NTOHS(x)	((x) = ntohs(x))
#define	HTONL(x)	((x) = htonl(x))
#define	HTONS(x)	((x) = htons(x))
#endif

/* Limits */
/*#if __SASC*/
#if 1 /* NC */
#ifndef _LIMITS_H
#include <limits.h>
#endif
#elif __GNUC__
#ifndef _MACHINE_LIMITS_H
#include <machine/limits.h>
#endif
#undef LONGBITS			/* exec/types wants to use this symbol */
#undef BITSPERBYTE		/* dos/dos.h defines this */
#undef MAXINT			/* dos/dos.h defines this */
#undef MININT			/* dos/dos.h defines this */
#endif

/* Bit map related macros. */
#define	setbit(a,i)	((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define	clrbit(a,i)	((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define	isset(a,i)	((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define	isclr(a,i)	(((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

/* Macros for counting and rounding. */
#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#define powerof2(x)	((((x)-1)&(x))==0)

#ifndef KERNEL			/* Kernel has these in kern/amiga_subr.h */
#if __SASC			/* SASC has built-in versions */
#ifndef _STRING_H
#ifndef USE_BUILTIN_MATH
#define USE_BUILTIN_MATH
#endif
#include <string.h>
#endif

#else

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif

#endif
#endif /* !KERNEL */

/* More types and definitions used throughout the kernel. */
#ifdef KERNEL

#ifndef SYS_CDEFS_H
#include <sys/cdefs.h>
#endif

#ifndef SYS_ERRNO_H
#include <sys/errno.h>
#endif

#ifndef SYS_TIME_H
#include <sys/time.h>
#endif

#ifndef SYS_UIO_H
#include <sys/uio.h>
#endif

#endif /* KERNEL */
#endif /* !SYS_PARAM_H */



#ifndef lint
static char *RCSid = "$Id$";
#endif

/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1992-1994  Anders Christensen <anders@pvv.unit.no>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "rexx.h"
#include <ctype.h>
/* Do _not_ use stddef, anders! _not_ stddef! Remember that!!!
   (it breaks on suns running gcc without fixincludes */
#include <stdio.h>
#include <limits.h>
#if defined(HAVE_ASSERT_H)
# include <assert.h>
#endif

#if defined(TIME_WITH_SYS_TIME)
# include <sys/time.h>
# include <time.h>
#else
# if defined(HAVE_SYS_TIME_H)
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if defined(_AMIGA) && !defined(__AROS__)
const char *Version="$VER: "PARSE_VERSION_STRING" "__AMIGADATE__" $";
# if defined(__SASC)
static const long __stack = 0x6000;
# endif
#endif

const unsigned char u_to_l[256] =
{
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
   0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
   0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
   0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
   0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
   0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
   0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
   0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
   0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
   0x78, 0x79, 0x7a, 0x7b, 0x5c, 0x5d, 0x5e, 0x5f,
   0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
   0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
   0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
   0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
   0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
   0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
   0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
   0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
   0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
   0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
   0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
   0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
   0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
   0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
   0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
   0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
   0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
   0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
   0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
} ;



/* FIXME, FGC: Nice idea but what happens in case of EBCDIC, etc? */
streng *upcase( streng *source )
{
   int i=0 ;

   for (i=0;i<Str_len(source);i++)
      if ((source->value[i]<=LAST_CHAR+CHAR_OFFSET)&&(source->value[i]>=FIRST_CHAR+CHAR_OFFSET))
         source->value[i] -= CHAR_OFFSET ;

   return source ;
}


void getsecs( time_t *secs, time_t *usecs )
{
#if defined(HAVE_GETTIMEOFDAY)
   struct timeval times ;

   gettimeofday(&times, NULL) ;
   *secs = times.tv_sec ;
   *usecs = times.tv_usec ;

   if (times.tv_usec < 0)
   {
      *usecs = (times.tv_usec + 1000000) ;
      *secs = times.tv_sec - 1 ;
   }
   assert( *secs>=0 && *usecs>=0 ) ;

#elif defined(HAVE_FTIME)
   struct timeb timebuffer;

   ftime(&timebuffer);
   *secs = timebuffer.time;
   *usecs = timebuffer.millitm * 1000;
   assert( *secs>=0 && *usecs>=0 ) ;
#else
   *secs = time(NULL) ;
   *usecs = 0 ;
   assert( *secs>=0 && *usecs>=0 ) ;
#endif
}


const char *system_type( void )
{
#if defined(VMS)
   return "VMS" ;
#elif defined(OS2)
   return "OS/2" ;
#elif defined(DOS)
# if defined(__EMX__)
   if (_osmode == DOS_MODE)
      return("DOS");
   else
      return("OS/2");
#else
   return("DOS");
# endif
#elif defined(WIN32)
   return "WIN32" ;
#elif defined(_AMIGA) || defined(AMIGA)
   return "AMIGA" ;
#elif defined(__AROS__)
   return "AROS" ;
#elif defined(__QNX__)
   return "QNX" ;
#elif defined(__BEOS__)
   return "BEOS" ;
#elif defined(__CYGWIN__)
   return "WIN32" ;
#elif defined(__WINS__)
   return "EPOC32-WINS" ;
#elif defined(__EPOC32__)
   return "EPOC32-MARM" ;
#else
   return "UNIX" ;
#endif
}


#if !defined(HAVE_STRERROR)
/*
 * Sigh! This must probably be done this way, although it's incredibly
 * backwards. Some versions of gcc comes with a complete set of ANSI C
 * include files, which contains the definition of strerror(). However,
 * that function does not exist in the default libraries of SunOS.
 * To circumvent that problem, strerror() is #define'd to get_sys_errlist()
 * in config.h, and here follows the definition of that function.
 * Originally, strerror() was #defined to sys_errlist[x], but that does
 * not work if string.h contains a declaration of the (non-existing)
 * function strerror().
 *
 * So, this is a mismatch between the include files and the library, and
 * it should not create problems for Regina. However, the _user_ will not
 * encounter any problems until he compiles Regina, so we'll have to
 * clean up after a buggy installation of the C compiler!
 */
const char *get_sys_errlist( int num )
{
   extern char *sys_errlist[] ;
   return sys_errlist[num] ;
}
#endif


double cpu_time( void )
{
#ifndef CLOCKS_PER_SEC
/*
 * Lots of systems don't seem to get this ANSI C piece of code correctly
 * but most of them seems to use one million ...  Using a million for
 * those systems that haven't defined CLOCKS_PER_SEC may give an incorrect
 * value if clock() does not return microseconds!
 */
# define CLOCKS_PER_SEC 1000000
#endif
   return ((double)(clock()))/((double)(CLOCKS_PER_SEC)) ;
}

/* HIGHBIT is an unsigned with the highest bit set */
#define HIGHBIT (((unsigned) 1) << ((sizeof(unsigned) * CHAR_BIT) - 1))

/* hashvalue computes a value for hashing from a string content. Use
 * hashvalue_ic for a case insensitive version.
 * length may less than 0. The string length is computed by strlen() in this
 * case.
 * The return value might be modified by the %-operator for real hash
 * values.
 */
unsigned hashvalue(const char *string, int length)
{
   unsigned retval = 0, wrap ;
   const unsigned char *ptr = (const unsigned char *) string; /* unsigned char makes it fast */
   unsigned char c;

   if (length < 0)
      length = strlen(string);

   while (length--) {
      c = *ptr++; /* Yes this is slower but believe in your optimizer! */
      retval ^= c;
      wrap = (retval & HIGHBIT) ? 1 : 0;
      retval <<= 1;
      retval |= wrap;
   }

   return(retval);
}

/* hashvalue_ic computes a value for hashing from a string content. Use
 * hashvalue for a case significant version. This is case insensitive.
 * length may less than 0. The string length is computed by strlen() in this
 * case.
 * The return value might be modified by the %-operator for real hash
 * values.
 */
unsigned hashvalue_ic(const char *string, int length)
{
   unsigned retval = 0, wrap ;
   const unsigned char *ptr = (const unsigned char *) string; /* unsigned char makes it fast */
   unsigned char c;

   if (length < 0)
      length = strlen(string);

   while (length--) {
      c = RXTOLOW(*ptr);
      ptr++;
      retval ^= c;
      wrap = (retval & HIGHBIT) ? 1 : 0;
      retval <<= 1;
      retval |= wrap;
   }

   return(retval);
}

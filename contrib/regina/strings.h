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

/*
 * $Id$
 */


#ifndef _STRINGS_ALREADY_DEFINED_
#define _STRINGS_ALREADY_DEFINED_

typedef struct strengtype {
   int len, max ;
#ifdef CHECK_MEMORY                     /* FGC: Test                         */
   char *value;
#else
   char value[4] ;
#endif
} streng ;
#define STRENG_TYPEDEFED 1

#ifndef CHECK_MEMORY
typedef struct streng8type {
   int len, max ;
   char value[8] ;
} streng8 ;
#else
#define streng8 streng
#endif

#define Str_len(a) ((a)->len)
#define Str_max(a) ((a)->max)
#define Str_in(a,b) (Str_len(a)>(b))
#define Str_end(a) ((a)->value+Str_len(a))
#define Str_zero(a) ((Str_len(a)<Str_max(a)) && ((a)->value[(a)->len]==0x00))


#define STRHEAD (1+(sizeof(int)<<1))


typedef struct num_descr_type
{
   char *num ;     /* pointer to matissa of presicion + 1 */
   int negative ;  /* boolean, true if negative number */
   int exp ;       /* value of exponent */
   int size ;      /* how much of num is actually used */
   int max ;       /* how much can num actually take */
} num_descr ;


#endif /* _STRINGS_ALREADY_INCLUDED_ */

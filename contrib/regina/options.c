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

#include <assert.h>
#include <string.h>
#include <ctype.h>


#define OPTION(opt) { #opt, EXT_##opt, NULL }
#define METAOP(name,value) { #name, -1, value }


static const struct option all_options[] = {  /* Must be alphabetically sorted! */
   METAOP( ANSI, "FAST_LINES_BIF_DEFAULT" ),
   METAOP( BUFFERS, "BUFTYPE_BIF DESBUF_BIF DROPBUF_BIF MAKEBUF_BIF" ),
   OPTION( BUFTYPE_BIF ),
   OPTION( CACHEEXT ),
   OPTION( CLOSE_BIF ),
   OPTION( DESBUF_BIF ),
   OPTION( DROPBUF_BIF ),
   OPTION( EXT_COMMANDS_AS_FUNCS ),
   OPTION( FAST_LINES_BIF_DEFAULT ),
   METAOP( FILEIO, "OPEN_BIF CLOSE_BIF" ),
   OPTION( FIND_BIF ),
   OPTION( FLUSHSTACK ),
   OPTION( INTERNAL_QUEUES ),
   OPTION( LINEOUTTRUNC ),
   OPTION( MAKEBUF_BIF ),
   OPTION( OPEN_BIF ),
   OPTION( PRUNE_TRACE ),
   OPTION( STDOUT_FOR_STDERR ),
   OPTION( TRACE_HTML ),
   { NULL, 0 }
} ;


void do_options( const tsd_t *TSD, streng *options, int toggle )
{
   char *cptr=NULL, *eptr=NULL, *start=NULL ;
   int length=0, inverse=0, tmp=0 ;
   const struct option *lower=NULL, *upper=NULL, *middle=NULL ;

   cptr = options->value ;
   eptr = cptr + options->len ;

   while (cptr<eptr)
   {
      for (;cptr<eptr && isspace(*cptr); cptr++) ;
      for (start=cptr; cptr<eptr && !isspace(*cptr); cptr++ )
         *cptr = (char) toupper( *cptr ) ;

      if (((inverse=(*start=='N' && *(start+1)=='O'))!=0) && cptr>start+2)
         start += 2 ;

      length = cptr - start ;

      lower = all_options ;
      upper = lower + (sizeof(all_options)/sizeof(struct option)) - 2 ;

      while( upper >= lower )
      {
         middle = lower + (upper-lower)/2 ;
         tmp = strncmp(middle->name,start,length) ;
         if (tmp==0 && middle->name[length]==0x00)
            break ;

         if (tmp>0)
            upper = middle - 1 ;
         else
            lower = middle + 1 ;
      }

      /* If option is unknown, don't care ... */
      if ( upper >= lower )
      {
         assert ( middle->name ) ;
         if (middle->offset == -1)
         {
            do_options( TSD, Str_creTSD(middle->contains), toggle^inverse ) ;
         }
         else
         {
#ifdef OLD_OPTIONS
            int obyte=0, obit=0;

            obyte = middle->offset/(sizeof(unsigned char)*8) ;
            obit = middle->offset - obyte*(sizeof(unsigned char)*8) ;
fprintf(stderr,"Now %x First %x Next %x open_bif %x ext_comm %x\n",
               TSD->currlevel->u.flags[obyte],(unsigned char)~(1<<(7-obit)),
               (unsigned char)(1<<(7-obit)),
               TSD->currlevel->u.options.open_bif,
               TSD->currlevel->u.options.ext_commands_as_funcs
               ) ;

            if (inverse^toggle)
               TSD->currlevel->u.flags[obyte] &= (unsigned char)(~(1<<(7-obit))) ;
            else
               TSD->currlevel->u.flags[obyte] |= (unsigned char)(1<<(7-obit)) ;
fprintf(stderr,"Now %x First %x Next %x open_bif %x ext_comm %x\n",
               TSD->currlevel->u.flags[obyte],(unsigned char)~(1<<(7-obit)),
               (unsigned char)(1<<(7-obit)),
               TSD->currlevel->u.options.open_bif,
               TSD->currlevel->u.options.ext_commands_as_funcs
               ) ;
#else

            if (inverse^toggle)
               set_options_flag( TSD->currlevel, middle->offset, 0 ) ;
            else
               set_options_flag( TSD->currlevel, middle->offset, 1 ) ;
#endif
         }
      }
   }
   Free_stringTSD( options ) ;
}

#ifndef OLD_OPTIONS
int get_options_flag( cproclevel pl, int offset )
{
   register int obyte = offset / ( sizeof( unsigned char ) * 8 ) ;
   register int obit  = offset % ( sizeof( unsigned char ) * 8 ) ;

   return ( pl->u.flags[obyte] & ( 1 << ( 7 - obit ) ) ) ;
}

void set_options_flag( proclevel pl, int offset, int status )
{
   register int obyte = offset / ( sizeof( unsigned char ) * 8 ) ;
   register int obit  = offset % ( sizeof( unsigned char ) * 8 ) ;

   if ( status )
      pl->u.flags[obyte] |= (unsigned char)(1<<(7-obit)) ;
   else
      pl->u.flags[obyte] &= (unsigned char)(~(1<<(7-obit))) ;
}
#endif

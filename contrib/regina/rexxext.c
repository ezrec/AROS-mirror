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
#include <stdio.h>
#if !defined(VMS)                                       /* MH 10-06-96 */
# ifdef HAVE_UNISTD_H
#  include <unistd.h>                                    /* MH 10-06-96 */
# endif
# ifdef HAVE_PWD_H
#  include <pwd.h>                                      /* MH 10-06-96 */
# endif                                                 /* MH 10-06-96 */
#endif

#if defined(WIN32)
# ifdef _MSC_VER
#  if _MSC_VER >= 1100
/* Stupid MSC can't compile own headers without warning at least in VC 5.0 */
#   pragma warning(disable: 4115 4201 4214)
#  endif
# endif
# include <windows.h>
# include <conio.h>
# ifdef _MSC_VER
#  if _MSC_VER >= 1100
#   pragma warning(default: 4115 4201 4214)
#  endif
# endif
#endif

static int actually_pause = 1;

streng *rex_userid( tsd_t *TSD, cparamboxptr parms )
{
#if defined(WIN32)
   char buf[100];
   DWORD bufsize=sizeof( buf );
#endif
   checkparam(  parms,  0,  0 , "USERID" ) ;
#if defined(VMS) || defined(MAC) || defined(__WATCOMC__) || defined(_MSC_VER) || defined(_AMIGA) || defined(__AROS__) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__EPOC32__)
# if defined(WIN32) && !defined(WDOSX)
   if ( GetUserName( buf, &bufsize ) )
   {
      return( Str_creTSD( buf ) ) ;
   }
   else
      return nullstringptr( ) ;
# else
   return nullstringptr( ) ;
# endif
#else
   return( Str_creTSD( getpwuid( getuid( ) )->pw_name ) ) ;
#endif
}

streng *rex_rxqueue( tsd_t *TSD, cparamboxptr parms )
{
   char opt='?' ;
   streng *result=NULL ;

   checkparam(  parms,  1,  2 , "RXQUEUE" ) ;

   opt = getoptionchar( TSD, parms->value, "RXQUEUE", 1, "CDGS", "T" ) ;
   switch ( opt )
   {
      case 'C': /* Create */
         if ( ( parms->next )
         && ( parms->next->value ) )
            create_queue( TSD, parms->next->value, &result );
            /* result created by create_queue() */
         else
            create_queue( TSD, NULL, &result );
            /* result created by create_queue() */
         break;
      case 'D': /* Delete */
         if ( ( parms->next )
         && ( parms->next->value ) )
            result = int_to_streng( TSD, delete_queue(TSD, parms->next->value ) );
            /* result created here */
         else
            exiterror( ERR_INCORRECT_CALL, 5, "RXQUEUE", 2 );
         break;
      case 'G': /* Get */
         if ( ( parms->next )
         && ( parms->next->value ) )
            exiterror( ERR_INCORRECT_CALL, 4, "RXQUEUE", 1 );
         else
            result = get_queue( TSD );
            /* result created by get_queue() */
         break;
      case 'S': /* Set */
         if ( ( parms->next )
         && ( parms->next->value ) )
            result = Str_dup_TSD( TSD, set_queue( TSD, parms->next->value ) );
         else
            exiterror( ERR_INCORRECT_CALL, 5, "RXQUEUE", 2 );
         break;
      case 'T': /* Timeout */
         if ( ( parms->next )
         && ( parms->next->value ) )
            result = int_to_streng( TSD, timeout_queue(TSD, parms->next->value, NULL ) );
            /* result created here */
         else
            exiterror( ERR_INCORRECT_CALL, 5, "RXQUEUE", 2 );
         break;
   }
   return result ;
}

char *mygetenv( const tsd_t *TSD, const char *name, char *buf, int bufsize )
{
   char *ptr=NULL;
#ifdef WIN32
   DWORD ret=0;
#else
   char *ptr1=NULL;
#endif

#ifdef WIN32
   if (!buf)
   {
      ptr = MallocTSD(100);
      if (!ptr)
         return NULL;
      ret = GetEnvironmentVariable( name, ptr, 100 );
      if (ret == 0)
      {
         FreeTSD(ptr);
         return NULL;
      }
      if (ret > 100)
      {
         FreeTSD(ptr);
         ptr = MallocTSD(ret);
         if (!ptr)
            return NULL;
         ret = GetEnvironmentVariable( name, ptr, ret );
         if (ret == 0)
         {
            FreeTSD(ptr);
            return NULL;
         }
      }
      return ptr;
   }
   else
   {
      ret = GetEnvironmentVariable( name, buf, bufsize );
      if (ret == 0)
      {
         return NULL;
      }
      if (ret > (DWORD) bufsize)
      {
         return  NULL;
      }
      return buf;
   }
#else
   ptr = getenv(name);
   if (!ptr)
      return NULL;
   if (!buf)
   {
      ptr1 = MallocTSD(strlen(ptr)+1);
      if (!ptr1)
         return NULL;
      strcpy(ptr1, ptr);
   }
   else
   {
      if (strlen(ptr) > (size_t) bufsize-1)
         return NULL;
      strcpy(buf, ptr);
      ptr1 = buf;
   }
   return ptr1;
#endif
}

#if defined(WIN32) && !defined(__WINS__) && !defined(__EPOC32__)
/*
 * These functions are used to allow Regina to display "Press ENTER key to exit..."
 * in the console if it is NOT started from a console.
 */
void do_pause_at_exit( void )
{
   int ch;
   if ( actually_pause )
   {
      printf("\nPress ENTER key to exit...");
      fflush( stdout );
      ch = getchar();
   }
}

void flush_stdout( void )
{
   fflush( stdout );
   return;
}

void dont_pause_at_exit( void )
{
   actually_pause = 0;
}

void set_pause_at_exit( void )
{
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   HANDLE hStdOutput;

   hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
   if ( GetConsoleScreenBufferInfo( hStdOutput, &csbi ) )
   {
      /* 
       * if cursor position is (0,0) then use pause
       */
      if ( csbi.dwCursorPosition.X == 0 
      &&   csbi.dwCursorPosition.Y == 0 )
      {
         atexit( do_pause_at_exit );
      }
      else
      {
         atexit( flush_stdout );
      }
   }
   else
   {
      atexit( flush_stdout );
   }
   return;
}
#endif

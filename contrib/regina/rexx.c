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
#if defined(WIN32) && defined(__IBMC__)
#include <windows.h>
#pragma warning(default: 4115 4201 4214)
#else
# ifdef RXLIB
#  define APIENTRY
#  if defined(__WATCOMC__) && defined(__NT__)
#   undef APIENTRY
#   include <windows.h>
#  endif
#  if defined(__MINGW32__)
#   undef APIENTRY
#   include <windows.h>
#  endif
#  if defined(WIN32) && defined(__BORLANDC__)
#   undef APIENTRY
#   include <windows.h>
#  endif

#  if defined(_MSC_VER)
#   undef APIENTRY
#   if _MSC_VER >= 1100
/* Stupid MSC can't compile own headers without warning at least in VC 5.0 */
#    pragma warning(disable: 4115 4201 4214)
#   endif
#   include <windows.h>
#   if _MSC_VER >= 1100
#    pragma warning(default: 4115 4201 4214)
#   endif
#  endif
# endif
#endif

#include "rexx.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#ifdef VMS
# include <stat.h>
#elif defined(MAC)
# include "mac.h"
#else
# include <sys/stat.h>
#endif

#if defined(DJGPP) || defined(__EMX__) || defined(_MSC_VER) || (defined(__WATCOMC__) && !defined(__QNX__))
# include <fcntl.h>
# include <io.h>
#endif

/*
 * Since development of Ultrix has ceased, and they never managed to
 * fix a few things, we want to define a few things, just in order
 * to kill a few warnings ...
 */
#if defined(FIX_PROTOS) && defined(FIX_ALL_PROTOS) && defined(ultrix)
   int fstat( int fd, struct stat *buf ) ;
   int stat( char *path, struct stat *buf ) ;
#endif


/* Don't terminate the following lines by a semicolon */
GLOBAL_PROTECTION_VAR(regina_globals)


/*
 * Note: must match the settings of NUM_FORM_* in flags.h
 */
const char *numeric_forms[] = { "SCIENTIFIC", "ENGINEERING" } ;

/*
 * Note: these must match the definitions of INVO_* in defs.h
 */
const char *invo_strings[] = {
   "COMMAND", "FUNCTION", "SUBROUTINE" } ;

#ifdef TRACEMEM
void marksubtree( nodeptr ptr )
{
   int i=0 ;
   if ( ptr )
   {
      markmemory(ptr,TRC_TREENODE) ;
      if (ptr->name) markmemory(ptr->name, TRC_TREENODE) ;
      for (i=0;i<sizeof(ptr->p)/sizeof(ptr->p[0]);marksubtree(ptr->p[i++])) ;
      if (ptr->next) marksubtree( ptr->next ) ;

      if (ptr->type == X_STRING || ptr->type == X_CON_SYMBOL)
         if (ptr->u.number)
         {
            markmemory( ptr->u.number, TRC_TREENODE ) ;
            markmemory( ptr->u.number->num, TRC_TREENODE ) ;
         }

      if (ptr->type == X_CEXPRLIST)
         if (ptr->u.strng)
            markmemory( ptr->u.strng, TRC_TREENODE ) ;
   }
}


#endif /* TRACEMEM */


/* FIXME, FGC: This is a useless function! */
nodeptr treadit( cnodeptr tree )
{
/*
   nodeptr left, mid, right ;
 */
   tree = tree;
   return NULL ;
/*
   if (!tree)
      return NULL ;

   left = tree->p[0] ;
   mid = tree->p[1] ;
   right = tree->p[2] ;

   switch (tree->type)
   {
      case X_OTHERWISE:
      case X_PROGRAM:
         treadit( left ) ;
         tree = NULL ;
         break ;

      case X_STATS:
      case X_WHENS:
         left->next = treadit( mid ) ;
         treadit( left ) ;
         tree = left ;
         break ;

      case X_IF:
         treadit( mid ) ;
      case X_DO:
         treadit( right ) ;
         break ;

      case X_SELECT:
         treadit( left ) ;
      case X_WHEN:
         treadit( mid ) ;
         break ;
   }

   return tree ;
 */
}

#ifdef RXLIB
int APIENTRY __regina_faked_main(int argc,char *argv[])
#else
int main(int argc,char *argv[])
#endif
{
   FILE *fptr = NULL ;
   streng *string=NULL ;
   int i=0, j=0, stdinput=1, state=0, rcode=0, oldi=0, trace_override=0 ;
   paramboxptr args=NULL ;
   char *arg=NULL ;
   int make_perl=0 ;
   int do_yydebug=0;
   char name[1024];
   internal_parser_type parsing;
   tsd_t *TSD;

#ifdef MAC
   InitCursorCtl(nil);
#endif
   TSD = GLOBAL_ENTRY_POINT();

   TSD->stddump = stderr ;

   TSD->systeminfo = creat_sysinfo( TSD, Str_creTSD("SYSTEM")) ;
   TSD->systeminfo->called_as = Str_creTSD( argv[0] ) ;

   TSD->systeminfo->currlevel0 = TSD->currlevel = newlevel( TSD, NULL ) ;
   TSD->systeminfo->trace_override = 0;

   for (i=1; i<argc; i++)
   {
      arg = argv[i] ;
      if (state==0)
      {
         if (*arg=='-')
         {
            switch (*(++arg))
            {
               case 'i':
                  starttrace(TSD) ;
                  set_trace_char(TSD, 'A') ;
                  intertrace(TSD) ;
                  intertrace(TSD) ;
                  break ;

               case 'C':
                  if (*(arg+1)=='i')
                     TSD->isclient = 1 ; /* Other than the default value of 0 */
                  break ;

               case 'p':
                  make_perl = 1 ;
                  break ;

               case 'v':
                  fprintf( stderr, "%s\n", PARSE_VERSION_STRING );
                  return 0;
                  break ;

               case 'y':
                  do_yydebug = 1 ;
                  break ;

               case 't':
                  queue_trace_char(TSD, (char) (*(arg+1)? *(++arg) : 'A')) ;
                  trace_override = 1;
                  break ;

               case 'd':
                  if (*(arg+1)=='m')
                     TSD->listleakedmemory = 1 ;
                  break ;
            }
         }
         else
         {
            stdinput = 0 ;
            get_external_routine( TSD, "REGINA_MACROS", argv[i], &fptr, name, 1 );
            if (!fptr)
            {
               get_external_routine( TSD, "PATH", argv[i], &fptr, name, 1 );
               if ( !fptr )
               {
                  TSD->systeminfo->input_file = Str_crestrTSD(argv[i]) ;
                  exiterror( ERR_PROG_UNREADABLE, 1, "Program is unreadable" )  ;
               }
            }
            TSD->systeminfo->input_file = Str_crestrTSD(name) ;
            TSD->systeminfo->input_fp = fptr ;
            break ;
         }
      }
    }
    /*
     * Under DJGPP setmode screws up Parse Pull and entering code interactively :-(
     */
#if defined(__EMX__) || defined(_MSC_VER) || (defined(__WATCOMC__) && !defined(__QNX__))
    setmode( fileno( stdin ), O_BINARY );
    setmode( fileno( stdout ), O_BINARY );
    setmode( fileno( stderr ), O_BINARY );
#endif

   if (stdinput)
   {
      TSD->systeminfo->input_file = Str_crestrTSD("<stdin>") ;
      TSD->systeminfo->input_fp = NULL;
   }

   if (TSD->isclient)
      return 0 ;

   oldi = ++i ;

   for (j=1;i<argc;i++)
      j += strlen(argv[i]) + 1 ;

   TSD->currlevel->args = args = MallocTSD(sizeof(parambox)) ;
   memset(args,0,sizeof(parambox)); /* especially ->value */
/*
   args->value = Str_dupTSD(TSD->systeminfo->input_file) ;
   args = args->next = MallocTSD(sizeof(parambox)) ;
 */
   args->next = NULL ;
   if (oldi>=argc)
      args->value = string = NULL ;
   else
   {
      args->value = string = Str_makeTSD( j ) ;
      string->len = 0 ;
   }

   for (i=oldi;i<argc;i++)
   {
      string = Str_catstrTSD(string,argv[i]) ;
      string->value[string->len++] = ' ' ;
   }
   if (string && string->len)
     string->len-- ;

   signal_setup( TSD ) ;

#ifndef NDEBUG
   __reginadebug = do_yydebug ;   /* 1 == yacc-debugging */
#endif
   fetch_file( TSD, fptr ? fptr : stdin, &parsing );

   if (parsing.result != 0)
      exiterror( ERR_YACC_SYNTAX, 0 ) ;
   else
      TSD->systeminfo->tree = parsing;

   if (trace_override)
      TSD->systeminfo->trace_override = 1;
   else
      TSD->systeminfo->trace_override = 0;

#ifndef R2PERL
#ifndef MINIMAL
#ifndef VMS
#ifndef DOS
#ifndef _MSC_VER
#ifndef __IBMC__
#ifndef MAC
   if ( stdinput )
   {
      struct stat buffer ;

      /*
       * The following line is likely to give a warning when compiled
       * under Ultrix, this can be safely ignored, since it is just a
       * result of Digital not defining their include files properly.
       */
      rcode = fstat( fileno(stdin), &buffer ) ;
      if (rcode==0 && S_ISCHR(buffer.st_mode))
      {
         printf("  \b\b") ;
         fflush(stdout) ;
         rewind(stdin) ;
      }
   }
#endif /* !MAC */
#endif /* !__IBMC__ */
#endif /* !_MSC_VER */
#endif /* !DOS */
#endif /* !VMS */
#endif /* !MINIMAL */
#endif /* !R2PERL */

   treadit( TSD->systeminfo->tree.root ) ;

#ifdef R2PERL
   if (make_perl)
   {
      preamble() ;
      translate( TSD, TSD->systeminfo->tree.root ) ;
      return( 0 ) ;
   }
#endif

   flush_trace_chars(TSD) ;
   {
      nodeptr savecurrentnode = TSD->currentnode; /* pgb */
      string = interpret( TSD, TSD->systeminfo->tree.root ) ;
      TSD->currentnode = savecurrentnode; /* pgb */
   }
   rcode = EXIT_SUCCESS ;
   if ( string
   &&   myisinteger( string ) )
      rcode = myatol( TSD, string ) ;

   purge_stacks( TSD );
#if defined(FLISTS) && defined(NEW_FLISTS)
   free_flists();
#endif

#ifdef DYNAMIC
   /*
    * Remove all external function package functions
    * and libraries. Only valid for the DYNAMIC library.
    */
   purge_library( TSD );
#endif

#ifdef TRACEMEM
   if (TSD->listleakedmemory)
      listleaked( TSD, MEMTRC_LEAKED )  ;
#endif

   killsystem( TSD, TSD->systeminfo );
   TSD->systeminfo = NULL ;

   /*
    * Remove all memory allocated by the flists internal memory manager.
    */
#ifdef FLISTS
   purge_flists( TSD );
#endif

   return(rcode) ;

}

#ifdef TRACEMEM
void mark_systeminfo( const tsd_t *TSD )
{
   sysinfo sinfo=NULL ;
   labelbox *lptr=NULL ;
   lineboxptr llptr=NULL ;

   for (sinfo=TSD->systeminfo; sinfo; sinfo=sinfo->previous)
   {
      markmemory(sinfo, TRC_SYSINFO) ;
      markmemory(sinfo->called_as, TRC_SYSINFO) ;
      markmemory(sinfo->input_file, TRC_SYSINFO) ;
      markmemory(sinfo->environment, TRC_SYSINFO) ;
      markmemory(sinfo->callstack, TRC_SYSINFO) ;

      markvariables( TSD, sinfo->currlevel0 ) ;
      marksource( sinfo->tree.first_source_line ) ;
      /* FGC, FIXME: rewrite this: marksubtree( sinfo->tree.root ) ; */

      for (lptr=sinfo->tree.first_label; lptr; lptr=lptr->next )
      {
         markmemory( lptr, TRC_SYSINFO ) ;
      }

      for (llptr=sinfo->tree.first_source_line; llptr; llptr=llptr->next )
      {
         markmemory( llptr, TRC_SYSINFO ) ;
         markmemory( llptr->line, TRC_SYSINFO ) ;
      }
   }
}
#endif


sysinfobox *creat_sysinfo( const tsd_t *TSD, streng *envir )
{
   sysinfobox *sinfo=NULL ;

   sinfo = MallocTSD( sizeof(sysinfobox) ) ;
   sinfo->environment = envir ;
   sinfo->tracing = DEFAULT_TRACING ;
   sinfo->interactive = DEFAULT_INT_TRACING ;
   sinfo->previous = NULL ;
   sinfo->invoked = INVO_COMMAND ;
   sinfo->called_as = NULL ;
   sinfo->input_file = NULL ;
   sinfo->input_fp = NULL ;
   sinfo->panic = NULL ;
   sinfo->hooks = 0 ;
   sinfo->callstack = MallocTSD(sizeof(nodeptr)*10) ;
   sinfo->result = NULL ;
   sinfo->cstackcnt = 0 ;
   sinfo->cstackmax = 10 ;
   sinfo->trace_override = 0 ;
   memset(&sinfo->tree, 0, sizeof(sinfo->tree));

   return sinfo ;
}

#if !defined(RXLIB)

static void NoAPI( void )
{
   fprintf (stderr, "Warning: SAA API not compiled into interpreter\n" ) ;
}

int hookup( const tsd_t *TSD, int dummy )
{
   /* This should never happen, if we don't have support for SAA API,
    * Then we should never get a system exit!
    */
   assert( 0 ) ;
   dummy = dummy; /* keep compiler happy */
   TSD = TSD; /* keep compiler happy */
   return 1 ;  /* to keep compiler happy */
}
int hookup_input( const tsd_t *TSD, int dummy1, streng **dummy2 )
{
   /* This should never happen, if we don't have support for SAA API,
    * Then we should never get a system exit!
    */
   TSD = TSD; /* keep compiler happy */
   dummy1 = dummy1; /* keep compiler happy */
   dummy2 = dummy2; /* keep compiler happy */
   assert( 0 ) ;
   return 1 ;  /* to keep compiler happy */
}
int hookup_input_output( const tsd_t *TSD, int dummy1, const streng *dummy2, streng **dummy3 )
{
   /* This should never happen, if we don't have support for SAA API,
    * Then we should never get a system exit!
    */
   TSD = TSD; /* keep compiler happy */
   dummy1 = dummy1; /* keep compiler happy */
   dummy2 = dummy2; /* keep compiler happy */
   dummy3 = dummy3; /* keep compiler happy */
   assert( 0 ) ;
   return 1 ;  /* to keep compiler happy */
}
int hookup_output( const tsd_t *TSD, int dummy1, const streng *dummy2 )
{
   /* This should never happen, if we don't have support for SAA API,
    * Then we should never get a system exit!
    */
   assert( 0 ) ;
   dummy1 = dummy1; /* keep compiler happy */
   dummy2 = dummy2; /* keep compiler happy */
   TSD = TSD; /* keep compiler happy */
   return 1 ;  /* to keep compiler happy */
}
int hookup_output2( const tsd_t *TSD, int dummy1, const streng *dummy2, const streng *dummy3 )
{
   /* This should never happen, if we don't have support for SAA API,
    * Then we should never get a system exit!
    */
   assert( 0 ) ;
   dummy1 = dummy1; /* keep compiler happy */
   dummy2 = dummy2; /* keep compiler happy */
   dummy3 = dummy3; /* keep compiler happy */
   TSD = TSD; /* keep compiler happy */
   return 1 ;  /* to keep compiler happy */
}

static void Exit(const tsd_t *TSD)
{
   if (TSD->in_protected)
   {
      jmp_buf h;

      memcpy(h,TSD->protect_return,sizeof(jmp_buf));
      /* cheat about the const, we go away anyway :-) */
      *((int*) &TSD->delayed_error_type) = PROTECTED_DelayedExit;
      *((int*) &TSD->expected_exit_error) = 1;
      longjmp( h, 1 ) ;
   }
   TSD->MTExit( 1 ) ;
}

streng *do_an_external_exe( tsd_t *TSD, const streng *dummy1, cparamboxptr dummy2, char dummy3, char dummy4 )
{
   NoAPI();
   dummy1 = dummy1; /* keep compiler happy */
   dummy2 = dummy2; /* keep compiler happy */
   dummy3 = dummy3; /* keep compiler happy */
   dummy4 = dummy4; /* keep compiler happy */
   Exit( TSD ) ;
   return NULL;
}

streng *do_an_external_dll( tsd_t *TSD, const void *dummy1, cparamboxptr dummy2, char dummy3 )
{
   NoAPI();
   dummy1 = dummy1; /* keep compiler happy */
   dummy2 = dummy2; /* keep compiler happy */
   dummy3 = dummy3; /* keep compiler happy */
   Exit( TSD ) ;
   return NULL;
}


streng *SubCom( const tsd_t *TSD, const streng *dummy1, const streng *dummy2, int *dummy3 )
{
   NoAPI();
   dummy1 = dummy1; /* keep compiler happy */
   dummy2 = dummy2; /* keep compiler happy */
   dummy3 = dummy3; /* keep compiler happy */
   Exit( TSD ) ;
   return NULL;
}

int IfcHaveFunctionExit(const tsd_t *TSD)
{
   TSD = TSD; /* keep compiler happy */
   return(0);
}

#endif


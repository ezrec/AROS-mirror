#ifndef lint
static char *RCSid = "$Id$";
#endif

/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1993-1994  Anders Christensen <anders@pvv.unit.no>
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
/* JH 20-10-99 */  /* To make Direct setting of stems Direct and not Symbolic. */
#if defined(WIN32) && defined(__IBMC__)
# include <windows.h>
# pragma warning(default: 4115 4201 4214)
#else
# if defined(WIN32) && (defined(__WATCOMC__) || defined(__BORLANDC__) || defined(__MINGW32__))
#  undef APIENTRY
#  include <windows.h>
# else
#  if defined(__EMX__) && defined(OS2)
#   define DONT_TYPEDEF_PFN
#   undef APIENTRY
#   include <os2emx.h>
#  else
#   if defined(__WATCOMC__) && defined(OS2)
#    undef APIENTRY
#    define DONT_TYPEDEF_PFN
#    include <os2.h>
#   else
#    if defined(_MSC_VER) && !defined(__WINS__)
#     undef APIENTRY
#     if _MSC_VER >= 1100
/* Stupid MSC can't compile own headers without warning at least in VC 5.0 */
#      pragma warning(disable: 4115 4201 4214)
#     endif
#     include <windows.h>
#     if _MSC_VER >= 1100
#      pragma warning(default: 4115 4201 4214)
#     endif
#    else
#     define APIENTRY
#    endif
#   endif
#  endif
# endif
#endif

#ifndef RXLIB  /* need __regina_faked_main which is known with RXLIB only */
# define RXLIB
#endif
#include "rexx.h"

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "rxiface.h"
#include <stdio.h>
#include <string.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_ASSERT_H
# include <assert.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifndef max
# define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

static int ReMapHook( const tsd_t *TSD, int NetHook ) ;

typedef struct { /* cli_tsd: static variables of this module (thread-safe) */
   char    count_params_buf[32]; /* handle_no_of_params() */

   /* Buffering of external parameters allows a delayed deletion in case of
    * errors.
    */
   char *  ExternalName ;
   int     StringsCount ;
   int *   Lengths ; /* StringsCount + 1 elements */
   char ** Strings ; /* StringsCount + 1 elements */
} cli_tsd_t; /* thread-specific but only needed by this module. see
              * init_client
              */

/* init_client initializes the module.
 * Currently, we set up the thread specific data.
 * The function returns 1 on success, 0 if memory is short.
 */
int init_client( tsd_t *TSD )
{
   cli_tsd_t *ct;

   if (TSD->cli_tsd != NULL)
      return(1);

   if ((ct = TSD->cli_tsd = MallocTSD(sizeof(cli_tsd_t))) == NULL)
      return(0);
   memset(ct,0,sizeof(cli_tsd_t));
   ct->StringsCount = -1;
   return(1);
}


/* wrapstring makes a streng from external (application) data. If you get
 * an exception you may have a wild pointer in your application.
 * Returns NULL if string==NULL or length<0.
 */
static streng *wrapstring( const tsd_t *TSD, const char *string, int length )
{
   streng *result=NULL ;

   if (length>=0 && string)
   {
      result = Str_makeTSD( length ) ;
      memcpy( result->value, string, length ) ;
      result->len = length ;
   }
   else
      result = NULL ;

   return result ;
}


static int map_type( int in )
{
   if (in==RX_TYPE_COMMAND)
      return INVO_COMMAND ;
   else if (in==RX_TYPE_FUNCTION)
      return INVO_FUNCTION ;
   else
   {
      assert( in == RX_TYPE_SUBROUTINE ) ;
      return INVO_SUBROUTINE ;
   }
}


static void closedown( const tsd_t *TSD )
{
   CloseOpenFiles( TSD );
   if (TSD->in_protected)
   {
      jmp_buf h;

      memcpy(h,TSD->protect_return,sizeof(jmp_buf));
      /* cheat about the const, we go away anyway :-) */
      *((int*) &TSD->delayed_error_type) = PROTECTED_DelayedExit;
      *((int*) &TSD->expected_exit_error) = 0;
      longjmp( h, 1 ) ;
   }
   TSD->MTExit( 0 ) ;
}

static paramboxptr parametrize( const tsd_t *TSD, int ArgCount, const int *ParLengths, const char **ParStrings )
{
   paramboxptr parms=NULL, root=NULL ;
   int count=0, i=0 ;

   count = ArgCount ;
   for (i=0; count--; i++ )
   {
      if (parms)
      {
         parms->next = MallocTSD( sizeof(parambox)) ;
         parms = parms->next ;
      }
      else
         parms = root = MallocTSD( sizeof(parambox )) ;

      parms->value = wrapstring( TSD, ParStrings[i], ParLengths[i] ) ;
   }

   if (parms)
      parms->next = NULL ;

   return root ;
}

int IfcExecScript( int NameLen, const char *Name,
                   int ArgCount, const int *ParLengths, const char **ParStrings,
                   int CallType, int ExitFlags, int EnvLen, const char *EnvName,
                   int SourceCode, int restricted,
                   const char *SourceString, unsigned long SourceStringLen,
                   const void *TinnedTree, unsigned long TinnedTreeLen,
                   int *RetLen, char **RetString, void **instore_buf,
                   unsigned long *instore_length )
{
   streng * volatile command=NULL ;
   int RetCode=0 ;
   int num=0 ;
   int type=0 ;
   volatile paramboxptr params=NULL ;
   jmp_buf panic, *oldpanic ;
   tsd_t *wTSD;
   /* We need volatiles here to prevent variable clobbering by longjmp. */
   volatile int panicked=0;
   volatile int hooks=0, ctype=0 ;
   volatile streng * volatile environment=NULL ;
   volatile streng * volatile result=NULL ;
   volatile tsd_t *volatile TSD;
   unsigned InterpreterStatus[IPRT_BUFSIZE];

   TSD = __regina_get_tsd(); /* The TSD should be fetched directly.
                              * see longjmp and below for more information.
                              */
   wTSD = (tsd_t *) TSD;

   SaveInterpreterStatus(wTSD,InterpreterStatus);
   *instore_buf = NULL;
   *instore_length = 0;
   command = wrapstring( wTSD, Name, NameLen ) ;
   assert(command) ;
   params = parametrize( wTSD, ArgCount, ParLengths, ParStrings ) ;

   ctype = map_type(CallType) ;

   TSD->restricted = restricted;

   for (hooks=num=0; num<30; num++)
   {
      if ((ExitFlags>>num) & 0x01)
         hooks |= (1 << (ReMapHook(wTSD, num))) ;
   }

   environment = wrapstring( wTSD, EnvName, EnvLen ) ;
   if (!environment)
      environment = Str_cre_TSD( wTSD, "DEFAULT" ) ;

   if ( !envir_exists( wTSD, (streng *)environment ) )
      add_envir( wTSD, Str_dup_TSD(wTSD, (streng *)environment), ENVIR_PIPE, 0 ) ;

   /* It may be that TSD->systeminfo->panic is not set. This may lead to
    * an exit() call, e.g. at Rexx "EXIT". We set TSD->systeminfo->panic if it
    * is not set. If a problem occurs we will be informed by a longjmp and
    * we will not completely been killed by an exit(). Remember: This
    * function is typically called if a program uses "us" as a subroutine.
    * Exiting is very harmful in this case.
    * Note: The memory allocated within called subsoutines will NOT be
    * freed. We must change many things to do this. Maybe, it's a good
    * idea to traverse all called subroutine trees and free the leaves.
    * FGC
    */
   if ( ( oldpanic = TSD->systeminfo->panic ) == NULL )
   {
      TSD->systeminfo->panic = &panic;
      assert(!TSD->in_protected);
      if ( setjmp( *TSD->systeminfo->panic ) )
      {
         wTSD = (tsd_t *) TSD; /* Values in(!!) TSD may have changes due longjmp */
         if (result == NULL)
            result = Str_make_TSD(wTSD, 0);
         if (!RetCode)
            RetCode = -1 ;
         panicked++ ;
      }
   }
   wTSD = (tsd_t *) TSD; /* Values in(!!) TSD may have changes due longjmp */
   if ( !panicked )
   {
      type = SourceCode ;
      if (type==RX_TYPE_EXTERNAL)
      {
         result=execute_external(wTSD,command,params,(streng *) environment,
                                 &RetCode,hooks,ctype);
         Free_string_TSD( wTSD, command );
      }
      else if (type==RX_TYPE_INSTORE)
      {
         result=do_instore(wTSD,command,params,(streng *) environment,&RetCode,
                           hooks,
                           TinnedTree,TinnedTreeLen,
                           SourceString,SourceStringLen,
                           NULL,
                           ctype);
      }
      else if (type==RX_TYPE_MACRO)
         result = Str_make_TSD(wTSD, 0);
      else if (type==RX_TYPE_SOURCE)
      {
         streng *SrcStr = wrapstring( wTSD, SourceString, SourceStringLen ) ;
         internal_parser_type ipt;
         ipt = enter_macro( wTSD, SrcStr, command, instore_buf, instore_length ) ;
         if ( CallType == RX_TYPE_COMMAND
         && ArgCount
         && *ParLengths
         && *ParLengths == 3
         && memcmp( "//T", *ParStrings, 3 ) == 0 )
            ; /* request for tokenisation only, don't execute */
         else
         {
            ipt.kill = SrcStr ;
            result=do_instore(wTSD,command,params,(streng *) environment,&RetCode,
                           hooks,
                           NULL,0,
                           NULL,0,
                           &ipt,
                           ctype);
            /* do_instore already has deleted the internal structure */
         }
      }
      else
      {
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;
         return 0 ;
      }
   }

   TSD->systeminfo->panic = oldpanic;
   del_envir( wTSD, (streng *) environment ) ;
   Free_string_TSD( wTSD, (streng *) environment ) ;
   RestoreInterpreterStatus(wTSD,InterpreterStatus);

   if (result && result->len)
   {
      *RetLen = result->len ;
      *RetString=(char *)IfcAllocateMemory(result->len+1);
      if ( *RetString )
      {
         memcpy(*RetString,(void *) result->value,result->len);
         *(*RetString+(result->len)) = '\0';
      }
      else
         *RetLen = RX_NO_STRING ;
   }
   else
      *RetLen = RX_NO_STRING ;
   if (result)
      Free_TSD(wTSD, (void *) result);

   return RetCode ;
}


static int handle_source( const tsd_t *TSD, int *Length, char **String )
{
   int infile=0, total=0, invoked=0 ;
   const char *stype=NULL ;
   char *ctmp=NULL ;
   int sleng=0 ;

   stype = system_type() ;
   sleng = strlen( stype ) ;
   infile = TSD->systeminfo->input_file->len ;
   invoked = strlen(invo_strings[TSD->systeminfo->invoked]) ;
   total = sleng + 1 + invoked + 1 + infile ;

   ctmp = *String = MallocTSD( (*Length=total)+2 ) ;
   sprintf( ctmp, "%s %s ", stype, invo_strings[TSD->systeminfo->invoked]) ;
   strncat( ctmp, TSD->systeminfo->input_file->value, infile ) ;

   return RX_CODE_OK ;
}

static int handle_param( tsd_t *TSD, int *Length, char **String )
{
   paramboxptr ptr=NULL ;
   streng *value=NULL ;
   int number=0 ;

   ptr = TSD->currlevel->args ;
   value = wrapstring( TSD, *String, *Length ) ;
   number = atopos( TSD, value, "internal", 1 ) ;
   Free_stringTSD( value ) ;
   value = get_parameter( ptr, number ) ;

   if (value)
   {
      *(Length+1) = value->len ;
      *(String+1) = value->value ;
   }
   else
      *(Length+1) = RX_NO_STRING ;

   return RX_CODE_OK ;
}

static int handle_no_of_params( const tsd_t *TSD, int *Length, char **String )
{
   paramboxptr ptr=NULL ;
   int count=0 ;
   cli_tsd_t *ct;

   ct = TSD->cli_tsd;
   ptr = TSD->currlevel->args ;
   count = count_params( ptr, PARAM_TYPE_HARD ) ;

   sprintf( ct->count_params_buf, "%d", count ) ;
   *String = ct->count_params_buf;
   *Length = strlen(*String) ;

   return RX_CODE_OK ;
}



static int get_next_var( tsd_t *TSD, int *Lengths, char **Strings )
{
   variableptr value=NULL, rval=NULL, rrval=NULL ;
   int l;

   if (TSD->var_indicator==0)
   {
      get_next_variable( TSD, 1 ) ;
      TSD->var_indicator = 1 ;
   }

   for (;;)
   {
      value = get_next_variable( TSD, 0 ) ;
      for (rval=value; rval && rval->realbox; rval=rval->realbox) 
      {
        ;
      }
      if (rval && !(rval->flag & VFLAG_STR))
      {
         if (rval->flag & VFLAG_NUM)
         {
#ifdef DEBUG
            expand_to_str( TSD, rval ) ;
#else
            expand_to_str( TSD, rval ) ;
#endif
            break ;
         }

         if (!rval->stem)
            continue ;

         for (rrval=rval->stem; rrval && rrval->realbox; rrval=rrval->realbox)
         {
           ;
         }
         if (rrval && !(rrval->flag & VFLAG_STR))
         {
            if (rval->flag & VFLAG_NUM)
            {
#ifdef DEBUG
               expand_to_str( TSD, rval ) ;
#else
               expand_to_str( TSD, rval ) ;
#endif
               break ;
            }
            else
               continue ;
         }
      }
      break ;
   }

   if (rval)
   {
      if (rval->stem)
      {
         l = Lengths[0] = rval->stem->name->len + rval->name->len ;
         Strings[0] = MallocTSD( (l < 1) ? 1 : l ) ;
         memcpy(Strings[0], rval->stem->name->value, rval->stem->name->len);
         memcpy(Strings[0]+rval->stem->name->len,
                                      rval->name->value, rval->name->len ) ;
      }
      else
      {
         Lengths[0] = rval->name->len ;
         Strings[0] = rval->name->value ;
      }

      if (rval->value)
      {
         Lengths[1] = rval->value->len ;
         Strings[1] = rval->value->value ;
      }
      else
      {
         assert( rval->stem && rrval->value ) ;
         l = Lengths[1] = rval->stem->name->len + rval->name->len ;
         Strings[1] = MallocTSD( (l < 1) ? 1 : l ) ;
         memcpy( Strings[1], rval->stem->name->value, value->stem->name->len );
         memcpy( Strings[1]+value->stem->name->len,
                                        rval->name->value, rval->name->len ) ;
      }

      return 2 ;
   }
   else
   {
      /*
       * Once we have reached the end of all variables, reset var_indicator
       * so next time we can start from the beginning again.
       */
      TSD->var_indicator = 0 ;
      return 0 ;
   }
}


static int MapHook( const tsd_t *TSD, int RexxHook )
{
   switch ( RexxHook )
   {
      case HOOK_STDOUT: return RX_EXIT_STDOUT ;
      case HOOK_STDERR: return RX_EXIT_STDERR ;
      case HOOK_TRCIN:  return RX_EXIT_TRCIN ;
      case HOOK_PULL:   return RX_EXIT_PULL ;

      case HOOK_INIT:   return RX_EXIT_INIT ;
      case HOOK_TERMIN: return RX_EXIT_TERMIN ;
      case HOOK_SUBCOM: return RX_EXIT_SUBCOM ;

      case HOOK_GETENV: return RX_EXIT_GETENV ;
      case HOOK_SETENV: return RX_EXIT_SETENV ;

      default:
         closedown( TSD ) ;
   }

   return 0 ;
}


static int ReMapHook( const tsd_t *TSD, int NetHook )
{
   switch ( NetHook )
   {
      case RX_EXIT_STDOUT: return HOOK_STDOUT ;
      case RX_EXIT_STDERR: return HOOK_STDERR ;
      case RX_EXIT_TRCIN:  return HOOK_TRCIN ;
      case RX_EXIT_PULL:   return HOOK_PULL ;

      case RX_EXIT_TERMIN: return HOOK_TERMIN ;
      case RX_EXIT_INIT:   return HOOK_INIT ;
      case RX_EXIT_SUBCOM: return HOOK_SUBCOM ;
      case RX_EXIT_FUNC:   return HOOK_FUNC ;

      case RX_EXIT_GETENV: return HOOK_GETENV ;
      case RX_EXIT_SETENV: return HOOK_SETENV ;

      default:
         closedown( TSD ) ;
   }

   return 0 ;
}



int hookup( const tsd_t *TSD, int hook )
{
   int rcode=0, code=0;

   code = MapHook ( TSD, hook );
   assert (code == RX_EXIT_INIT || code == RX_EXIT_TERMIN );
   rcode = IfcDoExit( TSD, code, 0, NULL, 0, NULL, NULL, NULL ) ;
   if (rcode==RX_HOOK_ERROR)
       exiterror( ERR_SYSTEM_FAILURE, 0 )  ;
   else if (rcode==RX_HOOK_GO_ON)
      rcode = HOOK_GO_ON ;
   else if (rcode==RX_HOOK_NOPE)
      rcode = HOOK_NOPE ;
   else
      exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;
   return rcode ;
}

int hookup_output( const tsd_t *TSD, int hook, const streng *outdata )
{
   int rcode=0, code=0 ;
   char *str;
   int len = 0;

   code = MapHook( TSD, hook );
   assert (code == RX_EXIT_STDOUT || code == RX_EXIT_STDERR );
   if (outdata)
   {
      str = str_of( TSD, outdata ) ;
      len = outdata->len ;
   }
   else
   {
      str = MallocTSD( 1 ) ;
      str[0] = '\0' ;
      len = 0 ;
   }

   rcode = IfcDoExit( TSD, code, len, str, 0, NULL, NULL, NULL) ;
   FreeTSD( str ) ;
   if (rcode==RX_HOOK_ERROR)
       exiterror( ERR_SYSTEM_FAILURE, 0 )  ;
   else if (rcode==RX_HOOK_GO_ON)
      rcode = HOOK_GO_ON ;
   else if (rcode==RX_HOOK_NOPE)
      rcode = HOOK_NOPE ;
   else
      exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;

   return rcode ;
}

int hookup_output2( const tsd_t *TSD, int hook, const streng *outdata1, const streng *outdata2 )
{
   int rcode=0, code=0 ;
   char *str1, *str2;
   int len1 = 0, len2 = 0;

   code = MapHook( TSD, hook );
   assert (code == RX_EXIT_SETENV );
   if (outdata1)
   {
      str1 = str_of( TSD, outdata1 ) ;
      len1 = outdata1->len ;
   }
   else
   {
      str1 = MallocTSD( 1 ) ;
      str1[0] = '\0' ;
      len1 = 0 ;
   }
   if (outdata2)
   {
      str2 = str_of( TSD, outdata2 ) ;
      len2 = outdata2->len ;
   }
   else
   {
      str2 = MallocTSD( 1 ) ;
      str2[0] = '\0' ;
      len2 = 0 ;
   }

   rcode = IfcDoExit( TSD, code, len1, str1, len2, str2, NULL, NULL) ;
   FreeTSD( str1 ) ;
   FreeTSD( str2 ) ;
   if (rcode==RX_HOOK_ERROR)
       exiterror( ERR_SYSTEM_FAILURE, 0 )  ;
   else if (rcode==RX_HOOK_GO_ON)
      rcode = HOOK_GO_ON ;
   else if (rcode==RX_HOOK_NOPE)
      rcode = HOOK_NOPE ;
   else
      exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;

   return rcode ;
}

int hookup_input( const tsd_t *TSD, int hook, streng **indata )
{
   int rcode=0, code=0 ;
/* The following value allows called programs to call "free" to the return
 * parameters without destroying our stack.
 */
#define ILLEGAL_USE_SIZE (8 * sizeof(void *))
   char RetBuf[ILLEGAL_USE_SIZE + 256] ;
   int retlen=256 ;
   char *retstr=RetBuf;

   code = MapHook( TSD, hook );
   assert (code == RX_EXIT_PULL || code == RX_EXIT_TRCIN );

   strcpy(RetBuf,"");
   /* FIXME, FGC: The documentation says that the input of a PULL or TRCIN is
    * undefined. Why do we provide a buffer?
    */
   rcode = IfcDoExit( TSD, code, 0, NULL, 0, NULL, &retlen, &retstr ) ;
   if (rcode==RX_HOOK_ERROR)
      exiterror( ERR_SYSTEM_FAILURE, 0 )  ;
   else if (rcode==RX_HOOK_GO_ON)
      rcode = HOOK_GO_ON ;
   else if (rcode==RX_HOOK_NOPE)
      rcode = HOOK_NOPE ;
   else
      exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;

   *indata = wrapstring( TSD, retstr, retlen ) ;
   FreeTSD( retstr ) ; /* retstr was always newly allocated */
#undef ILLEGAL_USE_SIZE
   return rcode ;
}

int hookup_input_output( const tsd_t *TSD, int hook, const streng *outdata, streng **indata )
{
   char *str;
   int len = 0;
   int rcode=0, code=0 ;
/* The following value allows called programs to call "free" to the return
 * parameters without destroying our stack.
 */
#define ILLEGAL_USE_SIZE (8 * sizeof(void *))
   char RetBuf[ILLEGAL_USE_SIZE + 256] ;
   int retlen=256 ;
   char *retstr=RetBuf;

   code = MapHook( TSD, hook );
   assert (code == RX_EXIT_GETENV );

   if (outdata)
   {
      str = str_of( TSD, outdata ) ;
      len = outdata->len ;
   }
   else
   {
      str = MallocTSD( 1 ) ;
      str[0] = '\0' ;
      len = 0 ;
   }

   strcpy(RetBuf,"");
   /* FIXME, FGC: The documentation says that the input of a PULL or TRCIN is
    * undefined. Why do we provide a buffer?
    */
   rcode = IfcDoExit( TSD, code, len, str, 0, NULL, &retlen, &retstr ) ;
   FreeTSD( str ) ;
   if (rcode==RX_HOOK_ERROR)
      exiterror( ERR_SYSTEM_FAILURE, 0 )  ;
   else if (rcode==RX_HOOK_GO_ON)
      rcode = HOOK_GO_ON ;
   else if (rcode==RX_HOOK_NOPE)
      rcode = HOOK_NOPE ;
   else
      exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;

   *indata = wrapstring( TSD, retstr, retlen ) ;
   FreeTSD( retstr ) ; /* retstr was always newly allocated */
#undef ILLEGAL_USE_SIZE
   return rcode ;
}



streng *SubCom( const tsd_t *TSD, const streng *command, const streng *envir, int *rc )
{
   int tmplen ;
   char *tmpptr ;
   streng *retval ;

   *rc = IfcSubCmd( TSD,
                    envir ? envir->len : RX_NO_STRING,
                    envir ? envir->value : NULL,
                    command->len, command->value, &tmplen, &tmpptr ) ;

   retval = wrapstring( TSD, tmpptr, tmplen ) ;
   FreeTSD( tmpptr ) ;
   return retval ;
}


/****************************************************************************
 *
 *  JH 13/12/1999 Trying to satisfy both BUG031 and BUG022.
 *
 *  BUG022  To make Direct setting of stems Direct and not Symbolic.
 *   - Added new parameter, "Code".  Assumed values are RX_GETSVAR, and
 *     RX_GETVAR.  They denote that the actiion is symbolic, or direct
 *     (respectively).
 *   - if direct call getdirvalue()
 *
 *  BUG031
 *   - if default value is being returned, uppercase the whole name for
 *     symbolics, and only the stem for directs.  This should be a moot
 *     issue, as there is code in variable.c to return default values.
 *
 ****************************************************************************/
static int GetVariable( tsd_t *TSD, int Code, int *Lengths, char *Strings[] )
{
   const streng *value ;
   streng *retval, *varbl;
   int rcode, i ;

   varbl = wrapstring( TSD, Strings[0], Lengths[0] ) ;

   assert(varbl); /* FIXME, FGC: Shouldn't this better result in
                   * RX_CODE_INVNAME?
                   */

   if (!valid_var_symbol(varbl))
   {
      Free_stringTSD( varbl ) ;
      Lengths[1] = RX_NO_STRING ;
      return RX_CODE_INVNAME ;
   }

   rcode = RX_CODE_OK ; /* default value */
   if (Code == RX_GETSVAR)
      value = getvalue( TSD, varbl, 1 ) ;
   else
      value = getdirvalue( TSD, varbl, 1 ) ;

   retval = Str_dupTSD(value) ;
   if (!var_was_found( TSD ))
   {
      if (Code == RX_GETSVAR)
         retval = Str_upper(retval) ;
      else
      {
           /* JH 13-12-99 If this is a direct variable, only uppercase the
            * stem name.
            */
         for(i=0;i<retval->len && '.' != retval->value[i];i++)
            retval->value[i] = (unsigned char) toupper(retval->value[i]);
      }
      rcode = RX_CODE_NOVALUE ;
   }

   Lengths[1] = retval->len ;
   Strings[1] = IfcAllocateMemory(retval->len + 1);
      /* Must be accessible by the application. */

   memcpy(Strings[1],retval->value,retval->len);
   Strings[1][retval->len] = '\0'; /* termination may help bogus
                                    * applications.
                                    */

   Free_stringTSD( retval ) ;
   Free_stringTSD( varbl ) ;
   return rcode ;
}

/****************************************************************************
 *
 *  JH 13/12/1999 Trying to satisfy both BUG031 and BUG022.
 *
 *  BUG022  To make Direct setting of stems Direct and not Symbolic.
 *   - Added new parameter, "Code".  Assumed values are RX_SETSVAR, and
 *     RX_SETVAR.  They denote that the actiion is symbolic, or direct
 *     (respectively).
 *  Until this point setting and dropping variables have been treated as
 *  the same.  If there is a value, then it is a set, otherwise drop it.
 *   - if direct call setdirvalue()
 *   - if direct call drop_dirvar()
 *  BUG031
 *   - uppercase the whole name for symbolics, and only the stem for directs.
 *
 ****************************************************************************/
static int SetVariable(const tsd_t *TSD, int Code, int *Lengths, char *Strings[] )
{
   streng *varbl, *varname, *value ;
   int rcode, i ;

   varbl = wrapstring( TSD, Strings[0], Lengths[0] ) ;

   assert(varbl); /* FIXME, FGC: Shouldn't this better result in
                   * RX_CODE_INVNAME?
                   */

   if (Code == RX_SETSVAR)
      varname = Str_upper(Str_dupTSD(varbl)) ;
   else
   {
      varname = Str_dupTSD(varbl) ;
      for (i=0;i<varname->len && '.' != varname->value[i];i++)
         varname->value[i] = (unsigned char) toupper(varname->value[i]);
   }
   Free_stringTSD( varbl ) ;
   varbl = NULL; /* For debugging purpose only */

   if (!valid_var_symbol(varname))
   {
      Free_stringTSD( varname ) ;
      return RX_CODE_INVNAME ;
   }

   value = wrapstring( TSD, Strings[1], Lengths[1] ) ;

   rcode = RX_CODE_OK ; /* default value */
   if (value)
   {
      if (Code == RX_SETSVAR)
         setvalue( TSD, varname, value ) ;
      else
         setdirvalue( TSD, varname, value ) ;
   }
   else
   {
      if (Code == RX_SETSVAR)
         drop_var( TSD, varname ) ;
      else
         drop_dirvar( TSD, varname ) ;
   }
   if (!var_was_found( TSD ))
      rcode = RX_CODE_NOVALUE ;

   Free_stringTSD( varname ) ;
   return rcode ;
}

static int handle_version( int *Length, char **String )
{
   *Length = strlen(PARSE_VERSION_STRING) ;
   *String = PARSE_VERSION_STRING ;
   return RX_CODE_OK ;
}


static int handle_queue( int *Length, char **String )
{
   *Length = 7 ;
   *String = "default" ;
   return RX_CODE_OK ;
}




/****************************************************************************
 *
 *  JH 13/12/1999
 *
 *  BUG022  To make Direct setting of stems Direct and not Symbolic.
 *   - Added checks for the direct variable functions RX_GETVAR and RX_SETVAR.
 *
 *
 *
 ****************************************************************************/
int IfcVarPool( tsd_t *TSD, int Code, int *Lengths, char *Strings[] )
{
   int rc=0 ;

   if (Code==RX_GETSVAR ||Code==RX_GETVAR )
      rc = GetVariable( TSD, Code, Lengths, Strings ) ;
   else if (Code==RX_SETSVAR ||Code==RX_SETVAR )
      rc = SetVariable( TSD, Code, Lengths, Strings ) ;
   else if (Code==RX_NEXTVAR)
      rc = get_next_var( TSD, Lengths, Strings ) ;
   else if (Code==RX_CODE_VERSION)
      rc = handle_version( Lengths, Strings ) ;
   else if (Code==RX_CODE_SOURCE)
      rc = handle_source( TSD, Lengths, Strings ) ;
   else if (Code==RX_CODE_QUEUE)
      rc = handle_queue( Lengths, Strings ) ;
   else if (Code==RX_CODE_PARAMS)
      rc = handle_no_of_params( TSD, Lengths, Strings ) ;
   else if (Code==RX_CODE_PARAM)
      rc = handle_param( TSD, Lengths, Strings ) ;
   else
      exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;

   return rc ;
}


/* FGC, FIXME: IfcRegFunc never returns a code for RXFUNC_DUP! */
int IfcRegFunc( tsd_t *TSD, const char *Name )
{
   streng *name;
   int rc;

   assert( Name ) ;
   name = Str_upper( Str_creTSD( Name ) );
   rc = addfunc( TSD, name, 1 ) ;
   if (rc < 1)
      Free_stringTSD(name);

   if (rc == -1)
      return RX_CODE_NOMEM ;
   return RX_CODE_OK ;
}


int IfcDelFunc( tsd_t *TSD, const char *Name )
{
   streng *name;
   int retval;

   assert( Name ) ;
   name = Str_upper( Str_creTSD( Name ) );
   retval = rex_rxfuncdlldrop(TSD, name);
   Free_stringTSD(name);
   return(retval);
}

int IfcQueryFunc( tsd_t *TSD, const char *Name )
{
   streng *name;
   int retval;

   assert( Name ) ;
   name = Str_upper( Str_creTSD( Name ) );
   retval = rex_rxfuncdllquery(TSD, name);
   Free_stringTSD(name);
   return(retval);
}

/* You are not allowed to use TSD or __regina_get_tsd() here! */
void *IfcAllocateMemory( unsigned long size )
{
   void *ret;
#if defined( WIN32 )
   /* We now use the Virtual-functions instead of Global... */
   ret = VirtualAlloc( NULL, size, MEM_COMMIT, PAGE_READWRITE ) ;
   return ret;
#elif defined( __EMX__ ) && !defined(DOS)
   if (_osmode == OS2_MODE)
   {
      if ( ( BOOL )DosAllocMem( &ret, size, fPERM|PAG_COMMIT ) )
         return NULL;
      else
         return ret;
   }
   else /* DOS or something else */
   {
      ret = (void *)malloc( size );
      return ret;
   }
#elif defined( OS2 )
   if ( ( BOOL )DosAllocMem( &ret, size, fPERM|PAG_COMMIT ) )
      return NULL;
   else
      return ret;
#else
   ret = (void *)malloc( size );
   return ret;
#endif
}

/* You are not allowed to use TSD or __regina_get_tsd() here! */
unsigned long IfcFreeMemory( void *ptr )
{
#if defined( WIN32 )
   /* In opposite to most(!) of the documentation from Microsoft we shouldn't
    * decommit and release together. This won't work at least under NT4SP6a.
    * We can first decommit and then release or release at once. FGC.
    */
   VirtualFree( ptr, 0, MEM_RELEASE ) ;
#elif defined( __EMX__ ) && !defined(DOS)
   if (_osmode == OS2_MODE)
      DosFreeMem( ptr );
   else /* DOS or something else */
      free( ptr );
#elif defined( OS2 )
   DosFreeMem( ptr );
#else
   free( ptr );
#endif
   return 0;
}


/* Removes ct->Strings and ct->Lengths AND ct->ExternalName. */
static void RemoveParams(const tsd_t *TSD)
{
   int i;
   cli_tsd_t *ct;

   ct = TSD->cli_tsd;

   if ( ct->Strings && ct->Lengths )
   {
      for (i = 0;i < ct->StringsCount;i++) /* The last one is always NULL */
      {
         if ((ct->Lengths[i] != RX_NO_STRING) &&
             (ct->Strings[i] != NULL))
            FreeTSD( ct->Strings[i] );
      }
   }

   if ( ct->Lengths )
      FreeTSD( ct->Lengths ) ;
   if ( ct->Strings )
      FreeTSD( ct->Strings ) ;
   if ( ct->ExternalName )
      FreeTSD( ct->ExternalName ) ;

   ct->StringsCount = -1;
   ct->Strings = NULL;
   ct->Lengths = NULL;
   ct->ExternalName = NULL;
}

/* Creates (possibly after a removal of old values) ct->Strings and ct->Lengths
 * from the given parms. One hidden NULL string is appended.
 */
static void MakeParams(const tsd_t *TSD, cparamboxptr parms)
{
   int i;
   cparamboxptr p=NULL ;
   cli_tsd_t *ct;

   ct = TSD->cli_tsd;

   /* Cleanup the old parameters before we set StringsCount */
   RemoveParams(TSD);

   ct->StringsCount = 0; /* This is the default in case of unused parameters */
   /* Detect the index of the last valid parameter */
   for (i=0,p=parms; p; p=p->next,i++)
   {
      if (p->value)
         ct->StringsCount=i+1 ;
   }

   /* add one NULL string at the end */
   ct->Lengths = MallocTSD( sizeof(int) * (ct->StringsCount+1) ) ;
   ct->Strings = MallocTSD( sizeof(char*) * (ct->StringsCount+1) ) ;

   for (i=0,p=parms; i < ct->StringsCount; p=p->next,i++)
   {
      if (p->value)
      {
         ct->Lengths[i] = Str_len( p->value ) ;
         ct->Strings[i] = str_of( TSD, p->value );
      }
      else
      {
         ct->Lengths[i] = RX_NO_STRING ;
         ct->Strings[i] = NULL ;
      }
   }

   /* Provide a hidden NULL string at the end */
   ct->Lengths[ct->StringsCount] = RX_NO_STRING ;
   ct->Strings[ct->StringsCount] = NULL ;
}

/* do_an_external calls IfcExecFunc with the appropriate parameters. Basically
 * it wraps the parameters. Either ExeName or box must be NULL.
 */
static streng *do_an_external( tsd_t *TSD,
                               const streng *ExeName,
                               const struct library_func *box,
                               cparamboxptr parms,
                               char exitonly,
                               char called )
{
   int RetLength=0 ;
   char *RetString=NULL ;
   streng *retval ;
   int rc ;
   int RC ;
   PFN Func;
   cli_tsd_t *ct;
   volatile char *tmpExternalName; /* used to save ct->ExternalName */
                                   /* when erroring                 */

   ct = TSD->cli_tsd;

   MakeParams( TSD, parms ) ;
   if (ExeName)
   {
      ct->ExternalName = str_of( TSD, ExeName );
      tmpExternalName = tmpstr_of( TSD, ExeName );
      Func = NULL;
   }
   else
   {
      ct->ExternalName = str_of( TSD, box->name );
      tmpExternalName = tmpstr_of( TSD, box->name );
      Func = box->addr;
   }

   rc = IfcExecFunc( TSD, Func, ct->ExternalName,
                     ct->StringsCount, ct->Lengths, ct->Strings,
                     &RetLength, &RetString, &RC, exitonly, called );

   RemoveParams( TSD ) ;

   if (RC)
   {
      switch( RC )
      {
         case ERR_ROUTINE_NOT_FOUND:
            exiterror( ERR_ROUTINE_NOT_FOUND, 1, tmpExternalName );
            break;
         default:
            exiterror( RC, 0) ;
            break;
      }
      retval = NULL ;
   }
   else
   {
      retval = Str_makeTSD( RetLength ) ;
      memcpy( retval->value, RetString, RetLength ) ;
      FreeTSD( RetString );
      retval->len = RetLength ;
   }
   return retval ;

}

streng *do_an_external_exe( tsd_t *TSD, const streng *name, cparamboxptr parms, char exitonly, char called )
{
   assert(name);

   return( do_an_external( TSD, name, NULL, parms, exitonly, called ) ) ;
}

streng *do_an_external_dll( tsd_t *TSD, const void *vbox, cparamboxptr parms, char called )
{
   assert(vbox);

   return( do_an_external( TSD, NULL, vbox, parms, 0, called ) ) ;
}

int IfcCreateQueue( tsd_t *TSD, const char *qname, const int qlen, char *data, unsigned long *dupflag, unsigned long buflen )
{
   streng *queuename=NULL,*strdata;
   int rc;

   if ( qname )
   {
      queuename = Str_makeTSD( qlen ) ;
      memcpy( queuename->value, qname, qlen ) ;
      queuename->len = qlen;
   }
   rc = create_queue( TSD, queuename, &strdata );

   if ( rc == 0 || rc == 1 )
   {
      /*
       * Return the new queue name
       */
      memcpy( data, strdata->value, max( strdata->len, (int) buflen) - 1 );
      *(data+(max( strdata->len, (int) buflen) - 1)) = '\0';
      /*
       * If the returned queue name is different to
       * the one we requested, set the dupflag
       */
      if ( queuename 
      &&   rc == 1 )
         *dupflag = 3;
      else
         *dupflag = 0;
      FreeTSD( strdata );
      rc = 0;
   }
   if ( queuename) FreeTSD( queuename );
   return rc;
}

int IfcDeleteQueue( tsd_t *TSD, const char *qname, const int qlen )
{
   streng *queuename;
   int rc;

   queuename = Str_makeTSD( qlen ) ;
   memcpy( queuename->value, qname, qlen ) ;
   queuename->len = qlen;
   rc = delete_queue( TSD, queuename );
   FreeTSD( queuename );
   return rc;
}

int IfcQueryQueue( tsd_t *TSD, const char *qname, const int qlen, unsigned long *count )
{
   streng *queuename;
   int rc;

   queuename = Str_makeTSD( qlen ) ;
   memcpy( queuename->value, qname, qlen ) ;
   queuename->len = qlen;
   rc = lines_in_stack( TSD, queuename );
   if ( rc < 0 )
      rc = -rc;
   else
      *count = rc;
   FreeTSD( queuename );
   return rc;
}

int IfcAddQueue( tsd_t *TSD, const char *qname, const int qlen, const char *data, const int datalen, unsigned long addlifo )
{
   streng *queuename,*strdata;
   int rc;

   queuename = Str_makeTSD( qlen ) ;
   memcpy( queuename->value, qname, qlen ) ;
   queuename->len = qlen;

   strdata = Str_makeTSD( datalen ) ;
   memcpy( strdata->value, data, datalen ) ;
   strdata->len = datalen;

   if ( addlifo )
      rc = stack_lifo( TSD, strdata, queuename );
   else
      rc = stack_fifo( TSD, strdata, queuename );
   FreeTSD( queuename );
   FreeTSD( strdata );
   return rc;
}

int IfcPullQueue( tsd_t *TSD, const char *qname, const int qlen, char **data, int *datalen, unsigned long waitforline )
{
   streng *queuename,*strdata;
   int rc;

   queuename = Str_makeTSD( qlen ) ;
   memcpy( queuename->value, qname, qlen ) ;
   queuename->len = qlen;

   strdata = popline( TSD, queuename, &rc, waitforline );

   if ( strdata == NULL )
   {
      /*
       * Queue is empty
       */
      *data = NULL;
      *datalen = 0;
   }
   else
   {
      *data = strdata->value;
      *datalen = strdata->len;
   }
   FreeTSD( queuename );
   return rc;
}

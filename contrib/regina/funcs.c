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
#include "rxiface.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

struct function_type
{
   int length ;
   streng *(*function)(tsd_t *,cparamboxptr) ;
   const char *funcname ;
} ;

#if defined(OLD_REGINA_FEATURES) && defined(AREXXIO)
#error OLD_REGINA_FEATURES and AREXXIO options are incompatible
#endif

static const struct function_type functions[] = {
  { 0,              std_abbrev, "ABBREV" },
  { 0,              std_abs, "ABS" },
#if defined(_AMIGA) || defined(__AROS__)
  { 0,              arexx_addlib, "ADDLIB" },
#endif
  { 0,              std_address, "ADDRESS" },
#ifdef TRACEMEM
  { 0,              dbg_allocated, "ALLOCATED" },
#endif
  { 0,              std_arg, "ARG" },
  { 0,              arexx_b2c, "B2C" },
  { 0,              std_b2x, "B2X" },
  { 0,              os2_beep, "BEEP" },
  { 0,              std_bitand, "BITAND" },
  { 0,              arexx_bitchg, "BITCHG" },
  { 0,              arexx_bitclr, "BITCLR" },
  { 0,              arexx_bitcomp, "BITCOMP" },
  { 0,              std_bitor, "BITOR" },
  { 0,              arexx_bitset, "BITSET" },
  { 0,              arexx_bittst, "BITTST" },
  { 0,              std_bitxor, "BITXOR" },
  { EXT_BUFTYPE_BIF,cms_buftype, "BUFTYPE" },
  { 0,              arexx_c2b, "C2B" },
  { 0,              std_c2d, "C2D" },
  { 0,              std_c2x, "C2X" },
  { 0,              unx_chdir, "CD" },
  { 0,              std_center, "CENTER" },
  { 0,              std_center, "CENTRE" },
  { 0,              std_changestr, "CHANGESTR" }, /* ANSI Std 1996 - MH 10-06-96 */
  { 0,              std_charin, "CHARIN" },
  { 0,              std_charout, "CHAROUT" },
  { 0,              std_chars, "CHARS" },
  { 0,              unx_chdir, "CHDIR" },
#ifdef OLD_REGINA_FEATURES
  { 0,              unx_close, "CLOSE" },
#endif
#ifdef AREXXIO
  { 0,              arexx_close, "CLOSE" },
#endif
  { 0,              std_compare, "COMPARE" },
  { 0,              arexx_compress, "COMPRESS" },
  { 0,              std_condition, "CONDITION" },
  { 0,              std_copies, "COPIES" },
  { 0,              std_countstr, "COUNTSTR" },   /* ANSI Std 1996 - MH 10-06-96 */
  { 0,              unx_crypt, "CRYPT" },
  { 0,              std_d2c, "D2C" },
  { 0,              std_d2x, "D2X" },
  { 0,              std_datatype, "DATATYPE" },
  { 0,              std_date, "DATE" },
  { 0,              std_delstr, "DELSTR" },
  { 0,              std_delword, "DELWORD" },
  { EXT_DESBUF_BIF, cms_desbuf, "DESBUF" },
  { 0,              std_digits, "DIGITS" },
  { 0,              os2_directory, "DIRECTORY" },
  { EXT_DROPBUF_BIF,cms_dropbuf, "DROPBUF" },
#ifndef NDEBUG
  { 0,              dbg_dumpfiles, "DUMPFILES" },
  { 0,              dbg_dumptree, "DUMPTREE" },
  { 0,              dbg_dumpvars, "DUMPVARS" },
#endif
#ifdef OLD_REGINA_FEATURES
  { 0,              unx_eof, "EOF" },
#endif
#ifdef AREXXIO
  { 0,              arexx_eof, "EOF" },
#endif
  { 0,              std_errortext, "ERRORTEXT" },
#ifdef VMS
  { 0,              vms_f_cvsi, "F$CVSI" },
  { 0,              vms_f_cvtime, "F$CVTIME" },
  { 0,              vms_f_cvui, "F$CVUI" },
  { 0,              vms_f_directory, "F$DIRECTORY" },
  { 0,              vms_f_element, "F$ELEMENT" },
  { 0,              vms_f_extract, "F$EXTRACT" },
  { 0,              vms_f_fao, "F$FAO" },
  { 0,              vms_f_file_attributes, "F$FILE_ATTRIBUTES" },
  { 0,              vms_f_getdvi, "F$GETDVI" },
  { 0,              vms_f_getjpi, "F$GETJPI" },
  { 0,              vms_f_getqui, "F$GETQUI" },
  { 0,              vms_f_getsyi, "F$GETSYI" },
  { 0,              vms_f_identifier, "F$IDENTIFIER" },
  { 0,              vms_f_integer, "F$INTEGER" },
  { 0,              vms_f_length, "F$LENGTH" },
  { 0,              vms_f_locate, "F$LOCATE" },
  { 0,              vms_f_logical, "F$LOGICAL" },
  { 0,              vms_f_message, "F$MESSAGE" },
  { 0,              vms_f_mode, "F$MODE" },
  { 0,              vms_f_parse, "F$PARSE" },
  { 0,              vms_f_pid, "F$PID" },
  { 0,              vms_f_privilege, "F$PRIVILEGE" },
  { 0,              vms_f_process, "F$PROCESS" },
  { 0,              vms_f_search, "F$SEARCH" },
  { 0,              vms_f_setprv, "F$SETPRV" },
  { 0,              vms_f_string, "F$STRING" },
  { 0,              vms_f_time, "F$TIME" },
  { 0,              vms_f_trnlnm, "F$TRNLNM" },
  { 0,              vms_f_type, "F$TYPE" },
  { 0,              vms_f_user, "F$USER" },
/*{ 0,              vms_f_verify, "F$VERIFY" }, */
#endif
  { 0,              os2_filespec, "FILESPEC" },
  { EXT_FIND_BIF,   cms_find, "FIND" },
#ifdef OLD_REGINA_FEATURES
  { 0,              unx_close, "FINIS" },
#endif /* OLD_REGINA_FEATURES */
  { 0,              unx_fork, "FORK" },
  { 0,              std_form, "FORM" },
  { 0,              std_format, "FORMAT" },
#if defined(REGINA_DEBUG_MEMORY)
  { 0,              dbg_freelists, "FREELISTS" },
#endif
  { 0,              arexx_freespace, "FREESPACE" },
  { 0,              std_fuzz, "FUZZ" },
#if defined(_AMIGA) || defined(__AROS__)
  { 0,              arexx_getclip, "GETCLIP" },
#endif
  { 0,              unx_getenv, "GETENV" },
  { 0,              unx_getpath, "GETPATH" },
  { 0,              unx_getpid, "GETPID" },
  { 0,              arexx_getspace, "GETSPACE" },
  { 0,              unx_gettid, "GETTID" },
  { 0,              arexx_hash, "HASH" },
  { 0,              cms_index, "INDEX" },
  { 0,              std_insert, "INSERT" },
  { 0,              cms_justify, "JUSTIFY" },
  { 0,              std_lastpos, "LASTPOS" },
  { 0,              std_left, "LEFT" },
  { 0,              std_length, "LENGTH" },
  { 0,              std_linein, "LINEIN" },
  { 0,              std_lineout, "LINEOUT" },
  { 0,              std_lines, "LINES" },
#ifdef TRACEMEM
  { 0,              dbg_listleaked, "LISTLEAKED" },
#endif
  { EXT_MAKEBUF_BIF,cms_makebuf, "MAKEBUF" },
  { 0,              std_max, "MAX" },
#ifdef TRACEMEM
  { 0,              dbg_memorystats, "MEMORYSTATS" },
#endif
  { 0,              std_min, "MIN" },
#ifdef OLD_REGINA_FEATURES
  { EXT_OPEN_BIF,   unx_open, "OPEN" },
#endif /* OLD_REGINA_FEATURES */
#ifdef AREXXIO
  { 0,              arexx_open, "OPEN" },
#endif
  { 0,              std_overlay, "OVERLAY" },
  { 0,              unx_popen, "POPEN" },
  { 0,              std_pos, "POS" },
  { 0,              std_qualify, "QUALIFY" },
  { 0,              std_queued, "QUEUED" },
  { 0,              std_random, "RANDOM" },
  { 0,              arexx_randu, "RANDU" },
#ifdef AREXXIO
  { 0,              arexx_readch, "READCH" },
  { 0,              arexx_readln, "READLN" },
#endif
#if defined(_AMIGA) || defined(__AROS__)
  { 0,              arexx_remlib, "REMLIB" },
#endif
  { 0,              std_reverse, "REVERSE" },
  { 0,              std_right, "RIGHT" },

  { 0,              rex_rxfuncadd, "RXFUNCADD" },
  { 0,              rex_rxfuncdrop, "RXFUNCDROP" },
  { 0,              rex_rxfuncerrmsg, "RXFUNCERRMSG" },
  { 0,              rex_rxfuncquery, "RXFUNCQUERY" },
  { 0,              rex_rxqueue, "RXQUEUE" },

#ifdef AREXXIO
  { 0,              arexx_seek, "SEEK" },
#endif
#if defined(_AMIGA) || defined(__AROS__)
  { 0,              arexx_setclip, "SETCLIP" },
#endif
  { 0,              std_sign, "SIGN" },
  { 0,              cms_sleep, "SLEEP" },
  { 0,              std_sourceline, "SOURCELINE" },
  { 0,              std_space, "SPACE" },
  { 0,              cms_state, "STATE" },
  { 0,              std_stream, "STREAM" },
  { 0,              std_strip, "STRIP" },
  { 0,              std_substr, "SUBSTR" },
  { 0,              std_subword, "SUBWORD" },
  { 0,              std_symbol, "SYMBOL" },
  { 0,              std_time, "TIME" },
  { 0,              std_trace, "TRACE" },
#ifndef NDEBUG
  { 0,              dbg_traceback, "TRACEBACK" },
#endif
  { 0,              std_translate, "TRANSLATE" },
  { 0,              arexx_trim, "TRIM" },
  { 0,              std_trunc, "TRUNC" },
  { 0,              unx_uname, "UNAME" },
  { 0,              unx_unixerror, "UNIXERROR" },
  { 0,              arexx_upper, "UPPER" },
  { 0,              rex_userid, "USERID" },
  { 0,              std_value, "VALUE" },
  { 0,              std_verify, "VERIFY" },
  { 0,              std_word, "WORD" },
  { 0,              std_wordindex, "WORDINDEX" },
  { 0,              std_wordlength, "WORDLENGTH" },
  { 0,              std_wordpos, "WORDPOS" },
  { 0,              std_words, "WORDS" },
#ifdef AREXXIO
  { 0,              arexx_writech, "WRITECH" },
  { 0,              arexx_writeln, "WRITELN" },
#endif
  { 0,              std_x2b, "X2B" },
  { 0,              std_x2c, "X2C" },
  { 0,              std_x2d, "X2D" },
  { 0,              std_xrange, "XRANGE" },
  { 0,              NULL, NULL }
} ;

static const int num_funcs = sizeof(functions) / (sizeof(functions[0])) - 1 ;

static const int MonthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
static const int DaysInYear[] = {0,31,59,90,120,151,181,212,243,273,304,334};

static int leapyear(long year) ;
static int base2date(long basedate,void *conv_tmdata) ;

#ifdef TRACEMEM
void mark_listleaked_params( const tsd_t *TSD )
{
   paramboxptr pptr=NULL ;

   for (pptr=TSD->listleaked_params; pptr; pptr=pptr->next)
   {
      markmemory( pptr, TRC_PROCARG ) ;
      if (pptr->value)
         markmemory( pptr->value, TRC_PROCARG ) ;
   }
}
#endif

streng *buildtinfunc( tsd_t *TSD, nodeptr this )
{
   int low=0, topp=0, mid=0, end=1, up=num_funcs-1, i=0 ;
   streng *ptr=NULL ;
   int ext=0 ;
   void *vptr=NULL ;
   streng *(*func)(tsd_t *,cparamboxptr)=NULL ;
   streng *upper_name;

   upper_name=Str_upper(Str_dupTSD(this->name));
   /*
    * Look for a function registered in a DLL
    */
   vptr = loaded_lib_func( TSD, upper_name ) ;
   if (vptr)
      func = std_center ; /* e.g. */

   Free_stringTSD( upper_name );

   /*
    * If no function registered in a DLL, look for one in the
    * current EXE
    */
   if (!func)
   {
      ext = external_func( TSD, this->name ) ;
      if (ext)
         func = std_center ; /* e.g. */
   }

   /*
    * If no function registered in a DLL or EXE, look for a builtin
    */
   if (!func)
   {
      topp = Str_len( this->name ) ;

      if (this->u.func)
         func = this->u.func ;
      else
      {
         mid = 0 ;  /* to keep the compiler happy */
         while ((end)&&(up>=low))
         {
            mid = (up+low)/2 ;
            for (i=0; i<topp; i++ )
               if (this->name->value[i] != functions[mid].funcname[i])
                  break ;

            if (i==topp)
               end = (functions[mid].funcname[i]!=0x00) ;
            else
               end = ( functions[mid].funcname[i] - this->name->value[i] ) ;

            if (end>0)
               up = mid-1 ;
            else
               low = mid+1 ;
         }
         if (!end)
         {
#if OLD_OPTIONS
            if (functions[mid].length)
            {
               int obyte = functions[mid].length/(sizeof(unsigned char)*8) ;
               int obit = functions[mid].length%(sizeof(unsigned char)*8) ;

               if (!(TSD->currlevel->u.flags[obyte] & (1<<(7-obit))))
                  func = NULL ;
               else
               {
                  func = functions[mid].function ;
                  if (TSD->currlevel->u.options.cacheext)
                     this->u.func = func ;
               }
            }
#else
            if (functions[mid].length)
            {
               if ( ! get_options_flag( TSD->currlevel, functions[mid].length ) )
                  func = NULL ;
               else
               {
                  func = functions[mid].function ;
                  if ( get_options_flag( TSD->currlevel, EXT_CACHEEXT ) )
                     this->u.func = func ;
               }
            }
#endif
            else
               this->u.func = func = functions[mid].function ;
         }
      }
   }

   if (func)
   {
      if (TSD->bif_first)
         deallocplink( TSD, TSD->bif_first ) ;
      TSD->bif_first = NULL; /* NEVER delete this! initplist
                     * may setjmp to the line above
                     * which results to a twice-called
                     * deallocplink. FGC
                     */

      TSD->bif_first = initplist( TSD, this ) ;
      if (ext)
         ptr = do_an_external_exe( TSD, this->name, TSD->bif_first, 0, (char) this->called) ;
      else if (vptr)
         ptr = do_an_external_dll( TSD, vptr, TSD->bif_first, (char) this->called ) ;
      else
         ptr = (*func)(TSD, TSD->bif_first /* ->next */ ) ;

      deallocplink( TSD, TSD->bif_first ) ;
      TSD->bif_first = NULL ;
      return ptr ;
   }
   else
   {
#if defined(_AMIGA) || defined(__AROS__)
      /* Function was not found: so on amiga/AROS try the function
       * libraries and hosts
       */
      if (TSD->bif_first)
	 deallocplink( TSD, TSD->bif_first ) ;
      TSD->bif_first = NULL; /* NEVER delete this! initplist
			      * may setjmp to the line above
			      * which results to a twice-called
			      * deallocplink. FGC
			      */

      TSD->bif_first = initplist( TSD, this );
      ptr = try_func_amiga( TSD, this->name, TSD->bif_first, (char) this->called );
      deallocplink( TSD, TSD->bif_first );
      
      if (ptr)
	 return ptr;
#endif

      if (IfcHaveFunctionExit( TSD )) /* we have an exit handler */
      {
         if (TSD->bif_first)
            deallocplink( TSD, TSD->bif_first ) ;
         TSD->bif_first = NULL; /* NEVER delete this! initplist
                                                 * may setjmp to the line above
                                                 * which results to a twice-called
                                                 * deallocplink. FGC
                                                 */


         TSD->bif_first = initplist( TSD, this ) ;
         ptr = do_an_external_exe( TSD, this->name, TSD->bif_first, 1, (char) this->called ) ;
         deallocplink( TSD, TSD->bif_first ) ;
         TSD->bif_first = NULL ;
      }
      else
         return NOFUNC ;
   }
   /* add return to keep compiler happy. won't ever reach here */
   return NULL;
}

paramboxptr initplist( tsd_t *TSD, cnodeptr this )
{
   paramboxptr first=NULL, new=NULL, currnt=NULL ;
   streng *pptr=NULL ;
   streng *junk=NULL ;

   first = currnt = NULL ;
   for (this=this->p[0]; this; this=this->p[1])
   {
      if (TSD->par_stack)
      {
         new = TSD->par_stack ;
         TSD->par_stack = new->next ;
      }
      else
         new = MallocTSD( sizeof( parambox )) ;

      if (!first)
         first = currnt = new ;
      else
      {
         currnt->next = new ;
         currnt = new ;
      }

      if (this->type==X_CEXPRLIST && TSD->trace_stat!='I')
      {
         if (this->u.strng)
            pptr = this->u.strng ;
         else
            pptr = NULL ;

         currnt->dealloc = 0 ;
      }
      else
      {
         currnt->dealloc = 1 ;
         if (this->p[0])
            pptr = evaluate( TSD, this->p[0], &junk ) ;
         else
            pptr = NULL ;

         if ( get_options_flag( TSD->currlevel, EXT_PGB_PATCH1 ) ) /* pgb */
         {
            /*
            * This code "removed" 16/04/99 byMH, based on suggestion from
            * Patrick McPhee.  The case that FGC states as a potential
            * error, does not cause a problem by removing this code.
            */
           /*
            * Reinstated by FGC 25/11/99 to fix bug 35
            */
           /*
            * FGC: HELP! I really don't know if this will work. The problem
            * is:
            * a) is pptr a "new" tree with "new" values ? In this case the
            *    Str_dupTSD() is wrong. What happens in this case if we don't
            *    delete it later (dealloc=0) ?
            * b) junk SEEMS to me as the same as pptr if newly allocated
            * (see expr.c/evaluate). Why, in this case does it crash ?
            */
            if (!junk)
            {
               if ( pptr != NULL )
                  pptr = Str_dupTSD( pptr );
            }
            /*
             * Currently only used in dealloc_plink() some lines below.
             * I think:
             *  !junk -->no newly allocated
             *  values --> not allowed to delete them
             * BUT, WHAT HAPPENS IN CASE OF junk==NULL? SHALL WE Str_dupTSD pptr ?
             * THERE ARE GOOD REASONS TO DO IT.  TRY THIS FIRST IN CASE OF
             * STRANGE RESULTS/CRASHES WITH ARGUMENTS.
             */
            else
            {
            /*
             * MH - 3-May-2000
             * added:
                  if ( pptr )
                     currnt->dealloc = 1 ;
                  else
             * to remove memory leak as reported by PJM - bug 20000502-55023
             * seems to be stable
             */
               if ( pptr )
                  currnt->dealloc = 1 ;
               else
                  currnt->dealloc = 0 ;
            }
         }
         else
         {
            /*
             * FGC: Following code will cause a crash if a variable x is
             * passed to a function which uses both arg(1) and x: arg(1)
             * is a reference to x. Modifying x causes a deletion of arg(1)
             */
            if (!junk)
   /*            pptr = Str_dupTSD( pptr ) ; */
               currnt->dealloc = 0 ;
         }

      }

      currnt->value = pptr ;
   }
#ifdef TRACEMEM
   TSD->listleaked_params = first ;
#endif
   currnt->next = NULL ;
   return first ;
}


void deallocplink( tsd_t *TSD, paramboxptr first )
{
   paramboxptr this=NULL ;

   for (;first;)
   {
      this = first ;
      first = first->next ;
      if (this->dealloc && this->value)
      {
         Free_stringTSD( this->value ) ;
         this->value = NULL;
      }

#if defined(CHECK_MEMORY)
      FreeTSD(this);
#else
      /* Back to the freed-parbox stack: */
      this->next = TSD->par_stack ;
      TSD->par_stack = this ;
#endif
   }
}


#ifdef TRACEMEM
void mark_param_cache( const tsd_t *TSD )
{
   paramboxptr ptr=NULL ;

   ptr = TSD->par_stack ;
   for (; ptr; ptr=ptr->next )
      markmemory( ptr, TRC_P_CACHE ) ;
}
#endif



int myatol( const tsd_t *TSD, const streng *text )
{
   int num, error ;

   num = streng_to_int( TSD, text, &error ) ;
   if (error)
       exiterror( ERR_INVALID_INTEGER, 0 )  ;

   return num ;
}

static int myintatol( tsd_t *TSD, const streng *text, int suberr, const char *bif, int argnum )
{
   int num, error ;

   num = streng_to_int( TSD, text, &error ) ;
   if ( error )
      exiterror( ERR_INCORRECT_CALL, suberr, bif, argnum, tmpstr_of( TSD, text ) ) ;

   return num ;
}


int atozpos( tsd_t *TSD, const streng *text, const char *bif, int argnum )
{
   int result=0 ;

   if ( ( result = myintatol( TSD, text, 13, bif, argnum ) ) < 0 )
      exiterror( ERR_INCORRECT_CALL, 13, bif, argnum, tmpstr_of( TSD, text ) )  ;

   return result ;
}


char getoptionchar( tsd_t *TSD, const streng *text, const char* bif, int argnum, const char *choices )
{
   char ch=0 ;
   const char *ptr = NULL;

   if (text->len == 0)
      exiterror( ERR_INCORRECT_CALL, 21, bif, argnum )  ;

   ch = (char) toupper( text->value[0] ) ;

   for ( ptr = choices; *ptr; ptr++ )
   {
      if ( *ptr == ch )
         return ch ;
   }
   exiterror( ERR_INCORRECT_CALL, 28, bif, argnum, choices, tmpstr_of( TSD, text ) );
   return 0 ;
}


char getonechar( tsd_t *TSD, const streng *text, const char *bif, int argnum )
{
   if ( !text )
      exiterror( ERR_INCORRECT_CALL, 23, bif, argnum, "" )  ;
   if ( Str_len( text ) != 1 )
      exiterror( ERR_INCORRECT_CALL, 23, bif, argnum, tmpstr_of( TSD, text ) ) ;

   return text->value[0] ;
}

int atopos( tsd_t *TSD, const streng *text, const char *bif, int argnum )
{
   int result=0 ;

   if ( ( result = myintatol( TSD, text, 14, bif, argnum ) ) <= 0 )
      exiterror( ERR_INCORRECT_CALL, 14, bif, argnum, tmpstr_of( TSD, text ) ) ;

   return result ;
}

int atoposorzero( tsd_t *TSD, const streng *text, const char *bif, int argnum )
{
   int result=0 ;

   if ( ( result = myintatol( TSD, text, 11, bif, argnum ) ) < 0 )
      exiterror( ERR_INCORRECT_CALL, 11, bif, argnum, tmpstr_of( TSD, text ) ) ;

   return result ;
}



streng *int_to_streng( const tsd_t *TSD, int input )
{
   streng *output=NULL ;
   char *cptr=NULL, *start=NULL, *top=NULL ;

   output = Str_makeTSD( sizeof(int)*3 + 2 ) ;
   start = output->value ;
   cptr = start + sizeof(int)*3 + 2 ;
   if (input)
   {
      if (input<0)
      {
         input = - input ;
         *(start++) = '-' ;
      }

      for (top=cptr;input;)
      {
         *(--cptr) = (char) (input % 10 + '0') ;
         input = input / 10 ;
      }

      memmove( start, cptr, top-cptr ) ;
      output->len = top-cptr + start-output->value ;
   }
   else
   {
      *start = '0' ;
      output->len = 1 ;
   }

   return output ;
}


void checkparam( cparamboxptr params, int min, int max, const char *name )
{
   int i=0 ;

   for (i=0;i<min;i++,params=(cparamboxptr) (params->next))
      if ((!params)||(!params->value))
         exiterror( ERR_INCORRECT_CALL, 3, name, min )  ;

   for (;(i<max)&&(params);i++,params=(cparamboxptr) (params->next)) ;
   if (((i==max)&&(params))&&((max)||(params->value)))
      exiterror( ERR_INCORRECT_CALL, 4, name, max )  ;
}


const streng *param( cparamboxptr ptr, int num )
{
   int i=0 ;
   for (i=1;i<num;i++,ptr=ptr->next)
      if (!ptr)
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__ )  ;

   return ((ptr)&&(ptr->value)) ? ptr->value : NULL ;
}

/*
 * These functions are rather ugly, but they works :-)
 */
/*
 * Converts a date supplied in an external format (specified by suppformat)
 * into a struct tm (individual values for year, month, day, year_days and
 * base days).
 */
int convert_date(const streng *suppdate, char suppformat, struct tm *indate)
{
   int rc=0,i=0,off=0;
   long num1=0,num2=0,num3=0;
   char buf[20];
   const char *ptr=suppdate->value;

   switch(suppformat)
   {
      case 'B':
         if (suppdate->len > 19)
            return(1);
         memcpy(buf,ptr,suppdate->len);
         buf[suppdate->len] = '\0';
         if ((num1 = atol(buf)) == 0)
            for (i=0;i<suppdate->len;i++)
               if (buf[i] != '0')
                  return(1);
         base2date(num1,indate);
         break;
      case 'D':
         if (suppdate->len > 19)
            return(1);
         memcpy(buf,ptr,suppdate->len);
         buf[suppdate->len] = '\0';
         if ((num1 = atol(buf)) == 0)
            for (i=0;i<suppdate->len;i++)
               if (buf[i] != '0')
                  return(1);
         base2date(num1+basedays(indate->tm_year)-1,indate);
         break;
      case 'I':
         if (suppdate->len > 19)
            return(1);
         memcpy(buf,ptr,suppdate->len);
         buf[suppdate->len] = '\0';
         if ((num1 = atol(buf)) == 0)
            for (i=0;i<suppdate->len;i++)
	       if (buf[i] != '0')
	          return(1);
         base2date(num1+basedays(1978)-1,indate);
         break;
      case 'E':
      case 'O':
      case 'U':
         if (suppdate->len != 8)
            return(1);
         if (*(ptr+2) != '/' && *(ptr+5) != '/')
              return( 1 );
         memcpy(buf,ptr,2);
         buf[2] = '\0';
         if ( !isdigit( buf[0] ) || !isdigit( buf[1] ) )
            return( 1 );
         num1 = atol( buf );
         memcpy(buf,(ptr+3),2);
         buf[2] = '\0';
         if ( !isdigit( buf[0] ) || !isdigit( buf[1] ) )
            return( 1 );
         num2 = atol( buf );
         memcpy(buf,(ptr+6),2);
         buf[2] = '\0';
         if ( !isdigit( buf[0] ) || !isdigit( buf[1] ) )
            return( 1 );
         num3 = atol( buf );
         switch(suppformat)
         {
            case 'E':
               if ( num1 == 0 || num2 == 0 )
                  return( 1 );
               indate->tm_mday = num1;
               indate->tm_mon = num2-1;
               indate->tm_year = num3;
               break;
            case 'O':
               if ( num3 == 0 || num2 == 0 )
                  return( 1 );
               indate->tm_mday = num3;
               indate->tm_mon = num2-1;
               indate->tm_year = num1;
               break;
            case 'U':
               if ( num2 == 0 || num1 == 0 )
                  return( 1 );
               indate->tm_mday = num2;
               indate->tm_mon = num1-1;
               indate->tm_year = num3;
               break;
         }
         if (indate->tm_year < 100)   /* do something with century ... */
            indate->tm_year += (indate->tm_year < 50) ? 2000 : 1900;
         break;
      case 'N':
         if (suppdate->len != 11 && suppdate->len != 10)
            return(1);
         if (suppdate->len == 10)
            off = 1;
         if (*(ptr+2-off) != ' ' && *(ptr+6-off) != ' ')
            return(1);
         memcpy(buf,ptr,2-off);
         buf[2-off] = '\0';
         if ((num1 = atol(buf)) == 0)
            return(1);
         memcpy(buf,(ptr+3-off),3);
         buf[3] = '\0';
         /* find month */
         num2 = (-1);
         for (i=0;i<12;i++)
         {
            if (strncmp(months[i],buf,3) == 0)
            {
               num2 = i;
               break;
            }
         }
         if (num2 == (-1))
            return(1);
         memcpy(buf,(ptr+7-off),4);
         buf[4] = '\0';
         if ((num3 = atol(buf)) == 0 && strcmp("0000",buf) != 0)
            return(1);
         indate->tm_mday = num1;
         indate->tm_mon = num2;
         indate->tm_year = num3;
         break;
      case 'S':
         if (suppdate->len != 8)
            return(1);
         memcpy(buf,ptr,4);
         buf[4] = '\0';
         if ((num1 = atol(buf)) == 0)
            return(1);
         memcpy(buf,(ptr+4),2);
         buf[2] = '\0';
         if ((num2 = atol(buf)) == 0)
            return(1);
         memcpy(buf,(ptr+6),2);
         buf[2] = '\0';
         if ((num3 = atol(buf)) == 0)
            return(1);
         indate->tm_mday = num3;
         indate->tm_mon = num2-1;
         indate->tm_year = num1;
         break;
      default:
         /* should not get here */
         break;
   }
   if (indate->tm_mday > MonthDays[indate->tm_mon] + leapyear(indate->tm_year)
   ||  indate->tm_mday < 1
   ||  indate->tm_mon > 11
   ||  indate->tm_mon < 0
   ||  indate->tm_year < 0)
      return(1);

   indate->tm_yday = DaysInYear[indate->tm_mon]+
                     ((leapyear(indate->tm_year)&&indate->tm_mon>1)?1:0)+
                     indate->tm_mday - 1;
   indate->tm_wday = (((indate->tm_yday+basedays(indate->tm_year))+8) % 7);
   return(rc);
}

/*
 * Converts a year (MUST have century) to a number of days
 * Base year is 0001 - hence the date 01 Jan 0001 is base day 1
 */
int basedays(int year)
{
   return((year-1)*365 + (year-1)/4 - (year-1)/100 + (year-1)/400);
}

/*
 * Determines if a year (MUST have a century) is a leap year
 */
static int leapyear(long year)
{
   if ((year%4 == 0 && year%100 != 0) || year%400 == 0)
      return(1);
   else
      return(0);
}

/*
 * Converts a number (representing the number of days since 01 Jan 0001)
 * to a struct tm value (individual fields for year, month, day and days in year)
 */
static int base2date(long basedate,void *conv_tmdata)
{
   struct tm *outdate=(struct tm *)conv_tmdata;
   int i=0;
   long day=0L,year=0L,month=0L,yeardays=0L,thismonth=0L;

   day = basedate + 1L;
   year = day / 366;
   day -= ((year*365) + (year/4) - (year/100) + (year/400));
   year++;
   while (day > (365 + leapyear(year)))
   {
      day -= (365 + leapyear(year));
      year++;
   }

   yeardays = day;
   for (i=0;i<11;i++)
   {
      thismonth = (MonthDays[i]) + ((i == 1) ? leapyear(year) : 0);
      if (day <= thismonth)
         break;
      day -= thismonth;
   }
   month = i;
   outdate->tm_year = year;
   outdate->tm_mon = month;
   outdate->tm_mday = day;
   outdate->tm_yday = yeardays;
   outdate->tm_wday = ((basedate+8) % 7);
   return(0);
}

/*
 * Converts a time supplied in an external format (specified by suppformat)
 * into a struct tm (individual values for hour, minute, second).
 */
int convert_time( const tsd_t *TSD, const streng *supptime, char suppformat, struct tm *intime, time_t *unow)
{
   int rc=0;
   long num1=0,num2=0,num3=0,num4=0;
   char buf[20];
   const char *ptr=supptime->value;

   switch(suppformat)
   {
      case 'C':
         if (*(ptr+2) != ':')
            return(1);
         if (memcmp("am",ptr+5,2) != 0 && memcmp("pm",ptr+5,2) != 0)
            return(1);
         memcpy(buf,ptr,2);
         buf[2] = '\0';
         if ((num1 = atol(buf)) == 0 && strcmp("00",buf) != 0)
            return(1);
         if (num1 > 12)
            return(1);
         memcpy(buf,ptr+3,2);
         buf[2] = '\0';
         if ((num2 = atol(buf)) == 0 && strcmp("00",buf) != 0)
            return(1);
         if (num2 > 59)
            return(1);
         intime->tm_sec = 0;
         if (memcmp("am",ptr+5,2)==0)
         {
            if (num1 == 12)
               intime->tm_hour = 0;
            else
               intime->tm_hour = num1;
         }
         else
         {
            if (num1 == 12)
               intime->tm_hour = num1;
            else
               intime->tm_hour = num1+12;
         }
         intime->tm_min = num2;
         *unow = 0;
         break;
      case 'H':
      case 'M':
      case 'S':
         /*
          * Convert supptime to whole number using streng_to_int()
          * rather than atoi(). Bug #20000922-78622
          */
         num1 = streng_to_int( TSD, supptime, &rc );
         if ( rc ) return 1;
         switch(suppformat)
         {
            case 'H':
               intime->tm_hour = num1;
               intime->tm_min = intime->tm_sec = 0;
               break;
            case 'M':
               intime->tm_hour = num1 / 60;
               intime->tm_min = num1 % 60;
               intime->tm_sec = 0;
               break;
            case 'S':
               intime->tm_hour = num1 / 3600;
               intime->tm_min = (num1 % 3600) / 60;
               intime->tm_sec = (num1 % 3600) % 60;
               break;
         }
         if (intime->tm_sec > 59 || intime->tm_hour > 23 || intime->tm_min > 59)
            return(1);
         *unow = 0;
         break;
      case 'L':
      case 'N':
         if (suppformat == 'N' && supptime->len != 8)
            return(1);
         if (suppformat == 'L' && supptime->len != 15)
            return(1);
         if (*(ptr+2) != ':' && *(ptr+5) != ':')
            return(1);
         memcpy(buf,ptr,2);
         buf[2] = '\0';
         if ((num1 = atol(buf)) == 0 && strcmp("00",buf) != 0)
            return(1);
         if (num1 < 0 || num1 > 23)
            return(1);

         memcpy(buf,ptr+3,2);
         buf[2] = '\0';
         if ((num2 = atol(buf)) == 0 && strcmp("00",buf) != 0)
            return(1);
         if (num2 < 0 || num2 > 59)
            return(1);

         memcpy(buf,ptr+6,2);
         buf[2] = '\0';
         if ((num3 = atol(buf)) == 0 && strcmp("00",buf) != 0)
            return(1);
         if (num3 < 0 || num3 > 59)
            return(1);
         intime->tm_sec = num3;
         intime->tm_hour = num1;
         intime->tm_min = num2;
         if (suppformat == 'N')
         {
            *unow = 0;
            break;
         }
         if (*(ptr+8) != '.')
            return(1);
         memcpy(buf,ptr+9,6);
         buf[6] = '\0';
         if ((num4 = atol(buf)) == 0 && strcmp("000000",buf) != 0)
            return(1);
         if (num4 < 0)
            return(1);
         *unow = num4;
         break;
      default:
         /* should not get here */
         break;
   }

   return(rc);
}

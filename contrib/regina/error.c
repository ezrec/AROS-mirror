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
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>


int lineno_of( cnodeptr node )
{
   if (node)
      return (node->lineno>=0) ? node->lineno : 0 ;
   else
      return 0 ;
}

static int charno_of( cnodeptr node )
{
   if (node)
      return (node->charnr>=0) ? node->charnr : 0 ;
   else
      return 0 ;
}


/* only to be used by syntax and runtime errors, and the halt condition
 * FIXME: FGC: This function is used while initializing the runtime system.
 *             Maybe, we don't have a functional tsd_t!
 *             What shall we do?
 */
void exiterror( int errorno, int suberrorno, ... )
{
   va_list argptr;
   int lineno=0, charno=0, signtype=0 ;
   streng *inputfile=NULL ;
   streng *suberror_streng=NULL;
   static const char err1[]="Error %d running \"%.*s\", line %d: %s" ;
   static const char err2[]="Error %d running \"%.*s\": %s" ;
   streng *errmsg=NULL ;
   int ok=0 ;
   const char *fmt = NULL, *etext ;
   FILE *fp = stderr ;
   tsd_t *TSD = __regina_get_tsd(); /* The TSD should be fetched directly. This
                                     * will help if someone corrupted a TSD as
                                     * an argument.
                                     * A "fresh" value is always better for
                                     * tracking down ugly errors.
                                     * Speed advantage is no reason here! */

   if (TSD->currentnode)
   {
      lineno = lineno_of( TSD->currentnode ) ;
      charno = charno_of( TSD->currentnode ) ;
   }
   else
   {
      charno =  0 ;
      lineno = parser_data.tline ;
   }

   signtype = SIGNAL_SYNTAX ;
   if ( errorno==ERR_PROG_INTERRUPT )
      signtype = SIGNAL_HALT ;
#ifdef HAVE_VSPRINTF
   /*
    * Expanded the sub-error text and pass this to condition_hook for
    * condition('D') to return the expanded string.
    */
   if (errorno <= 100
   &&  errorno > 0
   &&  suberrorno != 0)
   {
      fmt = suberrortext( errorno, suberrorno );
      if ( fmt != NULL )
      {
         va_start( argptr, suberrorno );
         suberror_streng = Str_makeTSD( 1024 );
         /*
          * Saved the errno and suberrno so that later when
          * a request for the expanded message is made, it
          * can be matched against the errorno and suberrno.
          */
         if (suberror_streng)
         {
            vsprintf( suberror_streng->value, fmt, argptr );
            suberror_streng->len = strlen(suberror_streng->value);
         }
         va_end( argptr );
      }
   }
#endif

   /* Here we should set sigtype to SIGNAL_FATAL for some 'errno's */

   /* enable a hook into the condition system */
   if (condition_hook( TSD, signtype, errorno, suberrorno, lineno, Str_creTSD(errortext(errorno)), suberror_streng))
   {
      if (suberror_streng)
         Free_stringTSD(suberror_streng);
      suberror_streng = NULL;
      return ; /* if CALL ON */
   }

   inputfile = TSD->systeminfo->input_file ;
   etext = errortext( errorno ) ;
   errmsg = Str_makeTSD( 80 + strlen( etext ) + Str_len( inputfile ) ) ;
   ok = HOOK_GO_ON ;
   if (lineno>0)
   {
      traceback(TSD) ;
      sprintf( errmsg->value, err1, errorno, Str_len( inputfile ), inputfile->value, lineno, etext ) ;
   }
   else
      sprintf(errmsg->value, err2, errorno, Str_len( inputfile ), inputfile->value, etext);

   errmsg->len = strlen( errmsg->value ) ;
   assert( errmsg->len < errmsg->max ) ;
   if (TSD->systeminfo->hooks & HOOK_MASK(HOOK_STDERR))
      ok = (hookup_output(TSD, HOOK_STDERR, errmsg) == HOOK_GO_ON) ;

   if ( get_options_flag( TSD->currlevel, EXT_STDOUT_FOR_STDERR ) )
      fp = stdout ;
   if (ok==HOOK_GO_ON)
   {
      fwrite( errmsg->value, Str_len(errmsg), 1, fp ) ;
#if defined(DOS) || defined(OS2) || defined(WIN32)
      /*
       * stdout is open in binary mode, so we need to add the
       * extra CR to the end of the line.
       */
      fputc( REGINA_CR, fp ) ;
#endif
      fputc( REGINA_EOL, fp ) ;
      /*
       * Display the sub-error text
       */
      if (errorno <= 100
      &&  errorno > 0
      &&  suberrorno != 0
      &&  suberror_streng)
      {
         fprintf( fp, "Error %d.%d: %s", errorno, suberrorno, suberror_streng->value );
#if defined(DOS) || defined(OS2) || defined(WIN32)
         /*
          * stdout is open in binary mode, so we need to add the
          * extra CR to the end of the line.
          */
         fputc( REGINA_CR, fp ) ;
#endif
         fputc( REGINA_EOL, fp ) ;
      }
      fflush( fp );
   }
   if (suberror_streng)
      Free_stringTSD(suberror_streng);

#ifndef NDEBUG
   if (errorno == ERR_INTERPRETER_FAILURE)
      abort() ;
#endif

   Free_stringTSD( errmsg ) ;
   if (TSD->systeminfo->panic)
   {
      TSD->systeminfo->result = NULL ;
      if (TSD->in_protected)
      {
         TSD->delayed_error_type = PROTECTED_DelayedSetjmpPanic;
         longjmp( TSD->protect_return, 1 ) ;
      }
      longjmp( *(TSD->systeminfo->panic), 1 ) ;
   }
   CloseOpenFiles( TSD );

   if (TSD->in_protected)
   {
#ifdef VMS
      TSD->expected_exit_error = EXIT_SUCCESS;
#else
      TSD->expected_exit_error = errorno;
#endif
      TSD->delayed_error_type = PROTECTED_DelayedExit;
      longjmp( TSD->protect_return, 1 ) ;
   }
#ifdef VMS
   TSD->MTExit( EXIT_SUCCESS ) ;
#else
   TSD->MTExit( errorno ) ;
#endif
}

/* This function is called by the parser (syntactical interpreter) if an error
 * occurs.
 */
void __reginaerror(char *dummy)
{
   /* We ignore the message although it may contain useful informations. */
   dummy = dummy; /* keep compiler happy */
   return ;
}


const char *errortext( int errorno )
{
   static const char *errmsg[] = {
/*  0 */     "" ,
/*  1 */     "" ,
/*  2 */     "Failure during finalization" ,
/*  3 */     "Failure during initialization" ,
/*  4 */     "Program interrupted" ,
/*  5 */     ERR_STORAGE_EXHAUSTED_TMPL,
/*  6 */     "Unmatched \"/*\" or quote" ,
/*  7 */     "WHEN or OTHERWISE expected" ,
/*  8 */     "Unexpected THEN or ELSE" ,
/*  9 */     "Unexpected WHEN or OTHERWISE" ,
/* 10 */     "Unexpected or unmatched END" ,
/* 11 */     "-Control stack full" ,
/* 12 */     "-Clause > 1024 characters" ,
/* 13 */     "Invalid character in program" ,
/* 14 */     "Incomplete DO/SELECT/IF" ,
/* 15 */     "Invalid hexadecimal or binary string" ,
/* 16 */     "Label not found" ,
/* 17 */     "Unexpected PROCEDURE" ,
/* 18 */     "THEN expected" ,
/* 19 */     "String or symbol expected" ,
/* 20 */     "Name expected" ,
/* 21 */     "Invalid data on end of clause" ,
/* 22 */     "Invalid character string" ,
/* 23 */     "Invalid data string" ,
/* 24 */     "Invalid TRACE request" ,
/* 25 */     "Invalid sub-keyword found" ,
/* 26 */     "Invalid whole number" ,
/* 27 */     "Invalid DO syntax" ,
/* 28 */     "Invalid LEAVE or ITERATE" ,
/* 29 */     "Environment name too long" ,
/* 30 */     "Name or string too long" ,
/* 31 */     "Name starts with number or \".\"" ,
/* 32 */     "-Invalid use of stem" ,
/* 33 */     "Invalid expression result" ,
/* 34 */     "Logical value not \"0\" or \"1\"",
/* 35 */     "Invalid expression" ,
/* 36 */     "Unmatched \"(\" in expression" ,
/* 37 */     "Unexpected \",\" or \")\"" ,
/* 38 */     "Invalid template or pattern" ,
/* 39 */     "-Evaluation stack overflow" ,
/* 40 */     "Incorrect call to routine" ,
/* 41 */     "Bad arithmetic conversion" ,
/* 42 */     "Arithmetic overflow/underflow" ,
/* 43 */     "Routine not found" ,
/* 44 */     "Function did not return data" ,
/* 45 */     "No data specified on function RETURN" ,
/* 46 */     "Invalid variable reference" ,
/* 47 */     "Unexpected label" ,
/* 48 */     "Failure in system service" ,
/* 49 */     "Interpreter Error",
/* 50 */     "Unrecognized reserved symbol",
/* 51 */     "Invalid function name",
/* 52 */     "",
/* 53 */     "Invalid option",
/* 54 */     "Invalid STEM value",
/* 55 */     "",
/* 56 */     "",
/* 57 */     "",
/* 58 */     "",
/* 59 */     "",
/* 60 */     "-Can't rewind transient file",
/* 61 */     "-Improper seek operation on file",
/* 62 */     "-Internal buffer too small",
/* 63 */     "",
/* 64 */     "-Syntax error while parsing",
/* 65 */     "",
/* 66 */     "",
/* 67 */     "",
/* 68 */     "",
/* 69 */     "",
/* 70 */     "",
/* 71 */     "",
/* 72 */     "",
/* 73 */     "",
/* 74 */     "",
/* 75 */     "",
/* 76 */     "",
/* 77 */     "",
/* 78 */     "",
/* 79 */     "",
/* 80 */     "",
/* 81 */     "",
/* 82 */     "",
/* 83 */     "",
/* 84 */     "",
/* 85 */     "",
/* 86 */     "",
/* 87 */     "",
/* 88 */     "",
/* 89 */     "",
/* 90 */     "",
/* 91 */     "",
/* 92 */     "",
/* 93 */     "Incorrect call to routine",
/* 94 */     "External queue interface error",
/* 95 */     "",
/* 96 */     "",
/* 97 */     "",
/* 98 */     "",
/* 99 */     "",
/* 100 */    "-Unknown filsystem error",
             "",
             } ;

   if ((errorno<=100)&&(errorno>0))
      return( errmsg[errorno] ) ;

   if (errorno>100)
      return( strerror(errorno-100) ) ;

   return ( "" ) ;
}

const char *suberrortext( int errorno, int suberrorno )
{
   typedef struct
   {
      unsigned int errorno ;
      unsigned int suberrorno ;
      char * suberrmsg ;
   } errormsg ;

   static const errormsg errmsg[] =
   {
      { 3,  1,   "Failure during initialization: %s" } ,
      { 6,  1,   "Unmatched comment delimiter (\"/*\")" } ,
      { 6,  2,   "Unmatched single quote (')" } ,
      { 6,  3,   "Unmatched double quote (\")" } ,
      { 8,  1,   "THEN has no corresponding IF or WHEN clause" } ,
      { 8,  2,   "ELSE has no corresponding THEN clause" } ,
      {10,  1,   "END has no corresponding DO or SELECT" } ,
      {13,  1,   "Invalid character in program \"%c\" ('%x'X)" } ,
      {14,  3,   "THEN requires a following instruction" } ,
      {14,  4,   "ELSE requires a following instruction" } ,
      {15,  1,   "Invalid location of blank in position %d in hexadecimal string" } ,
      {15,  2,   "Invalid location of blank in position %d in binary string" } ,
      {15,  3,   "Only 0-9, a-f, A-F, and blank are valid in hexadecimal string; found \"%s\"" } ,
      {15,  4,   "Only 0, 1, and blank are valid in binary string; found \"%s\"" } ,
      {16,  1,   "Label \"%s\" not found." } ,
      {20,  1,   "Name required; found \"%s\"" } ,
      {21,  1,   "The clause ended at an unexpected token; found \"%s\"" } ,
      {24,  1,   "TRACE request letter must be one of \"%s\"; found \"%c\"" } ,
      {25,  1,   "CALL ON must be followed by one of the keywords %s; found \"%s\"" } ,
      {25,  3,   "SIGNAL ON must be followed by one of the keywords %s; found \"%s\"" } ,
      {25, 11,   "NUMERIC FORM must be followed by one of the keywords %s; found \"%s\"" } ,
      {25, 12,   "PARSE must be followed by one of the keywords %s; found \"%s\"" } ,
      {25, 15,   "NUMERIC must be followed by one of the keywords %s; found \"%s\"" } ,
      {25, 17,   "PROCEDURE must be followed by one of the keywords %s; found \"%s\"" } ,
      {26,  5,   "NUMERIC DIGITS value must be zero or a positive whole number; found \"%s\"" } ,
      {26,  6,   "NUMERIC FUZZ value must be zero or a positive whole number; found \"%s\"" } ,
      {26,  7,   "Number used in TRACE setting must be a whole number; found; found \"%s\"" } ,
      {33,  1,   "Value of NUMERIC DIGITS \"%d\" must exceed value of NUMERIC FUZZ \"(%d)\"" } ,
      {33,  2,   "Value of NUMERIC DIGITS \"%d\" must not exceed %d" } ,
      {35,  1,   "Invalid expression detected at \"%s\"" } ,
      {38,  1,   "Invalid parsing template detected at \"%s\"" } ,
      {31,  2,   "Variable symbol must not start with a number; found \"%s\"" } ,
      {31,  3,   "Variable symbol must not start with a \".\"; found \"%s\"" } ,
      {38,  3,   "PARSE VALUE instruction requires WITH keyword" } ,
      {40,  3,   "Not enough arguments in invocation of \"%s\"; minimum expected is %d" } ,
      {40,  4,   "Too many arguments in invocation of \"%s\"; maximum expected is %d" } ,
      {40,  5,   "Missing argument in invocation of \"%s\"; argument %d is required" } ,
      {40, 11,   "%s argument %d must be a number; found \"%s\"" } ,
      {40, 13,   "%s argument %d must be zero or positive; found \"%s\"" } ,
      {40, 14,   "%s argument %d must be positive; found \"%s\"" } ,
      {40, 17,   "%s argument 1, must have an integer part in the range 0:90 and a decimal part no larger than .9; found \"%s\"" } ,
      {40, 19,   "%s argument 2, \"%s\", is not in the format described by argument 3, \"%s\"" } ,
      {40, 21,   "%s argument %d must not be null" } ,
      {40, 23,   "%s argument %d must be a single character; found \"%s\"" } ,
      {40, 27,   "%s argument 1, must be a valid stream name; found \"%s\"" } ,
      {40, 28,   "%s argument %d, option must start with one of \"%s\"; found \"%s\"" } ,
      {40, 32,   "%s the difference between argument 1 (\"%d\") and argument 2 (\"%d\") must not exceed 100000" } ,
      {40, 33,   "%s argument 1 (\"%d\") must be less than or equal to argument 2 (\"%d\")" } ,
      {40, 34,   "%s argument %d, (\"%d\") must be less than or equal to the number of lines in the program (%d)" } ,
      {40, 38,   "%s argument %d is not large enough to format \"%s\"" } ,
      {40, 42,   "%s argument 1; cannot position on this stream; found \"%s\"" } ,
      {40,914,   "%s argument %d, must be one of \"%s\"; found \"%s\"" } ,
      {40,920,   "%s: low-level stream I/O error; %s" } ,
      {40,921,   "%s argument %d, stream positioning mode \"%s\"; incompatible with stream open mode" } ,
      {40,922,   "%s argument %d, too few sub-commands; expecting %d; found %d" } ,
      {40,923,   "%s argument %d, too many sub-commands; expecting %d; found %d" } ,
      {40,924,   "%s argument %d, invalid positional specification; expecting one of \"%s\"; found \"%s\"" } ,
      {43,  1,   "Could not find routine \"%s\"" } ,
      {48,  1,   "Failure in system service: %s" } ,
      {49,  1,   "Interpreter failure in %s, line %d. Please report error." },
      {64,  1,   "Syntax error at line %d" } ,
      {64,  2,   "General syntax error at line %d, column %d" } ,
      {93,  1,   "STREAM command %s must be followed by one of \"%s\"; found \"%s\"" } ,
      {93,  2,   "%s argument %d must be 0 or 1; found \"%s\"" } ,
      {93,  3,   "STREAM command must be one of \"%s\"; found \"%s\"" } ,
      {94, ERR_RXSTACK_CANT_CONNECT  ,ERR_RXSTACK_CANT_CONNECT_TMPL    },
      {94, ERR_RXSTACK_NO_IP         ,ERR_RXSTACK_NO_IP_TMPL           },
      {94, ERR_RXSTACK_INVALID_SERVER,ERR_RXSTACK_INVALID_SERVER_TMPL  },
      {94, ERR_RXSTACK_INVALID_QUEUE ,ERR_RXSTACK_INVALID_QUEUE_TMPL   },
      {94, ERR_RXSTACK_NO_WINSOCK    ,ERR_RXSTACK_NO_WINSOCK_TMPL      },
      {94, ERR_RXSTACK_READING_SOCKET,ERR_RXSTACK_READING_SOCKET_TMPL  },
      {94, ERR_RXSTACK_INTERNAL      ,ERR_RXSTACK_INTERNAL_TMPL        },
      {94, ERR_RXSTACK_GENERAL       ,ERR_RXSTACK_GENERAL_TMPL         },
   } ;
   register int i = 0 ;
   int num_errmsg = sizeof(errmsg)/sizeof(errormsg);

   for (i=0; i < num_errmsg; i++ )
   {
      if (errmsg[i].errorno == (unsigned) errorno
      &&  errmsg[i].suberrorno == (unsigned) suberrorno)
         return( errmsg[i].suberrmsg ) ;
   }
   return NULL;
}

#ifndef NDEBUG

const char *getsym( int numb )
{
   char *symb=NULL ;

   switch (numb)
   {

      case X_NULL: symb="Null statement" ; break ;
      case X_PROGRAM: symb="Program" ; break ;
      case X_STATS: symb="Statements" ; break ;
      case X_COMMAND: symb="External command" ; break ;
      case X_ADDR_V: symb="ADDRESS (value) statement" ; break ;
      case X_ADDR_W: symb="ADDRESS (value) WITH statement" ; break ;
      case X_ADDR_N: symb="ADDRESS (normal) statement" ; break ;
      case X_ARG: symb="ARG statement" ; break ;
      case X_CALL: symb="CALL statement" ; break ;
      case X_DO: symb="DO statement" ; break ;
      case X_REP: symb="Repetitor in DO" ; break ;
      case X_REP_FOREVER: symb="Forever in DO" ; break ;
      case X_REP_COUNT: symb="Counter in DO" ; break ;
      case X_DO_TO: symb="Upper limit in DO" ; break ;
      case X_DO_BY: symb="Step-size in DO" ; break ;
      case X_DO_FOR: symb="Max number in DO" ; break ;
      case X_WHILE: symb="WHILE expr in DO" ; break ;
      case X_UNTIL: symb="UNTIL expr in DO" ; break ;
      case X_DROP: symb="DROP statement" ; break ;
      case X_EXIT: symb="EXIT statement" ; break ;
      case X_IF: symb="IF statement" ; break ;
      case X_IPRET: symb="INTERPRET statement" ; break ;
      case X_ITERATE: symb="ITERATE statement" ; break ;
      case X_LABEL: symb="Label specification" ; break ;
      case X_LEAVE: symb="LEAVE statement" ; break ;
      case X_NUM_D: symb="NUMERIC DIGIT statement" ; break ;
      case X_NUM_F: symb="NUMERIC FORM statement" ; break ;
      case X_NUM_FUZZ: symb="NUMERIC FUZZ statement" ; break ;
      case X_NUM_SCI: symb="Scientific numeric form" ; break ;
      case X_NUM_ENG: symb="Engeenering scientific form" ; break ;
      case X_PARSE: symb="PARSE statement" ; break ;
      case X_PARSE_U: symb="UPPER PARSE statement" ; break ;
      case X_PARSE_ARG: symb="PARSE ARG atatement" ; break ;
      case X_PARSE_EXT: symb="External parsing" ; break ;
      case X_PARSE_NUM: symb="Numeric parsing" ; break ;
      case X_PARSE_PULL: symb="Parse pull" ; break ;
      case X_PARSE_SRC: symb="Parse source" ; break ;
      case X_PARSE_VAR: symb="Parse variable" ; break ;
      case X_PARSE_VAL: symb="Parse value" ; break ;
      case X_PARSE_VER: symb="Parse version" ; break ;
      case X_PARSE_ARG_U: symb="PARSE UPPER ARG statement" ; break ;
      case X_PROC: symb="PROCEDURE statement" ; break ;
      case X_PULL: symb="PULL statement" ; break ;
      case X_PUSH: symb="PUSH statement" ; break ;
      case X_QUEUE: symb="QUEUE statement" ; break ;
      case X_RETURN: symb="RETURN statement" ; break ;
      case X_SAY: symb="SAY statement" ; break ;
      case X_SELECT: symb="SELECT statement" ; break ;
      case X_WHENS: symb="WHEN connector" ; break ;
      case X_WHEN: symb="WHEN clause" ; break ;
      case X_OTHERWISE: symb="OTHERWISE clause" ; break ;
      case X_SIG_VAL: symb="SIGNAL VALUE statement" ; break ;
      case X_SIG_LAB: symb="SIGNAL (label) statement" ; break ;
      case X_SIG_SET: symb="SIGNAL (setting) statement" ; break ;
      case X_ON: symb="Setting is ON" ; break ;
      case X_OFF: symb="Setting is OFF" ; break ;
      case X_S_ERROR: symb="ERROR option" ; break ;
      case X_S_HALT: symb="HALT option" ; break ;
      case X_S_NOVALUE: symb="NOVALUE option" ; break ;
      case X_S_SYNTAX: symb="SYNTAX option" ; break ;
      case X_TRACE: symb="TRACE statement" ; break ;
      case X_T_ALL: symb="ALL option" ; break ;
      case X_T_COMM: symb="COMMAND option" ; break ;
      case X_T_ERR: symb="ERROR option" ; break ;
      case X_T_INTER: symb="INTERMEDIATE option" ; break ;
      case X_T_LABEL: symb="LABEL option" ; break ;
      case X_T_NORMAL: symb="NORMAL option" ; break ;
      case X_T_OFF: symb="OFF option" ; break ;
      case X_T_SCAN: symb="SCAN option" ; break ;
      case X_UPPER_VAR: symb="UPPER statement" ; break ;
      case X_ASSIGN: symb="Assignment" ; break ;
      case X_LOG_NOT: symb="Logical NOT" ; break ;
      case X_PLUSS: symb="Plus operator" ; break ;
      case X_EQUAL: symb="Equal operator" ; break ;
      case X_MINUS: symb="Minus operator" ; break ;
      case X_MULT: symb="Multiplication operator" ; break ;
      case X_DEVIDE: symb="Division operator" ; break ;
      case X_MODULUS: symb="Modulus operator" ; break ;
      case X_LOG_OR: symb="Logical or" ; break ;
      case X_LOG_AND: symb="Logical and" ; break ;
      case X_LOG_XOR: symb="Logical xor" ; break ;
      case X_EXP: symb="Exponent operator" ; break ;
      case X_CONCAT: symb="String concatenation" ; break ;
      case X_SPACE: symb="Space separator" ; break ;
      case X_GTE: symb="Greater than or equal operator" ; break ;
      case X_LTE: symb="Less than or equal operator" ; break ;
      case X_GT: symb="Greater than operator" ; break ;
      case X_LT: symb="Less than operator" ; break ;
      case X_NEQUAL: symb="Not equal operator" ; break ;
      case X_NDIFF: symb="Not different operator" ; break ;
      case X_NGT: symb="Not greater than operator" ; break ;
      case X_NGTE: symb="Not greater than or equal operator" ; break ;
      case X_NLT: symb="Not less than operator" ; break ;
      case X_NLTE: symb="Not less than or equal operator" ; break ;
      case X_DIFF: symb="Different operator" ; break ;
      case X_SEQUAL: symb="Strictly equal operator" ; break ;
      case X_SDIFF: symb="Strictly different operator" ; break ;
      case X_SGT: symb="Strictly greater than operator" ; break ;
      case X_SGTE: symb="Strictly greater than or equal operator" ; break ;
      case X_SLT: symb="Strictly less than operator" ; break ;
      case X_SLTE: symb="Strictly less than or equal operator" ; break ;
      case X_SIM_SYMBOL: symb="Simple symbol" ; break ;
      case X_CON_SYMBOL: symb="Constant symbol" ; break ;
      case X_HEX_STR: symb="Hexadecimal string" ; break ;
      case X_STRING: symb="Constant string" ; break ;
      case X_FUNC: symb="Function call" ; break ;
      case X_U_MINUS: symb="Unary minus" ; break ;
      case X_S_EQUAL: symb="String equal operator" ; break ;
      case X_S_DIFF: symb="String different operator" ; break ;
      case X_SIMSYMB: symb="Simple symbol (2)" ; break ;
      case X_INTDIV: symb="Integer division" ; break ;
      case X_EX_FUNC: symb="External function call" ; break ;
      case X_IN_FUNC: symb="Internal function call" ; break ;
      case X_TPL_SOLID: symb="Solid point in template" ; break ;
      case X_TPL_MVE: symb="Constant pattern" ; break ;
      case X_TPL_VAR: symb="Variable pattern" ; break ;
      case X_TPL_TO: symb="Ehh, what does \"TO\" mean???" ; break ;
      case X_TPL_SYMBOL: symb="Variable in template" ; break ;
      case X_TPL_SPACE: symb="Space in template" ; break ;
      case X_TPL_POINT: symb="Placeholder in template" ; break ;
      case X_TMPLS: symb="Template connector" ; break ;
      case X_TPL_OFF: symb="Offset in template" ; break ;
      case X_TPL_PATT: symb="Pattern in template" ; break ;
      case X_NEG_OFFS: symb="Negative offset" ; break ;
      case X_POS_OFFS: symb="Positive offset" ; break ;
      case X_ABS_OFFS: symb="Absolute offset" ; break ;
      case X_EXPRLIST: symb="Expression connector" ; break ;
      case X_SYMBOLS: symb="Symbol connector" ; break ;
      case X_SYMBOL: symb="Symbol?" ; break ;
      case X_END: symb="End statement" ; break ;
      case X_IS_INTERNAL: symb="Internal function" ; break ;
      case X_IS_BUILTIN: symb="Builtin function" ; break ;
      case X_IS_EXTERNAL: symb="External function" ; break ;
      case X_CEXPRLIST: symb="Expression list" ; break ;
      case X_NASSIGN: symb="Numeric Assignment" ; break ;
      case X_VTAIL_SYMBOL: symb="Variable tail symbol" ; break ;
      case X_CTAIL_SYMBOL: symb="Constant tail symbol" ; break ;
      case X_HEAD_SYMBOL: symb="Compound variable symbol" ; break ;
      case X_STEM_SYMBOL: symb="Stem variable symbol" ; break ;
      case X_NO_OTHERWISE: symb="No otherwise statement" ; break ;
      default: symb="Unrecognized value" ;
   }

   return symb ;
}

#endif /* !NDEBUG */

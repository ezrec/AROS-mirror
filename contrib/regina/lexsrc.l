%{
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
#include "yaccsrc.h"
#include <ctype.h>
#include <string.h>
#include <assert.h>

/* Define ASCII_0_TERMINATES_STRING if you want that ASCII-0 terminates
 * an input string. Normally this should not happen. Input strings are
 * terminated by a length encoding. The string {"", length=1} is invalid for
 * the lexer (ASCII-0 is not allowed) while {"", length=0} is allowed (this
 * is an empty input).
 * ASCII_0_TERMINATES_STRING is only(!) for backward compatibility and
 * shouldn't be used under normal circumstances.
 * FGC
 */
#define ASCII_0_TERMINATES_STRING

#ifdef YYLMAX
# undef YYLMAX
#endif
#define YYLMAX BUFFERSIZE

#ifdef FLEX_SCANNER
#undef YY_CHAR
#define YY_CHAR YY_CHAR_TYPE
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=fill_buffer(buf,max_size)
#endif

/* NOTE: Every comment is replaced by a '`' character in the lower input
 * routines. These should check for such (illegal) characters.
 */
#define MY_ISBLANK(c) (((c)==' ')||((c)=='\t')||((c)=='`'))

PROTECTION_VAR(regina_parser)
/* externals which are protected by regina_parser */
internal_parser_type parser_data = {NULL, };
int retlength=0 ;
char retvalue[BUFFERSIZE] ;
unsigned SymbolDetect = 0;
/* end of externals protected by regina_parser */

/* locals, they are protected by regina_parser, too */
static int nextline = 1;
static int nextstart = 1;
static int do_level = 0 ;
static int in_numform=0, next_numform=0 ;
static int obs_with=0, in_do=0, in_then=0, dontlast=0 ;
static int sum=0 ;
static int code=0, nexta=0, in_parse=0, in_trace=0, itflag=0 ;
static int in_signal=0, in_call=0 ;
static enum { not_in_address = 0,
              in_address_keyword, /* ADDRESS just seen */
              in_address_main, /* after the first word */
              in_address_value, /* like main but VALUE was seen */
              in_address_with} in_address = not_in_address,
                               last_in_address = not_in_address ;
static enum {no_seek_with = 0,
             seek_with_from_parse,
             seek_with_from_address} seek_with = no_seek_with ;
static int preva=0, lasta=0 ;
static char ch, ech ;
static int kill_this_space=0, kill_next_space=1 ;
static int extnextline = -1, extnextstart; /* for a delayed line increment */
static int linenr=1 ;
static int contline = 0;
static int inEOF=0 ;
static int singlequote=0, doblequote=0 ;
static int firstln=0 ;
static int bufptr=0 ;
/* Previous bug. 8-bits clean combined with EOF ==> need at least short */
static short chbuffer[LOOKAHEAD] ;
static int ipretflag=0, cch=0 ;
static const char *interptr=NULL ;
static const char *interptrmax ;
static int cchmax = 0 ;

static YY_CHAR_TYPE *rmspc( YY_CHAR_TYPE *instr ) ;
static int fill_buffer( char *buf, int max_size ) ;
#define SET_NEXTSTART() (nextstart += yyleng)

#define YY_FATAL_ERROR(s) exiterror( ERR_PROG_UNREADABLE, 1, s )
%}


%start comm signal sgtype procd parse then with
%start numeric do1 other value1 ifcont signame nmform

%e 2500
%p 17000
%k 1500
%a 7000
%n 1000
%o 8000

%{
/* int yy_startcond=comm ; FGC: Needless? */
%}

not [\\^~]

csym [0-9.][a-zA-Z0-9.@#$!?_]*
ssym [a-zA-Z@#$!?_][a-zA-Z0-9.@#$!?_]*
strs ('([^']|'')*'|\"([^"]|\"\")*\")
labl ({sym}|{strs})+
sym [a-zA-Z0-9.@#$!?_]+
hsym [\t a-fA-F0-9]
bsym [\t 01]
hex {bl}*{hsym}*({bl}+({hsym}{hsym})+)*{bl}*
bin {bl}*{bsym}*({bl}+({bsym}{bsym}{bsym}{bsym})+)*{bl}*

vtail [a-zA-Z@#$!?_][a-zA-Z0-9@#$!?_]*
ctail [0-9][a-zA-Z0-9@#$!?_]*


a [aA]
b [bB]
c [cC]
d [dD]
e [eE]
f [fF]
g [gG]
h [hH]
i [iI]
j [jJ]
k [kK]
l [lL]
m [mM]
n [nN]
o [oO]
p [pP]
q [qQ]
r [rR]
s [sS]
t [tT]
u [uU]
v [vV]
w [wW]
x [xX]
y [yY]
z [zZ]


bl (\ |\`|\t)*
bbl (\ |\t)+

%%

   { if (nexta==1) {
        nexta = 0 ;
        lasta = (dontlast==0) ;
        dontlast = 0 ;
        return code ; }

     if (next_numform)
     {
        in_numform = 1 ;
        next_numform = 0 ;
     }
     else
        in_numform = 0 ;

     last_in_address = in_address ; /* just for the "Environment" */
    /* there can't be an intermediate SPACE between ADDRESS and the next word*/
     if (in_address == in_address_keyword)
        in_address = in_address_main ;

     kill_this_space = kill_next_space ;
     kill_next_space = 0 ;


     if (itflag)
        in_trace = seek_with = 0 ;
     itflag = (in_trace) ;

     if (extnextline != -1)
     {
        parser_data.tstart = nextstart = extnextstart;
        parser_data.tline = nextline = extnextline;
        extnextline = -1;
     }
     else
     {
        parser_data.tstart = nextstart ;
        parser_data.tline = nextline ;
     }
     preva = lasta ;
     lasta = 0 ;
   }

\`* SET_NEXTSTART() ;

<ifcont>{bl}[;\r?\n]{bl} {
   char *ptr;
   if ((ptr = strchr(yytext, '\n')) != NULL)
   {
      nextstart = yyleng - (int) (ptr - (char *) yytext) ;
      nextline++ ;
      if (extnextline != -1)
         extnextline++;
   }
   else
      SET_NEXTSTART() ;
   return STATSEP ; }

{bl}(;|\r?\n){bl} {
   char *ptr;
   BEGIN comm ;
   if (obs_with==1)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INVALID_TEMPLATE, 1, yytext )  ;
   }
   obs_with = in_do = 0 ;
   in_signal = in_address = in_call = 0 ;
   in_parse = 0 ;
   if (seek_with == seek_with_from_address)
      seek_with = no_seek_with ;
   if ((ptr = strchr(yytext, '\n')) != NULL)
   {
      nextstart = yyleng - (int) (ptr - (char *) yytext) ;
      nextline++ ;
      if (extnextline != -1)
         extnextline++;
   }
   else
      SET_NEXTSTART() ;
   return STATSEP ; }

<comm>{a}{d}{d}{r}{e}{s}{s}{bl} {
   BEGIN value1 ; /* Allow a following VALUE keyword */
   seek_with = seek_with_from_address ;
   in_address = in_address_keyword ;
   in_call = 1 ; /* Allow the next words to be given as in CALL. */
   SET_NEXTSTART() ;
   return ADDRESS ; }

<comm>{a}{r}{g}{bl} {
   BEGIN other ;
   in_parse = 1 ;
   SET_NEXTSTART() ;
   return ARG ; }

<comm>{c}{a}{l}{l}{bl} {
   BEGIN signal ;
   in_call = 1 ;
   SET_NEXTSTART() ;
   return CALL ; }

<comm>{d}{o}{bl} {
   BEGIN do1 ;
   assert( do_level >=0 ) ;
   do_level++ ;
   in_do = 1 ;
   SET_NEXTSTART() ;
   return DO ; }

<comm>{d}{r}{o}{p}{bl} {
   BEGIN other ;
   in_parse = 1 ;
   SET_NEXTSTART() ;
   return DROP ; }

<comm>{e}{l}{s}{e}{bl} {
   BEGIN comm ;
   SET_NEXTSTART() ;
   return ELSE ; }

<comm>{e}{x}{i}{t}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return EXIT ; }

<comm>{i}{f}{bl} {
   BEGIN ifcont ;
   in_then = 1 ;
   parser_data.if_linenr = linenr - 1;
   SET_NEXTSTART() ;
   return IF ; }

<comm>{i}{n}{t}{e}{r}{p}{r}{e}{t}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return INTERPRET ; }

<comm>{i}{t}{e}{r}{a}{t}{e}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return ITERATE ; }

<comm>{l}{e}{a}{v}{e}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return LEAVE ; }

<comm>{o}{p}{t}{i}{o}{n}{s}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return OPTIONS ; }

<comm>{n}{o}{p}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return NOP ; }

<comm>{n}{u}{m}{e}{r}{i}{c}{bl} {
   BEGIN numeric ;
   SET_NEXTSTART() ;
   return NUMERIC ; }

<comm>{p}{a}{r}{s}{e}{bl} {
   BEGIN parse ;
   in_parse = 1 ;
   SET_NEXTSTART() ;
   return PARSE ; }

<comm>{p}{r}{o}{c}{e}{d}{u}{r}{e}{bl} {
   BEGIN procd ;
   SET_NEXTSTART() ;
   return PROCEDURE ; }

<comm>{p}{u}{l}{l}{bl} {
   BEGIN other ;
   in_parse = 1 ;
   SET_NEXTSTART() ;
   return PULL ; }

<comm>{p}{u}{s}{h}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return PUSH ; }

<comm>{q}{u}{e}{u}{e}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return QUEUE ; }

<comm>{r}{e}{t}{u}{r}{n}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return RETURN ; }

<comm>{s}{a}{y}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return SAY ; }

<comm>{s}{e}{l}{e}{c}{t}{bl} {
   BEGIN other ;
   assert( do_level >= 0 ) ;
   do_level++ ;
   SET_NEXTSTART() ;
   return SELECT ; }

<comm>{s}{i}{g}{n}{a}{l}{bl} {
   BEGIN signal ;
   in_signal = 1 ;
   SET_NEXTSTART() ;
   return SIGNAL ; }

<comm>{t}{r}{a}{c}{e}{bl} {
   BEGIN value1 ;
   in_trace = 1 ;
   SET_NEXTSTART() ;
   return TRACE ; }

<comm>{u}{p}{p}{e}{r}{bl} {
   BEGIN other ;
   in_parse = 1 ;
   SET_NEXTSTART() ;
   return UPPER ; }

<comm>{w}{h}{e}{n}{bl} {
   BEGIN ifcont ;
   in_then = 1 ;
   parser_data.when_linenr = linenr - 1;
   SET_NEXTSTART() ;
   return WHEN ; }

<comm>{o}{t}{h}{e}{r}{w}{i}{s}{e}{bl} {
   BEGIN comm ;
   SET_NEXTSTART() ;
   return OTHERWISE ; }

<comm>{e}{n}{d}{bl} {
   BEGIN other ;
   assert( do_level >= 0 ) ;
   if (do_level==0)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_UNMATCHED_END, 1 ) ;
   }
   do_level-- ;
   SET_NEXTSTART() ;
   return END ; }

{bl} {
   if (in_parse)
   {
      SET_NEXTSTART() ;
      return yylex() ;
   }
   else
      REJECT ; }

\. {
   if (in_parse)
   {
      SET_NEXTSTART() ;
      return PLACEHOLDER ;
   }
   else
   {
      REJECT ;
#if 0
      /* FGC: What should the following mean after a REJECT? */
      retvalue[0] = '.' ;
      retvalue[1] = '\0' ;
      return CONSYMBOL ;
#endif
   }
 }

<comm>{csym}{bl}={bl} {
   parser_data.tline = linenr - 1 ; /* set tline for exiterror */
   if (yytext[0] == '.')
      exiterror( ERR_INVALID_START, 3, yytext )  ;
   else
      exiterror( ERR_INVALID_START, 2, yytext )  ;
   SET_NEXTSTART() ; }

<comm>{ssym}{bl}={bl} {
   unsigned i,j;
   BEGIN other ;

   j = 0 ;
   for (i=0;yytext[i];i++)
      if ('a' <= yytext[i] && yytext[i] <= 'z')
         retvalue[j++] = (char) (yytext[i] & 0xDF) ; /* ASCII only */
      else if (yytext[i]!='='  && yytext[i]!='\n' && !MY_ISBLANK(yytext[i]))
         retvalue[j++] = yytext[i] ;
   retvalue[j] = '\0' ;

   SET_NEXTSTART() ;
   return ASSIGNMENTVARIABLE ; }

<nmform,signal,value1>{bl}{v}{a}{l}{u}{e}{bl} {
   if (in_call)
   {
      if ((last_in_address == in_address_keyword) &&
          (in_address == in_address_main))
      {
         BEGIN other ; /* the next useful expression will set it to "other"
                        * in either case. The BEGIN-states aren't very
                        * handy in most cases; they are not flexible enough.
                        */
         in_address = in_address_value ;
         SET_NEXTSTART() ;
         return VALUE ;
      }
      REJECT ;
   }
   BEGIN other ;
   if ((!in_trace)&&(!in_address)&&(!in_signal)&&(!in_call)&&(!in_numform))
      obs_with = 1 ;
   in_trace = in_signal = in_call = 0 ;
   SET_NEXTSTART() ;
   return VALUE ; }

<signal>{o}{n}{bl} {
   BEGIN sgtype ;
   SET_NEXTSTART() ;
   return ON ; }

<signal>{o}{f}{f}{bl} {
   BEGIN sgtype ;
   SET_NEXTSTART() ;
   return OFF ; }

<signame>{n}{a}{m}{e}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return NAME ; }

<sgtype>{e}{r}{r}{o}{r}{bl} {
   BEGIN signame ;
   SET_NEXTSTART() ;
   return ERROR ; }

<sgtype>{h}{a}{l}{t}{bl} {
   BEGIN signame ;
   SET_NEXTSTART() ;
   return HALT ; }

<sgtype>{n}{o}{v}{a}{l}{u}{e}{bl} {
   BEGIN signame ;
   SET_NEXTSTART() ;
   return NOVALUE ; }

<sgtype>{n}{o}{t}{r}{e}{a}{d}{y}{bl} {
   BEGIN signame ;
   SET_NEXTSTART() ;
   return NOTREADY ; }

<sgtype>{f}{a}{i}{l}{u}{r}{e}{bl} {
   BEGIN signame ;
   SET_NEXTSTART() ;
   return FAILURE ; }

<sgtype>{s}{y}{n}{t}{a}{x}{bl} {
   BEGIN signame ;
   SET_NEXTSTART() ;
   return SYNTAX ; }

<sgtype>{l}{o}{s}{t}{d}{i}{g}{i}{t}{s}{bl} {
   BEGIN signame ;
   SET_NEXTSTART() ;
   return LOSTDIGITS ; }

<value1>{bl}[a-zA-Z?]+{bl} {
   if (!in_trace) REJECT ;
   strcpy(retvalue,rmspc( yytext )) ;
   SET_NEXTSTART() ;
   return WHATEVER ; }

<procd>{e}{x}{p}{o}{s}{e}{bl} {
   BEGIN other ;
   in_parse = 1 ;
   SET_NEXTSTART() ;
   return EXPOSE ; }

<parse>{u}{p}{p}{e}{r}{bl} {
   SET_NEXTSTART() ;
   return UPPER ; }

<parse>{a}{r}{g}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return ARG ; }

<parse>{n}{u}{m}{e}{r}{i}{c}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return NUMERIC ; }

<parse>{p}{u}{l}{l}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return PULL ; }

<parse>{s}{o}{u}{r}{c}{e}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return SOURCE ; }

<parse>{e}{x}{t}{e}{r}{n}{a}{l}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return EXTERNAL ; }

<parse>{l}{i}{n}{e}{i}{n}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return LINEIN ; }

<parse>{v}{e}{r}{s}{i}{o}{n}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return VERSION ; }

<parse>{v}{a}{r}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return VAR ; }

<parse>{v}{a}{l}{u}{e}{bl} {
   seek_with = seek_with_from_parse ;
   in_trace = 0 ;
   in_parse = 0 ;
   BEGIN with ; /* in fact this works as a "not comm" */
   SET_NEXTSTART() ;
   return VALUE ; }

<comm>{bl}{t}{h}{e}{n}{bl} {
   in_then = 0 ;
   SET_NEXTSTART() ;
   return THEN ; }

<other,ifcont>{bl}{t}{h}{e}{n}{bl} {
   if (in_then!=1) REJECT ;
   BEGIN comm ;
   in_then = 0 ;
   SET_NEXTSTART() ;
   return THEN ; }

{bl}{w}{i}{t}{h}{bl} {
   BEGIN other ;
   if ((in_do)||(!seek_with))
      REJECT ;
   if (seek_with == seek_with_from_parse)
      in_parse = 1 ;
   seek_with = 0 ;
   if (in_address) /* any address state */
      in_address = in_address_with ; /* WITH seen */
   SET_NEXTSTART() ;
   return WITH ; }


<numeric>{d}{i}{g}{i}{t}{s}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return DIGITS ; }

<numeric>{f}{o}{r}{m}{bl} {
   BEGIN nmform ;
   next_numform = 1 ;
   SET_NEXTSTART() ;
   return FORM ; }

<nmform>{s}{c}{i}{e}{n}{t}{i}{f}{i}{c}{bl} {
   SET_NEXTSTART() ;
   return SCIENTIFIC ; }

<nmform>{e}{n}{g}{i}{n}{e}{e}{r}{i}{n}{g}{bl} {
   SET_NEXTSTART() ;
   return ENGINEERING ; }

<numeric>{f}{u}{z}{z}{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return FUZZ ; }

<do1>{f}{o}{r}{e}{v}{e}{r}{bl} {
   BEGIN other ;
   assert(in_do) ;
   in_do = 2 ;
   SET_NEXTSTART() ;
   return FOREVER ; }

{bl}{t}{o}{bl} {
   if (in_do==2) 
   {
      BEGIN other ;
      SET_NEXTSTART() ;
      return TO ; 
   }
   else if (in_do==1)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INVALID_DO_SYNTAX, 1, "TO" )  ;
   }
   REJECT ; }

{bl}{b}{y}{bl} {
   if (in_do==2) 
   {
      BEGIN other ;
      SET_NEXTSTART() ;
      return BY ; 
   }
   else if (in_do==1)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INVALID_DO_SYNTAX, 1, "BY" ) ;
   }
   REJECT ; }

{bl}{f}{o}{r}{bl} {
   if (in_do==2) 
   {
      BEGIN other ;
      SET_NEXTSTART() ;
      return FOR ; 
   }
   else if (in_do==1)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INVALID_DO_SYNTAX, 1, "FOR" ) ;
   }
   REJECT ; }

{bl}{w}{h}{i}{l}{e}{bl} {
   if (in_do) 
   {
      if (in_do==3)
      {
         parser_data.tline = linenr - 1 ; /* set tline for exiterror */
         exiterror( ERR_INVALID_DO_SYNTAX, 1, "WHILE" )  ;
      }
      in_do=3 ;
      BEGIN other ;
      SET_NEXTSTART() ;
      return WHILE ; 
   }
   REJECT ; }

{bl}{u}{n}{t}{i}{l}{bl} {
   if (in_do) {
      if (in_do==3)
      {
         parser_data.tline = linenr - 1 ; /* set tline for exiterror */
         exiterror( ERR_INVALID_DO_SYNTAX, 1, "UNTIL" )  ;
      }

      in_do=3 ;
      BEGIN other ;
      SET_NEXTSTART() ;
      return UNTIL ; }
   REJECT ; }


<do1>{ssym}{bl}/= {
   BEGIN other ;
   in_do = 2 ;
   strcpy(retvalue,rmspc( yytext )) ;
   SET_NEXTSTART() ;
   return DOVARIABLE ; }

<comm>{sym}{bl}:{bl} {  /* set labl to sym for other kind or vice versa*/
   unsigned i,j;
   BEGIN comm ;

   for (i=j=0;(ch=yytext[i])!=0;i++) {
      if ('a' <= ch && ch <= 'z')
         retvalue[j++] = (char) (ch & 0xDF) ; /* ASCII only */
      /* FIXME: the following is WRONG, must first cut off {bl}:{bl} and
       * then fixup strings, FGC */
      else if ((ch!=',')&&(ch!='\n')&&(ch!=':')&&!MY_ISBLANK(ch))
         retvalue[j++] = ch ; }
   retvalue[j] = '\0' ;
   SET_NEXTSTART() ;
   return LABEL ; }


('([^']|'')+'|\"([^"]|\"\")+\")`*\( {
   int i;
   BEGIN other ;

   strcpy(retvalue,&yytext[1]) ;
   for (i=3; i<=yyleng && retvalue[yyleng-i]=='`'; i++) ;
   retvalue[yyleng-i] = '\0' ;

   kill_next_space = 1 ;
   if (preva==1) {
      nexta = dontlast = 1 ;
      code = EXFUNCNAME ;
      SET_NEXTSTART() ;
      return CONCATENATE ; }

   lasta = 0 ;
   SET_NEXTSTART() ;
   return EXFUNCNAME ; }



('{hex}'|\"{hex}\")[xX]/[^a-zA-Z0-9.@#$!?_(] {
   unsigned i,j,k;
   BEGIN other ;
   ech = yytext[0] ;

   /* first group can be large and odd-numbered; find # of zeros to pad */
   for (i=1; (yytext[i]!=ech) && isxdigit(yytext[i]); i++) ;

   /* j is the number of digits processed */
   j = (i-1)%2 ;
   sum = k = 0 ;

   for (i=1;(ech!=(ch=yytext[i]));i++)
   {
      if (isspace(ch))
      {
         if ((i==1)||(j))   /* leading space or space within a byte */
         {
            parser_data.tline = linenr - 1 ; /* set tline for exiterror */
            exiterror( ERR_INVALID_HEX_CONST, 1, i ) ;
         }
      }
      else if (isxdigit(ch))
      {
         sum = sum *16 + (HEXVAL(ch)) ;
         if ((++j)==2)
         {
            retvalue[k++] = (char) sum ;
            sum = j = 0 ;
         }
      }
   }

   if ((i>1) && isspace(yytext[i-1]))
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INVALID_HEX_CONST, 1, i ) ;
   }

   retvalue[k] = '\0' ;
   retlength = k ;

   if ((preva==1)&&(!in_parse)&&(!in_call))
   {
      nexta = 1 ;
      code = STRING ;
      SET_NEXTSTART() ;
      return CONCATENATE ;
   }

   if (in_call)
   {
      in_call = 0 ;
      kill_next_space = 1 ;
   }
   else
      lasta = 1 ;

   SET_NEXTSTART() ;
   return HEXSTRING ;
}



('{bin}'|\"{bin}\")[bB]/[^a-zA-Z0-9.@#$!?_(] {
   unsigned i,j,k;
   BEGIN other ;
   ech = yytext[0] ;

   /* first group can be large and odd-numbered; find # of zeros to pad */
   for (i=1; (yytext[i]!=ech) && isdigit(yytext[i]); i++) ;

   /* j is the number of digits processed */
   j = (4 - (i-1))%4 ;
   sum = k = 0 ;

   for (i=1;(ech!=(ch=yytext[i]));i++)
   {
      if (isspace(ch))
      {
         if ((i==1)||(j))   /* leading space or space within a byte */
         {
            parser_data.tline = linenr - 1 ; /* set tline for exiterror */
            exiterror( ERR_INVALID_HEX_CONST, 2, i ) ;
         }
      }
      else if ((ch=='0')||(ch=='1'))
      {
         sum = sum *2 + (ch-'0') ;
         if ((++j)==4)
         {
            retvalue[k++] = (char) sum ;
            sum = j = 0 ;
         }
      }
   }

   if ((i>1) && isspace(yytext[i-1]))
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INVALID_HEX_CONST, 2, i ) ;
   }

   j = (k%2) ;
   /* then pack the nibbles */
   for (i=j=(k%2); i<=k; i++)
   {
      if (i%2)
         retvalue[i/2] = (char)((retvalue[i/2]&0xf0) + retvalue[i-j]) ;
      else
         retvalue[i/2] = (char)((retvalue[i-j]&0x0f)<<4) ;
   }

   retvalue[retlength=i/2] = '\0' ;

   if ((preva==1)&&(!in_parse)&&(!in_call))
   {
      nexta = 1 ;
      code = STRING ;
      SET_NEXTSTART() ;
      return CONCATENATE ;
   }

   if (in_call)
   {
      in_call = 0 ;
      kill_next_space = 1 ;
   }
   else
      lasta = 1 ;

   SET_NEXTSTART() ;
   return BINSTRING ;
}

('([^']|'')*'|\"([^"]|"")*\")[xXbB]/[^a-zA-Z0-9.@#$!?_(] {
   unsigned int i;
   char bad=' ';
   BEGIN other ;
   parser_data.tline = linenr - 1 ; /* set tline for exiterror */
   for (i=0;yytext[i]; i++)
   {
      if ( !isxdigit(yytext[i]) && !(yytext[i] == '\"' && yytext[0] == '\"') && !(yytext[i] == '\'' && yytext[0] == '\'') && yytext[i] != ' ' )
      {
         bad = yytext[i];
         break;
      }
   }
   exiterror( ERR_INVALID_HEX_CONST, 3, bad )  ;
   SET_NEXTSTART() ;
   }

('([^']|'')*'|\"([^"]|\"\")*\") {
   unsigned i,j;
   BEGIN other ;
   for (i=1; yytext[i+1]; i++)
   {
      if (yytext[i]=='\n')
      {
         parser_data.tline = linenr - 1 ; /* set tline for exiterror */
         exiterror( ERR_UNMATCHED_QUOTE, 0 ) ;
      }

      if (yytext[i]==yytext[0] && yytext[i+1]==yytext[0])
         for (j=i+1; yytext[j]; j++)
            yytext[j] = yytext[j+1] ;
   }

   yytext[strlen(yytext)-1] = '\0' ;
   strcpy(retvalue,&yytext[1]) ;

   if (in_numform)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", retvalue ) ;
   }

   if ((preva==1)&&(!in_parse)&&(!in_call)) {
      nexta = 1 ;
      code = STRING ;
      SET_NEXTSTART() ;
      return CONCATENATE ; }

    if (in_call)
    {
       in_call = 0 ;
       kill_next_space = 1 ;
    }
    else
       lasta = 1 ;

   SET_NEXTSTART() ;
   return STRING ; }


[0-9]+ {
   if (!in_parse)
      REJECT ;
   strcpy(retvalue,yytext) ;
   SET_NEXTSTART() ;
   return OFFSET ; }

(((([0-9]+\.|\.?[0-9])[0-9]*{e}(\-|\+)[0-9]+))|([.0-9][a-zA-Z0-9.$!?@#_]*))\`*\( {
   int i;
   BEGIN other ;
   for (i=0; i<yyleng-1 && yytext[i]!='`'; i++) /* Copy value only */
      retvalue[i] = (char) toupper(yytext[i]) ;
   retvalue[i] = '\0' ;

   if (in_numform)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", retvalue ) ;
   }

   BEGIN other ;
   kill_next_space = 1 ;
   if (preva==1)
   {
      nexta = dontlast = 1 ;
      code = INFUNCNAME ;
      SET_NEXTSTART() ;
      return CONCATENATE ;
   }
   lasta = 0 ;
   SET_NEXTSTART() ;
   return INFUNCNAME ; }

((([0-9]+\.|\.?[0-9])[0-9]*{e}(\-|\+)[0-9]+))|([.0-9][a-zA-Z0-9.$!?@#_]*) {
   int i;
   BEGIN other ;
   for (i=0; i<=yyleng; i++) /* Copy '\0', too */
      retvalue[i] = (char) toupper(yytext[i]) ;

   if (in_numform)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", retvalue ) ;
   }

   if (in_call)
   {
      in_call = 0 ;
      BEGIN other ;
      kill_next_space = 1 ;
      lasta = 1 ;
      SET_NEXTSTART() ;
      return CONSYMBOL ;
   }

   if ((preva==1)&&(!in_parse)) {
      nexta = 1 ;
      code = CONSYMBOL ;
      SET_NEXTSTART() ;
      return CONCATENATE ; }

   lasta = 1 ;
   SET_NEXTSTART() ;
   return CONSYMBOL ; }

{ssym} {
   int i,j;
   /*
    * this might be a symbol in front of a function, but only if next
    * char in input stream is "(".
    */

   if (in_trace) REJECT ;

   for (i=0; i<=yyleng; i++) /* include terminating '\0' */
      retvalue[i] = (char) toupper(yytext[i]) ;

   if (in_numform)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", retvalue ) ;
   }

   if ((last_in_address == in_address_keyword) &&
       (in_address == in_address_main))
   {
      kill_next_space = 1 ;
      in_call = 0 ;
      SET_NEXTSTART() ;
      return SIMSYMBOL ;
   }

   /* We must check if a '(' follows. Remember the number of eaten chars. */
      j = 1;
   for (;(i=input())=='`';)
     j++ ;
   if (i != '(')
   {
      j-- ;
      unput(i) ;
   }
   /* input() has destroyed the yytext-terminator re-set it */
   yytext[yyleng] = '\0';
   SET_NEXTSTART() ;
   nextstart += j ;

   if (i=='(')
   {
      BEGIN other ;
      kill_next_space = 1 ;
      if (preva==1)
      {
         nexta = dontlast = 1 ;
         code = INFUNCNAME ;
         return CONCATENATE ;
      }
      lasta = 0 ;
      return INFUNCNAME ;
   }

   if (in_call)
   {
      kill_next_space = 1 ;
      BEGIN other ;
      in_call = 0 ;
      lasta = 1 ;
      return SIMSYMBOL ;
   }

   BEGIN other ;
   if ((preva==1)&&(!in_parse)) {
      nexta = 1 ;
      code = SIMSYMBOL ;
      return CONCATENATE ; }

   lasta = 1 ;

   if (in_address == in_address_with)
      kill_next_space = 1 ;
   if (SymbolDetect) /* allow a fast breakout */
   {
      /* We define a tricky preprocessor directive. This will give us
       * maximum performance without the loss of control or errors produced
       * by typos.
       */
#define RET_IF(s)  if ((SymbolDetect & SD_##s) &&      \
                       (yyleng == sizeof(#s) - 1) &&   \
                       (strncmp(retvalue,              \
                                #s,                    \
                                sizeof(#s) - 1) == 0)) \
                      return(s)
      /* e.g. RET_IF(INPUT); is replaced by:
       *  if ((SymbolDetect & SD_INPUT) &&
       *      (yyleng == sizeof("INPUT") - 1) &&
       *      (strncmp(retvalue,
       *               "INPUT",
       *               sizeof("INPUT") - 1) == 0))
       *     return(s);
       */
      RET_IF(INPUT);
      RET_IF(OUTPUT);
      RET_IF(ERROR);
      RET_IF(NORMAL);
      RET_IF(APPEND);
      RET_IF(REPLACE);
      RET_IF(STREAM);
      RET_IF(STEM);
      RET_IF(LIFO);
      RET_IF(FIFO);
#undef RET_IF
   }
   return SIMSYMBOL ; }

{bl}\) {
   lasta = 1 ;
   SET_NEXTSTART() ;
   return ')' ; }

\({bl} {
   BEGIN other ;
   if (preva==1)
   {
      nexta = dontlast = 1 ;
      code = '(' ;
      SET_NEXTSTART() ;
      return CONCATENATE ;
   }
   SET_NEXTSTART() ;
   return '(' ; }

{bl}\,{bl} {
   SET_NEXTSTART() ;
   return ',' ; }

{bl}\-{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return '-' ; }

{bl}\+{bl} {
   BEGIN other ;
   SET_NEXTSTART() ;
   return '+' ; }

{bl}\/{bl} {
   SET_NEXTSTART() ;
   return '/' ; }

{bl}%{bl} {
   SET_NEXTSTART() ;
   return '%' ; }

{bl}\*{bl} {
   SET_NEXTSTART() ;
   return '*' ; }

{bl}\|{bl} {
   SET_NEXTSTART() ;
   return '|' ; }

{bl}&{bl} {
   SET_NEXTSTART() ;
   return '&' ; }

{bl}={bl} {
   SET_NEXTSTART() ;
   return '=' ; }

{not}{bl} {
   /* why don't I have a {bl} in the beginning of this re? bug? */
   BEGIN other ;
   SET_NEXTSTART() ;
   return NOT ; }

{bl}\>{bl}\>{bl} {
   SET_NEXTSTART() ;
   return GTGT ; }

{bl}\<{bl}\<{bl} {
   SET_NEXTSTART() ;
   return LTLT ; }

{bl}{not}{bl}\>{bl}\>{bl} {
   SET_NEXTSTART() ;
   return NOTGTGT ; }

{bl}{not}{bl}\<{bl}\<{bl} {
   SET_NEXTSTART() ;
   return NOTLTLT ; }

{bl}\>{bl}\>{bl}={bl} {
   SET_NEXTSTART() ;
   return GTGTE ; }

{bl}\<{bl}\<{bl}={bl} {
   SET_NEXTSTART() ;
   return LTLTE ; }

{bl}(\>|{not}{bl}(\<{bl}=|={bl}\<)){bl} {
   SET_NEXTSTART() ;
   return GT ; }

{bl}({not}{bl}\<|={bl}\>|\>{bl}=){bl} {
   SET_NEXTSTART() ;
   return GTE ; }

{bl}(\<|{not}{bl}(\>{bl}=|={bl}\>)){bl} {
   SET_NEXTSTART() ;
   return LT ; }

{bl}({not}{bl}\>|={bl}\<|\<{bl}=){bl} {
   SET_NEXTSTART() ;
   return LTE ; }

{bl}({not}{bl}=|\<{bl}\>|\>{bl}\<){bl} {
   SET_NEXTSTART() ;
   return DIFFERENT ; }

{bl}={bl}={bl} {
   SET_NEXTSTART() ;
   return EQUALEQUAL ; }

{bl}{not}{bl}={bl}={bl} {
   SET_NEXTSTART() ;
   return NOTEQUALEQUAL ; }

{bl}\/{bl}\/{bl} {
   SET_NEXTSTART() ;
   return MODULUS ; }

{bl}&{bl}&{bl} {
   SET_NEXTSTART() ;
   return XOR ; }

{bl}\|{bl}\|{bl} {
   SET_NEXTSTART() ;
   return CONCATENATE ; }

{bl}\*{bl}\*{bl} {
   SET_NEXTSTART() ;
   return EXP ; }

{bl}[ \t]{bl} {
   if (in_address == in_address_value) /* Always allow spaces in the VALUE */
   {                                   /* part of the ADDRESS stmt.        */
      SET_NEXTSTART() ;
      return SPACE ;
   }
   if (kill_this_space)
   {
      SET_NEXTSTART() ;
      return yylex() ;
   }
   SET_NEXTSTART() ;
   return (((in_parse)&&(!seek_with)) ? yylex() : SPACE) ; }

['"] {
   SET_NEXTSTART() ;
   parser_data.tline = linenr - 1 ; /* set tline for exiterror */
   exiterror( ERR_UNMATCHED_QUOTE, 0 )  ; }


[^A-Za-z0-9 \t\n@#$&|.?!_*()+=%\\^'";:<,>/-] {
   SET_NEXTSTART() ;
   parser_data.tline = linenr - 1 ; /* set tline for exiterror */
   exiterror( ERR_INVALID_CHAR, 1, yytext[0], yytext[0] )  ; }

: {
   SET_NEXTSTART() ;
   parser_data.tline = linenr - 1 ; /* set tline for exiterror */
   exiterror( ERR_SYMBOL_EXPECTED, 1, yytext ) ;}

. {
   SET_NEXTSTART() ;
   parser_data.tline = linenr - 1 ; /* set tline for exiterror */
   exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;}


%%

#define NORMALSTAT  0
#define COMMENTSTAT 1
#define SINGLEQUOTE 2
#define DOUBLEQUOTE 3


/* rmspc uppercases all characters and removes blanks from a string.
 * Returns the input string.
 */
static YY_CHAR_TYPE *rmspc( YY_CHAR_TYPE *instr )
{
   YY_CHAR_TYPE *retval=instr ,
                *dest  =instr ,
                c ;

   while ((c = *instr++) != '\0')
   {
      c = (YY_CHAR_TYPE) toupper(c);
      /* These characters are treated as blanks: */
      if ((c!='`') && (c!=' ') && (c!=',') && (c!='\t') && (c!='\n'))
         *dest++ = c ;
   }
   *dest = '\0' ;

   return( retval ) ;
}


/* get_next_line: Lower level input fetcher.
 * Reads exactly one line from the input stream (file or string).
 * All EOL characters are removed and the result is stored in
 * last_source_line. A check for line overflow occurred here.
 * A special check is done for CP/M ^Z (DOS and friends use this for
 * "backward" compatibility, too).
 * line is filled with valid values on success.
 * max must reflect size of line and should be at least BUFFERSIZE + 2;
 * Returns -1 (no input) or the number of valid chars in line.
 */
static int get_next_line( char *line, int max, FILE *stream )
{
   lineboxptr newline ;
   offsrcline *incore_newline;
   int pos = 0;
   int c = 0, nextEOL ;

   if (inEOF) /* You can't use myunputc if EOF is reached! */
      return EOF ;

   while (pos <= max - 2)
   {
      /* get next char */
      if (bufptr>0)
         c = chbuffer[--bufptr] ;
      else if (ipretflag)
      {
         if (interptr>=interptrmax)
            c = EOF ;
         else
#ifdef ASCII_0_TERMINATES_STRING
            if ((c = *interptr++) == '\0')
               c = EOF ;
#else
            c = *interptr++ ;
#endif
      }
      else
         c = getc(stream) ;

      if ((c=='\r') || (c=='\n') || (c==EOF))
         break ;
      line[pos++] = (char) (unsigned char) c ;
   }

   /* first, check for overflow */
   if ((c!='\r') && (c!='\n') && (c!=EOF))
   {
      parser_data.tline = linenr ; /* set tline for exiterror */
      exiterror( ERR_TOO_LONG_LINE, 0 )  ;
   }

   /* We have either a (first) line terminator or EOF */
   if (c==EOF)
   {
      if ((pos==1) && (line[0]=='\x1A')) /* CP/M ^Z EOF? */
         pos-- ;
      if (pos == 0)
      {
         inEOF = 1 ;
         return EOF ;
      }
      nextEOL = EOF;
   }
   else
   {
      /* get one more char */
      if (bufptr>0)
         nextEOL = chbuffer[--bufptr] ;
      else if (ipretflag)
      {
         if (interptr>=interptrmax)
            nextEOL = EOF ;
         else
#ifdef ASCII_0_TERMINATES_STRING
            if ((nextEOL = *interptr++) == '\0')
               nextEOL = EOF ;
#else
            nextEOL = *interptr++ ;
#endif
      }
      else
         nextEOL = getc(stream) ;
   }

   /* Decide if the next character is the last char of a EOL pair.
    * Valid pairs are CR/LF or LF/CR. Put nextEOL back if there is no pair.
    */
   if (((c!='\n') || (nextEOL!='\r')) &&
       ((c!='\r') || (nextEOL!='\n')))
      chbuffer[bufptr++] = (short) nextEOL ;

   cch = 0 ; /* not needed ? */
   line[pos++] = '\n';

   if (parser_data.incore_source)
   {  /* We can use the incore string to describe a source line. */
      incore_newline = FreshLine() ;
      incore_newline->length = pos - 1 ;
      /* FIXME: What happens on the second attempt to read EOF or with CRLF? */
      incore_newline->offset = interptr - parser_data.incore_source ;
      return pos ;
   }

   newline = (lineboxptr)Malloc(sizeof(linebox)) ;
   newline->line = Str_make_TSD( parser_data.TSD, pos - 1 ) ;
   newline->line->len = pos - 1 ;
   memcpy(newline->line->value, line, pos - 1 ) ;
   newline->prev = parser_data.last_source_line ;
   newline->next = NULL ;
   newline->lineno = linenr++ ;

   if (parser_data.first_source_line==NULL)
      parser_data.first_source_line = newline ;
   else
      parser_data.last_source_line->next = newline ;
   parser_data.last_source_line = newline ;

   return pos ;
}

/* fill_buffer: Higher level input fetcher.
 * (To allow the C-file to compile, all Rexx comments in this comment
 *  are written as "{*" "*}" instead of the normal, C-like manner.)
 * Reads lines from the input stream (yyin or string) with get_next_line.
 * Only one line is returned to allow the saving of the line number.
 * This routine replaces all comments by '`' signs. This allows
 * the detection of a "pseudo" blank: The fragment "say x{* *}y" uses two
 * variables, not one called "xy". The parsing of comments must be done
 * here to check for the actual numbers of open and closes ("{*" and "*}").
 * While doing this we must always check for strings since "'{*'" is not part
 * of a comment.
 * Here is a problem: Is this a nested valid comment: "{* '{*' *} *}"?
 * I think so although you cannot remove the outer comment signs without an
 * error. Everything within a comment is a comment (per def.). Counting
 * opens and closes of comment signs is an ugly trick to help the user.
 * He/she must know what he/she is doing if nesting comments!
 *
 * max_size gives the maximum size of buf. This is filled up with input.
 * We never return less than one character until EOF is reached. Thus, we
 * read more than one true input line if a comment spans over more than one
 * line.
 * A line will either be terminated by a single '\n' or by a blank. The
 * later one replaces a line continuation (',' [spaces] EOL).
 * Errors in this low
 *
 * Conclusion: We have to fight very hard to set the expected line number.
 *             * Comments spanning over lines set them on getting the
 *               "follow" lines.
 *             * Concatenated lines set
 */
static int fill_buffer( char *buf, int max_size )
{
   /* statics protected by regina_parser */
   static char line[BUFFERSIZE+2] ; /* special buffer to allow max_size */
   static int pos = 0, max = 0 ;    /* being smaller than BUFFERSIZE+1  */
   static int nesting = 0;          /* nesting level of comments        */
   int nesting_start_line = 0;      /* start line of comment for errortext() */
   char *dest, c;
   int i, squote, dquote;

   if (firstln == 0)
   {
      firstln = 1;
      contline = 0;
      nesting = 0;
      pos = 0;
      max = get_next_line( line, sizeof(line), yyin ) ;
      if (max < 0) /* empty input file */
         return 0 ;
#if 0
      if (line[0] == '#')
      {  /* Ignore first line beginning this way for unix compat */
         max = 0;
         return fill_buffer( buf, max_size ) ;
      }
#else
      if (line[0] == '#')
      {  /* Ignore first line beginning this way for unix compat */
         max = 5;
         memcpy( line, "/**/\n", 5 );
      }
#endif
   }
   else if (pos < max) /* Are there still characters to transmit? */
   {
      /* Buffer already checked for correctness */
      if (max_size > max - pos)
         max_size = max - pos;
      memcpy(buf, line + pos, max_size);
      pos += max_size;
      return(max_size);
   }
   else /* Need next line */
   {
      if (contline && !nesting)
      {
         extnextline = ++nextline ;
         extnextstart = 1 ;
         contline = 0;
      }
      pos = 0;
      max = get_next_line( line, sizeof(line), yyin ) ;
      if (max < 0) /* empty input file */
      {
         if (nesting)
         {
            parser_data.tline = linenr - 1 ; /* set tline for exiterror */
            exiterror( ERR_UNMATCHED_QUOTE, 1 ) ;
         }
         return 0 ;
      }
   }

   /* A new line is available, check first for an ending comment */
   dest = line; /* we change comments in line */
   if (nesting) /* This may lead to more line reading */
   {
      /*
       * The first time extnextline is non-zero, we have the comment
       * starting sequence line. This is saved for use if no matching
       * ending comment sequence is found, so that the error message
       * reflects the start of the comment.
       * Regina feature request: #508788
       */
      if ( extnextline < 0 )
         nesting_start_line = nextline+1;
      extnextline = ++nextline ;
      extnextstart = 1; /* See Reference (*) below */
repeated_nesting:
      while (pos < max)
      {
         c = line[pos];
         if (c == '*') /* start of comment end? */
         {
            if (line[pos+1] == '/')
            {  /* pos+1 always exists, at least '\n' or '\0' */
               if (--nesting == 0)
               {
                  pos += 2;
                  *dest++ = '`';
                  *dest++ = '`';
                  break;
               }
               *dest++ = '`';
               pos++;
            }
         }
         else if (c == '/') /* start of new begin? */
         {
            if (line[pos+1] == '*')
            {
               nesting++;
               *dest++ = '`';
               pos++;
            }
         }
         *dest++ = '`';
         pos++;
      }
      if (pos >= max)
      {
         pos = 0;
         max = get_next_line( line, sizeof(line), yyin ) ;
         if (max < 0) /* empty input file */
         {
            if ( nesting_start_line )
               parser_data.tline = nesting_start_line ; /* set tline for exiterror */
            else
               parser_data.tline = linenr - 1 ; /* set tline for exiterror */
            exiterror( ERR_UNMATCHED_QUOTE, 1 ) ;
            return 0 ;
         }
         /* This is a comment continuation. If the lexer will return
          * something it already has a valid tline/tstart pair.
          * The lexer will return the current token and on the NEXT
          * call it expects a valid nextline/nextstart pair.
          */
         extnextline = ++nextline; extnextstart = 1;
         dest = line; /* we change comments in line */
         goto repeated_nesting;
      }
      extnextstart = pos + 1;
      if (contline)
      { /* Exception! Have a look at: "x='y',{*\n\n*}\n'z'". This should
         * result in "x = 'y' 'z'".
         * We must parse until EOL and check for whitespaces and comments...
         */
         while (pos < max)
         {
            c = line[pos];
            if (!isspace(c))
            {
               if (c == '/')
               {
                  if (line[pos+1] == '*')
                  {
                     pos += 2;
                     nesting++;
                     goto repeated_nesting;
                  }
               }
               parser_data.tline = linenr - 1 ; /* set tline for exiterror */
               exiterror( ERR_YACC_SYNTAX, 1, parser_data.tline ) ; /* standard error */
            }
            pos++;
         }
         /* All done, it was a continuation line. */
         /* contline will be resetted by: */
         return fill_buffer( buf, max_size ) ;
      }
   }
   /* We have something to play with. Run through the input and check for
    * strings including comments.
    */
   squote = dquote = 0;
   while (pos < max)
   {
      /* We use selective loops to reduce comparisons */
      if (nesting)
         do
         {
            c = line[pos];
            if (c == '*') /* start of comment end? */
            {
               if (line[pos+1] == '/')
               {  /* pos+1 always exists, at least '\n' or '\0' */
                  if (--nesting == 0)
                  {
                     pos += 2;
                     *dest++ = '`';
                     *dest++ = '`';
                     break;
                  }
                  pos++;
               }
            }
            else if (c == '/') /* start of new begin? */
            {
               if (line[pos+1] == '*')
               {
                  nesting++;
                  pos++;
                  *dest++ = '`';
               }
            }
            pos++;
            *dest++ = '`';
         } while (pos < max);
      else if (squote)
         {
            while ((c = line[pos]) != '\'')
            {
               *dest++ = c;
               if (++pos >= max)
               {
                  parser_data.tline = linenr - 1 ; /* set tline for exiterror */
                  exiterror( ERR_UNMATCHED_QUOTE, 2 ) ;
               }
            }
            *dest++ = '\'';
            pos++;
            squote = 0;
         }
      else if (dquote)
         {
            while ((c = line[pos]) != '\"')
            {
               *dest++ = c;
               if (++pos >= max)
               {
                  parser_data.tline = linenr - 1 ; /* set tline for exiterror */
                  exiterror( ERR_UNMATCHED_QUOTE, 3 ) ;
               }
            }
            *dest++ = '\"';
            pos++;
            dquote = 0;
         }
      else
         while (pos < max)
            switch (c = line[pos])
            {
               case '\'':
                  *dest++ = c ;
                  squote = 1 ;
                  pos++ ;
                  goto outer_loop;

               case '\"':
                  *dest++ = c ;
                  dquote = 1 ;
                  pos++ ;
                  goto outer_loop;

               case '/':
                  if (line[pos + 1] == '*')
                  {
                     *dest++ = '`' ;
                     *dest++ = '`' ;
                     pos += 2 ;
                     nesting++ ;
                     goto outer_loop;
                  }
                  else
                  {
                     *dest++ = c;
                     pos++ ;
                  }
                  break ;

               case '`':
                  parser_data.tline = linenr - 1 ; /* set tline for exiterror */
                  exiterror( ERR_INVALID_CHAR, 1, c, c ) ;

               default:
                  *dest++ = c;
                  pos++ ;
            }
outer_loop:
      ;
   }

   max = (int) (dest - line);

   /* Now we can replace a ',' [spaces|comments] '\n' with the line
    * continuation, but check for nesting first
    */
   if (nesting)
   { /* Don't leave ANY spaces at EOL. That would confuse the lexer. */
      i = max - 1;
      while ((i >= 0) && isspace(line[i]))
         i--;
      max = i + 1;
      /* Of course, there is one exception: line continuation */
      while ((i >= 0) && (line[i] == '`'))
         i-- ;
      if ((i >= 0) && (line[i] == ','))
      {
         contline = 1;
         line[i] = ' ';
         max = i + 1;
      }
      /* (Reference (*) )
       * At this point the lexer can't determine the nextline since we eat up
       * the \n. This leads to an incorrect count. But either the '`'-signs
       * are ignored or they are follows of a "token", a valid word.
       * Look at "say x;say y ``". This will cause the lexer to
       * return at least 4 tokens (SAY "x" ";" SAY) before "y" will be
       * returned. We can only set nextline/nextstart at "y".
       * Result: We set this pair at the start of the next call to
       * fill_buffer such that the next call to yylex will set the correct
       * values.
       */
   }
   else
   {
      i = max - 1; /* on last valid char */
      while (i >= 0)
      {
         if (!MY_ISBLANK(line[i]) && (line[i] != '\n'))
            break;
         i--;
      }
      /* i now -1 or on last nonblank */
      if ((i >= 0) && (line[i] == ','))
      {  /* FIXME: What shall be do with "," followed by EOF? */
         max = i + 1;
         line[i] = ' ';
         contline = 1;
      }
   }

   if (max_size > max)
      max_size = max;
   memcpy(buf, line, max_size);
   pos = max_size;
   return(max_size);
}


/* yywrap MAY be called by the lexer is EOF encounters, see (f)lex docu */
int yywrap( void )
{
   assert( do_level>= 0 ) ;
   if (do_level>0)
   {
      parser_data.tline = linenr - 1 ; /* set tline for exiterror */
      exiterror( ERR_INCOMPLETE_STRUCT, 0 )  ;
   }
   return 1 ;
}

/******************************************************************************
 ******************************************************************************
 * global interface ***********************************************************
 ******************************************************************************
 *****************************************************************************/

/* initalize all local and global values */
static void init_it_all( tsd_t *TSD )
{
#if defined(FLEX_SCANNER) && defined(FLEX_DEBUG)
   if (__reginadebug)
      yy_flex_debug = 1;
     else
      yy_flex_debug = 0;
#endif
   inEOF = 0 ;
   in_numform = 0 ;
   next_numform = 0 ;
   nexta = 0 ;
   lasta = 0 ;
   preva = 0 ;
   obs_with = 0 ;
   in_do = 0 ;
   in_then = 0 ;
   dontlast = 0 ;
   sum = 0 ;
   firstln = 0 ;
   in_parse = 0 ;
   in_trace = 0 ;
   itflag = 0 ;
   in_signal = 0 ;
   in_call = 0 ;
   in_address = 0 ;
   seek_with = 0 ;
   kill_this_space = 0 ;
   ipretflag = 0 ;
   do_level = 0 ;
   singlequote = 0 ;
   doblequote = 0 ;
   cch = 0 ;
   bufptr = 0 ;
   cchmax = 0 ;
   ch = '\0',
   code = 0,
   contline = 0;
   ech= '\0' ;
   extnextstart = 0;
   interptr = NULL ;
   interptrmax = NULL ;
                          /* non-zero values */
   linenr = 1 ;
   nextline = 1;
   nextstart = 1;
   kill_next_space = 1 ;
   extnextline = -1 ;
   SymbolDetect = 0;

   memset(&parser_data, 0, sizeof(internal_parser_type));
   parser_data.TSD = TSD;
}

/* fetch may only be called by fetch_protected. The parser and lexer are
 * already protected by regina_parser by fetch_protected.
 * This function prepares the lexer and parser and call them. The
 * result and all generated values are stored in result. The parser
 * tree isn't executed here.
 * Exactly fptr xor str must be non-null.
 */
static void fetch(tsd_t *TSD, FILE *fptr, const streng *str,
                  internal_parser_type *result)
{
   init_it_all( TSD ) ;

#ifdef FLEX_SCANNER
   yy_init = 1 ;
   yy_delete_buffer(YY_CURRENT_BUFFER) ;
   yyrestart(fptr) ;
#else
   yysptr = yysbuf ;
   yyin = fptr ;
#endif

   if (str != NULL)
   {
      ipretflag = 1 ;
      cchmax = str->len ;
      interptr = str->value ;
      interptrmax = interptr + cchmax ;
      result->incore_source = str->value;
   }

   BEGIN comm ;
   NewProg();
   parser_data.result = __reginaparse();

#ifdef FLEX_SCANNER
   yy_delete_buffer(YY_CURRENT_BUFFER) ;
#else
   yysptr = yysbuf ;
#endif
   yyin = NULL ;

   *result = parser_data;
   /* Some functions assume null values if parsing isn't running: */
   memset(&parser_data, 0, sizeof(internal_parser_type));
}

/* This function serializes the parser/lexer requests of the process and
 * call fetch which will make the work. Look there.
 */
static void fetch_protected(tsd_t *TSD, FILE *fptr, const streng *str,
                            internal_parser_type *result)
{
   volatile int panicked = 0;

   THREAD_PROTECT(regina_parser)
   TSD->in_protected = 1;

   if ( setjmp( TSD->protect_return ) )
      panicked = 1;
   else
      fetch(TSD, fptr, str, result);

   TSD->in_protected = 0;
   THREAD_UNPROTECT(regina_parser)

   if (!panicked)
      return;

   /* We got a fatal condition while fetching the input. */
   if (TSD->delayed_error_type == PROTECTED_DelayedExit)
      TSD->MTExit(TSD->expected_exit_error);
   if (TSD->delayed_error_type == PROTECTED_DelayedSetjmpBuf)
      longjmp( *(TSD->currlevel->buf), 1 ) ;
   longjmp( *(TSD->systeminfo->panic), 1 ) ;
}

/* fetch_file reads in a REXX file from disk (or a pipe). It returns without
 * executing the program. The parsed tree with all needed values including
 * the result of the parsing is copied to result.
 * fptr remains open after this call.
 * type is either PARSE_ONLY or PARSE_AND_TIN. In the later case a tinned variant of the
 * parsing tree is created, too.
 */
void fetch_file(tsd_t *TSD, FILE *fptr, internal_parser_type *result)
{
   fetch_protected(TSD, fptr, NULL, result);
}

/* fetch_string reads in a REXX macro from a streng. It returns without
 * executing the program. The parsed tree with all needed values including
 * the result of the parsing is copied to result.
 * type is either PARSE_ONLY or PARSE_AND_TIN. In the later case a tinned variant of the
 * parsing tree is created, too.
 * The function is typically called by an "INTERPRET" instruction.
 */
void fetch_string(tsd_t *TSD, const streng *str, internal_parser_type *result)
{
   fetch_protected(TSD, NULL, str, result);
}

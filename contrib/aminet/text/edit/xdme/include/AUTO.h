
/******************************************************************************

    MODULE
	AUTO.h

    DESCRIPTION
	Header file for use with "compiler
	independent autoinit/autoexit routines"

    USAGE

      METHOD 1

	that method does (hopefully) not use any compiler
	depencies, to enable auto-initialisation;
	all You need is an ANSI-Compiler, a Make-Utility
	and a Grep-Utility (better [f]lex)

      * call "AUTO_Init();" at the beginning of main()

      * each function, that should use auto_initialisation
	must get a line "AUTOINIT(<functionname>)"
	somewhere is your *.c module; (the same for
	auto-atexit and AUTOEXIT(...)).
	Be aware, that that line will be used o create
	a call to your function, so don't put any Garbage
	behind it.
	while declaring that line inside a comment,
	You need not include any additional file;
	while declaring that line in C-code, you have to
	#include "AUTO.h" or to #define AUTO(IN|EX)IT(x)
	to nothing;

      * put some additional dependencies into your Makefile:

	M> AUTO.o : AUTO.c _AUTO_enex.h
	M>
	M> _AUTO_enex.h : $(C_sources)
	M>	   egrep -h -e "^[ ]*AUTO(IN|EX)IT\([ ]*[A-Za-z0-9_]+[ ]*\)" $(C_sources) > $*.h
	M>

	and add AUTO.c to Your $(C_sources).


      METHOD 2

	that Method is designed to support the Compiler-builtin
	Auto-initialisation features, like used by DICE or SAS/C,
	but if not using one of those compilers,

      * call "AUTO_Init();" at the beginning of main()
	this is done for future support of GNU/C, which to my
	knowledge has no autoinit support

      * surround the definition of each auto??it-function
	with MK_AUTO??IT(), where MK_AUTO..IT must be the 1st
	word of a line. Wanna say, write
	C> MK_AUTOINIT( my_init ) { ...
	where DICE expects
	C> __autoinit void my_init(void) { ...
	or SAS/C expects
	C> void STI_my_init(void) { ...

      * as long as Your compiler is supporting auto??it-features,
	there is currently no need to do any addes to the Makefile;
	but it should be done anyway, since the program might also
	compiled with another system ...

      * there might occur problems, if the compiler expects autoinit
	functions to return errorcodes or successvalues

    REQUIREMENTS

      METHOD 1

      * You must use egrep or something like that to extract
	the autoinit-funktions from Your sources;
	the "-h" options seems not to be available for each version
	of egrep over there (#$&%^$& HPUX), so there might be some
	other solution neccessary.

	e.g. it should be possible using sedwith something like:
	M>  sed -n "s/^ *AUTO_..IT( *[A-Za-z0-9_]* *)/p" >
	but I was never forced to use something like that,
	so it is untested.

      METHOD 2

      * while using SAS/C or DICE there is no other tool necessary
	when using GNU/C a parser like shown below is necessary,
	so lex might be used to create it...

	the parser might be a lex-pgm like the following
	L> WS	[ \n\t]
	L> DLM	[A-Za-z_][A-Za-z_0-9]*
	L> %%
	L> ^{WS}*(MK_)?AUTO(IN|EX)IT{WS}*"("{WS}*{DLM}{WS}*")" printf("%s;",yytext);
	L> .					      ;
	to create the scanner just type
	%> lex lex.l
	%> gcc lex.yy.c -ll
	to get it work in a Makefile
	M> _AUTO_enex.h : $(OWN_C_SRCS)
	M>	   cat $(OWN_C_SRCS) | a.out > _AUTO_ENEX.h


    NOTES
	please have a look into AUTO.c, too

	Please note the two introduced methods are to be used mutally
	exclusive, i.e. using "AUTO_??IT(func_x)" and defining
	"MK_AUTO??IT(func_x){...}" causes func_x be called twice,
	which might lead into undefined situations ...

	if Your compiler is capable of an own technique for
	auto-init/exit declarations, there is no need for this
	module, but porting might be a little bit more easy
	while using it ;-)

	the following compilers have own techniques
	(the list is surely incomplete)
	SAS-C v6.*  (Platform AMIGA)
	DICE 2.0*r  (Platform AMIGA)

    HISTORY
	07-12-93 b_noll created
	15-06-94 b_noll added (void) into MK_AUTO??IT

******************************************************************************/

#ifndef AUTO_H
#define AUTO_H

/**************************************
	    Includes
**************************************/

/* none */

/**************************************
	    Global Variables
**************************************/

/* none */

/**************************************
	    Defines & Structures
**************************************/

/* ---- this is not necessary, if we are only using the named methods */
#ifndef _DCC
#define __autoinit
#define __autoexit
#endif

/**************************************
	    Macros
**************************************/

/* ---- Method # 1 - compiler independent   */
/*	the work is done in AUTO.c and with */
/*	a scanner			    */

#define AUTOINIT(x)
#define AUTOEXIT(x)


/* ---- Method # 2 - use compiler techniques, if possible */

#if defined(_DCC)
#   define SUPPORT_AUTO 1
#   define	MK_AUTOINIT(func_name) __autoinit void func_name (void)
#   define	MK_AUTOEXIT(func_name) __autoexit void func_name (void)
#endif

#if defined(__SASC)
#   define SUPPORT_AUTO 1
#   define	MK_AUTOINIT(func_name) void _STI_ ## func_name (void)
#   define	MK_AUTOEXIT(func_name) void _STD_ ## func_name (void)
#endif


/* ---- other compilers which support auto-entry/exit */
/*	features should be treated here 	      */



/* ---- ok, no compiler support; then we also need a */
/*	scanner here to invoke these functionality   */
#ifndef    SUPPORT_AUTO
#   define SUPPORT_AUTO 0
#   define	MK_AUTOINIT(func_name) void func_name (void)
#   define	MK_AUTOEXIT(func_name) void func_name (void)
#endif

/**************************************
	    Prototypes
**************************************/

/* ---- that function is recommended to be called at the beginning of	 */
/*	main(); it is used for method # 1 in any case, and for method 2, */
/*	if the used compiler is not known to support own auto-entry/exit */
/*	techniques							 */
#if SUPPORT_AUTO
#   define  AUTO_Init()
#else
extern void AUTO_Init(void);
#endif


#endif /* AUTO_H */

/******************************************************************************
*****  END AUTO.h
******************************************************************************/


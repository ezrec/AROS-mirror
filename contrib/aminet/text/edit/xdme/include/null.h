
/*
**  NULL.H
**
**  Definition file for changes
**  which have been introduced
**  by Bernd Noll
*/

#define abort(x)    { SETF_ABORTCOMMAND(Ep,1); return(x); }
#define abort2()    { SETF_ABORTCOMMAND(Ep,1); return; }

#define LINE_LENGTH MAXLINELEN
#define BUFFERS     1				    /* # of linebuffers */
#define NARGS	    8				    /* # of arguments passed to a command (including name) */

/* #define nomemory()  globalflags.memoryfail = 1 */
/* #define aborted	(globalflags.Abortcommand != 0) */
#define simplehash(s0,s1) (((s0 & 15) << 4) + (s1 & 15))
/* #define min(a,b) (a < b ? a : b) */

#define VAR_NEX 0  /* not existing variable */
#define VAR_FPK 1  /* explicite access to another ("foreign") package */
#define VAR_SF	2  /* dme special flags */
#define VAR_SI	3  /* dme special integer variable */
#define VAR_SV	4  /* dme special vars scanf/filename/... */
#define VAR_MF	5  /* dme macro   flag */
#define VAR_TF	6  /* dme text	  flag */
#define VAR_PF	7  /* dme package flag */
#define VAR_GF	8  /* dme global  flag */
#define VAR_TV	9  /* dme text	  variable */
#define VAR_MV	10 /* dme macro   variable */
#define VAR_PV	11 /* dme package variable */
#define VAR_GV	12 /* dme global  variable */
#define VAR_ARG 13 /* dme macro   parameter */
#define VAR_ENV 14 /* CBM env: variable */
#define VAR_SH	15 /* CBM local shell-var		 (FUTURE) */
#define VAR_CLP 16 /* rexx cliplist - entry */
#define VAR_RXX 17 /* rexx variable via RVI */
#define VAR_RXF 18 /* rexx result of functioncall */
#define VAR_MAP 19 /* dme package key-mapping */
#define VAR_MEN 20 /* dme package menu */
#define VAR_MNX 21 /* Arp shell-var			 (FUTURE) */

#define VAR_DME VAR_GV /* alias */


#define RET_SUCC    1
#define RET_FAIL    0

#define  BP  D(bug("XDME: Line %4ld in File %s\n", __LINE__, __FILE__));
#define DBP DD(bug("XDME: Line %4ld in File %s\n", __LINE__, __FILE__));

#define VarRoot 	void*
#define GetVarFrom	GetVarFromTree
#define InitVars(x)     *(x) = NULL
#define SetVarInto	SetVarIntoTree
#define DelAllVarsFrom	DelAllVarsFromTree

#include "gen_Flags.h"


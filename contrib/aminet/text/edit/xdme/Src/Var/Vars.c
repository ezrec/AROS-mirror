
/*
**  VARS.C
**
**	Variable Support for [X]DME
**
**  (C)Copyright 1992 by Bernd Noll for null/zero-soft
**  All Rights Reserved
**
**  RCS Header: $Id$
**
**  compiling with
**  "#define PATCH_NULL"
**  "#define PATCH_VARS"
**  "#define PATCH_FLAGS"
**  causes that module to be used (and var.c not to be used)
**
*!***************************************************************************
*!
*!  VARIABLES - OVERVIEW
*!
*!
*!  the vars-module shall support access to different types of
*!  variables
*!  which are currently "special" vars/flags, "global" vars/flags,
*!  "textlocal" vars/flags, "macrolocal" vars and "env" vars
*!  also keybindings and menuitems are accessible via variables
*!  in some versions of xdme also rexxclips, rexxvars, and macro-bodies
*!  might be accessible via the variable interface, but that feature
*!  has been dropped due to less feedback
**  (there are some other types used for DME, but not for XDME yet)
*!
*!
*!  WARNING:
*!	as this module is written very close
*!	to DME's internal hierachy, it will be object of major changes,
**	as soon as Packages are introduced as a new concept for management
**	of macros, menues, keys, flags and variables
*!	in fact, that means, that funcionality is dropped as soon, as
*!	the C-Interpreter is available
*!
*! some of the different VARIABLE command are following ...
*!
**
**
**  GetTypedVar / SetTypedVar / VAR_...types/prefixes
**
**  the "Highlevel" interface to most other variable modules,
**  which should most times be used in Programming -
**  Cleanest Possiblility of Access to Variables of any Type.
**
**  We have put all variable functions, which do not need access
**  to certain structures (rexx.../argstack...), together in that
**  module and try to export only a very small interface, which
**  should allow information hiding
**
**  this module should completely cover the "old" module var.c
**  so I put var.c into '#ifndef PATCH_VARS'
**
*****************************************************************
**
**  Implementor's note
**
**	The module looks pretty ugly,
**	I am not ready documenting and porting all
**	functions which had been written for DME
**	into XDME partly as many differences have occurred
**	partly as I have started designing a Packages-concept
**
**  XDME NOTE - the flags - routines are not used yet for XDME
*/

/**************************************
		Includes
**************************************/
#include "defs.h"


/**************************************
	   Internal Prototypes
**************************************/
Prototype void	    init_variables  (void);
Prototype char *    GetTypedVar     (const char *, int *);
Prototype int	    SetTypedVar     (const char *, const char *, int);
extern	  void	    SetDMEVar	    (char *, char *);   /* Proto'd for access from scanf.c */
extern	  void	    SetTextVar	    (char *, char *);
extern	  void	    SetDEnv	    (char *, char *);
extern	  char *    GetDMEVar	    (char *);
extern	  char *    GetTextVar	    (char *);
extern	  char *    GetEnvVar	    (char *);
extern	  char *    GetDEnv	    (char *);
Prototype void	    do_set	    (void);
Prototype void	    do_unset	    (void);
Prototype void	    do_setenv	    (void);
Prototype void	    do_unsetenv     (void);
Prototype void	    do_settvar	    (void);
Prototype void	    do_unsettvar    (void);
Prototype int	    is_tflagset     (int);              /* Proto'd for access from control.c */
Prototype int	    is_gflagset     (int);              /* Proto'd for access from control.c */
extern	  char *    GetTextFlag     (char *);
extern	  char *    GetGlobalFlag   (char *);
extern	  char	    SetAnyFlag	    (char *, char *);
extern	  char	    SetGlobalFlag   (char *, char *);
extern	  char	    SetTextFlag     (char *, char *);
Prototype void	    do_toggleflag   (void);
Prototype char *    getvar	    (const char *);


/**************************************
	    Internal Variables
**************************************/
#ifndef VAR_NUM
#define VAR_NUM 8
#endif

#ifndef SIGN_LOCAL_FLAG
# define SIGN_LOCAL_FLAG 't'
#endif

#ifdef DBASE
# define SList	  DmeBase.SList
# define VarsTree DmeBase.SList
#else
APTR	VarsTree = NULL;
#endif






/*
**  init_variables()
**	initialize the "global" variables structures
**	call that function from main or make it '__autoinit'
*/

void
init_variables (void)
{
    VarsTree = NULL;
} /* init_variables */






/***************************************************************
**
**  Access to the different kinds of Named Variables
**
***************************************************************/



/**************************************
**  Handling of DME "global" Variables
** (if we are using Packages "global" might be the wrong name)
**************************************/

char *
GetDMEVar (char * name)
{
    return (GetVarFromTree(&VarsTree, name));
} /* GetDMEVar */



void
SetDMEVar (char * name, char * value)
{
    SetVarIntoTree (&VarsTree, name, value);
} /* SetDMEVar */



/*
*! >SET name value
*! >UNSET name
*!
*!  set a std DME variable to a new value or drop it and its contents
*!  if the name is only a number, set the according dme std flag
*!
*!  NOTE that if packages are ready, SET should default to PVars/PFlags,
*!	not to GVars/GFlags
*!
*/

void
do_set (void)
{
    if (!SetGlobalFlag(av[1], av[2]))
	SetDMEVar (av[1],av[2]);
} /* do_set */



void
do_unset (void)
{
    DelVarFromTree (&VarsTree, av[1]);
} /* do_unset */






/**************************************
**  Handling of Text Local Variables
**************************************/

char *
GetTextVar (char * name)
{
    return (GetVarFromTree (&Ep->textvars, name));
} /* GetTextVar */



void
SetTextVar (char * name, char * value)
{
    SetVarIntoTree (&Ep->textvars, name, value);
} /* SetTextVar */



/*
*! >SETTVAR name value
*! >UNSETTVAR name
*!
*!  set a text local variable to a new value or drop it and its contents
*!  if the name is only a "t"number, set the according text local flag
*!
*/

void
do_settvar (void)
{
    if (!SetTextFlag(av[1], av[2]))
	SetTextVar (av[1],av[2]);
} /* do_settvar */



void
do_unsettvar (void)
{
    DelVarFromTree (&Ep->textvars, av[1]);
} /* do_unsettvar */






/***************************************************************
**
**  Access to Environment variables
**
***************************************************************/



char *
GetDEnv (char * ename)
{
    char * str	   = NULL;

#ifdef AMIGA
    long   siz;
    char buffer[LINE_LENGTH];

    if ((siz = GetVar (ename, buffer, 256, 0)) >= 0)
	if ((str = malloc (siz + 1)))
	    strcpy (str, buffer);
#endif

#ifdef UNIX
    if (str = getenv (ename))
	if (str = strdup(str))
	    return (str);
	else
	    nomemory();
    return (NULL);
#endif

    return (str);
} /* GetDEnv */



void
SetDEnv (char * ename, char * econt)
{
#ifdef AMIGA
    SetVar (ename, econt, -1, GVF_GLOBAL_ONLY);
#endif

#ifdef UNIX
    setenv (ename, econt, TRUE);
#endif
} /* SetDEnv */



char *
GetEnvVar(char * name)
{
    char * str;

    mountrequest(0);
    str = GetDEnv(name);
    mountrequest(1);
    return(str);
} /* GetEnvVar */



/*
*! >SETENV name value
*! >UNSETENV name
*!
*!  set an Environment variable to a new value or drop it and its contents
*/

void do_setenv (void)
{
    SetDEnv ((char *)av[1], (char *)av[2]);
} /* do_setenv */



void do_unsetenv (void)
{
#ifdef AMIGA
    DeleteVar (av[1], 0);
#endif

#ifdef UNIX
    unsetenv(av[1]);
#endif
} /* do_unsetenv */






/***************************************************************
**
**  Access to the different kinds of Flags
**
***************************************************************/


/**************************************
**  Handling of TEXT Local Flags
**************************************/

int is_tflagset(int num)
{
    return (IsFlagSet(Ep->textflags, num, MAX_TEXTFLAGS));
} /* is_tflagset */



char SetTextFlag(char * name, char *  value)
{
    return (SetFlag(Ep->textflags, name, value, MAX_TEXTFLAGS, "t"));
} /* SetTextFlag */



/*
 *  GetTextFlag: get a variable of a text's internal flags
 */

char * GetTextFlag (char * name)
{
    return (GetFlag(Ep->textflags, name, MAX_TEXTFLAGS, "t"));
} /* GetTextFlag */





#ifdef DBASE
# define tg DmeBase.gflags
#else
char tg[MAXTOGGLE/8]; /* test purposes */
#endif /* DBASE */

/**************************************
**  Handling of DME Global Flags
**************************************/

int is_gflagset (int num)
{
    return (IsFlagSet(tg, num, MAXTOGGLE));
} /* is_gflagset */



/*
 *  GetGlobalFlag : get a variable from dme's flag-list
 */

char SetGlobalFlag(char * name, char * value)
{
    return(SetFlag(tg, name, value, MAXTOGGLE, "")); /* "g" */
} /* SetGlobalFlag */



char * GetGlobalFlag (char * name)
{
    return(GetFlag(tg, name, MAXTOGGLE, ""));   /* "g" */
} /* GetGlobalFlag */






/**************************************
**  Interface to come around with all types of flags
**************************************/

char
SetAnyFlag(char * name, char * qual)
{
    if (name && qual)
    {
	if (is_number(name))
	{
	    return(SetGlobalFlag(name, qual));

#if 0
	} if (!is_digit(name[1])) {             /* Commented out for XDME */
	     return(SetSpecialFlag(name, qual));/* Commented out for XDME */
#endif

	} else if (name[0] == SIGN_LOCAL_FLAG)
	{
	    return(SetTextFlag(name, qual));

	} else
	{
	    /* SETF_ABORTCOMMAND(Ep,1); */
	    return 0;
	} /* if */
    } else
    {
	abort(0);
    } /* if */
} /* SetAnyFlag */



/*
*! >FLAG flagname on/off/toggle/switch/0/1/set/reset/true/false
*!
*!  set any flag that is controlled by DME
*!  with flagname is number, "t"number or specialflagname
*!
*/

DEFUSERCMD("flag", 2, CF_VWM|CF_COK|CF_ICO, void, do_flag, (void),)
{
    if (!SetAnyFlag(av[1],av[2]))
	abort2();
} /* do_flag */



/*
*! >[[RE]SET]TOGGLE flagname
*!
*!  modify any flag, that is controlled by DME
*!  with flagname is number, "i"number or specialflag
*!
*/

void
do_toggleflag (void)
{
    SetAnyFlag(av[1], av[0]);
} /* do_toggleflag */



/***************************************************************
**
**  Interface to be used for external functions and commands
**
***************************************************************/



/*
** the different types of "variables",
** which seem to be possible to be recognized
**
** these definitions must be visible to all modules
*/

#if 0
// #define VAR_NEX 0  /* not existing variable */
// #define VAR_FPK 1  /* explicite access to another ("foreign") package */
// #define VAR_SF  2  /* dme special flags */
// #define VAR_SI  3  /* dme special integer variable */
// #define VAR_SV  4  /* dme special vars scanf/filename/... */
// #define VAR_MF  5  /* dme macro   flag */
// #define VAR_TF  6  /* dme text    flag */
// #define VAR_PF  7  /* dme package flag */
// #define VAR_GF  8  /* dme global  flag */
// #define VAR_TV  9  /* dme text    variable */
// #define VAR_MV  10 /* dme macro   variable */
// #define VAR_PV  11 /* dme package variable */
// #define VAR_GV  12 /* dme global  variable */
// #define VAR_ARG 13 /* dme macro   parameter */
// #define VAR_ENV 14 /* CBM env: variable */
// #define VAR_SH  15 /* CBM local shell-var		    (FUTURE) */
// #define VAR_CLP 16 /* rexx cliplist - entry */
// #define VAR_RXX 17 /* rexx variable via RVI */
// #define VAR_RXF 18 /* rexx result of functioncall */
// #define VAR_MAP 19 /* dme package key-mapping */
// #define VAR_MEN 20 /* dme package menu */
// #define VAR_MNX 21 /* Arp shell-var			    (FUTURE) */

// #define VAR_DME VAR_GV /* alias */
#endif

#define VF_COP	1  /* duplicate the result */
#define VF_PAW	2  /* Prefix AlWays: dont use without Prefix (FUTURE) */


typedef struct _vtype
{
    const char * name;	/* prefix */
    int     id; 	/* number for communication */
    int     len;	/* size of prefix for comparison */
    int     offset;	/* offset to cut prefix */
    int     flags;	/* flags (e.g. duplicate result ...) */
    char*   replace;	/* if prefix matches, replace w/that prefix */
    void    (*do_set)(char*,char*); /* set-function */
    char*   (*do_get)(char*);       /* get-function */
} VTYPE;


#ifndef __GNUC__
#define NG_BRA {
#define NG_KET }
#else
#define NG_BRA
#define NG_KET
#endif

static CONST VTYPE vartypes[] =
{
#ifdef N_DEF
    NG_BRA "SHVAR_",  VAR_SH , 6, 6,     0, NULL, NULL, NULL NG_KET,
    NG_BRA "ARP_",    VAR_MNX, 4, 4,     0, NULL, NULL, NULL NG_KET,
    NG_BRA "RXFUNC ", VAR_RXF, 7, 7,     0, NULL, NULL, getQ NG_KET,
    NG_BRA "RXSFUNC ",VAR_RXF, 8, 8,     0, NULL, NULL, getQ NG_KET,
#endif
#if 0
    NG_BRA "SFLAG_",  VAR_SF,  6, 6,     0, NULL,     (void (*)(char*,char*))SetSpecialFlag, GetSpecialFlag NG_KET,    /* we need 3 names */
    NG_BRA "SINT_",   VAR_SI,  5, 5,     0, NULL,     (void (*)(char*,char*))SetSpecialInt,  GetSpecialInt NG_KET,     /* for the 3 different types (or we have to put them together) */
    NG_BRA "SPC_",    VAR_SV,  4, 4,     0, NULL,     (void (*)(char*,char*))SetSpecialVar,  GetSpecialVar NG_KET,     /* of special vars else there are big problems */
#else
    NG_BRA "SPC_",    VAR_SV,  4, 4,     0, NULL,     (void (*)(char*,char*))SPC_set,  SPC_get NG_KET,                 /* all spc vars put together */
#endif
    NG_BRA "TFLAG_",  VAR_TF,  6, 6,     0, "t%s",    (void (*)(char*,char*))SetTextFlag,    GetTextFlag NG_KET,
    NG_BRA "GFLAG_",  VAR_GF,  6, 6,     0, NULL,     (void (*)(char*,char*))SetGlobalFlag,  GetGlobalFlag NG_KET,  /* ifdef PATCH_PACK set replace to "g%s" */
    NG_BRA "ARG_",    VAR_ARG, 4, 4,     0, "arg%s",  NULL,           getmacroarg NG_KET,
    NG_BRA "arg",     VAR_ARG, 3, 0,     0, NULL,     NULL,           getmacroarg NG_KET,
    NG_BRA "MVAR_",   VAR_MV,  5, 5,     0, NULL,     SetMacroVar,    getmacrovar NG_KET,
    NG_BRA "TVAR_",   VAR_TV,  5, 5,     0, NULL,     SetTextVar,     GetTextVar NG_KET,
    NG_BRA "GVAR_",   VAR_GV,  5, 5,     0, NULL,     SetDMEVar,      GetDMEVar NG_KET,
    NG_BRA "ENV_",    VAR_ENV, 4, 4,     0, NULL,     SetDEnv,        GetEnvVar NG_KET,
#ifdef PATCH_RXCLIPS
    NG_BRA "RXCLP_",  VAR_CLP, 6, 6,     0, NULL,     SetRexxClip,    GetRexxClip NG_KET,
#endif /* PATCH_RXCLIPS */
#ifdef PATCH_RXVARS
    NG_BRA "RXVAR_",  VAR_RXX, 6, 6,     0, NULL,     setrexxvar,     getrexxvar NG_KET,
#endif /* PATCH_RXVARS */
    NG_BRA "KEY_",    VAR_MAP, 4, 4,VF_COP, NULL,     (void (*)(char*,char*))mapkey,         keyspectomacro NG_KET,
    NG_BRA "MENU_",   VAR_MEN, 5, 5,VF_COP, NULL,     NULL,           menutomacro NG_KET,
    NG_BRA NULL,      VAR_NEX, 0, 0,	 0, NULL,     NULL, NULL NG_KET
};



/*
**  GetTypedVar()
**	this function is nearly the same as getvar from cmd2.c
**	major differences:
**	* it tells where it has found a variable;
**	* first we check if the search-name matches a certain
**	  type-prefix and if it does, we use that type and its
**	  result (w/out respect if result != NULL)
**	* then we check all types until we get a non-NULL result
**	  or an abortion
**	* the end of the vartypes-list means - there is no variable
**	  of that name
**	(type may be NULL)
*/

char *
GetTypedVar (const char * find, int *  type)
{
    char  * found	= NULL;
    int     itype	= VAR_NEX;
    const VTYPE * vt;
    char    inter_abort = GETF_ABORTCOMMAND(Ep);

    if (type)
	*type = VAR_NEX;

    if (find == NULL)
    {		/* is there a name ? */
	abort (NULL);
    } else
    if (find[0] == '\0')
    { /* is it really a name ? */
	return (NULL);
    } /* if name corrupt */

    SETF_ABORTCOMMAND(Ep,0);

    for (vt = vartypes; vt->id != VAR_NEX && GETF_ABORTCOMMAND(Ep) == 0; vt++)
    {
	if (strncmp(find, vt->name, vt->len) == 0)
	{
	    char* ptr = find+vt->offset;
	    if (vt->replace)
	    {
		sprintf(tmp_buffer, vt->replace, ptr);
		ptr = tmp_buffer;
	    } /* if prefix replacement */
	    itype = vt->id;
	    found = (*vt->do_get)(ptr);
	    if (found)
	    {
		if (vt->flags & VF_COP)
		{
		    found = strdup(found);
		} /* if dup'd */
	    } else
	    {
		SETF_ABORTCOMMAND(Ep,1);
	    } /* if res(!)=0 */
	    goto gv_quit;
	} /* if matching prefix */
    } /* for all types */

    for (vt = vartypes; vt->id != VAR_NEX && GETF_ABORTCOMMAND(Ep) == 0; vt++)
    {
	if (vt->do_get)
	{
	    if ((found = (*vt->do_get)(find)))
	    {
		itype = vt->id;
		if (vt->flags & VF_COP)
		{
		    found = strdup(found);
		} /* if dup'd */
		goto gv_quit;
	    } /* if res#0 */
	} /* if ex det-func */
    } /* for all types */

gv_quit:

/* if (GETF_DEBUG(Ep)) printf("\tgetvar:prefix/err/oerr/val == %s/%d/%d/%s\n",vt->name, (int)Abortcommand, (int)inter_abort, found); */

    SETF_ABORTCOMMAND(Ep,GETF_ABORTCOMMAND(Ep) || inter_abort);

    if (type)
	*type = itype;

    return (found);
} /* GetTypedVar */



/*
**  Set a Variable
**	if it has a known type, use the associated set-command
**	else check if there is a matching prefix
**	if nothing fits - abort
*/

int
SetTypedVar (const char * name, const char * value, int type)
{
    const VTYPE* vt;

    if ((name == NULL) || (name[0] == '\0'))
    { /* is it a name ? */
	SETF_ABORTCOMMAND(Ep,1);
	return 0;
    } /* if name corrupt */

    if (type == VAR_NEX)
    {
	for (vt = vartypes; vt->id != VAR_NEX && GETF_ABORTCOMMAND(Ep) == 0; vt++)
	{
	    if (strncmp(name, vt->name, vt->len) == 0)
	    {
		char* ptr = name+vt->offset;
		if (vt->do_set != NULL && name[vt->offset] != 0)
		{
		    if (vt->replace)
		    {
			sprintf(tmp_buffer, vt->replace, ptr);
			ptr = tmp_buffer;
		    } /* if prefix replacement */
		    (*vt->do_set)(ptr, value);
		    return 1;
		} else
		{
		    SETF_ABORTCOMMAND(Ep,1);
		    return 0;
		} /* if (no) set-function */
	    } /* if matching prefix */
	} /* for all types */
	SETF_ABORTCOMMAND(Ep,1);
	return 0;
    } /* if unknown type */

    for (vt = vartypes; vt->id != VAR_NEX; vt++)
    {
	if (vt->id == type)
	{
	    if (vt->do_set)
	    {
		if (strncmp(name, vt->name, vt->len) == 0)
		{
		    char* ptr = name+vt->offset;
		    if (vt->replace)
		    {
			sprintf(tmp_buffer, vt->replace, ptr);
			ptr = tmp_buffer;
		    } /* if prefix replacement */
		    (*vt->do_set)(ptr, value);
		} else
		{
		    (*vt->do_set)(name, value);
		} /* if (no) matching prefix */
		return 1;
	    } else
	    {
		SETF_ABORTCOMMAND(Ep,1);
		return 0;
	    } /* if (no) set-function */
	} /* if id found */
    } /* for all types */

    SETF_ABORTCOMMAND(Ep,1);
    return 0;
} /* SetTypedVar */



/*
**  Search
**  (1) special Variables (2) Flags (3) macro's list,
**  (4) text's list, (5) internal list, (6) enviroment,
**  (7) Rexx cliplist (8) macros.  The variable is allocated
**  with malloc().  NULL if not found.  ENV: need not exist.
*/

char * getvar (const char * find)
{
    return (GetTypedVar(find,NULL));
} /* getvar */

#if 0
DEFUSERCMD("resettoggle", 1, CF_VWM|CF_COK|CF_ICO, void, do_toggleflag, (void),)
DEFUSERCMD("settoggle", 1, CF_VWM|CF_COK|CF_ICO, void, do_toggleflag, (void),)
DEFUSERCMD("settvar", 2, CF_VWM|CF_ICO|CF_COK, void, do_settvar, (void),)
DEFUSERCMD("toggle", 1, CF_VWM|CF_COK|CF_ICO, void, do_toggleflag, (void),)
DEFUSERCMD("unset", 1, CF_VWM|CF_COK|CF_ICO, void, do_unset, (void),)
DEFUSERCMD("unsetenv", 1, CF_VWM|CF_COK|CF_ICO, void, do_unsetenv, (void),)
DEFUSERCMD("unsettvar", 1, CF_VWM|CF_ICO|CF_COK, void, do_unsettvar, (void),)
DEFUSERCMD("set", 2, CF_VWM|CF_COK|CF_ICO, void, do_set, (void),)
DEFUSERCMD("setenv", 2, CF_VWM|CF_COK|CF_ICO, void, do_setenv, (void),)
DEFUSERCMD("spc", 2, CF_VWM|CF_COK|CF_ICO, void, do_setspecialvar, (void),)
#endif

/******************************************************************************
*****  END vars.c
******************************************************************************/

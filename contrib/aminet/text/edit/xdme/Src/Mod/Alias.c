/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	macros for DME/XDME

    NOTES
	a part of that module is in command.c

    BUGS
	<none known>

    TODO
	-/-

    EXAMPLES
	-/-

    SEE ALSO
	command.c

    INDEX

******************************************************************************/

/*
**  (C)Copyright 1992 by Bernd Noll for null/zero-soft
**  All Rights Reserved
**
**  RCS Header: $Id$
**
**  compiling with
**  "#define PATCH_NULL"
**  "#define PATCH_MACROS"
**  causes that module to be used
**
*!*********************************************************************
*!
*!  MACROS - OVERVIEW
**
**  this option allows translation of alias.c
**  and enables a change in do_command() to allow macro-execution
**  and (perhaps) one in main() to initialize the macro-lists
*!
*!  the module supports several commands:
*!  SETMACRO		definition of a new macro
*!  UNSETMACRO		deletion of a macro
*!
*!  SETMACROVAR 	definition of a macro's local variable
*!  UNSETMACROVAR	deletion of suh a variable
*!		    (at the end of a macrocall all its local variables
*!		     are deleted automatically)
*!
***********************************************************************
**
**  Implementation:
**
**  At the moment macros are stored in MACRO_NUM Lists, which
**  are selected with the 1st char of the macro's name
**  to speed up search-operations
**
**  All names are switched to lowercase, so there should be
**  no case sensitivity
**
**  please note, that there is acess to "av[]" and to several functions of
**  the module "varsbases" in this module, so porting to another program
**  than DME might need some changes.
**
*/


/**************************************
		Includes
**************************************/
#define MACRO struct MacroNode /* trick 17 * OPAQUE */
#include "defs.h"
#include "AVL.h"

#define MYDEBUG     0
#include "debug.h"



/**************************************
      Interne Defines & Struktures
**************************************/
/*
** U might use MACRO like an opaque type
*/


struct MacroNode {
	struct Node Node;   /* includes a ptr to the macro-name */
	char*	    Body;   /* statements to be executed at a macro-call (this all must fit in one line) */
	short	    NArgs;  /* 0..7 number of arguments to be given from do_command */
	short	    NFlags; /* PERHAPS FUTURE: 0..n number of local-flags to be supported */
	char	    freeme; /* NOT USED ANY MORE 0/1 flag if the macro is builtin or user-defined */
	char	    killme; /* 0/1 flag if he maxro shall be deleted after termination of its last macrocall */
	short	    NCalls; /* it seems to be necessary to forbid all deletions of an active macro */
};


struct MacroArg {
	struct MinNode	 ma_Node;	 /* hook for recursion-stack */
	struct MacroNode*ma_Macro;	 /* a reference to the called macro w/ all its data */
	VarRoot 	 ma_Locals;	 /* AVL-Tree or List of Local Variables */ /* we could also have a ptr to an array of ptrs instead of a list, but then it might be necessary to use one more parameter for SETMACRO */
	char*		 ma_Arguments[2];/* array of ptr to string; there must always be at least two entries: a 1st one referencing the macroname and a last one == NULL */
};

/**************************************
	   Prototypes & Exports
**************************************/

Prototype void	do_setmacro	(void);
Prototype void	do_unsetmacro	(void);
Prototype void	do_setmacrovar	(void);
Prototype void	do_unsetmacrovar(void);
Prototype void	do_retmacro	(void);
Prototype void	do_macrosave	(void);
Prototype void	do_macroload	(void);
Prototype void	SetMacroVar	(char* name, char* value);
Prototype MACRO*getmacro	(char* name);
Prototype int	callmacro	(MACRO*);
Prototype int	nummacroargs	(MACRO*);
Prototype char* getmacroarg	(char* name);
Prototype char* getmacrovar	(char* name);
Prototype void	DelMacros	(void);
Prototype int	SetMacro	(char* name, char* body, int nargs);

Prototype int	 savemacros	 (FILE *);          /* PATCH_NULL [25 Jan 1993] : proto'd for pack.c */
Prototype int	 loadmacros	 (FILE *, int *);   /* PATCH_NULL [25 Jan 1993] : proto'd for pack.c */
Prototype int	 GetDefMacros	 (void);            /* PATCH_NULL [25 Jan 1993] : proto'd for pack.c */

/*
**  usually I use a global Struct that contains all important
**  variables - so the Macrolists too, that is to allow
**  switching between several environments in future
**  if that global structure is not used, we can support our
**  own - local - variable, else we must access the lists via a
**  macro.
**  the argstack is not touched by that division, as command flow cannot
**  be splitted
**  !WARNING!	it CAN be splitted when using asynchroneous Rexx-ports
**		but when writing that module, i did not intend to
**		use really async rexx-commands
**
**  NOTE
*/

/**************************************
	    Internal Variables
**************************************/
    /*
    **	*argstack contains all data which is created during one macro-call
    **	*delenda is a delete-buffer for all macros, will have been removed
    **	 from their lists, but cannot deleted, as there is a prior call to
    **	 not returned; they are deleted then aftermacro-termination or
    **	 in exit_macros.
    */
static struct TreeNode* macros = NULL;	/* ... OF MACRO */
static struct TreeNode* delenda= NULL;	/* ... OF MACRO */
static MLIST argstack;			/* ... OF struct MacroArg */
char  MacroBreak = 0;			/* internal possibility to terminate a macro */ /* PATCH_NULL [08 Mar 1993] : removed "static" */

/* extern int count_tree (APTR); */

/**********************************************
**
**  Definition and deletion of Macros
**
**********************************************/


/*
**  FreeMacro()
**  DelMacro()
**	2 help-Functions of do_unsetmacro (and exit_macros)
**
**    DelMacro() should delete only Macros, which are not in use
**	for that reason it checks usecount, and if its not 0, the
**	macro is put into another list and marked to be killed by
**	callmacro() after execution.
*/

static void
FreeMacro(MACRO* m)
{
    if (m->freeme) {                    /* if that macro is not a builtin one, */
	free(m->Node.ln_Name);          /* free it and its contents */
	free(m->Body);
	free(m);
    } /* if */
} /* FreeMacro */


static void
DelMacro(char* name)
{
    MACRO* m;

DL;
    if ((m = getmacro(name))) {         /* find the node representing the macro ... */
DL;
	AVL_Remove (&macros, (APTR)m);  /* , remove it from its tree ... */
DL;
	if (!m->NCalls) {               /* and if it is not in use */
DL;
	    FreeMacro(m);               /* ... free its contents */
	} else {			/* ... else put it into a certain kill-list */
	    m->killme++;		/* ... and mark it to be killed */
DL;
	    AVL_Append (&delenda, (APTR)m);
DL;
	} /* if */
    } /* if */
DL;
} /* DelMacro */



/*
**  SetMacro()
**	a help-Function of do_setmacro (and maybe init_macros)
**	(it is also called by packages.mergpack)
**	which creates a new macro-node and fills copies of the parameters
**
*/

int
SetMacro(char* name, char* body, int nargs)
{
    MACRO* m;

    if (nargs >= NARGS) {
	error ("too many arguments specified for macro");
	return(0);
    } /* if too many args */

DL;
    DelMacro(name);                                         /* for sure: remove all macros of the same name */

DL;
    if ((m = malloc(sizeof(MACRO)))) {                      /* allocate a macro-structure... */
	setmem(m, sizeof(MACRO),0);
	if ((m->Node.ln_Name = strdup(name))) {             /* ... the name ... */
	    if ((m->Body = strdup(body))) {                 /* ... and the body, */
		m->NArgs= nargs;			    /* fill in all values,  */
		m->freeme = 1;
		AVL_Append (&macros, (APTR)m);              /* and append the structure to a certain macro-list */
DL;
		return(1);
	    } /* if block copied */
	    free(m->Node.ln_Name);                          /* if anything went wrong */
	} /* if name copied */				    /* free all allocated frags */
	free(m);                                            /* and set an error */
    } /* if struct allocated */
DL;
    nomemory();
DL;
    return(0);
} /* SetMacro */



/**********************************************
**
**  Access to Macros' arguments and local variables
**
**********************************************/



/*
**  getmacrovar()
**	get a local variable of the current(== last) macrocall
**	we are supposing that only one process has access to resources
**	that function is meant to be interface for getvar()
*/

char*
getmacrovar(char* name)
{
    struct MacroArg* n;

    if (!(n = GetHead(&argstack))) /* PATCH_NULL [03 Apr 1993] */
    { /* this function can not work if not called from within a macrocall */
	return(NULL);
    } /* if */

    return(GetVarFrom (&n->ma_Locals, name));
} /* getmacrovar */


/*
**  SetMacroVar()
**	a help-function of do_setmacrovar()
**	and an interface to settypedvar()
*/

void
SetMacroVar(char* name, char* value)
{
    struct MacroArg* n;

    if (!(n = GetHead(&argstack))) { /* this function can not work if not called from within a macrocall */
	abort2();
    } /* if */

    SetVarInto (&n->ma_Locals, name, value);
} /* SetMacroVar */



/*
**  getmacroarg()
**	get an parameter of the current(== last) macrocall
**	we are supposing that only one process has access to resources
**	so we only have to check the topmost element of argstack
*!  NOTE:   ARGUMENTS to a macro are READ-ONLY,
*!	    You can't alter their values from the body of the called macro
*!
*/

char*
getmacroarg(char* name)
{
    struct MacroArg* n;
    int num;
    char* res;

    if (!(n = GetHead(&argstack))) { /* this function can not work if not called from within a macrocall */
	return(NULL);
    } /* if */

    if ((strncmp(name,"arg",3) != 0) || (!is_number(name+3))) {     /* and it responds only to names of the type "ARGx" with x in /N */
	return(NULL);
    } /* if */
    num = atoi(name+3);

    if (n->ma_Macro->NArgs>=num) {                                     /* if the last called function has an arg ARGx */
	if ((res = strdup (n->ma_Arguments[num]))) {                   /* and U can duplicate it, then return the copy */
	    return(res);
	} else {
	    nomemory();
	    abort(NULL);
	} /* if (not) malloced res */
    } else {
	abort(NULL);
    } /* if (not) exists argX */
} /* getmacroarg */




/**********************************************
**
**  Usage of Macros
**
**  for calls from do_command the sequence must
**  be like that:
**
**  if (m=getmacro) then
**	for i=1 to nummacroargs(m) do
**	    put nextarg to av[i]
**	od;
**	callmacro(m);
**  fi;
**
**  callmacro() pushes av[i] onto argstack and
**  clears them, then locks the macro and
**  calls do_command with a copy of the
**  macro's block
**  when the execution returns, av[i] are
**  restored and topof argstack is cleared
**
**********************************************/



/*
**  callmacro()
**	Call a Macro as interface from do_command()
**	(first you must have got a "handle" via getmacro())
*/

int
callmacro(MACRO* m)
{
    struct MacroArg* n;
    char** args;
    char*  com;
    int ret	= 0;
    int i;
    int len	= m->NArgs*4+sizeof(struct MacroArg);

    n	= (struct MacroArg*)malloc(len);                     /* allocate space for the stack-element... */
    com = strdup(m->Body);                      /* and duplicate the macro's body */

    if (n && com) {
	setmem(n,len,0);                        /* clear all data */
	AddHead((LIST*)&argstack,(NODE*)n);     /* and push the struct to argstack */

	InitVars (&n->ma_Locals);                  /* for use with encapsulated local variables we must initialize their list */

	av[0] = m->Node.ln_Name;		/* define argslot 0 - it might be asked during execution, and perhaps it was not set in do_command */
	args = n->ma_Arguments; 		   /* then put all arguments into the struct */
	for (i = m->NArgs; i >= 0; i--) {       /* and clear their global values */
	    args[i] = av[i];
	    av[i] = NULL;
	} /* for */

						/* [25 Jan 1993] : line added */
	n->ma_Macro = m;
	m->NCalls++;				/* disable deletion of that macro during execution */

	MacroBreak = 0; 			/* clear the return - flag */

	ret = do_command(com);                  /* and call the macrobody (execute only the copy, as do_command might destroy the data) */
	if (MacroBreak) {
	    ret = 1-ret;
	    SETF_ABORTCOMMAND(Ep, MacroBreak-1);
	    MacroBreak	 = 0;
	} /* if */

	for (i = m->NArgs; i >= 0; i--) {       /* after the call restore the global values of all args, ... */
	    av[i] = args[i];
	} /* for */
	m->NCalls--;				/* and enable deletion of that macro after execution */
	if (m->NCalls == 0 && m->killme) {      /* if we must delete the macro, do it */
	    AVL_Remove(&delenda, (APTR)m);
	    FreeMacro(m);
	} /* if */

	DelAllVarsFrom (&n->ma_Locals);            /* delete all local variables ... */

	Remove((NODE*)n);                       /* and pop the margs-struct from argstack */
    } else {
	nomemory();
    } /* if */

    if (n)                                      /* and free anything you allocated */
	free(n);
    if (com)
	free(com);

    return(ret);
} /* callmacro */



/*
**  getmacro()
**
**	Find a Function of a given Name
**	so macros can be tested for existance and user can get a ptr to a "Handle"
**	that function is part of the interface to do_command()
*/

MACRO*
getmacro(char* name)
{
    MACRO* m;

DL;
    m = (MACRO*)AVL_Find (&macros, name);
DL;
    return(m);
} /* getmacro */


#if 0
/*
**  That function might be used as an interface for getvar
**  to access the contents of a macro
*/

char*
GetFuncBody(char* name)
{
    MACRO* m = NULL;
    char* str = NULL;

    if (m = (MACRO*)getmacro(name)) {
	str = strdup(m->Body);
    } /* if */
    return(str);
} /* GetFuncBody */
#endif /* NOT_DEF */



/*
**  nummacroargs()
**	Get the Number of Arguments of a Macro
**	that function is used by do_command to get informations
**	how many argslots to fill
*/

int
nummacroargs(MACRO* m)
{
    return(m->NArgs);
} /* nummacroargs */



/**********************************************
**
**  Initial Access to Macros
**
**
**********************************************/



/*
**  resmacros
**	that structure shows all builtin macros
**	which are defined at program start
**	they are initialized via GetDefMacros
*/

typedef struct _mres
{
    char * name;
    char * body;
    int   numargs;
} MRES;

static CONST MRES resmacros[] =
{
     /*  name,		block,			  args */

	{ "alias",       "setmacro   $arg1 0 $arg2", 2 },
	{ "aslload",     "arpload",                  0 },
	{ "aslinsfile",  "arpinsfile",               0 },
	{ "aslfont",     "arpfont",                  0 },
	{ "aslsave",     "arpsave",                  0 },
	/* { "asl",     "arp",                  0 }, */
	{ "unalias",     "unsetmacro $arg1",         1 },
	{ "firstwindow", "select f",                 0 },
	{ "lastwindow",  "select l",                 0 },
	{ "nextwindow",  "select n",                 0 },
	{ "prevwindow",  "select p",                 0 },
	{ "so",          "if m saveold",             0 },
	{ NULL, 	 NULL,			     0 }
}; /* resmacros */


/*
**  GetDefMacros
**	initialize the builtin default-macros
*/

int
GetDefMacros (void)
{
    int i;
DL;
    for (i = 0; resmacros[i].name; i++)
    {
	D(bug("name=%s\n", resmacros[i].name));
	if (!SetMacro(resmacros[i].name, resmacros[i].body, resmacros[i].numargs)) {
	    return (0);                 /* PATCH_NULL [25 Jan 1993] : line added */
	} /* if error */		/* PATCH_NULL [25 Jan 1993] : line added */
    } /* for all builtins */

    DL;
    return (1);
} /* GetDefMacros */




/* That static variable is needed to communicate between savesinglemacro and savemacros */
static FILE * mfi;

void savesinglemacro (MACRO * m)
{
    if (m && mfi) {
	fprintf (mfi, "\tMACRO %s\n", m->Node.ln_Name);
	if (m->NArgs) {
	    fprintf (mfi, "\t ARGS %d\n", m->NArgs);
	} /* if */
	fprintf (mfi, "\t BODY %s\n", m->Body);
    } /* if */
} /* savesinglemacro */




int savemacros (FILE * fo)
{
    fprintf(fo, "MACROS START\n");

    mfi = fo;
    AVL_ScanTree (&macros, (APTR)savesinglemacro, 1);

    fprintf(fo, "MACROS END\n");
    return (1);
} /* savemacros */



int loadmacros (FILE * fi, int * lineno)
{
    char * buf;
    char   nmacro [128];
    char   nargs  [128];
    char   body   [2*LINE_LENGTH];

    buf = getnextcomline(fi, lineno);
    if (!buf) {
	abort(0);
    } /* if */
    if (strncmp(buf, "MACROS START", 12) != 0) {
	error ("No Macrostart header");
	abort(0);
    } /* if */

    nmacro[0] = 0;
    nargs [0] = 0;
    body  [0] = 0;

    while (GETF_ABORTCOMMAND(Ep) == 0) {
	buf = getnextcomline(fi, lineno);
	if (!buf) {
	    abort(0);
	} /* if */

	if (strncmp(buf, "MACROS END", 10) == 0) {
	    return(1);
	} else if (strncmp(buf, "MACRO ", 6) == 0) {
	    buf += 6;
	    strncpy(nmacro, buf, 128);
	} else if (strncmp(buf, "ARGS ", 5) == 0) {
	    buf += 5;
	    if (nargs[0] != 0) {
		error("%s:\nDeclared numargs not w/in a Macro", av[0]);
		abort(0);
	    } /* if */
	    strncpy(nargs, buf, 128);
	} else if (strncmp(buf, "BEGIN", 5) == 0) {
	    int len = 0;

	    buf += 5;
	    while (*buf && *buf<33) buf++;
	    if (nmacro[0] == 0) {
		error("<%s:\nDeclared Body w/out Macro", av[0]);
		abort(0);
	    } /* if */

	    if (*buf) {
		len = strlen(buf);
		strcpy (body, buf);
	    } /* if */

	    while (!GETF_ABORTCOMMAND(Ep) && (buf = getnextcomline(fi, lineno))) {
		char * inter = &body[len];
		if (strncmp (buf, "END", 3) == 0) {
		    goto setit;
		} /* if */
		len += strlen(buf)+1;
		if (len < 2*LINE_LENGTH) {
		    *inter = ' ';
		    strcpy(inter+1, buf);
		} else {
		    error ("%s:\nmacro-body too long", av[0]);
		} /* if */
	    } /* while */
	    goto retfail;

	} else if (strncmp(buf, "BODY", 4) == 0 && (buf[4] < 0x21)) {
	    buf += 4;
	    if (*buf) buf++;
	    if (nmacro[0] == 0) {
		error("<%s:\nDeclared Body w/out Macro", av[0]);
		abort(0);
	    } /* if */
	    strncpy(body, buf, 2*LINE_LENGTH);
setit:
	    av[1] = nmacro;
	    av[2] = nargs[0] ? nargs : "0";
	    av[3] = body;
	    do_setmacro();
	    nmacro[0] = 0;
	    nargs [0] = 0;
	} else {
	    error ("%s:\nunknown identifier '%s'", av[0], buf);
	} /* if types */
    } /* while not ready */

retfail:
    return(0);
} /* loadmacros */





/***************************************************

	    COMMAND INTERFACE

***************************************************/


/*
*! >SETMACROVAR name value
*! >SMV 	name value
*!	define a local variable for the current macro-call
*!	all local vars will be deleted at the end of a macro-execution
*!	(SMV is a shortcut for SETMACROVAR as we can't define a short
*!	 macro for that command)
*!
*/

/*DEFHELP #cmd aliases,vars SMV name value - create or redefine a macrolocal variable (same as SETMACROVAR) */
/*DEFHELP #cmd aliases,vars SETMACROVAR name value - create or redefine a macrolocal variable (same as SMV) */

DEFUSERCMD("smv",         2, CF_VWM|CF_ICO|CF_COK, void, do_setmacrovar, (void),;)
DEFUSERCMD("setmacrovar", 2, CF_VWM|CF_ICO|CF_COK, void, do_setmacrovar, (void),)
{
    SetMacroVar(av[1], av[2]);
} /* do_setmacrovar */


/*
*! >UNSETMACROVAR name
*!	drop a local variable before the end of the current macro-call
*!	all local vars will be deleted at the end of a macro-execution
*!	so that command may be obsolete
*!
*/

/*DEFHELP #cmd aliases,vars UNSETMACROVAR name - delete a macrolocal variable */

DEFUSERCMD("unsetmacrovar", 1, CF_VWM|CF_ICO|CF_COK, void, do_unsetmacrovar, (void),)
{
    struct MacroArg* n;

    if (!(n = GetHead(&argstack))) { /* this function can not work if not called from within a macrocall */
	abort2();
    } /* if */

    DelVarFromTree (&n->ma_Locals, av[1]);
} /* do_unsetmacrovar */


/*
*! >RET
*!	Terminate the current macro
*!
*/

/*DEFHELP #cmd aliases RET - return from a macro (before reaching its end) */

DEFUSERCMD("ret", 0, CF_VWM|CF_ICO|CF_COK, void, do_retmacro, (void),)
{
    MacroBreak = GETF_ABORTCOMMAND(Ep) ? 2 : 1;
    SETF_ABORTCOMMAND(Ep, 1);
} /* do_retmacro */


/*
*! >SETMACRO macroname numargs body
*!	create a macro definition
*!	please note that a macro-body is read twice once on definition
*!	and once on execution (variables should be excaped so)
*!
*!	after such a definition it is possible to use macroname like
*!	any DME-command; the following numargs expressions are used
*!	as the macros variables, they can be accessed for read with
*!	$arg0-$arg<numargs> (not for write)($arg0 returnes the macro's
*!	name)
*!
*!  BUG:    if numargs is not a number then this command
*!	    will assume a value of "0"
*!
*!  WARNING:	the synopsis of that command MIGHT change in near future, as
*!		I am thinking about use of macros' local flags or static vars
*!		so there might come up another number of local-flags ->
*!		SETMACRO name numargs NUMFLAGS|LIST-OF-NAMES body
*!
*/

/*DEFHELP #cmd aliases SETMACRO name nargs body - create or redefine a commandmacro */

DEFUSERCMD("setmacro", 3, CF_VWM|CF_ICO|CF_COK, void, do_setmacro, (void),)
{
    SetMacro(av[1], av[3], atoi(av[2]));
} /* do_setmacro */


/*
*!  >UNSETMACRO name
*!	remove a macro definition
*!
*/

/*DEFHELP #cmd aliases UNSETMACRO name - delete a commandmacro */

DEFUSERCMD("unsetmacro", 1, CF_VWM|CF_ICO|CF_COK, void, do_unsetmacro, (void),)
{
    DelMacro(av[1]);
} /* do_unsetmacro */


/*
*!  >MACROSAVE filename
*!	save the current macros into a file of a special format
*!
*/

/*DEFHELP #cmd aliases MACROSAVE filename - save all commandmacros into a file */

DEFUSERCMD("macrosave", 1, CF_VWM|CF_ICO, void, do_macrosave, (void),)
{
    FILE * fo;

    if ((fo = fopen(av[1], "w"))) {
	savemacros(fo);
	fclose(fo);
    } else {
	error ("%s:\nCan't open file %s for output", av[0], av[1]);
	abort2();
    } /* if */
} /* do_macrosave */


/*
*!  >MACROLOAD filename
*!	read a special macrofile
*!
*/

/*DEFHELP #cmd aliases MACROLOAD name -  load commandmacros from a file */

DEFUSERCMD("macroload", 1, CF_VWM|CF_ICO, void, do_macroload, (void),)
{
    FILE * fi;
    int    lineno = 0;

    if ((fi = fopen(av[1], "r"))) {
	loadmacros(fi, &lineno);
	fclose(fi);
    } else {
	error ("%s:\nCan't open file %s for input", av[0], av[1]);
	SETF_ABORTCOMMAND(Ep, 1);
    } /* if */
} /* do_macrosave */



#ifdef STATIC_COM

void* lockmvar (char *name) {
    if (!(n = GetHead(&argstack))) { /* this function can not work if not called from within a macrocall */
	return(NULL);
    } /* if */

    return(AVL_Find (&n->ma_Locals, name));
} /* lockmvar */


char *macrobody(MACRO *handle) {
    return handle->body;
}





#endif

#ifdef PATCH_NULL /* PATCH_NULL < 24-06-94 */
/* **************************************************
	STATIC XDME Command Interface
************************************************** //

//	    Name    #Args Function	    Flags

//	    Name  Pri Lock     Unlock	Call	   NArgs
:FX:COMTREE maro    1 getmacro return0	callmacro  nummacroargs

//	    Name  Pri Lock	  Unlock  Get	    Set     Unset
:FX:VARTREE marg    1 getmacroarg free	  retval    return0 return0
:FX:VARTREE mvar    1 lockmvar	  return0 nodeget   nodeset return0
:FX:VARTREE mbody  -1 getmacro	  return0 macrobody return0 return0

// **************************************************
	DYNAMIC XDME Command Interface
************************************************** */


/*
**  ALIAS_init ()
**
**	Init All Macro-Stuff
**	    init all name-lists
**	    init the arg-stack
**	    enable the builtin macros (if macros are global)
**
**	That function has to be called at programstart
**	(that is done by the line below)
*/

static const
struct CommandNode ALIAS_Commands[] = {
    COMMAND("setmacro",       3, CF_VWM|CF_ICO|CF_COK, (FPTR)do_setmacro      )
    COMMAND("unsetmacro",     1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_unsetmacro    )
    COMMAND("ret",            0, CF_VWM|CF_ICO|CF_COK, (FPTR)do_retmacro      )
    COMMAND("smv",            2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_setmacrovar   )
    COMMAND("setmacrovar",    2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_setmacrovar   )
    COMMAND("unsetmacrovar",  1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_unsetmacrovar )
    COMMAND("macroload",      1,        CF_VWM|CF_ICO, (FPTR)do_macroload     ) /* tester */
    COMMAND("macrosave",      1,        CF_VWM|CF_ICO, (FPTR)do_macrosave     ) /* tester */
};


MK_AUTOINIT( ALIAS_Init )
{
    int i;

    /* ---- init the local structures */
    macros  = NULL;
    delenda = NULL;
    NewList ((LIST*)&argstack);

#ifndef PATCH_PACK
    GetDefMacros ();
#endif /* PATCH_PACK */

    for (i = sizeof (ALIAS_Commands)/sizeof (struct CommandNode) - 1;i >= 0; --i) {
	COM_Add (&ALIAS_Commands[i]);
    } /* for */
} /* ALIAS_Init */


#if 0

MK_AUTOEXIT( ALIAS_Exit )
{
    int i;
    APTR lock;
    for (i = sizeof (ALIAS_Commands)/sizeof (struct CommandNode) - 1;i >= 0; --i) {
	if (lock = COM_Lock (ALIAS_Commands[i].cn_Name))
	    COM_Remove (lock);
} /* ALIAS_Exit */
#endif
#else

Prototype int init_macros (void);


int init_macros (void) {
    NewList ((LIST*)&argstack);

    delenda = NULL;
    macros  = NULL;
DL;
    return GetDefMacros ();

} /* init_macros */
#endif

/******************************************************************************
*****  ENDE macros.c
******************************************************************************/


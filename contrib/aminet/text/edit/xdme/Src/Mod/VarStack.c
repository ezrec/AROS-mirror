/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Variable Stack for DME/XDME

    NOTES
      * we are using the non-std functions
	 ???	error(template, ...);
	 int   SetTypedVar   (const char *name, const char *value, int type);
	 char *GetTypedVar (const char *name, int *type);
	and the argument array
	 UBYTE *av[];
	they have to be supported by the application

    BUGS
      * we are only saving the lower 8 bits of "type" from GetTypedVar!
	that behaviour if not planned to be changed in the near future.
	(However, if it's urgently needed, uncomment the "Type"-slot in
	"VRST" and multireplace "Node.ln_Type" with "Type")

    TODO
	-/-

    EXAMPLES
	-/-

    SEE ALSO
	vars.c

    INDEX

    HISTORY
	05-12-92 b_noll created
	26-08-93 VSTK_drop added
	24-06-94 renamed VARS to VRST
	05-09-94 added VSTK_Internal
	21-09-94 b_noll added DEFCMD/DEFHELP-support
	22-09-94 b_noll totally rewritten

******************************************************************************/

/***********************************************************
*!
*!  Variable Stacking:
*!
*!  Stacking for all structures, which may be read and
*!  written via GetTypedVar()
*!  (Please notice that stackable are only variables,
*!   not macros, menues or mappings (they are stackable, but
*!   popping or picking might fail))
*!
*!  Users should be aware to pop their entries from the stack
*!  when they are needed not any more
*!  (We have NOT planned to create a garbage-collector :-))
*!
************************************************************/

/**************************************
		Includes
**************************************/

#include "defs.h"
/* #include "COM.h" */
/* #include "AUTO.h" */

/**************************************
	    Globale Exports
**************************************/


/**************************************
      Interne Defines & Strukturen
**************************************/

/* some short names */
#define MLIST struct MinList
#define MNODE struct MinNode
#define  NODE struct Node
#define  LIST struct List


typedef struct _VRST {
    NODE    Node;
    /* int     Type; */
    char*   Str;
} VRST;


/* possible values for VSTK_PickPop - POP or !POP */
#define POP  0

/* module internal error codes */
#define VSTK_OK 	 0
#define VSTK_NOMEM	-1
#define VSTK_NOVAR	-2
#define VSTK_NOENTRY	-3
#define VSTK_NOTSET	-4


/* is it sensible to put _that_ into an own function? */
#define VSTK_FindEntry(n) ((VRST *)FindName((LIST *)&varstack, (n)))

/**************************************
	   Interne Prototypes
**************************************/


/**************************************
	   Impementation
**************************************/

// int VSTK_Internal = 0;
static MLIST varstack =
{ (MNODE*)&varstack.mlh_Tail, NULL, (MNODE*)&varstack.mlh_Head };

/*************************************************
	    Entry Level Functions
*************************************************/


/* delete an entry */

static void VSTK_FreeEntry (VRST *v) {
    if (v) {
	free(v->Str);
	free(v->Node.ln_Name);
	free(v);
    } /* if */
} /* VSTK_FreeEntry */


/* remove and delete an entry */

static int VSTK_DropEntry (VRST *v) {
    /* ---- Remove an entry from the stack, and free its contents and itself */

    if (v) {
	Remove((NODE*)v);
	VSTK_FreeEntry(v);
	return VSTK_OK;
    } /* if */
    return VSTK_NOENTRY;
} /* VSTK_DropEntry */


/* Restore avariable from an entry and eventually delete it */

static int VSTK_PickPopEntry (VRST *v, int mode) {
    /* ---- restore the value of an entry */
    if (v) {
	VSTK_Internal = 1;

	/* **** ATTN - we must change Vars.c for this **** */
	if (!SetTypedVar(v->Node.ln_Name, v->Str, v->Node.ln_Type)) { /* <- EXTERNAL FUNCTION! */
	/* ******************************************************* */
	    VSTK_Internal = 0;
	    return VSTK_NOTSET;
	} /* if */
	VSTK_Internal = 0;
	if (mode == POP)
	    VSTK_DropEntry (v);
	return VSTK_OK;
    } /* if */
    return VSTK_NOENTRY;
} /* VSTK_PickPopEntry */


/*************************************************
	    Name Level Functions
*************************************************/

/* create a new entry */
static
int VSTK_Push (const char *name) {
    VRST *v;
    int type;
    char *val;

    VSTK_Internal = 1;				/* ask for the variable and get the value and type */
    val = GetTypedVar (name, &type);  /* <- EXTERNAL FUNCTION! */
	  /************************/
    VSTK_Internal = 0;

    if (!val)
	return (type == VAR_NEX)? VSTK_NOVAR: VSTK_NOMEM;

    if ((v = malloc (sizeof (VRST)))) {         /* allocate a node */
	v->Node.ln_Name = strdup (name);        /* and space for its name */
	v->Str		= val;
	v->Node.ln_Type = type;

	if (v->Node.ln_Name) {                  /* push the node onto the varstack */
	    AddHead((LIST*)&varstack, (NODE*)v);
	    return VSTK_OK;
	} /* if */
	VSTK_FreeEntry(v);
    }
    return VSTK_NOMEM;

} /* VSTK_Push */


/* Restore a variable entry and eventually delete its top entry */
static
int VSTK_PickPop (const char *name, int mode) {
    VRST *v;

    if ((v = VSTK_FindEntry(name)))
	return VSTK_PickPopEntry(v, mode);
    else
	return VSTK_NOENTRY;

} /* VSTK_PickPop */


/* remove the top entry of a certain name */
static
int VSTK_Drop (const char *name) {
    VRST *v;

    if ((v = VSTK_FindEntry(name)))                 /* find the first matching node */
	return VSTK_DropEntry (v);                  /* ... and drop it              */
    else
	return VSTK_NOENTRY;

} /* VSTK_Drop */


/* swap the contents of a variable with that of an entry */
static
int VSTK_Swap (const char *name) {
    VRST *v;

    if ((v = VSTK_FindEntry(name)))
    {						 /* if there is a matching stack-entry */
	int rv;
	if ((rv = VSTK_Push(name)) != VSTK_OK)   /* push the variables current contents onto stack */
	    return rv;
	Remove((NODE*)v);                        /* remove the elder one */
	AddHead((LIST*)&varstack, (NODE*)v);     /* and set it in front of the new node */
	return VSTK_PickPopEntry(v, POP);        /* and then pop it into the variable */

    } else
	return VSTK_NOENTRY;

} /* VSTK_Swap */


/* remove all entries of a certain name */
static
int VSTK_Purge (const char *name) {
    VRST *v;
#if 0
    VARS *w;

    for (v = (VRST *)varstack.mlh_Head; (w = (VRST *)v->Node.ln_Succ) != NULL; v = w) /* for all nodes ... */
	if (strcmp(v->Node.ln_Name, name) == 0)                                       /* if they match ... */
	    VSTK_DropEntry (v);                                                            /* drop them ...     */
#else
    /* ---- this is less code, slower but does not access the Node-slot */
    while ((v = VSTK_FindEntry(name)))              /* find all matching nodes  */
	return VSTK_DropEntry (v);                  /* ... and drop them        */
#endif

    return VSTK_OK;
} /* VSTK_Purge */


/*************************************************
	Application Interface
*************************************************/

/* Display a VarStack error */

static int VSTK_Error (int code) {
    switch (code) {
    case VSTK_OK:
	return RET_SUCC;
    case VSTK_NOVAR:
	error ("%s:\nNo Variable `%s' found!", av[0], av[1]);
	break;
    case VSTK_NOMEM:
	nomemory();
	// error ("%s:\nRan out of virtual memory!", av[0]); // HA! HA! HA!
	break;
    case VSTK_NOENTRY:
	error ("%s:\nNo Stackentry `%s' found!", av[0], av[1]);
	break;
    case VSTK_NOTSET:
	error("%s:\nUnable to set Variable `%s'", av[0], av[1]);
	break;
    } /* switch */

    return RET_FAIL;
} /* VSTK_Error */


/*************************************************
	    XDME Command Interface
*************************************************/


/*DEFHELP #cmd varstack PUSHVAR varname - put a variable onto the variable stack */

DEFUSERCMD("pushvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_pushVAR, (void),)
{
    return VSTK_Error(VSTK_Push(av[1]));
} /* do_pushVAR */

/*DEFHELP #cmd varstack DROPVAR varname - drop the first entry of a variable from the variable stack */

DEFUSERCMD("dropvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_dropVAR, (void),)
{
    return VSTK_Error( VSTK_Drop(av[1]));
} /* do_dropVAR */

/*DEFHELP #cmd varstack PICKVAR varname - get a variable from the variable stack without modifying the stack */

DEFUSERCMD("pickvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_pickVAR, (void),)
{
    return VSTK_Error( VSTK_PickPop(av[1], !POP));
} /* do_pickVAR */

/*DEFHELP #cmd varstack PURGEVAR varname - remove all occurencies of a variable from the variable stack */

DEFUSERCMD("purgevar", 1, CF_VWM|CF_COK|CF_ICO, int, do_purgeVAR, (void),)
{
    return VSTK_Error( VSTK_Purge(av[1]));
} /* do_purgeVAR */

/*DEFHELP #cmd varstack POPVAR varname - get a variable from the variable stack and remove it */

DEFUSERCMD("popvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_popVAR, (void),)
{
    return VSTK_Error( VSTK_PickPop(av[1], POP));
} /* do_popVAR */

/*DEFHELP #cmd varstack SWAPVAR varname - swap te contents of a variable with that of its 1st entry in the variable stack */

DEFUSERCMD("swapvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_swapVAR, (void),)
{
    return VSTK_Error( VSTK_Swap(av[1]));
} /* do_swapVAR */


#if 0 /* das wuerde nochmal ca 30 byte sparen */

int do_varstack (void) {
    int rv = !POP;

    switch (av[0][2]) {
    case 'a':
	rv = VSTK_Swap(av[1]);
	break;
    case 'p':
//	  rv = VSTK_PickPop(av[1], POP);
//	  break;
	rv = POP;
    case 'c':
	rv = VSTK_PickPop(av[1], rv);
	break;
    case 'o':
	rv = VSTK_Drop(av[1]);
	break;
    case 'r':
	rv = VSTK_Purge(av[1]);
	break;
    case 's':
	rv = VSTK_Push(av[1]);
	break;
    default:
	return RET_FAIL;
    } /* switch */

    return VSTK_Error(rv);
}
#endif


#ifdef STATIC_COM

    COMMAND("swapvar",        1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_swapVAR  )
    COMMAND("pickvar",        1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_pickVAR  )
    COMMAND("dropvar",        1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_dropVAR  )
    COMMAND("popvar",         1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_popVAR   )
    COMMAND("purgevar",       1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_purgeVAR )
    COMMAND("pushvar",        1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_pushVAR  )

    DEFFLAG( 94-09-09, VSTK_Internal, 0 )

#endif


/******************************************************************************
*****  ENDE varstack.c
******************************************************************************/

/* codesize - previous: 1360 now: 1302 */

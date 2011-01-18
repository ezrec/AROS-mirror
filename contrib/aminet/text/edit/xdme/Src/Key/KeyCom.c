/******************************************************************************

    MODUL
	keycom.c

    DESCRIPTION
	command interface for keyboard-control with key functions

    NOTES
	for interface with [X]DME version < 1.70
	    all functions in this module make use of av[] (readonly)
	extensively uses keyhashes.c

    BUGS
	plenty - but where

    TODO


    EXAMPLES

    description of the user-commands:

	the commands below might get other names;
	if so, please update their descriptions according
	to the names they are used in command.c

*!***************************************************************************
*!
*!  KEYS:
*!  ****
*!
*!  the following commands allow access to [X]DME's keymappings
*!  that way it is possible for each user to create an individual
*!  UI for "his" [X]DME, for "his" programming language and for
*!  his wishes.
*!
*! >MAP   key command	(will be "MAP key command help")
*! >UNMAP key
*!
*!	with these commands YOu can modify the keytable:
*!	MAP adds or modifies an existing key-entry to the keytable
*!	UNMAP deletes an existing key-entry
*!
*!	key consists of a set of QUALIFIERS, a "-" and one code-
*!	specification (both case sensitive)
*!	the qualifiers are:
*!	    s == any shift (caps-lock with alpha-keys)
*!	    c == control
*!	    a == any alt
*!	    A == any amiga
*!	    L == left mouse button
*!	    M == middle mouse button
*!	    R == right mouse button
*!	    u == key-up ( release of a key )
*!	    x == extended qualifier x
*!	    y == extended qualifier y
*!	    z == extended qualifier z
*!	    r == repeat
*!
*!	the code-specification is one out of:
*!	  main keyboard:
*!	    [single lowercase character] == the key on you main keyboard - that shows it
*!	    [single uppercase alpha]	 == the key on .... but that means a set s-qualifier
*!
*!	  numeric keypad:
*!	    nk0 .. nk9		== numerik keypad 0..9
*!	    nk+|nk-|nk*|nk/|nk. == numerik keypad +,-,*,/,.
*!
*!	    there are two more key on yor numerik keypad
*!	    with american|british mapping they are used as nk( and nk)
*!	    with most european mappings they are used as nk[ and nk]
*!	    with canadian mapping they are used as nk@ and nk°
*!	    if there is a standart mapping, that does map these keys
*!	    in another way, they are NOT supported yet. (please mail)
*!
*!	  other special keys:
*!	    f1 .. f10	   == Function keys
*!	    hel 	   == help-key
*!	    esc 	   == escape
*!	    del 	   == delete
*!	    bac|bs	   == backspace ( <- )
*!	    tab 	   == tabulator
*!	    ent 	   == enter
*!	    spc|spa	   == spacebar
*!	    up|dow|rig|lef == cursor keys
*!
*!	    lmb|mmb|rmb    == mouse buttons
*!	    mmo 	   == mouse movement with a mousebutton pressed
*!	    ?m2|?m3|?m4    == (? in {lmr}) double/triple/qraduple clicks of a
*!			      certain mouse button (times out!)
*!
*!	note that as mousebuttons can act for qualifiing as well as for
*!	specifiing, the specifiing mousebutton must also be qualifiing
*!	e.g L-rmb should not work, but LR-rmb should
*!
*!	note that certain keys may be used as dead-keys;
*!	keys that are deading when pressed without qualifiers
*!	are not accessible with that keyboard-interface
*!	e.g. on german keyboards there is a key "'" next to "\"
*!	     which cannot be accessed
*!	on the other hand, if a key is deading on a qualified level,
*!	but not when pressed without qualifiers, You can remap it
*!	e.g on german keyboards alt-f is deading, but -f not, so
*!	    You can access a-f; and if You map a-f to any function,
*!	    It looses it deading function for [X]DME
*!	    EXAMPLE:  <alt-f> <a> creates something like "á"
*!		after a "MAP a-f (NULL)" the same key-sequence
*!		creates a normal "a"
*!
*!	note that map soon is to expect another parameter:
*!	    the help-string that contains information 'bout the command
*!
*!	keys with the x|y|z-qualifiers set can not accessed immediately
*!	first You must set the right extended qualifiers (see QUALIFIER
*!	command), then You may press the remaining qualifiers and keys
*!	these keys were introduced to allow something like the
*!	CTL-X prefix of MicroEmacs
*!
*!	ATN! we check only up to three characters for code-specifiction
*!	    so e.g. -space is recognized as -spa some people
*!	    call this a feature, but in fact it is a BUG, as that
*!	    method avoids rexx-commands which start whose 3 first digits
*!	    are the same like a special key from being implicitely executed
*!
*! >QUALIFIER qualifierset
*!
*!	that command allows setting of certain qualifiers
*!	independently to the qualifiers pressed manually on the keyboard
*!	(see above) it is designed to allow multi-key-sequences
*!
*!	EXAMPLE:
*!	     MAP  c-x (QUALIFIER x)
*!	     MAP cx-a (SAVEOLD)
*!	    would cause the sequence <ctrl-x> <ctrl-a> calling SAVEOLD
*!
*! >KEYSAVE filename
*!
*!	save the keytable as a file that can be
*!	read by KEYLOAD
*!
*! >KEYLOAD filename
*!
*!	clear the keytable and instead build a new
*!	contents out of the file filename
*!
*!
*!  KEYTABLES:
*!  *********
*!
*!  the following commands do only make sense, if
*!  You are using multiple keytables
*!  at [X]DME's start a keytable called "default" is created;
*!  for "normal" usage, that single strip may suffer.
*!
*!  Please note that the above MENU... commands apply to the
*!  CURRENT keytable (and only to it)
*!
*! >USEKEYTABLE name
*!
*!	search for a certain keytable and use it as the current one
*!
*! >REMKEYTABLE
*!
*!	delete the current keytable, if it is not the only one
*!
*! >NEWKEYTABLE name
*!
*!	if there is already a keytable called name,
*!	    simply call USEKEYTABLE name
*!	else create a new keytable called name and use it
*!
*!

    SEE ALSO
	keyhashes.c keycodes.c keyaddes.c keycontrol.c

    INDEX

    HISTORY
	14-Dec-92  b_null created
	16-Dec-92  b_null some names updated & introduced currenthash
	05-08-94   null added STATIC Command Interface
	$Date$ last update

******************************************************************************/

/**************************************
		Includes
**************************************/
/* we do not need anythig else - I think */
#define  KEY_INTERNAL		    /* PATCH_NULL < 24-06-94 */
#include "defs.h"
#ifdef PATCH_NULL
#include "COM.h"
#endif


/**************************************
	    Globale Variable
**************************************/
Prototype char*keyspectomacro(char*);

Prototype void*currenthash  (void);

/**************************************
      Interne Defines & Strukturen
**************************************/
/* nothing */

/**************************************
	    Interne Variable
**************************************/
/* nothing */

/**************************************
	   Interne Prototypes
**************************************/
/* nothing */

/*****************************************************************************

    NAME
	do_map

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyhashes/mapkey

    NOTES
	for interface with [X]DME version < 1.70

    BUGS

    EXAMPLES

    SEE ALSO
	do_unmap keyhashes/mapkey

    INTERNALS

    HISTORY
	14-Dec-92  b_null created

******************************************************************************/

DEFUSERCMD("map", 2, CF_VWM|CF_ICO, void, do_map, (void),)
{
    mapkey ( currenthash(), GetArg(1), GetArg(2), NULL );
} /* do_map */



/*****************************************************************************

    NAME
	do_unmap

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyhashes/unmapkey

    NOTES
	for interface with [X]DME version < 1.70

    BUGS

    EXAMPLES

    SEE ALSO
	do_map, keyhashes/unmapkey

    INTERNALS

    HISTORY
	14-Dec-92  b_null created

******************************************************************************/

DEFUSERCMD("unmap", 1, CF_VWM|CF_COK|CF_ICO, void, do_unmap, (void),)
{
    unmapkey ( currenthash(), GetArg(1) );
} /* do_unmap */



/*****************************************************************************

    NAME
	do_keysave

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyhashes/savemap

    NOTES
	for interface with [X]DME version < 1.70

    BUGS

    EXAMPLES

    SEE ALSO
	do_keyload, keyhashes/savemap

    INTERNALS

    HISTORY
	14-Dec-92  b_null created

******************************************************************************/

DEFUSERCMD("keysave", 1, CF_VWM|CF_ICO, void, do_keysave, (void),)
{
    keysave ( currenthash(), GetArg(1) );
} /* do_keysave */



/*****************************************************************************

    NAME
	do_keyload

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyhashes/loadmap

    NOTES
	for interface with [X]DME version < 1.70

    BUGS

    EXAMPLES

    SEE ALSO
	do_keysave, keyhashes/loadmap

    INTERNALS

    HISTORY
	14-Dec-92  b_null created

******************************************************************************/

DEFUSERCMD("keyload", 1, CF_VWM|CF_ICO, void, do_keyload, (void),)
{
    keyload ( currenthash(), GetArg(1) );
} /* do_keyload */



/*****************************************************************************

    NAME
	do_qualifier

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyaddes/qualifier

    NOTES
	for interface with [X]DME version > 1.70

    BUGS

    EXAMPLES

    SEE ALSO
	keyhashes/qualifier

    INTERNALS

    HISTORY
	16 Dec 1992 b_null created

******************************************************************************/

DEFUSERCMD("qualifier", 1, CF_VWM|CF_ICO|CF_COK, void, do_qualifier, (void),)
{
    qualifier (GetArg(1));
} /* do_qualifier */


char * keyspectomacro (char * str)
{
    return (keyspec2macro(currenthash(), str));
} /* keyspectomacro */



/*****************************************************************************

    NAME
	do_new_keytable

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyaddes/qualifier

    NOTES
	<never tested>

    BUGS
	<none known>

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	28 Jan 1993  b_null created

******************************************************************************/

DEFUSERCMD("newkeytable", 1, CF_VWM|CF_ICO|CF_COK, void, do_new_keytable, (void),)
{
    Ep->keytable = new_keytable (GetArg(1), 1);
} /* do_new_keytable */



/*****************************************************************************

    NAME
	do_del_keytable

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyaddes/qualifier

    NOTES
	<never tested>

    BUGS
	<none known>

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	28 Jan 1993  b_null created

******************************************************************************/

DEFUSERCMD("remkeytable", 1, CF_VWM|CF_ICO|CF_COK, void, do_del_keytable, (void),)
{
    KEYTABLE * kt = currenthash ();
    ED	     * ep;

    for (ep = GetHead (&DBase); ep; ep = GetSucc (ep)) {
	if (ep->keytable == kt) {
	    ep->keytable = NULL;
	} /* if */
    } /* for */

    delete_keytable (kt, 0);
} /* do_del_keytable */



/*****************************************************************************

    NAME
	do_use_keytable

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyaddes/qualifier

    NOTES
	<never tested>

    BUGS
	<none known>

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	28 Jan 1993  b_null created

******************************************************************************/

DEFUSERCMD("usekeytable", 1, CF_VWM|CF_ICO|CF_COK, void, do_use_keytable, (void),)
{
    KEYTABLE * kt = get_keytable (GetArg (1));

    if (kt) {
	Ep->keytable = kt;
    } /* if */
} /* do_use_keytable */



/*****************************************************************************

    NAME
	currenthash

    PARAMETER
	void;

    RESULT
	the current keytable

    RETURN
	void;

    DESCRIPTION
	An interface to the current ED's window

    NOTES
	This function could easiliy be replaced by a macro

    HISTORY
	26-08-93    b_noll  created

******************************************************************************/

void * currenthash(void)
{
    /* return(onlyhash); */

    if ((Ep == NULL) || (Ep->keytable == NULL)) {
	return (get_keytable (NULL));
    } /* if */
    return (Ep->keytable);
} /* currenthash */

/*****************************************************************************

    NAME
	keyfind
	keygethelp
	keysethelp
	keygetcommand
	keysetcommand
	keycall

    PARAMETER

    RESULT

    RETURN

    DESCRIPTION
	interfaces to support handles used by the
	STATIC Command Interface
	in order to enable use of COMTREE key
	as well as VARTREE keycomm/keyhelp

    NOTES
	<never tested>

    BUGS
	<none known>

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	05-08-94 null created

******************************************************************************/

#ifdef STATIC_COM

Prototype APTR keyfind (char *name);
APTR keyfind (char *name) {
    return findhash(currenthash(), name);
} /* keyfind */

Prototype char *keygethelp(APTR handle);
char *keygethelp(APTR handle) {
    return ((HASH *)handle)->help;
} /* keygethelp */

Prototype char *keygetcommand(APTR handle);
char *keygetcommand(APTR handle) {
    return ((HASH *)handle)->com;
} /* keygetcommand */

Prototype char *keysetcommand(APTR handle, char *val);
char *keysetcommand(APTR handle, char *val) {
    char *ptr;
    if (ptr = DupFunc(val)) {
	DeallocFunc(((HASH *)handle)->com);
	((HASH *)handle)->com = ptr;
	return TRUE;
    } /* if */
    nomemory();
    return FALSE;
} /* keysetcommand */

Prototype char *keysethelp (APTR handle, char *val);
char *keysethelp (APTR handle, char *val) {
    char *ptr;
    if (ptr = DupFunc(val)) {
	DeallocFunc(((HASH *)handle)->help);
	((HASH *)handle)->help = ptr;
	return TRUE;
    } /* if */
    nomemory();
    return FALSE;
} /* keysethelp */

Prototype int keycall (APTR handle);
int keycall (APTR handle) {
    char *ptr;
    if (ptr = strdup(keygetcommand(handle))) {
	retval = do_command(ptr);
	free (ptr);
	return OK;
    } /* if */
    nomemory();
    return FALSE;
} /* keycall */

#endif /* STATIC_COM */


#ifdef PATCH_NULL /* PATCH_NULL < 24-06-94 */


/* **************************************************
	STATIC XDME Command Interface
************************************************** //

//	    Name    #Args Function	    Flags
:FX:COMMAND keyload	1 do_keyload
:FX:COMMAND keysave	1 do_keysave
:FX:COMMAND newkeytable 1 do_new_keytable
:FX:COMMAND remkeytable 1 do_rem_keytable
:FX:COMMAND usekeytable 1 do_use_keytable
:FX:COMMAND map 	2 do_map
:FX:COMMAND unmap	1 do_unmap

//	    Name  Pri Lock	     Unlock   Call	 NArgs
:FX:COMTREE key     0 keyspectomacro return0  do_command return0      Flags=NoHandles

//	    Name  Pri Lock	     Unlock   Get    Set     Unset
:FX:VARTREE key     0 keyspectomacro return0  strdup return0 return0  Flags=NoHandles

// **************************************************
	DYNAMIC XDME Command Interface
************************************************** */

static const
struct CommandNode KEY_Commands[] =
{
    COMMAND("keyload",        1,        CF_VWM|CF_ICO, (FPTR)do_keyload      ) /* tester */
    COMMAND("keysave",        1,        CF_VWM|CF_ICO, (FPTR)do_keysave      ) /* tester */
    COMMAND("newkeytable",    1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_new_keytable )
    COMMAND("remkeytable",    1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_del_keytable )
    COMMAND("usekeytable",    1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_use_keytable )
    COMMAND("map",            2,        CF_VWM|CF_ICO, (FPTR)do_map          )
    COMMAND("unmap",          1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_unmap        )
    COMMAND("qualifier",      1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_qualifier    )
 /* COMMAND("savemap",        1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_savemap      ) */
 /* COMMAND("savesmap",       1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_savemap      ) */
};



MK_AUTOINIT( KEY_init )
{
    int i;

    for (i = sizeof (KEY_Commands)/sizeof (struct CommandNode) - 1;i >= 0; --i)
	COM_Add (&KEY_Commands[i]);

 /* COM_AddClass()   ->keyspectomacro */
 /* COM_AddVarClass()->keyspectomacro */
} /* KEY_init */


#if 0

MK_AUTOEXIT( KEY_Exit )
{
    int  i;
    APTR lock;

    for (i = sizeof (KEY_Commands)/sizeof (struct CommandNode) - 1; (i >= 0); DEC(i))
	if (lock = COM_Lock (KEY_Commands[i].cn_Name))
	    COM_Remove (lock);
} /* KEY_Exit */

#endif

#endif


/******************************************************************************
*****  ENDE keycom.c
******************************************************************************/

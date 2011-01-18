/******************************************************************************

    MODUL
	keytables.c

    DESCRIPTION
	Management for Key-Hashing tables

    NOTES
	original from M.Dillon

	for the first(!) call of any function of that module,
	the corresponding HASH-table must be clean (setmem(...,0))

    BUGS
	thousands - but where
	there may be some inconsistencies

    TODO
	new method for management of hashes

    EXAMPLES

    SEE ALSO
	keycodes.c keycom.c keycontrol.c

    INDEX

    HISTORY
	14-Dec-92  b_null created
	15-Dez-92  b_null modified
	15-Dez-92  b_null docs updated, some checks added
	16-Dez-92  b_null documented
	16 Jun 93  b_null introduced "struct keyspec"
	29-Jun-94  b_null Docs updated
	$Date$ last update

******************************************************************************/

/**************************************
		Includes
**************************************/
#define  KEY_INTERNAL
#include "defs.h"
#include <devices/keymap.h>
#include <devices/console.h>
#include "keyhashes.h"
#include <proto/console.h>
#ifdef PATCH_NULL
#include "AUTO.h"
#else
#define MK_AUTOEXIT(x) void x (void)
#define MK_AUTOINIT(x) void x (void)
#endif /*  */


/**************************************
	    Globale Variable
**************************************/

Prototype void	 keyload	(KEYTABLE * kt, char * name);
Prototype void	 keysave	(KEYTABLE * kt, char * name);
Prototype int	 loadkeys	(KEYTABLE * kt, FILE * fi, int * lineno);
Prototype int	 savekeys	(KEYTABLE * kt, FILE * fo);
Prototype HASH * findhash	(KEYTABLE * kt, KEYSPEC *ks,       UBYTE *);
Prototype APTR	 keyspec2macro	(KEYTABLE * kt,              const UBYTE *);
Prototype int	 resethash	(KEYTABLE * kt);
Prototype int	 unmapkey	(KEYTABLE * kt, const UBYTE *key);
Prototype int	 mapkey 	(KEYTABLE * kt, const UBYTE *key, const UBYTE *value, const UBYTE *help);
Prototype int	 remhash	(KEYTABLE * kt, KEYSPEC *ks);
Prototype int	 addhash	(KEYTABLE * kt, KEYSPEC *ks, const UBYTE *comm, const UBYTE *help);
Prototype void	 dealloc_hash	(KEYTABLE * kt);

Prototype KEYTABLE * get_keytable    (char * name);
Prototype KEYTABLE * new_keytable    (char * name, int defaults);
Prototype void	     delete_keytable (KEYTABLE * kt, int force);

Prototype void	     exit_keytables (void);

/**************************************
      Interne Defines & Strukturen
**************************************/

#ifndef QUAL_UP
#define QUAL_UP 0x80
#endif


/**************************************
	    Interne Variable
**************************************/
/* extern TWOSTRINGS defmap[]; */
#include "defmap.h"


/* this is the list of all keytables */
static struct MinList KeyTables = { (struct MinNode *)&KeyTables.mlh_Tail,
	NULL, (struct MinNode *)&KeyTables.mlh_Head };

/**************************************
	   Interne Prototypes
**************************************/



/*****************************************************************************

    NAME
	freehash

    PARAMETER
	HASH * hash

    RESULT
	the hash->next entry if existing
	NULL if hash == NULL

    RETURN
	HASH *

    DESCRIPTION
	free a hash-entry and all associated data

    NOTES
	the free'd hash must not be entry in a hashtable,
	as we do not take care of any refernces, but only
	of hash, comm and help

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	23 Jan 1993 b_null created

******************************************************************************/

HASH * freehash (HASH * hash)
{
    HASH * next = NULL;

    if (hash) {
	next = hash->next;
	if (hash->comm) {
	    DeallocFunc (hash->comm);
	} /* if */
	if (hash->help) {
	    DeallocFunc (hash->help);
	} /* if */
	FreeFunc(hash, sizeof(HASH));
    } /* if */

    return (next);
} /* freehash */



/*****************************************************************************

    NAME
	addhash

    PARAMETER
	KEYTABLE    * kt
	KEYSPEC     * ks
	const UBYTE * comm
	const UBYTE * help

    RESULT
	success: RET_SUCC == ok; RET_FAIL == failure

    RETURN
	int

    DESCRIPTION
	create a new hash-entry for a known hash
	if there is already an existing one with the same code/qual
	simply change its command

    NOTES
	that function is called by mapkey

    BUGS
	WARNING : WE DO NOT LOOK FOR A SETTING OF help YET !!!

    EXAMPLES

    SEE ALSO

    INTERNALS
	finde den hash-slot
	falls der eintrag schon existiert, ersetze den comm
	sonst erstelle einen neuen eintrag
	falls nicht alles funktionierte, stelle den alten zustand wieder her

    HISTORY
	14-Dec-92  b_null included & modified
	16-Dec-92  b_null added handling of "nolink"

******************************************************************************/

int addhash (KEYTABLE * kt, KEYSPEC * ks, const UBYTE * comm, const UBYTE * help)
{
    HASH ** p,
	  * hash;
 /* APTR    oldcomm = NULL; */

    if (kt == NULL) {
	return (RET_FAIL);
    } /* if empty or missing table */

    hash = *(p = &KT_SLOT(kt,ks));

    while (hash) {
	if (KS_MATCH_EXACT(&hash->key, ks)) {
	    goto newstr;
	} /* if found old */
	hash = *(p = &hash->next);
    } /* while entries left */

    if (!(hash  = (HASH *)AllocFunc (sizeof (HASH), MEMF_ANY|MEMF_CLEAR))) {
	SETF_ABORTCOMMAND(Ep, 1);
	return (RET_FAIL);
    } /* if nomemory */
/*    clrmem (hash, sizeof (HASH)); */

    hash->next = NULL;
    KS_COPY(&hash->key,ks);

newstr:
    /* oldcomm = hash->comm; */
    DeallocFunc (hash->comm);
    DeallocFunc (hash->help);

    hash->comm = DupFunc (comm, 0);
    hash->help = DupFunc (help, 0);

    if ((!hash->comm) /* || (!hash->help) */ /* || (oldcomm == hash->comm) */) {
	*p = freehash (hash);
	return (RET_FAIL);
    } /* if something went wrong */

    /* everything worked fine - insert it into the table */
    *p = hash;
    return (RET_SUCC);
} /* addhash */



/*****************************************************************************

    NAME
	remhash

    PARAMETER
	KEYTABLE * kt
	KEYSPEC  * ks

    RESULT
	success:
	    RET_SUCC == ok;
	    RET_FAIL == failure or not found

    RETURN
	int

    DESCRIPTION
	delete an existing hashentry

    NOTES
	that function is called by unmapkey

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	finde den hash-slot
	falls der eintrag existiert, loesche ihn und seinen link
	sonst mache gar nichts

    HISTORY
	14-Dec-92  b_null included & modified
	23-Jan-93  b_null used freehash

******************************************************************************/

int remhash (KEYTABLE * kt, KEYSPEC * ks)
{
    HASH *  hash, /* current */
	 ** p;	  /* previous */

    if (kt == NULL) {
	return (RET_FAIL);
    } /* if empty or missing table */

    hash = *(p = &KT_SLOT(kt,ks));
    while (hash) {
	if (KS_MATCH_EXACT(&hash->key, ks)) {
	    *p = freehash (hash);
	    return   (RET_SUCC);
	} /* if found */
	hash = *(p = &hash->next);
    } /* while searching */

    /* not found: */
    return (RET_FAIL);
} /* remhash */



/*****************************************************************************

    NAME
	mapkey

    PARAMETER
	KEYTABLE    * kt
	const UBYTE * key
	const UBYTE * value
	const UBYTE * help

    RESULT
	success:
	    RET_FAIL == error
	    RET_OK   == everything ok

    RETURN
	int

    DESCRIPTION
	first abstraction for addhash
	which uses an ascii-name

    NOTES
	this function might call error()

    BUGS

    EXAMPLES

    SEE ALSO
	addhash, unmapkey

    INTERNALS
	falls es sich um einen gueltigen map handelt
	    belege ihn (neu)
	sonst gebe eine fehlermeldung aus

    HISTORY
	14-Dec-92  b_null included & modified
	23-Jan-93  b_null void -> int

******************************************************************************/

int mapkey (KEYTABLE * kt, const UBYTE * key, const UBYTE * value, const UBYTE *help)
{
    struct keyspec ks;

    if (get_codequal (key, &ks)) {
	return (addhash (kt, &ks, value, help));
    } else {
	error ("%s:\nUnknown Key '%s'", CommandName(), key);
    }
    return (RET_FAIL);
} /* mapkey */



/*****************************************************************************

    NAME
	unmapkey

    PARAMETER
	KEYTABLE    * kt
	const UBYTE * key

    RESULT
	success:
	    RET_FAIL == error
	    RET_OK   == everything ok

    RETURN
	int

    DESCRIPTION
	first abstraction for remhash
	which uses an ascii-name

    NOTES
	this function might call error()

    BUGS

    EXAMPLES

    SEE ALSO
	remhash, mapkey

    INTERNALS
	falls es sich um einen gueltigen map handelt
	    loesche seine belegung
	sonst gebe eine fehlermeldung aus

    HISTORY
	14-Dec-92  b_null included & modified
	23-Jan-93  b_null void -> int

******************************************************************************/

int unmapkey (KEYTABLE * kt, const UBYTE * key)
{
    struct keyspec ks;

    if (get_codequal (key, &ks)) {
	return (remhash (kt, &ks));
    } else {
	error ("%s:\nUnknown Key '%s'", CommandName(), key);
    }
    return (RET_FAIL);
} /* unmapkey */



/*****************************************************************************

    NAME
	dealloc_hash

    PARAMETER
	KEYTABLE * kt

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	frees all Hash-entries in a key-table

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	ueber allen hash-slots
	    loesche jeden eintrag des hash-slots

    HISTORY
	14-Dec-92  b_null included & modified

******************************************************************************/

void dealloc_hash ( KEYTABLE * kt )
{
    HASH * hash,
	 * hnext;  /* save ptr to the next entry when freeing the current */
    WORD   i;

    if (kt == NULL) {
	return;
    } /* if empty or missing table */

    for (i = 0; i < HASHSIZE; i++) {
	for (hash = kt->hash[i]; hash; hash = hnext) {
	    hnext = freehash (hash);
	} /* for */
	kt->hash[i] = NULL;
    } /* for */
} /* dealloc_hash */



/*****************************************************************************

    NAME
	resethash

    PARAMETER
	KEYTABLE * kt

    RESULT
	success:
	    RET_FAIL == error (might have worked partially)
	    RET_OK   == everything ok

    RETURN
	int

    DESCRIPTION
	dealloc all entries of a hashtable and then
	set its contents to default-values

    NOTES
	the defaults are stored in an array
	called defmap

    BUGS

    EXAMPLES

    SEE ALSO
	mapkey, dealloc_hash

    INTERNALS
	loesche die tabelle
	solange noch ein gueltiger eintrag in defmap ist (und alles gutging)
	    erzeuge eine map fuer ihn

    HISTORY
	14-Dec-92  b_null included
	16-Dec-92  b_null documented

******************************************************************************/

int resethash (KEYTABLE * kt)
{
    WORD i;

    if (kt == NULL) {
	return (RET_FAIL);
    } /* if empty or missing table */

    dealloc_hash (kt);

    for (i = 0; defmap[i].from; ++i) {
	if (!mapkey (kt, defmap[i].from, defmap[i].to, NULL)) { /* ,defmap[i].help)) */
	    return (RET_FAIL);
	} /* if error */
    } /* for defmap-entries */

    return (RET_SUCC);
} /* resethash */



/*****************************************************************************

    NAME
	findhash

    PARAMETER
	KEYTABLE *kt
	KEYSPEC  *ks
	UBYTE	 *hint ; oder ULONG pkeys
		       ; additional info to be used if the key is unmapped

    RESULT
	the hash-entry associated with code and qual

    RETURN
	HASH *

    DESCRIPTION
	simple searching

    NOTES
	this routine is part of keyspec2macro

    BUGS

    EXAMPLES

    SEE ALSO
	keyspec2macro

    INTERNALS
	finde den richtigen slot
	solange es noch eintraege in dem slot gibe, und der richtige nicht
	    gefunden ist,
	    gehe zum naechsten eintrag
	wenn du was gefunden hast, gib es aus, sonst NULL

    HISTORY
	16-Dec-92  b_null created

******************************************************************************/

HASH * findhash (KEYTABLE *kt, KEYSPEC *ks, UBYTE *hint)
{
    HASH * hash;

    if (kt == NULL) {
	return (NULL);
    } /* if empty or missing table */

    /* PATCH_NULL [11 Mar 1994] : added for better recognition */
    for (hash = KT_SLOT(kt,ks); hash; hash = hash->next) {
	if (KS_MATCH_EXACT(&hash->key,ks)) {
	    return (hash);
	} /* if */
    } /* for */

    for (hash = KT_SLOT(kt,ks); hash; hash = hash->next) {
	if (KS_MATCH_APPROX(&hash->key,ks)) {
	    return (hash);
	} /* if */
    } /* for */

    /* PATCH_NULL [02 Apr 1993] : we can not call that proc on unmapped keys yet >>> */
#ifndef NOT_DEF
    {
	static HASH retdummy;	  /* static, 'coz we return a ptr to that data */
	static char rdchars[36];

	/* we have to create a dummy-Intuimessage and call RawKeyConvert with it */
	struct InputEvent ie = { NULL, IECLASS_RAWKEY, 0, 0, 0, NULL, 0L,0L};
	long	    len;

	if (hint) {                       /* PATCH_NULL 27-06-94      */
	    strcpy (rdchars, hint);/* That dirty Hack is added */
	    goto findhash_dummyfound;	    /* to reenable DeadKeys	*/
	} /* if */

	ie.ie_Code	       = KS_CODE(ks);
	ie.ie_Qualifier        = qual2iqual(KS_QUAL(ks));
	/* ie.ie_position.ie_addr = (APTR)pkeys; */

	if (IsRawC(ks) && (len = RawKeyConvert (&ie, rdchars+1, 36-2, NULL) > 0)) { /* PATCH_NULL 27-06-94 changed ">" into ">=" to reenable Deadkeys */
/* printf("PseudoMap:   l=%ld; c=%s\n", len, rdchars+1); */
	    rdchars[0] = '\'';
	    rdchars[len+1] = '\0';
	    /* previously : */
	    /* if (len == 1) {           */
	    /*	  rdchars[0] = '\'';     */
	    /*	  rdchars[len+1] = '\0'; */
	    /* } else { 		 */
	    /*	  rdchars[0]	 = '`';  */
	    /*	  rdchars[len+1] = '\''; */
	    /* }			 */

findhash_dummyfound:
	    KS_COPY(&retdummy.key,ks);
	    retdummy.comm = rdchars;
	    retdummy.help = "unmapped Key!";
	    return (&retdummy);
	} /* if */
/* printf("NOPseudoMap: r?=%s;\n", IsRawC(ks)?"Y":"N"); */

    }
#endif /* NOT_DEF */
    /* PATCH_NULL [02 Apr 1993] : we can not call that proc on unmapped keys yet <<< */

    return (NULL);
} /* findhash */



/*****************************************************************************

    NAME
	keyspectohash

    PARAMETER
	KEYTABLE    *kt
	const UBYTE *str

    RESULT
	the Hashentry associated with
	the name str

    RETURN
	HASH *

    DESCRIPTION
	transform str into a Hash-value and search for its entry

    NOTES
	this routine uses findhash

    BUGS

    EXAMPLES

    SEE ALSO
	findhash

    INTERNALS
	falls es sich um einen gueltigen schluessel handelt,
	    finde eine passenden eintrag und
		gib ihn aus
	sonst gib NULL aus

    HISTORY
	23-Jan-93  b_null created

******************************************************************************/

HASH * keyspectohash (KEYTABLE * kt, const UBYTE *str)
{
    struct keyspec ks;

    if (get_codequal (str, &ks)) {
	return (findhash (kt, &ks, 0));
    } /* if is key */
    return (NULL);
} /* keyspectohash */



/*****************************************************************************

    NAME
	keyspec2macro

    PARAMETER
	KEYTABLE    * kt
	const UBYTE * str

    RESULT
	the macro-field of the Hashentry associated with
	the name str

    RETURN
	APTR

    DESCRIPTION
	transform str into a Hash-value and search for its entry
	then return its command-entry

    NOTES
	this routine is splitted into
	two minor ones by putting the searching
	into another routine called findhash&keyspectohash

    BUGS

    EXAMPLES

    SEE ALSO
	findhash

    INTERNALS
	finde den passenden eintrag
	    und gib dessen comm aus

    HISTORY
	14-Dec-92  b_null included
	16-Dec-92  b_null splitted & documented

******************************************************************************/

APTR keyspec2macro (KEYTABLE * kt, const UBYTE * str)
{
    HASH *hash;

    hash = keyspectohash (kt, str);
    if (hash) {
	return (hash->comm);
    } /* if */
    return (NULL);
} /* keyspec2macro */


/*****************************************************************************

    NAME
	savekeys

    PARAMETER
	KEYTABLE * kt
	FILE *	fi

    RESULT
	success:
	    RET_FAIL == error
	    RET_OK   == everything ok

    RETURN
	int

    DESCRIPTION
	write a special keymap file
	or the keymap section of a file
	such a section can be read with loadkeys

    NOTES
	currently the keymap-section is in ascii
	format - this may be changed to IFF
	this function is separated from keysave for use in packages

    BUGS

    EXAMPLES

    SEE ALSO
	loadkeys

    INTERNALS
	schreibe einen header
	gehe alle slots entlang und
	    schreibe alle eintraege der slots
	schreibe einen footer

    HISTORY
	14-Dec-92  b_null included
	16-Dec-92  b_null documented

******************************************************************************/

int savekeys (KEYTABLE * kt, FILE * fo)
{
    HASH * hash;
    int    i;

    if (kt == NULL) {
	return (RET_FAIL);
    } /* if empty or missing table */

    fprintf(fo, "KEYLIST START\n");
    for (i = 0; i < HASHSIZE; i++) {
	for (hash = kt->hash[i]; hash; hash = hash->next) {
	    fprintf (fo, "\tKEY   %s\n", cqtoa (&hash->key));
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
	    if (hash->help)
		fprintf (fo, "\t HELP %s\n", hash->help);
	    fprintf (fo, "\t COM  %s\n", hash->comm);
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
	} /* for keys */
    } /* for hashes */
    fprintf(fo, "KEYLIST END\n");
    return (RET_SUCC);
} /* savekeys */



/*****************************************************************************

    NAME
	loadkeys

    PARAMETER
	KEYTABLE * kt
	FILE *	fi
	int  *	lineno

    RESULT
	success:
	    RET_FAIL == error
	    RET_OK   == everything ok

    RETURN
	int

    DESCRIPTION
	read a special Keymap-File
	or the Keymap-section of a file
	which was created with savekeys

    NOTES
	this function might call error()
	this function is separated from keyload for use in packages
	can not support "help" yet

    BUGS

    EXAMPLES

    SEE ALSO
	savekeys

    INTERNALS
	lese den header
	solange alles gutging und der footer nicht erreicht wurde
	    lese nacheinander schluessel
	    und macro
	    falls eine Zeile nicht in diese Sequenz passt ABBRUCH

    das muss geaendert werden:
	lese den header
	solange alles gutging und der footer nicht erreicht wurde
	lese  schluessel und
	danach macro (und evtl help)
	    da die reihenfolge von macro und help variieren kann,
	       darf der naechste addhash erst durchgefuehrt werden beim naechsten KEY oder bei KEYLIST END

    HISTORY
	14-Dec-92  b_null included
	16-Dec-92  b_null documented

******************************************************************************/

#define next_nonblank(buf) {while (*buf && *buf<33) buf++; }

int loadkeys (KEYTABLE * kt, FILE * fi, int * lineno)
{
    char * buf;
    char   nkey   [128];
    char   help   [128];
    char   body   [LINE_LENGTH];

    if (kt == NULL) {
	return (RET_FAIL);
    } /* if empty or missing table */

    buf = getnextcomline (fi, lineno);

    if (!buf) {
	SETF_ABORTCOMMAND(Ep, 1);
	return (RET_FAIL);
    } /* if */
    if (strncmp(buf, "KEYLIST START", 13) != 0) {
	error  ("No Keystart header");
	return (RET_FAIL);
    } /* if */

    nkey  [0] = 0;
    help  [0] = 0;
    body  [0] = 0;

    while (GETF_ABORTCOMMAND(Ep) == 0) {
	buf = getnextcomline(fi, lineno);
/* printf("read %s\n", buf); */
	if (!buf) {
	    SETF_ABORTCOMMAND(Ep, 1);
	    return (RET_FAIL);
	} /* if */

	if (strncmp(buf, "KEYLIST END", 11) == 0) {
	    return (RET_SUCC);
	} else if (strncmp(buf, "KEY ", 4) == 0) {
	    buf += 3;
	    next_nonblank(buf);
	    if (nkey[0] != 0) {
		error  ("<%s:\nDeclared Key w/out Body '%s'", CommandName(), buf);
		return (RET_FAIL);
	    } /* if */
	    strncpy(nkey, buf, 128);
#ifndef NOT_DEF
	} else if (strncmp(buf, "HELP", 4) == 0) {
	    buf += 4;
	    next_nonblank(buf);
	    if (nkey[0] == 0) {
		error  ("<%s:\nDeclared Help w/out a Key", CommandName(), buf);
		return (RET_FAIL);
	    } /* if */
	    strncpy(help, buf, 128);
#endif
	} else if (strncmp (buf, "COM", 3) == 0) {
	    buf += 3;
	    next_nonblank(buf);
	    if (nkey[0] == 0) {
		error  ("<%s:\nDeclared Body w/out a Key", CommandName());
		return (RET_FAIL);
	    } /* if */
	    strncpy (body, buf, LINE_LENGTH);
	    mapkey  (kt, nkey, body, help[0]? help: NULL);
	    help[0] = 0;
	    nkey[0] = 0;
	} else {
	    error  ("%s:\nunknown identifier '%s'", CommandName(), buf);
	    return (RET_FAIL);
	} /* if types */
    } /* while not ready */
    return (RET_FAIL);
} /* loadkeys */


/*****************************************************************************

    NAME
	keysave

    PARAMETER
	KEYTABLE * kt
	char *	name

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	first abstraction of savekeys to single files:
	handling of open&close

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO
	savekeys

    INTERNALS
	oeffne ein file
	falls das ging,
	    lese die enthaltenen mappings
	    und schliesse es
	sonst gib einen Fehler aus

    HISTORY
	14-Dec-92  b_null included
	16-Dec-92  b_null renamed & documented

******************************************************************************/

void keysave (KEYTABLE * kt, char * name)
{
    FILE * fo;

    if ((fo = fopen (name, "w"))) {
	savekeys (kt, fo);
	fclose (fo);
    } else {
	error ("%s:\nCan't open file %s for output", CommandName(), name);
    } /* if */
} /* keysave */


/*****************************************************************************

    NAME
	keyload

    PARAMETER
	KEYTABLE * kt
	char *	name

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	first abstraction of loadkeys to single files:
	handling of open & close

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO
	loadkeys

    INTERNALS
	oeffne ein file
	falls das ging,
	    schreibe die aktuellen mappings hinein
	    und schliesse es
	sonst gib einen Fehler aus

    HISTORY
	14-Dec-92  b_null included
	16-Dec-92  b_null renamed & documented

******************************************************************************/

void keyload (KEYTABLE * kt, char * name)
{
    FILE * fi;
    int    lineno = 0;

    if ((fi = fopen (name, "r"))) {
	dealloc_hash (kt);
	loadkeys (kt, fi, &lineno);
	fclose (fi);
    } else {
	error ("%s:\nCan't open file %s for input", CommandName(), name);
    } /* if */
} /* keyload */





/*****************************************************************************

    BASIC FUNCTIONS ON KEYTABLES :

	get, new, delete

    all keytables created with new_keytable are deleted at program termination

*****************************************************************************/

/*****************************************************************************

    NAME
	get_keytable

    PARAMETER
	char * name

    RESULT
	the keytable with the name name, if it is already existing
	else NULL

    RETURN
	KEYTABLE *

    DESCRIPTION
	search function on the hidden list of keytables

    NOTES
	useful only, if we are using multiple keytables

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	27 Jan 1993 b_null created

******************************************************************************/

KEYTABLE * get_keytable (char * name)
{
    KEYTABLE * kt;

    if (name == NULL) {
	return (GetHead (&KeyTables));
    } /* if wanted default */

    for (kt = GetHead(&KeyTables); kt; kt = GetSucc(kt)) {
	if (strcmp (kt->node.ln_Name, name) == 0) {
	    return (kt);
	} /* if */
    } /* for */
    return (NULL);
} /* get_keytable */



/*****************************************************************************

    NAME
	delete_keytable

    PARAMETER
	KEYTABLE * kt
	int	   force

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	delete a full keytable
	that function does NOT dlete the last keytable,
	unless force is set to 1

    NOTES
	useful only, if we are using multiple keytables

	that function does NOT take care of any references to a keytable
	You must do that job on a higher abstraction-level
	    (e.g. in keycom.c - for all ED's if )

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	ausklinken aus der keytable-liste
	loeschen der hash-entries
	loeschen des namens
	loeschen des bodies

    HISTORY
	27 Jan 1993 b_null created

******************************************************************************/

void delete_keytable (KEYTABLE * kt, int force)
{
    if (kt) {
	Remove	    (&kt->node);
	if ((!force) && (!GetHead (&KeyTables))) {
	    AddHead ((struct List *)&KeyTables, &kt->node);
	    return;
	} /* if */

	dealloc_hash (kt);
	DeallocFunc  (kt->node.ln_Name);
	FreeFunc     (kt, sizeof(KEYTABLE));
    } /* if */
} /* delete_keytable */



/*****************************************************************************

    NAME
	new_keytable

    PARAMETER
	char * name
	int    defaults

    RESULT
	either a new keytable, if there is not already one with that name
	or the first keytable, that matches that name
	if we have to create a new one, we can set defaults to 1 to
	fill in the default-keytable

    RETURN
	KEYTABLE *

    DESCRIPTION

    NOTES
	useful only, if we are using multiple keytables

	if we can fill in only parts of the deafult-information,
	we do NOT fail

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	27 Jan 1993 b_null created

******************************************************************************/

KEYTABLE * new_keytable (char * name, int defaults)
{
    KEYTABLE * kt;

    if ((kt = get_keytable(name))) {
	return (kt);
    } /* if */

    kt = AllocFunc (sizeof(KEYTABLE), MEMF_ANY);
    if (kt == NULL) {
	return (NULL);
    } /* if */

    setmem (kt, sizeof(KEYTABLE), 0);
    kt->node.ln_Name = DupFunc (name, MEMF_ANY);
    if (kt->node.ln_Name == NULL) {
	FreeFunc (kt, sizeof (KEYTABLE));
	return (NULL);
    } /* if */

    if (defaults) {
	resethash (kt);
    } /* if */

    AddTail ((struct List *)&KeyTables, &kt->node);

    return (kt);
} /* new_keytable */



/*****************************************************************************

    NAME
	init_keytables
	exit_keytables

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	init and exit functions for keytables

    NOTES
	useful only, if we are using multiple keytables

	THESE are __AUTOEXIT/__AUTOEXIT - functions !!!!

    HISTORY
	27 Jan 1993 b_null created

******************************************************************************/


MK_AUTOINIT( init_keytables )
{
    NewList	 ((struct List*)&KeyTables);
    new_keytable ("default",1);
} /* init_keytables */


MK_AUTOEXIT( exit_keytables )
{
    KEYTABLE * kt;

/* PutStr ("key_exiting\n"); */
    while ((kt = GetHead (&KeyTables))) {
	delete_keytable (kt, 1);
    } /* while */
/* PutStr ("key_exited\n"); */
} /* exit_keytables */





/******************************************************************************
*****  ENDE keytables.c
******************************************************************************/

/******************************************************************************

    MODUL
	keycontrol.c

    DESCRIPTION
	interface for keyboard control from main
	Handle keyboard related stuff such as keyboard mappings. Every
	time a key is pressed, KEYCTL() is called with the code. KEYCTL()
	remembers which qualifier keys are currently held down, and when a
	non-qualifier key is pressed finds the hash entry for the key.	If
	no hash entry exists (e.g. you type a normal 'a') the default
	keymap is used.

    NOTES
	Amiga-specific & [X]DME-specific
	depends on macro-language

    BUGS

    TODO
	we might enable deadkeys on our own - shall we do this ?
	    that would be done with a command redefining a
	    variable that is used instead of NO_KEYS_PRESSED

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	15-Dez-92  b_null created
	16-Dez-92  b_null documented
	$Date$ last update

******************************************************************************/

/**************************************
		Includes
**************************************/
#define  KEY_INTERNAL
#include "defs.h"


/**************************************
	    Globale Variable
**************************************/
Prototype void keyctl (struct IntuiMessage *, int, ULONG);  /* PATCH_NULL [14 Feb 1993] : changed to void */


/**************************************
      Interne Defines & Strukturen
**************************************/
#define NO_KEYS_PRESSED 0x00000000  /* we might write 0C0? for accents */

/* some hardcoded keys on Amiga Keyboard - look also in keycodes */
#define CSPC  0x40
#define CB_S  0x41 /* always ? */
#define CENT  0x43
#define CRET  0x44


/**************************************
	    Interne Variable
**************************************/
/* nothing */

/**************************************
	   Interne Prototypes
**************************************/


/*****************************************************************************

    NAME
	keyctl

    PARAMETER
	struct IntuiMessage *	im
	int			code
	ULONG			qual

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	reaction on a key-information

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	erzeuge die mit der msg verbundenen keycodes
	und qualifiers
	suche dann nach einer map dafuer
	//falls es keine gab, benutze wenn moeglich die
	//system-belegung als macro

    HISTORY
	15-Dez-92  b_null moved here & modified
	16.Dez.92  b_null documented
	28.Dez.92  b_null added support for NO dead-keys
	27-06-94   b_null debugged - what a hack... =8-(

******************************************************************************/

void keyctl (struct IntuiMessage * im, int code, ULONG qual)
{
/* PATCH_DEAD 28-Dez-92 : this is the boolean flag for disabling Deadkeys, */
/*			  if the last non-"up"-key initiated a macrocall   */
    static  WORD  last_displayed = 0;
/* PATCH_DEAD 28-Dez-92 */
	    UBYTE  buf[LINE_LENGTH];
	    UWORD  c2;
	    WORD   blen = 0;
	    int    ext2;
/* static int id=0; */

    code &= 0xFF;		    /* shorten code to one byte */

/* ++id; */
/* printf ("\n[%04x] LD=%d iaddr %08lx->%08lx (A)\n", id, last_displayed, (im)?im->IAddress:NULL, (im)? *(long*)im->IAddress: 0L); */


#if defined(PATCH_FILTER)
/*
**  --- Before executing the keystroke,
**	check if a filter is active and if the key can pass the filter
*/
    if ((key_filter != 0) && (filter_key (code) != 0)) {
	return;
    } /* if */
#endif /* PATCH_FILTER */


/*
**  --- Before evaluating the key's Qualifiers,
**	append the extended Qualifiers to the pressed ones
*/
    ext2 =  ext_qualifiers;
    qual = (qual & 0xF000FFFF) | ext2;

    if (im) {
	UWORD oldQual = im->Qualifier;

/* PATCH_DEAD 28-Dez-92 : if necessary hide the last pressed keys */
	long  dummy_prev[1];
	long * inter_iaddr = im->IAddress;

	if (last_displayed) {
	    dummy_prev[0]  = NO_KEYS_PRESSED;
	    im->IAddress   = dummy_prev;
	    if (!(code & 0x80)) {   /* PATCH_NULL [14 Feb 1993] : added to come around with additional msges for -UP keys */
		last_displayed = 0;
	    } /* if no up-msg */
	 /* last_displayed = 0; */
	} /* if */
/* PATCH_DEAD 28-Dez-92 */

	im->Qualifier = (oldQual & ~IEQUALIFIER_REPEAT) | (0xFFFF & ext2);  /* == ... & 0xF000FFFF | ext2 -> === qual ohne extended */

/* printf ("[%04x] LD=%d iaddr %08lx->%08lx code/qual %04x/%04x (E)\n", id, last_displayed, im->IAddress, *((long*)im->IAddress), im->Code, im->Qualifier); */

	blen = DeadKeyConvert (im, buf+1, 254, NULL);
	im->Qualifier = oldQual;

/* PATCH_DEAD 28-Dez-92: restore the old value (that is obsolete, since we are using a copy of im, but...)*/
	im->IAddress = inter_iaddr;
/* PATCH_DEAD 28-Dez-92 */

	if (blen < 0) {
	    return;
	} else if (blen == 1) {
	    buf[0] = '\'';
	    buf[2] = 0;
	} else if (blen) {
	    buf[0] = '`';
	    buf[blen+1] = '\'';
	} else {
	    buf[0] = 0;
	    buf[1] = 0;
	} /* if */
    }
    ext_qualifiers = 0;

    c2 = iqual2qual (qual, blen, buf, code);
    code &= 0x7f;				/* Remove the up/down information AFTER iqual2qual */

/* printf ("key : %lx %x %s (blen=%d,buf=%c)\n", c2, code, cqtoa(code,c2), blen,buf[1]); */

    {
	HASH * hash;
	struct keyspec ks;

	ks.ks_code	 = code;
	ks.ks_qualifiers = c2;
	ks.ks_qualmask	 = QUALMASK;

/* PATCH_NULL [13 Jan 1993] : the following construct was added for [X]DME only to allow deletion on returnoverride */
#	ifdef NOT_DEF
	    /* that long version disables not only return but also backspace and space */
	    if (GETF_COMLINEMODE(Ep) && c2 == 0) {
		char * str = NULL;
		switch (code) {
		case CRET: str = "return";  break;
		case CSPC: str = "( )";     break;
		case CB_S: str = "bs";      break;
		} /* switch */
		if (str) {
		    last_displayed = 1;
		    strcpy (buf, str);
		    do_command (buf);   /* PATCH_NULL [14 Feb 1993] : added */
		    return;
		} /* if */
	    } /* if comline without quals */
#	else
	    /* that short version only disables return */
	    if (GETF_COMLINEMODE(Ep) && code == CRET && c2 == 0) {
		last_displayed = 1;
		strcpy (buf, "return");
		do_command (buf);       /* PATCH_NULL [14 Feb 1993] : added */
		return;
	    } /* if */
#	endif
/* PATCH_NULL [13 Jan 1993] */

	hash = findhash (currenthash(), &ks, im? buf: NULL);

	/*
	 *  Use hash entry only if not in command line mode, or if the
	 *  entry does not correspond to an alpha key.
	 */

	if (hash) {
	    /*
	     * found hash-entry
	     * only use it, if we are not in commandline-mode
	     * or (if in comlinemode:) if a qualifier was pressed
	     *			       or it is a non-alphanum
	     */
	    if (c2 || !GETF_COMLINEMODE(Ep) || blen > 1 || !CtoA(code)) {
/* PATCH_DEAD 28-Dez-92 : enable hiding of the last pressed keys */
		if (im && *hash->comm) {
		    last_displayed = 1;
		} /* if key from a message */
/* PATCH_DEAD 28-Dez-92 */
		strcpy (buf, hash->comm);
		do_command (buf);   /* PATCH_NULL [14 Feb 1993] : added */
		return;
	    } /* if  */
	} /* if */
    }


#if 1 /* PATCH_NULL 27-06-94 .... the following code was thought obsolete since findhash manages unmapped keys - as long as comline is supported, it is not */

    /*
     *	No hash entry
     */

    if (blen > 0) {
#if 0 /* the macro was already created immediately after RawKeyConvert */
	/*
	 *  try to create a macro if we had a result on DeadKeyConvert
	 */
	if (blen == 1) {
	    buf[0] = '\'';  /* This *MUST* be ' !!!! */
	    buf[2] = 0;
	} else {
	    buf[0] = '`';
	    buf[blen+1] = '\'';
	}
#endif
/*	last_displayed = 1; */	/* PATCH_DEAD 28-Dez-92 : it might be senseful to put it here, too ... */
				/* PATCH_DEAD 14-Jan-93 : ... but normally active keys are no deadkeys */
	do_command (buf);       /* PATCH_NULL [14 Feb 1993] : added */
	return;
/*
**  --- if there was no real reaction on that key
**	restore the extended Qualifiers
*/
    } else {
	ext_qualifiers = ext2; /* keep the qualifiers */
/* printf("nothing found w/ code %d,%d\n", code, c2); */
    } /* if (no) string from DeadKeyConvert */

#else
	ext_qualifiers = ext2;
#endif

} /* keyctl */




/******************************************************************************
*****  ENDE keycontrol.c
******************************************************************************/

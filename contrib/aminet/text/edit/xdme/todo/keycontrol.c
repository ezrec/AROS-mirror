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
/*	UARGGH! We must blend out shift, alt, etc, and for there */
/*	is no way to find out, which key is a qualifier, we use  */
/*	them hardcoded, so hat piece of code is non-portable	 */

#define CLShift 0x60
#define CRShift 0x61
#define CCaps	0x62
#define CCtrl	0x63
#define CLAlt	0x64
#define CRAlt	0x65
#define CLAmiga 0x66
#define CRAmiga 0x67

#define CSPC	0x40
#define CB_S	0x41 /* always ? */
#define CTab	0x42
#define CENT	0x43
#define CRET	0x44



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
	20-10-94   b_noll cleaned up a bit

******************************************************************************/

void keyctl (struct IntuiMessage * im, int code, ULONG qual)
{
/* PATCH_DEAD 28-Dez-92 : this is the boolean flag for disabling Deadkeys, */
/*			  if the last non-"up"-key initiated a macrocall   */
    static  WORD   last_displayed = 0;
	    UBYTE  buf[LINE_LENGTH];
	    UWORD  c2;
	    WORD   blen = 0;
	    int    ext2;
	    HASH  *hash;
	    struct keyspec ks;
	    BOOL   defaulting = FALSE;
/* static int id=0; */

    code &= 0xFF;		    /* shorten code to one byte */

/* ++id; */
/* printf ("\n[%04x] LD=%d iaddr %08lx->%08lx (A)\n", id, last_displayed, (im)?im->IAddress:NULL, (im)? *(long*)im->IAddress: 0L); */


#if defined(PATCH_FILTER)
    /*
    **	--- Before executing the keystroke,
    **	    check if a filter is active and if the key can pass the filter
    */
    if ((key_filter != 0) && (filter_key (code) != 0)) {
	return;
    } /* if */
#endif /* PATCH_FILTER */


    /*
    **	--- Before evaluating the key's Qualifiers,
    **	    append the extended Qualifiers to the pressed ones
    */
    ext2 =  ext_qualifiers;
    qual = (qual & 0xF000FFFF) | ext2;

    if (im) {
	UWORD oldQual = im->Qualifier;
	long  dummy_prev[1];
	long *inter_iaddr;

//printf ("[    ] LD=%d iaddr %08lx->%08lx code/qual %02x/%04x ic/xq %02x/%08lx (E)\n", last_displayed, im->IAddress, *((long*)im->IAddress), im->Code, im->Qualifier, code, ext_qualifiers);

	/* ---- if necessary hide the last pressed keys */
	inter_iaddr = im->IAddress;
	if (last_displayed) {
	    dummy_prev[0]  = NO_KEYS_PRESSED;
	    im->IAddress   = dummy_prev;
//	      if (!(code & IECODE_UP_PREFIX)) {
//puts ("clearing eta");
//		last_displayed = 0;
//	      }
	} /* if */


	im->Qualifier = (oldQual & ~IEQUALIFIER_REPEAT) | (0xFFFF & ext2);  /* == ... & 0xF000FFFF | ext2 -> === qual ohne extended */
	blen	      = DeadKeyConvert (im, buf+1, 254, NULL);

	/* ---- restore the old values (that is not necessary, since we */
	/*	are using a copy of im, but it makes code reusable	*/
	im->Qualifier = oldQual;
	im->IAddress = inter_iaddr;

	/* ---- this key can be displayed, it is no deading key */
	if (blen && !(code & IECODE_UP_PREFIX)) {
//puts ("setting zeta");
	    last_displayed = 1;

	    /* ---- only the Perm-quals stay valid */
	    ext_qualifiers &= 0x07000000;
	} /* if no up-msg */

	/* ---- surround the bufferstring w/ quotes in */
	/*	order to create a XDME write command   */
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
    } /* if */


    /* ---- create the xdme key information */
    c2		     = iqual2qual (qual, blen, buf, code);
    //code	    &= ~IECODE_UP_PREFIX;
    ks.ks_code	     = code & ~IECODE_UP_PREFIX;
    ks.ks_qualifiers = c2;
    ks.ks_qualmask   = QUALMASK;

/* the following construct was added for [X]DME only to allow deletion on returnoverride */
#   ifdef NOT_DEF
	/* that long version disables not only return but also backspace and space */
	if (GETF_COMLINEMODE(Ep) && c2 == 0) {
	    char * str = NULL;
	    switch (code) {
	    case CRET: str = "return";  break;
	    case CSPC: str = "( )";     break;
	    case CB_S: str = "bs";      break;
	    } /* switch */
	    if (str) {
//puts ("setting epsilon");
		last_displayed	=	   1;
		ext_qualifiers &= 0x07000000;
		strcpy (buf, str);
		do_command (buf);   /* PATCH_NULL [14 Feb 1993] : added */
		return;
	    } /* if */
	} /* if comline without quals */
#   else
	/* that short version only disables return */
	if (GETF_COMLINEMODE(Ep) && code == CRET && c2 == 0) {
//puts ("setting delta");
	    last_displayed  =	       1;
	    ext_qualifiers &= 0x07000000;

	    strcpy (buf, "return");
	    do_command (buf);       /* PATCH_NULL [14 Feb 1993] : added */
	    return;
	} /* if */
#   endif


    hash = findhash (currenthash(), &ks, im? buf: NULL, &defaulting);

#ifndef Key_NoDefExt
#define Key_NoDefExt 1
#endif
    DEFFLAG( 21-10-94, Key_NoDefExt, 0 )

    /* ---- we need not expand into nothing */
    if (!hash || (defaulting && ((blen <= 0) || ((c2 & 0x7700) && Key_NoDefExt)))) {

	/* ---- ignore Upstrokes & Quals in last_displayed		 */
	/*	UARGGH! We must blend out shift, alt, etc, and for there */
	/*	is no way to find out, which key is a qualifier, we use  */
	/*	them hardcoded, so hat piece of code is non-portable	 */
	if (!(code & IECODE_UP_PREFIX) && !((code >= CLShift) && (code <= CRAmiga))) {
//puts ("clearing gamma");
		last_displayed = 0;
	} /* if */
	return;
    } /* if */

    /*
     *	Use hash entry only if not in command line mode, or if the
     *	entry does not correspond to an alpha key.
     */

    if (c2 || !GETF_COMLINEMODE(Ep) || blen > 1 || !CtoA(code & ~IECODE_UP_PREFIX)) {
//puts ("setting beta");
	ext_qualifiers &= 0x07000000;
	last_displayed	=	   1;
	strcpy (buf, hash->comm);
	do_command (buf);
    } else {
	if (blen > 0) {
//puts ("setting alpha");
	    ext_qualifiers &= 0x07000000;
	    last_displayed  =	       1;
	    do_command (buf);
	} /* if */
    } /* if */

} /* keyctl */




/******************************************************************************
*****  ENDE keycontrol.c
******************************************************************************/

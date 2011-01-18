/******************************************************************************

    MODUL
	keycodes.c

    DESCRIPTION
	Lowlevel module for evaluation of keycodes and qualifiers

    NOTES
	highly amiga - specific

    BUGS
	no use of dead-keys

    TODO
	deadkey - checking
	qual2iqual

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	14-Dez-92 b_null created
	14-Jan-93 b_null added UP
	15-Jan-92 b_null added DeadKeyConvert, removed exit_kb - it caused a GURU
	04 Apr 93 b_null added QUAL_PR*, QUAL_NLK, QUAL_REP; renamed USHORT,UBYTE->TQUAL,TCODE
	30 Apr 93 b_null added is_rawc
	16 Jun 93 b_null introduced "struct keyspec"
	29-Jun-94 b_null Docs updated
	03-Jul-94 b_null Added "DoubleClick-Key"

******************************************************************************/

/**************************************
		Includes
**************************************/
#define  KEY_INTERNAL
#include "defs.h"
#include <devices/keymap.h>
#include <devices/console.h>
#include <proto/utility.h>
#include <proto/console.h>
#ifdef PATCH_NULL
#include "AUTO.h"
#else
#define MK_AUTOINIT(x) void x (void)
#endif /*  */

extern UBYTE CtlC;



/**************************************
	    Globale Variable
**************************************/

Prototype struct Library * ConsoleDevice;
struct Library * ConsoleDevice = NULL;

/**************************************
      Interne Defines & Strukturen
**************************************/
typedef struct IOStdReq CIO;


/* format of entries in lname[] */
#define LN(a,b,c,d)  ((a<<24)|(b<<16)|(c<<8)|d)

/* important numbers in lname[] */
#define F1_START  0	       /* Function keys */
#define SP_START 10	       /* special keys	*/
#define CS_START 20	       /* cursor keys	*/
#define N0_START 24	       /* NK numbers	*/
#define NS_START N0_START+10   /* NK specials	*/
#define MB_START 41	       /* Mouse 	*/


/* internal representation for qualifiers */
#define QUAL_SHIFT   0x0001 /* ANY SHIFT */
#define QUAL_CTRL    0x0002 /* CONTROL	 */
#define QUAL_AMIGA   0x0004 /* ANY AMIGA */
#define QUAL_ALT     0x0008 /* ANY ALT	 */
#define QUAL_LMB     0x0010 /* LEFT MOUSE BUTTON   */
#define QUAL_MMB     0x0020 /* MIDDLE MOUSE BUTTON */
#define QUAL_RMB     0x0040 /* RIGHT MOUSE BUTTON  */

#define QUAL_UP      0x0080 /* RELEASE - currently this MUST be 0x80 - that value is used in im.Code */

#define QUAL_XTX     0x0100 /* Extended Qualifiers for multi-key strokes - Removed after each keypress */
#define QUAL_XTY     0x0200
#define QUAL_XTZ     0x0400

#define QUAL_REP     0x0800 /* REPEAT */

#define QUAL_PRX     0x1000 /* Permanent Extended Qualifiers for multi-key strokes */
#define QUAL_PRY     0x2000
#define QUAL_PRZ     0x4000

#define QUAL_NLK     0x8000 /* NUMLOCK - refers only to numeric keypad */


/* additional pseudo-intuition qualifiers - referring to the Quals above */
#define EXTQUALIFIER_X	0x00100000
#define EXTQUALIFIER_Y	0x00200000
#define EXTQUALIFIER_Z	0x00400000

#define EXTQUALIFIER_NLK 0x0800000

#define PERQUALIFIER_X	0x01000000
#define PERQUALIFIER_Y	0x02000000
#define PERQUALIFIER_Z	0x04000000

#define EXTQUALIFIER_UP 0x00800000 /* this one is not used yet - the key-up information is part of ie.keyCode */


/* some hardcoded ( I hope ) keys on Amiga Keyboard */
#define CSPC  0x40 /* Spacebar	*/
#define CB_S  0x41 /* BackSpace */
#define CENT  0x43 /* Enter	*/
#define CRET  0x44 /* Return	*/


/**************************************
	    Interne Variable
**************************************/
static UBYTE ctoa[128];    /* code to ascii  */
static UBYTE cstoa[128];   /* code shifted to ascii */

static TQUAL qualignore[128]; /* which key ignores to which qualifier by default */ /* PATCH_NULL [04 Apr 1993] : added */
static UBYTE is_rawc[128];    /* which key can be used in a DeadKeyConversion */    /* PATCH_NULL [30 Apr 1993] : added */

static ULONG lname[] =
{
    /* FuncKeys */
    LN('f','1', 0 , 0  ), LN('f','2', 0 , 0  ), LN('f','3', 0 , 0  ),
    LN('f','4', 0 , 0  ), LN('f','5', 0 , 0  ), LN('f','6', 0 , 0  ),
    LN('f','7', 0 , 0  ), LN('f','8', 0 , 0  ), LN('f','9', 0 , 0  ),
    LN('f','1','0', 0  ),
    /* Specials */
    LN('h','e','l', 0  ), LN('e','s','c', 0  ), LN('d','e','l', 0  ),
    LN('b','a','c', 0  ), LN('b','s', 0 , 0  ), LN('t','a','b', 0  ),
    LN('r','e','t',CRET), LN('e','n','t',CENT), LN('s','p','c',CSPC),
    LN('s','p','a',CSPC),
    /* Cursor */
    LN('u','p', 0 , 0  ), LN('d','o','w', 0  ), LN('r','i','g', 0  ),
    LN('l','e','f', 0  ),
    /* Numeric digits */
    LN('n','k','0', 0  ), LN('n','k','1', 0  ), LN('n','k','2', 0  ),
    LN('n','k','3', 0  ), LN('n','k','4', 0  ), LN('n','k','5', 0  ),
    LN('n','k','6', 0  ), LN('n','k','7', 0  ), LN('n','k','8', 0  ),
    LN('n','k','9', 0  ),
    /* Numeric Specials */
    LN('n','k', 0 , 0  ), LN('n','k', 0 , 0  ), LN('n','k','/', 0  ),
    LN('n','k','*', 0  ), LN('n','k','+', 0  ), LN('n','k','-', 0  ),
    LN('n','k','.', 0  ),
    /* Mouse Buttons */
    LN('l','m','b',0x68), LN('m','m','b',0x6A), LN('r','m','b',0x69),
    LN('m','m','o',0x6B),
    /* /* WARNING - THESE VALUES MIGHT INTERFERE WITH OTHER KEYS */
    LN('l','m','2',0x67), LN('l','m','3',0x66), LN('l','m','4',0x65),
    LN('m','m','2',0x64), LN('m','m','3',0x63), LN('m','m','4',0x62),
    LN('r','m','2',0x61), LN('r','m','3',0x60), LN('r','m','4',0x5f)
    /* /* WARNING - THESE VALUES MIGHT INTERFERE WITH OTHER KEYS */
};



/**************************************
	   Interne Prototypes
**************************************/


/*****************************************************************************

    NAME
	keyboard_init

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	initializes the variables used in the current Module

    NOTES
	That functon MUST be called before any other call to any
	other functions of the key-modules

    BUGS
	if a dead- and a deadable-key are next to each other,
	tables might get corrupted (but I cannot imagine that)

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	14-Dec-92  b_null included & modified

******************************************************************************/

Prototype void keyboard_init (void);

void keyboard_init (void)
{
    static struct InputEvent ievent = {
	NULL,
	IECLASS_RAWKEY,
	0, 0, 0, NULL,	      /* 0,0 */ /* time */
    };
    UBYTE buf[32];
    short i,   /* TCODE */
	  len; /* short */

/* printf ("keyboard_init\n"); */
/*    clrmem (qualignore, sizeof (qualignore)); */ /* PATCH_NULL [04 Apr 1993] : added */

    /* ---- Init unshifted keys (ctoa) */
    /*	    together w/ longkeys and is_rawc */

    ievent.ie_Qualifier        = 0;
    ievent.ie_position.ie_addr = NULL;
    for (i = 0; i < 128; ++i) {
	ievent.ie_Code = i;

	len = RawKeyConvert (&ievent, buf, 32, NULL);

	qualignore[i] = QUAL_NLK; /* PATCH_NULL [04 Apr 1993] : added */
	is_rawc[i] = (len > 0);   /* PATCH_NULL [30 Apr 1993] : added */

	switch(len) {
	case 1:     /*	ESC/DEL/BS/TAB/NKx  */
	    if ((buf[0] & 0x7F) >= 32 && (buf[0] & 0x7F) < 127)
		ctoa[i] = buf[0];

	    switch(buf[0]) {
	    case 0x1B:	lname[SP_START+1] |= i; break;
	    case 0x7F:	lname[SP_START+2] |= i; break;
	    case 0x08:	lname[SP_START+3] |= i;
			lname[SP_START+4] |= i; break;
	    case 0x09:	lname[SP_START+5] |= i; break;
	    case '@':                                                           /* for setmap cdn     */
	    case '[':                                                           /* for setmap d,f,i,n */
	    case '(': if (i > 0x3A) lname[NS_START+0] |= LN(0,0,*buf,i); qualignore[i] &= ~QUAL_NLK; break; /* for setmap usa/gb  */
	    case '°':                                                           /* for setmap cdn     */
	    case ']':                                                           /* for setmap d,i,f,n */
	    case ')': if (i > 0x3A) lname[NS_START+1] |= LN(0,0,*buf,i); qualignore[i] &= ~QUAL_NLK; break; /* for setmap usa/gb  */
	    case '/': if (i > 0x3A) lname[NS_START+2] |= i; qualignore[i] &= ~QUAL_NLK; break;
	    case '*': if (i > 0x3A) lname[NS_START+3] |= i; qualignore[i] &= ~QUAL_NLK; break;
	    case '+': if (i > 0x3A) lname[NS_START+4] |= i; qualignore[i] &= ~QUAL_NLK; break;
	    case '-': if (i > 0x3A) lname[NS_START+5] |= i; qualignore[i] &= ~QUAL_NLK; break;
	    case '.': if (i > 0x3A) lname[NS_START+6] |= i; qualignore[i] &= ~QUAL_NLK; break;
	    default:
		if (i >= 0x0F && isdigit (buf[0])) {
		    lname[N0_START+buf[0]-'0'] |= i;
		    qualignore[i] &= ~QUAL_NLK; /* PATCH_NULL [04 Apr 1993] : added */
		} /* if */
	    } /* switch */
	    break;

	case 2:     /*	cursor		    */
	    if (buf[0] == 0x9B) {

		switch(buf[1]) {
		case 0x41:  lname[CS_START+0] |= i;  break;
		case 0x42:  lname[CS_START+1] |= i;  break;
		case 0x43:  lname[CS_START+2] |= i;  break;
		case 0x44:  lname[CS_START+3] |= i;  break;
		} /* switch */

	    } /* if */
	    break;

	case 3:     /*	function/help	    */
	    if (buf[0] == 0x9B && buf[2] == 0x7E) {

		if (buf[1] == 0x3F)
		    lname[SP_START+0] |= i;
		if (buf[1] >= 0x30 && buf[1] <= 0x39)
		    lname[buf[1]-0x30+F1_START] |= i;

	    } /* if */
	    break;
	} /* switch */
    } /* for */

    /* ---- init Shifted Keys (cstoa) */

    ievent.ie_Qualifier        = IEQUALIFIER_LSHIFT;
    ievent.ie_position.ie_addr = NULL;
    for (i = 0; i < 128; ++i) {
	ievent.ie_Code = i;

	len = RawKeyConvert (&ievent,buf,32,NULL);

	if (len == 1)
	    cstoa[i] = buf[0];
    } /* for */

    /* ---- calc CtlC - Code */

    {
	struct keyspec ks;

	get_codequal ("c", &ks);

	CtlC = KS_CODE(&ks);
    } /* block */
} /* keyboard_init */



/*****************************************************************************

    NAME
	cqtoa

    PARAMETER
	KEYSPEC *ks

    RESULT
	a ptr to the string that is the readable
	form of that key-stroke (static)

    RETURN
	UBYTE *

    DESCRIPTION

    NOTES
	we return a ptr to a static array of char
	for only use of Qualifiers simply use KS_CODE(ks) == -1

    BUGS
	(none known)

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	14-Dez-92 b_null created
	14-Jan-93 b_null added UP
	04 Apr 93 b_null added REP,NLK,PER*
	16 Jun 93 b_null introduced "struct keyspec"
	29-Jun-94 b_null changed Types

******************************************************************************/

Prototype UBYTE* cqtoa (KEYSPEC *ks);

UBYTE *cqtoa (KEYSPEC *ks)
{
    static UBYTE buf[32];
    UBYTE	*ptr = buf;
    int 	 i;

    /* ---- Build Qual part */

    if (KS_QUAL(ks) & QUAL_SHIFT)
	*ptr++ = 's';
    if (KS_QUAL(ks) & QUAL_CTRL)
	*ptr++ = 'c';
    if (KS_QUAL(ks) & QUAL_ALT)
	*ptr++ = 'a';
    if (KS_QUAL(ks) & QUAL_AMIGA)
	*ptr++ = 'A';
    if (KS_QUAL(ks) & QUAL_LMB)
	*ptr++ = 'L';
    if (KS_QUAL(ks) & QUAL_MMB)
	*ptr++ = 'M';
    if (KS_QUAL(ks) & QUAL_RMB)
	*ptr++ = 'R';

    if (KS_QUAL(ks) & QUAL_XTX)
	*ptr++ = 'x';
    if (KS_QUAL(ks) & QUAL_XTY)
	*ptr++ = 'y';
    if (KS_QUAL(ks) & QUAL_XTZ)
	*ptr++ = 'z';

    if (KS_QUAL(ks) & QUAL_PRX)
	*ptr++ = 'X';
    if (KS_QUAL(ks) & QUAL_PRY)
	*ptr++ = 'Y';
    if (KS_QUAL(ks) & QUAL_PRZ)
	*ptr++ = 'Z';

    if (KS_QUAL(ks) & QUAL_NLK)
	*ptr++ = 'n';
    if (KS_QUAL(ks) & QUAL_REP)
	*ptr++ = 'r';
    if (KS_QUAL(ks) & QUAL_UP)
	*ptr++ = 'u';
/*
**  --- That little fragment was added
**	to be able to get the extended qualifiers with already existing functions
**	U might uncomment it, if U wanna get information about
**	which quals have been set with QUALIFIER
*/
    if (KS_CODE(ks) == (TCODE)(-1)) {
/* printf("U used a wrong undef\n"); */
	*ptr = 0;
	return(buf); /* do not append a code-value */
    } /* if */

    /* ---- Append the '-' between Qual & Code */

    /* if (KS_QUAL(ks)) */       /* commented out for better readable keyfiles */
	*ptr++ = '-';

    /* ---- Try it as a Long Name */

    for (i = 0; i < sizeof(lname)/sizeof(lname[0]); ++i) {
	if ((lname[i]&0xFF) == KS_CODE(ks)) {
	    *ptr++ = (lname[i]>>24);
	    *ptr++ = (lname[i]>>16);
	    *ptr++ = (lname[i]>>8);
	    break;
	} /* if */
    } /* for */

    if (i == sizeof(lname)/sizeof(lname[0])) {

	/* ---- Try it as a normal Code */

	*ptr = ctoa[KS_CODE(ks)];

	if (*ptr) {
	    ptr ++;
	/* PATCH_NULL [14 Feb 1993] : direct access to rawcodes ... */
	} else {

	    /* ---- Try it as a HexValue */

	    sprintf (ptr, "0x%02x",KS_CODE(ks));
	    ptr += 4;
	} /* if */
	/* PATCH_NULL [14 Feb 1993] : ... direct access to rawcodes */
    } /* if no spcname */

    *ptr = 0;
    return buf;
} /* cqtoa */


/*****************************************************************************

    NAME
	get_codequal

    PARAMETER
	VAL (char *, string)
	VAR (struct keyspec, ks)

    RESULT
	success: TRUE == ok; FALSE == error (unknown string)

    RETURN
	BOOL

    DESCRIPTION
	we try to extract Qualifier- and Keycode -information
	from an ascii-String and put the found values into
	pcode and pqual

    NOTES
	for only use of Qualifiers simply use code == -1

    BUGS
	(none known)

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	14-Dec-92 b_null included & modified
	14-Jan-93 b_null added UP
	04 Apr 93 b_null added REP,NLK,PER*
	16 Jun 93 b_null introduced "struct keyspec"
	29-Jun-94 b_null changed Types

******************************************************************************/

Prototype BOOL get_codequal (const UBYTE *str, KEYSPEC *ks);

BOOL get_codequal (const UBYTE *str, KEYSPEC *ks)
{
    /* const char * base = str; */
    TQUAL	 qual;
    short	 i;
    short	 has_qual = 0;

    KS_MASK(ks) = QUALMASK;

    qual = 0;

    /* ---- Get the Qualifier */

    { /* PATCH_NULL 03-09-94 added: check _IF_ there is a qualifier */
	int f=0;
	const char *p = str;
	while (*p) {
	    if (f) {
		has_qual = 1;
		break;
	    }
	    f = (*(p++) == '-');
	}
    }

    if (has_qual) {
	for (; *str && (*str) && (*str != '-'); ++str) {
	    /* ---- Why the hell did I use IF insted of SWITCH? */
	    if (*str == 's')
		qual |= QUAL_SHIFT;
	    if (*str == 'c')
		qual |= QUAL_CTRL;
	    if (*str == 'a')
		qual |= QUAL_ALT;
	    if (*str == 'A')
		qual |= QUAL_AMIGA;
	    if (*str == 'L')
		qual |= QUAL_LMB;
	    if (*str == 'M')
		qual |= QUAL_MMB;
	    if (*str == 'R')
		qual |= QUAL_RMB;

	    if (*str == 'x')
		qual |= QUAL_XTX;
	    if (*str == 'y')
		qual |= QUAL_XTY;
	    if (*str == 'z')
		qual |= QUAL_XTZ;

	    if (*str == 'X')
		qual |= QUAL_PRX;
	    if (*str == 'Y')
		qual |= QUAL_PRY;
	    if (*str == 'Z')
		qual |= QUAL_PRZ;

	    if (*str == 'n')
		qual |= QUAL_NLK;
	    if (*str == 'u')
		qual |= QUAL_UP;
	    if (*str == 'r')
		qual |= QUAL_REP;

	    if (!qual) {
		/* error (mal_formed_key); */
		goto notqual;
	    }
	} /* for */

	++str;
	/* PATCH_NULL 03-09-94 disabled the test, its done before */
	/* ---- test if that was really a Qualified Key, and not only a LongName * /
	if (*str == 0) { qual = 0; str = base; } else { ++str; } / * if */
    } /* if */

    /* ---- Get the Code */

notqual:
    if (strlen(str) != 1) {           /* ---- long name */
	short shift = 24;
	long mult = 0;
	UBYTE c;
	const UBYTE * ptr = str; /* PATCH_NULL [14 Feb 1993] : added */

	/* ---- create LongName-Entry */

	KS_QUAL(ks) = qual;
	while ((c = *str) && shift >= 8) {
	    /* c = tolower (c); PATCH_NULL 06-09-94 COMMENTED OUT! */
	    mult |= c << shift;
	    shift -= 8;
	    ++str;
	} /* while */

	/* ---- Search LongNames */

	for (i = 0; lname[i]; ++i) {
	    if (mult == (lname[i] & 0xFFFFFF00)) {
		KS_CODE(ks) = lname[i] & 0xFF;
		return TRUE;
	    } /* if */
	} /* for */

	/* ---- Try it as a HexNumber */

	/* PATCH_NULL [14 Feb 1993] : direct access to raw-codes ... */
	/* NOTE 		    : we cannot save that value yet!!! */
/* printf ("testing %s with hex\n", str); */
	if ((*ptr == '0') && ((ptr[1]|0x20) == 'x')) {
	    i = (strtol (ptr, &str, 0x10) & 0xff);
	    if (ptr != str) {
		KS_CODE(ks) = i;
		return TRUE;
	    } /* if scanned */
	} /* if */
	/* PATCH_NULL [14 Feb 1993] : ... direct access to raw-codes */

    } else {		    /*	---- single character keycap */

	/* ---- check the normal codes */

	for (i = 0; i < sizeof(ctoa); ++i) {
	    if (*str == ctoa[i]) {
		KS_CODE(ks) = i;
		KS_QUAL(ks) = qual;
		return TRUE;
	    } /* if */
	} /* for */

	/* ---- check the shifted codes */

	for (i = 0; i < sizeof(cstoa); ++i) {
	    if (*str == cstoa[i]) {
		KS_CODE(ks) = i;
		KS_QUAL(ks) = qual|QUAL_SHIFT;
		return TRUE;
	    } /* if */
	} /* for */
    } /* if */
    return FALSE;
} /* get_codequal */


/*****************************************************************************

    NAME
	iqual2qual

    PARAMETER
	ULONG  iqual
	int    blen
	char * buf
	int    code ; TCODE code ?

    RESULT
	the [X]DME-internal qualifier referred by the intuition qualifier qual;

    RETURN
	TQUAL

    DESCRIPTION
	transform the different intuition - Qualifiers to [X]DME-Qualifiers
	(if we have caps-lock and buffer contains
	one lower char, it is used like shift)

    NOTES

    BUGS
	(none known)

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	14-Dec-92 b_null included & modified
	14-Jan-93 b_null added UP
	04 Apr 93 b_null added REP,NLK,PER*
	29-Jun-94 b_null changed Types

******************************************************************************/

Prototype TQUAL iqual2qual (ULONG qual, int blen, char * buf, int code);

TQUAL iqual2qual (ULONG iqual, int blen, char * buf, int code) /* (unsigned long iqual, int blen, char * buf, TCODE code) */
{
    TQUAL q2 = 0;

    if (iqual & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
	q2 |= QUAL_SHIFT;
    if (iqual & (IEQUALIFIER_CONTROL))
	q2 |= QUAL_CTRL;
    if (iqual & (IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND))
	q2 |= QUAL_AMIGA;
    if (iqual & (IEQUALIFIER_LALT|IEQUALIFIER_RALT))
	q2 |= QUAL_ALT;

/* We should replace "isalpha(buf[1])" by "((buf[1] != ToUpper(buf[1])) || (buf[1] != ToLower(buf[1])))"    */
/* Thats much longer, ok, but that way we can be sure to use a real uppercase character, also for non-Ascii */
/* an alternative was to check for the reverse - MapANSI(&buf[1],1,&buf[4], 1) and then check for */
/* a SHIFT-qual in our result */

    if (!(q2 & QUAL_SHIFT))
	/* if ((iqual & IEQUALIFIER_CAPSLOCK) && blen == 1 && isalpha (buf[1])) */
	if ((iqual & IEQUALIFIER_CAPSLOCK) && (blen == 1) && (isalpha (buf[1]) || ((buf[1] != ToUpper(buf[1])) || (buf[1] != ToLower(buf[1]))))) /* PATCH_NULL 27-09-93 */
	    q2 |= QUAL_SHIFT;

    if (iqual & IEQUALIFIER_LEFTBUTTON)
	q2 |= QUAL_LMB;
    if (iqual & IEQUALIFIER_MIDBUTTON)
	q2 |= QUAL_MMB;
    if (iqual & (IEQUALIFIER_RBUTTON))
	q2 |= QUAL_RMB;

    if (iqual & (EXTQUALIFIER_X))
	q2 |= QUAL_XTX;
    if (iqual & (EXTQUALIFIER_Y))
	q2 |= QUAL_XTY;
    if (iqual & (EXTQUALIFIER_Z))
	q2 |= QUAL_XTZ;

    if (iqual & (PERQUALIFIER_X))
	q2 |= QUAL_PRX;
    if (iqual & (PERQUALIFIER_Y))
	q2 |= QUAL_PRY;
    if (iqual & (PERQUALIFIER_Z))
	q2 |= QUAL_PRZ;

    if (iqual & (EXTQUALIFIER_NLK))
	q2 |= QUAL_NLK;
    if (iqual & (IEQUALIFIER_REPEAT))
	q2 |= QUAL_REP;

#if 0
    if (iqual & (EXTQUALIFIER_UP))
	q2 |= QUAL_UP;
    q2 |= (QUAL_UP & code);
#endif

    if (code & 0x080)
	q2 |= QUAL_UP;

    return q2;
} /* iqual2qual */


/*****************************************************************************

    NAME
	qual2iqual

    PARAMETER
	TQUAL qual ; might be changed to KEYSPEC

    RESULT
	Intuition Qualifiers, which represent the [X]DME - qualifiers

    RETURN
	ULONG

    DESCRIPTION
	simple transformation of internal representation for
	Qualifiers to a possible Amiga-representation

    NOTES
	very slow
	!ATTN!	there is currently no possibility to
		transform the UP-Qualifier

    BUGS
	QUAL_UP is not supported (we needed a Code for it)

    EXAMPLES

    SEE ALSO
	iqual2qual

    INTERNALS

    HISTORY
	14-Dez-92 b_null created
	14-Jan-93 b_null added UP
	04 Apr 93 b_null added REP,NLK,PER*
	29-Jun-94 b_null changed Types

******************************************************************************/

Prototype ULONG qual2iqual (TQUAL qual);

ULONG qual2iqual (TQUAL qual)
{
    ULONG iq2 = 0;

    if (qual & QUAL_XTX)
	iq2 |= EXTQUALIFIER_X;
    if (qual & QUAL_XTY)
	iq2 |= EXTQUALIFIER_Y;
    if (qual & QUAL_XTZ)
	iq2 |= EXTQUALIFIER_Z;

    if (qual & QUAL_PRX)
	iq2 |= PERQUALIFIER_X;
    if (qual & QUAL_PRY)
	iq2 |= PERQUALIFIER_Y;
    if (qual & QUAL_PRZ)
	iq2 |= PERQUALIFIER_Z;

    if (qual & QUAL_NLK)
	iq2 |= EXTQUALIFIER_NLK;
    if (qual & QUAL_REP)
	iq2 |= IEQUALIFIER_REPEAT;
#if 0
    if (qual & QUAL_UP)        /* that info must be placed into Code */
	iq2 |= EXTQUALIFIER_UP;
#endif

    if (qual & QUAL_SHIFT)
	iq2 |= IEQUALIFIER_LSHIFT;
    if (qual & QUAL_CTRL)
	iq2 |= IEQUALIFIER_CONTROL;
    if (qual & QUAL_AMIGA)
	iq2 |= IEQUALIFIER_LCOMMAND;
    if (qual & QUAL_ALT)
	iq2 |= IEQUALIFIER_LALT;
    if (qual & QUAL_LMB)
	iq2 |= IEQUALIFIER_LEFTBUTTON;
    if (qual & QUAL_MMB)
	iq2 |= IEQUALIFIER_MIDBUTTON;
    if (qual & QUAL_RMB)
	iq2 |= IEQUALIFIER_RBUTTON;

    return iq2;
} /* qual2iqual */


/*****************************************************************************

    NAME
	a2iqual

    PARAMETER
	const UBYTE* str
	ULONG	   *piqual

    RESULT
	success: TRUE == ok; FALSE == any error

    RETURN
	BOOL

    DESCRIPTION
	convert an ascii string into Input-Qualifiers
	we check each char of the string and set the according bit
	the string may be NULL-terminated or "-"-terminated
	any unknown char causes "return (0)"

    NOTES

    BUGS
	(none known)

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	14-Dez-92 null created
	14-Jan-93 b_null added UP
	04 Apr 93 b_null added REP,NLK,PER*
	29-Jun-94 b_null changed Types

******************************************************************************/

Prototype BOOL a2iqual (const UBYTE *str, ULONG *piqual);

BOOL a2iqual (const UBYTE* str, ULONG *piqual)
{
    *piqual = 0;
    for (; *str; str++) {
	switch (*str) {
	case 'a':   *piqual |= IEQUALIFIER_LALT;        break;
	case 'A':   *piqual |= IEQUALIFIER_LCOMMAND;    break;
	case 's':   *piqual |= IEQUALIFIER_LSHIFT;      break;
	case 'L':   *piqual |= IEQUALIFIER_LEFTBUTTON;  break;
	case 'M':   *piqual |= IEQUALIFIER_MIDBUTTON;   break;
	case 'R':   *piqual |= IEQUALIFIER_RBUTTON;     break;

	case 'u':   *piqual |= EXTQUALIFIER_UP;         break;
	case 'n':   *piqual |= EXTQUALIFIER_NLK;        break;
	case 'r':   *piqual |= IEQUALIFIER_REPEAT;      break;

	case 'x':   *piqual |= EXTQUALIFIER_X;          break;
	case 'y':   *piqual |= EXTQUALIFIER_Y;          break;
	case 'z':   *piqual |= EXTQUALIFIER_Z;          break;

	case 'X':   *piqual |= PERQUALIFIER_X;          break;
	case 'Y':   *piqual |= PERQUALIFIER_Y;          break;
	case 'Z':   *piqual |= PERQUALIFIER_Z;          break;

	case '-':   return TRUE;
	default:    return FALSE;
	} /* switch */
    } /* for */
    return TRUE;
} /* a2iqual */


/*****************************************************************************

    NAME
	iqual2a

    PARAMETER
	ULONG iqual

    RESULT
	the string that corresponds to iqual  (static)

    RETURN
	UBYTE *

    DESCRIPTION
	create a string representing the readble form of Intuition Qualifiers

    NOTES
	this is a slow routine, as we do not
	immediately evaluated the string, but
	use iqual2qual and cqtoa (but it works :-)

    BUGS
	(none known)

    EXAMPLES

    SEE ALSO
	iqual2qual(), cqtoa

    INTERNALS

    HISTORY
	14-Dez-92 null created
	29-Jun-94 b_null changed Types

******************************************************************************/

Prototype UBYTE *iqual2a (ULONG iqual);

UBYTE *iqual2a (ULONG iqual)
{
    struct keyspec ks = {0, 0, (UBYTE)~0};
    KS_QUAL(&ks) = iqual2qual (iqual, 0, NULL, 0);
    return (cqtoa (&ks));
} /* iqual2a */


/*****************************************************************************

    NAME
	init_kb
	(exit_kb - removed)

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	entry code for Keyboard Control

    NOTES
	this is the lowest level on Amiga Software, we use;
	exit_kb is removed, so user cannot call it any more
	init_kb is "__AUTOINIT" so user need not call it
	(he must indeed, if his compiler does not support __autoinit)

    BUGS
	for unknown reasons, we must not close ConsoleDevice,
	    else the machine is going to crash

    EXAMPLES

    SEE ALSO
	reset_hash

    INTERNALS
	Open the Console.Device and call keyboard_init

    HISTORY
	15-Dez-92  b_null created
	15-Jan-93  b_null removed exit_kb
	29-06-94   b_null removed KC_Used

******************************************************************************/

Prototype void init_kb (void);

MK_AUTOINIT( init_kb )
{
    struct IOStdReq cio;

#if 1
 printf ("init_kb\n");
#endif
    if (!ConsoleDevice) {
	OpenDevice ("console.device", -1, (struct IORequest *)&cio, 0);
	ConsoleDevice = (struct Library *)cio.io_Device;
	keyboard_init ();
    } /* if */
} /* init_kb */


/* FUNCTION TO BE KILLED AGAIN */

/*****************************************************************************

    NAME
	CtoA

    PARAMETER
	TCODE c

    RESULT
	ctoa[c]

    RETURN
	UBYTE

    DESCRIPTION
	this function was created to hide the stucture ctoa

	ctoa is designed to check, IF a KeyCode c represents a
	single ascii character and WHICH one

    NOTES
	a macro might be possible, too;
	but then You have to declare ctoa non-static

	ctoa is initialized by keyboard_init(), so
	any call to CtoA before keyboard initialisation is undefined

    BUGS
	none known

    EXAMPLES

    SEE ALSO
	ctoa

    INTERNALS

    HISTORY
	30-Dez-92 b_null created
	15-Jan-93 b_null documented
	29-Jun-94 b_null changed Types

******************************************************************************/

Prototype UBYTE  CtoA (TCODE c);

UBYTE CtoA (TCODE c)
{
    return (ctoa[c]);
} /* CtoA */



#ifdef NOT_DEF	 /* FUNCTION TO BE KILLED AGAIN */

/*****************************************************************************

    NAME
	CignoreQ

    PARAMETER
	TCODE c

    RESULT
	qualignore[c]

    RETURN
	TQUAL

    DESCRIPTION
	this function was created to hide the structure qualignore

	qualignore is designed to check, IF a KeyCode c ignores
	any [X]DME qualifiers and WHICH ones

    NOTES
	a macro might be possible, too;
	but then You have to declare qualignore non-static

	qualignore is initialized by keyboard_init(), so
	any call to CignoreQ before keyboard initialisation is undefined

	***  that function is not (yet) used (any more)  ***

    BUGS
	none known

    EXAMPLES

    SEE ALSO
	qualignore

    INTERNALS

    HISTORY
	04 Apr 93 b_null created & documented

******************************************************************************/

Prototype TQUAL CIgnoreQ (TCODE c);

TQUAL CignoreQ (TCODE c)
{
    return (qualignore[c]);
} /* CignoreQ */

#endif	    /* FUNCTION TO BE KILLED AGAIN */


/*****************************************************************************

    NAME
	IsRawC

    PARAMETER
	KEYSPEC *ks

    RESULT
	is_rawc[c]

    RETURN
	Bool

    DESCRIPTION
	this function was created to hide the structure is_rawc
	lateron I added a check for QUAL_UP

	is_rawc is designed to check for the possibility, if
	we can use a keycode as RawKey for DeadKeyConvert or not

    NOTES
	a macro might be possible, too;
	but then You have to declare is_rawc non-static

	is_rawc is initialized by keyboard_init(), so
	any call to IsRawC before keyboard initialisation
	is undefined

    BUGS
	none known

    EXAMPLES

    SEE ALSO
	is_rawc

    INTERNALS

    HISTORY
	30 Apr 93 b_null created & documented
	17 Jun 93 b_null introduced "struct keyspec" , QUAL_UP-check
	27-06-94  b_null added check for QUAL_ALT/SHIFT
	29-Jun-94 b_null changed Types

******************************************************************************/

Prototype BOOL IsRawC (KEYSPEC *ks);

BOOL IsRawC (KEYSPEC *ks)
{
    return (BOOL)((((is_rawc[KS_CODE(ks)] != 0) || KS_QUAL(ks) & (QUAL_SHIFT|QUAL_ALT))&& ((KS_QUAL(ks) & QUAL_UP) == 0))? TRUE: FALSE);
} /* IsRawC */


/*****************************************************************************

    NAME
	DeadKeyConvert

    PARAMETER
	struct IntuiMessage * msg
	UBYTE * 	      buf
	int		      bufsize
	struct KeyMap *       keymap

    RESULT
	actual - the number of characters in the buffer, -2 if no
	RawKey was given or -1 if a buffer overflow was about to occur.

    RETURN
	int

    DESCRIPTION
	convert any RawKey-Code to (VanillaKey) an ASCII-String

    NOTES
	this function is only used by keycontrol.keyctl
	but it for itself uses ConsoleDevice, so I could not
	decide where to put it.

    BUGS

    EXAMPLES

    SEE ALSO
	console.device/RawKeyConvert

    INTERNALS
	erzeuge eine iorequest mit den allernotwendigsten Eintraegen
	und lasse sie auswerten

    HISTORY
	15-Dez-92 b_null moved to keycontrol.c
	16.Dez.92 b_null documented
	15-Jan-93 b_null moved to keycodes.c

******************************************************************************/

Prototype int DeadKeyConvert (struct IntuiMessage * msg, UBYTE * buf, int bufsize, struct KeyMap * keymap);

int DeadKeyConvert (struct IntuiMessage * msg, UBYTE * buf, int bufsize, struct KeyMap * keymap)
{
    static struct InputEvent ievent = {
	NULL, IECLASS_RAWKEY
    };

    if (msg->Class != IDCMP_RAWKEY)
	return(-2);

    ievent.ie_Code	       = msg->Code;
    ievent.ie_Qualifier        = msg->Qualifier;
    ievent.ie_position.ie_addr = *((APTR *)msg->IAddress);

/* printf ("DeadKeyConvert %02x + %08lx\n", msg->Code, ievent.ie_position.ie_addr); */

    return (RawKeyConvert (&ievent, (char *)buf, bufsize, keymap));
} /* DeadKeyConvert */


/******************************************************************************
*****  ENDE keycodes.c
******************************************************************************/

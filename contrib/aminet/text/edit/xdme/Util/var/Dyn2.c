
/******************************************************************************

    MODULE
	DYN.c

    DESCRIPTION
	Simple Dynamic Strings in "C"

    NOTES
      * this file contains 2 implementations:
	one is using bigger base variables (12bytes) and is a little bit faster
	the other is using smaller base variables (4bytes) and is a little bit slower
      * for debugging purposes (only one function to modify)
	DynCpy is using DynCat, and DynCat is using DynIns,
	so the functions DynCpy and DynCat are much slower than necessary

    BUGS
	none known

    TODO
	tell me

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	15-09-94 b_noll created

******************************************************************************/


/**************************************
	      Includes
**************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef   DYN_H
#include "Dyn.h"
#endif /* DYN_H */

/**************************************
	    Global Variables
**************************************/

/*char *DynValue	(DSTR *pstr);
void  DynInit	(DSTR *pstr);
int   DynLength (DSTR *pstr);
void  DynClear	(DSTR *pstr);

BOOL  DynCpy	(DSTR *pstr, const char *src);
BOOL  DynCat	(DSTR *pstr, const char *src);
BOOL  DynIns	(DSTR *pstr, const char *src, int pos);
BOOL  DynDel	(DSTR *pstr, int pos, int chars);*/

/**************************************
      Internal Defines & Structures
**************************************/

#if 0
#if REF_VERS

struct _DSTR {
    int   len;
    int   use;
    char *str;
}; /* struct _DSTR */

#define DSTR struct _DSTR

#else

struct _DSTR {
    int   len;
    int   use;
    char  str[1];
}; /* struct _DSTR */

//#define DSTR struct _DSTR *

#endif

#if REF_VERS
#else
#endif
#endif

#define STRLEN	    256     /* each dynamic string is sized (1+strlen()/STRLEN)*STRLEN ... */

/**************************************
	   Internal Variables
**************************************/

#if REF_VERS
DSTR _EmptyDyn = {0,0,NULL};
#else
#define _EmptyDyn NULL;
#endif

/**************************************
	   Internal Prototypes
**************************************/


/**************************************
		 Macros
**************************************/


/**************************************
	     Implementation
**************************************/

#if REF_VERS

void DynInit (DSTR *pstr) {
    assert(pstr != NULL);
    *pstr = _EmptyDyn;
} /* DynInit */

void DynClear (DSTR *pstr) {
    assert(pstr != NULL);
    if (DynValue(pstr))
	free (DynValue(pstr));
    DynInit(pstr);
} /* DynClear */

char *DynValue (DSTR *pstr) {
    assert(pstr != NULL);
    return pstr->str;
} /* DynValue */

int DynLen (DSTR *pstr) {
    assert(pstr != NULL);
    return pstr->use;
} /* DynLen */

BOOL  DynDel	(DSTR *pstr, int pos, int chars) {
    assert(pstr != NULL);

    /* ---- Empty? or nothing to Delete */
    if (!pstr->str || (pstr->use < pos) || !chars)
	return TRUE;

    /* ---- End Truncated? */
    if (pstr->use < pos + chars) {
	pstr->use = pos;
	pstr->str[pos] = 0;
	return TRUE;
    } /* if */

    /* ---- Midth Deleted? */
    memcpy (pstr->str + pos, pstr->str + pos + chars, pstr->use + 1 - (chars + pos));
    pstr->use -= chars;
    return TRUE;
} /* DynDel */

BOOL  DynIns	(DSTR *pstr, const char *src, int pos) {
    int   slen, tlen;
    char *inter;
    assert(pstr != NULL);

    /* ---- make sure we have consistent data */
    if (!pstr->str)
	pstr->use = pstr->len = 0;

    /* ---- Empty append? */
    if (!src)
	return TRUE;
    slen = strlen (src);
    if (!slen)
	return TRUE;

    /* ---- Fits into old String? */
    if (slen + pstr->use < pstr->len) {
	strins (pstr->str + pos, src); /* DIFF1_2_Cat */
	pstr->use += slen;
	return TRUE;
    } /* if */

    /* ---- New Mem needed */
    if (!(inter = malloc (tlen = (STRLEN * (1 + pstr->use + slen + STRLEN - 1)/STRLEN ))))
	return FALSE;

    strcpy (inter, pstr->str);
    free   (pstr->str);
    strins (inter + pos, src); /* DIFF2_2_Cat */

    pstr->use += slen;
    pstr->len  = tlen;
    pstr->str  = inter;
    return TRUE;
} /* DynIns */

BOOL  DynCat	(DSTR *pstr, const char *src) {
    assert(pstr != NULL);
    return DynIns (pstr, src, pstr->use);
} /* DynCat */

BOOL  DynCpy	(DSTR *pstr, const char *src) {
    DynClear(pstr);
    return DynCat  (pstr, src);
} /* DynCpy */

/* Bonus1 ... */
#if 0
void vDynAppend (DSTR *pstr, int num, va_list adds) {
    assert(pstr != NULL);

    /* ---- make sure we have consistent data */
    if (!pstr->str)
	pstr->use = pstr->len = 0;

    /* ---- append all adds to str */
    while (num-- > 0) {
	int   il;
	char* istr;
	istr = va_arg(adds, char*);

	/* ---- ignore empty strings */
	if (!istr)
	    continue;

	/* ---- make sure we have enough mem */
	il = strlen (istr);
	if ((pstr->use + il) >= pstr->len) {
	    int   len;
	    char *inter;
	    len   = ((pstr->use + il + STRLEN) / STRLEN) * STRLEN;
	    if (!(inter = malloc(len)) )
		return FALSE;
	    if (pstr->str) {
		strcpy(inter, pstr->str);
		free (pstr->str);
	    } else
		*inter = 0;
	    pstr->str = inter;
	    pstr->len = len;
	} /* if */

	/* ---- append the string */
	strcpy (pstr->str + pstr->use, istr);
	pstr->use += il;
    } /* while */
    return TRUE;
} /* vDynAppend */
#endif

/* Bonus 2... */
BOOL DynStrip (DSTR *pstr) {
    int   i;
    char *str;

    assert(pstr != NULL);

    i = DynLength (pstr);
    str = DynValue(pstr);

    if (!i)
	return FALSE;

    while ((i >=0) && (str[i] < 33))
	str[i--] = 0;

    if (i < 0) {
	DynClear(pstr);
	return TRUE;
    } else if (pstr->len > i + 1) {
	pstr->len = i + 1;
	return TRUE;
    } else
	return FALSE;
} /* DynStrip */


#else


#ifndef DynInit
void DynInit (struct _DSTR *pstr)
{
    assert(pstr != NULL);
    pstr->len = 0;
    pstr->use = 0;
    pstr->str = NULL;
} /* DynInit */
#endif

void DynClear (DSTR *pstr) {
    assert(pstr != NULL);
    if (pstr->str)
	free (pstr->str);
    DynInit(pstr);
} /* DynClear */

char *DynValue (DSTR *pstr) {
    assert(pstr != NULL);
    if (pstr->str)
	return (pstr)->str;
    return "";
} /* DynValue */

int DynLen (DSTR *pstr) {
    assert(pstr != NULL);
    if (pstr->str)
	return (pstr)->use;
    return 0;
} /* DynLen */

BOOL  DynDel	(DSTR *pstr, int pos, int chars) {
    struct _DSTR *str;

    assert(pstr != NULL);
    *str = *pstr;
    /* ---- Empty? or nothing to Delete */
    if (!pstr || (pstr->use < pos) || !chars)
	return TRUE;

    /* ---- End Truncated? */
    if (dstr->use < pos + chars) {
	dstr->use = pos;
	dstr->str[pos] = 0;
	return TRUE;
    } /* if */

    /* ---- Midth Deleted? */
    memcpy (dstr->str + pos, dstr->str + pos + chars, dstr->use + 1 - (chars + pos));
    dstr->use -= chars;
    return TRUE;
} /* DynDel */

BOOL  DynIns	(DSTR *pstr, const char *src, int pos) {
    int   slen, tlen, use, len;
    char *inter;
    struct _DSTR *dstr;
    assert(pstr != NULL);

    /* ---- make sure we have consistent data */
    if (!(dstr = *pstr))
	use = len = 0;
    else {
	use = dstr->use;
	len = dstr->len;
    } /* if */

    /* ---- Empty append? */
    if (!src)
	return TRUE;
    slen = strlen (src);
    if (!slen)
	return TRUE;

    /* ---- Fits into old String? */
    if (slen + use < len) {
	strins (dstr->str + pos, src); /* DIFF1_2_Cat */
	dstr->use += len;
	return TRUE;
    } /* if */

    /* ---- New Mem needed */
    if (!(inter = malloc (tlen = (STRLEN * (sizeof(*dstr) + use + slen + STRLEN - 1)/STRLEN ))))
	return FALSE;

    strcpy (inter, dstr->str);
    free   (dstr);
    strins (inter + pos, src); /* DIFF2_2_Cat */
    *pstr = dstr = inter;
    dstr->use	 = use + slen;
    dstr->len	 = tlen;
    return TRUE;
} /* DynIns */

BOOL  DynCat	(DSTR *pstr, const char *src) {
    assert(pstr != NULL);
    return DynIns (pstr, src, (*pstr)? (*pstr)->use: 0);
} /* DynCat */

BOOL  DynCpy	(DSTR *pstr, const char *src) {
    DynClear(pstr);
    return DynCat  (pstr, src);
} /* DynCpy */


#endif


/******************************************************************************
*****  END DYN.c
******************************************************************************/


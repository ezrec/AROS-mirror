
/* **************************************** */

/* DYNAMIC ARRAYS */

/* **************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Dyn.h"

#define STRLEN	    256     /* each dynamic string is sized (2+strlen()/STRLEN)*STRLEN ... */

/* We do support the following functions (at the moment) */
/* some of'em might be implemented as Macros! */
/*
char*DynValue  (STRING *pstr);
void DynInit   (STRING *pstr);
int  DynLength (STRING *pstr);
void DynClear  (STRING *pstr);
void vDynAppend(STRING *pstr, int num, va_list adds);
void DynSet    (STRING *pstr, int num, ...);
void DynAppend (STRING *pstr, int num, ...);
BOOL DynApp1   (STRING *dest, char* str);

int  DynStrip	 (STRING *pstr);
int  DynDecomment(STRING *pstr);

extern const STRING  EmptyDyn;
extern const STRING _EmptyDyn;
*/


#if 0
struct _DSTR {
    int   len;
    int   use;
    char *str;
}; /* struct DSTR */

#define DSTR	  struct _DSTR

#define DynValue(pstr)  ((pstr)->str)
#define DynLength(pstr) ((pstr)->use)
#endif

const DSTR _EmptyDyn = { 0,0,NULL };
#define       EmptyDyn	 { 0,0,NULL }


void DynInit (DSTR *pstr) {

    assert(pstr != NULL);

    *pstr = _EmptyDyn;
} /* DynInit */


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
	    pstr->len = ((pstr->use + il + STRLEN) / STRLEN) * STRLEN;
if (pstr->str)
	    pstr->str = realloc(pstr->str, pstr->len);
else
	    pstr->str = malloc(pstr->len);
if (!pstr->str)
fprintf (stderr, "malloc err; len=%d\n", pstr->len);
	    assert (pstr->str != NULL);
	} /* if */

	/* ---- append the string */
	strcpy (pstr->str + pstr->use, istr);
	pstr->use += il;
    } /* while */

} /* vDynAppend */


void DynClear (DSTR *pstr) {

    assert(pstr != NULL);

    if (DynValue(pstr))
	free (DynValue(pstr));

    DynInit(pstr);
} /* DynClear */


BOOL DynStrip (DSTR *pstr) {
    int   i;
    char *str;

    assert(pstr != NULL);

    i = DynLength (pstr);
    str = DynValue(pstr);

    if (!i)
	return 0;

    while ((i >=0) && (str[i] < 33))
	str[i--] = 0;

    if (i < 0) {
	DynClear(pstr);
	return 1;
    } else if (pstr->len > i + 1) {
	pstr->len = i + 1;
	return 1;
    } else
	return 0;
} /* DynStrip */


int DynDecomment (DSTR *pstr) {
    char *ptr;

    assert(pstr != NULL);

    if (NULL != (ptr = strstr (DynValue(pstr), "//"))) {
	*ptr = 0;
	if (ptr == DynValue(pstr)) {
	    DynClear(pstr);
	} else {
	    pstr->len = ((unsigned long)ptr - (unsigned long)DynValue(pstr));
	    DynStrip(pstr);
	} /* if */
	return 1;
    } /* if */
    DynStrip(pstr);
    return 0;
} /* DynDecomment */



void DynSet (DSTR *pstr, int num, ...) {
    va_list adds;

    assert(pstr != NULL);

    va_start(adds, num);
    DynClear  (pstr);
    vDynAppend(pstr, num, adds);
    va_end(adds);

} /* DynSet */


void DynAppend(DSTR *pstr, int num, ... ) {
    va_list adds;

    assert(pstr != NULL);

    va_start(adds, num);
    vDynAppend(pstr, num, adds);
    va_end(adds);

} /* DynAppend */


BOOL DynApp1 (DSTR *dest, char* str) {

    assert(dest != NULL);

    vDynAppend(dest, 1, (va_list)&str);
    return 1;
} /* DynApp1 */




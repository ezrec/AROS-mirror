/*
 * $Id$
 *
**  Dieses modul ist die Memory import schnittstelle
**  fuer menu... und key...
**  ie. beide modulkomplexe sollten keine
**	anderen als die hier exportierten alloc/free - funktionen
**	verwenden.
**  sobald sichergestellt ist, dass aller in diesen beiden modulen
**  allozierter speicher wieder freigegeben wird, sollte
**  malloc in AllocMem und free in FreeMem umgewandelt werden,
**  um wenigstens einen kleineren Geschwindigkeitszuwachs zu
**  erzeugen;
**
**	(this is done for sure)
 *
*/


#include "defs.h"

#define  DEBUG_C
/* #include "debug_mem.h" */


Prototype APTR AllocFunc (long siz, long typ);

APTR AllocFunc (long siz, long typ)
{
    if (siz) {
	/* --- Wir koennen jetzt AllocMem und FreeMem verwenden, nehme ich an; alle Allokationen in Menu/key... sollten jetzt bei PgmEnde wieder freigegeben werden */
	/* return (malloc (siz)); */
	return (AllocMem (siz, typ));
    } /* if */

    return (NULL);
} /* AllocFunc */


Prototype void FreeFunc (APTR mem, long siz);

void FreeFunc (APTR mem, long siz)
{
    /* --- Wir koennen jetzt AllocMem und FreeMem verwenden, nehme ich an; alle Allokationen in Menu/key... sollten jetzt bei PgmEnde wieder freigegeben werden */

    if (mem) {
	FreeMem (mem, siz);
	/* free (mem); */
    } /* if */
} /* FreeFunc */


/*
 * Alle folgenden Funktionen sind
 * verwendbar wie Makros;
 * sie verwenden samt und sonders AllocFunc und FreeFunc,
 * daher reicht es, diese beiden Functionen umzudefinieren,
 * um andere memeory-routinen zu verwenden
 */

/*
 *  Deallociere den string str
 */

Prototype void DeallocFunc (char * str);

void DeallocFunc (char * str)
{
    if (str) {
	FreeFunc (str, strlen (str)+1);
    } /* if */
} /* DeallocFunc */


/*
 *  Dupliziere den string str
 */

Prototype char * DupFunc (const char * str, long typ);

char * DupFunc (const char * str, long typ)
{
    char * new;
    if (str && (new = AllocFunc(strlen(str)+1, typ))) {
	strcpy (new, str);
	return (new);
    } /* if */

    return (NULL);
} /* DupFunc */


/******************************************************************************
***** END mem.c
******************************************************************************/


/*
 *  tcp_locale.c - The locale function(s)
 *
 * $Date$
 * $Revision$
 *
 *  40.1
 *  20010719 DM Created. Only contains function for reading locale strings from
 *              the CatCompArray
 *
 */

#include <exec/types.h>
#include <proto/locale.h>

#define CATCOMP_ARRAY
#include "tcp_locale.h"

struct Catalog  *tcp_Catalog = NULL;/* Pointer to a catalog if we opne one */
static STRPTR   EmptyStr = "";      /* An empty string - pointer to this returned if we cannot find the locale string */


STRPTR GetLocString(ULONG msgid)
{
	STRPTR  str = EmptyStr; /* String corresponding to the ID we were passed */
	int     i;              /* index into catcomp array while parsing for built-in string */
	int     maxstr;         /* Maximum number of strings in array */

	/* Find default string from built-in CatComp array */
	maxstr = sizeof(CatCompArray)/sizeof(CatCompArray[0]);
	for(i = 0; i < maxstr; i++)
	{
		if(CatCompArray[i].cca_ID == msgid)
			str = CatCompArray[i].cca_Str;
	}

	if(i != maxstr)
	{
		/* Try to get string from catalog, only if the string was found in the
		 * defaults. If it wasn't found, chances are it will not be in the
		 * catalog, or at least will not be correct. Anyway, what's the program
		 * doing, searching for a string not part of the program?
		 */
		if(LocaleBase)
		{
			str = (STRPTR)GetCatalogStr(tcp_Catalog, (LONG)msgid, str);
		}
	}
	return(str);
}


/************************************************************
* xprzmodem_locale.c:  initialize the locale-strings;
*
* Version 2.63, 30 July 1993 build in locale, by Rainer Hess
*************************************************************/

#include <exec/types.h>
#include <libraries/locale.h>
#include <clib/locale_protos.h>
#include <pragmas/locale_pragmas.h>

#define CATCOMP_ARRAY

#include "xprzmodem_catalog.h"

STRPTR GetLocalString (struct LocaleInfo *li, LONG id);

#define LocaleBase li->li_LocaleBase
#define catalog    (struct Catalog *)li->li_Catalog

static UBYTE NULLSTRING[] = "";


STRPTR
GetLocalString (struct LocaleInfo *li, LONG id)
{
  const struct CatCompArrayType *as;
  STRPTR s = NULLSTRING;
  int k, l;

  l = sizeof (CatCompArray) / sizeof (CatCompArray[0]);
  as = CatCompArray;
  for (k = 0; k < l; k++, as++)
  {
    if (as->cca_ID == id)
    {
      s = as->cca_Str;
      break;
    }
  }

  if ((LocaleBase) && (catalog) && (*s))
    s = GetCatalogStr (catalog, id, s);

  return (s);
}
/* End of xprzmodem_locale.c source */
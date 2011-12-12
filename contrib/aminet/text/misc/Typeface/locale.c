#define CATCOMP_ARRAY
#include "locale.h"
#include "typeface.h"
#include <libraries/locale.h>
#include <proto/locale.h>

/*********************************************************************************************/

struct LocaleBase *LocaleBase;
struct Catalog *catalog;

/*********************************************************************************************/

void InitLocale(STRPTR catname, ULONG version)
{
#ifdef __AROS__
    LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library", 39);
#else
    LocaleBase = (struct Library    *)OpenLibrary("locale.library", 39);
#endif
    if (LocaleBase)
    {
	catalog = OpenCatalog(NULL, catname, OC_Version, version,
					     TAG_DONE);
    }
}

/*********************************************************************************************/

void CleanupLocale(void)
{
    if (catalog) CloseCatalog(catalog);
    if (LocaleBase) CloseLibrary((struct Library *)LocaleBase);
}

/*********************************************************************************************/

CONST_STRPTR GetString(ULONG id)
{
    CONST_STRPTR retval;
    
    if (catalog)
    {
	retval = GetCatalogStr(catalog, id, CatCompArray[id].cca_Str);
    }
    else
    {
	retval = CatCompArray[id].cca_Str;
    }
    
    return retval;
}

/*********************************************************************************************/

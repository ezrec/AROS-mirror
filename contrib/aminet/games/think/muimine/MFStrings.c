#include "MUIMine.h"
#include <libraries/locale.h>
#include <clib/locale_protos.h>
#include <pragmas/locale_pragmas.h>

#define CATCOMP_ARRAY
#include "MUIMineLocale.h"



/*
    locale variables
*/
#ifdef __AROS__
struct LocaleBase * LocaleBase = NULL;
#else
struct Library * LocaleBase = NULL;
#endif
struct Catalog * Catalog = NULL;

LONG StringTableSize = 0;


/*
    function :      gets a string

    parameters :    id = id of string to get

    return :        pointer to the string or NULL
*/
STRPTR GetStr(LONG id)
{
    int i;
    struct CatCompArrayType const * table = CatCompArray;
    STRPTR pRet = NULL;

    /*
        itterate the table looking for the given id
    */
    for (i = StringTableSize; i > 0; i--)
    {
        if (table->cca_ID == id)
        {
            /*
                found the entry, set the return value to this entry's
                string and exit the loop
            */
            pRet = table->cca_Str;
            if (LocaleBase)
            {
                pRet = (char *)GetCatalogStr(Catalog, id, pRet);
            }
            break;
        }
        table++;    // next entry
    }

    return pRet;
}

/*
    function :      opens strings, opens the locale.library and the catalog
                    and initialize the string table
*/
void OpenStrings(void)
{
    if ((LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library", 38)))
    {
        Catalog = OpenCatalog(NULL, "MUIMine.catalog", TAG_DONE);
    }

    StringTableSize = sizeof(CatCompArray) / sizeof(struct CatCompArrayType);
}

/*
    function :      close strings, close the catalog and locale.library
*/
void CloseStrings(void)
{
    if (Catalog)
    {
        CloseCatalog(Catalog);
        Catalog = NULL;
    }

    if (LocaleBase)
    {
        CloseLibrary((struct Library *)LocaleBase);
        LocaleBase = NULL;
    }
}



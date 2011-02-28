
#define CATCOMP_ARRAY
#include "class.h"

/****************************************************************************/

#define CATNAME "BWin.catalog"

static ULONG shapeIds[] = {MCPMSG_Shape_None,MCPMSG_Shape_Original,MCPMSG_Shape_Line,MCPMSG_Shape_Solid,0};
STRPTR shapeStrings[sizeof(shapeIds)/sizeof(ULONG)];

void ASM
initStrings(void)
{
    STRPTR *f;
    ULONG  *ids;

    if ((LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library",0)) &&
        (lib_cat = OpenCatalogA(NULL,CATNAME,NULL)))
    {
        struct Catalog             *cat = lib_cat;
        struct CatCompArrayType    *cca;
        int                        cnt;

        for (cnt = (sizeof(CatCompArray)/sizeof(struct CatCompArrayType))-1, cca = (struct CatCompArrayType *)CatCompArray+cnt;
             cnt>=0;
             cnt--, cca--)
        {
            STRPTR s;

            if (s = GetCatalogStr(cat,cca->cca_ID,cca->cca_Str)) cca->cca_Str = s;
        }
    }

    for (ids = shapeIds, f = shapeStrings; *ids; ids++, f++)
        *f = getString(*ids);
}

/****************************************************************************/

CONST_STRPTR ASM
getString(REG(d0) LONG id)
{
    struct CatCompArrayType    *cca;
    int                        cnt;

    for (cnt = (sizeof(CatCompArray)/sizeof(struct CatCompArrayType))-1, cca = (struct CatCompArrayType *)CatCompArray+cnt;
         cnt>=0;
         cnt--, cca--) if (cca->cca_ID==id) return cca->cca_Str;

    return "";
}

/***********************************************************************/

ULONG ASM
getKeyChar(REG(a0) STRPTR string)
{
    ULONG res = 0;

    if (string)
    {
        for (; *string && *string!='_'; string++);
        if (*string++) res = ToLower(*string);
    }

    return res;
}

/***********************************************************************/

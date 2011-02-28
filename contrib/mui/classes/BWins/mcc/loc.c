
#define CATCOMP_ARRAY
#include "class.h"

/****************************************************************************/

void ASM
initStrings(void)
{
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
            CONST_STRPTR s;

            if ((s = GetCatalogStr(cat,cca->cca_ID,cca->cca_Str))) cca->cca_Str = s;
        }
    }
}

/****************************************************************************/

CONST_STRPTR ASM
getString(REG(d0) ULONG id)
{
    struct CatCompArrayType    *cca;
    int                        cnt;

    for (cnt = (sizeof(CatCompArray)/sizeof(struct CatCompArrayType))-1, cca = (struct CatCompArrayType *)CatCompArray+cnt;
         cnt>=0;
         cnt--, cca--) if (cca->cca_ID==id) return cca->cca_Str;

    return "";
}

/***********************************************************************/

void ASM
localizeMenus(REG(a0) struct NewMenu *menu)
{
    while (menu->nm_Type!=NM_END)
    {
        if (menu->nm_Label!=NM_BARLABEL)
            menu->nm_Label = getString((IPTR)menu->nm_Label);

        if (menu->nm_CommKey) menu->nm_CommKey = getString((IPTR)menu->nm_CommKey);

        menu++;
    }
}

/***********************************************************************/


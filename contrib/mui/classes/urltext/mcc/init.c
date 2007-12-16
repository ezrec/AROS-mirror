
#include "class.h"

/****************************************************************************/

SAVEDS ASM IPTR
query(REG(d0) LONG which)
{
    switch (which)
    {
        case 0:
            return (IPTR)UrltextBase->mcc;

        default:
            return 0;
    }
}

/****************************************************************************/

void ASM
freeBase(REG(a0) struct UrltextBase *base)
{
    if (base->localeBase)
    {
        if (base->cat)
        {
            CloseCatalog(base->cat);
            base->cat = NULL;
        }

        CloseLibrary(base->localeBase);
        base->localeBase = NULL;
    }

    if (base->openURLBase)
    {
        CloseLibrary(base->openURLBase);
        base->openURLBase = NULL;
    }

    if (base->diskFontBase)
    {
        CloseLibrary(base->diskFontBase);
        base->diskFontBase = NULL;
    }

    if (base->iFFParseBase)
    {
        CloseLibrary(base->iFFParseBase);
        base->iFFParseBase = NULL;
    }

    if (base->muiMasterBase)
    {
        if (base->mcc)
        {
            MUI_DeleteCustomClass(base->mcc);
            base->mcc = NULL;
        }

        CloseLibrary(base->muiMasterBase);
        base->muiMasterBase = NULL;
    }

    base->flags &= ~BASEFLG_INIT;
}

/***********************************************************************/

BOOL ASM
initBase(REG(a0) struct UrltextBase *base)
{
    if ((base->muiMasterBase = OpenLibrary("muimaster.library",0)) &&
        (base->iFFParseBase = OpenLibrary("iffparse.library",0L)) &&
        (base->diskFontBase = OpenLibrary("diskfont.library",0)) &&
        initMCC(base))
    {
        base->dosBase       = (APTR)base->mcc->mcc_DOSBase;
        base->utilityBase   = base->mcc->mcc_UtilityBase;
        base->intuitionBase = (APTR)base->mcc->mcc_IntuitionBase;
        base->gfxBase       = base->mcc->mcc_GfxBase;

        base->openURLBase  = OpenLibrary("openurl.library",0);

        if ((base->localeBase = OpenLibrary("locale.library",0)))
            base->cat = OpenCatalogA(NULL,CATNAME,NULL);

        base->flags |= BASEFLG_INIT;

        return TRUE;
    }

    freeBase(base);

    return FALSE;
}

/***********************************************************************/

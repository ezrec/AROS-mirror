
#include "class.h"
#define USE_LINK_BODY
#define USE_LINK_COLORS
#include "link_iff.h"

/****************************************************************************/

SAVEDS ASM IPTR
query(REG(d0) LONG which)
{
    switch (which)
    {
        case 1:
            return (IPTR)UrltextBase->mcp;

        case 2:
            return (IPTR)BodychunkObject,
                MUIA_FixWidth,              LINK_WIDTH,
                MUIA_FixHeight,             LINK_HEIGHT,
                MUIA_Bitmap_Width,          LINK_WIDTH,
                MUIA_Bitmap_Height,         LINK_HEIGHT,
                MUIA_Bodychunk_Depth,       LINK_DEPTH,
                MUIA_Bodychunk_Body,        link_body,
                MUIA_Bodychunk_Compression, LINK_COMPRESSION,
                MUIA_Bodychunk_Masking,     LINK_MASKING,
                MUIA_Bitmap_SourceColors,   link_colors,
                MUIA_Bitmap_Transparent,    0,
            End;

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

    if (base->muiMasterBase)
    {
        if (base->mcp)
        {
            MUI_DeleteCustomClass(base->mcp);
            base->mcp = NULL;
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
        initMCP(base))
    {
        base->dosBase       = (APTR)base->mcp->mcc_DOSBase;
        base->utilityBase   = base->mcp->mcc_UtilityBase;
        base->intuitionBase = (APTR)base->mcp->mcc_IntuitionBase;
        base->gfxBase       = base->mcp->mcc_GfxBase;

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

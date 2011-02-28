
#include "class.h"
#define USE_BWIN_BODY
#define USE_BWIN_COLORS
#include "BWin_iff.h"

/***********************************************************************/

ULONG ASM SAVEDS
query(REG(d0) LONG which)
{
    switch (which)
    {
        case MUIV_Query_MCP:
            return (ULONG)lib_class;

        case MUIV_Query_MCPImage:
            return (ULONG)BodychunkObject,
                MUIA_FixWidth,              BWIN_WIDTH,
                MUIA_FixHeight,             BWIN_HEIGHT,
                MUIA_Bitmap_Width,          BWIN_WIDTH,
                MUIA_Bitmap_Height,         BWIN_HEIGHT,
                MUIA_Bodychunk_Depth,       BWIN_DEPTH,
                MUIA_Bodychunk_Body,        BWin_body,
                MUIA_Bodychunk_Compression, BWIN_COMPRESSION,
                MUIA_Bodychunk_Masking,     BWIN_MASKING,
                MUIA_Bitmap_SourceColors,   BWin_colors,
                //MUIA_Bitmap_Transparent,    0,
            End;

        default:
            return 0;
    }
}

/****************************************************************************/

void ASM
freeBase(void)
{
    if (MUIMasterBase)
    {
        if (lib_shape)
        {
            MUI_DeleteCustomClass(lib_shape);
            lib_shape = NULL;
        }

        if (lib_class)
        {
            MUI_DeleteCustomClass(lib_class);
            lib_class = NULL;
        }

        CloseLibrary(MUIMasterBase);
        MUIMasterBase = NULL;
    }

    if (LocaleBase)
    {
        if (lib_cat)
        {
            CloseCatalog(lib_cat);
            lib_cat = NULL;
        }
        CloseLibrary((struct Library *)LocaleBase);
        LocaleBase = NULL;
    }

    lib_flags &= ~BASEFLG_Init;
}

/***********************************************************************/

BOOL ASM
initBase(void)
{
    if ((MUIMasterBase = OpenLibrary("muimaster.library",19)) &&
        initShape() &&
        initMCP())
    {
        //register char buf[16];

        DOSBase       = (struct DosLibrary *)lib_class->mcc_DOSBase;
        UtilityBase   = (APTR)lib_class->mcc_UtilityBase;
        IntuitionBase = (struct IntuitionBase *)lib_class->mcc_IntuitionBase;

        initStrings();

        /*if (GetVar("BWINAVOID",buf,sizeof(buf),GVF_GLOBAL_ONLY)>0)
        {
            LONG v = 0;

            StrToLong(buf,&v);
            if (v) lib_flags |= BASEFLG_Avoid;
        }*/

        lib_flags |= BASEFLG_Init;

        return TRUE;
    }
    else
    {
        freeBase();

        return FALSE;
    }
}

/***********************************************************************/

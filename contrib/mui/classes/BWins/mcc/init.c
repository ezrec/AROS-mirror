
#include "class.h"

/****************************************************************************/

APTR ASM SAVEDS
query(REG(d0) LONG which)
{
    return ((which==MUIV_Query_MCC) ? lib_mcc : NULL);
}

/****************************************************************************/

void ASM
freeBase(void)
{
    if (lib_cat)
    {
        CloseCatalog(lib_cat);
        lib_cat = NULL;
    }

    if (LocaleBase)
    {
        CloseLibrary((struct Library *)LocaleBase);
        LocaleBase = NULL;
    }

    if (PopupMenuBase)
    {
        CloseLibrary((struct Library *)PopupMenuBase);
        PopupMenuBase = NULL;
    }

    if (lib_sgad)
    {
        FreeClass(lib_sgad);
        lib_sgad = NULL;
    }

    if (lib_boopsi)
    {
        MUI_DeleteCustomClass(lib_boopsi);
        lib_boopsi = NULL;
    }

    if (lib_contents)
    {
        MUI_DeleteCustomClass(lib_contents);
        lib_contents = NULL;
    }

    if (lib_mcc)
    {
        MUI_DeleteCustomClass(lib_mcc);
        lib_mcc = NULL;
    }

    if (MUIMasterBase)
    {
        CloseLibrary(MUIMasterBase);
        MUIMasterBase = NULL;
    }

    if (GfxBase)
    {
        CloseLibrary((struct Library *)GfxBase);
        GfxBase = NULL;
    }

    if (IntuitionBase)
    {
        CloseLibrary((struct Library *)IntuitionBase);
        IntuitionBase = NULL;
    }

    if (UtilityBase)
    {
        CloseLibrary((struct Library *)UtilityBase);
        UtilityBase = NULL;
    }

    if (DOSBase)
    {
        CloseLibrary((struct Library *)DOSBase);
        DOSBase = NULL;
    }

    lib_flags &= ~BASEFLG_Init;
}

/***********************************************************************/

BOOL ASM
initBase(void)
{
    if ((DOSBase = (APTR)OpenLibrary("dos.library",37)) &&
        (UtilityBase = (APTR)OpenLibrary("utility.library",37)) &&
        (IntuitionBase = (APTR)OpenLibrary("intuition.library",37)) &&
        (GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",37)) &&
        (MUIMasterBase = OpenLibrary("muimaster.library",19)) &&
        initSGad() && initBoopsi() && initContents() && initMCC())
    {

        initStrings();

        PopupMenuBase = (struct PopupMenuBase *)OpenLibrary("popupmenu.library",0);

        lib_flags |= BASEFLG_Init;

        return TRUE;
    }

    freeBase();

    return FALSE;
}

/***********************************************************************/

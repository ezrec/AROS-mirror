/*
    Copyright © 2002-2006, The AROS Development Team. 
    All rights reserved.
    
    $Id: miamipanel_init.c 24652 2006-08-21 10:35:08Z verhaegs $
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <aros/libcall.h>
#include <aros/symbolsets.h>

#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/muimaster.h>

#include "muimiamipanel_intern.h"

#include LC_LIBDEFS_FILE

#include <aros/debug.h>

#include "Classes/muimiamipanel_classes.h"

/****************************************************************************************/

static int InitMiamiPanel(LIBBASETYPEPTR LIBBASE)
{
    struct MiamiPanelBase_intern *MiamiPanelBaseIntern = LIBBASE;
D(bug("[MiamiPanel] Init()\n"));

    InitSemaphore(&MiamiPanelBaseIntern->mpb_libSem);
    InitSemaphore(&MiamiPanelBaseIntern->mpb_memSem);
    InitSemaphore(&MiamiPanelBaseIntern->mpb_procSem);
	
    return TRUE;
}

/****************************************************************************************/
/***********************************************************************/

void
freeMiamiPanelBase(LIBBASETYPEPTR LIBBASE)
{
    struct MiamiPanelBase_intern *MiamiPanelBaseIntern = LIBBASE;
D(bug("[MiamiPanel] freeMiamiPanelBase()\n"));

    if (MiamiPanelBaseIntern->mpb_timeTextClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_timeTextClass);
        MiamiPanelBaseIntern->mpb_timeTextClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_rateClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_rateClass);
        MiamiPanelBaseIntern->mpb_rateClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_trafficClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_trafficClass);
        MiamiPanelBaseIntern->mpb_trafficClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_lbuttonClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_lbuttonClass);
        MiamiPanelBaseIntern->mpb_lbuttonClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_ifClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_ifClass);
        MiamiPanelBaseIntern->mpb_ifClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_ifGroupClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_ifGroupClass);
        MiamiPanelBaseIntern->mpb_ifGroupClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_mgroupClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_mgroupClass);
        MiamiPanelBaseIntern->mpb_mgroupClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_aboutClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_aboutClass);
        MiamiPanelBaseIntern->mpb_aboutClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_prefsClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_prefsClass);
        MiamiPanelBaseIntern->mpb_prefsClass = NULL;
    }

    if (MiamiPanelBaseIntern->mpb_appClass)
    {
        MUI_DeleteCustomClass(MiamiPanelBaseIntern->mpb_appClass);
        MiamiPanelBaseIntern->mpb_appClass = NULL;
    }
/*
    if (MUIMasterBase)
    {
        CloseLibrary(MUIMasterBase);
        MUIMasterBase = NULL;
    }

    if (LocaleBase)
    {
        if (MiamiPanelBaseIntern->mpb_cat)
        {
            CloseCatalog(MiamiPanelBaseIntern->mpb_cat);
            lib_cat = NULL;
        }

        CloseLibrary((struct Library *)LocaleBase);
        LocaleBase = NULL;
    }

    if (IFFParseBase)
    {
        CloseLibrary((struct Library *)IFFParseBase);
        IFFParseBase = NULL;
    }

    if (IconBase)
    {
        CloseLibrary(IconBase);
        IconBase = NULL;
    }

    if (IntuitionBase)
    {
        CloseLibrary((struct Library *)IntuitionBase);
        IntuitionBase = NULL;
    }

    if (UtilityBase)
    {
        CloseLibrary(UtilityBase);
        UtilityBase = NULL;
    }

    if (GfxBase)
    {
        CloseLibrary((struct Library *)GfxBase);
        GfxBase = NULL;
    }

    if (DOSBase)
    {
        CloseLibrary((struct Library *)DOSBase);
        DOSBase = NULL;
    }
*/
    if (MiamiPanelBaseIntern->mpb_pool)
    {
        DeletePool(MiamiPanelBaseIntern->mpb_pool);
        MiamiPanelBaseIntern->mpb_pool = NULL;
    }

    MiamiPanelBaseIntern->mpb_flags &= ~BASEFLG_Init;
}

ULONG
initMiamiPanelBase(LIBBASETYPEPTR LIBBASE)
{
    struct MiamiPanelBase_intern *MiamiPanelBaseIntern = LIBBASE;
D(bug("[MiamiPanel] initMiamiPanelBase()\n"));

    if ((MiamiPanelBaseIntern->mpb_pool = CreatePool(MEMF_ANY|MEMF_CLEAR, 256, 64))
/*		&& (DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",37))
		&& (GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",39))
		&& (UtilityBase = OpenLibrary("utility.library",37))
		&& (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37))
		&& (MUIMasterBase = OpenLibrary("muimaster.library",19))
		&& (IconBase = OpenLibrary("icon.library",37))
		&& (IFFParseBase = OpenLibrary("iffparse.library",37))
		&& (LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library",0))*/
		)
    {
        NEWLIST(&MiamiPanelBaseIntern->mpb_msgList);
//        initStrings();

        if (MUIPC_App_ClassInit(MiamiPanelBaseIntern)
            && MUIPC_MGroup_ClassInit(MiamiPanelBaseIntern)
            && MUIPC_IfGroup_ClassInit(MiamiPanelBaseIntern)
            && MUIPC_If_ClassInit(MiamiPanelBaseIntern)
            && MUIPC_LButton_ClassInit(MiamiPanelBaseIntern)
            && MUIPC_Traffic_ClassInit(MiamiPanelBaseIntern)
            && MUIPC_Rate_ClassInit(MiamiPanelBaseIntern)
            && MUIPC_TimeText_ClassInit(MiamiPanelBaseIntern)
			)
        {
            MiamiPanelBaseIntern->mpb_flags |= BASEFLG_Init;

            return TRUE;
        }
    }

/*    freeMiamiPanelBase();*/

    return FALSE;
}

static int OpenMiamiPanel(LIBBASETYPEPTR LIBBASE)
{
    struct MiamiPanelBase_intern *MiamiPanelBaseIntern = LIBBASE;
D(bug("[MiamiPanel] Open()\n"));

    ObtainSemaphore(&MiamiPanelBaseIntern->mpb_libSem);

    LIBBASE->lib_OpenCnt++;
    LIBBASE->lib_Flags &= ~LIBF_DELEXP;

    if (!(MiamiPanelBaseIntern->mpb_flags & BASEFLG_Init) && !initMiamiPanelBase(LIBBASE))
    {
        LIBBASE->lib_OpenCnt--;
    }

    ReleaseSemaphore(&MiamiPanelBaseIntern->mpb_libSem);
	
    return TRUE;
}

/****************************************************************************************/

static int ExpungeMiamiPanel(LIBBASETYPEPTR LIBBASE)
{
    struct MiamiPanelBase_intern *MiamiPanelBaseIntern = LIBBASE;
D(bug("[MiamiPanel] Expunge()\n"));

    BOOL          success;

    ObtainSemaphore(&MiamiPanelBaseIntern->mpb_libSem);

    if (!(LIBBASE->lib_OpenCnt || MiamiPanelBaseIntern->mpb_use))
    {
        Remove((struct Node *)LIBBASE);
        FreeMem((UBYTE *)LIBBASE-LIBBASE->lib_NegSize, LIBBASE->lib_NegSize + LIBBASE->lib_PosSize);

        success = TRUE;
    }
    else
    {
        LIBBASE->lib_Flags |= LIBF_DELEXP;
        success = FALSE;
    }

    ReleaseSemaphore(&MiamiPanelBaseIntern->mpb_libSem);
	
    return success;
}

ADD2INITLIB(InitMiamiPanel, 0);
ADD2OPENLIB(OpenMiamiPanel, 0);
ADD2EXPUNGELIB(ExpungeMiamiPanel, 0);

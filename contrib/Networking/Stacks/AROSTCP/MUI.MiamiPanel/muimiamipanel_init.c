/*
    Copyright © 2002-2006, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <aros/libcall.h>
#include <aros/symbolsets.h>

#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/dos.h>

#include LC_LIBDEFS_FILE

#include "muimiamipanel_intern.h"
#include "muimiamipanel_locale.h"
#include "Classes/muimiamipanel_classes.h"

#define MiamiPanelBaseIntern LIBBASE

/****************************************************************************************/

static int InitMiamiPanel(LIBBASETYPEPTR LIBBASE)
{
D(bug("[MiamiPanel] Init(%x)\n", LIBBASE));

    InitSemaphore(&LIBBASE->mpb_libSem);
    InitSemaphore(&LIBBASE->mpb_memSem);
    InitSemaphore(&LIBBASE->mpb_procSem);

D(bug("[MiamiPanel] Init: Initialised Semaphores ..\n"));
D(bug("[MiamiPanel] Init:                - lib sem @ %x\n", &LIBBASE->mpb_libSem));
D(bug("[MiamiPanel] Init:                - mem sem @ %x\n", &LIBBASE->mpb_memSem));
D(bug("[MiamiPanel] Init:                - proc sem @ %x\n", &LIBBASE->mpb_procSem));
    return TRUE;
}

/****************************************************************************************/
/***********************************************************************/

void
freeMiamiPanelBase(LIBBASETYPEPTR LIBBASE)
{
D(bug("[MiamiPanel] freeMiamiPanelBase(%x)\n", LIBBASE));

    if (LIBBASE->mpb_timeTextClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_timeTextClass);
        LIBBASE->mpb_timeTextClass = NULL;
    }

    if (LIBBASE->mpb_rateClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_rateClass);
        LIBBASE->mpb_rateClass = NULL;
    }

    if (LIBBASE->mpb_trafficClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_trafficClass);
        LIBBASE->mpb_trafficClass = NULL;
    }

    if (LIBBASE->mpb_lbuttonClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_lbuttonClass);
        LIBBASE->mpb_lbuttonClass = NULL;
    }

    if (LIBBASE->mpb_ifClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_ifClass);
        LIBBASE->mpb_ifClass = NULL;
    }

    if (LIBBASE->mpb_ifGroupClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_ifGroupClass);
        LIBBASE->mpb_ifGroupClass = NULL;
    }

    if (LIBBASE->mpb_mgroupClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_mgroupClass);
        LIBBASE->mpb_mgroupClass = NULL;
    }

    if (LIBBASE->mpb_aboutClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_aboutClass);
        LIBBASE->mpb_aboutClass = NULL;
    }

    if (LIBBASE->mpb_prefsClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_prefsClass);
        LIBBASE->mpb_prefsClass = NULL;
    }

    if (LIBBASE->mpb_appClass)
    {
        MUI_DeleteCustomClass(LIBBASE->mpb_appClass);
        LIBBASE->mpb_appClass = NULL;
    }

    if (LIBBASE->mpb_MUIMasterBase)
    {
        CloseLibrary(LIBBASE->mpb_MUIMasterBase);
        LIBBASE->mpb_MUIMasterBase = NULL;
    }

    if (LIBBASE->mpb_LocaleBase)
    {
		Locale_Deinitialize(LIBBASE);

        CloseLibrary((struct Library *)LIBBASE->mpb_LocaleBase);
        LIBBASE->mpb_LocaleBase = NULL;
    }

    if (LIBBASE->mpb_IFFParseBase)
    {
        CloseLibrary((struct Library *)LIBBASE->mpb_IFFParseBase);
        LIBBASE->mpb_IFFParseBase = NULL;
    }

    if (LIBBASE->mpb_IconBase)
    {
        CloseLibrary(LIBBASE->mpb_IconBase);
        LIBBASE->mpb_IconBase = NULL;
    }

    if (LIBBASE->mpb_IntuitionBase)
    {
        CloseLibrary((struct Library *)LIBBASE->mpb_IntuitionBase);
        LIBBASE->mpb_IntuitionBase = NULL;
    }

    if (LIBBASE->mpb_UtilityBase)
    {
        CloseLibrary(LIBBASE->mpb_UtilityBase);
        LIBBASE->mpb_UtilityBase = NULL;
    }

    if (LIBBASE->mpb_GfxBase)
    {
        CloseLibrary((struct Library *)LIBBASE->mpb_GfxBase);
        LIBBASE->mpb_GfxBase = NULL;
    }

    if (LIBBASE->mpb_DOSBase)
    {
        CloseLibrary((struct Library *)LIBBASE->mpb_DOSBase);
        LIBBASE->mpb_DOSBase = NULL;
    }

    if (LIBBASE->mpb_pool)
    {
        DeletePool(LIBBASE->mpb_pool);
        LIBBASE->mpb_pool = NULL;
    }

    LIBBASE->mpb_flags &= ~BASEFLG_Init;
}

ULONG
initMiamiPanelBase(LIBBASETYPEPTR LIBBASE)
{
D(bug("[MiamiPanel] initMiamiPanelBase(%x)\n", LIBBASE));

    if ((LIBBASE->mpb_pool = CreatePool(MEMF_ANY|MEMF_CLEAR, 256, 64))
		&& (LIBBASE->mpb_DOSBase = OpenLibrary("dos.library", 0))
		&& (LIBBASE->mpb_GfxBase = OpenLibrary("graphics.library", 0))
		&& (LIBBASE->mpb_UtilityBase = OpenLibrary("utility.library", 0))
		&& (LIBBASE->mpb_IntuitionBase = OpenLibrary("intuition.library", 0))
		&& (LIBBASE->mpb_MUIMasterBase = OpenLibrary("muimaster.library", 0))
		&& (LIBBASE->mpb_IconBase = OpenLibrary("icon.library", 0))
		&& (LIBBASE->mpb_IFFParseBase = OpenLibrary("iffparse.library", 0))
		&& (LIBBASE->mpb_LocaleBase = OpenLibrary("locale.library", 0))
		)
    {
D(bug("[MiamiPanel] initMiamiPanelBase: mem pool allocated @ %x\n", LIBBASE->mpb_pool));
        NEWLIST(&LIBBASE->mpb_msgList);
		Locale_Initialize(LIBBASE);

        if (MUIPC_App_ClassInit(LIBBASE)
            && MUIPC_MGroup_ClassInit(LIBBASE)
            && MUIPC_IfGroup_ClassInit(LIBBASE)
            && MUIPC_If_ClassInit(LIBBASE)
            && MUIPC_LButton_ClassInit(LIBBASE)
            && MUIPC_Traffic_ClassInit(LIBBASE)
            && MUIPC_Rate_ClassInit(LIBBASE)
            && MUIPC_TimeText_ClassInit(LIBBASE)
			)
        {
D(bug("[MiamiPanel] initMiamiPanelBase: Classes Initialised ..\n"));
            LIBBASE->mpb_flags |= BASEFLG_Init;

            return TRUE;
        }
    }

D(bug("[MiamiPanel] initMiamiPanelBase: Initialisation Failed!\n"));
	
    freeMiamiPanelBase(LIBBASE);

    return FALSE;
}

static int OpenMiamiPanel(LIBBASETYPEPTR LIBBASE)
{
D(bug("[MiamiPanel] Open(%x)\n", LIBBASE));

    ObtainSemaphore(&LIBBASE->mpb_libSem);

    LIBBASE->mpb_library.lib_OpenCnt++;
    LIBBASE->mpb_library.lib_Flags &= ~LIBF_DELEXP;

    if (!(LIBBASE->mpb_flags & BASEFLG_Init) && !initMiamiPanelBase(LIBBASE))
    {
        LIBBASE->mpb_library.lib_OpenCnt--;
    }

    ReleaseSemaphore(&LIBBASE->mpb_libSem);
	
    return TRUE;
}

/****************************************************************************************/

static int ExpungeMiamiPanel(LIBBASETYPEPTR LIBBASE)
{
D(bug("[MiamiPanel] Expunge(%x)\n", LIBBASE));

    BOOL          success;

    ObtainSemaphore(&LIBBASE->mpb_libSem);

    if (!(LIBBASE->mpb_library.lib_OpenCnt || LIBBASE->mpb_use))
    {
//        Remove((struct Node *)LIBBASE);
//        FreeMem((UBYTE *)LIBBASE-LIBBASE->mpb_library.lib_NegSize, LIBBASE->mpb_library.lib_NegSize + LIBBASE->mpb_library.lib_PosSize);

        success = TRUE;
    }
    else
    {
        LIBBASE->mpb_library.lib_Flags |= LIBF_DELEXP;
        success = FALSE;
    }

    ReleaseSemaphore(&LIBBASE->mpb_libSem);
	
    return success;
}

ADD2INITLIB(InitMiamiPanel, 0);
ADD2OPENLIB(OpenMiamiPanel, 0);
ADD2EXPUNGELIB(ExpungeMiamiPanel, 0);

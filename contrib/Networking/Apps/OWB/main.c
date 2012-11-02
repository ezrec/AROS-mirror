/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>
#include <dos/var.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/alib.h>
#include <aros/debug.h>
#include <libraries/codesets.h>
#include <proto/codesets.h>

#include <WebViewZune.h>

#include "locale.h"
#include "browserapp.h"
#include "browserwindow.h"

int main(void)
{
    IPTR argArray[] = { 0 };
    struct RDArgs *args = NULL;
    char *url = NULL;
    Object *wnd, *app;
    
    if((args = ReadArgs("URL", argArray, NULL)) != NULL)
    {
	if(argArray[0])
	{
	    url = StrDup((const char*) argArray[0]);
	    if(!url)
	    {
		FreeArgs(args);
		return 1;
	    }
	}
	FreeArgs(args);
    }

    Locale_Initialize();
    
    SetVar("FONTCONFIG_PATH", "PROGDIR:fonts/config", -1, LV_VAR | GVF_LOCAL_ONLY);
    SetVar("DISABLE_NI_WARNING", "1", -1, LV_VAR | GVF_LOCAL_ONLY);
    
    // GUI creation
    app = NewObject(BrowserApp_CLASS->mcc_Class, NULL,
        SubWindow, wnd = NewObject(BrowserWindow_CLASS->mcc_Class, NULL,
	    MUIA_Window_Width, MUIV_Window_AltWidth_Screen(100),
	    MUIA_Window_Height, MUIV_Window_AltHeight_Screen(100),
	    MUIA_Window_AltWidth, MUIV_Window_AltWidth_Screen(75),
	    MUIA_Window_AltHeight, MUIV_Window_AltHeight_Screen(75),
	    MUIA_Window_ID, (IPTR) 1,
            TAG_END),
	TAG_END);

    if (app != NULL)
    {
        ULONG sigs = 0;

        /* Open the window */
        set(wnd, MUIA_Window_Open, TRUE);
       
        if(url)
            DoMethod(wnd, MUIM_BrowserWindow_OpenURLInActiveTab, url);
        
	/* Main loop */
	while((LONG)DoMethod(app, MUIM_Application_NewInput, (IPTR)&sigs)
	      != MUIV_Application_ReturnID_Quit)
	{
	    if (sigs)
	    {
		sigs = Wait(sigs | SIGBREAKF_CTRL_C);
		if (sigs & SIGBREAKF_CTRL_C)
		    break;
	    }
	}
        /* Destroy our application and all its objects */
        MUI_DisposeObject(app);
    }

    Locale_Deinitialize();
    
    if(url)
	FreeVec(url);
    return 0;
}

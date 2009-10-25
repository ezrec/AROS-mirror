/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>
#include <dos/var.h>
#include <exec/libraries.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/alib.h>
#include <aros/debug.h>

#include <fontconfig/fontconfig.h>

#include "locale.h"
#include "version.h"

#define OWB_STACK_SIZE 2097152

int main(void)
{
    IPTR argArray[] = { 0 };
    struct RDArgs *args = NULL;
    const char *url = NULL;
    Object *wnd, *app, *status;
    
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
    
    // GUI creation
    app = ApplicationObject,
        SubWindow, wnd = WindowObject,
	    MUIA_Window_Width, MUIV_Window_AltWidth_Screen(25),
	    MUIA_Window_Height, MUIV_Window_AltHeight_Screen(25),
	    MUIA_Window_Borderless, TRUE,
	    WindowContents, VGroup,
		MUIA_InnerTop, 10,
		MUIA_InnerBottom, 10,
		MUIA_InnerLeft, 10,
		MUIA_InnerRight, 10,
	        Child, ImageObject,
		    MUIA_Image_Spec, "3:PROGDIR:resources/logo.png",
		    End,
		Child, TextObject,
		    MUIA_Text_Contents, "\33b\33cOrigyn Web Browser for AROS\33n\n\33cVersion: " OWB_VERSION " (" OWB_RELEASE_DATE ")\n",
		    End,
		Child, status = TextObject,
		    MUIA_Text_PreParse, "\33c",
		    MUIA_Text_Contents, "Origyn Web Browser for AROS is starting...\n",
		    End,
	        End,
            End,
	End;

    /* Click Close gadget or hit Escape to quit */
    DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	    app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    if (app != NULL)
    {
        ULONG sigs = 0;

        /* Open the window */
        set(wnd, MUIA_Window_Open, TRUE);
       
        set(status, MUIA_Text_Contents, "Checking if TCP/IP stack is running...");
        struct Library *socketBase = OpenLibrary("bsdsocket.library", 4);
        if(socketBase)
        {
            CloseLibrary(socketBase);
        }
        else
        {
            set(status, MUIA_Text_Contents, "Error: \33bTCP/IP stack is not running!\33n");
            Delay(100);
            goto error;
        }
        
        set(status, MUIA_Text_Contents, "Generating font cache...");
        if(FcInit())
        {
            FcFini();
        }
        else
        {
            set(status, MUIA_Text_Contents, "Error: \33bCouldn't generate font cache!\33n");
            Delay(100);
            goto error;
        }
            
        set(status, MUIA_Text_Contents, "Loading...");
        BPTR owbSeg = LoadSeg("PROGDIR:owb");
        if(owbSeg)
        {
            set(status, MUIA_Text_Contents, "Executing...");
            set(wnd, MUIA_Window_Open, FALSE);
            
            SetIoErr(0);
            struct CommandLineInterface *cli = Cli();
            ULONG stackSize = (cli ? cli->cli_DefaultStack * CLI_DEFAULTSTACK_UNIT : OWB_STACK_SIZE);
	    LONG returncode = RunCommand(owbSeg, (stackSize > OWB_STACK_SIZE ? stackSize : OWB_STACK_SIZE), (url ? url : "\n"), 0);
	    
	    UnLoadSeg(owbSeg);
	    
	    if(returncode == -1)
	    {
		set(wnd, MUIA_Window_Open, TRUE);
		set(status, MUIA_Text_Contents, "Error: \33bCouldn't execute Origyn Web Browser\33n");
		Delay(100);
		goto error;		
	    }

	    MUI_DisposeObject(app);
	    
	    return returncode;
        }
        else
        {
            set(status, MUIA_Text_Contents, "Error: \33bCouldn't load Origyn Web Browser\33n");
            Delay(100);
            goto error;
        }
error:
        set(wnd, MUIA_Window_Open, FALSE);

        /* Destroy our application and all its objects */
        MUI_DisposeObject(app);
    }

    Locale_Deinitialize();
    
    if(url)
	FreeVec(url);
    return 0;
}

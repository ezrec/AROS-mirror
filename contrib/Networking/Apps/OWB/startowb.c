/*
    Copyright © 2009-2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>
#include <dos/var.h>
#include <exec/libraries.h>
#include <workbench/startup.h>

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

int main(int argc, char** argv)
{
    IPTR argArray[] = { 0 };
    struct RDArgs *args = NULL;
    char *url = NULL;
    Object *wnd, *app, *status;

    if (argc == 0)
    {
        struct WBStartup *startup = (struct WBStartup *) argv;

        if (startup->sm_NumArgs >= 2)
        {
            UBYTE buffer[1024] = "file:///";

            if(NameFromLock(startup->sm_ArgList[1].wa_Lock, &buffer[8], sizeof(buffer) - 8))
            {
                AddPart(&buffer[8], startup->sm_ArgList[1].wa_Name, sizeof(buffer) - 8);
                url = StrDup(buffer);
            }
        }
    }

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

            struct TagItem tags[] =
            {
                { NP_Seglist, (IPTR) owbSeg },
                { NP_StackSize, stackSize > OWB_STACK_SIZE ? stackSize : OWB_STACK_SIZE },
                { NP_Name, (IPTR) "Origyn Web Browser" },
                { url ? NP_Arguments : TAG_IGNORE, (IPTR) url },
                { NP_Cli, TRUE },
                { NP_FreeSeglist, TRUE },
                { NP_CommandName, (IPTR) "owb" },
                { TAG_DONE, 0 }
            };

            struct Process *owbProc = CreateNewProc(tags);

            if(!owbProc)
            {
                UnLoadSeg(owbSeg);
                set(wnd, MUIA_Window_Open, TRUE);
                set(status, MUIA_Text_Contents, "Error: \33bCouldn't execute Origyn Web Browser\33n");
                Delay(100);
                goto error;
            }

            MUI_DisposeObject(app);

            return 0;
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

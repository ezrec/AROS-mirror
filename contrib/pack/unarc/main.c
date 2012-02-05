/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/alib.h>

#include <libraries/mui.h>
#include <workbench/startup.h>

#include "unarcwindow_class.h"
#include "locale.h"

#define ARGTEMPLATE "ARCHIVE,DESTINATION"


enum
{
    ARG_ARCHIVE,
    ARG_DESTINATION,
    ARG_COUNT
};

static void cleanup_exit(CONST_STRPTR str);

static Object *app, *win;
static struct DiskObject *dobj;
static struct RDArgs *rda;
static IPTR args[ARG_COUNT];


int main(int argc, char **argv)
{
    Locale_Initialize();

    STRPTR archive = NULL;
    STRPTR destination = NULL;

    if (argc) // started from CLI
    {
        rda = ReadArgs(ARGTEMPLATE, args, NULL);
        if (!rda)
        {
            cleanup_exit(_(MSG_ERR_READARGS));
        }
        archive = (STRPTR)args[ARG_ARCHIVE];
        destination = (STRPTR)args[ARG_DESTINATION];
    }
    else // started from Wanderer
    {
        struct WBStartup *wbmsg = (struct WBStartup *)argv;
        struct WBArg *wbarg = wbmsg->sm_ArgList;
        STRPTR *toolarray;

        dobj = GetDiskObject(wbarg->wa_Name);
        if (dobj)
        {
            toolarray = dobj->do_ToolTypes;

            archive = FindToolType(toolarray, "ARCHIVE");
            destination = FindToolType(toolarray, "DESTINATION");
        }
    }

    app = ApplicationObject,
        MUIA_Application_Author, (IPTR)"The AROS Development Team",
        MUIA_Application_Base, (IPTR)"UNARC",
        MUIA_Application_Title, (IPTR)"Unarc",
        MUIA_Application_Version, (IPTR)"$VER: Unarc 1.0 (04.02.2012)",
        MUIA_Application_Copyright, __(MSG_AppCopyright),
        MUIA_Application_Description, __(MSG_AppDescription),
        SubWindow, (IPTR)(win = UnarcWindowObject,
            MUIA_UnarcWindow_Archive, (IPTR)archive,
            MUIA_UnarcWindow_Destination, (IPTR)destination,
        End),
    End;

    if (!app)
        cleanup_exit(_(MSG_ERR_NO_APPLICATION));

    SET(win, MUIA_Window_Open, TRUE);
    DoMethod(app, MUIM_Application_Execute);
    cleanup_exit(NULL);
    return 0;
}


static void cleanup_exit(CONST_STRPTR str)
{
    if (str)
    {
        struct EasyStruct es =
        {
            sizeof(struct EasyStruct), 0,
            _(MSG_ERR), str, _(MSG_OK)
        };

        EasyRequestArgs(NULL, &es, NULL, NULL);
    }
    MUI_DisposeObject(app);
    if (dobj) FreeDiskObject(dobj);
    if (rda) FreeArgs(rda);
    Locale_Deinitialize();
}

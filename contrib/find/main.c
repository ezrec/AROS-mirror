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

#include "findgroup_class.h"
#include "locale.h"


static void cleanup_exit(CONST_STRPTR str);

static Object *app, *win;
static struct DiskObject *dobj;


int main(int argc, char **argv)
{
    dobj = GetDiskObject("PROGDIR:Find");

    app = ApplicationObject,
        MUIA_Application_Author, (IPTR)"The AROS Development Team",
        MUIA_Application_Base, (IPTR)"FIND",
        MUIA_Application_Title, (IPTR)"Find",
        MUIA_Application_Version, (IPTR)"$VER: Find 1.0 (08.04.2012)",
        MUIA_Application_Copyright, __(MSG_AppCopyright),
        MUIA_Application_Description, __(MSG_AppDescription),
        MUIA_Application_DiskObject, (IPTR)dobj,
        SubWindow, (IPTR)(win = WindowObject,
            MUIA_Window_Title, __(MSG_WI_TITLE),
            MUIA_Window_ID, MAKE_ID('F', 'I', 'W', 'I'),
            WindowContents, (IPTR)(FindGroupObject,
            End),
        End),
    End;

    if (!app)
        cleanup_exit(_(MSG_ERR_NO_APPLICATION));

    DoMethod
    (
        win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
    );

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
}

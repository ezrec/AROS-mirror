/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include <libraries/mui.h>

#include "unarcwindow_class.h"
#include "locale.h"

#define ARGTEMPLATE "ARCHIVE,DESTINATION"

Object *app, *win;

int main(int argc, char **argv)
{
    Locale_Initialize();

    if (argc)
    {
        // ReadArgs
    }

    app = ApplicationObject,
        MUIA_Application_Author, "The AROS Development Team",
        MUIA_Application_Base, "UNARC",
        MUIA_Application_Title, "Unarc",
        MUIA_Application_Version, "$VER: Unarc 1.0 (04.02.2012)",
        MUIA_Application_Copyright, _(MSG_AppCopyright),
        MUIA_Application_Description, _(MSG_AppDescription),
        SubWindow, win = UnarcWindowObject,
        End,
    End;

    if (!app)
        Cleanup();

    SET(win, MUIA_Window_Open, TRUE);
    DoMethod(app, MUIM_Application_Execute);
    Cleanup();
    return 0;
}

void Cleanup()
{
    MUI_DisposeObject(app);
    Locale_Deinitialize();
}

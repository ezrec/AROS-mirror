/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <exec/types.h>
#include <libraries/mui.h>

#include <proto/muimaster.h>
#include <proto/dos.h>

#include "configurationeditor.h"
#include "locale.h"
#include "strings.h"

int main(void)
{
    Object *application, *window;
    application = ApplicationObject,
        MUIA_Application_Title      , "UAE Config",
        MUIA_Application_Version    , "$VER: UAE-Config 1.0 (14.10.2007)",
        MUIA_Application_Copyright  , "©2007 The AROS Dev Team",
        MUIA_Application_Author     , "The AROS Dev Team",
        MUIA_Application_Description, "UAE Configuration File Editor",
        MUIA_Application_Base       , "UAECONFIG",

        SubWindow, window = WindowObject,
            MUIA_Window_Title    , "UAE Config",
            MUIA_Window_ID       , MAKE_ID('U','A','E','C'),

            WindowContents, VGroup,
                Child, ConfigurationEditorObject,
                End,
            End,
        End,
    End;
    
    if (!application)
    {
        PutStr(_(MSG_ERROR_CLASSES));
        return RETURN_ERROR;
    }
    DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                 (IPTR)application, 2,
                 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    set(window, MUIA_Window_Open, TRUE);
    DoMethod(application, MUIM_Application_Execute, TRUE);
    MUI_DisposeObject(application);
    return RETURN_OK;
}


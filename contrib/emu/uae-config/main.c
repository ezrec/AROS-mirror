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
	SubWindow, window = WindowObject,
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

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>

#include <aros/autoinit.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <cybergraphx/cybergraphics.h>
#include <utility/tagitem.h>

#include <stdlib.h>

#include "sysconfig.h"
#include "sysdeps.h"

#include "config.h"
#include "options.h"
#include "uae.h"
#include "gui.h"

#include "aros-gui.h"

#include <aros/debug.h>

Object *uaedisplay = 0;
static Object *application;
static ULONG signals;

int gui_init (void)
{
    Object *window = NULL;

    application = EmulatorObject, End;
    if (!application)
    {
        __showerror("Could not create the application.");
        return -2;
    }

    get(application, MUIA_Emulator_Window, &window);
    get(window, MUIA_EmulatorWindow_DisplayObject, &uaedisplay);
    bug("uaedisplay = %p - &uaedisplay = %p\n", uaedisplay, &uaedisplay);

    /* Just in case abort() is invoked, in which case we have to make
       sure that the application is disposed.  */
    atexit(gui_exit);

    set(window, MUIA_Window_Open, TRUE);

    Delay(100);

    if
    (
        DoMethod(application, MUIM_Application_NewInput, (IPTR) &signals) ==
	(IPTR)MUIV_Application_ReturnID_Quit
    )
    {
        uae_quit();
    }

    SetTaskPri(FindTask(NULL), -5);

    return 0;
}

int gui_update (void)
{
    return 0;
}

void gui_exit (void)
{
    if (application)
    {
        MUI_DisposeObject(application);
	application = NULL;
    }
}

void gui_fps (int x)
{
}

void gui_led (int led, int on)
{
}

void gui_filename (int num, const char *name)
{
}

void gui_handle_events (void)
{
    if (signals)
    {
	ULONG signals_got = SetSignal(0,0);

	signals = signals_got & (signals | SIGBREAKF_CTRL_C);

	if
	(
	    signals
	  &&(
	        (signals & SIGBREAKF_CTRL_C)
              ||(DoMethod(application, MUIM_Application_NewInput, (IPTR) &signals) ==
                 (IPTR)MUIV_Application_ReturnID_Quit)
	    )
	)
	{
            uae_quit();
	}
    }
}

void gui_changesettings (void)
{
}

void gui_update_gfx (void)
{
}

void gui_lock (void)
{
}

void gui_unlock (void)
{
}

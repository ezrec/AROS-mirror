#include "eglutint.h"

#include <proto/intuition.h>
#include <proto/exec.h>

void
_eglutNativeInitDisplay(void)
{
    _eglut->native_dpy = EGL_DEFAULT_DISPLAY;
    _eglut->surface_type = EGL_WINDOW_BIT;
}

void
_eglutNativeFiniDisplay(void)
{
}

void
_eglutNativeInitWindow(struct eglut_window *win, const char *title,
                       int x, int y, int w, int h)
{
    struct Screen * pubscreen = NULL;
    struct Window * wind = NULL;

    if ((pubscreen = LockPubScreen(NULL)) == NULL)
        _eglutFatal("Failed to lock public screen");

    wind = OpenWindowTags(0,
                        WA_Title, (IPTR)title,
                        WA_PubScreen, pubscreen,
                        WA_CloseGadget, TRUE,
                        WA_DragBar, TRUE,
                        WA_DepthGadget, TRUE,
                        WA_Left, x,
                        WA_Top, y,
                        WA_InnerWidth, w,
                        WA_InnerHeight, h,
                        WA_Activate, TRUE,
                        WA_RMBTrap, TRUE,
                        WA_SimpleRefresh, TRUE,
                        WA_NoCareRefresh, TRUE,
                        WA_IDCMP, IDCMP_VANILLAKEY | IDCMP_CLOSEWINDOW,
                        TAG_DONE);
    UnlockPubScreen(NULL, pubscreen);

    if (!wind)
       _eglutFatal("Failed to open window"); 

    win->native.u.window = wind;
    win->native.width = w;
    win->native.height = h;
}

void
_eglutNativeFiniWindow(struct eglut_window *win)
{
    CloseWindow(win->native.u.window);
}

void
_eglutNativeEventLoop(void)
{
    BOOL finished = FALSE;
    _eglut->redisplay = 1;

    while (!finished) {
        struct eglut_window *win = _eglut->current;
        struct IntuiMessage *msg;

        while((msg = (struct IntuiMessage *)GetMsg(win->native.u.window->UserPort)))
        {
            switch(msg->Class)
            {
            case IDCMP_CLOSEWINDOW:
                finished = TRUE;
                break;
            case IDCMP_VANILLAKEY:
                if (msg->Code == 27 /* ESC */) finished = TRUE;
                break;
            }
            ReplyMsg((struct Message *)msg);
        }

        if (_eglut->idle_cb)
            _eglut->idle_cb();

        if (_eglut->redisplay) 
        {
            _eglut->redisplay = 0;

            if (win->display_cb)
                win->display_cb();
            eglSwapBuffers(_eglut->dpy, win->surface);
        }
        
        win->keyboard_cb(finished ? 27 : 0);
    }
    
    
}

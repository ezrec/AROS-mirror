#include "Private.h"

/*** Methods ***************************************************************/

///Window_Open

/*  Cette  Méthode  est   appelée   lorsque   l'application   se   réveille
(FM_Application_Run   ou   FM_Application_Awake),  lorsque  le  système  de
préférence a été modifié, ou encore parce que l'attribut FA_Window_Open est
devenu TRUE.

La méthode renvoie un pointeur vers la fenêtre qui a été ouverte,  ou  NULL
si l'ouverture de la fenêtre est impossible pour une raison quelconque. */

F_METHOD(struct Window *,Window_Open)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct Screen *scr;

/* Check if window is already opened */

    if (LOD->Win)
    {
        WindowToFront(LOD->Win);
        ActivateWindow(LOD->Win); return LOD->Win;
    }

    if (!F_Do(Obj,FM_Window_Setup))
    {
        return NULL;
    }

    scr = (struct Screen *) F_Get(LOD->Render->Display,(uint32) "FA_Display_Screen");

    if (!scr)
    {
        F_Log(FV_LOG_USER,"Unable to obtain Screen of %s{%08lx}",_classname(LOD->Render->Display),LOD->Render->Display);

        goto __error;
    }

    window_minmax(Class,Obj);

    if (LOD->MinMax.MinW > scr->Width || LOD->MinMax.MinH > scr->Height)
    {
        F_Log(FV_LOG_USER,"Window (%ld x %ld) to big for Screen (%ld x %ld)",LOD->Box.w,LOD->Box.h,scr->Width,scr->Height);

        goto __error;
    }

/* Zoom Positions */

    LOD->Zoom.y = scr->BarHeight + 1;
    LOD->Zoom.h = scr->Height - scr->BarHeight - 1;
    LOD->Zoom.x = 0;
    LOD->Zoom.w = scr->Width;

    if (_root_area_public)
    {
        if (_root_minh == _root_maxh)
        {
            LOD->Zoom.y = LOD->Box.y;
            LOD->Zoom.h = LOD->Box.h;
        }
        if (_root_minw == _root_maxw)
        {
            LOD->Zoom.x = LOD->Box.x;
            LOD->Zoom.w = LOD->Box.w;
        }
    }

    F_Do(Obj,FM_Window_Layout);

//    F_Log(0,"OPEN %ld : %ld, %ld x %ld",LOD->Box.x,LOD->Box.y,LOD->Box.w,LOD->Box.h);

    /* There is two way of opening a window. The first one  is  for  windows
    with a decorator. The second is for standard (intuition) windows. */

    if (LOD->Decorator)
    {
        LOD->Win =

            OpenWindowTags(NULL,

            WA_Left,            LOD->Box.x,
            WA_Top,             LOD->Box.y,
            WA_Width,           LOD->Box.w,
            WA_Height,          LOD->Box.h,
            WA_MinWidth,        LOD->MinMax.MinW,
            WA_MinHeight,       LOD->MinMax.MinH,
            WA_MaxWidth,        LOD->MinMax.MaxW,
            WA_MaxHeight,       LOD->MinMax.MaxH,

            WA_NotifyDepth,     TRUE,
            WA_AutoAdjust,      TRUE,
            WA_Flags,           WFLG_SIZEBBOTTOM | WFLG_REPORTMOUSE | WFLG_RMBTRAP,
            WA_BackFill,        LAYERS_NOBACKFILL,
            WA_CustomScreen,    scr,

            WA_Activate,        (0 != (LOD->WINFlags & FF_WINDOW_WIN_ACTIVE)),
            WA_Backdrop,        (0 != (LOD->WINFlags & FF_WINDOW_WIN_BACKDROP)),
            WA_Borderless,      TRUE,
            WA_SimpleRefresh,   (0 != (LOD->SYSFlags & FF_WINDOW_SYS_REFRESH_SIMPLE)),

            TAG_DONE);
    }
    else
    {
        LOD->Win =

            OpenWindowTags(NULL,

            WA_Left,            LOD->Box.x,
            WA_Top,             LOD->Box.y,
            WA_Width,           LOD->Box.w,
            WA_Height,          LOD->Box.h,
            WA_MinWidth,        LOD->MinMax.MinW,
            WA_MinHeight,       LOD->MinMax.MinH,
            WA_MaxWidth,        LOD->MinMax.MaxW,
            WA_MaxHeight,       LOD->MinMax.MaxH,

            WA_NotifyDepth,     TRUE,
            WA_AutoAdjust,      TRUE,
            WA_Flags,           WFLG_SIZEBBOTTOM | WFLG_REPORTMOUSE | WFLG_RMBTRAP,
            WA_BackFill,        LAYERS_NOBACKFILL,
            WA_CustomScreen,    scr,

            WA_Activate,        (0 != (LOD->WINFlags & FF_WINDOW_WIN_ACTIVE)),
            WA_Backdrop,        (0 != (LOD->WINFlags & FF_WINDOW_WIN_BACKDROP)),
            WA_Borderless,      (0 != (LOD->WINFlags & FF_WINDOW_WIN_BORDERLESS)),
            WA_SizeGadget,      (0 != (LOD->WINFlags & FF_WINDOW_WIN_RESIZABLE)),

            WA_DragBar,         (0 != (LOD->GADFlags & FF_WINDOW_GAD_DRAG)),
            WA_DepthGadget,     (0 != (LOD->GADFlags & FF_WINDOW_GAD_DEPTH)),
            WA_CloseGadget,     (0 != (LOD->GADFlags & FF_WINDOW_GAD_CLOSE)),
            WA_SimpleRefresh,   (0 != (LOD->SYSFlags & FF_WINDOW_SYS_REFRESH_SIMPLE)),

            (FF_WINDOW_WIN_RESIZABLE & LOD->WINFlags) ? WA_Zoom : TAG_IGNORE, &LOD->Zoom,

            TAG_DONE);
    }

    if (LOD->Win)
    {
        F_Set(LOD->Render,FA_Render_RPort,(uint32)(LOD->Win->RPort));

        F_Do(LOD->Application,FM_Application_ModifyEvents,window_collect_events(Class,Obj),Obj);

        LOD->AppWin = AddAppWindow(MAKE_ID('A','P','P','W'),(uint32)(Obj),LOD->Win,LOD->Win->UserPort,NULL);

        F_Do(Obj,FM_Set,

            FA_NoNotify,            TRUE,
            FA_Window_Title,        LOD->WinTitle,
            FA_Window_ScreenTitle,  LOD->ScrTitle,

            TAG_DONE);

        F_Do(LOD->Decorator,FM_Show);
        F_Do(LOD->Root,FM_Show);

        F_Do(Obj,FM_Window_Draw);

        return LOD->Win;
    }
    else
    {
        F_Log(FV_LOG_USER,"Unable to open window");
    }

__error:

    F_Do(Obj,FM_Window_Cleanup);

    return NULL;
}
//+
///Window_Close
/*

Cette méthode est appelée par l'application lorsque la  fenêtre  doit  être
fermée  ce qui se produit lorsque l'application est réduite ou qu'il y a eu
une modification du système de préférence. Cette méthode est aussi  appelée
lorsque l'attribut FA_Window_Open a été modifié.

*/
F_METHOD(void,Window_Close)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct Window *win = LOD->Win;

    if (win)
    {
        if (LOD->Render)
        {
            F_Set(Obj,FA_Window_ActiveObject,0);

            F_Do(LOD->Root,FM_Hide);
            F_Do(LOD->Decorator,FM_Hide);

            F_Set(LOD->Render,FA_Render_RPort,0);
        }

        LOD->Box.x = win->LeftEdge;
        LOD->Box.y = win->TopEdge;
        LOD->Box.w = win->Width;
        LOD->Box.h = win->Height;

        if (LOD->AppWin)
        {
            RemoveAppWindow(LOD->AppWin); LOD->AppWin = NULL;
        }

        F_Do(LOD->Application, FM_Application_ModifyEvents, 0, Obj);

        LOD->Win = NULL;

        CloseWindow(win);
    }

    F_Do(Obj,FM_Window_Cleanup);
}
//+

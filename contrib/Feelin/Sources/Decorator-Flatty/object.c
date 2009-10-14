#include "Private.h"

#define _gad_init(obj)                          FAreaPublic * gad_area_public = (FAreaPublic *) F_Get(obj, FA_Area_PublicData)
#define _gad_box                                (gad_area_public -> Box)
#define _gad_minmax                             (gad_area_public -> MinMax)
#define _gad_x                                  (_gad_box.x)
#define _gad_y                                  (_gad_box.y)
#define _gad_w                                  (_gad_box.w)
#define _gad_h                                  (_gad_box.h)
#define _gad_minw                               (_gad_minmax.MinW)
#define _gad_minh                               (_gad_minmax.MinH)
#define _gad_maxw                               (_gad_minmax.MaxW)
#define _gad_maxh                               (_gad_minmax.MaxH)
#define _gad_flags                              gad_area_public->Flags

/*** Methods ***************************************************************/

///Deco_New
F_METHOD(uint32,Deco_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    return F_SuperDo(Class,Obj,Method,

        FA_ChainToCycle, FALSE,

        FA_Font, "$decorator-font",
        FA_ColorScheme, "$decorator-scheme-active",
        FA_Back, FP_DRAGBAR_BACK,

    TAG_MORE, Msg);
}
//+
///Deco_Get
F_METHOD(void, Deco_Get)
{
    #if 0
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags, &item, Class))
    switch (item.ti_Tag)
    {
        case FA_Back:
        {
            FFramePublic *fp = (FFramePublic *) F_Get(Obj, FA_Frame_PublicData);

            F_Log(0,"back 0x%08lx 0x%08lx", LOD->dragbar_back, fp->Back);
//            F_STORE(LOD->dragbar_back);
        }
        break;
    }
    #endif

    F_SUPERDO();
}
//+
///Deco_Setup
F_METHODM(uint32,Deco_Setup,FS_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FObject win = Msg -> Render -> Window;

    LOD -> intuition_gadget_size = NewObject(NULL,"buttongclass",GA_SysGType,GTYP_SIZING,TAG_DONE);

    if (LOD->intuition_gadget_size == NULL)
    {
        return FALSE;
    }

    if (F_Get(win, FA_Window_GadDragbar))
    {
        LOD->intuition_gadget_drag = NewObject(NULL,"buttongclass",GA_SysGType,GTYP_WDRAGGING,TAG_DONE);

        if (LOD->intuition_gadget_drag == NULL)
        {
            return FALSE;
        }
    }

    if (F_Get(win,FA_Window_GadClose))
    {
        if ((LOD -> gadget_close = ImageObject, "FA_Image_Spec",FP_IMAGE_CLOSE, FA_ChainToCycle,FALSE, FA_SetMax,FV_SetBoth, FA_InputMode,FV_InputMode_Release, FA_ContextHelp,"Close Window", TAG_DONE)) != NULL)
        {
            F_Do(Obj,FM_AddMember,LOD -> gadget_close,FV_AddMember_Tail);
            F_Do(LOD->gadget_close,FM_Notify,FA_Pressed,FALSE,FV_Notify_Window,FM_Set,2,FA_Window_CloseRequest,TRUE);
        }
        else return FALSE;
    }

    if (F_Get(win,FA_Window_GadDepth))
    {
        if ((LOD -> gadget_depth = ImageObject, "FA_Image_Spec",FP_IMAGE_DEPTH, FA_ChainToCycle,FALSE, FA_SetMax,FV_SetBoth, FA_InputMode,FV_InputMode_Release, FA_ContextHelp,"Depth arrange Window", TAG_DONE)) != NULL)
        {
            F_Do(Obj,FM_AddMember,LOD -> gadget_depth,FV_AddMember_Tail);
            F_Do(LOD->gadget_depth,FM_Notify,FA_Pressed,FALSE,FV_Notify_Window,FM_Window_Depth,1,FV_Window_Depth_Toggle);
        }
        else return FALSE;
    }

    if ((LOD -> gadget_zoom = ImageObject, "FA_Image_Spec",FP_IMAGE_ZOOM, FA_ChainToCycle,FALSE, FA_SetMax,FV_SetBoth, FA_InputMode,FV_InputMode_Release, FA_ContextHelp,"Alternate Dimensions", TAG_DONE)) != NULL)
    {
        F_Do(Obj,FM_AddMember,LOD -> gadget_zoom,FV_AddMember_Tail);
        F_Do(LOD->gadget_zoom,FM_Notify,FA_Pressed,FALSE,FV_Notify_Window,FM_Window_Zoom,1,FV_Window_Zoom_Toggle);
    }
    else return FALSE;

    /* Gadgets must be linked to the decorator BEFORE passing the method to
    the superclass, otherwise children won't be setuped. */

    #if 0
    /*** $decorator-flatty-dragbar-back ***/

    data = F_Do(Msg->Render->Application, FM_Application_Resolve, FP_DRAGBAR_BACK, DEF_DRAGBAR_BACK);

    F_Log(0,"dragbar back (%s)",FP_DRAGBAR_BACK);

    LOD->dragbar_back = ImageDisplayObject,

        FA_ImageDisplay_Spec, data,

        End;

    if (F_Do(LOD->dragbar_back, FM_ImageDisplay_Setup, Msg->Render) == FALSE)
    {
        return FALSE;
    }
    #endif

    if (F_SUPERDO())
    {
        FFramePublic *fp = (FFramePublic *) F_Get(Obj, FA_Frame_PublicData);

        uint32 data = F_Do(_app, FM_Application_Resolve, "$decorator-scheme-inactive", NULL);

        LOD->scheme_active = (FPalette *) F_Get(Obj,FA_ColorScheme);
        LOD->scheme_inactive = (FPalette *) F_Do(_display, FM_CreateColorScheme, data, LOD->scheme_active);

        if (LOD->scheme_inactive == NULL)
        {
            return FALSE;
        }

        F_Set(fp->Back, FA_ImageDisplay_Origin, (uint32) &LOD->dragbar_box);

        /*** $decorator-flatty-sizebar-back ***/

        data = F_Do(_app, FM_Application_Resolve, FP_SIZEBAR_BACK, DEF_SIZEBAR_BACK);

        LOD->sizebar_back = ImageDisplayObject,

            FA_ImageDisplay_Spec, data,

            End;

        if (F_Do(LOD->sizebar_back, FM_ImageDisplay_Setup, _render) == FALSE)
        {
            return FALSE;
        }

        /*** text display ***/

        if (LOD->intuition_gadget_drag)
        {
            LOD->td = TextDisplayObject,

                FA_TextDisplay_Font,     _font,
                FA_TextDisplay_Shortcut, FALSE,

                End;

            if (F_Do(LOD -> td,FM_TextDisplay_Setup,_render) == FALSE)
            {
                return FALSE;
            }

            LOD->preparse_active = (STRPTR) F_Do(_app, FM_Application_Resolve, "$decorator-preparse-active", DEF_PREPARSE_ACTIVE);
            LOD->preparse_inactive = (STRPTR) F_Do(_app, FM_Application_Resolve, "$decorator-preparse-inactive", DEF_PREPARSE_INACTIVE);
        }

        LOD->sizebar_h = F_Do(_app, FM_Application_ResolveInt, FP_SIZEBAR_HEIGHT, DEF_SIZEBAR_HEIGHT);
        #if 0
        LOD->gadgets_w = F_Do(_app, FM_Application_ResolveInt, "$decorator-flatty-gadgets-width", 0);
        LOD->gadgets_h = F_Do(_app, FM_Application_ResolveInt, "$decorator-flatty-gadgets-height", 0);
        #endif
        LOD->spacing_horizontal = F_Do(_app, FM_Application_ResolveInt, FP_SPACING_HORIZONTAL, DEF_SPACING_HORIZONTAL);
        LOD->spacing_vertical = F_Do(_app, FM_Application_ResolveInt, FP_SPACING_VERTICAL, DEF_SPACING_VERTICAL);

        LOD -> notify_handler_window_active = (FNotifyHandler *) F_Do(_win,FM_Notify, FA_Window_Active,FV_Notify_Always, Obj,FM_Set,2, FA_Active,FV_Notify_Value);
//        LOD -> nn_window_title  = (FNotifyHandler *) F_Do(_win,FM_Notify,FA_Window_Title, FV_Notify_Always,LOD -> gadget_drag,FM_Draw,1,FF_Draw_Object);

        return TRUE;
    }
    return FALSE;
}
//+
///Deco_Cleanup
F_METHOD(void,Deco_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FFamilyNode *node;

    if (LOD->intuition_gadget_size)
    {
        DisposeObject(LOD->intuition_gadget_size); LOD->intuition_gadget_size = NULL;
    }

    if (LOD->intuition_gadget_drag)
    {
        DisposeObject(LOD->intuition_gadget_drag); LOD->intuition_gadget_drag = NULL;
    }

    if (_render)
    {
        F_Do(LOD->td, FM_TextDisplay_Cleanup, _render);
        F_DisposeObj(LOD->td); LOD->td = NULL;

        #if 0
        F_Do(LOD->dragbar_back, FM_ImageDisplay_Cleanup, _render);
        F_DisposeObj(LOD->dragbar_back); LOD->dragbar_back = NULL;
        #endif

        F_Do(LOD->sizebar_back, FM_ImageDisplay_Cleanup, _render);
        F_DisposeObj(LOD->sizebar_back); LOD->sizebar_back = NULL;





        F_Do(_win,FM_UnNotify,LOD -> notify_handler_window_active); LOD -> notify_handler_window_active = NULL;
//        F_Do(_win,FM_UnNotify,LOD -> nn_window_title);  LOD -> nn_window_title  = NULL;

        LOD->scheme_active = NULL;

        if (LOD->scheme_inactive)
        {
            F_Do(_display,FM_RemPalette,LOD->scheme_inactive); LOD->scheme_inactive = NULL;
        }
    }

    F_SUPERDO();

    while ((node = (FFamilyNode *) F_Get(Obj,FA_Family_Tail)) != NULL)
    {
        F_DisposeObj(node -> Object);
    }
}
//+

///Deco_AskMinMax
F_METHOD(void, Deco_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);

    uint32 maxw=0, maxh=0;

    F_SUPERDO();

    _maxw = FV_MAXMAX;
    _maxh = FV_MAXMAX;

    LOD->sizebar_h = MAX(LOD->sizebar_h, DEF_SIZEBAR_HEIGHT);

    if (LOD->gadget_close)
    {
        _gad_init(LOD->gadget_close);

        maxw = MAX(maxw, _gad_maxw);
        maxh = MAX(maxh, _gad_maxh);
    }

    if (LOD->gadget_zoom)
    {
        _gad_init(LOD->gadget_zoom);

        maxw = MAX(maxw, _gad_maxw);
        maxh = MAX(maxh, _gad_maxh);
    }
    if (LOD->gadget_depth)
    {
        _gad_init(LOD->gadget_depth);

        maxw = MAX(maxw, _gad_maxw);
        maxh = MAX(maxh, _gad_maxh);
    }

    //F_Log(0,"maxw %ld, maxh %ld, gad_w %ld, gad_h %ld",maxw, maxh,LOD->gadgets_w, LOD->gadgets_h);

    LOD->gadgets_w = MAX(LOD->gadgets_w, maxw);
    LOD->gadgets_h = MAX(LOD->gadgets_h, maxh);

    _bl = 1;
    _br = 1;

    if (LOD -> intuition_gadget_drag)
    {
        _bt = 2 + MAX(_font -> tf_YSize, LOD->gadgets_h + LOD->spacing_vertical * 2);
    }
    else
    {
        _bt = 1;
    }

    _bb = 2 + LOD->sizebar_h;
}
//+
///Deco_Layout
F_METHOD(void,Deco_Layout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 need_w=0;
//    uint32 need_h=0;

    struct Window *win;
    uint32 resizable;

    F_Do(_win, FM_Get,

        FA_Window, &win,
        FA_Window_Resizable, &resizable,

        TAG_DONE);

    LOD->dragbar_box.x = _x + 1;
    LOD->dragbar_box.y = _y + 1;
    LOD->dragbar_box.w = _w - 2;
    LOD->dragbar_box.h = _iy - _y + 1;

    if (resizable)
    {
        LOD->flags |= FF_DECO_SIZEBAR;

        SetAttrs(LOD -> intuition_gadget_size,

            GA_Left, _ix + 1,
            GA_Top, _iy2 + 1,
            GA_Width, _iw,
            GA_Height, _y2 - _iy2 + 1,

            TAG_DONE);

        if (win && ((FF_DECO_SIZEBAR_ADDED & LOD->flags) == 0))
        {
            AddGadget(win, LOD->intuition_gadget_size, -1);

            LOD->flags |= FF_DECO_SIZEBAR_ADDED;
        }
    }
    else
    {
        LOD->flags &= ~FF_DECO_SIZEBAR;

        if (win && ((FF_DECO_SIZEBAR_ADDED & LOD->flags) != 0))
        {
            RemoveGadget(win, LOD->intuition_gadget_size);

            LOD->flags &= ~FF_DECO_SIZEBAR_ADDED;
        }
    }

/*** compute gadgets dimensions ********************************************/

    if (LOD->intuition_gadget_drag)
    {
        LOD->flags |= FF_DECO_DRAGBAR;
        need_w += FV_DRAGBAR_MIN_WIDTH + LOD->spacing_horizontal * 2;

        if (LOD -> gadget_close)
        {
            need_w += LOD->gadgets_w + LOD->spacing_horizontal * 2;
        }
        #if 0
        if (LOD -> gadget_iconify)
        {
            need_w += LOD->gadgets_w + LOD->spacing_horizontal * 2;
        }
        #endif
        if (LOD -> gadget_depth)
        {
            need_w += LOD->gadgets_w + LOD->spacing_horizontal * 2;
        }
        if (LOD -> gadget_zoom)
        {
            need_w += LOD->gadgets_w + LOD->spacing_horizontal * 2;
        }

//        need_h += LOD->gadgets_h + LOD->spacing_vertical * 2;
    }
    else
    {
        need_w += LOD->spacing_horizontal * 2;
//        need_h += 1 + LOD->sizebar_h;
    }

    //F_Log(0,"NEED: %ld x %ld",need_w,need_h);

    LOD->flags &= ~FF_DECO_TITLEONLY;

    if (need_w > _w)
    {
        //F_Log(0,"needs too big: %ld x %ld (available %ld x %ld)",need_w,need_h,_w,_h);

        need_w = FV_DRAGBAR_MIN_WIDTH + LOD->spacing_vertical * 2;

        //F_Log(0,"TBAR %ld x %ld",need_w);

        if (need_w > _w)
        {
            LOD->flags &= ~FF_DECO_DRAGBAR;
        }
        else
        {
            LOD->flags |= FF_DECO_TITLEONLY;
        }
    }

    if (FF_DECO_DRAGBAR & LOD->flags)
    {
        if (FF_DECO_TITLEONLY & LOD->flags)
        {
            LOD->dragbar_x1 = _ix + 1;
            LOD->dragbar_x2 = _ix2 - 1;
        }
        else
        {
            uint16 x1 = _x + 1;
            uint16 y1 = _y + 1 + LOD->spacing_vertical;
            uint16 x2 = _x2 - 1;

/*** gadget_close & gadget_iconify *****************************************/

            if (!LOD -> gadget_close /*&& !LOD -> gadget_iconify*/)
            {
                x1 += LOD->spacing_horizontal;
            }
            else
            {
                if (LOD -> gadget_close)
                {
                    _gad_init(LOD -> gadget_close);

                    _gad_x = x1 + LOD->spacing_horizontal;
                    _gad_y = y1;
                    _gad_w = LOD->gadgets_w;
                    _gad_h = LOD->gadgets_h;
                    _gad_flags |= FF_Area_Damaged;

                    x1 += LOD->spacing_horizontal + LOD->gadgets_w;

                    //F_Log(0,"gad_close %ld x %ld",_gad_w,_gad_h);

                    F_Do(LOD -> gadget_close, FM_Layout);
                }

                #if 0
                if (LOD -> gadget_iconify)
                {
                    _gad_init(LOD -> gadget_iconify);

                    _gad_x = x1 + LOD->spacing_horizontal;
                    _gad_y = y1;
                    _gad_w = LOD->gadgets_w;
                    _gad_h = LOD->gadgets_h;
                    _gad_flags |= FF_Area_Damaged;

                    x1 += LOD->spacing_horizontal + LOD->gadgets_w;

                    //F_Log(0,"gad_Iconify %ld x %ld",_gad_w,_gad_h);

                    F_Do(LOD -> gadget_iconify, FM_Layout);
                }
                #endif
            }

/*** gadget_zoom & gadget_depth ********************************************/

            if (!LOD -> gadget_zoom && !LOD -> gadget_depth)
            {
                x2 -= LOD->spacing_horizontal;
            }
            else
            {
                if (LOD -> gadget_depth)
                {
                    _gad_init(LOD -> gadget_depth);

                    _gad_x = x2 - LOD->spacing_horizontal - LOD->gadgets_w + 1;
                    _gad_y = y1;
                    _gad_w = LOD->gadgets_w;
                    _gad_h = LOD->gadgets_h;
                    _gad_flags |= FF_Area_Damaged;

                    x2 -= LOD->spacing_horizontal + LOD->gadgets_w;

                    //F_Log(0,"gadget_depth %ld : %ld, %ld x %ld",_gad_x,_gad_y,_gad_w,_gad_h);

                    F_Do(LOD -> gadget_depth, FM_Layout);
                }

                if (LOD -> gadget_zoom)
                {
                    _gad_init(LOD -> gadget_zoom);

                    _gad_x = x2 - LOD->spacing_horizontal - LOD->gadgets_w + 1;
                    _gad_y = y1;
                    _gad_w = LOD->gadgets_w;
                    _gad_h = LOD->gadgets_h;
                    _gad_flags |= FF_Area_Damaged;

                    x2 -= LOD->spacing_horizontal + LOD->gadgets_w;

                    //F_Log(0,"gadget_zoom %ld : %ld, %ld x %ld",_gad_x,_gad_y,_gad_w,_gad_h);

                    F_Do(LOD -> gadget_zoom, FM_Layout);
                }
            }

            LOD->dragbar_x1 = x1 + LOD->spacing_horizontal;
            LOD->dragbar_x2 = x2 - LOD->spacing_horizontal;
        }

        if (LOD->intuition_gadget_drag)
        {
            SetAttrs(LOD->intuition_gadget_drag,

                GA_Left,     LOD->dragbar_x1,
                GA_Top,      _y + 1,
                GA_Width,    LOD->dragbar_x2 - LOD->dragbar_x1 + 1,
                GA_Height,   _iy - _y,

                TAG_DONE);
        }
    }
}
//+

///Deco_Show
F_METHOD(uint32,Deco_Show)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        struct Window *win;
        uint32 active;
        uint32 resizable;

        F_Do(_win, FM_Get,

            FA_Window, &win,
            FA_Window_Active, &active,
            FA_Window_Resizable, &resizable,

            TAG_DONE);

        F_Set(Obj, FA_Active, active);

        if (win)
        {
            if (resizable && ((FF_DECO_SIZEBAR_ADDED & LOD->flags) == 0))
            {
                AddGadget(win, LOD->intuition_gadget_size, -1);

                LOD->flags |= FF_DECO_SIZEBAR_ADDED;
            }

            if (LOD->intuition_gadget_drag)
            {
                AddGadget(win, LOD->intuition_gadget_drag, -1);
            }

            if (LOD -> gadget_close)
            {
                win->Flags |= WFLG_CLOSEGADGET;
            }
            if (LOD -> gadget_zoom)
            {
                win->Flags |= WFLG_HASZOOM;
            }
        }
    }
    return F_SUPERDO();
}
//+
///Deco_Hide
F_METHOD(uint32,Deco_Hide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        struct Window *win = (struct Window *) F_Get(_win, FA_Window);

        if (FF_DECO_SIZEBAR_ADDED & LOD->flags)
        {
            RemoveGadget(win, LOD->intuition_gadget_size);

            LOD->flags &= ~FF_DECO_SIZEBAR_ADDED;
        }

        if (LOD->intuition_gadget_drag)
        {
            RemoveGadget(win, LOD->intuition_gadget_drag);
        }
    }
    return F_SUPERDO();
}
//+

///Deco_GoActive
F_METHOD(void,Deco_GoActive)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);

    FAreaPublic *ap;

    if ((ap = (FAreaPublic *) F_Get(LOD->gadget_close, FA_Area_PublicData)) != NULL)
    {
        ap->Palette = LOD->scheme_active;
    }

    if ((ap = (FAreaPublic *) F_Get(LOD->gadget_zoom, FA_Area_PublicData)) != NULL)
    {
        ap->Palette = LOD->scheme_active;
    }

    if ((ap = (FAreaPublic *) F_Get(LOD->gadget_depth, FA_Area_PublicData)) != NULL)
    {
        ap->Palette = LOD->scheme_active;
    }

    F_Draw(Obj,FF_Draw_Object);
}
//+
///Deco_GoInactive
F_METHOD(void,Deco_GoInactive)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);

    FAreaPublic *ap;

    if ((ap = (FAreaPublic *) F_Get(LOD->gadget_close, FA_Area_PublicData)) != NULL)
    {
        ap->Palette = LOD->scheme_inactive;
    }

    if ((ap = (FAreaPublic *) F_Get(LOD->gadget_zoom, FA_Area_PublicData)) != NULL)
    {
        ap->Palette = LOD->scheme_inactive;
    }

    if ((ap = (FAreaPublic *) F_Get(LOD->gadget_depth, FA_Area_PublicData)) != NULL)
    {
        ap->Palette = LOD->scheme_inactive;
    }

    F_Draw(Obj,FF_Draw_Object);
}
//+

///Deco_Draw
F_METHODM(void,Deco_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);

    struct RastPort *rp = _rp;

    int16 x1 = _x;
    int16 y1 = _y;
    int16 x2 = _x2;
    int16 y2 = _y2;
    int16 ix1 = _ix - 1;
    int16 ix2 = _ix2 + 1;
    int16 iy1 = _iy - 1;
    int16 iy2 = _iy2 + 1;

    int8 active = F_Get(Obj, FA_Active);
    FPalette *palette = active ? LOD->scheme_active : LOD->scheme_inactive;
    uint32 *pn = palette->Pens;

    _palette = palette;
    _render->Palette = palette;
//    F_Log(0,"palette 0x%08lx",palette);

    _APen(pn[FV_Pen_Dark]);
    _Move(x1, y1); _Draw(x1, y2); _Draw(x2, y2); _Draw(x2, y1); _Draw(x1, y1);
    _Move(ix1, iy1); _Draw(ix2, iy1);
    _Move(ix1, iy2); _Draw(ix2, iy2);

    if (_parent != _win)
    {
        FBox b;

        b.x = _ix;
        b.y = _iy;
        b.w = _iw;
        b.h = _ih;

        F_Do(_parent, FM_Erase, &b, FF_Erase_Box);
    }

    if (FF_DECO_DRAGBAR & LOD->flags)
    {
        STRPTR title = (STRPTR) F_Get(_win, FA_Window_Title);
        FRect r;

        r.x1 = x1 + 1;
        r.y1 = y1 + 1;
        r.x2 = x2 - 1;
        r.y2 = iy1 - 1;

        F_Do(Obj, FM_Erase, &r, 0);

        if (title)
        {
            uint32 w,h;

            r.x1 = LOD->dragbar_x1 + 1;
            r.y1 = y1 + 1 + LOD->spacing_vertical;
            r.x2 = LOD->dragbar_x2 - 1;
            r.y2 = iy1 - 1 - LOD->spacing_vertical;

            F_Do(LOD->td, FM_Set,

                FA_TextDisplay_PreParse, active ? LOD->preparse_active : LOD->preparse_inactive,
                FA_TextDisplay_Contents, title,
                FA_TextDisplay_Width,    r.x2 - r.x1 + 1,
                FA_TextDisplay_Height,   r.y2 - r.y1 + 1,

                TAG_DONE);

            F_Do(LOD->td, FM_Get,

                FA_TextDisplay_Width, &w,
                FA_TextDisplay_Height, &h,

                TAG_DONE);

            //F_Log(0,"w (%ld) h (%ld) font (%ld) rect (%ld)",w,h,_font->tf_YSize,r.y2 - r.y1 + 1);

            if (w && h)
            {
                if (h < r.y2 - r.y1 + 1)
                {
                    r.y1 = (r.y2 - r.y1 + 1 - h) / 2 + r.y1 - 1;

                    #if 0
                    _APen(3);
                    _Boxf(r.x1, r.y1, r.x2, r.y1 + h);
                    #endif
                }

                F_Do(LOD->td, FM_TextDisplay_Draw, &r);
            }
        }

        if ((FF_DECO_TITLEONLY & LOD->flags) == 0)
        {
            F_Draw(LOD->gadget_close, FF_Draw_Update);
            F_Draw(LOD->gadget_zoom, FF_Draw_Update);
            F_Draw(LOD->gadget_depth, FF_Draw_Update);
        }
    }

    if (FF_DECO_SIZEBAR & LOD->flags)
    {
        FRect r;

        r.x1 = ix1 + 1;
        r.y1 = iy2 + 1;
        r.x2 = x2 - 1;
        r.y2 = y2 - 1;

        F_Do(LOD->sizebar_back, FM_ImageDisplay_Draw, _render, &r, 0);
    }
}
//+


#include "Private.h"

/*** Methods ***************************************************************/

///Shade_Setup
F_METHODM(uint32,Shade_Setup,FS_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FObject win = Msg -> Render -> Window;

/// Gadgets
   if (F_Get(win,FA_Window_GadClose))
   {
      if ((LOD -> gadget_close = F_NewObj(GadgetClass -> Name,FA_Gadget_Type,FV_GadgetType_Close,FA_ContextHelp,"Close Window",TAG_DONE)) != NULL)
      {
         F_Do(Obj,FM_AddMember,LOD -> gadget_close,FV_AddMember_Tail);
      }
      else return FALSE;
   }

   if (!(FF_SHADE_ZOOM_IS_ICONIFY & LOD->flags))
   {
      if (F_Get(win,FA_Window_GadIconify))
      {
         if ((LOD -> gadget_iconify = F_NewObj(GadgetClass -> Name,FA_Gadget_Type,FV_GadgetType_Iconify,FA_ContextHelp,"Iconify Application",TAG_DONE)) != NULL)
         {
            F_Do(Obj,FM_AddMember,LOD -> gadget_iconify,FV_AddMember_Tail);
         }
         else return FALSE;
      }
   }

   if (F_Get(win,FA_Window_GadDragbar))
   {
      if ((LOD -> gadget_drag = F_NewObj(GadgetClass -> Name,FA_Gadget_Type,FV_GadgetType_Dragbar,FA_ContextHelp,"Drag Window",TAG_DONE)) != NULL)
      {
         F_Do(Obj,FM_AddMember,LOD -> gadget_drag,FV_AddMember_Tail);
      }
      else return FALSE;
   }

   if (F_Get(win,FA_Window_GadDepth))
   {
      if ((LOD -> gadget_depth = F_NewObj(GadgetClass -> Name,FA_Gadget_Type,FV_GadgetType_Depth,FA_ContextHelp,"Depth arrange Window",TAG_DONE)) != NULL)
      {
         F_Do(Obj,FM_AddMember,LOD -> gadget_depth,FV_AddMember_Tail);
      }
      else return FALSE;
   }

    /* the following gadgets are always created but added  and  removed  on
    the fly depending on window sizable state */

    if ((LOD -> gadget_zoom = F_NewObj(GadgetClass -> Name,FA_Gadget_Type,FV_GadgetType_Zoom,FA_ContextHelp,"Zoom Window",TAG_DONE)) != NULL)
    {
        F_Do(Obj,FM_AddMember,LOD -> gadget_zoom,FV_AddMember_Tail);

        if ((LOD -> gadget_size = F_NewObj(GadgetClass -> Name,FA_Gadget_Type,FV_GadgetType_Size,FA_ContextHelp,"Resize Window",TAG_DONE)) != NULL)
        {
            F_Do(Obj,FM_AddMember,LOD -> gadget_size,FV_AddMember_Tail);
        }
        else return FALSE;
    }
    else return FALSE;

//+

    /* Gadgets must be linked to the decorator BEFORE passing the method to
    the superclass, otherwise children won't be setuped. */

    if (F_SUPERDO())
    {
        uint32 data;

        LOD->scheme_active = (FPalette *) F_Get(Obj,FA_ColorScheme);

        data = F_Do(Msg -> Render -> Application,FM_Application_Resolve,"$decorator-scheme-inactive",NULL);
        if (!(LOD->scheme_inactive = (FPalette *) F_Do(Msg -> Render -> Display,FM_CreateColorScheme,data,LOD->scheme_active))) return FALSE;

        LOD -> nn_window_active = (FNotifyHandler *) F_Do(_win,FM_Notify,FA_Window_Active,FV_Notify_Always,Obj,FM_Set,2,FA_Active,FV_Notify_Value);
        LOD -> nn_window_title  = (FNotifyHandler *) F_Do(_win,FM_Notify,FA_Window_Title, FV_Notify_Always,LOD -> gadget_drag,FM_Draw,1,FF_Draw_Object);

        LOD->border = F_Do(_app,FM_Application_ResolveInt,"$decorator-shade-border",DEF_TICKNESS); LOD->border = MAX(LOD->border,DEF_TICKNESS);
        LOD->gadgets_w = F_Do(_app,FM_Application_ResolveInt,"$decorator-shade-gadgets-width",_font -> tf_YSize + _font -> tf_YSize / 4); LOD->gadgets_w = MAX(LOD->gadgets_w,_font -> tf_YSize);
        LOD->gadgets_h = F_Do(_app,FM_Application_ResolveInt,"$decorator-shade-gadgets-height",_font -> tf_YSize); LOD->gadgets_h = MAX(LOD->gadgets_h,_font -> tf_YSize);

        _bl = 2 + LOD->border;
        _br = 2 + LOD->border;
        _bb = 2 + LOD->border;

        if (LOD -> gadget_drag)
        {
           _bt = 2 + MAX(_font -> tf_YSize,LOD->gadgets_h + TITLEY1 + TITLEY2);
        }
        else
        {
           _bt = 2 + LOD->border;
        }

        return TRUE;
    }
    return FALSE;
}
//+
///Shade_Cleanup
F_METHOD(void,Shade_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FFamilyNode *node;

   if (_render)
   {
      F_Do(_win,FM_UnNotify,LOD -> nn_window_active); LOD -> nn_window_active = NULL;
      F_Do(_win,FM_UnNotify,LOD -> nn_window_title);  LOD -> nn_window_title  = NULL;

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
///Shade_Layout
#define _gad_init(obj)                          FAreaPublic * gad_area_public = (FAreaPublic *) F_Get(obj, FA_Area_PublicData)
#define _gad_box                                (gad_area_public -> Box)
#define _gad_x                                  (_gad_box.x)
#define _gad_y                                  (_gad_box.y)
#define _gad_w                                  (_gad_box.w)
#define _gad_h                                  (_gad_box.h)

F_METHOD(void,Shade_Layout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 need_w=0;
//    uint32 need_h=0;
    
    BOOL resizable = F_Get(_win,FA_Window_Resizable);
    
    //F_Log(0,"Layout %ld : %ld, %ld x %ld",_x,_y,_w,_h);
    
/*** clear flags ***********************************************************/

    LOD->flags &= ~(FF_SHADE_TITLEBARONLY | FF_SHADE_DRAG | FF_SHADE_SIZE);
    #ifdef F_USE_LAST_DRAWN
    LOD->last_drawn = FV_Shade_LastDrawn_None;
    #endif

/*** resize relative gadgets ***********************************************/

    if (resizable)
    {
        if (LOD -> gadget_size)
        {
            _gad_init(LOD -> gadget_size);

            _gad_w = FV_GADSIZE_WIDTH - 1;
            _gad_h = FV_GADSIZE_WIDTH - 1;
            _gad_x = _x2 - _gad_w + 1;
            _gad_y = _y2 - _gad_h + 1;

            //F_Log(0,"gadget_size %ld : %ld, %ld x %ld",_gad_x,_gad_y,_gad_w,_gad_h);

            F_Do(LOD -> gadget_size,FM_Layout);
            
            LOD->flags |= FF_SHADE_SIZE;
        }
    }
    
/*** compute gadgets dimensions ********************************************/

    if (LOD -> gadget_drag)
    {
        LOD->flags |= FF_SHADE_DRAG;
 
        need_w += FV_DRAGBAR_MIN_WIDTH;
        
        if (LOD -> gadget_close)
        {
            need_w += LOD->gadgets_w + LOD->border * 2;
        }
        if (LOD -> gadget_iconify)
        {
            need_w += LOD->gadgets_w + LOD->border * 2;
        }
        if (LOD -> gadget_depth)
        {
            need_w += LOD->gadgets_w + LOD->border * 2;
        }
        if (LOD -> gadget_zoom)
        {
            need_w += LOD->gadgets_w + LOD->border * 2;
        }
    
//        need_h += LOD->gadgets_h + TITLEY1 + TITLEY2 + LOD->border * 2;
    }
    else
    {
        need_w += LOD->border * 2;
//        need_h += LOD->border * 2;
    }

    //F_Log(0,"NEED: %ld x %ld",need_w,need_h);
    
    if (need_w > _w)
    {
        //F_Log(0,"needs too big: %ld x %ld (available %ld x %ld)",need_w,need_h,_w,_h);
            
        need_w = FV_DRAGBAR_MIN_WIDTH + LOD->border * 2;
    
        //F_Log(0,"TBAR %ld x %ld",need_w);
        
        if (need_w > _w)
        {
            LOD->flags &= ~FF_SHADE_DRAG;
        }
        else
        {
            LOD->flags |= FF_SHADE_TITLEBARONLY;
        }
    }

    if (FF_SHADE_DRAG & LOD->flags)
    {
       _gad_init(LOD -> gadget_drag);

        if (FF_SHADE_TITLEBARONLY & LOD->flags)
        {
            _gad_x = _x + LOD->border + 2;
            _gad_y = _y + 1;
            _gad_w = _w - LOD->border * 2 - 4;
            _gad_h = LOD->gadgets_h + TITLEY1 + TITLEY2;

            //F_Log(0,"gad_titlebar_only %ld x %ld",_gad_w,_gad_h);

            F_Do(LOD -> gadget_drag,FM_Layout);
        }
        else
        {
            uint16 x1 = _x + 1;
            uint16 y1 = _y + 1;
            uint16 x2 = _x2 - 1;
            
/*** gadget_close & gadget_iconify *****************************************/
            
            if (!LOD -> gadget_close && !LOD -> gadget_iconify)
            {
                x1 += LOD->border;
            }
            else
            {
                if (LOD -> gadget_close)
                {
                    _gad_init(LOD -> gadget_close);

                    _gad_x = x1 + LOD->border;
                    _gad_y = y1 + TITLEY1;
                    _gad_w = LOD->gadgets_w;
                    _gad_h = LOD->gadgets_h;

                    x1 = LOD->border * 2 + x1 + LOD->gadgets_w;

                    //F_Log(0,"gad_close %ld x %ld",_gad_w,_gad_h);

                    F_Do(LOD -> gadget_close,FM_Layout);
                }

                if (LOD -> gadget_iconify)
                {
                    _gad_init(LOD -> gadget_iconify);

                    _gad_x = x1 + LOD->border;
                    _gad_y = y1 + TITLEY1;
                    _gad_w = LOD->gadgets_w;
                    _gad_h = LOD->gadgets_h;

                    x1 = LOD->border * 2 + x1 + LOD->gadgets_w;

                    //F_Log(0,"gad_Iconify %ld x %ld",_gad_w,_gad_h);

                    F_Do(LOD -> gadget_iconify,FM_Layout);
                }
            }

/*** gadget_zoom & gadget_depth ********************************************/

            if (!LOD -> gadget_zoom && !LOD -> gadget_depth)
            {
                x2 -= LOD->border;
            }
            else
            {
                if (LOD -> gadget_depth)
                {
                    _gad_init(LOD -> gadget_depth);

                    _gad_x = x2 - LOD->border - LOD->gadgets_w;
                    _gad_y = y1 + TITLEY1;
                    _gad_w = LOD->gadgets_w;
                    _gad_h = LOD->gadgets_h;

                    x2 = x2 - (LOD->border * 2) - LOD->gadgets_w;

                    //F_Log(0,"gadget_depth %ld : %ld, %ld x %ld",_gad_x,_gad_y,_gad_w,_gad_h);

                    F_Do(LOD -> gadget_depth,FM_Layout);
                }

                if (LOD -> gadget_zoom)
                {
                    _gad_init(LOD -> gadget_zoom);

                    _gad_x = x2 - LOD->border - LOD->gadgets_w;
                    _gad_y = y1 + TITLEY1;
                    _gad_w = LOD->gadgets_w;
                    _gad_h = LOD->gadgets_h;

                    x2 = x2 - (LOD->border * 2) - LOD->gadgets_w;

                    //F_Log(0,"gadget_zoom %ld : %ld, %ld x %ld",_gad_x,_gad_y,_gad_w,_gad_h);

                    F_Do(LOD -> gadget_zoom,FM_Layout);
                }
            }

/*** gadget_dragbar ********************************************************/
 
            //F_Log(0,"x1 %ld, x2 %ld, w %ld",x1,x2,x2-x1+1);
            
            if (LOD -> gadget_drag)
            {
                _gad_init(LOD -> gadget_drag);

                _gad_x = x1 + 1;
                _gad_y = y1;
                _gad_w = x2 - x1 - 2;
                _gad_h = LOD->gadgets_h + TITLEY1 + TITLEY2;

                //F_Log(0,"gadget_drag %ld : %ld, %ld x %ld",_gad_x,_gad_y,_gad_w,_gad_h);
                
                F_Do(LOD -> gadget_drag,FM_Layout);
            }
 
        }
    }
}
//+
///Shade_Show
F_METHOD(uint32,Shade_Show)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    #ifdef F_USE_LAST_DRAWN
    LOD->last_drawn = FV_Shade_LastDrawn_None;
    #endif

    if (_render)
    {
        struct Window *osw;
        uint32 active;
        
        F_Do(_win, FM_Get,
        
            FA_Window, &osw,
            FA_Window_Active, &active,
            
            TAG_DONE);

        F_Set(Obj, FA_Active, active);

        if (osw)
        {
            if (LOD -> gadget_close)
            {
                osw->Flags |= WFLG_CLOSEGADGET;
            }
            if (LOD -> gadget_zoom)
            {
                osw->Flags |= WFLG_HASZOOM;
            }
        }
    }
    return F_SUPERDO();
}
//+
///Shade_GoActive
F_METHOD(void,Shade_GoActive)
{
   F_Draw(Obj,FF_Draw_Object);
}
//+
///Shade_GoInactive
F_METHOD(void,Shade_GoInactive)
{
   F_Draw(Obj,FF_Draw_Object);
}
//+


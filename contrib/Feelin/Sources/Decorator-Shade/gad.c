#include "Private.h"

#undef LocalObjectData
#define LocalObjectData GAD_LocalObjectData

/*** Methods ***************************************************************/

///Gad_New
F_METHOD(FObject,Gad_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg;

    F_AREA_SAVE_PUBLIC;

    if ((LOD -> Type = GetTagData(FA_Gadget_Type,0,Tags)) != NULL)
    {
        if (F_SuperDo(Class,Obj,Method,

            FA_ChainToCycle,     FALSE,
            FA_InputMode,        (LOD -> Type == FV_GadgetType_Dragbar || LOD -> Type == FV_GadgetType_Size) ? FV_InputMode_None : FV_InputMode_Release,

        TAG_MORE, Msg))
        {
            switch (LOD -> Type)
            {
                case FV_GadgetType_Close:     F_Do(Obj,FM_Notify,FA_Pressed,FALSE,FV_Notify_Window,FM_Set,2,FA_Window_CloseRequest,TRUE);  break;
                case FV_GadgetType_Iconify:   F_Do(Obj,FM_Notify,FA_Pressed,FALSE,FV_Notify_Application,FM_Set,2,FA_Application_Sleep,TRUE); break;
                case FV_GadgetType_Zoom:      F_Do(Obj,FM_Notify,FA_Pressed,FALSE,FV_Notify_Window,FM_Window_Zoom,1,FV_Window_Zoom_Toggle); break;
                case FV_GadgetType_Depth:     F_Do(Obj,FM_Notify,FA_Pressed,FALSE,FV_Notify_Window,FM_Window_Depth,1,FV_Window_Depth_Toggle); break;
            }
            return Obj;
        }
    }
    return NULL;
}
//+
///Gad_Setup
F_METHOD(uint32,Gad_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        switch (LOD -> Type)
        {
            case FV_GadgetType_Dragbar:
            {
                if (F_Get(_win,FA_Window_GadDragbar))
                {
                    if (!(LOD -> Gadget = NewObject(NULL,"buttongclass",GA_SysGType,GTYP_WDRAGGING,TAG_DONE))) return FALSE;

                    LOD -> Prep[0] = (STRPTR) F_Do(_app,FM_Application_Resolve,"$decorator-preparse-active",NULL);
                    LOD -> Prep[1] = (STRPTR) F_Do(_app,FM_Application_Resolve,"$decorator-preparse-inactive",NULL);

                    LOD -> TDisplay = TextDisplayObject,

                        FA_TextDisplay_Font,     _font,
                        FA_TextDisplay_Shortcut, FALSE,
                        
                        End;

                    if (!F_Do(LOD -> TDisplay,FM_TextDisplay_Setup,_render))
                    {
                        return FALSE;
                    }
                }
            }
            break;

            case FV_GadgetType_Size:
            {
                if (!(LOD -> Gadget = NewObject(NULL,"buttongclass",GA_SysGType,GTYP_SIZING,TAG_DONE)))
                {
                    return FALSE;
                }
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}
//+
///Gad_Cleanup
F_METHOD(void,Gad_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Gadget)
    {
        DisposeObject(LOD -> Gadget); LOD -> Gadget = NULL;

        if (LOD -> TDisplay)
        {
            F_Do(LOD -> TDisplay,FM_TextDisplay_Cleanup);
            F_DisposeObj(LOD -> TDisplay); LOD -> TDisplay = NULL;
        }
    }
    F_SUPERDO();
}
//+
///Gad_Show
F_METHOD(uint32,Gad_Show)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Type == FV_GadgetType_Zoom)
    {
        F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);
    }

    if (LOD -> Gadget && _render)
    {
        struct Window *osw = (APTR) F_Get(_win,FA_Window);

        AddGadget(osw,LOD -> Gadget,-1);
    }
    return F_SUPERDO();
}
//+
///Gad_Hide
F_METHOD(uint32,Gad_Hide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Type == FV_GadgetType_Zoom)
    {
        F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);
    }

    if (LOD -> Gadget && _render)
    {
        struct Window *osw = (struct Window *) F_Get(_win,FA_Window);

        RemoveGadget(osw,LOD -> Gadget);
    }
    return F_SUPERDO();
}
//+
///Gad_Layout
F_METHOD(void,Gad_Layout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Gadget)
    {
        SetAttrs(LOD -> Gadget,
        
            GA_Left,     _x,
            GA_Top,      _y,
            GA_Width,    _w,
            GA_Height,   _h,
            
            TAG_DONE);
    }
    F_SUPERDO();
}
//+
///Gad_HandleEvent
F_METHODM(bits32,Gad_HandleEvent,FS_HandleEvent)
{
//   struct LocalObjectData *LOD = F_LOD(Class,Obj);
/*
    if (LOD -> Type == FV_GadgetType_Zoom &&
         Msg -> FEv -> Class == FF_EVENT_BUTTON)
    {


        _inside(FEv -> MouseX,_x,_x + _w - 1) &&
                         _inside(FEv -> MouseY,_y,_y + _h - 1))

        F_Log(0,"MOUSE BUTTON");
    }
*/
    return F_SUPERDO();
}
//+


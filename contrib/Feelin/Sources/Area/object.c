#include "Private.h"

#define _handle_flags(bits)         if (item.ti_Data) _flags |= (bits); else _flags &= ~(bits);

/*** Methods ***************************************************************/

///Area_New
F_METHOD(FObject,Area_New)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    _weight = 100;
    _flags |= FF_Area_CanShow;
    
    LOD -> Flags = FF_AREA_CHAIN;
    LOD -> FramePublic = (FFramePublic *) F_Get(Obj,FA_Frame_PublicData);
    LOD -> UserMinMax.MinW = 0xFFFF; LOD -> UserMinMax.MinH = 0xFFFF;
    LOD -> UserMinMax.MaxW = 0xFFFF; LOD -> UserMinMax.MaxH = 0xFFFF;

    LOD -> p_Font = (STRPTR) FV_Font_Inherit;

    /***/

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Back:
        {
            LOD -> p_Back   = (STRPTR) item.ti_Data;
        }
        break;
        
        case FA_ColorScheme:
        {
            LOD -> p_Scheme = (STRPTR) item.ti_Data;
        }
        break;
        
        case FA_DisabledColorScheme:
        {
            LOD -> p_DisabledScheme = (STRPTR) item.ti_Data;
        }
        break;
    
        case FA_Font:
        {
            LOD -> p_Font = (STRPTR)(item.ti_Data);
        }
        break;
        
        case FA_ChainToCycle:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_AREA_CHAIN;
            }
            else
            {
                LOD -> Flags &= ~FF_AREA_CHAIN;
            }
        }
        break;
        
        case FA_Horizontal:
        {
            _handle_flags(FF_Horizontal);
        }
        break;

        case FA_FixWidth:
        {
            if (item.ti_Data)
            {
                LOD -> UserMinMax.MinW = (uint16)(item.ti_Data);
                LOD -> UserMinMax.MaxW = (uint16)(item.ti_Data);

                _flags |= FF_Area_SetMaxW;
            }
        }
        break;

        case FA_FixHeight:
        {
            if (item.ti_Data)
            {
                LOD -> UserMinMax.MinH = (uint16)(item.ti_Data);
                LOD -> UserMinMax.MaxH = (uint16)(item.ti_Data);

                _flags |= FF_Area_SetMaxH;
            }
        }
        break;

        case FA_MinWidth:
        {
            LOD -> UserMinMax.MinW = (uint16)(item.ti_Data);
        }
        break;
        
        case FA_MinHeight:
        {
            LOD -> UserMinMax.MinH = (uint16)(item.ti_Data);
        }
        break;
        
        case FA_MaxWidth:
        {
            LOD -> UserMinMax.MaxW = (uint16)(item.ti_Data);
        }
        break;
        
        case FA_MaxHeight:
        {
            LOD -> UserMinMax.MaxH = (uint16)(item.ti_Data);
        }
        break;

        case FA_SetMin:
        {
           switch (item.ti_Data)
           {
              case FV_SetNone:   _flags &= ~(FF_Area_SetMinW | FF_Area_SetMinH);          break;
              case FV_SetBoth:   _flags |= (FF_Area_SetMinW | FF_Area_SetMinH);           break;
              case FV_SetWidth:  _flags |= FF_Area_SetMinW; _flags &= ~FF_Area_SetMinH;   break;
              case FV_SetHeight: _flags |= FF_Area_SetMinH; _flags &= ~FF_Area_SetMinW;   break;
           }
        }
        break;

        case FA_SetMax:
        {
           switch (item.ti_Data)
           {
              case FV_SetNone:   _flags &= ~(FF_Area_SetMaxW | FF_Area_SetMaxH);          break;
              case FV_SetBoth:   _flags |= (FF_Area_SetMaxW | FF_Area_SetMaxH);           break;
              case FV_SetWidth:  _flags |= FF_Area_SetMaxW; _flags &= ~FF_Area_SetMaxH;   break;
              case FV_SetHeight: _flags |= FF_Area_SetMaxH; _flags &= ~FF_Area_SetMaxW;   break;
           }
        }
        break;

        case FA_Weight:
        {
           _weight = item.ti_Data;
        }
        break;

        case FA_InputMode:
        {
           LOD -> InputMode = item.ti_Data;
        }
        break;
    }

    if (F_SUPERDO())
    {
        return Obj;
    }
    return NULL;
}
//+
///Area_Get
F_METHOD(void,Area_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    BOOL up = FALSE;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Area_PublicData:    F_STORE(LOD); break;

        case FA_Left:               F_STORE(_x); break;
        case FA_Top:                F_STORE(_y); break;
        case FA_Right:              F_STORE(_x + _w - 1); break;
        case FA_Bottom:             F_STORE(_y + _h - 1); break;
        case FA_Width:              F_STORE(_w); break;
        case FA_Height:             F_STORE(_h); break;

        case FA_MinWidth:           F_STORE(_minw); break;
        case FA_MinHeight:          F_STORE(_minh); break;
        case FA_MaxWidth:           F_STORE(_maxw); break;
        case FA_MaxHeight:          F_STORE(_maxh); break;

        case FA_Active:             F_STORE(0 != (_flags & FF_Area_Active)); break;
        case FA_Selected:           F_STORE(0 != (_flags & FF_Area_Selected)); break;
        case FA_Pressed:            F_STORE(0 != (_flags & FF_Area_Pressed)); break;
        case FA_Horizontal:         F_STORE(0 != (_flags & FF_Horizontal)); break;
        case FA_Hidden:             F_STORE(LOD -> HiddenCount > 0); break;
        case FA_Disabled:           F_STORE(LOD -> DisabledCount > 0); break;
        case FA_Draggable:          F_STORE(0 != (LOD -> Flags & FF_AREA_DRAGGABLE)); break;
        case FA_Dropable:           F_STORE(0 != (LOD -> Flags & FF_AREA_DROPABLE)); break;

        case FA_Font:               F_STORE(_font); break;
        case FA_NoFill:             F_STORE(0 != (LOD -> Flags & FF_AREA_NOFILL)); break;
        case FA_ControlChar:        F_STORE(LOD -> ControlChar); break;
        case FA_ChainToCycle:       F_STORE(0 != (LOD -> Flags & FF_AREA_CHAIN)); break;
        case FA_WindowObject:       F_STORE(_win); break;
        
        /* general attributes handled */

        case FA_Application:             F_STORE((_render) ? _render -> Application : (FObject) F_Get(_parent,FA_Application)); break;
        case FA_ColorScheme:             F_STORE(LOD -> Scheme); break;
        case FA_DisabledColorScheme:     F_STORE(LOD -> DisabledScheme); break;
        case FA_Parent:                  F_STORE(_parent); break;

  //      case FA_ContextHelp:    F_STORE((LOD -> ContextHelp); break;
  //      case FA_ContextMenu:    F_STORE(LOD -> ContextMenu); break;

        default:
        {
           up = TRUE;
        }
    }

    if (up) F_SUPERDO();
}
//+
///Area_Set
F_METHOD(void,Area_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
///FA_Active
        case FA_Active:
        {
            if (item.ti_Data)
            {
                if (!(_flags & FF_Area_Active))
                {
                    _flags |= FF_Area_Active; F_Do(Obj,FM_GoActive);
                }
            }
            else if (_flags & FF_Area_Active)
            {
                _flags &= ~FF_Area_Active; F_Do(Obj,FM_GoInactive);
            }
        }
        break;
//+
///FA_Selected
        case FA_Selected:
        {
            FInner *in;

            _handle_flags(FF_Area_Selected);

            in = &LOD -> FramePublic -> Border[(FF_Area_Selected & _flags) ? 1 : 0];
      
            CopyMem(in,&_inner,sizeof (FInner));

            in = &LOD -> FramePublic -> Padding[(FF_Area_Selected & _flags) ? 1 : 0];

            _bl += in -> l; _br += in -> r;
            _bb += in -> b; _bt += in -> t;

            F_Do(Obj,FM_Layout);

            F_Set(LOD -> FramePublic -> Back,FA_ImageDisplay_State,(item.ti_Data) ? (FV_Image_Selected) : (FV_Image_Normal));

            F_Draw(Obj,FF_Draw_Object);
        }
        break;
//+
///FA_Pressed
      case FA_Pressed:
      {
         _handle_flags(FF_Area_Pressed);
      }
      break;
//+
///FA_Hidden
        case FA_Hidden:
        {
            if (~(FF_Hidden_Check | FF_Hidden_NoNesting) & item.ti_Data)
            {
                if (FF_Hidden_Check & item.ti_Data)
                {
                    if (LOD->HiddenCount < 1) // shown
                    {
                        if (FF_Hidden_NoNesting & item.ti_Data)
                        {
                            LOD->HiddenCount = 1;
                        }
                        else
                        {
                            LOD->HiddenCount++;
                        }
                    }
                    else break;
                }
                else if (FF_Hidden_NoNesting & item.ti_Data)
                {
                    LOD->HiddenCount = 1;
                }
                else
                {
                    LOD -> HiddenCount++;
                }
            }
            else
            {
                if (FF_Hidden_Check & item.ti_Data)
                {
                    if (LOD->HiddenCount > 0) // hidden
                    {
                        if (FF_Hidden_NoNesting & item.ti_Data)
                        {
                            LOD->HiddenCount = 0;
                        }
                        else
                        {
                            LOD->HiddenCount--;
                        }
                    }
                    else break;
                }
                else if (FF_Hidden_NoNesting & item.ti_Data)
                {
                    LOD->HiddenCount = 0;
                }
                else
                {
                    LOD -> HiddenCount--;
                }
            }

            if (LOD -> HiddenCount == 1)
            {
                _flags &= ~FF_Area_CanShow; F_Do(Obj,FM_Hide);
            }
            else if (LOD -> HiddenCount == 0)
            {
                _flags |=  FF_Area_CanShow; F_Do(Obj,FM_Show);
            }
            else break;

            if (_render)
            {
                _flags |= FF_Area_Damaged;

                F_Do(_win,FM_Window_RequestRethink,Obj);
            }
        }
        break;
//+
///FA_Disabled
        case FA_Disabled:
        {
            if (~(FF_Disabled_Check | FF_Disabled_NoNesting) & item.ti_Data)
            {
                if (FF_Disabled_Check & item.ti_Data)
                {
                    if (LOD->DisabledCount < 1) // enabled
                    {
                        if (FF_Disabled_NoNesting & item.ti_Data)
                        {
                            LOD->DisabledCount = 1;
                        }
                        else
                        {
                            LOD->DisabledCount++;
                        }
                    }
                    else break;
                }
                else if (FF_Disabled_NoNesting & item.ti_Data)
                {
                    LOD->DisabledCount = 1;
                }
                else
                {
                    LOD -> DisabledCount++;
                }
            }
            else
            {
                if (FF_Disabled_Check & item.ti_Data)
                {
                    if (LOD->DisabledCount > 0) // disabled
                    {
                        if (FF_Disabled_NoNesting & item.ti_Data)
                        {
                            LOD->DisabledCount = 0;
                        }
                        else
                        {
                            LOD->DisabledCount--;
                        }
                    }
                    else break;
                }
                else if (FF_Disabled_NoNesting & item.ti_Data)
                {
                    LOD->DisabledCount = 0;
                }
                else
                {
                    LOD -> DisabledCount--;
                }
            }
 
            if (LOD -> DisabledCount == 1)
            {
                F_Do(Obj,FM_GoDisabled);
            }
            else if (LOD -> DisabledCount == 0)
            {
                F_Do(Obj,FM_GoEnabled);
            }
            else break;

            F_Draw(Obj, FF_Draw_Object);
        }
        break;
//+
      
        case FA_Draggable:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_AREA_DRAGGABLE;
            }
            else
            {
                LOD -> Flags &= ~FF_AREA_DRAGGABLE;
            }
        }
        break;
      
        case FA_Dropable:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_AREA_DROPABLE;
            }
            else
            {
                LOD -> Flags &= ~FF_AREA_DROPABLE;
            }
        }
        break;
      
        case FA_Bufferize:
        {
            _handle_flags(FF_Area_Bufferize);
            
            if (FF_Area_Bufferize & _flags)
            {
               F_Log(0,"FA_Bufferize TRUE");
            }
        }
        break;

        case FA_Font:
        {
            if (_render)
            {
                if (_font)
                {
                    CloseFont(_font);
                }

                _font = (struct TextFont *) F_Do(_app,FM_Application_OpenFont,Obj,item.ti_Data);
                
                _flags |= FF_Area_Damaged;
                
                F_Do(_win,FM_Window_RequestRethink,Obj);
            }
        }
        break;

        case FA_NoFill:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_AREA_NOFILL;
            }
            else
            {
                LOD -> Flags &= ~FF_AREA_NOFILL;
            }
        }
        break;

        case FA_ControlChar:
        {
            LOD -> ControlChar = (UBYTE)(item.ti_Data);
        }
        break;

        case FA_ContextHelp:
        {
            LOD -> ContextHelp = (STRPTR)(item.ti_Data);
        }
        break;
        
        case FA_ContextMenu:
        {
            LOD -> ContextMenu = (FObject)(item.ti_Data);
        }
        break;
   }

   F_SUPERDO();
}
//+

///Area_Connect
F_METHODM(uint32,Area_Connect,FS_Connect)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _parent = Msg -> Parent;

    return TRUE;
}
//+
///Area_Disconnect
F_METHOD(uint32,Area_Disconnect)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _parent = NULL;

    return TRUE;
}
//+
///Area_Import
F_METHODM(uint32,Area_Import,FS_Import)
{
    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        int32 *data = (int32 *) F_Do(Msg -> Dataspace,Msg -> id_Find,id);

        if (data)
        {
            F_Set(Obj,FA_Selected,(uint32)(*data));
        }
    }

    return TRUE;
}
//+
///Area_Export
F_METHODM(uint32,Area_Export,FS_Export)
{
    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        int32 data = F_Get(Obj,FA_Selected);
  
        F_Do(Msg -> Dataspace,Msg -> id_Add,id,&data,4);
    }

    return TRUE;
}
//+


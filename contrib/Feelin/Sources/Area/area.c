#include "Private.h"

/*** Methods ***************************************************************/

///Area_Setup
F_METHODM(int32,Area_Setup,FS_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FRender *Render = Msg -> Render;
    FInner *in;

    _render = Render;
    
    LOD -> Flags &= ~FF_AREA_HANDLEACTIVE;

    if (FF_AREA_CHAIN & LOD -> Flags)
    {
        F_Do(Render -> Window,FM_Window_ChainAdd,Obj);
    }

    /* FM_ModifyHandler only adds the handler if the object is not disabled
    and can be shown */

    if (LOD -> InputMode)
    {
        F_Do(Obj,FM_ModifyHandler,FF_EVENT_KEY | FF_EVENT_BUTTON,0);
    }
    else if (FF_AREA_DRAGGABLE & LOD -> Flags)
    {
        LOD -> InputMode = FV_InputMode_Immediate;

        F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);
    }

/*** color scheme : standard ***********************************************/

    if (LOD -> p_Scheme)
    {
        STRPTR spec = (STRPTR) F_Do(Render -> Application,FM_Application_Resolve,LOD -> p_Scheme,DEF_SCHEME_DATA);

        if (spec)
        {
            LOD -> Scheme = (FPalette *) F_Do(Render -> Display,FM_CreateColorScheme,spec,F_Get(_parent,FA_ColorScheme));
        }
    }

    if (LOD -> Scheme)
    {
        LOD -> Flags &= ~FF_AREA_INHERITED_PENS;
    }
    else
    {
        LOD -> Flags |= FF_AREA_INHERITED_PENS;
        LOD -> Scheme  = (FPalette *) F_Get(_parent,FA_ColorScheme);
    }

/*** color scheme : disabled ***********************************************/

    if (LOD -> p_DisabledScheme)
    {
        STRPTR spec = (STRPTR) F_Do(Render -> Application,FM_Application_Resolve,LOD -> p_DisabledScheme,DEF_DISABLEDSCHEME_DATA);

        if (spec)
        {
            LOD -> DisabledScheme = (FPalette *) F_Do(Render -> Display,FM_CreateColorScheme,spec,LOD -> Scheme);
        }
    }

    if (LOD -> DisabledScheme)
    {
        LOD -> Flags  &= ~FF_AREA_INHERITED_DISABLEDPENS;
    }
    else
    {
        LOD -> Flags  |= FF_AREA_INHERITED_DISABLEDPENS;
        LOD -> DisabledScheme  = (FPalette *) F_Get(_parent,FA_DisabledColorScheme);
    }

/*** frame *****************************************************************/

    F_SuperDo(Class,Obj,FM_Frame_Setup,Msg -> Render);

    in = &LOD -> FramePublic -> Border[(FF_Area_Selected & _flags) ? 1 : 0];

    CopyMem(in,&_inner,sizeof (FInner));

    in = &LOD -> FramePublic -> Padding[(FF_Area_Selected & _flags) ? 1 : 0];

    _bl += in -> l; _br += in -> r;
    _bb += in -> b; _bt += in -> t;

/*** background ************************************************************/

    if (LOD -> FramePublic -> Back)
    {
        LOD -> Flags &= ~FF_AREA_INHERITED_BACK;
        F_Set(LOD -> FramePublic -> Back,FA_ImageDisplay_Origin, (uint32) &_box);
    }
    else
    {
        LOD -> Flags |= FF_AREA_INHERITED_BACK;
        LOD -> FramePublic -> Back = (FObject) F_Get(_parent,FA_Back);
    }
  
/*** font ******************************************************************/

    _font = (struct TextFont *) F_Do(Render -> Application,FM_Application_OpenFont,Obj,LOD -> p_Font);
  
/*** set appropriate color scheme for drawing ******************************/

    if (FF_Area_Disabled & _flags)
    {
        #if F_CODE_DEPRECATED
        _pens = LOD -> DisabledScheme -> Pens;
        #else
        _palette = LOD->DisabledScheme;
        //F_Log(0,"palette 0x%08lx (disabled)",LOD->DisabledScheme);
        #endif
    }
    else
    {
        #if F_CODE_DEPRECATED
        _pens = LOD -> Scheme -> Pens;
        #else
        _palette = LOD->Scheme;
        //F_Log(0,"palette 0x%08lx (enabled)",LOD->Scheme);
        #endif
    }

/*** set flag **************************************************************/

    _flags |= FF_Area_Setup;

    return TRUE;
}
//+
///Area_Cleanup
F_METHOD(void,Area_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   _flags &= ~(FF_Area_Setup | FF_Area_CanDraw);

   if (_render)
   {
      if (FF_AREA_CHAIN & LOD -> Flags)
      {
         F_Do(_render -> Window,FM_Window_ChainRem,Obj);
      }

      if (LOD -> Handler)
      {
         F_Do(Obj,FM_ModifyHandler,NULL,ALL);
      }

      /**/

        #if F_CODE_DEPRECATED
        _pens = NULL;
        #else
        //F_Log(0,"clear palette");
        _palette = NULL;
        #endif

      if (FF_AREA_INHERITED_DISABLEDPENS & LOD -> Flags)
      {
         LOD -> Flags &= ~FF_AREA_INHERITED_DISABLEDPENS;
      }
      else if (LOD -> DisabledScheme)
      {
         F_Do(_render -> Display,FM_RemPalette,LOD -> DisabledScheme);
      }
      LOD -> DisabledScheme = NULL;

      if (FF_AREA_INHERITED_PENS & LOD -> Flags)
      {
         LOD -> Flags &= ~FF_AREA_INHERITED_PENS;
      }
      else if (LOD -> Scheme)
      {
         F_Do(_render -> Display,FM_RemPalette,LOD -> Scheme);
      }
      LOD -> Scheme = NULL;

      /**/

      if (FF_AREA_INHERITED_BACK & LOD -> Flags)
      {
         LOD -> FramePublic -> Back = NULL;
      }
      
      if (LOD -> FramePublic)
      {
         F_SuperDo(Class,Obj,FM_Frame_Cleanup,_render);
      }

      /**/

      _render = NULL;
   }

   if (_font)
   {
      CloseFont(_font); _font = NULL;
   }
}
//+
///Area_Show
F_METHOD(int32,Area_Show)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        FAreaPublic *pp = (FAreaPublic *) F_Get(_parent, FA_Area_PublicData);
 
        if (pp)
        {
            //F_Log(0,"set parent damage");
            
            pp->Flags |= FF_Area_Damaged;
        }

        _flags |= FF_Area_CanDraw;
 
        if (LOD -> Handler)
        {
            area_try_add_handler(Class,Obj,FeelinBase);
        }
    }

    return TRUE;
}
//+
///Area_Hide
F_METHOD(int32,Area_Hide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        FAreaPublic *pp = (FAreaPublic *) F_Get(_parent, FA_Area_PublicData);

        if (pp)
        {
            //F_Log(0,"set parent damage");

            pp->Flags |= FF_Area_Damaged;
        }

        if (Obj == (FObject) F_Get(_render -> Window,FA_Window_ActiveObject))
        {
           F_Set(_render -> Window,FA_Window_ActiveObject,NULL);
        }

        if (LOD -> Handler)
        {
           area_try_rem_handler(Class,Obj,FeelinBase);
        }
    }

    _x = _y = _w = _h = 0;
    
    _flags &= ~FF_Area_CanDraw;

    return TRUE;
}
//+
///Area_AskMinMax
F_METHOD(void,Area_AskMinMax)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   uint16 i;
   uint32 aw,ah;

///DB_ASKMINMAX
#ifdef DB_ASKMINMAX
   F_Log(0,"LAD_1> MIN %ld x %ld - MAX %ld x %ld",_minw,_minh,_maxw,_maxh);
#endif
//+

   aw = MAX(LOD -> FramePublic -> Border[0].l +
            LOD -> FramePublic -> Border[0].r +
            LOD -> FramePublic -> Padding[0].l +
            LOD -> FramePublic -> Padding[0].r,

            LOD -> FramePublic -> Border[1].l +
            LOD -> FramePublic -> Border[1].r +
            LOD -> FramePublic -> Padding[1].l +
            LOD -> FramePublic -> Padding[1].r);

   ah = MAX(LOD -> FramePublic -> Border[0].t +
            LOD -> FramePublic -> Border[0].b +
            LOD -> FramePublic -> Padding[0].t +
            LOD -> FramePublic -> Padding[0].b,

            LOD -> FramePublic -> Border[1].t +
            LOD -> FramePublic -> Border[1].b +
            LOD -> FramePublic -> Padding[1].t +
            LOD -> FramePublic -> Padding[1].b);
             
   _minw += aw; _maxw += aw;
   _minh += ah; _maxh += ah;

///DB_ASKMINMAX
#ifdef DB_ASKMINMAX
   F_Log(0,"LAD_2> MIN %ld x %ld - MAX %ld x %ld - USER %ld x %ld - %ld x %ld",_minw,_minh,_maxw,_maxh,LOD -> UserMinMax.MinW,LOD -> UserMinMax.MinH,LOD -> UserMinMax.MaxW,LOD -> UserMinMax.MaxH);
#endif
//+

   if ((i = LOD -> UserMinMax.MinW) != 0xFFFF) _minw = aw + MAX(i,_minw);
   if ((i = LOD -> UserMinMax.MinH) != 0xFFFF) _minh = ah + MAX(i,_minh);
   if ((i = LOD -> UserMinMax.MaxW) != 0xFFFF) _maxw = aw + MIN(i,_maxw);
   if ((i = LOD -> UserMinMax.MaxH) != 0xFFFF) _maxh = ah + MIN(i,_maxh);

///DB_ASKMINMAX
#ifdef DB_ASKMINMAX
   F_Log(0,"LAD_3> MIN %ld x %ld - MAX %ld x %ld",_minw,_minh,_maxw,_maxh);
#endif
//+

   if (_maxw < _minw)     _maxw = _minw;
   if (_maxh < _minh)     _maxh = _minh;
   if (_maxw > FV_MAXMAX) _maxw = FV_MAXMAX;
   if (_maxh > FV_MAXMAX) _maxh = FV_MAXMAX;

///DB_ASKMINMAX
#ifdef DB_ASKMINMAX
   F_Log(0,"LAD_4> MIN %ld x %ld - MAX %ld x %ld",_minw,_minh,_maxw,_maxh);//,(FF_Area_SetMaxW & _flags) ? "set" : "free",(FF_Area_SetMaxH & _flags) ? "set" : "free");
#endif
//+

   if ((FF_Area_SetMaxW & _flags) && (_maxw == FV_MAXMAX)) _maxw = _minw;
   if ((FF_Area_SetMaxH & _flags) && (_maxh == FV_MAXMAX)) _maxh = _minh;

///DB_ASKMINMAX
#ifdef DB_ASKMINMAX
   F_Log(0,"LAD_5> MIN %ld, %ld - MAX %ld",_minw,_minh,_maxw,_maxh);
#endif
//+
}
//+

#if F_CODE_DEPRECATED
///Area_RethinkLayout
F_METHODM(uint32,Area_RethinkLayout,FS_RethinkLayout)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   switch (Msg -> Operation)
   {
      case FV_RethinkLayout_Save:
      {
         CopyMem(&_box,&LOD -> RethinkBox,sizeof (FBox));
      }
      break;

      case FV_RethinkLayout_Compare:
      {
         if (_x != LOD -> RethinkBox.x || _y != LOD -> RethinkBox.y ||
             _w != LOD -> RethinkBox.w || _h != LOD -> RethinkBox.h)
         {
//            F_Log(0,"modified coordinates >> redraw");
//            F_Draw(Obj,FF_Draw_Object);

            return FF_RethinkLayout_Modified;
         }
      }
      break;
   }
   return 0;
}
//+
#endif

///Area_GoActive
F_METHOD(void,Area_GoActive)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort  *rp;

    LOD -> Flags |= FF_AREA_HANDLEACTIVE;

    if ((rp = _rp) != NULL)
    {
        uint32 x1 = _x - 1, x2 = x1 + _w + 1;
        uint32 y1 = _y - 1, y2 = y1 + _h + 1;
        uint16 oldpt = rp -> LinePtrn;

        //F_Log(0,"palette 0x%08lx pens 0x%08lx",_palette, _pens);
        SetABPenDrMd(rp,_pens[FV_Pen_Shine],_pens[FV_Pen_Dark],JAM2);

        SetDrPt(rp,0xCCCC);

//         _APen(LOD -> Pens[FV_Pen_Highlight]);
        _Move(x1,y1); _Draw(x2,y1); _Draw(x2,y2); _Draw(x1,y2); _Draw(x1,y1);

        SetDrPt(rp,oldpt);
    }
}
//+
///Area_GoInactive
F_METHOD(void,Area_GoInactive)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   LOD -> Flags &= ~FF_AREA_HANDLEACTIVE;

   if (_render)
   {
      struct Region *region;
      uint32          x1 = _x - 1, x2 = x1 + _w + 1,
                     y1 = _y - 1, y2 = y1 + _h + 1;

      if ((region = NewRegion()) != NULL)
      {
         FRect rect;

         rect.x1 = x1; rect.x2 = x2;
         rect.y1 = y1; rect.y2 = y2; OrRectRegion(region,(struct Rectangle *) &rect);

         rect.x1 += 1; rect.x2 -= 1;
         rect.y1 += 1; rect.y2 -= 1; ClearRectRegion(region,(struct Rectangle *) &rect);

         F_Do(_parent,FM_Erase,region,FF_Erase_Region);

         DisposeRegion(region);
      }
      else
      {
         F_Erase(_parent,x1,y1,x2,y1,0);
         F_Erase(_parent,x2,y1,x2,y2,0);
         F_Erase(_parent,x1,y2,x2,y2,0);
         F_Erase(_parent,x1,y1,x1,y2,0);
      }
   }
}
//+
///Area_GoEnabled
F_METHOD(uint32,Area_GoEnabled)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Scheme)
    {
        #if F_CODE_DEPRECATED
        _pens = LOD -> Scheme -> Pens;
        #else
        _palette = LOD->Scheme;
        #endif
    }
    
    _flags &= ~FF_Area_Disabled;
    _flags |= FF_Area_Damaged;

    if (LOD -> Handler)
    {
        area_try_add_handler(Class,Obj,FeelinBase);
    }

    return TRUE;
}
//+
///Area_GoDisabled
F_METHOD(uint32,Area_GoDisabled)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (LOD -> DisabledScheme)
    {
        #if F_CODE_DEPRECATED
        _pens = LOD -> DisabledScheme -> Pens;
        #else
        _palette = LOD->DisabledScheme;
        #endif
    }
    
    _flags |= (FF_Area_Disabled | FF_Area_Damaged);

    if (LOD -> Handler)
    {
        area_try_rem_handler(Class,Obj,FeelinBase);
    }
    
    return TRUE;
}
//+

///Area_BuildContextMenu
F_METHODM(APTR,Area_BuildContextMenu,FS_BuildContextMenu)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> ContextMenu)
   {
      if (Msg -> MouseX >= _x && Msg -> MouseX <= _x2 &&
          Msg -> MouseY >= _y && Msg -> MouseY <= _y2)
      {
         Msg -> Menu = LOD -> ContextMenu;
         Msg -> ContextOwner = Obj;

         return Msg;
      }
   }
   return NULL;
}
//+
///Area_BuildContextHelp
F_METHODM(STRPTR,Area_BuildContextHelp,FS_BuildContextHelp)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> ContextHelp && (FF_Area_CanShow & _flags))
   {
      if (Msg -> MouseX >= _x && Msg -> MouseX <= _x2 &&
          Msg -> MouseY >= _y && Msg -> MouseY <= _y2)
      {
         return LOD -> ContextHelp;
      }
   }
   return NULL;
}
//+

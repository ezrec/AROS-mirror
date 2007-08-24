#include "Private.h"

#define COMPATIBILITY

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Preview_New
F_METHOD_NEW(Preview_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    if (F_SUPERDO())
    {
        LOD -> preview = ImageDisplayObject,
        
            FA_ImageDisplay_Spec, "fill",
            FA_ImageDisplay_Origin, &LOD->preview_box,
           
            End;
   
        if (LOD -> preview)
        {
           return Obj;
        }
    }
    return NULL;
}
//+
///Preview_Dispose
F_METHOD_DISPOSE(Preview_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_DisposeObj(LOD -> preview); LOD -> preview = NULL;

   F_SUPERDO();
}
//+

///Preview_Setup
F_METHOD_SETUP(Preview_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
      uint32 spec = F_Get(Obj,F_IDR(FA_Preview_Spec));

      _render -> Palette = (struct FeelinPalette *) F_Get(Obj,FA_ColorScheme);

      spec = F_Do(_app,FM_Application_Resolve,spec,"shine;dark");

//      F_Log(0,"Spec '%s' -> 0x%08lx '%s'",LOD -> image_spec,spec,spec);

      F_Set(LOD -> preview,FA_ImageDisplay_Spec,spec);

      F_DoA(LOD -> preview,FM_ImageDisplay_Setup,Msg);
      
      return TRUE;
   }
   return FALSE;
}
//+
///Preview_Cleanup
F_METHOD(uint32,Preview_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (_render)
   {
      F_Do(LOD -> preview,FM_ImageDisplay_Cleanup,_render);
   }
   return F_SUPERDO();
}
//+
///Preview_Draw
F_METHODM(void,Preview_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    STRPTR spec = (STRPTR) F_Get(Obj,F_IDR(FA_Preview_Spec));

    F_SUPERDO();

    if (spec)
    {
        FRect r;
         
        r.x1 = _ix ; r.x2 = r.x1 + _iw - 1;
        r.y1 = _iy ; r.y2 = r.y1 + _ih - 1;

        LOD->preview_box.x = _ix;
        LOD->preview_box.y = _iy;
        LOD->preview_box.w = _iw;
        LOD->preview_box.h = _ih;
   
        if (F_Get(LOD -> preview, FA_ImageDisplay_Mask))
        {
           F_Do(Obj, FM_Erase, &r, 0);
        }
   
        F_Set(LOD -> preview,FA_ImageDisplay_State,(FF_Area_Selected & _flags) ? FV_Image_Selected : FV_Image_Normal);

        F_Do(LOD -> preview,FM_ImageDisplay_Draw,_render,&r,0);
        
        if (FF_Area_Disabled & _flags)
        {
           struct RastPort *rp = _rp;
           STATIC uint16 pattern[] = {0xAAAA,0x5555,0xAAAA,0x5555};
           uint16 *prev_ptrn = rp -> AreaPtrn;
           uint32 prev_ptsz = rp -> AreaPtSz;

           rp -> AreaPtrn = pattern;
           rp -> AreaPtSz = 2;

           SetDrMd(rp,JAM1);
   
           _FPen(FV_Pen_Fill);
           _BPen(0);
           _Boxf(r.x1,r.y1,r.x2,r.y2);

           rp -> AreaPtSz = prev_ptsz;
           rp -> AreaPtrn = prev_ptrn;
        }
    }
}
//+

///Preview_Query
F_METHODM(uint32,Preview_Query,FS_Preview_Query)
{
   if (Msg -> Spec)
   {
      int32 val;
 
      if (*Msg -> Spec == '<')
      {
         if (F_StrCmp("image ",Msg -> Spec + 1,6) == 0) return TRUE;
      }
      else if (*Msg -> Spec == '#')
      {
         return TRUE;
      }
      else if (stcd_l(Msg -> Spec,&val))
      {
         return TRUE;
      }
      else if (*Msg -> Spec >= 'd' && *Msg -> Spec <= 't')
      {
         return TRUE;
      }
#ifdef COMPATIBILITY
      else if (Msg -> Spec[1] == ':')
      {
         switch (*Msg -> Spec)
         {
            case 'r': case 's': case 'p': case 'c':
            case 'g': case 'B': case 'P':
               
            return TRUE;
         }
      }
#endif
   }
   return FALSE;
}
//+
///Preview_ToString
F_METHODM(STRPTR,Preview_ToString,FS_Preview_ToString)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
   STRPTR spec = (STRPTR) F_SUPERDO();

   if (_render)
   {
      _render -> Palette = (FPalette *) F_Get(Obj,FA_ColorScheme);

      F_Set(LOD -> preview,FA_ImageDisplay_Spec,(uint32)(spec));

      F_Draw(Obj,(spec) ? FF_Draw_Update : FF_Draw_Object);
   }
   return spec;
}
//+


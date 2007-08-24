#include "Private.h"

#define COMPATIBILITY
 
/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Preview_New
F_METHOD(uint32,Preview_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_AREA_SAVE_PUBLIC;
   
   LOD -> hatching = TRUE;

   return F_SuperDo(Class,Obj,Method,
   
      FA_Frame, NULL,
      
   TAG_MORE,Msg);
}
//+
///Preview_Dispose
F_METHOD(void,Preview_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_DisposeObj(LOD -> Frame); LOD -> Frame = NULL;

   F_SUPERDO();
}
//+
///Preview_Set
F_METHOD(void,Preview_Set)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_PreviewFrame_Hatching:   LOD -> hatching = item.ti_Data; break;
   }

   F_SUPERDO();
}
//+

///Preview_Setup
F_METHOD(int32,Preview_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
      F_Do(LOD -> Frame,FM_Frame_Setup,_render);

      return TRUE;
   }
   return FALSE;
}
//+
///Preview_Cleanup
F_METHOD(void,Preview_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (_render)
   {
      F_Do(LOD -> Frame,FM_Frame_Cleanup,_render);
   }
   F_SUPERDO();
}
//+
///Preview_Draw
F_METHODM(void,Preview_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    int16 x1 = _ix, x2 = x1 + _iw - 1;
    int16 y1 = _iy, y2 = y1 + _ih - 1;
    
    F_SUPERDO();

    if (LOD -> Frame)
    {
        FBox box;
        uint8 i=0;

        if (F_Get(Obj,F_IDR(FA_Preview_Couple)) && F_Get(Obj,FA_Selected))
        {
            i = 1;
        }

        box.x = x1; box.w = x2 - x1 + 1;
        box.y = y1; box.h = y2 - y1 + 1;

        x1 += LOD -> FramePublic -> Border[i].l;
        y1 += LOD -> FramePublic -> Border[i].t;
        x2 -= LOD -> FramePublic -> Border[i].r;
        y2 -= LOD -> FramePublic -> Border[i].b;

        F_Erase(Obj,x1,y1,x2,y2,0);
        
        F_Do(LOD -> Frame, FM_Frame_Draw, _render, &box, (i) ? FF_Frame_Draw_Select : 0);
   
        x1 += LOD -> FramePublic -> Padding[i].l;
        y1 += LOD -> FramePublic -> Padding[i].t;
        x2 -= LOD -> FramePublic -> Padding[i].r;
        y2 -= LOD -> FramePublic -> Padding[i].b;
        
        if (LOD -> hatching && x2 >= x1 && y2 >= y1)
        {
            struct RastPort *rp = _rp;
            static uint16 pattern[] = { 0x8888,0x4444,0x2222,0x1111 };

            uint16 *prv_ptrn = rp -> AreaPtrn;
            uint32 prv_ptsz = rp -> AreaPtSz;

            rp -> AreaPtrn = pattern;
            rp -> AreaPtSz = 2;

            SetABPenDrMd(rp,_pens[FV_Pen_HalfDark],0,JAM1);

            _Boxf(x1,y1,x2,y2);

            rp -> AreaPtSz = prv_ptsz;
            rp -> AreaPtrn = prv_ptrn;
        }
    }
}

#if 0
//+
/*
///Preview_DnDQuery
F_METHOD_DNDQUERY(Preview_DnDQuery)
{
   if (F_Get(Msg -> Source,FA_Frame_Data))
   {
      return (FObject) F_SuperDoA(Class -> Super,Obj,Method,Msg);
   }
   else return (FObject) F_SUPERDO();
}
//+
///Preview_DnDDrop
F_METHODM(void,Preview_DnDDrop,FS_DnDDrop)
{
   F_Log(0,"disabled");
/*
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   STRPTR spec;
   FFramePublic *frame;

   if ((spec = (STRPTR) F_Get(Msg -> Source,F_IDR(FA_Preview_Spec))) != NULL)
   {
      F_Set(Obj,F_IDR(FA_Preview_Spec),(uint32)(spec));
   }
   else if ((frame = (FFramePublic *) F_Get(Msg -> Source,FA_Frame_Data)) != NULL)
   {
      if (LOD -> FramePublic)
      {
         spec = F_StrNew(NULL,"%02lx.%08lx%lc%02lx.%08lx",frame -> ID[0],*((uint32 *)(&LOD -> FramePublic -> Padding[0])),FV_Frame_Separator,frame -> ID[1],*((uint32 *)(&LOD -> FramePublic -> Padding[1])));
      }
      else
      {
         spec = F_StrNew(NULL,"%02lx.%08lx%lc%02lx.%08lx",frame -> ID[0],*((uint32 *)(&frame -> Padding[0])),FV_Frame_Separator,frame -> ID[1],*((uint32 *)(&frame -> Padding[1])));
      }
   
      F_Set(Obj,F_IDR(FA_Preview_Spec),(uint32)(spec));

      F_Dispose(spec);
   }
*/
}
//+
*/
#endif

///Preview_Query
F_METHODM(uint32,Preview_Query,FS_Preview_Query)
{
   if (Msg -> Spec)
   {
      if (F_StrCmp("<frame ",Msg -> Spec,7) == 0)
      {
         return TRUE;
      }
#ifdef COMPATIBILITY
      else
      {
         int32 val;
         uint32 len = stch_l(Msg -> Spec,&val);

         if (len)
         {
            if (Msg -> Spec[len] == '.') return TRUE;
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
      F_Do(LOD -> Frame,FM_Frame_Cleanup,_render);
   }

   F_DisposeObj(LOD -> Frame); LOD -> Frame = NULL; LOD -> FramePublic = NULL;

   if (spec)
   {
      LOD -> Frame = F_NewObj(FC_Frame,FA_Frame,spec,TAG_DONE);
      LOD -> FramePublic = (FFramePublic *) F_Get(LOD -> Frame, FA_Frame_PublicData);
   }

   if (_render)
   {
      F_Do(LOD -> Frame,FM_Frame_Setup,_render);
   }

   F_Draw(Obj,(spec) ? FF_Draw_Update : FF_Draw_Object);
   
   return spec;
}
//+

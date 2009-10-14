#include "Private.h"

typedef uint32                                  FImage;

///id_raster_create
STATIC F_CODE_CREATE_XML(id_raster_create)
{
   uint32 id = FI_Fill;

   for ( ; Attribute ; Attribute = Attribute -> Next)
   {
      switch (Attribute -> Name -> ID)
      {
         case FV_XML_ID_ID: id = F_Do(CUD -> XMLDocument,F_ID(CUD -> IDs,FM_Document_Resolve),Attribute -> Data,FV_TYPE_INTEGER,NULL,NULL); break;
      }
   }

   return (APTR) MIN(FI_Shine_Highlight,id);
}
//+
///id_raster_draw
STATIC F_CODE_DRAW(id_raster_draw)
{
   struct RastPort *rp = Msg -> Render -> RPort;
   uint32 Raster = (uint32)(image);
   uint32 a = FV_Pen_Fill;
   uint32 b = FV_Pen_Fill;

   switch (Raster)
   {
      case FI_Shine:            a = FV_Pen_Shine;                          break; case FI_HalfShine:            a = FV_Pen_HalfShine;                          break; case FI_Fill:             a = FV_Pen_Fill;                          break; case FI_HalfShadow:           a = FV_Pen_HalfShadow;                       break; case FI_Shadow:           a = FV_Pen_Shadow;                       break; case FI_HalfDark:           a = FV_Pen_HalfDark;                       break; case FI_Dark:           a = FV_Pen_Dark;                       break; case FI_Highlight: a = FV_Pen_Highlight; break;
      case FI_Shine_HalfShine:  a = FV_Pen_Shine; b = FV_Pen_HalfShine;    break; case FI_HalfShine_Fill:       a = FV_Pen_HalfShine; b = FV_Pen_Fill;         break; case FI_Fill_HalfShadow:  a = FV_Pen_Fill; b = FV_Pen_HalfShadow;   break; case FI_HalfShadow_Shadow:    a = FV_Pen_HalfShadow; b = FV_Pen_Shadow;    break; case FI_Shadow_HalfDark:  a = FV_Pen_Shadow; b = FV_Pen_HalfDark;  break; case FI_HalfDark_Dark:      a = FV_Pen_HalfDark; b = FV_Pen_Dark;      break; case FI_Dark_Highlight: a = FV_Pen_Dark; b = FV_Pen_Highlight; break;
      case FI_Shine_Fill:       a = FV_Pen_Shine; b = FV_Pen_Fill;         break; case FI_HalfShine_HalfShadow: a = FV_Pen_HalfShine; b = FV_Pen_HalfShadow;   break; case FI_Fill_Shadow:      a = FV_Pen_Fill; b = FV_Pen_Shadow;       break; case FI_HalfShadow_HalfDark:  a = FV_Pen_HalfShadow; b = FV_Pen_HalfDark;  break; case FI_Shadow_Dark:      a = FV_Pen_Shadow; b = FV_Pen_Dark;      break; case FI_HalfDark_Highlight: a = FV_Pen_HalfDark; b = FV_Pen_Highlight; break;
      case FI_Shine_HalfShadow: a = FV_Pen_Shine; b = FV_Pen_HalfShadow;   break; case FI_HalfShine_Shadow:     a = FV_Pen_HalfShine; b = FV_Pen_Shadow;       break; case FI_Fill_HalfDark:    a = FV_Pen_Fill; b = FV_Pen_HalfDark;     break; case FI_HalfShadow_Dark:      a = FV_Pen_HalfShadow; b = FV_Pen_Dark;      break; case FI_Shadow_Highlight: a = FV_Pen_Shadow; b = FV_Pen_Highlight; break;
      case FI_Shine_Shadow:     a = FV_Pen_Shine; b = FV_Pen_Shadow;       break; case FI_HalfShine_HalfDark:   a = FV_Pen_HalfShine; b = FV_Pen_HalfDark;     break; case FI_Fill_Dark:        a = FV_Pen_Fill; b = FV_Pen_Dark;         break; case FI_HalfShadow_Highlight: a = FV_Pen_HalfShadow; b = FV_Pen_Highlight; break;
      case FI_Shine_HalfDark:   a = FV_Pen_Shine; b = FV_Pen_HalfDark;     break; case FI_HalfShine_Dark:       a = FV_Pen_HalfShine; b = FV_Pen_Dark;         break; case FI_Fill_Highlight:   a = FV_Pen_Fill; b = FV_Pen_Highlight;    break;
      case FI_Shine_Dark:       a = FV_Pen_Shine; b = FV_Pen_Dark;         break; case FI_HalfShine_Highlight:  a = FV_Pen_HalfShine; b = FV_Pen_Highlight;    break;
      case FI_Shine_Highlight:  a = FV_Pen_Shine; b = FV_Pen_Highlight;    break;
   }

   if (Raster >= FI_Shine && Raster <= FI_Highlight)
   {
      _APen(Msg -> Render -> Palette -> Pens[a]);
      _Boxf(Msg -> Rect -> x1,Msg -> Rect -> y1,Msg -> Rect -> x2,Msg -> Rect -> y2);
   }
   else if (Raster >= FI_Shine_HalfShine && Raster <= FI_Shine_Highlight)
   {
      static uint16 pattern_Mixed[] = {0xAAAA,0x5555,0xAAAA,0x5555};
      const uint16 *prev_ptrn = rp -> AreaPtrn;
      uint32 prev_ptsz = rp -> AreaPtSz;

      rp -> AreaPtrn = pattern_Mixed;
      rp -> AreaPtSz = 2;

      SetABPenDrMd(rp,Msg -> Render -> Palette -> Pens[a],Msg -> Render -> Palette -> Pens[b],JAM2);

      _Boxf(Msg -> Rect -> x1,Msg -> Rect -> y1,Msg -> Rect -> x2,Msg -> Rect -> y2);

      rp -> AreaPtSz = prev_ptsz;
      rp -> AreaPtrn = prev_ptrn;
   }

   return TRUE;
}
//+

struct in_CodeTable id_raster_table =
{
   (in_Code_Create *)   &id_raster_create,
   (in_Code_Delete *)   NULL,
   (in_Code_Setup *)    NULL,
   (in_Code_Cleanup *)  NULL,
   (in_Code_Get *)      NULL,
   (in_Code_Draw *)     &id_raster_draw
};

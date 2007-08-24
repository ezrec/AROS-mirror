#include "Private.h"
/*
///DrRect
STATIC void DrRect(FRender *Render,FRect *Rect,uint32 Flags)
{
   uint32 Pen = 3;//((uint32)(Rect) + (uint32)(Render)) % 128;
 
   if (FF_Erase_Region & Flags)
   {
      struct RegionRectangle *rr;

      for (rr = ((struct Region *)(Rect)) -> RegionRectangle ; rr ; rr = rr -> Next)
      {
         if (rr -> bounds.MinX <= rr -> bounds.MaxX && rr -> bounds.MinY <= rr -> bounds.MaxY)
         {
            SetAPen(Render -> RPort,Pen);
            RectFill(Render -> RPort,((struct Region *)(Rect)) -> bounds.MinX + rr -> bounds.MinX,
                                     ((struct Region *)(Rect)) -> bounds.MinY + rr -> bounds.MinY,
                                     ((struct Region *)(Rect)) -> bounds.MinX + rr -> bounds.MaxX,
                                     ((struct Region *)(Rect)) -> bounds.MinY + rr -> bounds.MaxY);
         }
         else F_Log(0,"bad coordinates (%ld > %ld) (%ld > %ld)",rr -> bounds.MinX,rr -> bounds.MaxX,rr -> bounds.MinY,rr -> bounds.MaxY);
      }
   }
   else
   {
      SetAPen(Render -> RPort,Pen);
      RectFill(Render -> RPort,Rect -> x1,Rect -> y1,Rect -> x2,Rect -> y2);
   }
}
//+
*/
#define INLINEDRAW
///
#ifndef INLINEDRAW
void InternDraw(uint32 id,uint32 x1,uint32 y1,uint32 x2,uint32 y2,struct FeelinRender *Render);
#else

#include <stdarg.h>

#define DB_FILL

#define NOPEN -1

#define _Line(x1,y1,x2,y2)    Move(rp,x1,y1); Draw(rp,x2,y2)
#define _Bevel(x1,y1,x2,y2,p1,p2,p3)    Frame_Bevel(rp,x1,y1,x2,y2,p1,p2,p3)
#define _Box(x1,y1,x2,y2)               Frame_Box(rp,x1,y1,x2,y2)

///DrLine
SAVEDS void DrLine(struct RastPort *rp,uint32 Pen,uint32 Count,...)
{
   uint32 x,y;

   va_list ap;
   va_start(ap,Count);
   
   if (Count <= 2) return;

   _APen(Pen);

   x = va_arg(ap,uint32);
   y = va_arg(ap,uint32);   
   _Move(x, y);

   while (--Count > 0)
   {
      x = va_arg(ap,uint32);
      y = va_arg(ap,uint32);   
      _Draw(x, y);
   }
}
//+

///Frame_Bevel
SAVEDS void Frame_Bevel(struct RastPort *rp,uint32 x1,uint32 y1,uint32 x2,uint32 y2,uint32 p1,uint32 p2,uint32 p3)
{
   if (p3 != NOPEN)
   {
      _APen(p3); _Plot(x1,y2); _Plot(x2,y1);
   }
   _APen(p1); _Move(x1,y2-1); _Draw(x1,y1); _Draw(x2-1,y1); _APen(p2); _Move(x1+1,y2); _Draw(x2,y2); _Draw(x2,y1+1);
}
//+
///Frame_Box
SAVEDS void Frame_Box(struct RastPort *rp,uint32 x1,uint32 y1,uint32 x2,uint32 y2)
{
   _Move(x1,y2) ; _Draw(x1,y1) ; _Draw(x2,y1) ; _Draw(x2,y2) ; _Draw(x1,y2);
}
//+
///Frame_Cadre
void Frame_Cadre(struct RastPort *rp,uint32 x1,uint32 y1,uint32 x2,uint32 y2,uint32 p1,uint32 p2)
{
   _APen(p1) ; _Move(x1,y2)     ; _Draw(x1,y1)     ; _Draw(x2,y1);
             ; _Move(x1+1,y2-1) ; _Draw(x2-1,y2-1) ; _Draw(x2-1,y1+1);
   _APen(p2) ; _Box(x1+1,y1+1,x2,y2);
}
//+
//+
#endif

/*** Methods ***************************************************************/

///Frame_Draw
F_METHODM(void,Frame_Draw,FS_Frame_Draw)
{
   struct LocalObjectData *LOD    = F_LOD(Class,Obj);
   
   struct RastPort *rp = Msg -> Render -> RPort;
   
   uint32 x1 = Msg -> Box -> x, x2 = x1 + Msg -> Box -> w - 1;
   uint32 y1 = Msg -> Box -> y, y2 = y1 + Msg -> Box -> h - 1;
   
   FRect p,r,tr; // Text Rectangle
   
   uint8                         f = (FF_Frame_Draw_Select & Msg -> Flags) ? 1 : 0;
   FInner                       *brd = &LOD -> Public.Border[f];
   struct Region                *clear_region=NULL;
   struct Region                *title_region=NULL;
   APTR                          clip=NULL;

   FFrameTitle *t_data = LOD -> TitleData;

   /****/
   
   if (!rp || x2 < x1 || y2 < y1) return;
   
   /* this region is used to collect rectangles to erase */

   if (!(FF_Frame_Draw_Only & Msg -> Flags))
   {
      clear_region = NewRegion();
   }
 
   if (t_data && ((int32)(x2 - x1 + 1 - 20 - brd -> l - brd -> r) > 0))
   {
      uint32 tw;
      
      tr.y1 = (t_data -> Down) ? y2 - t_data -> Height + 1 : y1;
      tr.y2 = (t_data -> Down) ? y2 : y1 + t_data -> Height - 1;

      if (t_data -> Width > (x2 - x1 + 1 - 20 - brd -> l - brd -> r))
      {
         tr.x1 = x1 + 10 + brd -> l;
         tr.x2 = x2 - 10 - brd -> r;
         
         F_Do(t_data -> TD,FM_Set,

            FA_TextDisplay_Width,   tr.x2 - tr.x1 + 1,
            FA_TextDisplay_Height,  tr.y2 - tr.y1 + 1,

         TAG_DONE);

         tw = F_Get(t_data -> TD,FA_TextDisplay_Width);
      }
      else
      {
         tw = t_data -> Width;
      }
   
      if (tw)
      {
         uint32 y;
         
         switch (t_data -> Position)
         {
            case FV_Position_Center:   tr.x1 = (x2 - x1 + 1 - tw) / 2 + x1 - 1; break;
            case FV_Position_Right:    tr.x1 = x2 - brd -> r - 10 - tw; break;
            case FV_Position_Left:
            default:                   tr.x1 = x1 + brd -> l + 10; break;
         }

         tr.x2 = tr.x1 + tw;
         
         /* clear the upper or lower backgroud of the text */

         p.x1 = x1;
         p.x2 = x2;
 
         if (t_data -> Down)
         {
            p.y2 = y2;
            y = y2 -= (t_data -> Height - t_data -> FrameBorderH[f]) / 2;
            p.y1 = y + 1;
         }
         else
         {
            p.y1 = y1;
            y = y1 += (t_data -> Height - t_data -> FrameBorderH[f]) / 2;
            p.y2 = y - 1;
         }

         if (t_data -> Down)
         {
            if (clear_region)
            {
               r.x1 = tr.x1 - 5; r.y1 = y - t_data -> FrameBorderH[f] + 1;
               r.x2 = tr.x2 + 5; r.y2 = y;
               
               OrRectRegion(clear_region,(struct Rectangle *) &r);
               
               r.x1 = x1 + brd -> l; r.y1 = Msg -> Box -> y + Msg -> Box -> h - 1 - brd -> b;
               r.x2 = x2 - brd -> r; r.y2 = y - t_data -> FrameBorderH[f];
               
               OrRectRegion(clear_region,(struct Rectangle *) &r);
            }
         }
         else
         {
            if (clear_region)
            {
               r.x1 = tr.x1 - 5; r.y1 = y;
               r.x2 = tr.x2 + 5; r.y2 = y + t_data -> FrameBorderH[f] - 1;
               
               OrRectRegion(clear_region,(struct Rectangle *) &r);

               r.x1 = x1 + brd -> l; r.y1 = y + t_data -> FrameBorderH[f];
               r.x2 = x2 - brd -> r; r.y2 = Msg -> Box -> y + brd -> t - 1;

               OrRectRegion(clear_region,(struct Rectangle *) &r);
            }
         }

         /* clip title region, to prevent frame drawing */

         if ((title_region = NewRegion()) != NULL)
         {
            r.x1 = x1; r.y1 = y1;
            r.x2 = x2; r.y2 = y2;

            OrRectRegion(title_region,(struct Rectangle *) &r);

            r.x1 = tr.x1 - 5; r.y1 = tr.y1;
            r.x2 = tr.x2 + 5; r.y2 = tr.y2;

            ClearRectRegion(title_region,(struct Rectangle *) &r);
         }
      }
      else
      {
         t_data = NULL;
      }
   }

   if (clear_region)
   {
      if (FF_Frame_Draw_Erase & Msg -> Flags)
      {
         r.x1 = Msg -> Box -> x + brd -> l;
         r.y1 = Msg -> Box -> y + brd -> t;
         r.x2 = Msg -> Box -> x + Msg -> Box -> w - 1 - brd -> r;
         r.y2 = Msg -> Box -> y + Msg -> Box -> h - 1 - brd -> b;

         OrRectRegion(clear_region,(struct Rectangle *) &r);
      }
      else
      {
         FInner *spc = &LOD -> Public.Padding[f];
         
         uint16 x1 = Msg -> Box -> x + brd -> l;
         uint16 y1 = Msg -> Box -> y + brd -> t;
         uint16 x2 = Msg -> Box -> x + Msg -> Box -> w - 1 - brd -> r;
         uint16 y2 = Msg -> Box -> y + Msg -> Box -> h - 1 - brd -> b;

         if (spc -> l)
         {
            r.x1 = x1; r.y1 = y1 + spc -> t;
            r.x2 = x1 + spc -> l - 1; r.y2 = y2 - spc -> b;

            OrRectRegion(clear_region,(struct Rectangle *) &r);
         }
         
         if (spc -> r)
         {
            r.x1 = x2 - spc -> r + 1; r.y1 = y1 + spc -> t;
            r.x2 = x2; r.y2 = y2 - spc -> b;

            OrRectRegion(clear_region,(struct Rectangle *) &r);
         }
         
         if (spc -> t)
         {
            r.x1 = x1; r.y1 = y1;
            r.x2 = x2; r.y2 = y1 + spc -> t - 1;

            OrRectRegion(clear_region,(struct Rectangle *) &r);
         }
         
         if (spc -> b)
         {
            r.x1 = x1; r.y1 = y2 - spc -> b + 1;
            r.x2 = x2; r.y2 = y2;

            OrRectRegion(clear_region,(struct Rectangle *) &r);
         }
      }
      F_Do(LOD -> Public.Back,FM_ImageDisplay_Draw,Msg -> Render,clear_region,FF_ImageDisplay_Region);
      
//      F_Log(0,"erase %ld >> BEGIN %ld (%ld : %ld - %ld x %ld) (0x%08lx)",FF_Frame_Draw_Erase & Msg -> Flags,F_Get(Obj,FA_ID),Msg -> Box -> x,Msg -> Box -> y,Msg -> Box -> w,Msg -> Box -> h,t_data);
//      DrRect(Msg -> Render,clear_region,FF_Erase_Region);
//      F_Log(0,"erase %ld >> DONE",FF_Frame_Draw_Erase & Msg -> Flags);

      DisposeRegion(clear_region);
   }

   if (title_region)
   {
      clip = (APTR) F_Do(Msg -> Render,FM_Render_AddClipRegion,title_region);
      DisposeRegion(title_region);
   }
 
/// Drawing Frame
   if (LOD -> ID[f])
   {
      uint32 *pn = Msg -> Render -> Palette -> Pens;
      uint32 shine       = pn[FV_Pen_Shine],
             halfshine   = pn[FV_Pen_HalfShine],
             fill        = pn[FV_Pen_Fill],
             halfshadow  = pn[FV_Pen_HalfShadow],
             shadow      = pn[FV_Pen_Shadow],
             halfdark    = pn[FV_Pen_HalfDark],
             dark        = pn[FV_Pen_Dark];

      switch (LOD -> ID[f])
      {
///01
         case 1:
__01:
            _APen(dark);
__01a:
            _Box(x1,y1,x2,y2);
            break;
//+
///02 >> 01a
         case 2:
            _APen(shine);
            goto __01a;
//+
///03 >> 01a
         case 3:
            _APen(halfdark);
            goto __01a;
//+
///04 >> 01a
         case 4:
            _APen(halfshine);
            goto __01a;
//+
///05
         case 5:
            _APen(halfdark);
__05:
            _Line(x1+1,y1, x2-1,y1);
            _Line(x2,y1+1, x2,y2-1);
            _Line(x2-1,y2, x1+1,y2);
            _Line(x1,y2-1, x1,y1+1);
            _APen(fill);
            _Plot(x1,y1) ; _Plot(x2,y1) ; _Plot(x2,y2) ; _Plot(x1,y2);
            break;
//+
///06 >> 05
         case 6:
            _APen(halfshine);
            goto __05;
//+
///08 -> 07
         case 8:
            shadow = halfshine;
            halfshadow = fill;
//+
///07
         case 7:
         {
            _APen(fill);
            
            _Boxf(x1,y1,x1+1,y1+1);
            _Boxf(x1,y2-1,x1+1,y2);
            _Boxf(x2-1,y1,x2,y1+1);
            _Boxf(x2-1,y2-1,x2,y2);

            _Line(x1+3,y1+1,x2-3,y1+1);
            _Line(x1+3,y2-1,x2-3,y2-1);
            _Line(x1+1,y1+3,x1+1,y2-3);
            _Line(x2-1,y1+3,x2-1,y2-3);
    
   /*
            struct Region *region;
            
            if (region = NewRegion())
            {
               FRect r;
            
               r.x1 = x1; r.y1 = y1; r.x2 = x1+1; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x1; r.y1 = y2-1; r.x2 = x1+1; r.y2 = y2; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y1; r.x2 = x2; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y2-1; r.x2 = x2; r.y2 = y2; OrRectRegion(region,&r);
               
               r.x1 = x1+3; r.y1 = y1+1; r.x2 = x2-3; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x1+3; r.y1 = y2-1; r.x2 = x2-3; r.y2 = y2-1; OrRectRegion(region,&r);
               
               r.x1 = x1+1; r.y1 = y1+3; r.x2 = x1+1; r.y2 = y2-3; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y1+3; r.x2 = x2-1; r.y2 = y2-3; OrRectRegion(region,&r);

               F_Do(Obj,FM_Erase,region,FF_Erase_Region);
            
               DisposeRegion(region);
            }
   */
            _APen(shadow);
            _Line(x1,y1+3, x1,y2-3); _Line(x1+3,y1, x2-3,y1); _Plot(x1+1,y1+1) ; _Plot(x1+1,y1+2); _Plot(x1+2,y1+1); _Plot(x2-1,y1+1) ; _Plot(x2-2,y1+1); _Plot(x2-1,y1+2);
            _Line(x2,y1+3, x2,y2-3); _Line(x1+3,y2, x2-3,y2); _Plot(x1+1,y2-1) ; _Plot(x1+1,y2-2); _Plot(x1+2,y2-1); _Plot(x2-1,y2-1) ; _Plot(x2-2,y2-1); _Plot(x2-1,y2-2);
            _APen(halfshadow);
            _Plot(x1,y1+2); _Plot(x1+2,y1); _Plot(x2-2,y1); _Plot(x2,y1+2);
            _Plot(x1,y2-2); _Plot(x1+2,y2); _Plot(x2-2,y2); _Plot(x2,y2-2);
         }
         break;
//+
///09
         case 9:
            Frame_Cadre(rp,x1,y1,x2,y2,shine,dark);
            break;
//+
///10
         case 10:
            Frame_Cadre(rp,x1,y1,x2,y2,dark,shine);
            break;
//+
///11
         case 11:
            Frame_Cadre(rp,x1,y1,x2,y2,halfshine,halfdark);
            break;
//+
///12
         case 12:
            Frame_Cadre(rp,x1,y1,x2,y2,halfdark,halfshine);
            break;
//+
///13
         case 13:
         {
            _APen(fill);
            _Boxf(x1,y1,x1+1,y1+1);
            _Boxf(x1,y2-1,x1+1,y2);
            _Boxf(x2-1,y1,x2,y1+1);
            _Boxf(x2-1,y2-1,x2,y2);

            _Line(x1+3,y1+1,x2-3,y1+1);
            _Line(x1+3,y2-1,x2-3,y2-1);
            _Line(x1+1,y1+3,x1+1,y2-3);
            _Line(x2-1,y1+3,x2-1,y2-3);

   /*
            struct Region *region;
             
            if (region = NewRegion())
            {
               FRect r;

               r.x1 = x1; r.y1 = y1; r.x2 = x1+1; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x1; r.y1 = y2-1; r.x2 = x1+1; r.y2 = y2; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y1; r.x2 = x2; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y2-1; r.x2 = x2; r.y2 = y2; OrRectRegion(region,&r);

               r.x1 = x1+3; r.y1 = y1+1; r.x2 = x2-3; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x1+3; r.y1 = y2-1; r.x2 = x2-3; r.y2 = y2-1; OrRectRegion(region,&r);

               r.x1 = x1+1; r.y1 = y1+3; r.x2 = x1+1; r.y2 = y2-3; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y1+3; r.x2 = x2-1; r.y2 = y2-3; OrRectRegion(region,&r);

               F_Do(Obj,FM_Erase,region,FF_Erase_Region | FF_Erase_Fill);
   //            DrRect(Msg -> Render,region,FF_Erase_Region);

               DisposeRegion(region);
            }
   */
            _APen(shadow);
            _Line(x1+3,y2, x2-3,y2); _Plot(x1+1,y2-2); _Plot(x1+1,y2-1); _Plot(x1+2,y2-1);
            _Line(x2,y1+3, x2,y2-3); _Plot(x2-2,y2-1); _Plot(x2-1,y2-1); _Plot(x2-1,y2-2); _Plot(x2-2,y1+1); _Plot(x2-1,y1+1); _Plot(x2-1,y1+2);
            _APen(halfshadow);
            _Plot(x1,y2-2); _Plot(x1+2,y2); _Plot(x2-2,y2); _Plot(x2,y2-2); _Plot(x2,y1+2); _Plot(x2-2,y1);
            _APen(fill);
            _Plot(x1,y2-3); _Plot(x2-3,y1);
            _APen(halfshine);
            _Line(x1,y1+2, x1,y2-4); _Line(x1+2,y1, x2-4,y1);
            _APen(shine);
            _Plot(x1+1,y1+1); _Plot(x1+2,y1+1); _Plot(x1+1,y1+2);
         }
         break;
//+
///14
         case 14:
         {
            _APen(fill);
            _Boxf(x1,y1,x1+1,y1+1);
            _Boxf(x1,y2-1,x1+1,y2);
            _Boxf(x2-1,y1,x2,y1+1);
            _Boxf(x2-1,y2-1,x2,y2);
            _Line(x1+3,y1+1,x2-3,y1+1);
            _Line(x1+3,y2-1,x2-3,y2-1);
            _Line(x1+1,y1+3,x1+1,y2-3);
            _Line(x2-1,y1+3,x2-1,y2-3);

   /*
            struct Region *region;
            
            if (region = NewRegion())
            {
               FRect r;

               r.x1 = x1; r.y1 = y1; r.x2 = x1+1; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x1; r.y1 = y2-1; r.x2 = x1+1; r.y2 = y2; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y1; r.x2 = x2; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y2-1; r.x2 = x2; r.y2 = y2; OrRectRegion(region,&r);

               r.x1 = x1+3; r.y1 = y1+1; r.x2 = x2-3; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x1+3; r.y1 = y2-1; r.x2 = x2-3; r.y2 = y2-1; OrRectRegion(region,&r);

               r.x1 = x1+1; r.y1 = y1+3; r.x2 = x1+1; r.y2 = y2-3; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y1+3; r.x2 = x2-1; r.y2 = y2-3; OrRectRegion(region,&r);

               DrRect(Msg -> Render,region,FF_Erase_Region);

               DisposeRegion(region);
            }
   */
            _APen(shadow);
            _Plot(x1+1,y1+1); _Plot(x1+2,y1+1); _Plot(x1+1,y1+2);
            _Plot(x2-1,y1+1); _Plot(x2-2,y1+1); _Plot(x2-1,y1+2);
            _Plot(x1+1,y2-1); _Plot(x1+1,y2-2); _Plot(x1+2,y2-1);
            _Line(x1,y1+3, x1,y2-3);
            _Line(x1+3,y1, x2-3,y1);
            _APen(halfshadow);
            _Plot(x1,y1+2); _Plot(x1+2,y1); _Plot(x1,y2-2); _Plot(x1+2,y2); _Plot(x2,y1+2); _Plot(x2-2,y1);
            _APen(fill);
            _Plot(x1+3,y2); _Plot(x2,y1+3);
            _APen(halfshine);
            _Line(x1+4,y2, x2-2,y2);
            _Line(x2,y1+4, x2,y2-2);
            _APen(shine);
            _Plot(x2-1,y2-1); _Plot(x2-2,y2-1); _Plot(x2-1,y2-2);
         }
         break;
//+
///15
         case 15:
         {
            _APen(fill);
            _Boxf(x1,y1,x1+1,y1+1);
            _Boxf(x1,y2-2,x1+2,y2); _Plot(x1+2,y2-3); _Plot(x1+3,y2-2);
            _Boxf(x2-2,y1,x2,y1+2); _Plot(x2-3,y1+2); _Plot(x2-2,y1+3);
            _Boxf(x2-1,y2-1,x2,y2);
            _Line(x1+4,y1+2,x2-4,y1+2);
            _Line(x1+4,y2-2,x2-4,y2-2);
            _Line(x1+2,y1+4,x1+2,y2-4);
            _Line(x2-2,y1+4,x2-2,y2-4);

   /*
            struct Region *region;
            
            if (region = NewRegion())
            {
               FRect r;

               r.x1 = x1; r.y1 = y1; r.x2 = x1+1; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x1; r.y1 = y2-1; r.x2 = x1+1; r.y2 = y2; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y1; r.x2 = x2; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y2-1; r.x2 = x2; r.y2 = y2; OrRectRegion(region,&r);

               r.x1 = x1+4; r.y1 = y1+2; r.x2 = x2-4; r.y2 = y1+2; OrRectRegion(region,&r);
               r.x1 = x1+4; r.y1 = y2-2; r.x2 = x2-4; r.y2 = y2-2; OrRectRegion(region,&r);

               r.x1 = x1+2; r.y1 = y1+4; r.x2 = x1+2; r.y2 = y2-4; OrRectRegion(region,&r);
               r.x1 = x2-2; r.y1 = y1+4; r.x2 = x2-2; r.y2 = y2-4; OrRectRegion(region,&r);

               DrRect(Msg -> Render,region,FF_Erase_Region);

               DisposeRegion(region);
            }
   */
            _APen(shadow);
            _Line(x1+4,y1+1, x2-3,y1+1); _Draw(x2,y1+4); _Draw(x2,y2-3); _Draw(x2-3,y2); _Draw(x1+4,y2); _Draw(x1+1,y2-3); _Draw(x1+1,y1+4); _Draw(x1+4,y1+1);
            _Plot(x1+2,y1+2); _Plot(x2-1,y1+2); _Plot(x1+2,y2-1); _Plot(x2-1,y2-1);
            _APen(halfshadow);
            _Plot(x1+3,y1+1); _Plot(x1+1,y1+3); _Plot(x2-2,y1+1); _Plot(x2,y1+3);
            _Plot(x1+1,y2-2); _Plot(x1+3,y2);   _Plot(x2-2,y2);   _Plot(x2,y2-2);
            _APen(halfshine);
            _Plot(x1+2,y1); _Plot(x1,y1+2); _Plot(x2-1,y2-3); _Plot(x2-3,y2-1);
            _APen(shine);
            _Line(x1,y2-3, x1,y1+3); _Draw(x1+3,y1); _Draw(x2-3,y1); _Plot(x1+1,y1+1);
            _Line(x1+4,y2-1, x2-4,y2-1); _Draw(x2-1,y2-4); _Draw(x2-1,y1+4); _Plot(x2-2,y2-2);
         }
         break;
//+
///16
         case 16:
         {
            _APen(fill);
            _Boxf(x1,y1,x1+1,y1+1);
            _Boxf(x1,y2-2,x1+2,y2); _Plot(x1+2,y2-3); _Plot(x1+3,y2-2);
            _Boxf(x2-2,y1,x2,y1+2); _Plot(x2-3,y1+2); _Plot(x2-2,y1+3);
            _Boxf(x2-1,y2-1,x2,y2);
            _Line(x1+4,y1+2,x2-4,y1+2);
            _Line(x1+4,y2-2,x2-4,y2-2);
            _Line(x1+2,y1+4,x1+2,y2-4);
            _Line(x2-2,y1+4,x2-2,y2-4);

   /*
            struct Region *region;

            if (region = NewRegion())
            {
               FRect r;

               r.x1 = x1; r.y1 = y1; r.x2 = x1+1; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x1; r.y1 = y2-1; r.x2 = x1+1; r.y2 = y2; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y1; r.x2 = x2; r.y2 = y1+1; OrRectRegion(region,&r);
               r.x1 = x2-1; r.y1 = y2-1; r.x2 = x2; r.y2 = y2; OrRectRegion(region,&r);

               r.x1 = x1+4; r.y1 = y1+2; r.x2 = x2-4; r.y2 = y1+2; OrRectRegion(region,&r);
               r.x1 = x1+4; r.y1 = y2-2; r.x2 = x2-4; r.y2 = y2-2; OrRectRegion(region,&r);

               r.x1 = x1+2; r.y1 = y1+4; r.x2 = x1+2; r.y2 = y2-4; OrRectRegion(region,&r);
               r.x1 = x2-2; r.y1 = y1+4; r.x2 = x2-2; r.y2 = y2-4; OrRectRegion(region,&r);

               DrRect(Msg -> Render,region,FF_Erase_Region);

               DisposeRegion(region);
            }
   */
            _APen(shadow);
            _Line(x1,y2-4, x1,y1+3); _Draw(x1+3,y1); _Draw(x2-4,y1);
            _Line(x1+4,y2-1, x2-4,y2-1); _Draw(x2-1,y2-4); _Draw(x2-1,y1+4);
            _Plot(x1+1,y1+1); _Plot(x2-2,y2-2);
            _APen(halfshadow);
            _Plot(x1,y2-3); _Plot(x1+2,y1); _Plot(x2-3,y1); _Plot(x1,y1+2); _Plot(x2,y2-3); _Plot(x2-1,y2-3); _Plot(x2-3,y2-1);
            _APen(halfshine);
            _Plot(x1+3,y1+1); _Plot(x2-2,y1+1); _Plot(x1+1,y1+3); _Plot(x2,y1+3);
            _Plot(x1+1,y2-2); _Plot(x2,y2-2); _Plot(x1+3,y2); _Plot(x2-2,y2);
            _APen(shine);
            _Line(x1+4,y1+1, x2-3,y1+1); _Draw(x2,y1+4); _Draw(x2,y2-3); _Draw(x2-3,y2); _Draw(x1+4,y2); _Draw(x1+1,y2-3); _Draw(x1+1,y1+4); _Draw(x1+4,y1+1);
            _Plot(x1+2,y1+2); _Plot(x1+2,y2-1); _Plot(x2-1,y1+2); _Plot(x2-1,y2-1);
         }
         break;
//+
///17 >> 23
         case 17:
            _Bevel(x1++,y1++,x2--,y2--,shadow,shine,fill);
            goto __23;
//+
///18
         case 18:
__18:
            _Bevel(x1,y1,x2,y2,shadow,shine,fill);
            _APen(dark);
            _Box(x1+1,y1+1,x2-1,y2-1);
            break;
//+
///19 >> 18
         case 19:
            DrLine(rp,shadow,3,     x1+2,y2-2, x1+2,y1+2, x2-2,y1+2);
            DrLine(rp,halfshadow,3, x1+3,y2-2, x1+3,y1+3, x2-2,y1+3);
            goto __18;
//+
///20 -> 21
         case 20:
            _Bevel(x1+3,y1+3,x2-3,y2-3,shine,shadow,fill);
            _Bevel(x1+4,y1+4,x2-4,y2-4,halfshine,halfshadow,fill);
//+
///21
         case 21:
__21:
            _APen(fill);
            _Boxf(x1,y2-2,x1+2,y2);
            _Boxf(x2-2,y1,x2,y1+2);
            _APen(dark);
            _Box(x1+2,y1+2,x2-2,y2-2);
            _APen(shadow);
            _Line(x1+1,y2-2, x1+1,y1+1); _Draw(x2-2,y1+1);
            _APen(halfshadow);
            _Line(x1,y2-3, x1,y1); _Draw(x2-3,y1);
            _APen(halfshine);
            _Line(x1+3,y2, x2,y2); _Draw(x2,y1+3);
            _APen(shine);
            _Line(x1+2,y2-1, x2-1,y2-1); _Draw(x2-1,y1+2);
            break;
//+
///22 >> 21
         case 22:
            _APen(halfdark);
            _Line(x1+3,y2-3, x1+3,y1+3); _Draw(x2-3,y1+3);
            _APen(shadow);
            _Line(x1+4,y2-3, x1+4,y1+4); _Draw(x2-3,y1+4);
            _APen(halfshadow);
            _Line(x1+5,y2-3, x1+5,y1+5); _Draw(x2-3,y1+5);
            goto __21;
//+
///25 >> 23
         case 25:
            _Bevel(x1+2,y1+2,x2-2,y2-2,halfshine,halfshadow,fill);
//+
///23
         case 23:
__23:
            _Bevel(x1+1,y1+1,x2-1,y2-1,shine,shadow,fill);
            goto __01;
//+
///24
         case 24:
            DrLine(rp,shadow,3,     x1+1,y2-1, x1+1,y1+1, x2-1,y1+1);
            DrLine(rp,halfshadow,3, x1+2,y2-1, x1+2,y1+2, x2-1,y1+2);
            goto __01;
//+
///26
         case 26:
            _APen(halfdark);
            _Line(x1+1,y2-1, x1+1,y1+1); _Draw(x2-1,y1+1);
            _APen(shadow);
            _Line(x1+2,y2-1, x1+2,y1+2); _Draw(x2-1,y1+2);
            _APen(halfshadow);
            _Line(x1+3,y2-1, x1+3,y1+3); _Draw(x2-1,y1+3);
            goto __01;
//+
///28 -> 27
         case 28:
         {
            uint32 temp = shadow; shadow = shine; shine = temp;
         }
//+
///27
         case 27:
            _Bevel(x1+1,y1+1,x2-1,y2-1,shine,shadow,fill);
            _APen(dark);
            goto __05;
//+
///30 -> 29
         case 30:
         {
            uint32 temp = shadow; shadow = shine; shine = temp; temp = halfshadow; halfshadow = halfshine; halfshine = temp;
         }
//+
///29
         case 29:
            _APen(fill);   _Boxf(x1,y1,x1+1,y1+1) ; _Boxf(x2-1,y1,x2,y1+1);
                           _Boxf(x1,y2-1,x1+1,y2) ; _Boxf(x2-1,y2-1,x2,y2);
            _APen(shine);
            _Line(x1+1,y2-2, x1+1,y1+2); _Draw(x1+2,y1+1); _Draw(x2-2,y1+1);
            _APen(shadow);
            _Line(x1+2,y2-1, x2-2,y2-1); _Draw(x2-1,y2-2); _Draw(x2-1,y1+2);
            _Bevel(x1+2,y1+2,x2-2,y2-2,halfshine,halfshadow,fill);
            _APen(shine)      ; _Plot(x1+2,y1+2);
            _APen(shadow)     ; _Plot(x2-2,y2-2);
            DrLine(rp,dark,9,    x1+2,y1, x2-2,y1, x2,y1+2, x2,y2-2, x2-2,y2, x1+2,y2, x1,y2-2, x1,y1+2, x1+2,y1);
            break;
//+
///31
         case 31:
            DrLine(rp,fill,3,    x1+1,y2-2, x1+1,y1+1, x2-2,y1+1);
            _Line(x1,y2-1, x1,y2); _Line(x2-1,y1, x2,y1);
__31a:
            _APen(dark);
            _Line(x1+1,y2, x2,y2); _Draw(x2,y1+1);
            _Bevel(x1,y1,x2-1,y2-1,shine,shadow,NOPEN);
            _APen(fill);   _Plot(x1,y2); _Plot(x1,y2-1); _Plot(x2,y1); _Plot(x2-1,y1);
            break;
//+
///32
         case 32:
            _APen(fill);   _Line(x1+2,y2-1, x2-1,y2-1);  _Draw(x2-1,y1+2);
__32a:
            _APen(dark);   _Line(x1,y2-1, x1,y1);        _Draw(x2-1,y1);
            _APen(shadow); _Line(x1+1,y2-1, x1+1,y1+1);  _Draw(x2-1,y1+1);
            _APen(shine);  _Line(x1+2,y2, x2,y2);        _Draw(x2,y1+2);
            _APen(fill);   _Plot(x1,y2); _Plot(x1+1,y2); _Plot(x2,y1); _Plot(x2,y1+1);
            break;
//+
///33 >> 31a
         case 33:
            DrLine(rp,fill,3,    x1+2,y2-3, x1+2,y1+2, x2-3,y1+2);
            _Bevel(x1+1,y1+1,x2-2,y2-2,halfshine,halfshadow,fill);
            goto __31a;
//+
///34 >> 32a
         case 34:
            DrLine(rp,fill,3,    x1+3,y2-2, x2-2,y2-2, x2-2,y1+3);
            _Bevel(x1+2,y1+2,x2-1,y2-1,halfshadow,halfshine,fill);
            goto __32a;
//+
///35
         case 35:
            _Bevel(x1,y1,x2,y2,shadow,dark,fill);
            _Bevel(x1+1,y1+1,x2-1,y2-1,shine,shadow,fill);
            _Bevel(x1+2,y1+2,x2-2,y2-2,halfshine,halfshadow,fill);
            break;
//+
///36
         case 36:
            _APen(fill);
            _Line(x1+2,y2-1, x2-1,y2-1); _Draw(x2-1,y1+2);
            _APen(halfshine);
            _Line(x1,y2, x1,y1); _Draw(x2,y1);
            _APen(halfdark);
            _Line(x1+1,y2-2, x1+1,y1+1); _Draw(x2-2,y1+1);
            _APen(shadow);
            _Line(x1+2,y2-2, x1+2,y1+2); _Draw(x2-2,y1+2);
            _APen(dark);
            _Line(x1+1,y2, x2,y2); _Draw(x2,y1+1);
            _APen(halfshadow) ; _Plot(x1+1,y2-1) ; _Plot(x2-1,y1+1);
            break;
//+
///37
         case 37:
            dark = halfdark; shine = halfshine; shadow = halfshadow;
            goto __23;
//+
///38
         case 38:
            dark = halfdark; shine = halfshadow; shadow = halfshine;
            goto __23;
//+
///39
         case 39:
            _APen(fill); _Plot(x1+1,y1+2) ; _Plot(x1+2,y1+1); _Plot(x1+1,y2-1) ; _Plot(x2-1,y1+1);
            _APen(dark); _Box(x1,y1,x2,y2);
            DrLine(rp,shine,4,      x1+1,y2-2, x1+1,y1+3, x1+3,y1+1, x2-2,y1+1);
             _Line(x1+2,y1+3, x1+3,y1+2);
            DrLine(rp,shadow,4,     x1+2,y2-1, x2-4,y2-1, x2-1,y2-4, x2-1,y1+2); _Plot(x2-2,y2-2);
            DrLine(rp,halfshine,4,  x1+2,y1+5, x1+2,y1+4, x1+4,y1+2, x1+5,y1+2);
            DrLine(rp,halfshadow,4, x2-5,y2-2, x2-4,y2-2, x2-2,y2-4, x2-2,y2-5);
            DrLine(rp,halfdark,3,   x2-4,y2-1, x2-1,y2-1, x2-1,y2-4);
            _APen(dark); _Plot(x2-1,y2-1);
            _APen(shadow); _Plot(x1+1,y1+1);
            break;
//+
///40
         case 40:
            _APen(halfshadow) ; _Plot(x1+1,y2-1) ; _Plot(x2-1,y1+1);
                              ; _Plot(x1+1,y1+2) ; _Plot(x1+2,y1+1);
            _APen(halfdark)   ; _Box(x1,y1,x2,y2);
            DrLine(rp,shadow,4,     x1+1,y2-2, x1+1,y1+3, x1+3,y1+1, x2-2,y1+1); _Line(x1+2,y1+3, x1+3,y1+2);
            DrLine(rp,fill,4,       x1+2,y2-1, x2-4,y2-1, x2-1,y2-4, x2-1,y1+2); _Plot(x2-2,y2-2);
            DrLine(rp,halfshine,3,  x2-4,y2-1, x2-1,y2-1, x2-1,y2-4); _Plot(x1+1,y1+1);
            _APen(shine)      ; _Plot(x2-1,y2-1);
            break;
//+
///41
         case 41:
            DrLine(rp,fill,3,       x1+1,y2-1, x1+1,y1+1, x2-1,y1+1);
            DrLine(rp,fill,3,       x1+4,y2-3, x2-3,y2-3, x2-3,y1+4);
            DrLine(rp,shine,3,      x1+2,y2-2, x1+2,y1+2, x2-1,y1+2);
            DrLine(rp,halfshine,3,  x1+3,y2-3, x1+3,y1+3, x2-2,y1+3);
            DrLine(rp,halfshadow,3, x1+3,y2-2, x2-2,y2-2, x2-2,y1+4);
            DrLine(rp,shadow,3,     x1+2,y2-1, x2-1,y2-1, x2-1,y1+2); _APen(halfdark);
            goto __05;
//+
///43 -> 42
         case 43:
         {
            uint32 temp;

            temp = shine ; shine = shadow ; shadow = temp;
            temp = halfshine ; halfshine = halfshadow ; halfshadow = temp;
         }
//+
///42
         case 42:
            _Bevel(x1,y1,x2,y2,shine,shadow,fill);
            _Bevel(x1+1,y1+1,x2-1,y2-1,halfshine,halfshadow,fill);
         break;
//+
///44
         case 44:
         {
            uint16 ptrn = rp -> LinePtrn;
            uint32 x3 = (x2 - x1 + 1) / 3 + x1;

            _APen(fill); _Boxf(x1,y2-1,x1+1,y2); _Boxf(x2-1,y1,x2,y1+1);

            DrLine(rp,dark,3,       x1+2,y2, x2,y2, x2,y1+2);

            SetABPenDrMd(rp,dark,halfdark,JAM2);

            SetDrPt(rp,0xAAAA);
            _Line(x1+2,y2-1, x3,y2-1);
            SetDrPt(rp,ptrn);

            SetDrMd(rp,JAM1);

            _APen(halfdark)   ; _Plot(x1+1,y2-1);

            DrLine(rp,halfdark,5,   x1+2,y2-2, x3,y2-2, x3+1,y2-1, x2-1,y2-1, x2-1,y1+1);
            DrLine(rp,halfshine,5,  x1,y2-2, x1,y1, x3,y1, x3+1,y1+1, x2-3,y1+1);
            _APen(shine);
             _Line(x3+1,y1, x2-2,y1);
            DrLine(rp,fill,3,       x1+1,y2-3, x1+1,y1+1, x3,y1+1);  _Plot(x2-2,y1+1);

            _APen(halfshadow) ; _Plot(x1+1,y2-2);
            DrLine(rp,halfdark,5,   x1+2,y2-2, x3,y2-2,   x3+1,y2-1, x2-1,y2-1, x2-1,y1+1);
            DrLine(rp,shadow,3,     x3+1,y2-2, x2-2,y2-2, x2-2,y1+2);

            _APen(halfshadow) ; _Boxf(x1+2,y1+2,x3,y2-3);
            _APen(fill)       ; _Boxf(x3+1,y1+2,x2-3,y2-3);
         }
         break;
//+
///45
         case 45:
         {
            uint32 x3 = (x2 - x1 + 1) / 3 + x1 + 1;
            DrLine(rp,dark,3,       x1,y2,     x1,y1,     x2,y1);
            DrLine(rp,halfdark,3,   x1+1,y2-1, x1+1,y1+1, x3,y1+1);
            DrLine(rp,shadow,3,     x1+2,y2-2, x1+2,y1+2, x3,y1+2);

                                _Line(x3+1,y1+1, x2-1,y1+1);
            _APen(halfshadow) ; _Line(x3+1,y1+2, x2-2,y1+2);   _Plot(x1+2,y2-1);
            _APen(fill)       ; _Line(x1+3,y2-1, x3,y2-1);     _Plot(x2-1,y1+2);
            _APen(halfshine)  ; _Line(x1+1,y2,   x3,y2);
            DrLine(rp,halfshine,3,  x3+1,y2-1, x2-1,y2-1, x2-1,y1+3);
            DrLine(rp,shine,3,      x3+1,y2, x2,y2, x2,y1+1);
            x1++; y1++; x2++; y2++;
            _APen(halfshadow) ; _Boxf(x1+2,y1+2,x3,y2-3);
            _APen(fill)       ; _Boxf(x3+1,y1+2,x2-3,y2-3);
         }
         break;
//+
///46
         case 46:
            _Bevel(x1,y1,x2,y2,shadow,shadow,fill);
            _Bevel(x1+1,y1+1,x2-1,y2-1,shine,halfdark,fill);
         break;
//+
///47
         case 47:
            _Bevel(x1,y1,x2,y2,shadow,shine,fill);
            _APen(halfdark);  _Move(x1+1,y2-1); _Draw(x1+1,y1+1); _Draw(x2-1,y1+1);
            _APen(shadow);    _Move(x1+2,y2-1); _Draw(x2-1,y2-1); _Draw(x2-1,y1+2);
         break;
//+
///48
         case 48:
            _Bevel(x1,y1,x2,y2,shine,shadow,fill);
            _APen(dark); _Box(x1+1,y1+1,x2-1,y2-1);
            _Bevel(x1+2,y1+2,x2-2,y2-2,shine,shadow,fill);
            break;
//+
///49
         case 49:
                              ; _Bevel(x1,y1,x2,y2,shine,shadow,fill);
            _APen(dark)       ; _Box(x1+1,y1+1,x2-1,y2-1);
            DrLine(rp,halfshadow,3, x1+2,y2-3, x1+2,y1+2, x2-3,y1+2);
            DrLine(rp,halfshine,3,  x1+2,y2-2, x2-2,y2-2, x2-2,y1+2);
            break;
//+
         case 50:
         {
            _APen(shadow);       _Line(x1,y2,x1,y1); _Draw(x2,y1);
            _APen(halfshadow);   _Line(x1+1,y2,x1+1,y1+1); _Draw(x2,y1+1);
         }
         break;
         
         case 51:
         {
            _APen(shadow);       _Line(x1,y1,x2,y1);
            _APen(halfshadow);   _Line(x1,y1+1,x2-1,y1+1);
            _APen(shine);        _Line(x2,y1+1,x2,y2);
            _APen(halfshine);    _Line(x2-1,y1+2,x2-1,y2);
         }
         break;
      
         case 52:
         {
            _APen(shine);        _Line(x2,y1,x2,y2); _Draw(x1,y2);
            _APen(halfshine);    _Line(x2-1,y1,x2-1,y2-1); _Draw(x1,y2-1);
         }
         break;
      
         case 53:
         {
            _APen(shine);        _Line(x1+1,y2,x2,y2);
            _APen(halfshine);    _Line(x1+2,y2-1,x2,y2-1);
            _APen(shadow);       _Line(x1,y1,x1,y2);
            _APen(halfshadow);   _Line(x1+1,y1,x1+1,y2-1);
         }
         break;
      }

   }
//+

   if (clip)
   {
      F_Do(Msg -> Render,FM_Render_RemClip,clip);
   }                       

   if (t_data)
   {
      F_Do((FObject) F_Get(Obj,FA_Parent),FM_Erase,&p,FF_Erase_Fill);
      
      F_Do(t_data -> TD,FM_TextDisplay_Draw,&tr);
   }
}
//+

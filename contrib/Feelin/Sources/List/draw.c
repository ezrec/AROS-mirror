#include "Private.h"

#define FF_DrawLine_Erase     (1 << 0)
//#define FF_DrawLine_Highlight (1 << 1) // Disable auto erase active

/*** Private ***************************************************************/

///List_DrawLine
STATIC void List_DrawLine(FClass *Class,FObject Obj,FLine *Line,ULONG Flags)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct RastPort        *rp  = _rp;
   FRect rect;
   WORD j;
   UWORD x;

   if (!Line || !LOD -> Columns) return;

   line_display(Class,Obj,Line);

   rect.x1 = _ix;
   rect.x2 = _ix2;
   rect.y1 = _iy + LOD -> TitleBarH + (Line -> Position - LOD -> First -> Position) * LOD -> MaximumH;
   rect.y2 = rect.y1 + LOD -> MaximumH - 1;

   if (FF_LINE_SELECTED & Line -> Flags)
   {
      if (LOD -> CursorActive)
      {
         F_Do(LOD -> CursorActive,FM_ImageDisplay_Draw,_render,&rect,0);
      }
      else
      {
         _FPen(FV_Pen_Highlight);
         _Boxf(rect.x1,rect.y1,rect.x2,rect.y2);
      }
   }
   else if (FF_DrawLine_Erase & Flags)
   {
      F_Do(Obj,FM_Erase,&rect,0);
   }

   x = rect.x1;
 
   for (j = 0 ; j < LOD -> ColumnCount; j++)
   {
      rect.x2 = x + LOD -> Columns[j].AdjustedWidth - 1;

//      _FPen(j + 1);
//      _Boxf(rect.x1,rect.y1,rect.x2,rect.y2);

      if ((FF_COLUMN_BAR & LOD -> Columns[j].Flags) && (j + 1 < LOD -> ColumnCount))
      {
         _FPen(FV_Pen_Shadow);
         _Move(rect.x2,rect.y1);
         _Draw(rect.x2,rect.y2);
      }

      if ((FF_COLUMN_PREVBAR & LOD -> Columns[j].Flags) && j)
      {
         _FPen(FV_Pen_Shine);
         _Move(x,rect.y1);
         _Draw(x,rect.y2);

         rect.x1 = x + 1;
      }
      else
      {
         rect.x1 = x;
      }
   
      if (FF_COLUMN_PADLEFT & LOD -> Columns[j].Flags)
      {
         rect.x1 += LOD -> Columns[j].Padding;
      }
      if (FF_COLUMN_PADRIGHT & LOD -> Columns[j].Flags)
      {
         rect.x2 -= LOD -> Columns[j].Padding;
      }

      if (Line -> Strings[j])
      {
         F_Do(LOD -> TDObj,FM_Set,
                           FA_TextDisplay_Width,      rect.x2 - rect.x1 + 1,
                           FA_TextDisplay_Height,     rect.y2 - rect.y1 + 1,
                           FA_TextDisplay_Contents,   Line -> Strings[j],
                           FA_TextDisplay_PreParse,   Line -> PreParses[j],
                           TAG_DONE);

         F_Do(LOD -> TDObj,FM_TextDisplay_Draw,&rect);
      }

      x += LOD -> Columns[j].AdjustedWidth;
   }
}
//+

/*** Methods ***************************************************************/

///List_Draw
F_METHODM(void,List_Draw,FS_Draw)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if ((LOD -> Quiet < 0) && (FF_Draw_Update & Msg -> Flags))
   {
      LOD -> Update = NULL; return;
   }

//   F_Log(0,"entries (%ld) visible (%ld) first (0x%08lx)(%ld)",LOD -> LineCount,LOD -> Visible,LOD -> First,LOD -> First -> Position);
   
   F_SUPERDO();
   
   if (FF_Draw_Object & Msg -> Flags)
   {
      titlebar_draw(Class,Obj);
   }

   if (LOD -> First && LOD -> Last)
   {
      if (FF_Draw_Line & Msg -> Flags)
      {
         if (LOD -> Update)
         {
            if (LOD -> Update -> Position >= LOD -> First -> Position &&
                LOD -> Update -> Position <= LOD -> Last -> Position)
            {
               List_DrawLine(Class,Obj,LOD -> Update,FF_DrawLine_Erase);
            }
         }
      }
      else
      {
         LONG rel=0;
         ULONG rel_abs=0;
 
         if (LOD -> LastFirst)
         {
            rel = LOD -> First -> Position - LOD -> LastFirst -> Position;
            rel_abs = (rel < 0) ? -rel : rel;
         }

         if (rel && (rel_abs < LOD -> Visible))
         {
            FLine *line = (rel < 0) ? LOD -> First : LOD -> Last;

            ScrollRasterBF(_rp,0,rel * LOD -> MaximumH,
               _ix,  _iy + LOD -> TitleBarH,
               _ix2, _iy + LOD -> TitleBarH + LOD -> Visible * LOD -> MaximumH - LOD -> Spacing);

            for ( ; line && rel_abs; rel_abs--)
            {
               List_DrawLine(Class,Obj,line,FF_DrawLine_Erase);
            
               line = (rel < 0) ? line -> Next : line -> Prev;
            }
         }
         else
         {
            FLine *line;
            FRect r;

            for (line = LOD -> First ; line ; line = line -> Next)
            {
               List_DrawLine(Class,Obj,line,FF_DrawLine_Erase);

               if (line == LOD -> Last) break;
            }

            r.x1 = _ix;
            r.x2 = _ix2;
            r.y1 = _iy + LOD -> TitleBarH + (LOD -> Last -> Position - LOD -> First -> Position + 1) * LOD -> MaximumH/* - 1*/;
            r.y2 = _iy2;

            F_Do(Obj,FM_Erase,&r,0);
         }
      }

      LOD -> LastFirst = LOD -> First;
   }
   else
   {
      FRect r;

      r.x1 = _ix;
      r.x2 = _ix2;
      r.y1 = _iy + LOD -> TitleBarH;
      r.y2 = _iy2;

      F_Do(Obj,FM_Erase,&r,0);
   }

   LOD -> Update = NULL;
}
//+

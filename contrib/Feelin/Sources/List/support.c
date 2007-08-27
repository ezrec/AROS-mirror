#include "Private.h"

///line_display

/* Call the FM_List_Display method for the given Line. It fills  out  'Line
-> Strings' and 'Line -> PreParses' arrays with strings to be displayed. */

void line_display(FClass *Class,FObject Obj,FLine *Line)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (Line)
   {
      ULONG i;

      for (i = 0 ; i < LOD -> ColumnCount ; i++)
      {
         Line -> PreParses[i] = NULL;
      }

      F_Do(Obj,F_IDM(FM_List_Display),Line -> Entry,Line -> Strings,Line -> PreParses);
   }
}
//+
///line_compute_dimensions
/*

   Compute the dimensions of a Line.

*/
void line_compute_dimensions(FClass *Class,FObject Obj,FLine *Line)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   Line -> Height = 0;

   if (_render && LOD -> Columns && Line)
   {
      ULONG j;

/*** only compute widths when needed ***************************************/

      if (!(FF_LINE_COMPUTED & Line -> Flags))
      {
         for (j = 0 ; j < LOD -> ColumnCount ; j++)
         {
            ULONG tw=0,th=0;
            line_display(Class,Obj,Line);

            if (Line -> Strings[j])
            {
               F_Do(LOD -> TDObj,FM_Set,
                                 FA_TextDisplay_Font,       _font,
                                 FA_TextDisplay_Width,      FV_MAXMAX,
                                 FA_TextDisplay_Height,     FV_MAXMAX,
                                 FA_TextDisplay_Contents,   Line -> Strings[j],
                                 FA_TextDisplay_PreParse,   Line -> PreParses[j],
                                 TAG_DONE);

               F_Do(LOD -> TDObj,FM_Get,
                                 FA_TextDisplay_Width,  &tw,
                                 FA_TextDisplay_Height, &th,
                                 TAG_DONE);
                                 
//               F_Log(0,"(%ld:%ld)(%s&%s) >> %ld x %ld",Line -> Position,j,Line -> PreParses[j],Line -> Strings[j],tw,th);
            }
               
            if (FF_COLUMN_PADLEFT & LOD -> Columns[j].Flags)
            {
               tw += LOD -> Columns[j].Padding;
            }
            if (FF_COLUMN_PADRIGHT & LOD -> Columns[j].Flags)
            {
               tw += LOD -> Columns[j].Padding;
            }
            if (FF_COLUMN_PREVBAR & LOD -> Columns[j].Flags)
            {
               tw++;
            }
            if (FF_COLUMN_BAR & LOD -> Columns[j].Flags)
            {
               tw++;
            }

            Line -> Height    = MAX(Line -> Height,th + LOD -> Spacing);
            Line -> Widths[j] = tw;
         }
         Line -> Flags |= FF_LINE_COMPUTED;
      }

/*** adjust columns maximum width ******************************************/

      for (j = 0 ; j < LOD -> ColumnCount ; j++)
      {
         LOD -> Columns[j].Width = MAX(LOD -> Columns[j].Width,Line -> Widths[j]);
               
//               F_Log(0,"Col(%ld) WIDTH (%ld)",j,LOD -> Columns[j].Width);
      }

/*** adjust maximum line height ********************************************/

      LOD -> MaximumH = MAX(LOD -> MaximumH,Line -> Height);
   }
}
//+

///list_adjust_first
void list_adjust_first(FClass *Class,FObject Obj,uint32 first_pos)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

//   F_Log(0,"first_pos (%ld) Visible (%ld)",first_pos,LOD -> Visible);

   if (LOD -> Visible)
   {
      FLine *line;
      ULONG last_pos = first_pos + LOD -> Visible - 1;
 
      if (last_pos > ((FLine *)(LOD -> LineList.Tail)) -> Position)
      {
//         F_Log(0,"last_pos too far (%ld)(%ld)",last_pos,((FLine *)(LOD -> LineList.Tail)) -> Position);
 
         last_pos = ((FLine *)(LOD -> LineList.Tail)) -> Position;
         first_pos = last_pos - LOD -> Visible + 1;
         
         if ((LONG)(first_pos) < 0)
         {
//            F_Log(0,"first_pos invalid (%ld)",first_pos);
 
            first_pos = 0;
         }
      }

//      F_Log(0,"First 0x%08lx (%ld)",LOD -> First,LOD -> First -> Position);
 
      for (line = (FLine *)(LOD -> LineList.Head) ; line ; line = line -> Next)
      {
         if (line -> Position == first_pos)
         {
            LOD -> First = line; break;
         }
      }

      for ( ; line ; line = line -> Next)
      {
         if (line -> Position == last_pos)
         {
            LOD -> Last = line; break;
         }
      }
   }
   else
   {
      LOD -> First = NULL;
      LOD -> Last  = NULL;
   }
}
//+
///list_update
void list_update(FClass *Class,FObject Obj,int32 Notify)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (_render && LOD -> Columns)
   {
      ULONG prev_visible = LOD -> Visible;
      FLine *prev_first = LOD -> First;

      if (LOD -> LineCount && LOD -> MaximumH)
      {
         LOD -> Visible = MIN(LOD -> LineCount,(_ih - LOD -> TitleBarH + LOD -> Spacing) / LOD -> MaximumH);
      }

      //F_Log(0,"update: first 0x%08lx (%ld)",LOD -> First,LOD -> First -> Position);
      
      list_adjust_first(Class,Obj,(LOD -> First) ? LOD -> First -> Position : 0);
      
      titlebar_adjust_dimensions(Class,Obj);

/*** update attributes *****************************************************/

      if (LOD -> Quiet >= 0 && Notify && prev_visible != LOD -> Visible || prev_first != LOD -> First)
      {
         /* only modified attributes trigger notification, avoiding  a  lot
         of notification loops */

         F_SuperDo(Class,Obj,FM_Set,
         
            (prev_visible != LOD -> Visible) ? F_IDA(FA_List_Visible) : TAG_IGNORE,LOD -> Visible,
            (prev_first != LOD -> First) ? F_IDA(FA_List_First) : TAG_IGNORE,(LOD -> First) ? LOD -> First -> Position : 0,
            
            TAG_DONE);
      }
   }
}
//+


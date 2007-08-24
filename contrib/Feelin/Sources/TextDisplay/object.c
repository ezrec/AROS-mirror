#include "Private.h"
#include "create.h"
#include "setup.h"

///td_text_extent
uint32 td_text_extent(struct TextFont *Font,STRPTR Str,uint32 Count,struct TextExtent *te)
{
   uint32 len;

   if (Font -> tf_Flags & FPF_PROPORTIONAL)
   {
      int16 idx;
      int16 defaultidx = Font -> tf_HiChar - Font -> tf_LoChar + 1; /* Last glyph is the default glyph */
      uint8 c;

      for (len = 0 ; Count ; Count--)
      {
         c = *Str++;

         if (c < Font -> tf_LoChar || c > Font -> tf_HiChar)
         {
            idx = defaultidx;
         }
         else
         {
            idx = c - Font -> tf_LoChar;
         }

         len += ((WORD *)(Font -> tf_CharKern))[idx];
         len += ((WORD *)(Font -> tf_CharSpace))[idx];
      }
   }
   else
   {
      len = Font -> tf_XSize * Count;
   }

   te -> te_Width       = len;
   te -> te_Height      = Font -> tf_YSize;
   te -> te_Extent.MinX = 0;
   te -> te_Extent.MinY = -Font -> tf_Baseline;
   te -> te_Extent.MaxX = te -> te_Width  - 1;
   te -> te_Extent.MaxY = te -> te_Height - 1 - Font -> tf_Baseline;

   return len;
}
//+
///td_adjust
void td_adjust(FClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FTDLine *line;
   FTDAdjust adjust;

   LOD -> Width  = 0;
//   LOD -> Height = 0;

   adjust.font    = LOD -> Font;
   adjust.limit_w = LOD -> limit_w;
   adjust.limit_h = LOD -> limit_h;

   for (line = (FTDLine *)(LOD -> LineList.Head) ; line ; line = line -> Next)
   {
      td_line_adjust(line,&adjust);

      LOD -> Width = MAX(LOD -> Width,line -> adjust_width);
//      LOD -> Height += line -> Height;
   }
}
//+

/*** Methods ***************************************************************/

///TD_New
F_METHOD(uint32,TD_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   LOD -> Flags   = FF_TD_SHORTCUT;
   LOD -> limit_w = FV_TD_NOLIMIT;
   LOD -> limit_h = FV_TD_NOLIMIT;

   return F_SUPERDO();
}
//+
///TD_Get
F_METHOD(void,TD_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_TextDisplay_Contents:   F_STORE(LOD -> Text);      break;
      case FA_TextDisplay_PreParse:   F_STORE(LOD -> Prep);      break;
      case FA_TextDisplay_Width:      F_STORE(LOD -> Width);     break;
      case FA_TextDisplay_Height:     F_STORE(LOD -> Height);    break;
      case FA_TextDisplay_Shortcut:   F_STORE(LOD -> Shortcut);  break;
      case FA_TextDisplay_Font:       F_STORE(LOD -> Font);      break;
   }

   F_SUPERDO();
}
//+
///TD_Set
#define FF_TD_UPDATE_CREATE                     (1 << 0)
#define FF_TD_UPDATE_ADJUST                     (1 << 1)

F_METHOD(void,TD_Set)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   struct TagItem         *Tags = Msg,item;

   bits32 update = 0;
   
   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_TextDisplay_Contents:
      {
         LOD -> Text = (STRPTR)(item.ti_Data); update |= FF_TD_UPDATE_CREATE;
      }
      break;

      case FA_TextDisplay_PreParse:
      {
         LOD -> Prep = (STRPTR)(item.ti_Data); update |= FF_TD_UPDATE_CREATE;
      }
      break;

      case FA_TextDisplay_Font:
      {
         /* should only UPDATE_resize */

         LOD -> Font = (struct TextFont *)(item.ti_Data); update |= FF_TD_UPDATE_CREATE;
      }
      break;

      case FA_TextDisplay_Width:
      {
         LOD -> limit_w = item.ti_Data; update |= FF_TD_UPDATE_ADJUST;
      }
      break;

      case FA_TextDisplay_Height:
      {
         LOD -> limit_h = item.ti_Data; update |= FF_TD_UPDATE_ADJUST;
      }
      break;

      case FA_TextDisplay_Shortcut:
      {
         if (item.ti_Data)
         {
            LOD -> Flags |= FF_TD_SHORTCUT;
         }
         else
         {
            LOD -> Flags &= ~FF_TD_SHORTCUT; LOD -> Shortcut = NULL;
         }

         update |= FF_TD_UPDATE_CREATE;
      }
      break;
   }

   if (update)
   {
      FRender *render = LOD -> Render;

      if (render)
      {
         if (FF_TD_UPDATE_CREATE & update)
         {
            F_Do(Obj,FM_TextDisplay_Cleanup);
            F_Do(Obj,FM_TextDisplay_Setup,render);
         }
         else if (FF_TD_UPDATE_ADJUST & update)
         {
            td_adjust(Class,Obj);
         }
      }
   }

   F_SUPERDO();
}
//+

///TD_Setup
F_METHODM(int32,TD_Setup,FS_TextDisplay_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   LOD -> Width  = 0;
   LOD -> Height = 0;
   LOD -> Render = Msg -> Render;

   if (LOD -> Render && LOD -> Font)
   {
      td_create(Class,Obj);

      /* D'abord on calcule la taille de toutes les lignes, et de tous  les
      chunks,  sans  aucune restriction. On en profite pour initialiser les
      images qui trennent, charger les fontes et allouer les couleurs */
      
      td_setup(Class,Obj);

      /* Maintenant, on ajuste les lignes en fonction des restrictions */

      td_adjust(Class,Obj);

      return TRUE;
   }
   return FALSE;
}
//+
///TD_Cleanup
F_METHOD(void,TD_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FTDLine *line;
   FTDChunk *chunk;
   FTDColor *color;
   
/*** lines *****************************************************************/

//   while ((line = (FTDLine *) LOD -> LineList.Tail) != NULL)
   while ((line = F_LinkRemove(&LOD -> LineList,LOD -> LineList.Tail)) != NULL)
   {
//      F_LinkRemove(&LOD -> LineList,(FNode *) line);

//      while ((chunk = (FTDChunk *) line -> ChunkList.Tail) != NULL)
      while ((chunk = F_LinkRemove(&line -> ChunkList,line -> ChunkList.Tail)) != NULL)
      {
//         F_LinkRemove(&line -> ChunkList,(FNode *) chunk);

         switch (_td_chunk_type)
         {
            case FV_TD_CHUNK_TEXT:
            {
               if (_td_chunk_text_hold_font)
               {
                  FTDContextFont *font;

                  if ((font = _td_chunk_text_font) != NULL)
                  {
                     if (font -> setup_font)
                     {
                        CloseFont(font -> setup_font); font -> setup_font = NULL;
                     }

                     F_Dispose(font -> face);
                     F_Dispose(font);
                  }
               }
            }
            break;

            case FV_TD_CHUNK_IMAGE:
            {
               if (LOD -> Render)
               {
                  F_Do(_td_chunk_image_object,FM_ImageDisplay_Cleanup,LOD -> Render);
               }
               F_DisposeObj(_td_chunk_image_object); _td_chunk_image_object = NULL;
               F_Dispose(_td_chunk_image_spec);
            }
            break;
         }
         F_Dispose(chunk);
      }
      F_Dispose(line -> hr);
      F_Dispose(line);
   }

/*** colors ****************************************************************/

   while ((color = (FTDColor *)(LOD -> ColorsList.Tail)) != NULL)
   {
      F_LinkRemove(&LOD -> ColorsList,(FNode *) color);

      if (color -> Type == FV_TD_COLOR_SPEC)
      {
         F_Dispose(color -> Spec);
      }
      
      if (color -> Color && LOD -> Render)
      {
         F_Do(LOD -> Render -> Display,FM_RemColor,color -> Color);
      }
 
      F_Dispose(color);
   }

   LOD -> Render = NULL;
}
//+

#include "Private.h"

struct in_FeelinTD_Context_Setup
{
   struct TextExtent                te;
   struct TextFont                 *font;
   char                             font_spec[64];
};

typedef struct in_FeelinTD_Context_Setup        FTDContextSetup;

///td_setup
void td_setup(struct FeelinClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FTDLine *line;
   FTDChunk *chunk;
   FTDColor *color;
   FTDContextSetup context;

   context.font = LOD -> Font;

/*** setup colors **********************************************************/
 
   for (color = (FTDColor *) LOD -> ColorsList.Head ; color ; color = color -> Next)
   {
      switch (color -> Type)
      {
         case FV_TD_COLOR_RGB:
         {
            if ((color -> Color = (FColor *) F_Do(LOD -> Render -> Display,FM_AddColor,color -> Spec)) != NULL)
            {
               color -> Pen = color -> Color -> Pen;
            }
         }
         break;

         case FV_TD_COLOR_SPEC:
         {
            if ((color -> Color = (FColor *) F_Do(LOD -> Render -> Display,FM_CreateColor,color -> Spec)) != NULL)
            {
               color -> Pen = color -> Color -> Pen;
            }
         }
         break;
      }
   }
 
/*** setup lines ***********************************************************/
 
   for (line = (FTDLine *) LOD -> LineList.Head ; line ; line = line -> Next)
   {
      UWORD font_above = 0;
      UWORD font_below = 0;

      line -> Width  = 0;
      line -> Height = context.font -> tf_YSize;

      for (chunk = (FTDChunk *) line -> ChunkList.Head ; chunk ; chunk = chunk -> Next)
      {
         switch (_td_chunk_type)
         {
            case FV_TD_CHUNK_TEXT:
            {
               FTDContextFont *font;

               if ((font = _td_chunk_text_font) != NULL)
               {
                  if (_td_chunk_text_hold_font)
                  {
                     if (font -> face || font -> size)
                     {
                        switch (font -> setup_size_type)
                        {
                           case FV_TD_CONTEXT_FONT_SIZE_SAME:     font -> setup_size = context.font -> tf_YSize; break;
                           case FV_TD_CONTEXT_FONT_SIZE_PIXELS:   font -> setup_size = font -> size; break;
                           case FV_TD_CONTEXT_FONT_SIZE_PERCENT:  font -> setup_size = context.font -> tf_YSize * font -> size / 100; break;
                        }

                        F_StrFmt((STRPTR)(&context.font_spec),"%s/%ld",(font -> face) ? font -> face : (STRPTR)(context.font -> tf_Message.mn_Node.ln_Name),font -> setup_size);

                        font -> setup_font = (struct TextFont *) F_Do(LOD -> Render -> Application,FM_Application_OpenFont,NULL,&context.font_spec);
                     }
                  }

                  if (font -> setup_font)
                  {
                     context.font = font -> setup_font;
                  }
                  else
                  {
                     context.font = LOD -> Font;
                  }
               }
               else
               {
                  context.font = LOD -> Font;
               }

               td_text_extent(context.font,_td_chunk_text_text,_td_chunk_text_chars,&context.te);

//               F_Log(0,"font '%s/%ld' (chunk: %ldx%ld)",context.font -> tf_Message.mn_Node.ln_Name,context.font -> tf_YSize,context.te.te_Width,context.te.te_Height);

               _td_chunk_width  = context.te.te_Width;
               _td_chunk_height = context.te.te_Height;

               font_above = MAX(font_above,context.font -> tf_Baseline);
               font_below = MAX(font_below,context.font -> tf_YSize - context.font -> tf_Baseline);

               line -> baseline = MAX(line -> baseline,context.font -> tf_Baseline);
            }
            break;

            case FV_TD_CHUNK_IMAGE:
            {
               FTDChunkImage *image = (FTDChunkImage *) chunk;

               if ((image -> object = ImageDisplayObject,

                     FA_ImageDisplay_Spec,  image -> spec,

               End))
               {
                  uint32 w,h;

                  F_Do(image -> object,FM_ImageDisplay_Setup,LOD -> Render);

                  F_Do(image -> object,FM_Get,
                                       FA_ImageDisplay_Width,  &w,
                                       FA_ImageDisplay_Height, &h,
                                       TAG_DONE);

                  _td_chunk_width  = (image -> w) ? (image -> w) : w;
                  _td_chunk_height = (image -> h) ? (image -> h) : h;
               }
            }
            break;
         }

         line -> Width += _td_chunk_width;
         line -> Height = MAX(line -> Height,_td_chunk_height);
      }

      line -> Height = MAX(line -> Height,font_above + font_below);
      line -> baseline = font_above;

//         F_Log(0,"line %ld x %ld - baseline %ld - above %ld, below %ld",line -> Width,line -> Height,line -> baseline,font_above,font_below);

      if (line -> hr)
      {
         line -> hr -> spacing = context.font -> tf_YSize / 2;

         if (!line -> hr -> compact)
         {
            line -> Height += line -> hr -> size + line -> hr -> spacing * 2;
         }
      }

      LOD -> Width = MAX(LOD -> Width,line -> Width);
      LOD -> Height += line -> Height;

      if (line -> Next)
      {
         LOD -> Height += line -> spacing;
      }
   }

   if (FF_TD_EXTRA_TOPLEFT & LOD -> Flags)
   {
      LOD -> Width += 1;
      LOD -> Height += 1;
   }
   if (FF_TD_EXTRA_BOTTOMRIGHT & LOD -> Flags)
   {
      LOD -> Width += 1;
      LOD -> Height += 1;
   }

//   F_Log(0,"%ld x %ld",LOD -> Width,LOD -> Height);
}
//+

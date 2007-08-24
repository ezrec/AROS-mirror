#include "Private.h"
#include "support.h"

typedef struct FeelinImageData
{
   struct in_CodeTable             *Table;
   uint16                           type;
   int16                            hicolors;
   uint32                           data;
   FColor                          *color;
}
FImage;
 
///id_color_create
STATIC F_CODE_CREATE(id_color_create)
{
   uint32 data=0;
   uint32 type = color_decode_spec(Spec,&data);

//   F_Log(0,"Spec (%s) >> (%ld)(0x%08lx)",Spec,type,data);
 
   if (type)
   {
      FImage *image = F_NewP(CUD -> Pool,sizeof (FImage));

      if (image)
      {
         image -> type = type;
         image -> data = data;

         return image;
      }
   }
   return NULL;
}
//+
///id_color_delete
STATIC F_CODE_DELETE(id_color_delete)
{
   F_Dispose(image);
}
//+
///id_color_setup
STATIC F_CODE_SETUP(id_color_setup)
{
   if (image -> type == COLOR_TYPE_RGB)
   {
      if ((FF_Render_TrueColors & Render -> Flags) && CyberGfxBase)
      {
         image -> hicolors = TRUE;

         return TRUE;
      }
      else if ((image -> color = (FColor *) F_Do(Render -> Display,FM_AddColor,image -> data)))
      {
         return TRUE;
      }
   
      return FALSE;
   }
   return TRUE;
}
//+
///id_color_cleanup
STATIC F_CODE_CLEANUP(id_color_cleanup)
{
   if (image -> color)
   {
      F_Do(Render -> Display,FM_RemColor,image -> color); image -> color = NULL;
   }
}
//+
///id_color_draw
STATIC F_CODE_DRAW(id_color_draw)
{
   switch (image -> type)
   {
      case COLOR_TYPE_SCHEME:
      {
         SetAPen(Msg -> Render -> RPort,Msg -> Render -> Palette -> Pens[image -> data]);
         RectFill(Msg -> Render -> RPort,Msg -> Rect -> x1,Msg -> Rect -> y1,Msg -> Rect -> x2,Msg -> Rect -> y2);
      }
      break;
       
      case COLOR_TYPE_PEN:
      {
         SetAPen(Msg -> Render -> RPort,image -> data);
         RectFill(Msg -> Render -> RPort,Msg -> Rect -> x1,Msg -> Rect -> y1,Msg -> Rect -> x2,Msg -> Rect -> y2);
      }
      break;

      case COLOR_TYPE_RGB:
      {
         if (image -> color)
         {
            SetAPen(Msg -> Render -> RPort,image -> color -> Pen);
            RectFill(Msg -> Render -> RPort,Msg -> Rect -> x1,Msg -> Rect -> y1,Msg -> Rect -> x2,Msg -> Rect -> y2);
         }
         else if (CyberGfxBase)
         {
            FillPixelArray(Msg -> Render -> RPort,
                           Msg -> Rect -> x1,
                           Msg -> Rect -> y1,
                           Msg -> Rect -> x2 - Msg -> Rect -> x1 + 1,
                           Msg -> Rect -> y2 - Msg -> Rect -> y1 + 1,
                           image -> data);
         }
      }
      break;
   }
   return TRUE;
}
//+

struct in_CodeTable id_color_table =
{
   (in_Code_Create *) &id_color_create,
   (in_Code_Delete *) &id_color_delete,
   (in_Code_Setup *) &id_color_setup,
   (in_Code_Cleanup *) &id_color_cleanup,
   NULL,
   (in_Code_Draw *) &id_color_draw,
};

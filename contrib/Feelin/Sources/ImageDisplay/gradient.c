#include "Private.h"
#include "support.h"

#define CODE_NOMATH
 
#ifdef CODE_NOMATH
#include "gradient.h"
#else
#include <math.h>
#endif

#define SHIFT (sizeof(uint32)*8 - (8 + 1))

typedef struct in_Image_Gradient
{
   uint32                           angle;
   uint32                           type1;
   uint32                           data1;
   uint32                           type2;
   uint32                           data2;
   uint32                           type3;
   uint32                           data3;
   uint32                           type4;
   uint32                           data4;
}
FImage;

///color_type_to_rgb
STATIC uint32 color_type_to_rgb(uint32 type,uint32 data,FRender *Render)
{
   switch (type)
   {
      case COLOR_TYPE_SCHEME:
      {
         return Render -> Palette -> ARGB[data];
      }
      break;

      case COLOR_TYPE_PEN:
      {
         struct ColorMap *cm;
         uint32 depth;

         F_Do(Render -> Display,FM_Get,

            (uint32) "ColorMap", &cm,
            (uint32) "Depth",    &depth,

            TAG_DONE);
            
         depth = MIN(depth,8);

         if (cm)
         {
            uint32 rgb[3];

            if ((data = data % (1 << depth)) < 0)
            {
               data = (1 << depth) + data;
            }

            GetRGB32(cm,data,1,(uint32 *)(&rgb));


            return (0x00FF0000 & rgb[0]) | (0x0000FF00 & rgb[1]) | (0x000000FF & rgb[2]);
         }
      }
      break;

      case COLOR_TYPE_RGB:
      {
         return data;
      }
      break;
   }
   return 0;
}
//+

///id_gradient_create
STATIC F_CODE_CREATE_XML(id_gradient_create)
{
    FImage *img;
    
    if ((img = F_NewP(CUD -> Pool,sizeof (FImage))))
    {
        for ( ; Attribute ; Attribute = Attribute -> Next)
        {
            switch (Attribute -> Name -> ID)
            {
                case FV_XML_ID_ANGLE:
                {
                    img -> angle = F_Do(CUD -> XMLDocument,F_ID(CUD -> IDs,FM_Document_Resolve),Attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                }
                break;
                
                case FV_XML_ID_START:
                case FV_XML_ID_FIRST_START:
                {
                    img -> type1 = color_decode_spec(Attribute -> Data,&img -> data1);
                }
                break;
                
                case FV_XML_ID_MIDDLE:
                {
                    img -> type2 = color_decode_spec(Attribute -> Data,&img -> data2);
                    img -> type3 = color_decode_spec(Attribute -> Data,&img -> data3);
                }
                break;
                 
                case FV_XML_ID_FIRST_END:
                {
                    img -> type2 = color_decode_spec(Attribute -> Data,&img -> data2);
                }
                break;
                
                case FV_XML_ID_SECOND_START:
                {
                    img -> type3 = color_decode_spec(Attribute -> Data,&img -> data3);
                }
                break;
                
                case FV_XML_ID_END:
                case FV_XML_ID_SECOND_END:
                {
                    img -> type4 = color_decode_spec(Attribute -> Data,&img -> data4);
                }
                break;
            }
        }
     
        if (img -> type1 && img -> type4)
        {
            return img;
        }
        else
        {
            F_Dispose(img);
        }
    }
    return NULL;
}
//+
///id_gradient_delete
STATIC F_CODE_DELETE(id_gradient_delete)
{
   F_Dispose(image);
}
//+

///id_gradient_draw_vertical
STATIC void id_gradient_draw_vertical
(
   struct RastPort *rp,
   int16 x1, int16 y1, int16 x2, int16 y2,
   int16 oy1, int16 oy2,
   uint32 start_rgb,
   uint32 end_rgb
)
{
    int32 delta_max = (oy2 - oy1 > 0) ? oy2 - oy1 : 1;
    int32 width = x2  - x1 + 1;

    int32 step_r = ((int32)(((0x00FF0000 & end_rgb) >> 16) - ((0x00FF0000 & start_rgb) >> 16)) << SHIFT) / delta_max;
    int32 step_g = ((int32)(((0x0000FF00 & end_rgb) >>  8) - ((0x0000FF00 & start_rgb) >>  8)) << SHIFT) / delta_max;
    int32 step_b = ((int32)( (0x000000FF & end_rgb)        -  (0x000000FF & start_rgb)       ) << SHIFT) / delta_max;

    int32 y, offset_y = y1 - oy1;

    int32 red   = ((1 << SHIFT) >> 1) + (((0x00FF0000 & start_rgb) >> 16) << SHIFT) + offset_y*step_r;
    int32 green = ((1 << SHIFT) >> 1) + (((0x0000FF00 & start_rgb) >>  8) << SHIFT) + offset_y*step_g;
    int32 blue  = ((1 << SHIFT) >> 1) + ( (0x000000FF & start_rgb)        << SHIFT) + offset_y*step_b;

    for (y = y1; y <= y2; y++)
    {
        FillPixelArray(rp, x1, y, width, 1,((red >> SHIFT) << 16) + ((green >> SHIFT) << 8) + (blue >> SHIFT));

        red   += step_r;
        green += step_g;
        blue  += step_b;
    }
}
//+
///id_gradient_draw_horizontal
STATIC void id_gradient_draw_horizontal
(
    struct RastPort *rp,
    int16 x1, int16 y1, int16 x2, int16 y2,
    int16 ox1, int16 ox2,
    uint32 start_rgb,
    uint32 end_rgb
)
{
    int32 delta_max = (ox2 - ox1 > 0) ? ox2 - ox1 : 1;
    int32 height = y2  - y1 + 1;

    int32 step_r = ((int32)(((0x00FF0000 & end_rgb) >> 16) - ((0x00FF0000 & start_rgb) >> 16)) << SHIFT) / delta_max;
    int32 step_g = ((int32)(((0x0000FF00 & end_rgb) >>  8) - ((0x0000FF00 & start_rgb) >>  8)) << SHIFT) / delta_max;
    int32 step_b = ((int32)( (0x000000FF & end_rgb)        -  (0x000000FF & start_rgb)       ) << SHIFT) / delta_max;

    int32 x, offset_x = x1 - ox1;

   /* 1 << (SHIFT - 1) is 0.5 in  fixed  point  math.  We  add  it  to  the
   variable  so  that,  at the moment in which the variable is converted to
   integer, rounding is done properly. That is, a+x, with 0 < x <  0.5,  is
   rounded down to a, and a+x, with 0.5 <= x < 1, is rounded up to a+1. */

   int32 red   = ((1 << SHIFT) >> 1) + (((0x00FF0000 & start_rgb) >> 16) << SHIFT) + offset_x * step_r;
   int32 green = ((1 << SHIFT) >> 1) + (((0x0000FF00 & start_rgb) >>  8) << SHIFT) + offset_x * step_g;
   int32 blue  = ((1 << SHIFT) >> 1) + ( (0x000000FF & start_rgb)        << SHIFT) + offset_x * step_b;

   for (x = x1; x <= x2; x++)
   {
      FillPixelArray(rp, x, y1, 1, height,((red >> SHIFT) << 16) + ((green >> SHIFT) << 8) + (blue >> SHIFT));

      red   += step_r;
      green += step_g;
      blue  += step_b;
   }
}
//+
///id_gradient_draw_angle
struct myrgb
{
    int red,green,blue;
};

/*****************************************************************
 Fill the given rectangle with a angle oriented gradient. The
 unit angle uses are degrees
******************************************************************/
STATIC int id_gradient_draw_angle
(
   struct RastPort *rp,
   int32 xt,
   int32 yt,
   int32 xb,
   int32 yb,
   uint32 start_rgb,
   uint32 end_rgb,
   int32 angle)
{

   /* The basic idea of this algorithm is to calc the intersection  between
   the  diagonal of the rectangle (xs,ys) with dimension (xw,yw) a with the
   line starting at (x,y) (every pixel  inside  the  rectangle)  and  angle
   angle with direction vector (vx,vy).

   Having the intersection point we then know the color of the pixel.

   TODO: Turn the algorithm into  a  incremental  one  Remove  the  use  of
   floating point variables */

#ifndef CODE_NOMATH
   double rad = angle * PI / 180;
   double cosarc = cos(rad);
   double sinarc = sin(rad);
#endif
   struct myrgb startRGB,endRGB;
   int diffR, diffG, diffB;

   int r,t; /* some helper variables to short the code */
   int l,y,c;
   int y1; /* The intersection point */
   int incr_y1; /* increment of y1 */
   int xs,ys,xw,yw;
   int xadd,ystart,yadd;
#ifdef CODE_NOMATH
   int vx = vx_array[angle];
   int vy = vy_array[angle];
#else
   int vx = (int)(-cosarc*0xff);
   int vy = (int)(sinarc*0xff);
#endif
   int width = xb - xt + 1;
   int height = yb - yt + 1;

   UBYTE *buf = F_New(width*3);
   if (!buf) return 0;
/*
   {
      BPTR vx_out,vy_out;
      
      if (vx_out = Open("ram:vx",MODE_NEWFILE))
      {
         if (vy_out = Open("ram:vy",MODE_NEWFILE))
         {
            uint32 i;
            
            for (i = 0 ; i < 361 ; i++)
            {
               double rad = i * PI / 180;
               double cosarc = cos(rad);
               double sinarc = sin(rad);

               int vx = (int)(-cosarc*0xff);
               int vy = (int)(sinarc*0xff);

               if (i % 8 == 0)
               {
                  FPrintf(vx_out,"\n");
                  FPrintf(vy_out,"\n");
               }
 
               FPrintf(vx_out,"0x%08lx,",vx);
               FPrintf(vy_out,"0x%08lx,",vy);
            }
         
            FPrintf(vx_out,"DONE");
            FPrintf(vy_out,"DONE");
 
            Close(vy_out);
         }
         Close(vx_out);
      }
   }
*/
   startRGB.red   = ((0x00FF0000 & start_rgb) >> 16);
   startRGB.green = ((0x0000FF00 & start_rgb) >>  8);
   startRGB.blue  =  (0x000000FF & start_rgb);
   endRGB.red     = ((0x00FF0000 & end_rgb) >> 16);
   endRGB.green   = ((0x0000FF00 & end_rgb) >>  8);
   endRGB.blue    =  (0x000000FF & end_rgb);

   diffR = endRGB.red - startRGB.red;
   diffG = endRGB.green - startRGB.green;
   diffB = endRGB.blue - startRGB.blue;

   /* Normalize the angle */
   if (angle < 0) angle = 360 - ((-angle)%360);
   if (angle >= 0) angle = angle % 360;

   if (angle <= 90 || (angle > 180 && angle <= 270))
   {
      /* The to be intersected diagonal goes from the top left edge to the bottom right edge */
      xs = 0;
      ys = 0;
      xw = width;
      yw = height;
   }
   else
   {
      /* The to be intersected diagonal goes from the bottom left edge to the top right edge */
      xs = 0;
      ys = height;
      xw = width;
      yw = -height;
   }

   if (angle > 90 && angle <= 270)
   {
      /* for these angle we have y1 = height - y1. Instead of
       *
       *  y1 = height - (-vy*(yw*  xs -xw*  ys)         + yw*(vy*  x -vx*  y))        /(-yw*vx + xw*vy);
       *
       * we can write
       *
         *  y1 =          (-vy*(yw*(-xs)-xw*(-ys+height)) + yw*(vy*(-x)-vx*(-y+height)))/(-yw*vx + xw*vy);
         *
         * so height - y1 can be expressed with the normal formular adapting some parameters.
       *
       * Note that if one would exchanging startRGB/endRGB the values would only work
       * for linear color gradients
       */

      xadd = -1;
      yadd = -1;
      ystart = height;

      xs = -xs;
      ys = -ys + height;
   }
   else
   {
      xadd = 1;
      yadd = 1;
      ystart = 0;
   }

   r = -vy*(yw*xs-xw*ys);
   t = -yw*vx + xw*vy;

    /* The formular as shown above is
     *
     *    y1 = ((-vy*(yw*xs-xw*ys) + yw*(vy*x-vx*y)) /(-yw*vx + xw*vy));
     *
     * We see that only yw*(vy*x-vx*y) changes during the loop.
     *
     * We write
     *
     *   Current Pixel: y1(x,y) = (r + yw*(vy*x-vx*y))/t = r/t + yw*(vy*x-vx*y)/t
     *   Next Pixel:    y1(x+xadd,y) = (r + vw*(vy*(x+xadd)-vx*y))/t
     *
     *   t*(y1(x+xadd,y) - y1(x,y)) = yw*(vy*(x+xadd)-vx*y) - yw*(vy*x-vx*y) = yw*vy*xadd;
     *
     */

   incr_y1 = yw*vy*xadd;

   for (l = 0, y = ystart; l < height; l++, y+=yadd)
   {
      UBYTE *bufptr = buf;

      /* Calculate initial y1 accu, can be brought out of the loop as  well
      (x=0).  It's  probably  a  a  good  idea  to add here also a value of
      (t-1)/2 to ensure the correct rounding This (and for  r)  is  also  a
      place  were  actually a overflow can happen |yw|=16 |y|=16. So for vx
      nothing is left, currently 9 bits are used for vx or vy */

      int y1_mul_t_accu = r - yw*vx*y;

      for (c = 0 ; c < width ; c++)
      {
         int red,green,blue;

         /* Calculate the intersection of two lines, this is not the fastet
         way  to  do but it is intuitive. Note: very slow! Will be optimzed
         later (remove FFP usage and making it incremental)...update:  it's
         now  incremental  and  no  FFP  is  used  but  it  probably can be
         optimized more by removing some more of the divisions and  further
         specialize the stuff here (use of three accus). */

         y1 = y1_mul_t_accu / t;

         red = startRGB.red + (int)(diffR*y1/height);
         green = startRGB.green + (int)(diffG*y1/height);
         blue = startRGB.blue + (int)(diffB*y1/height);

         /* By using full 32 bits this can be made faster as well */

         *bufptr++ = red;
         *bufptr++ = green;
         *bufptr++ = blue;

         y1_mul_t_accu += incr_y1;
      }

      /* By bringing building the gradient array in the same format as  the
      RastPort BitMap a call * to WritePixelArray() can be made also faster
      */

      WritePixelArray(buf,0,0,width*3 /* srcMod */,
         rp,xt,yt+l,width,1,RECTFMT_RGB);
   }

   F_Dispose(buf);

   return 1;
}
//+

///id_gradient_draw
STATIC F_CODE_DRAW(id_gradient_draw)
{
    uint32 rgb1 = color_type_to_rgb(image -> type1,image -> data1,Msg -> Render);
    uint32 rgb2 = color_type_to_rgb(image -> type2,image -> data2,Msg -> Render);
    uint32 rgb3 = color_type_to_rgb(image -> type3,image -> data3,Msg -> Render);
    uint32 rgb4 = color_type_to_rgb(image -> type4,image -> data4,Msg -> Render);

/*
   if (Msg -> Origin)
   {
      FRect o;
      
      o.x1 = Msg -> Origin -> x;
      o.y1 = Msg -> Origin -> y;
      o.x2 = Msg -> Origin -> x + Msg -> Origin -> w - 1;
      o.y2 = Msg -> Origin -> y + Msg -> Origin -> h - 1;
 
      F_Do(Msg -> Render,(uint32) "DrawGradient",Msg -> Rect,&o,image -> angle,s_rgb,e_rgb,0);
   }
   else
   {
      F_Do(Msg -> Render,(uint32) "DrawGradient",Msg -> Rect,NULL,image -> angle,s_rgb,e_rgb,0);
   }

   return TRUE;
*/

    if (!(FF_Render_TrueColors & Msg -> Render -> Flags) || !(CyberGfxBase))
    {
        return FALSE;
    }

    //F_Log(0,"START 0x%06lx - END 0x%06lx - ANGLE %ld",s_rgb,e_rgb,image -> angle);
  
    switch (image -> angle)
    {
///Vertical2
        case 180:
        {
            uint32 tmp;
         
            tmp = rgb4; rgb4 = rgb1; rgb1 = tmp;
            tmp = rgb3; rgb3 = rgb2; rgb2 = tmp;
        }
//+
///Vertical
      case 0:
      {
         if (image -> type2)
         {
            uint32 y;
            
            if (Msg -> Origin)
            {
               y = Msg -> Origin -> h / 2 + Msg -> Origin -> y - 1;
            }
            else
            {
               y = (Msg -> Rect -> y2 - Msg -> Rect -> y1 + 1) / 2 + Msg -> Rect -> y1 - 1;
            }
 
            id_gradient_draw_vertical
            (
               Msg -> Render -> RPort,

               Msg -> Rect -> x1,
               Msg -> Rect -> y1,
               Msg -> Rect -> x2,
               MIN(Msg -> Rect -> y2,y),

               (Msg -> Origin) ? Msg -> Origin -> y : Msg -> Rect -> y1,
               y,

               rgb1,
               rgb2
            );

            id_gradient_draw_vertical
            (
               Msg -> Render -> RPort,

               Msg -> Rect -> x1,
               MAX(Msg -> Rect -> y1,y+1),
               Msg -> Rect -> x2,
               Msg -> Rect -> y2,

               y + 1,
               (Msg -> Origin) ? Msg -> Origin -> y + Msg -> Origin -> h - 1 : Msg -> Rect -> y2,

               rgb3,
               rgb4
            );
         }
         else
         {
            id_gradient_draw_vertical
            (
               Msg -> Render -> RPort,
               
               Msg -> Rect -> x1,
               Msg -> Rect -> y1,
               Msg -> Rect -> x2,
               Msg -> Rect -> y2,
               
               (Msg -> Origin) ? Msg -> Origin -> y : Msg -> Rect -> y1,
               (Msg -> Origin) ? Msg -> Origin -> y + Msg -> Origin -> h - 1 : Msg -> Rect -> y2,
               
               rgb1,
               rgb4
            );
         }
      }
      break;
//+
///Horizontal2
        case 270:
        {
            uint32 tmp;
         
            tmp = rgb4; rgb4 = rgb1; rgb1 = tmp;
            tmp = rgb3; rgb3 = rgb2; rgb2 = tmp;
        }  
//+
///Horizontal
        case 90:
        {
__horizontal:
 
            if (image -> type2)
            {
               uint32 x;

               if (Msg -> Origin)
               {
                   x = Msg -> Origin -> w / 2 + Msg -> Origin -> x - 1;
               }
               else
               {
                   x = (Msg -> Rect -> x2 - Msg -> Rect -> x1 + 1) / 2 + Msg -> Rect -> x1 - 1;
               }

               id_gradient_draw_horizontal
               (
                  Msg -> Render -> RPort,

                  Msg -> Rect -> x1,
                  Msg -> Rect -> y1,
                  MIN(Msg -> Rect -> x2,x),
                  Msg -> Rect -> y2,

                  (Msg -> Origin) ? Msg -> Origin -> x : Msg -> Rect -> x1,
                  x,

                  rgb1,
                  rgb2
               );
               
               id_gradient_draw_horizontal
               (
                  Msg -> Render -> RPort,

                  MAX(Msg -> Rect -> x1,x),
                  Msg -> Rect -> y1,
                  Msg -> Rect -> x2,
                  Msg -> Rect -> y2,

                  x,
                  (Msg -> Origin) ? Msg -> Origin -> x + Msg -> Origin -> w - 1 : Msg -> Rect -> x2,

                  rgb3,
                  rgb4
               );
            }
            else
            {
               id_gradient_draw_horizontal
               (
                  Msg -> Render -> RPort,
                  
                  Msg -> Rect -> x1,
                  Msg -> Rect -> y1,
                  Msg -> Rect -> x2,
                  Msg -> Rect -> y2,

                  (Msg -> Origin) ? Msg -> Origin -> x : Msg -> Rect -> x1,
                  (Msg -> Origin) ? Msg -> Origin -> x + Msg -> Origin -> w - 1 : Msg -> Rect -> x2,

                  rgb1,
                  rgb4
               );
            }
        }
        break;
//+
        default:
        {
            APTR clip=NULL;
    
            if (Msg -> Origin)
            {
                if (Msg -> Origin -> x != Msg -> Rect -> x1 ||
                    Msg -> Origin -> y != Msg -> Rect -> y1 ||
                    Msg -> Origin -> x + Msg -> Origin -> w - 1 != Msg -> Rect -> x2 ||
                    Msg -> Origin -> y + Msg -> Origin -> h - 1 != Msg -> Rect -> y2)
                {
                    clip = (APTR) F_Do(Msg -> Render,FM_Render_AddClip,Msg -> Rect);
                }
            }
    
            id_gradient_draw_angle
            (
                Msg -> Render -> RPort,

                (clip) ? Msg -> Origin -> x : Msg -> Rect -> x1,
                (clip) ? Msg -> Origin -> y : Msg -> Rect -> y1,
                (clip) ? Msg -> Origin -> x + Msg -> Origin -> w - 1 : Msg -> Rect -> x2,
                (clip) ? Msg -> Origin -> y + Msg -> Origin -> h - 1 : Msg -> Rect -> y2,
                
                rgb1,
                rgb4,
                
                image -> angle
            );
            
            if (clip)
            {
                F_Do(Msg -> Render,FM_Render_RemClip,clip);
            }
        }
        break;
    }
    return TRUE;
}
//+

struct in_CodeTable id_gradient_table =
{
   (in_Code_Create *)   &id_gradient_create,
   (in_Code_Delete *)   &id_gradient_delete,
   (in_Code_Setup *)    NULL,
   (in_Code_Cleanup *)  NULL,
   (in_Code_Get *)      NULL,
   (in_Code_Draw *)     &id_gradient_draw
};

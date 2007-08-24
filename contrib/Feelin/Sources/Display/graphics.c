#include "Private.h"

///D_PixelRead
F_METHODM(uint32,D_PixelRead,FS_Display_PixelRead)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   if (LOD -> Screen)
   {
      if (LOD -> Depth > 8 && CyberGfxBase)
      {
         return ReadRGBPixel(&LOD -> Screen -> RastPort,Msg -> PixelX,Msg -> PixelY);
      }
      else
      {
         return ReadPixel(&LOD -> Screen -> RastPort,Msg -> PixelX,Msg -> PixelY);
      }
   }
   return 0;
}
//+
///D_PixelWrite
F_METHOD(void,D_PixelWrite)
{
   F_Log(0,"NOT IMPLEMENTED YET");
}
//+


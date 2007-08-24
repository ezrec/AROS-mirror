/*

   HSV to RGB using no maths by Olivier LAVIALE
 
*/

#include <libraries/feelin.h>

///hsv_to_rgb
void hsv_to_rgb(unsigned int h,unsigned int s,unsigned int v,FRGB *rgb)
{
   if (s == 0)
   {
      rgb -> r = rgb -> g = rgb -> b = v * 255 / 100;
   }
   else
   {
      ULONG base = ((255 * (100 - s)) * v) / 10000;

      switch (h / 60)
      {
         case 0:
         {
            rgb -> r = 255 * v / 100;
            rgb -> g = (255 - base) * h * v / 6000 + base;
            rgb -> b = base;
         }
         break;

         case 1:
         {
            rgb -> r = (255 - ((255 - base) * (h - 60) / 60 )) * v / 100;
            rgb -> g = 255 * v / 100;
            rgb -> b = base;
         }
         break;

         case 2:
         {
            rgb -> r = base;
            rgb -> g = 255 * v / 100;
            rgb -> b = (255 - base) * (h - 120) * v / 6000 + base;
         }
         break;

         case 3:
         {
            rgb -> r = base;
            rgb -> g = (255 - ((255 - base) * (h - 180) / 60 )) * v / 100;
            rgb -> b = 255 * v / 100;
         }
         break;

         case 4:
         {
            rgb -> r = (255 - base) * (h - 240) * v / 6000 + base;
            rgb -> g = base;
            rgb -> b = 255 * v / 100;
         }
         break;

         case 5:
         case 6:
         {
            rgb -> r = 255 * v / 100;
            rgb -> g = base;
            rgb -> b = (255 - ((255 - base) * (h - 300) / 60 )) * v / 100;
         }
         break;
      }
   }
}
//+


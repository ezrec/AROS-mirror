/*

   RGB to HSV using integers by Olivier LAVIALE

*/

#include <libraries/feelin.h>

void rgb_to_hsv(long r,long g,long b,FHSV *hsv)
{
   long max, min, delta;
   long temp;

   max = MAX(r, MAX(g, b));
   min = MIN(r, MIN(g, b));
   delta = max-min;

   if (max == 0)
   {
      hsv -> s = hsv -> h = hsv -> v = 0;
   }
   else
   {
      hsv -> v = max * 100 / 255;
      hsv -> s = delta * 100 / max;

      if (r == max)
      {
         temp = (g-b) * 60 / 255;
      }
      else if (g == max)
      {
         temp = 120 + ((b-r) * 60 / 255);
      }
      else
      {
         temp = 240 + ((r-g) * 60 / 255);
      }

      if (temp < 0)
      {
         temp += 360;
      }

      hsv -> h = temp;
   }
}

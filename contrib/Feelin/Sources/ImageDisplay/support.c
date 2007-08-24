#include "Private.h"
#include "support.h"

///color_decode_spec
uint32 color_decode_spec(STRPTR Spec,uint32 *Data)
{
   uint32 type = 0;
 
   if (Spec && Data)
   {
      if (*Spec == '#')
      {
         type = COLOR_TYPE_RGB;

         stch_l(Spec + 1,(int32 *)(Data));
      }
      else if (*Spec >= 'd' && *Spec <= 't')
      {
         switch (*Spec)
         {
            case 'd':
            {
               if (F_StrCmp("dark",Spec,4) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_Dark;
               }
            }
            break;

            case 'f':
            {
               if (F_StrCmp("fill",Spec,4) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_Fill;
               }
            }
            break;

            case 'h':
            {
               int32 cmp;

               if ((cmp = F_StrCmp("halfdark",Spec,8)) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_HalfDark;
               }
               else if (cmp > 0) break;

               if ((cmp = F_StrCmp("halfshadow",Spec,10)) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_HalfShadow;
               }
               else if (cmp > 0) break;

               if ((cmp = F_StrCmp("halfshine",Spec,9)) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_HalfShine;
               }
               else if (cmp > 0) break;

               if ((cmp = F_StrCmp("highlight",Spec,9)) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_Highlight;
               }
               else if (cmp > 0) break;
            }
            break;

            case 's':
            {
               int32 cmp;

               if ((cmp = F_StrCmp("shadow",Spec,6)) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_Shadow;
               }
               else if (cmp > 0) break;

               if ((cmp = F_StrCmp("shine",Spec,5)) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_Shine;
               }
               else if (cmp > 0) break;
            }
            break;

            case 't':
            {
               if (F_StrCmp("text",Spec,4) == 0)
               {
                  type = COLOR_TYPE_SCHEME;
                  *Data = FV_Pen_Text;
               }
            }
         }
      }
      else if (stcd_l(Spec,(int32 *) Data))
      {
         type = COLOR_TYPE_PEN;
      }
      else
      {
         F_Log(0,"Unable to decode color spec (%s)",Spec);
      }
   }
   return type;
}
//+

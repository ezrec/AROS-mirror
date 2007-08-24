#include "Private.h"

#define COMPATIBILITY

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Preview_Query
F_METHODM(uint32,Preview_Query,FS_Preview_Query)
{
   if (Msg -> Spec)
   {
      switch (*Msg -> Spec)
      {
         case '#':
         {
            return TRUE;
         }
         break;

#ifdef COMPATIBILITY
         case 'c':
         {
            if (Msg -> Spec[1] == ':')
            {
               return TRUE;
            }
         }
         break;
#endif

         case 'd':
         {
            if (F_StrCmp("dark",Msg -> Spec,4) == 0)
            {
               return TRUE;
            }
         }
         break;

         case 'f':
         {
            if (F_StrCmp("fill",Msg -> Spec,4) == 0)
            {
               return TRUE;
            }
         }
         break;

         case 'h':
         {
            int32 cmp;

            if ((cmp = F_StrCmp("halfdark",Msg -> Spec,8)) == 0)
            {
               return TRUE;
            }
            else if (cmp > 0) break;

            if ((cmp = F_StrCmp("halfshadow",Msg -> Spec,10)) == 0)
            {
               return TRUE;
            }
            else if (cmp > 0) break;

            if ((cmp = F_StrCmp("halfshine",Msg -> Spec,9)) == 0)
            {
               return TRUE;
            }
            else if (cmp > 0) break;

            if ((cmp = F_StrCmp("highlight",Msg -> Spec,9)) == 0)
            {
               return TRUE;
            }
            else if (cmp > 0) break;
         }
         break;

#ifdef COMPATIBILITY
         case 'p':
         {
            if (Msg -> Spec[1] == ':')
            {
               return TRUE;
            }
         }
         break;
#endif

         case 's':
         {
#ifdef COMPATIBILITY
            if (Msg -> Spec[1] == ':')
            {
               return TRUE;
            }
            else
            {
#endif
               int32 cmp;

               if ((cmp = F_StrCmp("shadow",Msg -> Spec,6)) == 0)
               {
                  return TRUE;
               }
               else if (cmp > 0) break;

               if ((cmp = F_StrCmp("shine",Msg -> Spec,5)) == 0)
               {
                  return TRUE;
               }
               else if (cmp > 0) break;
#ifdef COMPATIBILITY
            }
#endif
         }
         break;

         case 't':
         {
            if (F_StrCmp("text",Msg -> Spec,4) == 0)
            {
               return TRUE;
            }
         }
      
         default:
         {
            int32 val;
 
            if (stcd_l(Msg -> Spec,&val))
            {
               return TRUE;
            }
         }
      }
   }
   return FALSE;

}
//+


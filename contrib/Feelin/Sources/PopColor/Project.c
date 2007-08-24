/*

$VER: 03.00 (2005/08/10)
 
   Portability update.
   
   FM_Preview_Query implementation

$VER: 02.00 (2005/04/04)
 
   Class rewritten according to FC_Preview

*/

#include <string.h>

#include <libraries/feelin.h>
#include <proto/feelin.h>

#include "Project.h"

#include "_locale/enums.h"
#include "_locale/table.h"

#define COMPATIBILITY

enum  {
   
      FA_Preview_Spec

      };
      
enum  {
   
      FA_PopColor_AddScheme,
      FA_PopColor_AddPen,
      FA_PopColor_AddRGB
   
      };
 
///PopColor_New
F_METHOD(FObject,PopColor_New)
{
   struct TagItem *Tags = Msg,item;
   
   BOOL addscheme = TRUE;
   BOOL addpen = TRUE;
   BOOL addrgb = TRUE;
                     
   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_PopColor_AddScheme:   addscheme = item.ti_Data; break;
      case FA_PopColor_AddPen:      addpen = item.ti_Data; break;
      case FA_PopColor_AddRGB:      addrgb = item.ti_Data; break;
   }
 
   if (F_SuperDo(Class,Obj,Method,

      FA_Frame,        "$popbutton-frame",
      FA_Back,         "$popbutton-back",
      FA_ColorScheme,  "$popbutton-scheme",
      FA_InputMode,     FV_InputMode_Release,
      FA_ContextHelp,   F_CAT(HELP),

      TAG_MORE,Msg))
   {
      F_Do(Obj,FM_Notify,FA_Pressed,FALSE,
      
         Obj,"FM_Preview_Adjust",9,
         
         F_CAT(TITLE),
         
         "AdjustColor",
         
         "FA_AdjustColor_AddScheme",   addscheme,
         "FA_AdjustColor_AddPen",      addpen,
         "FA_AdjustColor_AddRGB",      addrgb,
         
         TAG_DONE);

      return Obj;
   }
   return NULL;
}
//+
///PopColor_Query
F_METHODM(uint32,PopColor_Query,FS_Preview_Query)
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
         break;
      }
   }
   return FALSE;

}
//+

F_QUERY()
{
   switch (Which)
   {
      case FV_Query_ClassTags:
      {
         STATIC F_ATTRIBUTES_ARRAY =
         {
            F_ATTRIBUTES_ADD("AddScheme", FV_TYPE_BOOLEAN),
            F_ATTRIBUTES_ADD("AddPen", FV_TYPE_BOOLEAN),
            F_ATTRIBUTES_ADD("AddRGB", FV_TYPE_BOOLEAN),
          
            F_ARRAY_END
         };
          
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD        (PopColor_Query,   "FM_Preview_Query"),
            F_METHODS_ADD_STATIC (PopColor_New,      FM_New),

            F_ARRAY_END
         };
         
         STATIC F_RESOLVES_ARRAY =
         {
            F_RESOLVES_ADD("FA_Preview_Spec"),
            
            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(PreviewImage),
            F_TAGS_ADD_METHODS,
            F_TAGS_ADD_ATTRIBUTES,
            F_TAGS_ADD_RESOLVES,
            F_TAGS_ADD_CATALOG,
            
            TAG_DONE
         };

         return F_TAGS;
      }

   }
   return NULL;
}

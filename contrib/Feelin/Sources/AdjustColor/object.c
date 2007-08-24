#include "Private.h"

/*** Methods ***************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   BOOL addscheme = TRUE;
   BOOL addpen = TRUE;
   BOOL addrgb = TRUE;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_AdjustColor_AddScheme:   addscheme = item.ti_Data; break;
      case FA_AdjustColor_AddPen:      addpen = item.ti_Data; break;
      case FA_AdjustColor_AddRGB:      addrgb = item.ti_Data; break;
   }

   if (addscheme)
   {
      LOD -> sch = AdjustSchemeEntryObject, "Preview", FALSE, End;
   }

   if (addpen)
   {
      LOD -> pen = AdjustPenObject, "Preview", FALSE, End;
   }

   if (addrgb)
   {
      LOD -> rgb = AdjustRGBObject, "Preview", FALSE, End;
   }

   if (F_SuperDo(Class,Obj,Method,

      "PreviewTitle", "Color",
      "PreviewClass",  FC_PreviewColor,
      "Separator",     FV_ImageDisplay_Separator,

      Child, VGroup,
         Child, LOD -> page = Page,
            (addscheme) ? Child : TAG_IGNORE, LOD -> sch,
            (addpen)    ? Child : TAG_IGNORE, LOD -> pen,
            (addrgb)    ? Child : TAG_IGNORE, LOD -> rgb,
         End,
      End,
      
      TAG_MORE,Msg))
   {
      F_Do(LOD -> sch,FM_Notify,"Spec",FV_Notify_Always, Obj,F_IDR(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> pen,FM_Notify,"Spec",FV_Notify_Always, Obj,F_IDR(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> rgb,FM_Notify,"Spec",FV_Notify_Always, Obj,F_IDR(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

      return Obj;
   }

   return NULL;
}
//+

///Adjust_Query
F_METHODM(FObject,Adjust_Query,FS_Adjust_Query)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
   if (Msg -> Spec)
   {
      if (F_OBJDO(LOD -> sch))
      {
         return LOD -> sch;
      }
      else if (F_OBJDO(LOD -> pen))
      {
         return LOD -> pen;
      }
      else if (F_OBJDO(LOD -> rgb))
      {
         return LOD -> rgb;
      }
   }
   return NULL;
}
//+
///Adjust_ToObject
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (Msg -> Spec)
   {
      FObject group = (FObject) F_Do(Obj,F_IDM(FM_Adjust_Query),Msg -> Spec);
      int8 found=FALSE;
      uint8 page=0;
      
      if (LOD -> sch)
      {
         page++;
         
         if (LOD -> sch == group) found = TRUE;
      }
   
      if (LOD -> pen && !found)
      {
         page++;
         
         if (LOD -> pen == group) found = TRUE;
      }
   
      if (LOD -> rgb && !found)
      {
         page++;
         
         if (LOD -> rgb == group) found = TRUE;
      }
   
      if (found)
      {
         F_Do(group,FM_Set,FA_NoNotify,TRUE,"Spec",Msg -> Spec,TAG_DONE);
   
         F_Set(LOD -> page,FA_Group_ActivePage,page - 1);
      }
   }
   else
   {
      F_Set(LOD -> page,FA_Group_ActivePage,0);
      
      F_Do(LOD -> sch,FM_Set,FA_NoNotify,TRUE,"Spec",NULL,TAG_DONE);
      F_Do(LOD -> pen,FM_Set,FA_NoNotify,TRUE,"Spec",NULL,TAG_DONE);
      F_Do(LOD -> rgb,FM_Set,FA_NoNotify,TRUE,"Spec",NULL,TAG_DONE);
   }
}
//+


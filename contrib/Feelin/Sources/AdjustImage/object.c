#include "Private.h"

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SuperDo(Class,Obj,Method,

      "PreviewTitle", "Image",
      "PreviewClass", "PreviewImage",
      "Separator",    FV_ImageDisplay_Separator,
 
      /* here further adjust objects are added */

      Child, LOD -> page = Page,
         Child, LOD -> color = AdjustColorObject, "Preview",FALSE, End,
         
         Child, LOD -> raster = AdjustRasterObject, "Preview",FALSE, End,
         Child, LOD -> gradient = AdjustGradientObject, "Preview",FALSE, TAG_DONE),
         
         Child, LOD -> page_bitmap = Page, FA_Group_PageTitle,"BitMap",
            Child, LOD -> picture = F_NewObj("AdjustPicture", "Preview",FALSE, TAG_DONE),
            Child, LOD -> brush = F_NewObj("AdjustBrush", "Preview",FALSE, TAG_DONE),
         End,
      End,

      TAG_MORE, Msg))
   {
      F_Do(LOD -> color,    FM_Notify,"Spec",FV_Notify_Always, Obj,F_IDR(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> raster,   FM_Notify,"Spec",FV_Notify_Always, Obj,F_IDR(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> gradient, FM_Notify,"Spec",FV_Notify_Always, Obj,F_IDR(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> picture,  FM_Notify,"Spec",FV_Notify_Always, Obj,F_IDR(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> brush,    FM_Notify,"Spec",FV_Notify_Always, Obj,F_IDR(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

      return Obj;
   }
   return NULL;
}
//+
///Adjust_Query
F_METHODM(FObject,Adjust_Query,FS_Adjust_Query)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   if (F_OBJDO(LOD -> color))
   {
      return LOD -> color;
   }
   else if (F_OBJDO(LOD -> raster))
   {
      return LOD -> raster;
   }
   else if (F_OBJDO(LOD -> gradient))
   {
      return LOD -> gradient;
   }
   else if (F_OBJDO(LOD -> picture))
   {
      return LOD -> picture;
   }
   else if (F_OBJDO(LOD -> brush))
   {
      return LOD -> brush;
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

      if (group)
      {
         uint32 page1=0,page2=0;
         
         if (group == LOD -> raster)
         {
            page1 = 1;
         }
         else if (group == LOD -> gradient)
         {
            page1 = 2;
         }
         else if (group == LOD -> picture)
         {
            page1 = 3;
         }
         else if (group == LOD -> brush)
         {
            page1 = 3; page2 = 1;
         }
 
         F_Do(group,FM_Set, FA_NoNotify,TRUE, "Spec",Msg -> Spec, TAG_DONE);
         
//         F_Log(0,"PAGE1 (%ld) PAGE2 (%ld)",page1,page2);

         if (page1 == 3)
         {
            F_Set(LOD -> page_bitmap,FA_Group_ActivePage,page2);
         }

         F_Set(LOD -> page,FA_Group_ActivePage,page1);
      }
   }
   else
   {
      F_Set(LOD -> page,FA_Group_ActivePage,0);

      F_Do(LOD -> color,    FM_Set,FA_NoNotify,TRUE,"Spec",NULL,TAG_DONE);
      F_Do(LOD -> raster,   FM_Set,FA_NoNotify,TRUE,"Spec",NULL,TAG_DONE);
      F_Do(LOD -> gradient, FM_Set,FA_NoNotify,TRUE,"Spec",NULL,TAG_DONE);
      F_Do(LOD -> picture,  FM_Set,FA_NoNotify,TRUE,"Spec",NULL,TAG_DONE);
      F_Do(LOD -> brush,    FM_Set,FA_NoNotify,TRUE,"Spec",NULL,TAG_DONE);
   }
}
//+


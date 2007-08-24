#include "Private.h"

enum  {

      FV_XML_ID_IMAGE = FV_XMLDOCUMENT_ID_DUMMY,
      FV_XML_ID_TYPE,
      FV_XML_ID_BRUSH,
      FV_XML_ID_SRC

      };

STATIC FDOCID xml_attributes_ids[] =
{
   { "image",     5, FV_XML_ID_IMAGE      },
   { "type",      4, FV_XML_ID_TYPE       },
   { "src",       3, FV_XML_ID_SRC        },

   F_ARRAY_END
};

STATIC FDOCValue xml_attributes_values[] =
{
   { "brush",        FV_XML_ID_BRUSH      },

   F_ARRAY_END
};

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SuperDo(Class,Obj,Method,

      "PreviewTitle", "Brush",
      "PreviewClass", "PreviewImage",
      "Separator",     FV_ImageDisplay_Separator,
      
      Child, VGroup,
         Child, LOD -> file = PopFileObject, End,
      End,

      TAG_MORE,Msg))
   {
      F_Do(LOD -> file,FM_Notify,"Contents",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

      return Obj;
   }

   return NULL;
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
   if (F_StrCmp("<image ",Msg -> Spec,7) == 0)
   {
      FObject doc;
      uint32  id_resolve;
      FXMLAttribute *attribute;

      attribute = (FXMLAttribute *) F_Do(Obj,F_IDR(FM_Adjust_ParseXML),Msg -> Spec,FV_Document_SourceType_Memory, xml_attributes_ids, FV_XML_ID_IMAGE, &doc,&id_resolve);

      if (attribute)
      {
         for ( ; attribute ; attribute = attribute -> Next)
         {
            switch (attribute -> Name -> ID)
            {
               case FV_XML_ID_TYPE:
               {
                  if (F_Do(doc,id_resolve,attribute -> Data,0,xml_attributes_values,NULL) == FV_XML_ID_BRUSH)
                  {
                     return TRUE;
                  }
               }
               break;
            }
         }
      }
   }
   return FALSE;
}
//+
///Adjust_ToString
F_METHODM(uint32,Adjust_ToString,FS_Adjust_ToString)
{
   if (Msg -> Notify)
   {
      STRPTR spec = F_StrNew(NULL,"<image type='brush' src='%s' />",Msg -> Spec);
      uint32 rc = F_SuperDo(Class,Obj,Method,spec,Msg -> Notify);

      F_Dispose(spec);

      return rc;
   }
   
   return F_SUPERDO(); 
}
//+
///Adjust_ToObject
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   STRPTR src=NULL;
   
   if (F_StrCmp("<image ",Msg -> Spec,7) == 0)
   {
      FObject doc;
      uint32  id_resolve;
      FXMLAttribute *attribute;

      attribute = (FXMLAttribute *) F_Do(Obj,F_IDR(FM_Adjust_ParseXML),Msg -> Spec,FV_Document_SourceType_Memory, xml_attributes_ids, FV_XML_ID_IMAGE, &doc,&id_resolve);

      if (attribute)
      {
         uint32 type=0;
 
         for ( ; attribute ; attribute = attribute -> Next)
         {
            switch (attribute -> Name -> ID)
            {
               case FV_XML_ID_TYPE: type = F_Do(doc,id_resolve,attribute -> Data,0,xml_attributes_values,NULL); break;
               case FV_XML_ID_SRC:  src = attribute -> Data; break;
            }
         }
      
         if (type != FV_XML_ID_BRUSH)
         {
            src = NULL;
         }
      }
   }
   
   F_Do(LOD -> file,FM_Set, FA_NoNotify,TRUE, "Contents",src, TAG_DONE);
}
//+

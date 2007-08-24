#include "Private.h"

#define COMPATIBILITY

///typedef
 
STATIC STRPTR cycle_entries_mode[] =
{
    "no-repeat",
    "repeat",
    "scale",

    F_ARRAY_END
};

enum    {
    
        FV_MODE_NOREPEAT,
        FV_MODE_REPEAT,
        FV_MODE_SCALE

        };
        
/***/
        
STATIC STRPTR cycle_entries_filter[] =
{
    "nearest",
    "average",
    "bilinear",
    
    F_ARRAY_END
};

enum    {
    
        FV_FILTER_NEAREST,
        FV_FILTER_AVERAGE,
        FV_FILTER_BILINEAR

        };
        
/***/

enum    {

        FV_XML_ID_IMAGE = FV_XMLDOCUMENT_ID_DUMMY,
        FV_XML_ID_TYPE,
        FV_XML_ID_PICTURE,
        FV_XML_ID_SRC,
        FV_XML_ID_MODE,
        FV_XML_ID_FILTER

        };

STATIC FDOCID xml_attributes_ids[] =
{
    { "image",  5,  FV_XML_ID_IMAGE     },
    { "type",   4,  FV_XML_ID_TYPE      },
    { "src",    3,  FV_XML_ID_SRC       },
    { "mode",   4,  FV_XML_ID_MODE      },
    { "filter", 5,  FV_XML_ID_FILTER    },

    F_ARRAY_END
};

STATIC FDOCValue xml_attributes_values[] =
{
    { "picture",    FV_XML_ID_PICTURE   },

    F_ARRAY_END
};
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (F_SuperDo(Class,Obj,Method,

        "PreviewTitle", "Picture",
        "PreviewClass", "PreviewImage",
        "Separator",     FV_ImageDisplay_Separator,
       
        Child, ColGroup(2),
            Child, HLabel("File"),
            Child, LOD->file = PopFileObject, End,
            Child, HLabel("Mode"),
            Child, LOD->mode = CycleObject, "Entries", cycle_entries_mode, "EntriesType", FV_Cycle_EntriesType_Array, End,
            Child, HLabel("Filter"),
            Child, LOD->filter = CycleObject, "Entries", cycle_entries_filter, "EntriesType", FV_Cycle_EntriesType_Array, End,
        End,

        TAG_MORE,Msg))
    {
        F_Do(LOD->file,FM_Notify,"Contents",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
        F_Do(LOD->mode,FM_Notify,"FA_Cycle_Active",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
        F_Do(LOD->filter,FM_Notify,"FA_Cycle_Active",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

        return Obj;
    }

    return NULL;
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
   if (Msg->Spec)
   {
      if (F_StrCmp("<image ",Msg->Spec,7) == 0)
      {
         FObject doc;
         uint32  id_resolve;
         FXMLAttribute *attribute;

         attribute = (FXMLAttribute *) F_Do(Obj,F_IDR(FM_Adjust_ParseXML),Msg->Spec,FV_Document_SourceType_Memory, xml_attributes_ids, FV_XML_ID_IMAGE, &doc,&id_resolve);

         if (attribute)
         {
            for ( ; attribute ; attribute = attribute->Next)
            {
               switch (attribute->Name->ID)
               {
                  case FV_XML_ID_TYPE:
                  {
                     if (F_Do(doc,id_resolve,attribute->Data,0,xml_attributes_values,NULL) == FV_XML_ID_PICTURE)
                     {
                        return TRUE;
                     }
                  }
                  break;
               }
            }
         }
      }
#ifdef COMPATIBILITY
      if (Msg->Spec[0] == 'P' && Msg->Spec[1] == ':')
      {
         return TRUE;
      }
#endif
   }
   return FALSE;
}
//+
///Adjust_ToString
F_METHODM(uint32,Adjust_ToString,FS_Adjust_ToString)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (Msg->Notify)
    {
        uint32 rc;
        STRPTR spec = NULL;
        STRPTR src = (STRPTR) F_Get(LOD->file, (uint32) "Contents");
        
        if (src)
        {
            spec = F_StrNew(NULL,"<image type='picture' src='%s' mode='%s' filter='%s' />",
                src,
                cycle_entries_mode[F_Get(LOD->mode, (uint32) "FA_Cycle_Active")],
                cycle_entries_filter[F_Get(LOD->filter, (uint32) "FA_Cycle_Active")]);
        }

        rc = F_SuperDo(Class,Obj,Method,spec,Msg->Notify);

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
    
    STRPTR src = NULL;
    uint32 mode = FV_MODE_NOREPEAT;
    uint32 filter = FV_FILTER_NEAREST;

    if (Msg->Spec)
    {
        if (F_StrCmp("<image ",Msg->Spec,7) == 0)
        {
            FObject doc;
            uint32  id_resolve;
            FXMLAttribute *attribute;

            attribute = (FXMLAttribute *) F_Do(Obj,F_IDR(FM_Adjust_ParseXML),Msg->Spec,FV_Document_SourceType_Memory, xml_attributes_ids, FV_XML_ID_IMAGE, &doc,&id_resolve);

            if (attribute)
            {
                for ( ; attribute ; attribute = attribute->Next)
                {
                    switch (attribute->Name->ID)
                    {
                        case FV_XML_ID_SRC:
                        {
                            src = attribute->Data;
                        }
                        break;
                      
                        case FV_XML_ID_MODE:
                        {
                            STATIC FDOCValue xml_values_mode[] =
                            {
                                "no-repeat",    FV_MODE_NOREPEAT,
                                "repeat",       FV_MODE_REPEAT,
                                "scale",        FV_MODE_SCALE,
                                
                                F_ARRAY_END
                            };
 
                            mode = F_Do(doc,id_resolve, attribute->Data, FV_TYPE_INTEGER, xml_values_mode, NULL);
                        }
                        break;
                    
                        case FV_XML_ID_FILTER:
                        {
                            STATIC FDOCValue xml_values_filter[] =
                            {
                                "nearest",      FV_FILTER_NEAREST,
                                "average",      FV_FILTER_AVERAGE,
                                "bilinear",     FV_FILTER_BILINEAR,
                                
                                F_ARRAY_END
                            };
                            
                            filter = F_Do(doc,id_resolve, attribute->Data, FV_TYPE_INTEGER, xml_values_filter, NULL);
                        }
                        break;
                    }
                }
            }
        }
#ifdef COMPATIBILITY
        else if (Msg->Spec[0] == 'P' && Msg->Spec[1] == ':')
        {
            src = Msg->Spec + 2;
        }
#endif
    }
  
    F_Do(LOD->file,FM_Set,FA_NoNotify,TRUE,"Contents",src,TAG_DONE);
    F_Do(LOD->mode,FM_Set,FA_NoNotify,TRUE,"FA_Cycle_Active",mode,TAG_DONE);
    F_Do(LOD->filter,FM_Set,FA_NoNotify,TRUE,"FA_Cycle_Active",filter,TAG_DONE);
}
//+

#include "Private.h"

#define COMPATIBILITY

#ifdef COMPATIBILITY
///id_clone_string
STATIC STRPTR id_clone_string(STRPTR *Source,UBYTE OpenChar,UBYTE CloseChar,struct FeelinBase *FeelinBase)
{
   if (!OpenChar || !CloseChar)
   {
      uint32 len = F_StrLen(*Source);
      
      if (len)
      {
         STRPTR copy = F_New(len + 1);
         
         if (copy)
         {
            CopyMem(*Source,copy,len); *Source += len;
            
            return copy;
         }
      }
   }
   else if (**Source == OpenChar)
   {
      STRPTR start = *Source += 1;

      while ((**Source != CloseChar) && (**Source)) *Source += 1;

      if (**Source == CloseChar)
      {
         STRPTR copy = F_New(*Source - start + 1);

         if (copy)
         {
            CopyMem(start,copy,*Source - start); *Source += 1;

            return copy;
         }
      }
   }
   return NULL;
}
//+
#endif

enum    {

        FV_CYCLE_TYPE_SIMPLE,
        FV_CYCLE_TYPE_TRIPLE,
        FV_CYCLE_TYPE_AQUA

        };

STATIC STRPTR cycle_angle_entries_full[] =
{
    "Vertical",
    "Vertical 2",
    "Horizontal",
    "Horizontal 2",
    "Diagonal",
    "Diagonal 2",
    "Custom",

    F_ARRAY_END
};

STATIC STRPTR cycle_angle_entries_restricted[] =
{
    "Vertical",
    "Vertical 2",
    "Horizontal",
    "Horizontal 2",
    
    F_ARRAY_END
};

enum    {
    
        FV_CYCLE_ANGLE_VERTICAL,
        FV_CYCLE_ANGLE_VERTICAL2,
        FV_CYCLE_ANGLE_HORIZONTAL,
        FV_CYCLE_ANGLE_HORIZONTAL2,
        FV_CYCLE_ANGLE_DIAGONAL,
        FV_CYCLE_ANGLE_DIAGONAL2,
        FV_CYCLE_ANGLE_CUSTOM

        };

STATIC STRPTR cycle_type_entries[] =
{
    "Simple",
    "Triple",
    "Aqua",

    F_ARRAY_END
};

///update_from_cycle_angle
void update_from_cycle_angle(FClass *Class,FObject Obj,uint32 Active,struct FeelinBase *FeelinBase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    uint32 angle;

    switch (Active)
    {
        case FV_CYCLE_ANGLE_VERTICAL:     angle = 0;    break;
        case FV_CYCLE_ANGLE_VERTICAL2:    angle = 180;  break;
        case FV_CYCLE_ANGLE_HORIZONTAL:   angle = 90;   break;
        case FV_CYCLE_ANGLE_HORIZONTAL2:  angle = 270;  break;
        case FV_CYCLE_ANGLE_DIAGONAL:     angle = 45;   break;
        case FV_CYCLE_ANGLE_DIAGONAL2:    angle = 225;  break;
    
        default: return;
    }

    F_Do(LOD->angle, FM_Set, FA_NoNotify,TRUE, "Value",angle, TAG_DONE);
}
//+
///update_from_cycle_type
void update_from_cycle_type(FClass *Class,FObject Obj,uint32 Active,struct FeelinBase *FeelinBase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 restrict_angle = TRUE;
 
    switch (Active)
    {
        case FV_CYCLE_TYPE_SIMPLE:
        {
            F_Do(Obj, FM_MultiSet, FA_Hidden, FF_Hidden_Check | TRUE, LOD->group2, LOD->group3, LOD->sep, NULL);
            
            restrict_angle = FALSE;
        }
        break;
         
        case FV_CYCLE_TYPE_TRIPLE:
        {
            F_Set(LOD->label2, FA_Text, (uint32) "middle");
 
            F_Set(LOD->group2, FA_Hidden, FF_Hidden_Check | FALSE);
            F_Do(Obj, FM_MultiSet, FA_Hidden, FF_Hidden_Check | TRUE, LOD->group3, LOD->sep, NULL);
        }
        break;

        case FV_CYCLE_TYPE_AQUA:
        {
            F_Set(LOD->label2, FA_Text, (uint32) "end");
            F_Set(LOD->label3, FA_Text, (uint32) "start");
            
            F_Do(Obj, FM_MultiSet, FA_Hidden, FF_Hidden_Check | FALSE, LOD->group2, LOD->group3, LOD->sep, NULL);
        }
        break;
    }

    F_Do(LOD->cycle_angle, FM_Set,
    
        FA_NoNotify, TRUE,
    
        "FA_Cycle_Entries", restrict_angle ? cycle_angle_entries_restricted : cycle_angle_entries_full,
        "FA_Cycle_EntriesType", "Array",
        
        TAG_DONE);

    F_Set(LOD->angle, FA_Disabled, FF_Disabled_Check | restrict_angle);
}
//+
///update_from_angle
void update_from_angle(FClass *Class,FObject Obj,uint32 Angle,struct FeelinBase *FeelinBase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    int32 active=FV_CYCLE_ANGLE_CUSTOM;

    switch (Angle)
    {
       case 0:     active = FV_CYCLE_ANGLE_VERTICAL; break;
       case 180:   active = FV_CYCLE_ANGLE_VERTICAL2; break;
       case 90:    active = FV_CYCLE_ANGLE_HORIZONTAL; break;
       case 270:   active = FV_CYCLE_ANGLE_HORIZONTAL2; break;
       case 45:    active = FV_CYCLE_ANGLE_DIAGONAL; break;
       case 225:   active = FV_CYCLE_ANGLE_DIAGONAL2; break;
    }

    F_Do(LOD->cycle_angle, FM_Set, FA_NoNotify,TRUE, "Active",active, TAG_DONE);
}
//+
///update_from_specs
void update_from_specs(FClass *Class,FObject Obj,STRPTR fss,STRPTR fes,STRPTR sss, STRPTR ses,struct FeelinBase *FeelinBase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    uint32 type = FV_CYCLE_TYPE_SIMPLE;
    
    if (fss && fes && sss && ses)
    {
        type = FV_CYCLE_TYPE_AQUA;
    }
    else if (fss && fes && ses)
    {
        type = FV_CYCLE_TYPE_TRIPLE;
    }
    
    F_Do(LOD->cycle_type, FM_Set, FA_NoNotify,TRUE, "FA_Cycle_Active", type, TAG_DONE);
    
    update_from_cycle_type(Class,Obj,type,FeelinBase);
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

enum    {
   
        NOTIFY_ANGLE,
        NOTIFY_CYCLE_ANGLE,
        NOTIFY_CYCLE_TYPE,
        NOTIFY_FIRST_START,
        NOTIFY_FIRST_END,
        NOTIFY_SECOND_START,
        NOTIFY_SECOND_END

        };
        
enum    {

        FV_XML_ID_IMAGE = FV_XMLDOCUMENT_ID_DUMMY,
        FV_XML_ID_TYPE,
        FV_XML_ID_GRADIENT,
        FV_XML_ID_ANGLE,
        
        FV_XML_ID_START,
        FV_XML_ID_END,
        FV_XML_ID_MIDDLE,
        
        FV_XML_ID_FIRST_START,
        FV_XML_ID_FIRST_END,
        FV_XML_ID_SECOND_START,
        FV_XML_ID_SECOND_END

        };
      
STATIC FDOCID xml_attributes_ids[] =
{
   { "image",            5, FV_XML_ID_IMAGE         },
   { "type",             4, FV_XML_ID_TYPE          },
   { "gradient",         8, FV_XML_ID_GRADIENT      },
   { "angle",            5, FV_XML_ID_ANGLE         },
   { "start",            5, FV_XML_ID_START         },
   { "end",              3, FV_XML_ID_END           },
   { "middle",           6, FV_XML_ID_MIDDLE        },
   { "first-start",     11, FV_XML_ID_FIRST_START   },
   { "first-end",        9, FV_XML_ID_FIRST_END     },
   { "second-start",    12, FV_XML_ID_SECOND_START  },
   { "second-end",      10, FV_XML_ID_SECOND_END    },

   F_ARRAY_END
};

STATIC FDOCValue xml_attributes_values[] =
{
   { "gradient",     FV_XML_ID_GRADIENT   },
      
   F_ARRAY_END
};
 
///Adjust_New
F_METHOD(FObject,Adjust_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (F_SuperDo(Class,Obj,Method,
                                
        "PreviewTitle", F_CAT(GRADIENT),
        "PreviewClass", "PreviewImage",
        "Separator",     FV_ImageDisplay_Separator,
        
        Child, VGroup,
            Child, VGroup, GroupFrame, FA_Frame_Title,F_CAT(ANGLE),
                Child, LOD->angle = SliderObject, "Max",359, "Horizontal",TRUE, FA_SetMax,FV_SetHeight, End,
                Child, LOD->cycle_angle = CycleObject, "FA_Cycle_Entries",cycle_angle_entries_full, "FA_Cycle_EntriesType",FV_Cycle_EntriesType_Array, FA_SetMax,FV_SetHeight, End,
            End,

            Child, VGroup, GroupFrame, FA_Frame_Title,F_CAT(COLORS),
                Child, HGroup,
                    Child, HLabel("Type:"),
                    Child, LOD->cycle_type = CycleObject, "FA_Cycle_Entries",cycle_type_entries , "FA_Cycle_EntriesType",FV_Cycle_EntriesType_Array, FA_SetMax,FV_SetHeight, End,
                End,

                Child, HGroup,
                    Child, LOD->group1 = VGroup, FA_Group_VSpacing,2,
                        Child, LOD->spec1 = PopColorObject, End,
                        Child, LOD->label1 = VLabel("start"),
                    End,
                    
                    Child, LOD->group2 = VGroup, FA_Group_VSpacing,2, FA_Hidden,TRUE,
                        Child, LOD->spec2 = PopColorObject, End,
                        Child, LOD->label2 = VLabel("end"),
                    End,
                    
                    Child, LOD->sep = BarObject, FA_Hidden,TRUE, End,
                        
                    Child, LOD->group3 = VGroup, FA_Group_VSpacing,2, FA_Hidden,TRUE,
                        Child, LOD->spec3 = PopColorObject, End,
                        Child, LOD->label3 = VLabel("start"),
                    End,
                    
                    Child, LOD->group4 = VGroup, FA_Group_VSpacing,2,
                        Child, LOD->spec4 = PopColorObject, End,
                        Child, LOD->label4 = VLabel("end"),
                    End,
                End,
            End,
        End,
        
        TAG_MORE,Msg))
    {
        F_Do(LOD->angle,FM_Notify,"Value", FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_ANGLE);
        F_Do(LOD->spec1,FM_Notify,"Spec", FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_FIRST_START);
        F_Do(LOD->spec2,FM_Notify,"Spec", FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_FIRST_END);
        F_Do(LOD->spec3,FM_Notify,"Spec", FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_SECOND_START);
        F_Do(LOD->spec4,FM_Notify,"Spec", FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_SECOND_END);
        F_Do(LOD->cycle_angle,FM_Notify,"Active", FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_CYCLE_ANGLE);
        F_Do(LOD->cycle_type,FM_Notify,"Active", FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_CYCLE_TYPE);

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
                            if (F_Do(doc,id_resolve,attribute->Data,0,xml_attributes_values,NULL) == FV_XML_ID_GRADIENT)
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
        else if ((Msg->Spec[0] == 'g' && Msg->Spec[1] == ':'))
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
        STRPTR spec1 = (STRPTR) F_Get(LOD->spec1, (uint32) "Spec");
        STRPTR spec2 = (STRPTR) F_Get(LOD->spec2, (uint32) "Spec");
        STRPTR spec3 = (STRPTR) F_Get(LOD->spec3, (uint32) "Spec");
        STRPTR spec4 = (STRPTR) F_Get(LOD->spec4, (uint32) "Spec");

        switch (Msg->UserData)
        {
            case NOTIFY_CYCLE_ANGLE:
            {
                update_from_cycle_angle(Class, Obj, (uint32) Msg->Spec, FeelinBase);
            }
            break;
        
            case NOTIFY_CYCLE_TYPE:
            {
                update_from_cycle_type(Class, Obj, (uint32) Msg->Spec, FeelinBase);
            }
            break;
 
            case NOTIFY_ANGLE:
            {
                update_from_angle(Class,Obj,(uint32) Msg->Spec,FeelinBase);
            }
            break;
        }
    
        if (spec1 && spec4)
        {
            uint32 angle = F_Get(LOD->angle,(uint32) "Value");
            uint32 type = F_Get(LOD->cycle_type, (uint32) "FA_Cycle_Active");
            STRPTR spec=NULL;
            uint32 rc;

            if (type == FV_CYCLE_TYPE_AQUA && spec2 && spec3)
            {
                spec = F_StrNew(NULL,"<image type='gradient' angle='%ld' first-start='%s' first-end='%s' second-start='%s' second-end='%s' />",angle,spec1,spec2,spec3,spec4);
            }
            else if (type == FV_CYCLE_TYPE_TRIPLE && spec2)
            {
                spec = F_StrNew(NULL,"<image type='gradient' angle='%ld' start='%s' middle='%s' end='%s' />",angle,spec1,spec2,spec4);
            }
            else
            {
                spec = F_StrNew(NULL,"<image type='gradient' angle='%ld' start='%s' end='%s' />",angle,spec1,spec4);
            }

            rc = F_SuperDo(Class,Obj,Method,spec,Msg->Notify);

            F_Dispose(spec);

            return rc;
        }
        
        return NULL;
    }

    return F_SUPERDO();
}
//+
///Adjust_ToObject
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 angle=0;
    uint32 type=FV_CYCLE_TYPE_SIMPLE;
    STRPTR spec1=NULL;
    STRPTR spec2=NULL;
    STRPTR spec3=NULL;
    STRPTR spec4=NULL;

    if (Msg->Spec)
    {
        if (F_StrCmp("<image ",Msg->Spec,7) == 0)
        {
            FObject doc;
            uint32 id_resolve;
            FXMLAttribute *attribute;

            attribute = (FXMLAttribute *) F_Do(Obj,F_IDR(FM_Adjust_ParseXML),Msg->Spec,FV_Document_SourceType_Memory, xml_attributes_ids, FV_XML_ID_IMAGE, &doc,&id_resolve);
            
            if (attribute)
            {
                for ( ; attribute ; attribute = attribute->Next)
                {
                    switch (attribute->Name->ID)
                    {
                        case FV_XML_ID_ANGLE:
                        {
                            angle = F_Do(doc,id_resolve,attribute->Data,FV_TYPE_INTEGER,NULL,NULL);
                        }
                        break;
                        
                        case FV_XML_ID_FIRST_START:
                        {
                            type = FV_CYCLE_TYPE_AQUA;
                        }
                        case FV_XML_ID_START:
                        {
                            spec1 = attribute->Data;
                        }
                        break;
                        
                        case FV_XML_ID_MIDDLE:
                        {
                            type = FV_CYCLE_TYPE_TRIPLE;
                            spec2 = attribute->Data;
                        }
                        break;
                    
                        case FV_XML_ID_FIRST_END:
                        {
                            type = FV_CYCLE_TYPE_AQUA;
                            spec2 = attribute->Data;
                        }
                        break;
                        
                        case FV_XML_ID_SECOND_START:
                        {
                            type = FV_CYCLE_TYPE_AQUA;
                            spec3 = attribute->Data;
                        }
                        break;

                        case FV_XML_ID_SECOND_END:
                        {
                            type = FV_CYCLE_TYPE_AQUA;
                        }
                        case FV_XML_ID_END:
                        {
                            spec4 = attribute->Data;
                        }
                        break;
                    }
                }
            }
        }
///COMPATIBILITY
#ifdef COMPATIBILITY
        else if (Msg->Spec[0] == 'G' && Msg->Spec[1] == ':')
        {
            STRPTR String = Msg->Spec + 2;
            STRPTR a = id_clone_string(&String,'[',']',FeelinBase);
            
            if (a)
            {
                STRPTR sspec = id_clone_string(&String,'(',')',FeelinBase);

                if (sspec)
                {
                    STRPTR espec = id_clone_string(&String,'(',')',FeelinBase);

                    if (espec)
                    {
                        F_Do(LOD->angle,FM_SetAs, FV_SetAs_Decimal,"Value",angle);
                        F_Do(LOD->spec1,FM_Set, FA_NoNotify,TRUE, "Spec",spec1, TAG_DONE);
                        F_Do(LOD->spec4,FM_Set, FA_NoNotify,TRUE, "Spec",spec4, TAG_DONE);
                        
                        angle = F_Get(LOD->angle,(uint32) "Value");
                        spec1 = (STRPTR) F_Get(LOD->spec1,(uint32) "Spec");
                        spec4 = (STRPTR) F_Get(LOD->spec4,(uint32) "Spec");

                        F_Dispose(espec);
                    }
                    F_Dispose(sspec);
                }
                F_Dispose(a);
            }
        }
#endif
//+
    }

//    F_Log(0,"spec1 (%s) spec2 (%s) spec3 (%s) spec4 (%s) angle (%s)",spec1,spec2,spec3,spec4,angle);
 
    F_Do(LOD->angle,FM_Set, FA_NoNotify,TRUE, "Value",angle, TAG_DONE);

    update_from_angle(Class,Obj,angle,FeelinBase);

    F_Do(LOD->cycle_type,FM_Set, FA_NoNotify,TRUE, "Active",type, TAG_DONE);
    
    update_from_cycle_type(Class,Obj,type,FeelinBase);
    
    F_Do(LOD->spec1,FM_Set, FA_NoNotify,TRUE, "Spec",spec1, TAG_DONE);
    F_Do(LOD->spec2,FM_Set, FA_NoNotify,TRUE, "Spec",spec2, TAG_DONE);
    F_Do(LOD->spec3,FM_Set, FA_NoNotify,TRUE, "Spec",spec3, TAG_DONE);
    F_Do(LOD->spec4,FM_Set, FA_NoNotify,TRUE, "Spec",spec4, TAG_DONE);
}
//+


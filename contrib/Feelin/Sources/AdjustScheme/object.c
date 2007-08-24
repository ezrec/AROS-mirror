#include "Private.h"

///select_disabled
void select_disabled(FClass *Class,FObject Obj,struct FeelinBase *FeelinBase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    STRPTR spec = (STRPTR) F_Get(LOD -> fill,F_IDO(FA_Preview_Spec));

    if (F_Get(LOD -> edit,FA_Selected))
    {
        F_Set(LOD -> shine,FA_Disabled,FF_Disabled_Check | FALSE);
        F_Set(LOD -> dark,FA_Disabled,FF_Disabled_Check | FALSE);
        F_Set(LOD -> adjust,FA_Disabled,FF_Disabled_Check | TRUE);

/*
      if (F_Get(LOD -> shine,FA_Disabled))   F_Set(LOD -> shine,FA_Disabled,FALSE);
      if (F_Get(LOD -> dark,FA_Disabled))    F_Set(LOD -> dark,FA_Disabled,FALSE);
      if (!F_Get(LOD -> adjust,FA_Disabled)) F_Set(LOD -> adjust,FA_Disabled,TRUE);
*/
    }
    else
    {
        F_Set(LOD -> shine,FA_Disabled,FF_Disabled_Check | TRUE);
        F_Set(LOD -> dark,FA_Disabled,FF_Disabled_Check | TRUE);
/*
      if (!F_Get(LOD -> shine,FA_Disabled))  F_Set(LOD -> shine,FA_Disabled,TRUE);
      if (!F_Get(LOD -> dark,FA_Disabled))   F_Set(LOD -> dark,FA_Disabled,TRUE);
*/
      if (spec)
      {
//         if (F_Get(LOD -> adjust,FA_Disabled))  F_Set(LOD -> adjust,FA_Disabled,FALSE);
        F_Set(LOD -> adjust,FA_Disabled,FF_Disabled_Check | FALSE);
      }
   }

   if (!spec/* && !F_Get(LOD -> adjust,FA_Disabled)*/)
   {
//      F_Set(LOD -> adjust,FA_Disabled,TRUE);
      F_Set(LOD -> adjust,FA_Disabled,FF_Disabled_Check | TRUE);
   }
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_AREA_SAVE_PUBLIC;
 
   /* create object */

   if (F_SuperDo(Class,Obj,Method,

      "PreviewClass",   "PreviewScheme",
      "PreviewTitle",   "Scheme",
      "Separator",      0,

      /* */

       Child, VGroup, 
         Child, VGroup, GroupFrame, FA_Frame_Title, "Colors",
            Child, RowGroup(2), FA_Group_SameWidth,TRUE, FA_Group_VSpacing,2,
               Child, LOD -> fill = PopColorObject, End,
               Child, LOD -> text = PopColorObject, End,
               Child, LOD -> high = PopColorObject, End,
               Child, VLabel("Fill"),
               Child, VLabel("Text"),
               Child, VLabel("Highlight"),
            End,
            
            Child, BarObject, End,
            
            Child, RowGroup(2), FA_Group_SameWidth,TRUE, FA_Group_VSpacing,2,
               Child, LOD -> edit = Checkbox(FALSE),
               Child, LOD -> shine = PopColorObject, FA_Disabled,TRUE,End,
               Child, LOD -> dark = PopColorObject, FA_Disabled,TRUE,End,
               Child, VLabel("Edit"),
               Child, VLabel("Shine"),
               Child, VLabel("Dark"),
            End,
         End,
             

         Child, LOD -> adjust = RowGroup(2), FA_Disabled,TRUE, GroupFrame, FA_Frame_Title, "Adjust",
            Child, HLabel("Contrast"),
            Child, LOD -> contrast = SliderObject, FA_Horizontal,TRUE, FA_SetMax,FV_SetHeight, "Min",-100, "Max",100, "Value",0, End,
            Child, HLabel("Saturation"),
            Child, LOD -> saturation = SliderObject, FA_Horizontal,TRUE, FA_SetMax,FV_SetHeight, "Max",100, "Value",100, End,
         End,
      End,

   TAG_MORE, Msg))
   {
      F_Do(LOD -> shine, FM_Notify,F_IDO(FA_Preview_Spec),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> fill, FM_Notify,F_IDO(FA_Preview_Spec),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> dark, FM_Notify,F_IDO(FA_Preview_Spec),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> text, FM_Notify,F_IDO(FA_Preview_Spec),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> high, FM_Notify,F_IDO(FA_Preview_Spec),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

      F_Do(LOD -> edit, FM_Notify,FA_Selected,FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> contrast, FM_Notify,"Value",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> saturation, FM_Notify,"Value",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

      return Obj;
   }
   return NULL;
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
   if (Msg -> Spec)
   {
      if (F_StrCmp("<scheme ",Msg -> Spec,8) == 0)
      {
         return TRUE;
      }
   }
   return FALSE;
}
//+
///Adjust_ToString
F_METHODM(uint32,Adjust_ToString,FS_Adjust_ToString)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg -> Notify)
    {
        STRPTR spec = F_New(1024);

        select_disabled(Class,Obj,FeelinBase);
        
        if (spec)
        {
            STRPTR fill = (STRPTR) F_Get(LOD->fill, F_IDO(FA_Preview_Spec));
            STRPTR shine = (STRPTR) F_Get(LOD->shine, F_IDO(FA_Preview_Spec));
            STRPTR dark = (STRPTR) F_Get(LOD->dark, F_IDO(FA_Preview_Spec));
            STRPTR text = (STRPTR) F_Get(LOD->text, F_IDO(FA_Preview_Spec));
            STRPTR high = (STRPTR) F_Get(LOD->high, F_IDO(FA_Preview_Spec));
            STRPTR buf;
            uint32 rc;
                    
            if (fill || shine || dark || text || high)
            {
                buf = F_StrFmt(spec,"<scheme");
                
                if (fill)
                {
                   buf = F_StrFmt(buf," fill='%s'", fill);
                }
                
                if (F_Get(LOD -> edit,FA_Selected))
                {
                   if (shine)
                   {
                      buf = F_StrFmt(buf," shine='%s'", shine);
                   }

                   if (dark)
                   {
                      buf = F_StrFmt(buf," dark='%s'", dark);
                   }
                }
                else
                {
                   buf = F_StrFmt(buf," contrast='%ld'",F_Get(LOD -> contrast,(uint32) "Value"));
                   
                   rc = F_Get(LOD -> saturation,(uint32) "Value");

                   /* At 100, the 'saturation' attribute is useless */
        
                   if (rc != 100)
                   {
                      buf = F_StrFmt(buf," saturation='%ld'",rc);
                   }
                }
             
                if (text)
                {
                   buf = F_StrFmt(buf," text='%s'", text);
                }

                if (high)
                {
                   buf = F_StrFmt(buf," highlight='%s'", high);
                }

                F_StrFmt(buf,"/>");
            }
            
            rc = F_SuperDo(Class,Obj,Method,spec,Msg -> Notify);

            F_Dispose(spec);
            
            return rc;
        }
    }
    
    return F_SUPERDO();
}
//+
///Adjust_ToObject

enum  {

      FV_XML_ID_DARK = FV_XMLDOCUMENT_ID_DUMMY,
      FV_XML_ID_FILL,
      FV_XML_ID_HIGHLIGHT,
      FV_XML_ID_SCHEME,
      FV_XML_ID_SHINE,
      FV_XML_ID_TEXT,
      FV_XML_ID_CONTRAST,
      FV_XML_ID_SATURATION

      };
      
#define FF_DONE_DARK                            (1 << 0)
#define FF_DONE_FILL                            (1 << 1)
#define FF_DONE_HIGHLIGHT                       (1 << 2)
#define FF_DONE_SHINE                           (1 << 3)
#define FF_DONE_TEXT                            (1 << 4)
#define FF_DONE_CONTRAST                        (1 << 5)
#define FF_DONE_SATURATION                      (1 << 6)

F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   bits32 done=0;

   if (Msg -> Spec)
   {
      STATIC FDOCID xml_attributes_ids[] =
      {
         { "contrast",    8, FV_XML_ID_CONTRAST   },
         { "dark",        4, FV_XML_ID_DARK       },
         { "fill",        4, FV_XML_ID_FILL       },
         { "highlight",   9, FV_XML_ID_HIGHLIGHT  },
         { "saturation", 10, FV_XML_ID_SATURATION },
         { "scheme",      6, FV_XML_ID_SCHEME     },
         { "shine",       5, FV_XML_ID_SHINE      },
         { "text",        4, FV_XML_ID_TEXT       },

         F_ARRAY_END
      };

      FObject doc;
      uint32 id_resolve;
      FXMLAttribute *attribute;

      attribute = (FXMLAttribute *) F_Do(Obj,F_IDR(FM_Adjust_ParseXML),Msg -> Spec,FV_Document_SourceType_Memory, xml_attributes_ids, FV_XML_ID_SCHEME, &doc,&id_resolve);

      if (attribute)
      {
         for ( ; attribute ; attribute = attribute -> Next)
         {
            FObject dst=NULL;
    
            switch (attribute -> Name -> ID)
            {
               case FV_XML_ID_DARK:       dst = LOD -> dark;         done |= FF_DONE_DARK; break;
               case FV_XML_ID_SHINE:      dst = LOD -> shine;        done |= FF_DONE_SHINE; break;
               case FV_XML_ID_FILL:       dst = LOD -> fill;         done |= FF_DONE_FILL; break;
               case FV_XML_ID_TEXT:       dst = LOD -> text;         done |= FF_DONE_TEXT; break;
               case FV_XML_ID_HIGHLIGHT:  dst = LOD -> high;         done |= FF_DONE_HIGHLIGHT; break;
               case FV_XML_ID_CONTRAST:   dst = LOD -> contrast;     done |= FF_DONE_CONTRAST; break;
               case FV_XML_ID_SATURATION: dst = LOD -> saturation;   done |= FF_DONE_SATURATION; break;
            }
         
            if (dst)
            {
               if (attribute -> Name -> ID == FV_XML_ID_CONTRAST ||
                   attribute -> Name -> ID == FV_XML_ID_SATURATION)
               {
                  F_Do(dst,FM_SetAs,FV_SetAs_Decimal,"Value",attribute -> Data);
               }
               else
               {
                  F_Do(dst,FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Preview_Spec),attribute -> Data, TAG_DONE);
               }
            }
         }
      }
   }
   
   if (!(FF_DONE_DARK & done))
   {
      F_Do(LOD -> dark,FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Preview_Spec),NULL, TAG_DONE);
   }
   if (!(FF_DONE_FILL & done))
   {
      F_Do(LOD -> fill,FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Preview_Spec),NULL, TAG_DONE);
   }
   if (!(FF_DONE_HIGHLIGHT & done))
   {
      F_Do(LOD -> high,FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Preview_Spec),NULL, TAG_DONE);
   }
   if (!(FF_DONE_SHINE & done))
   {
      F_Do(LOD -> shine,FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Preview_Spec),NULL, TAG_DONE);
   }
   if (!(FF_DONE_TEXT & done))
   {
      F_Do(LOD -> text,FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Preview_Spec),NULL, TAG_DONE);
   }
   if (!(FF_DONE_CONTRAST & done))
   {
      F_Do(LOD -> contrast,FM_Set, FA_NoNotify,TRUE, "Value",0, TAG_DONE);
   }
   if (!(FF_DONE_SATURATION & done))
   {
      F_Do(LOD -> saturation,FM_Set, FA_NoNotify,TRUE, "Value",100, TAG_DONE);
   }

   F_Do(LOD -> edit,FM_Set, FA_NoNotify,TRUE, FA_Selected,(done & (FF_DONE_DARK | FF_DONE_SHINE)) ? TRUE : FALSE, TAG_DONE);
   
   select_disabled(Class,Obj,FeelinBase);
}
//+

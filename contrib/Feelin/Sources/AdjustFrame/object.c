#include "Private.h"

#define COMPATIBILITY
         
enum    {

        FV_XML_ID_FRAME = FV_XMLDOCUMENT_ID_DUMMY,
        FV_XML_ID_ID,
        FV_XML_ID_PADDING,
        FV_XML_ID_PADDING_WIDTH,
        FV_XML_ID_PADDING_HEIGHT,
        FV_XML_ID_PADDING_LEFT,
        FV_XML_ID_PADDING_RIGHT,
        FV_XML_ID_PADDING_TOP,
        FV_XML_ID_PADDING_BOTTOM

        };

STATIC FDOCID xml_attributes_ids[] =
{
   { "frame",           5, FV_XML_ID_FRAME            },
   { "id",              2, FV_XML_ID_ID               },
   { "padding",         7, FV_XML_ID_PADDING          },
   { "padding-width",  13, FV_XML_ID_PADDING_WIDTH    },
   { "padding-height", 14, FV_XML_ID_PADDING_HEIGHT   },
   { "padding-left",   12, FV_XML_ID_PADDING_LEFT     },
   { "padding-right",  13, FV_XML_ID_PADDING_RIGHT    },
   { "padding-top",    11, FV_XML_ID_PADDING_TOP      },
   { "padding-bottom", 14, FV_XML_ID_PADDING_BOTTOM   },

   F_ARRAY_END
};

STATIC struct in_FeelinSample samples_specs[] =
{
   { 0x07,0x08, "<frame id='#07'/>\n<frame id='#08'/>","fill"              },
   { 0x0B,0x0C, "<frame id='#0B'/>\n<frame id='#0C'/>","fill"              },
   { 0x0D,0x0E, "<frame id='#0D'/>\n<frame id='#0E'/>","fill"              },
   { 0x0F,0x10, "<frame id='#0F'/>\n<frame id='#10'/>","fill"              },
   { 0x11,0x13, "<frame id='#11'/>\n<frame id='#13'/>","fill"              },
   { 0x14,0x16, "<frame id='#14'/>\n<frame id='#16'/>","fill"              },
   { 0x17,0x18, "<frame id='#17'/>\n<frame id='#18'/>","fill"              },
   { 0x19,0x1A, "<frame id='#19'/>\n<frame id='#1A'/>","fill"              },
   { 0x1B,0x1C, "<frame id='#1B'/>\n<frame id='#1C'/>","fill"              },
   { 0x1D,0x1E, "<frame id='#1D'/>\n<frame id='#1E'/>","fill"              },
   { 0x1F,0x20, "<frame id='#1F'/>\n<frame id='#20'/>","fill"              },
   { 0x23,0x24, "<frame id='#23'/>\n<frame id='#24'/>","fill;halfshadow"   },
   { 0x27,0x28, "<frame id='#27'/>\n<frame id='#28'/>","fill;halfshadow"   },
   { 0x29,0x05, "<frame id='#29'/>\n<frame id='#05'/>","fill;dark"         },
   { 0x2A,0x2B, "<frame id='#2A'/>\n<frame id='#2B'/>","fill"              },
   { 0x2F,0x2F, "<frame id='#2F'/>\n<frame id='#2F'/>","dark;shine"        },
   
   F_ARRAY_END
};

///Sample_
STATIC FObject Sample_(struct LocalObjectData *LOD,uint16 id,struct FeelinBase *FeelinBase)
{
   return LOD -> sample[id] = AreaObject,
      
      FA_ChainToCycle,     FALSE,
      FA_InputMode,        FV_InputMode_Toggle,
      FA_Draggable,        TRUE,
      FA_Dropable,         FALSE,
      FA_ContextHelp,      NULL,
      FA_Back,             samples_specs[id].backspec,
      FA_MinWidth,         10,
      FA_MinHeight,        10,
      FA_Frame,            samples_specs[id].framespec,
      
      End;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SuperDo(Class,Obj,Method,

      "PreviewClass",  "PreviewFrame",
      "PreviewTitle",  "Frame",
      "Separator",      FV_Frame_Separator,
      
      /* */
 
      Child, VGroup,
         Child, ColGroup(4), GroupFrame, FA_Frame_Title, F_CAT(SAMPLES), FA_ContextHelp, F_CAT(SAMPLES_HELP),
            Child, Sample_(LOD, 0,FeelinBase),
            Child, Sample_(LOD, 1,FeelinBase),
            Child, Sample_(LOD, 2,FeelinBase),
            Child, Sample_(LOD, 3,FeelinBase),
            Child, Sample_(LOD, 4,FeelinBase),
            Child, Sample_(LOD, 5,FeelinBase),
            Child, Sample_(LOD, 6,FeelinBase),
            Child, Sample_(LOD, 7,FeelinBase),
            Child, Sample_(LOD, 8,FeelinBase),
            Child, Sample_(LOD, 9,FeelinBase),
            Child, Sample_(LOD,10,FeelinBase),
            Child, Sample_(LOD,11,FeelinBase),
            Child, Sample_(LOD,12,FeelinBase),
            Child, Sample_(LOD,13,FeelinBase),
            Child, Sample_(LOD,14,FeelinBase),
            Child, Sample_(LOD,15,FeelinBase),
         End,

         Child, LOD -> slider = F_MakeObj(FV_MakeObj_Slider,TRUE, 0,53,0, FA_SetMax,FV_SetHeight,TAG_DONE),

         Child, LOD -> spacing = AdjustSpacingObject, End,
      End,

      TAG_MORE,Msg))
   {
      F_Do(LOD -> slider,FM_Notify,"Value",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
      F_Do(LOD -> spacing,FM_Notify,"Spec", FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

      return Obj;
   }

   return NULL;
}
//+
///Adjust_DnDQuery
F_METHOD_DNDQUERY(Adjust_DnDQuery)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   FObject preview = (FObject) F_Get(Obj,F_IDR(FA_Adjust_Preview));

   if (preview && (Msg -> Source != preview))
   {
      FObject *sample;
      uint32 i;

      for (i = 0,sample = (FObject *) &LOD -> sample ; *sample ; i++, sample++)
      {
         if (*sample == Msg -> Source)
         {
            break;
         }
      }
      
      if (*sample)
      {
         FAreaPublic * ap = (FAreaPublic *) F_Get(preview,FA_Area_PublicData);
 
         if (Msg -> MouseX >= ap -> Box.x &&
             Msg -> MouseX <= ap -> Box.x + ap -> Box.w - 1 &&
             Msg -> MouseY >= ap -> Box.y &&
             Msg -> MouseY <= ap -> Box.y + ap -> Box.h - 1)
         {
            Msg -> Box -> x = ap -> Box.x; Msg -> Box -> w = ap -> Box.w;
            Msg -> Box -> y = ap -> Box.y; Msg -> Box -> h = ap -> Box.h;
 
//            F_Log(0,"sample (%ld)(%s)",i,samples_specs[i].framespec);
            
            return Obj;
         }
      }
   }
   return (FObject) F_SUPERDO();
}
//+
///Adjust_DnDDrop
F_METHOD_DNDDROP(Adjust_DnDDrop)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   FObject *sample;
   uint32 i;

   for (i = 0,sample = (FObject *) &LOD -> sample ; *sample ; i++,sample++)
   {
      if (*sample == Msg -> Source)
      {
         break;
      }
   }

   if (*sample)
   {
      STRPTR buf = F_New(1024);
      
      if (buf)
      {
         STRPTR spec = (STRPTR) F_Get(Obj,F_IDR(FA_Adjust_Spec));
 
         FObject doc;
         uint32  id_resolve;
         FXMLAttribute *attribute = (FXMLAttribute *) F_Do(Obj,F_IDR(FM_Adjust_ParseXML),spec,FV_Document_SourceType_Memory, xml_attributes_ids, FV_XML_ID_FRAME, &doc,&id_resolve);

//         F_Log(0,"sample (%ld) (%s) with (%s)",i,samples_specs[i].framespec,F_Get(Obj,(uint32) "Spec"));

         spec = buf;
         
         if (attribute)
         {
            FXMLMarkup *markup;
            
            int8 render_done = FALSE;
            
            for (markup = (FXMLMarkup *) F_Get(doc,(uint32) "FA_XMLDocument_Markups") ; markup ; markup = markup -> Next)
            {
               if (markup -> Name -> ID == FV_XML_ID_FRAME)
               {
                  if (render_done)
                  {
                     
                     /* a previous frame  has  already  been  written,  the
                     separator  character  must  be added before writting a
                     new frame */
                  
                     buf = F_StrFmt(buf,"%lc<%s",FV_Frame_Separator,markup -> Name -> Key);
                  }
                  else
                  {
                     buf = F_StrFmt(buf,"<%s",markup -> Name -> Key);
                  }
               
//                  F_Log(0,"MARKUP (%s)(0x%08lx)",markup -> Name -> Key,markup -> Name -> ID);
               
                  for (attribute = (FXMLAttribute *) markup -> AttributesList.Head ; attribute ; attribute =  attribute -> Next)
                  {
//                     F_Log(0,"ATTRIBUTE (%s)(0x%08lx) >> (%s)",attribute -> Name -> Key,attribute -> Name -> ID,attribute -> Data);
                     
                     if (attribute -> Name -> ID == FV_XML_ID_ID)
                     {
                        buf = F_StrFmt(buf," %s='%ld'",attribute -> Name -> Key,(render_done) ? samples_specs[i].select_id : samples_specs[i].render_id);
                     }
                     else
                     {
                        buf = F_StrFmt(buf," %s='%s'",attribute -> Name -> Key,attribute -> Data);
                     }
                  }
               
                  buf = F_StrFmt(buf,"/>");
               
                  render_done = TRUE;
               }
            }
         }
         else
         {
            buf = F_StrFmt(buf,"<frame id='%ld' />%lc<frame id='%ld' />",samples_specs[i].render_id,FV_Frame_Separator,samples_specs[i].select_id);
         }

//         F_Log(0,">>> %s",spec);
      
         F_Set(Obj,F_IDR(FA_Adjust_Spec),(uint32) spec);
      
         F_Dispose(spec);
      }
   }
   else
   {
      F_SUPERDO();
   }
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
   if (Msg -> Spec)
   {
      if (F_StrCmp("<frame ",Msg -> Spec,7) == 0)
      {
         return TRUE;
      }
#ifdef COMPATIBILITY
      else
      {
         int32 val;
         uint32 len = stch_l(Msg -> Spec,&val);

         if (len)
         {
            if (Msg -> Spec[len] == '.')
            {
               return TRUE;
            }
         }
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
   
   if (Msg -> Notify)
   {
      uint32 rc=0;
      STRPTR spec = F_New(1024);
      
      if (spec)
      {
         FInner *in = (FInner *) F_Get(LOD -> spacing,(uint32) "FA_AdjustSpacing_Spec");
         
         STRPTR buf = F_StrFmt(spec,"<frame id='%ld'",F_Get(LOD -> slider,(uint32) "Value"));
         
         if (in -> l == in -> r && in -> r == in -> t && in -> t == in -> b)
         {
            if (in -> l)
            {
               buf = F_StrFmt(buf," padding='%ld'",in -> l);
            }
         }
         else
         {
            if (in -> l == in -> r)
            {
               if (in -> l)
               {
                  buf = F_StrFmt(buf," padding-width='%ld'",in -> l);
               }
            }
            else
            {
               if (in -> l)
               {
                  buf = F_StrFmt(buf," padding-left='%ld'",in -> l);
               }
               if (in -> r)
               {
                  buf = F_StrFmt(buf," padding-right='%ld'",in -> r);
               }
            }
            
            if (in -> t == in -> b)
            {
               if (in -> t)
               {
                  buf = F_StrFmt(buf," padding-height='%ld'",in -> t);
               }
            }
            else
            {
               if (in -> t)
               {
                  buf = F_StrFmt(buf," padding-top='%ld'",in -> t);
               }
               if (in -> b)
               {
                  buf = F_StrFmt(buf," padding-bottom='%ld'",in -> b);
               }
            }
         }
         
         buf = F_StrFmt(buf," />");

//F_Log(0,"%s",spec);
 
         rc = F_SuperDo(Class,Obj,Method,spec,Msg -> Notify);
         
         F_Dispose(spec);
      }
      return rc;
   }
   
   return F_SUPERDO();
}
//+
///Adjust_ToObject
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   uint32 id = 0;
   FInner in = { 0, 0, 0, 0 };

   if (Msg -> Spec)
   {
      if (F_StrCmp("<frame ",Msg -> Spec,7) == 0)
      {
         FObject doc;
         uint32  id_resolve;
         FXMLAttribute *attribute = (FXMLAttribute *) F_Do(Obj,F_IDR(FM_Adjust_ParseXML),Msg -> Spec,FV_Document_SourceType_Memory, xml_attributes_ids, FV_XML_ID_FRAME, &doc,&id_resolve);

         if (attribute)
         {
            for ( ; attribute ; attribute = attribute -> Next)
            {
               switch (attribute -> Name -> ID)
               {
                  case FV_XML_ID_ID:
                  {
                     id = F_Do(doc,id_resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                  }
                  break;

                  case FV_XML_ID_PADDING:
                  {
                     uint32 val = F_Do(doc,id_resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);

                     in.l = val;
                     in.r = val;
                     in.t = val;
                     in.b = val;
                  }
                  break;

                  case FV_XML_ID_PADDING_WIDTH:
                  {
                     uint32 val = F_Do(doc,id_resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);

                     in.l = val;
                     in.r = val;
                  }
                  break;

                  case FV_XML_ID_PADDING_HEIGHT:
                  {
                     uint32 val = F_Do(doc,id_resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);

                     in.t = val;
                     in.b = val;
                  }
                  break;

                  case FV_XML_ID_PADDING_LEFT:
                  {
                     in.l = F_Do(doc,id_resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                  }
                  break;

                  case FV_XML_ID_PADDING_RIGHT:
                  {
                     in.r = F_Do(doc,id_resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                  }
                  break;

                  case FV_XML_ID_PADDING_TOP:
                  {
                     in.t = F_Do(doc,id_resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                  }
                  break;

                  case FV_XML_ID_PADDING_BOTTOM:
                  {
                     in.b = F_Do(doc,id_resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                  }
                  break;
               }
            }
         }
      }
#ifdef COMPATIBILITY
      else
      {
         stch_l(Msg -> Spec,(int32 *)(&id));
         stch_l(Msg -> Spec + 3,(int32 *)(&in));
      }
#endif
   }

   F_Do(LOD -> slider,FM_Set, FA_NoNotify,TRUE, "Value",id, TAG_DONE);
   F_Do(LOD -> spacing,FM_Set, FA_NoNotify,TRUE, "Spec",&in, TAG_DONE);
}
//+


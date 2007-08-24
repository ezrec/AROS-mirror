#include "Private.h"

enum  {

      FV_PEN_UP,
      FV_PEN_DOWN,
      FV_PEN_LIGHT,
      FV_PEN_SHADOW,
      FV_PEN_TEXT

      };

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> HTMLParser = HTMLDocumentObject, End)
   {
      F_XML_REFS_INIT(11);
      F_XML_REFS_ADD("style-bold",&LOD -> style_bold);
      F_XML_REFS_ADD("style-italic",&LOD -> style_italic);
      F_XML_REFS_ADD("style-underlined",&LOD -> style_underlined);
      F_XML_REFS_ADD("align",&LOD -> align);
      F_XML_REFS_ADD("pens-up",&LOD -> pens_up);
      F_XML_REFS_ADD("pens-down",&LOD -> pens_down);
      F_XML_REFS_ADD("pens-light",&LOD -> pens_light);
      F_XML_REFS_ADD("pens-shadow",&LOD -> pens_shadow);
      F_XML_REFS_ADD("pens-text",&LOD -> pens_text);
      F_XML_REFS_ADD("pens-style",&LOD -> pens_style);
      F_XML_REFS_ADD("test",&LOD -> test);
      F_XML_REFS_DONE;
   
      if (LOD -> XMLObject = XMLObjectObject,
         
            "Source",         "feelin/system-adjust-preparse.xml",
            "SourceType",     "File",
            "References",     F_XML_REFS,
            "Tags",           Msg,
         
         End)
      {
         if (F_SuperDo(Class,Obj,Method,
   
            "Preview",        FALSE,

         TAG_MORE, F_Get(LOD -> XMLObject,(uint32) "Tags")))
         {
            F_Do(LOD -> style_bold,FM_Notify,FA_Selected,FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> style_italic,FM_Notify,FA_Selected,FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> style_underlined,FM_Notify,FA_Selected,FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> align,FM_Notify,"FA_Cycle_Active",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> pens_up,FM_Notify,"Spec",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> pens_down,FM_Notify,"Spec",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> pens_light,FM_Notify,"Spec",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> pens_shadow,FM_Notify,"Spec",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> pens_text,FM_Notify,"Spec",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);
            F_Do(LOD -> pens_style,FM_Notify,"FA_Cycle_Active",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

            return Obj;
         }
      }
   }

   return NULL;
}
//+
///Adjust_Dispose
F_METHOD(void,Adjust_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   F_DisposeObj(LOD -> XMLObject); LOD -> XMLObject = NULL;
   F_DisposeObj(LOD -> HTMLParser); LOD -> HTMLParser = NULL;
   
   F_SUPERDO();
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
   if (Msg -> Spec && *Msg -> Spec == '<')
   {
      return TRUE;
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
      uint32 rc;
      STRPTR spec = F_New(1024);
      
      if (spec)
      {
         STRPTR buf = spec;
 
         if (F_Get(LOD -> style_bold,FA_Selected))
         {
            buf = F_StrFmt(buf,"<b>");
         }

         if (F_Get(LOD -> style_italic,FA_Selected))
         {
            buf = F_StrFmt(buf,"<i>");
         }

         if (F_Get(LOD -> style_underlined,FA_Selected))
         {
            buf = F_StrFmt(buf,"<u>");
         }
                                                   
         switch (F_Get(LOD -> align,(uint32) "FA_Cycle_Active"))
         {
            case 1: buf = F_StrFmt(buf,"<align='center'>"); break;
            case 2: buf = F_StrFmt(buf,"<align='right'>"); break;
         }
         
/*
         if (F_Get(LOD -> font_face,(uint32) "Spec") ||
             F_Get(LOD -> font_color,(uint32) "Spec"))
         {
            F_StrFmt(buf,"<font"); buf += F_StrLen(buf);
 
            if (temp = (STRPTR) F_Get(LOD -> font_face,(uint32) "Spec"))
            {
               STRPTR size;
               STRPTR font = F_StrNew(NULL,"%s",temp);

               if (size = PathPart(font))
               {
                  *size++ = 0;

                  F_StrFmt(buf," face=\"%s\" size=\"%spx\"",font,size); buf += F_StrLen(buf);
               }
            
               F_Dispose(font);
            }
         
            if (temp = (STRPTR) F_Get(LOD -> font_color,(uint32) "Spec"))
            {
               if (*temp == 'c')
               {
                  F_StrFmt(buf," color=\"#%s\"",temp + 2); buf += F_StrLen(buf);
               }
               else if (*temp == 's')
               {
                  F_StrFmt(buf," color=\"%s\"",temp + 2); buf += F_StrLen(buf);
               }
               else if (*temp == 'p')
               {
                  F_StrFmt(buf," color=\"%s\"",temp); buf += F_StrLen(buf);
               }
            }
         
            *buf++ ='>';
         }
*/
         {
            STRPTR pens[5];
            uint32 pens_style    = F_Get(LOD -> pens_style, (uint32) "FA_Cycle_Active");
 
            pens[FV_PEN_UP]      = (STRPTR) F_Get(LOD -> pens_up,    (uint32) "Spec");
            pens[FV_PEN_DOWN]    = (STRPTR) F_Get(LOD -> pens_down,  (uint32) "Spec");
            pens[FV_PEN_LIGHT]   = (STRPTR) F_Get(LOD -> pens_light, (uint32) "Spec");
            pens[FV_PEN_SHADOW]  = (STRPTR) F_Get(LOD -> pens_shadow,(uint32) "Spec");
            pens[FV_PEN_TEXT]    = (STRPTR) F_Get(LOD -> pens_text,  (uint32) "Spec");
         
            if (pens[FV_PEN_UP] || pens[FV_PEN_DOWN] || pens[FV_PEN_LIGHT] || pens[FV_PEN_SHADOW] || pens[FV_PEN_TEXT] || pens_style)
            {
               STATIC STRPTR pens_name[] =
               {
                  "up","down","light","shadow","text"
               };

               uint32 i;
 
               buf = F_StrFmt(buf,"<pens");
               
               for (i = 0 ; i < 5 ; i++)
               {
                  if (pens[i])
                  {
                     buf = F_StrFmt(buf," %s='%s'",pens_name[i],pens[i]);
                  }
               }
            
               if (pens_style)
               {
                  STATIC STRPTR strings[] =
                  {
                     "emboss","ghost","glow","light","shadow"
                  };

                  buf = F_StrFmt(buf," style='%s'",strings[pens_style - 1]);
               }
            
               *buf = '>';
            }
         } 
      }

      rc = F_SuperDo(Class,Obj,Method,spec,Msg -> Notify);

      F_Set(LOD -> test,FA_Text_PreParse,rc);
 
      F_Dispose(spec);

      return rc;
   }
   else if (Msg -> Spec && *Msg -> Spec == '<')
   {
      return F_SUPERDO();
   }

   /* clear */
   
   return F_SuperDo(Class,Obj,Method,NULL,Msg -> Notify);
}
//+
///Adjust_ToObject

enum  {
   
      FV_HTML_ID_BOLD = FV_HTMLDOCUMENT_ID_DUMMY,
      FV_HTML_ID_ITALIC,
      FV_HTML_ID_UNDERLINED,
      FV_HTML_ID_ALIGN,
//      FV_HTML_ID_FONT,
//      FV_HTML_ID_FACE,
//      FV_HTML_ID_COLOR,
      FV_HTML_ID_PENS,
      FV_HTML_ID_UP,
      FV_HTML_ID_LIGHT,
      FV_HTML_ID_DOWN,
      FV_HTML_ID_SHADOW,
      FV_HTML_ID_TEXT,
      FV_HTML_ID_STYLE

      };
  
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
   STATIC FDOCID resolve_ids[] =
   {
      "b",        1, FV_HTML_ID_BOLD,
      "i",        1, FV_HTML_ID_ITALIC,
      "u",        1, FV_HTML_ID_UNDERLINED,
      "align",    5, FV_HTML_ID_ALIGN,
//      "font",     4, FV_HTML_ID_FONT,
//      "face",     4, FV_HTML_ID_FACE,
//      "color",    5, FV_HTML_ID_COLOR,
      "pens",     4, FV_HTML_ID_PENS,
      "up",       2, FV_HTML_ID_UP,
      "light",    5, FV_HTML_ID_LIGHT,
      "down",     4, FV_HTML_ID_DOWN,
      "shadow",   6, FV_HTML_ID_SHADOW,
      "text",     4, FV_HTML_ID_TEXT,
      "style",    5, FV_HTML_ID_STYLE,
       NULL
   };
   
   uint32 id_Resolve = F_DynamicFindID("FM_Document_Resolve");
   uint32 n;

   F_Do(LOD -> test,FM_Set,FA_Text_PreParse,Msg -> Spec,TAG_DONE);

   F_Do(LOD -> HTMLParser,FM_Set,
                         "FA_Document_Source",      Msg -> Spec,
                         "FA_Document_SourceType",  "Memory",
                          TAG_DONE);

   n = F_Do(LOD -> HTMLParser,(uint32) "FM_Document_AddIDs",resolve_ids);
 
   if (n)
   {
      FHTMLNode *node;
 
      for (node = (FHTMLNode *) F_Get(LOD -> HTMLParser,(uint32) "FA_HTMLDocument_Nodes") ; node ; node = node -> Next)
      {
         FHTMLMarkup *markup = (FHTMLMarkup *)(node);

         if (node -> Type != FV_HTMLDocument_Node_Markup) continue;
         if (markup -> Terminator) continue;

         switch (markup -> Name -> ID)
         {
///<b>
            case FV_HTML_ID_BOLD:
            {
               F_Do(LOD -> style_bold,FM_Set,FA_NoNotify,TRUE,FA_Selected,TRUE,TAG_DONE);
            }
            break;
//+
/// <i>
            case FV_HTML_ID_ITALIC:
            {
               F_Do(LOD -> style_italic,FM_Set,FA_NoNotify,TRUE,FA_Selected,TRUE,TAG_DONE);
            }
            break;
//+
/// <u>
            case FV_HTML_ID_UNDERLINED:
            {
               F_Do(LOD -> style_underlined,FM_Set,FA_NoNotify,TRUE,FA_Selected,TRUE,TAG_DONE);
            }
            break;
//+
/// <align>
            case FV_HTML_ID_ALIGN:
            {
               FHTMLAttribute *attribute;

               for (attribute = (FHTMLAttribute *)(markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
               {
                  if (attribute -> Name -> ID == FV_HTML_ID_ALIGN)
                  {
                     STATIC FDOCValue resolve[] =
                     {
                        "left",0, "center",1, "right",2, NULL
                     };
                     
                     uint32 val = F_Do(LOD -> HTMLParser,id_Resolve,attribute -> Data,0,resolve,NULL);
                     
                     F_Do(LOD -> align,FM_Set, FA_NoNotify,TRUE, "FA_Cycle_Active",val, TAG_DONE);
                  }
               }
            }
            break;
//+
/*
/// <font>
               case FV_HTML_ID_FONT:
               {
                  FHTMLAttribute *attribute;
                  
                  for (attribute = (FHTMLAttribute *)(markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
                  {
                     switch (attribute -> Name -> ID)
                     {
                        case FV_HTML_ID_COLOR:
                        {
                           if (*attribute -> Data == '#')
                           {
                              STRPTR spec = F_StrNew(NULL,"c:%s",attribute -> Data + 1);
                              
                              F_Do(LOD -> font_color,FM_Set,FA_NoNotify,TRUE,"Spec",spec,TAG_DONE);
                              
                              F_Dispose(spec);
                           }
                           else
                           {
                              F_Do(LOD -> font_color,FM_Set,FA_NoNotify,TRUE,"Spec",attribute -> Data,TAG_DONE);
                           }
                        }
                        break;
                        
                        case FV_HTML_ID_FACE:
                        {
                           F_Do(LOD -> font_face,FM_Set,FA_NoNotify,TRUE,"Spec",attribute -> Data,TAG_DONE);
                        }
                        break;
                     }
                  }

               }
               break;
//+
*/
/// <pens>
            case FV_HTML_ID_PENS:
            {
               FHTMLAttribute *attribute;

               for (attribute = (FHTMLAttribute *)(markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
               {
                  if (attribute -> Name -> ID == FV_HTML_ID_STYLE)
                  {
                     STATIC FDOCValue resolve[] =
                     {
                        "emboss",1,"ghost",2,"glow",3,"light",4,"shadow",5,NULL
                     };

                     uint32 val = F_Do(LOD -> HTMLParser,id_Resolve,attribute -> Data,0,resolve,NULL);

                     F_Do(LOD -> pens_style,FM_Set, FA_NoNotify,TRUE, "FA_Cycle_Active",val, TAG_DONE);
                  }
                  else
                  {
                     FObject pen_obj=NULL;

                     switch (attribute -> Name -> ID)
                     {
                        case FV_HTML_ID_UP:     pen_obj = LOD -> pens_up; break;
                        case FV_HTML_ID_LIGHT:  pen_obj = LOD -> pens_light; break;
                        case FV_HTML_ID_DOWN:   pen_obj = LOD -> pens_down; break;
                        case FV_HTML_ID_SHADOW: pen_obj = LOD -> pens_shadow; break;
                        case FV_HTML_ID_TEXT:   pen_obj = LOD -> pens_text; break;
                     }

                     if (pen_obj)
                     {
                        F_Do(pen_obj,FM_Set, FA_NoNotify,TRUE, "Spec",attribute -> Data, TAG_DONE);
                     }
                  }
               }
            }
            break;
//+
         }
      }
   }
}
//+

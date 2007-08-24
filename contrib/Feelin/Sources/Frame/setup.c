#include "Private.h"

#define DEF_FRAME                               "<frame id='1' padding='2' />\n<frame id='2' padding='2' />"
 
/*** Methods ***************************************************************/

///Frame_Setup
F_METHODM(LONG,Frame_Setup,FS_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   STRPTR spec;
   uint32 i,a;

/*** reset possible previous values ****************************************/

   LOD -> ID[0] = 0;
   LOD -> ID[1] = 0;
   
   LOD -> Public.Back = NULL;

   LOD -> Public.Padding[0].l = 0;
   LOD -> Public.Padding[0].r = 0;
   LOD -> Public.Padding[0].t = 0;
   LOD -> Public.Padding[0].b = 0;

   LOD -> Public.Padding[1].l = 0;
   LOD -> Public.Padding[1].r = 0;
   LOD -> Public.Padding[1].t = 0;
   LOD -> Public.Padding[1].b = 0;

/*** title *****************************************************************/
 
   if (LOD -> TitleData)
   {
      FFrameTitle *data = LOD -> TitleData;
      uint32 pos;

      data -> Down = FALSE;
      data -> Prep = (STRPTR) F_Do(Msg -> Render -> Application,FM_Application_Resolve,data -> p_Prep,NULL);

      pos = F_Do(Msg -> Render -> Application,FM_Application_ResolveInt,data -> p_Position,FV_Frame_UpLeft);

      switch (pos)
      {
         case FV_Frame_DownLeft:
         case FV_Frame_DownRight:
         case FV_Frame_DownCenter: data -> Down = TRUE; break;
      }

      switch (pos)
      {
         case FV_Frame_UpLeft:     case FV_Frame_DownLeft:   data -> Position = FV_Position_Left;    break;
         case FV_Frame_UpRight:    case FV_Frame_DownRight:  data -> Position = FV_Position_Right;   break;
         case FV_Frame_UpCenter:   case FV_Frame_DownCenter: data -> Position = FV_Position_Center;  break;
      }

      data -> Font = (struct TextFont *) F_Do(Msg -> Render -> Application,FM_Application_OpenFont,Obj,data -> p_Font);

      data -> TD = TextDisplayObject,

         FA_TextDisplay_Contents,  data -> Title,
         FA_TextDisplay_PreParse,  data -> Prep,
         FA_TextDisplay_Font,      data -> Font,
         FA_TextDisplay_Shortcut,  FALSE,

      End;

      if (!F_Do(data -> TD,FM_TextDisplay_Setup,Msg -> Render))
      {
         F_DisposeObj(data -> TD); data -> TD = NULL;
      }

      if (data -> TD)
      {
         data -> Width  = F_Get(data -> TD,FA_TextDisplay_Width);
         data -> Height = F_Get(data -> TD,FA_TextDisplay_Height);
      }
   }

/*** background ************************************************************/

   if ((spec = (STRPTR) F_Do(Msg -> Render -> Application,FM_Application_Resolve,LOD -> p_back,NULL)) != NULL)
   {
      LOD -> Public.Back = ImageDisplayObject,
         
         FA_ImageDisplay_Spec,   spec,
         
      End;
      
      if (LOD -> Public.Back)
      {
         if (!(F_Do(LOD -> Public.Back,FM_ImageDisplay_Setup,Msg -> Render)))
         {
            F_DisposeObj(LOD -> Public.Back); LOD -> Public.Back = NULL;
         }
      }
   }

/*** frame *****************************************************************/

   if ((spec = (STRPTR) F_Do(Msg -> Render -> Application,FM_Application_Resolve,LOD -> p_frame,DEF_FRAME)) != NULL)
   {
      if (F_StrCmp("<frame ",spec,7) == 0)
      {
         STATIC FDOCID xml_ids[] =
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

         FXMLMarkup *markup;
         
         uint32 render_done=FALSE;

         F_Do(CUD -> XMLDocument,FM_Lock,FF_Lock_Exclusive);

         F_Do(CUD -> XMLDocument,FM_Set,

            F_ID(CUD -> XMLIDs,FA_Document_Source),      spec,
            F_ID(CUD -> XMLIDs,FA_Document_SourceType),  FV_Document_SourceType_Memory,

            TAG_DONE);

         F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_AddIDs), xml_ids);
         
         for (markup = (FXMLMarkup *) F_Get(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FA_XMLDocument_Markups)) ; markup ; markup = markup -> Next)
         {
            if (markup -> Name -> ID == FV_XML_ID_FRAME)
            {
               FXMLAttribute *attribute;
            
               uint32 n = (render_done) ? 1 : 0;

               for (attribute = (FXMLAttribute *) markup -> AttributesList.Head ; attribute ; attribute = attribute -> Next)
               {
                  switch (attribute -> Name -> ID)
                  {
                     case FV_XML_ID_ID:
                     {
                        LOD -> ID[n] = F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_Resolve),attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                     }
                     break;
                      
                     case FV_XML_ID_PADDING:
                     {
                        uint32 val = F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_Resolve),attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                        
                        LOD -> Public.Padding[n].l = val;
                        LOD -> Public.Padding[n].r = val;
                        LOD -> Public.Padding[n].t = val;
                        LOD -> Public.Padding[n].b = val;
                     }
                     break;

                     case FV_XML_ID_PADDING_WIDTH:
                     {
                        uint32 val = F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_Resolve),attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);

                        LOD -> Public.Padding[n].l = val;
                        LOD -> Public.Padding[n].r = val;
                     }
                     break;

                     case FV_XML_ID_PADDING_HEIGHT:
                     {
                        uint32 val = F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_Resolve),attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);

                        LOD -> Public.Padding[n].t = val;
                        LOD -> Public.Padding[n].b = val;
                     }
                     break;

                     case FV_XML_ID_PADDING_LEFT:
                     {
                        LOD -> Public.Padding[n].l = F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_Resolve),attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                     }
                     break;

                     case FV_XML_ID_PADDING_RIGHT:
                     {
                        LOD -> Public.Padding[n].r = F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_Resolve),attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                     }
                     break;

                     case FV_XML_ID_PADDING_TOP:
                     {
                        LOD -> Public.Padding[n].t = F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_Resolve),attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                     }
                     break;

                     case FV_XML_ID_PADDING_BOTTOM:
                     {
                        LOD -> Public.Padding[n].b = F_Do(CUD -> XMLDocument,F_ID(CUD -> XMLIDs,FM_Document_Resolve),attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                     }
                     break;
                  }
               }
               
               render_done = TRUE;
            }
         }
      
         F_Do(CUD -> XMLDocument,FM_Unlock);
      }
      else
      {
         F_Log(0,"deprecated frame specification (%s)",spec);
      }
   }

   LOD -> Public.Padding[0].l += LOD -> UserInner.l;
   LOD -> Public.Padding[0].r += LOD -> UserInner.r;
   LOD -> Public.Padding[0].t += LOD -> UserInner.t;
   LOD -> Public.Padding[0].b += LOD -> UserInner.b;

   LOD -> Public.Padding[1].l += LOD -> UserInner.l;
   LOD -> Public.Padding[1].r += LOD -> UserInner.r;
   LOD -> Public.Padding[1].t += LOD -> UserInner.t;
   LOD -> Public.Padding[1].b += LOD -> UserInner.b;
   
   /*** ***/

   for (i = 0 ; i < 2 ; i += 1)
   {
      switch (LOD -> ID[i])
      {
         case  1: case  2: case  3: case  4: case  5: case  6:

            a = 0x01010101; break;

         case  7: case  8: case  9: case 10: case 11: case 12:
         case 13: case 14: case 18: case 23: case 27: case 28:
         case 31: case 32: case 37: case 38: case 39: case 40:
         case 42: case 43: case 46: case 47:

            a = 0x02020202; break;

         case 15: case 16: case 17: case 21: case 25: case 29:
         case 30: case 33: case 34: case 35: case 48: case 49:

            a = 0x03030303; break;

         case 19:

            a = 0x04040202; break;

         case 41:

            a = 0x04040404; break;

         case 20:

            a = 0x05050505; break;

         case 44:

            a = 0x02020303; break;

         case 24:

            a = 0x03030101; break;

         case 36: case 45:

            a = 0x03030202; break;

         case 26:

            a = 0x04040101; break;

         case 22:

            a = 0x06060303; break;
         
         case 50:
            
            a = 0x02020000; break;
            
         case 51:
            
            a = 0x00020200; break;
            
         case 52:
            
            a = 0x00000202; break;
            
         case 53:
            
            a = 0x02000002; break;

         default:

            a = 0x00000000; break;
      }

   #ifdef __AROS__
      LOD -> Public.Border[i].l = (a & 0xFF000000) >> 24;
      LOD -> Public.Border[i].t = (a & 0xFF0000) >> 16;
      LOD -> Public.Border[i].r = (a & 0xFF00) >> 8;
      LOD -> Public.Border[i].b = (a & 0xFF);      
      /* The below is not portable at all! */
   #else
      CopyMem(&a,&LOD -> Public.Border[i],sizeof (FInner));
   #endif

//      F_Log(0,"ID %2ld - Border 0x%08lx - Space 0x%08lx",LOD -> Public.ID[i],*((uint32 *)(&LOD -> Public.Border[i])),*((uint32 *)(&LOD -> Public.Padding[i])));

      if (LOD -> TitleData)
      {
         if (LOD -> TitleData -> Down)
         {
            LOD -> TitleData -> FrameBorderH[i] = LOD -> Public.Border[i].b;
            LOD -> Public.Border[i].b = LOD -> TitleData -> Height;
         }
         else
         {
            LOD -> TitleData -> FrameBorderH[i] = LOD -> Public.Border[i].t;
            LOD -> Public.Border[i].t = LOD -> TitleData -> Height;
         }
      }
   }
   return TRUE;
}
//+
///Frame_Cleanup
F_METHODM(void,Frame_Cleanup,FS_Frame_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

//   F_Log(0,"Render 0x%08lx (0x%08lx)",Msg -> Render,F_Get(Msg -> Render,FA_Render_RPort));
   
   F_Do(LOD -> Public.Back,FM_ImageDisplay_Cleanup,Msg -> Render);
   F_DisposeObj(LOD -> Public.Back); LOD -> Public.Back = NULL;
 
   if (LOD -> TitleData)
   {
      F_Do(LOD -> TitleData -> TD,FM_TextDisplay_Cleanup);
      F_DisposeObj(LOD -> TitleData -> TD); LOD -> TitleData -> TD = NULL;

      if (LOD -> TitleData -> Font)
      {
         CloseFont(LOD -> TitleData -> Font); LOD -> TitleData -> Font = NULL;
      }
   }
}
//+

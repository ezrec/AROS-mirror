#include "Private.h"

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Preview_New
F_METHOD(uint32,Preview_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    F_AREA_SAVE_PUBLIC;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Preview_Alive:  if (item.ti_Data) LOD -> flags |= FF_PREVIEW_ALIVE; else LOD -> flags &= ~FF_PREVIEW_ALIVE; break;
        case FA_Preview_Couple: if (item.ti_Data) LOD -> flags |= FF_PREVIEW_COUPLE; else LOD -> flags &= ~FF_PREVIEW_COUPLE; break;
    }

    return F_SuperDo(Class,Obj,Method,
       
        FA_NoFill,        TRUE,
        FA_SetMin,        FALSE,
        FA_SetMax,        FALSE,
        FA_MinWidth,      20,
        FA_MinHeight,     20,
        FA_Draggable,     TRUE,
        FA_Dropable,      TRUE,
        FA_Frame,         "<frame id='5' /><frame id='6' />",
        FA_ChainToCycle,  (FF_PREVIEW_COUPLE & LOD -> flags) ? TRUE : FALSE,
        FA_ContextHelp,   "You can use Drag'n'Drop from or to similar objects",
          
    TAG_MORE,Msg);
}
//+
///Preview_Dispose
F_METHOD(void,Preview_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Dispose(LOD -> spec); LOD -> spec = NULL;
    
    F_DisposeObj(LOD -> XMLDocument); LOD -> XMLDocument = NULL;
    F_DisposeObj(LOD -> AdjustWindow);
    
    LOD -> AdjustWindow = NULL;
    LOD -> Adjust = NULL;
    
    F_SUPERDO();
}
//+
///Preview_Set
F_METHOD(void,Preview_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;

    while  ((tag = F_DynamicNTI(&Tags,&item,Class)))
    switch (item.ti_Tag)
    {
        case FA_Preview_Spec:
        {
            uint32 ok = FALSE;
            
            if (item.ti_Data)
            {
               ok = F_Do(Obj,F_IDM(FM_Preview_Query),item.ti_Data);
            }
            
            F_Do(Obj,F_IDM(FM_Preview_ToString),(ok) ? item.ti_Data : NULL);
            
            tag -> ti_Data = (uint32) LOD -> spec;
        }
        break;
    }

    F_SUPERDO();
}
//+
///Preview_Get
F_METHOD(void,Preview_Get)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Preview_Spec:   F_STORE(LOD -> spec); break;
        case FA_Preview_Couple: F_STORE((FF_PREVIEW_COUPLE & LOD -> flags) ? TRUE : FALSE); break;
    }

    F_SUPERDO();
}
//+
///Preview_Import
F_METHODM(uint32,Preview_Import,FS_Import)
{
    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        STRPTR data = (STRPTR) F_Do(Msg -> Dataspace,Msg -> id_Find,id);

        if (data)
        {
            F_Set(Obj,F_IDA(FA_Preview_Spec),(uint32)(data));
        }
    }

    return TRUE;
}
//+
///Preview_Export
F_METHODM(uint32,Preview_Export,FS_Export)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        if (LOD -> spec)
        {
            F_Do(Msg -> Dataspace,Msg -> id_Add,id,LOD -> spec,F_StrLen(LOD -> spec) + 1);
        }
    }

    return TRUE;
}
//+

///Preview_Setup
F_METHODM(LONG,Preview_Setup,FS_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
  
    if (F_SUPERDO())
    {
        if (FF_PREVIEW_ALIVE & LOD -> flags)
        {
            F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);
        }
     
        LOD -> TD = TextDisplayObject,

            FA_TextDisplay_PreParse,   "<align=center><pens style=ghost>",
            FA_TextDisplay_Contents,   F_CAT(EMPTY),
            FA_TextDisplay_Font,       _font,

            End;

        if (!F_Do(LOD -> TD,FM_TextDisplay_Setup,_render))
        {
            F_DisposeObj(LOD -> TD); LOD -> TD = NULL;
        }
    
        return TRUE;
    }
    return FALSE;
}
//+
///Preview_Cleanup
F_METHOD(uint32,Preview_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (FF_PREVIEW_ALIVE & LOD -> flags)
    {
        F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);
    }

    if (LOD -> TD)
    {
        F_Do(LOD -> TD,FM_TextDisplay_Cleanup,_render);
        F_DisposeObj(LOD -> TD); LOD -> TD = NULL;
    }

    return F_SUPERDO();
}
//+
///Preview_Draw
F_METHODM(void,Preview_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_SUPERDO();
  
    if ((LOD -> spec == NULL) && (_iw > 4) && (_ih > 4))
    {
        FRect rect;
        
        rect.x1 = _ix ; rect.x2 = rect.x1 + _iw - 1;
        rect.y1 = _iy ; rect.y2 = rect.y1 + _ih - 1;

        F_Do(Obj, FM_Erase, &rect, 0);

        if (LOD -> TD)
        {
           uint32 td_h = F_Get(LOD -> TD,FA_TextDisplay_Height);

           if (td_h < (rect.y2 - rect.y1 + 1))
           {
              rect.y1 = ((rect.y2 - rect.y1 + 1) - td_h) / 2 + rect.y1;
           }
        
           F_Do(LOD -> TD, FM_TextDisplay_Draw, &rect);
        }
        else
        {
           struct RastPort *rp = _rp;

           int16 x1 = _ix, x2 = x1 + _iw - 1;
           int16 y1 = _iy, y2 = y1 + _ih - 1;

           _FPen(FV_Pen_Shadow);
           _Move(x1,y1);   _Draw(x2-1,y2);
           _Move(x1+1,y2); _Draw(x2,y1);
           _FPen(FV_Pen_HalfShine);
           _Move(x1+1,y1); _Draw(x2,y2);
           _Move(x1,y2);   _Draw(x2-1,y1);
        }
    }
}
//+
///Preview_HandleEvent
F_METHODM(uint32,Preview_HandleEvent,FS_HandleEvent)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (!(FF_PREVIEW_ALIVE & LOD -> flags))
   {
      return F_SUPERDO();
   }
 
   switch (Msg -> Event -> Class)
   {
      case FF_EVENT_BUTTON:
      {
         if (Msg -> Event -> Code == FV_EVENT_BUTTON_SELECT)
         {
            if ((Msg -> Event -> MouseX >= _x) && (Msg -> Event -> MouseX <= _x2) &&
                (Msg -> Event -> MouseY >= _y) && (Msg -> Event -> MouseY <= _y2))
            {
               if (FF_EVENT_BUTTON_DOWN & Msg -> Event -> Flags)
               {
                  if (LOD -> spec)
                  {
                     LOD -> flags |= FF_PREVIEW_CLICKED;
 
                     F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION,0);
                  }

                  return FF_HandleEvent_Eat;
               }
               else if (FF_PREVIEW_CLICKED & LOD -> flags)
               {
                  int32 selected = F_Get(Obj,FA_Selected);

                  LOD -> flags &= ~FF_PREVIEW_CLICKED;
 
                  F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION);

                  F_SuperDo(Class -> Super,Obj,FM_Set,FA_Selected,!selected,TAG_DONE);
               }
            }
         }
      }
      break;
      
      case FF_EVENT_MOTION:
      {
         F_Do(_app,FM_Application_PushMethod,Obj,FM_DnDDo,2,Msg -> Event -> MouseX,Msg -> Event -> MouseY);

         F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION);

         return FF_HandleEvent_Eat;
      }
      break;
   }

   return 0;
}
//+
///Preview_DnDQuery
F_METHOD_DNDQUERY(Preview_DnDQuery)
{
   if (F_SUPERDO())
   {
      STRPTR spec = (STRPTR) F_Get(Msg -> Source,F_IDA(FA_Preview_Spec));
 
//      F_Log(0,"QUERY (%s)",spec);
      
      if (F_Do(Obj,F_IDM(FM_Preview_Query),spec))
      {
//         F_Log(0,"QUERY >> OK");
 
         return Obj;
      }
//      F_Log(0,"QUERY >> FAILED");
   }
   return NULL;
}
//+
///Preview_DnDDrop
F_METHODM(void,Preview_DnDDrop,FS_DnDDrop)
{
   STRPTR spec = (STRPTR) F_Get(Msg -> Source,F_IDA(FA_Preview_Spec));

   if (spec)
   {
      F_Set(Obj,F_IDA(FA_Preview_Spec),(uint32)(spec));
   }
}
//+

///Preview_Query
F_METHODM(uint32,Preview_Query,FS_Preview_Query)
{
   F_Log(0,"The method *must* be implemented by subclass");
   
   return FALSE;
}
//+
///Preview_ParseXML

F_METHODM(FXMLAttribute *,Preview_ParseXML,FS_Preview_ParseXML)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (Msg -> Source && Msg -> SourceType & Msg -> MarkupID)
   {
      if (!LOD -> XMLDocument)
      {
         LOD -> XMLDocument = XMLDocumentObject,
         
            "FA_Document_Source",      Msg -> Source,
            "FA_Document_SourceType",  Msg -> SourceType,
            
            End;
      }
      else
      {
         F_Do(LOD -> XMLDocument,FM_Set,

            F_IDO(FA_Document_Source),      Msg -> Source,
            F_IDO(FA_Document_SourceType),  Msg -> SourceType,

            TAG_DONE);
      }

      if (LOD -> XMLDocument)
      {
         FXMLMarkup *markup;
      
         F_Do(LOD -> XMLDocument,F_IDO(FM_Document_AddIDs), Msg -> IDs);
 
         for (markup = (FXMLMarkup *) F_Get(LOD -> XMLDocument,F_IDO(FA_XMLDocument_Markups)) ; markup ; markup = markup -> Next)
         {
            if (markup -> Name -> ID == Msg -> MarkupID)
            {
               if (Msg -> XMLDocument)
               {
                  *Msg -> XMLDocument = LOD -> XMLDocument;
               }

               if (Msg -> id_Resolve)
               {
                  *Msg -> id_Resolve = F_IDO(FM_Document_Resolve);
               }

               return (FXMLAttribute *) markup -> AttributesList.Head;
            }
         }
      }
   }
   return NULL;
}
//+
///Preview_ToString
F_METHODM(STRPTR,Preview_ToString,FS_Preview_ToString)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
   F_Dispose(LOD -> spec);
   
   LOD -> spec = F_StrNew(NULL,"%s",Msg -> Spec);

   return LOD -> spec;
}
//+
///Preview_Adjust
F_METHODM(void,Preview_Adjust,FS_Preview_Adjust)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> AdjustWindow)
    {
        F_Log(0,"AdjustWindow already created, opening >>");
        
        F_Set(LOD -> AdjustWindow,FA_Window_Open,TRUE);
    }
    else
    {
        LOD -> AdjustWindow = DialogObject,
           
           FA_Window_Title,        Msg -> WindowTitle,
           FA_Width,               "60%",
          "FA_Dialog_Buttons",     FV_Dialog_Buttons_Boolean,
          "FA_Dialog_Separator",   TRUE,
             
           End;
           
        if (LOD -> AdjustWindow)
        {
            LOD -> Adjust = F_NewObj(Msg -> AdjustClass,
            
                "FA_Adjust_Couple",  (FF_PREVIEW_COUPLE & LOD -> flags) ? TRUE : FALSE,
                "FA_Adjust_Spec",    LOD -> spec,
                
                TAG_MORE,((uint32)(Msg)) + sizeof (struct FS_Preview_Adjust));
    
            if (LOD -> Adjust)
            {
                FObject root = (FObject) F_Get(LOD -> AdjustWindow,FA_Child);
             
                if (F_Do(root,FM_AddMember,LOD -> Adjust,FV_AddMember_Head) &&
                    F_Do(_app,FM_AddMember,LOD -> AdjustWindow,FV_AddMember_Head))
                {
                    F_Do(LOD -> AdjustWindow,FM_Notify,"FA_Dialog_Response",FV_Notify_Always,FV_Notify_Application, FM_Application_PushMethod,4, Obj,F_IDM(FM_Preview_Update),1,FV_Notify_Value);
                   
                    return;
                }
                F_DisposeObj(LOD -> Adjust); LOD -> Adjust = NULL;
            }
            F_DisposeObj(LOD -> AdjustWindow); LOD -> AdjustWindow = NULL;
        }
    }
}
//+
///Preview_Update
F_METHODM(void,Preview_Update,FS_Preview_Update)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg -> Response == FV_Dialog_Response_Ok)
    {
        uint32 spec = F_Get(LOD -> Adjust,(uint32) "FA_Adjust_Spec");

        F_Set(Obj,F_IDA(FA_Preview_Spec),spec);
    }

    F_DisposeObj(LOD -> AdjustWindow);

    LOD -> AdjustWindow = NULL;
    LOD -> Adjust = NULL;
}
//+

/****************************************************************************
*** Preferences *************************************************************
****************************************************************************/

STATIC FPreferenceScript Script[] =
{
    { "$popbutton-frame",   FV_TYPE_STRING, "Spec", "<frame id='#23' padding='2'/>\n<frame id='#24' padding-left='3' padding-top='3' padding-right='2' padding-bottom='2' />",0 },
    { "$popbutton-back",    FV_TYPE_STRING, "Spec", NULL,0 },
    { "$popbutton-scheme",  FV_TYPE_STRING, "Spec", NULL,0 },
    { "$popstring-frame",   FV_TYPE_STRING, "Spec", "<frame id='#21' padding-width='3' padding-height='1'/>\n<frame id='#28' padding-width='3' padding-height='1' />",0 },
    { "$popstring-back",    FV_TYPE_STRING, "Spec", "fill;halfshadow",0 },
    { "$popstring-scheme",  FV_TYPE_STRING, "Spec", NULL,0 },
    { "$popfont-image",     FV_TYPE_STRING, "Spec", "<image type='brush' src='defaults/pop.fb0' />",0 },
    { "$popfile-image",     FV_TYPE_STRING, "Spec", "<image type='brush' src='defaults/pop.fb0' />",0 },
    { "$poppreparse-image", FV_TYPE_STRING, "Spec", "<image type='brush' src='defaults/pop.fb0' />",0 },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Popups",

        "Script", Script,
        "XMLSource", "feelin/preference/popups.xml",

    TAG_MORE,Msg);
}

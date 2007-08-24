#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///adjust_update_text
void adjust_update_text(FClass *Class,FObject Obj,struct FeelinBase *FeelinBase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    uint32 id;
    
    if (!LOD -> render)
    {
       id = FV_TEXT_NO_RENDER;
    }
    else
    {
       if (F_Get(LOD -> preview, FA_Selected))
       {
          id = (LOD -> select) ? FV_TEXT_SELECT : FV_TEXT_NO_SELECT;
       }
       else
       {
          id = (LOD -> render) ? FV_TEXT_RENDER : FV_TEXT_NO_RENDER;
       }
    }

    if (id != LOD -> last_text)
    {
       STRPTR text=NULL;
       
       switch (id)
       {
          case FV_TEXT_RENDER:    text = F_CAT(RENDER); break;
          case FV_TEXT_SELECT:    text = F_CAT(SELECT); break;
          case FV_TEXT_NO_RENDER: text = F_CAT(RENDER_NONE); break;
          case FV_TEXT_NO_SELECT: text = F_CAT(SELECT_NONE); break;
       }
    
       LOD -> last_text = id;
    
       F_Set(LOD -> text,FA_Text,(uint32)(text));
    }
}
//+
 
/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD_NEW(Adjust_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;
    
    BOOL couple = FALSE;
    BOOL preview = TRUE;
    STRPTR preview_title = F_CAT(PREVIEW);
    STRPTR preview_class = NULL;
    FObject preview_grp = NULL;

    F_AREA_SAVE_PUBLIC;
    
    LOD -> separator = FV_Adjust_Separator;
  
    /* do we need to create the preview object ? */

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Adjust_Preview:       preview = item.ti_Data; break;
        case FA_Adjust_PreviewTitle:  preview_title = (STRPTR)(item.ti_Data); break;
        case FA_Adjust_PreviewClass:  preview_class = (STRPTR)(item.ti_Data); break;
        case FA_Adjust_Couple:        couple = item.ti_Data; break;
        case FA_Adjust_Separator:     LOD -> separator = item.ti_Data; break;
    }

    if (preview && !preview_class)
    {
        preview = FALSE; F_Log(FV_LOG_DEV,"PreviewClass not defined");
    }

    if (!LOD -> separator && couple)
    {
        couple = FALSE; F_Log(FV_LOG_DEV,"Couple spec requested, but separator is NULL");
    }
   
    if (preview)
    {
        if (couple)
        {
            LOD -> text = TextObject,
                
                FA_SetMax,           FV_SetHeight,
                FA_ChainToCycle,     FALSE,
                FA_Text_PreParse,    "<align=\"center\"><pens style=\"shadow\" text=\"shine\">",
              
                End;
        }
   
        preview_grp = VGroup, FA_Weight,75,
            Child, VGroup, GroupFrame, FA_Frame_Title,preview_title,
                Child, LOD -> preview = F_NewObj(preview_class,"FA_Preview_Alive",TRUE,"FA_Preview_Couple",couple,TAG_DONE),
                (couple) ? Child : TAG_IGNORE, LOD -> text,
            End,
                 
            Child, LOD -> reset = F_MakeObj(FV_MakeObj_Button,F_CAT(CLEAR),FA_Disabled,TRUE,TAG_DONE),
        End;
    }

    /* The FC_XMLDocument object must be ready before we pass the method  to
    our  superclass, a subclass will need it if the FA_Adjust_Spec attribute
    it already defined within the initial taglist. */
    
    if (preview)
    {
        if (!F_SuperDo(Class,Obj,Method,
           
            FA_Horizontal, TRUE,
            FA_Group_PageTitle, preview_title,
            
            Child, preview_grp,
            Child, BalanceObject,End,
           
        TAG_MORE,Msg))
        {
           return NULL;
        }
    }
    else
    {
        if (!F_SuperDo(Class,Obj,Method,

           FA_Horizontal, TRUE,
           FA_Group_PageTitle, preview_title,
        
        TAG_MORE,Msg))
        {
            return NULL;
        }
    }

    if (LOD -> text)
    {
        F_Do(LOD -> preview,FM_Notify,FA_Selected,FV_Notify_Always,Obj,F_IDM(FM_Adjust_TogglePreview),0);

        adjust_update_text(Class,Obj,FeelinBase);
    }

    F_Do(LOD -> reset,FM_Notify,FA_Pressed,FALSE,Obj,FM_Set,2,F_IDA(FA_Adjust_Spec),NULL);

    return Obj;
}
//+
///Adjust_Dispose
F_METHOD_DISPOSE(Adjust_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_SUPERDO();
    
    F_Dispose(LOD -> spec); LOD -> spec = NULL;
    F_Dispose(LOD -> render); LOD -> render = NULL;
    F_Dispose(LOD -> select); LOD -> select = NULL;
    F_DisposeObj(LOD -> XMLDocument); LOD -> XMLDocument = NULL;
}
//+
///Adjust_Get
F_METHOD_GET(Adjust_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Adjust_Spec:          F_STORE(LOD -> spec); break;
      case FA_Adjust_Preview:       F_STORE(LOD -> preview); break;
   }

   F_SUPERDO();
}
//+
///Adjust_Set
F_METHOD_SET(Adjust_Set)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,*real,item;

   while  ((real = F_DynamicNTI(&Tags,&item,Class)) != NULL)
   switch (item.ti_Tag)
   {
      case FA_Adjust_Spec:
      {
         STRPTR spec = (STRPTR)(item.ti_Data);
         
         if (spec && *spec)
         {
            STRPTR render=NULL;
            STRPTR select=NULL;
            STRPTR start=spec;
            
            while (*spec && *spec != LOD -> separator) spec++;
               
            if (spec - start)
            {
               if ((render = F_New(spec - start + 1)) != NULL)
               {
                  CopyMem(start,render,spec - start);
                  
                  if (LOD -> separator && (*spec++ == LOD -> separator))
                  {
                     start = spec;
                     
                     while (*spec && *spec != LOD -> separator) spec++;
         
                     if (spec - start)
                     {
                        if ((select = F_New(spec - start + 1)) != NULL)
                        {
                           CopyMem(start,select,spec - start);
                        }
                     }
                  }
               }
            }
            
            if (LOD -> text)
            {
               int32 ok;
               int32 state = F_Get(LOD -> preview,FA_Selected);
               
               F_Set(_render,FA_Render_Forbid,TRUE);
               F_Do(LOD -> preview,FM_Set,FA_NoNotify,TRUE,FA_Selected,!state,TAG_DONE);
         
//               F_Log(0,"QUERY (%s) >> BEGIN",(state) ? render : select);
               ok = F_Do(Obj,F_IDM(FM_Adjust_Query),(state) ? (render) : (select));
//               F_Log(0,"QUERY (%s) >> END (0x%08lx)",(state) ? render : select,ok);
               F_Do(Obj,F_IDM(FM_Adjust_ToString),(ok) ? ((state) ? (render) : (select)) : NULL,FALSE);
               
               F_Do(LOD -> preview,FM_Set,FA_NoNotify,TRUE,FA_Selected,state,TAG_DONE);
               F_Set(_render,FA_Render_Forbid,FALSE);
 
//               F_Log(0,"QUERY (%s) >> BEGIN",(state) ? select : render);
               ok = F_Do(Obj,F_IDM(FM_Adjust_Query),(state) ? (select) : (render));
//               F_Log(0,"QUERY (%s) >> END (0x%08lx)",(state) ? select : render,ok);
               F_Do(Obj,F_IDM(FM_Adjust_ToString),(ok) ? ((state) ? (select) : (render)) : NULL,FALSE);
            }
            else
            {
               int32 ok;
               
//               F_Log(0,"QUERY (%s) >> BEGIN",render);
               ok = F_Do(Obj,F_IDM(FM_Adjust_Query),render);
//               F_Log(0,"QUERY (%s) >> END (0x%08lx)",render,ok);
               F_Do(Obj,F_IDM(FM_Adjust_ToString),(ok) ? render : NULL,FALSE);
            }
            F_Dispose(render);
            F_Dispose(select);
         }
         else
         {
            F_Do(Obj,F_IDM(FM_Adjust_ToString),NULL,FALSE);
         }

//         F_Log(0,"NOTIFY>> (%s)",LOD -> spec);
 
         real -> ti_Data = (uint32)(LOD -> spec);
      }
      break;
   }

   F_SUPERDO();
}
//+

///Adjust_DnDQuery
F_METHOD_DNDQUERY(Adjust_DnDQuery)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> preview && (Msg -> Source != LOD -> preview))
   {
//      F_Log(0,"OBJDO %s{%08lx}",_classname(LOD -> preview),LOD -> preview);
      
      if (F_OBJDO(LOD -> preview))
      {
         STRPTR spec = (STRPTR) F_Get(Msg -> Source,F_IDO(FA_Preview_Spec));
   
         if (spec)
         {
            int32 ok;
            
//            F_Log(0,"QUERY (%s) >> BEGIN",spec);
            ok = F_Do(Obj,F_IDM(FM_Adjust_Query),spec);
//            F_Log(0,"QUERY (%s) >> END (0x%08lx)",spec,ok);
         
            if (ok)
            {
               return Obj;
            }
            else
            {
               return NULL;
            }
         }
      }
   }

//   F_Log(0,"SUPERDO");
 
   return (FObject) F_SUPERDO();
}
//+
///Adjust_DnDDrop
F_METHOD_DNDDROP(Adjust_DnDDrop)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
   STRPTR spec;

   /* I forbid rendering because the preview object will be  updated  later
   when the FA_Adjust_Spec attribute will be set */
 
   F_Set(_render,FA_Render_Forbid,TRUE);
   F_OBJDO(LOD -> preview);
   F_Set(_render,FA_Render_Forbid,FALSE);

   spec = (STRPTR) F_Get(LOD -> preview,F_IDO(FA_Preview_Spec));

   F_Set(Obj,F_IDA(FA_Adjust_Spec),(uint32)(spec));
}
//+
///Adjust_DnDDiffer

/* FM_DnDBegin, FM_DnDReport and FM_DnDFinish methods are  passed  directly
to the 'preview' object */

F_METHOD(uint32,Adjust_DnDDiffer)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   return F_OBJDO(LOD -> preview);
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
   F_Log(0,"the method *must* be implemented by a subclass");

   return FALSE;
}
//+
///Adjust_ParseXML

//struct  FS_Adjust_ParseXML                      { STRPTR Source; uint32 SourceType; FDOCID *IDs; uint32 MarkupID; FObject **XMLDocument; uint32 **id_Resolve; };

F_METHODM(FXMLAttribute *,Adjust_ParseXML,FS_Adjust_ParseXML)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (Msg -> Source && Msg -> SourceType & Msg -> MarkupID)
   {
      if (LOD -> XMLDocument)
      {
         F_Do(LOD -> XMLDocument,FM_Set,

            F_IDO(FA_Document_Source),      Msg -> Source,
            F_IDO(FA_Document_SourceType),  Msg -> SourceType,

            TAG_DONE);
      }
      else
      {
         LOD -> XMLDocument = XMLDocumentObject,

            "FA_Document_Source",            Msg -> Source,
            "FA_Document_SourceType",        Msg -> SourceType,

            End;
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
///Adjust_ToString

/* This method should come from  a  subclass,  which  should  have  checked
everything.  This method only clone de specification. If 'Notify' is TRUE I
trigger a notification on FA_Adjust_Spec. */
 
F_METHODM(STRPTR,Adjust_ToString,FS_Adjust_ToString)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    STRPTR rc=NULL;

    /* dispose previous spec */

    F_Dispose(LOD -> spec); LOD -> spec = NULL;

    /* if 'LOD -> text' we are editing two specification (render &  select),
    thus  we  need to update only the needed spec. Otherwise, we simply copy
    the string */
  
    if (LOD -> text)
    {
        if (F_Get(LOD -> preview,FA_Selected))
        {
            F_Dispose(LOD -> select); LOD -> select = NULL;
            
            if (Msg -> Spec)
            {
                LOD -> select = F_StrNew(NULL,"%s",Msg -> Spec); rc = LOD -> select;
            }
        }
        else
        {
            F_Dispose(LOD -> render); LOD -> render = NULL;
            
            if (Msg -> Spec)
            {
                LOD -> render = F_StrNew(NULL,"%s",Msg -> Spec); rc = LOD -> render;
            }
        }

        /* build spec from render and select */
        
        if (LOD -> render)
        {
            if (LOD -> select)
            {
                LOD -> spec = F_StrNew(NULL,"%s%lc%s",LOD -> render,LOD -> separator,LOD -> select);
            }
            else
            {
                LOD -> spec = F_StrNew(NULL,"%s",LOD -> render);
            }
        } 
 //      F_Log(0,"render (%s) select (%s) spec (%s)",LOD -> render,LOD -> select,LOD -> spec);
    }
    else if (Msg -> Spec)
    {
        LOD -> spec = F_StrNew(NULL,"%s",Msg -> Spec); rc = LOD -> spec;
    }
    
    /* update preview object */
  
    F_Set(LOD -> preview,F_IDO(FA_Preview_Spec),(uint32) LOD -> spec);

    /* update text object, which display if the  spec  is  defined  for  the
    current state or empty */
  
    if (LOD -> text)
    {
        adjust_update_text(Class,Obj,FeelinBase);
    }

    /* if 'Msg -> Notify' the method comes from a notified object,  thus  we
    need  to set the FA_Adjust_Spec attribute. Because we have already clone
    de specification, we just passe the FM_Set method to our superclass.
    
    Otherwise, the method has  been  invoked  because  the  FA_Adjust_Object
    attribute  has  been  modified  (either  set  by  the  user,  or  a  DnD
    operation). In the particuliar case, the  method  FM_Adjust_ToObject  in
    invoked to update objects. */
  
    if (Msg -> Notify)
    {
       //F_Log(0,"SET FA_Adjust_Spec (%s)",LOD -> spec);
        F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_Adjust_Spec),LOD -> spec,TAG_DONE);
    }
    else
    {
       //F_Log(0,"TOOBJECT (0x%08lx)(%s)",rc,rc);
        F_Do(Obj,F_IDM(FM_Adjust_ToObject),rc,LOD -> XMLDocument);
    }

    /* disabled 'clear' if there is nothing to clear (no spec defined) */
  
    if (LOD -> spec)
    {
       F_Set(LOD -> reset,FA_Disabled,FF_Disabled_Check | FALSE);
    }
    else
    {
       F_Set(LOD -> reset,FA_Disabled,FF_Disabled_Check | TRUE);
    }

    return rc;
}
//+
///Adjust_ToObject
F_METHODM(uint32,Adjust_ToObject,FS_Adjust_ToObject)
{
//   F_Log(0,"Must be implemented by subclasses (%s)",Msg -> Spec);
   
   return 0;
}
//+
///Adjust_TogglePreview
F_METHOD(void,Adjust_TogglePreview)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   STRPTR spec = NULL;

   if (!LOD -> render)
   {
      if (F_Get(LOD -> preview,FA_Selected))
      {
         F_Do(LOD -> preview,FM_Set,FA_Selected,FALSE,FA_NoNotify,TRUE,TAG_DONE);
      }
      else
      {
         return;
      }
   }
   else
   {
      if (F_Get(LOD -> preview,FA_Selected))
      {
         spec = LOD -> render; F_Do(LOD -> preview,FM_Set,FA_Selected,FALSE,FA_NoNotify,TRUE,TAG_DONE);
      }
      else
      {
         spec = LOD -> select; F_Do(LOD -> preview,FM_Set,FA_Selected,TRUE,FA_NoNotify,TRUE,TAG_DONE);
      }
   }

   adjust_update_text(Class,Obj,FeelinBase);

//   F_Log(0,"toggle images (0x%08lx)(%s)",spec,spec);
   
   F_Do(Obj,F_IDM(FM_Adjust_ToObject),spec,LOD -> XMLDocument);
}
//+

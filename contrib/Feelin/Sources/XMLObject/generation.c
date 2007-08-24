#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

/*** builtin object collection *********************************************/

typedef void (*FMakeFunc)(FObject Obj,FXMLMarkup *Markup,uint32 *Params);

///make_bartitle
void make_bartitle(FObject Obj,FXMLMarkup *Markup,uint32 *Params)
{
   *Params = (uint32) Markup -> Body;
}
//+
///make_checkbox
void make_checkbox(FObject Obj,FXMLMarkup *Markup,uint32 *Params)
{
   *Params = F_Do(Obj,(uint32) "FM_Document_Resolve",Markup -> Body,FV_TYPE_BOOLEAN,NULL,NULL);
}
//+
///make_hgroup
void make_hgroup(FObject Obj,FXMLMarkup *Markup,uint32 *Params)
{
   *Params++ = (uint32) FA_Horizontal;
   *Params   = (uint32) TRUE;
}
//+
///make_hlabel
void make_hlabel(FObject Obj,FXMLMarkup *Markup,uint32 *Params)
{
   *Params++ = (uint32) TRUE;
   *Params   = (uint32) Markup -> Body;
}
//+
///make_page
void make_page(FObject Obj,FXMLMarkup *Markup,uint32 *Params)
{
   *Params++ = FA_Group_PageMode;
   *Params   = TRUE;
}
//+
///make_simplebutton
void make_simplebutton(FObject Obj,FXMLMarkup *Markup,uint32 *Params)
{
   *Params   = (uint32) Markup -> Body;
}
//+
///make_vlabel
void make_vlabel(FObject Obj,FXMLMarkup *Markup,uint32 *Params)
{
   *Params++ = (uint32) FALSE;
   *Params   = (uint32) Markup -> Body;
}
//+

typedef struct FeelinXMLMake
{
    STRPTR                          Name;
    STRPTR                          ClassName;
    uint32                          ID;
    uint32                          Params;
    FMakeFunc                       write;
}                                                           FXMLMake;

static FXMLMake xml_object_make[] =
{
   { "BarTitle",       "Bar",   FV_MakeObj_BarTitle,    1,make_bartitle },
   { "Checkbox",       "Image", FV_MakeObj_Checkbox,    1,make_checkbox },
   { "HGroup",         "Group", FV_MakeObj_None,        2,make_hgroup },
   { "HLabel",         "Text",  FV_MakeObj_Label,       2,make_hlabel },
   { "Page",           "Group", FV_MakeObj_None,        2,make_page },
   { "SimpleButton",   "Text",  FV_MakeObj_Button,      1,make_simplebutton },
   { "VLabel",         "Text",  FV_MakeObj_Label,       2,make_vlabel },

   F_ARRAY_END
};

/***************************************************************************/

///f_create_reference
FHashLink * f_create_reference(FClass *Class,FObject Obj,STRPTR Name,APTR Pool)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
      
   uint32 len=F_StrLen(Name);
   uint32 hash=0;
   FHashLink *link;

   if (!(link = F_HashFind(LOD -> HTable,Name,len,&hash)))
   {
      if ((link = F_NewP(Pool,sizeof (FHashLink) + len + 1)) != NULL)
      {
         link -> Key = (APTR)((uint32)(link) + sizeof (FHashLink));
         link -> KeyLength = len;

         CopyMem(Name,link -> Key,len);

         link -> Next = LOD -> HTable -> Entries[hash];
         LOD -> HTable -> Entries[hash] = link;

//         F_Log(0,"NAME (%s) - NEXT 0x%08lx - HASH 0x%08lx",link -> Key,link -> Next,hash);
      }
   }
   return link;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///XMLObject_Create
F_METHODM(FObject,XMLObject_Create,FS_XMLObject_Create)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   STRPTR cl_name = Msg -> Markup -> Name -> Key;
   FXMLMake *make;
   FClass *cl;

/*** check markup **********************************************************/
 
   if (!Msg -> Markup)
   {
      F_Log(FV_LOG_DEV,"Markup is NULL"); return NULL;
   }

/*** check from definitions ************************************************/

   if (LOD -> Definitions)
   {
      FXMLDefinition *def;

      for (def = LOD -> Definitions ; def -> Name ; def++)
      {
         if (F_StrCmp(Msg -> Markup -> Name -> Key,def -> Name,ALL) == 0)
         {
            if (def -> Data)
            {
               cl_name = ((FClass *)(def -> Data)) -> Name; break;
            }
            else return NULL;
         }
      }
   }

/*** check from built in collection ****************************************/

   for (make = xml_object_make ; make -> Name ; make++)
   {
      int32 cmp = F_StrCmp(make -> Name,Msg -> Markup -> Name -> Key,ALL);

      if (cmp == 0)
      {
         cl_name = make -> ClassName; break;
      }
      else if (cmp > 0)
      {
         make = NULL; break;
      }
   }

   if (make && !make -> Name) make = NULL;

/*** open class ************************************************************/

   if ((cl = F_OpenClass(cl_name)))
   {
      FXMLMarkup *markup;
      FXMLAttribute *attribute;
      uint32 i= (make) ? 1 + make -> Params : 1;
      struct TagItem *tags; 
      
      /* now that we have opened the  class,  we  can  create  and  resolve
      attributes   and   parameters.   Note  well  the  difference  between
      parameters (uint32) and tags (struct TagItem *). If 'make  ->  ID'  is
      different  then FV_MakeObj_None (0) then we use F_MakeObj() to create
      the object, thats why parameters are so important.
      
      Free space is reserved for parameters, right before additional  tags,
      which  must  be  filled  with  parameters  for F_MakeObj() or default
      attributes for F_NewObj(). */
   
      /* attributes */
 
      for (attribute = (FXMLAttribute *)(Msg -> Markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
      {
         i += 2;
      }
                  
      /* children */
 
      for (markup = (FXMLMarkup *)(Msg -> Markup -> ChildrenList.Head) ; markup ; markup = markup -> Next)
      {
         i += 2;
      }
   
      /* we allocate enough memory for parameters (or default  tags),  user
      attributes and TAG_DONE */

      if ((tags = F_NewP(Msg -> Pool,i * sizeof (uint32))))
      {
         struct TagItem *item = tags;
         STRPTR name=NULL;
         FObject obj=NULL;
         
         /*  if  we  are  going  to  create  an  object  from  our  builtin
         collection,  we  call  the  function  'make -> write' to write the
         appropriate parameters or tags before we add markup attributes and
         children */
         
         if (make)
         {
            make -> write(Obj,Msg -> Markup,(uint32 *)(tags)); item = (struct TagItem *)((uint32)(tags) + make -> Params * sizeof (uint32));
         }

         /* Now we parse and add markup attributes. Don't forget to  handle
         the speciale attribute 'name' used to reference the object. */
         
         for (attribute = (FXMLAttribute *)(Msg -> Markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
         {
            UBYTE c = *attribute -> Name -> Key;
            
            if (attribute -> Name -> ID == FV_XMLOBJECT_ID_NAME)
            {
               name = attribute -> Data;
 
               if (name)
               {
                  FHashLink *link;
                  
                  /* before going any further, we check  reference  clones.
                  Reference  name  must  be unique. If the name has already
                  been defined the build fails. */
                  
                  if ((link = F_HashFind(LOD -> HTable,name,F_StrLen(name),NULL)))
                  {
                     F_Do(Obj,F_IDR(FM_Document_Log),attribute -> Line,NULL,"Reference name (%s) used multiple time",name);

                     goto __error;
                  }
               }
            }
            else if (c >= 'A' && c <= 'Z')
            {
               FClassAttribute *ca = F_DynamicFindAttribute(attribute -> Name -> Key,cl,NULL);

               if (ca)
               {
                  int32 resolve_ok=FALSE;

                  item -> ti_Tag = ca -> ID;
                  item -> ti_Data = F_Do(Obj,F_IDM(FM_Document_Resolve),attribute -> Data,ca -> Type,ca -> Values,&resolve_ok);

                  if (!resolve_ok)
                  {
                     F_Do(Obj,F_IDR(FM_Document_Log),attribute -> Line,NULL,"(%s) is not a valid value for (%s)",attribute -> Data,ca -> Name);

                     goto __error;
                  }

                  item++;
               }
               else
               {
                  F_Do(Obj,F_IDR(FM_Document_Log),attribute -> Line,NULL,"Attribute (%s) is not defined by FC_%s",attribute -> Name -> Key,cl -> Name);

                  goto __error;
               }
            }
            else
            {
               F_Do(Obj,F_IDR(FM_Document_Log),attribute -> Line,NULL,"Incorrect Attribute (%s)",attribute -> Name -> Key);

               goto __error;
            }
         }
         
         /* create and add children to the taglist */

         for (markup = (FXMLMarkup *)(Msg -> Markup -> ChildrenList.Head) ; markup ; markup = markup -> Next)
         {
            FObject child = (FObject) F_Do(Obj,F_IDM(FM_XMLObject_Create),markup,Msg -> Pool);

            if (child)
            {
               item -> ti_Tag  = FA_Child;
               item -> ti_Data = (uint32) child;
               item++;
            }
            else goto __error;
         }
      
         /* Eveything is ready, we can create the object now. If  'make  ->
         ID' we use F_MakeObj(), otherwise F_NewObj(). */

         if (make && make -> ID)
         {
            obj = F_MakeObjA(make -> ID,(uint32 *)(tags));
         }
         else
         {
            obj = F_NewObjA(cl -> Name,tags);
         }
      
         if (obj)
         {
            if (name)
            {
               FHashLink *link = f_create_reference(Class,Obj,name,Msg -> Pool);
               
               if (link)
               {
                  link -> Data = obj;
                  
                  if (LOD -> References)
                  {
                     FXMLReference *ref;
                     
                     for (ref = LOD -> References ; ref -> Name ; ref++)
                     {
                        if (F_StrCmp(ref -> Name,name,ALL) == 0)
                        {
                           if (ref -> ObjectPtr)
                           {
                              *ref -> ObjectPtr = obj;
                           }
                        }
                     }
                  }
               }
               else goto __error;
            }
         
            F_Dispose(tags);

            F_CloseClass(cl);
             
            return obj;
         }

/*** error handling ********************************************************/

__error:

         if (obj)
         {
            F_DisposeObj(obj);
         }
         else if (tags)
         {
            struct TagItem *item;
            struct TagItem *next = (make) ? (struct TagItem *)((uint32)(tags) + make -> Params * sizeof (uint32)) : tags;

            /* if the object has not been created we need to  dispose  each
            child found in the taglist */

            while ((item = NextTagItem(&next)) != NULL)
            {
               if (item -> ti_Tag == FA_Child)
               {
                  F_DisposeObj((FObject)(item -> ti_Data)); item -> ti_Tag = TAG_IGNORE;
               }
            }
         }
 
         F_Dispose(tags);
      }
      F_CloseClass(cl);
   }
   else
   {
      F_Log(0,"unable to open class (0x%08lx)(%s)\nMarkup (0x%08lx) Key (0x%08lx)(%s)",cl_name,cl_name,Msg -> Markup,Msg -> Markup -> Name -> Key,Msg -> Markup -> Name -> Key);
   }
   return NULL;
}
//+


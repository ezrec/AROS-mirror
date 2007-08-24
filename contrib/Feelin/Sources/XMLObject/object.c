#include "Private.h"

//#define DB_DEFINITIONS

STATIC FDOCID xml_object_ids[] =
{
    { "name",            4, FV_XMLOBJECT_ID_NAME    },
    { "feelin:objects", 14, FV_XMLOBJECT_ID_OBJECTS },

    F_ARRAY_END
};

/*** Methods ***************************************************************/

///XMLObject_New
F_METHOD(FObject,XMLObject_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_XMLObject_Definitions: LOD -> Definitions = (FXMLDefinition *)(item.ti_Data); break;
        case FA_XMLObject_References:  LOD -> References = (FXMLReference *)(item.ti_Data); break;
        case FA_XMLObject_Tags:        LOD -> InitTags = (struct TagItem *)(item.ti_Data); break;
    }

#ifdef DB_DEFINITIONS
    if (LOD -> Definitions)
    {
        FXMLDefinition *en;
        
        for (en = LOD -> Definitions ; en -> Name ; en++)
        {
            F_Log(0,"XMLDefinition (%s)(%s)(0x%08lx)",en -> Name,en -> Data,en -> Data);
        }
    }
#endif
 
    if (LOD -> References)
    {
        FXMLReference *ref;
        
        for (ref = LOD -> References ; ref -> Name ; ref++)
        {
            if (ref -> ObjectPtr)
            {
                *ref -> ObjectPtr = NULL;
            }
            else F_Log(FV_LOG_DEV,"FXMLReference (%s) : Pointer is NULL",ref -> Name);
        }
    }

    if (F_SUPERDO())
    {
        uint32 markup,pool;
        
        F_Do(Obj,FM_Get,
            
            "FA_Document_Pool", &pool,
            "FA_XMLDocument_Markups", &markup,

            TAG_DONE);

        if (markup && pool)
        {
            LOD -> HTable = F_NewP((APTR)(pool),sizeof (FHashTable) + sizeof (APTR) * FV_HASH_NORMAL);
            
            if (LOD -> HTable)
            {
                LOD -> HTable -> Size = FV_HASH_NORMAL;
                LOD -> HTable -> Entries = (APTR)((uint32)(LOD -> HTable) + sizeof (FHashTable));

                F_Do(Obj,F_IDR(FM_Document_AddIDs),xml_object_ids);
            
                if (F_Do(Obj,F_IDM(FM_XMLObject_Build)))
                {
                    return Obj;
                }
                else
                {
                    F_Log(FV_LOG_DEV,"Building failed");
                }
            }
        }
        else
        {
            F_Log(FV_LOG_DEV,"Document is empty");
        }
    }

    return NULL;
}
//+
///XMLObject_Get
F_METHOD(void,XMLObject_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;
    
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_XMLObject_Definitions:   F_STORE(LOD -> Definitions); break;
        case FA_XMLObject_References:    F_STORE(LOD -> References); break;
        case FA_XMLObject_Tags:          F_STORE(LOD -> BuiltTags); break;
    }

    F_SUPERDO();
}
//+
///XMLObject_Read
F_METHODM(uint32,XMLObject_Read,FS_Document_Read)
{
    if (Msg -> Type == FV_Document_SourceType_File)
    {
        STRPTR buf;
        uint32 rc=NULL;
        BPTR lock;

        if ((lock = Lock(Msg -> Source,ACCESS_READ)) != NULL)
        {
            rc = F_SUPERDO();
        }
        else if ((lock = Lock(FilePart(Msg -> Source),ACCESS_READ)) != NULL)
        {
            rc = F_SuperDo(Class,Obj,Method,FilePart(Msg -> Source),Msg -> Type,Msg -> Pool);
        }
        else if ((buf = F_NewP(Msg -> Pool,1024)) != NULL)
        {
            F_StrFmt(buf,"Feelin:XMLSources");
            AddPart(buf,Msg -> Source,1024);
                
            if ((lock = Lock(buf,ACCESS_READ)) != NULL)
            {
                rc = F_SuperDo(Class, Obj, Method, buf, Msg -> Type, Msg -> Pool);
            }
            else
            {
                F_Log(FV_LOG_DEV,"Unable to lock (%s)",buf);
            }
            
            F_Dispose(buf);
        }
    
        if (lock)
        {
            UnLock(lock); return rc;
        }
        else
        {
            F_Log(FV_LOG_DEV,"Unable to lock (%s)",Msg->Source);
        }
    }
    return F_SUPERDO();
}
//+
///XMLObject_Find
F_METHODM(FObject,XMLObject_Find,FS_XMLObject_Find)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    FHashLink *link = F_HashFind(LOD -> HTable,Msg -> Name,F_StrLen(Msg -> Name),NULL);
    
    if (link)
    {
        return (FObject)(link -> Data);
    }
/*
    FXReference *xref;

    for (xref = (FXReference *)(LOD -> ReferenceList.Head) ; xref ; xref = xref -> Next)
    {
        if (F_StrCmp(Msg -> Name,xref -> Name,ALL) == 0) return xref -> Object;
    }
*/
    F_Log(FV_LOG_DEV,"Object '%s' is not referenced",Msg -> Name);

    return NULL;
}
//+
///XMLObject_Build
F_METHOD(int32,XMLObject_Build)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    APTR pool=NULL;
    FXMLMarkup *markup=NULL;
    
    F_Do(Obj,FM_Get,
        
        "FA_Document_Pool",         &pool,
        "FA_XMLDocument_Markups",   &markup,
        
        TAG_DONE);

/*** searching 'objects' markup ********************************************/
                
    for ( ; markup ; markup = markup -> Next)
    {
        if (markup -> Name -> ID == FV_XMLOBJECT_ID_OBJECTS)
        {
            break;
        }
    }

    if (markup)
    {
        struct TagItem *tags;
        FXMLMarkup *m;
        uint32 n=0;
        
        for (m = (FXMLMarkup *)(markup -> ChildrenList.Head) ; m ; m = m -> Next)
        {
            n++;
        }
    
        tags = F_NewP(pool,sizeof (struct TagItem) * (n + 1));

        if (tags)
        {
            struct TagItem *item = tags;
            
            for (m = (FXMLMarkup *)(markup -> ChildrenList.Head) ; m ; m = m -> Next)
            {
                item -> ti_Tag = FA_Child;
                item -> ti_Data = F_Do(Obj,F_IDM(FM_XMLObject_Create),m,pool);

                if (!item -> ti_Data)
                {
                    for (item = tags ; item -> ti_Tag ; item++)
                    {
                        F_DisposeObj((FObject)(item -> ti_Data));
                    }
                
                    return FALSE;
                }
                item++;
            }
        
            if (LOD -> InitTags)
            {
                item -> ti_Tag = TAG_MORE;
                item -> ti_Data = (uint32)(LOD -> InitTags);
            }
            else
            {
                item -> ti_Tag = TAG_DONE;
                item -> ti_Data = 0;
            }
        
            LOD -> BuiltTags = tags;

            return TRUE;
        }
    }
    else
    {
        F_Do(Obj,F_IDR(FM_Document_Log),0,NULL,"Unable to locate markup 'feelin:objects'");
    }
    
    return FALSE;
}
//+
///XMLObject_Resolve
F_METHODM(uint32,XMLObject_Resolve,FS_Document_Resolve)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 value = 0;
    int32 done=FALSE;

    if (Msg -> Data)
    {

/*** check code definitions ************************************************/

        if (LOD -> Definitions)
        {
            FXMLDefinition *def;

            for (def = LOD -> Definitions ; def -> Name ; def++)
            {
                if (F_StrCmp(Msg -> Data,def -> Name,ALL) == 0) break;
            }

            if (def -> Name)
            {
                value = (uint32)(def -> Data); done = 0xFFFFFFFF;
            }
        }

/*** check objects *********************************************************/
 
        if (!done && (FV_TYPE_OBJECT == Msg -> Type))
        {
            value = F_Do(Obj,F_IDM(FM_XMLObject_Find),Msg -> Data);
 
            if (value)
            {
                done = FV_TYPE_OBJECT;
            }
        }

/*** pass to XMLDocument ***************************************************/
  
        if (!done)
        {
            value = F_SuperDo(Class,Obj,Method,Msg -> Data,Msg -> Type,Msg -> Values,&done);
        }
    }

    if (Msg -> Done)
    {
        *Msg -> Done = done;
    }

    return value;
}
//+

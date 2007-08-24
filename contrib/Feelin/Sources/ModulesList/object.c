#include "Private.h"

//#define DB_SAVE

///typedefs
/*

<?xml version="1.0" ?>

<feelin:modules>
    <module name="" size="" days="" minutes="" tick="">
        <meta>bool</meta>
        <prefs>bool</prefs>
        <decorator>bool</decorator>
    </module>
</feelin:modules>

*/

enum    {

        FV_XML_ID_ROOT = FV_XMLDOCUMENT_ID_DUMMY,
        FV_XML_ID_MODULE,
        FV_XML_ID_NAME,
        FV_XML_ID_SIZE,
        FV_XML_ID_DAYS,
        FV_XML_ID_MINUTE,
        FV_XML_ID_TICK,
        FV_XML_ID_CLASS,
        FV_XML_ID_META_CLASS,
        FV_XML_ID_PREFS,
        FV_XML_ID_META_PREFS,
        FV_XML_ID_DECORATOR_PREFS,
        FV_XML_ID_META_DECORATOR_PREFS
        
        };

FDOCID xml_modules_ids[] =
{
    { "feelin:modules",          14, FV_XML_ID_ROOT                 },
    { "module",                   6, FV_XML_ID_MODULE               },
    { "name",                     4, FV_XML_ID_NAME                 },
    { "size",                     4, FV_XML_ID_SIZE                 },
    { "days",                     4, FV_XML_ID_DAYS                 },
    { "minute",                   6, FV_XML_ID_MINUTE               },
    { "tick",                     4, FV_XML_ID_TICK                 },
    { "class",                    5, FV_XML_ID_CLASS                },
    { "meta-class",              10, FV_XML_ID_META_CLASS           },
    { "prefs",                    5, FV_XML_ID_PREFS                },
    { "meta-prefs",              10, FV_XML_ID_META_PREFS           },
    { "decarator-prefs",         15, FV_XML_ID_DECORATOR_PREFS      },
    { "meta-decarator-prefs",    20, FV_XML_ID_META_DECORATOR_PREFS },
    
    F_ARRAY_END
};

#define F_MARKUP_NAME(id)           xml_modules_ids[id - FV_XMLDOCUMENT_ID_DUMMY].Name
 
struct in_FeelinModuleNode
{
    FModuleNode                     Public;

    uint32                          name_length;
    uint32                          size;
    uint32                          days;
    uint32                          minute;
    uint32                          tick;
};

#define PATH_MAX                                1024

enum    {

        FV_MODULESLIST_CREATE_CLASSES,
        FV_MODULESLIST_CREATE_PREFS,
        FV_MODULESLIST_CREATE_DECORATORPREFS

        };
//+
 
/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///module_update
int32 module_update(FClass *Class, FObject Obj, STRPTR Name,struct in_FeelinModuleNode *node, struct FileInfoBlock *fib)
{
    struct Library *FeelinClassBase = OpenLibrary(Name,0);

//    F_Log(0,"update (%s)(0x%08lx)",Name,FeelinClassBase);
 
    if (FeelinClassBase)
    {
        node->size = fib->fib_Size;
        node->days = fib->fib_Date.ds_Days;
        node->minute = fib->fib_Date.ds_Minute;
        node->tick = fib->fib_Date.ds_Tick;
        
        if (F_Query(FV_Query_ClassTags,FeelinBase))
        {
            node->Public.Flags |= FF_MODULE_CLASS;
        }
        else
        {
            node->Public.Flags &= ~FF_MODULE_CLASS;
        }
        
        if (F_Query(FV_Query_MetaClassTags,FeelinBase))
        {
            node->Public.Flags |= FF_MODULE_METACLASS;
        }
        else
        {
            node->Public.Flags &= ~FF_MODULE_METACLASS;
        }

        if (F_Query(FV_Query_PrefsTags,FeelinBase))
        {
            node->Public.Flags |= FF_MODULE_PREFS;
        }
        else
        {
            node->Public.Flags &= ~FF_MODULE_PREFS;
        }
    
        if (F_Query(FV_Query_MetaPrefsTags,FeelinBase))
        {
            node->Public.Flags |= FF_MODULE_METAPREFS;
        }
        else
        {
            node->Public.Flags &= ~FF_MODULE_METAPREFS;
        }

        if (F_Query(FV_Query_DecoratorPrefsTags,FeelinBase))
        {
            node->Public.Flags |= FF_MODULE_DECORATORPREFS;
        }
        else
        {
            node->Public.Flags &= ~FF_MODULE_DECORATORPREFS;
        }

        if (F_Query(FV_Query_MetaDecoratorPrefsTags,FeelinBase))
        {
            node->Public.Flags |= FF_MODULE_METADECORATORPREFS;
        }
        else
        {
            node->Public.Flags &= ~FF_MODULE_METADECORATORPREFS;
        }

        CloseLibrary(FeelinClassBase);
 
        return TRUE;
    }
    else
    {
        F_Log(0,"Unable to open (%s)",node->Public.Name);
    }
    return FALSE;
}
//+

///modules_list_load
int32 modules_list_load(FClass *Class, FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    
    int32 modified = FALSE;

    BPTR file = Open(LOD->fast, MODE_OLDFILE);
    
    if (file)
    {
        FObject doc = XMLDocumentObject,

            "FA_Document_Source", LOD->fast,
            "FA_Document_SourceType", FV_Document_SourceType_File,

            End;

        if (doc)
        {
            uint32 id_Resolve = F_DynamicFindID("FM_Document_Resolve");
            FXMLMarkup *root;
            
            F_Do(doc, (uint32) "FM_Document_AddIDs", xml_modules_ids);
        
            for (root = (FXMLMarkup *) F_Get(doc, (uint32) "FA_XMLDocument_Markups") ; root ; root = root->Next)
            {
                if (root->Name->ID == FV_XML_ID_ROOT)
                {
                    FXMLMarkup *markup;
                    
                    for (markup = (FXMLMarkup *) root->ChildrenList.Head ; markup ; markup = markup->Next)
                    {
                        if (markup->Name->ID == FV_XML_ID_MODULE)
                        {
    /*
                            F_Log(0,"MODULE MARKUP (0x%08lx)(%s) ATTRIBUTES (0x%08lx) CHILDREN (0x%08lx)",
                                markup,markup->Name->Key, markup->AttributesList.Head, markup->ChildrenList.Head);
    */
     
                            #if 1
                            FXMLAttribute *attribute;
                            
                            STRPTR name=NULL;
                            STRPTR name_data=NULL;
                            uint32 name_length=0;
                            uint32 size=0;
                            uint32 days=0;
                            uint32 minute=0;
                            uint32 tick=0;
                            
                            for (attribute = (FXMLAttribute *) markup->AttributesList.Head ; attribute ; attribute = attribute->Next)
                            {
                                switch (attribute->Name->ID)
                                {
                                    case FV_XML_ID_NAME:
                                    {
                                        name_data = attribute->Data;
                                        name_length = F_StrLen(attribute->Data);
                                    }
                                    break;
                                
                                    case FV_XML_ID_SIZE:
                                    {
                                        size = F_Do(doc, id_Resolve, attribute->Data, FV_TYPE_INTEGER, NULL, NULL);
                                    }
                                    break;
                                    
                                    case FV_XML_ID_DAYS:
                                    {
                                        days = F_Do(doc, id_Resolve, attribute->Data, FV_TYPE_INTEGER, NULL, NULL);
                                    }
                                    break;
                                    
                                    case FV_XML_ID_MINUTE:
                                    {
                                        minute = F_Do(doc, id_Resolve, attribute->Data, FV_TYPE_INTEGER, NULL, NULL);
                                    }
                                    break;
                                    
                                    case FV_XML_ID_TICK:
                                    {
                                        tick = F_Do(doc, id_Resolve, attribute->Data, FV_TYPE_INTEGER, NULL, NULL);
                                    }
                                    break;
                                }
                            }
                        
                            if (name_data && name_length)
                            {
                                BPTR lock;
                                
                                F_StrFmt(LOD->buffer, "%s", LOD->path);
                                AddPart(LOD->buffer, name_data, PATH_MAX);
                                
                                lock = Lock(LOD->buffer, ACCESS_READ);
                                
                                if (lock == NULL)
                                {
//                                    F_Log(0,"(%s) deprecated", LOD->buffer);
     
                                    modified = TRUE;
                                    
                                    break;
                                }
                            
                                UnLock(lock);
     
                                name = F_NewP(LOD->pool, name_length + 1);

                                if (name)
                                {
                                    CopyMem(name_data, name, name_length);
                                }
                            }
     

                            if (name)
                            {
                                struct in_FeelinModuleNode *node = F_NewP(LOD->pool, sizeof (struct in_FeelinModuleNode));
                                
                                if (node)
                                {
                                    FXMLMarkup *child;
     
                                    node->Public.Name = name;
                                    node->name_length = name_length;
                                    node->size = size;
                                    node->days = days;
                                    node->minute = minute;
                                    node->tick = tick;
                                    
                                    for (child = (FXMLMarkup *) markup->ChildrenList.Head ; child ; child = child->Next)
                                    {
                                        switch (child->Name->ID)
                                        {
                                        
                                            case FV_XML_ID_CLASS:
                                            {
                                                if (F_Do(doc, id_Resolve, child->Body, FV_TYPE_BOOLEAN, NULL, NULL))
                                                {
                                                    node->Public.Flags |= FF_MODULE_CLASS;
                                                }
                                            }
                                            break;
                                             
                                            case FV_XML_ID_META_CLASS:
                                            {
                                                if (F_Do(doc, id_Resolve, child->Body, FV_TYPE_BOOLEAN, NULL, NULL))
                                                {
                                                    node->Public.Flags |= FF_MODULE_METACLASS;
                                                }
                                            }
                                            break;
                                             
                                            case FV_XML_ID_PREFS:
                                            {
                                                if (F_Do(doc, id_Resolve, child->Body, FV_TYPE_BOOLEAN, NULL, NULL))
                                                {
                                                    node->Public.Flags |= FF_MODULE_PREFS;
                                                }
                                            }
                                            break;
                                             
                                            case FV_XML_ID_META_PREFS:
                                            {
                                                if (F_Do(doc, id_Resolve, child->Body, FV_TYPE_BOOLEAN, NULL, NULL))
                                                {
                                                    node->Public.Flags |= FF_MODULE_METAPREFS;
                                                }
                                            }
                                            break;
                                             
                                            case FV_XML_ID_DECORATOR_PREFS:
                                            {
                                                if (F_Do(doc, id_Resolve, child->Body, FV_TYPE_BOOLEAN, NULL, NULL))
                                                {
                                                    node->Public.Flags |= FF_MODULE_DECORATORPREFS;
                                                }
                                            }
                                            break;
                                             
                                            case FV_XML_ID_META_DECORATOR_PREFS:
                                            {
                                                if (F_Do(doc, id_Resolve, child->Body, FV_TYPE_BOOLEAN, NULL, NULL))
                                                {
                                                    node->Public.Flags |= FF_MODULE_METADECORATORPREFS;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    
                                    #if 0
                                    F_Log(0,"loaded node : (%s)(0x%08lx) (%ld) (%ld)(%ld)(%ld)",
                                    
                                    node->Public.Name,
                                    node->Public.Flags,
                                    node->size,
                                    node->days,
                                    node->minute,
                                    node->tick);
                                    #endif
                                    
                                    F_LinkTail(&LOD->list, (FNode *) node);
                                    
    //                                F_Log(0,"HEAD 0x%08lx TAIL 0x%08lx NODE 0x%08lx",LOD->list.Head,LOD->list.Tail,node);
                                }
                            }
                            #endif
                        }
                    }
                }
            }
        
            F_DisposeObj(doc);
        }
        Close(file);
    }

    return modified;
}
//+
///modules_list_read

/* returns TRUE is the list was updated */
 
int32 modules_list_read(FClass *Class, FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);

    int32 update = FALSE;
    
    BPTR lock = Lock(LOD->path,ACCESS_READ);

    if (lock)
    {
        struct FileInfoBlock *fib = AllocDosObject(DOS_FIB,NULL);

        if (fib)
        {
            if (Examine(lock, fib))
            {
                while (ExNext(lock, fib))
                {
		#ifdef __AROS__
		#warning "AROS CHECKME EntryType <-> DirEntryType\n");
                    if (fib -> fib_DirEntryType < 0)
		#else
                    if (fib -> fib_EntryType < 0)
		#endif
                    {
                        uint32 len = F_StrLen(fib->fib_FileName);
                        struct in_FeelinModuleNode *node;
                        
                        F_StrFmt(LOD->buffer, "%s", LOD->path);
                        AddPart(LOD->buffer, fib->fib_FileName, PATH_MAX);
                        
                        //F_Log(0,"(%s)",buf);

                        for (node = (struct in_FeelinModuleNode *) LOD->list.Head ; node ; node = (struct in_FeelinModuleNode *) node->Public.Next)
                        {
                            if ((len == node->name_length) &&
                                (F_StrCmp(fib->fib_FileName, node->Public.Name, ALL) == 0))
                            {
                                break;
                            }
                        }

                        if (node == NULL)
                        {
                            node = F_NewP(LOD->pool, sizeof (struct in_FeelinModuleNode));
                             
//                                F_Log(0,"new file (%s)",fib->fib_FileName);
                            
                            if (node)
                            {
                                node->Public.Name = F_NewP(LOD->pool, len + 1);
                                node->name_length = len;
                                
                                if (node->Public.Name)
                                {
                                    CopyMem(fib->fib_FileName, node->Public.Name, len);
                                    
                                    if (module_update(Class,Obj,LOD->buffer,node,fib))
                                    {
                                        update = TRUE;
                                        
                                        F_LinkTail(&LOD->list, (FNode *) node);
                                    }
                                }
                            }
                        }
                        else if (node->size != fib->fib_Size ||
                             node->days != fib->fib_Date.ds_Days ||
                             node->minute != fib->fib_Date.ds_Minute ||
                             node->tick != fib->fib_Date.ds_Tick)
                        {
                            update = TRUE;
                            
                            module_update(Class,Obj,LOD->buffer,node,fib);
                            
//                                F_Log(0,"update file (%s)",node->Public.Name);
                        }
                    }
                }
            }

            FreeDosObject(DOS_FIB,fib);
        }

        UnLock(lock);
    }

    return update;
}
//+
///modules_list_save
void modules_list_save(FClass *Class, FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
 
    BPTR file = Open(LOD->fast,MODE_NEWFILE);
    
    if (file)
    {
        struct in_FeelinModuleNode *node;
        
        #ifdef DB_SAVE
        uint32 i=0;

        F_Log(0,"file (0x%08lx)(%s)",file,LOD->fast);
        #endif
 
        FPrintf(file, "<?xml version='1.0' ?>\n\n");
        FPrintf(file, "<%s>\n",F_MARKUP_NAME(FV_XML_ID_ROOT));
 
        for (node = (struct in_FeelinModuleNode *) LOD->list.Head ; node ; node = (struct in_FeelinModuleNode *) node->Public.Next)
        {
            #ifdef DB_SAVE
            i++;
            #endif
 
            FPrintf(file, "\t<%s",F_MARKUP_NAME(FV_XML_ID_MODULE));
            FPrintf(file, " %s='%s'",F_MARKUP_NAME(FV_XML_ID_NAME),node->Public.Name);
            FPrintf(file, " %s='%ld'",F_MARKUP_NAME(FV_XML_ID_SIZE),node->size);
            FPrintf(file, " %s='%ld'",F_MARKUP_NAME(FV_XML_ID_DAYS),node->days);
            FPrintf(file, " %s='%ld'",F_MARKUP_NAME(FV_XML_ID_MINUTE),node->minute);
            FPrintf(file, " %s='%ld'>\n",F_MARKUP_NAME(FV_XML_ID_TICK),node->tick);

            if (FF_MODULE_CLASS & node->Public.Flags)
            {
                FPrintf(file, "\t\t<%s>true</%s>\n",F_MARKUP_NAME(FV_XML_ID_CLASS),F_MARKUP_NAME(FV_XML_ID_CLASS));
            }
            
            if (FF_MODULE_METACLASS & node->Public.Flags)
            {
                FPrintf(file, "\t\t<%s>true</%s>\n",F_MARKUP_NAME(FV_XML_ID_META_CLASS),F_MARKUP_NAME(FV_XML_ID_META_CLASS));
            }
            
            if (FF_MODULE_PREFS & node->Public.Flags)
            {
                FPrintf(file, "\t\t<%s>true</%s>\n",F_MARKUP_NAME(FV_XML_ID_PREFS),F_MARKUP_NAME(FV_XML_ID_PREFS));
            }
 
            if (FF_MODULE_METAPREFS & node->Public.Flags)
            {
                FPrintf(file, "\t\t<%s>true</%s>\n",F_MARKUP_NAME(FV_XML_ID_META_PREFS),F_MARKUP_NAME(FV_XML_ID_META_PREFS));
            }
            
            if (FF_MODULE_DECORATORPREFS & node->Public.Flags)
            {
                FPrintf(file, "\t\t<%s>true</%s>\n",F_MARKUP_NAME(FV_XML_ID_DECORATOR_PREFS),F_MARKUP_NAME(FV_XML_ID_DECORATOR_PREFS));
            }
            
            if (FF_MODULE_METADECORATORPREFS & node->Public.Flags)
            {
                FPrintf(file, "\t\t<%s>true</%s>\n",F_MARKUP_NAME(FV_XML_ID_META_DECORATOR_PREFS),F_MARKUP_NAME(FV_XML_ID_META_DECORATOR_PREFS));
            }
        
            FPrintf(file, "\t</%s>\n",F_MARKUP_NAME(FV_XML_ID_MODULE));
        }
        
        #ifdef DB_SAVE
        F_Log(0,"%ld module info saved",i);
        #endif
 
        FPrintf(file, "</%s>",F_MARKUP_NAME(FV_XML_ID_ROOT));
        
        Close(file);
    }
    else
    {
        F_Log(FV_LOG_DEV,"Unable to create (%s)",LOD->fast);
    }
}
//+
///modules_list_create
FModuleCreated * modules_list_create(FClass *Class, FObject Obj, uint32 Which)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);

    FList *list=NULL;

    #if 0
    bits32 meta_flags;
    uint32 meta_query;
    STRPTR meta_add;
    #endif
     
    bits32 class_flags=0;
    uint32 class_query=0;
    STRPTR class_add=NULL;

    STRPTR buf;

    struct in_FeelinModuleNode *node;

//    F_Log(0,"WHICH (%ld)",Msg->Which);

    switch (Which)
    {
        case FV_MODULESLIST_CREATE_CLASSES:
        {
            list = &LOD->list_class;

            #if 0
            meta_flags = FF_MODULE_METACLASS;
            meta_query = FV_Query_MetaClassTags;
            meta_add = "meta";
            #endif
             
            class_flags = FF_MODULE_CLASS;
            class_query = FV_Query_ClassTags;
            class_add = NULL;
        }
        break;

        case FV_MODULESLIST_CREATE_PREFS:
        {
            list = &LOD->list_prefs;

            #if 0
            meta_flags = FF_MODULE_METAPREFS;
            meta_query = FV_Query_MetaPrefsTags;
            meta_add = "meta-prefs";
            #endif

            class_flags = FF_MODULE_PREFS;
            class_query = FV_Query_PrefsTags;
            class_add = "prefs";
        }
        break;

        case FV_MODULESLIST_CREATE_DECORATORPREFS:
        {
            list = &LOD->list_decoratorprefs;

            #if 0
            meta_flags = FF_MODULE_METADECORATORPREFS;
            meta_query = FV_Query_MetaDecoratorPrefsTags;
            meta_add = "meta-decorator-prefs";
            #endif

            class_flags = FF_MODULE_DECORATORPREFS;
            class_query = FV_Query_DecoratorPrefsTags;
            class_add = "decorator-prefs";
        }
        break;
    }

    if (list->Head)
    {
        return (FModuleCreated *) list->Head;
    }

    #if 0
    F_Log(0,"LIST 0x%08lx - META (0x%08lx, 0x%08lx, '%s') - CLASS (0x%08lx, 0x%08lx, '%s')",

    list,

    meta_flags,
    meta_query,
    meta_add,
    class_flags,
    class_query,
    class_add
    #endif

    buf = F_NewP(LOD->pool, PATH_MAX);

    if (buf)
    {
        for (node = (struct in_FeelinModuleNode *) LOD->list.Head ; node ; node = (struct in_FeelinModuleNode *) node->Public.Next)
        {
//            F_Log(0,"node (0x%08lx)(%s)",node,node->Public.Name);

            if (node->Public.Flags & class_flags)
            {
                FModuleCreated *created = F_NewP(LOD->pool,sizeof (FModuleCreated));

                F_StrFmt(buf, "%s", LOD->path);
                AddPart(buf, node->Public.Name, PATH_MAX);

                if (created)
                {
                    struct Library *FeelinClassBase = OpenLibrary(buf,0);

                    if (FeelinClassBase)
                    {
                        struct TagItem *tags = F_Query(class_query, FeelinBase);
                        FClass *cl=NULL;

                        if (tags)
                        {
                            STRPTR name = F_StrNew(NULL,":%s:%s",class_add,node->Public.Name);

                            cl = F_NewObj(FC_Class, FA_Class_Name,name, TAG_MORE, tags);

                            F_Dispose(name);
                        }

                        if (cl)
                        {
                            created->Module = FeelinClassBase;
                            created->Class = cl;

//                            F_Log(0,"created (0x%08lx)(%s) >> Module (0x%08lx) Class (0x%08lx)",created,cl->Name,created->Module,created->Class);

                            F_LinkTail(list, (FNode *) created);
                        }
                        else
                        {
                            F_Dispose(created);
                            CloseLibrary(FeelinClassBase);
                        }
                    }
                    else
                    {
                        F_Dispose(created);
                    }
                }

#if 0
///
                        struct TagItem *tags = F_Query(meta_query, FeelinBase);

                        created->Module = FeelinClassBase;

                        /* Before creating the class, I request  FV_Query_MetaTags.
                        These tags are used to create the class of the class object
                        (the meta class). */

                        if (tags)
                        {
                            STRPTR meta_name = F_StrNew(NULL,":%s:%s",meta_add,node->Public.Name);

                            created->Meta = F_NewObj(FC_Class,

                                  FA_Class_Super,   FC_Class,
                                  FA_Class_Pool,    FALSE,
                                  FA_Class_Name,    meta_name,

                                  TAG_MORE, tags);

                            F_Dispose(meta_name);

                            if (meta)
                            {
                                tags = F_Query(class_query, FeelinBase);

                                if (tags)
                                {
                                    STRPTR class_name = F_StrNew(NULL,":%s:%s",class_add,node->Public.Name);

                                    created->Class = F_NewObj(meta -> Public.Name,

                                        FA_Class_Name, class_name,

                                        TAG_MORE, tags))
                                }
                            }
                        }
                        else
                        {
                            tags = F_Query(class_query, FeelinBase);

                            if (tags)
                            {
                                cl = F_NewObj(FC_Class,

                                    TAG_MORE, tags);
                            }
                        }

                        if (!cl)
                        {
                            CloseLibrary(FeelinClassBase);
                        }
//+
#endif
            }
        }

        F_Dispose(buf);
    }

    return (FModuleCreated *) list->Head;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///ML_New
F_METHOD(uint32, ML_New)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = Msg,item;

    LOD->path = "LIBS:Feelin/";
    LOD->fast = "s:.feelin-modules.xml";

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
       case FA_ModulesList_Path:
       {
          LOD->path = (STRPTR) item.ti_Data;
       }
       break;

       case FA_ModulesList_Fast:
       {
          LOD->fast = (STRPTR) item.ti_Data;
       }
       break;
    }
    
    if (LOD->path == NULL || LOD->fast == NULL)
    {
        return NULL;
    }

    LOD->pool = F_CreatePool(4096, FA_Pool_Items,1, FA_Pool_Name,"modules-list", TAG_DONE);

    if (LOD->pool)
    {
        if ((LOD->buffer = F_NewP(LOD->pool, PATH_MAX)) != NULL)
        {
            int32 modified = FALSE;
            
            if (modules_list_load(Class, Obj))
            {
                modified = TRUE;
            }
            
            if (modules_list_read(Class, Obj))
            {
                modified = TRUE;
            }
        
            if (modified)
            {
                modules_list_save(Class, Obj);
            }
            
            #if 0
            {
                uint32 i=0;
                struct in_FeelinModuleNode *node;
             
                for (node = (struct in_FeelinModuleNode *) LOD->list.Head ; node ; node = (struct in_FeelinModuleNode *) node->Public.Next)
                {
                    i++;
         
        //            F_Log(0,"NODE (0x%08lx)(%s) NEXT 0x%08lx",node,node->Public.Name,node->Public.Next);
                }
        
                F_Log(0,">>> %ld NODES",i);
            }
            #endif

            return F_SUPERDO();
        }
    }
    return NULL;
}
//+
///ML_Dispose
F_METHOD(void, ML_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    
    FModuleCreated *node;
 
    for (node = (FModuleCreated *) LOD->list_class.Head ; node ; node = node->Next)
    {
        F_DeleteClass(node->Class);
        F_DeleteClass(node->Meta);
        CloseLibrary(node->Module);
    }

    for (node = (FModuleCreated *) LOD->list_prefs.Head ; node ; node = node->Next)
    {
        F_DeleteClass(node->Class);
        F_DeleteClass(node->Meta);
        CloseLibrary(node->Module);
    }
    
    for (node = (FModuleCreated *) LOD->list_decoratorprefs.Head ; node ; node = node->Next)
    {
        F_DeleteClass(node->Class);
        F_DeleteClass(node->Meta);
        CloseLibrary(node->Module);
    }
    
    F_DeletePool(LOD->pool); LOD->pool = NULL;

    F_SUPERDO();
}
//+
///ML_Get
F_METHOD(void, ML_Get)
{
    struct TagItem *Tags = Msg,item;
    
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_ModulesList_Classes:
        {
            F_STORE(modules_list_create(Class, Obj, FV_MODULESLIST_CREATE_CLASSES));
        }
        break;

        case FA_ModulesList_Prefs:
        {
            F_STORE(modules_list_create(Class, Obj, FV_MODULESLIST_CREATE_PREFS));
        }
        break;

        case FA_ModulesList_DecoratorPrefs:
        {
            F_STORE(modules_list_create(Class, Obj, FV_MODULESLIST_CREATE_DECORATORPREFS));
        }
        break;
    }
    
    F_SUPERDO();
}
//+

#include "Private.h"

///PG_New
F_METHOD(FObject,PG_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;
    struct TagItem complete_tags[3];

    STRPTR source=NULL;
    FXMLDefinition *defs=NULL;
    FXMLReference *refs=NULL;
 
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_PreferenceGroup_XMLSource:        source = (STRPTR)(item.ti_Data); break;
        case FA_PreferenceGroup_XMLDefinitions:   defs = (FXMLDefinition *)(item.ti_Data); break;
        case FA_PreferenceGroup_XMLReferences:    refs = (FXMLReference *)(item.ti_Data); break;
    
        case FA_PreferenceGroup_Script:           LOD -> Script = (FPreferenceScript *)(item.ti_Data); break;
    }

    /*** create preference script data if script has been defined ***/

    if (LOD -> Script)
    {
        FXMLReference *usr_r;
        FPreferenceScript *en;
        uint32 n=0;

        for (en = LOD -> Script ; en -> Name ; en++)
        {
            if (*en -> Name != '$')
            {
                F_Log(0,"INVALID (%s)",en -> Name);

                goto __error;
            }

            n++;
        }
    
        if (refs)
        {
            for (usr_r = refs ; usr_r -> Name ; usr_r++)
            {
                n++;
            }
        }
    
        if ((LOD -> References = F_New((n + 1) * (sizeof (FXMLReference) + sizeof (FObject *)))) != NULL)
        {
            FXMLReference *r = LOD -> References;
            FObject *ptr = (FObject *)((uint32)(LOD -> References) + (n + 1) * sizeof (FXMLReference));
            
            LOD -> Objects = ptr;
            
            /*** script references ***/
            
            for (en = LOD -> Script ; en -> Name ; en++, r++)
            {
                r -> Name = en -> Name + 1;
                r -> ObjectPtr = ptr++;
            }

            /*** user references ***/

            if (refs)
            {
                for (usr_r = refs ; usr_r -> Name ; usr_r++, r++)
                {
                    r -> Name = usr_r -> Name;
                    r -> ObjectPtr = usr_r -> ObjectPtr;
                }
            }

            refs = LOD -> References;
        }
        else goto __error;
    }

    complete_tags[0].ti_Tag  = FA_ChainToCycle;
    complete_tags[0].ti_Data = FALSE;
    complete_tags[1].ti_Tag  = FA_Frame;
    complete_tags[1].ti_Data = (uint32) "$preference-frame";
    complete_tags[2].ti_Tag  = TAG_MORE;
    complete_tags[2].ti_Data = (uint32) Msg;

    if (source)
    {
        LOD -> XMLObject = XMLObjectObject,
        
            "Source",      source,
            "SourceType",  "File",
            "Definitions", defs,
            "References",  refs,
            "Tags",        Msg,
        
            End;
        
        if (LOD -> XMLObject)
        {
            complete_tags[2].ti_Data = F_Get(LOD -> XMLObject,(uint32) "FA_XMLObject_Tags");
        }
        else goto __error;
    }

    if (F_SuperDoA(Class,Obj,Method,complete_tags))
    {
        uint32 i=0;
        
        Tags = complete_tags;

        while (F_DynamicNTI(&Tags,&item,NULL))
        {
            if (item.ti_Tag == FA_Child)
            {
                i++;
            }
        }

        if (i)
        {
            if (LOD -> Script)
            {
                FPreferenceScript *s;
                        
                for (i = 0, s = LOD -> Script ; s -> Name ; i++, s++)
                {
                    if (!LOD -> Objects[i])
                    {
                        F_Log(0,"(%s) not found in (%s)",s -> Name + 1,source);
                    }
                }
            }
                    
            return Obj;
        }
        else
        {
            F_Log(FV_LOG_DEV,"PreferenceGroup have no children - source (%s)",source);
        }
    }

__error:

    Tags = Msg;
    
    while ((tag = F_DynamicNTI(&Tags,&item,NULL)) != NULL)
    {
        if (item.ti_Tag == FA_Child && item.ti_Data)
        {
            F_Log(0,"disposed %s",_classname(item.ti_Data));
 
            F_DisposeObj((FObject)(item.ti_Data)); tag -> ti_Tag = TAG_IGNORE; tag -> ti_Data = NULL;
        }
    }
 
    return NULL;
}
//+
///PG_Dispose
F_METHOD(void,PG_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
    F_SUPERDO();
    
    F_DisposeObj(LOD -> XMLObject);
    F_Dispose(LOD -> References);
}
//+
///PG_Get
F_METHOD(void,PG_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_PreferenceGroup_XMLObject: F_STORE(LOD -> XMLObject); break;
    }
    
    F_SUPERDO();
}
//+

///PG_Load
F_METHODM(void,PG_Load,FS_PreferenceGroup_Load)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Script && LOD -> References)
    {
        FPreferenceScript *s;
        uint32 i;
        
        for (s = LOD -> Script, i = 0 ; s -> Name ; s++, i++)
        {
            uint32 data = F_Do(Msg -> Prefs,(s -> Type == FV_TYPE_INTEGER) ? Msg -> id_ResolveInt : Msg -> id_Resolve,s -> Name,s -> Default);

            F_Set(LOD -> Objects[i],(uint32) s -> Attribute,data);
        }
    }
}
//+
///PG_Save
F_METHODM(void,PG_Save,FS_PreferenceGroup_Save)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Script && LOD -> References)
    {
        FPreferenceScript *s;
        uint32 i;

        for (s = LOD -> Script, i = 0 ; s -> Name ; s++, i++)
        {
            uint32 data = F_Get(LOD -> Objects[i],(uint32) s -> Attribute);
            uint32 id;
            
            switch (s -> Type)
            {
                case FV_TYPE_STRING:
                {
                    id = Msg -> id_AddString;
                }
                break;
                
                case FV_TYPE_POINTER:
                {
                    id = Msg -> id_Add;
                }
                break;
                
                default:
                {
                    id = Msg -> id_AddLong;
                }
                break;
            }
            
            F_Do(Msg -> Prefs,id,s -> Name,data,s -> Length);
        }
    }
}
//+

#include "Private.h"

STATIC FPreferenceScript Script[] =
{
    { "$decorator-preparse-active",    FV_TYPE_STRING,  "Contents", NULL,0 },
    { "$decorator-preparse-inactive",  FV_TYPE_STRING,  "Contents", NULL,0 },
    { "$decorator-scheme-active",      FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$decorator-scheme-inactive",    FV_TYPE_STRING,  "Spec",     "<scheme shine='fill' fill='halfshadow' dark='shadow' text='halfdark' highlight='dark' />",0 },
    { "$decorator-font",               FV_TYPE_STRING,  "Contents", NULL,0 },
    
    F_ARRAY_END
};

struct FS_SELECT                                { FClass *Class; uint32 Active; };

///code_select_decorator

F_HOOKM(void,code_select_decorator,FS_SELECT)
{
    struct p_LocalObjectData *LOD = F_LOD(Msg -> Class,Obj);

    if (Msg->Active == 0)
    {
        if (LOD->active_decorator)
        {
            F_Do(Obj, FM_Group_InitChange);
            
            F_Do(Obj, FM_RemMember, LOD->active_decorator); LOD->active_decorator = NULL;
            
            F_Do(Obj, FM_Group_ExitChange);
        }
        F_Set(LOD->grp_edit, FA_Disabled, FF_Disabled_Check | TRUE);
    }
    else
    {
        struct FeelinDGroup *gnode;
        
        for (gnode = (struct FeelinDGroup *) LOD->dglist.Head ; gnode ; gnode = gnode->next)
        {
            if (gnode->position == Msg->Active)
            {
                break;
            }
        }
    
        if (gnode->object != LOD->active_decorator)
        {
            F_Do(Obj, FM_Group_InitChange);

            F_Do(Obj, FM_RemMember, LOD->active_decorator); LOD->active_decorator = NULL;
            
            if (gnode)
            {
                LOD->active_decorator = gnode->object;
 
                F_Do(Obj, FM_AddMember, LOD->active_decorator, FV_AddMember_Tail);
            }

            F_Do(Obj, FM_Group_ExitChange);
        }
        F_Set(LOD->grp_edit, FA_Disabled, FF_Disabled_Check | FALSE);
    }
}
//+

///Prefs_New
F_METHOD(FObject,Prefs_New)
{
    struct p_LocalObjectData *LOD = F_LOD(Class,Obj);
                    
    F_XML_REFS_INIT(3);
    F_XML_REFS_ADD("decorator-class",&LOD -> chooser);
    F_XML_REFS_ADD("group-edit",&LOD -> grp_edit);
    F_XML_REFS_DONE;

    LOD->moduleslist = F_SharedOpen("ModulesList");
    
    if (LOD->moduleslist)
    {
        struct FeelinDGroup *gnode;
        FModuleCreated *cnode;
        uint32 i=1;
        
        for (cnode = (FModuleCreated *) F_Get(LOD->moduleslist, (uint32) "FA_ModulesList_DecoratorPrefs") ; cnode ; cnode = cnode -> Next)
        {
            gnode = F_New(sizeof (struct FeelinDGroup));
            
            if (gnode)
            {
                gnode->object = F_NewObj(cnode->Class->Name,TAG_DONE);
                gnode->classname = F_StrNew(NULL,"%s",cnode->Module->lib_Node.ln_Name);
                
                if (gnode->object && gnode->classname)
                {
                    struct FeelinDGroup *gprev;
                    STRPTR c = gnode->classname;
                    
                    while (*c && *c != '.') c++;
                        
                    if (*c == '.')
                    {
                        *c = '\0';
                    }

                    gnode -> name = (STRPTR) F_Get(gnode->object, FA_Group_PageTitle);

                    for (gprev = (struct FeelinDGroup *) LOD->dglist.Tail ; gprev ; gprev = gprev->prev)
                    {
                       if (F_StrCmp(gnode->name,gprev->name,ALL) > 0) break;
                    }

                    F_LinkInsert(&LOD->dglist,(FNode *) gnode,(FNode *) gprev);
                
//                    F_Log(0,"gnode (0x%08lx) name (%s) class (%s)",gnode,gnode->name,gnode->classname);
                    
                    continue;
                }
                
                F_Log(FV_LOG_DEV,"Unable to create object from %s",cnode -> Module -> lib_Node.ln_Name);
            
                F_DisposeObj(gnode->object);
                F_Dispose(gnode->classname);
                F_Dispose(gnode);
            }
        }
    
        for (gnode = (struct FeelinDGroup *) LOD -> dglist.Head ; gnode ; gnode = gnode->next)
        {
            gnode->position = i++;
        }
    
        LOD->cycle_array = F_New(sizeof (STRPTR) * (i + 1));
        
        if (LOD->cycle_array)
        {
            STRPTR *ar = LOD->cycle_array;
            
            *ar++ = (STRPTR) "none, use Intuition";
            
            for (gnode = (struct FeelinDGroup *) LOD -> dglist.Head ; gnode ; gnode = gnode->next)
            {
                *ar++ = gnode->name;
            }
        }
    }

    if (LOD->cycle_array)
    {
        if (F_SuperDo(Class,Obj,Method,
                                            
            FA_Group_PageTitle, "Decorator",

            "Script",         Script,
            "XMLSource",      "feelin/preference/decorator.xml",
            "XMLReferences",  F_XML_REFS,

        TAG_MORE,Msg))
        
        {
            F_Do(LOD->chooser, FM_Set,
            
                "FA_Cycle_Entries", LOD->cycle_array,
                "FA_Cycle_EntriesType", FV_Cycle_EntriesType_Array,
                
                TAG_DONE);
            
            F_Do(LOD->chooser, FM_Notify, "FA_Cycle_Active",FV_Notify_Always, Obj,FM_CallHookEntry,3, code_select_decorator,Class,FV_Notify_Value);
 
            F_Set(LOD -> chooser,(uint32) "FA_Cycle_Active", 0);

            return Obj;
        }
    }
    return NULL;
}
//+
///Prefs_Dispose
F_METHOD(void, Prefs_Dispose)
{
    struct p_LocalObjectData *LOD = F_LOD(Class,Obj);
    
    struct FeelinDGroup *gnode;
    
    while ((gnode = F_LinkRemove(&LOD->dglist, LOD->dglist.Head)) != NULL)
    {
        F_DisposeObj(gnode->object);
        F_Dispose(gnode->classname);
        F_Dispose(gnode);
    }
    
    F_SUPERDO();
    
    F_Dispose(LOD->cycle_array); LOD->cycle_array = NULL;
 
    F_SharedClose(LOD->moduleslist); LOD->moduleslist = NULL;
}
//+

///Prefs_Load
F_METHODM(uint32, Prefs_Load, FS_PreferenceGroup_Load)
{
    struct p_LocalObjectData *LOD = F_LOD(Class,Obj);
    struct FeelinDGroup *gnode;
    
    STRPTR name = (STRPTR) F_Do(Msg->Prefs, Msg->id_Resolve, "$decorator-class", NULL);
    uint32 active = 0;

    for (gnode = (struct FeelinDGroup *) LOD->dglist.Head ; gnode ; gnode = gnode->next)
    {
        if (F_StrCmp(name, gnode->classname, ALL) == 0)
        {
            active = gnode->position;
        }
 
        F_OBJDO(gnode->object);
    }

    F_Set(LOD->chooser, (uint32) "FA_Cycle_Active", active);
    
    return F_SUPERDO();
}
//+
///Prefs_Save
F_METHODM(uint32, Prefs_Save, FS_PreferenceGroup_Save)
{
    struct p_LocalObjectData *LOD = F_LOD(Class,Obj);
    struct FeelinDGroup *gnode;

    uint32 active = F_Get(LOD->chooser, (uint32) "FA_Cycle_Active");
    STRPTR name = NULL;

    for (gnode = (struct FeelinDGroup *) LOD->dglist.Head ; gnode ; gnode = gnode->next)
    {
        if (active == gnode->position)
        {
            name = gnode->classname;
        }
 
        F_OBJDO(gnode->object);
    }

    F_Do(Msg->Prefs, Msg->id_AddString, "$decorator-class", name);

    return F_SUPERDO();
}
//+


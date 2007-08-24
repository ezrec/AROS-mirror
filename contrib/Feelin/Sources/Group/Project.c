/*

$VER: 11.00 (2005/08/10)
 
    New Feelin design
   
    FA_Group_Rows and FA_Group_Columns are now initial attributes only [I..]

    The FC_Family object is now created *before* passing the FM_New  method
    to  the  superclass.  Thus,  the  FM_Set  method  won't  be  invoked by
    FC_Object if the  family  failed  to  be  created  (preventing  setting
    attribute  to  disposed  object,  whom  pointers have been saved in the
    LocalObjectData of subclasses).
   
    Metaclass support
   
    Implements  the  new  "damaged"  technique.  FM_RethinkLayout  is   now
    deprecated.
 
$VER: 10.00 (2005/05/04)
 
    FM_RethinkLayout has (again) been totaly rewritten. It  is  faster  and
    uses no extra memory. See FM_Window_RequestRethink.
   
    Improved a lot rendering in complex mode (especialy for pages).
 
$VER: 09.00 (2004/12/18)

    The class has  been  modified  occording  to  the  changes  applied  to
    FC_Family (04.00). The class uses the new features to create and delete
    family's nodes to add further information on its children (espacially a
    pointer  to  FAreaData).  Because  the class uses a FC_Family object to
    manage its children, the members 'Next' and 'Prev' of FC_Area  are  now
    deprecated.

    FM_RethinkLayout behaviour has been completely rewritten.

*/

#include "Project.h"
#include "_locale/Table.h"

struct ClassUserData      *CUD;

///METHODS
F_METHOD_PROTO(void,Group_New);
F_METHOD_PROTO(void,Group_Dispose);
F_METHOD_PROTO(void,Group_Get);
F_METHOD_PROTO(void,Group_Set);
F_METHOD_PROTO(void,Group_AddMember);
F_METHOD_PROTO(void,Group_RemMember);
F_METHOD_PROTO(void,Group_Setup);
F_METHOD_PROTO(void,Group_Cleanup);
F_METHOD_PROTO(void,Group_Show);
F_METHOD_PROTO(void,Group_Hide);
F_METHOD_PROTO(void,Group_AskMinMax);
F_METHOD_PROTO(void,Group_Layout);

#if F_CODE_DEPRECATED
F_METHOD_PROTO(void,Group_RethinkLayout);
#endif
 
F_METHOD_PROTO(void,Group_Draw);
F_METHOD_PROTO(void,Group_GoActive);
F_METHOD_PROTO(void,Group_GoInactive);
F_METHOD_PROTO(void,Group_HandleEvent);
F_METHOD_PROTO(void,Group_BuildContextHelp);
F_METHOD_PROTO(void,Group_TreeUp);
F_METHOD_PROTO(void,Group_Propagate);
F_METHOD_PROTO(void,Group_Forward);
F_METHOD_PROTO(void,Group_InitChange);
F_METHOD_PROTO(void,Group_ExitChange);
F_METHOD_PROTO(void,Prefs_New);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
    CUD = F_LOD(Class,Obj);
    
    CUD -> Hook_HLayout.h_Entry = (HOOKFUNC) F_FUNCTION_GATE(code_layout_horizontal);
    CUD -> Hook_HLayout.h_Data  = Obj;
    CUD -> Hook_VLayout.h_Entry = (HOOKFUNC) F_FUNCTION_GATE(code_layout_vertical);
    CUD -> Hook_VLayout.h_Data  = Obj;
    CUD -> Hook_PLayout.h_Entry = (HOOKFUNC) F_FUNCTION_GATE(code_layout_page);
    CUD -> Hook_PLayout.h_Data  = Obj;
    CUD -> Hook_ALayout.h_Entry = (HOOKFUNC) F_FUNCTION_GATE(code_layout_array);
    CUD -> Hook_ALayout.h_Data  = Obj;

    CUD -> Hook_CreateFamilyNode.h_Entry = (HOOKFUNC) F_FUNCTION_GATE(code_family_create_node);
    CUD -> Hook_DeleteFamilyNode.h_Entry = (HOOKFUNC) F_FUNCTION_GATE(code_family_delete_node);

    if ((CUD -> NodesPool = F_CreatePool(sizeof (FAreaNode),FA_Pool_Name,"Group.NodesPool",TAG_DONE)))
    {
        return (FObject) F_SUPERDO();
    }
    return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
    F_DeletePool(CUD -> NodesPool); CUD -> NodesPool = NULL;
   
    F_SUPERDO();
}
//+

F_QUERY()
{
    switch (Which)
    {
///Meta
        case FV_Query_MetaClassTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Class_New, FM_New),
                F_METHODS_ADD_STATIC(Class_Dispose, FM_Dispose),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Class),
                F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(ActivePage) =
            {
                F_VALUES_ADD("Prev", FV_Group_ActivePage_Prev),
                F_VALUES_ADD("Next", FV_Group_ActivePage_Next),
                F_VALUES_ADD("Last", FV_Group_ActivePage_Last),
                F_VALUES_ADD("First", FV_Group_ActivePage_First),

                F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_STATIC("HSpacing", FV_TYPE_INTEGER, FA_Group_HSpacing),
                F_ATTRIBUTES_ADD_STATIC("VSpacing", FV_TYPE_INTEGER, FA_Group_VSpacing),
                F_ATTRIBUTES_ADD_STATIC("SameWidth", FV_TYPE_BOOLEAN, FA_Group_SameWidth),
                F_ATTRIBUTES_ADD_STATIC("SameHeight", FV_TYPE_BOOLEAN, FA_Group_SameHeight),
                F_ATTRIBUTES_ADD_STATIC("SameSize", FV_TYPE_BOOLEAN, FA_Group_SameSize),
                F_ATTRIBUTES_ADD_STATIC("RelSizing", FV_TYPE_BOOLEAN, FA_Group_RelSizing),
                F_ATTRIBUTES_ADD_STATIC("Forward", FV_TYPE_BOOLEAN, FA_Group_Forward),
                F_ATTRIBUTES_ADD_STATIC("PageMode", FV_TYPE_BOOLEAN, FA_Group_PageMode),
                F_ATTRIBUTES_ADD_STATIC("PageFont", FV_TYPE_STRING, FA_Group_PageFont),
                F_ATTRIBUTES_ADD_STATIC("PageTitle", FV_TYPE_STRING, FA_Group_PageTitle),
                F_ATTRIBUTES_ADD_STATIC("PageAPreParse", FV_TYPE_STRING, FA_Group_PageAPreParse),
                F_ATTRIBUTES_ADD_STATIC("PageIPreParse", FV_TYPE_STRING, FA_Group_PageIPreParse),
                F_ATTRIBUTES_ADD_STATIC_WITH_VALUES("ActivePage", FV_TYPE_INTEGER, FA_Group_ActivePage, ActivePage),
                F_ATTRIBUTES_ADD_STATIC("Rows", FV_TYPE_INTEGER, FA_Group_Rows),
                F_ATTRIBUTES_ADD_STATIC("Columns", FV_TYPE_INTEGER, FA_Group_Columns),
                F_ATTRIBUTES_ADD_STATIC("LayoutHook", FV_TYPE_POINTER, FA_Group_LayoutHook),
                F_ATTRIBUTES_ADD_STATIC("MinMaxHook", FV_TYPE_POINTER, FA_Group_MinMaxHook),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC (Group_New,             FM_New),
                F_METHODS_ADD_STATIC (Group_Dispose,         FM_Dispose),
                F_METHODS_ADD_STATIC (Group_Get,             FM_Get),
                F_METHODS_ADD_STATIC (Group_Set,             FM_Set),
                F_METHODS_ADD_STATIC (Group_AddMember,       FM_AddMember),
                F_METHODS_ADD_STATIC (Group_RemMember,       FM_RemMember),
                F_METHODS_ADD_STATIC (Group_Propagate,       FM_Import),
                F_METHODS_ADD_STATIC (Group_Propagate,       FM_Export),
                F_METHODS_ADD_STATIC (Group_Setup,           FM_Setup),
                F_METHODS_ADD_STATIC (Group_Cleanup,         FM_Cleanup),
                F_METHODS_ADD_STATIC (Group_Show,            FM_Show),
                F_METHODS_ADD_STATIC (Group_Hide,            FM_Hide),

                F_METHODS_ADD_STATIC (Group_AskMinMax,       FM_AskMinMax),
                F_METHODS_ADD_STATIC (Group_Layout,          FM_Layout),
                F_METHODS_ADD_STATIC (Group_Draw,            FM_Draw),
                
                #if F_CODE_DEPRECATED
                F_METHODS_ADD_STATIC (Group_RethinkLayout,   FM_RethinkLayout),
                #endif
                 
                F_METHODS_ADD_STATIC (Group_GoActive,        FM_GoActive),
                F_METHODS_ADD_STATIC (Group_GoInactive,      FM_GoInactive),
                F_METHODS_ADD_STATIC (Group_Propagate,       FM_GoEnabled),
                F_METHODS_ADD_STATIC (Group_Propagate,       FM_GoDisabled),
                F_METHODS_ADD_STATIC (Group_HandleEvent,     FM_HandleEvent),
                F_METHODS_ADD_STATIC (Group_TreeUp,          FM_BuildContextHelp),
                F_METHODS_ADD_STATIC (Group_TreeUp,          FM_BuildContextMenu),
                F_METHODS_ADD_STATIC (Group_TreeUp,          FM_DnDQuery),

                F_METHODS_ADD_BOTH   (Group_Forward,    "Forward",    FM_Group_Forward),
                F_METHODS_ADD_BOTH   (Group_InitChange, "InitChange", FM_Group_InitChange),
                F_METHODS_ADD_BOTH   (Group_ExitChange, "ExitChange", FM_Group_ExitChange),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Area),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_CATALOG,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Prefs
        case FV_Query_PrefsTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Prefs_New, FM_New),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(PreferenceGroup),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
   }
   return NULL;
};
//+

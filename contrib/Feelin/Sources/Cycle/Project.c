/*

$VER: 03.00 (2005/08/13)
        
    [OLAV]
 
    Portability update.
   
    Added FA_Cycle_Level and support for $cycle-level.
    
    Added FA_Cycle_Separator. The caracter defined  by  this  attribute  is
    used    to    separate   strings   set   with   FA_CycleEntries   (with
    FA_Cycle_EntriesTypes  equal   to   FV_Cycle_EntriesType_Strings).   By
    default the separator character is a tabulation '|'.
    
    [GROG]

    FV_Cycle_EntriesType_String is the default entries type.

    $cycle-level has an effect now (LOD modified).
 
$VER: 02.00 (2005/04/12)
 
    Finaly added a very customizable popup menu.

    FA_Cycle_EntriesType
 
        Instead of an array of string pointers, entries can now be  created
        from   a  single  string  made  of  several  entries  separated  by
        tabulations (\t).
 
*/

#include "Project.h"

struct Hook                         Hook_MinMax;

#ifdef __MORPHOS__
struct IntuitionBase               *IntuitionBase;
#endif

///METHODS
F_METHOD_PROTO(void,Cycle_New);
F_METHOD_PROTO(void,Cycle_Dispose);
F_METHOD_PROTO(void,Cycle_Get);
F_METHOD_PROTO(void,Cycle_Set);
F_METHOD_PROTO(void,Cycle_Setup);
F_METHOD_PROTO(void,Cycle_Cleanup);
F_METHOD_PROTO(void,Cycle_GoActive);
F_METHOD_PROTO(void,Cycle_GoInactive);
F_METHOD_PROTO(void,Cycle_HandleEvent);
F_METHOD_PROTO(void,Prefs_New);

F_HOOK_PROTO(void,code_minmax);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
    Hook_MinMax.h_Entry = (HOOKFUNC) code_minmax;
    Hook_MinMax.h_Data  = Obj;

    #ifdef __MORPHOS__
    IntuitionBase = FeelinBase->Intuition;
    #endif
  
    return (FObject) F_SUPERDO();
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

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Class),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(Active) =
            {
                F_VALUES_ADD("Next",FV_Cycle_Active_Next),
                F_VALUES_ADD("Prev",FV_Cycle_Active_Prev),
                F_VALUES_ADD("Last",FV_Cycle_Active_Last),
                F_VALUES_ADD("First",FV_Cycle_Active_First),

                F_ARRAY_END
            };
       
            STATIC F_VALUES_ARRAY(EntriesType) =
            {
                F_VALUES_ADD("Array", FV_Cycle_EntriesType_Array),
                F_VALUES_ADD("String", FV_Cycle_EntriesType_String),

                F_ARRAY_END
            };
               
            STATIC F_VALUES_ARRAY(Position) =
            {
                F_VALUES_ADD("Active",0),
                F_VALUES_ADD("Below",1),

                F_ARRAY_END
            };
               
            STATIC F_VALUES_ARRAY(Layout) =
            {
                F_VALUES_ADD("Right",FV_Cycle_Layout_Right),
                F_VALUES_ADD("Left",FV_Cycle_Layout_Left),

                F_ARRAY_END
            };
       
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_WITH_VALUES("Active", FV_TYPE_INTEGER,Active),
                F_ATTRIBUTES_ADD("Entries", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD_WITH_VALUES("EntriesType", FV_TYPE_POINTER,EntriesType),
                F_ATTRIBUTES_ADD("PreParse", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD_WITH_VALUES("Position", FV_TYPE_INTEGER,Position),
                F_ATTRIBUTES_ADD_WITH_VALUES("Layout", FV_TYPE_INTEGER,Layout),
                F_ATTRIBUTES_ADD("Level", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Separator", FV_TYPE_INTEGER),

                F_ARRAY_END
            };
                              
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Cycle_New,           FM_New),
                F_METHODS_ADD_STATIC(Cycle_Dispose,       FM_Dispose),
                F_METHODS_ADD_STATIC(Cycle_Get,           FM_Get),
                F_METHODS_ADD_STATIC(Cycle_Set,           FM_Set),
                F_METHODS_ADD_STATIC(Cycle_Setup,         FM_Setup),
                F_METHODS_ADD_STATIC(Cycle_Cleanup,       FM_Cleanup),
                F_METHODS_ADD_STATIC(Cycle_GoActive,      FM_GoActive),
                F_METHODS_ADD_STATIC(Cycle_GoInactive,    FM_GoInactive),
                F_METHODS_ADD_STATIC(Cycle_HandleEvent,   FM_HandleEvent),

                F_ARRAY_END
            };
                           
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Group),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,

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
                F_METHODS_ADD_STATIC(Prefs_New,FM_New),

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
}


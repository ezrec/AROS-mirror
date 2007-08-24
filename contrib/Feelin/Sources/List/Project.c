/*

$VER: 03.02 (2005/08/10)
 
    Portability update

$VER: 03.00 (2005/05/10)
 
    Rendering greatly improved.
    
    The new attribute FA_List_Activation can be get to  know  how  the  last
    item  has  been  activated  :  mouse-click,  mouse-double-click,  key or
    external.
    
    Added frames around title cells

    Handles FF_EVENT_MOUSE_WHEEL
    
    FA_List_AdjustWidth and FA_List_AdjustHeight are useless since FA_SetMin
    does the same.
    
    The hook called by the FM_List_Display method  should  no  longer  fills
    line's  strings and preparses array because it's too stupidly dangerous.
    Instead it shoudl return an array of FListDisplay previously filled with
    its   strings   and   preparses.   The  array  is  then  copied  by  the
    FM_List_Display method and avoids memory overflows.

    *** BUGS REMOVED ***
    
    Forgot to set FA_TextDisplay_Width and FA_TextDisplay_Height to  -1  (no
    limits)  before  computing  line  dimensions.  Dimensions where computed
    within the bounds of the last drawing.
    
    A mouse selection below the last entry was selecting the entry.
    
    The colomns format is not defined with an XML string :
    
        <col fixed='boolean' padding='integer' span='integer'/>

    >> list_adjust_first() seams buggy, but I'm not sure :-)
    
$VER: 02.00 (2004/12/18)

    The class has been mostly rewritten.


    Bon, on reprend tout depuis le début.

    Supprimer la méthode "Clear"  et  la  remplace  par  "Remove"  avec  des
    valeurs  spéciales  telles  que "All" (pour tout effacer), "Active" pour
    effacer la ligne active, "FromTop" (efface de la  première  ligne  à  la
    ligne  active),  "ToBottom"  (de  la ligne active à la dernière), "Next"
    (efface la ligne suivante), "Prev" (efface la ligne précédente), "First"
    and "Last", "Others" (pour effacer toutes les autres, sauf l'active).

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,List_New);
F_METHOD_PROTO(void,List_Dispose);
F_METHOD_PROTO(void,List_Set);
F_METHOD_PROTO(void,List_Get);
F_METHOD_PROTO(void,List_Setup);
F_METHOD_PROTO(void,List_Cleanup);
F_METHOD_PROTO(void,List_AskMinMax);
F_METHOD_PROTO(void,List_Layout);
F_METHOD_PROTO(void,List_Draw);
F_METHOD_PROTO(void,List_HandleEvent);
F_METHOD_PROTO(void,List_GoActive);
F_METHOD_PROTO(void,List_GoInactive);

F_METHOD_PROTO(void,List_Construct);
F_METHOD_PROTO(void,List_Destruct);
F_METHOD_PROTO(void,List_Compare);
F_METHOD_PROTO(void,List_Display);
F_METHOD_PROTO(void,List_GetEntry);
F_METHOD_PROTO(void,List_Insert);
F_METHOD_PROTO(void,List_InsertSingle);
F_METHOD_PROTO(void,List_Remove);
F_METHOD_PROTO(void,List_FindString);

F_METHOD_PROTO(void,Prefs_New);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(Active) =
            {
                F_VALUES_ADD("PageDown", FV_List_Active_PageDown),
                F_VALUES_ADD("PageUp", FV_List_Active_PageUp),
                F_VALUES_ADD("Down", FV_List_Active_Down),
                F_VALUES_ADD("Up", FV_List_Active_Up),
                F_VALUES_ADD("Bottom", FV_List_Active_Bottom),
                F_VALUES_ADD("Top", FV_List_Active_Top),
                F_VALUES_ADD("None", FV_List_Active_None),
                
                F_ARRAY_END
            };
 
            STATIC F_VALUES_ARRAY(Activation) =
            {
                F_VALUES_ADD("Click", FV_List_Activation_Click),
                F_VALUES_ADD("DoubleClick", FV_List_Activation_DoubleClick),
                F_VALUES_ADD("Key", FV_List_Activation_Key),
                F_VALUES_ADD("External", FV_List_Activation_External),

                F_ARRAY_END
            };
            
            STATIC F_VALUES_ARRAY(String) =
            {
                F_VALUES_ADD("String", FV_List_Hook_String),

                F_ARRAY_END
            };
             
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_WITH_VALUES("Active", FV_TYPE_INTEGER, Active),
                F_ATTRIBUTES_ADD_WITH_VALUES("Activation", FV_TYPE_INTEGER, Activation),
                F_ATTRIBUTES_ADD_WITH_VALUES("CompareHook", FV_TYPE_POINTER, String),
                F_ATTRIBUTES_ADD_WITH_VALUES("ConstructHook", FV_TYPE_POINTER, String),
                F_ATTRIBUTES_ADD_WITH_VALUES("DestructHook", FV_TYPE_POINTER, String),
                F_ATTRIBUTES_ADD_WITH_VALUES("DisplayHook", FV_TYPE_POINTER, String),
                F_ATTRIBUTES_ADD("Entries", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("Visible", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("First", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Last", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Format", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Pool", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("PoolItemNumber", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("PoolItemSize", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Quiet", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("SortMode", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("SourceArray", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("Title", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("ReadOnly", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("Steps", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Spacing", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("CursorActive", FV_TYPE_STRING),
            
                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(List_Compare,       "Compare"),
                F_METHODS_ADD(List_Construct,     "Construct"),
                F_METHODS_ADD(List_Destruct,      "Destruct"),
                F_METHODS_ADD(List_Display,       "Display"),
                F_METHODS_ADD(List_GetEntry,      "GetEntry"),
                F_METHODS_ADD(List_Insert,        "Insert"),
                F_METHODS_ADD(List_InsertSingle,  "InsertSingle"),
                F_METHODS_ADD(List_Remove,        "Remove"),
                F_METHODS_ADD(List_FindString,    "FindString"),
                
                F_METHODS_ADD_STATIC(List_New,         FM_New),
                F_METHODS_ADD_STATIC(List_Dispose,     FM_Dispose),
                F_METHODS_ADD_STATIC(List_Get,         FM_Get),
                F_METHODS_ADD_STATIC(List_Set,         FM_Set),

                F_METHODS_ADD_STATIC(List_Setup,       FM_Setup),
                F_METHODS_ADD_STATIC(List_Cleanup,     FM_Cleanup),
                F_METHODS_ADD_STATIC(List_AskMinMax,   FM_AskMinMax),
                F_METHODS_ADD_STATIC(List_Layout,      FM_Layout),
                F_METHODS_ADD_STATIC(List_Draw,        FM_Draw),
                F_METHODS_ADD_STATIC(List_HandleEvent, FM_HandleEvent),
                F_METHODS_ADD_STATIC(List_GoActive,    FM_GoActive),
                F_METHODS_ADD_STATIC(List_GoInactive,  FM_GoInactive),

                F_ARRAY_END
            };
                        
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Area),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                
                F_ARRAY_END
            };

            return F_TAGS;
        }

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
}


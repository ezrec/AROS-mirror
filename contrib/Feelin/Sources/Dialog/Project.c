/*

$VER: 01.02 (2005/08/10)
 
    Portability update
 
$VER: 01.00 (2005/05/05)
 
    Base class for dialog window

*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,Dialog_New);
F_METHOD_PROTO(void,Dialog_Open);
F_METHOD_PROTO(void,Dialog_Close);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(Buttons) =
            {
                F_VALUES_ADD("None",FV_Dialog_Buttons_None),
                F_VALUES_ADD("Ok",FV_Dialog_Buttons_Ok),
                F_VALUES_ADD("Boolean",FV_Dialog_Buttons_Boolean),
                F_VALUES_ADD("Confirm",FV_Dialog_Buttons_Confirm),
                F_VALUES_ADD("Always",FV_Dialog_Buttons_Always),
                F_VALUES_ADD("Preference",FV_Dialog_Buttons_Preference),
                F_VALUES_ADD("PreferenceTest",FV_Dialog_Buttons_PreferenceTest),

                F_ARRAY_END
            };

            STATIC F_VALUES_ARRAY(Response) =
            {
                F_VALUES_ADD("None",FV_Dialog_Response_None),
                F_VALUES_ADD("Ok",FV_Dialog_Response_Ok),
                F_VALUES_ADD("Cancel",FV_Dialog_Response_Cancel),
                F_VALUES_ADD("Apply",FV_Dialog_Response_Apply),
                F_VALUES_ADD("Yes",FV_Dialog_Response_Yes),
                F_VALUES_ADD("No",FV_Dialog_Response_No),
                F_VALUES_ADD("All",FV_Dialog_Response_All),
                F_VALUES_ADD("Save",FV_Dialog_Response_Save),
                F_VALUES_ADD("Use",FV_Dialog_Response_Use),
                
                F_ARRAY_END
            };
            
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_WITH_VALUES("Buttons", FV_TYPE_INTEGER,Buttons),
                F_ATTRIBUTES_ADD("Separator", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD_WITH_VALUES("Response", FV_TYPE_INTEGER,Response),
                F_ATTRIBUTES_ADD("ApplyText", FV_TYPE_STRING),
                
                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Dialog_New, FM_New),
                F_METHODS_ADD_STATIC(Dialog_Open, FM_Window_Open),
                F_METHODS_ADD_STATIC(Dialog_Close, FM_Window_Close),
            
                F_ARRAY_END
            };
            
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Window),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_CATALOG,
                
                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}

/*

$VER: 04.00 (2005/08/21)
 
    Portability update.
    
    If FA_Numeric_Format is NULL, the  FM_Numeric_Stringify  method  is  not
    invoked.
    
    Added FA_Numeric_StringArray.
 
$VER: 03.00 (2004/12/06)
 
    New step handling. FA_Numeric_Steps defined the number of steps from min
    to max. FA_Numeric_Step is the width of on step.
    
    The event class code FV_EVENT_BUTTON_WHEEL is now handled by the  class.
    You  can  now  use  your  mouse's  wheel to increase or decrease numeric
    values (FA_Numeric_Steps is used to compute the steps).

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Numeric_New);
F_METHOD_PROTO(void,Numeric_Dispose);
F_METHOD_PROTO(void,Numeric_Set);
F_METHOD_PROTO(void,Numeric_Get);
F_METHOD_PROTO(void,Numeric_Export);
F_METHOD_PROTO(void,Numeric_Import);
F_METHOD_PROTO(void,Numeric_HandleEvent);
F_METHOD_PROTO(void,Numeric_Increase);
F_METHOD_PROTO(void,Numeric_Decrease);
F_METHOD_PROTO(void,Numeric_Stringify);
F_METHOD_PROTO(void,Numeric_Reset);
//+

F_QUERY()
{
    switch (Which)
    {
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Default",     FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Value",       FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Min",         FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Max",         FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Step",        FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Steps",       FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Format",      FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Buffer",      FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("ComputeHook", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("StringArray", FV_TYPE_POINTER),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(Numeric_Increase,    "Increase"),
                F_METHODS_ADD(Numeric_Decrease,    "Decrease"),
                F_METHODS_ADD(Numeric_Stringify,   "Stringify"),
                F_METHODS_ADD(Numeric_Reset,       "Reset"),
                
                F_METHODS_ADD_STATIC(Numeric_New,         FM_New),
                F_METHODS_ADD_STATIC(Numeric_Dispose,     FM_Dispose),
                F_METHODS_ADD_STATIC(Numeric_Set,         FM_Set),
                F_METHODS_ADD_STATIC(Numeric_Get,         FM_Get),
                F_METHODS_ADD_STATIC(Numeric_Export,      FM_Export),
                F_METHODS_ADD_STATIC(Numeric_Import,      FM_Import),
                F_METHODS_ADD_STATIC(Numeric_HandleEvent, FM_HandleEvent),
                
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
    }
    return NULL;
}

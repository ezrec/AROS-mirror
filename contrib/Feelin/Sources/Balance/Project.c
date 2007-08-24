/*

$VER: 03.00 (2005/08/14)
 
    Portability update
    
    Use the new "damaged" rendering technique.
 
$VER: 02.02 (2005/05/19)
 
    The method FM_RethinkLayout is used instead of custom  functions,  which
    results in better update and above all neat code :-)
 
$VER: 02.00 (2004/09/12)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Balance_New);
F_METHOD_PROTO(void,Balance_Set);
F_METHOD_PROTO(void,Balance_Import);
F_METHOD_PROTO(void,Balance_Export);
F_METHOD_PROTO(void,Balance_Setup);
F_METHOD_PROTO(void,Balance_Cleanup);
F_METHOD_PROTO(void,Balance_GoActive);
F_METHOD_PROTO(void,Balance_GoInactive);
F_METHOD_PROTO(void,Balance_AskMinMax);
F_METHOD_PROTO(void,Balance_Draw);
F_METHOD_PROTO(void,Balance_HandleEvent);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("QuickDraw", FV_TYPE_BOOLEAN),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Balance_New,         FM_New),
                F_METHODS_ADD_STATIC(Balance_Set,         FM_Set),
                F_METHODS_ADD_STATIC(Balance_Import,      FM_Import),
                F_METHODS_ADD_STATIC(Balance_Export,      FM_Export),
                F_METHODS_ADD_STATIC(Balance_Setup,       FM_Setup),
                F_METHODS_ADD_STATIC(Balance_Cleanup,     FM_Cleanup),
                F_METHODS_ADD_STATIC(Balance_GoActive,    FM_GoActive),
                F_METHODS_ADD_STATIC(Balance_GoInactive,  FM_GoInactive),
                F_METHODS_ADD_STATIC(Balance_AskMinMax,   FM_AskMinMax),
                F_METHODS_ADD_STATIC(Balance_Draw,        FM_Draw),
                F_METHODS_ADD_STATIC(Balance_HandleEvent, FM_HandleEvent),

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
//+
    }
    return NULL;
}

/*

$VER: 03.00 (2005/08/10)
 
    Portability update.
   
    FM_Preview_Query support.
    
    Scheme was not updated correctly when cleared.
 
$VER: 02.00 (2005/04/04)
 
    Class rewritten for the new FC_Preview.
 
$VER: 01.00 (2004/01/11)

    Used to give a preview of a color scheme.  This  color  scheme  can  be
    modified as wished at any time.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Preview_New);
F_METHOD_PROTO(void,Preview_Setup);
F_METHOD_PROTO(void,Preview_Cleanup);
F_METHOD_PROTO(void,Preview_Draw);

F_METHOD_PROTO(void,Preview_Query);
F_METHOD_PROTO(void,Preview_ToString);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(Preview_Query,           "FM_Preview_Query"),
                F_METHODS_ADD(Preview_ToString,        "FM_Preview_ToString"),

                F_METHODS_ADD_STATIC(Preview_New,       FM_New),
                F_METHODS_ADD_STATIC(Preview_Setup,     FM_Setup),
                F_METHODS_ADD_STATIC(Preview_Cleanup,   FM_Cleanup),
                F_METHODS_ADD_STATIC(Preview_Draw,      FM_Draw),

                F_ARRAY_END
            };

            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FA_Preview_Spec"),
               
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Preview),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_RESOLVES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}


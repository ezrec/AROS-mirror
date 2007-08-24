/*

$VER: 01.02 (2005/08/10)
 
    Portability update

$VER: 01.00 (2004/01/19)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Scale_New);
F_METHOD_PROTO(void,Scale_Setup);
F_METHOD_PROTO(void,Scale_AskMinMax);
F_METHOD_PROTO(void,Scale_Draw);
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
                F_METHODS_ADD_STATIC(Scale_New,        FM_New),
                F_METHODS_ADD_STATIC(Scale_Setup,      FM_Setup),
                F_METHODS_ADD_STATIC(Scale_AskMinMax,  FM_AskMinMax),
                F_METHODS_ADD_STATIC(Scale_Draw,       FM_Draw),

               F_ARRAY_END
            };
             
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Area),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,

               F_ARRAY_END
            };
 
            return F_TAGS;
        }
//+
    }
    return NULL;
}


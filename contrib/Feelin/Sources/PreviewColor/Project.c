/*

$VER: 01.00 (2005/08/10)
 
*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Preview_Query);
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
                F_METHODS_ADD(Preview_Query, "FM_Preview_Query"),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(PreviewImage),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}


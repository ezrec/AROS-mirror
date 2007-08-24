/*

$VER: 03.00 (2005/08/10)
 
    Portability update.
   
    Uses new specifications.
   
    The FA_ImageDisplay_Origin is set to the object's box.
 
$VER: 02.00 (2005/03/25)
 
    Subclass of FC_Preview dedicated to the preview of images.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Preview_New);
F_METHOD_PROTO(void,Preview_Dispose);
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
                F_METHODS_ADD(Preview_Query, "FM_Preview_Query"),
                F_METHODS_ADD(Preview_ToString, "FM_Preview_ToString"),

                F_METHODS_ADD_STATIC(Preview_New, FM_New),
                F_METHODS_ADD_STATIC(Preview_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(Preview_Setup, FM_Setup),
                F_METHODS_ADD_STATIC(Preview_Cleanup, FM_Cleanup),
                F_METHODS_ADD_STATIC(Preview_Draw, FM_Draw),

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


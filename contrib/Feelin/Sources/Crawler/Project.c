/*

$VER: 02.10 (2005/08/10)
 
    Portability update. Remove semaphoe object, since from v10 every  object
    have semaphore facilities.
 
$VER: 02.00 (2003/09/16)

    Ported from E to C. The class is now using Dynamic IDs.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Crawler_New);
F_METHOD_PROTO(void,Crawler_Set);
F_METHOD_PROTO(void,Crawler_Setup);
F_METHOD_PROTO(void,Crawler_Cleanup);
F_METHOD_PROTO(void,Crawler_Show);
F_METHOD_PROTO(void,Crawler_Hide);
F_METHOD_PROTO(void,Crawler_AskMinMax);
F_METHOD_PROTO(void,Crawler_Layout);
F_METHOD_PROTO(void,Crawler_Draw);
F_METHOD_PROTO(void,Crawler_Crawl);
F_METHOD_PROTO(void,Crawler_HandleEvent);
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
                F_ATTRIBUTES_ADD("Delay", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Micros", FV_TYPE_INTEGER),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD        (Crawler_Crawl,      "Crawl"),
                F_METHODS_ADD_STATIC (Crawler_New,         FM_New),
                F_METHODS_ADD_STATIC (Crawler_Set,         FM_Set),
                F_METHODS_ADD_STATIC (Crawler_Setup,       FM_Setup),
                F_METHODS_ADD_STATIC (Crawler_Cleanup,     FM_Cleanup),
                F_METHODS_ADD_STATIC (Crawler_Show,        FM_Show),
                F_METHODS_ADD_STATIC (Crawler_Hide,        FM_Hide),
                F_METHODS_ADD_STATIC (Crawler_AskMinMax,   FM_AskMinMax),
                F_METHODS_ADD_STATIC (Crawler_Layout,      FM_Layout),
                F_METHODS_ADD_STATIC (Crawler_Draw,        FM_Draw),
                F_METHODS_ADD_STATIC (Crawler_HandleEvent, FM_HandleEvent),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Group),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}
//+

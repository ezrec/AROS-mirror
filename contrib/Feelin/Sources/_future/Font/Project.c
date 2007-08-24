/*

$VER: 01.00 (2005/07/22)

    FA_Font_Spec

*/

#include "Private.h"

struct FeelinBase                  *FeelinBase;
struct Library                     *TTEngineBase;

///METHODS
F_METHOD_PROTO(void,Font_New);
F_METHOD_PROTO(void,Font_Dispose);
//+

/*   You   can   (and   should)   remove   Class_New,   Class_Dispose   and
FV_Query_MetaTags if you don't need to create a metaclass for your class */

///Class_New
F_METHOD_NEW(Class_New)
{
    TTEngineBase = OpenLibrary(TTENGINENAME,TTENGINEVERSION);

    return (FObject) F_SUPERDO();
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
    if (TTEngineBase)
    {
        CloseLibrary(TTEngineBase); TTEngineBase = NULL;
    }

    F_SUPERDO();
}
//+
 
F_QUERY()
{
    F_SAVE_BASE;
 
    switch (Which)
    {
///Meta
        case FV_Query_MetaTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Class_New,      FM_New),
                F_METHODS_ADD_STATIC(Class_Dispose,  FM_Dispose),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Class),
//                F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Spec", FV_STRING),
                
                F_ARRAY_END
            };
                
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Font_New, FM_New),
                F_METHODS_ADD_STATIC(Font_Dispose, FM_Dispose),

                F_ARRAY_END
            };
            
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(<name>),
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

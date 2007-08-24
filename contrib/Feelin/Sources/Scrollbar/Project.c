/*

$VER: 01.10 (2005/08/10)
 
    Portability update
 
$VER: 01.02 (2005/04/27)
 
    The new attribute FA_Prop_Useless is now used to hide the object when it
    is useless.
 
$VER: 01.00 (2004/12/18)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,SB_New);
F_METHOD_PROTO(void,SB_Dispose);
F_METHOD_PROTO(void,SB_Set);
F_METHOD_PROTO(void,SB_Get);
F_METHOD_PROTO(void,SB_Setup);
F_METHOD_PROTO(void,SB_Cleanup);

F_METHOD_PROTO(void,Prefs_New);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(Type) =
            {
                F_VALUES_ADD("Bottom",   FV_Scrollbar_Type_Bottom),
                F_VALUES_ADD("Sym",      FV_Scrollbar_Type_Sym),
                F_VALUES_ADD("Top",      FV_Scrollbar_Type_Top),

                F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_WITH_VALUES("Type", FV_TYPE_INTEGER, Type),
                F_ATTRIBUTES_ADD("Entries",  FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("First",    FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Visible",  FV_TYPE_INTEGER),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(SB_New,       FM_New),
                F_METHODS_ADD_STATIC(SB_Dispose,   FM_Dispose),
                F_METHODS_ADD_STATIC(SB_Set,       FM_Set),
                F_METHODS_ADD_STATIC(SB_Get,       FM_Get),
                F_METHODS_ADD_STATIC(SB_Setup,     FM_Setup),
                F_METHODS_ADD_STATIC(SB_Cleanup,   FM_Cleanup),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_SUPER(Group),
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Prefs
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
                F_TAGS_ADD(LODSize, sizeof (struct p_LocalObjectData)),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}

/*

$VER: 01.04 (2005/08/10)
 
    Portability update
 
$VER: 01.02 (2005/04/06)
 
    Added     $decorator-gadget-width,     $decorator-gadget-height      and
    $decorator-border
 
$VER: 01.00 (2003/08/19)

    Master class of all window decorators.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void, Prefs_New);
F_METHOD_PROTO(void, Prefs_Dispose);

F_METHOD_PROTO(void, Prefs_Load);
F_METHOD_PROTO(void, Prefs_Save);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Group),

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
                F_METHODS_ADD(Prefs_Load, "FM_PreferenceGroup_Load"),
                F_METHODS_ADD(Prefs_Save, "FM_PreferenceGroup_Save"),

                F_METHODS_ADD_STATIC(Prefs_New,  FM_New),
                F_METHODS_ADD_STATIC(Prefs_Dispose,  FM_Dispose),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(PreferenceGroup),
                F_TAGS_ADD(LODSize,sizeof (struct p_LocalObjectData)),
                F_TAGS_ADD_METHODS,
                
                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}


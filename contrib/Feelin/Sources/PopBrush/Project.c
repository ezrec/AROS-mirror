/*

$VER: 01.02 (2005/08/10)

    Portability update
 
$VER: 01.00 (2005/04/30)
 
*/

#include "Project.h"

#include <proto/feelin.h>
#include <libraries/feelin.h>

#include "_locale/enums.h"
#include "_locale/table.h"

enum    {
    
        FA_Preview_Spec

        };
 
///PopBrush_New
F_METHOD(FObject,PopBrush_New)
{
    if (F_SuperDo(Class,Obj,Method,

        FA_Frame,        "$popstring-frame",
        FA_Back,         "$popstring-back",
        FA_ColorScheme,  "$popstring-scheme",
        FA_InputMode,     FV_InputMode_Release,
        FA_ContextHelp,   F_CAT(HELP),

        TAG_MORE,Msg))
    {
        F_Do(Obj,FM_Notify,FA_Pressed,FALSE,Obj,"FM_Preview_Adjust",2,"AdjustBrush",F_CAT(TITLE));

        return Obj;
    }
    return NULL;
}
//+
///PopBrush_DnDQuery
F_METHODM(FObject,PopBrush_DnDQuery,FS_DnDQuery)
{
    if (F_SUPERDO())
    {
        STRPTR spec = (STRPTR) F_Get(Msg -> Source,F_IDR(FA_Preview_Spec));

        if (spec)
        {
            if (*spec++ == 'B' && *spec ==':')
            {
                return Obj;
            }
        }
    }
    return NULL;
}
//+

F_QUERY()
{
    switch (Which)
    {
        case FV_Query_ClassTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(PopBrush_New,       FM_New),
                F_METHODS_ADD_STATIC(PopBrush_DnDQuery,  FM_DnDQuery),

                F_ARRAY_END
            };
            
            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FA_Preview_Spec"),
                
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(PreviewImage),
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_RESOLVES,
                F_TAGS_ADD_CATALOG,
                
                F_ARRAY_END
            };

            return F_TAGS;
        }

    }
    return NULL;
}

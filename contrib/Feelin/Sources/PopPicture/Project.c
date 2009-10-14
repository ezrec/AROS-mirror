/*

$VER: 02.00 (2005/08/10)
 
*/

#include <libraries/feelin.h>
#include <proto/feelin.h>

#include "Project.h"

#include "_locale/enums.h"
#include "_locale/table.h"

enum  {
   
      FA_Preview_Spec

      };
 
///PopPicture_New
F_METHOD(FObject,PopPicture_New)
{
    if (F_SuperDo(Class,Obj,Method,

       FA_Frame,        "$popbutton-frame",
       FA_Back,         "$popbutton-back",
       FA_ColorScheme,  "$popbutton-scheme",
       FA_InputMode,     FV_InputMode_Release,
       FA_ContextHelp,   F_CAT(HELP),

       TAG_MORE,Msg))
    {
        F_Do(Obj,FM_Notify,FA_Pressed,FALSE,Obj,"FM_Preview_Adjust",2,F_CAT(TITLE),"AdjustPicture",TAG_DONE);

        return Obj;
    }
    return NULL;
}
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
                F_METHODS_ADD_STATIC(PopPicture_New, FM_New),

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
//+
    }
    return NULL;
}

/*

$VER: 02.02 (2005/08/10)
 
    Portability update, removed a lot of unecessary stuff.
    
$VER: 02.00 (2005/04/04)
 
    Class rewritten according to the new FC_Preview class.

$VER: 01.00 (2004/01/11)

    La class est une sous-classe de FC_PreviewFrame. Elle ajoute à la classe
    la  gestion  du  drag'n'drop  et  la possibilité de modifier son contenu
    grâce à un objet FC_AdjustFrame.

*/

#include <proto/feelin.h>

#include <libraries/feelin.h>

#include "Project.h"
#include "_locale/enums.h"
#include "_locale/table.h"

///PopFrame_New
F_METHOD(FObject,PopFrame_New)
{
    if (F_SuperDo(Class,Obj,Method,

        FA_Frame,        "$popbutton-frame",
        FA_Back,         "$popbutton-back",
        FA_ColorScheme,  "$popbutton-scheme",
        FA_InputMode,     FV_InputMode_Release,
        FA_ContextHelp,   F_CAT(HELP),

    TAG_MORE,Msg))
    {
        F_Do(Obj,FM_Notify,FA_Pressed,FALSE,Obj,"FM_Preview_Adjust",2,F_CAT(TITLE),"AdjustFrame",TAG_DONE);

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
                F_METHODS_ADD_STATIC(PopFrame_New, FM_New),

                F_ARRAY_END
            };
            
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(PreviewFrame),
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_CATALOG,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}
//+

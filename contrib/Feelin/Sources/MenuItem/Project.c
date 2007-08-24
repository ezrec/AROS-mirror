/*

$VER: 01.00 (2005/08/01)

    FA_MenuItem_Icon
    
        FC_ImageDisplay  spec,   should   be   a   preference   item   e.g.
        $menu-icon-open.
 
    FA_MenuItem_Label

    FA_MenuItem_APParse
    
        $menuitem-preparse-active
    
    FA_MenuItem_IPParse
    
        $menuitem-preparse-inactive
    
    FA_MenuItem_Accel
    
        <ctrl><alt><lalt><ralt><shift><lshift><rshift><command>
        
    FA_MenuItem_Selected
    
        TRUE when a menu item is clicked
    
    FA_MenuItem_Checked
    
*/

#include "Private.h"

///METHODS
F_METHOD(void,<function>);

F_METHOD(void,Prefs_New);
F_METHOD(void,Prefs_Load);
F_METHOD(void,Prefs_Save);
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
                F_ATTRIBUTES_ADD("Icon", FV_STRING),
                F_ATTRIBUTES_ADD("Label", FV_STRING),
                F_ATTRIBUTES_ADD_BOTH("Selected", FV_STRING, FA_Selected),
                
                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(MI_New, FM_New),
                F_METHODS_ADD_STATIC(MI_Dispose, FM_Dispose),
            };
            

 
            return F_TAGS;
        }
//+
    }
    return NULL;
}

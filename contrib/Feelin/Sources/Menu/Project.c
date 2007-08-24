/*

$VER: 01.00 (2005/07/31)

FA_Application_Menu
FA_Application_MenuSelected
            
MenuShell
    MenuItem
        Menu
            RootMenu
            MenuBar

 
MenuItem

struct FMenuItemPublic
{
    FObject                         icon;
    STRPTR                          label;
    bits8                           flags;
    uint8                           accel_key;
    bits16                          accel_flags;
};
 
*/

#include "Private.h"

///METHODS
F_METHOD(void, Menu_New);
F_METHOD(void, Menu_Dispose);
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
                F_METHODS_ADD_STATIC(Menu_New, FM_New),
                F_METHODS_ADD_STATIC(Menu_Dispose, FM_Dispose),
                
                F_ARRAY_END
            };
            
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(MenuItem),
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

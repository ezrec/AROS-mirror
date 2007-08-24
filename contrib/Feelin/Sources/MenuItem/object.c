#include "Private.h"

struct FeelinMenuRender
{
    FObject                         SharedTD;   // shared FC_TextDisplay object
    FPalette                       *APalette;
    FPalette                       *IPalette;

    FObject                         icons_back;
    FObject                         item_frame;
 
    uint8                           padding-icon;
    uint8                           padding-accel;
    uint8                           padding-horizontal;
};

/* EVENTS:

    $menu-qualifier: qualifier for menu shortcuts. On amiga this should  be
    <lcommand>.  The  developer  can  add <ctrl>, <alt> and <shift>. Events
    flags are built from these.
 
*/

struct LocalObjectData
{
    FObject                         icon;
    STRPTR                          icon_spec;
    STRPTR                          label;
    STRPTR                          accel;
};

#undef _render
#define _render                                 LOD->Render

///MenuItem_New
F_METHOD(uint32, MenuItem_New)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = Msg, item;
    
    while  F_
}
//+
///MenuItem_Set
F_METHOD(void, MenuItem_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = Msg, item;

    while  (F_DynamicNTI(&Tags, &item, Class))
    switch (item.ti_Tag)
    {
        case FA_MenuItem_Icon:
        {
            LOD->icon_spec = (STRPTR) item.ti_Data;
            F_Set(LOD->icon, FA_ImageDisplay_Spec, item.ti_Data);
        }
        break;
    
        case FA_MenuItem_Label:
        {
            F_Dispose(LOD->label);
            LOD->label = F_StrNew(NULL, "%s", item.ti_Data);
        }
        break;
    
        case FA_MenuItem_Accel:
        {
            F_Dispose(LOD->accel);
            LOD->accel = F_StrNew(NULL, "%s", item.ti_Data);
        }
        break;
    }
    
    F_SUPERDO();
}
//+


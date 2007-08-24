/*** Includes **************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <graphics/text.h>
#include <libraries/feelin.h>
#include <feelin/preference.h>

#include "_locale/Enums.h"

/*** Globals ***************************************************************/

#define DOSBase                     FeelinBase->DOS
#define       GfxBase               FeelinBase -> Graphics
#define       LayersBase            FeelinBase -> Layers
#define       IntuitionBase         FeelinBase -> Intuition
#define       UtilityBase           FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/*** Structures ************************************************************/

struct ClassUserData
{
    APTR                            NodesPool;
    struct Hook                     Hook_CreateFamilyNode;
    struct Hook                     Hook_DeleteFamilyNode;
    struct Hook                     Hook_HLayout;
    struct Hook                     Hook_VLayout;
    struct Hook                     Hook_PLayout;
    struct Hook                     Hook_ALayout;
};

struct Group_PageData
{
    FObject                         TD;
    struct TextFont                *Font;
    STRPTR                          IPrep;
    STRPTR                          APrep;
    FAreaNode                      *ActiveNode;
    FAreaNode                      *UpdateNode;         // Previous active page
//  Preference
    STRPTR                          p_Font;
    STRPTR                          p_IPrep;
    STRPTR                          p_APrep;
};
/*
struct Group_VirtualData
{
    LONG                            xOff;           // virt_offx;
    LONG                            yOff;           // virt_offy;       diplay offsets
    LONG                            xOld;           // old_virt_offx;
    LONG                            yOld;           // old_virt_offy;   Saved virtual positions, used for update == 2
    LONG                            MinW;           // virt_mwidth;
    LONG                            MinH;           // virt_mheight;    The complete width
    APTR                            HProp;
    APTR                            VProp;          // used to scroll virtual group
};
*/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                         Family;

    bits16                          Flags;
    uint8                           HSpace;
    uint8                           VSpace;
    STRPTR                          Name;               // Used for page mode instead of FA_Group_PageTitles

    struct Hook                    *LayoutHook;
    struct Hook                    *MinMaxHook;
    struct Group_PageData          *PageData;           // Only in Page Mode
//    struct Group_VirtualData       *VirtualData;        // Only in Virtual Mode
//  ArrayMode
    uint16                          Rows;
    uint16                          Columns;
    uint32                          Members;            // computed during askminmax;
//  Refreshing
    struct Region                  *Region;
//  Preferences
    STRPTR                          p_HSpace;
    STRPTR                          p_VSpace;
};

enum    {

        FV_Position_Left,
        FV_Position_Center,
        FV_Position_Right

        };

#define FF_Group_SameWidth      (1 << 0)
#define FF_Group_SameHeight     (1 << 1)
#define FF_Group_RelSizing      (1 << 2)
#define FF_GROUP_CHANGING       (1 << 3)
#define FF_GROUP_RETHINKING     (1 << 4)
#define FF_Group_BufferRegion   (1 << 5)
#define FF_Group_SameSize       (FF_Group_SameWidth | FF_Group_SameHeight)

/****************************************************************************
*** Prototypes **************************************************************
****************************************************************************/

void    Group_DoA       (FClass *Class,FObject Obj,uint32 Method,APTR Msg);

/*** layout.c ***/

extern F_HOOK_PROTO(void, code_layout_horizontal);
extern F_HOOK_PROTO(void, code_layout_vertical);
extern F_HOOK_PROTO(void, code_layout_array);
extern F_HOOK_PROTO(void, code_layout_page);

/*** family.c ***/

extern F_HOOKM_PROTO(APTR, code_family_create_node, FS_Family_CreateNode);
extern F_HOOKM_PROTO(void, code_family_delete_node, FS_Family_DeleteNode);

/****************************************************************************
*** Macros ******************************************************************
****************************************************************************/

#define FF_COMPUTE_W                            FF_Area_Reserved1
#define FF_COMPUTE_H                            FF_Area_Reserved2

/* direct peeking and poking of FC_Area datas is no  longer  possible  e.g.
_w(sub)  is no longer valid. The following macros are very usefull and will
save a *LOT* of typing.  Note  that  the  variable  "area_offset"  must  be
defined. This variable is used to obtain FAreaPublic */

#define _sub_declare_all                        FAreaNode *node
#define _sub_change                             FObject sub = node -> Object
#define _sub_x                                  (node -> AreaPublic -> Box.x)
#define _sub_y                                  (node -> AreaPublic -> Box.y)
#define _sub_w                                  (node -> AreaPublic -> Box.w)
#define _sub_h                                  (node -> AreaPublic -> Box.h)
#define _sub_x2                                 (_sub_x + _sub_w - 1)
#define _sub_y2                                 (_sub_y + _sub_h - 1)
#define _sub_minw                               (node -> AreaPublic -> MinMax.MinW)
#define _sub_minh                               (node -> AreaPublic -> MinMax.MinH)
#define _sub_maxw                               (node -> AreaPublic -> MinMax.MaxW)
#define _sub_maxh                               (node -> AreaPublic -> MinMax.MaxH)
#define _sub_fixw                               (node -> AreaPublic -> MinMax.MinW == node -> AreaPublic -> MinMax.MaxW)
#define _sub_fixh                               (node -> AreaPublic -> MinMax.MinH == node -> AreaPublic -> MinMax.MaxH)
#define _sub_flags                              node -> AreaPublic -> Flags
#define _sub_private_flags                      node->Flags
#define _sub_weight                             (node -> AreaPublic -> Weight)
#define _each                                   (node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head) ; node ; node = node -> Next)

/*** Preference ************************************************************/

#define DEF_GROUP_BACK                          NULL
#define DEF_GROUP_POSITION                      FV_Frame_UpLeft
#define DEF_GROUP_PREPARSE                      "<pens up='shine' shadow='shadow'>"
#define DEF_GROUP_FONT                          NULL
#define DEF_GROUP_FRAME                         "<frame id='15' padding='5' />"
#define DEF_PAGE_PREPARSE                       "<align=center>"
#define DEF_PAGE_ALTPREPARSE                    "<align=center><pens shadow='shadow'><b>"
#define DEF_PAGE_FONT                           NULL
#define DEF_VIRTUAL_FRAME                       "<frame id='15' padding='5' />"
#define DEF_PREFERENCE_FRAME                    "<frame id='18' padding='5' />"
#define DEF_PAGE_BACK                           NULL
#define DEF_GROUP_HSPACING                      5
#define DEF_GROUP_VSPACING                      5


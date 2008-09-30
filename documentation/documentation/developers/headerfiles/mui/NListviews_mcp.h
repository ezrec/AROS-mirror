/*
  NListviews.mcp (c) Copyright 1996-1998 by Gilles Masson
  Registered MUI class, Serial Number: 1d51                            0x9d510001 to 0x9d51001F
                                                                   and 0x9d510101 to 0x9d51013F
  *** use only YOUR OWN Serial Number for your public custom class ***
  NListviews_mcp.h
*/

#ifndef MUI_NLISTVIEWS_MCP_H
#define MUI_NLISTVIEWS_MCP_H


#ifdef USE_ZUNE
#include <mui.h>
#else
#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif
#endif

//#ifndef MUI_NListview_MCC_H
//#include <mui/NListview_mcc.h>
//#endif

#ifndef DEVICES_INPUTEVENT_H
#include <devices/inputevent.h>
#endif


#define MUIC_NListviews_mcp "NListviews.mcp"
#define NListviewsMcpObject MUI_NewObject(MUIC_NListviews_mcp



#define MUICFG_NList_Pen_Title      0x9d510001
#define MUICFG_NList_Pen_List       0x9d510002
#define MUICFG_NList_Pen_Select     0x9d510003
#define MUICFG_NList_Pen_Cursor     0x9d510004
#define MUICFG_NList_Pen_UnselCur   0x9d510005

#define MUICFG_NList_BG_Title       0x9d510006
#define MUICFG_NList_BG_List        0x9d510007
#define MUICFG_NList_BG_Select      0x9d510008
#define MUICFG_NList_BG_Cursor      0x9d510009
#define MUICFG_NList_BG_UnselCur    0x9d51000a

#define MUICFG_NList_Font           0x9d51000b
#define MUICFG_NList_Font_Little    0x9d51000c
#define MUICFG_NList_Font_Fixed     0x9d51000d

#define MUICFG_NList_VertInc        0x9d51000e
#define MUICFG_NList_DragType       0x9d51000f
#define MUICFG_NList_MultiSelect    0x9d510010

#define MUICFG_NListview_VSB        0x9d510011
#define MUICFG_NListview_HSB        0x9d510012

#define MUICFG_NList_DragQualifier  0x9d510013
#define MUICFG_NList_Smooth         0x9d510014
#define MUICFG_NList_ForcePen       0x9d510015
#define MUICFG_NList_StackCheck     0x9d510016
#define MUICFG_NList_ColWidthDrag   0x9d510017
#define MUICFG_NList_PartialCol     0x9d510018
#define MUICFG_NList_List_Select    0x9d510019
#define MUICFG_NList_Menu           0x9d51001A
#define MUICFG_NList_PartialChar    0x9d51001B
#define MUICFG_NList_PointerColor   0x9d51001C
#define MUICFG_NList_SerMouseFix    0x9d51001D
#define MUICFG_NList_Keys           0x9d51001E
#define MUICFG_NList_DragLines      0x9d51001F

#define MUICFG_NList_WheelStep      0x9d510101
#define MUICFG_NList_WheelFast      0x9d510102
#define MUICFG_NList_WheelMMB       0x9d510103

#define MUIV_NList_MultiSelect_MMB_On     0x0300
#define MUIV_NList_MultiSelect_MMB_Off    0x0100

#define MUIV_NList_ColWidthDrag_One       0
#define MUIV_NList_ColWidthDrag_All       1
#define MUIV_NList_ColWidthDrag_Visual    2


#define DEFAULT_PEN_TITLE    "m5"
#define DEFAULT_PEN_LIST     "m5"
#define DEFAULT_PEN_SELECT   "m5"
#define DEFAULT_PEN_CURSOR   "m5"
#define DEFAULT_PEN_UNSELCUR "m5"
#define DEFAULT_BG_TITLE     "2:m3"
#define DEFAULT_BG_LIST      "2:m2"
#define DEFAULT_BG_SELECT    "0:135"
#define DEFAULT_BG_CURSOR    "0:131"
#define DEFAULT_BG_UNSELCUR  "2:m3"
#define DEFAULT_VERT_INC     1
#define DEFAULT_HSB          MUIV_NListview_HSB_Auto
#define DEFAULT_CWD          MUIV_NList_ColWidthDrag_All
#define DEFAULT_WHEELSTEP    1
#define DEFAULT_WHEELFAST    5
#define DEFAULT_DRAGLINES    10


#define KEYTAG_QUALIFIER_MULTISELECT       0x9d51C001
#define KEYTAG_QUALIFIER_DRAG              0x9d51C002
#define KEYTAG_QUALIFIER_BALANCE           0x9d51C003
#define KEYTAG_COPY_TO_CLIPBOARD           0x9d518001
#define KEYTAG_DEFAULT_WIDTH_COLUMN        0x9d518002
#define KEYTAG_DEFAULT_WIDTH_ALL_COLUMNS   0x9d518003
#define KEYTAG_DEFAULT_ORDER_COLUMN        0x9d518004
#define KEYTAG_DEFAULT_ORDER_ALL_COLUMNS   0x9d518005
#define KEYTAG_SELECT_TO_TOP               0x9d518006
#define KEYTAG_SELECT_TO_BOTTOM            0x9d518007
#define KEYTAG_SELECT_TO_PAGE_UP           0x9d518008
#define KEYTAG_SELECT_TO_PAGE_DOWN         0x9d518009
#define KEYTAG_SELECT_UP                   0x9d51800A
#define KEYTAG_SELECT_DOWN                 0x9d51800B
#define KEYTAG_TOGGLE_ACTIVE               0x9d51800C
#define KEYTAG_QUALIFIER_WHEEL_FAST        0x9d51800D
#define KEYTAG_QUALIFIER_WHEEL_HORIZ       0x9d51800E
#define KEYTAG_QUALIFIER_TITLECLICK2       0x9d51800F


struct KeyBinding {
  ULONG kb_KeyTag;
  UWORD kb_Code;
  UWORD kb_Qualifier;
};

#define KBQUAL_MASK   0x01FF    /* only qualifier keys bits are used in kb_Qualifier */
#define KBSYM_MASK    0x7000    /* upper kb_Qualifier bits are use for synonyms */

#define KBSYM_SHIFT   0x1000    /* left- and right- shift are equivalent     */
#define KBSYM_CAPS    0x2000    /* either shift or caps lock are equivalent  */
#define KBSYM_ALT     0x4000    /* left- and right- alt are equivalent       */

#define KBQUALIFIER_SHIFT  (KBSYM_SHIFT | IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)  /* 'shift' */
#define KBQUALIFIER_CAPS   (KBSYM_CAPS | KBQUALIFIER_SHIFT | IEQUALIFIER_CAPSLOCK)  /* 'caps' */
#define KBQUALIFIER_ALT    (KBSYM_ALT | IEQUALIFIER_LALT | IEQUALIFIER_RALT)        /* 'alt' */

#define DEFAULT_KEYS_ARRAY                                            \
  struct KeyBinding default_keys[] =                                  \
  {                                                                   \
    { KEYTAG_QUALIFIER_MULTISELECT    , (UWORD)~0  , KBQUALIFIER_SHIFT    }, \
    { KEYTAG_QUALIFIER_DRAG           , (UWORD)~0  , IEQUALIFIER_CONTROL  }, \
    { KEYTAG_QUALIFIER_DRAG           , (UWORD)~0  , IEQUALIFIER_CONTROL|KBQUALIFIER_SHIFT  }, \
    { KEYTAG_QUALIFIER_BALANCE        , (UWORD)~0  , KBQUALIFIER_SHIFT    }, \
    { KEYTAG_COPY_TO_CLIPBOARD        , 0x33, IEQUALIFIER_RCOMMAND }, \
    { KEYTAG_COPY_TO_CLIPBOARD        , 0x33, IEQUALIFIER_LCOMMAND }, \
    { KEYTAG_COPY_TO_CLIPBOARD        , 0x32, IEQUALIFIER_RCOMMAND }, \
    { KEYTAG_COPY_TO_CLIPBOARD        , 0x32, IEQUALIFIER_LCOMMAND }, \
    { KEYTAG_DEFAULT_WIDTH_COLUMN     , 0x1D, IEQUALIFIER_RCOMMAND|IEQUALIFIER_NUMERICPAD }, \
    { KEYTAG_DEFAULT_WIDTH_ALL_COLUMNS, 0x1E, IEQUALIFIER_RCOMMAND|IEQUALIFIER_NUMERICPAD }, \
    { KEYTAG_DEFAULT_ORDER_COLUMN     , 0x1F, IEQUALIFIER_RCOMMAND|IEQUALIFIER_NUMERICPAD }, \
    { KEYTAG_DEFAULT_ORDER_ALL_COLUMNS, 0x2D, IEQUALIFIER_RCOMMAND|IEQUALIFIER_NUMERICPAD }, \
    { KEYTAG_SELECT_TO_TOP            , 0x4C, IEQUALIFIER_CONTROL|KBQUALIFIER_ALT },   \
    { KEYTAG_SELECT_TO_BOTTOM         , 0x4D, IEQUALIFIER_CONTROL|KBQUALIFIER_ALT },   \
    { KEYTAG_SELECT_TO_PAGE_UP        , 0x4C, IEQUALIFIER_CONTROL|KBQUALIFIER_SHIFT }, \
    { KEYTAG_SELECT_TO_PAGE_DOWN      , 0x4D, IEQUALIFIER_CONTROL|KBQUALIFIER_SHIFT }, \
    { KEYTAG_SELECT_UP                , 0x4C, IEQUALIFIER_CONTROL }, \
    { KEYTAG_SELECT_DOWN              , 0x4D, IEQUALIFIER_CONTROL }, \
    { KEYTAG_TOGGLE_ACTIVE            , 0x40, IEQUALIFIER_CONTROL }, \
    { KEYTAG_QUALIFIER_WHEEL_FAST     , (UWORD)~0  , KBQUALIFIER_SHIFT }, \
    { KEYTAG_QUALIFIER_WHEEL_HORIZ    , (UWORD)~0  , KBQUALIFIER_ALT }, \
    { KEYTAG_QUALIFIER_TITLECLICK2    , (UWORD)~0  , KBQUALIFIER_SHIFT }, \
    { 0L, (UWORD)~0, 0 } \
  };

/*
 *  #define IEQUALIFIER_LSHIFT        0x0001   'lshift'
 *  #define IEQUALIFIER_RSHIFT        0x0002   'rshift'
 *  #define IEQUALIFIER_CAPSLOCK      0x0004   'capslock'
 *  #define IEQUALIFIER_CONTROL       0x0008   'control'
 *  #define IEQUALIFIER_LALT          0x0010   'lalt'
 *  #define IEQUALIFIER_RALT          0x0020   'ralt'
 *  #define IEQUALIFIER_LCOMMAND      0x0040   'lcommand'
 *  #define IEQUALIFIER_RCOMMAND      0x0080   'rcommand'
 *  #define IEQUALIFIER_NUMERICPAD    0x0100   'numpad'
 */

#endif /* MUI_NLISTVIEWS_MCP_H */

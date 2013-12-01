// pmprefs.h
//
// Menu Prefs
//
// $Date$
// $Revision$
//

#ifndef PM_PREFS_H
#define PM_PREFS_H

#include "prefs/popupmenu.h"


struct PopupMenuPrefs
{
    UBYTE pmp_Flags;            /* Enable shadows, transparency, etc. */
    UBYTE pmp_SubMenuDelay;     /* Delay before opening submenus      */
    UBYTE pmp_Animation;        /* Animation, see below for defines   */
                                /* (Only one animation effect implemented currently) */
    UBYTE pmp_PulldownPos;      /* Where to show pulldownmenus        */
    WORD  pmp_Sticky;           /* Use 'sticky' mode                  */
    UBYTE pmp_MenuBorder;       /* Menu border                        */
    UBYTE pmp_SelItemBorder;    /* Border around selected item        */
    UBYTE pmp_SeparatorBar;     /* Separator bar style                */
    UBYTE pmp_MenuTitles;       /* Style flags for drawing menu texts       */
    UBYTE pmp_MenuItems;        /* Style flags for drawing menu item texts  */
    UBYTE pmp_XOffset;
    UBYTE pmp_YOffset;
    UBYTE pmp_XSpace;
    UBYTE pmp_YSpace;
    UBYTE pmp_Intermediate;
    BYTE  pmp_TextDisplace;
    UBYTE pmp_TransparencyBlur;
};

/// Frames
#define BUTTON_FRAME		0
#define MAGIC_FRAME		1
#define THICK_BUTTON_FRAME	2
#define DOUBLE_FRAME		3
#define DROPBOX_FRAME		4
#define INTUI_FRAME		5
///

/// TextPatch
#define TP_CENTER       0x0001
#define TP_UNDERLINE	0x0002
#define TP_BOLD         0x0004
#define TP_SHINE        0x0008
#define TP_SHADOW       0x0010
#define TP_TEXT         0x0020
#define TP_HILITE       0x0040
#define TP_SHADOWED     0x0080
#define TP_LEFT         0x0100
#define TP_RIGHT        0x0200
#define TP_EMBOSS       0x0400
#define TP_KILLBAR      0x0800
#define TP_OUTLINE      0x1000
#define TP_ACTIVATE     0x8000
///

/// File name/ID
#define PMP_ID		(0x504d4e55)
#define PMP_VERSION	1
#define PMP_PATH_OLD	"ENV:sys/PopupMenu.prefs"
#define PMP_PATH_NEW	"ENV:sys/NewPopupMenu.prefs"
///

extern struct PopupMenuPrefs	*PM_Prefs;

void PM_Prefs_Load(CONST_STRPTR prefsFileOld, CONST_STRPTR prefsFileNew);
void PM_Prefs_Free(void);

#endif

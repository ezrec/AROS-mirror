#ifndef PREFS_POPUPMENU_H
#define PREFS_POPUPMENU_H
/*
**	$VER: popupmenu.h 50.1 (15.5.2002)
**	Includes Release 50.1
**
**	File format for example preferences
**
**	(C) Copyright 2002 Amiga, Inc.
**	All Rights Reserved
*/

/*****************************************************************************/


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

/*****************************************************************************/

// preferences.library IDs

#define PMP_AnimationType	0x80007001	// UBYTE
#define PMP_PullDownMenuPos	0x80007002	// UBYTE
#define PMP_Sticky      	0x80007003	// WORD
#define PMP_Flags       	0x80007004	// UBYTE
#define PMP_SubMenuDelay	0x80007005	// UBYTE
#define PMP_MenuBorderType	0x80007006	// UBYTE
#define PMP_SelItemBorderType	0x80007007	// UBYTE
#define PMP_SeparatorBarStyle	0x80007008	// UBYTE
#define PMP_XOffset		0x80007009	// UBYTE
#define PMP_YOffset		0x8000700a	// UBYTE
#define PMP_XSpace		0x8000700b	// UBYTE
#define PMP_YSpace		0x8000700c	// UBYTE
#define PMP_Intermediate	0x8000700d	// UBYTE
#define PMP_TransparencyBlur	0x8000700e	// UBYTE
#define PMP_TextDisplace	0x8000700f	// BYTE
#define PMP_MenuTitleStyle	0x80007010	// UBYTE
#define PMP_MenuItemStyle	0x80007011	// UBYTE

/*****************************************************************************/


#define ID_PMNU MAKE_ID('P','M','N','U')

/* !!! obsolete !!! */
struct oldPopupMenuPrefs
{
    UBYTE pmp_Flags;            /* Enable shadows, transparency, etc. */
    UBYTE pmp_SubMenuDelay;     /* Delay before opening submenus      */
    UBYTE pmp_Animation;        /* Animation, see below for defines   */
                                /* (Only one animation effect implemented currently) */
    UBYTE pmp_PulldownPos;      /* Where to show pulldownmenus        */
    BOOL  pmp_Sticky;           /* Use 'sticky' mode                  */
    BOOL  pmp_SameHeight;       /* !!unused!! Try to give all items same height  */
    UBYTE pmp_MenuBorder;       /* Menu border                        */
    UBYTE pmp_SelItemBorder;    /* Border around selected item        */
    UBYTE pmp_SeparatorBar;     /* Separator bar style                */
    UBYTE pmp_MenuTitles;       /* Flags for drawing menu texts       */
    UBYTE pmp_MenuItems;        /* Flags for drawing menu item texts  */
    UBYTE pmp_XOffset;
    UBYTE pmp_YOffset;
    UBYTE pmp_XSpace;
    UBYTE pmp_YSpace;
    UBYTE pmp_Intermediate;
    BYTE  pmp_TextDisplace;
    BYTE  pmp_ShadowR;          /* !!unused!! */
    BYTE  pmp_ShadowG;          /* !!unused!! */
    BYTE  pmp_ShadowB;          /* !!unused!! */
    BYTE  pmp_TransparencyR;    /* !!unused!! */
    BYTE  pmp_TransparencyG;    /* !!unused!! */
    BYTE  pmp_TransparencyB;    /* !!unused!! */
    UBYTE pmp_TransparencyBlur;
    UBYTE pmp_AnimationSpeed;   /* !!unused!! */
    UBYTE pmp_Reserved[16];     /* Reserved for future use */
};

#define PMP_FLAGS_SHADOWS       0x01
#define PMP_FLAGS_TRANSPARENCY  0x02

#define PMP_ANIM_NONE		0
#define PMP_ANIM_ZOOM		1
#define PMP_ANIM_FADE		2
#define PMP_ANIM_EXPLODE	3

#define PMP_PD_SCREENBAR	0
#define PMP_PD_WINDOWBAR	1
#define PMP_PD_MOUSE		2	/* Show as popup menu */

#define PMP_TITLE_NORMAL	0x00
#define PMP_TITLE_ITALIC	0x01
#define PMP_TITLE_BOLD		0x02
#define PMP_TITLE_UNDERLINE	0x04
#define PMP_TITLE_SHADOW	0x08
#define PMP_TITLE_EMBOSS	0x10
#define PMP_TITLE_OUTLINE	0x20

#define PMP_TEXT_NORMAL		0x00
#define PMP_TEXT_ITALIC		0x01
#define PMP_TEXT_BOLD		0x02
#define PMP_TEXT_UNDERLINE	0x04
#define PMP_TEXT_SHADOW		0x08
#define PMP_TEXT_EMBOSS		0x10
#define PMP_TEXT_OUTLINE	0x20

#define PMP_MENUBORDER_THIN	0
#define PMP_MENUBORDER_MM	1
#define PMP_MENUBORDER_THICK	2
#define PMP_MENUBORDER_RIDGE	3
#define PMP_MENUBORDER_DROPBOX	4
#define PMP_MENUBORDER_OLDSTYLE	5

#define PMP_SELITEM_NO_BORDER	0
#define PMP_SELITEM_RECESS	1
#define PMP_SELITEM_RAISE	2

/*****************************************************************************/


#endif /* PREFS_POPUPMENU_H */

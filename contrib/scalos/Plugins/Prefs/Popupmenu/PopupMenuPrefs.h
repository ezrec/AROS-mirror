// PopupMenuPrefs.h
// $Date$
// $Revision$


#ifndef SCALOS_POPUPMENUPREFS_H
#define	SCALOS_POPUPMENUPREFS_H

#include <prefs/popupmenu.h>

//----------------------------------------------------------------------------

#define PMP_PATH_OLD	"ENV:sys/PopupMenu.prefs"
#define PMP_PATH_NEW	"ENV:sys/NewPopupMenu.prefs"
#define PMP_S_PATH_OLD	"ENVARC:sys/PopupMenu.prefs"
#define PMP_S_PATH_NEW	"ENVARC:sys/NewPopupMenu.prefs"

//----------------------------------------------------------------------------

enum HookIndex
{
	HOOKNDX_ImagePopAslFileStart,

	HOOKNDX_ResetToDefaults,
	HOOKNDX_Open,
	HOOKNDX_LastSaved,
	HOOKNDX_Restore,
	HOOKNDX_SaveAs,

	HOOKNDX_About,
	HOOKNDX_AppMessage,
	HOOKNDX_SettingsChanged,
	HOOKNDX_AslIntuiMsg,

	HOOKNDX_FrameSelected,
	HOOKNDX_SelFrameSelected,

	HOOKNDX_MAX
};

enum ObjectIndex
{
	OBJNDX_APP_Main,
	OBJNDX_WIN_Main,

	OBJNDX_Group_Main,

	OBJNDX_Lamp_Changed,

	OBJNDX_Listview_ColorNames,
	OBJNDX_CheckMark_UseDrawInfoPens,

	OBJNDX_Slider_DelaySubMenus,
	OBJNDX_Cycle_AnimationType,
	OBJNDX_CheckMark_MenuShadows,
	OBJNDX_CheckMark_Sticky,

	OBJNDX_Cycle_ImageSet,
	OBJNDX_PopAsl_LoadImageSet,

	OBJNDX_Button_FrameThin,
	OBJNDX_Button_FrameMM,
	OBJNDX_Button_FrameThick,
	OBJNDX_Button_FrameRidge,
	OBJNDX_Button_FrameDropBox,
	OBJNDX_Button_FrameOldStyle,
	OBJNDX_Button_Normal,
	OBJNDX_Button_Raised,
	OBJNDX_Button_Recessed,
	OBJNDX_CheckMark_NewLook,
	OBJNDX_CheckMark_OldLook,

	OBJNDX_Slider_HorizontalSpacing,
	OBJNDX_Slider_VerticalSpacing,
	OBJNDX_Slider_Horizontal,
	OBJNDX_Slider_VerticalOffset,
	OBJNDX_Slider_IntermediateSpacing,
	OBJNDX_Slider_TextDisplacement,

	OBJNDX_CheckMark_MenuTitles_Italic,
	OBJNDX_CheckMark_MenuTitles_Underlined,
	OBJNDX_CheckMark_MenuTitles_Bold,
	OBJNDX_CheckMark_MenuTitles_Shadowed,
	OBJNDX_CheckMark_MenuTitles_Outlined,
	OBJNDX_CheckMark_MenuTitles_Embossed,

	OBJNDX_CheckMark_MenuItems_Italic,
	OBJNDX_CheckMark_MenuItems_Underlined,
	OBJNDX_CheckMark_MenuItems_Bold,
	OBJNDX_CheckMark_MenuItems_Shadowed,
	OBJNDX_CheckMark_MenuItems_Outlined,
	OBJNDX_CheckMark_MenuItems_Embossed,

	OBJNDX_Cycle_PulldownMode,
	OBJNDX_Cycle_PulldownLocation,

	OBJNDX_Cycle_ColourPrecision,
	OBJNDX_CheckMark_UseWindows,
	OBJNDX_CheckMark_RealShadows,

	OBJNDX_Slider_Transp_Blur,
	OBJNDX_CheckMark_Transparency_Enable,
	OBJNDX_Group_Transp_Blur,

	OBJNDX_MAX
};

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

struct PopupMenuPrefsInst
{
	ULONG	mpb_fCreateIcons;
	CONST_STRPTR	mpb_ProgramName;

	struct	Hook mpb_Hooks[HOOKNDX_MAX];
	APTR	mpb_Objects[OBJNDX_MAX];

	ULONG	mpb_PageIsActive;
	BOOL	mpb_Changed;
	BOOL	mpb_QuietFlag;

	struct Screen *mpb_WBScreen;

	struct FileRequester *mpb_LoadReq;
	struct FileRequester *mpb_SaveReq;

	struct PopupMenuPrefs mpb_PMPrefs;
};

//----------------------------------------------------------------------------

#ifndef	Sizeof
#define	Sizeof(array)	(sizeof(array)/sizeof(array[0]))
#endif

//----------------------------------------------------------------------------

#define	d1(x)	;
#define	d2(x)	x;

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//----------------------------------------------------------------------------

struct ScalosPopupMenu_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif /* SCALOS_POPUPMENUPREFS_H */

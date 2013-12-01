#ifndef SCALOS_PREFERENCES_H
#define SCALOS_PREFERENCES_H
/*
**  $VER: preferences.i 41.7 (10 Aug 2009 17:02:08)
**
** $Date$
** $Revision$
**
**      preferences.library include
**
**   (C) Copyright 1996-1997 ALiENDESiGN
**   (C) Copyright 2000-2009 The Scalos Team
**  All Rights Reserved
*/

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

struct PrefsStruct 
{
	WORD        ps_Size;
	/* followed by the data, a total of ps_Size bytes */
};

// -- Scalos specific Prefs:

// ------------------ ScalosPrefs -------------------------

// Name: "Scalos" or "ScalosPrefs"
// ID: "MAIN"

#define SCP_IconOffsets			0x80000001	// struct 4 * UWORD - left/top/right/bottom offset
#define SCP_IconNormFrame		0x80000002	// word - frame type around unselected icons
#define SCP_IconSelFrame		0x80000003	// word - frame type around selected icons
#define SCP_IconTextMode		0x80000004	// word
#define SCP_IconSecLine			0x80000005	// word
#define SCP_IconTextSkip		0x80000006	// word

#define SCP_BobsType			0x80000010	// byte image or image+text
#define SCP_BobsMethod			0x80000011	// byte system or custom
#define SCP_BobsTranspMode		0x80000012	// byte solid or transparent
#define SCP_BobsTranspType		0x80000013	// byte ghosted or real transparent
#define SCP_BobsTransp			0x80000014	// long when transparent

#define SCP_ScreenTitle			0x80000020	// string
#define SCP_RootWinTitle		0x80000021	// string
#define SCP_WindowTitle			0x80000022	// string
#define SCP_Separator			0x80000023	// byte
#define SCP_TitleRefresh		0x80000024	// byte

#define SCP_PathsDefIcons		0x80000025	// string
#define SCP_PathsDiskCopy		0x80000026	// string
#define SCP_PathsWBStartup		0x80000027	// string
#define SCP_PathsHome			0x80000028	// string

#define SCP_MiscAutoRemove		0x80000029	// byte
#define SCP_MiscClickTransp		0x8000002a	// byte
#define SCP_MiscHardEmulation		0x8000002b	// byte
#define SCP_MiscUseExAll		0x8000002c	// byte
#define SCP_MiscWindowType		0x8000002d	// byte
#define SCP_MiscDoWaitDelay		0x8000002e	// byte
#define SCP_MiscDiskiconsRefresh	0x8000002f	// byte delay
#define SCP_MiscMenuCurrentDir		0x80000030	// byte

#define SCP_NewIconsTransparent		0x80000031	// byte
#define SCP_NewIconsPrecision		0x80000032	// long

#define SCP_TextModeFont		0x80000033	// string
#define SCP_TextModeDateFormat		0x80000034	// byte

#define SCP_PlugInList			0x80000035	// list
#define	SCP_MMB_Move			0x80000036	// byte +jl+
#define	SCP_TextMode_ListColumns	0x80000037	// 7 bytes +jl+
#define	SCP_RefreshOnMemoryChange	0x80000038	// byte +jl+
#define SCP_WindowPopupTitleOnly	0x80000039	// byte
#define	SCP_DefaultWindowSize		0x8000003a	// 4 * word +jl+
#define	SCP_IconCleanupSpace		0x8000003b	// 4 x word +jl+
#define	SCP_FullBench			0x8000003c	// byte
#define	SCP_LoadDefIconsFirst		0x8000003d	// byte
#define	SCP_DefaultIconsSaveable	0x8000003e	// byte
#define	SCP_MacLikeSelection		0x8000003f	// byte
#define	SCP_DropStart			0x80000040	// byte
#define	SCP_HideHiddenFiles		0x80000041	// byte
#define	SCP_PopScreenTitle		0x80000042	// byte

#define	SCP_SplashWindowEnable		0x80000043	// byte
#define	SCP_SplashWindowHoldTime	0x80000044	// UWORD
#define	SCP_StatusBarEnable		0x80000045	// byte
#define	SCP_ToolTipsEnable		0x80000046	// BYTE
#define	SCP_ShowDragDropObjCount	0x80000047	// BYTE
#define	SCP_DragDropIconsSingle		0x80000048	// BYTE
#define	SCP_TextWindowsStriped		0x80000049	// BYTE
#define	SCP_DropMarkMode		0x8000004a	// BYTE	enum WindowDropMarkTypes
#define	SCP_DragDropCopyQualifier	0x8000004b	// ULONG Keyboard qualifier to force D&D copy operation
#define	SCP_UnderSoftLinkNames		0x8000004c	// BYTE
#define	SCP_ConsoleName			0x8000004d	// char[256] name for CLI/ARexx console
#define	SCP_TransparencyIconDrag	0x8000004e	// UWORD percentage of transparency during D&D
#define	SCP_TransparencyIconShadow	0x8000004f	// UWORD percentage of transparency for icon shadows
#define	SCP_PopupApplySelectedQualifier	0x80000050	// ULONG Keyboard qualifier to apply popup menus to all selected icons
#define	SCP_ToolTipPopupDelay		0x80000051	// ULONG delay until tooltips pop up [s]
#define	SCP_DefaultStackSize		0x80000052	// ULONG default stack size for WB applications
#define	SCP_TransparencyDefaultIcon	0x80000053	// UWORD percentage of transparency for default icons
#define	SCP_EasyMultiSelect		0x80000054	// BYTE  select multiple icons without holding SHIFT key
#define	SCP_EasyMultiDrag		0x80000055	// BYTE  start dragging multiple selected icons without holding SHIFT key
#define	SCP_UseTTScreenFont		0x80000056	// BYTE  enable TrueType font as screen font
#define	SCP_UseTTIconFont		0x80000057	// BYTE  enable TrueType font as icon font
#define	SCP_UseTTTextWindowFont		0x80000058	// BYTE  enable TrueType font as text window font
#define	SCP_TTScreenFontDesc		0x80000059	// char[256] Screen TrueType font descriptor
#define	SCP_TTIconFontDesc		0x8000005a	// char[256] Icon TrueType font descriptor
#define	SCP_TTTextWindowFontDesc	0x8000005b	// char[256] Text window TrueType font descriptor
#define	SCP_DragDropMakeLinkQualifier	0x8000005c	// ULONG Keyboard qualifier to force D&D create link operation
#define	SCP_DragDropMoveQualifier	0x8000005d	// ULONG Keyboard qualifier to force D&D move operation
#define	SCP_CreateSoftLinks		0x8000005e	// BYTE  1=create softlinks, 0=create hardlinks
#define	SCP_HideProtectHiddenFiles	0x8000005f	// BYTE  1=hide files with FIBF_HIDDEN protection bit activated, 0=show files with FIBF_HIDDEN  protection bit activated
#define	SCP_TTfAntialiasing		0x80000060	// BYTE  Value for ttengine antialiasing (one of TT_Antialias_Auto, TT_Antialias_Off, TT_Antialias_On)
#define	SCP_TTfGamma			0x80000061	// WORD	 1000 * value for ttengine gamma correction
#define SCP_CleanupOnResize		0x80000062	// BYTE  1=clean window contents on resize
#define SCP_IconHiliteUnderMouse	0x80000063	// BYTE  1=Hilight icons under mouse
#define SCP_AutoLeaveOut		0x80000064	// BYTE  1=draggins icons on desktop autmatically issues "leave out"
#define	SCP_WinRefreshOnMemoryChange	0x80000065	// byte +jmc+
#define SCP_WinTitleRefresh		0x80000066	// byte +jmc
#define SCP_ShowAllDefault		0x80000067	// UBYTE default "show all" display - one of DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
#define SCP_ViewByDefault		0x80000068	// UBYTE default "view by" mode - one of DDVM_BYDEFAULT, DDVM_BYICON, DDVM_BYNAME, DDVM_BYDATE, DDVM_BYSIZE, DDVM_BYTYPE
#define SCP_IconSizeConstraints		0x80000069	// struct Rectangle - icon size limits, larger or smaller icons are scaled to defined sizes.
#define SCP_SelectTextIconName		0x8000006a	// UBYTE 1=text icons selectable area is limited to the "name" column
#define SCP_ShowThumbnails		0x8000006b	// UBYTE 0=never, 1=as default icon, 2=always
#define SCP_ThumbnailSize		0x8000006c	// UWORD desired x/y size for thumbnails
#define SCP_PathsTheme			0x8000006d	// string
#define SCP_PathsThumbnailDb		0x8000006e	// string
#define SCP_ThumbnailMaxAge		0x8000006f	// UWORD maximum number of days thumbnails are cached without access
#define SCP_DeviceWinIconLayout		0x80000070	// UBYTE[9] device window icon layout mode, icon type (e.g. WBDISK) is index
#define SCP_IconWinIconLayout		0x80000071	// UBYTE[9] icon window icon layout mode, icon type (e.g. WBDISK) is index
#define SCP_ThumbnailMinSizeLimit	0x80000072	// UWORD Minimum images's dimensions to cache.
#define SCP_ShowThumbnailsAsDefault	0x80000073	// BYTE 0=Don't show thumbnails with "ShowOnlyIcons" show type
#define SCP_ToolTipTransparency		0x80000074	// UWORD percentage of transparency for tooltips
#define SCP_PopupApplySelectedAlways	0x80000075	// BYTE 1=always apply popup menus to all selected icons
#define SCP_ThumbnailQuality		0x80000076	// ULONG Thumbnail quality: SCALOSPREVIEWA_Quality_Min..SCALOSPREVIEWA_Quality_Max
#define SCP_ThumbnailsSquare		0x80000077	// BYTE 1=thumbnail icons are always square
#define SCP_IconNormThumbnailFrame	0x80000078	// WORD - frame type around unselected thumbnail icons
#define SCP_IconSelThumbnailFrame	0x80000079	// WORD - frame type around selected thumbnail icons
#define SCP_ThumbnailOffsets           	0x8000007a	// struct 4 * UWORD - left/top/right/bottom offset
#define SCP_ThumbnailsFillBackground	0x8000007b	// UBYTE - Flag: background of square thumbnails is filled with PENIDX_THUMBNAILBACKGROUNDPEN
#define SCP_TransparencyThumbnailsBack 	0x8000007c	// UWORD - percentage of thumbnail background transparency
#define SCP_SingleWindowLassoQualifier	0x8000007d	// ULONG - Key qualifier to use single-window lasso operation
#define SCP_EnableDropMenu		0x8000007e	// UBYTE - Flag: use drop menu for copy/move/create link selection
#define SCP_PathsImagecache		0x8000007f	// string -- path to Scalos datatypes image cache
#define SCP_ControlBarGadgetsBr		0x80000080	// array of SCP_GadgetEntry for browser mode windows
#define SCP_ControlBarGadgetStringsBr	0x80000081	// SCP_GadgetStringEntry -- strings for SCP_ControlBarGadgets, starts with ULONG containing total length for browser mode windows
#define	SCP_CheckOverlappingIcons	0x80000082	// UBYTE - check for overlapping icons in icon windows
#define SCP_CopyBuffLen			0x80000083	// ULONG - buffer size for file copying
#define SCP_SelectedTextRectangle	0x80000084	// UBYTE - Flag: Draw rectangle around selected icon text
#define SCP_SelTextRectBorderX		0x80000085	// UWORD - Additional horizontal border around selected icon text rectangle
#define SCP_SelTextRectBorderY		0x80000086	// UWORD - Additional vertical border around selected icon text rectangle
#define SCP_SelTextRectRadius		0x80000087	// UWORD - Radius for selected icon text rectangle corners
#define SCP_ActiveWindowTransparency	0x80000088	// UWORD - percentage of active Scalos window opacity (0=transparent, 100=opaque)
#define SCP_InactiveWindowTransparency	0x80000089	// UWORD - percentage of inactive Scalos window opacity (0=transparent, 100=opaque)
#define SCP_PopupWindowsEnable		0x8000008a	// BYTE - enable "spring-loaded" popup windows during D&D
#define SCP_PopupWindowsDelay		0x8000008b	// UWORD - Time in s before drawer window pops up during D&D
#define	SCP_SelectMarkerBaseColor	0x8000008c	// struct ARGB - base color for text window selection marker
#define	SCP_SelectMarkerTransparency	0x8000008d	// UBYTE - transparency for text window selection marker (0=transparent, 0xff=opaque)
#define SCP_ControlBarGadgetsStd	0x8000008e	// array of SCP_GadgetEntry for normal windows
#define SCP_ControlBarGadgetStringsStd	0x8000008f	// SCP_GadgetStringEntry -- strings for SCP_ControlBarGadgets, starts with ULONG containing total length for normal windows
#define SCP_MaxUndoSteps		0x80000090	// ULONG - maximum number of Undo/Redo steps
#define SCP_PathsSQLiteTempDir          0x80000091	// string -- path for SQLite3 temporary file
#define	SCP_DrawerSortMode		0x80000092	// enum ScalosDrawerSortType - where are drawers sorted in text windows (first/last/mixed)
#define SCP_IconNominalSize		0x80000093	// UWORD -- standard icon scaling factor in percent

//---------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* SCALOS_PREFERENCES_H */


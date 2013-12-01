OPT MODULE
OPT PREPROCESS
OPT EXPORT

/*
**  $VER: preferences_struct.e 40.35 (08.07.06)
**
** $Date: 2006-07-30 10:03:01 +0200 (dim, 30 juil 2006) $
** $Revision: 1826 $
** $Id: $
**
**      Scalos preferences module
**
**   (C) Copyright 1996-1997 ALiENDESiGN
**   (C) Copyright 2000-2006 The Scalos Team
**  All Rights Reserved
*/

OBJECT prefsstruct 
	ps_Size
	/* followed by the data, a total of ps_Size bytes */
ENDOBJECT

-> -- Scalos specific Prefs:

-> ------------------ ScalosPrefs -------------------------

-> Name: "Scalos" or "ScalosPrefs"
-> ID: "MAIN"

CONST	SCP_IconOffsets			= $80000001,	-> struct [8]	4 * word
	SCP_IconNormFrame		= $80000002,	-> word
	SCP_IconSelFrame		= $80000003,	-> word
	SCP_IconTextMode		= $80000004,	-> word
	SCP_IconSecLine			= $80000005,	-> word
	SCP_IconTextSkip		= $80000006,	-> word

	SCP_BobsType			= $80000010,	-> byte image or image+text
	SCP_BobsMethod			= $80000011,	-> byte system or custom
	SCP_BobsTranspMode		= $80000012,	-> byte solid or transparent
	SCP_BobsTranspType		= $80000013,	-> byte ghosted or real transparent
	SCP_BobsTransp			= $80000014,	-> long when transparent

	SCP_ScreenTitle			= $80000020,	-> string
	SCP_RootWinTitle		= $80000021,	-> string
	SCP_WindowTitle			= $80000022,	-> string
	SCP_Separator			= $80000023,	-> byte
	SCP_TitleRefresh		= $80000024,	-> byte

	SCP_PathsDefIcons		= $80000025,	-> string
	SCP_PathsDiskCopy		= $80000026,	-> string
	SCP_PathsWBStartup		= $80000027,	-> string
	SCP_PathsHome			= $80000028,	-> string

	SCP_MiscAutoRemove		= $80000029,	-> byte
	SCP_MiscClickTransp		= $8000002a,	-> byte
	SCP_MiscHardEmulation		= $8000002b,	-> byte
	SCP_MiscUseExAll		= $8000002c,	-> byte
	SCP_MiscWindowType		= $8000002d,	-> byte
	SCP_MiscDoWaitDelay		= $8000002e,	-> byte
	SCP_MiscDiskiconsRefresh	= $8000002f,	-> byte delay
	SCP_MiscMenuCurrentDir		= $80000030,	-> byte

	SCP_NewIconsTransparent		= $80000031,	-> byte
	SCP_NewIconsPrecision		= $80000032,	-> long

	SCP_TextModeFont		= $80000033,	-> string
	SCP_TextModeDateFormat		= $80000034,	-> byte

	SCP_PlugInList			= $80000035,	-> list
	SCP_MMB_Move			= $80000036,	-> byte +jl+
	SCP_TextMode_ListColumns	= $80000037,	-> 7 bytes +jl+
	SCP_RefreshOnMemoryChange	= $80000038,	-> byte +jl+
	SCP_WindowPopupTitleOnly	= $80000039,	-> byte
	SCP_DefaultWindowSize		= $8000003a,	-> 4 * word +jl+
	SCP_IconCleanupSpace		= $8000003b,	-> 4 x word +jl+
	SCP_FullBench			= $8000003c,	-> byte
	SCP_LoadDefIconsFirst		= $8000003d,	-> byte
	SCP_DefaultIconsSaveable	= $8000003e,	-> byte
	SCP_MacLikeSelection		= $8000003f,	-> byte
	SCP_DropStart			= $80000040,	-> byte
	SCP_HideHiddenFiles		= $80000041,	-> byte
	SCP_PopScreenTitle		= $80000042,	-> byte

	SCP_SplashWindowEnable		= $80000043,	-> byte
	SCP_SplashWindowHoldTime	= $80000044,	-> UWORD
	SCP_StatusBarEnable		= $80000045,	-> byte
	SCP_ToolTipsEnable		= $80000046,	-> BYTE
	SCP_ShowDragDropObjCount	= $80000047,	-> BYTE
	SCP_DragDropIconsSingle		= $80000048,	-> BYTE
	SCP_TextWindowsStriped		= $80000049,	-> BYTE
	SCP_DropMarkMode		= $8000004a,	-> BYTE	enum WindowDropMarkTypes
	SCP_DragDropCopyQualifier	= $8000004b,	-> ULONG Keyboard qualifier to force D&D copy operation
	SCP_UnderSoftLinkNames		= $8000004c,	-> BYTE
	SCP_ConsoleName			= $8000004d,	-> char[256] name for CLI/ARexx console
	SCP_TransparencyIconDrag	= $8000004e,	-> UWORD percentage of transparency during D&D
	SCP_TransparencyIconShadow	= $8000004f,	-> UWORD percentage of transparency for icon shadows
	SCP_PopupApplySelectedQualifier	= $80000050,	-> ULONG Keyboard qualifier to apply popup menus to all selected icons
	SCP_ToolTipPopupDelay		= $80000051,	-> ULONG delay until tooltips pop up [s]
	SCP_DefaultStackSize		= $80000052,	-> ULONG default stack size for WB applications
	SCP_TransparencyDefaultIcon	= $80000053,	-> UWORD percentage of transparency for default icons
	SCP_EasyMultiSelect		= $80000054,	-> BYTE  select multiple icons without holding SHIFT key
	SCP_EasyMultiDrag		= $80000055,	-> BYTE  start dragging multiple selected icons without holding SHIFT key
	SCP_UseTTScreenFont		= $80000056,	-> BYTE  enable TrueType font as screen font
	SCP_UseTTIconFont		= $80000057,	-> BYTE  enable TrueType font as icon font
	SCP_UseTTTextWindowFont		= $80000058,	-> BYTE  enable TrueType font as text window font
	SCP_TTScreenFontDesc		= $80000059,	-> char[256] Screen TrueType font descriptor
	SCP_TTIconFontDesc		= $8000005a,	-> char[256] Icon TrueType font descriptor
	SCP_TTTextWindowFontDesc	= $8000005b,	-> char[256] Text window TrueType font descriptor
	SCP_DragDropMakeLinkQualifier	= $8000005c,	-> ULONG Keyboard qualifier to force D&D create link operation
	SCP_DragDropMoveQualifier	= $8000005d,	-> ULONG Keyboard qualifier to force D&D move operation
	SCP_CreateSoftLinks		= $8000005e,	-> BYTE  1=create softlinks, 0=create hardlinks
	SCP_HideProtectHiddenFiles	= $8000005f,	-> BYTE  1=hide files with FIBF_HIDDEN protection bit activated, 0=show files with FIBF_HIDDEN  protection bit activated
	SCP_TTfAntialiasing		= $80000060,	-> BYTE  Value for ttengine antialiasing (one of TT_Antialias_Auto, TT_Antialias_Off, TT_Antialias_On)
	SCP_TTfGamma			= $80000061,	-> WORD	 1000 * value for ttengine gamma correction
	SCP_CleanupOnResize		= $80000062,	-> BYTE  1=clean window contents on resize
	SCP_IconHiliteUnderMouse	= $80000063,	-> BYTE  1=Hilight icons under mouse
	SCP_AutoLeaveOut		= $80000064,	-> BYTE  1=draggins icons on desktop autmatically issues "leave out"
	SCP_WinRefreshOnMemoryChange	= $80000065,	-> byte +jmc+
	SCP_WinTitleRefresh		= $80000066,	-> byte +jmc
	SCP_ShowAllDefault		= $80000067,	-> UBYTE default "show all" display - one of DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
	SCP_ViewByDefault		= $80000068,	-> UBYTE default "view by" mode - one of DDVM_BYDEFAULT, DDVM_BYICON, DDVM_BYNAME, DDVM_BYDATE, DDVM_BYSIZE, DDVM_BYTYPE
	SCP_IconSizeConstraints		= $80000069,	-> struct Rectangle - icon size limits, larger or smaller icons are scaled to defined sizes.
	SCP_SelectTextIconName		= $8000006a,	-> UBYTE 1=text icons selectable area is limited to the "name" column
	SCP_ShowThumbnails		= $8000006b,	-> UBYTE 0=never, 1=as default icon, 2=always
	SCP_ThumbnailSize		= $8000006c,	-> UWORD desired x/y size for thumbnails
	SCP_PathsTheme			= $8000006d,	-> string
	SCP_PathsThumbnailDb		= $8000006e,	-> string
	SCP_ThumbnailMaxAge		= $8000006f,	-> UWORD maximum number of days thumbnails are cached without access
	SCP_DeviceWinIconLayout		= $80000070,	-> UBYTE[9] device window icon layout mode, icon type (e.g. WBDISK) is index
	SCP_IconWinIconLayout		= $80000071,	-> UBYTE[9] icon window icon layout mode, icon type (e.g. WBDISK) is index
	SCP_ThumbnailMinSizeLimit	= $80000072,	-> UWORD Minimum images's dimensions to cache.
	SCP_ShowThumbnailsAsDefault	= $80000073,	-> BYTE 0=Don't show thumbnails with "ShowOnlyIcons" show type
	SCP_ToolTipTransparency		= $80000074,	-> UWORD percentage of transparency for tooltips
	SCP_PopupApplySelectedAlways	= $80000075,	-> BYTE 1=always apply popup menus to all selected icons
	SCP_ThumbnailQuality		= $80000076,	-> ULONG Thumbnail quality: SCALOSPREVIEWA_Quality_Min..SCALOSPREVIEWA_Quality_Max
	SCP_ThumbnailsSquare		= $80000077	-> BYTE 1=thumbnail icons are always square

OBJECT scp_pluginentry
	pey_Priority
	pey_InstSize;
	pey_NeededVersion;
	pey_unused;
	pey_Path[1]:ARRAY OF CHAR		-> variable-length string, terminated by '\0'
->	pey_ClassName[1]:ARRAY OF CHAR		-> variable-length string, terminated by '\0'
->	pey_SuperclassName[1]:ARRAY OF CHAR	-> variable-length string, terminated by '\0'
ENDOBJECT

-> ---------------------------------------------------------------


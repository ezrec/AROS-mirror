	IFND	PREFERENCES_I
PREFERENCES_I	SET	1
**
**	$VER: preferences.i 41.7 (10 Aug 2009 19:02:08)
**
**      preferences.library include
**
**   (C) Copyright 1996-1997 ALiENDESiGN
**   (C) Copyright 2000-2009 The Scalos Team
**	All Rights Reserved
**

    STRUCTURE PrefsStruct,0
	UWORD	ps_Size
	STRUCT	ps_data,ps_Size
	LABEL	ps_SIZE


;-- Scalos specific Prefs:

;------------------ ScalosPrefs -------------------------

;Name: "Scalos" or "ScalosPrefs"
;ID: "MAIN"

SCP_IconOffsets			EQU	$80000001	;struct [8] 4 * word
SCP_IconNormFrame		EQU	$80000002	;word
SCP_IconSelFrame		EQU	$80000003	;word
SCP_IconTextMode		EQU	$80000004	;word
SCP_IconSecLine			EQU	$80000005	;word
SCP_IconTextSkip		EQU	$80000006	;word

SCP_BobsType			EQU	$80000010	;byte	image or image+text
SCP_BobsMethod			EQU	$80000011	;byte	system or custom
SCP_BobsTranspMode		EQU	$80000012	;byte	solid or transparent
SCP_BobsTranspType		EQU	$80000013	;byte	ghosted or real transparent
SCP_BobsTransp			EQU	$80000014	;long	when transparent

SCP_ScreenTitle			EQU	$80000020	;string
SCP_RootWinTitle		EQU	$80000021	;string
SCP_WindowTitle			EQU	$80000022	;string
SCP_Separator			EQU	$80000023	;byte
SCP_TitleRefresh		EQU	$80000024	;byte

SCP_PathsDefIcons		EQU	$80000025	;string
SCP_PathsDiskCopy		EQU	$80000026	;string
SCP_PathsWBStartup		EQU	$80000027	;string
SCP_PathsHome			EQU	$80000028	;string

SCP_MiscAutoRemove		EQU	$80000029	;byte
SCP_MiscClickTransp		EQU	$8000002a	;byte
SCP_MiscHardEmulation		EQU	$8000002b	;byte
SCP_MiscUseExAll		EQU	$8000002c	;byte
SCP_MiscWindowType		EQU	$8000002d	;byte
SCP_MiscDoWaitDelay		EQU	$8000002e	;byte
SCP_MiscDiskiconsRefresh	EQU	$8000002f	;byte	;delay
SCP_MiscMenuCurrentDir		EQU	$80000030	;byte

SCP_NewIconsTransparent		EQU	$80000031	;byte
SCP_NewIconsPrecision		EQU	$80000032	;long

SCP_TextModeFont		EQU	$80000033	;string
SCP_TextModeDateFormat		EQU	$80000034	;byte

SCP_PlugInList			EQU	$80000035	;list
SCP_MMB_Move			equ	$80000036	; byte
SCP_TextMode_ListColumns	equ	$80000037	; 7 bytes +jl+
SCP_RefreshOnMemoryChange	equ	$80000038	; byte +jl+
SCP_WindowPopupTitleOnly    	EQU	$80000039	; byte
SCP_DefaultWindowSize		equ	$8000003a	; 4 x word +jl+
SCP_IconCleanupSpace		equ	$8000003b	; 4 x word +jl+
SCP_FullBench		    	EQU	$8000003c	; byte
SCP_LoadDefIconsFirst	    	EQU	$8000003d	; byte
SCP_DefaultIconsSaveable    	EQU	$8000003e	; byte
SCP_MacLikeSelection	    	EQU	$8000003f	; byte
SCP_DropStart		    	EQU	$80000040	; byte
SCP_HideHiddenFiles	    	EQU	$80000041	; byte
SCP_PopScreenTitle	    	EQU	$80000042	; byte

SCP_SplashWindowEnable		EQU	$80000043	; byte
SCP_SplashWindowHoldTime	EQU	$80000044	; UWORD
SCP_StatusBarEnable		EQU	$80000045	; byte
SCP_ToolTipsEnable		EQU	$80000046	; BYTE
SCP_ShowDragDropObjCount	EQU	$80000047	; BYTE
SCP_DragDropIconsSingle		EQU	$80000048	; BYTE
SCP_TextWindowsStriped		EQU	$80000049	; BYTE
SCP_DropMarkMode		EQU	$8000004a	; BYTE	enum WindowDropMarkTypes
SCP_DragDropCopyQualifier	EQU	$8000004b	; ULONG Keyboard qualifier to force D&D copy operation
SCP_UnderSoftLinkNames		EQU	$8000004c	; BYTE
SCP_ConsoleName			EQU	$8000004d	; char[256] name for CLI/ARexx console
SCP_TransparencyIconDrag	EQU	$8000004e	; UWORD percentage of transparency during D&D
SCP_TransparencyIconShadow	EQU	$8000004f	; UWORD percentage of transparency for icon shadows
SCP_PopupApplySelectedQualifier	EQU	$80000050	; ULONG Keyboard qualifier to apply popup menus to all selected icons
SCP_ToolTipPopupDelay		EQU	$80000051	; ULONG delay until tooltips pop up [s]
SCP_DefaultStackSize		EQU	$80000052	; ULONG default stack size for WB applications
SCP_TransparencyDefaultIcon	EQU	$80000053	; UWORD percentage of transparency for default icons
SCP_DragDropMakeLinkQualifier	EQU	$8000005c	; ULONG Keyboard qualifier to force D&D create link operation
SCP_DragDropMoveQualifier	EQU	$8000005d	; ULONG Keyboard qualifier to force D&D move operation
SCP_CreateSoftLinks		EQU	$8000005e	; BYTE  1=create softlinks, 0=create hardlinks
SCP_HideProtectHiddenFiles	EQU	$8000005f	; BYTE  1=hide files with FIBF_HIDDEN protection bit activated, 0=show files with FIBF_HIDDEN  protection bit activated
SCP_TTfAntialiasing		EQU	$80000060	; BYTE  Value for ttengine antialiasing (one of TT_Antialias_Auto, TT_Antialias_Off, TT_Antialias_On)
SCP_TTfGamma			EQU	$80000061	; WORD	 1000 * value for ttengine gamma correction
SCP_CleanupOnResize		EQU	$80000062	; BYTE  1=clean window contents on resize
SCP_IconHiliteUnderMouse	EQU	$80000063	; BYTE  1=Hilight icons under mouse
SCP_AutoLeaveOut		EQU	$80000064	; BYTE  1=draggins icons on desktop autmatically issues "leave out"
SCP_WinRefreshOnMemoryChange	EQU	$80000065	; byte +jmc+
SCP_WinTitleRefresh		EQU	$80000066	; byte +jmc
SCP_ShowAllDefault		EQU	$80000067	; UBYTE default "show all" display - one of DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
SCP_ViewByDefault		EQU	$80000068	; UBYTE default "view by" mode - one of DDVM_BYDEFAULT, DDVM_BYICON, DDVM_BYNAME, DDVM_BYDATE, DDVM_BYSIZE, DDVM_BYTYPE
SCP_IconSizeConstraints		EQU	$80000069	; struct Rectangle - icon size limits, larger or smaller icons are scaled to defined sizes.
SCP_SelectTextIconName		EQU	$8000006a	; UBYTE 1=text icons selectable area is limited to the "name" column
SCP_ShowThumbnails		EQU	$8000006b	; UBYTE 0=never, 1=as default icon, 2=always
SCP_ThumbnailSize		EQU	$8000006c	; UWORD desired x/y size for thumbnails
SCP_PathsTheme			EQU	$8000006d	; string
SCP_PathsThumbnailDb		EQU	$8000006e	; string
SCP_ThumbnailMaxAge		EQU	$8000006f	; UWORD maximum number of days thumbnails are cached without access
SCP_DeviceWinIconLayout		EQU	$80000070	; UBYTE[9] device window icon layout mode, icon type (e.g. WBDISK) is index
SCP_IconWinIconLayout		EQU	$80000071	; UBYTE[9] icon window icon layout mode, icon type (e.g. WBDISK) is index
SCP_ThumbnailMinSizeLimit	EQU	$80000072	; UWORD Minimum images's dimensions to cache.
SCP_ShowThumbnailsAsDefault	EQU	$80000073	; BYTE 0=Don't show thumbnails with "ShowOnlyIcons" show type
SCP_ToolTipTransparency		EQU	$80000074	; UWORD percentage of transparency for tooltips
SCP_PopupApplySelectedAlways	EQU	$80000075	; BYTE 1=always apply popup menus to all selected icons
SCP_ThumbnailQuality		EQU	$80000076	; ULONG Thumbnail quality: SCALOSPREVIEWA_Quality_Min..SCALOSPREVIEWA_Quality_Max
SCP_ThumbnailsSquare		EQU	$80000077	; BYTE 1=thumbnail icons are always square
SCP_IconNormThumbnailFrame	EQU	$80000078	; WORD - frame type around unselected thumbnail icons
SCP_IconSelThumbnailFrame	EQU	$80000079	; WORD - frame type around selected thumbnail icons
SCP_ThumbnailOffsets           	EQU	$8000007a	; struct 4 * UWORD - left/top/right/bottom offset
SCP_ThumbnailsFillBackground	EQU	$8000007b	; UBYTE - Flag: background of square thumbnails is filled with PENIDX_THUMBNAILBACKGROUNDPEN
SCP_TransparencyThumbnailsBack 	EQU	$8000007c	; UWORD - percentage of thumbnail background transparency
SCP_SingleWindowLassoQualifier	EQU	$8000007d	; ULONG - Key qualifier to use single-window lasso operation
SCP_EnableDropMenu		EQU	$8000007e	; UBYTE - Flag: use drop menu for copy/move/create link selection
SCP_PathsImagecache		EQU	$8000007f	; string -- path to Scalos datatypes image cache
SCP_ControlBarGadgetsBr		EQU	$80000080	; array of SCP_GadgetEntry for browser mode windows
SCP_ControlBarGadgetStringsBr	EQU	$80000081	; SCP_GadgetStringEntry -- strings for SCP_ControlBarGadgets, starts with ULONG containing total length for browser mode windows
SCP_CheckOverlappingIcons	EQU	$80000082	; UBYTE - check for overlapping icons in icon windows
SCP_CopyBuffLen			EQU	$80000083	; ULONG - buffer size for file copying
SCP_SelectedTextRectangle	EQU	$80000084	; UBYTE - Flag: Draw rectangle around selected icon text
SCP_SelTextRectBorderX		EQU	$80000085	; UWORD - Additional horizontal border around selected icon text rectangle
SCP_SelTextRectBorderY		EQU	$80000086	; UWORD - Additional vertical border around selected icon text rectangle
SCP_SelTextRectRadius		EQU	$80000087	; UWORD - Radius for selected icon text rectangle corners
SCP_ActiveWindowTransparency	EQU	$80000088	; UWORD - percentage of active Scalos window opacity (0=transparent, 100=opaque)
SCP_InactiveWindowTransparency	EQU	$80000089	; UWORD - percentage of inactive Scalos window opacity (0=transparent, 100=opaque)
SCP_PopupWindowsEnable		EQU	$8000008a	; BYTE - enable "spring-loaded" popup windows during D&D
SCP_PopupWindowsDelay		EQU	$8000008b	; UWORD - Time in s before drawer window pops up during D&D
SCP_SelectMarkerBaseColor	EQU	$8000008c	; struct ARGB - base color for text window selection marker
SCP_SelectMarkerTransparency	EQU	$8000008d	; UBYTE - transparency for text window selection marker (0=transparent, 0xff=opaque)
SCP_ControlBarGadgetsStd	EQU	$8000008e	; array of SCP_GadgetEntry for normal windows
SCP_ControlBarGadgetStringsStd	EQU	$8000008f	; SCP_GadgetStringEntry -- strings for SCP_ControlBarGadgets, starts with ULONG containing total length for normal windows
SCP_MaxUndoSteps		EQU	$80000090	; ULONG - maximum number of Undo/Redo steps
SCP_PathsSQLiteTempDir          EQU	$80000091	; string -- path for SQLite3 temporary file
SCP_DrawerSortMode		EQU	$80000092	; enum ScalosDrawerSortType - where are drawers sorted in text windows (first/last/mixed)
SCP_IconNominalSize		EQU	$80000093	; UWORD -- standard icon scaling factor in percent


	ENDC	; PREFERENCES_I

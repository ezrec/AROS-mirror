// pref_s.h
// $Date$
// $Revision$

#ifndef	SCALOSPREFS_H
#define SCALOSPREFS_H

// values in ScalosPalettePrefs
// >>> !!! when doing changes here, don't forget to update PenNames.h """
enum vScalosPenIndex
	{
	PENIDX_HSHINEPEN, PENIDX_HSHADOWPEN,
	PENIDX_ICONTEXTOUTLINEPEN,
	PENIDX_DRAWERTEXT, PENIDX_DRAWERTEXTSEL,
	PENIDX_DRAWERBG,
	PENIDX_FILETEXT, PENIDX_FILETEXTSEL,
	PENIDX_FILEBG,
	PENIDX_BACKDROPDETAIL, PENIDX_BACKDROPBLOCK,
	PENIDX_TOOLTIP_TEXT, PENIDX_TOOLTIP_BG,
	PENIDX_DRAGINFOTEXT_TEXT, PENIDX_DRAGINFOTEXT_BG,
	PENIDX_STATUSBAR_BG, PENIDX_STATUSBAR_TEXT,

	PENIDX_ICONTEXTPEN,
	PENIDX_ICONTEXTPENSEL, PENIDX_ICONTEXTSHADOWPEN,

	PENIDX_THUMBNAILBACKGROUNDPEN,
	PENIDX_THUMBNAILBACKGROUNDPENSEL,

	PENIDX_ICONTEXTPENBGSEL,

	PENIDX_MAX		// max. Pen Index - 1
	// >>> !!! when doing changes here, don't forget to update PenNames.h """
	};

enum ScalosDrawerSortType
	{
	DRAWER_SORT_First,
	DRAWER_SORT_Last,
	DRAWER_SORT_Inbetween,
	};

struct ScalosFontPrefs
	{
	UBYTE fprf_FontFrontPen;				// Icon font foreground pen
	UBYTE fprf_FontBackPen;					// Icon font background pen
	UBYTE fprf_TextDrawModeSel;				// Icon font selected state draw mode (JAM1, JAM2, ...)
	UBYTE fprf_TextDrawMode;				// Icon font draw mode (JAM1, JAM2, ...)
	struct TextAttr fprf_TextAttr;				// Icon font attributes
	STRPTR fprf_AllocName;					// Allocated string for ta_Name
	};

struct ScalosPalettePrefs
	{
	ULONG *pal_ScreenColorList;				// +jl+ 20010801 screen color table in LoadRGB32() format
	LONG pal_AllocatedPens[PENIDX_MAX];
	WORD pal_PensList[PENIDX_MAX];			       	// +jl+ 20010731 Scalos pen list
	WORD pal_origPensList[PENIDX_MAX];		       	// copy of pref_PensList with the values originally read
	WORD pal_driPens[13];				       	// +jl+ 20010801 dri_Pens array
	};

struct ScalosPatternPrefs
	{
	UBYTE patt_AsyncBackFill;				// +jl+ 20011118 Layout backfill patterns asynchronously
	UBYTE patt_UseFriendBitMap;				// +jl+ 20011118 Make a copy of the backfill pattern ?
	UBYTE patt_NewRenderFlag;				// +jl+ 20011122 ReRender after resize
	UBYTE patt_RandomizeEverytime;				// +jl+ 20011130 Always randomize patterns

	WORD patt_DefWBPatternNr;				// +jl+ 20010825 Workbench backdrop pattern number
	WORD patt_DefWindowPatternNr;				// +jl+ 20010826 icon window pattern number
	WORD patt_DefTextWinPatternNr;				// +jl+ 20010826 text window pattern number
	WORD patt_DefScreenPatternNr;				// +jl+ 20010825 Screen pattern number

	BYTE patt_AsyncProcTaskPri;				// +jl+ 20011118 AsyncProc priority
	};

struct ScalosPrefs
	{
	APTR pref_Handle;					// +jl+ 20011129 PrefsHandle of the Scalos.prefs

	UBYTE pref_AutoCleanupOnResize;				// automatic icon clean on window resize
	UBYTE pref_AutoLeaveOut;				// dragging icon to desktop automatically updates .backdrop
	UBYTE pref_MarkIconUnderMouse;				// Flag: icons are highlighted while mouse pointer is over them.

	UBYTE pref_DragType;					// Drag&Drop type (image only / image+text)
	UBYTE pref_AutoRemoveFlag;				// Auto remove icons on D&D
	UBYTE pref_DragMethod;

	UWORD pref_ThumbNailWidth;				// Default width for thumbnails
	UWORD pref_ThumbNailHeight;				// Default height for thumbnails
	UWORD pref_ThumbnailMaxAge;				// Maximum number of days unaccessed thumbnails are cached
	UWORD pref_ThumbnailMinSizeLimit;			// Minimum dimensions for images to cache.

	UBYTE pref_ShowThumbnails;				// Flag: display image icons as thumbnails
	BYTE  pref_ShowThumbnailsAsDefault;			// +jmc+ Display thumbnails icons using "ShowOnlyicons" show mode for default icons.
	ULONG pref_ThumbnailQuality;				// Thumbnail generation quality
	BYTE  pref_ThumbnailsSquare;				// Flag: always generate square thumbnail icons

	struct ImageBorders pref_ThumbnailImageBorders;

	UBYTE pref_ThumbnailFillBackground;			// Flag: background of square thumbnails is filled with PENIDX_THUMBNAILBACKGROUNDPEN
	UWORD pref_ThumbnailBackgroundTransparency;		// percentage of thumbnail background transparency

	struct ARGB pref_IconHighlightK;			// RGB color shift for icon highlighting
	UBYTE pref_DragTranspMode;	     			// 0=solid/trans, 1=always solid, 2=always trans, 3=solid/trans

	ULONG pref_DragTranspObjects;

	UBYTE pref_RealTransFlag;				// +jl+ Flag: Use real transparency for bobs if available
	UBYTE pref_WindowDropMarkMode;				// +jl+ 20011208 Mode for display of window drop marks

	UBYTE pref_DefaultShowAllFiles;				// Default "show all": DDFLAGS_SHOWDEFAULT,
	UBYTE pref_DefaultViewBy; 				// IDTV_ViewModes_Default views mode

	UBYTE pref_SelectTextIconName;				// Flag: text icons selectable area is limited to the "name" column

	struct IconCleanSpace pref_CleanupSpace;        	// cleanup spaces around icons

	CONST_STRPTR pref_DefIconPath;				// +jl+ 20010619 path for default icons

	BYTE pref_TextWindowStriped;				// +jl+ 20010916 Flag: Draw stripes in text windows
	UBYTE pref_ShowDDCountText;				// +jl+ 20011205 Flag: on D&D, show number of object dragged
	UBYTE pref_DDIconsMayOverlap;				// +jl+ 20011013 Flag: no icon overlap checking when D&D
	BYTE pref_UseOldDragIcons;				// +jl+ 20010814 Flag: use old drag (all icons visible) instead of new icon stack
	BYTE pref_DisplayStatusBar;				// +jl+ 30010731 Flag: display status bar in scalos windows
	BYTE pref_EnableTooltips;				// +jl+ 20010626 Flag: enable icon tool tips
	BYTE pref_EnableSplash;					// +jl+ 20010626 Flag: enable Splash window on startup
	BYTE pref_EnablePopupWindows;				// Flag: enable "spring-loaded" popup windows during D&D
	UWORD pref_SplashHoldTime;				// +jl+ 20030705 Hold time for splash window in seconds

	BYTE pref_ColumnsArray[WIDTHARRAY_MAX];   		// +jl+ 20010311 Text window title columns

	UWORD pref_TypeRestartTime;				// +jl+ 20011005 Time in s before restarting keyboard icon selection

	UWORD pref_PopupWindowDelaySeconds;			// Time in s before drawer window pops up during D&D

	ULONG pref_FileCountTimeout;				// +jl+ 20011214 Timeout for file transfer counting in ms
	ULONG pref_FileTransWindowDelay;			// +jl+ 20011215 Open delay for file transfer window in ms

	BYTE pref_ConsoleName[256];

	CONST_STRPTR pref_MainWindowTitle;			// +jl+ 20010707 title string for device windows
	CONST_STRPTR pref_StandardWindowTitle;			// +jl+ 20010707 title string for drawer windows

	CONST_STRPTR pref_WBStartupDir;				// +jl+ 20010914 Path to WBStartup dir
	CONST_STRPTR pref_ScalosHomeDir;			// +jl+ 20011129
	CONST_STRPTR pref_DefDiskCopy;				// +jl+ 20010217 path to DiskCopy
	CONST_STRPTR pref_ThumbDatabasePath;			// Path of thumbnail cache database file
	CONST_STRPTR pref_ThemesDir;				// default path for THEME:
	CONST_STRPTR pref_ImageCacheDir;			// default path for datatypes image cache
	CONST_STRPTR pref_SQLiteTempDir;			// path for SQLite3 temporary files

	UBYTE pref_GroupSeparator;				// +jl+ 20010829 separates groups of digits, see locale.h
	UBYTE pref_TitleRefreshSec;				// +jl+ 20010831 number of seconds for Title-refresh
	BYTE pref_RefreshOnMemChg;				// +jl+ 20010831 Refresh title only on memchange

	UBYTE pref_WinTitleRefreshSec;				// +jmc+ 20050327 number of seconds for windows Title-refresh
	BYTE pref_WinRefreshOnMemChg;				// +jmc+ 20050327 Refresh windows title only on memchange

	UBYTE pref_CheckOverlappingIcons;			// Flag: always check for overlapping icons, and reposition conflicting icons.
								// this is the global default, can be overridden for each Scalos window

	UBYTE pref_TextMode;					// +jl+ Normal/Outline/Shadow
	BYTE pref_MenuCurrentDir;				// +jl+ 20010603 New Currentdir on all windows
	UBYTE pref_SaveDefIcons;				// +jl+ 20010701 Deficons are saveable ?
	UBYTE pref_DefIconsFirst;				// +jl+ 20010619 Flag: try Def-Diskicons first
	UBYTE pref_ClickTransp;					// +jl+ 20010530 Click on Images only ?
	UBYTE pref_SmartRefresh;				// +jl+ 20010831 1=all windows smart refresh
	UBYTE pref_UseExAll;					// +jl+ Flag: Use ExAll for directory scanning
	UBYTE pref_CreateSoftLinks;				// Flag: D&D Create Soft Links
	UBYTE pref_DiskRefresh;					// +jl+ 20010925 number of seconds for Diskicon-refresh
	UBYTE pref_DoWaitDelay;					// +jl+ 20011001 number of seconds to wait
	UBYTE pref_MMBMoveFlag;					// +jl+ 20010312 Flag: MMB mouse move (2 = inverse move) ??
	UBYTE pref_FullPopupFlag;				// +jl+ 20010303 Flag: WindowPopup not just on windowtitle
	UBYTE pref_HideHiddenFlag;				// +jl+ Hide hidden files

	UBYTE pref_HideProtectHiddenFlag;			// +jmc+ Protection bit=hidden files

	UBYTE pref_DragStartFlag;
	UBYTE pref_EasyMultiselectFlag;				// +jl+ 20011123 Multiselection without SHIFT key.
	UBYTE pref_EasyMultiDragFlag;				// +jl+ dragging of multiple selected icons without SHIFT key.
	UBYTE pref_DisableCloseWorkbench;			// +jl+ 20010623 Flag: disable CloseWorkbench()
	UBYTE pref_PopTitleFlag;				// +jl+ 20011129 Install the Commodity
	UBYTE pref_FullBenchFlag;				// +jl+ 20010709 Flag: FullBench
	UBYTE pref_HardEmulation;				// +jl+ 20011129 Flag: Use private WB patches
	UBYTE pref_WBBackdrop;					// +jl+ 20011127 Flag: Mainwindow in backdrop mode
	UBYTE pref_IconTextMuliLine;				// +jl+ 20040520 Flag: Allow splitting of icon labels into multiple lines
	UBYTE pref_EnableDropMenu;				// +jl+ Flag: use drop menu for copy/move/create link selection
	UBYTE pref_SelectedTextRectangle;			// Flag: Draw rectangle around selected icon text

	UWORD pref_FrameType;					// +jl+
	UWORD pref_FrameTypeSel;				// +jl+
	UWORD pref_ThumbnailFrameType;				// +jl+
	UWORD pref_ThumbnailFrameTypeSel;			// +jl+
	struct ImageBorders pref_ImageBorders;
	UWORD pref_TextSkip;					// +jl+

	UWORD pref_SelTextRectBorderX;                          // Additional horizontal border around selected icon text rectangle
	UWORD pref_SelTextRectBorderY;                          // Additional vertical border around selected icon text rectangle
	UWORD pref_SelTextRectRadius;                           // Radius for selected icon text rectangle corners

	UWORD pref_DefaultLinePattern;				// +jl+ 20010809 line pattern for lasso
	UWORD pref_DefLineSpeed;

	LONG pref_WindowHScroll;				// +dm+ 20011106
	LONG pref_WindowVScroll;				// +dm+ 20011106

	struct IBox pref_DefWindowBox;				// +jl+ 20010707 default box for new Scalos windows
	struct IBox pref_WBWindowBox;				// +jl+ 20010831 Main window IBox

	WORD pref_WBWindowXOffset;				// +jl+ 20010831 Main window x offset
	WORD pref_WBWindowYOffset;				// +jl+ 20010831 Main window y offset

	ULONG pref_CopyQualifier;				// +jl+ 20030709 Key qualifier to force D&D copy operation
	ULONG pref_PopupApplySelectedQualifier;			// +jl+ 20030709 Key qualifier to apply popup menus to all selected icons
	ULONG pref_MakeLinkQualifier;				// Key qualifier to force D&D to create links
	ULONG pref_ForceMoveQualifier;				// Key qualifier to force D&D move operation
	ULONG pref_SingleWindowLassoQualifier; 	      		// Key qualifier to use single-window lasso operation
	BYTE pref_PopupApplySelectedAlways;			// Flag: popup menus always apply to every selected icon

	T_TIMEVAL pref_ClickDelay;				// +jl+ 20011123

	ULONG pref_SupportedIconTypes;                   	// Flags - decide if NewIcons, color icons are supported

	ULONG pref_LinkTextStyle;				// icon text style for links

	ULONG pref_ToolTipDelaySeconds;				// time in seconds until icon tool tips will show up

	ULONG pref_DefaultStackSize;				// default stack size

	ULONG pref_WindowTitleSize; 				// +jl+ 20020210 size of window title buffer
	ULONG pref_ScreenTitleSize;				// +jl+ 20020210 size of screen title buffer

	CONST_STRPTR pref_ScreenTitle;				// +jl+ 20011129 screen title string

	UWORD pref_DragTransparency;				// percentage of transparency during D&D [0..100%]
	UWORD pref_IconShadowTransparency;			// percentage of transparency for icon shadows [0..100%]
	UWORD pref_DefaultIconTransparency;			// percentage of transparency default icons [0..100%]
	UWORD pref_TooltipsTransparency;			// percentage of transparency for Tooltips [0..100%]

	ULONG pref_iCandy;

	UBYTE pref_TTFontAntialias;				// Antialiasing for TrueType fonts
	WORD pref_TTFontGamma;					// 1000 * gamma correction for TrueType fonts
	char pref_ScreenTTFontDescriptor[256];			// TrueType font descriptor for screen font
	char pref_IconTTFontDescriptor[256];			// TrueType font descriptor for icon font
	char pref_TextWindowTTFontDescriptor[256];		// TrueType font descriptor for text window font
	BYTE pref_UseScreenTTFont;				// Flag: Use TrueType screen font
	BYTE pref_UseIconTTFont;				// Flag: Use TrueType icon font
	BYTE pref_UseTextWindowTTFont;				// Flag: Use TrueType text window font

	ULONG pref_CopyBuffLen;					// size of file copy buffer

	struct Rectangle pref_IconSizeConstraints;		// size limits for icons, larger or smaller icons are scaled
	UWORD pref_IconScaleFactor;				// standard icon scaling factor in percent

	UBYTE pref_DeviceWindowLayoutModes[ICONTYPE_MAX];	// Icon layout modes for desktop window
	UBYTE pref_IconWindowLayoutModes[ICONTYPE_MAX];		// Icon layout modes for icon window

	struct ScalosTextAttr pref_TextModeTextAttr;    	// +jl+ 20011129 TextAttr for text window font
	struct ScalosTextAttr pref_IconFontAttr;        	// +jl+ 20010826 Icon font TextAttr

	UWORD pref_ActiveWindowTransparency;			// percentage of active Scalos window transparency
	UWORD pref_InactiveWindowTransparency;			// percentage of inactive Scalos window transparency

	struct ARGB pref_SelectMarkerBaseColor;			// base color for text window selection marker
	UBYTE pref_SelectMarkerTransparency;			// transparency for text window selection marker (0..255)

	ULONG pref_MaxUndoSteps;				// maximum number of Undo/Redo steps

	enum ScalosDrawerSortType pref_DrawerSortMode;		// where are drawers sorted in text windows (first/last/mixed)
	};

#endif /* SCALOSPREFS_H */


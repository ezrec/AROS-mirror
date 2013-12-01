// Prefs.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <libraries/pm.h>
#include <libraries/iffparse.h>
#include <libraries/ttengine.h>
#include <libraries/mcpgfx.h>
#include <prefs/font.h>
#include <prefs/prefhdr.h>
#include <prefs/pointer.h>
#include <prefs/workbench.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#ifdef __AROS__
#include <proto/popupmenu.h>
#else
#include <proto/pm.h>
#endif
#include <proto/iffparse.h>
#include <proto/preferences.h>
#include <proto/datatypes.h>
#include <proto/guigfx.h>
#include <proto/gadtools.h>
#include <proto/diskfont.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/palette.h>
#include <scalos/pattern.h>
#include <scalos/scalos.h>
#include <scalos/scalosprefs.h>
#include <scalos/GadgetBar.h>
#include <scalos/scalospreviewplugin.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"

#include "IconImageClass.h"

//----------------------------------------------------------------------------

// local data structures

#define	INITIAL_CRC	((ULONG) ~0)

#define	ID_MAIN		MAKE_ID('M','A','I','N')
#define	PS_DATA(prefsstruct)    ((STRPTR) (prefsstruct)) + sizeof((prefsstruct)->ps_Size)

struct WBCFChunk
	{
	ULONG	wbcf_Version;		// 1
	WORD	wbcf_XOffset;
	WORD	wbcf_YOffset;
	WORD	wbcf_LeftEdge;
	WORD	wbcf_TopEdge;
	WORD	wbcf_Width;
	WORD	wbcf_Height;
	ULONG	wbcf_Flags;
	};

#define	WBCF_FLAGF_Backdrop	0x00010000

//----------------------------------------------------------------------------

// local data items

static const char DefaultMainWindowTitle[] = "%pa %d( %dp%% full, %DF free, %DU in use %d)";
static const char DefaultWindowTitle[] = "%pa";
static const char DefaultScreenTitle[] = "Scalos V%wb %fc graphics mem %ff other mem";

static BYTE UnderlineSoftLinkNames = TRUE;

static ULONG PalettePrefsCRC = 0;
static ULONG PatternPrefsCRC = 0;
static ULONG FontPrefsCRC = 0;
static ULONG MainPrefsCRC = 0;
   
//----------------------------------------------------------------------------

// public data items

struct Hook *globalCopyHook = NULL;
struct Hook *globalDeleteHook = NULL;
struct Hook *globalTextInputHook = NULL;

struct List globalCloseWBHookList;				// Hooks in this list will be called on AppSleep/AppWakeup

struct List ControlBarGadgetListNormal;	 // List of control bar gadgets for standard windows
struct List ControlBarGadgetListBrowser; // List of control bar gadgets for browser-mode windows

SCALOSSEMAPHORE	CopyHookSemaphore;
SCALOSSEMAPHORE	DeleteHookSemaphore;
SCALOSSEMAPHORE	TextInputHookSemaphore;
SCALOSSEMAPHORE	CloseWBHookListSemaphore;

struct PatternNode *PatternNodes = NULL;			// +jl+ 20010826

static const struct ScalosFontPrefs DefaultFontPrefs =
	{
	1,				// fprf_FontFrontPen
	0,				// fprf_FontBackPen
	JAM1,				// fprf_TextDrawModeSel
	JAM1,				// fprf_TextDrawMode
	{ "topaz.font", 8, FS_NORMAL, FPF_ROMFONT }, // fprf_TextAttr
	NULL,				// fprf_AllocName
	};

static const struct ScalosPalettePrefs DefaultPalettePrefs =
	{
	NULL,				// pal_ScreenColorList
	{ 0 },				// pal_AllocatedPens
	{
		0, 0, 			// PENIDX_HSHINEPEN, PENIDX_HSHADOWPEN
		2,			// PENIDX_ICONTEXTOUTLINEPEN
		3, 2, 			// PENIDX_DRAWERTEXT, PENIDX_DRAWERTEXTSEL
		3,			// PENIDX_DRAWERBG
		1, 2, 			// PENIDX_FILETEXT, PENIDX_FILETEXTSEL
		1,			// PENIDX_FILEBG
		0, 0,			// PENIDX_BACKDROPDETAIL, PENIDX_BACKDROPBLOCK
		1, 2,			// PENIDX_TOOLTIP_TEXT, PENIDX_TOOLTIP_BG
		1, 2,			// PENIDX_DRAGINFOTEXT_TEXT, PENIDX_DRAGINFOTEXT_BG
		2, 1,			// PENIDX_STATUSBAR_BG, PENIDX_STATUSBAR_TEXT
		1,			// PENIDX_ICONTEXTPEN,
		2, 0,			// PENIDX_ICONTEXTPENSEL, PENIDX_ICONTEXTSHADOWPEN
		0,			// PENIDX_THUMBNAILBACKGROUNDPEN
		0,			// PENIDX_THUMBNAILBACKGROUNDPENSEL
		3,			// PENIDX_ICONTEXTPENBGSEL
	},					// pal_PensList
	{ 0 },					// pal_origPensList
	{ -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1 },	// pal_driPens
	};

static const struct ScalosPrefs DefaultPrefs =
	{
	NULL,					// pref_Handle

	FALSE,					// pref_AutoCleanupOnResize
	FALSE,					// pref_AutoLeaveOut
	FALSE,					// pref_MarkIconUnderMouse

	DRAGTYPE_ImageAndText,			// pref_DragType
	TRUE,					// pref_AutoRemoveFlag
	DRAGMETHOD_Custom,			// pref_DragMethod

	64,					// pref_ThumbNailWidth
	64,					// pref_ThumbNailHeight
	14,					// pref_ThumbnailMaxAge
	128,					// pref_ThumbnailMinSizeLimit
	THUMBNAILS_Never,			// pref_ShowThumbnails
	TRUE,					// pref_ShowThumbnailsAsDefault
	SCALOSPREVIEWA_Quality_Max,		// pref_ThumbnailQuality
	FALSE,					// pref_ThumbnailsSquare

	{ 2, 2, 2, 2 },				// pref_ThumbnailImageBorders
	FALSE,					// pref_ThumbnailFillBackground
	50,					// pref_ThumbnailBackgroundTransparency

	{ 0xff, 0x22, 0x22, 0x22 },		// pref_IconHighlightK
	DRAGTRANSPMODE_SolidAndTransp,		// pref_DragTranspMode

	DRAGTRANSPF_DiskIcons | DRAGTRANSPB_DrawerIcons | DRAGTRANSPB_TrashcanIcons, // pref_DragTranspObjects

	TRUE,					// pref_RealTransFlag
	IDTV_WinDropMark_WindowedOnly,		// pref_WindowDropMarkMode

	FALSE,					// pref_DefaultShowAllFiles
	IDTV_ViewModes_Icon,			// pref_DefaultViewBy

	FALSE,					// pref_SelectTextIconName

	{ 5, 4, 5, 3 },				// pref_CleanupSpace

#ifdef __AROS__
	"ENVARC:sys",				// pref_DefIconPath
#else
	"ENV:sys",				// pref_DefIconPath
#endif

	FALSE,					// pref_TextWindowStriped
	TRUE,					// pref_ShowDDCountText
	TRUE,					// pref_DDIconsMayOverlap
	FALSE,					// pref_UseOldDragIcons
	TRUE,					// pref_DisplayStatusBar
	TRUE,					// pref_EnableTooltips
	TRUE,					// pref_EnableSplash
	TRUE,					// pref_EnablePopupWindows
	5,					// pref_SplashHoldTime

	{ 0, 1, 2, 3, 4, 5, -1 },		// pref_ColumnsArray

	3,					// pref_TypeRestartTime

	3,					// pref_PopupWindowDelaySeconds

	60000,					// pref_FileCountTimeout (ms)
	1000,					// pref_FileTransWindowDelay

	"CON:0/11//150/Scalos Output Window/AUTO/WAIT",	// pref_ConsoleName

	DefaultMainWindowTitle,			// pref_MainWindowTitle
	DefaultWindowTitle,			// pref_StandardWindowTitle

	"SYS:WBStartup",			// pref_WBStartupDir
	"Scalos:",				// pref_ScalosHomeDir
	"Sys:System/DiskCopy",			// pref_DefDiskCopy
	"Scalos:Thumbnails.db",			// pref_ThumbDatabasePath
	"Scalos:Themes",			// pref_ThemesDir
	"t:",					// pref_ImageCacheDir
	"t:",					// pref_SQLiteTempDir

	'.',					// GroupSeparator
	5,					// pref_TitleRefreshSec
	FALSE,					// pref_RefreshOnMemChg

	5,					// pref_WinTitleRefreshSec
	FALSE,					// pref_WinRefreshOnMemChg

#if defined(__MORPHOS__)
	TRUE,					// pref_CheckOverlappingIcons
#else /* __MORPHOS__ */
	FALSE,					// pref_CheckOverlappingIcons
#endif /* __MORPHOS__ */

	IDTV_TextMode_Normal,			// pref_TextMode
	FALSE,					// pref_MenuCurrentDir
	TRUE,					// pref_SaveDefIcons
	TRUE,					// pref_DefIconsFirst
	TRUE,					// pref_ClickTransp
	FALSE,					// pref_SmartRefresh
	TRUE,					// pref_UseExAll
	FALSE,					// pref_CreateSoftLinks
	5,					// pref_DiskRefresh
	5,					// pref_DoWaitDelay
	FALSE,					// pref_MMBMoveFlag
	FALSE,					// pref_FullPopupFlag
	FALSE,					// pref_HideHiddenFlag

	FALSE,					// pref_HideProtectHiddenFlag

	FALSE,					// pref_DragStartFlag
	FALSE,					// pref_EasyMultiselectFlag
	FALSE,					// pref_EasyMultiDragFlag
	FALSE,					// pref_DisableCloseWorkbench
	FALSE,					// pref_PopTitleFlag
	FALSE,					// pref_FullBenchFlag
	TRUE,					// pref_HardEmulation
	TRUE,					// pref_WBBackdrop
	FALSE,					// pref_IconTextMuliLine
	FALSE,					// pref_EnableDropMenu
	FALSE,					// pref_SelectedTextRectangle

	MF_FRAME_NONE,				// pref_FrameType
	MF_FRAME_NONE | 0x8000,			// pref_FrameTypeSel
	MF_FRAME_NONE,				// pref_ThumbnailFrameType
	MF_FRAME_NONE | 0x8000,			// pref_ThumbnailFrameTypeSel
	{ 2, 2, 2, 2 },				// pref_ImageBorders
	1,					// pref_TextSkip

	4,					// pref_SelTextRectBorderX
	2,					// pref_SelTextRectBorderY
	5,					// pref_SelTextRectRadius

	0xff00,					// pref_DefaultLinePattern
	3,					// pref_DefLineSpeed

	20,					// pref_WindowHScroll
	20,					// pref_WindowVScroll

	{ 100, 100, 200, 100 },			// pref_DefWindowBox
	{ 0, 0, 200, 100 },			// pref_WBWindowBox

	0,					// pref_WBWindowXOffset
	0,					// pref_WBWindowYOffset

	IEQUALIFIER_CONTROL,			// pref_CopyQualifier
	IEQUALIFIER_LSHIFT,			// pref_PopupApplySelectedQualifier
	IEQUALIFIER_LALT,			// pref_MakeLinkQualifier
	IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT,	// pref_ForceMoveQualifier
	IEQUALIFIER_CONTROL,			// pref_SingleWindowLassoQualifier
	FALSE,					// pref_PopupApplySelectedAlways

	{ 0, 200000 },                          // pref_ClickDelay

	IDTV_IconType_NewIcon | IDTV_IconType_ColorIcon,	// pref_SupportedIconTypes

	FSF_UNDERLINED,				// pref_LinkTextStyle

	2,					// pref_ToolTipDelaySeconds

	16384,					// pref_DefaultStackSize

	600,					// pref_WindowTitleSize
	1000,					// pref_ScreenTitleSize

	DefaultScreenTitle,			// pref_ScreenTitle

	50,					// pref_DragTransparency
	25,					// pref_IconShadowTransparency
	70,					// pref_DefaultIconTransparency
	PREFS_TRANSPARENCY_OPAQUE,	        // pref_TooltipsTransparency

	2,					// pref_iCandy

	TT_Antialias_Auto,			// pref_TTFontAntialias
	2500,					// pref_TTFontGamma
	"0/400/11/Arial",			// pref_ScreenTTFontDescriptor
	"0/400/11/Arial",			// pref_IconTTFontDescriptor
	"0/400/11/Arial",			// pref_TextWindowTTFontDescriptor

	FALSE,					// pref_UseScreenTTFont
	FALSE,					// pref_UseIconTTFont
	FALSE,					// pref_UseTextWindowTTFont

	262144,					// pref_CopyBuffLen

	{ 0, 0, SHRT_MAX, SHRT_MAX },		// pref_IconSizeConstraints

	100,					// pref_IconScaleFactor

	{
		ICONLAYOUT_Columns,
		ICONLAYOUT_Columns,	//WBDISK
		ICONLAYOUT_Columns,	//WBDRAWER
		ICONLAYOUT_Rows,	//WBTOOL
		ICONLAYOUT_Rows,     	//WBPROJECT
		ICONLAYOUT_Columns,	//WBGARBAGE
		ICONLAYOUT_Columns,     //WBDEVICE
		ICONLAYOUT_Columns,	//WBKICK
		ICONLAYOUT_Rows,	//WBAPPICON
	},					// pref_DeviceWindowLayoutModes
	{
		ICONLAYOUT_Columns,
		ICONLAYOUT_Columns,     //WBDISK
		ICONLAYOUT_Columns,	//WBDRAWER
		ICONLAYOUT_Columns,	//WBTOOL
		ICONLAYOUT_Columns,	//WBPROJECT
		ICONLAYOUT_Columns,	//WBGARBAGE
		ICONLAYOUT_Columns,	//WBDEVICE
		ICONLAYOUT_Columns,	//WBKICK
		ICONLAYOUT_Columns,	//WBAPPICON
	},					// pref_IconWindowLayoutModes

	{
	{ "topaz.font", 8, FS_NORMAL, FPF_ROMFONT },
	NULL
	},					// pref_TextModeTextAttr

	{
	{ "topaz.font", 8, FS_NORMAL, FPF_ROMFONT },
	NULL
	},					// pref_IconFontAttr

	PREFS_TRANSPARENCY_OPAQUE,		// pref_ActiveWindowTransparency
	PREFS_TRANSPARENCY_OPAQUE,		// pref_InactiveWindowTransparency

	{ (UBYTE) ~0, 133, 195, 221 },		// pref_SelectMarkerBaseColor
	128,					// pref_SelectMarkerTransparency
	10,					// pref_MaxUndoSteps
	DRAWER_SORT_First,			// pref_DrawerSortMode
	};

static const struct ScalosPatternPrefs DefaultPatternPrefs =
	{
	TRUE,					// pref_AsyncBackFill
	FALSE,					// pref_UseFriendBitMap
	FALSE,					// pref_NewRenderFlag
	TRUE,					// pref_RandomizeEverytime

	1,					// pref_DefWBPatternNr
	3,					// pref_DefWindowPatternNr
	0,					// pref_DefTextWinPatternNr
	2,					// pref_DefScreenPatternNr

	-3,					// AsyncProcTaskPri
	};

struct ScalosPrefs CurrentPrefs;
struct ScalosPalettePrefs PalettePrefs;
struct ScalosPatternPrefs PatternPrefs;
struct ScalosFontPrefs FontPrefs;

//----------------------------------------------------------------------------

// local functions

static CONST_STRPTR GetPrefsConfigString(APTR prefsHandle, ULONG Id, CONST_STRPTR DefaultString);
static void InternalFreeScalosPrefs(struct ScalosPrefs *Prefs);
static void CreatePatternNode(struct IFFHandle *iff, const struct ContextNode *cn);
static void ReadPatternDefaults(struct IFFHandle *iff);
static SAVEDS(LONG) PatternNodeCompareFunc(struct Hook *theHook, struct PatternNode *pNode2,
	struct PatternNode *pNode1);
static void FreePatternNode(struct PatternNode **PatternNodeList, struct PatternNode *pNode);
static ULONG NewPaletteProc(APTR arg, struct SM_RunProcess *msg);
static void ShowScreenTitle(BOOL showTitle);
static void SetTextAttr(struct ScalosTextAttr *Attr, struct PrefsStruct *ps);
static void CleanupControlBarGadgetsList(struct List *CbGadgetsList);
static void ReadControlBarGadgetList(APTR p_MyPrefsHandle, LONG lID, struct List *CbGadgetsList,
	ULONG prefsIDGadgets, ULONG prefsIDStrings);

//----------------------------------------------------------------------------

ULONG ReadPalettePrefs(void)
{
	LONG n, Result;
	struct IFFHandle *iff;
	BOOL iffOpened = FALSE;

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			{
			Result = IoErr();
			break;
			}

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR) Open("ENV:Scalos/Palette13.prefs", MODE_OLDFILE);
		if (0 == iff->iff_Stream)
			iff->iff_Stream = (IPTR) Open("ENV:Scalos/Palette.prefs", MODE_OLDFILE);

		PalettePrefsCRC = GetPrefsCRCFromFH((BPTR) iff->iff_Stream);
		d1(KPrintF("%s/%s/%ld: PalettePrefsCRC=%08lx\n", __FILE__, __FUNC__, __LINE__, PalettePrefsCRC));

		if (0 == iff->iff_Stream)
			{
			Result = IoErr();
			break;
			}

		FreePalettePrefs();

		PalettePrefs.pal_driPens[0] = 0;
		PalettePrefs.pal_driPens[1] = 1;

		for (n=2; n<sizeof(PalettePrefs.pal_driPens)/sizeof(PalettePrefs.pal_driPens[0]); n++)
			{
			PalettePrefs.pal_driPens[n] = ~0;
			}

		Result = OpenIFF(iff, IFFF_READ);
		if (RETURN_OK != Result)
			break;

		iffOpened = TRUE;

		Result = StopChunk(iff, ID_PREF, ID_SPAL);
		if (RETURN_OK != Result)
			break;

		Result = StopChunk(iff, ID_PREF, ID_PENS);
		if (RETURN_OK != Result)
			break;

		while (RETURN_OK == Result)
			{
			struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_SCAN);
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);
			if (NULL == cn)
				continue;

			if (ID_PENS == cn->cn_ID)
				{
				LONG Actual;
				WORD Pen, PenCount;

				Actual = ReadChunkBytes(iff, &PenCount, sizeof(PenCount));
				if (Actual < 0)
					break;

				PenCount = SCA_BE2WORD(PenCount);

				d1(KPrintF("%s/%s/%ld: PenCount=%ld\n", __FILE__, __FUNC__, __LINE__, PenCount));

				if (PenCount < 2)
					break;

				for (Pen=0; Pen<PenCount; Pen++)
					{
					struct ScalosPen sPen;

					if (ReadChunkBytes(iff, &sPen, sizeof(sPen)) >= 0)
						{
						WORD Index = sPen.sp_pentype = SCA_BE2WORD(sPen.sp_pentype);
						sPen.sp_pen = SCA_BE2WORD(sPen.sp_pen);

						d1(KPrintF("%s/%s/%ld: Pen=%ld  Index=%ld  PenNr=%ld\n", __FILE__, __FUNC__, __LINE__, Pen, Index, sPen.sp_pen));

						if (Index >= 0)
							{
							// leave room for trailing "-1" in driPens
							if (Index < sizeof(PalettePrefs.pal_driPens)-2)
								{
								PalettePrefs.pal_driPens[Index] = sPen.sp_pen;
								}
							}
						else
							{
							Index = ~Index;		// -1 ==> 0 !!!

							if (Index < PENIDX_MAX)
								{
								PalettePrefs.pal_origPensList[Index] = PalettePrefs.pal_PensList[Index] = sPen.sp_pen;
								d1(KPrintF("%s/%s/%ld: PalettePrefs.pal_PensList[%ld]=%ld\n", __FILE__, __FUNC__, __LINE__, \
									Index, PalettePrefs.pal_PensList[Index]));
								}
							}
						}
					}
				}
			else if (ID_SPAL == cn->cn_ID)
				{
				LONG Actual;

				PalettePrefs.pal_ScreenColorList = ScalosAlloc(cn->cn_Size + 16);
				d1(KPrintF("%s/%s/%ld: ScreenColorList=%08lx\n", __FILE__, __FUNC__, __LINE__, PalettePrefs.pal_ScreenColorList));
				if (NULL == PalettePrefs.pal_ScreenColorList)
					continue;

				Actual = ReadChunkBytes(iff, PalettePrefs.pal_ScreenColorList, cn->cn_Size);

				d1(KPrintF("%s/%s/%ld: Read Palette Size=%ld  Actual=%ld\n", __FILE__, __FUNC__, __LINE__, cn->cn_Size, Actual));
				if (Actual < 0)
					{
					Result = IoErr();

					ScalosFree(PalettePrefs.pal_ScreenColorList);
					PalettePrefs.pal_ScreenColorList = NULL;
					break;
					}
				}
			}
		} while (0);

	if (iff)
		{
		if (iffOpened)
			CloseIFF(iff);

		if (iff->iff_Stream)
			Close((BPTR) iff->iff_Stream);

		FreeIFF(iff);
		}

	if (NULL == PalettePrefs.pal_ScreenColorList)
		{
		// couldn't read palette prefs
		PalettePrefs.pal_driPens[0] = ~0;
		}

	return (ULONG) Result;
}


void FreePalettePrefs(void)
{
	d1(KPrintF("%s/%s/%ld: ScreenColorList=%08lx\n", __FILE__, __FUNC__, __LINE__, PalettePrefs.pal_ScreenColorList));

	UnlockScalosPens();

	if (PalettePrefs.pal_ScreenColorList)
		{
		ScalosFree(PalettePrefs.pal_ScreenColorList);
		PalettePrefs.pal_ScreenColorList = NULL;
		}
}


void ReadFontPrefs(void)
{
	struct IFFHandle *iff;
	struct FontPrefs *FontChunk = NULL;
	BOOL IffOpened = FALSE;

	FreeFontPrefs();
	FontPrefs = DefaultFontPrefs;

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			break;

		InitIFFasDOS(iff);

		iff->iff_Stream = (ULONG) Open("ENV:sys/font.prefs", MODE_OLDFILE);

		d1(KPrintF("%s/%s/%ld: iff_Stream=%08lx\n", __FILE__, __FUNC__, __LINE__, iff->iff_Stream));
		if (0 == iff->iff_Stream)
			break;

		FontPrefsCRC = GetPrefsCRCFromFH((BPTR) iff->iff_Stream);
		d1(KPrintF("%s/%s/%ld: FontPrefsCRC=%08lx\n", __FILE__, __FUNC__, __LINE__, FontPrefsCRC));

		if (RETURN_OK != OpenIFF(iff, IFFF_READ))
			break;

		d1(KPrintF("%s/%s/%ld: OpenIFF() OK\n", __FILE__, __FUNC__, __LINE__));
		IffOpened = TRUE;

		if (RETURN_OK != StopChunk(iff, ID_PREF, ID_FONT))
			break;

		d1(KPrintF("%s/%s/%ld: StopChunk() OK\n", __FILE__, __FUNC__, __LINE__));

		while (1)
			{
			// We should read three successive FontChunk's, with different fp_Type:
			// FP_WBFONT = Workbench icon font
			// FP_SYSFONT = System default font
			// FP_SCREENFONT = Screen font
			struct ContextNode *cn;

			if (RETURN_OK != ParseIFF(iff, IFFPARSE_SCAN))
				break;

			d1(KPrintF("%s/%s/%ld: ParseIFF() OK\n", __FILE__, __FUNC__, __LINE__));

			cn = CurrentChunk(iff);

			d1(KPrintF("%s/%s/%ld: CurrentChunk=%08lx\n", __FILE__, __FUNC__, __LINE__, cn));
			if (NULL == cn)
				break;

			d1(KPrintF("%s/%s/%ld: CurrentChunk=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, cn, cn->cn_Size));

			FontChunk = ScalosAlloc(cn->cn_Size);
			d1(KPrintF("%s/%s/%ld: FontChunk=%08lx\n", __FILE__, __FUNC__, __LINE__, FontChunk));
			if (NULL == FontChunk)
				break;

			if (ReadChunkBytes(iff, FontChunk, cn->cn_Size) < 0)
				break;

			d1(KPrintF("%s/%s/%ld: ReadChunkBytes OK\n", __FILE__, __FUNC__, __LINE__));

			FontChunk->fp_Type = SCA_BE2WORD(FontChunk->fp_Type);
			FontChunk->fp_TextAttr.ta_YSize = SCA_BE2WORD(FontChunk->fp_TextAttr.ta_YSize);

			if (FP_WBFONT == FontChunk->fp_Type)
				{
				FontPrefs.fprf_FontFrontPen = FontChunk->fp_FrontPen;
				FontPrefs.fprf_FontBackPen = FontChunk->fp_BackPen;
				FontPrefs.fprf_TextDrawModeSel = FontPrefs.fprf_TextDrawMode = FontChunk->fp_DrawMode;

				d1(KPrintF("%s/%s/%ld: FrontPen=%ld  BackPen=%ld  DrawMode=%ld\n", \
					__FILE__, __FUNC__, __LINE__, FontChunk->fp_FrontPen, FontChunk->fp_BackPen, FontChunk->fp_DrawMode));

				if (FontPrefs.fprf_AllocName)
					ScalosFree(FontPrefs.fprf_AllocName);

				FontPrefs.fprf_TextAttr = FontChunk->fp_TextAttr;
				FontPrefs.fprf_AllocName = ScalosAlloc(1 + strlen(FontChunk->fp_Name));
				if (FontPrefs.fprf_AllocName)
					{
					strcpy(FontPrefs.fprf_AllocName, FontChunk->fp_Name);
					FontPrefs.fprf_TextAttr.ta_Name = FontPrefs.fprf_AllocName;
					}
				else
					{
					FontPrefs.fprf_TextAttr.ta_Name = (STRPTR) "topaz.font";
					}

				d1(KPrintF("%s/%s/%ld: fp_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, FontChunk->fp_Name));
				d1(KPrintF("%s/%s/%ld: iconfont=<%s>  YSize=%ld\n", __FILE__, __FUNC__, __LINE__, \
					FontPrefs.fprf_TextAttr.ta_Name, FontPrefs.fprf_TextAttr.ta_YSize));
				}

			d1(KPrintF("%s/%s/%ld: FontAttr.ta_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, FontPrefs.fprf_TextAttr.ta_Name));

			ScalosFree(FontChunk);
			FontChunk = NULL;
			}
		} while (0);

	if (FontChunk)
		ScalosFree(FontChunk);
	if (iff)
		{
		if (IffOpened)
			CloseIFF(iff);
		if (iff->iff_Stream)
			Close((BPTR) iff->iff_Stream);
		FreeIFF(iff);
		}
}


void FreeFontPrefs(void)
{
}


LONG WriteWBConfig(void)
{
	struct IFFHandle *iff;
	struct PrefHeader prhd;
	struct WBCFChunk wbcf;
	BOOL IffOpen = FALSE;
	LONG Result = RETURN_OK;
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

	memset(&prhd, 0, sizeof(prhd));

	d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			break;

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR) Open("ENVARC:sys/wbconfig.prefs", MODE_NEWFILE);
		if (0 == iff->iff_Stream)
			break;

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		Result = OpenIFF(iff, IFFF_WRITE);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		IffOpen = TRUE;

		Result = PushChunk(iff, ID_PREF, ID_FORM, IFFSIZE_UNKNOWN);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		Result = PushChunk(iff, 0, ID_PRHD, IFFSIZE_UNKNOWN);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		if (WriteChunkBytes(iff, &prhd, sizeof(prhd)) < 0)
			break;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		Result = PopChunk(iff);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		Result = PushChunk(iff, 0, MAKE_ID('W','B','C','F'), IFFSIZE_UNKNOWN);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		wbcf.wbcf_Version = 1;

		if (iwtMain->iwt_BackDrop)
			{
			wbcf.wbcf_XOffset = CurrentPrefs.pref_WBWindowXOffset;
			wbcf.wbcf_YOffset = CurrentPrefs.pref_WBWindowYOffset;

			wbcf.wbcf_LeftEdge = CurrentPrefs.pref_WBWindowBox.Left;
			wbcf.wbcf_TopEdge = CurrentPrefs.pref_WBWindowBox.Top;
			wbcf.wbcf_Width = CurrentPrefs.pref_WBWindowBox.Width;
			wbcf.wbcf_Height = CurrentPrefs.pref_WBWindowBox.Height;

			wbcf.wbcf_Flags = WBCF_FLAGF_Backdrop;
			}
		else
			{
			wbcf.wbcf_XOffset = iInfos.xii_iinfos.ii_MainWindowStruct->ws_xoffset;
			wbcf.wbcf_YOffset = iInfos.xii_iinfos.ii_MainWindowStruct->ws_yoffset;
			wbcf.wbcf_LeftEdge = iInfos.xii_iinfos.ii_MainWindowStruct->ws_Left;
			wbcf.wbcf_TopEdge = iInfos.xii_iinfos.ii_MainWindowStruct->ws_Top;
			wbcf.wbcf_Width = iInfos.xii_iinfos.ii_MainWindowStruct->ws_Width;
			wbcf.wbcf_Height = iInfos.xii_iinfos.ii_MainWindowStruct->ws_Height;

			wbcf.wbcf_Flags = 0;
			}

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		wbcf.wbcf_Version = SCA_LONG2BE(wbcf.wbcf_Version);
		wbcf.wbcf_XOffset = SCA_WORD2BE(wbcf.wbcf_XOffset);
		wbcf.wbcf_YOffset = SCA_WORD2BE(wbcf.wbcf_YOffset);
		wbcf.wbcf_LeftEdge = SCA_WORD2BE(wbcf.wbcf_LeftEdge);
		wbcf.wbcf_TopEdge = SCA_WORD2BE(wbcf.wbcf_TopEdge);
		wbcf.wbcf_Width = SCA_WORD2BE(wbcf.wbcf_Width);
		wbcf.wbcf_Height = SCA_WORD2BE(wbcf.wbcf_Height);
		wbcf.wbcf_Flags = SCA_LONG2BE(wbcf.wbcf_Flags);

		if (WriteChunkBytes(iff, &wbcf, sizeof(wbcf)) < 0)
			break;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		Result = PopChunk(iff);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		Result = PopChunk(iff);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		CloseIFF(iff);
		IffOpen = FALSE;
		} while (0);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (iff)
		{
		if (IffOpen)
			CloseIFF(iff);
		if (iff->iff_Stream)
			Close((BPTR) iff->iff_Stream);

		FreeIFF(iff);
		}

	return Result;
}


LONG ReadWBConfig(void)
{
	struct IFFHandle *iff;
	struct PrefHeader prhd;
	struct WBCFChunk wbcf;
	BOOL IffOpen = FALSE;
	LONG Result = RETURN_OK;

	memset(&prhd, 0, sizeof(prhd));

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			break;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR) Open("ENVARC:sys/wbconfig.prefs", MODE_OLDFILE);
		if (0 == iff->iff_Stream)
			break;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		Result = OpenIFF(iff, IFFF_READ);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		IffOpen = TRUE;

		Result = StopChunk(iff, ID_PREF, MAKE_ID('W','B','C','F'));
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		Result = ParseIFF(iff, IFFPARSE_SCAN);
		d1(kprintf("%s/%s/%ld:  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		if (sizeof(wbcf) == ReadChunkBytes(iff, &wbcf, sizeof(wbcf)))
			{
			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__, wbcf.wbcf_Version));

			if (1 != SCA_BE2LONG(wbcf.wbcf_Version))
				break;

			CurrentPrefs.pref_WBWindowXOffset = SCA_BE2WORD(wbcf.wbcf_XOffset);
			CurrentPrefs.pref_WBWindowYOffset = SCA_BE2WORD(wbcf.wbcf_YOffset);

			CurrentPrefs.pref_WBWindowBox.Left = SCA_BE2WORD(wbcf.wbcf_LeftEdge);
			CurrentPrefs.pref_WBWindowBox.Top = SCA_BE2WORD(wbcf.wbcf_TopEdge);
			CurrentPrefs.pref_WBWindowBox.Width = SCA_BE2WORD(wbcf.wbcf_Width);
			CurrentPrefs.pref_WBWindowBox.Height = SCA_BE2WORD(wbcf.wbcf_Height);

			CurrentPrefs.pref_WBBackdrop = 0 != (SCA_BE2LONG(wbcf.wbcf_Flags) & WBCF_FLAGF_Backdrop);
			}

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (iff)
		{
		if (IffOpen)
			CloseIFF(iff);
		if (iff->iff_Stream)
			Close((BPTR) iff->iff_Stream);

		FreeIFF(iff);
		}

	return Result;
}


void InitScalosPrefs(void)
{
	CurrentPrefs = DefaultPrefs;
	PalettePrefs = DefaultPalettePrefs;
}


BOOL ReadScalosPrefs(void)
{
	struct ScalosPrefs *NewPrefs;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		WORD PluginArray[304];
		struct PrefsStruct *ps;
		UWORD uw;
		LONG Entry;
		CONST_STRPTR MainPrefsFileName = "ENV:Scalos/Scalos.prefs";

		NewPrefs = ScalosAlloc(sizeof(struct ScalosPrefs));
		d1(KPrintF("%s/%s/%ld: NewPrefs=%08lx\n", __FILE__, __FUNC__, __LINE__, NewPrefs));
		if (NULL == NewPrefs)
			break;

		*NewPrefs = DefaultPrefs;

		NewPrefs->pref_Handle = AllocPrefsHandle((STRPTR) "Scalos");
		if (NULL == NewPrefs->pref_Handle)
			break;

		MainPrefsCRC = GetPrefsCRCFromName(MainPrefsFileName);
		d1(KPrintF("%s/%s/%ld: MainPrefsCRC=%08lx\n", __FILE__, __FUNC__, __LINE__, MainPrefsCRC));

		while (RemEntry(NewPrefs->pref_Handle, ID_MAIN, SCP_PlugInList, 0))
			;
		while (RemEntry(NewPrefs->pref_Handle, ID_MAIN, SCP_ControlBarGadgetsStd, 0))
			;
		while (RemEntry(NewPrefs->pref_Handle, ID_MAIN, SCP_ControlBarGadgetsBr, 0))
			;

		ReadPrefsHandle(NewPrefs->pref_Handle, (STRPTR) MainPrefsFileName);

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconOffsets, &NewPrefs->pref_ImageBorders, sizeof(NewPrefs->pref_ImageBorders)))
			{
			NewPrefs->pref_ImageBorders.Left = SCA_BE2WORD(NewPrefs->pref_ImageBorders.Left);
			NewPrefs->pref_ImageBorders.Top = SCA_BE2WORD(NewPrefs->pref_ImageBorders.Top);
			NewPrefs->pref_ImageBorders.Right = SCA_BE2WORD(NewPrefs->pref_ImageBorders.Right);
			NewPrefs->pref_ImageBorders.Bottom = SCA_BE2WORD(NewPrefs->pref_ImageBorders.Bottom);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconNormFrame, &NewPrefs->pref_FrameType, sizeof(NewPrefs->pref_FrameType)))
			{
			NewPrefs->pref_FrameType = SCA_BE2WORD(NewPrefs->pref_FrameType);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconSelFrame, &NewPrefs->pref_FrameTypeSel, sizeof(NewPrefs->pref_FrameTypeSel)))
			{
			NewPrefs->pref_FrameTypeSel = SCA_BE2WORD(NewPrefs->pref_FrameTypeSel);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconNormThumbnailFrame, &NewPrefs->pref_ThumbnailFrameType, sizeof(NewPrefs->pref_ThumbnailFrameType)))
			{
			NewPrefs->pref_ThumbnailFrameType = SCA_BE2WORD(NewPrefs->pref_ThumbnailFrameType);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconSelThumbnailFrame, &NewPrefs->pref_ThumbnailFrameTypeSel, sizeof(NewPrefs->pref_ThumbnailFrameTypeSel)))
			{
			NewPrefs->pref_ThumbnailFrameTypeSel = SCA_BE2WORD(NewPrefs->pref_ThumbnailFrameTypeSel);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconTextSkip, &NewPrefs->pref_TextSkip, sizeof(NewPrefs->pref_TextSkip)))
			{
			NewPrefs->pref_TextSkip = SCA_BE2WORD(NewPrefs->pref_TextSkip);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SelTextRectBorderX, &NewPrefs->pref_SelTextRectBorderX, sizeof(NewPrefs->pref_SelTextRectBorderX)))
			{
			NewPrefs->pref_SelTextRectBorderX = SCA_BE2WORD(NewPrefs->pref_SelTextRectBorderX);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SelTextRectBorderY, &NewPrefs->pref_SelTextRectBorderY, sizeof(NewPrefs->pref_SelTextRectBorderY)))
			{
			NewPrefs->pref_SelTextRectBorderY = SCA_BE2WORD(NewPrefs->pref_SelTextRectBorderY);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SelTextRectRadius, &NewPrefs->pref_SelTextRectRadius, sizeof(NewPrefs->pref_SelTextRectRadius)))
			{
			NewPrefs->pref_SelTextRectRadius = SCA_BE2WORD(NewPrefs->pref_SelTextRectRadius);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconSecLine, &NewPrefs->pref_IconTextMuliLine, sizeof(NewPrefs->pref_IconTextMuliLine));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_EnableDropMenu, &NewPrefs->pref_EnableDropMenu, sizeof(NewPrefs->pref_EnableDropMenu));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SelectedTextRectangle, &NewPrefs->pref_SelectedTextRectangle, sizeof(NewPrefs->pref_SelectedTextRectangle));
		// UBYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconTextMode, &uw, sizeof(uw)))
			{
			NewPrefs->pref_TextMode = SCA_BE2WORD(uw);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_CleanupOnResize, &NewPrefs->pref_AutoCleanupOnResize, sizeof(NewPrefs->pref_AutoCleanupOnResize));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_AutoLeaveOut, &NewPrefs->pref_AutoLeaveOut, sizeof(NewPrefs->pref_AutoLeaveOut));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconHiliteUnderMouse, &NewPrefs->pref_MarkIconUnderMouse, sizeof(NewPrefs->pref_MarkIconUnderMouse));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_BobsType, &NewPrefs->pref_DragType, sizeof(NewPrefs->pref_DragType));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_BobsMethod, &NewPrefs->pref_DragMethod, sizeof(NewPrefs->pref_DragMethod));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_BobsTranspMode, &NewPrefs->pref_DragTranspMode, sizeof(NewPrefs->pref_DragTranspMode));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_BobsTranspType, &NewPrefs->pref_RealTransFlag, sizeof(NewPrefs->pref_RealTransFlag));
		// UBYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_BobsTransp, &NewPrefs->pref_DragTranspObjects, sizeof(NewPrefs->pref_DragTranspObjects)))
			{
			NewPrefs->pref_DragTranspObjects = SCA_BE2LONG(NewPrefs->pref_DragTranspObjects);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ShowThumbnails, &NewPrefs->pref_ShowThumbnails, sizeof(NewPrefs->pref_ShowThumbnails));
		// UBYTE
		d1(KPrintF("%s/%s/%ld: pref_ShowThumbnails=%ld\n", __FILE__, __FUNC__, __LINE__, NewPrefs->pref_ShowThumbnails));

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ThumbnailSize, &NewPrefs->pref_ThumbNailWidth, sizeof(NewPrefs->pref_ThumbNailWidth)))
			{
			NewPrefs->pref_ThumbNailWidth = SCA_BE2WORD(NewPrefs->pref_ThumbNailWidth);
			}

		NewPrefs->pref_ThumbNailHeight = NewPrefs->pref_ThumbNailWidth;	      // Thumbnails are square by default

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ThumbnailMaxAge, &NewPrefs->pref_ThumbnailMaxAge, sizeof(NewPrefs->pref_ThumbnailMaxAge)))
			{
			NewPrefs->pref_ThumbnailMaxAge = SCA_BE2WORD(NewPrefs->pref_ThumbnailMaxAge);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ThumbnailMinSizeLimit, &NewPrefs->pref_ThumbnailMinSizeLimit, sizeof(NewPrefs->pref_ThumbnailMinSizeLimit)))
			{
			NewPrefs->pref_ThumbnailMinSizeLimit = SCA_BE2WORD(NewPrefs->pref_ThumbnailMinSizeLimit);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ShowThumbnailsAsDefault, &NewPrefs->pref_ShowThumbnailsAsDefault, sizeof(NewPrefs->pref_ShowThumbnailsAsDefault)); // +jmc+
		// BYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ThumbnailQuality, &NewPrefs->pref_ThumbnailQuality, sizeof(NewPrefs->pref_ThumbnailQuality)))
			{
			NewPrefs->pref_ThumbnailQuality = SCA_BE2LONG(NewPrefs->pref_ThumbnailQuality);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ThumbnailsSquare, &NewPrefs->pref_ThumbnailsSquare, sizeof(NewPrefs->pref_ThumbnailsSquare));
		// BYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ThumbnailOffsets, &NewPrefs->pref_ThumbnailImageBorders, sizeof(NewPrefs->pref_ThumbnailImageBorders)))
			{
			NewPrefs->pref_ThumbnailImageBorders.Left = SCA_BE2WORD(NewPrefs->pref_ThumbnailImageBorders.Left);
			NewPrefs->pref_ThumbnailImageBorders.Top = SCA_BE2WORD(NewPrefs->pref_ThumbnailImageBorders.Top);
			NewPrefs->pref_ThumbnailImageBorders.Right = SCA_BE2WORD(NewPrefs->pref_ThumbnailImageBorders.Right);
			NewPrefs->pref_ThumbnailImageBorders.Bottom = SCA_BE2WORD(NewPrefs->pref_ThumbnailImageBorders.Bottom);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ThumbnailsFillBackground, &NewPrefs->pref_ThumbnailFillBackground, sizeof(NewPrefs->pref_ThumbnailFillBackground));
		// UBYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TransparencyThumbnailsBack, &NewPrefs->pref_ThumbnailBackgroundTransparency, sizeof(NewPrefs->pref_ThumbnailBackgroundTransparency)))
			{
			NewPrefs->pref_ThumbnailBackgroundTransparency = SCA_BE2WORD(NewPrefs->pref_ThumbnailBackgroundTransparency);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconNormFrame, &NewPrefs->pref_FrameType, sizeof(NewPrefs->pref_FrameType)))
			{
			NewPrefs->pref_FrameType = SCA_BE2WORD(NewPrefs->pref_FrameType);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DragDropCopyQualifier, &NewPrefs->pref_CopyQualifier, sizeof(NewPrefs->pref_CopyQualifier)))
			{
			NewPrefs->pref_CopyQualifier = SCA_BE2LONG(NewPrefs->pref_CopyQualifier);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_PopupApplySelectedQualifier, &NewPrefs->pref_PopupApplySelectedQualifier, sizeof(NewPrefs->pref_PopupApplySelectedQualifier)))
			{
			NewPrefs->pref_PopupApplySelectedQualifier = SCA_BE2LONG(NewPrefs->pref_PopupApplySelectedQualifier);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DragDropMakeLinkQualifier, &NewPrefs->pref_MakeLinkQualifier, sizeof(NewPrefs->pref_MakeLinkQualifier)))
			{
			NewPrefs->pref_MakeLinkQualifier = SCA_BE2LONG(NewPrefs->pref_MakeLinkQualifier);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DragDropMoveQualifier, &NewPrefs->pref_ForceMoveQualifier, sizeof(NewPrefs->pref_ForceMoveQualifier)))
			{
			NewPrefs->pref_ForceMoveQualifier = SCA_BE2LONG(NewPrefs->pref_ForceMoveQualifier);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SingleWindowLassoQualifier, &NewPrefs->pref_SingleWindowLassoQualifier, sizeof(NewPrefs->pref_SingleWindowLassoQualifier)))
			{
			NewPrefs->pref_SingleWindowLassoQualifier = SCA_BE2LONG(NewPrefs->pref_SingleWindowLassoQualifier);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TransparencyIconDrag, &NewPrefs->pref_DragTransparency, sizeof(NewPrefs->pref_DragTransparency)))
			{
			NewPrefs->pref_DragTransparency = SCA_BE2WORD(NewPrefs->pref_DragTransparency);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TransparencyIconShadow, &NewPrefs->pref_IconShadowTransparency, sizeof(NewPrefs->pref_IconShadowTransparency)))
			{
			NewPrefs->pref_IconShadowTransparency = SCA_BE2WORD(NewPrefs->pref_IconShadowTransparency);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TransparencyDefaultIcon, &NewPrefs->pref_DefaultIconTransparency, sizeof(NewPrefs->pref_DefaultIconTransparency)))
			{
			NewPrefs->pref_DefaultIconTransparency = SCA_BE2WORD(NewPrefs->pref_DefaultIconTransparency);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ToolTipTransparency, &NewPrefs->pref_TooltipsTransparency, sizeof(NewPrefs->pref_TooltipsTransparency)))
			{
			NewPrefs->pref_TooltipsTransparency = SCA_BE2WORD(NewPrefs->pref_TooltipsTransparency);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DefaultStackSize, &NewPrefs->pref_DefaultStackSize, sizeof(NewPrefs->pref_DefaultStackSize)))
			{
			NewPrefs->pref_DefaultStackSize = SCA_BE2LONG(NewPrefs->pref_DefaultStackSize);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ConsoleName, &NewPrefs->pref_ConsoleName, sizeof(NewPrefs->pref_ConsoleName));
		// BYTE[256]

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TTScreenFontDesc, NewPrefs->pref_ScreenTTFontDescriptor, sizeof(NewPrefs->pref_ScreenTTFontDescriptor) );
		// char[256]

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TTIconFontDesc, NewPrefs->pref_IconTTFontDescriptor, sizeof(NewPrefs->pref_IconTTFontDescriptor) );
		// char[256]

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TTTextWindowFontDesc, NewPrefs->pref_TextWindowTTFontDescriptor, sizeof(NewPrefs->pref_TextWindowTTFontDescriptor) );
		// char[256]

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_UseTTScreenFont, &NewPrefs->pref_UseScreenTTFont, sizeof(NewPrefs->pref_UseScreenTTFont) );
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_UseTTIconFont, &NewPrefs->pref_UseIconTTFont, sizeof(NewPrefs->pref_UseIconTTFont) );
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_UseTTTextWindowFont, &NewPrefs->pref_UseTextWindowTTFont, sizeof(NewPrefs->pref_UseTextWindowTTFont) );
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TTfAntialiasing, &NewPrefs->pref_TTFontAntialias, sizeof(NewPrefs->pref_TTFontAntialias) );
		// UBYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TTfGamma, &NewPrefs->pref_TTFontGamma, sizeof(NewPrefs->pref_TTFontGamma) ))
			{
			NewPrefs->pref_TTFontGamma = SCA_BE2WORD(NewPrefs->pref_TTFontGamma);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_CopyBuffLen, &NewPrefs->pref_CopyBuffLen, sizeof(NewPrefs->pref_CopyBuffLen) ))
			{
			NewPrefs->pref_CopyBuffLen = SCA_BE2LONG(NewPrefs->pref_CopyBuffLen);
			}

		NewPrefs->pref_ScreenTitle = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_ScreenTitle, DefaultScreenTitle);

		SetAttrs(ScreenTitleObject,
			SCCA_Title_Format, (ULONG) NewPrefs->pref_ScreenTitle,
			TAG_END);

		NewPrefs->pref_MainWindowTitle = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_RootWinTitle, DefaultMainWindowTitle);
		NewPrefs->pref_StandardWindowTitle = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_WindowTitle, DefaultWindowTitle);

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TitleRefresh, &NewPrefs->pref_TitleRefreshSec, sizeof(NewPrefs->pref_TitleRefreshSec));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_RefreshOnMemoryChange, &NewPrefs->pref_RefreshOnMemChg, sizeof(NewPrefs->pref_RefreshOnMemChg));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_WinTitleRefresh, &NewPrefs->pref_WinTitleRefreshSec, sizeof(NewPrefs->pref_WinTitleRefreshSec));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_WinRefreshOnMemoryChange, &NewPrefs->pref_WinRefreshOnMemChg, sizeof(NewPrefs->pref_WinRefreshOnMemChg));
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_CheckOverlappingIcons, &NewPrefs->pref_CheckOverlappingIcons, sizeof(NewPrefs->pref_CheckOverlappingIcons));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MiscAutoRemove, &NewPrefs->pref_AutoRemoveFlag, sizeof(NewPrefs->pref_AutoRemoveFlag));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MiscClickTransp, &NewPrefs->pref_ClickTransp, sizeof(NewPrefs->pref_ClickTransp));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MiscHardEmulation, &NewPrefs->pref_HardEmulation, sizeof(NewPrefs->pref_HardEmulation));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MiscUseExAll, &NewPrefs->pref_UseExAll, sizeof(NewPrefs->pref_UseExAll));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_CreateSoftLinks, &NewPrefs->pref_CreateSoftLinks, sizeof(NewPrefs->pref_CreateSoftLinks));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MiscWindowType, &NewPrefs->pref_SmartRefresh, sizeof(NewPrefs->pref_SmartRefresh));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MiscDoWaitDelay, &NewPrefs->pref_DoWaitDelay, sizeof(NewPrefs->pref_DoWaitDelay));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MiscDiskiconsRefresh, &NewPrefs->pref_DiskRefresh, sizeof(NewPrefs->pref_DiskRefresh));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MiscMenuCurrentDir, &NewPrefs->pref_MenuCurrentDir, sizeof(NewPrefs->pref_MenuCurrentDir));
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MMB_Move, &NewPrefs->pref_MMBMoveFlag, sizeof(NewPrefs->pref_MMBMoveFlag));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_FullBench, &NewPrefs->pref_FullBenchFlag, sizeof(NewPrefs->pref_FullBenchFlag));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_WindowPopupTitleOnly, &NewPrefs->pref_FullPopupFlag, sizeof(NewPrefs->pref_FullPopupFlag));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DefaultIconsSaveable, &NewPrefs->pref_SaveDefIcons, sizeof(NewPrefs->pref_SaveDefIcons));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_LoadDefIconsFirst, &NewPrefs->pref_DefIconsFirst, sizeof(NewPrefs->pref_DefIconsFirst));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_EasyMultiSelect, &NewPrefs->pref_EasyMultiselectFlag, sizeof(NewPrefs->pref_EasyMultiselectFlag));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_EasyMultiDrag, &NewPrefs->pref_EasyMultiDragFlag, sizeof(NewPrefs->pref_EasyMultiDragFlag));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DropStart, &NewPrefs->pref_DragStartFlag, sizeof(NewPrefs->pref_DragStartFlag));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_HideHiddenFiles, &NewPrefs->pref_HideHiddenFlag, sizeof(NewPrefs->pref_HideHiddenFlag));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_HideProtectHiddenFiles, &NewPrefs->pref_HideProtectHiddenFlag, sizeof(NewPrefs->pref_HideProtectHiddenFlag)); // jmc
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_PopScreenTitle, &NewPrefs->pref_PopTitleFlag, sizeof(NewPrefs->pref_PopTitleFlag));
		// UBYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DefaultWindowSize, &NewPrefs->pref_DefWindowBox, sizeof(NewPrefs->pref_DefWindowBox)))
			{
			NewPrefs->pref_DefWindowBox.Left = SCA_BE2WORD(NewPrefs->pref_DefWindowBox.Left);
			NewPrefs->pref_DefWindowBox.Top = SCA_BE2WORD(NewPrefs->pref_DefWindowBox.Top);
			NewPrefs->pref_DefWindowBox.Width = SCA_BE2WORD(NewPrefs->pref_DefWindowBox.Width);
			NewPrefs->pref_DefWindowBox.Height = SCA_BE2WORD(NewPrefs->pref_DefWindowBox.Height);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconCleanupSpace, &NewPrefs->pref_CleanupSpace, sizeof(NewPrefs->pref_CleanupSpace)))
			{
			NewPrefs->pref_CleanupSpace.Left = SCA_BE2WORD(NewPrefs->pref_CleanupSpace.Left);
			NewPrefs->pref_CleanupSpace.Top = SCA_BE2WORD(NewPrefs->pref_CleanupSpace.Top);
			NewPrefs->pref_CleanupSpace.XSkip = SCA_BE2WORD(NewPrefs->pref_CleanupSpace.XSkip);
			NewPrefs->pref_CleanupSpace.YSkip = SCA_BE2WORD(NewPrefs->pref_CleanupSpace.YSkip);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SplashWindowEnable, &NewPrefs->pref_EnableSplash, sizeof(NewPrefs->pref_EnableSplash));
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_PopupWindowsEnable, &NewPrefs->pref_EnablePopupWindows, sizeof(NewPrefs->pref_EnablePopupWindows));
		// BYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SplashWindowHoldTime, &NewPrefs->pref_SplashHoldTime, sizeof(NewPrefs->pref_SplashHoldTime)))
			{
			NewPrefs->pref_SplashHoldTime = SCA_BE2WORD(NewPrefs->pref_SplashHoldTime);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_StatusBarEnable, &NewPrefs->pref_DisplayStatusBar, sizeof(NewPrefs->pref_DisplayStatusBar));
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ToolTipsEnable, &NewPrefs->pref_EnableTooltips, sizeof(NewPrefs->pref_EnableTooltips));
		// BYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ToolTipPopupDelay, &NewPrefs->pref_ToolTipDelaySeconds, sizeof(NewPrefs->pref_ToolTipDelaySeconds)))
			{
			NewPrefs->pref_ToolTipDelaySeconds = SCA_BE2LONG(NewPrefs->pref_ToolTipDelaySeconds);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_PopupWindowsDelay, &NewPrefs->pref_PopupWindowDelaySeconds, sizeof(NewPrefs->pref_PopupWindowDelaySeconds)))
			{
			NewPrefs->pref_PopupWindowDelaySeconds = SCA_BE2WORD(NewPrefs->pref_PopupWindowDelaySeconds);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ShowDragDropObjCount, &NewPrefs->pref_ShowDDCountText, sizeof(NewPrefs->pref_ShowDDCountText));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DragDropIconsSingle, &NewPrefs->pref_UseOldDragIcons, sizeof(NewPrefs->pref_UseOldDragIcons));
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TextWindowsStriped, &NewPrefs->pref_TextWindowStriped, sizeof(NewPrefs->pref_TextWindowStriped));
		// BYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DropMarkMode, &NewPrefs->pref_WindowDropMarkMode, sizeof(NewPrefs->pref_WindowDropMarkMode));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ShowAllDefault, &NewPrefs->pref_DefaultShowAllFiles, sizeof(NewPrefs->pref_DefaultShowAllFiles));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ViewByDefault, &NewPrefs->pref_DefaultViewBy, sizeof(NewPrefs->pref_DefaultViewBy));
		// UBYTE

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SelectTextIconName, &NewPrefs->pref_SelectTextIconName, sizeof(NewPrefs->pref_SelectTextIconName));
		// UBYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconSizeConstraints, &NewPrefs->pref_IconSizeConstraints, sizeof(NewPrefs->pref_IconSizeConstraints)))
			{
			NewPrefs->pref_IconSizeConstraints.MinX = SCA_BE2WORD(NewPrefs->pref_IconSizeConstraints.MinX);
			NewPrefs->pref_IconSizeConstraints.MinY = SCA_BE2WORD(NewPrefs->pref_IconSizeConstraints.MinY);
			NewPrefs->pref_IconSizeConstraints.MaxX = SCA_BE2WORD(NewPrefs->pref_IconSizeConstraints.MaxX);
			NewPrefs->pref_IconSizeConstraints.MaxY = SCA_BE2WORD(NewPrefs->pref_IconSizeConstraints.MaxY);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconNominalSize, &NewPrefs->pref_IconScaleFactor, sizeof(NewPrefs->pref_IconScaleFactor)))
			{
			NewPrefs->pref_IconScaleFactor = SCA_BE2WORD(NewPrefs->pref_IconScaleFactor);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DeviceWinIconLayout, &NewPrefs->pref_DeviceWindowLayoutModes, sizeof(NewPrefs->pref_DeviceWindowLayoutModes));
		// UBYTE[]

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_IconWinIconLayout, &NewPrefs->pref_IconWindowLayoutModes, sizeof(NewPrefs->pref_IconWindowLayoutModes));
		// UBYTE[]

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_PopupApplySelectedAlways, &NewPrefs->pref_PopupApplySelectedAlways, sizeof(NewPrefs->pref_PopupApplySelectedAlways));
		// BYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_ActiveWindowTransparency, &NewPrefs->pref_ActiveWindowTransparency, sizeof(NewPrefs->pref_ActiveWindowTransparency)))
			{
			NewPrefs->pref_ActiveWindowTransparency = SCA_BE2WORD(NewPrefs->pref_ActiveWindowTransparency);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_InactiveWindowTransparency, &NewPrefs->pref_InactiveWindowTransparency, sizeof(NewPrefs->pref_InactiveWindowTransparency)))
			{
			NewPrefs->pref_InactiveWindowTransparency = SCA_BE2WORD(NewPrefs->pref_InactiveWindowTransparency);
			}

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SelectMarkerBaseColor, &NewPrefs->pref_SelectMarkerBaseColor, sizeof(NewPrefs->pref_SelectMarkerBaseColor));
		// struct ARGB

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_SelectMarkerTransparency, &NewPrefs->pref_SelectMarkerTransparency, sizeof(NewPrefs->pref_SelectMarkerTransparency));
		// UBYTE

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_MaxUndoSteps, &NewPrefs->pref_MaxUndoSteps, sizeof(NewPrefs->pref_MaxUndoSteps)))
			{
			NewPrefs->pref_MaxUndoSteps = SCA_BE2LONG(NewPrefs->pref_MaxUndoSteps);
			}

		if (GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_DrawerSortMode, &NewPrefs->pref_DrawerSortMode, sizeof(NewPrefs->pref_DrawerSortMode)))
			{
			NewPrefs->pref_DrawerSortMode = SCA_BE2LONG(NewPrefs->pref_DrawerSortMode);
			}

		// Plausibility check for icon size constraints
		// make sure that max never smaller than min
		if (NewPrefs->pref_IconSizeConstraints.MinX > NewPrefs->pref_IconSizeConstraints.MaxX)
			NewPrefs->pref_IconSizeConstraints.MaxX = NewPrefs->pref_IconSizeConstraints.MinX;
		if (NewPrefs->pref_IconSizeConstraints.MinY > NewPrefs->pref_IconSizeConstraints.MaxY)
			NewPrefs->pref_IconSizeConstraints.MaxY = NewPrefs->pref_IconSizeConstraints.MinY;

		GetPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_UnderSoftLinkNames, &UnderlineSoftLinkNames, sizeof(UnderlineSoftLinkNames));
		// BYTE
		if (UnderlineSoftLinkNames)
			NewPrefs->pref_LinkTextStyle = FSF_UNDERLINED;
		else
			NewPrefs->pref_LinkTextStyle = FS_NORMAL;

		NewPrefs->pref_DefIconPath = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_PathsDefIcons, DefaultPrefs.pref_DefIconPath);
		NewPrefs->pref_DefDiskCopy = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_PathsDiskCopy, DefaultPrefs.pref_DefDiskCopy);
		NewPrefs->pref_WBStartupDir = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_PathsWBStartup, DefaultPrefs.pref_WBStartupDir);
		NewPrefs->pref_ScalosHomeDir = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_PathsHome, DefaultPrefs.pref_ScalosHomeDir);
		NewPrefs->pref_ThemesDir = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_PathsTheme, DefaultPrefs.pref_ThemesDir);
		NewPrefs->pref_ImageCacheDir = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_PathsImagecache, DefaultPrefs.pref_ImageCacheDir);
		NewPrefs->pref_SQLiteTempDir = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_PathsSQLiteTempDir, DefaultPrefs.pref_SQLiteTempDir);

		NewPrefs->pref_ThumbDatabasePath = GetPrefsConfigString(NewPrefs->pref_Handle, SCP_PathsThumbnailDb, DefaultPrefs.pref_ThumbDatabasePath);

		ps = FindPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TextModeFont);

		if (ps)
			{
			ps->ps_Size = SCA_BE2WORD(ps->ps_Size);
			SetTextAttr(&NewPrefs->pref_TextModeTextAttr, ps);
			d1(KPrintF("%s/%s/%ld: textfont=<%s>  YSize=%ld\n", __FILE__, __FUNC__, __LINE__, \
				NewPrefs->pref_TextModeTextAttr.sta_TextAttr.ta_Name, NewPrefs->pref_TextModeTextAttr.sta_TextAttr.ta_YSize));
			}

		SetAttrs(ScreenTitleObject,
			SCCA_Title_Format, (ULONG) NewPrefs->pref_ScreenTitle,
			TAG_END);

		ps = FindPreferences(NewPrefs->pref_Handle, ID_MAIN, SCP_TextMode_ListColumns);
		if (ps)
			{
			BYTE *pu = (BYTE *) PS_DATA(ps);
			short n = 0;

			while (~0 != *pu && n < Sizeof(NewPrefs->pref_ColumnsArray) - 1)
				{
				BYTE col = *pu++;

				d1(kprintf("%s/%s/%ld: Column=%ld\n", __FILE__, __FUNC__, __LINE__, col));

				if (!(col & 0x80))
					{
					NewPrefs->pref_ColumnsArray[n++] = col;
					d1(kprintf("%s/%s/%ld: ColumnsArray[%ld]=%ld\n", __FILE__, __FUNC__, __LINE__, n-1, NewPrefs->pref_ColumnsArray[n-1]));
					}
				}

			NewPrefs->pref_ColumnsArray[n] = (UBYTE)-1;
			}

		Entry = 0;

		while (GetEntry(NewPrefs->pref_Handle, ID_MAIN, SCP_PlugInList, PluginArray, sizeof(PluginArray), Entry) > 0)
			{
			struct SCP_PluginEntry *pey = (struct SCP_PluginEntry *) PluginArray;

			d1(kprintf("%s/%s/%ld: MinVersion=%ld\n", __FILE__, __FUNC__, __LINE__, pey->pey_NeededVersion));

			if (SCA_BE2WORD(pey->pey_NeededVersion) <= ScalosBase->scb_LibNode.lib_Version)
				{
				STRPTR lp;
				struct PluginClass *pl = (struct PluginClass *) SCA_AllocStdNode((struct ScalosNodeList *)(APTR) &ScalosPluginList, NTYP_PluginNode);

				if (pl)
					{
					pl->plug_priority = SCA_BE2WORD(pey->pey_Priority);
					pl->plug_instsize = SCA_BE2WORD(pey->pey_InstSize);

					d1(kprintf("%s/%s/%ld: priority=%ld  instsize=%ld\n", __FILE__, __FUNC__, __LINE__, pl->plug_priority, pl->plug_instsize));

					lp = pey->pey_Path;

					pl->plug_filename = AllocCopyString(lp);
					d1(kprintf("%s/%s/%ld: filename=<%s>\n", __FILE__, __FUNC__, __LINE__, pl->plug_filename));

					lp += 1 + strlen(lp);
					pl->plug_classname = AllocCopyString(lp);
					d1(kprintf("%s/%s/%ld: classname=<%s>\n", __FILE__, __FUNC__, __LINE__, pl->plug_classname));

					lp += 1 + strlen(lp);
					pl->plug_superclassname = AllocCopyString(lp);

					d1(kprintf("%s/%s/%ld: superclassname=<%s>\n", __FILE__, __FUNC__, __LINE__, pl->plug_superclassname));
					}
				}
			Entry++;
			}

		Success = TRUE;
		} while (0);

	if (Success)
		{
		ReadControlBarGadgetList(NewPrefs->pref_Handle, ID_MAIN,
			&ControlBarGadgetListNormal, SCP_ControlBarGadgetsStd, SCP_ControlBarGadgetStringsStd);
		ReadControlBarGadgetList(NewPrefs->pref_Handle, ID_MAIN,
			&ControlBarGadgetListBrowser, SCP_ControlBarGadgetsBr, SCP_ControlBarGadgetStringsBr);

		InternalFreeScalosPrefs(&CurrentPrefs);
		CurrentPrefs = *NewPrefs;
		}

	if (NewPrefs)
		ScalosFree(NewPrefs);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}


static CONST_STRPTR GetPrefsConfigString(APTR prefsHandle, ULONG Id, CONST_STRPTR DefaultString)
{
	struct PrefsStruct *ps = FindPreferences(prefsHandle, ID_MAIN, Id);

	if (ps)
		return (CONST_STRPTR) PS_DATA(ps);

	return DefaultString;
}


void FreeScalosPrefs(void)
{
	InternalFreeScalosPrefs(&CurrentPrefs);
	CleanupControlBarGadgetsList(&ControlBarGadgetListNormal);
	CleanupControlBarGadgetsList(&ControlBarGadgetListBrowser);
}


static void InternalFreeScalosPrefs(struct ScalosPrefs *Prefs)
{
	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (Prefs->pref_TextModeTextAttr.sta_AllocName)
		{
		ScalosFree(Prefs->pref_TextModeTextAttr.sta_AllocName);
		Prefs->pref_TextModeTextAttr.sta_AllocName = NULL;
		Prefs->pref_TextModeTextAttr.sta_TextAttr.ta_Name = (STRPTR) "topaz.font";
		}
	if (Prefs->pref_IconFontAttr.sta_AllocName)
		{
		ScalosFree(Prefs->pref_IconFontAttr.sta_AllocName);
		Prefs->pref_IconFontAttr.sta_AllocName = NULL;
		Prefs->pref_IconFontAttr.sta_TextAttr.ta_Name = (STRPTR) "topaz.font";
		}

	Prefs->pref_MainWindowTitle = DefaultPrefs.pref_MainWindowTitle;
	Prefs->pref_StandardWindowTitle = DefaultPrefs.pref_StandardWindowTitle;
	Prefs->pref_ScreenTitle = DefaultPrefs.pref_ScreenTitle;

	Prefs->pref_DefIconPath = DefaultPrefs.pref_DefIconPath;
	Prefs->pref_DefDiskCopy = DefaultPrefs.pref_DefDiskCopy;
	Prefs->pref_WBStartupDir = DefaultPrefs.pref_WBStartupDir;
	Prefs->pref_ScalosHomeDir = DefaultPrefs.pref_ScalosHomeDir;
	Prefs->pref_ThemesDir = DefaultPrefs.pref_ThemesDir;
	Prefs->pref_ImageCacheDir = DefaultPrefs.pref_ImageCacheDir;

	if (Prefs->pref_Handle)
		{
		FreePrefsHandle(Prefs->pref_Handle);
		Prefs->pref_Handle = NULL;
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


LONG ReadPatternPrefs(void)
{
	static const LONG ReadPatternPrefsStopChunks[] =
		{
		ID_PREF, ID_DEFS,
		ID_PREF, ID_PATT,
		};
	struct IFFHandle *iff;
	BOOL IffOpen = FALSE;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	ScalosObtainSemaphore(&PatternSema);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	FreePatternPrefs();

	PatternPrefs = DefaultPatternPrefs;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	do	{
		LONG Result;

		iff = AllocIFF();
		if (NULL == iff)
			break;

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR) Open("ENV:Scalos/Pattern.prefs", MODE_OLDFILE);
		if (0 == iff->iff_Stream)
			break;

		PatternPrefsCRC = GetPrefsCRCFromFH((BPTR) iff->iff_Stream);
		d1(KPrintF("%s/%s/%ld: PatternPrefsCRC=%08lx\n", __FILE__, __FUNC__, __LINE__, PatternPrefsCRC));

		Result = OpenIFF(iff, IFFF_READ);
		if (RETURN_OK != Result)
			break;

		IffOpen = TRUE;

		Result = StopChunks(iff, (LONG *) ReadPatternPrefsStopChunks, 2);
		if (RETURN_OK != Result)
			break;

		while (RETURN_OK == Result)
			{
			const struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_SCAN);
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);
			if (NULL == cn)
				break;

			switch (cn->cn_ID)
				{
			case ID_DEFS:
				ReadPatternDefaults(iff);
				break;

			case ID_PATT:
				CreatePatternNode(iff, cn);
				break;
				}
			}
		} while (0);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (iff)
		{
		if (IffOpen)
			CloseIFF(iff);

		if (iff->iff_Stream)
			{
			Close((BPTR) iff->iff_Stream);
			iff->iff_Stream = 0;
			}
		FreeIFF(iff);
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (PatternNodes)
		{
		struct Hook SortHook;

		SETHOOKFUNC(SortHook, PatternNodeCompareFunc);
		SortHook.h_Data = NULL;

		SCA_SortNodes((struct ScalosNodeList *)(APTR) &PatternNodes, &SortHook, SCA_SortType_Best);

		if (!PatternPrefs.patt_RandomizeEverytime)
			{
			struct PatternNode *pNode1 = PatternNodes;

			while (pNode1)
				{
				struct PatternNode *pNode2 = pNode1;
				ULONG Count;

				for (Count=0; pNode1 && pNode2->ptn_number == pNode1->ptn_number;
						pNode1 = (struct PatternNode *) pNode1->ptn_Node.mln_Succ)
					{
					Count++;
					}

				if (Count > 1)
					{
					LONG n;
					LONG Number = RandomNumber(Count - 1);

					// Remove all PatternNodes with the same number
					// except the one selected by random number.
					for (n=0; n<Count; n++)
						{
						struct PatternNode *pNode3 = pNode2;

						pNode2 = (struct PatternNode *) pNode2->ptn_Node.mln_Succ;

						if (n != Number)
							FreePatternNode(&PatternNodes, pNode3);
						}
					}
				}
			}
		}

	ScalosReleaseSemaphore(&PatternSema);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;
}


static void CreatePatternNode(struct IFFHandle *iff, const struct ContextNode *cn)
{
	struct ScaPatternPrefs *pPrefs;
	struct PatternNode *patNode = NULL;

	do	{
		pPrefs = ScalosAlloc(cn->cn_Size);
		if (NULL == pPrefs)
			break;

		if (cn->cn_Size != ReadChunkBytes(iff, pPrefs, cn->cn_Size))
			break;

		pPrefs->scp_Number = SCA_BE2WORD(pPrefs->scp_Number);
		pPrefs->scp_RenderType = SCA_BE2WORD(pPrefs->scp_RenderType);
		pPrefs->scp_Flags = SCA_BE2WORD(pPrefs->scp_Flags);
		pPrefs->scp_NumColors = SCA_BE2WORD(pPrefs->scp_NumColors);
		pPrefs->scp_DitherMode = SCA_BE2WORD(pPrefs->scp_DitherMode);
		pPrefs->scp_DitherAmount = SCA_BE2WORD(pPrefs->scp_DitherAmount);

		patNode = (struct PatternNode *) SCA_AllocStdNode((struct ScalosNodeList *)(APTR) &PatternNodes, NTYP_PatternNode);
		if (NULL == patNode)
			break;

		patNode->ptn_semaphore = ScalosCreateSemaphore();
		if (NULL == patNode->ptn_semaphore)
			break;

		patNode->ptn_number = pPrefs->scp_Number;
		patNode->ptn_type = pPrefs->scp_RenderType;
		patNode->ptn_BgMode = pPrefs->scp_Type;
		patNode->ptn_flags = pPrefs->scp_Flags;
		patNode->ptn_weight = pPrefs->scp_NumColors;
		patNode->ptn_dithermode = pPrefs->scp_DitherMode;
		patNode->ptn_ditheramount = pPrefs->scp_DitherAmount;
		patNode->ptn_prec = pPrefs->scp_Precision;
		memcpy(patNode->ptn_BgColor1, pPrefs->scp_color1, sizeof(patNode->ptn_BgColor1));
		memcpy(patNode->ptn_BgColor2, pPrefs->scp_color2, sizeof(patNode->ptn_BgColor2));

		patNode->ptn_filename = AllocCopyString((STRPTR) (pPrefs + 1));

		d1(kprintf("%s/%s/%ld: <%s>  number=%ld  type=%ld  cn_size=%ld\n", __FILE__, __FUNC__, __LINE__, \
			patNode->ptn_filename, patNode->ptn_number, patNode->ptn_type, cn->cn_Size));

		// do not free patNode
		patNode = NULL;
		} while (0);

	if (patNode)
		SCA_FreeNode((struct ScalosNodeList *)(APTR) &PatternNodes, &patNode->ptn_Node);
	if (pPrefs)
		ScalosFree(pPrefs);
}


static void ReadPatternDefaults(struct IFFHandle *iff)
{
	LONG Len;
	struct ScaPatternDefs pdp;

	Len = ReadChunkBytes(iff, &pdp, sizeof(pdp));
	if (Len <= 0)
		return;

	pdp.scd_Flags = SCA_BE2WORD(pdp.scd_Flags);
	pdp.scd_WorkbenchPattern = SCA_BE2WORD(pdp.scd_WorkbenchPattern);
	pdp.scd_ScreenPattern = SCA_BE2WORD(pdp.scd_ScreenPattern);
	pdp.scd_WindowPattern = SCA_BE2WORD(pdp.scd_WindowPattern);
	pdp.scd_TextModePattern = SCA_BE2WORD(pdp.scd_TextModePattern);

	d1(kprintf("%s/%s/%ld: Len=%ld\n", __FILE__, __FUNC__, __LINE__, Len));

	PatternPrefs.patt_AsyncBackFill = 0 != (pdp.scd_Flags & SCDF_ASYNCLAYOUT);
	PatternPrefs.patt_UseFriendBitMap = 0 != (pdp.scd_Flags & SCDF_USEFRIENDBM);
	PatternPrefs.patt_NewRenderFlag = 0 != (pdp.scd_Flags & SCDF_RELAYOUT);
	PatternPrefs.patt_RandomizeEverytime = 0 != (pdp.scd_Flags & SCDF_RANDOM);

	d1(kprintf("%s/%s/%ld: PatternPrefs.patt_AsyncBackFill=%ld  PatternPrefs.patt_UseFriendBitMap=%ld\n", \
		__FILE__, __FUNC__, __LINE__, PatternPrefs.patt_AsyncBackFill, PatternPrefs.patt_UseFriendBitMap));
	d1(kprintf("%s/%s/%ld: PatternPrefs.patt_NewRenderFlag=%ld  PatternPrefs.patt_RandomizeEverytime=%ld\n", \
		__FILE__, __FUNC__, __LINE__, PatternPrefs.patt_NewRenderFlag, PatternPrefs.patt_RandomizeEverytime));

	PatternPrefs.patt_DefWBPatternNr = pdp.scd_WorkbenchPattern;
	PatternPrefs.patt_DefScreenPatternNr = pdp.scd_ScreenPattern;
	PatternPrefs.patt_DefWindowPatternNr = pdp.scd_WindowPattern;

	d1(kprintf("%s/%s/%ld: PatternPrefs.patt_DefWBPatternNr=%ld  PatternPrefs.patt_DefScreenPatternNr=%ld  PatternPrefs.patt_DefWindowPatternNr=%ld\n", \
		__FILE__, __FUNC__, __LINE__, PatternPrefs.patt_DefWBPatternNr, PatternPrefs.patt_DefScreenPatternNr, PatternPrefs.patt_DefWindowPatternNr));

	if (Len > offsetof(struct ScaPatternDefs, scd_TextModePattern))
		{
		PatternPrefs.patt_DefTextWinPatternNr = pdp.scd_TextModePattern;
		PatternPrefs.patt_AsyncProcTaskPri = pdp.scd_TaskPriority;

		d1(kprintf("%s/%s/%ld: PatternPrefs.patt_DefTextWinPatternNr=%ld  AsyncProcTaskPri=%ld\n", \
			__FILE__, __FUNC__, __LINE__, PatternPrefs.patt_DefTextWinPatternNr, PatternPrefs.patt_AsyncProcTaskPri));
		}
}


static SAVEDS(LONG) PatternNodeCompareFunc(
	struct Hook *theHook,
	struct PatternNode *pNode2,
	struct PatternNode *pNode1)
{
	(void) theHook;

	return pNode1->ptn_number - pNode2->ptn_number;
}


static void FreePatternNode(struct PatternNode **PatternNodeList, struct PatternNode *pNode)
{
	d1(kprintf("%s/%s/%ld: pNode=%08lx\n", __FILE__, __FUNC__, __LINE__, pNode));

	ScalosDeleteSemaphore(pNode->ptn_semaphore);

	if (pNode->ptn_bitmap && (pNode->ptn_flags & PTNF_FreeBitmap))
		FreeBitMap(pNode->ptn_bitmap);

	if (pNode->ptn_object)
		DisposeDTObject(pNode->ptn_object);

	if (GuiGFXBase && pNode->ptn_drawhandle)
		{
		ReleaseDrawHandle(pNode->ptn_drawhandle);

		d1(kprintf("%s/%s/%ld: ptn_colorhandle=%08lx\n", __FILE__, __FUNC__, __LINE__, pNode->ptn_colorhandle));

		if (pNode->ptn_colorhandle)
			RemColorHandle(pNode->ptn_colorhandle);
		if (pNode->ptn_picture)
			DeletePicture(pNode->ptn_picture);
		}

	if (pNode->ptn_filename)
		FreeCopyString(pNode->ptn_filename);

	SCA_FreeNode((struct ScalosNodeList *) PatternNodeList, &pNode->ptn_Node);
}


void FreePatternPrefs(void)
{
	struct PatternNode *pNode, *pNodeNext;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ScalosObtainSemaphore(&PatternSema);

	for (pNode = PatternNodes; pNode; pNode=pNodeNext)
		{
		pNodeNext = (struct PatternNode *) pNode->ptn_Node.mln_Succ;
		FreePatternNode(&PatternNodes, pNode);
		}

	ScalosReleaseSemaphore(&PatternSema);
}


ULONG RandomNumber(ULONG MaxRand)
{
	RandomSeed = RandomSeed * 1103515245 + 12345;
	return (ULONG)(RandomSeed / 65536) % (1 + MaxRand);
//	  RandomSeed = (RandomSeed * 51479u) + 3715436908u;
//
//	  return (((RandomSeed & 0xffff) * MaxRand) / 65535u) & 0xffff;
}


void NewPatternPrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct SM_NewPreferences *smnp;

	d1(KPrintF("%s/%s/%ld: smnp_PrefsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, SMNPFLAGF_PATTERNPREFS));

	smnp = (struct SM_NewPreferences *) SCA_AllocMessage(MTYP_NewPreferences, 0);
	if (smnp)
		{
		smnp->smnp_PrefsFlags = SMNPFLAGF_PATTERNPREFS;
		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smnp->ScalosMessage.sm_Message);
		}
}


BOOL ChangedPatternPrefs(struct MainTask *mt)
{
	ULONG NewPatternPrefsCRC;
	BOOL Changed = FALSE;

	NewPatternPrefsCRC = GetPrefsCRCFromName("ENV:Scalos/Pattern.prefs");
	d1(KPrintF("%s/%s/%ld: NewPatternPrefsCRC=%08lx  PatternPrefsCRC=%08lx\n",
		__FILE__, __FUNC__, __LINE__, NewPatternPrefsCRC, PatternPrefsCRC));

	if (INITIAL_CRC != NewPatternPrefsCRC &&
		NewPatternPrefsCRC != PatternPrefsCRC)
		{
		Changed = TRUE;
		PatternsOff(mt, iInfos.xii_iinfos.ii_MainMsgPort);
		FreePatternPrefs();
		ReadPatternPrefs();
		PatternsOn(mt);
		}
	else
		{
		RandomizePatterns();
		}

	return Changed;
}


void NewMainPrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct SM_NewPreferences *smnp;

	d1(KPrintF("%s/%s/%ld: smnp_PrefsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, SMNPFLAGF_SCALOSPREFS));

	smnp = (struct SM_NewPreferences *) SCA_AllocMessage(MTYP_NewPreferences, 0);
	if (smnp)
		{
		smnp->smnp_PrefsFlags = SMNPFLAGF_SCALOSPREFS;
		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smnp->ScalosMessage.sm_Message);
		}
}


BOOL ChangedMainPrefs(struct MainTask *mt)
{
	ULONG NewMainPrefsCRC;
	BOOL Changed = FALSE;

	NewMainPrefsCRC = GetPrefsCRCFromName("ENV:Scalos/Scalos.prefs");
	d1(KPrintF("%s/%s/%ld: NewMainPrefsCRC=%08lx  MainPrefsCRC=%08lx\n",
		__FILE__, __FUNC__, __LINE__, NewMainPrefsCRC, MainPrefsCRC));

	if (INITIAL_CRC != NewMainPrefsCRC &&
		NewMainPrefsCRC != MainPrefsCRC)
		{
		struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
		BOOL oldPrefFullBenchFlag, oldPrefPopTitleFlag;
		BOOL oldPrefMarkIconUnderMouse;
		UWORD oldPrefActiveWindowTransparency;
		UWORD oldPrefInactiveWindowTransparency;

		Changed = TRUE;

		oldPrefFullBenchFlag = CurrentPrefs.pref_FullBenchFlag;
		oldPrefPopTitleFlag = CurrentPrefs.pref_PopTitleFlag;
		oldPrefMarkIconUnderMouse = CurrentPrefs.pref_MarkIconUnderMouse;
		oldPrefActiveWindowTransparency	= CurrentPrefs.pref_ActiveWindowTransparency;
		oldPrefInactiveWindowTransparency = CurrentPrefs.pref_InactiveWindowTransparency;

		ReadScalosPrefs();

		if (oldPrefFullBenchFlag != CurrentPrefs.pref_FullBenchFlag
			|| oldPrefPopTitleFlag != CurrentPrefs.pref_PopTitleFlag
			|| oldPrefMarkIconUnderMouse != CurrentPrefs.pref_MarkIconUnderMouse)
			{
			if (CurrentPrefs.pref_FullBenchFlag)
				{
				FreeBackFill(&iwtMain->iwt_WindowTask.wt_PatternInfo);

				iwtMain->iwt_WindowTask.wt_PatternInfo.ptinf_height = iwtMain->iwt_WinScreen->Height;
				iInfos.xii_iinfos.ii_MainWindowStruct->ws_Top = 0;
				iInfos.xii_iinfos.ii_MainWindowStruct->ws_Height = 
					iwtMain->iwt_WinScreen->Height - iInfos.xii_iinfos.ii_MainWindowStruct->ws_Top;

				ChangeWindowBox(iwtMain->iwt_WindowTask.wt_Window,
					iwtMain->iwt_WindowTask.wt_Window->LeftEdge,
					iInfos.xii_iinfos.ii_MainWindowStruct->ws_Top,
					iwtMain->iwt_WindowTask.wt_Window->Width,
					iInfos.xii_iinfos.ii_MainWindowStruct->ws_Height);

				SetBackFill(iwtMain, iInfos.xii_iinfos.ii_MainWindowStruct->ws_PatternNode,
					&iwtMain->iwt_WindowTask.wt_PatternInfo,
                                        0,
					iwtMain->iwt_WinScreen);
				}
			else
				{
				FreeBackFill(&iwtMain->iwt_WindowTask.wt_PatternInfo);

				iwtMain->iwt_WindowTask.wt_PatternInfo.ptinf_height =
					iwtMain->iwt_WinScreen->Height - iwtMain->iwt_WinScreen->BarHeight + 1;

				iInfos.xii_iinfos.ii_MainWindowStruct->ws_Top = iwtMain->iwt_WinScreen->BarHeight + 1;
				iInfos.xii_iinfos.ii_MainWindowStruct->ws_Height = 
					iwtMain->iwt_WinScreen->Height - iInfos.xii_iinfos.ii_MainWindowStruct->ws_Top;

				ChangeWindowBox(iwtMain->iwt_WindowTask.wt_Window,
					iwtMain->iwt_WindowTask.wt_Window->LeftEdge,
					iInfos.xii_iinfos.ii_MainWindowStruct->ws_Top,
					iwtMain->iwt_WindowTask.wt_Window->Width,
					iInfos.xii_iinfos.ii_MainWindowStruct->ws_Height);

				SetBackFill(iwtMain, iInfos.xii_iinfos.ii_MainWindowStruct->ws_PatternNode,
					&iwtMain->iwt_WindowTask.wt_PatternInfo,
					0,
					iwtMain->iwt_WinScreen);

				CurrentPrefs.pref_PopTitleFlag = FALSE;
				}

			d1(KPrintF("%s/%s/%ld: oldPrefMarkIconUnderMouse=[%ld] CurrentPrefs.pref_MarkIconUnderMouse=[%ld] [] \n", \
					__FILE__, __FUNC__, __LINE__, oldPrefMarkIconUnderMouse, CurrentPrefs.pref_MarkIconUnderMouse));

			InitCx();

			ShowScreenTitle(!CurrentPrefs.pref_PopTitleFlag ? !CurrentPrefs.pref_FullBenchFlag : CurrentPrefs.pref_FullBenchFlag);	  // +jmc+

			d1(KPrintF("%s/%s/%ld:\nSTART\n   ShowScreenTitle(!CurrentPrefs.pref_PopTitleFlag)=[%ld]\n   CurrentPrefs.pref_PopTitleFlag=[%ld]\n   CurrentPrefs.pref_FullBenchFlag=[%ld]\nEND\n\n", \
					__FILE__, __FUNC__, __LINE__, (!CurrentPrefs.pref_PopTitleFlag ? !CurrentPrefs.pref_FullBenchFlag : CurrentPrefs.pref_FullBenchFlag), CurrentPrefs.pref_PopTitleFlag, CurrentPrefs.pref_FullBenchFlag));
			}

		// Update iwt_IconLayoutInfo and transparency for all windows
		if (SCA_LockWindowList(SCA_LockWindowList_AttemptShared))
			{
			struct ScaWindowStruct *ws;

			for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
				{
				struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
				const UBYTE *LayoutModes;

				// if default transparency is used, update window transparency
				if (oldPrefActiveWindowTransparency == ws->ws_WindowOpacityActive)
					{
					ws->ws_WindowOpacityActive = CurrentPrefs.pref_ActiveWindowTransparency;
					SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
						SCCA_IconWin_ActiveTransparency, CurrentPrefs.pref_ActiveWindowTransparency,
						TAG_END);
					}
				if (oldPrefInactiveWindowTransparency == ws->ws_WindowOpacityInactive)
					{
					SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
						SCCA_IconWin_InactiveTransparency, CurrentPrefs.pref_InactiveWindowTransparency,
						TAG_END);
					}

				if (WSV_Type_DeviceWindow == ws->ws_WindowType)
					LayoutModes = CurrentPrefs.pref_DeviceWindowLayoutModes;
				else
					LayoutModes = CurrentPrefs.pref_IconWindowLayoutModes;

				memcpy(iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes, 
					LayoutModes, sizeof(iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes));
				}
			SCA_UnLockWindowList();
			}

		// Screen TrueType Font
		Scalos_CloseFont(NULL, &ScreenTTFont);
		if (CurrentPrefs.pref_UseScreenTTFont)
			Scalos_OpenTTFont(CurrentPrefs.pref_ScreenTTFontDescriptor, &ScreenTTFont);

		// Text window font
		OpenTextWindowFont();

		d1(KPrintF("%s/%s/%ld: iwtMain=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtMain));

		// update icon window font
		OpenIconWindowFont();

		d1(KPrintF("%s/%s/%ld: IconWindowFont=%08lx\n", __FILE__, __FUNC__, __LINE__, IconWindowFont));

		if (IsIwtViewByIcon(iwtMain))
			{
			iwtMain->iwt_IconFont = IconWindowFont;
			iwtMain->iwt_IconTTFont = IconWindowTTFont;
			}
		else
			{
			iwtMain->iwt_IconFont = TextWindowFont;
			iwtMain->iwt_IconTTFont = TextWindowTTFont;
			}

		// AppIcons will not get re-created like other icons,
		// so we do a ReLayout here
		DoMethod(iwtMain->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Redraw, REDRAWF_ReLayoutIcons);

		// Force flushing of cached FileType descriptors
		// to get rid of still open fonts
		FileTypeFlush(FALSE);
		}

	return Changed;
}


void NewPalettePrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct MainTask *mt = MainWindowTask;

	d1(KPrintF("%s/%s/%ld: smnp_PrefsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, SMNPFLAGF_PALETTEPREFS));

	if (mt->mt_ChangedPalettePrefsName)
		FreeCopyString(mt->mt_ChangedPalettePrefsName);
	mt->mt_ChangedPalettePrefsName = AllocCopyString(msg->nm_NReq->nr_FullName);

	if (mt->mt_ChangedPalettePrefsName)
		{
		struct SM_NewPreferences *smnp;

		smnp = (struct SM_NewPreferences *) SCA_AllocMessage(MTYP_NewPreferences, 0);
		if (smnp)
			{
			smnp->smnp_PrefsFlags = SMNPFLAGF_PALETTEPREFS;
			PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smnp->ScalosMessage.sm_Message);
			}
		}
}


BOOL ChangedPalettePrefs(struct MainTask *mt)
{
	BOOL Changed = FALSE;

	if (mt->emulation)
		{
		ULONG NewPalettePrefsCRC;

		NewPalettePrefsCRC = GetPrefsCRCFromName(mt->mt_ChangedPalettePrefsName);
		d1(KPrintF("%s/%s/%ld: NewPalettePrefsCRC=%08lx  PalettePrefsCRC=%08lx\n",
			__FILE__, __FUNC__, __LINE__, NewPalettePrefsCRC, PalettePrefsCRC));

		if (INITIAL_CRC != NewPalettePrefsCRC &&
			NewPalettePrefsCRC != PalettePrefsCRC)
			{
			Changed = TRUE;
			RunProcess(&mt->mwt.iwt_WindowTask, NewPaletteProc, 0, NULL, iInfos.xii_iinfos.ii_MainMsgPort);
			}
		}

	return Changed;
}


static ULONG NewPaletteProc(APTR arg, struct SM_RunProcess *msg)
{
	BOOL oldCloseWBDisabled;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	oldCloseWBDisabled = CurrentPrefs.pref_DisableCloseWorkbench;
	CurrentPrefs.pref_DisableCloseWorkbench = FALSE;

	while (1)
		{
		if (CloseWorkBench())
			{
			break;
			}
		else
			{
			if (!UseRequest(NULL, MSGID_CLOSEWBNAME, MSGID_GADGETSNAME, NULL))
				break;
			}
		}

	CurrentPrefs.pref_DisableCloseWorkbench = oldCloseWBDisabled;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	FreePalettePrefs();
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	ReadPalettePrefs();
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	OpenWorkBench();

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


void NewFontPrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct SM_NewPreferences *smnp;

	d1(KPrintF("%s/%s/%ld: smnp_PrefsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, SMNPFLAGF_FONTPREFS));

	smnp = (struct SM_NewPreferences *) SCA_AllocMessage(MTYP_NewPreferences, 0);
	if (smnp)
		{
		smnp->smnp_PrefsFlags = SMNPFLAGF_FONTPREFS;
		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smnp->ScalosMessage.sm_Message);
		}
}


BOOL ChangedFontPrefs(struct MainTask *mt)
{
	ULONG NewFontPrefsCRC;
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	BOOL Changed = FALSE;

	NewFontPrefsCRC = GetPrefsCRCFromName("ENV:sys/font.prefs");
	d1(KPrintF("%s/%s/%ld: NewFontPrefsCRC=%08lx  FontPrefsCRC=%08lx\n",
		__FILE__, __FUNC__, __LINE__, NewFontPrefsCRC, FontPrefsCRC));

	if (NewFontPrefsCRC != FontPrefsCRC)
		{
		d1(KPrintF("%s/%s/%ld: Changed font prefs detected\n", __FILE__, __FUNC__, __LINE__));

		Changed = TRUE;

		ReadFontPrefs();

		OpenIconWindowFont();

		d1(KPrintF("%s/%s/%ld: IconWindowFont=%08lx\n", __FILE__, __FUNC__, __LINE__, IconWindowFont));

		if (IsIwtViewByIcon(iwtMain))
			{
			iwtMain->iwt_IconFont = IconWindowFont;
			iwtMain->iwt_IconTTFont = IconWindowTTFont;
			}
		else
			{
			iwtMain->iwt_IconFont = TextWindowFont;
			iwtMain->iwt_IconTTFont = TextWindowTTFont;
			}

		// AppIcons will not get re-created like other icons,
		// so we do a ReLayout here
		DoMethod(iwtMain->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Redraw, REDRAWF_ReLayoutIcons);
		}

	return Changed;
}


static void ShowScreenTitle(BOOL showTitle)
{
	struct SM_ShowTitle *sMsg;

	d1(kprintf("%s/%s/%ld: Screen Title ON\n", __FILE__, __FUNC__, __LINE__));

	sMsg = (struct SM_ShowTitle *) SCA_AllocMessage(MTYP_ShowTitle, 0);
	if (sMsg)
		{
		sMsg->smst_showTitle = showTitle;
		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &sMsg->ScalosMessage.sm_Message);
		}
}


ULONG GetPrefsCRCFromFH(BPTR fh)
{
	UBYTE buffer[256];
	ULONG crc = INITIAL_CRC;
	SLONG64	FilePos;
	LONG len;

	if (BNULL == fh)
		return 0;

	d1(KPrintF("%s/%s/%ld: START  fh=%08lx\n", __FILE__, __FUNC__, __LINE__, fh));
	FilePos = ScalosSeek(fh, MakeS64(0), OFFSET_BEGINNING);

	while ((len = FRead(fh, buffer, 1, sizeof(buffer))) > 0)
		{
		crc = update_crc(crc, buffer, len);
		}
	ScalosSeek(fh, FilePos, OFFSET_BEGINNING);

	d1(KPrintF("%s/%s/%ld: END  fh=%08lx  crc=%08lx\n", __FILE__, __FUNC__, __LINE__, fh, crc));

	return crc;
}


ULONG GetPrefsCRCFromName(CONST_STRPTR FileName)
{
	ULONG crc = 0;
	BPTR fh;

	fh = Open((STRPTR) FileName, MODE_OLDFILE);
	d1(KPrintF("%s/%s/%ld: FileName=<%s>  fh=%08lx\n", __FILE__, __FUNC__, __LINE__, FileName, fh));
	if (fh)
		{
		crc = GetPrefsCRCFromFH(fh);
		Close(fh);
		}

	d1(KPrintF("%s/%s/%ld: END  crc=%08lx\n", __FILE__, __FUNC__, __LINE__, crc));

	return crc;
}


static void SetTextAttr(struct ScalosTextAttr *sta, struct PrefsStruct *ps)
{
	CONST_STRPTR fontName = (CONST_STRPTR) PS_DATA(ps);
	CONST_STRPTR fp;
	size_t fnLen;
	LONG l;

	if (sta->sta_AllocName)
		{
		ScalosFree(sta->sta_AllocName);
		sta->sta_AllocName = NULL;
		}

	for (fp=fontName, fnLen=0; *fp && '/' != *fp; fnLen++)
		fp++;

	sta->sta_TextAttr.ta_Name = sta->sta_AllocName = ScalosAlloc(fnLen + 5 + 1);
	if (sta->sta_TextAttr.ta_Name)
		{
		stccpy(sta->sta_TextAttr.ta_Name, fontName, 1 + fnLen);

		// append ".font" if not already present
		if (0 != strcmp(sta->sta_TextAttr.ta_Name + fnLen - 5, ".font"))
			strcat(sta->sta_TextAttr.ta_Name, ".font");
		}

	if ('/' == *fp)
		fp++;

	if (StrToLong((STRPTR) fp, &l))
		sta->sta_TextAttr.ta_YSize = l;
	else
		sta->sta_TextAttr.ta_YSize = 8;
}


void OpenIconWindowFont(void)
{
	static struct TextAttr Topaz8Attr = { "topaz.font", 8, FS_NORMAL, FPF_ROMFONT };

	Scalos_CloseFont(&IconWindowFont, &IconWindowTTFont);

	d1(KPrintF("%s/%s/%ld: %s/%ld\n", __FILE__, __FUNC__, __LINE__, FontPrefs.fprf_TextAttr.ta_Name, FontPrefs.fprf_TextAttr.ta_YSize));

	IconWindowFont = OpenDiskFont(&FontPrefs.fprf_TextAttr);

	d1(KPrintF("%s/%s/%ld: IconWindowFont=%08lx\n", __FILE__, __FUNC__, __LINE__, IconWindowFont));
	if (NULL == IconWindowFont)
		IconWindowFont = OpenFont(&Topaz8Attr);

	// Icon Window TrueType Font
	if (CurrentPrefs.pref_UseIconTTFont)
		Scalos_OpenTTFont(CurrentPrefs.pref_IconTTFontDescriptor, &IconWindowTTFont);
}


void OpenTextWindowFont(void)
{
	static struct TextAttr Topaz8Attr = { "topaz.font", 8, FS_NORMAL, FPF_ROMFONT };

	Scalos_CloseFont(&TextWindowFont, &TextWindowTTFont);

	d1(KPrintF("%s/%s/%ld: %s/%ld\n", __FILE__, __FUNC__, __LINE__, CurrentPrefs.pref_TextModeTextAttr.sta_TextAttr.ta_Name, CurrentPrefs.pref_TextModeTextAttr.sta_TextAttr.ta_YSize));

	TextWindowFont = OpenDiskFont(&CurrentPrefs.pref_TextModeTextAttr.sta_TextAttr);
	d1(KPrintF("%s/%s/%ld: TextWindowFont=%08lx\n", __FILE__, __FUNC__, __LINE__, TextWindowFont));

	if (NULL == TextWindowFont)
		TextWindowFont = OpenFont(&Topaz8Attr);

	if (CurrentPrefs.pref_UseTextWindowTTFont)
		Scalos_OpenTTFont(CurrentPrefs.pref_TextWindowTTFontDescriptor, &TextWindowTTFont);
}


static void CleanupControlBarGadgetsList(struct List *CbGadgetsList)
{
	struct ControlBarGadgetEntry *cgy;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	while ((cgy  = (struct ControlBarGadgetEntry *) RemHead(CbGadgetsList)))
		{
		if (cgy->cgy_NormalImage)
			{
			FreeCopyString(cgy->cgy_NormalImage);
			cgy->cgy_NormalImage = NULL;
			}
		if (cgy->cgy_SelectedImage)
			{
			FreeCopyString(cgy->cgy_SelectedImage);
			cgy->cgy_SelectedImage = NULL;
			}
		if (cgy->cgy_DisabledImage)
			{
			FreeCopyString(cgy->cgy_DisabledImage);
			cgy->cgy_DisabledImage = NULL;
			}
		if (cgy->cgy_HelpText)
			{
			FreeCopyString(cgy->cgy_HelpText);
			cgy->cgy_HelpText = NULL;
			}
		ScalosFree(cgy);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void ReadControlBarGadgetList(APTR p_MyPrefsHandle, LONG lID, struct List *CbGadgetsList,
	ULONG prefsIDGadgets, ULONG prefsIDStrings)
{
	struct SCP_GadgetStringEntry *gse;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct SCP_GadgetStringEntry gseTemp;
		struct SCP_GadgetEntry sgy;
		size_t size;
		LONG Entry;

		CleanupControlBarGadgetsList(CbGadgetsList);

		memset(&gseTemp, 0, sizeof(gseTemp));
		Entry = 0;

		// first determine required size of prefsIDStrings
		GetPreferences(p_MyPrefsHandle, lID, prefsIDStrings, &gseTemp, sizeof(gseTemp) );
		gseTemp.gse_Length = SCA_BE2LONG(gseTemp.gse_Length);
		size = sizeof(struct SCP_GadgetStringEntry) + gseTemp.gse_Length;
		d1(KPrintF("%s/%s/%ld: gse_Length=%lu  size=%lu\n", __FILE__, __FUNC__, __LINE__, gseTemp.gse_Length, size));

		gse = ScalosAlloc(size);
		d1(KPrintF("%s/%s/%ld: gse=%08lx\n", __FILE__, __FUNC__, __LINE__, gse));
		if (NULL == gse)
			break;

		// now read complete prefsIDStrings
		GetPreferences(p_MyPrefsHandle, lID, prefsIDStrings, gse, size );
		gse->gse_Length = SCA_BE2LONG(gse->gse_Length);

		// read array of prefsIDGadgets
		while ((size = GetEntry(p_MyPrefsHandle, lID, prefsIDGadgets, &sgy, sizeof(sgy), Entry)) > 0)
			{
			struct ControlBarGadgetEntry *cgy;

			d1(KPrintF("%s/%s/%ld: size=%lu\n", __FILE__, __FUNC__, __LINE__, size));

			d1(KPrintF("%s/%s/%ld: sgy_Action=<%s>\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_Action));
			d1(KPrintF("%s/%s/%ld: sgy_NormalImageIndex=%lu  sgy_SelectedImageIndex=%lu\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_NormalImageIndex, sgy.sgy_SelectedImageIndex));
			d1(KPrintF("%s/%s/%ld: sgy_DisabledImageIndex=%lu  sgy_HelpTextIndex=%lu\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_DisabledImageIndex, sgy.sgy_HelpTextIndex));

			cgy = ScalosAlloc(sizeof(struct ControlBarGadgetEntry));
			d1(KPrintF("%s/%s/%ld: cgy=%08lx  Entry=%ld  sgy_GadgetType=%ld\n", __FILE__, __FUNC__, __LINE__, cgy, Entry, sgy.sgy_GadgetType));
			if (NULL == cgy)
				break;

			// create ControlBarGadgetEntry from SCP_GadgetEntry
			cgy->cgy_GadgetType = SCA_BE2WORD(sgy.sgy_GadgetType);
			stccpy(cgy->cgy_Action, sgy.sgy_Action, sizeof(cgy->cgy_Action));
			cgy->cgy_NormalImage = AllocCopyString(&gse->gse_Strings[SCA_BE2LONG(sgy.sgy_NormalImageIndex)]);
			cgy->cgy_SelectedImage = AllocCopyString(&gse->gse_Strings[SCA_BE2LONG(sgy.sgy_SelectedImageIndex)]);
			cgy->cgy_DisabledImage = AllocCopyString(&gse->gse_Strings[SCA_BE2LONG(sgy.sgy_DisabledImageIndex)]);
			cgy->cgy_HelpText = AllocCopyString(&gse->gse_Strings[SCA_BE2LONG(sgy.sgy_HelpTextIndex)]);

			// add new ControlBarGadgetEntry to CbGadgetsList
			AddTail(CbGadgetsList, &cgy->cgy_Node);

			Entry++;
			}
		} while (0);

	if (gse)
		ScalosFree(gse);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

#if 0
static void ByteDump(unsigned char *Data, size_t Length)
{
	unsigned long count;
	unsigned char Line[80], *lp;

	lp = Line;
	for (count=0; count<Length; count++)
		{
		*lp++ = isprint(*Data) ? *Data : '.';
		KPrintF("%02x ", *Data++);
		if ((count+1) % 16 == 0)
			{
			*lp = '\0';
			lp = Line;
			KPrintF("\t%s\n", Line);
			}
		}

	*lp = '\0';
	while (count % 16 != 0)
		{
		KPrintF("   ");
		count++;
		}
	KPrintF("\t%s\n", Line);
}
#endif

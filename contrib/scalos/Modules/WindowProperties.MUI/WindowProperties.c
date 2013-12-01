// WindowProperties.c
// $Date$
// $Revision$


#ifdef __AROS__
#define MUIMASTER_YES_INLINE_STDARG
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <dos/dos.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <devices/clipboard.h>
#include <devices/timer.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/classusr.h>
#include <prefs/prefhdr.h>
#include <prefs/wbpattern.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/preferences.h>
#include <scalos/pattern.h>
#include <scalos/scalosgfx.h>

#include <clib/alib_protos.h>

#include <mui/NListview_mcc.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/iffparse.h>
#include <proto/iconobject.h>
#include <proto/scalos.h>
#include <proto/preferences.h>
#include <proto/scalosgfx.h>
#include <proto/timer.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include "WindowProperties.h"
#include <IconobjectMCC.h>
#include "BitMapMCC.h"
#include "Backfill.h"
#include <ToolTypes.h>
#include "debug.h"

#define	CATCOMP_NUMBERS
#define	CATCOMP_ARRAY
#define	CATCOMP_CODE
#define	WindowProperties_NUMBERS
#define WindowProperties_ARRAY
#define WindowProperties_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#define	VERSION_MAJOR	40
#define	VERSION_MINOR	9

//----------------------------------------------------------------------------

#define	ID_MAIN	MAKE_ID('M','A','I','N')
#define	PS_DATA(prefsstruct)	((STRPTR) (prefsstruct)) + sizeof((prefsstruct)->ps_Size)

#define	MAX_FILENAME	512

// dimensions of empty thumbnail image
#define	THUMBNAIL_WIDTH		50
#define	THUMBNAIL_HEIGHT	50

//----------------------------------------------------------------------------

long _stack = 16384;		// minimum stack size, used by SAS/C startup code

//----------------------------------------------------------------------------

// local data structures

struct TempRastPort
	{
	struct RastPort trp_rp;
	ULONG trp_Width;
	};

struct ThumbnailLifetimeSliderInst
	{
	char buf[20];
	};

struct PatternEntryDef
	{
	struct Node ped_Node;
	struct ScaExtPatternPrefs ped_PatternPrefs;
	Object *ped_MUI_ImageObject;
	Object *ped_MUI_AllocatedImageObject;
	ULONG	ped_ThumbnailImageNr;
	struct ScalosBitMapAndColor *ped_SAC;
	UBYTE *ped_BitMapArray;
	};

struct PatternListEntry
	{
	UWORD ple_PatternNumber;
	char ple_ImageNrBuffer[25];
	STRPTR ple_ImageDescBuffer;
	ULONG ple_PatternCount;
	struct List ple_PatternList;
	};

#ifdef __AROS__
#define ThumbnailLifetimeSliderObject BOOPSIOBJMACRO_START(ThumbnailLifetimeSliderClass->mcc_Class)
#else
#define ThumbnailLifetimeSliderObject NewObject(ThumbnailLifetimeSliderClass->mcc_Class, 0
#endif

#define KeyButtonHelp(name,key,HelpText)\
	TextObject,\
		ButtonFrame,\
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp, HelpText,\
		MUIA_CycleChain, TRUE, \
		End

#define CheckMarkHelp(selected, HelpTextID)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_InputMode        , MUIV_InputMode_Toggle,\
		MUIA_Image_Spec       , MUII_CheckMark,\
		MUIA_Image_FreeVert   , TRUE,\
		MUIA_Selected         , selected,\
		MUIA_Background       , MUII_ButtonBack,\
		MUIA_ShowSelState     , FALSE,\
		MUIA_CycleChain       , TRUE,\
		MUIA_ShortHelp	      , GetLocString(HelpTextID), \
		End

#define	Application_Return_Ok	1001

#define THUMBNAIL_LIFETIME_NOTSET	366
#define	CHECKOVERLAP_NOTSET		2

//----------------------------------------------------------------------------

// local functions

static void init(void);
static void fail(APTR APP_Main, CONST_STRPTR str);
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutFunc(struct Hook *hook, Object *o, Msg msg);
static STRPTR GetLocString(ULONG MsgId);
static void TranslateStringArray(STRPTR *stringArray);
static void SaveSettings(Object *IconObj, struct ScaWindowStruct *ws);
static Object *GetIconObject(CONST_STRPTR IconName, BPTR DirLock);
static Object *GetDeviceIconObject(CONST_STRPTR IconName, CONST_STRPTR VolumeName, CONST_STRPTR DeviceName);
static BOOL isDiskWritable(BPTR dLock);
static ULONG CheckInfoData(const struct InfoData *info);
static BOOL IsDevice(struct WBArg *arg);
static void GetDeviceName(BPTR dLock, STRPTR DeviceName, size_t MaxLen);
static BOOL ReadScalosPrefs(void);
static CONST_STRPTR GetPrefsConfigString(APTR prefsHandle, ULONG Id, CONST_STRPTR DefaultString);
static void StripTrailingChar(STRPTR String, char CharToRemove);
static struct ScaWindowStruct *FindWindowByLock(BPTR xLock);
static void BuildDefVolumeNameNoSpace(STRPTR Buffer, CONST_STRPTR VolumeName, size_t MaxLength);
static SAVEDS(APTR) INTERRUPT PatternListConstructHookFunc(struct Hook *hook, Object *obj, struct NList_ConstructMessage *msg);
static SAVEDS(void) INTERRUPT PatternListDestructHookFunc(struct Hook *hook, Object *obj, struct NList_DestructMessage *msg);
static SAVEDS(ULONG) INTERRUPT PatternListDisplayHookFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm);
static SAVEDS(LONG) INTERRUPT PatternListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *msg);
static SAVEDS(void) INTERRUPT PatternPopupWindowHookFunc(struct Hook *hook, Object *pop, Object *win);
static SAVEDS(void) INTERRUPT NewPatternHookFunc(struct Hook *hook, Object *pop, Object *win);
static SAVEDS(void) INTERRUPT TogglePathHookFunc(struct Hook *hook, Object *o, Object *x);
static Object *CreateEmptyThumbnailImage(void);
static LONG ReadPatternPrefsFile(CONST_STRPTR Filename, BOOL Quiet);
static LONG ReadPrefsBitMap(struct IFFHandle *iff, struct PatternEntryDef *ped, struct TempRastPort *trp);
static void CreateThumbnailImages(void);
static void CreateThumbnailImage(struct PatternEntryDef *ped);
DISPATCHER_PROTO(ThumbnailLifetimeSlider);
static void SelectPattern(ULONG PatternNumber);
static void AddDefaultPatternEntry(void);
static struct PatternEntryDef *CreatePatternEntryDef(void);
static void SetIconSizeConstraints(const struct Rectangle *SizeConstraints);
static void GetIconSizeConstraints(struct Rectangle *SizeConstraints);

//----------------------------------------------------------------------------

// local data items

static UBYTE prefDefIconsFirst = TRUE;		// Flag: try Def-Diskicons first
static CONST_STRPTR prefDefIconPath = "ENV:sys";
static UWORD prefsActiveWindowTransparency;
static UWORD prefsInactiveWindowTransparency;
static UWORD prefsIconScaleFactor;
static struct Rectangle prefIconSizeConstraints;

struct IntuitionBase *IntuitionBase = NULL;
struct Library *IconBase = NULL;
T_LOCALEBASE LocaleBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *IconobjectBase = NULL;
struct ScalosBase *ScalosBase = NULL;
struct Library *PreferencesBase = NULL;
struct GfxBase *GfxBase;
struct ScalosGfxBase *ScalosGfxBase = NULL;
struct Library *IFFParseBase;
struct Library *CyberGfxBase;
T_TIMERBASE TimerBase;

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition = NULL;
struct IconIFace *IIcon = NULL;
struct LocaleIFace *ILocale = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct IconobjectIFace *IIconobject = NULL;
struct ScalosIFace *IScalos = NULL;
struct PreferencesIFace *IPreferences = NULL;
struct ScalosGfxIFace *IScalosGfx = NULL;
struct GraphicsIFace *IGraphics = NULL;
struct IFFParseIFace *IIFFParse = NULL;
struct CyberGfxIFace *ICyberGfx = NULL;
struct TimerIFace *ITimer = NULL;
#endif

static struct Screen *WBScreen;

static struct Catalog *WindowPropertiesCatalog;

static struct Hook AboutHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(OpenAboutFunc),
	NULL
        };
static struct Hook AboutMUIHook =
	{ { NULL, NULL },
	HOOKFUNC_DEF(OpenAboutMUIFunc),
	NULL
        };

static struct Hook PatternListDisplayHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(PatternListDisplayHookFunc),
	NULL
	};
static struct Hook PatternListConstructHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(PatternListConstructHookFunc),
	NULL
	};
static struct Hook PatternListDestructHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(PatternListDestructHookFunc),
	NULL
	};
static struct Hook PatternListCompareHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(PatternListCompareHookFunc),
	NULL
	};
static struct Hook PatternPopupWindowHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(PatternPopupWindowHookFunc),
	NULL
	};
static struct Hook NewPatternHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(NewPatternHookFunc),
	NULL
	};
static struct Hook TogglePathHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(TogglePathHookFunc),
	NULL
	};

static STRPTR cShowThumbnails[] =
	{
	(STRPTR) MSGID_SHOWTHUMBNAILS_NOTSET,
	(STRPTR) MSGID_SHOWTHUMBNAILS_NEVER,
	(STRPTR) MSGID_SHOWTHUMBNAILS_AS_DEFAULT,
	(STRPTR) MSGID_SHOWTHUMBNAILS_ALWAYS,
	NULL
	};

static STRPTR cCheckOverlap[] =
	{
	(STRPTR) MSGID_CHECKOVERLAP_NO,
	(STRPTR) MSGID_CHECKOVERLAP_YES,
	(STRPTR) MSGID_CHECKOVERLAP_NOTSET,
	NULL
	};

static STRPTR cHideStatusBar[] =
	{
	(STRPTR) MSGID_HIDESTATUSBAR_NOTSET,
	(STRPTR) MSGID_HIDESTATUSBAR_HIDE,
	NULL
	};

static STRPTR cHideControlBar[] =
	{
	(STRPTR) MSGID_HIDECONTROLBAR_NOTSET,
	(STRPTR) MSGID_HIDECONTROLBAR_HIDE,
	NULL
	};

static STRPTR cRegisterTitleStrings[] =
	{
	(STRPTR) MSGID_REGISTERTITLE_WINDOW,
	(STRPTR) MSGID_REGISTERTITLE_ICONS,
	NULL
	};

static STRPTR cIconSizesMin[] =
{
	(STRPTR) MSGID_ICONSIZES_UNLIMITED,
	(STRPTR) MSGID_ICONSIZES_16x16,
	(STRPTR) MSGID_ICONSIZES_24x24,
	(STRPTR) MSGID_ICONSIZES_32x32,
	(STRPTR) MSGID_ICONSIZES_48x48,
	(STRPTR) MSGID_ICONSIZES_64x64,
	(STRPTR) MSGID_ICONSIZES_96x96,
	(STRPTR) MSGID_ICONSIZES_128x128,
	NULL
};

static STRPTR cIconSizesMax[] =
{
	(STRPTR) MSGID_ICONSIZES_16x16,
	(STRPTR) MSGID_ICONSIZES_24x24,
	(STRPTR) MSGID_ICONSIZES_32x32,
	(STRPTR) MSGID_ICONSIZES_48x48,
	(STRPTR) MSGID_ICONSIZES_64x64,
	(STRPTR) MSGID_ICONSIZES_96x96,
	(STRPTR) MSGID_ICONSIZES_128x128,
	(STRPTR) MSGID_ICONSIZES_UNLIMITED,
	NULL
};

static Object *Group_Buttons2;
static Object *APP_Main;
static Object *WIN_Main;
static Object *WIN_AboutMUI;
static Object *WIN_Progress;
static Object *OkButton, *CancelButton;
static Object *CycleMarkNoStatusBar;
static Object *CycleMarkNoControlBar;
static Object *NumericButtonThumbnailsLifetime;
static Object *CycleThumbnails;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit, *Path, *IconImage;
static Object *Group_Virtual;						//+jmc+ To switch 0/1 "MUIA_ShowMe" the ScroolGroupObject group.
static Object *CycleCheckOverlap;

static Object *PopObjectPatternNumber;
static Object *NListviewPatterns;
static Object *NListPatterns;
static Object *EmptyThumbnailBitmap;
static Object *GaugeProgress;
static Object *TextMsgProgress1, *TextMsgProgress2;
static Object *BackfillPatternPreview;
static Object *TextPatternNumber;

static Object *SliderTransparencyActiveWindow;
static Object *SliderTransparencyInactiveWindow;
static Object *CheckboxTransparencyActiveWindow;
static Object *CheckboxTransparencyInactiveWindow;
static Object *GroupTransparencyActiveWindow;
static Object *GroupTransparencyInactiveWindow;

static Object *SliderNominalIconSize;
static Object *CycleIconMinSize;
static Object *CycleIconMaxSize;
static Object *CheckboxNominalIconSize;
static Object *CheckboxIconSizeConstraints;
static Object *GroupNominalIconSize;
static Object *GroupIconSizeConstraints;

static struct MUI_CustomClass *IconobjectClass;
static struct MUI_CustomClass *ThumbnailLifetimeSliderClass;
struct MUI_CustomClass *BitMapPicClass;

static ULONG NewPatternNumber = 0;
static ULONG PatternNumber = 0;
static ULONG ThumbnailMode = 0;
static ULONG ThumbnailLifetime = THUMBNAIL_LIFETIME_NOTSET;
static ULONG CheckOverlap = CHECKOVERLAP_NOTSET;
static ULONG ActiveWindowTransparency = 100;
static ULONG InactiveWindowTransparency = 100;
static BOOL ActiveWindowTransparencyDefault = TRUE;
static BOOL InactiveWindowTransparencyDefault = TRUE;
static ULONG IconScaleFactor = 100;
static BOOL IconScaleFactorDefault = TRUE;
static struct Rectangle IconSizeConstraints = { 0, 0, SHRT_MAX, SHRT_MAX };
static BOOL IconSizeConstraintsDefault = TRUE;

static ULONG BitMapsRead = 0;
static ULONG ThumbnailImageNumber = 0;

static ULONG ThumbnailWidth = THUMBNAIL_WIDTH;
static ULONG ThumbnailHeight = THUMBNAIL_HEIGHT;

static struct ScaPatternDefs pDefs;

static T_TIMEREQUEST *TimerIORequest;
static char PathName[MAX_FILENAME];
static struct ScaWindowStruct *ws = NULL;

static struct PatternListEntry *AllPatterns[USHRT_MAX];

//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	LONG win_opened = 0;
	BPTR oldDir = (BPTR)NULL;
	CONST_STRPTR IconName = "";
	Object *iconObj = NULL;
	BOOL IsWriteable = TRUE;
	struct WBStartup *WBenchMsg =
		(argc == 0) ? (struct WBStartup *)argv : NULL;

	init();

	if (WBenchMsg && WBenchMsg->sm_ArgList)
		{
		struct WBArg *arg;

		if (WBenchMsg->sm_NumArgs > 1)
			{
			// Scalos typically calls this module with wa_Lock pointing to the 
			// window's directory, and empty wa_Name

			arg = &WBenchMsg->sm_ArgList[1];

			IconName = arg->wa_Name;

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: wa_Lock=%08lx  wa_Name=<%s>\n", \
				__LINE__, arg->wa_Lock, arg->wa_Name));
			debugLock_d1(arg->wa_Lock);

			oldDir = CurrentDir(arg->wa_Lock);

			if (IsDevice(arg))
				{
				static char VolumeName[256];
				char DeviceName[256];

				NameFromLock(arg->wa_Lock, VolumeName, sizeof(VolumeName));
				GetDeviceName(arg->wa_Lock, DeviceName, sizeof(DeviceName));

				StripTrailingChar(VolumeName, ':');
				StripTrailingChar(DeviceName, ':');

				iconObj = GetDeviceIconObject(IconName, VolumeName, DeviceName);

				if (iconObj)
					SetAttrs(iconObj, DTA_Name, "disk", TAG_END);

				IconName = VolumeName;
				stccpy(PathName, VolumeName, sizeof(PathName));
				}
			else
				{
				if (NULL == IconName || strlen(IconName) < 1)
					{
					static char DirName[1024];

					NameFromLock(arg->wa_Lock, DirName, sizeof(DirName));
					IconName = FilePart(DirName);

					iconObj = GetIconObject(DirName, arg->wa_Lock);
					}
				else
					{
					iconObj = GetIconObject(IconName, arg->wa_Lock);
					}
				NameFromLock(ParentDir(arg->wa_Lock), PathName, sizeof(PathName));
				}

			IsWriteable = isDiskWritable(arg->wa_Lock);

			ws = FindWindowByLock(arg->wa_Lock);
			d1(kprintf("%s/%s/%ld: ws=%08lx\n", __FILE__, __FUNC__, __LINE__, ws));
			}
		}

	d1(kprintf(__FUNC__ "/%ld: IconName=<%s>\n", __LINE__, IconName));

	if (strlen(IconName) > 0)
		{
		ULONG NoStatusBar = FALSE;
		ULONG NoControlBar = FALSE;

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx\n", __LINE__, iconObj));
		if (iconObj)
			{
			STRPTR tt;

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_THUMBNAIL_LIFETIME", &tt))
				{
				DoMethod(iconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &ThumbnailLifetime);
				}
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: tt=%08lx\n", __LINE__, tt));

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_PATTERNNO", &tt))
				{
				DoMethod(iconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &PatternNumber);
				NewPatternNumber = PatternNumber;
				}
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: tt=%08lx\n", __LINE__, tt));

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NOSTATUSBAR", &tt))
				NoStatusBar = TRUE;

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: tt=%08lx\n", __LINE__, tt));

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NOCONTROLBAR", &tt))
				NoControlBar = TRUE;

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: tt=%08lx\n", __LINE__, tt));

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_THUMBNAILS", &tt))
				{
				while (*tt && '=' !=  *tt)
					tt++;

				if ('=' ==  *tt)
					{
					++tt;	// Skip "="
					if (0 == Stricmp(tt, "NEVER"))
						ThumbnailMode = 1;
					else if (0 == Stricmp(tt, "ASDEFAULT"))
						ThumbnailMode = 2;
					else if (0 == Stricmp(tt, "ALWAYS"))
						ThumbnailMode = 3;
					}
				}

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_CHECKOVERLAP", &tt))
				{
				DoMethod(iconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &CheckOverlap);
				CheckOverlap = CheckOverlap != 0;	// allow only 0 or 1
				}

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_ACTIVETRANSPARENCY", &tt))
				{
				DoMethod(iconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &ActiveWindowTransparency);
				ActiveWindowTransparencyDefault = FALSE;
				}

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_INACTIVETRANSPARENCY", &tt))
				{
				DoMethod(iconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &InactiveWindowTransparency);
				InactiveWindowTransparencyDefault = FALSE;
				}

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_ICONSCALEFACTOR", &tt))
				{
				IconScaleFactorDefault = FALSE;

				DoMethod(iconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &IconScaleFactor);

				if (IconScaleFactor < IDTA_ScalePercentage_MIN)
					IconScaleFactor = IDTA_ScalePercentage_MIN;
				else if (IconScaleFactor > IDTA_ScalePercentage_MAX)
					IconScaleFactor = IDTA_ScalePercentage_MAX;

				d1(KPrintF("%s/%s/%ld: iwp_IconScaleFactor=%lu\n", __FILE__, __FUNC__, __LINE__, iwp->iwp_IconScaleFactor));
				}

			tt = NULL;
			if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_ICONSIZECONSTRAINTS", &tt))
				{
				long long1, long2;
				LONG IconSizeMin = 0, IconSizeMax = SHRT_MAX;

				IconSizeConstraintsDefault = FALSE;

				d1(KPrintF("%s/%s/%ld: tt=<%s>\n", __FILE__, __FUNC__, __LINE__, tt));
				while (*tt && '=' !=  *tt)
					tt++;

				if (2 == sscanf(tt, "=%ld,%ld", &long1, &long2))
					{
					IconSizeMin = long1;
					IconSizeMax = long2;

					if ((IconSizeMax > 128) || (IconSizeMax < 0))
						IconSizeMax = SHRT_MAX;
					if ((IconSizeMin < 0) || (IconSizeMin >= IconSizeMax))
						IconSizeMin = 0;

					IconSizeConstraints.MinX = IconSizeConstraints.MinY = IconSizeMin;
					IconSizeConstraints.MaxX = IconSizeConstraints.MaxY = IconSizeMax;
					}
				d1(KPrintF("%s/%s/%ld: IconSizeMin=%ld  IconSizeMax=%ld\n", __FILE__, __FUNC__, __LINE__, IconSizeMin, IconSizeMax));
				}
			}

		TranslateStringArray(cShowThumbnails);
		TranslateStringArray(cCheckOverlap);
		TranslateStringArray(cHideStatusBar);
		TranslateStringArray(cHideControlBar);
		TranslateStringArray(cIconSizesMin);
		TranslateStringArray(cIconSizesMax);
		TranslateStringArray(cRegisterTitleStrings);

		APP_Main = ApplicationObject,
			MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
			MUIA_Application_Version,	"$VER: Scalos WindowProperties.module V"
							STR(VERSION_MAJOR) "." STR(VERSION_MINOR)
							" (" __DATE__ ") " COMPILER_STRING,
			MUIA_Application_Copyright,	"The Scalos Team, 2004" CURRENTYEAR,
			MUIA_Application_Author,	"The Scalos Team",
			MUIA_Application_Description,	"Scalos Window Properties module",
			MUIA_Application_Base,		"SCALOS_WINDOW_PROP",

			SubWindow, WIN_Main = WindowObject,
				MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
//				MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
				MUIA_Window_AppWindow, TRUE,

				MUIA_Window_TopEdge, MUIV_Window_TopEdge_Moused,
				MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Moused,

				MUIA_Window_Width, MUIV_Window_Width_MinMax(0),
				MUIA_Window_Height, MUIV_Window_Height_MinMax(0),

				WindowContents, VGroup,
					Child, VGroup,
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, TextObject,
							MUIA_Text_PreParse, MUIX_C MUIX_B,
							MUIA_Text_Contents, IconName,
							TextFrame,
							MUIA_Background, MUII_TextBack,
							MUIA_ShortHelp, GetLocString(MSGID_TEXT_WINDOWNAME_SHORTHELP),
						End, //TextObject

						Child, TextObject,
							MUIA_ShowMe, !IsWriteable,
							MUIA_Text_PreParse, MUIX_C,
							MUIA_Text_Contents, GetLocString(MSGID_READ_ONLY),
						End, //TextObject


						Child, HGroup,

							Child, RectangleObject, End, //--- Instead: Child, VSpace(0)

							Child, IconImage = IconobjectMCCObject,
								MUIA_Iconobj_Object, iconObj,
								MUIA_InputMode, MUIV_InputMode_Toggle, //MUIV_InputMode_RelVerify,
								MUIA_ShowSelState, FALSE,
								MUIA_CycleChain, TRUE,
							End, //IconobjectMCCObject

							Child,	RegisterObject,
								MUIA_Register_Titles, cRegisterTitleStrings,
								MUIA_CycleChain, TRUE,

								Child,	VGroup,
									Child, HGroup,
										Child, Label1(GetLocString(MSGID_POPUP_PATTERNNUMBER)),
										Child, HGroup,
											MUIA_Disabled, !IsWriteable,

											Child, TextPatternNumber = TextObject,
												MUIA_Text_PreParse, MUIX_C,
												MUIA_Text_Contents, GetLocString(MSGID_PATTERNR_DEFAULT),
												End, //TextObject

											Child, BackfillPatternPreview = BackfillObject,
												ImageButtonFrame,
												MUIA_Background, MUII_ButtonBack,
												ImageButtonFrame,
												MUIA_InputMode, MUIV_InputMode_RelVerify,
												MUIA_CycleChain, TRUE,
												BFA_BitmapObject, EmptyThumbnailBitmap,
												End, //BackfillClass

											Child, PopObjectPatternNumber = PopobjectObject,
												MUIA_CycleChain, TRUE,
												MUIA_Popobject_WindowHook, &PatternPopupWindowHook,
												MUIA_Popstring_Button, PopButton(MUII_PopUp),
												MUIA_Popobject_Object, NListviewPatterns = NListviewObject,
													MUIA_Listview_DragType, MUIV_Listview_DragType_None,
													MUIA_Listview_Input, TRUE,
													MUIA_CycleChain, TRUE,
													MUIA_Listview_List, NListPatterns = NListObject,
														InputListFrame,
														MUIA_Background, MUII_ListBack,
														MUIA_NList_Format, "P=\33r BAR, BAR",
														MUIA_NList_TitleSeparator, FALSE,
														MUIA_NList_Title, FALSE,
														MUIA_NList_DisplayHook2, &PatternListDisplayHook,
														MUIA_NList_ConstructHook2, &PatternListConstructHook,
														MUIA_NList_DestructHook2, &PatternListDestructHook,
														MUIA_NList_CompareHook2, &PatternListCompareHook,
														MUIA_NList_PoolPuddleSize, sizeof(struct PatternListEntry) * 64,
														MUIA_NList_PoolThreshSize, sizeof(struct PatternListEntry),
														MUIA_NList_AutoVisible, TRUE,
														MUIA_NList_SortType, 0,
														MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 0,
														MUIA_NList_EntryValueDependent, TRUE,
														MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_None,
														End, //NListObject
													MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_FullAuto,
													End, //NListviewObject
												End, //PopobjectObject
											End, //HGroup
										End, //HGroup

									Child, HVSpace,

									Child, ColGroup(2),
										Child, Label1(GetLocString(MSGID_CYCLE_NOSTATUSBAR)),
										Child, CycleMarkNoStatusBar = CycleObject,
											MUIA_CycleChain, TRUE,
											MUIA_Cycle_Active, NoStatusBar,
											MUIA_Cycle_Entries, cHideStatusBar,
											MUIA_Disabled, !IsWriteable,
											MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CYCLE_NOSTATUSBAR_SHORTHELP),
											End, //CycleObject

										Child, Label1(GetLocString(MSGID_CYCLE_NOCONTROLBAR)),
										Child, CycleMarkNoControlBar = CycleObject,
											MUIA_CycleChain, TRUE,
											MUIA_Cycle_Active, NoControlBar,
											MUIA_Cycle_Entries, cHideControlBar,
											MUIA_Disabled, !IsWriteable,
											MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CYCLE_NOCONTROLBAR_SHORTHELP),
											End, //CycleObject

										End, //ColGroup

									Child, HVSpace,

									Child, VGroup,
#if defined(__MORPHOS__)
										MUIA_ShowMe, IntuitionBase->LibNode.lib_Version >= 51,
#elif !defined(__amigaos4__)
										MUIA_ShowMe, FALSE,
#endif //!defined(__MORPHOS__) && !defined(__amigaos4__)
										Child, VGroup,
											MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TRANSPARENCY_ACTIVEWINDOW),
											GroupFrame,
											MUIA_Background, MUII_GroupBack,

											MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TRANSPARENCY_ACTIVEWINDOW_SHORTHELP),

											Child, HGroup,
												Child, Label1((ULONG) GetLocString(MSGID_CHECKBOX_TRANSPARENCY_USEDEFAULT)),
												Child, CheckboxTransparencyActiveWindow = CheckMarkHelp(ActiveWindowTransparencyDefault, MSGID_CHECKBOX_TRANSPARENCY_USEDEFAULT_SHORTHELP),
												Child, HVSpace,
												End, //HGroup

											Child, GroupTransparencyActiveWindow = ColGroup(3),
												MUIA_Disabled, ActiveWindowTransparencyDefault,
												Child, Label1((ULONG) GetLocString(MSGID_TRANSPARENCY_TRANSPARENT)),

												Child, SliderTransparencyActiveWindow = SliderObject,
													MUIA_CycleChain, TRUE,
													MUIA_Numeric_Min, 0,
													MUIA_Numeric_Max, 100,
													MUIA_Slider_Horiz, TRUE,
													MUIA_Numeric_Value, ActiveWindowTransparency,
													End, //Slider

												Child, Label1((ULONG) GetLocString(MSGID_TRANSPARENCY_OPAQUE)),

												End, //ColGroup

											End, //VGroup

										Child, VGroup,
											MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TRANSPARENCY_INACTIVEWINDOW),
											GroupFrame,
											MUIA_Background, MUII_GroupBack,

											MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TRANSPARENCY_INACTIVEWINDOW_SHORTHELP),

											Child, HGroup,
												Child, Label1((ULONG) GetLocString(MSGID_CHECKBOX_TRANSPARENCY_USEDEFAULT)),
												Child, CheckboxTransparencyInactiveWindow = CheckMarkHelp(InactiveWindowTransparencyDefault, MSGID_CHECKBOX_TRANSPARENCY_USEDEFAULT_SHORTHELP),
												Child, HVSpace,
												End, //HGroup

											Child, GroupTransparencyInactiveWindow = ColGroup(3),
												MUIA_Disabled, InactiveWindowTransparencyDefault,
												Child, Label1((ULONG) GetLocString(MSGID_TRANSPARENCY_TRANSPARENT)),

												Child, SliderTransparencyInactiveWindow = SliderObject,
													MUIA_CycleChain, TRUE,
													MUIA_Numeric_Min, 0,
													MUIA_Numeric_Max, 100,
													MUIA_Slider_Horiz, TRUE,
													MUIA_Numeric_Value, InactiveWindowTransparency,
												End, //Slider

												Child, Label1((ULONG) GetLocString(MSGID_TRANSPARENCY_OPAQUE)),

												End, //ColGroup

											End, //VGroup

										End, //VGroup

									Child, HVSpace,
									End, //VGroup

								Child,	VGroup,
									Child, ColGroup(2),
										Child, Label1(GetLocString(MSGID_CYCLE_CHECKOVERLAP)),
										Child, CycleCheckOverlap = CycleObject,
											MUIA_CycleChain, TRUE,
											MUIA_Disabled, !IsWriteable,
											MUIA_Cycle_Entries, cCheckOverlap,
											MUIA_Cycle_Active, CheckOverlap,
											MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CYCLE_CHECKOVERLAP_SHORTHELP),
											End, //CycleObject

										Child, Label1(GetLocString(MSGID_CYCLE_THUMBNAILS)),
										Child, CycleThumbnails = CycleObject,
											MUIA_CycleChain, TRUE,
											MUIA_Disabled, !IsWriteable,
											MUIA_Cycle_Entries, cShowThumbnails,
											MUIA_Cycle_Active, ThumbnailMode,
											MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CYCLE_THUMBNAILS_SHORTHELP),
											End, //CycleObject

										Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_MAXAGE)),
										Child, NumericButtonThumbnailsLifetime = ThumbnailLifetimeSliderObject,
											MUIA_CycleChain, TRUE,
											MUIA_Numeric_Min, 0,
											MUIA_Numeric_Max, 366,
											MUIA_Slider_Horiz, TRUE,
											MUIA_Numeric_Value, ThumbnailLifetime,
											MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_MAXAGE_SHORTHELP),
											End, //ThumbnailLifetimeSliderClass
										End, //ColGroup

									Child, HVSpace,

									Child, VGroup,
										MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_ICONSCALING),
										GroupFrame,
										MUIA_Background, MUII_GroupBack,

										Child, HGroup,
											Child, Label1((ULONG) GetLocString(MSGID_CHECKBOX_ICONSCALING_NOMINALSIZE)),
											Child, CheckboxNominalIconSize  = CheckMarkHelp(IconScaleFactorDefault, MSGID_CHECKBOX_ICONSCALING_NOMINALSIZE_SHORTHELP),
											Child, HVSpace,
											End, //HGroup

										Child, GroupNominalIconSize = HGroup,
											MUIA_Disabled, IconScaleFactorDefault,
											MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSCALING_NOMINALSIZE_SHORTHELP),

											Child, Label1((ULONG) GetLocString(MSGID_ICONSCALING_NOMINALSIZE)),

											Child, SliderNominalIconSize = SliderObject,
												MUIA_CycleChain, TRUE,
												MUIA_Numeric_Min, IDTA_ScalePercentage_MIN,
												MUIA_Numeric_Max, IDTA_ScalePercentage_MAX,
												MUIA_Slider_Horiz, TRUE,
												MUIA_Numeric_Value, IconScaleFactor,
												End, //Slider

											Child, Label1((ULONG) GetLocString(MSGID_ICONSCALING_PERCENT)),
											End, //HGroup

										Child, HVSpace,

										Child, HGroup,
											Child, Label1((ULONG) GetLocString(MSGID_CHECKBOX_ICONSIZECONSTRAINTS_USEDEFAULT)),
											Child, CheckboxIconSizeConstraints  = CheckMarkHelp(IconSizeConstraintsDefault, MSGID_CHECKBOX_ICONSIZECONSTRAINTS_USEDEFAULT_SHORTHELP),
											Child, HVSpace,
											End, //HGroup

										Child, GroupIconSizeConstraints = HGroup,
											MUIA_Disabled, IconSizeConstraintsDefault,

											Child, HVSpace,

											Child, Label1((ULONG) GetLocString(MSGID_ICONSCALING_MINSIZE)),
											Child, CycleIconMinSize = CycleObject,
												MUIA_CycleChain, TRUE,
												MUIA_Cycle_Entries, cIconSizesMin,
												MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSCALING_MINSIZE_SHORTHELP),
												End, //Cycle

											Child, HVSpace,

											Child, Label1((ULONG) GetLocString(MSGID_ICONSCALING_MAXSIZE)),
											Child, CycleIconMaxSize = CycleObject,
												MUIA_CycleChain, TRUE,
												MUIA_Cycle_Entries, cIconSizesMax,
												MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSCALING_MAXSIZE_SHORTHELP),
												End, //Cycle

											Child, HVSpace,
											End, //HGroup

										End, //VGroup
									End, //VGroup

									Child, HVSpace,
								End, //VGroup

							End, //RegisterObject

						Child, Group_Virtual = ScrollgroupObject, // +jmc+
							MUIA_Scrollgroup_VertBar, NULL,
							MUIA_Scrollgroup_HorizBar, NULL,
							MUIA_Scrollgroup_FreeHoriz, TRUE,
							MUIA_Scrollgroup_FreeVert, FALSE,
							MUIA_Scrollgroup_Contents,
							VirtgroupObject,
								Child, Path = TextObject, TextFrame,
									MUIA_Background, MUII_TextBack,
									MUIA_Text_PreParse, MUIX_C,
									MUIA_Text_Contents, PathName,
									MUIA_ShortHelp, GetLocString(MSGID_TEXT_PARENTDIR_SHORTHELP),
									End, //TextObject
								End, //VirtgroupObject
							End, //ScrollgroupObject

						Child, HVSpace,

						End, //VGroup,

					Child, Group_Buttons2 = HGroup,
						MUIA_Group_SameWidth, TRUE,
						Child, OkButton = KeyButtonHelp(GetLocString(MSGID_OKBUTTON), 
									'o', GetLocString(MSGID_SHORTHELP_OKBUTTON)),
						Child, CancelButton = KeyButtonHelp(GetLocString(MSGID_CANCELBUTTON), 
									'c', GetLocString(MSGID_SHORTHELP_CANCELBUTTON)),
						End, //HGroup
					End, //VGroup
				End, //WindowObject

			SubWindow, WIN_Progress = WindowObject,
			//		MUIA_Window_Borderless, TRUE,
					MUIA_Window_Activate, FALSE,
					MUIA_Window_NoMenus, TRUE,
					MUIA_Window_Title, GetLocString(MSGID_WINDOW_STARTUP),
					WindowContents, VGroup,
						Child, GaugeProgress = GaugeObject,
							MUIA_Gauge_Horiz, TRUE,
							GaugeFrame,
							MUIA_Gauge_InfoText, " ",
							End, //GaugeObject
						Child, TextMsgProgress1 = TextObject,
							MUIA_Text_PreParse, MUIX_C,
							MUIA_Text_Contents, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
							End, //TextObject
						Child, TextMsgProgress2 = TextObject,
							MUIA_Text_PreParse, MUIX_C,
							MUIA_Text_Contents, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
							End, //TextObject
						End, //VGroup
				End, //WindowObject

			MUIA_Application_Menustrip, MenustripObject,
				Child, MenuObjectT(GetLocString(MSGID_MENU_PROJECT)),

					Child, MenuAbout = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_ABOUT),
					End,
					Child, MenuAboutMUI = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_ABOUTMUI),
					End,
					Child, MenuitemObject,
						MUIA_Menuitem_Title, -1,
					End,
					Child, MenuQuit = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_QUIT),
						MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_PROJECT_QUIT_SHORT),
					End,
						
				End, //MenuObjectT
			End, //MenuStripObject
		End; //ApplicationObject

		if (NULL == APP_Main)
			{
			fail(APP_Main, "Failed to create Application.");
			}

		if (ActiveWindowTransparencyDefault)
			setslider(SliderTransparencyActiveWindow, prefsActiveWindowTransparency);
		if (InactiveWindowTransparencyDefault)
			setslider(SliderTransparencyInactiveWindow, prefsInactiveWindowTransparency);
		if (IconScaleFactorDefault)
			setslider(SliderNominalIconSize, prefsIconScaleFactor);
		if (IconSizeConstraintsDefault)
			SetIconSizeConstraints(&prefIconSizeConstraints);
		else
			SetIconSizeConstraints(&IconSizeConstraints);

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
			WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);
		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
		DoMethod(OkButton, MUIM_Notify,MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, Application_Return_Ok);

		DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutHook);
		DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutMUIHook);
		DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(IconImage, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &TogglePathHook);

		DoMethod(NListviewPatterns, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
			APP_Main, 2, MUIM_CallHook, &NewPatternHook);

		// Clicking on the preview image causes the popup to open
		DoMethod(BackfillPatternPreview, MUIM_Notify, MUIA_Pressed, FALSE,
			PopObjectPatternNumber, 1, MUIM_Popstring_Open);

		set(WIN_Main, MUIA_Window_ActiveObject, OkButton);

		// GroupTransparencyActiveWindow is disabled whenever CheckboxTransparencyActiveWindow is checked
		DoMethod(CheckboxTransparencyActiveWindow, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			GroupTransparencyActiveWindow, 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

		// Set SliderTransparencyActiveWindow to prefs value when "use default" is checked
		DoMethod(CheckboxTransparencyActiveWindow, MUIM_Notify, MUIA_Selected, TRUE,
			SliderTransparencyActiveWindow, 3, MUIM_Set, MUIA_Numeric_Value, prefsActiveWindowTransparency);

		// GroupTransparencyInactiveWindow is disabled whenever CheckboxTransparencyInactiveWindow is checked
		DoMethod(CheckboxTransparencyInactiveWindow, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			GroupTransparencyInactiveWindow, 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

		// Set SliderTransparencyInactiveWindow to prefs value when "use default" is checked
		DoMethod(CheckboxTransparencyInactiveWindow, MUIM_Notify, MUIA_Selected, TRUE,
			SliderTransparencyInactiveWindow, 3, MUIM_Set, MUIA_Numeric_Value, prefsInactiveWindowTransparency);

		// GroupIconSizeConstraints is disabled whenever CheckboxIconSizeConstraints is checked
		DoMethod(CheckboxIconSizeConstraints, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			GroupIconSizeConstraints, 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

		// GroupNominalIconSize is disabled whenever CheckboxNominalIconSize is checked
		DoMethod(CheckboxNominalIconSize, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			GroupNominalIconSize, 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

		// Set SliderNominalIconSize  to prefs value when "use default" is checked
		DoMethod(CheckboxNominalIconSize, MUIM_Notify, MUIA_Selected, TRUE,
			SliderNominalIconSize, 3, MUIM_Set, MUIA_Numeric_Value, prefsIconScaleFactor);

		// disable Ok button for read-only icons
		set(OkButton, MUIA_Disabled, !IsWriteable);
		set(Group_Virtual, MUIA_ShowMe, 0);

		set(WIN_Main, MUIA_Window_Open, TRUE);
		get(WIN_Main, MUIA_Window_Open, &win_opened);
		get(WIN_Main, MUIA_Window_Screen, &WBScreen);

		if (win_opened)
			{
			ULONG sigs = 0;
			BOOL Run = TRUE;

			ReadPatternPrefsFile("ENV:Scalos/Pattern.prefs", TRUE);
			SelectPattern(PatternNumber);

			while (Run)
				{
				ULONG Action = DoMethod(APP_Main, MUIM_Application_NewInput, &sigs);

				switch (Action)
					{
				case Application_Return_Ok:
					SaveSettings(iconObj, ws);
					Run = FALSE;
					break;
				case MUIV_Application_ReturnID_Quit:
					Run = FALSE;
					break;
					}

				if (Run && sigs)
					{
					sigs = Wait(sigs | SIGBREAKF_CTRL_C);

					if (sigs & SIGBREAKF_CTRL_C) 
						{
						Run = FALSE;
						}
					}
				}
			}
		else
			{
			printf("failed to open main window !\n");
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		set(WIN_Main, MUIA_Window_Open, FALSE);
		}

	d1(KPrintF("%s/%s/%ld: ws=%08lx\n", __FILE__, __FUNC__, __LINE__, ws));
	if (ws)
		SCA_UnLockWindowList();
	d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));
	if (iconObj)
		DisposeIconObject(iconObj);
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (oldDir)
		CurrentDir(oldDir);

	fail(APP_Main, NULL);

	return 0;
}

//----------------------------------------------------------------------------

DISPATCHER(ThumbnailLifetimeSlider)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case MUIM_Numeric_Stringify:
		{
		struct ThumbnailLifetimeSliderInst *inst = INST_DATA(cl,obj);
		struct MUIP_Numeric_Stringify *mstr = (struct MUIP_Numeric_Stringify *) msg;

		if (0 == mstr->value)
			snprintf(inst->buf, sizeof(inst->buf), MUIX_C "%s", GetLocString(MSGID_THUMBNAILS_LIFETIME_FOREVER));
		else if (mstr->value > 365)
			snprintf(inst->buf, sizeof(inst->buf), MUIX_C "%s", GetLocString(MSGID_THUMBNAILS_LIFETIME_NOTSET));
		else
			{
			snprintf(inst->buf, sizeof(inst->buf), MUIX_C "%lu %s",
				(unsigned long)mstr->value,
				GetLocString(MSGID_ICONSPAGE_THUMBNAILS_MAXAGE_DAYS));
			}
		d1(KPrintF("%s/%s/%ld: inst=%08lx  buf=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, inst, inst->buf, inst->buf));
		Result = (ULONG) inst->buf;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static VOID fail(APTR APP_Main, CONST_STRPTR str)
{
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (APP_Main)
		{
		MUI_DisposeObject(APP_Main);
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (EmptyThumbnailBitmap)
		{
		MUI_DisposeObject(EmptyThumbnailBitmap);
		EmptyThumbnailBitmap = NULL;
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (ThumbnailLifetimeSliderClass)
		{
		MUI_DeleteCustomClass(ThumbnailLifetimeSliderClass);
		ThumbnailLifetimeSliderClass = NULL;
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (IconobjectClass)
		{
		CleanupIconobjectClass(IconobjectClass);
		IconobjectClass = NULL;
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (BitMapPicClass)
		{
		CleanupBitMappicClass(BitMapPicClass);
		BitMapPicClass = NULL;
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	CleanupBackfillClass();
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (WindowPropertiesCatalog)
		{
		CloseCatalog(WindowPropertiesCatalog);
		WindowPropertiesCatalog = NULL;
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	CloseLibraries();

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (str)
		{
		puts(str);
		exit(20);
		}

	exit(0);
}

//----------------------------------------------------------------------------

static void init(void)
{
	if (!OpenLibraries())
		fail(NULL, "Failed to call OpenLibraries.");

	if (LocaleBase)
		WindowPropertiesCatalog = OpenCatalogA(NULL, "Scalos/WindowProperties.catalog", NULL);

	IconobjectClass = InitIconobjectClass();
	if (NULL == IconobjectClass)
		fail(NULL, "Failed to create IconobjectClass.");

	ThumbnailLifetimeSliderClass = MUI_CreateCustomClass(NULL, MUIC_Numericbutton, NULL,
		sizeof(struct ThumbnailLifetimeSliderInst), DISPATCHER_REF(ThumbnailLifetimeSlider));
	if (NULL == ThumbnailLifetimeSliderClass)
		fail(NULL, "Unable to create ThumbnailLifetimeSliderClass.");

	if (NULL == BitMapPicClass)
		{
		BitMapPicClass = InitBitMappicClass();
		}
	if (NULL == BitMapPicClass)
		fail(NULL, "Unable to create BitMapPicClass.");

	if (NULL == BackfillClass)
		{
		BackfillClass = InitBackfillClass();
		}
	if (NULL == BackfillClass)
		fail(NULL, "Unable to create BackfillClass.");

	EmptyThumbnailBitmap = CreateEmptyThumbnailImage();
	if (NULL == EmptyThumbnailBitmap)
		fail(NULL, "Unable to create empty thumbnail image.");

	ReadScalosPrefs();
}


static BOOL OpenLibraries(void)
{
	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		fail(NULL, "Failed to open intuition.library.");
#ifdef __amigaos4__
	else
		{
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			fail(NULL, "Failed to open intuition interface.");
		}
#endif /* __amigaos4__ */

	MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		fail(NULL, "Failed to open muimaster.library.");
#ifdef __amigaos4__
	else
		{
		IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
		if (NULL == IMUIMaster)
			fail(NULL, "Failed to open muimaster interface.");
		}
#endif /* __amigaos4__ */

	IconBase = OpenLibrary("icon.library", 0);
	if (NULL == IconBase)
		fail(NULL, "Failed to open icon.library.");
#ifdef __amigaos4__
	else
		{
		IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
		if (NULL == IIcon)
			fail(NULL, "Failed to open icon interface.");
		}
#endif /* __amigaos4__ */

	ScalosBase = (struct ScalosBase *) OpenLibrary("scalos.library", 40);
	if (NULL == ScalosBase)
		fail(NULL, "Failed to open scalos.library.");
#ifdef __amigaos4__
	else
		{
		IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
		if (NULL == IScalos)
			fail(NULL, "Failed to open scalos interface.");
		}
#endif /* __amigaos4__ */

	IFFParseBase = OpenLibrary("iffparse.library", 36);
	if (NULL == IFFParseBase)
		fail(NULL, "Failed to open iffparse.library.");
#ifdef __amigaos4__
	IIFFParse = (struct IFFParseIFace *) GetInterface((struct Library *) IFFParseBase, "main", 1, NULL);
	if (NULL == IIFFParse)
		fail(NULL, "Failed to open iffparse interface.");
#endif /* __amigaos4__ */

	PreferencesBase = OpenLibrary("preferences.library", 39);
	if (NULL == PreferencesBase)
		fail(NULL, "Failed to open preferences.");
#ifdef __amigaos4__
	else
		{
		IPreferences = (struct PreferencesIFace *)GetInterface((struct Library *)PreferencesBase, "main", 1, NULL);
		if (NULL == IPreferences)
			fail(NULL, "Failed to open preferences interface.");
		}
#endif /* __amigaos4__ */

	IconobjectBase = OpenLibrary("iconobject.library", 0);
	if (NULL == IconobjectBase)
		fail(NULL, "Failed to open iconobject.library.");
#ifdef __amigaos4__
	else
		{
		IIconobject = (struct IconobjectIFace *)GetInterface((struct Library *)IconobjectBase, "main", 1, NULL);
		if (NULL == IIconobject)
			fail(NULL, "Failed to open iconobject interface.");
		}
#endif /* __amigaos4__ */

	GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 39);
	if (NULL == GfxBase)
		fail(NULL, "Failed to open graphics.library.");
#ifdef __amigaos4__
	IGraphics = (struct GraphicsIFace *) GetInterface((struct Library *) GfxBase, "main", 1, NULL);
	if (NULL == IGraphics)
		fail(NULL, "Failed to open graphics interface.");
#endif /* __amigaos4__ */

	ScalosGfxBase = (struct ScalosGfxBase *) OpenLibrary(SCALOSGFXNAME, 41);
	if (NULL == ScalosGfxBase)
		fail(NULL, "Failed to open scalosgfx.library.");
#ifdef __amigaos4__
	IScalosGfx = (struct ScalosGfxIFace *) GetInterface((struct Library *) ScalosGfxBase, "main", 1, NULL);
	if (NULL == IScalosGfx)
		fail(NULL, "Failed to open scalosgfx interface.");
#endif /* __amigaos4__ */

	TimerIORequest = (T_TIMEREQUEST *) CreateIORequest(CreateMsgPort(), sizeof(T_TIMEREQUEST));
	if (NULL == TimerIORequest)
		fail(NULL, "Failed to call CreateIORequest.");
	if (0 != OpenDevice("timer.device", UNIT_VBLANK, &TimerIORequest->tr_node, 0))
		{
		// OpenDevice failed
		DeleteIORequest((struct IORequest *)TimerIORequest);
		TimerIORequest = NULL;
		fail(NULL, "Failed to open timer.device.");
		}
	TimerBase = (T_TIMERBASE) TimerIORequest->tr_node.io_Device;
	if (NULL == TimerBase)
		fail(NULL, "Failed to get TimerBase.");
#ifdef __amigaos4__
	ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
	if (NULL == ITimer)
		fail(NULL, "Failed to open timer interface.");
#endif /* __amigaos4__ */

	CyberGfxBase = (APTR) OpenLibrary( "cybergraphics.library", 40);
#ifdef __amigaos4__
	if (CyberGfxBase)
		ICyberGfx = (struct CyberGfxIFace *) GetInterface((struct Library *) CyberGfxBase, "main", 1, NULL);
#endif
	// CyberGfxBase may be NULL

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		{
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
		if (NULL == ILocale)
			fail(NULL, "Failed to get locale interface.");
		}
#endif /* __amigaos4__ */

	return TRUE;
}


static void CloseLibraries(void)
{
	if (LocaleBase)
		{
		if (WindowPropertiesCatalog)
			{
			CloseCatalog(WindowPropertiesCatalog);
			WindowPropertiesCatalog = NULL;
			}
#ifdef __amigaos4__
		if (ILocale)
			{
			DropInterface((struct Interface *)ILocale);
			IIntuition = NULL;
			}
#endif /* __amigaos4__ */
		CloseLibrary((struct Library *) LocaleBase);
		LocaleBase = NULL;
		}

#ifdef __amigaos4__
	if (ICyberGfx)
		{
		DropInterface((struct Interface *)ICyberGfx);
		ICyberGfx = NULL;
		}
#endif /* __amigaos4__ */
	if (CyberGfxBase)
		{
		CloseLibrary(CyberGfxBase);
		CyberGfxBase = NULL;
		}

	if (TimerIORequest)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ITimer);
#endif
		CloseDevice(&TimerIORequest->tr_node);
		DeleteIORequest((struct IORequest *)TimerIORequest);
		TimerIORequest = NULL;
		}

#ifdef __amigaos4__
	if (IScalosGfx)
		{
		DropInterface((struct Interface *)IScalosGfx);
		IScalosGfx = NULL;
		}
#endif /* __amigaos4__ */
	if (ScalosGfxBase)
		{
		CloseLibrary((struct Library *) ScalosGfxBase);
		ScalosGfxBase = NULL;
		}
#ifdef __amigaos4__
	if (IGraphics)
		{
		DropInterface((struct Interface *)IGraphics);
		IGraphics = NULL;
		}
#endif /* __amigaos4__ */
	if (GfxBase)
		{
		CloseLibrary((struct Library *) GfxBase);
		GfxBase = NULL;
		}
#ifdef __amigaos4__
	if (IIconobject)
		{
		DropInterface((struct Interface *)IIconobject);
		IIconobject = NULL;
		}
#endif /* __amigaos4__ */
	if (IconobjectBase)
		{
		CloseLibrary(IconobjectBase);
		IconobjectBase = NULL;
		}
#ifdef __amigaos4__
	if (IPreferences)
		{
		DropInterface((struct Interface *)IPreferences);
		IPreferences = NULL;
		}
#endif /* __amigaos4__ */
	if (PreferencesBase)
		{
		CloseLibrary(PreferencesBase);
		PreferencesBase = NULL;
		}

#ifdef __amigaos4__
	if (IIFFParse)
		{
		DropInterface((struct Interface *)IIFFParse);
		IIFFParse = NULL;
		}
#endif /* __amigaos4__ */
	if (IFFParseBase)
		{
		CloseLibrary(IFFParseBase);
		IFFParseBase = NULL;
		}

#ifdef __amigaos4__
	if (IScalos)
		{
		DropInterface((struct Interface *)IScalos);
		IScalos = NULL;
		}
#endif /* __amigaos4__ */
	if (ScalosBase)
		{
		CloseLibrary((struct Library *) ScalosBase);
		ScalosBase = NULL;
		}
#ifdef __amigaos4__
	if (IIcon)
		{
		DropInterface((struct Interface *)IIcon);
		IIcon = NULL;
		}
#endif /* __amigaos4__ */
	if (IconBase)
		{
		CloseLibrary(IconBase);
		IconBase = NULL;
		}
#ifdef __amigaos4__
	if (IMUIMaster)
		{
		DropInterface((struct Interface *)IMUIMaster);
		IMUIMaster = NULL;
		}
#endif /* __amigaos4__ */
	if (MUIMasterBase)
		{
		CloseLibrary(MUIMasterBase);
		MUIMasterBase = NULL;
		}
#ifdef __amigaos4__
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif /* __amigaos4__ */
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
}

//----------------------------------------------------------------------------

static STRPTR GetLocString(ULONG MsgId)
{
	struct WindowProperties_LocaleInfo li;

	li.li_Catalog = WindowPropertiesCatalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else /* __amigaos4__ */
	li.li_ILocale = ILocale;
#endif /* __amigaos4__ */

	return (STRPTR)GetWindowPropertiesString(&li, MsgId);
}

static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg)
{
	if (NULL == WIN_AboutMUI)
		{
		WIN_AboutMUI = MUI_NewObject(MUIC_Aboutmui,
			MUIA_Window_RefWindow, WIN_Main,
			MUIA_Aboutmui_Application, APP_Main,
			TAG_END);
		}

	if (WIN_AboutMUI)
		set(WIN_AboutMUI, MUIA_Window_Open, TRUE);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT OpenAboutFunc(struct Hook *hook, Object *o, Msg msg)
{
	MUI_Request(APP_Main, WIN_Main, 0, NULL, 
		GetLocString(MSGID_ABOUTREQOK), 
		GetLocString(MSGID_ABOUTREQFORMAT), 
		VERSION_MAJOR, VERSION_MINOR, COMPILER_STRING, CURRENTYEAR);
}

//----------------------------------------------------------------------------

static void SaveSettings(Object *IconObj, struct ScaWindowStruct *ws)
{
	if (IconObj)
		{
		ULONG HideStatusBar = 0;
		ULONG HideControlBar = 0;
		ULONG NewCheckOverlap = 0;
		ULONG NewThumbnailMode = 0;
		ULONG ThumbnailLifetime = THUMBNAIL_LIFETIME_NOTSET;
		ULONG NewActiveWindowTransparencyDefault = 0;
		ULONG NewInactiveWindowTransparencyDefault = 0;
		ULONG NewIconScaleFactorDefault = 0;
		ULONG NewIconSizeConstraintsDefault = 0;
		APTR UndoStep = NULL;
		CONST_STRPTR *ToolTypesArray;
		STRPTR *OldToolTypesArray;

		if (ws)
			{
			UndoStep = (APTR) DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_BeginUndoStep);
			}

		GetAttr(IDTA_ToolTypes, IconObj, (APTR) &ToolTypesArray);
		OldToolTypesArray = CloneToolTypeArray(ToolTypesArray, 0);

		get(CycleMarkNoStatusBar, MUIA_Cycle_Active, &HideStatusBar);
		if (HideStatusBar)
			SetToolType(IconObj, "SCALOS_NOSTATUSBAR", "");
		else
			RemoveToolType(IconObj, "SCALOS_NOSTATUSBAR");

		get(CycleMarkNoControlBar, MUIA_Cycle_Active, &HideControlBar);
		if (HideControlBar)
			SetToolType(IconObj, "SCALOS_NOCONTROLBAR", "");
		else
			RemoveToolType(IconObj, "SCALOS_NOCONTROLBAR");

		if (ws)
			{
			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_StatusBar, !HideStatusBar,
				SCCA_IconWin_ControlBar, !HideControlBar,
				TAG_END);
			}

		get(NumericButtonThumbnailsLifetime, MUIA_Numeric_Value, &ThumbnailLifetime);
		if (THUMBNAIL_LIFETIME_NOTSET != ThumbnailLifetime)
			{
			char ThumbnailLifetimeString[20];

			snprintf(ThumbnailLifetimeString, sizeof(ThumbnailLifetimeString),
				"%ld", (long)ThumbnailLifetime);
			SetToolType(IconObj, "SCALOS_THUMBNAIL_LIFETIME", ThumbnailLifetimeString);
			}
		else
			{
			// system-wide default thumbnail lifetime has been selected
			RemoveToolType(IconObj, "SCALOS_THUMBNAIL_LIFETIME");
			}

		if (PatternNumber != NewPatternNumber)
			{
			if (0 != NewPatternNumber)
				{
				char PatternNumberString[20];

				snprintf(PatternNumberString, sizeof(PatternNumberString),
					"%ld", (long)NewPatternNumber);
				SetToolType(IconObj, "SCALOS_PATTERNNO", PatternNumberString);
				}
			else
				{
				// default pattern has been selected
				RemoveToolType(IconObj, "SCALOS_PATTERNNO");
				}

			d1(KPrintF("%s/%s/%ld: ws=%08lx\n", __FILE__, __FUNC__, __LINE__, ws));
			if (ws)
				{
				if (0 == NewPatternNumber)
					{
					// default pattern has been selected
					if (SCAV_ViewModes_Icon == ws->ws_Viewmodes)
						NewPatternNumber = PATTERNNR_IconWindowDefault;
					else
						NewPatternNumber = PATTERNNR_TextWindowDefault;
					}

				DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_NewPatternNumber, NewPatternNumber);
				}
			}

		get(CycleThumbnails, MUIA_Cycle_Active, &NewThumbnailMode);
		if (NewThumbnailMode != ThumbnailMode)
			{
			if (NewThumbnailMode > 0 && NewThumbnailMode <= 3)
				{
				static const char *ThumbnailModes[] =
					{
					"NEVER",
					"ASDEFAULT",
					"ALWAYS"
					};
				char ThumbnailsString[20];

				snprintf(ThumbnailsString, sizeof(ThumbnailsString),
					"%s", ThumbnailModes[NewThumbnailMode - 1]);
				SetToolType(IconObj, "SCALOS_THUMBNAILS", ThumbnailsString);
				}
			else
				{
				// default thumbnail mode has been selected
				RemoveToolType(IconObj, "SCALOS_THUMBNAILS");
				}

			if (ws)
				{
				SetAttrs(ws->ws_WindowTask->mt_MainObject,
					SCCA_IconWin_ThumbnailView, NewThumbnailMode - 1,
					TAG_END);
				}
			}

		get(CycleCheckOverlap, MUIA_Cycle_Active, &NewCheckOverlap);
		if (NewCheckOverlap != CheckOverlap)
			{
			if (CHECKOVERLAP_NOTSET == NewCheckOverlap)
				{
				// default overlap checking has been selected
				RemoveToolType(IconObj, "SCALOS_CHECKOVERLAP");
				}
			else
				{
				char CheckOverlapString[10];

				snprintf(CheckOverlapString, sizeof(CheckOverlapString), "%ld", (long)NewCheckOverlap);
				SetToolType(IconObj, "SCALOS_CHECKOVERLAP", CheckOverlapString);
				}

			if (ws)
				{
				DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_Update);
				}
			}

		get(CheckboxTransparencyActiveWindow, MUIA_Selected, &NewActiveWindowTransparencyDefault);
		if (NewActiveWindowTransparencyDefault)
			{
			RemoveToolType(IconObj, "SCALOS_ACTIVETRANSPARENCY");

			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_ActiveTransparency, prefsActiveWindowTransparency,
				TAG_END);
			}
		else
			{
			char TransparencyString[10];

			get(SliderTransparencyActiveWindow, MUIA_Numeric_Value, &ActiveWindowTransparency);

			snprintf(TransparencyString, sizeof(TransparencyString), "%ld", (long)ActiveWindowTransparency);
			SetToolType(IconObj, "SCALOS_ACTIVETRANSPARENCY", TransparencyString);

			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_ActiveTransparency, ActiveWindowTransparency,
				TAG_END);
			}

		get(CheckboxTransparencyInactiveWindow, MUIA_Selected, &NewInactiveWindowTransparencyDefault);
		if (NewInactiveWindowTransparencyDefault)
			{
			RemoveToolType(IconObj, "SCALOS_INACTIVETRANSPARENCY");

			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_InactiveTransparency, prefsInactiveWindowTransparency,
				TAG_END);
			}
		else
			{
			char TransparencyString[10];

			get(SliderTransparencyInactiveWindow, MUIA_Numeric_Value, &InactiveWindowTransparency);

			snprintf(TransparencyString, sizeof(TransparencyString), "%ld", (long)InactiveWindowTransparency);
			SetToolType(IconObj, "SCALOS_INACTIVETRANSPARENCY", TransparencyString);

			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_InactiveTransparency, InactiveWindowTransparency,
				TAG_END);
			}

		get(CheckboxNominalIconSize, MUIA_Selected, &NewIconScaleFactorDefault);
		if (NewIconScaleFactorDefault)
			{
			RemoveToolType(IconObj, "SCALOS_ICONSCALEFACTOR");

			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_IconScaleFactor, prefsIconScaleFactor,
				TAG_END);
			}
		else
			{
			char IconSizeString[10];

			get(SliderNominalIconSize, MUIA_Numeric_Value, &IconScaleFactor);

			snprintf(IconSizeString, sizeof(IconSizeString), "%ld", (long)IconScaleFactor);
			SetToolType(IconObj, "SCALOS_ICONSCALEFACTOR", IconSizeString);

			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_IconScaleFactor, IconScaleFactor,
				TAG_END);
			}

		get(CheckboxIconSizeConstraints, MUIA_Selected, &NewIconSizeConstraintsDefault);
		if (NewIconSizeConstraintsDefault)
			{
			RemoveToolType(IconObj, "SCALOS_ICONSIZECONSTRAINTS");

			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_IconSizeConstraints, (ULONG) &prefIconSizeConstraints,
				TAG_END);
			}
		else
			{
			char IconSizeConstraintsString[20];

			GetIconSizeConstraints(&IconSizeConstraints);

			snprintf(IconSizeConstraintsString, sizeof(IconSizeConstraintsString), "%ld,%ld",
				(long) IconSizeConstraints.MinX, (long) IconSizeConstraints.MaxX);
			SetToolType(IconObj, "SCALOS_ICONSIZECONSTRAINTS", IconSizeConstraintsString);

			SetAttrs(ws->ws_WindowTask->mt_MainObject,
				SCCA_IconWin_IconSizeConstraints, (ULONG) &IconSizeConstraints,
				TAG_END);
			}

		if (ws)
			{
			CONST_STRPTR *NewToolTypeArray = NULL;
			STRPTR iconName = NULL;
			BPTR dirLock = CurrentDir((BPTR) NULL);

			CurrentDir(dirLock);

			GetAttr(IDTA_ToolTypes, IconObj, (APTR) &NewToolTypeArray);
			get(IconObj, DTA_Name, &iconName);

			DoMethod(ws->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_AddUndoEvent,
				UNDO_SetToolTypes,
				UNDOTAG_IconDirLock, dirLock,
				UNDOTAG_IconName, (ULONG) iconName,
				UNDOTAG_OldToolTypes, (ULONG) OldToolTypesArray,
				UNDOTAG_NewToolTypes, (ULONG) NewToolTypeArray,
				TAG_END
				);

			DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_EndUndoStep, UndoStep);
			}
		if (OldToolTypesArray)
			{
			free(OldToolTypesArray);
			}
		}
}

//----------------------------------------------------------------------------

static Object *GetIconObject(CONST_STRPTR IconName, BPTR DirLock)
{
	Object *IconObj;
	struct ScaWindowList *WinList = NULL;
	struct FileInfoBlock *fib = NULL;
	BPTR fLock = (BPTR)NULL;

	do	{
		struct ScaWindowStruct *ws;

		IconObj = NewIconObject(IconName, NULL);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx\n", __LINE__, IconObj));
		if (IconObj)
			break;

		fLock = Lock(IconName, ACCESS_READ);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: fLock=%08lx  IconName=<%s>\n", __LINE__, fLock, IconName));
		if ((BPTR)NULL == fLock)
			break;

		fib = AllocDosObject(DOS_FIB, TAG_END);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: fib=%08lx\n", __LINE__, fib));
		if (NULL == fib)
			break;

		if (!Examine(fLock, fib))
			break;

		WinList = SCA_LockWindowList(SCA_LockWindowList_Shared);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: WinList=%08lx\n", __LINE__, WinList));
		if (NULL == WinList)
			break;


		for (ws = WinList->wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (LOCK_SAME == SameLock(ws->ws_Lock, DirLock))
				{
				IconObj = (Object *) DoMethod(ws->ws_WindowTask->mt_MainObject,
					SCCM_IconWin_GetDefIcon, IconName, 
					fib->fib_DirEntryType, fib->fib_Protection);
				}
			}

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx\n", __LINE__, IconObj));
		} while (0);

	if (fLock)
		UnLock(fLock);
	if (fib)
		FreeDosObject(DOS_FIB, fib);
	if (WinList)
		SCA_UnLockWindowList();

	return IconObj;
}

//----------------------------------------------------------------------------

static Object *GetDeviceIconObject(CONST_STRPTR IconName, CONST_STRPTR VolumeName, CONST_STRPTR DeviceName)
{
	Object *IconObj = NULL;
	BPTR OldDir = (BPTR)NULL;
	BPTR dLock = (BPTR)NULL;

	do	{
		char DefIconName[256];

		if (prefDefIconsFirst)
			{
			dLock = Lock(prefDefIconPath, ACCESS_READ);
			if ((BPTR)NULL == dLock)
				break;

			OldDir = CurrentDir(dLock);

			snprintf(DefIconName, sizeof(DefIconName), "def_%s", VolumeName);

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			BuildDefVolumeNameNoSpace(DefIconName, VolumeName, sizeof(DefIconName));

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			snprintf(DefIconName, sizeof(DefIconName), "def_%s", DeviceName);

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			CurrentDir(OldDir);
			OldDir = (BPTR)NULL;

			IconObj = NewIconObject("disk", NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, IconName));
			if (IconObj)
				break;

			}
		else
			{
			IconObj = NewIconObject("disk", NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, IconName));
			if (IconObj)
				break;

			dLock = Lock(prefDefIconPath, ACCESS_READ);
			if ((BPTR)NULL == dLock)
				break;

			OldDir = CurrentDir(dLock);

			snprintf(DefIconName, sizeof(DefIconName), "def_%s", VolumeName);

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			BuildDefVolumeNameNoSpace(DefIconName, VolumeName, sizeof(DefIconName));

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			snprintf(DefIconName, sizeof(DefIconName), "def_%s", DeviceName);

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;
			}

		IconObj = GetDefIconObject(WBDISK, NULL);
		} while (0);

	if (OldDir)
		CurrentDir(OldDir);
	if (dLock)
		UnLock(dLock);

	return IconObj;
}

//----------------------------------------------------------------------------

static BOOL isDiskWritable(BPTR dLock)
{
	struct InfoData *info = malloc(sizeof(struct InfoData));
	BOOL Result = TRUE;

	if (info)
		{
		Info(dLock, info);

		if (!CheckInfoData(info))
			Result = FALSE;

		free(info);
		}

	return Result;
}


// return TRUE if Disk is writable
static ULONG CheckInfoData(const struct InfoData *info)
{
	ULONG Result = TRUE;

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DiskType=%ld  DiskState=%ld\n", __LINE__, info->id_DiskType, info->id_DiskState));

	switch (info->id_DiskType)
		{
	case ID_NO_DISK_PRESENT:
	case ID_UNREADABLE_DISK:
		Result = FALSE;
		break;
		}

	if (ID_WRITE_PROTECTED == info->id_DiskState)
		Result = FALSE;

	return Result;
}

//----------------------------------------------------------------------------

static BOOL IsDevice(struct WBArg *arg)
{
	BOOL isDevice = FALSE;
	BPTR OldDir;
	BPTR fLock = (BPTR)NULL;

	do	{
		char VolumeName[256];
		size_t Len;

		OldDir = CurrentDir(arg->wa_Lock);

		if (arg->wa_Name && strlen(arg->wa_Name) > 0)
			{
			fLock = Lock(arg->wa_Name, ACCESS_READ);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: fLock=%08lx  Name=<%s>\n", __LINE__, fLock, arg->wa_Name));
			debugLock_d1(fLock);
			if ((BPTR)NULL == fLock)
				{
				if (0 == stricmp(arg->wa_Name, "disk"))
					{
					struct WBArg ArgCopy = *arg;

					ArgCopy.wa_Name = "";
					isDevice = IsDevice(&ArgCopy);
					}
				break;
				}

			NameFromLock(fLock, VolumeName, sizeof(VolumeName));
			}
		else
			{
			NameFromLock(arg->wa_Lock, VolumeName, sizeof(VolumeName));
			}

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: VolumeName=<%s>\n", __LINE__, VolumeName));

		Len = strlen(VolumeName);
		isDevice = Len > 0 && ':' == VolumeName[Len - 1];
		} while (0);

	if (fLock)
		UnLock(fLock);
	CurrentDir(OldDir);

	return isDevice;
}

//----------------------------------------------------------------------------

static void GetDeviceName(BPTR dLock, STRPTR DeviceName, size_t MaxLen)
{
	struct InfoData *info = malloc(sizeof(struct InfoData));

	strcpy(DeviceName, "");
	if (info)
		{
		struct DosList *VolumeNode;

		Info(dLock, info);

		VolumeNode = (struct DosList *) BADDR(info->id_VolumeNode);

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: VolumeNode=%08lx\n", __LINE__, VolumeNode));

		if (VolumeNode && VolumeNode->dol_Task && VolumeNode->dol_Task->mp_SigTask)
			stccpy(DeviceName, ((struct Task *) VolumeNode->dol_Task->mp_SigTask)->tc_Node.ln_Name, MaxLen);

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DeviceName=<%s>\n", __LINE__, DeviceName));

		free(info);
		}
}

//----------------------------------------------------------------------------

static BOOL ReadScalosPrefs(void)
{
	CONST_STRPTR MainPrefsFileName = "ENV:Scalos/Scalos.prefs";
	APTR MainPrefsHandle;

	MainPrefsHandle = AllocPrefsHandle((STRPTR) "Scalos");
	if (NULL == MainPrefsHandle)
		return FALSE;

	ReadPrefsHandle(MainPrefsHandle, (STRPTR) MainPrefsFileName);

	if (GetPreferences(MainPrefsHandle, ID_MAIN, SCP_ActiveWindowTransparency, &prefsActiveWindowTransparency, sizeof(&prefsActiveWindowTransparency)))
		{
		prefsActiveWindowTransparency = SCA_BE2WORD(prefsActiveWindowTransparency);
		}

	if (GetPreferences(MainPrefsHandle, ID_MAIN, SCP_InactiveWindowTransparency, &prefsInactiveWindowTransparency, sizeof(&prefsInactiveWindowTransparency)))
		{
		prefsInactiveWindowTransparency	= SCA_BE2WORD(prefsInactiveWindowTransparency);
		}

	if (GetPreferences(MainPrefsHandle, ID_MAIN, SCP_IconSizeConstraints, &prefIconSizeConstraints, sizeof(prefIconSizeConstraints)))
		{
		prefIconSizeConstraints.MinX = SCA_BE2WORD(prefIconSizeConstraints.MinX);
		prefIconSizeConstraints.MinY = SCA_BE2WORD(prefIconSizeConstraints.MinY);
		prefIconSizeConstraints.MaxX = SCA_BE2WORD(prefIconSizeConstraints.MaxX);
		prefIconSizeConstraints.MaxY = SCA_BE2WORD(prefIconSizeConstraints.MaxY);
		}

	if (GetPreferences(MainPrefsHandle, ID_MAIN, SCP_IconNominalSize, &prefsIconScaleFactor, sizeof(prefsIconScaleFactor)))
		{
		prefsIconScaleFactor = SCA_BE2WORD(prefsIconScaleFactor);
		}

	GetPreferences(MainPrefsHandle, ID_MAIN, SCP_LoadDefIconsFirst, &prefDefIconsFirst, sizeof(prefDefIconsFirst));
	// UBYTE

	prefDefIconPath = GetPrefsConfigString(MainPrefsHandle, SCP_PathsDefIcons, prefDefIconPath);

	if (prefDefIconPath)
		prefDefIconPath = strdup(prefDefIconPath);

	FreePrefsHandle(MainPrefsHandle);

	return TRUE;
}

static CONST_STRPTR GetPrefsConfigString(APTR prefsHandle, ULONG Id, CONST_STRPTR DefaultString)
{
	struct PrefsStruct *ps = FindPreferences(prefsHandle, ID_MAIN, Id);

	if (ps)
		return (CONST_STRPTR) PS_DATA(ps);

	return DefaultString;
}

//----------------------------------------------------------------------------

static void StripTrailingChar(STRPTR String, char CharToRemove)
{
	size_t Len = strlen(String);

	if (Len > 0 && CharToRemove == String[Len - 1])
		String[Len - 1] = '\0';
}

//----------------------------------------------------------------------------

// !! SCALOS WINDOW LIST WILL STAY LOCKED IF FOUND (Result != NULL) !!
static struct ScaWindowStruct *FindWindowByLock(BPTR xLock)
{
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	struct ScaWindowStruct *Result = NULL;

	d1(kprintf(__FUNC__ "/%ld: xLock=%08lx\n", __LINE__, xLock));

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; NULL == Result && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d1(kprintf(__FUNC__ "/%ld: swi=%08lx Lock=%08lx  Same=%ld\n", __LINE__, \
				swi, swi->ws_Lock, SameLock(swi->ws_Lock, xLock)));

			if (LOCK_SAME == SameLock(swi->ws_Lock, xLock))
				{
				d1(kprintf(__FUNC__ "/%ld: swi=%08lx\n", __LINE__, swi));
				Result = swi;
				}
			}

		if (NULL == Result)
			{
			d1(kprintf(__FUNC__ "/%ld: vor SCA_UnLockWindowList\n", __LINE__));
			SCA_UnLockWindowList();
			}
		}

	d1(kprintf(__FUNC__ "/%ld: Result=%08lx\n", __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------

static void BuildDefVolumeNameNoSpace(STRPTR Buffer, CONST_STRPTR VolumeName, size_t MaxLength)
{
	STRPTR dlp;
	size_t Len;

	stccpy(Buffer, "def_", MaxLength);

	Len = MaxLength - 1 - strlen(Buffer);

	dlp = Buffer + strlen(Buffer);
	while (Len && *VolumeName)
		{
		if (' ' != *VolumeName)
			{
			*dlp++ = *VolumeName;
			Len--;
			}
		VolumeName++;
		}
	*dlp = '\0';
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT PatternListConstructHookFunc(struct Hook *hook, Object *obj, struct NList_ConstructMessage *msg)
{
	struct PatternListEntry *scp = AllocPooled(msg->pool, sizeof(struct PatternListEntry));

	if (scp)
		{
		scp->ple_PatternCount = 0;
		scp->ple_ImageDescBuffer = 0;
		NewList(&scp->ple_PatternList);
		}

	return scp;
}

static SAVEDS(void) INTERRUPT PatternListDestructHookFunc(struct Hook *hook, Object *obj, struct NList_DestructMessage *msg)
{
	struct PatternListEntry *scp = (struct PatternListEntry *) msg->entry;
	struct PatternEntryDef *ped;

	d1(KPrintF("%s/%s/%ld: START scp=%08lx  ple_PatternNumber=%lu\n", __FILE__, __FUNC__, __LINE__, scp, scp->ple_PatternNumber));
	d1(KPrintF("%s/%s/%ld: ple_ImageDescBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_ImageDescBuffer));

	if (scp->ple_ImageDescBuffer)
		{
		free(scp->ple_ImageDescBuffer);
		scp->ple_ImageDescBuffer = NULL;
		}

	while ((ped = (struct PatternEntryDef *) RemHead(&scp->ple_PatternList)))
		{
		if (ped->ped_BitMapArray)
			{
			free(ped->ped_BitMapArray);
			ped->ped_BitMapArray = NULL;
			}
		if (ped->ped_MUI_ImageObject)
			{
			DoMethod(NListPatterns,
				MUIM_NList_UseImage, NULL, ped->ped_ThumbnailImageNr, 0);

			if (ped->ped_MUI_AllocatedImageObject)
				MUI_DisposeObject(ped->ped_MUI_AllocatedImageObject);
			ped->ped_MUI_ImageObject = ped->ped_MUI_AllocatedImageObject = NULL;
			}
		if (ped->ped_SAC)
			{
			ScalosGfxFreeSAC(ped->ped_SAC);
			ped->ped_SAC = NULL;
			}
		d1(KPrintF("%s/%s/%ld: ped=%08lx\n", __FILE__, __FUNC__, __LINE__, ped));
		free(ped);
		}

	FreePooled(msg->pool, scp, sizeof(struct PatternListEntry));

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static SAVEDS(ULONG) INTERRUPT PatternListDisplayHookFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm)
{
	if (ndm->entry)
		{
		struct PatternListEntry *scp = (struct PatternListEntry *) ndm->entry;

		d1(KPrintF("%s/%s/%ld: ple_PatternNumber=%lu  ple_ImageDescBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_PatternNumber, scp->ple_ImageDescBuffer));

		if (NULL == scp->ple_ImageDescBuffer && scp->ple_PatternCount > 0)
			scp->ple_ImageDescBuffer = malloc(scp->ple_PatternCount * 10);

		// 1st column -- pattern number
		ndm->preparses[0] = "\033r";

		// 2nd column -- thumbnail(s)
		ndm->preparses[1] = "\033l";

		// 1st column -- pattern number
		if (scp->ple_PatternNumber)
			{
			snprintf(scp->ple_ImageNrBuffer, sizeof(scp->ple_ImageNrBuffer),
				"%d", scp->ple_PatternNumber);
			ndm->strings[0]	= scp->ple_ImageNrBuffer;
			}
		else
			ndm->strings[0]	= GetLocString(MSGID_PATTERNR_DEFAULT);

		if (scp->ple_ImageDescBuffer)
			{
			// 2nd column -- thumbnail(s)
			const struct PatternEntryDef *ped;
			STRPTR lp = scp->ple_ImageDescBuffer;

			for (ped = (const struct PatternEntryDef *) scp->ple_PatternList.lh_Head;
				ped != (const struct PatternEntryDef *) &scp->ple_PatternList.lh_Tail;
				ped = (const struct PatternEntryDef *) ped->ped_Node.ln_Succ)
				{
				sprintf(lp, "\33o[%ld]", (unsigned long)ped->ped_ThumbnailImageNr);
				lp += strlen(lp);

				d1(KPrintF("%s/%s/%ld: lp=<%s>  ord=%ld  scp=%08lx  ImageObj=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, lp, ndm->entry_pos, scp, ped->ped_MUI_ImageObject));
				}
			ndm->strings[1] = scp->ple_ImageDescBuffer;
			d1(KPrintF("%s/%s/%ld: len=%lu  ple_PatternCount=%lu\n", __FILE__, __FUNC__, __LINE__, lp - scp->ple_ImageDescBuffer, scp->ple_PatternCount));
			}
		else
			{
			ndm->strings[1] = "";
			}
		}
	else
		{
		// display titles

		// 1st column -- pattern number
		ndm->preparses[0] = "\033c";

		// 2nd column -- thumbnail
		ndm->preparses[1] = "\033c";

		// 1st column -- pattern number
		ndm->strings[0] = GetLocString(MSGID_COLUMNTITLE_1);

		// 2nd column -- thumbnail
		ndm->strings[1] = GetLocString(MSGID_COLUMNTITLE_3);
		}

	return 0;
}


static SAVEDS(LONG) INTERRUPT PatternListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm)
{
	const struct PatternListEntry *scp1 = (const struct PatternListEntry *) ncm->entry1;
	const struct PatternListEntry *scp2 = (const struct PatternListEntry *) ncm->entry2;
	LONG Result = 0;

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		// sort by number
		if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
			Result = scp2->ple_PatternNumber - scp1->ple_PatternNumber;
		else
			Result = scp1->ple_PatternNumber - scp2->ple_PatternNumber;
		}

	return Result;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT PatternPopupWindowHookFunc(struct Hook *hook, Object *pop, Object *win)
{
	set(win, MUIA_Window_Width, MUIV_Window_Width_Default);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT NewPatternHookFunc(struct Hook *hook, Object *o, Object *x)
{
	struct PatternListEntry *scp = NULL;
	ULONG n = 0;

	get(NListPatterns, MUIA_NList_Active, &n);
	DoMethod(PopObjectPatternNumber, MUIM_Popstring_Close, TRUE);
	d1(KPrintF("%s/%s/%ld: n=%lu\n", __FILE__, __FUNC__, __LINE__, n));

	DoMethod(NListPatterns, MUIM_NList_GetEntry, n, &scp);

	if (scp)
		{
		NewPatternNumber = scp->ple_PatternNumber;
		SelectPattern(NewPatternNumber);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT TogglePathHookFunc(struct Hook *hook, Object *o, Object *x)
{
	if (strlen(PathName) > 0)
		{
		LONG selected = 0;

		get(Group_Virtual, MUIA_ShowMe, &selected);
		set(Group_Virtual, MUIA_ShowMe, !selected);
		}
}

//----------------------------------------------------------------------------

static Object *CreateEmptyThumbnailImage(void)
{
	static const ULONG ThumbnailImageColors[] =
	{
	   0xaaaaaaaa,0xaaaaaaaa,0xa0a0a0a0,
	   0x00000000,0x00000000,0x00000000,
	   0xffffffff,0xffffffff,0xffffffff,
	   0x66666666,0x88888888,0xbbbbbbbb,
	   0x99999999,0x99999999,0x99999999,
	   0xbbbbbbbb,0xbbbbbbbb,0xbbbbbbbb,
	   0xbbbbbbbb,0xaaaaaaaa,0x99999999,
	   0xffffffff,0xbbbbbbbb,0xaaaaaaaa
	};
	struct RastPort rp;
	struct BitMap *bm;
	Object *BitMapObj = NULL;

	rp.BitMap = NULL;
	InitRastPort(&rp);

	do	{
		bm = rp.BitMap = AllocBitMap(ThumbnailWidth, ThumbnailHeight, 2, BMF_CLEAR, NULL);
		d1(kprintf("%s/%s/%ld: bm=%08lx\n", __FILE__, __FUNC__, __LINE__, bm));
		if (NULL == bm)
			break;

		SetRast(&rp, 0);
		SetAPen(&rp, 1);

		Move(&rp, 0, 0);
		Draw(&rp, 0, ThumbnailHeight - 1);
		Draw(&rp, ThumbnailWidth - 1, ThumbnailHeight - 1);
		Draw(&rp, ThumbnailWidth - 1, 0);
		Draw(&rp, 0, 0);

		Move(&rp, 0, 0);
		Draw(&rp, ThumbnailWidth - 1, ThumbnailHeight - 1);

		Move(&rp, 0, ThumbnailHeight - 1);
		Draw(&rp, ThumbnailWidth - 1, 0);

		BitMapObj = BitmapObject,
			MUIA_Bitmap_Width, ThumbnailWidth,
			MUIA_Bitmap_Height, ThumbnailHeight,
			MUIA_Bitmap_Bitmap, (IPTR)bm,
			MUIA_Bitmap_Transparent, 0,
			MUIA_Bitmap_UseFriend, TRUE,
			MUIA_Bitmap_Precision, PRECISION_ICON,
			MUIA_Bitmap_SourceColors, (IPTR)ThumbnailImageColors,
			End;

		d1(kprintf("%s/%s/%ld: BitMapObj=%08lx\n", __FILE__, __FUNC__, __LINE__, BitMapObj));

		if (BitMapObj)
			bm = NULL;	// don't free bm
		} while (0);

	if (bm)
		FreeBitMap(bm);

	return BitMapObj;
}

//----------------------------------------------------------------------------

static LONG ReadPatternPrefsFile(CONST_STRPTR Filename, BOOL Quiet)
{
	static const LONG StopChunkList[] =
		{
		ID_PREF, ID_PATT,
		ID_PREF, ID_DEFS,
		ID_PREF, ID_TCOL,
		ID_PREF, ID_TBMP
		};
	struct TempRastPort trp;
	LONG Result;
	struct IFFHandle *iff;
	BOOL iffOpened = FALSE;

	InitRastPort(&trp.trp_rp);
	trp.trp_rp.BitMap = NULL;
	trp.trp_rp.Layer = NULL;
	trp.trp_Width = 0;

	BitMapsRead = 0;
	ThumbnailImageNumber = 0;
	DoMethod(NListPatterns, MUIM_NList_Clear);
	set(WIN_Main, MUIA_Window_Sleep, TRUE);
	set(NListPatterns, MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	do	{
		struct PatternEntryDef *ped = NULL;

		iff = AllocIFF();
		if (NULL == iff)
			{
			Result = IoErr();
			break;
			}

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR)Open(Filename, MODE_OLDFILE);
		if (0 == iff->iff_Stream)
			{
			Result = IoErr();
			break;
			}

		Result = OpenIFF(iff, IFFF_READ);
		if (RETURN_OK != Result)
			break;

		iffOpened = TRUE;

		Result = StopChunks(iff, StopChunkList, 5);
		if (RETURN_OK != Result)
			break;

		while (1)
			{
			struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_SCAN);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);
			if (NULL == cn)
				break;

			if (ID_DEFS == cn->cn_ID)
				{
				LONG Actual;

				Actual = ReadChunkBytes(iff, &pDefs, sizeof(pDefs));
				if (sizeof(pDefs) != Actual)
					{
					Result = IoErr();
					break;
					}
				pDefs.scd_Flags = SCA_BE2WORD(pDefs.scd_Flags);
				pDefs.scd_WorkbenchPattern = SCA_BE2WORD(pDefs.scd_WorkbenchPattern);
				pDefs.scd_ScreenPattern = SCA_BE2WORD(pDefs.scd_ScreenPattern);
				pDefs.scd_WindowPattern = SCA_BE2WORD(pDefs.scd_ScreenPattern);
				pDefs.scd_TextModePattern = SCA_BE2WORD(pDefs.scd_TextModePattern);
				}
			else if (ID_PATT == cn->cn_ID)
				{
				struct PatternListEntry *scp = NULL;
				ULONG nEntries = 0;

				ped = CreatePatternEntryDef();
				if (NULL == ped)
					break;

				if (0 == ReadChunkBytes(iff, &ped->ped_PatternPrefs.scxp_PatternPrefs, sizeof(struct ScaExtPatternPrefs)))
					{
					Result = IoErr();
					break;
					}

				ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Number = SCA_BE2WORD(ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Number);
				ped->ped_PatternPrefs.scxp_PatternPrefs.scp_RenderType = SCA_BE2WORD(ped->ped_PatternPrefs.scxp_PatternPrefs.scp_RenderType);
				ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Flags = SCA_BE2WORD(ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Flags);
				ped->ped_PatternPrefs.scxp_PatternPrefs.scp_NumColors = SCA_BE2WORD(ped->ped_PatternPrefs.scxp_PatternPrefs.scp_NumColors);
				ped->ped_PatternPrefs.scxp_PatternPrefs.scp_DitherMode = SCA_BE2WORD(ped->ped_PatternPrefs.scxp_PatternPrefs.scp_DitherMode);
				ped->ped_PatternPrefs.scxp_PatternPrefs.scp_DitherAmount = SCA_BE2WORD(ped->ped_PatternPrefs.scxp_PatternPrefs.scp_DitherAmount);

				if (AllPatterns[ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Number])
					{
					scp = AllPatterns[ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Number];
					}
				else
					{
					DoMethod(NListPatterns, MUIM_NList_InsertSingle, "", MUIV_NList_Insert_Bottom);
					get(NListPatterns, MUIA_NList_Entries, &nEntries);

					nEntries--;
					DoMethod(NListPatterns, MUIM_NList_GetEntry, nEntries, &scp);
					}

				d1(KPrintF("%s/%s/%ld: ID_PATT scp=%08lx\n", __FILE__, __FUNC__, __LINE__, scp));
				if (scp)
					{
					scp->ple_PatternCount++;
					scp->ple_PatternNumber = ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Number;

					AllPatterns[ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Number] = scp;

					AddTail(&scp->ple_PatternList, &ped->ped_Node);
					}
				}
			else if (ID_TCOL == cn->cn_ID)
				{
				// ID_TCOL must always be immediately preceeded by ID_PATT for the same pattern
				d1(KPrintF("%s/%s/%ld: ID_TCOL ped=%08lx  sac=%08lx\n", __FILE__, __FUNC__, __LINE__, ped, ped->ped_SAC));
				if (ped)
					{
					if (NULL == ped->ped_SAC)
						{
						// create preliminary SAC with sac_ColorTable
						ped->ped_SAC = ScalosGfxCreateSAC(1, 1, 8, NULL, NULL);
						}
					d1(KPrintF("%s/%s/%ld: ped=%08lx  sac=%08lx\n", __FILE__, __FUNC__, __LINE__, ped, ped->ped_SAC));
					if (ped->ped_SAC)
						{
						ULONG n;
						LONG Length;
						WORD NumColors;
						UBYTE byteColorTable[3*256];

						Length = sizeof(NumColors);

						if (Length != ReadChunkBytes(iff, &NumColors, Length))
							{
							Result = IoErr();
							break;
							}
						NumColors = SCA_BE2WORD(NumColors);
						d1(KPrintF("%s/%s/%ld: NumColors=%ld\n", __FILE__, __FUNC__, __LINE__, NumColors));
						Length = NumColors * 3;
						if (Length != ReadChunkBytes(iff, byteColorTable, Length))
							{
							Result = IoErr();
							break;
							}

						for (n=0; n < NumColors * 3; n++)
							{
							UBYTE ch = byteColorTable[n];

							ped->ped_SAC->sac_ColorTable[n] = (ch << 24) + (ch << 16) + (ch << 8) + ch;
							d1(KPrintF("%s/%s/%ld: ColorTable[%ld]=%08lx\n", __FILE__, __FUNC__, __LINE__, n, ped->ped_SAC->sac_ColorTable[n]));
							}
						}
					}
				}
			else if (ID_TBMP == cn->cn_ID)
				{
				// ID_TBMP must always be preceeded by ID_PATT and ID_TCOL
				d1(KPrintF("%s/%s/%ld: ID_TBMP ped=%08lx  sac=%08lx\n", __FILE__, __FUNC__, __LINE__, ped, ped->ped_SAC));
				if (ped)
					{
					Result = ReadPrefsBitMap(iff, ped, &trp);
					d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
					if (RETURN_OK != Result)
						break;
					}
				BitMapsRead++;
				}
			}

		d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (IFFERR_EOF == Result)
			Result = RETURN_OK;
		} while (0);

	if (iff)
		{
		if (iffOpened)
			CloseIFF(iff);

		if (iff->iff_Stream)
			Close((BPTR)iff->iff_Stream);

		FreeIFF(iff);
		}

	if (RETURN_OK != Result && !Quiet)
		{
		char Buffer[120];

		Fault(Result, "", Buffer, sizeof(Buffer) - 1);

		// MUI_RequestA()
		MUI_Request(APP_Main, WIN_Main, 0, NULL,
			GetLocString(MSGID_ABOUTREQOK),
			GetLocString(MSGID_REQTITLE_READERROR),
			Filename,
			Buffer);
		}

	if (trp.trp_rp.BitMap)
		FreeBitMap(trp.trp_rp.BitMap);

	CreateThumbnailImages();
	AddDefaultPatternEntry();

	set(WIN_Main, MUIA_Window_Sleep, FALSE);
	set(NListPatterns, MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG ReadPrefsBitMap(struct IFFHandle *iff, struct PatternEntryDef *ped, struct TempRastPort *trp)
{
	LONG Result = RETURN_OK;
	struct ScalosBitMapAndColor *sac = NULL;

	do	{
		LONG Length;
		struct RastPort rp;
		struct ScaPatternPrefsBitMap spb;

		if (sizeof(spb) != ReadChunkBytes(iff, &spb, sizeof(spb)))
			{
			Result = IoErr();
			break;
			}

		spb.spb_Width = SCA_BE2WORD(spb.spb_Width);
		spb.spb_Height = SCA_BE2WORD(spb.spb_Height);

		sac = ScalosGfxCreateSAC(spb.spb_Width, spb.spb_Height,
			8, NULL, NULL);
		d1(KPrintF("%s/%s/%ld: sac=%08lx\n", __FILE__, __FUNC__, __LINE__, sac));
		if (NULL == sac)
			break;

		InitRastPort(&rp);
		rp.BitMap = sac->sac_BitMap;

		if (NULL == trp->trp_rp.BitMap || trp->trp_Width != TEMPRP_WIDTH(spb.spb_Width))
			{
			if (trp->trp_rp.BitMap)
				FreeBitMap(trp->trp_rp.BitMap);

			trp->trp_Width = TEMPRP_WIDTH(spb.spb_Width);
			trp->trp_rp.BitMap = AllocBitMap(trp->trp_Width, 1, 8, BMF_STANDARD, NULL);
			if (NULL == trp->trp_rp.BitMap)
				{
				Result = ERROR_NO_FREE_STORE;
				break;
				}
			}

		Length = PIXELARRAY8_BUFFERSIZE(spb.spb_Width, spb.spb_Height - 1);

		ped->ped_BitMapArray = malloc(Length);
		if (NULL == ped->ped_BitMapArray)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		if (Length != ReadChunkBytes(iff, ped->ped_BitMapArray, Length))
			{
			Result = IoErr();
			break;
			}

		// Blit image data from ple_BitMapArray to sac_BitMap
		WritePixelArray8(&rp, 0, 0,
			spb.spb_Width - 1, spb.spb_Height - 1,
			ped->ped_BitMapArray, &trp->trp_rp);

		if (ped->ped_SAC && ped->ped_SAC->sac_NumColors <= 256)
			{
			size_t Length = ped->ped_SAC->sac_NumColors * 3 * sizeof(ULONG);

			// copy ColorTable from preliminary SAC
			memcpy(sac->sac_ColorTable, ped->ped_SAC->sac_ColorTable, Length);

			// free preliminary SAC
			ScalosGfxFreeSAC(ped->ped_SAC);
			}

		ped->ped_SAC = sac;
		sac = NULL;
		} while (0);

	if (RETURN_OK != Result)
		{
		if (ped->ped_SAC)
			{
			ScalosGfxFreeSAC(ped->ped_SAC);
			ped->ped_SAC = NULL;
			}
		}
	if (sac)
		ScalosGfxFreeSAC(sac);

	return Result;
}

//----------------------------------------------------------------------------

static void CreateThumbnailImages(void)
{
	T_TIMEVAL tvNow;
	T_TIMEVAL tvStart;
	ULONG n, nEntries = 0;
	APTR prWindowPtr;
	struct Process *MyProcess = (struct Process *) FindTask(NULL);

	prWindowPtr = MyProcess->pr_WindowPtr;
	MyProcess->pr_WindowPtr = (APTR) ~0;	// suppress error requesters

	set(WIN_Main, MUIA_Window_Sleep, TRUE);

	set(TextMsgProgress1, MUIA_Text_Contents, (ULONG) GetLocString(MSGID_LOADING_THUMBNAILS));

	get(NListPatterns, MUIA_NList_Entries, &nEntries);

	set(NListPatterns, MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	for (n = 0; n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(NListPatterns, MUIM_NList_GetEntry, n, &scp);

		if (scp)
			{
			// replace images by empty thumbnail
			struct PatternEntryDef *ped;

			for (ped = (struct PatternEntryDef *) scp->ple_PatternList.lh_Head;
				ped != (struct PatternEntryDef *) &scp->ple_PatternList.lh_Tail;
				ped = (struct PatternEntryDef *) ped->ped_Node.ln_Succ)
				{
				DoMethod(NListPatterns, MUIM_NList_UseImage,
					EmptyThumbnailBitmap,
					ped->ped_ThumbnailImageNr, 0);

				if (ped->ped_MUI_AllocatedImageObject)
					{
					MUI_DisposeObject(ped->ped_MUI_AllocatedImageObject);
					ped->ped_MUI_ImageObject = ped->ped_MUI_AllocatedImageObject = NULL;
					}
				}
			}
		}

	ThumbnailImageNumber = 0;

	// for performance reasons, GaugeProgress, TextMsgProgress1, and TextMsgProgress2
	// are updated no faster than every 100ms
	GetSysTime(&tvStart);
	tvNow.tv_secs = 0;
	tvNow.tv_micro = 100000;
	AddTime(&tvStart, &tvNow);	// +100ms

	set(GaugeProgress, MUIA_Gauge_Max, nEntries);

	for (n = 0; n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(APP_Main, MUIM_Application_InputBuffered);

		GetSysTime(&tvNow);

		DoMethod(NListPatterns, MUIM_NList_GetEntry, n, &scp);

		if (scp)
			{
			struct PatternEntryDef *ped;

			for (ped = (struct PatternEntryDef *) scp->ple_PatternList.lh_Head;
				ped != (struct PatternEntryDef *) &scp->ple_PatternList.lh_Tail;
				ped = (struct PatternEntryDef *) ped->ped_Node.ln_Succ)
				{
				if (CmpTime(&tvStart, &tvNow) > 0)
					{
					// it's more than 100ms since initial start
					char TextLine[150];
					ULONG WindowIsOpen = 0;

					tvNow.tv_secs = 0;
					tvNow.tv_micro = 100000;
					AddTime(&tvStart, &tvNow);	// +100ms

					set(TextMsgProgress1, MUIA_Text_Contents, (ULONG) GetLocString(MSGID_LOADING_THUMBNAIL));
					set(TextMsgProgress2, MUIA_Text_Contents, (ULONG) ped->ped_PatternPrefs.scxp_Name);

					// make sure the progress display window is open
					get(WIN_Progress, MUIA_Window_Open, &WindowIsOpen);
					if (!WindowIsOpen)
						{
						set(WIN_Progress, MUIA_Window_Open, TRUE);
						}

					snprintf(TextLine, sizeof(TextLine), GetLocString(MSGID_PROGRESS_THUMBNAILS), n, nEntries);
					set(GaugeProgress, MUIA_Gauge_InfoText, (ULONG) TextLine);

					set(GaugeProgress, MUIA_Gauge_Current, n);
					}

				CreateThumbnailImage(ped);
				}
			}
		}

	set(GaugeProgress, MUIA_Gauge_Current, nEntries);	// 100%

	set(NListPatterns, MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	set(WIN_Progress, MUIA_Window_Open, FALSE);
	set(WIN_Main, MUIA_Window_Sleep, FALSE);

	// restore pr_WindowPtr
	MyProcess->pr_WindowPtr = prWindowPtr;
}

//----------------------------------------------------------------------------

static void CreateThumbnailImage(struct PatternEntryDef *ped)
{
	struct RastPort rp;

	rp.BitMap = NULL;
	InitRastPort(&rp);

	do	{
		d1(KPrintF("%s/%s/%ld: ple_MUI_ImageObject=%08lx  ple_SAC=%08lx\n", __FILE__, __FUNC__, __LINE__, ped->ped_MUI_ImageObject, ped->ped_SAC));
		d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, ped->ped_PatternPrefs.scxp_Name));

		if (NULL == ped->ped_MUI_ImageObject)
			{
			UBYTE *BitMapArray;
			ULONG ScreenDepth;

			ScreenDepth = GetBitMapAttr(WBScreen->RastPort.BitMap, BMA_DEPTH);

			d1(KPrintF("%s/%s/%ld: sac=%08lx\n", __FILE__, __FUNC__, __LINE__, ped->ped_SAC));
			if (NULL == ped->ped_SAC)
				{
				d1(KPrintF("%s/%s/%ld: sac=%08lx\n", __FILE__, __FUNC__, __LINE__, ped->ped_SAC));
				break;
				}
			else
				{
				BitMapArray = ped->ped_BitMapArray;
				}

			if (CyberGfxBase && ScreenDepth > 8)
				{
				ped->ped_MUI_ImageObject = ped->ped_MUI_AllocatedImageObject = BitMapPicObject,
					MUIA_ScaBitMappic_BitMap, (IPTR)ped->ped_SAC->sac_BitMap,
					MUIA_ScaBitMappic_ColorTable, (IPTR)ped->ped_SAC->sac_ColorTable,
					MUIA_ScaBitMappic_Width, ped->ped_SAC->sac_Width,
					MUIA_ScaBitMappic_Height, ped->ped_SAC->sac_Height,
					MUIA_ScaBitMappic_Screen, (IPTR)WBScreen,
					MUIA_ScaBitmappic_BitMapArray, (IPTR)BitMapArray,
					End;
				}
			else
				{
				ped->ped_MUI_ImageObject = ped->ped_MUI_AllocatedImageObject = BitmapObject,
					MUIA_Bitmap_Width, ped->ped_SAC->sac_Width,
					MUIA_Bitmap_Height, ped->ped_SAC->sac_Height,
					MUIA_Bitmap_Bitmap, (IPTR)ped->ped_SAC->sac_BitMap,
					MUIA_Bitmap_SourceColors, (IPTR)ped->ped_SAC->sac_ColorTable,
					MUIA_Bitmap_UseFriend, TRUE,
					MUIA_Bitmap_Precision, PRECISION_ICON,
					End;
				}

			d1(KPrintF("%s/%s/%ld: ple_MUI_ImageObject=%08lx\n", __FILE__, __FUNC__, __LINE__, ped->ped_MUI_ImageObject));
			if (NULL == ped->ped_MUI_ImageObject)
				break;

			// generate unique image number
			ped->ped_ThumbnailImageNr = ++ThumbnailImageNumber;
			}

		rp.BitMap = NULL;       // don't free rp.BitMap
		} while (0);

	d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (rp.BitMap)
		FreeBitMap(rp.BitMap);

	d1(KPrintF("%s/%s/%ld: ImageNr=%ld\n", __FILE__, __FUNC__, __LINE__, ped->ped_ThumbnailImageNr));
	d1(KPrintF("%s/%s/%ld: ple_MUI_ImageObject=%08lx\n", __FILE__, __FUNC__, __LINE__, ped->ped_MUI_ImageObject));

	if (ped->ped_MUI_ImageObject)
		{
		DoMethod(NListPatterns, MUIM_NList_UseImage,
			ped->ped_MUI_ImageObject, ped->ped_ThumbnailImageNr, 0);
		}
	else
		{
		DoMethod(NListPatterns, MUIM_NList_UseImage,
			EmptyThumbnailBitmap, ped->ped_ThumbnailImageNr, 0);
		}

	d1(KPrintF("%s/%s/%ld: end\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static void SelectPattern(ULONG PatternNumber)
{
	static char buffer[50];
	ULONG n, nEntries = 0;
	BOOL Found;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	get(NListPatterns, MUIA_NList_Entries, &nEntries);

	d1(KPrintF("%s/%s/%ld: Pattern=%lu  Entries=%lu\n", __FILE__, __FUNC__, __LINE__, Pattern, nEntries));

	for (Found = FALSE, n = 0; !Found && n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(NListPatterns, MUIM_NList_GetEntry, n, &scp);
		set(NListPatterns, MUIA_NList_Active, n);

		if (scp && scp->ple_PatternNumber == PatternNumber)
			{
			struct PatternEntryDef *ped = NULL;

			Found = TRUE;

			if (!IsListEmpty(&scp->ple_PatternList))
				ped = (struct PatternEntryDef *) scp->ple_PatternList.lh_Head;

			if (ped && ped->ped_MUI_ImageObject)
				{
				d1(KPrintF("%s/%s/%ld: ple_MUI_ImageObject=%08lx\n", __FILE__, __FUNC__, __LINE__, ped->ped_MUI_ImageObject));
				set(BackfillPatternPreview, BFA_BitmapObject, ped->ped_MUI_ImageObject);
				}
			}
		}

	if (PatternNumber)
		snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)PatternNumber);
	else
		stccpy(buffer, GetLocString(MSGID_PATTERNR_DEFAULT), sizeof(buffer));
	set(TextPatternNumber, MUIA_Text_Contents, buffer);

	if (!Found)
		{
		set(BackfillPatternPreview, BFA_BitmapObject, EmptyThumbnailBitmap);
		set(NListPatterns, MUIA_NList_Active, MUIV_NList_Active_Off);
		}
}

//----------------------------------------------------------------------------

static void AddDefaultPatternEntry(void)
{
	struct PatternListEntry *scpDefault = NULL;
	ULONG nEntries = 0;
	BOOL Found;
	ULONG n;
	ULONG DefaultPatternNumber;

	if (ws && 0 == strcmp(ws->ms_ClassName, "TextWindow.sca"))
		DefaultPatternNumber = pDefs.scd_TextModePattern;
	else
		DefaultPatternNumber = pDefs.scd_WindowPattern;


	get(NListPatterns, MUIA_NList_Entries, &nEntries);

	for (Found = FALSE, n = 0; !Found && n < nEntries; n++)
		{
		DoMethod(NListPatterns, MUIM_NList_GetEntry, n, &scpDefault);
		set(NListPatterns, MUIA_NList_Active, n);

		if (scpDefault && scpDefault->ple_PatternNumber == DefaultPatternNumber)
			{
			d1(KPrintF("%s/%s/%ld: BitMapObject=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_MUI_ImageObject));
			Found = TRUE;
			}
		}

	if (Found)
		{
		struct PatternListEntry *scpNew = NULL;

		set(NListPatterns, MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

		DoMethod(NListPatterns, MUIM_NList_InsertSingle, "", MUIV_NList_Insert_Top);
		DoMethod(NListPatterns, MUIM_NList_GetEntry, 0, &scpNew);

		if (scpNew)
			{
			const struct PatternEntryDef *ped;

			scpNew->ple_PatternNumber = 0;
			scpNew->ple_PatternCount = scpDefault->ple_PatternCount;

			for (ped = (const struct PatternEntryDef *) scpDefault->ple_PatternList.lh_Head;
				ped != (const struct PatternEntryDef *) &scpDefault->ple_PatternList.lh_Tail;
				ped = (const struct PatternEntryDef *) ped->ped_Node.ln_Succ)
				{
				struct PatternEntryDef *pedNew;
			
				pedNew = CreatePatternEntryDef();

				if (pedNew)
					{
					*pedNew = *ped;
					pedNew->ped_MUI_AllocatedImageObject = NULL;
					pedNew->ped_SAC = ScalosGfxCreateSAC(1, 1, 8, NULL, NULL);
					pedNew->ped_BitMapArray = NULL;

					AddTail(&scpNew->ple_PatternList, &pedNew->ped_Node);
					}
				}
			}
		
		set(NListPatterns, MUIA_NList_Quiet, MUIV_NList_Quiet_None);
                }

}

//----------------------------------------------------------------------------

static struct PatternEntryDef *CreatePatternEntryDef(void)
{
	struct PatternEntryDef *ped;

	ped = malloc(sizeof(struct PatternEntryDef));
	if (ped)
		{
		ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Number = 1;
		ped->ped_PatternPrefs.scxp_PatternPrefs.scp_RenderType = SCP_RenderType_Tiled;
		ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Flags = SCPF_ENHANCED | SCPF_AUTODITHER;
		ped->ped_PatternPrefs.scxp_PatternPrefs.scp_NumColors = 100;
		ped->ped_PatternPrefs.scxp_PatternPrefs.scp_DitherMode = DITHERMODE_FS;
		ped->ped_PatternPrefs.scxp_PatternPrefs.scp_DitherAmount = 1;
		ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Precision = PRECISION_IMAGE;
		ped->ped_PatternPrefs.scxp_PatternPrefs.scp_Type = SCP_BgType_Picture;

		ped->ped_ThumbnailImageNr = 0;
		ped->ped_SAC = NULL;
		ped->ped_MUI_ImageObject = ped->ped_MUI_AllocatedImageObject = NULL;
		ped->ped_BitMapArray = NULL;
		}

	return ped;
}

//----------------------------------------------------------------------------

static void SetIconSizeConstraints(const struct Rectangle *SizeConstraints)
{
	ULONG MinSize, MaxSize;

	if (SizeConstraints->MinX < 16)
		MinSize = ICONSIZEMIN_Unlimited;
	else if (SizeConstraints->MinX < 24)
		MinSize = ICONSIZEMIN_16;
	else if (SizeConstraints->MinX < 32)
		MinSize = ICONSIZEMIN_24;
	else if (SizeConstraints->MinX < 48)
		MinSize = ICONSIZEMIN_32;
	else if (SizeConstraints->MinX < 64)
		MinSize = ICONSIZEMIN_48;
	else if (SizeConstraints->MinX < 96)
		MinSize = ICONSIZEMIN_64;
	else if (SizeConstraints->MinX < 128)
		MinSize = ICONSIZEMIN_96;
	else
		MinSize = ICONSIZEMIN_128;

	if (SizeConstraints->MaxX <= 16)
		MaxSize = ICONSIZEMAX_16;
	else if (SizeConstraints->MaxX <= 24)
		MaxSize = ICONSIZEMAX_24;
	else if (SizeConstraints->MaxX <= 32)
		MaxSize = ICONSIZEMAX_32;
	else if (SizeConstraints->MaxX <= 48)
		MaxSize = ICONSIZEMAX_48;
	else if (SizeConstraints->MaxX <= 64)
		MaxSize = ICONSIZEMAX_64;
	else if (SizeConstraints->MaxX <= 96)
		MaxSize = ICONSIZEMAX_96;
	else if (SizeConstraints->MaxX <= 128)
		MaxSize = ICONSIZEMAX_128;
	else
		MaxSize = ICONSIZEMAX_Unlimited;

	set(CycleIconMinSize, MUIA_Cycle_Active, MinSize);
	set(CycleIconMaxSize, MUIA_Cycle_Active, MaxSize);
}

//----------------------------------------------------------------------------

static void GetIconSizeConstraints(struct Rectangle *SizeConstraints)
{
	ULONG MinSize = 0, MaxSize = 0;

	get(CycleIconMinSize, MUIA_Cycle_Active, &MinSize);
	get(CycleIconMaxSize, MUIA_Cycle_Active, &MaxSize);

	switch (MinSize)
		{
	case ICONSIZEMIN_16:
		SizeConstraints->MinX = SizeConstraints->MinY = 16;
		break;
	case ICONSIZEMIN_24:
		SizeConstraints->MinX = SizeConstraints->MinY = 24;
		break;
	case ICONSIZEMIN_32:
		SizeConstraints->MinX = SizeConstraints->MinY = 32;
		break;
	case ICONSIZEMIN_48:
		SizeConstraints->MinX = SizeConstraints->MinY = 48;
		break;
	case ICONSIZEMIN_64:
		SizeConstraints->MinX = SizeConstraints->MinY = 64;
		break;
	case ICONSIZEMIN_96:
		SizeConstraints->MinX = SizeConstraints->MinY = 96;
		break;
	case ICONSIZEMIN_128:
		SizeConstraints->MinX = SizeConstraints->MinY = 128;
		break;
	case ICONSIZEMIN_Unlimited:
	default:
		SizeConstraints->MinX = SizeConstraints->MinY = 0;
		break;
		}

	switch (MaxSize)
		{
	case ICONSIZEMAX_16:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 16;
		break;
	case ICONSIZEMAX_24:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 24;
		break;
	case ICONSIZEMAX_32:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 32;
		break;
	case ICONSIZEMAX_48:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 48;
		break;
	case ICONSIZEMAX_64:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 64;
		break;
	case ICONSIZEMAX_96:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 96;
		break;
	case ICONSIZEMAX_128:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 128;
		break;
	case ICONSIZEMAX_Unlimited:
	default:
		SizeConstraints->MaxX = SizeConstraints->MaxY = SHRT_MAX;
		break;
		}
}

//----------------------------------------------------------------------------



// Information.c
// $Date$
// $Revision$

#ifdef __AROS__
#define MUIMASTER_YES_INLINE_STDARG
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include <dos/dos.h>
#include <dos/filehandler.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/mcpgfx.h>
#include <devices/timer.h>
#include <resources/filesysres.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <mui/NListview_mcc.h>
#include <mui/TextEditor_mcc.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <prefs/prefhdr.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/preferences.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/scalos.h>
#include <proto/preferences.h>
#include <proto/wb.h>
#include <proto/timer.h>

#include <DefIcons.h>
#include <defs.h>
#include <Year.h> // +jmc+
#include <int64.h>

#if !defined(MUIA_Application_UsedClasses)
#include <MUIundoc.h>
#endif /* MUIA_Application_UsedClasses */

#include "Information.h"
#include "IconobjectMCC.h"
#include "ToolTypes.h"
#include "FsAbstraction.h"
#include "debug.h"

#define	Information_NUMBERS
#define	Information_ARRAY
#define	Information_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#define	VERSION_MAJOR	40
#define	VERSION_MINOR	13


#define	VERS_MAJOR	STR(VERSION_MAJOR)
#define	VERS_MINOR	STR(VERSION_MINOR)


//----------------------------------------------------------------------------

#define	ID_MAIN	MAKE_ID('M','A','I','N')
#define	PS_DATA(prefsstruct)	((STRPTR) (prefsstruct)) + sizeof((prefsstruct)->ps_Size)

#if !defined(MADF_OBJECTVISIBLE)
#define MADF_OBJECTVISIBLE     (1<<14) // The object is visible
#endif /* MADF_OBJECTVISIBLE */

//----------------------------------------------------------------------------

// local macros

#define Label1S(name) \
	VGroup, \
		Child, VSpace(0), \
		Child, HGroup, \
			Child, HSpace(0), \
			Child, Label1(name), \
		End,  \
		Child, VSpace(0), \
	End //VGroup

#define ConstantText(contents) \
	TextObject, \
		TextFrame, \
		MUIA_Background, MUII_TextBack, \
		MUIA_Text_Contents, (contents), \
	End //Textobject

#define ConstantFloatText(contents) \
	FloattextObject, \
		TextFrame, \
		MUIA_Weight, 0, \
		MUIA_Background, MUII_TextBack, \
		MUIA_Floattext_Text, (contents), \
	End //Textobject

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
		MUIA_ShortHelp    , HelpText,\
		MUIA_CycleChain   , TRUE,\
		End

#define ButtonHelp(name,HelpText,InputMode)\
	TextObject,\
		ButtonFrame,\
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_InputMode    , InputMode,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp    , HelpText,\
		MUIA_CycleChain   , TRUE,\
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

#define UpArrowObject \
	ImageObject, \
		ImageButtonFrame, \
		MUIA_InputMode, MUIV_InputMode_RelVerify, \
		MUIA_Image_Spec, MUII_TapeUp, \
		MUIA_Background, MUII_ButtonBack, \
		MUIA_CycleChain, TRUE, \
		MUIA_ShowSelState, TRUE, \
		End

#define DownArrowObject \
	ImageObject, \
		ImageButtonFrame, \
		MUIA_InputMode, MUIV_InputMode_RelVerify, \
		MUIA_Image_Spec, MUII_TapeDown, \
		MUIA_Background, MUII_ButtonBack, \
		MUIA_CycleChain, TRUE, \
		MUIA_ShowSelState, TRUE, \
		End

#if defined(MUII_StringBack)
	#define	StringBack	MUIA_Background, MUII_StringBack
#else /* MUII_StringBack */
	#define StringBack	TAG_IGNORE, 0
#endif /* MUII_StringBack */

//-----------------------------------------------------------------------------------------------

#define GroupStackSize(stringobj, iconObj, uparrow, downarrow, name, slidobj, privalue) \
	HGroup, \
		Child, HGroup, MUIA_Group_HorizSpacing, 0, \
			Child, stringobj = StringObject, \
				StringFrame, \
				MUIA_String_Integer, GetStackSize(iconObj), \
				MUIA_String_Accept, "0123456789", \
				MUIA_CycleChain, TRUE, \
			End, \
		End, \
				\
		Child, slidobj = SliderObject, \
			MUIA_CycleChain, TRUE, \
			MUIA_Slider_Horiz, TRUE, \
			MUIA_Numeric_Min, -128, \
			MUIA_Numeric_Max, 127, \
			MUIA_Numeric_Value, privalue, \
		End, \
	End

//------------------------------------------------------------------------------------------------------------------

#define	MAX_PATH_SIZE			512

#define	Application_Return_Ok		1001
#define	Application_Return_Path		1002

#define RESULT_HALT	1

#define FIBF_HIDDEN	(1<<7)		// 25.11.2004 Define Hidden protection bit

#define CR	'\x0d'
#define LF	'\n'

//----------------------------------------------------------------------------

// local data structures

typedef void (*DropZoneHandler)(struct WBArg *, Object *, struct DefIconInfo *);

struct FormatDateHookData
	{
	STRPTR fdhd_Buffer;		// buffer to write characters to
	size_t fdhd_Length;		// length of buffer
	};

struct DropZone
	{
	struct Node dz_Node;
	Object *dz_Object;
	struct IBox dz_Box;
	struct AppWindowDropZone *dz_DropZone;
	DropZoneHandler dz_Handler;
	};

//----------------------------------------------------------------------------

// local functions

static void init(void);
static void fail(APTR APP_Main, CONST_STRPTR str);
static void Cleanup(void);
static BOOL OpenLibraries(STRPTR *LibName);
static void CloseLibraries(void);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT DrawerSizeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT DefaultIconHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT AppWindowDropZoneHookFunc(struct Hook *hook, APTR Reserved, struct AppWindowDropZoneMsg *adzm);
static SAVEDS(void) INTERRUPT ChangeIconTypeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT StartFromCycleHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(LONG) INTERRUPT SelectFromButtonHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT IconifyHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT StringIntegerIncrementHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT StringIntegerDecrementHookFunc(struct Hook *hook, Object *o, Msg msg);

static void DropZoneMsgHandler(struct DropZoneMsg *dzm);
static void HandleAppMessage(struct AppMessage *AppMsg, struct DefIconInfo *dii);
static void  ReplaceIcon(Object *NewIconObj, Object **OldIconObj);
static void UpdateDrawerSize(Object *ButtonSize, ULONG *FileCount,
	ULONG *DrawersCount, ULONG *BlocksCount, ULONG64 *ByteCount);
static LONG ExamineDrawer(Object *ButtonSize, CONST_STRPTR Name, ULONG *FileCount,
	ULONG *DrawersCount, ULONG *BlocksCount, ULONG64 *ByteCount, T_TIMEVAL *lastUpdateTime);
static LONG GetElapsedTime(T_TIMEVAL *tv);
static CONST_STRPTR GetLocString(ULONG MsgId);
static void TranslateStringArray(STRPTR *stringArray);
static void SaveSettings(Object *IconObj, Object *originalIconObj, CONST_STRPTR IconName);
static void GetIconObject(struct DefIconInfo *dii, CONST_STRPTR IconName, BPTR DirLock,
	STRPTR FileTypeName, size_t FileTypeNameSize);
static void GetDeviceIconObject(struct DefIconInfo *dii, CONST_STRPTR IconName,
	CONST_STRPTR VolumeName, CONST_STRPTR DeviceName,
	STRPTR FileTypeName, size_t FileTypeNameSize);
static BOOL isDiskWritable(BPTR dLock);
static ULONG CheckInfoData(const struct InfoData *info);
static BOOL IsDevice(struct WBArg *arg);
static void GetDeviceName(BPTR dLock, CONST_STRPTR VolumeName, STRPTR DeviceName, size_t MaxLen);
static BOOL ReadScalosPrefs(void);
static CONST_STRPTR GetPrefsConfigString(APTR prefsHandle, ULONG Id, CONST_STRPTR DefaultString);
static void StripTrailingChar(STRPTR String, char CharToRemove);
static void BuildDefVolumeNameNoSpace(STRPTR Buffer, CONST_STRPTR VolumeName, size_t MaxLength);
static STRPTR ScaFormatString(CONST_STRPTR FormatString, STRPTR Buffer, size_t MaxLen, LONG NumArgs, ...);
static void ScaFormatDate(struct DateTime *dt, ULONG DayMaxLen, ULONG DateMaxLen, ULONG TimeMaxLen);
static M68KFUNC_P3_PROTO(void, FormatDateHookFunc,
	A0, struct Hook *, theHook,
	A2, struct Locale *, locale,
	A1, char, ch);
static void BtoCstring(BSTR bstr, STRPTR Buffer, size_t BuffLen);
static void ByteCount(STRPTR Buffer, size_t BuffLen, LONG NumBlocks, LONG BytesPerBlock);
static UBYTE MakePrintable(UBYTE ch);
static CONST_STRPTR GetIconTypeName(Object *IconObj);
static const struct FileSysEntry *FindFileSysResEntry(ULONG DosType);
static STRPTR GetChangeDate(BOOL Valid, struct DateStamp *ChangeDate);
static ULONG GetStackSize(Object *IconObj);
static STRPTR GetDefaultTool(Object *IconObj);
static STRPTR GetVersionString(CONST_STRPTR IconName);
static void SetGuiFromIconType(Object *IconObj);
static CONST_STRPTR FindVersionString(const UBYTE *block, size_t BlockLen);
const struct Resident *SearchResident(const UWORD *block, size_t BlockLen);
static void ReportError(ULONG MsgId);
static BOOL RenameDevice(CONST_STRPTR OldName, CONST_STRPTR NewName);
static BOOL RenameObject(CONST_STRPTR OldName, CONST_STRPTR NewName);
static void SetupToolTypes(Object *IconObject, Object *TextEditor);
static void SetChangedToolTypes(Object *IconObj, Object *TextEditor);
static STRPTR GetSizeString(void);
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev);
static BOOL ToolTypeNameCmp(CONST_STRPTR ToolTypeName, CONST_STRPTR ToolTypeContents);
static void GetIconObjectFromWBArg(struct DefIconInfo *dii, struct WBArg *arg,
	STRPTR FileTypeName, size_t FileTypeNameSize);
static BOOL RemoveDropZoneForObject(Object *o);
static void AddDropZoneForObject(Object *o, DropZoneHandler Handler);
static void AdjustDropZones(void);
static void HandleDropOnIcon(struct WBArg *arg, Object *o, struct DefIconInfo *dii);
static void HandleDropOnString(struct WBArg *arg, Object *o, struct DefIconInfo *dii);
static void HandleDropOnName(struct WBArg *arg, Object *o, struct DefIconInfo *dii);
static void HandleDropOnToolTypes(struct WBArg *arg, Object *o, struct DefIconInfo *dii);
static BOOL CheckIsWBStartup(BPTR DirLock);
static Object *GetIconObjectFromScalos(BPTR DirLock, CONST_STRPTR IconName,
	STRPTR FileTypeName, size_t FileTypeNameSize);
#if 0
static Object *GetDeviceIconObjectFromScalos(BPTR dLock,
	STRPTR FileTypeName, size_t FileTypeNameSize);
#endif
static void GetIconFileType(struct ScaWindowTask *wt, struct ScaIconNode *in,
	STRPTR FileTypeName, size_t FileTypeNameSize);
static struct InfoData *AllocInfoData(void);
static void FreeInfoData(struct InfoData *pId);
static struct ScaWindowStruct *FindScalosWindow(BPTR dirLock);

//----------------------------------------------------------------------------

// local data items

static UBYTE prefDefIconsFirst = TRUE;		// Flag: try Def-Diskicons first
static CONST_STRPTR prefDefIconPath = "ENV:Sys";

extern struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *IconBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *WorkbenchBase = NULL;
struct Library *IconobjectBase = NULL;
struct ScalosBase *ScalosBase = NULL;
struct Library *PreferencesBase = NULL;
T_LOCALEBASE LocaleBase = NULL;
T_TIMERBASE TimerBase;

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition = NULL;
struct IconIFace *IIcon = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct WorkbenchIFace *IWorkbench = NULL;
struct IconobjectIFace *IIconobject = NULL;
struct ScalosIFace *IScalos = NULL;
struct PreferencesIFace *IPreferences = NULL;
struct LocaleIFace *ILocale = NULL;
struct TimerIFace *ITimer;
#endif

static struct Catalog *InformationCatalog;
static struct Locale *InformationLocale;

static struct MsgPort *TimerPort;
static T_TIMEREQUEST *TimerIO;

static STRPTR DeviceRegisterTitleStrings[] =
	{
	(STRPTR) MSGID_DEVICE_REGISTER_DEVICE,
	(STRPTR) MSGID_DEVICE_REGISTER_ICON,
	NULL
	};

static STRPTR DrawerRegisterTitleStrings[] =
	{
	(STRPTR) MSGID_DRAWER_REGISTER_DRAWER,
	(STRPTR) MSGID_DRAWER_REGISTER_ICON,
	NULL
	};

static STRPTR ProjectRegisterTitleStrings[] =
	{
	(STRPTR) MSGID_PROJECT_REGISTER_PROJECT,
	(STRPTR) MSGID_TOOL_REGISTER_ICON,
	NULL
	};

static STRPTR ToolRegisterTitleStrings[] =
	{
	(STRPTR) MSGID_TOOL_REGISTER_TOOL,
	(STRPTR) MSGID_TOOL_REGISTER_ICON,
	NULL
	};

static STRPTR StartFromCycleChoices[] =
	{
	(STRPTR) MSGID_START_FROM_WORKBENCH,
	(STRPTR) MSGID_START_FROM_CLI,
	(STRPTR) MSGID_START_FROM_AREXX,
	NULL
	};

static struct Hook AboutHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutHookFunc), NULL };
static struct Hook AboutMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIHookFunc), NULL };
static struct Hook ChangeIconTypeHook = { { NULL, NULL }, HOOKFUNC_DEF(ChangeIconTypeHookFunc), NULL };
static struct Hook StartFromCycleHook = { { NULL, NULL }, HOOKFUNC_DEF(StartFromCycleHookFunc), NULL };
static struct Hook DrawerSizeHook =  { { NULL, NULL }, HOOKFUNC_DEF(DrawerSizeHookFunc), NULL };
static struct Hook DefaultIconHook = {{ NULL, NULL }, HOOKFUNC_DEF(DefaultIconHookFunc), NULL };
static struct Hook AppWindowDropZoneHook = {{ NULL, NULL }, HOOKFUNC_DEF(AppWindowDropZoneHookFunc), NULL };
static struct Hook SelectFromButtonHook = {{ NULL, NULL }, HOOKFUNC_DEF(SelectFromButtonHookFunc), NULL }; // +jmc+
static struct Hook IconifyHook = {{ NULL, NULL }, HOOKFUNC_DEF(&IconifyHookFunc), NULL }; // +jmc+
static struct Hook StringIntegerIncrementHook = {{ NULL, NULL }, HOOKFUNC_DEF(&StringIntegerIncrementHookFunc), NULL };
static struct Hook StringIntegerDecrementHook = {{ NULL, NULL }, HOOKFUNC_DEF(&StringIntegerDecrementHookFunc), NULL };

//----------------------------------------------------------------------------

static char TextDeviceName[128];
static char TextDeviceOnDev[64];
static char TextDeviceHandler[128];
static char TextDeviceCreateDate[128];
static char TextDeviceUsed[128];
static char TextDeviceFree[128];
static char TextDeviceSize[128];
static char TextPosXString[64];
static char TextPosYString[64];
static char TextFileTypeName[108];
static char DeviceBlockSize[128];
static char DeviceState[128];
static char TextLastChange[128];
static char TextSize[64];
static char TextVersion[512];
static char InitButtonSize[128];	// +jmc+ : Initial ButtonSize(ButtonDiskSize or ButtonDrawerSize) gadget's contents. 
					// 	   When size calculation is aborted, itinial text is inserted.

//----------------------------------------------------------------------------

static Object *Group_Buttons2;
static Object *APP_Main;
static Object *WIN_Main;
static Object *WIN_AboutMUI;
static Object *OkButton, *CancelButton;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit;
static Object *MenuTypeDisk, *MenuTypeDrawer, *MenuTypeProject;
static Object *MenuTypeTool, *MenuTypeTrashcan;
static Object *MenuImageDefault;
static Object *MenuIconPath, *MenuGetSize;	// +jmc+ : Variables options.
static Object *GroupDisk, *GroupDrawer;
static Object *GroupProject, *GroupTool;
static Object *TextTypeName, *TextOnDev;
static Object *StringStackSizeProject, *StringStackSizeTool;
static Object *PopAslDefaultTool;
static Object *SliderToolPriProject, *SliderToolPriTool;
static Object *CycleStartFromProject, *CycleStartFromTool;
static Object *ButtonPromptForInputProject, *ButtonPromptForInputTool;
static Object *StringCommentDrawer, *StringCommentProject, *StringCommentTool;
static Object *StringCommentDevice;
static Object *TextEditorToolTypesDrawer, *TextEditorToolTypesProject;
static Object *TextEditorToolTypesDevice, *TextEditorToolTypesTool;
static Object *ScrollBarToolTypesDrawer, *ScrollBarToolTypesProject;
static Object *ScrollBarToolTypesDevice, *ScrollBarToolTypesTool;
static Object *StringName;
static Object *ButtonDrawerSize, *ButtonDiskSize;
static Object *TextPosX, *TextPosY;

static Object *CheckMarkProtectionReadDrawer, *CheckMarkProtectionWriteDrawer;
static Object *CheckMarkProtectionArchiveDrawer, *CheckMarkProtectionDeleteDrawer;
static Object *CheckMarkProtectionExecuteDrawer, *CheckMarkProtectionScriptDrawer;


static Object *CheckMarkProtectionHideProject, *CheckMarkProtectionPureProject;
static Object *CheckMarkProtectionReadProject, *CheckMarkProtectionWriteProject;
static Object *CheckMarkProtectionArchiveProject, *CheckMarkProtectionDeleteProject;
static Object *CheckMarkProtectionExecuteProject, *CheckMarkProtectionScriptProject;

static Object *CheckMarkProtectionHideTool, *CheckMarkProtectionPureTool;
static Object *CheckMarkProtectionReadTool, *CheckMarkProtectionWriteTool;
static Object *CheckMarkProtectionArchiveTool, *CheckMarkProtectionDeleteTool;
static Object *CheckMarkProtectionExecuteTool, *CheckMarkProtectionScriptTool;

static Object *SliderStartPriTool, *CheckMarkWaitUntilFinishedTool, *StringWaitTimeTool;
static Object *LabelStartPriTool, *LabelWaitUntilFinishedTool;
static Object *GroupWaitUntilFinished, *GroupIconTool, *GroupWaitTimeTool;
static Object *UpArrowStackSizeProject, *DownArrowStackSizeProject;
static Object *UpArrowStackSizeTool, *DownArrowStackSizeTool;
static Object *UpArrowWaitTimeTool, *DownArrowWaitTimeTool;
static Object *MccIconObj;
static Object *GroupMccIconObj, *Path, *Group_Virtual;						//+jmc+ To switch 0/1 "MUIA_ShowMe" the ScroolGroupObject group.

static Object *SliderStartPriTool2, *CheckMarkWaitUntilFinishedTool2, *StringWaitTimeTool2;
static Object *LabelStartPriTool2, *LabelWaitUntilFinishedTool2;
static Object *GroupWaitUntilFinished2, *GroupIconProject, *GroupWaitTimeTool2, *UpArrowWaitTimeTool2, *DownArrowWaitTimeTool2;

//----------------------------------------------------------------------------

static struct MUI_CustomClass *IconobjectClass;

static T_ExamineData *fib = NULL;
static BOOL FibValid = FALSE;
static LONG Result;

static struct AppWindow *myAppWindow;
static struct MsgPort *AppMsgPort;
static struct MsgPort *DropZoneMsgPort;

static struct Task *myTask;

static struct List DropZoneList;

static BOOL FoundDeviceHandler = FALSE;
static BOOL FoundDeviceDosType = FALSE;
static BOOL FoundDeviceFromArg = FALSE;
static char PathName[512];

//----------------------------------------------------------------------------

static void ScaFormatStringMaxLength(char *Buffer, size_t BuffLen, const char *Format, ...);
static void ScaFormatStringArgs(char *Buffer, size_t BuffLength, const char *Format, APTR Args);
static STRPTR AllocCopyString(CONST_STRPTR clp);
static void FreeCopyString(STRPTR lp);

//----------------------------------------------------------------------------

const char versTag[] = "\0$VER: Scalos Information.module V" VERS_MAJOR "." VERS_MINOR " (" __DATE__ ") " COMPILER_STRING;

#if !defined(__amigaos4__) && !defined(__AROS__)
#include <dos.h>

long _stack = 32768;		// minimum stack size, used by SAS/C startup code
#endif


//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	LONG win_opened = 0;
	BPTR oldDir = (BPTR)NULL;
	CONST_STRPTR IconName = "";
	Object *iconObjFromScalos = NULL;
	Object *iconObjFromDisk = NULL;
	Object *backupIconObjFromDisk = NULL;
	BOOL IsWriteable = TRUE;
	BOOL IsWBStartup = FALSE;
	char VolumeName[128];
	LONG ShowIconPath;
	LONG AutoGetSize;
	char Buffer[25];
	struct DefIconInfo dii;
	struct WBStartup *WBenchMsg =
		(argc == 0) ? (struct WBStartup *)argv : NULL;

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	dii.dii_IconObjectFromDisk = &iconObjFromDisk;
	dii.dii_IconObjectFromScalos = &iconObjFromScalos;
	dii.dii_BackupIconObjectFromDisk = &backupIconObjFromDisk;

	ChangeIconTypeHook.h_Data = DefaultIconHook.h_Data = &dii;

	init();

	myTask = FindTask(NULL);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (!CheckMCCVersion(MUIC_NListview, 19, 66)
		|| !CheckMCCVersion(MUIC_TextEditor, 15, 28))
		{
		Cleanup();
		return 10;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	DropZoneMsgPort	= CreateMsgPort();
	if (NULL == DropZoneMsgPort)
		{
		Cleanup();
		return 10;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	//--- +jmc+ --------------------------------------------------------------------------------------------------

	//--- InfoShowIconPath variable ---
	if (GetVar(INFO_ICONPATH_ENV, Buffer, sizeof(Buffer), GVF_GLOBAL_ONLY) <= 0)
		{
		d1(KPrintF("%s/%s/%ld: %s NOT FOUND\n", __FILE__, __FUNC__, __LINE__, INFO_ICONPATH_ENV));

		ShowIconPath = 1;
		ScaFormatStringMaxLength(Buffer, strlen(Buffer), "%lu", ShowIconPath);

		SetVar(INFO_ICONPATH_ENV, Buffer, -1, GVF_GLOBAL_ONLY);
		}
	else
		{
		unsigned long ulong1;
		d1(KPrintF("%s/%s/%ld: INFO_ICONPATH_ENV=<%s>\n", __FILE__, __FUNC__, __LINE__, Buffer));
		sscanf(Buffer, "%lu", &ulong1);
		ShowIconPath = ulong1;
		}

	//--- InfoAutoGetSize variable ----
	if (GetVar(INFO_AUTOGETSIZE_ENV, Buffer, sizeof(Buffer), GVF_GLOBAL_ONLY) <= 0)
		{
		d1(KPrintF("%s/%s/%ld: %s NOT FOUND\n", __FILE__, __FUNC__, __LINE__, INFO_AUTOGETSIZE_ENV));

		AutoGetSize = 0;
		ScaFormatStringMaxLength(Buffer, strlen(Buffer), "%lu", ShowIconPath);

		SetVar(INFO_AUTOGETSIZE_ENV, Buffer, -1, GVF_GLOBAL_ONLY);
		}
	else
		{
		unsigned long long1;
		d1(KPrintF("%s/%s/%ld: INFO_AUTOGETSIZE_ENV=<%s>\n", __FILE__, __FUNC__, __LINE__, Buffer));
		sscanf(Buffer, "%lu", &long1);
		AutoGetSize = long1;
		}

	//------------------------------------------------------------------------------------------------------------

	TranslateStringArray(StartFromCycleChoices);
	TranslateStringArray(DeviceRegisterTitleStrings);
	TranslateStringArray(DrawerRegisterTitleStrings);
	TranslateStringArray(ProjectRegisterTitleStrings);
	TranslateStringArray(ToolRegisterTitleStrings);

	d1(KPrintF(__FILE__ "/%s/%ld: WBenchMsg=%08lx\n", __FUNC__, __LINE__, WBenchMsg));

	if (WBenchMsg && WBenchMsg->sm_ArgList)
		{
		struct WBArg *arg;

		d1(KPrintF(__FILE__ "/%s/%ld: NumArgs=%lu\n", __FUNC__, __LINE__, WBenchMsg->sm_NumArgs));

		if (WBenchMsg->sm_NumArgs > 1)
			{
			BPTR fLock;

			arg = &WBenchMsg->sm_ArgList[1];

			DrawerSizeHook.h_Data = arg;

			IsWBStartup = CheckIsWBStartup(arg->wa_Lock);

			IconName = arg->wa_Name;

			d1(kprintf(__FILE__ "/%s/%ld: wa_Lock=%08lx\n", __FUNC__, __LINE__, arg->wa_Lock));

			oldDir = CurrentDir(arg->wa_Lock);

			fLock = Lock(arg->wa_Name, ACCESS_READ);
			if (fLock)
				{
				if (ScalosExamineLock(fLock, &fib))
					FibValid = TRUE;
				UnLock(fLock);
				}

			GetIconObjectFromWBArg(&dii, arg, TextFileTypeName, sizeof(TextFileTypeName));

			dii.dii_DirLock = arg->wa_Lock;
			dii.dii_IconName = arg->wa_Name;

			if (IsDevice(arg))
				{
				NameFromLock(arg->wa_Lock, VolumeName, sizeof(VolumeName));
				fLock = Lock(VolumeName, ACCESS_READ);
				if (fLock)
					{
					if (ScalosExamineLock(fLock, &fib))	   // +jmc+ - fib->fib_Comment for WBDISK(Supported by some FileSystems - Not for PFS).
						FibValid = TRUE;
					UnLock(fLock);
					}

				StripTrailingChar(VolumeName, ':');

				d1(kprintf(__FILE__ "/%s/%ld: VolumeName=<%s>\n", __FUNC__, __LINE__, VolumeName));

				IconName = VolumeName;
				}

			IsWriteable = isDiskWritable(arg->wa_Lock);
			}
		}

	d1(KPrintF(__FILE__ "/%s/%ld: iconObjFromDisk=%08lx  iconObjFromScalos=%08lx\n", \
		__FUNC__, __LINE__, iconObjFromDisk, iconObjFromScalos));

	if (iconObjFromDisk)
		{
		const struct ExtGadget *gg = (const struct ExtGadget *) iconObjFromDisk;
		STRPTR *ToolTypesArray = NULL;
		ULONG IconType;
		enum StartFromType StartFrom = STARTFROM_Workbench;
		BOOL PromptForInput = FALSE;
		BOOL WaitUntilFinished = TRUE;
		LONG ToolPri = 0;
		LONG StartPri = 0;
		LONG WaitTime = 0;
		ULONG Protection = FibValid ? ScalosExamineGetProtection(fib) : 0;
		CONST_STRPTR Comment = FibValid ? ScalosExamineGetComment(fib) : (CONST_STRPTR) "";

		d1(kprintf(__FILE__ "/%s/%ld: FibValid=%ld  fib=%08lx\n", __FUNC__, __LINE__, FibValid, fib));

		GetAttr(IDTA_ToolTypes, iconObjFromDisk, (APTR)&ToolTypesArray);
		GetAttr(IDTA_Type, iconObjFromDisk, &IconType);

		if ((0 == gg->LeftEdge && 0 == gg->TopEdge)
			|| NO_ICON_POSITION_SHORT == (UWORD) gg->LeftEdge || NO_ICON_POSITION_SHORT == (UWORD) gg->TopEdge)
			{
			stccpy(TextPosXString, GetLocString(MSGID_ICONPOS_NONE), sizeof(TextPosXString));
			stccpy(TextPosYString, GetLocString(MSGID_ICONPOS_NONE), sizeof(TextPosYString));
			}
		else
			{
			snprintf(TextPosXString, sizeof(TextPosXString), "%d", gg->LeftEdge);
			snprintf(TextPosYString, sizeof(TextPosYString), "%d", gg->TopEdge);
			}

		d1(KPrintF(__FILE__ "/%s/%ld: LeftEdge=%lx  TopEdge=%lx\n", __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge));

		if (ToolTypesArray)
			{
			STRPTR ToolPriString;
			long long1;

			if (FindToolType(ToolTypesArray, "DONOTWAIT"))
				WaitUntilFinished = FALSE;
			else
				WaitUntilFinished = TRUE;

			ToolPriString = FindToolType(ToolTypesArray, "WAIT");
			if (ToolPriString)
				{
				sscanf(ToolPriString, "%ld", &long1);
				WaitTime = long1;
				}
			ToolPriString = FindToolType(ToolTypesArray, "STARTPRI");
			if (ToolPriString)
				{
				sscanf(ToolPriString, "%ld", &long1);
				StartPri = long1;
				}
			if (FindToolType(ToolTypesArray, "CLI"))
				StartFrom = STARTFROM_CLI;
			else if (FindToolType(ToolTypesArray, "REXX"))
				StartFrom = STARTFROM_Arexx;

			if (STARTFROM_CLI == StartFrom || STARTFROM_Arexx == StartFrom)
				{
				PromptForInput = NULL == FindToolType(ToolTypesArray, "DONOTPROMPT");
				}

			ToolPriString = FindToolType(ToolTypesArray, "TOOLPRI");
			if (ToolPriString)
				sscanf(ToolPriString, "%ld", &ToolPri);
			}

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		APP_Main = ApplicationObject,
			MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
			MUIA_Application_Version,	versTag + 1,
			MUIA_Application_Copyright,	"© The Scalos Team, 2004" CURRENTYEAR,
			MUIA_Application_Author,	"The Scalos Team",
			MUIA_Application_Description,	"Scalos Information module",
			MUIA_Application_Base,		"SCALOS_INFORMATION",

			SubWindow, WIN_Main = WindowObject,
				MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
//				MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
				MUIA_Window_AppWindow, FALSE,

				MUIA_Window_TopEdge, MUIV_Window_TopEdge_Moused,
				MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Moused,

				MUIA_Window_Width, MUIV_Window_Width_MinMax(0),
				MUIA_Window_Height, MUIV_Window_Height_MinMax(0),

				WindowContents, VGroup,

					Child, HGroup,
						Child, StringName = StringObject,
							MUIA_Text_PreParse, MUIX_C MUIX_B,
							MUIA_String_Contents, IconName,
							MUIA_String_MaxLen, 108,
							StringFrame,
							StringBack,
							MUIA_String_Format, MUIV_String_Format_Center,
							MUIA_Weight, 100,
							MUIA_CycleChain, TRUE,
						End, //StringObject
						Child, TextOnDev = TextObject,
							MUIA_ShowMe, strlen(TextDeviceOnDev) > 0,
							MUIA_Text_Contents, TextDeviceOnDev,
							MUIA_Weight, 0,
						End, //TextObject
						Child, TextTypeName = TextObject,
							MUIA_Text_Contents, GetIconTypeName(iconObjFromDisk),
							MUIA_Weight, 0,
						End, //TextObject
					End, //HGroup

//---- +jmc+ -----------------------------------------------------------------------------------------------------------------------------------

					Child, Group_Virtual = ScrollgroupObject,
						MUIA_ShowMe, strlen(PathName) > 0 ? ShowIconPath : FALSE,
						MUIA_CycleChain, TRUE,
						MUIA_Scrollgroup_FreeHoriz, TRUE,
						MUIA_Scrollgroup_FreeVert, FALSE,
						MUIA_Scrollgroup_Contents,
						VirtgroupObject,
							Child, Path = TextObject, TextFrame,
								MUIA_Background, MUII_TextBack,
								MUIA_Text_PreParse, MUIX_C,
								MUIA_Text_Contents, PathName,
								MUIA_ShortHelp, GetLocString(MSGID_TEXT_ICONPATH_SHORTHELP),
							End, //TextObject
						End, //VirtgroupObject
					End, //ScrollgroupObject

//----------------------------------------------------------------------------------------------------------------------------------------------
					Child, HGroup,
						Child, GroupMccIconObj = VGroup,
							Child, VSpace(0),

							Child, TextObject,
								MUIA_Text_PreParse, MUIX_C,
								MUIA_Text_Contents, TextFileTypeName,
							End, //TextObject

							Child, VSpace(1),

							Child, MccIconObj = IconobjectMCCObject,
								MUIA_Iconobj_Object, iconObjFromScalos ? iconObjFromScalos : iconObjFromDisk,
								MUIA_InputMode, MUIV_InputMode_Toggle,
								MUIA_ShowSelState, FALSE,
								MUIA_Dropable, TRUE,
							End, //IconobjectMCCObject

							Child, VSpace(1),

							Child, ColGroup(4),
								Child, HSpace(0),

								Child, TextObject,
									MUIA_Text_Contents, GetLocString(MSGID_ICONPOS_X),
								End, //TextObject
								Child, TextPosX = TextObject,
									MUIA_Text_PreParse, MUIX_R,
									MUIA_Text_Contents, TextPosXString,
								End, //TextObject

								Child, HSpace(0),
								Child, HSpace(0),

								Child, TextObject,
									MUIA_Text_Contents, GetLocString(MSGID_ICONPOS_Y),
								End, //TextObject
								Child, TextPosY = TextObject,
									MUIA_Text_PreParse, MUIX_R,
									MUIA_Text_Contents, TextPosYString,
								End, //TextObject

								Child, HSpace(0),
							End, //ColGroup(2)

							Child, VSpace(0),
						End, //VGroup

						Child, VGroup,
							MUIA_Background, MUII_GroupBack,

							Child, GroupDisk = RegisterObject,
								MUIA_Register_Titles, DeviceRegisterTitleStrings,
								MUIA_CycleChain, TRUE,
								MUIA_ShowMe, (WBDISK == IconType),

								Child,	VGroup,
									GroupFrame,
									Child, ColGroup(2),
										strlen(TextDeviceName) > 0 ? Child : TAG_IGNORE, Label1(GetLocString(MSGID_DEVICE_DEVICE)),
										strlen(TextDeviceName) > 0 ? Child : TAG_IGNORE, ConstantText(TextDeviceName),

										// strlen(TextDeviceHandler) > 0 ? Child : TAG_IGNORE, Label1(GetLocString(MSGID_DEVICE_HANDLER)),
										FoundDeviceHandler ? Child : TAG_IGNORE, Label1(GetLocString(MSGID_DEVICE_HANDLER)),
										(!FoundDeviceHandler && FoundDeviceDosType) ? Child : TAG_IGNORE, Label1(GetLocString(MSGID_DEVICE_DOSTYPE)),

										strlen(TextDeviceHandler) > 0 ? Child : TAG_IGNORE, ConstantFloatText(TextDeviceHandler),

										Child, Label1(GetLocString(MSGID_DEVICE_CREATED_ON)),
										Child, ConstantText(TextDeviceCreateDate),

										Child, Label1(GetLocString(MSGID_DEVICE_SPACE_USED)),
										Child, ButtonDiskSize = ButtonHelp(TextDeviceUsed, GetLocString(MSGID_DRAWER_SIZE_SHORTHELP), MUIV_InputMode_Toggle),

										Child, Label1(GetLocString(MSGID_DEVICE_SPACE_FREE)),
										Child, ConstantText(TextDeviceFree),

										Child, Label1(GetLocString(MSGID_DEVICE_SIZE)),
										Child, ConstantText(TextDeviceSize),

										Child, Label1(GetLocString(MSGID_DEVICE_BLOCK_SIZE)),
										Child, ConstantText(DeviceBlockSize),

										Child, Label1(GetLocString(MSGID_DEVICE_STATE)),
										Child, ConstantText(DeviceState),

										Child, Label1S(GetLocString(MSGID_COMMENT)),
										Child, StringCommentDevice = StringObject,
											StringFrame,
											StringBack,
											MUIA_String_Contents, (ULONG) Comment,
											MUIA_CycleChain, TRUE,
											MUIA_Dropable, TRUE,
										End, //StringObject
									End, //ColGroup
								End, //VGroup

								Child,	VGroup,
									GroupFrame,

									Child, ColGroup(2),
										Child, VGroup,
											Child, VSpace(0),
											Child, Label1(GetLocString(MSGID_TOOLTYPES)),
											Child, VSpace(0),
										End, //VGroup
										Child, HGroup,
											Child, TextEditorToolTypesDevice = TextEditorObject,
												TextFrame,
												MUIA_Background, MUII_TextBack,
												MUIA_CycleChain, TRUE,
												//MUIA_Textinput_Multiline, TRUE,
												MUIA_Dropable, TRUE,
											End, //TextEditorObject
											Child, ScrollBarToolTypesDevice = ScrollbarObject,
												MUIA_Background, MUII_PropBack,
												SliderFrame,
											End, //ScrollbarObject
										End, //HGroup
									End, //ColGroup
								End, //VGroup
							End, //RegisterObject

							Child,	GroupDrawer = RegisterObject,
								MUIA_Register_Titles, DrawerRegisterTitleStrings,
								MUIA_CycleChain, TRUE,
								MUIA_ShowMe, (WBDRAWER == IconType) || (WBGARBAGE == IconType),

								Child,	VGroup,
									GroupFrame,
									Child, ColGroup(2),
										Child, Label1S(GetLocString(MSGID_LASTCHANGE)),
										Child, ConstantText(GetChangeDate(FibValid, ScalosExamineGetDate(fib))),
										Child, Label1S(GetLocString(MSGID_SIZE)),
										Child, ButtonDrawerSize = ButtonHelp(GetLocString(MSGID_INITIAL_DRAWER_SIZE), GetLocString(MSGID_DRAWER_SIZE_SHORTHELP), MUIV_InputMode_Toggle),
										Child, Label1S(GetLocString(MSGID_PROTECTION)),
										Child, VGroup,
											Child, VSpace(0),
											Child, HGroup,
												GroupFrame,
												Child, HSpace(0),
												Child, ColGroup(2 * 3),
													MUIA_Disabled, !FibValid || !IsWriteable,

													Child, Label1(GetLocString(MSGID_PROTECTION_READ)),
													Child, CheckMarkProtectionReadDrawer =  CheckMarkHelp(!(Protection & FIBF_READ), MSGID_PROTECTION_READ_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_WRITE)),
													Child, CheckMarkProtectionWriteDrawer = CheckMarkHelp(!(Protection & FIBF_WRITE), MSGID_PROTECTION_WRITE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_ARCHIVE)),
													Child, CheckMarkProtectionArchiveDrawer = CheckMarkHelp(Protection & FIBF_ARCHIVE, MSGID_PROTECTION_ARCHIVE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_DELETE)),
													Child, CheckMarkProtectionDeleteDrawer = CheckMarkHelp(!(Protection & FIBF_DELETE), MSGID_PROTECTION_DELETE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_EXECUTE)),
													Child, CheckMarkProtectionExecuteDrawer = CheckMarkHelp(!(Protection & FIBF_EXECUTE), MSGID_PROTECTION_EXECUTE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_SCRIPT)),
													Child, CheckMarkProtectionScriptDrawer = CheckMarkHelp(Protection & FIBF_SCRIPT, MSGID_PROTECTION_SCRIPT_SHORTHELP),
												End, //ColGroup
												Child, HSpace(0),
											End, //HGroup
											Child, VSpace(0),
										End, //VGroup
										Child, Label1S(GetLocString(MSGID_COMMENT)),
										Child, StringCommentDrawer = StringObject,
											StringFrame,
											StringBack,
											MUIA_String_Contents, Comment,
											MUIA_CycleChain, TRUE,
											MUIA_Dropable, TRUE,
										End, //StringObject
									End, //ColGroup
								End, //VGroup

								Child,	VGroup,
									GroupFrame,

									Child, ColGroup(2),
										Child, VGroup,
											Child, VSpace(0),
											Child, Label1(GetLocString(MSGID_TOOLTYPES)),
											Child, VSpace(0),
										End, //VGroup
										Child, HGroup,
											Child, TextEditorToolTypesDrawer = TextEditorObject,
												TextFrame,
												MUIA_Background, MUII_TextBack,
												MUIA_CycleChain, TRUE,
												//MUIA_Textinput_Multiline, TRUE,
												MUIA_Dropable, TRUE,
											End, //TextEditorObject
											Child, ScrollBarToolTypesDrawer = ScrollbarObject,
												MUIA_Background, MUII_PropBack,
												SliderFrame,
											End, //ScrollbarObject
										End, //HGroup
									End, //ColGroup
								End, //VGroup

							End, //RegisterObject

							Child,	GroupProject = RegisterObject,
								MUIA_Register_Titles, ProjectRegisterTitleStrings,
								MUIA_CycleChain, TRUE,
								MUIA_ShowMe, WBPROJECT == IconType,

								Child,	VGroup,
									GroupFrame,
									Child, ColGroup(2),
										Child, Label1S(GetLocString(MSGID_VERSION)),
										Child, ConstantFloatText(GetVersionString(IconName)),

										Child, Label1S(GetLocString(MSGID_LASTCHANGE)),
										Child, ConstantText(GetChangeDate(FibValid, ScalosExamineGetDate(fib))),

										Child, Label1S(GetLocString(MSGID_SIZE)),
										Child, ConstantText(GetSizeString()),

										Child, Label1S(GetLocString(MSGID_PROTECTION)),
										Child, VGroup,
											Child, VSpace(0),
											Child, HGroup,
												GroupFrame,
												Child, HSpace(0),

												Child, ColGroup(2 * 4),
													MUIA_Disabled, !FibValid || !IsWriteable,

													Child, Label1(GetLocString(MSGID_PROTECTION_HIDDEN)),
													Child, CheckMarkProtectionHideProject = CheckMarkHelp((Protection & FIBF_HIDDEN), MSGID_PROTECTION_HIDDEN_SHORTHELP),


													Child, Label1(GetLocString(MSGID_PROTECTION_READ)),
													Child, CheckMarkProtectionReadProject = CheckMarkHelp(!(Protection & FIBF_READ), MSGID_PROTECTION_READ_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_WRITE)),
													Child, CheckMarkProtectionWriteProject = CheckMarkHelp(!(Protection & FIBF_WRITE), MSGID_PROTECTION_WRITE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_ARCHIVE)),
													Child, CheckMarkProtectionArchiveProject = CheckMarkHelp(Protection & FIBF_ARCHIVE, MSGID_PROTECTION_ARCHIVE_SHORTHELP),


													Child, Label1(GetLocString(MSGID_PROTECTION_DELETE)),
													Child, CheckMarkProtectionDeleteProject = CheckMarkHelp(!(Protection & FIBF_DELETE), MSGID_PROTECTION_DELETE_SHORTHELP),


													Child, Label1(GetLocString(MSGID_PROTECTION_PURE)),
													Child, CheckMarkProtectionPureProject = CheckMarkHelp((Protection & FIBF_PURE), MSGID_PROTECTION_PURE_SHORTHELP),


													Child, Label1(GetLocString(MSGID_PROTECTION_EXECUTE)),
													Child, CheckMarkProtectionExecuteProject = CheckMarkHelp(!(Protection & FIBF_EXECUTE), MSGID_PROTECTION_EXECUTE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_SCRIPT)),
													Child, CheckMarkProtectionScriptProject = CheckMarkHelp(Protection & FIBF_SCRIPT, MSGID_PROTECTION_SCRIPT_SHORTHELP),
												End, //ColGroup

												Child, HSpace(0),
											End, //HGroup
											Child, VSpace(0),
										End, //VGroup

										Child, Label1S(GetLocString(MSGID_COMMENT)),
										Child, StringCommentProject = StringObject,
											StringFrame,
											StringBack,
											MUIA_String_Contents, (ULONG) Comment,
											MUIA_CycleChain, TRUE,
											MUIA_Dropable, TRUE,
										End, //TextObject
									End, //ColGroup
								End, //VGroup

								Child,	VGroup,
									GroupFrame,

									Child, GroupIconProject = ColGroup(2),
										Child, Label1(GetLocString(MSGID_DEFAULTTOOL)),
										Child, PopAslDefaultTool = PopaslObject,
											MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
											MUIA_Dropable, TRUE,
											MUIA_Popstring_String, StringObject,
												StringFrame,
												StringBack,
												MUIA_String_Contents, GetDefaultTool(iconObjFromDisk),
												MUIA_CycleChain, TRUE,
											End, //StringObject
//											  ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_SCALOS_HOME_ASLTITLE),
//											  MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_SCALOS_HOME_SHORTHELP),
										End, //PopaslObject


										Child, Label1(GetLocString(MSGID_STACKSIZE)),
										Child, HGroup,
											Child, HGroup,
												MUIA_Group_HorizSpacing, 0,
												Child, StringStackSizeProject = StringObject,
													StringFrame,
													StringBack,
													MUIA_String_Integer, GetStackSize(iconObjFromDisk),
													MUIA_String_Accept, "0123456789",
													MUIA_CycleChain, TRUE,
												End, //StringObject
												Child, VGroup,
													MUIA_Group_VertSpacing, 0,
													Child, UpArrowStackSizeProject = UpArrowObject,
													Child, DownArrowStackSizeProject = DownArrowObject,
													End, //VGroup
												End, //HGroup

											Child, Label1(GetLocString(MSGID_TOOL_PRIORITY)),
											Child, SliderToolPriProject = SliderObject,
												MUIA_CycleChain, TRUE,
												MUIA_Slider_Horiz, TRUE,
												MUIA_Numeric_Min, -128,
												MUIA_Numeric_Max, 127,
												MUIA_Numeric_Value, ToolPri,
											End, //SliderObject
										End, //HGroup

										Child, Label1(GetLocString(MSGID_START_FROM)),
										Child, HGroup,
											Child, CycleStartFromProject = CycleObject,
												MUIA_CycleChain, TRUE,
												MUIA_Cycle_Entries, StartFromCycleChoices,
												MUIA_Cycle_Active, StartFrom,
											End, //CycleObject

											Child, Label1(GetLocString(MSGID_PROMPT_FOR_INPUT)),
											Child, ButtonPromptForInputProject = ImageObject,
												ButtonFrame,
												MUIA_InputMode, MUIV_InputMode_Toggle,
												MUIA_Image_Spec, MUII_CheckMark,
												MUIA_Selected, PromptForInput,
												MUIA_Background, MUII_ButtonBack,
												MUIA_ShowSelState, FALSE,
											End, //Image
										End, //HGroup

										Child, VGroup,
											Child, VSpace(0),
											Child, Label1(GetLocString(MSGID_TOOLTYPES)),
											Child, VSpace(0),
										End, //VGroup

										Child, HGroup,
											Child, TextEditorToolTypesProject = TextEditorObject,
												TextFrame,
												MUIA_Background, MUII_TextBack,
												MUIA_CycleChain, TRUE,
												//MUIA_Textinput_Multiline, TRUE,
												MUIA_Dropable, TRUE,
											End, //TextEditorObject
											Child, ScrollBarToolTypesProject = ScrollbarObject,
												MUIA_Background, MUII_PropBack,
												SliderFrame,
											End, //ScrollbarObject
										End, //HGroup

//************************************************************* WBStartup members ****************************************************************

										// the following members are only used for icons located in WBStartup
										// for all other directories, those members will be removed later
										Child, LabelStartPriTool2 = Label1S(GetLocString(MSGID_STARTPRI)),
										Child, SliderStartPriTool2 = SliderObject,
											MUIA_CycleChain, TRUE,
											MUIA_Slider_Horiz, TRUE,
											MUIA_Numeric_Min, -128,
											MUIA_Numeric_Max, 127,
											MUIA_Numeric_Value, StartPri,
										End, //SliderObject

										Child, LabelWaitUntilFinishedTool2 = Label1S(GetLocString(MSGID_WAITUNTILFINISHED)),
										Child, GroupWaitUntilFinished2 = HGroup,
											Child, CheckMarkWaitUntilFinishedTool2 = CheckMarkHelp(WaitUntilFinished, MSGID_WAITUNTILFINISHED_SHORTHELP),

											Child, GroupWaitTimeTool2 = HGroup,
												MUIA_Group_HorizSpacing, 0,
												MUIA_Disabled, !WaitUntilFinished,

												Child, StringWaitTimeTool2 = StringObject,
													StringFrame,
													StringBack,
													MUIA_String_Integer, WaitTime,
													MUIA_String_Accept, "0123456789",
													MUIA_CycleChain, TRUE,
												End, //StringObject
												Child, VGroup,
													MUIA_Group_VertSpacing, 0,
													Child, UpArrowWaitTimeTool2 = UpArrowObject,
													Child, DownArrowWaitTimeTool2 = DownArrowObject,
												End, //VGroup
											End, //HGroup

											Child, Label1(GetLocString(MSGID_TOOL_SECONDS)),
										End, //HGroup

//*************************************************************************************************************************************************

									End, //ColGroup
								End, //VGroup

							End, //RegisterObject

							Child,	GroupTool = RegisterObject,
								MUIA_Register_Titles, ToolRegisterTitleStrings,
								MUIA_CycleChain, TRUE,
								MUIA_ShowMe, WBTOOL == IconType,

								Child,	VGroup,
									GroupFrame,
									Child, ColGroup(2),
										Child, Label1S(GetLocString(MSGID_VERSION)),
										Child, ConstantFloatText(GetVersionString(IconName)),

										Child, Label1S(GetLocString(MSGID_LASTCHANGE)),
										Child, ConstantText(GetChangeDate(FibValid, ScalosExamineGetDate(fib))),

										Child, Label1S(GetLocString(MSGID_SIZE)),
										Child, ConstantText(GetSizeString()),

										Child, Label1S(GetLocString(MSGID_PROTECTION)),
										Child, VGroup,
											Child, VSpace(0),
											Child, HGroup,
												GroupFrame,
												Child, HSpace(0),

												Child, ColGroup(2 * 4),
													MUIA_Disabled, !FibValid || !IsWriteable,

													Child, Label1(GetLocString(MSGID_PROTECTION_HIDDEN)),
													Child, CheckMarkProtectionHideTool = CheckMarkHelp((Protection & FIBF_HIDDEN), MSGID_PROTECTION_HIDDEN_SHORTHELP),


													Child, Label1(GetLocString(MSGID_PROTECTION_READ)),
													Child, CheckMarkProtectionReadTool = CheckMarkHelp(!(Protection & FIBF_READ), MSGID_PROTECTION_READ_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_WRITE)),
													Child, CheckMarkProtectionWriteTool = CheckMarkHelp(!(Protection & FIBF_WRITE), MSGID_PROTECTION_WRITE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_ARCHIVE)),
													Child, CheckMarkProtectionArchiveTool = CheckMarkHelp(Protection & FIBF_ARCHIVE, MSGID_PROTECTION_ARCHIVE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_DELETE)),
													Child, CheckMarkProtectionDeleteTool = CheckMarkHelp(!(Protection & FIBF_DELETE), MSGID_PROTECTION_DELETE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_PURE)),
													Child, CheckMarkProtectionPureTool = CheckMarkHelp((Protection & FIBF_PURE), MSGID_PROTECTION_PURE_SHORTHELP),


													Child, Label1(GetLocString(MSGID_PROTECTION_EXECUTE)),
													Child, CheckMarkProtectionExecuteTool = CheckMarkHelp(!(Protection & FIBF_EXECUTE), MSGID_PROTECTION_EXECUTE_SHORTHELP),

													Child, Label1(GetLocString(MSGID_PROTECTION_SCRIPT)),
													Child, CheckMarkProtectionScriptTool = CheckMarkHelp(Protection & FIBF_SCRIPT, MSGID_PROTECTION_SCRIPT_SHORTHELP),
												End, //ColGroup

												Child, HSpace(0),
											End, //HGroup
											Child, VSpace(0),
										End, //VGroup

										Child, Label1S(GetLocString(MSGID_COMMENT)),
										Child, StringCommentTool = StringObject,
											StringFrame,
											StringBack,
											MUIA_String_Contents, (ULONG) Comment,
											MUIA_CycleChain, TRUE,
											MUIA_Dropable, TRUE,
										End, //TextObject
									End, //ColGroup
								End, //VGroup

								Child,	VGroup,
									GroupFrame,

									Child, GroupIconTool = ColGroup(2),


									Child, Label1S(GetLocString(MSGID_STACKSIZE)),
										Child, HGroup,
											Child, HGroup,
												MUIA_Group_HorizSpacing, 0,
												Child, StringStackSizeTool = StringObject,
													StringFrame,
													StringBack,
													MUIA_String_Integer, GetStackSize(iconObjFromDisk),
													MUIA_String_Accept, "0123456789",
													MUIA_CycleChain, TRUE,
												End, //StringObject
												Child, VGroup,
													MUIA_Group_VertSpacing, 0,
													Child, UpArrowStackSizeTool = UpArrowObject,
													Child, DownArrowStackSizeTool = DownArrowObject,
													End, //VGroup
												End, //HGroup

											Child, Label1(GetLocString(MSGID_TOOL_PRIORITY)),
											Child, SliderToolPriTool = SliderObject,
												MUIA_CycleChain, TRUE,
												MUIA_Slider_Horiz, TRUE,
												MUIA_Numeric_Min, -128,
												MUIA_Numeric_Max, 127,
												MUIA_Numeric_Value, ToolPri,
											End, //SliderObject
										End, //HGroup

										Child, Label1S(GetLocString(MSGID_START_FROM)),
										Child, HGroup,
											Child, CycleStartFromTool = CycleObject,
												MUIA_Cycle_Entries, StartFromCycleChoices,
												MUIA_Cycle_Active, StartFrom,
												MUIA_CycleChain, TRUE,
											End, //CycleObject

											Child, Label1(GetLocString(MSGID_PROMPT_FOR_INPUT)),
											Child, ButtonPromptForInputTool = ImageObject,
												ButtonFrame,
												MUIA_InputMode, MUIV_InputMode_Toggle,
												MUIA_Image_Spec, MUII_CheckMark,
												MUIA_Selected, PromptForInput,
												MUIA_Background, MUII_ButtonBack,
												MUIA_ShowSelState, FALSE,
											End, //Image
										End, //HGroup

										Child, VGroup,
											Child, VSpace(0),
											Child, Label1(GetLocString(MSGID_TOOLTYPES)),
											Child, VSpace(0),
										End, //VGroup

										Child, HGroup,
											Child, TextEditorToolTypesTool = TextEditorObject,
												TextFrame,
												MUIA_Background, MUII_TextBack,
												MUIA_CycleChain, TRUE,
												//MUIA_Textinput_Multiline, TRUE,
												MUIA_Dropable, TRUE,
											End, //TextEditorObject
											Child, ScrollBarToolTypesTool = ScrollbarObject,
												MUIA_Background, MUII_PropBack,
												SliderFrame,
											End, //ScrollbarObject
										End, //HGroup

										// the following members are only used for icons located in WBStartup
										// for all other directories, those members will be removed later
										Child, LabelStartPriTool = Label1S(GetLocString(MSGID_STARTPRI)),
										Child, SliderStartPriTool = SliderObject,
											MUIA_CycleChain, TRUE,
											MUIA_Slider_Horiz, TRUE,
											MUIA_Numeric_Min, -128,
											MUIA_Numeric_Max, 127,
											MUIA_Numeric_Value, StartPri,
											End, //SliderObject

										Child, LabelWaitUntilFinishedTool = Label1S(GetLocString(MSGID_WAITUNTILFINISHED)),
										Child, GroupWaitUntilFinished = HGroup,
											Child, CheckMarkWaitUntilFinishedTool = CheckMarkHelp(WaitUntilFinished, MSGID_WAITUNTILFINISHED_SHORTHELP),

											Child, GroupWaitTimeTool = HGroup,
												MUIA_Group_HorizSpacing, 0,
												MUIA_Disabled, !WaitUntilFinished,

												Child, StringWaitTimeTool = StringObject,
													StringFrame,
													StringBack,
													MUIA_String_Integer, WaitTime,
													MUIA_String_Accept, "0123456789",
													MUIA_CycleChain, TRUE,
												End, //StringObject
												Child, VGroup,
													MUIA_Group_VertSpacing, 0,
													Child, UpArrowWaitTimeTool = UpArrowObject,
													Child, DownArrowWaitTimeTool = DownArrowObject,
													End, //VGroup
												End, //HGroup

											Child, Label1(GetLocString(MSGID_TOOL_SECONDS)),
										End, //HGroup
									End, //ColGroup
								End, //VGroup

							End, //RegisterObject
						End, //VGroup
					End, //HGroup

//
					Child, VSpace(0),

					Child, Group_Buttons2 = HGroup,
						MUIA_Group_SameWidth, TRUE,
						Child, OkButton = KeyButtonHelp(GetLocString(MSGID_OKBUTTON), 
									'o', GetLocString(MSGID_SHORTHELP_OKBUTTON)),
						Child, CancelButton = KeyButtonHelp(GetLocString(MSGID_CANCELBUTTON), 
									'c', GetLocString(MSGID_SHORTHELP_CANCELBUTTON)),
						End, //HGroup
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

				Child, MenuObjectT(GetLocString(MSGID_MENU_TYPE)),

					Child, MenuTypeDisk = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_TYPE_DISK),
						MUIA_Menuitem_Shortcut, "1",
						MUIA_Menuitem_Checkit, TRUE,
						MUIA_Menuitem_Exclude, 0x1e,
					End,
					Child, MenuTypeDrawer = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_TYPE_DRAWER),
						MUIA_Menuitem_Shortcut, "2",
						MUIA_Menuitem_Checkit, TRUE,
						MUIA_Menuitem_Exclude, 0x1d,
					End,
					Child, MenuTypeProject = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_TYPE_PROJECT),
						MUIA_Menuitem_Shortcut, "3",
						MUIA_Menuitem_Checkit, TRUE,
						MUIA_Menuitem_Exclude, 0x1b,
					End,
					Child, MenuTypeTool = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_TYPE_TOOL),
						MUIA_Menuitem_Shortcut, "4",
						MUIA_Menuitem_Checkit, TRUE,
						MUIA_Menuitem_Exclude, 0x17,
					End,
					Child, MenuTypeTrashcan = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_TYPE_TRASHCAN),
						MUIA_Menuitem_Shortcut, "5",
						MUIA_Menuitem_Checkit, TRUE,
						MUIA_Menuitem_Exclude, 0x0f,
					End,
				End, //MenuObjectT

				Child, MenuObjectT(GetLocString(MSGID_MENU_IMAGE)),

					Child, MenuImageDefault = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_IMAGE_DEFAULT),
						MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_IMAGE_DEFAULT_SHORT),
					End,
				End, //MenuObjectT

//--- +jmc+ --------------------------------------------------------------------------------------------------------------

				Child, MenuObjectT(GetLocString(MSGID_MENU_OPTIONS)),
					Child, MenuIconPath = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_ALWAYS_ICONPATH),
						MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_ALWAYS_ICONPATH_SHORT),
						MUIA_Menuitem_Checkit, TRUE,
						MUIA_Menuitem_Toggle, TRUE,
						// MUIA_Menuitem_Enabled, (WBDISK != IconType),
					End,
					Child, MenuGetSize = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_ALWAYS_GETSIZE),
						MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_ALWAYS_GETSIZE_SHORT),
						MUIA_Menuitem_Checkit, TRUE,
						MUIA_Menuitem_Toggle, TRUE,
						MUIA_Menuitem_Enabled, (WBDRAWER == IconType) || (WBGARBAGE == IconType),
					End,
				End, //MenuObjectT

//------------------------------------------------------------------------------------------------------------------------

			End, //MenuStripObject

		End; //ApplicationObject

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		if (NULL == APP_Main)
			{
			fail(APP_Main, GetLocString(MSGID_CREATE_APPLICATION_FAILED));
			}

		SetGuiFromIconType(iconObjFromDisk);

		//--------------------------------------------------------------------------//

		// Connect TextEditor objects with their scrollbars

		set(TextEditorToolTypesProject, MUIA_TextEditor_Slider, ScrollBarToolTypesProject);
		set(TextEditorToolTypesTool,    MUIA_TextEditor_Slider, ScrollBarToolTypesTool);
		set(TextEditorToolTypesDrawer,  MUIA_TextEditor_Slider, ScrollBarToolTypesDrawer);
		set(TextEditorToolTypesDevice,  MUIA_TextEditor_Slider, ScrollBarToolTypesDevice);

		//--------------------------------------------------------------------------//

		// connect UpArrow and DownArrow with the corresponding string Gadgets

		DoMethod(UpArrowWaitTimeTool, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			StringWaitTimeTool, 2, MUIM_CallHook, &StringIntegerIncrementHook);
		DoMethod(DownArrowWaitTimeTool, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			StringWaitTimeTool, 2, MUIM_CallHook, &StringIntegerDecrementHook);

		DoMethod(UpArrowWaitTimeTool2, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			StringWaitTimeTool2, 2, MUIM_CallHook, &StringIntegerIncrementHook);
		DoMethod(DownArrowWaitTimeTool2, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			StringWaitTimeTool2, 2, MUIM_CallHook, &StringIntegerDecrementHook);

		DoMethod(UpArrowStackSizeProject, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			StringStackSizeProject, 2, MUIM_CallHook, &StringIntegerIncrementHook);
		DoMethod(DownArrowStackSizeProject, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			StringStackSizeProject, 2, MUIM_CallHook, &StringIntegerDecrementHook);

		DoMethod(UpArrowStackSizeTool, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			StringStackSizeTool, 2, MUIM_CallHook, &StringIntegerIncrementHook);
		DoMethod(DownArrowStackSizeTool, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			StringStackSizeTool, 2, MUIM_CallHook, &StringIntegerDecrementHook);

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
			WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			WIN_Main, 3 , MUIM_WriteLong, RESULT_HALT, &Result);

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 3 , MUIM_WriteLong, RESULT_HALT, &Result);

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

		// Call IconifyHook every time we get iconified or uniconified
		DoMethod(APP_Main, MUIM_Notify, MUIA_Application_Iconified, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &IconifyHook);

		// Get drawer or disk size.
		// Call getsize's hook when buttons are selected.
		DoMethod(ButtonDrawerSize, MUIM_Notify, MUIA_Selected, TRUE,
			APP_Main, 2, MUIM_CallHook, &DrawerSizeHook);

		DoMethod(ButtonDiskSize, MUIM_Notify, MUIA_Selected, TRUE,
			APP_Main, 2, MUIM_CallHook, &DrawerSizeHook);

		// +jmc+ -------------------------------------------------------------------//

		// Set up a notification checking buttons state everytime.
		// Unselected = RESULT_HALT / Selected = RETURN_OK.
		DoMethod(ButtonDiskSize, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			ButtonDiskSize, 3, MUIM_CallHook, &SelectFromButtonHook);

		DoMethod(ButtonDrawerSize, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			ButtonDrawerSize, 3, MUIM_CallHook, &SelectFromButtonHook);

		//--- +jmc+ : variables menu's options -----------------------------------
		// Menu options methods - Icon path.
		DoMethod(MenuIconPath, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
			APP_Main, 3, MUIM_WriteLong, MUIV_TriggerValue, &ShowIconPath);

		DoMethod(MenuIconPath, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, Application_Return_Path);

		// Menu options methods - Auto GetSize.
		DoMethod(MenuGetSize, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
			APP_Main, 3, MUIM_WriteLong, MUIV_TriggerValue, &AutoGetSize);

		//--------------------------------------------------------------------------//

		// Setup notification when icon type is changed via menu
		DoMethod(MenuTypeDisk, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
			APP_Main, 3, MUIM_CallHook, &ChangeIconTypeHook, WBDISK);
		DoMethod(MenuTypeDrawer, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
			APP_Main, 3, MUIM_CallHook, &ChangeIconTypeHook, WBDRAWER);
		DoMethod(MenuTypeProject, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
			APP_Main, 3, MUIM_CallHook, &ChangeIconTypeHook, WBPROJECT);
		DoMethod(MenuTypeTool, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
			APP_Main, 3, MUIM_CallHook, &ChangeIconTypeHook, WBTOOL);
		DoMethod(MenuTypeTrashcan, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
			APP_Main, 3, MUIM_CallHook, &ChangeIconTypeHook, WBGARBAGE);

		// call hook when menu command "icon image/default" is issued
		DoMethod(MenuImageDefault, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &DefaultIconHook);

		// enable/disable the ButtonPromptForInput****,
		// according to state of CycleStartFrom****
		DoMethod(CycleStartFromProject, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
			CycleStartFromProject, 2, MUIM_CallHook, &StartFromCycleHook);
		DoMethod(CycleStartFromTool, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
			CycleStartFromTool, 2, MUIM_CallHook, &StartFromCycleHook);

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		// disable Ok button for read-only icons
		set(OkButton, MUIA_Disabled, !IsWriteable);

		//-- +jmc+ : Variables options ----------------------------------------------------

		// Check on or off Menuitem to always display icon path gadget.
		set(MenuIconPath,MUIA_Menuitem_Checked, ShowIconPath);

		// Check on or off Menuitem to always display icon path gadget.
		set(MenuGetSize, MUIA_Menuitem_Checked, AutoGetSize);

		//------------------------------------------------------------------------

		d1(KPrintF(__FILE__ "/%s/%ld: PathName lenght = %ld\n", __FUNC__, __LINE__, strlen(PathName)));

		// disable all input gadgets for read-only icons
		set(StringCommentDevice, MUIA_Disabled, !IsWriteable);
		set(StringCommentDrawer, MUIA_Disabled, !IsWriteable);
		set(StringCommentProject, MUIA_Disabled, !IsWriteable);
		set(StringCommentTool, MUIA_Disabled, !IsWriteable);
		set(StringName, MUIA_Disabled, !IsWriteable);
		set(TextEditorToolTypesDevice, MUIA_Disabled, !IsWriteable);
		set(TextEditorToolTypesDrawer, MUIA_Disabled, !IsWriteable);
		set(TextEditorToolTypesProject, MUIA_Disabled, !IsWriteable);
		set(TextEditorToolTypesTool, MUIA_Disabled, !IsWriteable);
		set(StringStackSizeProject, MUIA_Disabled, !IsWriteable);
		set(StringStackSizeTool, MUIA_Disabled, !IsWriteable);
		set(PopAslDefaultTool, MUIA_Disabled, !IsWriteable);
		set(SliderToolPriProject, MUIA_Disabled, !IsWriteable);
		set(SliderToolPriTool, MUIA_Disabled, !IsWriteable);
		set(CycleStartFromProject, MUIA_Disabled, !IsWriteable);
		set(CycleStartFromTool, MUIA_Disabled, !IsWriteable);
		set(ButtonPromptForInputProject, MUIA_Disabled,
			!IsWriteable || (STARTFROM_CLI != StartFrom && STARTFROM_Arexx != StartFrom));
		set(ButtonPromptForInputTool, MUIA_Disabled,
			!IsWriteable || (STARTFROM_CLI != StartFrom && STARTFROM_Arexx != StartFrom));

		//--------------------------------------------------------------------------//

		if (IsWBStartup)
			{
			// disable GroupWaitTimeTool if "Wait until finished" is turned off
			DoMethod(CheckMarkWaitUntilFinishedTool, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
				GroupWaitTimeTool, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

			DoMethod(CheckMarkWaitUntilFinishedTool2, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
				GroupWaitTimeTool2, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

			}
		else
			{
			// show WBStartup parameters only for objects located in WBStartup
			// for some reason, MUIA_ShowMe doesn't work in column groups,
			// so I choose to remove the objects instead.

			DoMethod(GroupIconTool, MUIM_Group_InitChange);
			DoMethod(GroupIconTool, OM_REMMEMBER, LabelStartPriTool);
			DoMethod(GroupIconTool, OM_REMMEMBER, SliderStartPriTool);
			DoMethod(GroupIconTool, OM_REMMEMBER, LabelWaitUntilFinishedTool);
			DoMethod(GroupIconTool, OM_REMMEMBER, GroupWaitUntilFinished);
			DoMethod(GroupIconTool, MUIM_Group_ExitChange);

			DoMethod(GroupIconProject, MUIM_Group_InitChange);
			DoMethod(GroupIconProject, OM_REMMEMBER, LabelStartPriTool2);
			DoMethod(GroupIconProject, OM_REMMEMBER, SliderStartPriTool2);
			DoMethod(GroupIconProject, OM_REMMEMBER, LabelWaitUntilFinishedTool2);
			DoMethod(GroupIconProject, OM_REMMEMBER, GroupWaitUntilFinished2);
			DoMethod(GroupIconProject, MUIM_Group_ExitChange);

			// since those objects are no longer childs of anything,
			// we need to dispose of them ourselves
			MUI_DisposeObject(LabelStartPriTool);
			MUI_DisposeObject(LabelStartPriTool2);

			MUI_DisposeObject(SliderStartPriTool);
			MUI_DisposeObject(SliderStartPriTool2);

			MUI_DisposeObject(LabelWaitUntilFinishedTool);
			MUI_DisposeObject(LabelWaitUntilFinishedTool2);

			MUI_DisposeObject(GroupWaitUntilFinished);
			MUI_DisposeObject(GroupWaitUntilFinished2);

			LabelStartPriTool = NULL;
			LabelStartPriTool2 = NULL;

			SliderStartPriTool = NULL;
			SliderStartPriTool2 = NULL;

			LabelWaitUntilFinishedTool = NULL;
			LabelWaitUntilFinishedTool2 = NULL;

			GroupWaitUntilFinished = NULL;
			GroupWaitUntilFinished2 = NULL;

			CheckMarkWaitUntilFinishedTool = NULL;
			CheckMarkWaitUntilFinishedTool2 = NULL;

			StringWaitTimeTool = NULL;
			StringWaitTimeTool2 = NULL;

			GroupWaitTimeTool = NULL;
			GroupWaitTimeTool2 = NULL;
			}

		//--------------------------------------------------------------------------//

		set(WIN_Main, MUIA_Window_Open, TRUE);
		get(WIN_Main, MUIA_Window_Open, &win_opened);

		if (win_opened)
			{
			ULONG sigs = 0;
			BOOL Run = TRUE;
			struct Window *MainWin = NULL;
			ULONG AppMsgSignal;
			ULONG DropZoneMsgSignal;
			char VarBuffer[3];

			d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

			//-- +jmc+ ------------------------------------------------------------------------------------
			// Only start size calculation at startup if the icon isn't WBDISK type, because getsize gadget
			// is used for drawers and disks(ButtonSize = ButtonDrawerSize or ButtonSize = ButtonDiskSize)
			// from DrawerSizeHookFunc() hook.

			if ((WBDRAWER == IconType) || (WBGARBAGE == IconType))
				set(ButtonDrawerSize, MUIA_Selected, AutoGetSize);

			//---------------------------------------------------------------------------------------------

			get(WIN_Main, MUIA_Window_Window, &MainWin);

			myAppWindow = AddAppWindowA(0, 0,
				MainWin, AppMsgPort, NULL);

			AppMsgSignal = 1 << AppMsgPort->mp_SigBit;
			DropZoneMsgSignal = 1 << DropZoneMsgPort->mp_SigBit;

			AddDropZoneForObject(MccIconObj, HandleDropOnIcon);
			AddDropZoneForObject(StringName, HandleDropOnName);
			AddDropZoneForObject(PopAslDefaultTool, HandleDropOnString);
			AddDropZoneForObject(StringCommentDrawer, HandleDropOnString);
			AddDropZoneForObject(StringCommentProject, HandleDropOnString);
			AddDropZoneForObject(StringCommentTool, HandleDropOnString);
			AddDropZoneForObject(StringCommentDevice, HandleDropOnString);
			AddDropZoneForObject(TextEditorToolTypesDrawer, HandleDropOnToolTypes);
			AddDropZoneForObject(TextEditorToolTypesProject, HandleDropOnToolTypes);
			AddDropZoneForObject(TextEditorToolTypesDevice, HandleDropOnToolTypes);
			AddDropZoneForObject(TextEditorToolTypesTool, HandleDropOnToolTypes);

			d1(KPrintF(__FILE__ "/%s/%ld: Application_Return_Ok(%ld) MUIV_Application_ReturnID_Quit(%ld)\n",
					__FUNC__, __LINE__, Application_Return_Ok, MUIV_Application_ReturnID_Quit));


			while (Run)
				{
				ULONG Action = DoMethod(APP_Main, MUIM_Application_NewInput, &sigs);

				d1(KPrintF(__FILE__ "/%s/%ld: Action=%ld\n", __FUNC__, __LINE__, Action));

				switch (Action)
					{
				case Application_Return_Ok:
					SaveSettings(iconObjFromDisk, backupIconObjFromDisk, IconName);
					Run = FALSE;
					break;

				case Application_Return_Path:
					if(strlen(PathName) > 0)
						set(Group_Virtual, MUIA_ShowMe, ShowIconPath);
						d1(KPrintF(__FILE__ "/%s/%ld: strlen(PathName)=%ld ShowIconPath=%ld\n", __FUNC__, __LINE__, strlen(PathName), ShowIconPath));
					break;

				case MUIV_Application_ReturnID_Quit:
					//---- Show icon path gadget --------------------------------------------------------------------------------------
					get(MenuIconPath, MUIA_Menuitem_Checked, &ShowIconPath);
					sprintf(VarBuffer, "%ld", (long) ShowIconPath);
					SetVar(INFO_ICONPATH_ENV, VarBuffer, -1, GVF_GLOBAL_ONLY);
					SetVar(INFO_ICONPATH_ENVARC, VarBuffer, -1, GVF_GLOBAL_ONLY);

					d1(KPrintF(__FILE__ "/%s/%ld: ShowIconPath=%ld VarBuffer=<%s>\nINFO_ICONPATH_ENV=<%s>\nINFO_ICONPATH_ENVARC=<%s>\n",
						__FUNC__, __LINE__, ShowIconPath, VarBuffer, INFO_ICONPATH_ENV, INFO_ICONPATH_ENVARC));

					//---- Auto GetSize calculation at startup -------------------------------------------------------------------------

					get(MenuGetSize, MUIA_Menuitem_Checked, &AutoGetSize);
					sprintf(VarBuffer, "%ld", (long) AutoGetSize);
					SetVar(INFO_AUTOGETSIZE_ENV, VarBuffer, -1, GVF_GLOBAL_ONLY);
					SetVar(INFO_AUTOGETSIZE_ENVARC, VarBuffer, -1, GVF_GLOBAL_ONLY);

					d1(KPrintF(__FILE__ "/%s/%ld: AutoGetSize=%ld VarBuffer=<%s>\nINFO_AUTOGETSIZE_ENV=<%s>\nINFO_AUTOGETSIZE_ENVARC=<%s>\n",
						__FUNC__, __LINE__, AutoGetSize, VarBuffer, INFO_AUTOGETSIZE_ENV, INFO_AUTOGETSIZE_ENVARC));

					Run = FALSE;
					break;
					}

				if (Run && sigs)
					{
					AdjustDropZones();

					sigs = Wait(sigs | AppMsgSignal | DropZoneMsgSignal | SIGBREAKF_CTRL_C);

					if (sigs & SIGBREAKF_CTRL_C) 
						{
						Run = FALSE;
						}
					if (sigs & AppMsgSignal)
						{
						struct AppMessage *AppMsg;

						while ((AppMsg = (struct AppMessage *) GetMsg(AppMsgPort)))
							{
							HandleAppMessage(AppMsg, &dii);
							ReplyMsg(&AppMsg->am_Message);
							}
						}
					if (sigs & DropZoneMsgSignal)
						{
						struct DropZoneMsg *dzm;

						d1(KPrintF(__FILE__ "/%s/%ld: sigs=%08lx\n", __FUNC__, __LINE__, sigs));

						while ((dzm = (struct DropZoneMsg *) GetMsg(DropZoneMsgPort)))
							{
							d1(KPrintF(__FILE__ "/%s/%ld: dzm=%08lx\n", __FUNC__, __LINE__, dzm));
							DropZoneMsgHandler(dzm);
							d1(KPrintF(__FILE__ "/%s/%ld: dzm=%08lx\n", __FUNC__, __LINE__, dzm));
							//free(dzm);
							ReplyMsg(&dzm->dzm_Msg);
							}
						}
					}
				}
			}
		else
			{
			printf(GetLocString(MSGID_CREATE_MAINWINDOW_FAILED));
			}

		if (myAppWindow)
			{
			RemoveAppWindow(myAppWindow);
			myAppWindow = NULL;
			}
		set(WIN_Main, MUIA_Window_Open, FALSE);
		}

	if (backupIconObjFromDisk)
		DisposeIconObject(backupIconObjFromDisk);
	if (iconObjFromDisk)
		DisposeIconObject(iconObjFromDisk);
	if (iconObjFromScalos)
		DisposeIconObject(iconObjFromScalos);
	if (oldDir)
		CurrentDir(oldDir);

	fail(APP_Main, NULL);

	return 0;
}


static VOID fail(APTR APP_Main, CONST_STRPTR str)
{
	Cleanup();

	if (str)
		{
		puts(str);
		exit(20);
		}

	exit(0);
}


static void init(void)
{
	STRPTR LibName;

	NewList(&DropZoneList);

	if (!OpenLibraries(&LibName))
		{
		char FailText[256];

		sprintf(FailText, GetLocString(MSGID_OPEN_LIBRARY_FAILED), LibName);
		fail(NULL, FailText);
		}

	if (LocaleBase)
		{
		InformationLocale = OpenLocale(NULL);
		InformationCatalog = OpenCatalogA(NULL, "Scalos/Information.catalog", NULL);
		}

	AppMsgPort = CreateMsgPort();
	if (NULL == AppMsgPort)
		{
		fail(NULL, "");
		}

	IconobjectClass = InitIconobjectClass();

	ReadScalosPrefs();
}


static void Cleanup(void)
{
	if (APP_Main)
		{
		MUI_DisposeObject(APP_Main);
		}
	if (IconobjectClass)
		{
		CleanupIconobjectClass(IconobjectClass);
		IconobjectClass = NULL;
		}
	if (InformationCatalog)
		{
		CloseCatalog(InformationCatalog);
		InformationCatalog = NULL;
		}

	if (AppMsgPort)
		{
		struct Message *msg;

		Forbid();
		while ((msg = GetMsg(AppMsgPort)))
			ReplyMsg(msg);

		DeleteMsgPort(AppMsgPort);
		Permit();

		AppMsgPort = NULL;
		}
	if (DropZoneMsgPort)
		{
		struct Message *msg;

		Forbid();
		while ((msg = GetMsg(DropZoneMsgPort)))
			free(msg);

		DeleteMsgPort(DropZoneMsgPort);
		Permit();

		AppMsgPort = NULL;
		}

	CloseLibraries();
}


static BOOL OpenLibraries(STRPTR *LibName)
{
	*LibName = "intuition.library";
	IntuitionBase = (struct IntuitionBase *) OpenLibrary(*LibName, 39);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			return FALSE;
		}
#endif

	*LibName = MUIMASTER_NAME;
	MUIMasterBase = OpenLibrary(*LibName, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
		if (NULL == IMUIMaster)
			return FALSE;
		}
#endif


	*LibName = "icon.library";
	IconBase = OpenLibrary(*LibName, 0);
	if (NULL == IconBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
		if (NULL == IIcon)
			return FALSE;
		}
#endif


	*LibName = "scalos.library";
	ScalosBase = (struct ScalosBase *) OpenLibrary(*LibName, 40);
	if (NULL == ScalosBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
		if (NULL == IScalos)
			return FALSE;
		}
#endif


	*LibName = "preferences.library";
	PreferencesBase = OpenLibrary(*LibName, 39);
	if (NULL == PreferencesBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IPreferences = (struct PreferencesIFace *)GetInterface((struct Library *)PreferencesBase, "main", 1, NULL);
		if (NULL == IPreferences)
			return FALSE;
		}
#endif


	*LibName = "iconobject.library";
	IconobjectBase = OpenLibrary(*LibName, 0);
	if (NULL == IconobjectBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IIconobject = (struct IconobjectIFace *)GetInterface((struct Library *)IconobjectBase, "main", 1, NULL);
		if (NULL == IIconobject)
			return FALSE;
		}
#endif


	*LibName = "workbench.library";
	WorkbenchBase = OpenLibrary(*LibName, 39);
	if (NULL == WorkbenchBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IWorkbench = (struct WorkbenchIFace *)GetInterface((struct Library *)WorkbenchBase, "main", 1, NULL);
		if (NULL == IWorkbench)
			return FALSE;
		}
#endif


	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
#endif

	*LibName = "timer.device";
	TimerPort = CreateMsgPort();
	if (NULL == TimerPort)
		return FALSE;
	TimerIO	= (T_TIMEREQUEST *)CreateIORequest(TimerPort, sizeof(T_TIMEREQUEST));
	if (NULL == TimerIO)
		return FALSE;
	if (0 != OpenDevice("timer.device", UNIT_VBLANK, &TimerIO->tr_node, 0))
		return FALSE;
	TimerBase = (T_TIMERBASE) TimerIO->tr_node.io_Device;
	if (NULL == TimerBase)
		return FALSE;
#ifdef __amigaos4__
	ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
	if (NULL == ITimer)
		return FALSE;
#endif

	if (!ScalosExamineBegin(&fib))
		return FALSE;

	return TRUE;
}


static void CloseLibraries(void)
{
#ifdef __amigaos4__
	if (ITimer)
		{
		DropInterface((struct Interface *)ITimer);
		ITimer = NULL;
		}
#endif
	if (TimerIO)
		{
		CloseDevice(&TimerIO->tr_node);
		DeleteIORequest(&TimerIO->tr_node);
		TimerIO = NULL;
		}
	if (TimerPort)
		{
		DeleteMsgPort(TimerPort);
		TimerPort = NULL;
		}

	ScalosExamineEnd(&fib);
	if (LocaleBase)
		{
		if (InformationLocale)
			{
			CloseLocale(InformationLocale);
			InformationLocale = NULL;
			}
		if (InformationCatalog)
			{
			CloseCatalog(InformationCatalog);
			InformationCatalog = NULL;
			}
#ifdef __amigaos4__
		if (ILocale)
			{
			DropInterface((struct Interface *)ILocale);
			ILocale = NULL;
			}
#endif
		CloseLibrary((struct Library *) LocaleBase);
		LocaleBase = NULL;
		}
#ifdef __amigaos4__
	if (IIconobject)
		{
		DropInterface((struct Interface *)IIconobject);
		IIconobject = NULL;
		}
#endif
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
#endif
	if (PreferencesBase)
		{
		CloseLibrary(PreferencesBase);
		PreferencesBase = NULL;
		}
#ifdef __amigaos4__
	if (IScalos)
		{
		DropInterface((struct Interface *)IScalos);
		IScalos = NULL;
		}
#endif
	if (ScalosBase)
		{
		CloseLibrary((struct Library *) ScalosBase);
		ScalosBase = NULL;
		}
#ifdef __amigaos4__
	if (IWorkbench)
		{
		DropInterface((struct Interface *)IWorkbench);
		IWorkbench = NULL;
		}
#endif
	if (WorkbenchBase)
		{
		CloseLibrary(WorkbenchBase);
		WorkbenchBase = NULL;
		}
#ifdef __amigaos4__
	if (IIcon)
		{
		DropInterface((struct Interface *)IIcon);
		IIcon = NULL;
		}
#endif
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
#endif
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
#endif
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
}

//----------------------------------------------------------------------------

static CONST_STRPTR GetLocString(ULONG MsgId)
{
	struct Information_LocaleInfo li;

	li.li_Catalog = InformationCatalog;  
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return GetInformationString(&li, MsgId);
}

static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = (STRPTR)GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg)
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

static SAVEDS(void) INTERRUPT OpenAboutHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	MUI_Request(APP_Main, WIN_Main, 0, NULL, 
		(STRPTR)GetLocString(MSGID_ABOUTREQOK),
		(STRPTR)GetLocString(MSGID_ABOUTREQFORMAT),
		VERSION_MAJOR, VERSION_MINOR, COMPILER_STRING, CURRENTYEAR);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT DrawerSizeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	ULONG FileCount, DrawersCount, BlocksCount;
	ULONG64 ByteCount;
	struct WBArg *arg = (struct WBArg *) hook->h_Data;
	BPTR OldDir;
	Object *ButtonSize;
	T_TIMEVAL lastUpdateTime;

	FileCount = DrawersCount = 0;
	ByteCount = MakeU64(0);
	BlocksCount = 1;
	Result = RETURN_OK;

	d1(KPrintF(__FILE__ "/%s/%ld: arg=%08lx  wa_Lock=%08lx  wa_Name=<%s>\n", __FUNC__, __LINE__, arg, arg->wa_Lock, arg->wa_Name));

	GetSysTime(&lastUpdateTime);

	OldDir = CurrentDir(arg->wa_Lock);

	if (IsDevice(arg))
		{
		ButtonSize = ButtonDiskSize;
		stccpy(InitButtonSize, TextDeviceUsed, sizeof(InitButtonSize)); // +jmc+

		d1(KPrintF(__FILE__ "/%s/%ld: ButtonDiskSize=<%s>\n", __FUNC__, __LINE__, InitButtonSize));

		ExamineDrawer(ButtonSize, ":", &FileCount, &DrawersCount, &BlocksCount, &ByteCount, &lastUpdateTime);
		}
	else
		{
		ButtonSize = ButtonDrawerSize;
		stccpy(InitButtonSize, GetLocString(MSGID_INITIAL_DRAWER_SIZE), sizeof(InitButtonSize)); // +jmc+

		d1(KPrintF(__FILE__ "/%s/%ld: ButtonDiskSize=<%s>\n", __FUNC__, __LINE__, InitButtonSize));

		ExamineDrawer(ButtonSize, arg->wa_Name, &FileCount, &DrawersCount, &BlocksCount, &ByteCount, &lastUpdateTime);
		}

	CurrentDir(OldDir);

	if (RESULT_HALT == Result) // +jmc+
		{
		set(ButtonSize, MUIA_Text_Contents, (ULONG) InitButtonSize); // +jmc+
		}
	else
		{
		UpdateDrawerSize(ButtonSize, &FileCount, &DrawersCount, &BlocksCount, &ByteCount);
		set(ButtonSize, MUIA_Selected, FALSE); // +jmc+
		}

	d1(KPrintF(__FILE__ "/%s/%ld: RESULT = [%ld] InitButtonSize <%s>\n", __FUNC__, __LINE__, Result, InitButtonSize));
}

//----------------------------------------------------------------------------

static void UpdateDrawerSize(Object *ButtonSize, ULONG *FileCount,
	ULONG *DrawersCount, ULONG *BlocksCount, ULONG64 *ByteCount)
{
	char ByteCountText[40];
	char SizeText[256];

	Convert64(InformationLocale, *ByteCount, ByteCountText, sizeof(ByteCountText));

	ScaFormatString(GetLocString(MSGID_DRAWER_SIZE),
		SizeText, sizeof(SizeText), 4,
		ByteCountText, *FileCount, *DrawersCount, *BlocksCount);
	set(ButtonSize, MUIA_Text_Contents, SizeText);
}

//----------------------------------------------------------------------------

static LONG ExamineDrawer(Object *ButtonSize, CONST_STRPTR Name, ULONG *FileCount,
	ULONG *DrawersCount, ULONG *BlocksCount, ULONG64 *ByteCount, T_TIMEVAL *lastUpdateTime)
{
	T_ExamineDirHandle edh;
	BPTR dLock = (BPTR)NULL;

	DoMethod(APP_Main, MUIM_Application_InputBuffered);

	// do not update more often than every 100ms
	if (GetElapsedTime(lastUpdateTime) > 100)
		{
		GetSysTime(lastUpdateTime);
		UpdateDrawerSize(ButtonSize, FileCount, DrawersCount, BlocksCount, ByteCount);
		}

	DoMethod(APP_Main, MUIM_Application_InputBuffered);

	do	{
		dLock = Lock(Name, ACCESS_READ);
		if ((BPTR)NULL == dLock)
			{
			Result = IoErr();
			d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
			break;
			}
		if (!ScalosExamineDirBegin(dLock, &edh))
			{
			Result = IoErr();
			d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
			break;
			}

		do	{
			T_ExamineData *exd;

			DoMethod(APP_Main, MUIM_Application_InputBuffered);

			if (!ScalosExamineDir(dLock, &edh, &exd))
				{
				Result = IoErr();
				d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
				break;
				}

			d1(KPrintF(__FILE__ "/%s/%ld: ScalosExamineDir() success - Name=<%s>  DirEntryType=%ld\n", __FUNC__, __LINE__, \
				ScalosExamineGetName(exd), ScalosExamineGetDirEntryType(exd)));

			(*BlocksCount) += ScalosExamineGetBlockCount(exd);

			if (ScalosExamineIsDrawer(exd))
				{
				BPTR OldDir = CurrentDir(dLock);

				(*DrawersCount)++;
				d1(KPrintF(__FILE__ "/%s/%ld: Name=<%s>\n", __FUNC__, __LINE__, ScalosExamineGetName(exd)));

				Result = ExamineDrawer(ButtonSize, ScalosExamineGetName(exd),
					FileCount, DrawersCount, BlocksCount, ByteCount, lastUpdateTime);

				CurrentDir(OldDir);
				}
			else
				{
				(*FileCount)++;
				*ByteCount = Add64(*ByteCount, ScalosExamineGetSize(exd) );
				}
			DoMethod(APP_Main, MUIM_Application_InputBuffered);

			// do not update more often than every 100ms
			if (GetElapsedTime(lastUpdateTime) > 100)
				{
				GetSysTime(lastUpdateTime);
				UpdateDrawerSize(ButtonSize, FileCount,
					DrawersCount, BlocksCount, ByteCount);
				}

			DoMethod(APP_Main, MUIM_Application_InputBuffered);

			} while (RETURN_OK == Result);
		} while (0);

	if (ERROR_NO_MORE_ENTRIES == Result)
		Result = RETURN_OK;

	ScalosExamineDirEnd(&edh);
	if (dLock)
		UnLock(dLock);

	d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld InitButtonSize <%s>\n", __FUNC__, __LINE__, Result, InitButtonSize));

	return Result;
}

//----------------------------------------------------------------------------

// Get elapsed time since time <tv> in milliseconds.
static LONG GetElapsedTime(T_TIMEVAL *tv)
{
	T_TIMEVAL Now;
	LONG Diff;

	GetSysTime(&Now);
	SubTime(&Now, tv);

	Diff = 1000 * Now.tv_secs + Now.tv_micro / 1000;

	d1(kprintf("%s/%s/%ld: Now s=%ld  ms=%ld   Start s=%ld  ms=%ld  Diff=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
			Now.tv_secs, Now.tv_micro, \
			tv->tv_secs, tv->tv_micro, Diff));

	return Diff;
}

// +jmc+ ----------------------------------------------------------------------//

static SAVEDS(LONG) INTERRUPT SelectFromButtonHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	LONG Sel = 0;

	get(o, MUIA_Selected, &Sel);
	if(!Sel)
		Result = RESULT_HALT;
	else
		Result = RETURN_OK;

	d1(KPrintF(__FILE__ "/%s/%ld: Result=[%ld] Sel=[%ld]\n", __FUNC__, __LINE__, Result, Sel));

	return Result;
}

//----------------------------------------------------------------------------//

static SAVEDS(void) INTERRUPT IconifyHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	ULONG Iconified = 0, attr = 0;
	struct DropZone *dz;
	struct Window *MainWin = NULL;

	get(APP_Main, MUIA_Application_Iconified, &Iconified);
	get(WIN_Main, MUIA_Window, &attr);

	d1(KPrintF("%s/%ld: Iconified=%ld  MainWin=%08lx  myAppWindow=%08lx\n", __FUNC__, __LINE__, Iconified, MainWin, myAppWindow));

	if (!Iconified && MainWin && NULL == myAppWindow)
		{
		myAppWindow = AddAppWindowA(0, 0, MainWin, AppMsgPort, NULL);
		}

	for (dz = (struct DropZone *) DropZoneList.lh_Head;
		dz != (struct DropZone *) &DropZoneList.lh_Tail;
		dz = (struct DropZone *) dz->dz_Node.ln_Succ)
		{
		if (Iconified)
			{
			d1(KPrintF("%s/%ld: dz_DropZone=%08lx\n", __FUNC__, __LINE__, dz->dz_DropZone));
			if (dz->dz_DropZone)
				RemoveAppWindowDropZone(myAppWindow, dz->dz_DropZone);
			}
		else
			{
			struct MUI_AreaData *mad;

			mad = muiAreaData(dz->dz_Object);

			if (mad)
				{
				d1(KPrintF("%s/%ld: dz=%08lx  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, dz, mad->mad_Box.Left, mad->mad_Box.Top, mad->mad_Box.Width, mad->mad_Box.Height));

				dz->dz_Box = mad->mad_Box;

				if (mad->mad_Flags & MADF_OBJECTVISIBLE)
					{
					dz->dz_DropZone = AddAppWindowDropZone(myAppWindow, 0,
						(ULONG) dz,
						WBDZA_Hook, (ULONG) &AppWindowDropZoneHook,
						WBDZA_Box, (ULONG) &dz->dz_Box,
						TAG_END);
					}
				}
			}
		
                }

	if (Iconified && myAppWindow)
		{
		RemoveAppWindow(myAppWindow);
		myAppWindow = NULL;
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT DefaultIconHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct DefIconInfo *dii = (struct DefIconInfo *) hook->h_Data;
	Object *NewIconObj;

	NewIconObj = SCA_GetDefIconObject(dii->dii_DirLock, dii->dii_IconName);

	if (NULL == NewIconObj)
		{
		IPTR IconType;

		GetAttr(IDTA_Type, *dii->dii_IconObjectFromDisk, &IconType);
		NewIconObj = GetDefIconObject(IconType, NULL);
		}
	if (NewIconObj)
		ReplaceIcon(NewIconObj, dii->dii_IconObjectFromDisk);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT StringIntegerIncrementHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	LONG Value = 0;

	get(o, MUIA_String_Integer, &Value);
	set(o, MUIA_String_Integer, Value + 1);
}


static SAVEDS(void) INTERRUPT StringIntegerDecrementHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	LONG Value = 0;

	get(o, MUIA_String_Integer, &Value);
	set(o, MUIA_String_Integer, Value - 1);
}

//----------------------------------------------------------------------------

static void HandleAppMessage(struct AppMessage *AppMsg, struct DefIconInfo *dii)
{
	struct DropZone *dz = (struct DropZone *) AppMsg->am_UserData;
	ULONG Disabled = 0;

	d1(KPrintF(__FILE__ "/%s/%ld: AppMsg=%08lx  am_NumArgs=%ld  am_Type=%ld\n", __FUNC__, __LINE__, AppMsg, AppMsg->am_NumArgs, AppMsg->am_Type));

	switch (AppMsg->am_Type)
		{
	case AMTYPE_APPWINDOW:
		d1(KPrintF("%s/%ld: AMTYPE_APPWINDOW\n", __FUNC__, __LINE__));
		if (IsListEmpty(&DropZoneList))
			{
			if (AppMsg->am_NumArgs >= 1)
				HandleDropOnIcon(&AppMsg->am_ArgList[0], MccIconObj, dii);
			}
		break;

	case AMTYPE_APPWINDOWZONE:
		d1(KPrintF("%s/%ld: AMTYPE_APPWINDOWZONE\n", __FUNC__, __LINE__));
		get(dz->dz_Object, MUIA_Disabled, &Disabled);
		DoMethod(dz->dz_Object, MUIM_DragFinish, NULL);

		if (!Disabled && AppMsg->am_NumArgs >= 1 && dz->dz_Handler)
			{
			dz->dz_Handler(&AppMsg->am_ArgList[0], dz->dz_Object, dii);
			}
		break;
		}
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT AppWindowDropZoneHookFunc(struct Hook *hook,
	APTR Reserved, struct AppWindowDropZoneMsg *adzm)
{
	struct DropZoneMsg *dzm;

	d1(KPrintF("%s/%ld: adzm_Action=%ld\n", __FUNC__, __LINE__, adzm->adzm_Action));
	d1(KPrintF("%s/%ld: Task=<%s>\n", __FUNC__, __LINE__, FindTask(NULL)->tc_Node.ln_Name));

	do	{
		struct MsgPort *ReplyPort;

		ReplyPort = CreateMsgPort();
		if (NULL == ReplyPort)
			break;

		dzm = malloc(sizeof(struct DropZoneMsg));
		if (NULL == dzm)
			break;

		dzm->dzm_adzm = *adzm;
		dzm->dzm_Msg.mn_Length = sizeof(struct DropZoneMsg);
		dzm->dzm_Msg.mn_ReplyPort = ReplyPort;
		d1(KPrintF("%s/%ld: dzm=%08lx \n", __FUNC__, __LINE__, dzm));
		PutMsg(DropZoneMsgPort, &dzm->dzm_Msg);

		d1(KPrintF("%s/%ld:\n", __FUNC__, __LINE__));
		WaitPort(ReplyPort);

		while ((dzm = (struct DropZoneMsg *) GetMsg(ReplyPort)))
			{
			d1(KPrintF(__FILE__ "/%s/%ld: dzm=%08lx\n", __FUNC__, __LINE__, dzm));
			free(dzm);
			}
		d1(KPrintF("%s/%ld:\n", __FUNC__, __LINE__));
		DeleteMsgPort(ReplyPort);
		} while (0);

	return 0;
}

//----------------------------------------------------------------------------

static void DropZoneMsgHandler(struct DropZoneMsg *dzm)
{
	struct DropZone *dz = (struct DropZone *) dzm->dzm_adzm.adzm_UserData;
	ULONG Disabled = 0;

	d1(KPrintF("%s/%ld: START dz=%08lx  o=%08lx  adzm_Action=%ld\n", __FUNC__, __LINE__, dz, dz->dz_Object, dzm->dzm_adzm.adzm_Action));
	d1(KPrintF("%s/%ld: Task=<%s>\n", __FUNC__, __LINE__, FindTask(NULL)->tc_Node.ln_Name));

	switch (dzm->dzm_adzm.adzm_Action)
		{
	case ADZMACTION_Enter:
		d1(KPrintF("%s/%ld: ADZMACTION_Enter\n", __FUNC__, __LINE__));
		get(dz->dz_Object, MUIA_Disabled, &Disabled);
		if (!Disabled)
			{
			DoMethod(dz->dz_Object, MUIM_DragBegin, NULL);
#if defined(__MORPHOS__)
			if (DOSBase->dl_lib.lib_Version >= 51)
				{
				Delay(1);
				WaitTOF();
				}
#endif //defined(__MORPHOS__)
			}
		break;
	case ADZMACTION_Leave:
		d1(KPrintF("%s/%ld: ADZMACTION_Leave\n", __FUNC__, __LINE__));
		DoMethod(dz->dz_Object, MUIM_DragFinish, NULL, FALSE);
#if defined(__MORPHOS__)
		if (DOSBase->dl_lib.lib_Version >= 51)
			{
			Delay(1);
			WaitTOF();
			}
#endif //defined(__MORPHOS__)
		break;
		}
	d1(KPrintF("%s/%ld:  END\n", __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static void ReplaceIcon(Object *NewIconObj, Object **OldIconObj)
{
	//STRPTR str;
	//struct IBox *WinRect;
	struct ExtGadget *ggOld = (struct ExtGadget *) *OldIconObj;
	struct ExtGadget *ggNew = (struct ExtGadget *) NewIconObj;
	IPTR  ul;
	IPTR  IconType;

	d1(KPrintF("%s/%ld: START NewIconObj=%08lx  OldIconObj=%08lx\n", __FUNC__, __LINE__, NewIconObj, OldIconObj));

	d1(KPrintF("%s/%ld: *OldIconObj=%08lx\n", __FUNC__, __LINE__, *OldIconObj));
	GetAttr(IDTA_Type, *OldIconObj, &IconType);
	d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));
	set(NewIconObj, IDTA_Type, IconType);

	d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

	ggNew->BoundsLeftEdge = ggOld->LeftEdge + (ggNew->LeftEdge - ggNew->BoundsLeftEdge);
	ggNew->BoundsTopEdge = ggOld->TopEdge + (ggNew->TopEdge - ggNew->BoundsTopEdge);
	ggNew->LeftEdge = ggOld->LeftEdge;
	ggNew->TopEdge = ggNew->TopEdge;

	d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

	GetAttr(IDTA_DefaultTool, *OldIconObj, &ul);
	set(NewIconObj, IDTA_DefaultTool, ul);

	d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

	GetAttr(IDTA_WindowRect, *OldIconObj, &ul);
	set(NewIconObj, IDTA_WindowRect, ul);

	GetAttr(IDTA_WinCurrentX, *OldIconObj, &ul);
	set(NewIconObj, IDTA_WinCurrentX, ul);
	GetAttr(IDTA_WinCurrentY, *OldIconObj, &ul);
	set(NewIconObj, IDTA_WinCurrentY, ul);

	GetAttr(IDTA_Flags, *OldIconObj, &ul);
	set(NewIconObj, IDTA_Flags, ul);

	d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

	GetAttr(IDTA_ViewModes, *OldIconObj, &ul);
	set(NewIconObj, IDTA_ViewModes, ul);

	d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

	set(MccIconObj, MUIA_Iconobj_Object, NewIconObj);

	d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

	DisposeIconObject(*OldIconObj);
	*OldIconObj = NewIconObj;

	d1(KPrintF("%s/%ld:  END\n", __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static void SaveSettings(Object *IconObj, Object *originalIconObj, CONST_STRPTR IconName)
{
	d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, IconObj, IconName));

	if (IconObj)
		{
		IPTR  IconType;
		IPTR ToolPri;
		char ToolPriString[15];
		struct ScaWindowStruct *ws;
		CONST_STRPTR *ToolTypesArray;
		STRPTR *OldToolTypesArray;
		enum StartFromType StartFrom = STARTFROM_Workbench;
		APTR UndoStep = NULL;
		STRPTR Comment = "";
		STRPTR DefaultTool = "";
		IPTR  Selected;
		ULONG Protection = 0;
		IPTR  StackSize;
		CONST_STRPTR NewName = ScalosExamineGetName(fib);
		char GetName[128];
		char VolumeName[128];
		ULONG Pos2;
		BPTR dirLock = CurrentDir((BPTR) NULL);

		CurrentDir(dirLock);
		ws = FindScalosWindow(dirLock);

		if (ws)
			{
			UndoStep = (APTR) DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_BeginUndoStep);
			}

		GetAttr(IDTA_Type, IconObj, &IconType);
		GetAttr(MUIA_String_Contents, StringName, (APTR)&NewName);

		GetAttr(IDTA_ToolTypes, IconObj, (APTR) &ToolTypesArray);
		OldToolTypesArray = CloneToolTypeArray(ToolTypesArray, 0);

		Protection = ScalosExamineGetProtection(fib) & ~(FIBF_READ | FIBF_WRITE | FIBF_ARCHIVE | FIBF_DELETE | FIBF_EXECUTE | FIBF_SCRIPT);

		switch (IconType)
			{
		case WBDISK:
			GetAttr(MUIA_String_Contents, StringCommentDevice, (APTR)&Comment);
			SetChangedToolTypes(IconObj, TextEditorToolTypesDevice);
			break;
		case WBDRAWER:
			SetChangedToolTypes(IconObj, TextEditorToolTypesDrawer);

			GetAttr(MUIA_String_Contents, StringCommentDrawer, (APTR)&Comment);

			GetAttr(MUIA_Selected, CheckMarkProtectionReadDrawer, &Selected);
			if (!Selected)
				Protection |= FIBF_READ;
			GetAttr(MUIA_Selected, CheckMarkProtectionWriteDrawer, &Selected);
			if (!Selected)
				Protection |= FIBF_WRITE;
			GetAttr(MUIA_Selected, CheckMarkProtectionArchiveDrawer, &Selected);
			if (Selected)
				Protection |= FIBF_ARCHIVE;
			GetAttr(MUIA_Selected, CheckMarkProtectionDeleteDrawer, &Selected);
			if (!Selected)
				Protection |= FIBF_DELETE;
			GetAttr(MUIA_Selected, CheckMarkProtectionExecuteDrawer, &Selected);
			if (!Selected)
				Protection |= FIBF_EXECUTE;
			GetAttr(MUIA_Selected, CheckMarkProtectionScriptDrawer, &Selected);
			if (Selected)
				Protection |= FIBF_SCRIPT;
			break;

		case WBGARBAGE:
			SetChangedToolTypes(IconObj, TextEditorToolTypesDrawer);

			GetAttr(MUIA_String_Contents, StringCommentDrawer, (APTR)&Comment);

			GetAttr(MUIA_Selected, CheckMarkProtectionReadDrawer, &Selected);
			if (!Selected)
				Protection |= FIBF_READ;
			GetAttr(MUIA_Selected, CheckMarkProtectionWriteDrawer, &Selected);
			if (!Selected)
				Protection |= FIBF_WRITE;
			GetAttr(MUIA_Selected, CheckMarkProtectionArchiveDrawer, &Selected);
			if (Selected)
				Protection |= FIBF_ARCHIVE;
			GetAttr(MUIA_Selected, CheckMarkProtectionDeleteDrawer, &Selected);
			if (!Selected)
				Protection |= FIBF_DELETE;
			GetAttr(MUIA_Selected, CheckMarkProtectionExecuteDrawer, &Selected);
			if (!Selected)
				Protection |= FIBF_EXECUTE;
			GetAttr(MUIA_Selected, CheckMarkProtectionScriptDrawer, &Selected);
			if (Selected)
				Protection |= FIBF_SCRIPT;
			break;

		case WBTOOL:
			SetChangedToolTypes(IconObj, TextEditorToolTypesTool);

			GetAttr(MUIA_String_Contents, StringCommentTool, (APTR)&Comment);
			GetAttr(MUIA_Cycle_Active, CycleStartFromTool, (APTR)&StartFrom);
			GetAttr(MUIA_String_Integer, StringStackSizeTool, &StackSize);

			SetAttrs(IconObj,
				IDTA_Stacksize, StackSize,
				TAG_END);

			switch (StartFrom)
				{
			case STARTFROM_CLI:
				SetToolType(IconObj, "CLI", "");
				RemoveToolType(IconObj, "REXX");
				break;
			case STARTFROM_Arexx:
				SetToolType(IconObj, "REXX", "");
				RemoveToolType(IconObj, "CLI");
				break;
			case STARTFROM_Workbench:
			default:
				RemoveToolType(IconObj, "CLI");
				RemoveToolType(IconObj, "REXX");
				break;
				}


			if (SliderStartPriTool)
				{
				IPTR StartPri;

				GetAttr(MUIA_Numeric_Value, SliderStartPriTool, &StartPri);
				if (0 != StartPri)
					{
					sprintf(ToolPriString, "%ld", StartPri);
					SetToolType(IconObj, "STARTPRI", ToolPriString);
					}
				else
					RemoveToolType(IconObj, "STARTPRI");
				}

			if (StringWaitTimeTool)
				{
				IPTR WaitTime;

				GetAttr(MUIA_String_Integer, StringWaitTimeTool, &WaitTime);
				if (0 != WaitTime)
					{
					sprintf(ToolPriString, "%ld", (long) WaitTime);
					SetToolType(IconObj, "WAIT", ToolPriString);
					}
				else
					RemoveToolType(IconObj, "WAIT");
				}

			if (CheckMarkWaitUntilFinishedTool)
				{
				IPTR WaitUntilFinished;

				GetAttr(MUIA_Selected, CheckMarkWaitUntilFinishedTool, &WaitUntilFinished);
				if (WaitUntilFinished)
					RemoveToolType(IconObj, "DONOTWAIT");
				else
					SetToolType(IconObj, "DONOTWAIT", "");
				}

			if (STARTFROM_CLI == StartFrom || STARTFROM_Arexx == StartFrom)
				{
				IPTR PromptForInput;

				GetAttr(MUIA_Selected, ButtonPromptForInputTool, &PromptForInput);

				if (PromptForInput)
					RemoveToolType(IconObj, "DONOTPROMPT");
				else
					SetToolType(IconObj, "DONOTPROMPT", "");
				}

			GetAttr(MUIA_Numeric_Value, SliderToolPriTool, &ToolPri);
			if (ToolPri)
				{
				sprintf(ToolPriString, "%ld", (long) ToolPri);
				SetToolType(IconObj, "TOOLPRI", ToolPriString);
				}
			else
				RemoveToolType(IconObj, "TOOLPRI");

			GetAttr(MUIA_Selected, CheckMarkProtectionHideTool, &Selected);
			if (Selected)
				Protection |= FIBF_HIDDEN;

			GetAttr(MUIA_Selected, CheckMarkProtectionReadTool, &Selected);
			if (!Selected)
				Protection |= FIBF_READ;
			GetAttr(MUIA_Selected, CheckMarkProtectionWriteTool, &Selected);
			if (!Selected)
				Protection |= FIBF_WRITE;
			GetAttr(MUIA_Selected, CheckMarkProtectionArchiveTool, &Selected);
			if (Selected)
				Protection |= FIBF_ARCHIVE;
			GetAttr(MUIA_Selected, CheckMarkProtectionDeleteTool, &Selected);
			if (!Selected)
				Protection |= FIBF_DELETE;


			GetAttr(MUIA_Selected, CheckMarkProtectionPureTool, &Selected);
			if (Selected)
				Protection |= FIBF_PURE;

			GetAttr(MUIA_Selected, CheckMarkProtectionExecuteTool, &Selected);
			if (!Selected)
				Protection |= FIBF_EXECUTE;
			GetAttr(MUIA_Selected, CheckMarkProtectionScriptTool, &Selected);
			if (Selected)
				Protection |= FIBF_SCRIPT;
			break;

		case WBPROJECT:
			SetChangedToolTypes(IconObj, TextEditorToolTypesProject);

			GetAttr(MUIA_String_Contents, StringCommentProject, (APTR)&Comment);
			GetAttr(MUIA_Cycle_Active, CycleStartFromProject, (APTR)&StartFrom);
			GetAttr(MUIA_String_Integer, StringStackSizeProject, &StackSize);
			GetAttr(MUIA_String_Contents, PopAslDefaultTool, (APTR)&DefaultTool);

			SetAttrs(IconObj,
				IDTA_Stacksize, StackSize,
				IDTA_DefaultTool, DefaultTool,
				TAG_END);

			switch (StartFrom)
				{
			case STARTFROM_CLI:
				SetToolType(IconObj, "CLI", "");
				RemoveToolType(IconObj, "REXX");
				break;
			case STARTFROM_Arexx:
				SetToolType(IconObj, "REXX", "");
				RemoveToolType(IconObj, "CLI");
				break;
			case STARTFROM_Workbench:
			default:
				RemoveToolType(IconObj, "CLI");
				RemoveToolType(IconObj, "REXX");
				break;
				}
//-----------------------------------------------------------------------------------------------

			if (SliderStartPriTool2)
				{
				IPTR StartPri;

				GetAttr(MUIA_Numeric_Value, SliderStartPriTool2, &StartPri);
				if (0 != StartPri)
					{
					sprintf(ToolPriString, "%ld", StartPri);
					SetToolType(IconObj, "STARTPRI", ToolPriString);
					}
				else
					RemoveToolType(IconObj, "STARTPRI");
				}

			if (StringWaitTimeTool2)
				{
				IPTR WaitTime;

				GetAttr(MUIA_String_Integer, StringWaitTimeTool2, &WaitTime);
				if (0 != WaitTime)
					{
					sprintf(ToolPriString, "%ld", (long) WaitTime);
					SetToolType(IconObj, "WAIT", ToolPriString);
					}
				else
					RemoveToolType(IconObj, "WAIT");
				}

			if (CheckMarkWaitUntilFinishedTool2)
				{
				IPTR WaitUntilFinished;

				GetAttr(MUIA_Selected, CheckMarkWaitUntilFinishedTool2, &WaitUntilFinished);
				if (WaitUntilFinished)
					RemoveToolType(IconObj, "DONOTWAIT");
				else
					SetToolType(IconObj, "DONOTWAIT", "");
				}
//--------------------------------------------------------------------------------------------

			if (STARTFROM_CLI == StartFrom || STARTFROM_Arexx == StartFrom)
				{
				IPTR PromptForInput;

				GetAttr(MUIA_Selected, ButtonPromptForInputProject, &PromptForInput);

				if (PromptForInput)
					RemoveToolType(IconObj, "DONOTPROMPT");
				else
					SetToolType(IconObj, "DONOTPROMPT", "");
				}

			GetAttr(MUIA_Numeric_Value, SliderToolPriProject, (IPTR *) &ToolPri);
			if (ToolPri)
				{
				sprintf(ToolPriString, "%ld", (long) ToolPri);
				SetToolType(IconObj, "TOOLPRI", ToolPriString);
				}
			else
				RemoveToolType(IconObj, "TOOLPRI");

			GetAttr(MUIA_Selected, CheckMarkProtectionHideProject, &Selected);
			if (Selected)
				Protection |= FIBF_HIDDEN;

			GetAttr(MUIA_Selected, CheckMarkProtectionReadProject, &Selected);
			if (!Selected)
				Protection |= FIBF_READ;
			GetAttr(MUIA_Selected, CheckMarkProtectionWriteProject, &Selected);
			if (!Selected)
				Protection |= FIBF_WRITE;
			GetAttr(MUIA_Selected, CheckMarkProtectionArchiveProject, &Selected);
			if (Selected)
				Protection |= FIBF_ARCHIVE;
			GetAttr(MUIA_Selected, CheckMarkProtectionDeleteProject, &Selected);
			if (!Selected)
				Protection |= FIBF_DELETE;
			GetAttr(MUIA_Selected, CheckMarkProtectionExecuteProject, &Selected);
			if (!Selected)
				Protection |= FIBF_EXECUTE;

			GetAttr(MUIA_Selected, CheckMarkProtectionPureProject, &Selected);
			if (Selected)
				Protection |= FIBF_PURE;

			GetAttr(MUIA_Selected, CheckMarkProtectionScriptProject, &Selected);
			if (Selected)
				Protection |= FIBF_SCRIPT;
			break;

		default:
			break;
			}

		d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, IconObj, IconName));

		// +jmc+ Check WBDISK, PutIconObjectTags() with strip ".info" from name if needed.
		// If ".info" exists, icon will be overwritten instead to create
		// a icon from the complete name with ".info"(i.e. "name.info.info").

		if (WBDISK == IconType && FoundDeviceFromArg)
			{
			IconName = "disk";
			d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, IconObj, IconName));
			}

		stccpy(GetName, IconName, sizeof(GetName));
		Pos2 = IsIconName(GetName);

		if (Pos2 && ~0 != Pos2)
			{
			// strip ".info" from name
			*((char *) Pos2) = '\0';
			}

		d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, IconObj, IconName));

		if (ws)
			{
			DoMethod(ws->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_AddUndoEvent,
				UNDO_ChangeIconObject,
				UNDOTAG_UndoMultiStep, UndoStep,
				UNDOTAG_IconDirLock, dirLock,
				UNDOTAG_IconName, (ULONG) ScalosExamineGetName(fib),
				UNDOTAG_OldIconObject, (ULONG) originalIconObj,
				UNDOTAG_NewIconObject, (ULONG) IconObj,
				TAG_END
				);
			}

		PutIconObjectTags(IconObj, GetName,
			ICONA_NoNewImage, TRUE,
			TAG_END);

		d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, IconObj, IconName));


		if (WBDISK != IconType)
			{
			if (FibValid && Protection != ScalosExamineGetProtection(fib))
				{
				// Protection has changed
				if (ws)
					{
					DoMethod(ws->ws_WindowTask->mt_MainObject,
						SCCM_IconWin_AddUndoEvent,
						UNDO_SetProtection,
						UNDOTAG_UndoMultiStep, UndoStep,
						UNDOTAG_IconDirLock, dirLock,
						UNDOTAG_IconName, (ULONG) ScalosExamineGetName(fib),
						UNDOTAG_OldProtection, ScalosExamineGetProtection(fib),
						UNDOTAG_NewProtection, Protection,
						TAG_END
						);
					}
				if (!SetProtection(IconName, Protection))
					ReportError(MSGID_ERROR_SETPROTECTION);
				}
			}

		if (FibValid && 0 != strcmp(NewName, ScalosExamineGetName(fib)))
			{
			// Name has changed
			if (WBDISK == IconType)
				{
				if (ws)
					{
					DoMethod(ws->ws_WindowTask->mt_MainObject,
						SCCM_IconWin_AddUndoEvent,
						UNDO_Relabel,
						UNDOTAG_UndoMultiStep, UndoStep,
						UNDOTAG_CopySrcDirLock, dirLock,
						UNDOTAG_CopySrcName, (ULONG) ScalosExamineGetName(fib),
						UNDOTAG_CopyDestName, NewName,
						TAG_END
						);
					}
				RenameDevice(ScalosExamineGetName(fib), NewName);
				}
			else
				{
				if (ws)
					{
					DoMethod(ws->ws_WindowTask->mt_MainObject,
						SCCM_IconWin_AddUndoEvent,
						UNDO_Rename,
						UNDOTAG_UndoMultiStep, UndoStep,
						UNDOTAG_CopySrcDirLock, dirLock,
						UNDOTAG_CopySrcName, (ULONG) ScalosExamineGetName(fib),
						UNDOTAG_CopyDestName, (ULONG) NewName,
						TAG_END
						);
					}
				RenameObject(ScalosExamineGetName(fib), NewName);
				}

			if (ws)
				SCA_UnLockWindowList();
			}

		if (FibValid && 0 != strcmp(Comment, ScalosExamineGetComment(fib)))
			{
			// Comment has changed
			if (WBDISK == IconType && FoundDeviceFromArg)	// +jmc+ - Add ":" to volume name for SetComment().
				{
				stccpy(VolumeName, NewName, sizeof(VolumeName));
				strcat(VolumeName,":");
				IconName = VolumeName;
				}
			if (ws)
				{
				DoMethod(ws->ws_WindowTask->mt_MainObject,
					SCCM_IconWin_AddUndoEvent,
					UNDO_SetComment,
					UNDOTAG_UndoMultiStep, UndoStep,
					UNDOTAG_IconDirLock, dirLock,
					UNDOTAG_IconName, (ULONG) ScalosExamineGetName(fib),
					UNDOTAG_OldComment, (ULONG) ScalosExamineGetComment(fib),
					UNDOTAG_NewComment, (ULONG) Comment,
					TAG_END
					);
				}
			if (!SetComment(IconName, Comment))
				ReportError(MSGID_ERROR_SETCOMMENT);
			}

		if (ws)
			{
			DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_EndUndoStep, UndoStep);
			SCA_UnLockWindowList();
			}
		if (OldToolTypesArray)
			{
			free(OldToolTypesArray);
			}
		}

		d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, IconObj, IconName));
}

//----------------------------------------------------------------------------

static void GetIconObject(struct DefIconInfo *dii, CONST_STRPTR IconName, BPTR DirLock,
	STRPTR FileTypeName, size_t FileTypeNameSize)
{
	char IconPath[MAX_PATH_SIZE];

	do	{
		strcpy(FileTypeName, "");

		if (NULL == IconName || strlen(IconName) < 1)
			{
			if (!NameFromLock(DirLock, IconPath, sizeof(IconPath)))
				break;
			IconName = IconPath;
			}

		*dii->dii_IconObjectFromScalos = GetIconObjectFromScalos(DirLock, IconName, FileTypeName, FileTypeNameSize);

		*dii->dii_IconObjectFromDisk = NewIconObject(IconName, NULL);
		d1(KPrintF(__FILE__ "/%s/%ld: IconName=<%s> dii_IconObjectFromDisk=%08lx\n", __FUNC__, __LINE__, IconName, *dii->dii_IconObjectFromDisk));
		if (*dii->dii_IconObjectFromDisk)
			{
			if (NULL == *dii->dii_BackupIconObjectFromDisk)
				*dii->dii_BackupIconObjectFromDisk = NewIconObject(IconName, NULL);
			break;
			}

		*dii->dii_IconObjectFromDisk = SCA_GetDefIconObjectTags(DirLock, IconName,
			TAG_END);
		if (*dii->dii_IconObjectFromDisk)
			{
			if (NULL == *dii->dii_BackupIconObjectFromDisk)
				{
				*dii->dii_BackupIconObjectFromDisk = SCA_GetDefIconObjectTags(DirLock, IconName,
					TAG_END);
				}
			}

		d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk));
		} while (0);
}

//----------------------------------------------------------------------------

static void GetDeviceIconObject(struct DefIconInfo *dii, CONST_STRPTR IconName,
	CONST_STRPTR VolumeName, CONST_STRPTR DeviceName,
	STRPTR FileTypeName, size_t FileTypeNameSize)
{
	BPTR OldDir = (BPTR)NULL;
	BPTR dLock = (BPTR)NULL;
	BPTR DiskLock = (BPTR) NULL;

	do	{
		char DefIconName[128];

		strcpy(DefIconName, VolumeName);
		strcat(DefIconName, ":");

		DiskLock = Lock(DefIconName, ACCESS_READ);

		if (prefDefIconsFirst)
			{
			dLock = Lock(prefDefIconPath, ACCESS_READ);
			if ((BPTR)NULL == dLock)
				break;

//			  *dii->dii_IconObjectFromScalos = GetDeviceIconObjectFromScalos(dLock, FileTypeName, FileTypeNameSize);

			OldDir = CurrentDir(dLock);

			strcpy(DefIconName, "def_");
			strcat(DefIconName, VolumeName);

			*dii->dii_IconObjectFromDisk = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  <%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, DefIconName));
			if (*dii->dii_IconObjectFromDisk)
				break;

			BuildDefVolumeNameNoSpace(DefIconName, VolumeName, sizeof(DefIconName));

			*dii->dii_IconObjectFromDisk = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  DefIconName=<%s> VolumeName=<%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, DefIconName, VolumeName));
			if (*dii->dii_IconObjectFromDisk)
				break;

			strcpy(DefIconName, "def_");
			strcat(DefIconName, DeviceName);

			*dii->dii_IconObjectFromDisk = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  <%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, DefIconName));
			if (*dii->dii_IconObjectFromDisk)
				break;

			CurrentDir(OldDir);
			OldDir = (BPTR)NULL;

			*dii->dii_IconObjectFromDisk = NewIconObject(IconName, NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  <%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, IconName));
			if (*dii->dii_IconObjectFromDisk)
				break;

			OldDir = CurrentDir(DiskLock);

			*dii->dii_IconObjectFromDisk = NewIconObject("disk", NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, IconName));
			if (*dii->dii_IconObjectFromDisk)
				break;
			}
		else
			{
			dLock = Lock(prefDefIconPath, ACCESS_READ);
			if ((BPTR)NULL == dLock)
				break;

//			  *dii->dii_IconObjectFromScalos = GetDeviceIconObjectFromScalos(dLock, FileTypeName, FileTypeNameSize);

			if (IconName && strlen(IconName))
				{
				*dii->dii_IconObjectFromDisk = NewIconObject(IconName, NULL);
				d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, IconName));
				if (*dii->dii_IconObjectFromDisk)
					break;
				}

			OldDir = CurrentDir(DiskLock);

			*dii->dii_IconObjectFromDisk = NewIconObject("disk", NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, IconName));
			if (*dii->dii_IconObjectFromDisk)
				break;

			(void) CurrentDir(dLock);

			strcpy(DefIconName, "def_");
			strcat(DefIconName, VolumeName);

			*dii->dii_IconObjectFromDisk = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: *dii->dii_IconObjectFromDisk=%08lx  <%s> VolumeName=<%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, DefIconName, VolumeName));
			if (*dii->dii_IconObjectFromDisk)
				break;

			BuildDefVolumeNameNoSpace(DefIconName, VolumeName, sizeof(DefIconName));

			*dii->dii_IconObjectFromDisk = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  DefIconName=<%s> VolumeName=<%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, DefIconName, VolumeName));
			if (*dii->dii_IconObjectFromDisk)
				break;

			strcpy(DefIconName, "def_");
			strcat(DefIconName, DeviceName);

			*dii->dii_IconObjectFromDisk = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  <%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, DefIconName));
			if (*dii->dii_IconObjectFromDisk)
				break;
			}

		if (DiskLock)
			{
			*dii->dii_IconObjectFromDisk = SCA_GetDefIconObjectTags(DiskLock, "",
				TAG_END);
			d1(KPrintF(__FILE__ "/%s/%ld: dii_IconObjectFromDisk=%08lx  <%s>\n", __FUNC__, __LINE__, *dii->dii_IconObjectFromDisk, DefIconName));
			}
		} while (0);

	if (OldDir)
		CurrentDir(OldDir);
	if (dLock)
		UnLock(dLock);
	if (DiskLock)
		UnLock(DiskLock);
}

//----------------------------------------------------------------------------

static BOOL isDiskWritable(BPTR dLock)
{
	struct InfoData *info = AllocInfoData();
	BOOL Result = TRUE;

	if (info)
		{
		Info(dLock, info);

		if (!CheckInfoData(info))
			Result = FALSE;

		FreeInfoData(info);
		}

	return Result;
}

//----------------------------------------------------------------------------

// return TRUE if Disk is writable
static ULONG CheckInfoData(const struct InfoData *info)
{
	ULONG Result = TRUE;
	ULONG64 UsedPercent64;
	ULONG UsedPercent;
	char ByteCountText[16];

	d1(kprintf(__FILE__ "/%s/%ld: DiskType=%ld  DiskState=%ld\n", __FUNC__, __LINE__, info->id_DiskType, info->id_DiskState));

	UsedPercent64 = Div64(Mul64(MakeU64(100), MakeU64(info->id_NumBlocksUsed), NULL), MakeU64(info->id_NumBlocks), NULL);
	UsedPercent = ULONG64_LOW(UsedPercent64);

	ByteCount(ByteCountText, sizeof(ByteCountText), info->id_NumBlocksUsed, info->id_BytesPerBlock);
	ScaFormatString(GetLocString(MSGID_DEVICE_BLOCKS_PERCENT),
		TextDeviceUsed, sizeof(TextDeviceUsed),
		 3, ByteCountText, info->id_NumBlocksUsed, UsedPercent);

	ByteCount(ByteCountText, sizeof(ByteCountText),
		info->id_NumBlocks - info->id_NumBlocksUsed, info->id_BytesPerBlock);
	ScaFormatString(GetLocString(MSGID_DEVICE_BLOCKS_PERCENT),
		TextDeviceFree, sizeof(TextDeviceFree),
		 3, ByteCountText,
		info->id_NumBlocks - info->id_NumBlocksUsed,
		100 - UsedPercent);

	ByteCount(ByteCountText, sizeof(ByteCountText), info->id_NumBlocks, info->id_BytesPerBlock);
	ScaFormatString(GetLocString(MSGID_DEVICE_BLOCKS),
		TextDeviceSize, sizeof(TextDeviceSize),
		2, ByteCountText, info->id_NumBlocks);


	switch (info->id_DiskState)
		{
	case ID_WRITE_PROTECTED:
		stccpy(DeviceState, GetLocString(MSGID_DEVICE_DISKSTATE_READONLY), sizeof(DeviceState));
		break;
	case ID_VALIDATING:
		stccpy(DeviceState, GetLocString(MSGID_DEVICE_DISKSTATE_VALIDATING), sizeof(DeviceState));
		break;
	case ID_VALIDATED:
		stccpy(DeviceState, GetLocString(MSGID_DEVICE_DISKSTATE_READ_WRITE), sizeof(DeviceState));
		break;
	default:
		stccpy(DeviceState, GetLocString(MSGID_DEVICE_DISKSTATE_UNKNOWN), sizeof(DeviceState));
		break;
		}

	ScaFormatString(GetLocString(MSGID_DEVICE_BLOCKSIZE_BYTES),
		DeviceBlockSize, sizeof(DeviceBlockSize),
		1, info->id_BytesPerBlock);

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

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	do	{
		char VolumeName[128];
		size_t Len;

		OldDir = CurrentDir(arg->wa_Lock);

		if (arg->wa_Name && strlen(arg->wa_Name) > 0)
			{
			fLock = Lock(arg->wa_Name, ACCESS_READ);
			d1(kprintf(__FILE__ "/%s/%ld: fLock=%08lx  Name=<%s>\n", __FUNC__, __LINE__, fLock, arg->wa_Name));
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

			d1(KPrintF(__FILE__ "/%s/%ld: VolumeName=<%s>\n", __FUNC__, __LINE__, VolumeName));

			Len = strlen(VolumeName);
			isDevice = Len > 0 && ':' == VolumeName[Len - 1];
			}
		else
			{
			T_ExamineData *fib;

			if (ScalosExamineBegin(&fib))
				{
				BPTR parentDir;

				if (ScalosExamineLock(arg->wa_Lock, &fib))
					{
					d1(KPrintF(__FILE__ "/%s/%ld: fib_DirEntryType=%ld  <%s>\n", __FUNC__, __LINE__, \
						ScalosExamineGetDirEntryType(fib), ScalosExamineGetName(fib)));
					isDevice = ST_ROOT == ScalosExamineGetDirEntryType(fib);
					}

				parentDir = ParentDir(arg->wa_Lock);
				d1(KPrintF(__FILE__ "/%s/%ld: parentDir=%08lx\n", __FUNC__, __LINE__, parentDir));
				if (parentDir)
					UnLock(parentDir);
				else
					{
					// ParentDir() of root directory returns NULL
					isDevice = TRUE;
					}

				ScalosExamineEnd(&fib);
				}
			}
		} while (0);

	if (fLock)
		UnLock(fLock);
	CurrentDir(OldDir);

	d1(KPrintF(__FILE__ "/%s/%ld: END isDevice=%ld\n", __FUNC__, __LINE__, isDevice));

	return isDevice;
}

//----------------------------------------------------------------------------

static void GetDeviceName(BPTR dLock, CONST_STRPTR VolumeName, STRPTR DeviceName, size_t MaxLen)
{
	struct InfoData *info = AllocInfoData();

	strcpy(DeviceName, "");
	if (info)
		{
		struct DosList *VolumeNode;

		Info(dLock, info);

		VolumeNode = (struct DosList *) BADDR(info->id_VolumeNode);

		d1(KPrintF(__FILE__ "/%s/%ld: VolumeNode=%08lx  Type=%08lx\n", __FUNC__, __LINE__, VolumeNode, VolumeNode->dol_Type));

		if (VolumeNode)
			{
			char DosTypeString[16];
			const struct FileSysEntry *fse;
			char FseHandler[80] = "";
			LONG FseVersion;
			LONG FseRevision;

			if (DLT_VOLUME == VolumeNode->dol_Type)
				{
				d1(KPrintF(__FILE__ "/%s/%ld: DiskType=%08lx\n", __FUNC__, __LINE__, VolumeNode->dol_misc.dol_volume.dol_DiskType));

				stccpy(TextDeviceCreateDate,
					 GetChangeDate(TRUE, &VolumeNode->dol_misc.dol_volume.dol_VolumeDate),
                                         sizeof(TextDeviceCreateDate));
				}

			fse = FindFileSysResEntry(VolumeNode->dol_misc.dol_volume.dol_DiskType);
			if (fse)
				{
				LONG *seg;

				d1(KPrintF(__FILE__ "/%s/%ld: fse=%08lx  ln_Name=%08lx\n", __FUNC__, __LINE__, fse, fse->fse_Node.ln_Name));

				if (fse->fse_Node.ln_Name)
					stccpy(FseHandler, fse->fse_Node.ln_Name, sizeof(FseHandler));
					
				FseVersion = fse->fse_Version >> 16;
				FseRevision = fse->fse_Version & 0xffff;

				sprintf(TextDeviceHandler, GetLocString(MSGID_DEVICE_HANDLER_DISKID),
					FseHandler,
					FseVersion, FseRevision);


				seg = (LONG *) BADDR(fse->fse_SegList);
				d1(KPrintF(__FILE__ "/%s/%ld: seg=%08lx\n", __FUNC__, __LINE__, seg));
				while (seg)
					{
					const struct Resident *rt;
					CONST_STRPTR VersionString;
					size_t SegLength = /* sizeof(LONG) * */ seg[-1];

					d1(KPrintF(__FILE__ "/%s/%ld: seg=%08lx  len=%lu\n", __FUNC__, __LINE__, seg, SegLength));

					rt = SearchResident((UWORD *) (seg + 1), SegLength - 8);
					d1(KPrintF(__FILE__ "/%s/%ld: rt=%08lx\n", __FUNC__, __LINE__, rt));
					if (rt && rt->rt_IdString)
						{
						strcpy(TextDeviceHandler, rt->rt_IdString);
						d1(KPrintF(__FILE__ "/%s/%ld: rt->rt_IdString: TextDeviceHandler=%s\n", __FUNC__, __LINE__, TextDeviceHandler));
						break;
						}

					VersionString = FindVersionString((UBYTE *) (seg + 1), SegLength - 8);
					d1(KPrintF(__FILE__ "/%s/%ld: VersionString=%08lx\n", __FUNC__, __LINE__, VersionString));
					if (VersionString)
						{
						strcpy(TextDeviceHandler, VersionString);
						break;
						}

					seg = (LONG *) BADDR(*seg);
					}
				}

			sprintf(DosTypeString, GetLocString(MSGID_DEVICE_DOSTYPESTRING),
				MakePrintable((VolumeNode->dol_misc.dol_volume.dol_DiskType >> 24) & 0xff),
				MakePrintable((VolumeNode->dol_misc.dol_volume.dol_DiskType >> 16) & 0xff),
				MakePrintable((VolumeNode->dol_misc.dol_volume.dol_DiskType >>  8) & 0xff),
				MakePrintable(VolumeNode->dol_misc.dol_volume.dol_DiskType & 0xff));

			if (VolumeNode->dol_Task && VolumeNode->dol_Task->mp_SigTask)
				{
				struct DosList *dl;

#if defined(__amigaos4__)
				{
				struct FileSystemData *fsd;

				fsd = GetDiskFileSystemData(VolumeName);
				d1(kprintf(__FILE__ "/%s/%ld:  fsd=%08lx\n", __FUNC__, __LINE__, fsd));
				if (fsd)
					{
					d1(kprintf(__FILE__ "/%s/%ld:  fsd_FileSystemName=%08lx\n", __FUNC__, __LINE__, fsd->fsd_FileSystemName));
					d1(kprintf(__FILE__ "/%s/%ld:  fsd_FileSystemName=<%s>\n", __FUNC__, __LINE__, fsd->fsd_FileSystemName));
					stccpy(DeviceName, fsd->fsd_FileSystemName, sizeof(DeviceName));
					FreeDiskFileSystemData(fsd);
					}
				else
					{
					stccpy(DeviceName, ((struct Task *) VolumeNode->dol_Task->mp_SigTask)->tc_Node.ln_Name, MaxLen);
					}
				}
#else //defined(__amigaos4__)
				stccpy(DeviceName, ((struct Task *) VolumeNode->dol_Task->mp_SigTask)->tc_Node.ln_Name, MaxLen);
#endif //defined(__amigaos4__)

				sprintf(TextDeviceOnDev, GetLocString(MSGID_DEVICE_ON_DEVNAME), DeviceName);

				d1(KPrintF(__FILE__ "/%s/%ld: DeviceName=<%s>\n", __FUNC__, __LINE__, DeviceName));

				dl = LockDosList(LDF_DEVICES | LDF_READ);

				dl = FindDosEntry(dl, DeviceName, LDF_DEVICES);
				d1(KPrintF(__FILE__ "/%s/%ld: dl=%08lx\n", __FUNC__, __LINE__, dl));
				if (dl)
					{
					struct FileSysStartupMsg *fsm;

					d1(KPrintF(__FILE__ "/%s/%ld: dl=%08lx  Type=%08lx\n", __FUNC__, __LINE__, dl, dl->dol_Type));
					d1(KPrintF(__FILE__ "/%s/%ld: dl=%08lx  dol_handler=%08lx\n", __FUNC__, __LINE__, dl, dl->dol_misc.dol_handler.dol_Handler));

					if (dl->dol_misc.dol_handler.dol_Handler)
						{
						char Handler[128];

						BtoCstring(dl->dol_misc.dol_handler.dol_Handler, Handler, sizeof(Handler));
						d1(KPrintF(__FILE__ "/%s/%ld: Handler=<%s>\n", __FUNC__, __LINE__, Handler));
						sprintf(TextDeviceHandler, "%s", Handler);
						}

					fsm = BADDR(dl->dol_misc.dol_handler.dol_Startup);
					d1(KPrintF(__FILE__ "/%s/%ld: fsm=%08lx\n", __FUNC__, __LINE__, fsm));
					if (fsm)
						{
						char DevName[128];
						struct DosEnvec *env;

						d1(KPrintF(__FILE__ "/%s/%ld: fssm_Device=%08lx\n", __FUNC__, __LINE__, fsm->fssm_Device));

						BtoCstring(fsm->fssm_Device, DevName, sizeof(DevName));
						sprintf(TextDeviceName, GetLocString(MSGID_DEVICE_DEVICE_UNIT), DevName, fsm->fssm_Unit);

						d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

						env = BADDR(fsm->fssm_Environ);

						d1(KPrintF(__FILE__ "/%s/%ld: env=%08lx\n", __FUNC__, __LINE__, env));

						if ( 0 == (((ULONG) fsm->fssm_Environ) & 0xc0000000)
							&& env
							&& 0 != TypeOfMem((APTR) env)
							&& env->de_TableSize >= DE_DOSTYPE)
							{
							sprintf(DosTypeString, GetLocString(MSGID_DEVICE_DOSTYPESTRING),
								MakePrintable((env->de_DosType >> 24) & 0xff),
								MakePrintable((env->de_DosType >> 16) & 0xff),
								MakePrintable((env->de_DosType >>  8) & 0xff),
								MakePrintable(env->de_DosType & 0xff));
							}
						}
					d1(KPrintF(__FILE__ "/%s/%ld: fsm=%08lx\n", __FUNC__, __LINE__, fsm));
					}

				UnLockDosList(LDF_DEVICES | LDF_READ);
				}

			if (CR == TextDeviceHandler[strlen(TextDeviceHandler) - 2])
				{
				TextDeviceHandler[strlen(TextDeviceHandler) - 2] = '\0';
				strcat(TextDeviceHandler, "\n");
				d1(KPrintF(__FILE__ "/%s/%ld: rt->rt_IdString: TextDeviceHandler - CR =%s\n", __FUNC__, __LINE__, TextDeviceHandler));
				}

			if (strlen(TextDeviceHandler) > 0 && strlen(DosTypeString) > 0)
				{
				if (LF != TextDeviceHandler[strlen(TextDeviceHandler) - 1])
					strcat(TextDeviceHandler, " ");
				strcat(TextDeviceHandler, DosTypeString);
				FoundDeviceHandler = TRUE;
				}
			else if (strlen(TextDeviceHandler) == 0 && strlen(DosTypeString) > 0)	// +jmc+
				{
				stccpy(TextDeviceHandler, DosTypeString, sizeof(TextDeviceHandler));
				FoundDeviceDosType = TRUE;
				d1(KPrintF(__FILE__ "/%s/%ld: TextDeviceHandler: DosTypeString only=<%s>\n", __FUNC__, __LINE__, TextDeviceHandler));
				}
			}

		d1(kprintf(__FILE__ "/%s/%ld: DeviceName=<%s>\n", __FUNC__, __LINE__, DeviceName));

		FreeInfoData(info);
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

	GetPreferences(MainPrefsHandle, ID_MAIN, SCP_LoadDefIconsFirst, &prefDefIconsFirst, sizeof(prefDefIconsFirst));
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

static void BuildDefVolumeNameNoSpace(STRPTR Buffer, CONST_STRPTR VolumeName, size_t MaxLength)
{
	STRPTR dlp;
	size_t Len;

	strcpy(Buffer, "def_");

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

static STRPTR ScaFormatString(CONST_STRPTR formatString, STRPTR Buffer, size_t MaxLen, LONG NumArgs, ...)
{
	va_list args;

	va_start(args, NumArgs);

	if (InformationLocale)
		{
		ULONG *ArgArray;

		ArgArray = malloc(sizeof(ULONG) * NumArgs);
		if (ArgArray)
			{
			struct FormatDateHookData fd;
			struct Hook fmtHook;
			ULONG n;
			STATIC_PATCHFUNC(FormatDateHookFunc)

			for (n = 0; n < NumArgs; n++)
				ArgArray[n] = va_arg(args, LONG);

			fmtHook.h_Entry = (HOOKFUNC) PATCH_NEWFUNC(FormatDateHookFunc);
			fmtHook.h_Data = &fd;

			fd.fdhd_Buffer = Buffer;
			fd.fdhd_Length = MaxLen;

			FormatString(InformationLocale, (STRPTR) formatString, ArgArray, &fmtHook);

			free(ArgArray);
			}
		}
	else
		{
		vsprintf(Buffer, formatString, args);
		}

	va_end(args);

	return Buffer;
}

//----------------------------------------------------------------------------

static void ScaFormatDate(struct DateTime *dt, ULONG DayMaxLen, ULONG DateMaxLen, ULONG TimeMaxLen)
{
	if (InformationLocale)
		{
		struct FormatDateHookData fd;
		struct Hook fmtHook;
		STATIC_PATCHFUNC(FormatDateHookFunc)

		d1(kprintf(__FILE__ "/%s/%ld: DateBuff=%08lx Len=%ld  TimeBuff=%08lx Len=%ld\n", \
			__FUNC__, __LINE__, dt->dat_StrDate, DateMaxLen, dt->dat_StrTime, DateMaxLen));

		fmtHook.h_Entry = (HOOKFUNC) PATCH_NEWFUNC(FormatDateHookFunc);
		fmtHook.h_Data = &fd;

		if (dt->dat_StrDay)
			{
			fd.fdhd_Buffer = dt->dat_StrDay;
			fd.fdhd_Length = DayMaxLen;

			FormatDate(InformationLocale, "%A",
				&dt->dat_Stamp, &fmtHook);
			}

		if (dt->dat_StrDate)
			{
			fd.fdhd_Buffer = dt->dat_StrDate;
			fd.fdhd_Length = DateMaxLen;

			FormatDate(InformationLocale, InformationLocale->loc_ShortDateFormat,
				&dt->dat_Stamp, &fmtHook);
			}

		if (dt->dat_StrTime)
			{
			fd.fdhd_Buffer = dt->dat_StrTime;
			fd.fdhd_Length = TimeMaxLen;

			FormatDate(InformationLocale, InformationLocale->loc_ShortTimeFormat,
				&dt->dat_Stamp, &fmtHook);
			}
		}
	else
		{
		DateToStr(dt);		// no size checking possible here :(
		}
}

//----------------------------------------------------------------------------

static M68KFUNC_P3(void, FormatDateHookFunc,
	A0, struct Hook *, theHook,
	A2, struct Locale *, locale,
	A1, char, ch)
{
	struct FormatDateHookData *fd = (struct FormatDateHookData *) theHook->h_Data;

	(void) locale;

	if (fd->fdhd_Length >= 1)
		{
		*(fd->fdhd_Buffer)++ = ch;
		fd->fdhd_Length--;
		}
}
M68KFUNC_END

//----------------------------------------------------------------------------

static void BtoCstring(BSTR bstr, STRPTR Buffer, size_t BuffLen)
{
#ifdef __AROS__
	// AROS needs special handling because it uses NULL-terminated
	// strings on some platforms.
	size_t Len = AROS_BSTR_strlen(bstr);
	if (Len >= BuffLen)
		Len = BuffLen  - 2;
	strncpy(Buffer, AROS_BSTR_ADDR(bstr), Len);
	Buffer[Len] = '\0';
#else
	const char *bString = BADDR(bstr);

	*Buffer = '\0';

	if ( 0 == (((ULONG) bstr) & 0xc0000000) && bString && 0 != TypeOfMem((APTR) bString) )
		{
		size_t Len = *bString;

		if (Len >= BuffLen)
			Len = BuffLen  - 2;

		strncpy(Buffer, bString + 1, Len);
		Buffer[Len] = '\0';
		}
#endif
}

//----------------------------------------------------------------------------

static void ByteCount(STRPTR Buffer, size_t BuffLen, LONG NumBlocks, LONG BytesPerBlock)
{
	CONST_STRPTR Decimal;

	if (InformationLocale)
		Decimal = InformationLocale->loc_DecimalPoint;
	else
		Decimal = ".";

	// Normalize block size to KBytes
	while (BytesPerBlock > 1024)
		{
		BytesPerBlock >>= 1;
		NumBlocks <<= 1;
		}
	while (BytesPerBlock < 1024)
		{
		BytesPerBlock <<= 1;
		NumBlocks >>= 1;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: NumBlocks=%lu\n", __FUNC__, __LINE__, NumBlocks));
	
        if (NumBlocks > 2 * 1024 * 1024)
		{
		// > 2 GB
		ULONG GBytes = NumBlocks / (1024 * 1024);
		ULONG MBytes = NumBlocks / 1024 - GBytes * 1024;

		d1(KPrintF(__FILE__ "/%s/%ld: GBytes=%lu  MBytes=%lu\n", __FUNC__, __LINE__, GBytes, MBytes));

		if (GBytes > 200)
			ScaFormatString(GetLocString(MSGID_BYTES_GB), Buffer, BuffLen, 1, GBytes);
		else if (GBytes > 20)
			ScaFormatString(GetLocString(MSGID_BYTES_GB_1), Buffer, BuffLen, 3, GBytes, Decimal, MBytes / 100);
		else
			ScaFormatString(GetLocString(MSGID_BYTES_GB_2), Buffer, BuffLen, 3, GBytes, Decimal, MBytes / 10);
		}
	else if (NumBlocks > 2 * 1024)
		{
		// > 2 MB
		ULONG MBytes = NumBlocks / 1024;
		ULONG KBytes = NumBlocks % 1024;

		if (MBytes > 200)
			ScaFormatString(GetLocString(MSGID_BYTES_MB), Buffer, BuffLen, 1, MBytes);
		else if (MBytes > 20)
			ScaFormatString(GetLocString(MSGID_BYTES_MB_1), Buffer, BuffLen, 3, MBytes, Decimal, KBytes / 100);
		else
			ScaFormatString(GetLocString(MSGID_BYTES_MB_2), Buffer, BuffLen, 3, MBytes, Decimal, KBytes / 10);
		}
	else
		{
		ScaFormatString(GetLocString(MSGID_BYTES_KB), Buffer, BuffLen, 1, NumBlocks);
		}

}

//----------------------------------------------------------------------------

static UBYTE MakePrintable(UBYTE ch)
{
	if (ch < ' ')
		ch += '0';

	return ch;
}

//----------------------------------------------------------------------------

static CONST_STRPTR GetIconTypeName(Object *IconObj)
{
	IPTR IconType;
	CONST_STRPTR TypeName;

	GetAttr(IDTA_Type, IconObj, &IconType);

	switch (IconType)
		{
	case WBDISK:
		TypeName = GetLocString(MSGID_ICONTYPE_DISK);
		break;
	case WBDRAWER:
		TypeName = GetLocString(MSGID_ICONTYPE_DRAWER);
		break;
	case WBTOOL:
		TypeName = GetLocString(MSGID_ICONTYPE_TOOL);
		break;
	case WBPROJECT:
		TypeName = GetLocString(MSGID_ICONTYPE_PROJECT);
		break;
	case WBGARBAGE:
		TypeName = GetLocString(MSGID_ICONTYPE_TRASHCAN);
		break;
	case WBDEVICE:
		TypeName = GetLocString(MSGID_ICONTYPE_DEVICE);
		break;
	case WBKICK:
		TypeName = GetLocString(MSGID_ICONTYPE_KICKSTART);
		break;
	case WBAPPICON:
		TypeName = GetLocString(MSGID_ICONTYPE_APPICON);
		break;
	default:
		TypeName = GetLocString(MSGID_ICONTYPE_UNKNOWN);
		break;
		}

	return TypeName;
}

//----------------------------------------------------------------------------

static const struct FileSysEntry *FindFileSysResEntry(ULONG DosType)
{
	struct FileSysResource *FileSysRes;

	FileSysRes = OpenResource(FSRNAME);
	if (FileSysRes)
		{
		const struct FileSysEntry *fse;

		for (fse = (const struct FileSysEntry *) FileSysRes->fsr_FileSysEntries.lh_Head;
			fse != (const struct FileSysEntry *) &FileSysRes->fsr_FileSysEntries.lh_Tail;
			fse = (const struct FileSysEntry *) fse->fse_Node.ln_Succ)
			{
			if (fse->fse_DosType == DosType)
				return fse;
			}
		}

	return NULL;
}

//----------------------------------------------------------------------------

static STRPTR GetChangeDate(BOOL Valid, struct DateStamp *ChangeDate)
{
	if (Valid)
		{
		char DayBuffer[64], DateBuffer[64], TimeBuffer[64];
		struct DateTime dt;

		memset(&dt, 0, sizeof(dt));

		dt.dat_Stamp = *ChangeDate;
		dt.dat_StrDay  = DayBuffer;
		dt.dat_StrDate = DateBuffer;
		dt.dat_StrTime = TimeBuffer;

		ScaFormatDate(&dt, sizeof(DayBuffer), sizeof(DateBuffer), sizeof(TimeBuffer));

		sprintf(TextLastChange, "%s, %s %s", DayBuffer, DateBuffer, TimeBuffer);
		}
	else
		{
		strcpy(TextLastChange, GetLocString(MSGID_VERSION_NOT_AVAILABLE));
		}

	return TextLastChange;
}

//----------------------------------------------------------------------------

static ULONG GetStackSize(Object *IconObj)
{
	IPTR StackSize = 16384;

	GetAttr(IDTA_Stacksize, IconObj, &StackSize);

	return StackSize;
}

//----------------------------------------------------------------------------

static STRPTR GetDefaultTool(Object *IconObj)
{
	STRPTR DefaultTool = "";

	GetAttr(IDTA_DefaultTool, IconObj, (APTR)&DefaultTool);

	return DefaultTool ? DefaultTool : (STRPTR) "";
}

//----------------------------------------------------------------------------

static STRPTR GetVersionString(CONST_STRPTR IconName)
{
///
	char VersMask[10];
	BPTR fh = (BPTR)NULL;
	UBYTE *Buffer;
	const size_t BuffLength = 1024;
	T_TIMEVAL StartTime, currentTime;
	STRPTR VersionString = TextVersion;
	size_t MaxLen = sizeof(TextVersion);

	stccpy(VersionString, GetLocString(MSGID_VERSION_NOT_AVAILABLE), MaxLen);

	// do not use statically initalized "$VER" variable here
	// since it might confuse the "version" command.
	// strcat(VersMask, "VER: ");
	stccpy(VersMask, versTag + 1, 6 + 1);

	switch (ScalosExamineGetDirEntryType(fib))
		{
	case ST_FILE:
	case ST_LINKFILE:
	case ST_PIPEFILE:
		break;
	default:
		return TextVersion;
		break;
		}

	do	{
		ULONG TotalLen = 0;
		LONG ActLen;
		ULONG MaskNdx = 0;
		BOOL Finished = FALSE;
		BOOL Found = FALSE;

		Buffer = malloc(BuffLength);
		if (NULL == Buffer)
			break;

		CurrentTime((ULONG *)&StartTime.tv_secs, (ULONG *)&StartTime.tv_micro);

		fh = Open(IconName, MODE_OLDFILE);
		if ((BPTR)NULL == fh)
			break;

		do	{
			ActLen = Read(fh, Buffer, BuffLength);

			if (ActLen > 0)
				{
				ULONG n;
				UBYTE *BufPtr = Buffer;

				TotalLen += ActLen;

				for (n=0; !Finished && n<ActLen; n++)
					{
					if (Found)
						{
						if (MaxLen > 1 && *BufPtr && *BufPtr >= ' ' && *BufPtr <= 0x7f)
							{
							*VersionString++ = *BufPtr++;
							MaxLen--;
							}
						else
							Finished = TRUE;
						}
					else
						{
						if (*BufPtr++ == VersMask[MaskNdx])
							{
							MaskNdx++;

							if (MaskNdx >= strlen(VersMask))
								{
								Found = TRUE;
								}
							}
						else
							{
							MaskNdx = 0;
							}
						}
					}
				}

			CurrentTime((ULONG *)&currentTime.tv_secs, (ULONG *)&currentTime.tv_micro);
			if ((currentTime.tv_secs - StartTime.tv_secs) > 2)
				break;
			} while (ActLen > 0 && TotalLen < 0x100000 && !Finished);
		} while (0);

	*VersionString = '\0';

	if (Buffer)
		free(Buffer);
	if (fh)
		Close(fh);

	return TextVersion;
///
}

//----------------------------------------------------------------------------

static CONST_STRPTR FindVersionString(const UBYTE *block, size_t BlockLen)
{
	static char VersMask[10] = "$";

	// do not use statically initalized "$VER" variable here
	// since it might confuse the "version" command.
	strcat(VersMask, "VER: ");

	while (BlockLen--)
		{
		if (0 == memcmp(VersMask, block, strlen(VersMask)))
			return (CONST_STRPTR)(block + strlen(VersMask));

		block++;
		}

	return NULL;
}

//----------------------------------------------------------------------------

const struct Resident *SearchResident(const UWORD *block, size_t BlockLen)
{
	while (BlockLen -= sizeof(UWORD))
		{
		const struct Resident *res = (struct Resident *) block;

		if (RTC_MATCHWORD == res->rt_MatchWord && res->rt_MatchTag == res)
			return res;

		block++;
		}

	return NULL;
}

//----------------------------------------------------------------------------

static void SetGuiFromIconType(Object *IconObj)
{
	IPTR IconType;

	GetAttr(IDTA_Type, IconObj, &IconType);

	SetAttrs(TextTypeName,
		MUIA_Text_Contents, GetIconTypeName(IconObj),
		TAG_END);

	// Attention:
	// Always make sure the "MUIA_ShowMe, FALSE" lines are grouped BEFORE the "MUIA_ShowMe, TRUE" ones.
	// Otherwise, the information window will grow substantially in vertical direction.

	switch (IconType)
		{
	case WBDISK:
		SetAttrs(GroupDrawer, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupProject, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupTool, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupDisk, MUIA_ShowMe, TRUE, TAG_END);
		SetAttrs(MenuTypeDisk, MUIA_Menuitem_Checked, TRUE, TAG_END);
		SetupToolTypes(IconObj, TextEditorToolTypesDevice);
		break;
	case WBDRAWER:
		SetAttrs(GroupDisk, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupProject, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupTool, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupDrawer, MUIA_ShowMe, TRUE, TAG_END);
		SetAttrs(MenuTypeDrawer, MUIA_Menuitem_Checked, TRUE, TAG_END);
		SetupToolTypes(IconObj, TextEditorToolTypesDrawer);
		break;
	case WBGARBAGE:
		SetAttrs(GroupDisk, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupProject, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupTool, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupDrawer, MUIA_ShowMe, TRUE, TAG_END);
		SetAttrs(MenuTypeTrashcan, MUIA_Menuitem_Checked, TRUE, TAG_END);
		SetupToolTypes(IconObj, TextEditorToolTypesDrawer);
		break;
	case WBTOOL:
		SetAttrs(GroupDisk, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupDrawer, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupProject, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupTool, MUIA_ShowMe, TRUE, TAG_END);
		SetAttrs(MenuTypeTool, MUIA_Menuitem_Checked, TRUE, TAG_END);
		SetupToolTypes(IconObj, TextEditorToolTypesTool);
		break;
	case WBPROJECT:
		SetAttrs(GroupDisk, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupDrawer, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupTool, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupProject, MUIA_ShowMe, TRUE, TAG_END);
		SetAttrs(MenuTypeProject, MUIA_Menuitem_Checked, TRUE, TAG_END);
		SetupToolTypes(IconObj, TextEditorToolTypesProject);
		break;
	default:
		SetAttrs(GroupDisk, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupDrawer, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupProject, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(GroupTool, MUIA_ShowMe, FALSE, TAG_END);
		SetAttrs(MenuTypeDisk, MUIA_Menuitem_Checked, TRUE, TAG_END);
		break;
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT ChangeIconTypeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct DefIconInfo *dii = (struct DefIconInfo *) hook->h_Data;
	ULONG *pNewType = (ULONG *) msg;

	d1(KPrintF(__FILE__ "/%s/%ld: o=%08lx  msg=%08lx  NewType=%08lx\n",  __FUNC__, __LINE__, o, msg, *pNewType));

	SetAttrs(*dii->dii_IconObjectFromDisk,
		IDTA_Type, *pNewType,
		TAG_END);

	SetGuiFromIconType(*dii->dii_IconObjectFromDisk);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT StartFromCycleHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	IPTR StartFrom;

	GetAttr(MUIA_Cycle_Active, o, &StartFrom);

	SetAttrs(ButtonPromptForInputProject,
		MUIA_Disabled, (STARTFROM_CLI != StartFrom && STARTFROM_Arexx != StartFrom),
		TAG_END);
	SetAttrs(ButtonPromptForInputTool,
		MUIA_Disabled, (STARTFROM_CLI != StartFrom && STARTFROM_Arexx != StartFrom),
		TAG_END);
}

//----------------------------------------------------------------------------

static void ReportError(ULONG MsgId)
{
	LONG ErrorCode;
	char Buffer[256];

	ErrorCode = IoErr();
	Fault(ErrorCode, "", Buffer, sizeof(Buffer));

	MUI_Request(APP_Main, WIN_Main, 0, NULL,
		(STRPTR)GetLocString(MSGID_ERROR_REQ_OK),
		(STRPTR)GetLocString(MsgId),
		Buffer);
}

//----------------------------------------------------------------------------

static BOOL RenameDevice(CONST_STRPTR OldName, CONST_STRPTR NewName)
{
	STRPTR VolumeName;
	BOOL Success = FALSE;

	do	{
		VolumeName = malloc(1 + 1 + strlen(OldName));
		if (NULL == VolumeName)
			break;

		strcpy(VolumeName, OldName);
		strcat(VolumeName, ":");

		if (!Relabel(VolumeName, NewName))
			{
			ReportError(MSGID_ERROR_RELABEL);
			break;
			}

		Success = TRUE;
		} while (0);

	if (VolumeName)
		free(VolumeName);

	return Success;
}

//----------------------------------------------------------------------------

static BOOL RenameObject(CONST_STRPTR OldName, CONST_STRPTR NewName)
{
	STRPTR OldIconName;
	STRPTR NewIconName = NULL;
	BOOL Success = FALSE;

	do	{
		OldIconName = malloc(1 + 5 + strlen(OldName));
		if (NULL == OldIconName)
			break;

		NewIconName = malloc(1 + 5 + strlen(NewName));
		if (NULL == NewIconName)
			break;

		if (!Rename(OldName, NewName))
			{
			ReportError(MSGID_ERROR_RENAME);
			break;
			}

		strcpy(OldIconName, OldName);
		strcat(OldIconName, ".info");

		strcpy(NewIconName, NewName);
		strcat(NewIconName, ".info");

		if (!Rename(OldIconName, NewIconName))
			{
			ReportError(MSGID_ERROR_RENAME);
			break;
			}

		Success = TRUE;
		} while (0);

	if (OldIconName)
		free(OldIconName);
	if (NewIconName)
		free(NewIconName);

	return Success;
}

//----------------------------------------------------------------------------

static void SetupToolTypes(Object *IconObj, Object *TextEditor)
{
	STRPTR *ToolTypesArray = NULL;
	STRPTR *TTPtr;
	BOOL IsNewIcon = FALSE;

	GetAttr(IDTA_ToolTypes, IconObj, (APTR)&ToolTypesArray);

	for (TTPtr = ToolTypesArray; TTPtr && *TTPtr; TTPtr++)
		{
		if (ToolTypeNameCmp("*** DON'T EDIT THE FOLLOWING LINES!! ***", *TTPtr)	)
			{
			IsNewIcon = TRUE;
			continue;
			}
		if (IsNewIcon && ToolTypeNameCmp("IM1", *TTPtr))
			continue;
		if (IsNewIcon && ToolTypeNameCmp("IM2", *TTPtr))
			continue;

		if (!ToolTypeNameCmp("CLI", *TTPtr)
			&& !ToolTypeNameCmp("REXX", *TTPtr)
			&& !ToolTypeNameCmp("TOOLPRI", *TTPtr)
			&& !ToolTypeNameCmp("WAIT", *TTPtr)
			&& !ToolTypeNameCmp("STARTPRI", *TTPtr)
			&& !ToolTypeNameCmp("DONOTWAIT", *TTPtr)
			&& !ToolTypeNameCmp("DONOTPROMPT", *TTPtr))
			{
			DoMethod(TextEditor, MUIM_TextEditor_InsertText,
				*TTPtr,
				MUIV_TextEditor_InsertText_Bottom);
			DoMethod(TextEditor, MUIM_TextEditor_InsertText,
				"\n",
				MUIV_TextEditor_InsertText_Bottom);
			}
		}
}

//----------------------------------------------------------------------------

static void SetChangedToolTypes(Object *IconObj, Object *TextEditor)
{
	IPTR  Changed = FALSE;
	ULONG ToolTypesCount = 0;
	STRPTR Contents = NULL;
	STRPTR *ToolTypeArray = NULL;

	do	{
		CONST_STRPTR lp;
		ULONG n;

		GetAttr(MUIA_TextEditor_HasChanged, TextEditor, &Changed);
		if (!Changed)
			break;

		Contents = (STRPTR) DoMethod(TextEditor, MUIM_TextEditor_ExportText);
		if (NULL == Contents)
			break;

		get(TextEditor, MUIA_TextEditor_Prop_Entries, &ToolTypesCount);

		ToolTypeArray = calloc(sizeof(STRPTR), 1 + ToolTypesCount);
		if (NULL == ToolTypeArray)
			break;

		lp = Contents;
		n = 0;
		while (*lp)
			{
			CONST_STRPTR Line = lp;
			size_t LineLength = 0;

			while (*lp && '\n' != *lp)
				{
				LineLength++;
				lp++;
				}
			if (*lp)
				lp++;	// skip LF

			ToolTypeArray[n] = malloc(1 + LineLength);
			if (NULL == ToolTypeArray[n])
				break;

			memcpy(ToolTypeArray[n], Line, LineLength);
			ToolTypeArray[n][LineLength] = '\0';

			if (++n >= ToolTypesCount)
				break;
			}

		SetAttrs(IconObj,
			IDTA_ToolTypes, ToolTypeArray,
			TAG_END);

		set(TextEditor, MUIA_TextEditor_HasChanged, FALSE);
		} while (0);

	if (Contents)
		{
		FreeVec(Contents);
		}
	if (ToolTypeArray)
		{
		STRPTR *lp = ToolTypeArray;

		while (*lp)
			{
			free(*lp);
			*lp = NULL;
			lp++;
			}

		free(ToolTypeArray);
		}
}

//----------------------------------------------------------------------------

static STRPTR GetSizeString(void)
{
	STRPTR SizeString;

	if (FibValid)
		{
		char ByteCountText[40];

		Convert64(InformationLocale, ScalosExamineGetSize(fib), ByteCountText, sizeof(ByteCountText));

		SizeString = ScaFormatString(GetLocString(MSGID_SIZE_FORMAT), TextSize, sizeof(TextSize),
			1, ByteCountText);
		}
	else
		SizeString = (STRPTR)GetLocString(MSGID_VERSION_NOT_AVAILABLE);

	return SizeString;
}

//----------------------------------------------------------------------------

//  Checks if a certain version of a MCC is available
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev)
{
///
	BOOL flush = TRUE;

	d1(kprintf("%s/%s/%ld: %s ", __FILE__, __FUNC__,  __LINE__, name));

	while (1)
		{
		ULONG ver = 0;
		ULONG rev = 0;
		struct Library *base;
		char libname[256];

		// First we attempt to acquire the version and revision through MUI
		Object *obj = MUI_NewObject((STRPTR) name, TAG_DONE);
		if (obj)
			{
			get(obj, MUIA_Version, &ver);
			get(obj, MUIA_Revision, &rev);

			MUI_DisposeObject(obj);

			if(ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf("%s/%s/%ld: v%ld.%ld found through MUIA_Version/Revision\n", __FILE__, __FUNC__, __LINE__, ver, rev));
				return TRUE;
				}
			}

		// If we did't get the version we wanted, let's try to open the
		// libraries ourselves and see what happens...
		stccpy(libname, "PROGDIR:mui", sizeof(libname));
		AddPart(libname, name, sizeof(libname));

		if ((base = OpenLibrary(&libname[8], 0)) || (base = OpenLibrary(&libname[0], 0)))
			{
			UWORD OpenCnt = base->lib_OpenCnt;

			ver = base->lib_Version;
			rev = base->lib_Revision;

			CloseLibrary(base);

			// we add some additional check here so that eventual broken .mcc also have
			// a chance to pass this test (i.e. Toolbar.mcc is broken)
			if (ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf("%s/%s/%ld: v%ld.%ld found through OpenLibrary()\n", __FILE__, __FUNC__, __LINE__, ver, rev));
				return TRUE;
				}

			if (OpenCnt > 1)
				{
				if (MUI_Request(NULL, NULL, 0L, (STRPTR)GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR)GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD), (STRPTR)GetLocString(MSGID_STARTUP_MCC_IN_USE),
					name, minver, minrev, ver, rev))
					{
					flush = TRUE;
					}
				else
					break;
				}

			// Attempt to flush the library if open count is 0 or because the
			// user wants to retry (meaning there's a chance that it's 0 now)

			if (flush)
				{
				struct Library *result;

				Forbid();
				if ((result = (struct Library *) FindName(&((struct ExecBase *)SysBase)->LibList, name)))
					RemLibrary(result);
				Permit();
				flush = FALSE;
				}
			else
				{
				d1(kprintf("%s/%s/%ld: couldn`t find minimum required version.\n", __FILE__, __FUNC__, __LINE__));

				// We're out of luck - open count is 0, we've tried to flush
				// and still haven't got the version we want
				if (MUI_Request(NULL, NULL, 0L, (STRPTR)GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR)GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD), (STRPTR)GetLocString(MSGID_STARTUP_OLD_MCC),
					name, minver, minrev, ver, rev))
					{
					flush = TRUE;
					}
				else
					break;
				}
			}
		else
			{
			// No MCC at all - no need to attempt flush
			flush = FALSE;
			if (!MUI_Request(NULL, NULL, 0L, (STRPTR)GetLocString(MSGID_STARTUP_FAILURE),
				(STRPTR)GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD), (STRPTR)GetLocString(MSGID_STARTUP_MCC_NOT_FOUND),
				name, minver, minrev))
				{
				break;
				}
			}
		}

	return FALSE;
///
}

//----------------------------------------------------------------------------

static BOOL ToolTypeNameCmp(CONST_STRPTR ToolTypeName, CONST_STRPTR ToolTypeContents)
{
	while (*ToolTypeName && *ToolTypeContents)
		{
		UBYTE ch1;	// Work-around for GCC compiler bug

		if ('=' == *ToolTypeContents)
			break;
		ch1 = ToLower(*ToolTypeName);
		if (ch1 != ToLower(*ToolTypeContents))
			break;

		ToolTypeName++;
		ToolTypeContents++;
		}

	return (BOOL) ('\0' == *ToolTypeName && ('\0' == *ToolTypeContents || '=' == *ToolTypeContents));
}

//----------------------------------------------------------------------------

static void GetIconObjectFromWBArg(struct DefIconInfo *dii, struct WBArg *arg,
	STRPTR FileTypeName, size_t FileTypeNameSize)
{
	ULONG Pos;
	STRPTR NewIconName;

	d1(KPrintF(__FILE__ "/%s/%ld: wa_Lock=%08lx  wa_Name=<%s>\n", __FUNC__, __LINE__, arg->wa_Lock, arg->wa_Name));

	if (*dii->dii_IconObjectFromDisk)
		{
		DisposeIconObject(*dii->dii_IconObjectFromDisk);
		*dii->dii_IconObjectFromDisk = NULL;
		}
	if (*dii->dii_IconObjectFromScalos)
		{
		DisposeIconObject(*dii->dii_IconObjectFromScalos);
		*dii->dii_IconObjectFromScalos = NULL;
		}

	if (IsDevice(arg))
		{
		static char VolumeName[128];
		char DeviceName[128];

		d1(KPrintF(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

		NewIconName = "disk";

		NameFromLock(arg->wa_Lock, VolumeName, sizeof(VolumeName));
		GetDeviceName(arg->wa_Lock, VolumeName, DeviceName, sizeof(DeviceName));

		d1(kprintf(__FILE__ "/%s/%ld: VolumeName=<%s>\n", __FUNC__, __LINE__, VolumeName));

		StripTrailingChar(VolumeName, ':');
		StripTrailingChar(DeviceName, ':');

		d1(kprintf(__FILE__ "/%s/%ld: VolumeName=<%s>\n", __FUNC__, __LINE__, VolumeName));
		d1(kprintf(__FILE__ "/%s/%ld: DeviceName=<%s>\n",  __FUNC__, __LINE__, DeviceName));

		GetDeviceIconObject(dii, NewIconName, VolumeName, DeviceName, FileTypeName, FileTypeNameSize);

		if (*dii->dii_IconObjectFromDisk)
			{
			SetAttrs(*dii->dii_IconObjectFromDisk,
				IDTA_Type, WBDISK,
				TAG_END);

			FoundDeviceFromArg = TRUE;
			}
		}
	else
		{
		char xName[128];

		d1(KPrintF(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
		
		stccpy(xName, arg->wa_Name, sizeof(xName));
		Pos = IsIconName(xName);

		if (Pos && ~0 != Pos)
			{
			// strip ".info" from name
			*((char *) Pos) = '\0';
			}

		GetIconObject(dii, xName, arg->wa_Lock, FileTypeName, FileTypeNameSize);

		NameFromLock(arg->wa_Lock,PathName,sizeof(PathName));
		}
}

//----------------------------------------------------------------------------

static BOOL RemoveDropZoneForObject(Object *o)
{
	struct DropZone *dz;

	for (dz = (struct DropZone *) DropZoneList.lh_Head;
		dz != (struct DropZone *) &DropZoneList.lh_Tail;
		dz = (struct DropZone *) dz->dz_Node.ln_Succ)
		{
		if (dz->dz_Object == o)
			{
			Remove(&dz->dz_Node);

			if (myAppWindow && dz->dz_DropZone)
				RemoveAppWindowDropZone(myAppWindow, dz->dz_DropZone);

			free(dz);

			return TRUE;	// found
			}
		}

	return FALSE;	// not found
}

//----------------------------------------------------------------------------

static void AddDropZoneForObject(Object *o, DropZoneHandler Handler)
{
	struct DropZone *dz;

	d1(KPrintF("%s/%ld: o=%08lx\n", __FUNC__, __LINE__, o));

	if (WorkbenchBase->lib_Version < 44)
		return;

	// if a dropzone already exists for this object, remove it
	RemoveDropZoneForObject(o);

	// Now create new dropzone for this object
	do	{
		ULONG Left = 0, Top = 0, Width = 0, Height = 0;

		dz = malloc(sizeof(struct DropZone));
		if (NULL == dz)
			break;

		dz->dz_Object = o;
		dz->dz_Handler = Handler;

		get(o, MUIA_LeftEdge, &Left);
		get(o, MUIA_TopEdge, &Top);
		get(o, MUIA_Width, &Width);
		get(o, MUIA_Height, &Height);

		d1(KPrintF("%s/%ld: o=%08lx  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, o, Left, Top, Width, Height));

		dz->dz_Box.Left = Left;
		dz->dz_Box.Top = Top;
		dz->dz_Box.Width = Width;
		dz->dz_Box.Height = Height;

		if (myAppWindow && Width && Height)
			{
			// AddAppWindowDropZoneA()
			dz->dz_DropZone = AddAppWindowDropZone(myAppWindow, 0,
				(ULONG) dz,
				WBDZA_Hook, (ULONG) &AppWindowDropZoneHook,
				WBDZA_Box, (ULONG) &dz->dz_Box,
				TAG_END);

			d1(KPrintF("%s/%ld: dz_DropZone=%08lx\n", __FUNC__, __LINE__, dz->dz_DropZone));
			if (NULL == dz->dz_DropZone)
				break;
			}

		AddTail(&DropZoneList, &dz->dz_Node);
		dz = NULL;
		} while (0);

	// If we did not succeed, free allocated DropZone
	if (dz)
		free(dz);
}

//----------------------------------------------------------------------------

static void AdjustDropZones(void)
{
	struct DropZone *dz;

	for (dz = (struct DropZone *) DropZoneList.lh_Head;
		dz != (struct DropZone *) &DropZoneList.lh_Tail;
		dz = (struct DropZone *) dz->dz_Node.ln_Succ)
		{
		struct MUI_AreaData *mad;

		mad = muiAreaData(dz->dz_Object);

		if (mad)
			{
			if (((mad->mad_Flags & MADF_OBJECTVISIBLE) && NULL == dz->dz_DropZone)
				|| (!(mad->mad_Flags & MADF_OBJECTVISIBLE) && dz->dz_DropZone)
				|| mad->mad_Box.Left != dz->dz_Box.Left
				|| mad->mad_Box.Top != dz->dz_Box.Top
				|| mad->mad_Box.Width != dz->dz_Box.Width
				|| mad->mad_Box.Height != dz->dz_Box.Height)
				{
				d1(KPrintF("%s/%ld: myAppWindow=%08lx  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, myAppWindow, mad->mad_Box.Left, mad->mad_Box.Top, mad->mad_Box.Width, mad->mad_Box.Height));

				dz->dz_Box = mad->mad_Box;

				if (dz->dz_DropZone)
					RemoveAppWindowDropZone(myAppWindow, dz->dz_DropZone);

				if (mad->mad_Flags & MADF_OBJECTVISIBLE)
					{
					dz->dz_DropZone = AddAppWindowDropZone(myAppWindow, 0,
						(ULONG) dz,
						WBDZA_Hook, (ULONG) &AppWindowDropZoneHook,
						WBDZA_Box, (ULONG) &dz->dz_Box,
						TAG_END);
					}
				else
					{
					dz->dz_DropZone = NULL;
					}
				}
			}
		}
}

//----------------------------------------------------------------------------

static void HandleDropOnIcon(struct WBArg *arg, Object *o, struct DefIconInfo *dii)
{
	BPTR OldDir;
	Object *OldIconObj = *dii->dii_IconObjectFromDisk;

	(void) o;

	*dii->dii_IconObjectFromDisk = NULL;

	OldDir = CurrentDir(arg->wa_Lock);

	d1(KPrintF(__FILE__ "/%s/%ld: wa_Lock=%08lx  wa_Name=<%s>\n", __FUNC__, __LINE__, arg->wa_Lock, arg->wa_Name));

	GetIconObjectFromWBArg(dii, arg, TextFileTypeName, sizeof(TextFileTypeName));

	if (*dii->dii_IconObjectFromDisk)
		ReplaceIcon(*dii->dii_IconObjectFromDisk, &OldIconObj);
	else
		DisposeIconObject(OldIconObj);

	CurrentDir(OldDir);
}

//----------------------------------------------------------------------------

static void HandleDropOnString(struct WBArg *arg, Object *o, struct DefIconInfo *dii)
{
	d1(KPrintF(__FILE__ "/%s/%ld: wa_Lock=%08lx  wa_Name=<%s>\n", __FUNC__, __LINE__, arg->wa_Lock, arg->wa_Name));

	(void) dii;

	do	{
		char Path[MAX_PATH_SIZE];
		struct MUI_AreaData *mad;

		if (!NameFromLock(arg->wa_Lock, Path, sizeof(Path)))
			break;

		if (!AddPart(Path, arg->wa_Name, sizeof(Path)))
			break;

		mad = muiAreaData(o);
		if (NULL == mad)
			break;
		d1(KPrintF(__FILE__ "/%s/%ld: mad=%08lx  mad_Flags=%08lx\n", __FUNC__, __LINE__, mad, mad->mad_Flags));
		if (!(mad->mad_Flags & MADF_OBJECTVISIBLE))
			break;

		set(o, MUIA_String_Contents, Path);
                } while (0);
}

//----------------------------------------------------------------------------

static void HandleDropOnName(struct WBArg *arg, Object *o, struct DefIconInfo *dii)
{
	d1(KPrintF(__FILE__ "/%s/%ld: wa_Lock=%08lx  wa_Name=<%s>\n", __FUNC__, __LINE__, arg->wa_Lock, arg->wa_Name));

	(void) dii;

	do	{
		struct MUI_AreaData *mad;
		char Path[MAX_PATH_SIZE];
		STRPTR Name = arg->wa_Name;

		if (NULL == arg->wa_Name || strlen(arg->wa_Name) < 1)
			{
			if (!NameFromLock(arg->wa_Lock, Path, sizeof(Path)))
				break;
			Name = FilePart(Path);
			}
		if (NULL == Name)
			break;

		mad = muiAreaData(o);
		if (NULL == mad)
			break;
		d1(KPrintF(__FILE__ "/%s/%ld: mad=%08lx  mad_Flags=%08lx\n", __FUNC__, __LINE__, mad, mad->mad_Flags));
		if (!(mad->mad_Flags & MADF_OBJECTVISIBLE))
			break;

		set(o, MUIA_String_Contents, Name);
                } while (0);
}

//----------------------------------------------------------------------------

static void HandleDropOnToolTypes(struct WBArg *arg, Object *o, struct DefIconInfo *dii)
{
	BPTR OldDir;

	OldDir = CurrentDir(arg->wa_Lock);

	d1(KPrintF(__FILE__ "/%s/%ld: wa_Lock=%08lx  wa_Name=<%s>\n", __FUNC__, __LINE__, arg->wa_Lock, arg->wa_Name));

	GetIconObjectFromWBArg(dii, arg, TextFileTypeName, sizeof(TextFileTypeName));

	if (*dii->dii_IconObjectFromDisk)
		{
		DoMethod(o, MUIM_TextEditor_ClearText);
		SetupToolTypes(*dii->dii_IconObjectFromDisk, o);
		}

	CurrentDir(OldDir);
}

//----------------------------------------------------------------------------

static BOOL CheckIsWBStartup(BPTR DirLock)
{
	BPTR WbStartupLock;
	BPTR ParentLock;
	BPTR NewDirLock	= (BPTR)NULL;
	BOOL IsWBStartup = FALSE;

	do	{
		WbStartupLock = Lock("SYS:WBStartup", ACCESS_READ);
		if ((BPTR)NULL == WbStartupLock)
			break;

		while (1)
			{
			if (LOCK_SAME == SameLock(DirLock, WbStartupLock))
				{
				IsWBStartup = TRUE;
				break;
				}

			ParentLock = ParentDir(DirLock);
			if ((BPTR)NULL == ParentLock)
				break;	// we have reached root

			if (NewDirLock)
				UnLock(NewDirLock);
			NewDirLock = ParentLock;

			DirLock = ParentLock;
			}
		} while (0);

	if (NewDirLock)
		UnLock(NewDirLock);
	if (WbStartupLock)
		UnLock(WbStartupLock);

	return IsWBStartup;
}

//----------------------------------------------------------------------------

static void ScaFormatStringMaxLength(char *Buffer, size_t BuffLen, const char *Format, ...)
{
	va_list Args;

	va_start(Args, Format);

	ScaFormatStringArgs(Buffer, BuffLen, Format, Args);

	va_end(Args);
}

//-----------------------------------------------------------------------------------

static void ScaFormatStringArgs(char *Buffer, size_t BuffLength, const char *Format, APTR Args)
{
	// here converting %lU und %lD format strings to lower case
	// because sprintf doesn't recognize them
	char *FormatCopy;

	FormatCopy = AllocCopyString(Format);
	d1(kprintf("%s/%s/%ld: ALLOC FormatCopy=%08lx\n", __FILE__, __FUNC__, __LINE__, FormatCopy));
	if (FormatCopy)
		{
		char *p;

		p = strstr(Format, "%lU");
		if (p)
			p[2] = 'u';

		p = strstr(Format, "%lD");
		if (p)
			p[2] = 'd';

		vsprintf(Buffer, Format, Args);
		d1(kprintf("%s/%s/%ld: FREE FormatCopy=%08lx\n", __FILE__, __FUNC__, __LINE__, FormatCopy));
		FreeCopyString(FormatCopy);
		}
}

//----------------------------------------------------------

static STRPTR AllocCopyString(CONST_STRPTR clp)
{
	STRPTR lp;

	if (NULL == clp)
		clp = "";

	lp = AllocVec(1 + strlen(clp), MEMF_PUBLIC);
	if (lp)
		strcpy(lp, clp);

	d1(KPrintF("%s/%s/%ld: kp=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, lp, clp));

	return lp;
}


static void FreeCopyString(STRPTR lp)
{
	d1(KPrintF("%s/%s/%ld: lp=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, lp, lp));
	if (lp)
		FreeVec(lp);
}

//----------------------------------------------------------

static Object *GetIconObjectFromScalos(BPTR DirLock, CONST_STRPTR IconName,
	STRPTR FileTypeName, size_t FileTypeNameSize)
{
	struct ScaWindowList *WinList = NULL;
	Object *IconObj = NULL;

	d1(KPrintF(__FILE__ "/%s/%ld: IconName=<%s>\n", __FUNC__, __LINE__, IconName));

	WinList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	d1(KPrintF(__FILE__ "/%s/%ld: WinList=%08lx\n", __FUNC__, __LINE__, WinList));
	if (WinList)
		{
		struct ScaWindowStruct *ws;

		for (ws = WinList->wl_WindowStruct; (NULL == IconObj) && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if ((BPTR) NULL == ws->ws_Lock
				|| LOCK_SAME == SameLock(ws->ws_Lock, DirLock))
				{
				struct ScaWindowTask *wt = ws->ws_WindowTask;
				struct ScaIconNode *in;

				ObtainSemaphoreShared(wt->wt_IconSemaphore);

				for (in = wt->wt_IconList; (NULL == IconObj) && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					CONST_STRPTR Name = NULL;

					GetAttr(IDTA_Text, in->in_Icon, (APTR) &Name);

					if ( (((BPTR) NULL == ws->ws_Lock && in->in_Lock && LOCK_SAME == SameLock(in->in_Lock, DirLock))
						|| (LOCK_SAME == SameLock(ws->ws_Lock, DirLock)))
						&& (Name && 0 == Stricmp(IconName, Name)) )
						{
						struct TagItem CloneTags[1];

						GetIconFileType(wt, in, FileTypeName, FileTypeNameSize);

						CloneTags[0].ti_Tag = TAG_END;

						if (SCAV_ViewModes_Icon != ws->ws_Viewmodes)
							{
							//GetAttr(TIDTA_IconObject, in->in_Icon, (ULONG *) &IconObj);
							IconObj = NULL;
							}
						else
							{
							IconObj = (Object *) DoMethod(in->in_Icon,
								IDTM_CloneIconObject, CloneTags);
							}

						d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx\n", __FUNC__, __LINE__, IconObj));

						if (IconObj)
							{
							SetAttrs(IconObj,
								IDTA_InnerTop, 0,
								IDTA_InnerLeft, 0,
								IDTA_InnerBottom, 0,
								IDTA_InnerRight, 0,
								IDTA_FrameType, MF_FRAME_NONE,
								IDTA_FrameTypeSel, MF_FRAME_NONE,
								TAG_END);
							}
                                                }
					}

				ReleaseSemaphore(wt->wt_IconSemaphore);
				}
			}

		SCA_UnLockWindowList();
		}

	d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx\n", __FUNC__, __LINE__, IconObj));

	return IconObj;
}

//----------------------------------------------------------
#if 0
static Object *GetDeviceIconObjectFromScalos(BPTR dLock,
	STRPTR FileTypeName, size_t FileTypeNameSize)
{
	struct ScaWindowList *WinList = NULL;
	Object *IconObj = NULL;

	d1(KPrintF(__FILE__ "/%s/%ld: dLock=%08lx\n", __FUNC__, __LINE__, dLock));

	WinList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	d1(KPrintF(__FILE__ "/%s/%ld: WinList=%08lx\n", __FUNC__, __LINE__, WinList));
	if (WinList)
		{
		struct ScaWindowStruct *ws;

		for (ws = WinList->wl_WindowStruct; (NULL == IconObj) && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if ((BPTR) NULL == ws->ws_Lock)
				{
				// This is the desktop window
				struct ScaWindowTask *wt = ws->ws_WindowTask;
				struct ScaIconNode *in;
				struct FileLock *fl = (struct FileLock *) BADDR(dLock);

				ObtainSemaphoreShared(wt->wt_IconSemaphore);

				for (in = wt->wt_IconList; (NULL == IconObj) && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					d1(KPrintF(__FILE__ "/%s/%ld: in=%08lx  <%s>\n", __FUNC__, __LINE__, in, in->in_Name));
					if (in->in_DeviceIcon)
						{
						const struct ScaDeviceIcon *di = in->in_DeviceIcon;

						if ( fl->fl_Task == di->di_Handler )
							{
							struct TagItem CloneTags[1];

							GetIconFileType(wt, in, FileTypeName, FileTypeNameSize);

							CloneTags[0].ti_Tag = TAG_END;

							IconObj = (Object *) DoMethod(in->in_Icon,
								IDTM_CloneIconObject, CloneTags);
							d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx\n", __FUNC__, __LINE__, IconObj));

							if (IconObj && (SCAV_ViewModes_Icon != ws->ws_Viewmodes))
								{
								GetAttr(TIDTA_IconObject, IconObj, (ULONG *) &IconObj);
								}

							if (IconObj)
								{
								SetAttrs(IconObj,
									IDTA_InnerTop, 0,
									IDTA_InnerLeft, 0,
									IDTA_InnerBottom, 0,
									IDTA_InnerRight, 0,
									IDTA_FrameType, MF_FRAME_NONE,
									IDTA_FrameTypeSel, MF_FRAME_NONE,
									TAG_END);
								}
	                                                }
						}
					}

				ReleaseSemaphore(wt->wt_IconSemaphore);
				break;
				}
			}

		SCA_UnLockWindowList();
		}

	d1(KPrintF(__FILE__ "/%s/%ld: IconObj=%08lx\n", __FUNC__, __LINE__, IconObj));

	return IconObj;
}
#endif
//----------------------------------------------------------

static void GetIconFileType(struct ScaWindowTask *wt, struct ScaIconNode *in,
	STRPTR FileTypeName, size_t FileTypeNameSize)
{
	DoMethod(wt->mt_MainObject, SCCM_IconWin_GetIconFileType, in);

	d1(KPrintF(__FILE__ "/%s/%ld: in=%08lx  <%s>  in_FileType=%08lx\n", __FUNC__, __LINE__, in, in->in_Name, in->in_FileType));

	switch ((ULONG) in->in_FileType)
		{
	case WBDRAWER:
	case WB_TEXTICON_DRAWER:
		stccpy(FileTypeName, GetLocString(MSGID_FILETYPE_DRAWER), FileTypeNameSize);
		break;

	case WBGARBAGE:
		stccpy(FileTypeName, GetLocString(MSGID_FILETYPE_GARBAGE), FileTypeNameSize);
		break;

	case WBDISK:
		stccpy(FileTypeName, GetLocString(MSGID_FILETYPE_DISK), FileTypeNameSize);
		break;

	case WBTOOL:
	case WB_TEXTICON_TOOL:		// Text Tool
		stccpy(FileTypeName, GetLocString(MSGID_FILETYPE_TOOL), FileTypeNameSize);
		break;

	case WBPROJECT:
		stccpy(FileTypeName, GetLocString(MSGID_FILETYPE_PROJECT), FileTypeNameSize);
		break;

	case WBAPPICON:
		stccpy(FileTypeName, GetLocString(MSGID_FILETYPE_APPICON), FileTypeNameSize);
		break;

	default:
		if ( 0 != TypeOfMem((APTR) in->in_FileType) )
			{
			const struct TypeNode *tn = (const struct TypeNode *) in->in_FileType;

			stccpy(FileTypeName, tn->tn_Name, FileTypeNameSize);
			}
		break;
		}
}

//----------------------------------------------------------

static struct InfoData *AllocInfoData(void)
{
#ifdef __amigaos4__
	return (struct InfoData *) AllocDosObject(DOS_INFODATA, NULL);
#else // __amigaos4__
	return (struct InfoData *) malloc(sizeof(struct InfoData));
#endif //__amigaos4__
}

// ----------------------------------------------------------

static void FreeInfoData(struct InfoData *pId)
{
	if (pId)
		{
#ifdef __amigaos4__
		FreeDosObject(DOS_INFODATA, pId);
#else // __amigaos4__
		free(pId);
#endif //__amigaos4__
		}
}

// ----------------------------------------------------------

static struct ScaWindowStruct *FindScalosWindow(BPTR dirLock)
{
	struct ScaWindowList *wl;
	BOOL Found = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: START \n", __FUNC__, __LINE__));

	wl = SCA_LockWindowList(SCA_LockWindowList_Shared);

	if (wl)
		{
		struct ScaWindowStruct *ws;

		for (ws = wl->wl_WindowStruct; !Found && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (((BPTR) NULL == dirLock && (BPTR) NULL == ws->ws_Lock) || (LOCK_SAME == SameLock(dirLock, ws->ws_Lock)))
				{
				return ws;
				}
			}
		SCA_UnLockWindowList();
		}

	return NULL;
}

//----------------------------------------------------------------------------


// Find.c
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
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/exall.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/gadtools.h>
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include <devices/clipboard.h>
#include <devices/timer.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <scalos/scalos.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/scalos.h>
#include <proto/timer.h>

#include <DefIcons.h>
#include <defs.h>
#include <Year.h> // +jmc+

#include "Find.h"
#include "debug.h"

#define	Find_NUMBERS
#define	Find_ARRAY
#define	Find_CODE
#define	Find_NUMBERS
#define	Find_ARRAY
#define	Find_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#if !defined(__amigaos4__) && !defined(__AROS__)
#include <dos.h>

long _stack = 16384;		// minimum stack size, used by SAS/C startup code
#endif

//----------------------------------------------------------------------------

// local data structures

#define SEARCH_PATTERN_DEFAULT	"#?"
#define SEARCH_CONTENTS_DEFAULT	""
#define SEARCH_FILETYPE_DEFAULT	""

#define BUFFERSIZE (4096*10) /* XXX: perhaps should be 8192 ? test.. */

#define USE_EXALL	1

// maximum number of popup nlist entries that will be saved persistently
#define	MAX_SAVED_NLIST_ENTRIES	10

#define KeyButtonHelp(name,key,HelpText)\
	TextObject,\
		ButtonFrame,\
		MUIA_CycleChain, TRUE, \
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp, HelpText,\
		End

struct ResultsListEntry
	{
	STRPTR rle_Name;
	STRPTR rle_Path;
	STRPTR rle_CopyString;
	};

struct FindResult
	{
	BPTR fr_Lock;		// Lock to parent directory
	STRPTR fr_Name;		// File name
	};

struct MyNListExport
	{
	ULONG nle_Size;		// Total Size
	char nle_Data[0];	// Contents
	};

//----------------------------------------------------------------------------

// local functions

static void init(void);
static void fail(APTR APP_Main, CONST_STRPTR str);
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT AppMessageHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT IntuiMessageHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT SelectSourceHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT RemoveSourceHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT AddSourceHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ResultsConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm);
static SAVEDS(void) INTERRUPT ResultsDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm);
static SAVEDS(ULONG) INTERRUPT ResultsDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm);
static SAVEDS(LONG) INTERRUPT ResultsCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm);
static SAVEDS(LONG) INTERRUPT ResultsCopyEntryToClipHookFunc(struct Hook *hook, Object *obj, struct NList_CopyEntryToClipMessage *ncm);
static SAVEDS(LONG) INTERRUPT StartSearchHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(LONG) INTERRUPT StopSearchHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(VOID) INTERRUPT OpenResultHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(VOID) INTERRUPT CopyResultsToClipHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(VOID) INTERRUPT OpenResultWithFindHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(VOID) INTERRUPT OpenResultWithMultiviewHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT FileTypesConstructHookFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_ConstructMessage *ltcm);
static SAVEDS(void) INTERRUPT FileTypesDestructHookFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DestructMessage *ltdm);
static SAVEDS(ULONG) INTERRUPT FileTypesDisplayHookFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DisplayMessage *ltdm);
static SAVEDS(LONG) INTERRUPT HideFindHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(LONG) INTERRUPT PatternHistoryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(LONG) INTERRUPT ContentsHistoryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(LONG) INTERRUPT FiletypeSelectHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(LONG) INTERRUPT CheckEmptyStringsHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(LONG) INTERRUPT FindFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(LONG) INTERRUPT FindNextFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CollapseFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ExpandFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CollapseAllFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ExpandAllFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SelectFileTypesEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(VOID) INTERRUPT FileTypesPopupWindowHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(VOID) INTERRUPT ClearHistoryHookFunc(struct Hook *hook, Object *o, Msg msg);

static void StartSearch(void);
static void DoSearchPath(CONST_STRPTR Path, CONST_STRPTR Pattern,
	CONST_STRPTR Contents, CONST_STRPTR Filetype);
static void DoSearchDir(BPTR DirLock, CONST_STRPTR Pattern,
	CONST_STRPTR Contents, CONST_STRPTR Filetype);
static void SearchEntry(BPTR DirLock, CONST_STRPTR Name, LONG DirEntryType,
	CONST_STRPTR Pattern, CONST_STRPTR Contents, CONST_STRPTR Filetype);
static void SearchFile(CONST_STRPTR FileName, CONST_STRPTR Pattern,
	CONST_STRPTR Contents, CONST_STRPTR Filetype);
static void preQsBc(const UBYTE *x, LONG m, LONG qsBc[]);
static BOOL SearchTypeNodeTree(const struct TypeNode *tn, CONST_STRPTR Name);
static LONG QuickSearch(const UBYTE *x, LONG m, UBYTE *y, LONG n);
static BOOL MatchFileContents(BPTR fh, CONST_STRPTR Contents);

static STRPTR GetLocString(ULONG MsgId);
//static void TranslateStringArray(STRPTR *stringArray);
static void TranslateNewMenu(struct NewMenu *nm);

static void FillSourcesList(struct WBStartup *WBenchMsg);
static Object *MyPopButton(ULONG img);
BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev);
static BOOL SearchFileType(CONST_STRPTR SearchString, BOOL SearchNext);
static LONG GetElapsedTime(T_TIMEVAL *tv);
static struct MUI_NListtree_TreeNode *FindPatternInListtree(Object *lt,
	struct MUI_NListtree_TreeNode *list, CONST_STRPTR pattern,
	struct MUI_NListtree_TreeNode *startnode, ULONG *dosearch);
static void AddSourcesLine(CONST_STRPTR NewSourcePath);
static void PushEntry(Object *NList, CONST_STRPTR Contents, LONG Position);
static void ExportNList(Class *cl, Object *obj, struct MUIP_Export *msg);
static void ImportNList(Class *cl, Object *obj, struct MUIP_Import *msg);

//----------------------------------------------------------------------------

static struct Hook AboutHook = {{ NULL, NULL }, HOOKFUNC_DEF(OpenAboutFunc), NULL };
static struct Hook AboutMUIHook = {{ NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIFunc), NULL };
static struct Hook AppMessageHook = {{ NULL, NULL }, HOOKFUNC_DEF(AppMessageHookFunc), NULL };
static struct Hook SelectSourceHook = {{ NULL, NULL }, HOOKFUNC_DEF(SelectSourceHookFunc), NULL };
static struct Hook RemoveSourceHook = {{ NULL, NULL }, HOOKFUNC_DEF(RemoveSourceHookFunc), NULL };
static struct Hook AddSourceHook = {{ NULL, NULL }, HOOKFUNC_DEF(AddSourceHookFunc), NULL };
static struct Hook ResultsConstructHook = {{ NULL, NULL }, HOOKFUNC_DEF(ResultsConstructHookFunc), NULL };
static struct Hook ResultsDestructHook = {{ NULL, NULL }, HOOKFUNC_DEF(ResultsDestructHookFunc), NULL };
static struct Hook ResultsDisplayHook = {{ NULL, NULL }, HOOKFUNC_DEF(ResultsDisplayHookFunc), NULL };
static struct Hook ResultsCompareHook = {{ NULL, NULL }, HOOKFUNC_DEF(ResultsCompareHookFunc), NULL };
static struct Hook ResultsCopyEntryToClipHook = {{ NULL, NULL }, HOOKFUNC_DEF(ResultsCopyEntryToClipHookFunc), NULL };
static struct Hook StartSearchHook = {{ NULL, NULL }, HOOKFUNC_DEF(StartSearchHookFunc), NULL };
static struct Hook StopSearchHook = {{ NULL, NULL }, HOOKFUNC_DEF(StopSearchHookFunc), NULL };
static struct Hook IntuiMessageHook = {{ NULL, NULL }, HOOKFUNC_DEF(IntuiMessageHookFunc), NULL };
static struct Hook OpenResultHook = {{ NULL, NULL }, HOOKFUNC_DEF(OpenResultHookFunc), NULL };
static struct Hook CopyResultsToClipHook = {{ NULL, NULL }, HOOKFUNC_DEF(CopyResultsToClipHookFunc), NULL };
static struct Hook FileTypesConstructHook = {{ NULL, NULL }, HOOKFUNC_DEF(FileTypesConstructHookFunc), NULL };
static struct Hook FileTypesDestructHook = {{ NULL, NULL }, HOOKFUNC_DEF(FileTypesDestructHookFunc), NULL };
static struct Hook FileTypesDisplayHook = {{ NULL, NULL }, HOOKFUNC_DEF(FileTypesDisplayHookFunc), NULL };
static struct Hook HideFindHook = {{ NULL, NULL }, HOOKFUNC_DEF(HideFindHookFunc), NULL };
static struct Hook PatternHistoryHook = {{ NULL, NULL }, HOOKFUNC_DEF(PatternHistoryHookFunc), NULL };
static struct Hook ContentsHistoryHook = {{ NULL, NULL }, HOOKFUNC_DEF(ContentsHistoryHookFunc), NULL };
static struct Hook FiletypeSelectHook = {{ NULL, NULL }, HOOKFUNC_DEF(FiletypeSelectHookFunc), NULL };
static struct Hook CheckEmptyStringsHook = {{ NULL, NULL }, HOOKFUNC_DEF(CheckEmptyStringsHookFunc), NULL };
static struct Hook FindFileTypeHook = {{ NULL, NULL }, HOOKFUNC_DEF(FindFileTypeHookFunc), NULL };
static struct Hook FindNextFileTypeHook = {{ NULL, NULL }, HOOKFUNC_DEF(FindNextFileTypeHookFunc), NULL };
static struct Hook CollapseFileTypesHook = {{ NULL, NULL }, HOOKFUNC_DEF(CollapseFileTypesHookFunc), NULL };
static struct Hook ExpandFileTypesHook = {{ NULL, NULL }, HOOKFUNC_DEF(ExpandFileTypesHookFunc), NULL };
static struct Hook CollapseAllFileTypesHook = {{ NULL, NULL }, HOOKFUNC_DEF(CollapseAllFileTypesHookFunc), NULL };
static struct Hook ExpandAllFileTypesHook = {{ NULL, NULL }, HOOKFUNC_DEF(ExpandAllFileTypesHookFunc), NULL };
static struct Hook ClearHistoryHook = {{ NULL, NULL }, HOOKFUNC_DEF(ClearHistoryHookFunc), NULL };
static struct Hook SelectFileTypesEntryHook = {{ NULL, NULL }, HOOKFUNC_DEF(SelectFileTypesEntryHookFunc), NULL };
static struct Hook FileTypesPopupWindowHook = {{ NULL, NULL }, HOOKFUNC_DEF(FileTypesPopupWindowHookFunc), NULL };
static struct Hook OpenResultWithFindHook = {{ NULL, NULL }, HOOKFUNC_DEF(OpenResultWithFindHookFunc), NULL };
static struct Hook OpenResultWithMultiviewHook = {{ NULL, NULL }, HOOKFUNC_DEF(OpenResultWithMultiviewHookFunc), NULL };

//----------------------------------------------------------------------------

// Context menu in "results" list
static struct NewMenu NewContextMenuResults[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_MENU_RESULTS_TITLE,		NULL, 	0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_RESULTS_OPEN,   	NULL, 	0,	0, &OpenResultWithMultiviewHook },
	{  NM_ITEM, (STRPTR) MSGID_MENU_RESULTS_OPENDRAWER, 	NULL, 	0,	0, &OpenResultHook },
	{  NM_ITEM, (STRPTR) MSGID_MENU_RESULTS_COPYTOCLIP, 	NULL, 	0,	0, &CopyResultsToClipHook },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_RESULTS_FIND, 		NULL, 	0,	0, &OpenResultWithFindHook },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 	0,	0, (APTR) &AboutHook },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

// Context menu in "filetypes" list
static struct NewMenu NewContextMenuFileTypes[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_MENU_FILETYPES_TITLE,	NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COLLAPSE,	 	NULL, 	NM_ITEMDISABLED,	0, (APTR) &CollapseFileTypesHook },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_EXPAND, 		NULL, 	NM_ITEMDISABLED,	0, (APTR) &ExpandFileTypesHook },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COLLAPSEALL, 	NULL, 	0,			0, (APTR) &CollapseAllFileTypesHook },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_EXPANDALL, 	NULL, 	0,			0, (APTR) &ExpandAllFileTypesHook },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 	0,			0, (APTR) &AboutHook },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

// Context menu in "pattern" and "contents" history popup list
static struct NewMenu NewContextMenuHistoryPopup[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_MENU_FILETYPES_TITLE,	NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_CLEARHISTORY, 		NULL, 	0,			0, (APTR) &ClearHistoryHook },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 	0,			0, (APTR) &AboutHook },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

//----------------------------------------------------------------------------

// local data items

struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct ScalosBase *ScalosBase = NULL;
T_LOCALEBASE LocaleBase = NULL;
T_TIMERBASE TimerBase;

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct ScalosIFace *IScalos = NULL;
struct LocaleIFace *ILocale = NULL;
struct TimerIFace *ITimer;
#endif

static struct Catalog *FindCatalog;

static BOOL StopSearch = FALSE;

static Object *APP_Main;
static Object *WIN_Main;
static Object *WIN_AboutMUI;
static Object *NListviewSources, *NListSources;
static Object *StringNewSource;
static Object *NListResults;
static Object *StringFilePattern, *StringFileContents, *StringFileType;
static Object *TextCurrentFile;
static Object *GroupFindFiletype;
static Object *ButtonHideFind;
static Object *PopObjectPattern, *PopObjectContents, *PopObjectFiletypes;
static Object *NListPatternHistory, *NListContentsHistory;
static Object *ButtonSearch, *ButtonStop;
static Object *StringFindFileType;
static Object *ButtonFindNextFileType;
static Object *ButtonAddSource, *ButtonRemoveSource;
static Object *PopAslNewSource;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit;
static Object *MenuExpandFileTypes, *MenuCollapseFileTypes;
static Object *ContextMenuResults;
static Object *ContextMenuFileTypes;
static Object *ContextMenuPatternPopup, *ContextMenuContentsPopup;

Object *ListtreeFileTypes;

struct WBStartup *WBenchMsg;

static char CurrentPath[1024];

static T_TIMEREQUEST *TimerIO;
static struct MsgPort *TimerPort;
static T_TIMEVAL LastUpdate;

DISPATCHER_PROTO(myFindResultsNList);
DISPATCHER_PROTO(myFileTypesNListTree);
DISPATCHER_PROTO(myPersistentNList);
DISPATCHER_PROTO(myPopObject);

static struct MUI_CustomClass *myFindResultsNListClass;
static struct MUI_CustomClass *myFileTypesNListTreeClass;
static struct MUI_CustomClass *myPersistentNListClass;
static struct MUI_CustomClass *myPopObjectClass;


#if defined(__AROS__)
#define FindResultsNListObject		BOOPSIOBJMACRO_START(myFindResultsNListClass->mcc_Class)
#define FileTypesNListTreeObject	BOOPSIOBJMACRO_START(myFileTypesNListTreeClass->mcc_Class)
#define PersistentNListObject		BOOPSIOBJMACRO_START(myPersistentNListClass->mcc_Class)
#define PopObject			BOOPSIOBJMACRO_START(myPopObjectClass->mcc_Class)
#else
#define FindResultsNListObject		NewObject(myFindResultsNListClass->mcc_Class, NULL
#define FileTypesNListTreeObject	NewObject(myFileTypesNListTreeClass->mcc_Class, NULL
#define PersistentNListObject		NewObject(myPersistentNListClass->mcc_Class, NULL
#define PopObject			NewObject(myPopObjectClass->mcc_Class, NULL
#endif

//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	LONG win_opened;
	
	WBenchMsg = (argc == 0) ? (struct WBStartup *)argv : NULL;

	init();

	if (!CheckMCCVersion(MUIC_NListview, 19, 66)
		|| !CheckMCCVersion(MUIC_NListtree, 18, 18))
		{
		d1(KPrintF(__FILE__ "/%s/%ld: CheckMCCVersion failed\n", __FUNC__, __LINE__));
		return 10;
		}

	ContextMenuResults = MUI_MakeObject(MUIO_MenustripNM, NewContextMenuResults, 0);
	if (NULL == ContextMenuResults)
		{
		fail(APP_Main, "Failed to create Results Context Menu.");
		}

	ContextMenuFileTypes = MUI_MakeObject(MUIO_MenustripNM, NewContextMenuFileTypes, 0);
	if (NULL == ContextMenuFileTypes)
		{
		fail(APP_Main, "Failed to create Filetypes Context Menu.");
		}

	ContextMenuPatternPopup	= MUI_MakeObject(MUIO_MenustripNM, NewContextMenuHistoryPopup, 0);
	if (NULL == ContextMenuPatternPopup)
		{
		fail(APP_Main, "Failed to create Pattern Popup Context Menu.");
		}

	ContextMenuContentsPopup = MUI_MakeObject(MUIO_MenustripNM, NewContextMenuHistoryPopup, 0);
	if (NULL == ContextMenuContentsPopup)
		{
		fail(APP_Main, "Failed to create Contents Popup Context Menu.");
		}

	APP_Main = ApplicationObject,
		MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
		MUIA_Application_Version,	"$VER: Scalos Find.module V" VERS_MAJOR "." VERS_MINOR " (" __DATE__ ") " COMPILER_STRING,
		MUIA_Application_Copyright,	"© The Scalos Team, 2008" CURRENTYEAR,
		MUIA_Application_Author,	"The Scalos Team",
		MUIA_Application_Description,	"Scalos Find module",
		MUIA_Application_Base,		"SCALOS_FIND_MODULE",

		SubWindow, WIN_Main = WindowObject,
			MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
			MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
			MUIA_Window_AppWindow, TRUE,

			WindowContents, VGroup,

				Child, HGroup,
					Child, VGroup,
						Child, ColGroup(2),
							Child, Label1(GetLocString(MSGID_POPSTRING_NAME)),
							Child, PopObjectPattern = PopObject,
								MUIA_Popstring_Button, MyPopButton(MUII_PopUp),
								MUIA_Popstring_String, StringFilePattern = StringObject,
									StringFrame,
									MUIA_CycleChain, TRUE,
									MUIA_String_Contents, SEARCH_PATTERN_DEFAULT,
									End, //StringObject
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_POPSTRING_NAME_SHORTHELP),
								MUIA_Popobject_Object, VGroup,
									Child, NListviewObject,
										MUIA_NListview_NList, NListPatternHistory = PersistentNListObject,
											MUIA_ObjectID, OBJID_NLIST_NAME,
											MUIA_ContextMenu, ContextMenuPatternPopup,
											MUIA_NList_Exports, MUIV_NList_Exports_All,
											MUIA_NList_Imports, MUIV_NList_Imports_All,
											MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
											MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
											End, //NListObject
										End, //NListviewObject
									End, //VGroup
								End, //PopobjectObject

							Child, Label1(GetLocString(MSGID_POPSTRING_TEXT)),
							Child, PopObjectContents = PopObject,
								MUIA_Popstring_Button, MyPopButton(MUII_PopUp),
								MUIA_Popstring_String, StringFileContents = StringObject,
									StringFrame,
									MUIA_CycleChain, TRUE,
									MUIA_String_Contents, SEARCH_CONTENTS_DEFAULT,
									End, //StringObject
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_POPSTRING_TEXT_SHORTHELP),
								MUIA_Popobject_Object, VGroup,
									Child, NListviewObject,
										MUIA_NListview_NList, NListContentsHistory = PersistentNListObject,
											MUIA_ObjectID, OBJID_NLIST_TEXT,
											MUIA_ContextMenu, ContextMenuContentsPopup,
											MUIA_NList_Exports, MUIV_NList_Exports_All,
											MUIA_NList_Imports, MUIV_NList_Imports_All,
											MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
											MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
											End, //NListObject
										End, //NListviewObject
									End, //VGroup
								End, //PopobjectObject

							Child, Label1(GetLocString(MSGID_POPSTRING_TYPE)),
							Child, PopObjectFiletypes = PopObject,
								MUIA_Popstring_Button, MyPopButton(MUII_PopUp),
								MUIA_Popstring_String, StringFileType = StringObject,
									StringFrame,
									MUIA_CycleChain, TRUE,
									MUIA_String_Contents, SEARCH_FILETYPE_DEFAULT,
									End, //StringObject
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_POPSTRING_TYPE_SHORTHELP),
								MUIA_Popobject_WindowHook, &FileTypesPopupWindowHook,
								MUIA_Popobject_Object, VGroup,
									Child, NListviewObject,
										MUIA_CycleChain, TRUE,
										MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_LISTVIEW_FILETYPES),
										MUIA_NListview_NList, ListtreeFileTypes = FileTypesNListTreeObject,
											MUIA_Background, MUII_ListBack,
											MUIA_NListtree_DisplayHook, &FileTypesDisplayHook,
											MUIA_NListtree_ConstructHook, &FileTypesConstructHook,
											MUIA_NListtree_DestructHook, &FileTypesDestructHook,
											MUIA_NListtree_FindNameHook, MUIV_NListtree_FindNameHook_PartCaseInsensitive,
											MUIA_NListtree_EmptyNodes, TRUE,
											MUIA_NListtree_AutoVisible, MUIV_NListtree_AutoVisible_Expand,
											MUIA_ContextMenu, ContextMenuFileTypes,
											End, //myFileTypesNListTreeClass
										End, //NListviewObject
									Child, GroupFindFiletype = HGroup,
										GroupFrame,
										Child, ButtonHideFind = TextObject,
											MUIA_Weight, 10,
											ButtonFrame,
											MUIA_CycleChain, TRUE,
#if defined(MUII_Close)
											MUIA_Text_Contents, MUIX_C "\33I[6:" STR(MUII_Close) "]",
#else
											MUIA_Text_Contents, MUIX_C "\33I[6:" STR(MUII_TapeStop) "]",
#endif
											MUIA_InputMode, MUIV_InputMode_RelVerify,
											MUIA_Background, MUII_ButtonBack,
											MUIA_ShortHelp, GetLocString(MSGID_BUTTON_FIND_HIDE_SHORTHELP),
											End,

										Child, StringFindFileType = StringObject,
											MUIA_CycleChain, TRUE,
											StringFrame,
											End, //StringObject
										Child, ButtonFindNextFileType = KeyButtonHelp(GetLocString(MSGID_BUTTON_FIND_NEXT),
											*GetLocString(MSGID_BUTTON_FIND_NEXT_KEY),
											GetLocString(MSGID_BUTTON_FIND_NEXT_SHORTHELP)),
										End, //HGroup
									End, //VGroup
								End, //PopobjectObject

							End, //ColGroup

						Child, VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_SOURCES),
							GroupFrame,
							MUIA_Background, MUII_GroupBack,

							Child, NListviewSources = NListviewObject,
								MUIA_NListview_NList, NListSources = NListObject,
									MUIA_CycleChain, TRUE,
									MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
									MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
									End, //NListObject
								End, //NListviewObject

							Child, HGroup,
								Child, PopAslNewSource = PopaslObject,
									MUIA_CycleChain, TRUE,
									MUIA_Popasl_Type, ASL_FileRequest,
									MUIA_Popstring_Button, MyPopButton(MUII_PopDrawer),
									MUIA_Popstring_String, StringNewSource = StringObject,
										StringFrame,
										MUIA_CycleChain, TRUE,
									End, //StringObject

									ASLFR_TitleText, (ULONG) GetLocString(MSGID_GROUP_SOURCES_ASLTITLE),
									ASLFR_DrawersOnly, TRUE,
									ASLFR_IntuiMsgFunc, &IntuiMessageHook,
									End, //PopaslObject

								Child, ButtonAddSource = KeyButtonHelp(GetLocString(MSGID_BUTTON_ADD),
									*GetLocString(MSGID_BUTTON_ADD_KEY),
		                                                        GetLocString(MSGID_BUTTON_ADD_SHORTHELP)),
								Child, ButtonRemoveSource = KeyButtonHelp(GetLocString(MSGID_BUTTON_REMOVE),
									*GetLocString(MSGID_BUTTON_REMOVE_KEY),
									GetLocString(MSGID_BUTTON_REMOVE_SHORTHELP)),
								End, //HGroup
							End, //VGroup
						End, //VGroup

					Child, BalanceObject,
						End, // BalanceObject

					Child, VGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_RESULTS),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, NListviewObject,
							//MUIA_CycleChain, TRUE,
							MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_FullAuto,
							MUIA_NListview_NList, NListResults = FindResultsNListObject,
								MUIA_CycleChain, TRUE,
								MUIA_NList_Format, "BAR,BAR,",
								MUIA_NList_ConstructHook2, &ResultsConstructHook,
								MUIA_NList_DestructHook2, &ResultsDestructHook,
								MUIA_NList_DisplayHook2, &ResultsDisplayHook,
								MUIA_NList_CompareHook2, &ResultsCompareHook,
								MUIA_NList_CopyEntryToClipHook2, &ResultsCopyEntryToClipHook,
								MUIA_NList_TitleSeparator, TRUE,
								MUIA_NList_Title, TRUE,
								MUIA_NList_SortType, 0,
								MUIA_ContextMenu, ContextMenuResults,
								MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 0,
								End, //NListObject
							End, //NListviewObject

	                                        End, //VGroup

					End, // HGroup

				Child, TextCurrentFile = TextObject,
					MUIA_Text_Contents, "",
					End, //TextObject

				Child, ColGroup(2),
					Child, ButtonSearch = KeyButtonHelp(GetLocString(MSGID_BUTTON_SEARCH),
						*GetLocString(MSGID_BUTTON_SEARCH_KEY),
						GetLocString(MSGID_BUTTON_SEARCH_SHORTHELP)),
					Child, ButtonStop = KeyButtonHelp(GetLocString(MSGID_BUTTON_STOP),
						*GetLocString(MSGID_BUTTON_STOP_KEY),
						GetLocString(MSGID_BUTTON_STOP_SHORTHELP)),
					End, //ColGroup
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

	InitDefIcons();

	DoMethod(APP_Main, MUIM_Application_Load, MUIV_Application_Load_ENV);

	//--------------------------------------------------------------------------//

	DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
		WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);
	DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
		APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	DoMethod(ButtonStop, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 2, MUIM_CallHook, &StopSearchHook);
	DoMethod(ButtonSearch, MUIM_Notify,MUIA_Pressed, FALSE,
		APP_Main, 2, MUIM_CallHook, &StartSearchHook);

	//--------------------------------------------------------------------------//

	// Find selected entries from context menus
	MenuExpandFileTypes = (Object *) DoMethod(ContextMenuFileTypes,
			MUIM_FindUData, &ExpandFileTypesHook);
	MenuCollapseFileTypes = (Object *) DoMethod(ContextMenuFileTypes,
			MUIM_FindUData, &CollapseFileTypesHook);

	// activate StringFilePattern on startup
	set(WIN_Main, MUIA_Window_ActiveObject, StringFilePattern);

	set(ButtonStop, MUIA_Disabled, TRUE);
	set(ButtonRemoveSource, MUIA_Disabled, TRUE);

	// Call hook whenever an entry in sources list is selected
	DoMethod(NListSources, MUIM_Notify, MUIA_NList_SelectChange, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &SelectSourceHook);
	// Call hook whenever "remove source" button is clicked
	DoMethod(ButtonRemoveSource, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 2, MUIM_CallHook, &RemoveSourceHook);
	// Call hook whenever "add source" button is clicked
	DoMethod(ButtonAddSource, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 2, MUIM_CallHook, &AddSourceHook);

	// Call Add Source hook whenever return is hit in source string gadget
	DoMethod(StringNewSource, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &AddSourceHook);

	// Start Search whenever return it hit in "name" string gadget
	DoMethod(StringFilePattern, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &StartSearchHook);

	// Start Search whenever return it hit in "text" string gadget
	DoMethod(StringFileContents, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &StartSearchHook);

	// Call the AppMsgHook when an icon is dropped on sources listview
	DoMethod(NListviewSources, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
		 NListviewSources, 3, MUIM_CallHook, &AppMessageHook, MUIV_TriggerValue);

	// Call OpenResultHook when an result entry is double-clicked
	DoMethod(NListResults, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
		APP_Main, 3, MUIM_CallHook, &OpenResultHook, MUIV_TriggerValue);

	// Call HideFindHook when Find Filetype Hide button is clicked
	DoMethod(ButtonHideFind, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 2, MUIM_CallHook, &HideFindHook);

	// Call PatternHistoryHook when an entry in pattern history is double-clicked
	DoMethod(NListPatternHistory, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
		APP_Main, 3, MUIM_CallHook, &PatternHistoryHook, MUIV_TriggerValue);

	// Call ContentsHistoryHook when an entry in contents history is double-clicked
	DoMethod(NListContentsHistory, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
		APP_Main, 3, MUIM_CallHook, &ContentsHistoryHook, MUIV_TriggerValue);

	// Call FiletypeSelectHook when a filetype is double-clicked
	DoMethod(ListtreeFileTypes, MUIM_Notify, MUIA_NListtree_DoubleClick, MUIV_EveryTime,
		APP_Main, 3, MUIM_CallHook, &FiletypeSelectHook, MUIV_TriggerValue);

	// on every contents change, check all 3 search string gadgets,
	// and disable "start search" button if all strings are empty
	DoMethod(StringFilePattern, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &CheckEmptyStringsHook);
	DoMethod(StringFileContents, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &CheckEmptyStringsHook);
	DoMethod(StringFileType, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &CheckEmptyStringsHook);

	// Call FindFileTypeHook everytime the contents of StringFindFileType changes
	DoMethod(StringFindFileType, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &FindFileTypeHook);

	// Call FindNextFileTypeHook when "Next" button is clicked
	DoMethod(ButtonFindNextFileType, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 2, MUIM_CallHook, &FindNextFileTypeHook);

	// call hook everytime a new filetype is selected
	DoMethod(ListtreeFileTypes, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
		ListtreeFileTypes, 3, MUIM_CallHook, &SelectFileTypesEntryHook, MUIV_TriggerValue );

	// setup sorting hooks for plugin list
	DoMethod(NListResults, MUIM_Notify, MUIA_NList_TitleClick, MUIV_EveryTime,
		NListResults, 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
	DoMethod(NListResults, MUIM_Notify, MUIA_NList_TitleClick2, MUIV_EveryTime,
		NListResults, 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
	DoMethod(NListResults, MUIM_Notify, MUIA_NList_SortType, MUIV_EveryTime,
		NListResults, 3, MUIM_Set, MUIA_NList_TitleMark, MUIV_TriggerValue);
	DoMethod(NListResults, MUIM_Notify, MUIA_NList_SortType2, MUIV_EveryTime,
		NListResults, 3, MUIM_Set, MUIA_NList_TitleMark2, MUIV_TriggerValue);

	//--------------------------------------------------------------------------//

	DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &AboutHook);
	DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &AboutMUIHook);
	DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
		APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	FillSourcesList(WBenchMsg);

	set(WIN_Main, MUIA_Window_Open, TRUE);
	get(WIN_Main, MUIA_Window_Open, &win_opened);

	if (win_opened)
		{
		ULONG sigs = 0;
		BOOL Run = TRUE;

		while (Run)
			{
			ULONG Action = DoMethod(APP_Main, MUIM_Application_NewInput, &sigs);

			switch (Action)
				{
			case MUIV_Application_ReturnID_Quit:
				Run = FALSE;
				break;
			default:
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

	set(WIN_Main, MUIA_Window_Open, FALSE);

	DoMethod(APP_Main, MUIM_Application_Save, MUIV_Application_Save_ENV);
	DoMethod(APP_Main, MUIM_Application_Save, MUIV_Application_Save_ENVARC);

	fail(APP_Main, NULL);

	return 0;
}

//----------------------------------------------------------------------------

static VOID fail(APTR APP_Main, CONST_STRPTR str)
{
	CleanupDefIcons();

	if (APP_Main)
		{
		MUI_DisposeObject(APP_Main);
		}
	if (ContextMenuFileTypes)
		{
		MUI_DisposeObject(ContextMenuFileTypes);
		ContextMenuFileTypes = NULL;
		}
	if (ContextMenuPatternPopup)
		{
		MUI_DisposeObject(ContextMenuPatternPopup);
		ContextMenuPatternPopup	= NULL;
		}
	if (ContextMenuContentsPopup)
		{
		MUI_DisposeObject(ContextMenuContentsPopup);
		ContextMenuContentsPopup = NULL;
		}
	if (ContextMenuResults)
		{
		MUI_DisposeObject(ContextMenuResults);
		ContextMenuResults = NULL;
		}
	if (FindCatalog)
		{
		CloseCatalog(FindCatalog);
		FindCatalog = NULL;
		}
	if (myPopObjectClass)
		{
		MUI_DeleteCustomClass(myPopObjectClass);
		myPopObjectClass = 0;
		}
	if (myPersistentNListClass)
		{
		MUI_DeleteCustomClass(myPersistentNListClass);
		myPersistentNListClass = NULL;
		}
	if (myFindResultsNListClass)
		{
		MUI_DeleteCustomClass(myFindResultsNListClass);
		myFindResultsNListClass	= NULL;
		}
	if (myFileTypesNListTreeClass)
		{
		MUI_DeleteCustomClass(myFileTypesNListTreeClass);
		myFileTypesNListTreeClass = NULL;
		}

	CloseLibraries();

	if (str)
		{
		puts(str);
		exit(20);
		}

	exit(0);
}

//----------------------------------------------------------------------------

DISPATCHER(myPopObject)
{
	ULONG Result;
	Object *opop = NULL;

	d1(kprintf(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_Export:
		d1(KPrintF("%s/%ld:  MUIM_Export\n", __FUNC__, __LINE__));

		get(obj, MUIA_Popobject_Object, &opop);
		if (opop)
			DoMethodA(opop, msg);

		Result = DoSuperMethodA(cl, obj, msg);
		break;

	case MUIM_Import:
		d1(KPrintF("%s/%ld:  MUIM_Import\n", __FUNC__, __LINE__));

		get(obj, MUIA_Popobject_Object, &opop);
		if (opop)
			DoMethodA(opop, msg);

		Result = DoSuperMethodA(cl, obj, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(myPersistentNList)
{
	ULONG Result;

	d1(KPrintF(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_Export:
		d1(KPrintF("%s/%ld:  MUIM_Export id=%08lx\n", __FUNC__, __LINE__, muiNotifyData(obj)->mnd_ObjectID));
		Result = DoSuperMethodA(cl, obj, msg);
		ExportNList(cl, obj, (struct MUIP_Export *) msg);
		d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
		break;

	case MUIM_Import:
		d1(KPrintF("%s/%ld:  MUIM_Import id=%08lx\n", __FUNC__, __LINE__, muiNotifyData(obj)->mnd_ObjectID));
		//Result = DoSuperMethodA(cl, obj, msg);
		ImportNList(cl, obj, (struct MUIP_Import *) msg);
		Result = 0;
		d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
		break;

	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		struct Hook *MenuHook = NULL;

		d1(kprintf("%s/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf("%s/%ld: MenuObj=%08lx\n", __FUNC__, __LINE__, MenuObj));
		d1(kprintf("%s/%ld: msg=%08lx *msg=%08lx\n", __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHook);

		d1(kprintf("%s/%ld: MenuHook=%08lx\n", __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(obj, MUIM_CallHook, MenuHook, 0);

		Result = 0;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		}

	d1(KPrintF(__FILE__ "/%s/%ld: MethodID=%08lx  Result=%ld\n", __FUNC__, __LINE__, msg->MethodID, Result));

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(myFindResultsNList)
{
	ULONG Result;

	d1(kprintf(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_Export:
		d1(KPrintF("%s/%ld:  MUIM_Export\n", __FUNC__, __LINE__));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
	case MUIM_Import:
		d1(KPrintF("%s/%ld:  MUIM_Import\n", __FUNC__, __LINE__));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		struct Hook *MenuHook = NULL;

		d1(KPrintF("%s/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(KPrintF("%s/%ld: MenuObj=%08lx\n", __FUNC__, __LINE__, MenuObj));
		d1(KPrintF("%s/%ld: msg=%08lx *msg=%08lx\n", __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHook);

		d1(KPrintF("%s/%ld: MenuHook=%08lx\n", __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(APP_Main, MUIM_CallHook, MenuHook, MenuObj);

		Result = 0;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(myFileTypesNListTree)
{
	ULONG Result;

	d1(kprintf(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_Export:
		d1(KPrintF("%s/%ld:  MUIM_Export\n", __FUNC__, __LINE__));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
	case MUIM_Import:
		d1(KPrintF("%s/%ld:  MUIM_Import\n", __FUNC__, __LINE__));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		struct Hook *MenuHook = NULL;

		d1(kprintf("%s/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf("%s/%ld: MenuObj=%08lx\n", __FUNC__, __LINE__, MenuObj));
		d1(kprintf("%s/%ld: msg=%08lx *msg=%08lx\n", __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHook);


		d1(kprintf("%s/%ld: MenuHook=%08lx\n", __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(APP_Main, MUIM_CallHook, MenuHook, 0);

		Result = 0;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static void init(void)
{
	if (!OpenLibraries())
		fail(NULL, "Failed to call OpenLibraries");

	if (LocaleBase)
		FindCatalog = OpenCatalogA(NULL, "Scalos/Find.catalog", NULL);

	TranslateNewMenu(NewContextMenuResults);
	TranslateNewMenu(NewContextMenuFileTypes);
	TranslateNewMenu(NewContextMenuHistoryPopup);

	myFindResultsNListClass	= MUI_CreateCustomClass(NULL, MUIC_NList,
		NULL, 0, DISPATCHER_REF(myFindResultsNList));

	myFileTypesNListTreeClass = MUI_CreateCustomClass(NULL, MUIC_NListtree,
		NULL, 0, DISPATCHER_REF(myFileTypesNListTree));

	myPersistentNListClass = MUI_CreateCustomClass(NULL, MUIC_NList,
		NULL, 0, DISPATCHER_REF(myPersistentNList));

	myPopObjectClass = MUI_CreateCustomClass(NULL, MUIC_Popobject,
		NULL, 0, DISPATCHER_REF(myPopObject));
}

//----------------------------------------------------------------------------

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
#endif

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
#endif

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
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		{
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
		if (NULL == ILocale)
			fail(NULL, "Failed to open locale interface.");
		}
#endif

	TimerPort = CreateMsgPort();
	TimerIO = (T_TIMEREQUEST *)CreateIORequest(TimerPort, sizeof(T_TIMEREQUEST));
	if (NULL == TimerIO)
		fail(NULL, "Failed to call CreateIORequest.");

	OpenDevice("timer.device", UNIT_VBLANK, &TimerIO->tr_node, 0);
	TimerBase = (T_TIMERBASE) TimerIO->tr_node.io_Device;
	if (NULL == TimerBase)
		fail(NULL, "Failed to open timer.device.");
#ifdef __amigaos4__
	ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
	if (NULL == ITimer)
		fail(NULL, "Failed to open timer interface.");
#endif

	return TRUE;
}

//----------------------------------------------------------------------------

static void CloseLibraries(void)
{
	if (TimerIO)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ITimer);
#endif
		CloseDevice(&TimerIO->tr_node);
		DeleteIORequest(&TimerIO->tr_node);

		TimerIO	= NULL;
		TimerBase = NULL;
		}
	if (TimerPort)
		{
		DeleteMsgPort(TimerPort);
		TimerPort = NULL;
		}
	if (LocaleBase)
		{
		if (FindCatalog)
			{
			CloseCatalog(FindCatalog);
			FindCatalog = NULL;
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

static STRPTR GetLocString(ULONG MsgId)
{
	struct Find_LocaleInfo li;

	li.li_Catalog = FindCatalog;  
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetFindString(&li, MsgId);
}

//----------------------------------------------------------------------------

#if 0
static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}
#endif

//----------------------------------------------------------------------------

static void TranslateNewMenu(struct NewMenu *nm)
{
	while (nm && NM_END != nm->nm_Type)
		{
		if (NM_BARLABEL != nm->nm_Label)
			nm->nm_Label = GetLocString((ULONG) nm->nm_Label);

		if (nm->nm_CommKey)
			nm->nm_CommKey = GetLocString((ULONG) nm->nm_CommKey);

		nm++;
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
			TAG_DONE);
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

static SAVEDS(void) INTERRUPT IntuiMessageHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct IntuiMessage *imsg = (struct IntuiMessage *) msg;

	if (IDCMP_REFRESHWINDOW == imsg->Class)
		DoMethod(APP_Main, MUIM_Application_CheckRefresh);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT AppMessageHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct AppMessage *AppMsg = *(struct AppMessage **) msg;
	ULONG n;

	d1(KPrintF("%s/%s/%ld: AppMsg=%08lx  am_NumArgs=%ld\n", __FILE__, __FUNC__,, __LINE__, AppMsg, AppMsg->am_NumArgs));

	for (n = 0; n < AppMsg->am_NumArgs; n++)
		{
		struct WBArg *arg = &AppMsg->am_ArgList[n];
		char Path[512];

		NameFromLock(arg->wa_Lock, Path, sizeof(Path));
		AddPart(Path, arg->wa_Name, sizeof(Path));

		d1(KPrintF("%s/%s/%ld: wa_Lock=%08lx\n", __FILE__, __FUNC__,, __LINE__, arg->wa_Lock));

		AddSourcesLine(Path);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT SelectSourceHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	STRPTR SourceEntry = NULL;

	DoMethod(NListSources, MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active,
		&SourceEntry);

	// Enable "remove" button only if any entry is selected
	set(ButtonRemoveSource, MUIA_Disabled, NULL == SourceEntry);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT RemoveSourceHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	LONG pos;

	get(NListSources, MUIA_NList_Active, &pos);

	d1(KPrintF( "%s/%s/%ld: pos=%ld\n", __FILE__, __FUNC__, __LINE__, pos));
	if (MUIV_NList_Active_Off != pos)
		{
		DoMethod(NListSources, MUIM_NList_Remove, pos);

		set(NListSources, MUIA_NList_Active, MUIV_NList_Active_Off);
		set(ButtonRemoveSource, MUIA_Disabled, TRUE);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT AddSourceHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	STRPTR NewSource = NULL;

	get(StringNewSource, MUIA_String_Contents, &NewSource);
	if (NewSource && strlen(NewSource) > 0)
		{
		AddSourcesLine(NewSource);
		setstring(StringNewSource, "");
		}
	else
		{
		DoMethod(PopAslNewSource, MUIM_Popstring_Open);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ResultsConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm)
{
	struct ResultsListEntry *rle = AllocPooled(nlcm->pool, sizeof(struct ResultsListEntry));
	BOOL Success = FALSE;

	(void) unused;

	do	{
		struct FindResult *newFr = (struct FindResult *) nlcm->entry;
		STRPTR Path;
		size_t Length = 1024;

		if (NULL == rle)
			break;

		rle->rle_CopyString = NULL;

		rle->rle_Name = strdup(newFr->fr_Name);
		if (NULL == rle->rle_Name)
			break;

		Path = malloc(Length);
		if (NULL == Path)
			break;

		if (!NameFromLock(newFr->fr_Lock, Path, Length))
			break;

		rle->rle_Path = strdup(Path);
		if (NULL == rle->rle_Path)
			break;

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		if (rle)
			{
			if (rle->rle_Name)
				free(rle->rle_Name);
			if (rle->rle_Path)
				free(rle->rle_Path);
			if (rle->rle_CopyString)
				free(rle->rle_CopyString);
			FreePooled(nlcm->pool, rle, sizeof(struct ResultsListEntry));
			rle = NULL;
			}
		}

	return (APTR) rle;
}


static SAVEDS(void) INTERRUPT ResultsDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm)
{
	struct ResultsListEntry *rle = (struct ResultsListEntry *) nldm->entry;

	(void) unused;

	if (rle->rle_Name)
		free(rle->rle_Name);
	if (rle->rle_Path)
		free(rle->rle_Path);
	if (rle->rle_CopyString)
		free(rle->rle_CopyString);

	FreePooled(nldm->pool, rle, sizeof(struct ResultsListEntry));
}


static SAVEDS(ULONG) INTERRUPT ResultsDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm)
{
	(void) unused;

	if (nldm->entry)
		{
		struct ResultsListEntry *rle = (struct ResultsListEntry *) nldm->entry;

		nldm->strings[0] = rle->rle_Name;
		nldm->strings[1] = rle->rle_Path;
		}
	else
		{
		// display titles
		nldm->strings[0] = GetLocString(MSGID_RESULTLIST_NAME);
		nldm->strings[1] = GetLocString(MSGID_RESULTLIST_LOCATION);
		}

	return 0;
}


static SAVEDS(LONG) INTERRUPT ResultsCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm)
{
	const struct ResultsListEntry *rle1 = (const struct ResultsListEntry *) ncm->entry1;
	const struct ResultsListEntry *rle2 = (const struct ResultsListEntry *) ncm->entry2;
	LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
	LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
	LONG Result = 0;

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		// primary sorting
		switch (col1)
			{
		case 0:		// sort by name
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(rle2->rle_Name, rle1->rle_Name);
			else
				Result = Stricmp(rle1->rle_Name, rle2->rle_Name);
			break;
		case 1:		// sort by path
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(rle2->rle_Path, rle1->rle_Path);
			else
				Result = Stricmp(rle1->rle_Path, rle2->rle_Path);
			break;
		default:
			break;
			}

		if (0 == Result && col1 != col2)
			{
			// Secondary sorting
			switch (col2)
				{
			case 0:		// sort by name
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = Stricmp(rle2->rle_Name, rle1->rle_Name);
				else
					Result = Stricmp(rle1->rle_Name, rle2->rle_Name);
				break;
			case 1:		// sort by path
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = Stricmp(rle2->rle_Path, rle1->rle_Path);
				else
					Result = Stricmp(rle1->rle_Path, rle2->rle_Path);
				break;
			default:
				break;
				}
			}
		}

	return Result;
}


static SAVEDS(LONG) INTERRUPT ResultsCopyEntryToClipHookFunc(struct Hook *hook, Object *obj, struct NList_CopyEntryToClipMessage *ncc)
{
	struct ResultsListEntry *rle = (struct ResultsListEntry *) ncc->entry;
	LONG Result = 0;
	size_t len;

	len = strlen(rle->rle_Path)
		+ 1		// room for separator
		+ strlen(rle->rle_Name)
		+ 2;		// room for "\n" and trailing 0.

	rle->rle_CopyString = realloc(rle->rle_CopyString, len);
	if (rle->rle_CopyString)
		{
		strcpy(rle->rle_CopyString, rle->rle_Name);
		strcat(rle->rle_CopyString, "\t");
		strcat(rle->rle_CopyString, rle->rle_Path);
		strcat(rle->rle_CopyString, "\n");

		ncc->str_result = rle->rle_CopyString;
		}

	return Result;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT StartSearchHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	(void) hook;
	(void) o;
	(void) msg;

	StartSearch();

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT StopSearchHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	(void) hook;
	(void) o;
	(void) msg;

	StopSearch = TRUE;

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(VOID) INTERRUPT OpenResultHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	(void) hook;
	(void) o;
	(void) msg;

	do	{
		struct ResultsListEntry *rle = NULL;

		DoMethod(NListResults, MUIM_NList_GetEntry,
			MUIV_NList_GetEntry_Active, &rle);

		if (NULL == rle)
			break;

		// Open result drawer in Scalos
		SCA_OpenDrawerByNameTags(rle->rle_Path,
			SCA_ShowAllMode, DDFLAGS_SHOWALL,
			TAG_END);
                } while (0);
}


static SAVEDS(VOID) INTERRUPT CopyResultsToClipHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	(void) hook;
	(void) o;
	(void) msg;

	DoMethod(NListResults, MUIM_NList_CopyToClip,
		MUIV_NList_CopyToClip_All,
		0,	// clipboard number to copy to
		NULL,
		NULL );
}


static SAVEDS(VOID) INTERRUPT OpenResultWithFindHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct WBArg wbArg[2] = { { (BPTR)NULL }, { (BPTR)NULL } };
	BOOL Success = FALSE;

	(void) hook;
	(void) o;
	(void) msg;

	do	{
		struct ResultsListEntry *rle = NULL;

		if (NULL == WBenchMsg)
			break;

		DoMethod(NListResults, MUIM_NList_GetEntry,
			MUIV_NList_GetEntry_Active, &rle);

		if (NULL == rle)
			break;

		wbArg[1].wa_Name = rle->rle_Name;
		wbArg[1].wa_Lock = Lock(rle->rle_Path, ACCESS_READ);
		if ((BPTR)NULL == wbArg[1].wa_Lock)
			break;;

		wbArg[0].wa_Name = WBenchMsg->sm_ArgList[0].wa_Name;
		wbArg[0].wa_Lock = DupLock(WBenchMsg->sm_ArgList[0].wa_Lock);
		if ((BPTR) NULL == wbArg[0].wa_Lock)
			break;

		// SCA_WBStart()
		Success = SCA_WBStartTags(wbArg, 2,
				SCA_Flags, SCAF_WBStart_Wait,
				SCA_WaitTimeout, 0,
				TAG_END);
                } while (0);

	if (!Success)
		{
		if (wbArg[0].wa_Lock)
			UnLock(wbArg[0].wa_Lock);
		if (wbArg[1].wa_Lock)
			UnLock(wbArg[1].wa_Lock);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(VOID) INTERRUPT OpenResultWithMultiviewHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct WBArg wbArg[2] = { { (BPTR)NULL }, { (BPTR)NULL } };
	BOOL Success = FALSE;

	(void) hook;
	(void) o;
	(void) msg;

	do	{
		struct ResultsListEntry *rle = NULL;

		DoMethod(NListResults, MUIM_NList_GetEntry,
			MUIV_NList_GetEntry_Active, &rle);

		if (NULL == rle)
			break;

		wbArg[1].wa_Name = rle->rle_Name;
		wbArg[1].wa_Lock = Lock(rle->rle_Path, ACCESS_READ);
		if ((BPTR)NULL == wbArg[1].wa_Lock)
			break;;

		wbArg[0].wa_Name = (STRPTR) "Multiview";
		wbArg[0].wa_Lock = Lock("SYS:Utilities", ACCESS_READ);
		if ((BPTR) NULL == wbArg[0].wa_Lock)
			break;

		// SCA_WBStart()
		Success = SCA_WBStartTags(wbArg, 2,
				SCA_Flags, SCAF_WBStart_Wait,
				SCA_WaitTimeout, 0,
				TAG_END);
                } while (0);

	if (!Success)
		{
		if (wbArg[0].wa_Lock)
			UnLock(wbArg[0].wa_Lock);
		if (wbArg[1].wa_Lock)
			UnLock(wbArg[1].wa_Lock);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT FileTypesConstructHookFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_ConstructMessage *ltcm)
{
	struct FileTypesEntry *fte;
	BOOL Success = FALSE;

	d1(kprintf(__FILE__ "/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FUNC__, __LINE__, obj, ltcm, ltcm->MemPool, ltcm->UserData));

	do	{
		const struct TypeNode *TnOrig = ltcm->UserData;

		fte = AllocPooled(ltcm->MemPool, sizeof(struct FileTypesEntry));
		d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));
		if (NULL == fte)
			break;

		NewList(&fte->fte_MagicList);
		fte->fte_TypeNode = *TnOrig;

		fte->fte_AllocatedName = fte->fte_TypeNode.tn_Name = strdup(TnOrig->tn_Name);
		if (NULL == fte->fte_AllocatedName)
			break;

		Success = TRUE;
		} while (0);

	if (!Success && fte)
		{
		if (fte->fte_AllocatedName)
			{
			free(fte->fte_AllocatedName);
			fte->fte_AllocatedName = NULL;
			}
		FreePooled(ltcm->MemPool, fte, sizeof(struct FileTypesEntry));
		fte = NULL;
		}

	return fte;
}

static SAVEDS(void) INTERRUPT FileTypesDestructHookFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DestructMessage *ltdm)
{
	struct FileTypesEntry *fte = (struct FileTypesEntry *) ltdm->UserData;

	d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));

	if (fte->fte_AllocatedName)
		free(fte->fte_AllocatedName);

	FreePooled(ltdm->MemPool, fte, sizeof(struct FileTypesEntry));
	ltdm->UserData = NULL;
}

static SAVEDS(ULONG) INTERRUPT FileTypesDisplayHookFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DisplayMessage *ltdm)
{
	struct FileTypesEntry *fte = (struct FileTypesEntry *) ltdm->TreeNode->tn_User;

	d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));

	if (fte)
		{
		ltdm->Array[0] = fte->fte_TypeNode.tn_Name;
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT HideFindHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	LONG isVisible;

	(void) hook;
	(void) o;
	(void) msg;

	get(GroupFindFiletype, MUIA_ShowMe, &isVisible);
	set(GroupFindFiletype, MUIA_ShowMe, !isVisible);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT PatternHistoryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	LONG *pPos = (LONG *) msg;

	(void) hook;
	(void) o;

	if (*pPos >= 0)
		{
		STRPTR entry;

		DoMethod(NListPatternHistory, MUIM_NList_GetEntry, *pPos, &entry);
		d1(KPrintF(__FILE__ "/%s/%ld: *pPos=%ld  entry=%08lx\n", __FUNC__, __LINE__, *pPos, entry));
		if (entry)
			setstring(StringFilePattern, entry);

		DoMethod(PopObjectPattern, MUIM_Popstring_Close, 1);
		}

	return 0;
}
//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT ContentsHistoryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	LONG *pPos = (LONG *) msg;

	(void) hook;
	(void) o;

	if (*pPos >= 0)
		{
		STRPTR entry;

		DoMethod(NListContentsHistory, MUIM_NList_GetEntry, *pPos, &entry);
		d1(KPrintF(__FILE__ "/%s/%ld: *pPos=%ld  entry=%08lx\n", __FUNC__, __LINE__, *pPos, entry));
		if (entry)
			setstring(StringFileContents, entry);

		DoMethod(PopObjectContents, MUIM_Popstring_Close, 1);
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT FiletypeSelectHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MUI_NListtree_TreeNode **tn = (struct MUI_NListtree_TreeNode **) msg;

	(void) hook;
	(void) o;

	d1(KPrintF(__FILE__ "/%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, *tn));
	d1(KPrintF(__FILE__ "/%s/%ld: tn_Name=<%s>\n", __FUNC__, __LINE__, (*tn)->tn_Name));

	setstring(StringFileType, (*tn)->tn_Name);

	DoMethod(PopObjectFiletypes, MUIM_Popstring_Close, 1);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT CheckEmptyStringsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	CONST_STRPTR FilePattern = NULL;
	CONST_STRPTR FileContents = NULL;
	CONST_STRPTR FileType = NULL;
	size_t TotalLength = 0;

	(void) hook;
	(void) o;
	(void) msg;

	get(StringFilePattern, MUIA_String_Contents, &FilePattern);
	get(StringFileContents, MUIA_String_Contents, &FileContents);
	get(StringFileType, MUIA_String_Contents, &FileType);

	if (FilePattern)
		TotalLength += strlen(FilePattern);
	if (FileContents)
		TotalLength += strlen(FileContents);
	if (FileType)
		TotalLength += strlen(FileType);

	set(ButtonSearch, MUIA_Disabled, 0 == TotalLength);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT FindFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	CONST_STRPTR FindString = NULL;

	(void) hook;
	(void) o;
	(void) msg;

	get(StringFindFileType, MUIA_String_Contents, &FindString);

	if (FindString && strlen(FindString) > 0)
		{
#if 1
		SearchFileType(FindString, FALSE);
#else
		DoMethod(ListtreeFileTypes, MUIM_NListtree_FindName,
			MUIV_NListtree_FindName_ListNode_Root,
			FindString, MUIV_NListtree_FindName_Flag_Activate);
#endif
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT FindNextFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	CONST_STRPTR FindString = NULL;

	(void) hook;
	(void) o;
	(void) msg;

	get(StringFindFileType, MUIA_String_Contents, &FindString);

	if (FindString && strlen(FindString) > 0)
		{
#if 1
		SearchFileType(FindString, TRUE);
#else
		struct MUI_NListtree_TreeNode *tn = NULL;

		tn = (struct MUI_NListtree_TreeNode *) DoMethod(ListtreeFileTypes,
			MUIM_NListtree_GetEntry,
			MUIV_NListtree_GetEntry_ListNode_Active,
			MUIV_NListtree_GetEntry_Position_Active,
			0);
		d1(KPrintF("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

		if (tn)
			{
			tn = (struct MUI_NListtree_TreeNode *) DoMethod(ListtreeFileTypes,
				MUIM_NListtree_GetEntry,
				tn,
				MUIV_NListtree_GetEntry_Position_Next,
				0);
			d1(KPrintF("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));
			}
		if (NULL == tn)
			tn = (struct MUI_NListtree_TreeNode *) MUIV_NListtree_FindName_ListNode_Root;

		DoMethod(ListtreeFileTypes,
			MUIM_NListtree_FindName,
			MUIV_NListtree_FindName_ListNode_Active,
			FindString,
			MUIV_NListtree_FindName_Flag_StartNode | MUIV_NListtree_FindName_Flag_Activate);
#endif
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CollapseFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	(void) hook;
	(void) o;
	(void) msg;

	DoMethod(ListtreeFileTypes,
		MUIM_NListtree_Close,
		MUIV_NListtree_Close_ListNode_Active,
		MUIV_NListtree_Close_TreeNode_All,
		0);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT ExpandFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MUI_NListtree_TreeNode *tn;

	(void) hook;
	(void) o;
	(void) msg;

	set(ListtreeFileTypes, MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(ListtreeFileTypes,
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	DoMethod(ListtreeFileTypes,
		MUIM_NListtree_Open,
		MUIV_NListtree_Open_ListNode_Active,
		MUIV_NListtree_Open_TreeNode_Active,
		0);

	if (tn)
		{
		struct MUI_NListtree_TreeNode *tnChild;

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(ListtreeFileTypes,
			MUIM_NListtree_GetEntry,
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tnChild, tnChild ? tnChild->tn_Name : (STRPTR) ""));

		if (tnChild)
			{
			set(ListtreeFileTypes, MUIA_NListtree_Active, (ULONG) tnChild);

			DoMethod(ListtreeFileTypes,
				MUIM_NListtree_Open,
				MUIV_NListtree_Open_ListNode_Active,
				MUIV_NListtree_Open_TreeNode_All,
				0);
			}

		set(ListtreeFileTypes, MUIA_NListtree_Active, (ULONG) tn);
		}

	set(ListtreeFileTypes, MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT CollapseAllFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	(void) hook;
	(void) o;
	(void) msg;


	DoMethod(ListtreeFileTypes, MUIM_NListtree_Close,
		MUIV_NListtree_Close_ListNode_Root,
		MUIV_NListtree_Close_TreeNode_All,
		0);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT ExpandAllFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	(void) hook;
	(void) o;
	(void) msg;


	DoMethod(ListtreeFileTypes, MUIM_NListtree_Open,
		MUIV_NListtree_Open_ListNode_Root,
		MUIV_NListtree_Open_TreeNode_All,
		0);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT SelectFileTypesEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MUI_NListtree_TreeNode **tn = (struct MUI_NListtree_TreeNode **) msg;

	if (*tn)
		{
		set(MenuExpandFileTypes, MUIA_Menuitem_Enabled, TRUE);
		set(MenuCollapseFileTypes, MUIA_Menuitem_Enabled, TRUE);
		}
	else
		{
		set(MenuExpandFileTypes, MUIA_Menuitem_Enabled, FALSE);
		set(MenuCollapseFileTypes, MUIA_Menuitem_Enabled, FALSE);
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(VOID) INTERRUPT FileTypesPopupWindowHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	Object *PopWindow = (Object *) msg;

	set(PopWindow, MUIA_Window_ActiveObject, StringFindFileType);
}

//----------------------------------------------------------------------------

static SAVEDS(VOID) INTERRUPT ClearHistoryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	DoMethod(o, MUIM_NList_Clear, 0);
}

//----------------------------------------------------------------------------

static void StartSearch(void)
{
	static ULONG Searching = 0;
	ULONG Entries, n;
	CONST_STRPTR FilePattern = NULL;
	CONST_STRPTR FileContents = NULL;
	CONST_STRPTR FileType = NULL;
	STRPTR PatternBuf = NULL;
	STRPTR ContentsBuf = NULL;
	STRPTR FiletypeBuf = NULL;

	d1(KPrintF("%s/%s/%ld: START  Searching=%lu\n", __FILE__, __FUNC__, __LINE__, Searching));

	do	{
		if (++Searching > 1)
			break;

		set(ButtonSearch, MUIA_Disabled, TRUE);
		set(ButtonStop, MUIA_Disabled, FALSE);
		set(StringFilePattern, MUIA_Disabled, TRUE);
		set(StringFileContents, MUIA_Disabled, TRUE);
		set(StringFileType, MUIA_Disabled, TRUE);

		// Clear search results
		DoMethod(NListResults, MUIM_NList_Clear);

		get(StringFilePattern, MUIA_String_Contents, &FilePattern);
		get(StringFileContents, MUIA_String_Contents, &FileContents);
		get(StringFileType, MUIA_String_Contents, &FileType);

		if (FilePattern && strlen(FilePattern) > 0)
			{
			// Need to match file name
			size_t len = 2 * strlen(FilePattern) + 10;

			PushEntry(NListPatternHistory, FilePattern, MUIV_NList_Insert_Top);

			PatternBuf = malloc(len);
			if (NULL == PatternBuf)
				break;

			if (ParsePatternNoCase(FilePattern, PatternBuf, len) < 0)
				break;
			}
		if (FileContents && strlen(FileContents) > 0)
			{
			// Need to match contents
			PushEntry(NListContentsHistory, FileContents, MUIV_NList_Insert_Top);

			ContentsBuf = strdup(FileContents);
			if (NULL == ContentsBuf)
				break;

			strlwr(ContentsBuf);
			}
		if (FileType && strlen(FileType) > 0)
			{
			FiletypeBuf = strdup(FileType);
			if (NULL == FiletypeBuf)
				break;
			}

		get(NListSources, MUIA_NList_Entries, &Entries);

		for (n = 0; !StopSearch && n < Entries; n++)
			{
			STRPTR SearchPath;

			DoMethod(NListSources, MUIM_NList_GetEntry,
				n, &SearchPath);

			DoSearchPath(SearchPath, PatternBuf, ContentsBuf, FiletypeBuf);

			DoMethod(APP_Main, MUIM_Application_InputBuffered);
			}
		} while (0);

	if (PatternBuf)
		free(PatternBuf);
	if (ContentsBuf)
		free(ContentsBuf);
	if (FiletypeBuf)
		free(FiletypeBuf);

	if (0 == --Searching)
		{
		setstring(StringFilePattern, SEARCH_PATTERN_DEFAULT);
		setstring(StringFileContents, SEARCH_CONTENTS_DEFAULT);
		setstring(StringFileType, SEARCH_FILETYPE_DEFAULT);

		set(TextCurrentFile, MUIA_Text_Contents,
			GetLocString(StopSearch ? MSGID_SEARCH_ABORTED : MSGID_SEARCH_FINISHED));
		set(ButtonStop, MUIA_Disabled, TRUE);
		set(StringFilePattern, MUIA_Disabled, FALSE);
		set(StringFileContents, MUIA_Disabled, FALSE);
		set(StringFileType, MUIA_Disabled, FALSE);

		StopSearch = FALSE;
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static void DoSearchPath(CONST_STRPTR Path, CONST_STRPTR Pattern,
	CONST_STRPTR Contents, CONST_STRPTR Filetype)
{
	BPTR DirLock;
	size_t PathLength = strlen(CurrentPath);

	d1(KPrintF("%s/%s/%ld: START  Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

	if (Pattern)
		{
		// Match file name pattern
		if (NULL == Contents && NULL == Filetype
			&& MatchPatternNoCase(Pattern, (STRPTR) Path))
			{
			struct FindResult fr;

			d1(KPrintF("%s/%s/%ld: Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

			fr.fr_Lock = CurrentDir((BPTR)NULL);
			CurrentDir(fr.fr_Lock);
			fr.fr_Name = (STRPTR) Path;

			DoMethod(NListResults, MUIM_NList_InsertSingle,
				&fr, MUIV_NList_Insert_Sorted);
			}
		}

	d1(KPrintF("%s/%s/%ld: PathLength=%lu  CurrentPath=<%s>  Path=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, PathLength, CurrentPath, Path));

	if (PathLength)
		AddPart(CurrentPath, Path, sizeof(CurrentPath));
	else
		strcpy(CurrentPath, Path);

	d1(KPrintF("%s/%s/%ld: CurrentPath=<%s>\n", __FILE__, __FUNC__, __LINE__, CurrentPath));

	DirLock = Lock(Path, ACCESS_READ);
	if (DirLock)
		{
		DoSearchDir(DirLock, Pattern, Contents, Filetype);

		UnLock(DirLock);
		}

	CurrentPath[PathLength] = '\0';

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static void DoSearchDir(BPTR DirLock, CONST_STRPTR Pattern,
	CONST_STRPTR Contents, CONST_STRPTR Filetype)
{
	BPTR OldCurrentDir = CurrentDir(DirLock);
#if USE_EXALL
	const size_t eadSize = 32768;
	struct ExAllControl *eac = NULL;
	struct ExAllData *EAData;
	BOOL More = FALSE;
#else /* USE_EXALL */
	struct FileInfoBlock *fib = NULL;
#endif /* USE_EXALL */

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
#if USE_EXALL
		EAData = malloc(eadSize);
		if (NULL == EAData)
			break;

		eac = AllocDosObject(DOS_EXALLCONTROL, NULL);
		if (NULL == eac)
			break;

		eac->eac_MatchString = NULL;
		eac->eac_MatchFunc = NULL;
		eac->eac_LastKey = 0;
		More = TRUE;

		while (!StopSearch && More)
			{
			struct ExAllData *ead;

			DoMethod(APP_Main, MUIM_Application_InputBuffered);

			More = ExAll(DirLock, EAData, eadSize, ED_TYPE, eac);
			if (!More && (IoErr() != ERROR_NO_MORE_ENTRIES))
				break;

			if (0 == eac->eac_Entries)
				continue;

			for (ead = EAData; ead; ead = ead->ed_Next)
				{
				if (GetElapsedTime(&LastUpdate) > 100)
					{
					// Update at most every 100ms
					char FullName[512];

					strcpy(FullName, CurrentPath);
					AddPart(FullName, ead->ed_Name, sizeof(FullName));
					set(TextCurrentFile, MUIA_Text_Contents, FullName);

					GetSysTime(&LastUpdate);
					}

				SearchEntry(DirLock, ead->ed_Name, ead->ed_Type, Pattern, Contents, Filetype);

				DoMethod(APP_Main, MUIM_Application_InputBuffered);
				}
			}
#else /* USE_EXALL */
		fib = AllocDosObject(DOS_FIB, NULL);
		if (NULL == fib)
			break;

		if (!Examine(DirLock, fib))
			break;

		d1(KPrintF("%s/%s/%ld: fib_fileName=<%s>\n", __FILE__, __FUNC__, __LINE__, fib->fib_FileName));

		while (!StopSearch)
			{
			if (GetElapsedTime(&LastUpdate) > 100)
				{
				// Update at most every 100ms
				char FullName[512];

				strcpy(FullName, Path);
				AddPart(FullName, fib->fib_FileName, sizeof(FullName));
				set(TextCurrentFile, MUIA_Text_Contents, FullName);

				GetSysTime(&LastUpdate);
				}

			if (!ExNext(DirLock, fib))
				break;

			SearchEntry(DirLock, fib->fib_FileName, fib->fib_DirEntryType, Pattern, Contents, Filetype);

			d1(KPrintF("%s/%s/%ld: fib_fileName=<%s>\n", __FILE__, __FUNC__, __LINE__, fib->fib_FileName));

			DoMethod(APP_Main, MUIM_Application_InputBuffered);
			}
#endif /* USE_EXALL */
		} while (0);

#if USE_EXALL
	if (eac)
		{
		if (More)
			ExAllEnd(DirLock, EAData, eadSize, ED_TYPE, eac);

		FreeDosObject(DOS_EXALLCONTROL, eac);
		}
	if (EAData)
		free(EAData);
#else /* USE_EXALL */
	if (fib)
		FreeDosObject(DOS_FIB, fib);
#endif /* USE_EXALL */

	CurrentDir(OldCurrentDir);
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static void SearchEntry(BPTR DirLock, CONST_STRPTR Name, LONG DirEntryType,
	CONST_STRPTR Pattern, CONST_STRPTR Contents, CONST_STRPTR Filetype)
{
	d1(KPrintF("%s/%s/%ld: Name=<%s>  DirEntryType=%ld\n", __FILE__, __FUNC__, __LINE__, Name, DirEntryType));
	debugLock_d1(DirLock);

	switch (DirEntryType)
		{
	case ST_ROOT:
	case ST_USERDIR:
	case ST_LINKDIR:
		DoSearchPath(Name, Pattern, Contents, Filetype);
		break;

	case ST_SOFTLINK:
	case ST_FILE:
	case ST_LINKFILE:
	case ST_PIPEFILE:
		SearchFile(Name, Pattern, Contents, Filetype);
		break;

	default:
		if (DirEntryType > 0)
			DoSearchPath(Name, Pattern, Contents, Filetype);
		else if (DirEntryType < 0)
			SearchFile(Name, Pattern, Contents, Filetype);
		break;
		}
}

//----------------------------------------------------------------------------

static void SearchFile(CONST_STRPTR FileName, CONST_STRPTR Pattern,
	CONST_STRPTR Contents, CONST_STRPTR Filetype)
{
	do	{
		struct FindResult fr;

		if (Pattern)
			{
			// Match file name pattern
			if (!MatchPatternNoCase(Pattern, (STRPTR) FileName))
				break;
			}
		if (Contents)
			{
			// Match file contents
			BPTR fh;
			BOOL Match;

			fh = Open(FileName, MODE_OLDFILE);
			d1(KPrintF("%s/%s/%ld: FileName=<%s>  fh=%08lx\n", __FILE__, __FUNC__, __LINE__, FileName, fh));
			if (0 == fh)
				break;

			Match = MatchFileContents(fh, Contents);

			Close(fh);

			if (!Match)
				break;
			}
		if (Filetype)
			{
			const struct TypeNode *tn;
			BPTR dirLock;

			dirLock	= CurrentDir((BPTR)NULL);
			CurrentDir(dirLock);
			
			tn = DefIconsIdentify(dirLock, FileName);
			if (tn && IS_TYPENODE(tn))
				{
				d1(KPrintF("%s/%s/%ld: identify: tn_Name=<%s>  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, tn->tn_Name, FileName));
				if (!SearchTypeNodeTree(tn, Filetype))
					break;
				}
			else
				{
				break;
				}
			}

		fr.fr_Lock = CurrentDir((BPTR)NULL);
		CurrentDir(fr.fr_Lock);
		fr.fr_Name = (STRPTR) FileName;

		DoMethod(NListResults, MUIM_NList_InsertSingle,
			&fr, MUIV_NList_Insert_Sorted);
		} while (0);
}

//----------------------------------------------------------------------------

static BOOL SearchTypeNodeTree(const struct TypeNode *tn, CONST_STRPTR Name)
{
	if (0 == Stricmp(tn->tn_Name, Name))
		{
		d1(KPrintF("%s/%s/%ld: match: tn_Name=<%s>  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, tn->tn_Name, Name));
		return TRUE;
		}

	for  (tn = tn->tn_Parent; tn; tn = tn->tn_Parent)
		{
		if (0 == Stricmp(tn->tn_Name, Name))
			{
			d1(KPrintF("%s/%s/%ld: match: tn_Name=<%s>  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, tn->tn_Name, Name));
			return TRUE;
			}
		}

	return FALSE;	// not found
}

//----------------------------------------------------------------------------

/*
 * QuickSearch algoritm (Modified Bayer-Moore).
 * http://www-igm.univ-mlv.fr/~lecroq/string/index.html
 */

#define ASIZE 256   /* alphabet size */

static void preQsBc(const UBYTE *x, LONG m, LONG qsBc[])
{
	LONG i;

	for (i = 0; i < ASIZE; ++i)
		qsBc[i] = m + 1;
	for (i = 0; i < m; ++i)
		qsBc[x[i]] = m - i;
}


static LONG QuickSearch(const UBYTE *x, LONG m, UBYTE *y, LONG n)
{
	LONG j, qsBc[ASIZE];

	// Initialize qsBc[]
	preQsBc(x, m, qsBc);

	/* Searching */
	j = 0;
	while (j <= n - m)
	{
		if (memcmp(x, y + j, m) == 0)
			return j;
		j += qsBc[y[j + m]];               /* shift */
	}

	return -1;
}


static BOOL MatchFileContents(BPTR fh, CONST_STRPTR Contents)
{
	UBYTE *ReadBuffer;
	BOOL Match = FALSE;

	do	{
		size_t ContentsLen = strlen(Contents);
		LONG offset = 0;

		ReadBuffer = malloc(1 + BUFFERSIZE);
		if (NULL == ReadBuffer)
			break;

		while (!StopSearch)
			{
			LONG BytesRead;
			ULONG n;

			BytesRead = FRead(fh, ReadBuffer + offset, 1, BUFFERSIZE - offset);
			if (0 == BytesRead)
				break;

			ReadBuffer[BytesRead + offset] = 0;

			for(n = 0; n < BytesRead + offset; n++)
				{
				ReadBuffer[n] = ToLower(ReadBuffer[n]);
				}

			if (-1 != QuickSearch((const UBYTE *)Contents, ContentsLen, ReadBuffer, BytesRead + offset))
				{
				d1(KPrintF("%s/%s/%ld: Contents=<%s>  ContentsLen=%lu\n", __FILE__, __FUNC__, __LINE__, Contents, ContentsLen));
				Match = TRUE;
				break;
				}

			// Copy end of buffer to the begining.
			if (BytesRead == BUFFERSIZE - offset)
				{
				memcpy(ReadBuffer, ReadBuffer + BUFFERSIZE - 1 - ContentsLen, ContentsLen);
				offset = ContentsLen;
				}
			else
				{
				break;
				}

			DoMethod(APP_Main, MUIM_Application_InputBuffered);
			}
		} while (0);

	if (ReadBuffer)
		free(ReadBuffer);

	return Match;
}

//----------------------------------------------------------------------------

static void FillSourcesList(struct WBStartup *WBenchMsg)
{
	if (WBenchMsg && WBenchMsg->sm_ArgList)
		{
		ULONG n;

		for (n = 1; n < WBenchMsg->sm_NumArgs; n++)
			{
			struct WBArg *arg = &WBenchMsg->sm_ArgList[n];
			char Path[512];

			NameFromLock(arg->wa_Lock, Path, sizeof(Path));
			AddPart(Path, arg->wa_Name, sizeof(Path));

			d1(KPrintF("%s/%s/%ld: wa_Lock=%08lx\n", __FILE__, __FUNC__,, __LINE__, arg->wa_Lock));

			AddSourcesLine(Path);
			}
		}
}

//----------------------------------------------------------------------------

static Object *MyPopButton(ULONG img)
{
	Object *obj;

	obj = MUI_MakeObject(MUIO_PopButton, img);

	if (obj)
		{
		set(obj, MUIA_CycleChain, TRUE);
		}

	return obj;
}

//----------------------------------------------------------------------------

//  Checks if a certain version of a MCC is available
BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev)
{
	BOOL flush = TRUE;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: %s ", __LINE__, name);)

	while (1)
		{
		ULONG ver;
		ULONG rev;
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
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: v%ld.%ld found through MUIA_Version/Revision\n", __LINE__, ver, rev);)
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
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: v%ld.%ld found through OpenLibrary()\n", __LINE__, ver, rev);)
				return TRUE;
				}

			if (OpenCnt > 1)
				{
				if (MUI_Request(NULL, NULL, 0L,
					(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                        (STRPTR) GetLocString(MSGID_STARTUP_MCC_IN_USE),
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
				extern struct ExecBase *SysBase;

				Forbid();
				if ((result = (struct Library *) FindName(&SysBase->LibList, name)))
					RemLibrary(result);
				Permit();
				flush = FALSE;
				}
			else
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: couldn`t find minimum required version.\n", __LINE__);)

				// We're out of luck - open count is 0, we've tried to flush
				// and still haven't got the version we want
				if (MUI_Request(NULL, NULL, 0L,
					(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                        (STRPTR) GetLocString(MSGID_STARTUP_OLD_MCC),
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
			if (!MUI_Request(NULL, NULL, 0L,
				(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
				(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                (STRPTR) GetLocString(MSGID_STARTUP_MCC_NOT_FOUND),
				name, minver, minrev))
				{
				break;
				}
			}
		}

	return FALSE;
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
		__LINE__, Now.tv_secs, Now.tv_micro, \
			tv->tv_secs, tv->tv_micro, Diff));

	return Diff;
}

//----------------------------------------------------------------------------

static BOOL SearchFileType(CONST_STRPTR SearchString, BOOL SearchNext)
{
	BOOL Found = FALSE;
	STRPTR SearchPattern = NULL;
	STRPTR PatternBuffer = NULL;

	do	{
		struct MUI_NListtree_TreeNode *tn = NULL;
		size_t SearchPatternLength;
		size_t PatternBufLength;
		ULONG dosearch;

		if (SearchString == NULL || strlen(SearchString) < 1)
			break;

		/*
		 * Search for pattern in mimetype name and description.
		 */
		SearchPatternLength = strlen(SearchString) + 2 + 2 + 1;

		SearchPattern = malloc(SearchPatternLength);
		if (NULL == SearchPattern)
			break;

		snprintf(SearchPattern, SearchPatternLength, "#?%s#?", SearchString);

		PatternBufLength = 2 * strlen(SearchPattern) + 2;

		PatternBuffer = malloc(PatternBufLength);
		if (NULL == PatternBuffer)
			break;

		if (ParsePatternNoCase(SearchPattern, PatternBuffer, PatternBufLength) < 0)
			break;

		/*
		 * When searching for next one, fetch selected one first.
		 */

		if (SearchNext)
			{
			tn = (struct MUI_NListtree_TreeNode *) DoMethod(ListtreeFileTypes,
				MUIM_NListtree_GetEntry,
				MUIV_NListtree_GetEntry_ListNode_Active,
				MUIV_NListtree_GetEntry_Position_Active,
                                0);
			}

		dosearch = NULL == tn;

		tn = FindPatternInListtree(ListtreeFileTypes,
			MUIV_NListtree_GetEntry_ListNode_Root, PatternBuffer, tn, &dosearch);

		if (NULL == tn)
			break;

		Found = TRUE;
		/*
		 * Open node if needed and select.
		 */
		DoMethod(ListtreeFileTypes,
			MUIM_NListtree_Open,
			MUIV_NListtree_Open_ListNode_Parent,
			tn,
			0);
		set(ListtreeFileTypes, MUIA_NListtree_Active, tn);

		} while (0);

	if (SearchPattern)
		free(SearchPattern);
	if (PatternBuffer)
		free(PatternBuffer);

	return Found;
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *FindPatternInListtree(Object *ListTree,
	struct MUI_NListtree_TreeNode *list, CONST_STRPTR pattern,
	struct MUI_NListtree_TreeNode *startnode, ULONG *dosearch)
{
	struct MUI_NListtree_TreeNode *tn;
	ULONG pos;

	for (pos = 0; ; pos++)
		{
		tn = (struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
			MUIM_NListtree_GetEntry,
                        list,
			pos,
			MUIV_NListtree_GetEntry_Flag_SameLevel);

		if (NULL == tn)
			break;

		if (tn->tn_Flags & TNF_LIST)
			{
			struct MUI_NListtree_TreeNode *tnChild;

			tnChild	= FindPatternInListtree(ListTree, tn, pattern, startnode, dosearch);

			if (tnChild)
				return tnChild;

			if (*dosearch)
				{
				if (MatchPatternNoCase(pattern, tn->tn_Name))
					return tn;
				}
			}

		if ( tn == startnode )
			*dosearch = TRUE;
		}

	return NULL;
}

//----------------------------------------------------------------------------

static void AddSourcesLine(CONST_STRPTR NewSourcePath)
{
	PushEntry(NListSources, NewSourcePath, MUIV_NList_Insert_Bottom);
}

//----------------------------------------------------------------------------

static void PushEntry(Object *NList, CONST_STRPTR Contents, LONG Position)
{
	ULONG entries = 0;
	ULONG n;

	get(NList, MUIA_NList_Entries, &entries);

	for (n = 0; n < entries; n++)
		{
		STRPTR entry = NULL;

		DoMethod(NList, MUIM_NList_GetEntry,
			n,
			&entry);
		if (entry && 0 == Stricmp(entry, Contents))
			{
			DoMethod(NList,
				MUIM_NList_Remove,
				n);
			get(NList, MUIA_NList_Entries, &entries);
			n--;
			}
		}

	DoMethod(NList, MUIM_NList_InsertSingle,
		Contents,
		MUIV_NList_Insert_Top);
}

//----------------------------------------------------------------------------

static void ExportNList(Class *cl, Object *obj, struct MUIP_Export *msg)
{
	ULONG id = muiNotifyData(obj)->mnd_ObjectID;

	d1(KPrintF("%s/%ld:  START id=%08lx\n", __FUNC__, __LINE__, id));
	if (0 != id)
        {
		size_t TotalLength = 0;
		ULONG entries = 0;
		ULONG n;

		get(obj, MUIA_NList_Entries, &entries);

		if (entries > MAX_SAVED_NLIST_ENTRIES)
			entries = MAX_SAVED_NLIST_ENTRIES;

		for (n = 0; n < entries; n++)
			{
			STRPTR entry = NULL;

			DoMethod(obj, MUIM_NList_GetEntry,
				n,
				&entry);

			if (entry)
				TotalLength += 1 + strlen(entry);
			}

		if (TotalLength)
			{
			struct MyNListExport *nle;

			TotalLength += sizeof(struct MyNListExport);

			nle = malloc(TotalLength);

			if (nle)
				{
				STRPTR bp = nle->nle_Data;

				nle->nle_Size = TotalLength;
				d1(KPrintF("%s/%ld:  entries=%lu  nle_Size=%lu\n", __FUNC__, __LINE__, entries, nle->nle_Size));

				for (n = 0; n < entries; n++)
					{
					STRPTR entry = NULL;

					DoMethod(obj, MUIM_NList_GetEntry,
						n,
						&entry);

					if (entry)
						{
						d1(KPrintF("%s/%ld:  entry=<%s>\n", __FUNC__, __LINE__, entry));
						strcpy(bp, entry);
						bp += 1 + strlen(bp);
						}
					}

				DoMethod(msg->dataspace,
					MUIM_Dataspace_Add,
					nle,
					TotalLength,
	                                id);

				free(nle);
				}
			}
		else
			{
			DoMethod(msg->dataspace, MUIM_Dataspace_Remove, id);
			}
        }
}

//----------------------------------------------------------------------------

static void ImportNList(Class *cl, Object *obj, struct MUIP_Import *msg)
{
	ULONG id = muiNotifyData(obj)->mnd_ObjectID;

	d1(KPrintF("%s/%ld:  START id=%08lx\n", __FUNC__, __LINE__, id));
	if (0 != id)
        {
		struct MyNListExport *nle;

		nle = (struct MyNListExport *) DoMethod(msg->dataspace, MUIM_Dataspace_Find, id);

		d1(KPrintF("%s/%ld:  nle=%08lx\n", __FUNC__, __LINE__, nle));

		if (nle)
			{
			d1(KPrintF("%s/%ld:  nle_Size=%lu\n", __FUNC__, __LINE__, nle->nle_Size));

			if (nle->nle_Size >= sizeof(struct MyNListExport))
				{
				size_t TotalSize = nle->nle_Size - sizeof(struct MyNListExport);
				STRPTR entry = nle->nle_Data;

				while (TotalSize)
					{
					size_t len = 1 + strlen(entry);

					d1(KPrintF("%s/%ld:  TotalSize=%lu  entry=<%s>\n", __FUNC__, __LINE__, TotalSize, entry));

					DoMethod(obj, MUIM_NList_InsertSingle,
						entry,
                                                MUIV_NList_Insert_Bottom);

					entry += len;
					if (TotalSize > len)
						TotalSize -= len;
					else
						TotalSize = 0;
					}
				}
			}
        }
}

//----------------------------------------------------------------------------



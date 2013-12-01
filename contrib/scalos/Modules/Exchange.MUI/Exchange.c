// Exchange.c
// $Date$
// $Revision$

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/execbase.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>		// MAKE_ID()
#include <libraries/commodities.h>
#include <mui/NListview_mcc.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/commodities.h>

#include <DefIcons.h>
#include <defs.h>
#include <Year.h> // +jmc+

#include "Exchange.h"
#include "cx_private.h"
#include "debug.h"

#define	Exchange_NUMBERS
#define	Exchange_ARRAY
#define	Exchange_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#if !defined(__amigaos4__) && !defined(__AROS__)
#include <dos.h>

long _stack = 16384;		// minimum stack size, used by SAS/C startup code
#endif

//----------------------------------------------------------------------------

// local data structures

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

struct CxListEntry
{
	STRPTR xle_Name;
	STRPTR xle_Title;
	STRPTR xle_Description;
	struct Task *xle_Task;
	APTR xle_Addr;
	LONG xle_Pri;
	UWORD xle_Flags;
	char xle_PriString[10];
};

//----------------------------------------------------------------------------

// local functions

static void init(void);
static void fail(APTR APP_Main, CONST_STRPTR str);
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CxListConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm);
static SAVEDS(void) INTERRUPT CxListDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm);
static SAVEDS(ULONG) INTERRUPT CxListDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm);
static SAVEDS(LONG) INTERRUPT CxListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm);
static SAVEDS(void) INTERRUPT RescanHookFunc(struct Hook *hook, Object *obj, Msg *msg);
static SAVEDS(void) INTERRUPT SelectCxHookFunc(struct Hook *hook, Object *obj, Msg *msg);
static SAVEDS(void) INTERRUPT CxCommandHookFunc(struct Hook *hook, Object *obj, LONG *cmd);
static SAVEDS(void) INTERRUPT BrokerHookFunc(struct Hook *hook, Object *obj, CxMsg *msg);

static STRPTR GetLocString(ULONG MsgId);
static void TranslateStringArray(STRPTR *stringArray);
#if 0
static void TranslateNewMenu(struct NewMenu *nm);
#endif

BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev);

//----------------------------------------------------------------------------

static struct Hook AboutHook = {{ NULL, NULL }, HOOKFUNC_DEF(OpenAboutFunc), NULL };
static struct Hook AboutMUIHook = {{ NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIFunc), NULL };

static struct Hook CxListConstructHook = {{ NULL, NULL }, HOOKFUNC_DEF(CxListConstructHookFunc), NULL };
static struct Hook CxListDestructHook = {{ NULL, NULL }, HOOKFUNC_DEF(CxListDestructHookFunc), NULL };
static struct Hook CxListDisplayHook = {{ NULL, NULL }, HOOKFUNC_DEF(CxListDisplayHookFunc), NULL };
static struct Hook CxListCompareHook = {{ NULL, NULL }, HOOKFUNC_DEF(CxListCompareHookFunc), NULL };
static struct Hook RescanHook = {{ NULL, NULL }, HOOKFUNC_DEF(RescanHookFunc), NULL };
static struct Hook SelectCxHook = {{ NULL, NULL }, HOOKFUNC_DEF(SelectCxHookFunc), NULL };
static struct Hook CxCommandHook = {{ NULL, NULL }, HOOKFUNC_DEF(CxCommandHookFunc), NULL };
static struct Hook BrokerHook = {{ NULL, NULL }, HOOKFUNC_DEF(BrokerHookFunc), NULL };

//----------------------------------------------------------------------------

static STRPTR CycleActiveEntries[] =
	{
	(STRPTR) MSGID_CYCLE_ACTIVE_ACTIVE,
	(STRPTR) MSGID_CYCLE_ACTIVE_INACTIVE,
	NULL
	};

//----------------------------------------------------------------------------

// local data items

static struct List BrokerCopyList;

struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *CxBase = NULL;
T_LOCALEBASE LocaleBase = NULL;

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct CxIFace *ICx = NULL;
struct LocaleIFace *ILocale = NULL;
#endif

static struct Catalog *ExchangeCatalog;

static Object *APP_Main;
static Object *WIN_Main;
static Object *WIN_AboutMUI;
static Object *ButtonShow, *ButtonHide, *ButtonRemove;
static Object *CycleActive;
static Object *NListCxList;
static Object *FloattextDescription;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit;

//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	LONG win_opened = 0;
	
	init();

	if (!CheckMCCVersion(MUIC_NListview, 19, 66) )
		{
		d1(KPrintF(__FILE__ "/%s/%ld: CheckMCCVersion failed\n", __FUNC__, __LINE__));
		return 10;
		}

	APP_Main = ApplicationObject,
		MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
		MUIA_Application_Version,	"$VER: Scalos Exchange.module V" VERS_MAJOR "." VERS_MINOR " (" __DATE__ ") " COMPILER_STRING,
		MUIA_Application_Copyright,	"© The Scalos Team, 2008" CURRENTYEAR,
		MUIA_Application_Author,	"The Scalos Team",
		MUIA_Application_Description,	"Scalos Exchange module",
		MUIA_Application_Base,		"SCALOS_EXCHANGE_MODULE",
		MUIA_Application_BrokerHook, 	&BrokerHook,

		SubWindow, WIN_Main = WindowObject,
			MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
			MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
			MUIA_Window_AppWindow, TRUE,

			WindowContents, VGroup,
				Child, NListviewObject,
					MUIA_Weight, 200,
					MUIA_CycleChain, TRUE,
					MUIA_NListview_NList, NListCxList = NListObject,
						MUIA_NList_Title, TRUE,
						MUIA_NList_TitleSeparator, TRUE,
						MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 0,
						MUIA_NList_SortType, 0,
						MUIA_NList_Format, "BAR,BAR,BAR,BAR",
						MUIA_NList_DisplayHook2, &CxListDisplayHook,
						MUIA_NList_ConstructHook2, &CxListConstructHook,
						MUIA_NList_DestructHook2, &CxListDestructHook,
						MUIA_NList_CompareHook2, &CxListCompareHook,
						End, //NListObject
					End, //NListviewObject

				Child, VGroup,
					Child, FloattextDescription = FloattextObject,
						TextFrame,
						MUIA_VertWeight, 0,
						End, //FloattextObject

					Child, ColGroup(2),
						Child, ButtonShow = KeyButtonHelp(GetLocString(MSGID_BUTTON_SHOW),
							*GetLocString(MSGID_BUTTON_SHOW_KEY),
							GetLocString(MSGID_BUTTON_SHOW_SHORTHELP)),
						Child, ButtonHide = KeyButtonHelp(GetLocString(MSGID_BUTTON_HIDE),
							*GetLocString(MSGID_BUTTON_HIDE_KEY),
							GetLocString(MSGID_BUTTON_HIDE_SHORTHELP)),

						Child, CycleActive = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, CycleActiveEntries,
							End, //CycleObject

						Child, ButtonRemove = KeyButtonHelp(GetLocString(MSGID_BUTTON_REMOVE),
							*GetLocString(MSGID_BUTTON_REMOVE_KEY),
							GetLocString(MSGID_BUTTON_REMOVE_SHORTHELP)),
						End, //ColGroup
					End, //VGroup
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

	DoMethod(APP_Main, MUIM_Application_Load, MUIV_Application_Load_ENV);

	d1(KPrintF("%s/%s/%ld: before MUIM_CallHook APP_Main=%08lx\n", __FILE__, __FUNC__, __LINE__, APP_Main));

	DoMethod(APP_Main, MUIM_CallHook, &RescanHook, 0);
	d1(KPrintF("%s/%s/%ld: after MUIM_CallHook APP_Main=%08lx\n", __FILE__, __FUNC__, __LINE__, APP_Main));

	//--------------------------------------------------------------------------//

	DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
		WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);
	DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
		APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	// Call hook whenever an entry is selected
	DoMethod(NListCxList, MUIM_Notify, MUIA_NList_SelectChange, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &SelectCxHook);

	// Show Cx Application Window whenever "Show" button is pressed
	DoMethod(ButtonShow, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 3, MUIM_CallHook, &CxCommandHook, CXCMD_APPEAR);

	// Show Cx Application Window whenever list entry is double-clicked
	DoMethod(NListCxList, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		APP_Main, 3, MUIM_CallHook, &CxCommandHook, CXCMD_APPEAR);

	// Hide Cx Application Window whenever "Hide" button is pressed
	DoMethod(ButtonHide, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 3, MUIM_CallHook, &CxCommandHook, CXCMD_DISAPPEAR);

	// Activate Cx Application when cycle "active" is selected
	DoMethod(CycleActive, MUIM_Notify, MUIA_Cycle_Active, 0,
		APP_Main, 3, MUIM_CallHook, &CxCommandHook, CXCMD_ENABLE);

	// Deactivate Cx Application when cycle "inactive" is selected
	DoMethod(CycleActive, MUIM_Notify, MUIA_Cycle_Active, 1,
		APP_Main, 3, MUIM_CallHook, &CxCommandHook, CXCMD_DISABLE);

	// Remove Cx Application whenever "Remove" button is pressed
	DoMethod(ButtonRemove, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 3, MUIM_CallHook, &CxCommandHook, CXCMD_KILL);

	// setup sorting hooks for Cx list
	DoMethod(NListCxList, MUIM_Notify, MUIA_NList_TitleClick, MUIV_EveryTime,
		NListCxList, 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
	DoMethod(NListCxList, MUIM_Notify, MUIA_NList_TitleClick2, MUIV_EveryTime,
		NListCxList, 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
	DoMethod(NListCxList, MUIM_Notify, MUIA_NList_SortType, MUIV_EveryTime,
		NListCxList, 3, MUIM_Set, MUIA_NList_TitleMark, MUIV_TriggerValue);
	DoMethod(NListCxList, MUIM_Notify, MUIA_NList_SortType2, MUIV_EveryTime,
		NListCxList, 3, MUIM_Set, MUIA_NList_TitleMark2, MUIV_TriggerValue);

	DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &AboutHook);
	DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &AboutMUIHook);
	DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
		APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

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
	CxFreeBrokerList(&BrokerCopyList);

	if (APP_Main)
		{
		MUI_DisposeObject(APP_Main);
		}
	if (ExchangeCatalog)
		{
		CloseCatalog(ExchangeCatalog);
		ExchangeCatalog = NULL;
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

static void init(void)
{
	NewList(&BrokerCopyList);

	if (!OpenLibraries())
		fail(NULL, "Failed to open "MUIMASTER_NAME".");

	if (LocaleBase)
		ExchangeCatalog = OpenCatalogA(NULL, "Scalos/Exchange.catalog", NULL);

//	  TranslateNewMenu(NewContextMenuHistoryPopup);
	TranslateStringArray(CycleActiveEntries);
}

//----------------------------------------------------------------------------

static BOOL OpenLibraries(void)
{
	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
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

	MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
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

	CxBase = OpenLibrary("commodities.library", 40);
	if (NULL == CxBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		ICx = (struct CxIFace *)GetInterface((struct Library *)CxBase, "main", 1, NULL);
		if (NULL == ICx)
			return FALSE;
		}
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		{
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
		if (NULL == ILocale)
			return FALSE;
		}
#endif

	return TRUE;
}

//----------------------------------------------------------------------------

static void CloseLibraries(void)
{
	if (LocaleBase)
		{
		if (ExchangeCatalog)
			{
			CloseCatalog(ExchangeCatalog);
			ExchangeCatalog = NULL;
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
	if (ICx)
		{
		DropInterface((struct Interface *)ICx);
		ICx = NULL;
		}
#endif
	if (CxBase)
		{
		CloseLibrary((struct Library *) CxBase);
		CxBase = NULL;
		}
#ifdef __amigaos4__
	if (IMUIMaster)
		{
		DropInterface((struct Interface *) IMUIMaster);
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
		DropInterface((struct Interface *) IIntuition);
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
	struct Exchange_LocaleInfo li;

	li.li_Catalog = ExchangeCatalog;
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetExchangeString(&li, MsgId);
}

//----------------------------------------------------------------------------

static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}

//----------------------------------------------------------------------------
#if 0
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
#endif
//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg)
{
	if (NULL == WIN_AboutMUI)
		{
		WIN_AboutMUI = MUI_NewObject(MUIC_Aboutmui,
			MUIA_Window_RefWindow, WIN_Main,
			MUIA_Aboutmui_Application, APP_Main,
			End;
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

static SAVEDS(APTR) INTERRUPT CxListConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm)
{
	struct CxListEntry *xle = AllocPooled(nlcm->pool, sizeof(struct CxListEntry));
	BOOL Success = FALSE;

	(void) unused;

	do	{
		struct BrokerCopy *bc = nlcm->entry;

		d1(KPrintF("%s/%s/%ld: bc=%08lx\n", __FILE__, __FUNC__, __LINE__, bc));
		d1(KPrintF("%s/%s/%ld: bc_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, bc->bc_Name));
		d1(KPrintF("%s/%s/%ld: bc_Title=<%s>\n", __FILE__, __FUNC__, __LINE__, bc->bc_Title));
		d1(KPrintF("%s/%s/%ld: bc_Descr=<%s>\n", __FILE__, __FUNC__, __LINE__, bc->bc_Descr));

		if (NULL == xle)
			break;

		xle->xle_Name = strdup(bc->bc_Name);
		if (NULL == xle->xle_Name)
			break;

		xle->xle_Title = strdup(bc->bc_Title);
		if (NULL == xle->xle_Title)
			break;

		xle->xle_Description = strdup(bc->bc_Descr);
		if (NULL == xle->xle_Description)
			break;

		xle->xle_Task  = bc->bc_Task;
		xle->xle_Pri   = bc->bc_Node.ln_Pri;
		xle->xle_Flags = bc->bc_Flags;
#ifdef __AROS__
		xle->xle_Addr  = bc->bc_Port;
#else
		xle->xle_Addr  = bc->bc_Addr;
#endif

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		if (xle)
			{
			if (xle->xle_Name)
				free(xle->xle_Name);
			if (xle->xle_Title)
				free(xle->xle_Title);
			if (xle->xle_Description)
				free(xle->xle_Description);

			FreePooled(nlcm->pool, xle, sizeof(struct CxListEntry));
			xle = NULL;
			}
		}

	d1(KPrintF("%s/%s/%ld: END  xle=%08lx\n", __FILE__, __FUNC__, __LINE__, xle));

	return (APTR) xle;
}


static SAVEDS(void) INTERRUPT CxListDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm)
{
	struct CxListEntry *xle = (struct CxListEntry *) nldm->entry;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	(void) unused;

	if (xle)
		{
		if (xle->xle_Name)
			free(xle->xle_Name);
		if (xle->xle_Title)
			free(xle->xle_Title);
		if (xle->xle_Description)
			free(xle->xle_Description);

		FreePooled(nldm->pool, xle, sizeof(struct CxListEntry));
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static SAVEDS(ULONG) INTERRUPT CxListDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm)
{
	(void) unused;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (nldm->entry)
		{
		struct CxListEntry *xle = (struct CxListEntry *) nldm->entry;

		d1(KPrintF("%s/%s/%ld: xle=%08lx\n", __FILE__, __FUNC__, __LINE__, xle));
		d1(KPrintF("%s/%s/%ld: xle_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, xle->xle_Name));

		sprintf(xle->xle_PriString, "%ld", (long int)xle->xle_Pri);

		nldm->strings[0] = xle->xle_Name;
		nldm->strings[1] = xle->xle_Title;
		nldm->strings[2] = CycleActiveEntries[(xle->xle_Flags & COF_ACTIVE) ? 0 : 1];
		nldm->strings[3] = xle->xle_PriString;
		}
	else
		{
		// display titles
		nldm->strings[0] = GetLocString(MSGID_CXLIST_NAME);
		nldm->strings[1] = GetLocString(MSGID_CXLIST_TITLE);
		nldm->strings[2] = GetLocString(MSGID_CXLIST_STATE);
		nldm->strings[3] = GetLocString(MSGID_CXLIST_PRIORITY);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static SAVEDS(LONG) INTERRUPT CxListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm)
{
	const struct CxListEntry *xle1 = (const struct CxListEntry *) ncm->entry1;
	const struct CxListEntry *xle2 = (const struct CxListEntry *) ncm->entry2;
	LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
	LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
	LONG Result = 0;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		// primary sorting
		switch (col1)
			{
		case 0:		// sort by name
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(xle2->xle_Name, xle1->xle_Name);
			else
				Result = Stricmp(xle1->xle_Name, xle2->xle_Name);
			break;
		case 1:		// sort by title
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(xle2->xle_Title, xle1->xle_Title);
			else
				Result = Stricmp(xle1->xle_Title, xle2->xle_Title);
			break;
		case 2:		// sort by state
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = (xle2->xle_Flags & COF_ACTIVE) - (xle1->xle_Flags & COF_ACTIVE);
			else
				Result = (xle1->xle_Flags & COF_ACTIVE) - (xle2->xle_Flags & COF_ACTIVE);
			break;
		case 3:		// sort by priority
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = xle2->xle_Pri - xle1->xle_Pri;
			else
				Result = xle1->xle_Pri - xle2->xle_Pri;
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
					Result = Stricmp(xle2->xle_Name, xle1->xle_Name);
				else
					Result = Stricmp(xle1->xle_Name, xle2->xle_Name);
				break;
			case 1:		// sort by title
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = Stricmp(xle2->xle_Title, xle1->xle_Title);
				else
					Result = Stricmp(xle1->xle_Title, xle2->xle_Title);
				break;
			case 2:		// sort by state
				if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
					Result = (xle2->xle_Flags & COF_ACTIVE) - (xle1->xle_Flags & COF_ACTIVE);
				else
					Result = (xle1->xle_Flags & COF_ACTIVE) - (xle2->xle_Flags & COF_ACTIVE);
				break;
			case 3:		// sort by priority
				if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
					Result = xle2->xle_Pri - xle1->xle_Pri;
				else
					Result = xle1->xle_Pri - xle2->xle_Pri;
				break;
			default:
				break;
				}
			}
		}

	d1(KPrintF("%s/%s/%ld: END  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT RescanHookFunc(struct Hook *hook, Object *obj, Msg *msg)
{
	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct BrokerCopy *bc;
		LONG res;

		CxFreeBrokerList(&BrokerCopyList);

		set(NListCxList, MUIA_NList_Quiet, MUIV_NList_Quiet_Full);
		DoMethod(NListCxList, MUIM_NList_Clear);

		res = CxCopyBrokerList(&BrokerCopyList);
		d1(KPrintF("%s/%s/%ld: CxCopyBrokerList returned %ld\n", __FILE__, __FUNC__, __LINE__, res));

		for (bc = (struct BrokerCopy *) BrokerCopyList.lh_Head;
			bc != (struct BrokerCopy *) &BrokerCopyList.lh_Tail;
			bc = (struct BrokerCopy *) bc->bc_Node.ln_Succ)
			{
			d1(KPrintF("%s/%s/%ld: bc=%08lx\n", __FILE__, __FUNC__, __LINE__, bc));
			d1(KPrintF("%s/%s/%ld: bc_Name=<%s>  bc_Title=<%s>  bc_Descr=<%s>\n", \
				__FILE__, __FUNC__, __LINE__, bc->bc_Name, bc->bc_Title, bc->bc_Descr));
			d1(KPrintF("%s/%s/%ld: bc_Addr=%08lx  bc_Dummy2=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, bc->bc_Addr, bc->bc_Dummy2));

			DoMethod(NListCxList,
				MUIM_NList_InsertSingle,
				bc,
				MUIV_NList_Insert_Sorted);
			}

		set(NListCxList, MUIA_NList_Quiet, MUIV_NList_Quiet_None);
		} while (0);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT SelectCxHookFunc(struct Hook *hook, Object *obj, Msg *msg)
{
	const struct CxListEntry *xle = NULL;

	DoMethod(NListCxList,
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active,
		&xle);

	if (xle)
		{
		STRPTR Description;

		Description = malloc(3 + strlen(xle->xle_Title) + strlen(xle->xle_Description));
		if (Description)
			{
			sprintf(Description, "%s\n%s", xle->xle_Title, xle->xle_Description);
			set(FloattextDescription, MUIA_Floattext_Text, Description);
			free(Description);
			}

		set(ButtonShow, MUIA_Disabled, !(xle->xle_Flags & COF_SHOW_HIDE));
		set(ButtonHide, MUIA_Disabled, !(xle->xle_Flags & COF_SHOW_HIDE));
		set(ButtonRemove, MUIA_Disabled, FALSE);
		set(CycleActive, MUIA_Disabled, FALSE);
		setcycle(CycleActive, (xle->xle_Flags & COF_ACTIVE) ? 0 : 1);
		}
	else
		{
		set(FloattextDescription, MUIA_Floattext_Text, "");
		set(ButtonShow, MUIA_Disabled, TRUE);
		set(ButtonHide, MUIA_Disabled, TRUE);
		set(ButtonRemove, MUIA_Disabled, TRUE);
		set(CycleActive, MUIA_Disabled, TRUE);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT CxCommandHookFunc(struct Hook *hook, Object *obj, LONG *cmd)
{
	struct CxListEntry *xle = NULL;

	DoMethod(NListCxList,
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active,
		&xle);

	if (xle)
		{
		(void) CxBrokerCommand(xle->xle_Name, *cmd);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT BrokerHookFunc(struct Hook *hook, Object *obj, CxMsg *msg)
{
	if (CXM_COMMAND == CxMsgType(msg))
		{
		switch (CxMsgID(msg))
			{
		case CXCMD_APPEAR:
			set(WIN_Main, MUIA_Window_Open, TRUE);
			break;
		case CXCMD_LIST_CHG:
			DoMethod(APP_Main, MUIM_CallHook, &RescanHook);
			break;
		default:
			break;
			}
		}
}

//----------------------------------------------------------------------------

//  Checks if a certain version of a MCC is available
BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev)
{
	BOOL flush = TRUE;

	d1(KPrintF("%s/%s/%ld: %s ", __FILE__, __FUNC__, __LINE__, name);)

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
				d1(kprintf("%s/%s/%ld: v%ld.%ld found through MUIA_Version/Revision\n", __FILE__, __FUNC__, __LINE__, ver, rev);)
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
				d1(kprintf("%s/%s/%ld: v%ld.%ld found through OpenLibrary()\n", __FILE__, __FUNC__, __LINE__, ver, rev);)
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
				d1(kprintf("%s/%s/%ld: couldn`t Exchange minimum required version.\n", __LINE__);)

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



// NewDrawer.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/alerts.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/classusr.h>
#include <libraries/mui.h>
#include <mui/BetterString_mcc.h>
#include <scalos/scalos.h>
#include <scalos/undo.h>
#include <workbench/startup.h>
#include <datatypes/iconobject.h>

/* For the MAKE_ID macro */
#include <libraries/iffparse.h>

/* For the PrefsStruct structure */
#include <scalos/preferences.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <proto/preferences.h>
#include <proto/scalos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include "NewDrawer.h"

#define	NewDrawer_NUMBERS
#define	NewDrawer_ARRAY
#define	NewDrawer_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#define	VERSION_MAJOR	40
#define	VERSION_MINOR	4

//----------------------------------------------------------------------------

#define	Application_Return_Ok	1001
#define	Application_Return_Skip	1002

//----------------------------------------------------------------------------

#define  PREFSENV	"Env:Scalos/NewDrawer.prefs"
#define  PREFSENVARC	"Envarc:Scalos/NewDrawer.prefs"

//----------------------------------------------------------------------------

#define KeyButtonHelp(name,key,HelpText)\
	TextObject,\
		ButtonFrame,\
		MUIA_Font         , MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
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

//----------------------------------------------------------------------------

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static void UpdateGadgets(struct WBStartup *wbMsg);
static void ReportError(LONG Result, CONST_STRPTR NewObjName);
static LONG MakeNewDrawer(struct WBStartup *wbMsg);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutHookFunc(struct Hook *hook, Object *o, Msg msg);
static STRPTR GetLocString(ULONG MsgId);
static BOOL CheckInfoData(const struct InfoData *info);
static BOOL isDiskWritable(BPTR dLock);
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev);
static void ReadPrefs(STRPTR PrefsFile);	// +jmc+ - Load Prefs.
static void WritePrefs(STRPTR PrefsFile);	// +jmc+ - Save pres.
static struct ScaWindowStruct *FindScalosWindow(BPTR dirLock);

//----------------------------------------------------------------------------

#if !defined(__amigaos4__) && !defined(__AROS__)
#include <dos.h>

long _stack = 16384;		// minimum stack size, used by SAS/C startup code
#endif

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase = NULL;
T_LOCALEBASE LocaleBase = NULL;
#ifndef __amigaos4__
T_UTILITYBASE UtilityBase = NULL;
#endif
struct Library *MUIMasterBase = NULL;
struct Library *IconBase = NULL;
struct Library *IconobjectBase = NULL;
struct Library *PreferencesBase = NULL;
struct ScalosBase *ScalosBase = NULL;

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition = NULL;
struct LocaleIFace *ILocale = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct IconIFace *IIcon = NULL;
struct IconobjectIFace *IIconobject = NULL;
struct PreferencesIFace *IPreferences = NULL;
struct ScalosIFace *IScalos = NULL;
#endif

static BYTE gd_Selected = 1;			// +jmc+ - CreateIcon gadget state.

static struct Catalog *gb_Catalog;

static struct Hook AboutHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutHookFunc), NULL };
static struct Hook AboutMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIHookFunc), NULL };

//----------------------------------------------------------------------------

static Object *Group_Buttons2;
static Object *APP_Main;
static Object *WIN_Main;
static Object *WIN_AboutMUI;
static Object *OkButton, *CancelButton;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit;
static Object *TextEnterName;
static Object *StringNewName;
static Object *CheckmarkCreateIcon;

//----------------------------------------------------------------------------


int main(int argc, char *argv[])
{
	LONG win_opened = 0;
	struct WBStartup *WBenchMsg =
		(argc == 0) ? (struct WBStartup *)argv : NULL;

#if 0
	if (WBenchMsg)
		{
		LONG n;

		kprintf("NumArgs=%ld\n", WBenchMsg->sm_NumArgs);

		for (n=0; n<WBenchMsg->sm_NumArgs; n++)
			{
			char xName[512];

			NameFromLock(WBenchMsg->sm_ArgList[n].wa_Lock, xName, sizeof(xName));

			kprintf("%ld. Lock=<%s>  Name=<%s>\n", n, xName, WBenchMsg->sm_ArgList[n].wa_Name);
			}
		}
#endif

	if (NULL == WBenchMsg)
		{
		return 5;
		}
	if (WBenchMsg->sm_NumArgs < 2)
		{
		return 5;
		}

	do	{
		ULONG sigs = 0;
		BOOL Run = TRUE;

		if (!OpenLibraries())
			break;

		if (!CheckMCCVersion(MUIC_BetterString, 11, 0))
			break;

		gb_Catalog = OpenCatalogA(NULL, "Scalos/NewDrawer.catalog",NULL);

		ReadPrefs(PREFSENV); // load Newdrawer.prefs

		APP_Main = ApplicationObject,
			MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
			MUIA_Application_Version,	"$VER: Scalos NewDrawer.module V40.6 (" __DATE__ ") " COMPILER_STRING,
			MUIA_Application_Copyright,	"© The Scalos Team, 2004" CURRENTYEAR,
			MUIA_Application_Author,	"The Scalos Team",
			MUIA_Application_Description,	"Scalos NewDrawer module",
			MUIA_Application_Base,		"SCALOS_NEWDRAWER",

			SubWindow, WIN_Main = WindowObject,
				MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
//				MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
				MUIA_Window_AppWindow, TRUE,
				MUIA_Window_UseBottomBorderScroller, TRUE, // +jmc+

				MUIA_Window_TopEdge, MUIV_Window_TopEdge_Moused,
				MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Moused,

				MUIA_Window_Width, MUIV_Window_Width_Screen(30), // +jmc+

				WindowContents, VGroup,
					Child, 	GroupObject, // +jmc+
						Child, ScrollgroupObject,
							MUIA_Scrollgroup_VertBar, NULL,
							MUIA_Scrollgroup_HorizBar, NULL,
							MUIA_Scrollgroup_FreeHoriz, TRUE,
							MUIA_Scrollgroup_FreeVert, FALSE,
							MUIA_Scrollgroup_UseWinBorder, TRUE,
							MUIA_Scrollgroup_Contents,
							VirtgroupObject,
								Child, TextEnterName = TextObject,
									MUIA_Text_PreParse, MUIX_C,
									MUIA_ShortHelp, GetLocString(MSGID_TEXT_ENTERNAME_SHORTHELP),
									End, //TextObject
							End, //VirtgroupObject
						End, //ScrollgroupObject
					End, //GroupObject

						Child, VSpace(1),

						Child, HGroup,
							Child, Label1(GetLocString(MSGID_LABEL_ENTERNAME)),
							Child, StringNewName = BetterStringObject,
								StringFrame,
								MUIA_String_Contents, GetLocString(MSGID_NEWNAME_DEFAULT),
								MUIA_String_Reject, ":",
								MUIA_Background, MUII_TextBack,
								MUIA_String_MaxLen, 108,
								MUIA_CycleChain, TRUE,
								MUIA_BetterString_SelectSize, -strlen(GetLocString(MSGID_NEWNAME_DEFAULT)),
								End, //StringObject
						End, //HGroup

					Child, HGroup,
						Child, RectangleObject,
							End,
						Child, Label1(GetLocString(MSGID_LABEL_CREATE_ICON)),
						Child, CheckmarkCreateIcon = CheckMarkHelp(gd_Selected, MSGID_CHECKMARK_CREATEICON_SHORTHELP),
						Child, RectangleObject,
							End,
					End, //HGroup

					Child, VSpace(1),

					Child, Group_Buttons2 = HGroup,
						MUIA_Group_SameWidth, TRUE,
						Child, OkButton = KeyButtonHelp(GetLocString(MSGID_OKBUTTON),
									*GetLocString(MSGID_OKBUTTON_SHORT), GetLocString(MSGID_SHORTHELP_OKBUTTON)),
						Child, CancelButton = KeyButtonHelp(GetLocString(MSGID_CANCELBUTTON),
									*GetLocString(MSGID_CANCELBUTTON_SHORT), GetLocString(MSGID_SHORTHELP_CANCELBUTTON)),
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
			End, //MenuStripObject

		End; //ApplicationObject

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));

		if (NULL == APP_Main)
			{
			printf(GetLocString(MSGID_CREATE_APPLICATION_FAILED));
			break;
			}

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);
		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
		DoMethod(OkButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, Application_Return_Ok);

		DoMethod(StringNewName, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, Application_Return_Ok);

		DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutHook);
		DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutMUIHook);
		DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		// Loading of MUI objects ID settings.
		// DoMethod(APP_Main, MUIM_Application_Load,MUIV_Application_Load_ENV);

		UpdateGadgets(WBenchMsg);

		set(WIN_Main, MUIA_Window_ActiveObject, StringNewName);

		set(WIN_Main, MUIA_Window_Open, TRUE);
		get(WIN_Main, MUIA_Window_Open, &win_opened);

		if (!win_opened)
			{
			printf(GetLocString(MSGID_CREATE_MAINWINDOW_FAILED));
			break;
			}

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));

		while (Run)
			{
			ULONG Action = DoMethod(APP_Main, MUIM_Application_NewInput, &sigs);

			switch (Action)
				{
			case Application_Return_Ok:
				MakeNewDrawer(WBenchMsg);
				WritePrefs(PREFSENV);
				Run = FALSE;
				break;
			case MUIV_Application_ReturnID_Quit:
				WritePrefs(PREFSENV);
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

		set(WIN_Main, MUIA_Window_Open, FALSE);
		} while (0);

	if (APP_Main)
		MUI_DisposeObject(APP_Main);
	if(gb_Catalog)
		CloseCatalog(gb_Catalog);
		
	CloseLibraries();

	return 0;
}

//----------------------------------------------------------------------------

static BOOL OpenLibraries(void)
{
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

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 38);
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
#endif

	IconBase = OpenLibrary("icon.library", 39);
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

#ifndef __amigaos4__
	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;
#endif

	IconobjectBase = OpenLibrary(ICONOBJECTNAME, 39);
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

	PreferencesBase = OpenLibrary("preferences.library", 0);
	if (NULL == PreferencesBase)
		{
		printf(GetLocString(MSGID_OPEN_PREFS_LIBRARY_FAILED), "preferences.library");
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: PreferencesBase = NULL\n", __LINE__));
		}
#ifdef __amigaos4__
	else
		{
		IPreferences = (struct PreferencesIFace *)GetInterface((struct Library *)PreferencesBase, "main", 1, NULL);
		if (NULL == IPreferences)
			return FALSE;
		}
#endif

	ScalosBase = (struct ScalosBase *) OpenLibrary(SCALOSNAME, 40);
	if (NULL == ScalosBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
		if (NULL == IScalos)
			return FALSE;
		}
#endif //__amigaos4__

	return TRUE;
}

//----------------------------------------------------------------------------

static void CloseLibraries(void)
{
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
#ifndef __amigaos4__
	if (UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
#endif
#ifdef __amigaos4__
	if (ILocale)
		{
		DropInterface((struct Interface *)ILocale);
		ILocale = NULL;
		}
#endif
	if (LocaleBase)
		{
		CloseLibrary((struct Library *)LocaleBase);
		LocaleBase = NULL;
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
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase = NULL;
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
}

//----------------------------------------------------------------------------

static void UpdateGadgets(struct WBStartup *wbMsg)
{
	char xName[512];
	char LabelText[512];
	BOOL IsWriteable = isDiskWritable(wbMsg->sm_ArgList[1].wa_Lock);

	NameFromLock(wbMsg->sm_ArgList[1].wa_Lock, xName, sizeof(xName));

	sprintf(LabelText, GetLocString(MSGID_TEXT_ENTERNAME), xName);

	SetAttrs(TextEnterName,
		MUIA_Text_Contents, LabelText,
		TAG_END);

	set(OkButton, MUIA_Disabled, !IsWriteable);
	set(StringNewName, MUIA_Disabled, !IsWriteable);
	set(CheckmarkCreateIcon, MUIA_Disabled, !IsWriteable);
}

//----------------------------------------------------------------------------

static void ReportError(LONG Result, CONST_STRPTR NewObjName)
{
	char FaultText[256];

	Fault(Result, "", FaultText, sizeof(FaultText));

	MUI_Request(APP_Main, WIN_Main, 0, NULL,
		GetLocString(MSGID_REQ_OK),
		GetLocString(MSGID_ERRORREQ_BODYTEXT),
		NewObjName, FaultText);
}

//----------------------------------------------------------------------------

static LONG MakeNewDrawer(struct WBStartup *wbMsg)
{
	BPTR oldDir = CurrentDir(wbMsg->sm_ArgList[1].wa_Lock);
	LONG Result = RETURN_OK;
	STRPTR NewObjName = NULL;
	struct ScaWindowStruct *ws;
	APTR UndoStep = NULL;
	BPTR newDirLock;
	IPTR CreateIcon = FALSE;

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

	GetAttr(MUIA_String_Contents, StringNewName, (APTR) &NewObjName);

	ws = FindScalosWindow(wbMsg->sm_ArgList[1].wa_Lock);

	if ('/' == NewObjName[strlen(NewObjName) - 1])	// +jmc+
			{
			NewObjName[strlen(NewObjName) - 1] = '\0';
			set(StringNewName, MUIA_String_Contents, NewObjName);
			}

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
	
	GetAttr(MUIA_Selected, CheckmarkCreateIcon, &CreateIcon);

	if (ws)
		{
		UndoStep = (APTR) DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_BeginUndoStep);
		}

	if (NewObjName && strlen(NewObjName) > 0)
		{
		if (ws)
			{
			DoMethod(ws->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_AddUndoEvent,
				UNDO_NewDrawer,
				UNDOTAG_UndoMultiStep, UndoStep,
				UNDOTAG_IconDirLock, wbMsg->sm_ArgList[1].wa_Lock,
				UNDOTAG_IconName, NewObjName,
				UNDOTAG_CreateIcon, CreateIcon,
				TAG_END
				);
			}

		newDirLock = CreateDir(NewObjName);

		d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
		
		if (newDirLock)
			{
			if (CreateIcon)
				{
				Object *IconObj;

				d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
				
				IconObj	= GetDefIconObject(WBDRAWER, NULL);
				
				d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
				
				if (IconObj)
					{
					PutIconObjectTags(IconObj, NewObjName,
						TAG_END);
					DisposeIconObject(IconObj);
					}
				else
					Result = IoErr();
				}

			UnLock(newDirLock);
			}
		else
			{
			Result = IoErr();
			}
		}

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
	
	CurrentDir(oldDir);

	if (ws)
		{
		DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_EndUndoStep, UndoStep);
		SCA_UnLockWindowList();
		}
	if (Result)
		ReportError(Result, NewObjName);

	return Result;
}


//----------------------------------------------------------------------------

static STRPTR GetLocString(ULONG MsgId)
{
	struct NewDrawer_LocaleInfo li;

	li.li_Catalog = gb_Catalog;
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetNewDrawerString(&li, MsgId);
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg)
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

static SAVEDS(void) INTERRUPT OpenAboutHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	MUI_Request(APP_Main, WIN_Main, 0, NULL,
		GetLocString(MSGID_ABOUTREQOK),
		GetLocString(MSGID_ABOUTREQFORMAT),
		VERSION_MAJOR, VERSION_MINOR, COMPILER_STRING, CURRENTYEAR);
}

//----------------------------------------------------------------------------

// return TRUE if Disk is writable
static BOOL CheckInfoData(const struct InfoData *info)
{
	BOOL Result = TRUE;

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

//----------------------------------------------------------------------------

//  Checks if a certain version of a MCC is available
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev)
{
	BOOL flush = TRUE;

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: %s ", name, __LINE__);)

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
				if (MUI_Request(NULL, NULL, 0L, GetLocString(MSGID_STARTUP_FAILURE),
					GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD), GetLocString(MSGID_STARTUP_MCC_IN_USE),
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
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: couldn`t find minimum required version.\n", __LINE__);)

				// We're out of luck - open count is 0, we've tried to flush
				// and still haven't got the version we want
				if (MUI_Request(NULL, NULL, 0L, GetLocString(MSGID_STARTUP_FAILURE),
					GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD), GetLocString(MSGID_STARTUP_OLD_MCC),
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
			if (!MUI_Request(NULL, NULL, 0L, GetLocString(MSGID_STARTUP_FAILURE),
				GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD), GetLocString(MSGID_STARTUP_MCC_NOT_FOUND),
				name, minver, minrev))
				{
				break;
				}
			}
		}

	return FALSE;
}

//----------------------------------------------------------------------------
// +jmc+
static void ReadPrefs(STRPTR PrefsFile)
{
	struct PrefsHandle *Nd_prefhandle;
	LONG ID_GAD;
	struct PrefsStruct  *prefstruct;
	BPTR EnvLock;

	if (PreferencesBase)
		{
		Nd_prefhandle = AllocPrefsHandle("NDrawer");
		ID_GAD = MAKE_ID('M','A','I','N');
		if(Nd_prefhandle)
			{
			if(EnvLock = Lock(PrefsFile, ACCESS_READ))
				{
				ReadPrefsHandle(Nd_prefhandle, PrefsFile);
				if(NULL != (prefstruct = FindPreferences(Nd_prefhandle, ID_GAD, NDP_IconCreation)) )
					GetPreferences(Nd_prefhandle, ID_GAD, NDP_IconCreation, &gd_Selected, sizeof(gd_Selected) );

				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ID_GAD=%08lx NDP_IconCreation=%08lx gd_Selected = %ld\n", __LINE__, ID_GAD, NDP_IconCreation, gd_Selected));

				UnLock(EnvLock);
				}

			FreePrefsHandle(Nd_prefhandle);
			}
		}
}

//----------------------------------------------------------------------------
// +jmc+
static void WritePrefs(STRPTR PrefsFile)
{
	APTR Nd_prefhandle;
	LONG ID_GAD;

	if (PreferencesBase)
		{
		Nd_prefhandle = AllocPrefsHandle("NDrawer");
		ID_GAD = MAKE_ID('M','A','I','N');
		if(Nd_prefhandle)
			{
			gd_Selected = mui_getv(CheckmarkCreateIcon, MUIA_Selected);

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: gd_Selected = %ld\n", __LINE__, gd_Selected));
		
			SetPreferences(Nd_prefhandle, ID_GAD, NDP_IconCreation, &gd_Selected, sizeof(gd_Selected) );

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ID_GAD=%08lx NDP_IconCreation=%08lx gd_Selected = %ld\n", __LINE__, ID_GAD, NDP_IconCreation, gd_Selected));

			WritePrefsHandle(Nd_prefhandle, PrefsFile);

			FreePrefsHandle(Nd_prefhandle);
			}
		}
}

//----------------------------------------------------------------------------

IPTR mui_getv(APTR obj, ULONG attr)
{
    IPTR v;
    GetAttr(attr, obj, &v);
    return (v);
}

//----------------------------------------------------------------------------

static struct ScaWindowStruct *FindScalosWindow(BPTR dirLock)
{
	struct ScaWindowList *wl;
	BOOL Found = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: START \n", __FUNC__, __LINE__));
	debugLock_d1(dirLock);

	wl = SCA_LockWindowList(SCA_LockWindowList_Shared);

	if (wl)
		{
		struct ScaWindowStruct *ws;

		for (ws = wl->wl_WindowStruct; !Found && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (((BPTR)NULL == dirLock && (BPTR)NULL == ws->ws_Lock) || (LOCK_SAME == SameLock(dirLock, ws->ws_Lock)))
				{
				d1(KPrintF(__FILE__ "/%s/%ld: END  ws=%08lx\n", __FUNC__, __LINE__, ws));
				return ws;
				}
			}
		SCA_UnLockWindowList();
		}

	d1(KPrintF(__FILE__ "/%s/%ld: END  (not found)\n", __FUNC__, __LINE__));
	return NULL;
}

//----------------------------------------------------------------------------

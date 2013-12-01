// Execute_Command.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/alerts.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/classusr.h>
#include <libraries/mui.h>
#include <mui/BetterString_mcc.h>
#include <libraries/asl.h>
#include <libraries/openurl.h>	// +jmc+
#include <workbench/startup.h>
#include <scalos/scalos.h>	// +jmc+

#include <clib/alib_protos.h>
#include <clib/scalos_protos.h>	//+jmc+

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/scalos.h>	// +jmc+
#include <proto/graphics.h> 	// +jmc+
#include <proto/openurl.h>	// +jmc+

#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <proto/asl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include "Execute_Command.h"

#define	Execute_Command_NUMBERS
#define	Execute_Command_ARRAY
#define	Execute_Command_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#define	VERSION_MAJOR	40
#define	VERSION_MINOR	4

//----------------------------------------------------------------------------

#define	Application_Return_Ok		1001
#define Application_Return_OpenAsl	1002
#define Application_Return_GetDefStack	1003

//----------------------------------------------------------------------------

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

#define CheckMarkHelp(InputMode, Selected, Image,HelpTextID, NumID)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_InputMode      , InputMode,\
		MUIA_Image_Spec     , Image,\
		MUIA_Image_FreeVert , TRUE,\
		MUIA_Background     , MUII_ButtonBack,\
		MUIA_ShowSelState   , Selected,\
		MUIA_CycleChain     , TRUE,\
		MUIA_ShortHelp	    , GetLocString(HelpTextID), \
		MUIA_ExportID       ,	NumID, \
		End

//----------------------------------------------------------------------------

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static void ReportError(LONG Result, CONST_STRPTR CmdString);

static LONG ExecuteCommand(struct WBArg *arg);

static void InitCurrent(struct WBArg *arg);	//+jmc+
static void WBLRequestFile(struct WBArg *arg);	//+jmc+

static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT StringMUIHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT IntegerMUIHookFunc(struct Hook *hook, Object *o, Msg msg);

static STRPTR GetLocString(ULONG MsgId);
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev);
static STRPTR BuildConsole(STRPTR wtitle, struct Screen *scr);	//+jmc+
static void MakeAPPSleep(BOOL etat);				//+jmc+
static BOOL TrackSpace(STRPTR Buffer);				//+jmc+
static BOOL FindURL(STRPTR StringExec);				//+jmc+
static void GetDefStack(void);					//+jmc+
BPTR DupWBPathList(void);					//+jmc+
static void SaveToEnv(void);					//+jmc+

//----------------------------------------------------------------------------

#if !defined(__AROS__) && !defined(__amigaos4__)
#include <dos.h>

long _stack = 16384;	        // minimum stack size, used by SAS/C startup code
#endif

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase = NULL;
T_LOCALEBASE LocaleBase = NULL;
#ifndef __amigaos4__
T_UTILITYBASE UtilityBase = NULL;
#endif
struct Library *MUIMasterBase = NULL;
struct ScalosBase *ScalosBase = NULL;     //+jmc+

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition = NULL;
struct LocaleIFace *ILocale = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct ScalosIFace *IScalos = NULL;
#endif

static struct Catalog *gb_Catalog;

static struct Hook AboutHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutHookFunc), NULL };
static struct Hook AboutMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIHookFunc), NULL };
static struct Hook StringMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(StringMUIHookFunc), NULL };	     //+jmc+
static struct Hook IntegerMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(IntegerMUIHookFunc), NULL };	     //+jmc+

static struct Screen *scr;	//+jmc+

//----------------------------------------------------------------------------

static Object *Group_Buttons2;
static Object *APP_Main;
static Object *WIN_Main;
static Object *WIN_AboutMUI;
static Object *OkButton, *CancelButton;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit;
static Object *TextEnterCommand;
static Object *PopAslCommand;
static Object *StringName;	// +jmc+ String gadget replacing PopaslObject.
static Object *TextEnterName;	// +jmc+ TextObject for Current dir.
static Object *CheckQuit;	// +jmc+ CheckMark to quit or notafter execute command finished.
static Object *Group_Virtual;	//+jmc+ To switch 0/1 "MUIA_ShowMe" the ScroolGroupObject group.
static Object *StringStack;	//+jmc+ Stack integer
static Object *GetStack;	//+jmc+ Get default stack button
static BOOL Run = TRUE;
static BOOL Success = FALSE;
static ULONG stackNum;		//+jmc+
static ULONG oldstackNum=0;   	//+jmc+
static ULONG PrevInteger;	//+jmc+

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

		// kprintf("NumArgs=%ld\n", WBenchMsg->sm_NumArgs);

		for (n=0; n<WBenchMsg->sm_NumArgs; n++)
			{
			char xName[512];

			NameFromLock(WBenchMsg->sm_ArgList[n].wa_Lock, xName, sizeof(xName));

			// kprintf("%ld. Lock=<%s>  Name=<%s>\n", n, xName, WBenchMsg->sm_ArgList[n].wa_Name);
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
		ULONG QuitSelected = 0;
		ULONG StackSelected = 0;

		struct WBArg *arg = &WBenchMsg->sm_ArgList[1];

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		if (!OpenLibraries())
			break;

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		if (!CheckMCCVersion(MUIC_BetterString, 11, 0))
			break;

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		GetDefStack();

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		gb_Catalog = OpenCatalogA(NULL, "Scalos/Execute_Command.catalog",NULL);

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		APP_Main = ApplicationObject,
			MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
			MUIA_Application_Version,	"$VER: Scalos Execute_Command.module V40.5 (" __DATE__ ") " COMPILER_STRING,
			MUIA_Application_Copyright,	"© The Scalos Team, 2004" CURRENTYEAR,
			MUIA_Application_Author,	"The Scalos Team",
			MUIA_Application_Description,	"Scalos Execute_Command module",
			MUIA_Application_Base,		"SCALOS_EXECUTE_COMMAND",

			SubWindow, WIN_Main = WindowObject,
				MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
//				MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
				MUIA_Window_AppWindow, TRUE,
				MUIA_Window_UseBottomBorderScroller, TRUE, 	// +jmc+

				MUIA_Window_TopEdge, MUIV_Window_TopEdge_Moused,
				MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Moused,

				MUIA_Window_Width, MUIV_Window_Width_Screen(30), //+jmc+

				WindowContents, VGroup,
					Child, 	GroupObject, // +jmc+
						Child, Group_Virtual = ScrollgroupObject,
							MUIA_Scrollgroup_VertBar, NULL,
							MUIA_Scrollgroup_HorizBar, NULL,
							MUIA_Scrollgroup_FreeHoriz, TRUE,
							MUIA_Scrollgroup_FreeVert, FALSE,
							MUIA_Scrollgroup_UseWinBorder, TRUE,
							MUIA_Scrollgroup_Contents,
							VirtgroupObject,
								Child, TextEnterName = TextObject,
									MUIA_Text_PreParse, MUIX_C,
									MUIA_ShortHelp, GetLocString(MSGID_CURRENTDIR_SHORTHELP),
									End, //TextObject
							End, //VirtgroupObject
						End, //ScrollgroupObject
					End, //GroupObject

					Child, VGroup,
						Child, TextEnterCommand = TextObject,
							MUIA_Text_PreParse, MUIX_C,
							MUIA_Text_Contents, GetLocString(MSGID_TEXT_ENTER_COMMAND),
							End, //TextObject

						Child, VSpace(1),

						Child, HGroup, MUIA_Group_Spacing, 0,
							Child, Label1(GetLocString(MSGID_LABEL_COMMAND)),
							Child, StringName = BetterStringObject,
								StringFrame,
								MUIA_String_Contents, "",
								MUIA_CycleChain, TRUE,
								MUIA_ExportID, 1,
								End, //BetterStringObject
							Child, PopAslCommand = CheckMarkHelp(MUIV_InputMode_RelVerify, TRUE,\
											MUII_PopFile,\
											MSGID_ASL_REQUESTER_SHORTHELP, NULL),
						End, //HGroup
					End, //VGroup

					Child, HGroup,
						Child, RectangleObject,
							End,
						Child, Label1(GetLocString(MSGID_LABEL_STACK)),
						Child, HGroup, MUIA_Group_Spacing, 0,
							Child, StringStack = BetterStringObject, StringFrame,
								MUIA_String_Integer, stackNum,
								MUIA_String_Accept , "0123456789",
								MUIA_String_Format, MUIV_String_Format_Center,
								MUIA_CycleChain, TRUE,
							End, //	BetterStringObject
							Child, GetStack = CheckMarkHelp(MUIV_InputMode_Toggle, FALSE,\
										MUII_CheckMark,\
										MSGID_GET_DEFSTACK_SHORTHELP, NULL),
						End, //HGroup

						Child, HGroup,
							Child, Label1(GetLocString(MSGID_LABEL_CHECKQUIT)),
							Child, CheckQuit = CheckMarkHelp(MUIV_InputMode_Toggle, FALSE,\
									MUII_CheckMark,\
									MSGID_CHECKQUIT_SHORTHELP,2),

						End, // HGroup
						Child, RectangleObject,
							End,
					End, // HGroup

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

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

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

		DoMethod(PopAslCommand, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, Application_Return_OpenAsl);


		DoMethod(StringName, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, Application_Return_Ok);

		DoMethod(StringStack, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, Application_Return_GetDefStack);


		DoMethod(CheckQuit, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			WIN_Main, 3, MUIM_Set, MUIA_Window_ActiveObject, StringName);

		DoMethod(GetStack, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			APP_Main, 3, MUIM_Application_ReturnID, Application_Return_GetDefStack);


		DoMethod(StringName, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
			StringName, 3, MUIM_CallHook, &StringMUIHook);

		DoMethod(StringStack, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			StringStack, 2, MUIM_CallHook, &IntegerMUIHook);

		DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutHook);

		DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutMUIHook);

		DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


		set(WIN_Main, MUIA_Window_ActiveObject, StringName);

		set(GetStack, MUIA_Selected, TRUE);

		// Loading of MUI objects ID settings.
		DoMethod(APP_Main, MUIM_Application_Load,MUIV_Application_Load_ENV);

		InitCurrent(arg);

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
				ExecuteCommand(&WBenchMsg->sm_ArgList[1]);
				SaveToEnv();
				get(CheckQuit, MUIA_Selected, &QuitSelected);
				if (!QuitSelected)
					Run = FALSE;
				break;
			case Application_Return_OpenAsl:
				set(Group_Virtual, MUIA_ShowMe, 0);
				WBLRequestFile(arg);
				set(Group_Virtual, MUIA_ShowMe, 1);
				if (Success)
					{
					SaveToEnv();
					get(CheckQuit, MUIA_Selected, &QuitSelected);
					// kprintf("QuitSelected= [%ld] Success=[%ld]\n", QuitSelected, Success);
					if (!QuitSelected)
						Run = FALSE;
					}
				break;
			case Application_Return_GetDefStack:
				GetDefStack();
				// kprintf("Selected: oldstackNum =[ %ld ] stackNum=[ %ld ]\n", oldstackNum, stackNum);
				get(GetStack, MUIA_Selected, &StackSelected);
				if(oldstackNum>=stackNum)
					{
					if(StackSelected)
						{
						set(StringStack, MUIA_String_Integer, stackNum);
						set(StringStack, MUIA_Disabled, TRUE);
						}
					else
						{
						set(StringStack, MUIA_Disabled, FALSE);
						set(StringStack, MUIA_String_Integer, oldstackNum);
						set(WIN_Main, MUIA_Window_ActiveObject, StringStack);
						}
					}
				else
					{
					set(StringStack, MUIA_String_Integer, stackNum);
					set(StringStack, MUIA_Disabled, TRUE);
					oldstackNum=stackNum;
					}
				// kprintf("oldstackNum =[ %ld ] stackNum=[ %ld ]\n", oldstackNum, stackNum);
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
		} while (0);

	set(WIN_Main, MUIA_Window_Open, FALSE);

	if (APP_Main)
		MUI_DisposeObject(APP_Main);
	if(gb_Catalog)
		CloseCatalog(gb_Catalog);
		
	CloseLibraries();

	return 0;
}

//----------------------------------------------------------------------------
// +jmc+
// Export all IDs of MUI objects to ENV:MUI.
// Availlable for "Application_Return_Ok" and "Application_Return_OpenAsl",

static void SaveToEnv(void)
{
	DoMethod(APP_Main, MUIM_Application_Save,MUIV_Application_Save_ENV);
}

//----------------------------------------------------------------------------

static BOOL OpenLibraries(void)
{
	ScalosBase = (struct ScalosBase *) OpenLibrary("scalos.library", 40);
	if (NULL == ScalosBase)
		return FALSE;
#ifdef __amigaos4__
	IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
	if (NULL == IScalos)
		return FALSE;
#endif

	MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		return FALSE;
#ifdef __amigaos4__
	IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
	kprintf("MUIMasterBase = %lx, IMUIMaster = %lx\n", MUIMasterBase, IMUIMaster);
	if (NULL == IMUIMaster)
		return FALSE;

#endif

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 38);
	if (NULL == LocaleBase)
		return FALSE;
#ifdef __amigaos4__
	ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
	if (NULL == ILocale)
		return FALSE;
#endif

#ifndef __amigaos4__
	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;
#endif

	return TRUE;
}

//----------------------------------------------------------------------------

static void CloseLibraries(void)
{
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
		CloseLibrary((struct Library *) LocaleBase);
		LocaleBase = NULL;
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
#ifdef __amigaos4__
	if (IScalos)
		{
		DropInterface((struct Interface *)IScalos);
		IScalos = NULL;
		}
#endif
	if (ScalosBase)
		{
		CloseLibrary(&ScalosBase->scb_LibNode);
		ScalosBase = NULL;
		}
}

//----------------------------------------------------------------------------

static void ReportError(LONG Result, CONST_STRPTR CmdString)
{
	char FaultText[256];

	Fault(Result, "", FaultText, sizeof(FaultText));

	MUI_Request(APP_Main, WIN_Main, 0, NULL,
		GetLocString(MSGID_REQ_OK),
		GetLocString(MSGID_ERRORREQ_BODYTEXT),
		CmdString, FaultText);
}

//----------------------------------------------------------------------------
// +jmc+
static void MakeAPPSleep(BOOL etat)
{
	set(APP_Main, MUIA_Application_Sleep, etat);
}


//----------------------------------------------------------------------------
// +jmc+
static void GetDefStack(void)
{
	SCA_ScalosControl(NULL,
		SCALOSCTRLA_GetDefaultStackSize, (ULONG) &stackNum,
		TAG_END);
}

//----------------------------------------------------------------------------
// +jmc+
static void InitCurrent(struct WBArg *arg)
{
	char xName[512];
	char LabelText[512];
	char SpaceName[256];
	STRPTR LastContent = NULL;

	NameFromLock(arg->wa_Lock, xName, sizeof(xName));
	sprintf(LabelText, GetLocString(MSGID_CURRENTDIR), xName);

	set(TextEnterName, MUIA_Text_Contents, LabelText);

	if (arg->wa_Name && strlen(arg->wa_Name) > 0)
		{
		BOOL Space;

		if ( Space = TrackSpace(arg->wa_Name) )
			sprintf(SpaceName, "\"%s\"", arg->wa_Name);
		else
			stccpy(SpaceName, arg->wa_Name, sizeof(SpaceName));

		set(StringName,	MUIA_String_Contents, SpaceName);
 		set(StringName,	MUIA_String_BufferPos, 0);
		}
	else
		{
		get(StringName,	MUIA_String_Contents, &LastContent);
		// stccpy(SpaceName, LastContent, sizeof(SpaceName));
		// if(strlen(SpaceName) == 0)
		if(strlen(LastContent) == 0)
			set(OkButton, MUIA_Disabled, TRUE);
		}

}

//----------------------------------------------------------------------------

static LONG ExecuteCommand(struct WBArg *arg)
{
	STRPTR CommandString = NULL;
	STRPTR StackDisabled = NULL;
	BPTR oldDir;
	LONG Result = RETURN_OK;
	STRPTR conDef = "CON:////Execute_Command/AUTO/CLOSE/WAIT";
	BOOL GetUrl;

	GetDefStack();

	get(StringStack, MUIA_Disabled, &StackDisabled);
	if (!StackDisabled)
		set(StringStack, MUIA_String_Acknowledge, TRUE);
	else
		{
		PrevInteger = oldstackNum;
		oldstackNum = stackNum;
		}

	if(oldstackNum>stackNum)
			stackNum = oldstackNum;
			
	get(StringName, MUIA_String_Contents, &CommandString);

	if ( GetUrl = FindURL(CommandString) )
		{
		struct Library *OpenURLBase;
#ifdef __amigaos4__
		struct OpenURLIFace *IOpenURL;
#endif

		OpenURLBase = OpenLibrary(OPENURLNAME, 3);
#ifdef __amigaos4__
		IOpenURL = (struct OpenURLIFace *)GetInterface(OpenURLBase, "main", 1, NULL);
		if (NULL == IOpenURL)
			{
			CloseLibrary(OpenURLBase);
			OpenURLBase = NULL;
			}
#endif
		if (NULL != OpenURLBase)
			{
			URL_Open(CommandString,
				 URL_Launch,	TRUE,
				 URL_Show,	TRUE,
				 TAG_DONE);

#ifdef __amigaos4__
			DropInterface((struct Interface *)IOpenURL);
#endif
			CloseLibrary(OpenURLBase);
			}
		else
			printf(GetLocString(MSGID_OPEN_LIBRARY_FAILED), OPENURLNAME); 
		}
	else
		{
		oldDir = CurrentDir(arg->wa_Lock);

		if(scr = LockPubScreen(NULL))
			{
			STRPTR conName;
			BOOL ConBuilt = TRUE;

			conName = BuildConsole(GetLocString(MSGID_CONSOLE_NAME),scr);
			if(!conName)
				{
				ConBuilt = FALSE;
				conName = conDef;
				}

			//SystemTagList()
			Result = SystemTags(CommandString,
				SYS_Asynch, TRUE,
				SYS_Output, NULL,
				SYS_Input, Open(conName,MODE_OLDFILE),
				SYS_UserShell, TRUE,
				NP_StackSize, stackNum,
				NP_Path, DupWBPathList(),
				TAG_DONE);

			if (ConBuilt)
				FreeVec(conName);

			// kprintf("ConBuilt=[ %ld ] - stackNum=[ %ld ] oldstackNum=[ %ld ]\n", ConBuilt, stackNum, oldstackNum);

			if (RETURN_OK != Result)
				ReportError(Result, CommandString);

			CurrentDir(oldDir);

			UnlockPubScreen(NULL,scr);

			oldstackNum = PrevInteger;
			}
		}
	return Result;
}

//----------------------------------------------------------------------------
// +jmc+
static BOOL FindURL(STRPTR StringExec)
{
	BOOL URLFound = FALSE;

	if ( (strnicmp("HTTP://",StringExec,7) == 0) || (strnicmp("FILE://",StringExec,7) == 0)
		 || (strnicmp("FTP://",StringExec,6) == 0) || (strnicmp("MAILTO:",StringExec,7) == 0) )
		URLFound = TRUE;

	return URLFound;
}

//----------------------------------------------------------------------------
// +jmc+
static STRPTR BuildConsole(STRPTR wtitle, struct Screen *scr)
{
  const STATIC STRPTR fmt = "CON:%ld/%ld/%ld/%ld/%s/AUTO/CLOSE/WAIT";

	STRPTR buf = (STRPTR)AllocVec( strlen(fmt)+strlen(wtitle)+40,0 );
	if(buf)
		{
		LONG height=280;
		LONG width =120;
		LONG top=8;
		LONG left=8*2;

		if(scr)
			{
			struct Rectangle rect;
			if(QueryOverscan(GetVPModeID( &scr->ViewPort), &rect, OSCAN_TEXT))
				{
				height = rect.MaxY - rect.MinY + 1;
				width  = rect.MaxX - rect.MinX + 1;
				} 
			else
				{
				height = scr->Height;
				width  = scr->Width;
				}
			height = height*25/100;

			width  = (width*75/100)-((scr->BarHeight*2)*75/100);

			left   = (scr->Width/2)-(width/2)-scr->BarHeight;

			top = -scr->TopEdge;

			if( top <= scr->BarHeight )
				top = scr->BarHeight + 1;
			}

		sprintf(buf, fmt, left, top, width, height, wtitle);
		// kprintf("buf=<%s>\nLeft=<%ld>\nTop=<%ld>\nWidth=<%ld>\nHeight=<%ld>\nWtitle=<%s>\n\n", buf, left, top, width, height, wtitle);
		}
	return buf;
}

//----------------------------------------------------------------------------

static STRPTR GetLocString(ULONG MsgId)
{
	struct Execute_Command_LocaleInfo li;

	li.li_Catalog = gb_Catalog;
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetExecute_CommandString(&li, MsgId);
}

/*
static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}
*/
//----------------------------------------------------------------------------
// +jmc+
static void WBLRequestFile(struct WBArg *arg)
{
	char Path[1024];
	char SpacePath[1024];
	struct FileRequester *fileReq = NULL;

	do	{
		STRPTR FileName;
		Success = FALSE;

		if (!NameFromLock(arg->wa_Lock, Path, sizeof(Path)))
			break;

		if (arg->wa_Name && strlen(arg->wa_Name) > 0)
			FileName = arg->wa_Name;
		else
			FileName = "";

		// AllocAslRequest()
		fileReq = AllocAslRequestTags(ASL_FileRequest,
				ASLFR_TitleText, (ULONG) GetLocString(MSGID_COMMAND_ASLTITLE),
				ASLFR_DoSaveMode, FALSE,
				ASLFR_RejectIcons, FALSE,
				TAG_END);

		if (NULL == fileReq)
			break;

		MakeAPPSleep(TRUE); // APP_Main Sleep

		// AslRequest()
		if (AslRequestTags(fileReq,
				ASLFR_InitialFile, (ULONG) FileName,
				ASLFR_InitialDrawer, (ULONG) Path,
				TAG_END))
			Success = TRUE;
		else
			Success = FALSE;

		MakeAPPSleep(FALSE); // APP_Main Sleep

		if (Success)
			{
			BPTR dirLock;
			char xName[512];
			char LabelText[512];
			BOOL Space;

			dirLock = Lock(fileReq->fr_Drawer, ACCESS_READ);
			if ((BPTR)NULL == dirLock)
				{
				break;
				}

			NameFromLock(dirLock, Path, sizeof(Path));
			sprintf(LabelText, GetLocString(MSGID_CURRENTDIR), Path);
			stccpy(xName, fileReq->fr_File, sizeof(xName));

			if (strlen(xName) > 0)
				{
				AddPart(Path, xName, sizeof(xName));
				}

			set(TextEnterName, MUIA_Text_Contents, LabelText);

			if ( Space = TrackSpace(Path) )
				{
				sprintf(SpacePath, "\"%s\"", Path);
				set(StringName, MUIA_String_Contents, SpacePath);
				// kprintf("Space [%ld] - SpacePath=<%s>\n", Space, SpacePath);
				}
			else
				{
				set(StringName, MUIA_String_Contents, Path);
				}

			UnLock(arg->wa_Lock);
			arg->wa_Lock = dirLock;
			arg->wa_Name = NULL;

			ExecuteCommand(arg);
			}
		} while (0);

	if (fileReq)
		FreeAslRequest(fileReq);

}

//----------------------------------------------------------------------
// +jmc+
static BOOL TrackSpace(STRPTR Buffer)
{
	BOOL FindSpace = FALSE;
	LONG i=0; 

	while (Buffer[i])
		{
		if (Buffer[i] == ' ')
			{
			FindSpace = TRUE;
			break;
			}
		i++;
		}
	// kprintf("Lenght = [%ld] - Find Space = [%ld]\n", i, FindSpace);
	return FindSpace;
}

//-------- DupWBPathList() - From in CliStart.c --------------------------
// +jmc+
BPTR DupWBPathList(void)
{
	struct Process *wbProc;
	struct CommandLineInterface *cli;
	struct AssignList *aList, *StartList = NULL, **nList = NULL;

	Forbid();
	wbProc = (struct Process *) FindTask("Workbench");
	Permit();

	if (NULL == wbProc)
		return (BPTR)NULL;
	if (NT_PROCESS != wbProc->pr_Task.tc_Node.ln_Type)
		return (BPTR)NULL;

	cli = BADDR(wbProc->pr_CLI);
	if (NULL == cli)
		return (BPTR)NULL;

	aList = BADDR(cli->cli_CommandDir);
	if (NULL == aList)
		return (BPTR)NULL;

	while (aList)
		{
		struct AssignList *nNode = AllocVec(sizeof(struct AssignList), MEMF_PUBLIC);

		if (NULL == nNode)
			break;
		if (NULL == nList)
			StartList = nNode;
		else
			*nList = (struct AssignList *) MKBADDR(nNode);

		nNode->al_Next = NULL;
		nNode->al_Lock = DupLock(aList->al_Lock);
		nList = &nNode->al_Next;

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: aList=%08lx Next=%08lx Lock=%08lx\n", __LINE__, \
			aList, aList->al_Next, aList->al_Lock));

		aList = BADDR(aList->al_Next);
		}

	return MKBADDR(StartList);
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
// +jmc+
static SAVEDS(void) INTERRUPT StringMUIHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	STRPTR	defstring = NULL;

	get(StringName, MUIA_String_Contents, &defstring);
	if(defstring && strlen(defstring) > 0)
		set(OkButton, MUIA_Disabled, FALSE);
	else
		set(OkButton, MUIA_Disabled, TRUE);
}

//----------------------------------------------------------------------------
// +jmc+
static SAVEDS(void) INTERRUPT IntegerMUIHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	get(StringStack, MUIA_String_Integer, &PrevInteger);
	if(PrevInteger<stackNum)
		set(GetStack, MUIA_Selected, TRUE);
	else
		oldstackNum = PrevInteger;

	// kprintf("Acknowledge: oldstackNum =[ %ld ] stackNum=[ %ld ] PrevInteger= [ %ld ]\n", oldstackNum, stackNum, PrevInteger);
}

//-----------------------------------------------------------------------------

//  Checks if a certain version of a MCC is available
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev)
{
	BOOL flush = TRUE;

	d1(kprintf(__FILE__ "/%s/%ld: %s\n", __FUNC__, __LINE__, name);)

	while (1)
		{
		ULONG ver = 0;
		ULONG rev = 0;
		struct Library *base;
		char libname[256];
		Object *obj;

		d1(kprintf(__FILE__ "/%s/%ld: MUIMasterBase=%lx IMUIMaster=%lx\n",
			__FUNC__, __LINE__, MUIMasterBase, IMUIMaster));
		// First we attempt to acquire the version and revision through MUI
		obj = MUI_NewObjectA((STRPTR) name, NULL);
		d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__);)
		if (obj)
			{
			get(obj, MUIA_Version, &ver);
			get(obj, MUIA_Revision, &rev);

			d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__);)
			
			MUI_DisposeObject(obj);

			if(ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf(__FILE__ "/%s/%ld: v%ld.%ld found through MUIA_Version/Revision\n", __FUNC__, __LINE__, ver, rev);)
				return TRUE;
				}
			}

		// If we did't get the version we wanted, let's try to open the
		// libraries ourselves and see what happens...
		stccpy(libname, "PROGDIR:mui", sizeof(libname));
		AddPart(libname, name, sizeof(libname));

		d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__);)

		if ((base = OpenLibrary(&libname[8], 0)) || (base = OpenLibrary(&libname[0], 0)))
			{
			UWORD OpenCnt = base->lib_OpenCnt;

			d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__);)

			ver = base->lib_Version;
			rev = base->lib_Revision;

			CloseLibrary(base);

			// we add some additional check here so that eventual broken .mcc also have
			// a chance to pass this test (i.e. Toolbar.mcc is broken)
			if (ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf(__FILE__ "/%s/%ld: v%ld.%ld found through OpenLibrary()\n", __FUNC__, __LINE__, ver, rev);)
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
			d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__);)

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


// Palette.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <exec/execbase.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <prefs/prefhdr.h>

#include <clib/alib_protos.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/scalosprefsplugin.h>

#include <scalos/palette.h>
#include <scalos/scalosprefsplugin.h>

#include <defs.h>
#include <Year.h> // +jmc+
#include <scalosdebug.h>

struct ScalosPalette_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#define	ScalosPalette_NUMBERS
#define	ScalosPalette_ARRAY
#define	ScalosPalette_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#define	Sizeof(array)	(sizeof(array) / sizeof((array)[0]))

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
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_CycleChain   , TRUE, \
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp, HelpText,\
		End


#define	Application_Return_EDIT	0
#define	Application_Return_USE	1001
#define	Application_Return_SAVE	1002

//----------------------------------------------------------------------------

// local functions

static void init(void);
static void fail(Object *APP_Main, CONST_STRPTR str);
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static STRPTR GetLocString(ULONG MsgId);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg);
static struct MUI_CustomClass *OpenPrefsModule(CONST_STRPTR FileName);
static struct TagItem *CreatePluginSubWindowArray(void);
static void DisposePrefsPlugin(void);
static BOOL CheckMCCforPlugin(STRPTR *UsedClasses, size_t MaxUsedClasses);
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev);
#if !defined(__SASC) &&!defined(__MORPHOS__)
size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !defined(__SASC) &&!defined(__MORPHOS__)  */

//----------------------------------------------------------------------------

// local data items

struct Library *MUIMasterBase;
T_LOCALEBASE LocaleBase;
struct Library *IconBase;
struct IntuitionBase *IntuitionBase;
struct Library *ScalosPrefsPluginBase;

#ifdef __amigaos4__
extern struct Library *SysBase;
struct MUIMasterIFace *IMUIMaster;
struct LocaleIFace *ILocale;
struct IconIFace *IIcon;
struct IntuitionIFace *IIntuition;
struct ScalosPrefsPluginIFace *IScalosPrefsPlugin;
#endif

static struct MUI_CustomClass *PalettePluginClass;

static struct Hook OpenAboutMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIFunc), NULL };

static struct TagItem *SubWindowTagList;

static struct Catalog *PaletteCatalog;

static Object **PluginSubWindows;
static Object *APP_Main;
static Object *WIN_Main;
static Object *WIN_AboutMUI;
static Object *Group_Plugin;
static Object *SaveButton, *UseButton, *CancelButton;
static Object *MenuOpen, *MenuSaveAs, *MenuAbout, *MenuAboutMUI, *MenuQuit;
static Object *MenuResetToDefaults, *MenuLastSaved, *MenuRestore;
static Object *MenuCreateIcons;

//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	LONG Action = Application_Return_EDIT;
	LONG win_opened = 0;
	struct RDArgs *rdArgs = NULL;
	CONST_STRPTR GivenFileName = NULL;
	BPTR oldDir = (BPTR)NULL;
	ULONG fCreateIcons = TRUE;
	struct DiskObject *PaletteDiskObject = NULL;
	STRPTR ProgramName;
	STRPTR UsedClasses[32];
	struct WBStartup *WBenchMsg =
		(argc == 0) ? (struct WBStartup *)argv : NULL;

	init();

	if (WBenchMsg && WBenchMsg->sm_ArgList)
		{
		static char PrgNamePath[512];
		struct WBArg *arg;
		struct DiskObject *icon;

		if (WBenchMsg->sm_NumArgs > 1)
			{
			arg = &WBenchMsg->sm_ArgList[1];
			GivenFileName = arg->wa_Name;
			}
		else
			{
			arg = &WBenchMsg->sm_ArgList[0];
			}

		ProgramName = PrgNamePath;

		NameFromLock(WBenchMsg->sm_ArgList[0].wa_Lock, PrgNamePath, sizeof(PrgNamePath));
		AddPart(PrgNamePath, WBenchMsg->sm_ArgList[0].wa_Name, sizeof(PrgNamePath));

		oldDir = CurrentDir(arg->wa_Lock);

		icon = GetDiskObject(arg->wa_Name);
		if (icon)
			{
			STRPTR tt;

			tt = FindToolType(icon->do_ToolTypes, "CREATEICONS");
			if (tt)
				{
				if (MatchToolValue(tt, "NO"))
					fCreateIcons = FALSE;
				}

			tt = FindToolType(icon->do_ToolTypes, "ACTION");
			if (tt)
				{
				if (MatchToolValue(tt, "EDIT"))
					Action = Application_Return_EDIT;
				if (MatchToolValue(tt, "USE"))
					Action = Application_Return_USE;
				if (MatchToolValue(tt, "SAVE"))
					Action = Application_Return_SAVE;
				}

			FreeDiskObject(icon);
			}
		}
	else
		{
		SIPTR ArgArray[4];

		ProgramName = argv[0];

		memset(ArgArray, 0, sizeof(ArgArray));

		rdArgs = ReadArgs("FROM,EDIT/S,USE/S,SAVE/S", ArgArray, NULL);

		if (ArgArray[0])
			GivenFileName = (CONST_STRPTR) ArgArray[0];
		if (ArgArray[1])
			Action = Application_Return_EDIT;
		if (ArgArray[2])
			Action = Application_Return_USE;
		if (ArgArray[3])
			Action = Application_Return_SAVE;
		}

	if (NULL == PalettePluginClass)
		fail(APP_Main, "Failed to open Palette Plugin.");

	PaletteDiskObject = GetDiskObject(ProgramName);

	Group_Plugin = NewObject(PalettePluginClass->mcc_Class, 0,
		MUIA_ScalosPrefs_CreateIcons, fCreateIcons,
		MUIA_ScalosPrefs_ProgramName, (ULONG) ProgramName,
		TAG_END);
	if (NULL == Group_Plugin)
		fail(APP_Main, "Failed to create Group_Plugin.");

	if (!CheckMCCforPlugin(UsedClasses, Sizeof(UsedClasses)))
		fail(APP_Main, "Required MCC missing.");

	SubWindowTagList = CreatePluginSubWindowArray();

	APP_Main = ApplicationObject,
		MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
		MUIA_Application_Version,	"$VER: Scalos Palette 40.11 (" __DATE__ ")" COMPILER_STRING,
		MUIA_Application_Copyright,	"The Scalos Team, 2000" CURRENTYEAR,
		MUIA_Application_Author,	"The Scalos Team",
		MUIA_Application_Description,	"Scalos Palette preferences editor",
		MUIA_Application_Base,		"SCALOS_PALETTE",
		PaletteDiskObject ? MUIA_Application_DiskObject : TAG_IGNORE, PaletteDiskObject,
#if defined(MUIA_Application_UsedClasses)
		MUIA_Application_UsedClasses,	UsedClasses,
#endif /* MUIA_Application_UsedClasses */

		SubWindow, WIN_Main = WindowObject,
			MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
			MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
			WindowContents, VGroup,
				Child,	Group_Plugin,

				Child, ColGroup(3),
					Child, SaveButton = KeyButtonHelp(GetLocString(MSGID_SAVENAME), 's', GetLocString(MSGID_SHORTHELP_SAVEBUTTON)),
					Child, UseButton = KeyButtonHelp(GetLocString(MSGID_USENAME), 'u', GetLocString(MSGID_SHORTHELP_USEBUTTON)),
					Child, CancelButton = KeyButtonHelp(GetLocString(MSGID_CANCELNAME), 'c', GetLocString(MSGID_SHORTHELP_CANCELBUTTON)),
				End,
			End,
		End,

		MUIA_Application_Menustrip, MenustripObject,
			Child, MenuObjectT(GetLocString(MSGID_MENU_PROJECT)),
				Child, MenuOpen = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_OPEN),
					MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_PROJECT_OPEN_SHORT),
				End,
				Child, MenuSaveAs = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_SAVEAS),
					MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_PROJECT_SAVEAS_SHORT),
				End,
				Child, MenuitemObject,
					MUIA_Menuitem_Title, -1,
				End,
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
			Child, MenuObjectT(GetLocString(MSGID_MENU_EDIT)),
				Child, MenuResetToDefaults = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_EDIT_RESETTODEFAULTS),
					MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_EDIT_RESETTODEFAULTS_SHORT),
				End,
				Child, MenuLastSaved = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_EDIT_LASTSAVED),
					MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_EDIT_LASTSAVED_SHORT),
				End,
				Child, MenuRestore = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_EDIT_RESTORE),
					MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_EDIT_RESTORE_SHORT),
				End,
			End, //MenuObjectT
			Child, MenuObjectT(GetLocString(MSGID_MENU_SETTINGS)),
				Child, MenuCreateIcons = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_SETTINGS_CREATEICONS),
					MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_SETTINGS_CREATEICONS_SHORT),
					MUIA_Menuitem_Checkit, TRUE,
					MUIA_Menuitem_Checked, fCreateIcons,
				End,
			End, //MenuObjectT
		End,

		SubWindowTagList ? TAG_MORE : TAG_IGNORE, SubWindowTagList,
	End; //ApplicationObject

	if (NULL == APP_Main)
		{
		MUI_DisposeObject(Group_Plugin);
		Group_Plugin = NULL;
		fail(APP_Main, "Failed to create Application.");
		}

	set(Group_Plugin, MUIA_ScalosPrefs_MainWindow, (ULONG) WIN_Main);
	set(Group_Plugin, MUIA_ScalosPrefs_Application, (ULONG) APP_Main);

	DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
		WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);
	DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
		APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	DoMethod(MenuQuit, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	DoMethod(SaveButton, MUIM_Notify, MUIA_Pressed, FALSE,
		APP_Main, 2, MUIM_Application_ReturnID, Application_Return_SAVE);

	DoMethod(UseButton, MUIM_Notify,MUIA_Pressed,FALSE,
		APP_Main, 2, MUIM_Application_ReturnID, Application_Return_USE);

	DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
		APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	DoMethod(MenuOpen, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
		Group_Plugin, 1, MUIM_ScalosPrefs_OpenConfig);

	DoMethod(MenuSaveAs, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		Group_Plugin, 1, MUIM_ScalosPrefs_SaveConfigAs);

	DoMethod(MenuResetToDefaults, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		Group_Plugin, 1, MUIM_ScalosPrefs_ResetToDefaults);

	DoMethod(MenuLastSaved, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		Group_Plugin, 1, MUIM_ScalosPrefs_LastSavedConfig);

	DoMethod(MenuRestore, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		Group_Plugin, 1, MUIM_ScalosPrefs_RestoreConfig);

	DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		Group_Plugin, 1, MUIM_ScalosPrefs_About);

	DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		APP_Main, 2, MUIM_CallHook, &OpenAboutMUIHook);

	DoMethod(MenuCreateIcons, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
		Group_Plugin, 3, MUIM_Set, MUIA_ScalosPrefs_CreateIcons, MUIV_TriggerValue);

	if (GivenFileName)
		{
		DoMethod(Group_Plugin, MUIM_ScalosPrefs_LoadNamedConfig, GivenFileName);
		}
	else
		{
		DoMethod(Group_Plugin, MUIM_ScalosPrefs_LoadConfig);
		}

	if (Application_Return_EDIT == Action)
		{
		set(WIN_Main, MUIA_Window_Open, TRUE);

		get(WIN_Main, MUIA_Window_Open, &win_opened);

		if (win_opened)
			{
			ULONG sigs = 0;
			BOOL Run = TRUE;

			while (Run)
				{
				Action = DoMethod(APP_Main, MUIM_Application_NewInput, &sigs);

				switch (Action)
					{
				case MUIV_Application_ReturnID_Quit:
				case Application_Return_SAVE:
				case Application_Return_USE:
					Run = FALSE;
					break;
					}

				if (Run && sigs)
					{
					sigs = Wait(sigs | SIGBREAKF_CTRL_C);

					if (sigs & SIGBREAKF_CTRL_C) 
						{
						Run = FALSE;
						break;
						}
					}
				}
			}
		else
			{
			printf("failed to open main window !\n");
			}
		}

	switch (Action)
		{
	case Application_Return_SAVE:
		DoMethod(Group_Plugin, MUIM_ScalosPrefs_SaveConfig);
		break;
	case Application_Return_USE:
		DoMethod(Group_Plugin, MUIM_ScalosPrefs_UseConfig);
		break;
		}

	set(WIN_Main, MUIA_Window_Open, FALSE);

	if (PaletteDiskObject)
		FreeDiskObject(PaletteDiskObject);
	if (oldDir)
		CurrentDir(oldDir);
	if (rdArgs)
		FreeArgs(rdArgs);

	fail(APP_Main, NULL);

	return 0;
}

//----------------------------------------------------------------------------

static VOID fail(Object * APP_Main, CONST_STRPTR str)
{
	if (APP_Main)
		MUI_DisposeObject(APP_Main);

	DisposePrefsPlugin();

	if (PaletteCatalog)
		{
		CloseCatalog(PaletteCatalog);
		PaletteCatalog = NULL;
		}

	CloseLibraries();

	if (str)
		{
		puts(str);
		exit(20);
		}

	exit(0);
}

static void init(void)
{
	APP_Main = NULL;

	if (!OpenLibraries())
		fail(NULL, "Failed to open "MUIMASTER_NAME".");

	if (LocaleBase)
		PaletteCatalog = OpenCatalogA(NULL, "Scalos/ScalosPalette.catalog", NULL);

	PalettePluginClass = OpenPrefsModule("Palette.prefsplugin");
}

//----------------------------------------------------------------------------

static BOOL OpenLibraries(void)
{
	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
#endif

	MUIMasterBase = OpenLibrary("zune.library", 0);
	if (NULL == MUIMasterBase)
		MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		return FALSE;
#ifdef __amigaos4__
	IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
	if (NULL == IMUIMaster)
		return FALSE;
#endif

	IconBase = OpenLibrary("icon.library", 0);
	if (NULL == IconBase)
		return FALSE;
#ifdef __amigaos4__
	IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
	if (NULL == IIcon)
		return FALSE;
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
	if (LocaleBase)
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
#endif

	return TRUE;
}


static void CloseLibraries(void)
{
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

static STRPTR GetLocString(ULONG MsgId)
{
	struct ScalosPalette_LocaleInfo li;

	li.li_Catalog = PaletteCatalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR) GetScalosPaletteString(&li, MsgId);
}

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

static struct MUI_CustomClass *OpenPrefsModule(CONST_STRPTR FileName)
{
	struct MUI_CustomClass *pclass = NULL;

	d1(kprintf(__FUNC__ "/%ld:  FileName=<%s>\n", __LINE__, FileName));

	do	{
		ScalosPrefsPluginBase = OpenLibrary(FileName, 0);
		d1(kprintf(__FUNC__ "/%ld:  ScalosPrefsPluginBase=%08lx\n", __LINE__, ScalosPrefsPluginBase));
		if (NULL == ScalosPrefsPluginBase)
			break;
#ifdef __amigaos4__
		IScalosPrefsPlugin = (struct ScalosPrefsPluginIFace *)GetInterface(
			ScalosPrefsPluginBase, "main", 1, NULL
		);
#endif

		pclass = (struct MUI_CustomClass *) SCAGetPrefsInfo(SCAPREFSINFO_GetClass);
		d1(kprintf(__FUNC__ "/%ld:  PluginClass=%08lx\n", __LINE__, ppl->ppl_PluginClass));
		if (NULL == pclass)
			break;
		} while (0);

	if (NULL == pclass)
		DisposePrefsPlugin();

	return pclass;
}


static struct TagItem *CreatePluginSubWindowArray(void)
{
	struct TagItem *SubWindowTagList;
	struct TagItem *ti;
	ULONG SubWindowCount = 0;
	ULONG n;

	PluginSubWindows = (Object **) DoMethod(Group_Plugin, MUIM_ScalosPrefs_CreateSubWindows);

	for (n=0; PluginSubWindows && PluginSubWindows[n]; n++)
		SubWindowCount++;

	SubWindowTagList = ti = calloc(1 + SubWindowCount, sizeof(struct TagItem));
	if (NULL == SubWindowTagList)
		return NULL;

	for (n=0; PluginSubWindows && PluginSubWindows[n]; n++)
		{
		ti->ti_Tag = MUIA_Application_Window;
		ti->ti_Data = (ULONG) PluginSubWindows[n];
		ti++;
		}

	ti->ti_Tag = TAG_END;

	return SubWindowTagList;
}


static void DisposePrefsPlugin(void)
{
#ifdef __amigaos4__
	if (IScalosPrefsPlugin)
		{
		DropInterface((struct Interface *)IScalosPrefsPlugin);
		IScalosPrefsPlugin = NULL;
		}
#endif
	if (ScalosPrefsPluginBase)
		{
		d1(kprintf(__FUNC__ "/%ld:  Plugin=<%s>  OpenCount=%ld\n", \
			__LINE__, ScalosPrefsPluginBase->lib_Node.ln_Name, ScalosPrefsPluginBase->lib_OpenCnt));

		if (NULL == Group_Plugin)
		CloseLibrary(ScalosPrefsPluginBase);
		ScalosPrefsPluginBase = NULL;
		}
}

static BOOL CheckMCCforPlugin(STRPTR *UsedClasses, size_t MaxUsedClasses)
{
	const struct MUIP_ScalosPrefs_MCCList *RequiredMccList = NULL;

	RequiredMccList	= (const struct MUIP_ScalosPrefs_MCCList *) DoMethod(Group_Plugin, MUIM_ScalosPrefs_GetListOfMCCs);

	while (RequiredMccList && RequiredMccList->MccName)
		{
		if (!CheckMCCVersion(RequiredMccList->MccName,
			RequiredMccList->MccMinVersion,
			RequiredMccList->MccMinRevision))
			return FALSE;

		if (MaxUsedClasses > 1)
			{
			*UsedClasses++ = (STRPTR) RequiredMccList->MccName;
			MaxUsedClasses--;
			}

		RequiredMccList++;
		}

	*UsedClasses = NULL;

	return TRUE;
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
					(ULONG) name, minver, minrev, ver, rev))
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
					(ULONG) name, minver, minrev, ver, rev))
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
				(ULONG) name, minver, minrev))
				{
				break;
				}
			}
		}

	return FALSE;
}

//----------------------------------------------------------------------------

#if !defined(__SASC) && !defined(__amigaos4__)
// Replacement for SAS/C library functions

#if !defined(__MORPHOS__)
// Replacement for SAS/C library functions

size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}
#endif /*__MORPHOS__*/

#endif /* !defined(__SASC) && !defined(__amigaos4__) */
//-----------------------------------------------------------------------------

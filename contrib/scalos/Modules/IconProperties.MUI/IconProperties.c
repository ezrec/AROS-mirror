// IconProperties.c
// $Date$
// $Revision$
// $Id$

// NOTE: Replaced "VSpace(0)" by "RectangleObject" before
// "IconobjectMCCObject" and after "CheckMarkNoDrag ImageObject" and,
// removed VSpace(0) between groupframe and "Group_Buttons2 = HGroup",
// to show entirely the window's title.

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
#include <workbench/workbench.h>
#include <workbench/icon.h>
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
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/scalos.h>
#include <proto/preferences.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include "IconProperties.h"
#include "IconobjectMCC.h"
#include "ToolTypes.h"
#include "debug.h"

#define	IconProperties_NUMBERS
#define	IconProperties_ARRAY
#define	IconProperties_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#define	VERSION_MAJOR	40
#define	VERSION_MINOR	8

#define	VERS_MAJOR	STR(VERSION_MAJOR)
#define	VERS_MINOR	STR(VERSION_MINOR)
//----------------------------------------------------------------------------

#define	ID_MAIN	MAKE_ID('M','A','I','N')
#define	PS_DATA(prefsstruct)	((STRPTR) (prefsstruct)) + sizeof((prefsstruct)->ps_Size)

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
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_CycleChain, TRUE, \
		MUIA_ShortHelp, HelpText,\
		End


#define	Application_Return_Ok	1001
#define	Application_Return_Path	1002

//----------------------------------------------------------------------------

// local functions

static void init(void);
static void fail(APTR APP_Main, CONST_STRPTR str);
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT AppMessageHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT DefaultIconHookFunc(struct Hook *hook, Object *o, Msg msg);

static STRPTR GetLocString(ULONG MsgId);
//static void TranslateStringArray(STRPTR *stringArray);
static void SaveSettings(Object *IconObj, CONST_STRPTR IconName);
static Object *GetIconObject(CONST_STRPTR IconName, BPTR DirLock);
static Object *GetDeviceIconObject(CONST_STRPTR IconName, CONST_STRPTR VolumeName, CONST_STRPTR DeviceName);
static BOOL isDiskWritable(BPTR dLock);
static ULONG CheckInfoData(const struct InfoData *info);
static BOOL IsDevice(struct WBArg *arg);
static void GetDeviceName(BPTR dLock, STRPTR DeviceName, size_t MaxLen);
static BOOL ReadScalosPrefs(void);
static CONST_STRPTR GetPrefsConfigString(APTR prefsHandle, ULONG Id, CONST_STRPTR DefaultString);
static void StripTrailingChar(STRPTR String, char CharToRemove);
static void BuildDefVolumeNameNoSpace(STRPTR Buffer, CONST_STRPTR VolumeName, size_t MaxLength);
static void  ReplaceIcon(Object *NewIconObj, Object **OldIconObj);
static struct ScaWindowStruct *FindScalosWindow(BPTR dirLock);


static struct Hook AppMessageHook = {{ NULL, NULL }, HOOKFUNC_DEF(AppMessageHookFunc), NULL };
static struct Hook DefaultIconHook = {{ NULL, NULL }, HOOKFUNC_DEF(DefaultIconHookFunc), NULL };

//----------------------------------------------------------------------------

// local data items

static UBYTE prefDefIconsFirst = TRUE;		// Flag: try Def-Diskicons first
static CONST_STRPTR prefDefIconPath = "ENV:sys";

struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *IconBase = NULL;
struct Library *IconobjectBase = NULL;
struct ScalosBase *ScalosBase = NULL;
struct Library *PreferencesBase = NULL;
T_LOCALEBASE LocaleBase = NULL;

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct IconIFace *IIcon = NULL;
struct IconobjectIFace *IIconobject = NULL;
struct ScalosIFace *IScalos = NULL;
struct PreferencesIFace *IPreferences = NULL;
struct LocaleIFace *ILocale = NULL;
#endif

static struct Catalog *IconPropertiesCatalog;

static struct Hook AboutHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(OpenAboutFunc),
	NULL
        };
static struct Hook AboutMUIHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(OpenAboutMUIFunc),
	NULL
        };

static APTR Group_Buttons2;
static APTR APP_Main;
static APTR WIN_Main;
static APTR WIN_AboutMUI;
static APTR OkButton, CancelButton;
static Object *CheckMarkNoText, *CheckMarkNoDrag;
static Object *CheckMarkNoToolTips;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit, *Path, *IconImage, *GroupMccIconObj;
static Object *Group_Virtual;									//+jmc+ To switch 0/1 "MUIA_ShowMe" the ScroolGroupObject group.
static Object *CheckMarkBrowserMode;								//+jmc+ BrowserMode tooltype.
static Object *MenuImageDefault;

static struct MUI_CustomClass *IconobjectClass;

//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	LONG win_opened;
	BPTR oldDir = (BPTR)NULL;
	CONST_STRPTR IconName = "";
	Object *iconObj = NULL;
	BOOL IsWriteable = TRUE;
	static char PathName[512];
	LONG selected;
	struct WBStartup *WBenchMsg =
		(argc == 0) ? (struct WBStartup *)argv : NULL;

	init();

	if (WBenchMsg && WBenchMsg->sm_ArgList)
		{
		struct WBArg *arg;

		if (WBenchMsg->sm_NumArgs > 1)
			{
			arg = &WBenchMsg->sm_ArgList[1];

			IconName = arg->wa_Name;

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: wa_Lock=%08lx\n", __LINE__, arg->wa_Lock));

			oldDir = CurrentDir(arg->wa_Lock);

			if (IsDevice(arg))
				{
				static char VolumeName[256];
				char DeviceName[256];

				NameFromLock(arg->wa_Lock, VolumeName, sizeof(VolumeName));
				GetDeviceName(arg->wa_Lock, DeviceName, sizeof(DeviceName));

				StripTrailingChar(VolumeName, ':');
				StripTrailingChar(DeviceName, ':');

				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: VolumeName=<%s>\n", __LINE__, VolumeName));
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DeviceName=<%s>\n", __LINE__, DeviceName));

				iconObj = GetDeviceIconObject(IconName, VolumeName, DeviceName);

				IconName = VolumeName;
				}
			else
				{
				ULONG Pos;
				char xName[256];

				stccpy(xName, arg->wa_Name, sizeof(xName));
				Pos = IsIconName(xName);

				if (Pos && ~0 != Pos)
					*((char *) Pos) = '\0';

				iconObj = GetIconObject(xName, arg->wa_Lock);
				
				NameFromLock(arg->wa_Lock,PathName,sizeof(PathName));
				}
			IsWriteable = isDiskWritable(arg->wa_Lock);
			}
		}

	DefaultIconHook.h_Data = &iconObj;
	AppMessageHook.h_Data = &iconObj;


	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx\n", __LINE__, iconObj));

	if ((strlen(IconName) > 0 && iconObj))
		{
		ULONG NoText = FALSE;
		ULONG NoDrag = FALSE;
		ULONG NoToolTip = FALSE;
		ULONG BrowserMode = FALSE;
		BOOL BrowserSupported = FALSE;
		ULONG IconType;
		STRPTR tt;

		GetAttr(IDTA_Type, iconObj, (IPTR *)&IconType);

		if ((WBDISK == IconType) || (WBDRAWER == IconType) || (WBGARBAGE == IconType))
			BrowserSupported = TRUE;
			
		tt = NULL;
		if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NOTEXT", &tt))
			NoText = TRUE;
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: tt=%08lx\n", __LINE__, tt));

		tt = NULL;
		if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NODRAG", &tt))
			NoDrag = TRUE;
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: tt=%08lx\n", __LINE__, tt));

		tt = NULL;
		if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NOTOOLTIPS", &tt))
			NoToolTip = TRUE;

		tt = NULL;
		if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_BROWSERMODE", &tt))
			BrowserMode = TRUE;

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: tt=%08lx\n", __LINE__, tt));
		

		APP_Main = ApplicationObject,
			MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
			MUIA_Application_Version,	"$VER: Scalos IconProperties.module V" VERS_MAJOR "." VERS_MINOR " (" __DATE__ ") " COMPILER_STRING,
			MUIA_Application_Copyright,	"© The Scalos Team, 2004" CURRENTYEAR,
			MUIA_Application_Author,	"The Scalos Team",
			MUIA_Application_Description,	"Scalos Icon Properties module",
			MUIA_Application_Base,		"SCALOS_ICON_PROPERTIES",

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
							MUIA_ShortHelp, GetLocString(MSGID_TEXT_ICONNAME_SHORTHELP),
						End, //TextObject

						Child, TextObject,
							MUIA_ShowMe, !IsWriteable,
							MUIA_Text_PreParse, MUIX_C,
							MUIA_Text_Contents, GetLocString(MSGID_READ_ONLY),
						End, //TextObject

						// Child, VSpace(0),

						Child, HGroup,
							Child, GroupMccIconObj = HGroup,
								Child, RectangleObject, End, //--- Instead: Child, VSpace(0)

								Child, IconImage = IconobjectMCCObject,
									MUIA_Iconobj_Object, iconObj,
									MUIA_InputMode, MUIV_InputMode_Toggle,
									MUIA_ShowSelState, FALSE,
								End, //IconobjectMCCObject
							End, //VGroup

							Child, VSpace(0),

							Child, ColGroup(2),
								Child, Label1(GetLocString(MSGID_CHECKMARK_NOTEXT)),
								Child, CheckMarkNoText = ImageObject,
									ImageButtonFrame,
									MUIA_CycleChain, TRUE,
									MUIA_InputMode, MUIV_InputMode_Toggle,
									MUIA_Image_Spec, MUII_CheckMark,
									MUIA_Image_FreeVert, TRUE,
									MUIA_Selected, NoText,
									MUIA_Disabled, !IsWriteable,
									MUIA_Background, MUII_ButtonBack,
									MUIA_ShowSelState, FALSE,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CHECKMARK_NOTEXT_SHORTHELP),
								End, //Image

								Child, Label1(GetLocString(MSGID_CHECKMARK_NODRAG)),
								Child, CheckMarkNoDrag = ImageObject,
									ImageButtonFrame,
									MUIA_CycleChain, TRUE,
									MUIA_InputMode, MUIV_InputMode_Toggle,
									MUIA_Image_Spec, MUII_CheckMark,
									MUIA_Image_FreeVert, TRUE,
									MUIA_Selected, NoDrag,
									MUIA_Disabled, !IsWriteable,
									MUIA_Background, MUII_ButtonBack,
									MUIA_ShowSelState, FALSE,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CHECKMARK_NODRAG_SHORTHELP),
								End, //Image

								Child, Label1(GetLocString(MSGID_CHECKMARK_NOTOOLTIPS)),
								Child, CheckMarkNoToolTips = ImageObject,
									ImageButtonFrame,
									MUIA_CycleChain, TRUE,
									MUIA_InputMode, MUIV_InputMode_Toggle,
									MUIA_Image_Spec, MUII_CheckMark,
									MUIA_Image_FreeVert, TRUE,
									MUIA_Selected, NoToolTip,
									MUIA_Disabled, !IsWriteable,
									MUIA_Background, MUII_ButtonBack,
									MUIA_ShowSelState, FALSE,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CHECKMARK_NOTOOLTIPS_SHORTHELP),
								End, //Image

								Child, Label1(GetLocString(MSGID_CHECKMARK_BROWSERMODE)),
								Child, CheckMarkBrowserMode = ImageObject,
									ImageButtonFrame,
									MUIA_CycleChain, TRUE,
									MUIA_InputMode, MUIV_InputMode_Toggle,
									MUIA_Image_Spec, MUII_CheckMark,
									MUIA_Image_FreeVert, TRUE,
									MUIA_Selected, BrowserMode,
									MUIA_Disabled, (!IsWriteable || !BrowserSupported),
									MUIA_Background, MUII_ButtonBack,
									MUIA_ShowSelState, FALSE,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CHECKMARK_BROWSERMODE_SHORTHELP),
								End, //Image

							End, //ColGroup

							Child, RectangleObject, End, //--- Instead: Child, VSpace(0)

						End, //HGroup

						Child, Group_Virtual = ScrollgroupObject, // +jmc+
							MUIA_CycleChain, TRUE,
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
									MUIA_ShortHelp, GetLocString(MSGID_TEXT_ICONPATH_SHORTHELP),
								End, //TextObject
							End, //VirtgroupObject
						End, //ScrollgroupObject

					End, //VGroup,

					// Child, VSpace(0), //--- Removed 

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

				Child, MenuObjectT(GetLocString(MSGID_MENU_IMAGE)),
					Child, MenuImageDefault = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_IMAGE_DEFAULT),
						MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_IMAGE_DEFAULT_SHORT),
					End,
				End, //MenuObjectT

			End, //MenuStripObject

		End; //ApplicationObject

		if (NULL == APP_Main)
			{
			fail(APP_Main, "Failed to create Application.");
			}

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
			WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);
		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
		DoMethod(OkButton, MUIM_Notify,MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, Application_Return_Ok);

		//--------------------------------------------------------------------------//

		// call hook when menu command "icon image/default" is issued
		DoMethod(MenuImageDefault, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &DefaultIconHook);

		// setup notification when an icon is dropped onto the icon image
		DoMethod(GroupMccIconObj, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
			GroupMccIconObj, 3, MUIM_CallHook, &AppMessageHook, MUIV_TriggerValue);

		//--------------------------------------------------------------------------//

		DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutHook);
		DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutMUIHook);
		DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(IconImage, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			APP_Main, 3, MUIM_Application_ReturnID, Application_Return_Path);

		DoMethod(IconImage, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			APP_Main, 3, MUIM_WriteLong, MUIV_TriggerValue, &selected);

		// disable Ok button for read-only icons
		set(OkButton, MUIA_Disabled, !IsWriteable);
		set(Group_Virtual, MUIA_ShowMe, 0);

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
				case Application_Return_Ok:
					SaveSettings(iconObj, IconName);
					Run = FALSE;
					break;
				case Application_Return_Path:
					if(strlen(PathName) > 0)
						set(Group_Virtual, MUIA_ShowMe, selected);
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

		set(WIN_Main, MUIA_Window_Open, FALSE);
		}

	if (iconObj)
		DisposeIconObject(iconObj);
	if (oldDir)
		CurrentDir(oldDir);

	fail(APP_Main, NULL);

	return 0;
}


static VOID fail(APTR APP_Main, CONST_STRPTR str)
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
	if (IconPropertiesCatalog)
		{
		CloseCatalog(IconPropertiesCatalog);
		IconPropertiesCatalog = NULL;
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
	if (!OpenLibraries())
		fail(NULL, "Failed to call OpenLibraries()");

	if (LocaleBase)
		IconPropertiesCatalog = OpenCatalogA(NULL, "Scalos/IconProperties.catalog", NULL);

	IconobjectClass = InitIconobjectClass();

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
#endif

	MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		fail(NULL, "Failed to open "MUIMASTER_NAME".");
#ifdef __amigaos4__
	else
		{
		IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
		if (NULL == IMUIMaster)
			fail(NULL, "Failed to open muimaster interface.");
		}
#endif

	IconBase = OpenLibrary("icon.library", 0);
	if (NULL == IconBase)
		fail(NULL, "Failed to open icon.library");
#ifdef __amigaos4__
	else
		{
		IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
		if (NULL == IIcon)
			fail(NULL, "Failed to open icon interface.");
		}
#endif

	ScalosBase = (struct ScalosBase *) OpenLibrary("scalos.library", 40);
	if (NULL == ScalosBase)
		fail(NULL, "Failed to open scalos.library");
#ifdef __amigaos4__
	else
		{
		IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
		if (NULL == IScalos)
			fail(NULL, "Failed to open scalos interface.");
		}
#endif

	PreferencesBase = OpenLibrary("preferences.library", 39);
	if (NULL == PreferencesBase)
		fail(NULL, "Failed to open preferences.library.");
#ifdef __amigaos4__
	else
		{
		IPreferences = (struct PreferencesIFace *)GetInterface((struct Library *)PreferencesBase, "main", 1, NULL);
		if (NULL == IPreferences)
			fail(NULL, "Failed to open preferences interface.");
		}
#endif

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
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		{
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
		if (NULL == ILocale)
			fail(NULL, "Failed to open locale interface");
		}
#endif

	return TRUE;
}


static void CloseLibraries(void)
{
	if (LocaleBase)
		{
		if (IconPropertiesCatalog)
			{
			CloseCatalog(IconPropertiesCatalog);
			IconPropertiesCatalog = NULL;
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
	struct IconProperties_LocaleInfo li;

	li.li_Catalog = IconPropertiesCatalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetIconPropertiesString(&li, MsgId);
}

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

static SAVEDS(void) INTERRUPT DefaultIconHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	Object **OldIconObj = (Object **) hook->h_Data;
	Object *NewIconObj;
	IPTR IconType;

	GetAttr(IDTA_Type, *OldIconObj, &IconType);
	NewIconObj = GetDefIconObject(IconType, NULL);
	if (NewIconObj)
		ReplaceIcon(NewIconObj, OldIconObj);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT AppMessageHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct AppMessage *AppMsg = *(struct AppMessage **) msg;
	Object **OldIconObj = (Object **) hook->h_Data;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: AppMsg=%08lx  am_NumArgs=%ld\n", __LINE__, AppMsg, AppMsg->am_NumArgs));

	if (AppMsg->am_NumArgs >= 1)
		{
		struct WBArg *arg = &AppMsg->am_ArgList[0];
		BPTR OldDir;
		Object *NewIconObj;
		ULONG Pos;
		char xName[256];
		STRPTR NewIconName;

		OldDir = CurrentDir(arg->wa_Lock);

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: wa_Lock=%08lx  wa_Name=<%s>\n", __LINE__, arg->wa_Lock, arg->wa_Name));

		if (IsDevice(arg))
			{
			static char VolumeName[256];
			char DeviceName[256];

			NewIconName = "disk";

			NameFromLock(arg->wa_Lock, VolumeName, sizeof(VolumeName));
			GetDeviceName(arg->wa_Lock, DeviceName, sizeof(DeviceName));

			StripTrailingChar(VolumeName, ':');
			StripTrailingChar(DeviceName, ':');

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: VolumeName=<%s>\n", __LINE__, VolumeName));
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DeviceName=<%s>\n", __LINE__, DeviceName));

			NewIconObj = GetDeviceIconObject(NewIconName, VolumeName, DeviceName);

			if (NewIconObj)
				{
				SetAttrs(NewIconObj,
					IDTA_Type, WBDISK,
					TAG_END);
				}
			}
		else
			{
			if (strlen(arg->wa_Name) > 0)
				{
				stccpy(xName, arg->wa_Name, sizeof(xName));
				Pos = IsIconName(xName);
				if (Pos && ~0 != Pos)
					*((char *) Pos) = '\0';
				NewIconObj = GetIconObject(xName, arg->wa_Lock);
				}
			else
				{
				NameFromLock(arg->wa_Lock, xName, sizeof(xName));
				NewIconObj = GetIconObject(xName, arg->wa_Lock); // ParentDir(arg->wa_Lock)
				}
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: xName=<%s>\n", __LINE__, xName));
			}

		if (NewIconObj)
			ReplaceIcon(NewIconObj, OldIconObj);

		CurrentDir(OldDir);
		}
}

//----------------------------------------------------------------------------

static void ReplaceIcon(Object *NewIconObj, Object **OldIconObj)
{
	STRPTR str;
	struct IBox *WinRect;
	struct ExtGadget *ggOld = (struct ExtGadget *) *OldIconObj;
	struct ExtGadget *ggNew = (struct ExtGadget *) NewIconObj;
	IPTR  ul;
	IPTR  IconType;
	IPTR  TheName;
	STRPTR *ToolTypesArray = NULL;

	GetAttr(IDTA_Type, *OldIconObj, &IconType);
	set(NewIconObj, IDTA_Type, IconType);

	GetAttr(DTA_Name, *OldIconObj, &TheName);
	set(NewIconObj, DTA_Name, TheName);

	ggNew->BoundsLeftEdge = ggOld->LeftEdge + (ggNew->LeftEdge - ggNew->BoundsLeftEdge);
	ggNew->BoundsTopEdge = ggOld->TopEdge + (ggNew->TopEdge - ggNew->BoundsTopEdge);
	ggNew->LeftEdge = ggOld->LeftEdge;
	ggNew->TopEdge = ggNew->TopEdge;


	GetAttr(IDTA_ToolTypes, *OldIconObj, (APTR) &ToolTypesArray);
	set(NewIconObj, IDTA_ToolTypes, (ULONG) ToolTypesArray);

	GetAttr(IDTA_DefaultTool, *OldIconObj, (APTR) &str);
	set(NewIconObj, IDTA_DefaultTool, (ULONG) str);

	GetAttr(IDTA_WindowRect, *OldIconObj, (APTR) &WinRect);
	set(NewIconObj, IDTA_WindowRect, (ULONG) WinRect);

	GetAttr(IDTA_WinCurrentX, *OldIconObj, &ul);
	set(NewIconObj, IDTA_WinCurrentX, ul);
	GetAttr(IDTA_WinCurrentY, *OldIconObj, &ul);
	set(NewIconObj, IDTA_WinCurrentY, ul);

	GetAttr(IDTA_Flags, *OldIconObj, &ul);
	set(NewIconObj, IDTA_Flags, ul);

	GetAttr(IDTA_ViewModes, *OldIconObj, &ul);
	set(NewIconObj, IDTA_ViewModes, ul);

	set(IconImage, MUIA_Iconobj_Object, NewIconObj);
	DisposeIconObject(*OldIconObj);
	*OldIconObj = NewIconObj;
}

//----------------------------------------------------------------------------

static void SaveSettings(Object *IconObj, CONST_STRPTR IconName)
{
	if (IconObj)
		{
		IPTR  IconType;
		ULONG Checked = 0;
		char CheckName[256];
		ULONG Pos;
		APTR UndoStep = NULL;
		struct ScaWindowStruct *ws;
		CONST_STRPTR *ToolTypesArray;
		STRPTR *OldToolTypesArray;
		BPTR dirLock = CurrentDir((BPTR) NULL);

		CurrentDir(dirLock);
		ws = FindScalosWindow(dirLock);

		if (ws)
			{
			UndoStep = (APTR) DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_BeginUndoStep);
			}

		stccpy(CheckName, IconName, sizeof(CheckName));
		Pos = IsIconName(CheckName);

		if (Pos && ~0 != Pos)
			{
			// strip ".info" from name
			*((char *) Pos) = '\0';
			IconName = CheckName;
			IconObj = NewIconObject(IconName, NULL);
			}

		GetAttr(IDTA_ToolTypes, IconObj, (APTR) &ToolTypesArray);
		OldToolTypesArray = CloneToolTypeArray(ToolTypesArray, 0);

		GetAttr(IDTA_Type, IconObj, &IconType);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconType=%ld\n", __LINE__, IconType));
		if (WBDISK == IconType)
			{
			IconName = "disk";
			SetAttrs(IconObj, 
				DTA_Name, IconName,
				TAG_END);
			}

		get(CheckMarkNoText, MUIA_Selected, &Checked);
		if (Checked)
			SetToolType(IconObj, "SCALOS_NOTEXT", "");
		else
			RemoveToolType(IconObj, "SCALOS_NOTEXT");

		get(CheckMarkNoDrag, MUIA_Selected, &Checked);
		if (Checked)
			SetToolType(IconObj, "SCALOS_NODRAG", "");
		else
			RemoveToolType(IconObj, "SCALOS_NODRAG");

		get(CheckMarkNoToolTips, MUIA_Selected, &Checked);
		if (Checked)
			SetToolType(IconObj, "SCALOS_NOTOOLTIPS", "");
		else
			RemoveToolType(IconObj, "SCALOS_NOTOOLTIPS");

		get(CheckMarkBrowserMode, MUIA_Selected, &Checked);
		if (Checked)
			SetToolType(IconObj, "SCALOS_BROWSERMODE", "");
		else
			RemoveToolType(IconObj, "SCALOS_BROWSERMODE");

		if (ws)
			{
			CONST_STRPTR *NewToolTypeArray = NULL;

			GetAttr(IDTA_ToolTypes, IconObj, (APTR) &NewToolTypeArray);

			DoMethod(ws->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_AddUndoEvent,
				UNDO_SetToolTypes,
				UNDOTAG_IconDirLock, dirLock,
				UNDOTAG_IconName, (ULONG) IconName,
				UNDOTAG_OldToolTypes, (ULONG) OldToolTypesArray,
				UNDOTAG_NewToolTypes, (ULONG) NewToolTypeArray,
				TAG_END
				);
			}

		PutIconObjectTags(IconObj, IconName,
			ICONA_NoNewImage, TRUE,
			TAG_END);

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
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconName=<%s> IconObj=%08lx\n", __LINE__, IconName, IconObj));
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

			strcpy(DefIconName, "def_");
			strcat(DefIconName, VolumeName);

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			BuildDefVolumeNameNoSpace(DefIconName, VolumeName, sizeof(DefIconName));

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			strcpy(DefIconName, "def_");
			strcat(DefIconName, DeviceName);

			IconObj = NewIconObject(DefIconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			CurrentDir(OldDir);
			OldDir = (BPTR)NULL;

			IconObj = NewIconObject(IconName, NULL);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, IconName));
			if (IconObj)
				break;

			}
		else
			{
			IconObj = NewIconObject(IconName, NULL);
			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, IconName));
			if (IconObj)
				break;

			dLock = Lock(prefDefIconPath, ACCESS_READ);
			if ((BPTR)NULL == dLock)
				break;

			OldDir = CurrentDir(dLock);

			strcpy(DefIconName, "def_");
			strcat(DefIconName, VolumeName);

			IconObj = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			BuildDefVolumeNameNoSpace(DefIconName, VolumeName, sizeof(DefIconName));

			IconObj = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;

			strcpy(DefIconName, "def_");
			strcat(DefIconName, DeviceName);

			IconObj = NewIconObject(DefIconName, NULL);
			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: IconObj=%08lx  <%s>\n", __LINE__, IconObj, DefIconName));
			if (IconObj)
				break;
			}

		IconObj = GetDefIconObject(WBDISK, NULL);
		} while (0);

	if (IconObj)
		{
		SetAttrs(IconObj, 
			IDTA_Type, WBDISK, 
			TAG_END);
		}

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

	GetPreferences(MainPrefsHandle, ID_MAIN, SCP_LoadDefIconsFirst, &prefDefIconsFirst, sizeof(prefDefIconsFirst));
	// UBYTE, no endiannes handling needed

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


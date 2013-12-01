// Hooks.c
// $Date$
// $Revision$

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/ttengine.h>
#include <workbench/workbench.h>
#include <intuition/imageclass.h>
#include <mui/NFloattext_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/TextEditor_mcc.h>
#include <scalos/scalosprefsplugin.h>

#ifdef __amigaos4__
#include <dos/anchorpath.h>
#endif

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#define USE_INLINE_STDARG
#include <proto/ttengine.h>
#undef	USE_INLINE_STDARG
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include "General.h"
#include "ScalosPrefs.h"
#include "ReadWritePrefs.h"
#include "HiddenDevices.h"
#include "PrefsPlugins.h"
#include "FontSampleMCC.h"
#include <ScalosMcpGfx.h>
#include <scalos/scalosgfx.h>
#include "McpFrameMCC.h"
#include "DataTypesMCC.h"
#include "SelectMarkSampleClass.h"
#include "Hooks.h"
#include "locale.h"

//-----------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT AddPluginAslFilterHookFunc(struct Hook *hook, Object *o, Msg msg);
static void BuildTTDescFromAttrList(char *buffer, size_t length, struct TagItem *AttrList);
static BOOL ParseTTFontFromDesc(CONST_STRPTR FontDesc, 
	ULONG *FontStyle, ULONG *FontWeight, ULONG *FontSize,
	STRPTR FontName, size_t FontNameSize);
static LONG TTFontHeightFromDesc(CONST_STRPTR FontDesc);
static LONG DiskFontHeightFromDesc(CONST_STRPTR FontDesc);

//-----------------------------------------------------------------

static struct Hook AddPluginAslFilterHook = { { NULL, NULL }, HOOKFUNC_DEF(AddPluginAslFilterHookFunc), NULL };

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT StartFontPrefs_Func(struct Hook *hook, Object *pPrefsWindow, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	set(pPrefsWindow, MUIA_Window_Sleep, TRUE);
	Execute(SYS_FONTPREFS_NAME, 0, 0);

	// Update (possibly changed) icon text 
	ReadIconFontPrefs(app);

	set(pPrefsWindow, MUIA_Window_Sleep, FALSE);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT StartWorkbenchPrefs_Func(struct Hook *hook, Object *pPrefsWindow, Msg msg)
{
	set(pPrefsWindow, MUIA_Window_Sleep, TRUE);
	Execute(SYS_WORKBENCHPREFS_NAME, 0, 0);
	set(pPrefsWindow, MUIA_Window_Sleep, FALSE);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT CheckPopTitleBarBox(struct Hook *hook, Object *pPrefsWindow, Msg msg)
{
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT ControlBarGadgetListBrowserActiveHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct ControlBarGadgetEntry *cgy = NULL;

	DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cgy);

	if (cgy && SCPGadgetType_ActionButton == cgy->cgy_GadgetType)
		{
		set(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIA_Disabled, FALSE);
		set(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], MUIA_Disabled, FALSE);
		set(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], MUIA_Disabled, FALSE);
		set(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION], MUIA_Disabled, FALSE);
		set(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT], MUIA_Disabled, FALSE);
		set(app->Obj[POP_CONTROLBARGADGETS_BROWSER_ACTION], MUIA_Disabled, FALSE);

		// use nnset() here since notification would cause remaining strings to be set to
		// empty value when first string is changed here.
		nnset(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIA_String_Contents,
			cgy->cgy_NormalImage ? cgy->cgy_NormalImage : (STRPTR) "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], MUIA_String_Contents,
			cgy->cgy_SelectedImage ? cgy->cgy_SelectedImage : (STRPTR) "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], MUIA_String_Contents,
			cgy->cgy_DisabledImage ? cgy->cgy_DisabledImage : (STRPTR) "");
		nnset(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT], MUIA_TextEditor_Contents, cgy->cgy_HelpText);
		nnset(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION], MUIA_String_Contents, cgy->cgy_Action);
		}
	else
		{
		set(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIA_Disabled, TRUE);
		set(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], MUIA_Disabled, TRUE);
		set(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], MUIA_Disabled, TRUE);
		set(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION], MUIA_Disabled, TRUE);
		set(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT], MUIA_Disabled, TRUE);
		set(app->Obj[POP_CONTROLBARGADGETS_BROWSER_ACTION], MUIA_Disabled, TRUE);

		nnset(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIA_String_Contents, "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], MUIA_String_Contents, "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], MUIA_String_Contents, "");
		nnset(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT], MUIA_TextEditor_Contents, "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION], MUIA_String_Contents, "");
		}

	set(app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIA_ScaDtpic_Name,
		cgy->cgy_NormalImage ? cgy->cgy_NormalImage : (STRPTR) "");
	set(app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], MUIA_ScaDtpic_Name,
		cgy->cgy_SelectedImage ? cgy->cgy_SelectedImage : (STRPTR) "");
	set(app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], MUIA_ScaDtpic_Name,
		cgy->cgy_DisabledImage ? cgy->cgy_DisabledImage : (STRPTR) "");
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT ControlBarGadgetListNormalActiveHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct ControlBarGadgetEntry *cgy = NULL;

	DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cgy);

	if (cgy && SCPGadgetType_ActionButton == cgy->cgy_GadgetType)
		{
		set(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIA_Disabled, FALSE);
		set(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], MUIA_Disabled, FALSE);
		set(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], MUIA_Disabled, FALSE);
		set(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION], MUIA_Disabled, FALSE);
		set(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT], MUIA_Disabled, FALSE);
		set(app->Obj[POP_CONTROLBARGADGETS_NORMAL_ACTION], MUIA_Disabled, FALSE);

		// use nnset() here since notification would cause remaining strings to be set to
		// empty value when first string is changed here.
		nnset(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIA_String_Contents,
			cgy->cgy_NormalImage ? cgy->cgy_NormalImage : (STRPTR) "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], MUIA_String_Contents,
			cgy->cgy_SelectedImage ? cgy->cgy_SelectedImage : (STRPTR) "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], MUIA_String_Contents,
			cgy->cgy_DisabledImage ? cgy->cgy_DisabledImage : (STRPTR) "");
		nnset(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT], MUIA_TextEditor_Contents, cgy->cgy_HelpText);
		nnset(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION], MUIA_String_Contents, cgy->cgy_Action);
		}
	else
		{
		set(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIA_Disabled, TRUE);
		set(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], MUIA_Disabled, TRUE);
		set(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], MUIA_Disabled, TRUE);
		set(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION], MUIA_Disabled, TRUE);
		set(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT], MUIA_Disabled, TRUE);
		set(app->Obj[POP_CONTROLBARGADGETS_NORMAL_ACTION], MUIA_Disabled, TRUE);

		nnset(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIA_String_Contents, "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], MUIA_String_Contents, "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], MUIA_String_Contents, "");
		nnset(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT], MUIA_TextEditor_Contents, "");
		nnset(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION], MUIA_String_Contents, "");
		}

	set(app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIA_ScaDtpic_Name,
		cgy->cgy_NormalImage ? cgy->cgy_NormalImage : (STRPTR) "");
	set(app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], MUIA_ScaDtpic_Name,
		cgy->cgy_SelectedImage ? cgy->cgy_SelectedImage : (STRPTR) "");
	set(app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], MUIA_ScaDtpic_Name,
		cgy->cgy_DisabledImage ? cgy->cgy_DisabledImage : (STRPTR) "");
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT PluginActive_Func(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct PluginDef *pd;

	DoMethod(app->Obj[PLUGIN_LIST], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &pd);

	if (pd)
		{
		set(app->Obj[PLUGIN_DESCRIPTION], MUIA_Floattext_Text, pd->plug_Description);
		set(app->Obj[PLUGIN_CREATOR], MUIA_Floattext_Text, pd->plug_Creator);
		set(app->Obj[REMOVE_PLUGIN], MUIA_Disabled, FALSE);
		set(app->Obj[PLUGIN_NAME], MUIA_Text_Contents, pd->plug_RealName);
		set(app->Obj[PLUGIN_FILENAME], MUIA_Text_Contents, pd->plug_filename);
		set(app->Obj[PLUGIN_VERSION], MUIA_Text_Contents, pd->plug_VersionString);
		}
	else
		{
		set(app->Obj[PLUGIN_DESCRIPTION], MUIA_Floattext_Text, "");
		set(app->Obj[PLUGIN_CREATOR], MUIA_Floattext_Text, "");
		set(app->Obj[REMOVE_PLUGIN], MUIA_Disabled, TRUE);
		set(app->Obj[PLUGIN_NAME], MUIA_Text_Contents, "");
		set(app->Obj[PLUGIN_FILENAME], MUIA_Text_Contents, "");
		set(app->Obj[PLUGIN_VERSION], MUIA_Text_Contents, "");
		}
}


SAVEDS(void) INTERRUPT AddPlugin_Func(struct Hook *hook, Object *pPluginList, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct FileRequester *Req;

	AddPluginAslFilterHook.h_Data = app;

	Req = MUI_AllocAslRequestTags(ASL_FileRequest,
		ASLFR_SleepWindow, TRUE,
		ASLFR_Flags1, FRF_DOPATTERNS,
		ASLFR_InitialFile, (ULONG) "",
		ASLFR_InitialDrawer, (ULONG) "Scalos:Plugins/OOP",
		ASLFR_InitialPattern, (ULONG) "#?.plugin",
		ASLFR_UserData, app,
		ASLFR_IntuiMsgFunc, &AslIntuiMsgHook,
		ASLFR_FilterFunc, &AddPluginAslFilterHook,
		TAG_END);

	if (Req)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(app->Obj[WINDOW_MAIN], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(Req,
			ASLFR_Window, (ULONG) win,
			ASLFR_TitleText, (ULONG) GetLocString(MSGID_ADD_PLUGIN_ASLTITLE),
			ASLFR_DoMultiSelect, TRUE,
			TAG_END);

		if (Result)
			{
			ULONG n;

			for (n = 0; n < Req->fr_NumArgs; n++)
				{
				struct WBArg *arg = &Req->fr_ArgList[n];

				BPTR oldDir = CurrentDir(arg->wa_Lock);

				if (!AddPlugin(app, arg->wa_Name))
					{
					MUI_Request(app->Obj[APPLICATION],
						NULL,
						0,
						(STRPTR) GetLocString(MSGID_ADDPLUGIN_FAILURE),
						(STRPTR) GetLocString(MSGID_ADDPLUGIN_OK_GAD),
						(STRPTR) GetLocString(MSGID_ADDPLUGIN_COULD_NOT_ADD),
						arg->wa_Name
						);
					}

				CurrentDir(oldDir);
				}
			}

		MUI_FreeAslRequest(Req);
		}
}


static SAVEDS(LONG) INTERRUPT AddPluginAslFilterHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct FileRequester *fr = (struct FileRequester *) o;
	struct AnchorPath *ap = (struct AnchorPath *) msg;
	LONG AcceptEntry = TRUE;
	BPTR DirLock;

	(void) fr;

	d1(KPrintF("%s/%s/%ld: fib_FileName=<%s> ap_Strlen=%ld\n", __FILE__, __FUNC__, __LINE__, ap->ap_Info.fib_FileName, ap->ap_Strlen));
	d1(KPrintF("%s/%s/%ld: fr_Drawer=<%s>\n", __FILE__, __FUNC__, __LINE__, fr->fr_Drawer));

	DirLock = Lock(fr->fr_Drawer, ACCESS_READ);
	d1(KPrintF("%s/%s/%ld: DirLock=%08lx\n", __FILE__, __FUNC__, __LINE__, DirLock));
	if (DirLock)
		{
		BPTR OldDir;
		BPTR fLock;

		OldDir = CurrentDir(DirLock);

		fLock = Lock(ap->ap_Info.fib_FileName, ACCESS_READ);
		d1(KPrintF("%s/%s/%ld: fLock=%08lx\n", __FILE__, __FUNC__, __LINE__, fLock));
		if (fLock)
			{
			// exclude entry from file requester if already presebnt in plugin list
			if (FindPlugin(app, fLock))
				AcceptEntry = FALSE;

			UnLock(fLock);
			}

		CurrentDir(OldDir);
		UnLock(DirLock);
		}

	return AcceptEntry;
}


SAVEDS(void) INTERRUPT RemovePluginFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct PluginDef *pd;

	DoMethod(app->Obj[PLUGIN_LIST], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &pd);

	if (pd)
		{
		RemovePlugin(app, pd);
		}
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT OpenHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct FileRequester *Req;

	Req = MUI_AllocAslRequestTags(ASL_FileRequest,
		ASLFR_SleepWindow, TRUE,
		ASLFR_Flags1, FRF_DOPATTERNS,
		ASLFR_InitialDrawer, (ULONG) "SYS:Prefs/presets",
		ASLFR_InitialPattern, (ULONG) "#?.(pre|prefs)",
		ASLFR_InitialFile, (ULONG) "Scalos.pre",
		ASLFR_UserData, app,
		ASLFR_IntuiMsgFunc, &AslIntuiMsgHook,
		TAG_END);

	if (Req)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(app->Obj[WINDOW_MAIN], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(Req,
			ASLFR_Window, (ULONG) win,
			ASLFR_TitleText, (ULONG) GetLocString(MSGID_MENU_PROJECT_OPEN_ASLTITLE),
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(Req->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				BPTR oldDir = CurrentDir(dirLock);

				ReadScalosPrefs(Req->fr_File);
				UpdateGuiFromPrefs(app);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}

		MUI_FreeAslRequest(Req);
		}
}

SAVEDS(void) INTERRUPT LastSavedFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	DoMethodForAllPrefsPlugins(1, MUIM_ScalosPrefs_LastSavedConfig);

	ReadWorkbenchPrefs("ENVARC:sys/workbench.prefs");
	ReadScalosPrefs("ENVARC:Scalos/Scalos.prefs");
	UpdateGuiFromPrefs(app);
}

SAVEDS(void) INTERRUPT SaveAsFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct FileRequester *Req;
	
	Req = MUI_AllocAslRequestTags(ASL_FileRequest,
		ASLFR_InitialFile, (ULONG) "Scalos.pre",
		ASLFR_SleepWindow, TRUE,
		ASLFR_DoSaveMode, TRUE,
		ASLFR_InitialDrawer, (ULONG) "SYS:Prefs/presets",
		ASLFR_UserData, app,
		ASLFR_IntuiMsgFunc, &AslIntuiMsgHook,
		TAG_END);

	if (Req)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(app->Obj[WINDOW_MAIN], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(Req,
			ASLFR_TitleText, (ULONG) GetLocString(MSGID_MENU_PROJECT_SAVEAS_ASLTITLE),
			ASLFR_Window, (ULONG) win,
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(Req->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				BPTR oldDir = CurrentDir(dirLock);

				WriteScalosPrefs(app, Req->fr_File);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}

		MUI_FreeAslRequest(Req);
		}
}

SAVEDS(void) INTERRUPT ResetToDefaultsFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	DoMethodForAllPrefsPlugins(1, MUIM_ScalosPrefs_ResetToDefaults);
	HiddenDevicesCleanup();
	ResetToDefaults(app);
}

SAVEDS(void) INTERRUPT OpenAboutFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	MUI_Request(app->Obj[APPLICATION], app->Obj[WINDOW_MAIN], 0, NULL, 
		(STRPTR) GetLocString(MSGID_ABOUTREQOK),
		(STRPTR) GetLocString(MSGID_ABOUTREQFORMAT),
		VERSION_MAJOR, VERSION_MINOR, COMPILER_STRING, CURRENTYEAR);
}

SAVEDS(void) INTERRUPT OpenAboutMorpOSFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	set(app->Obj[WIN_ABOUT_MORPHOS], MUIA_Window_Open, TRUE);
}


SAVEDS(void) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	DoMethod(app->Obj[APPLICATION], MUIM_Application_AboutMUI,
		app->Obj[WINDOW_MAIN]);
}


SAVEDS(void) INTERRUPT RestoreFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	DoMethodForAllPrefsPlugins(1, MUIM_ScalosPrefs_RestoreConfig);

	ReadScalosPrefs("ENV:Scalos/Scalos.prefs");
	UpdateGuiFromPrefs(app);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT ModulesPrefsPopupHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct ModuleListEntry *mdle = NULL;
	ULONG ButtonClicked;

	ButtonClicked = getv(app->Obj[NLIST_MODULES], MUIA_NList_ButtonClick);

	DoMethod(app->Obj[NLIST_MODULES], MUIM_NList_GetEntry, ButtonClicked - 1, &mdle);

	d1(kprintf("%s/%s/%ld: Click %lu  mdle=%08lx\n", __FILE__, __FUNC__, __LINE__, ButtonClicked, mdle));

	if (mdle)
		{
		set(app->Obj[WINDOW_MAIN], MUIA_Window_Sleep, TRUE);
		Execute(mdle->mdle_Filename, 0, 0);
		set(app->Obj[WINDOW_MAIN], MUIA_Window_Sleep, FALSE);
		}
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT DesktopHiddenDevicesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct HiddenDeviceListEntry *hde = NULL;
	ULONG ButtonClicked, EntryClicked;

	ButtonClicked = getv(app->Obj[NLIST_HIDDENDEVICES], MUIA_NList_ButtonClick);
	EntryClicked = getv(app->Obj[NLIST_HIDDENDEVICES], MUIA_NList_EntryClick);

	DoMethod(app->Obj[NLIST_HIDDENDEVICES], MUIM_NList_GetEntry,
		EntryClicked, &hde);

	d1(KPrintF("%s/%s/%ld: Click button=%lu  entry=%lu  hde=%08lx <%s>\n", \
		__FILE__, __FUNC__, __LINE__, ButtonClicked, EntryClicked, hde, hde->hde_DeviceName));

	if (hde)
		{
		// toggle select state
		hde->hde_Hidden = !hde->hde_Hidden;
		set(hde->hde_CheckboxImage, MUIA_Selected, hde->hde_Hidden);

		if (hde->hde_Hidden)
			AddHiddenDevice(hde->hde_DeviceName);
		else
			RemoveHiddenDevice(hde->hde_DeviceName);

		DoMethod(app->Obj[NLIST_HIDDENDEVICES], MUIM_NList_RedrawEntry,  hde);
                }
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT IconsSelectBobRoutineHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	ULONG BobMethod;

	BobMethod = getv(app->Obj[CYCLE_ROUTINE], MUIA_Cycle_Active);

	set(app->Obj[CYCLE_TRANSPMODE], MUIA_Disabled, DRAGMETHOD_GELS == BobMethod);

	if (DRAGMETHOD_GELS == BobMethod)
		set(app->Obj[CYCLE_TRANSPMODE], MUIA_Cycle_Active, TRANSPTYPE_Ghosted);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT CreateIconsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	get(app->Obj[MENU_CREATE_ICONS], MUIA_Menuitem_Checked, &fCreateIcons);

	SetAttributeForAllPrefsPlugins(MUIA_ScalosPrefs_CreateIcons, fCreateIcons);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT PageChangeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	ULONG ActivePage = ~0;

	get(app->Obj[GROUP], MUIA_Group_ActivePage, &ActivePage);

	NotifyPrefsPluginsPageChange(ActivePage);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT PluginListAppMsgHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct AppMessage *AppMsg = *((struct AppMessage **) msg);
	ULONG n;

	for (n=0; n<AppMsg->am_NumArgs; n++)
		{
		char Path[MAX_FILENAME];

		if (NameFromLock(AppMsg->am_ArgList[n].wa_Lock, Path, sizeof(Path))
			&& AddPart(Path, AppMsg->am_ArgList[n].wa_Name, sizeof(Path)))
			{
			AddPlugin(app, Path);
			}
		}
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT ScreenTtfPopOpenHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	APTR ttRequest;
	ULONG Success = 0;

	ttRequest = TT_AllocRequest();

	if (ttRequest)
		{
		struct Window *PrefsWindow = NULL;
		struct TagItem *AttrList;
		char FontName[MAX_TTFONTDESC];
		ULONG FontStyle, FontWeight, FontSize;
		STRPTR FontDesc = NULL;

		set(app->Obj[WINDOW_MAIN], MUIA_Window_Sleep, TRUE);

		get(app->Obj[WINDOW_MAIN], MUIA_Window_Window, &PrefsWindow);
		get(o, MUIA_String_Contents, &FontDesc);

		ParseTTFontFromDesc(FontDesc, &FontStyle, &FontWeight, 
			&FontSize, FontName, sizeof(FontName));

		//TT_RequestA()
		AttrList = TT_Request(ttRequest,
			TTRQ_Window, (ULONG) PrefsWindow,
			TTRQ_TitleText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTSCREENFONT_ASLTITLE),
			TTRQ_PositiveText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTFONT_ASL_OKBUTTON),
			TTRQ_NegativeText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTFONT_ASL_CANCELBUTTON),
			TTRQ_DoSizes, TRUE,
			TTRQ_DoStyle, TRUE,
			TTRQ_DoWeight, TRUE,
			TTRQ_Activate, TRUE,
			TTRQ_DoPreview, TRUE,
			TTRQ_InitialName, (ULONG) FontName,
			TTRQ_InitialSize, FontSize,
			TTRQ_InitialStyle, FontStyle,
			TAG_END);

		d1(kprintf("%s/%s/%ld: AttrList=%08lx\n", __FILE__, __FUNC__, __LINE__, AttrList));

		if (AttrList)
			{
			char buffer[MAX_TTFONTDESC];

			BuildTTDescFromAttrList(buffer, sizeof(buffer), AttrList);

			d1(kprintf("%s/%s/%ld: FontDesc=<%s>\n", __FILE__, __FUNC__, __LINE__, buffer));

			set(o, MUIA_String_Contents, buffer);
			set(app->Obj[MCC_TTSCREENFONT_SAMPLE], MUIA_FontSample_TTFontDesc, buffer);

			Success = 1;
			}

		TT_FreeRequest(ttRequest);
		}

	DoMethod(o, MUIM_Popstring_Close, Success);
}

SAVEDS(void) INTERRUPT ScreenTtfPopCloseHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	set(app->Obj[WINDOW_MAIN], MUIA_Window_Sleep, FALSE);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT IconTtfPopOpenHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	APTR ttRequest;
	ULONG Success = 0;

	ttRequest = TT_AllocRequest();

	if (ttRequest)
		{
		struct Window *PrefsWindow = NULL;
		struct TagItem *AttrList;
		char FontName[MAX_TTFONTDESC];
		ULONG FontStyle, FontWeight, FontSize;
		STRPTR FontDesc = NULL;

		set(app->Obj[WINDOW_MAIN], MUIA_Window_Sleep, TRUE);

		get(app->Obj[WINDOW_MAIN], MUIA_Window_Window, &PrefsWindow);
		get(o, MUIA_String_Contents, &FontDesc);

		ParseTTFontFromDesc(FontDesc, &FontStyle, &FontWeight, 
			&FontSize, FontName, sizeof(FontName));

		//TT_RequestA()
		AttrList = TT_Request(ttRequest,
			TTRQ_Window, (ULONG) PrefsWindow,
			TTRQ_TitleText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTICONFONT_ASLTITLE),
			TTRQ_PositiveText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTFONT_ASL_OKBUTTON),
			TTRQ_NegativeText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTFONT_ASL_CANCELBUTTON),
			TTRQ_DoSizes, TRUE,
			TTRQ_DoStyle, TRUE,
			TTRQ_DoWeight, TRUE,
			TTRQ_Activate, TRUE,
			TTRQ_DoPreview, TRUE,
			TTRQ_InitialName, (ULONG) FontName,
			TTRQ_InitialSize, FontSize,
			TTRQ_InitialStyle, FontStyle,
			TAG_END);

		d1(kprintf("%s/%s/%ld: AttrList=%08lx\n", __FILE__, __FUNC__, __LINE__, AttrList));

		if (AttrList)
			{
			char buffer[MAX_TTFONTDESC];

			BuildTTDescFromAttrList(buffer, sizeof(buffer), AttrList);

			d1(kprintf("%s/%s/%ld: FontDesc=<%s>\n", __FILE__, __FUNC__, __LINE__, buffer));

			set(app->Obj[POPSTRING_TTICONFONT],  MUIA_String_Contents, buffer);
			set(app->Obj[POPSTRING_ICONSPAGE_TTICONFONT],  MUIA_String_Contents, buffer);

			set(app->Obj[MCC_TTICONFONT_SAMPLE], MUIA_FontSample_TTFontDesc, buffer);
			set(app->Obj[MCC_ICONSPAGE_TTICONFONT_SAMPLE], MUIA_FontSample_TTFontDesc, buffer);

			Success = 1;
			}

		TT_FreeRequest(ttRequest);
		}

	DoMethod(o, MUIM_Popstring_Close, Success);
}

SAVEDS(void) INTERRUPT IconTtfPopCloseHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	set(app->Obj[WINDOW_MAIN], MUIA_Window_Sleep, FALSE);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT TextWindowTtfPopOpenHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	APTR ttRequest;
	ULONG Success = 0;

	ttRequest = TT_AllocRequest();

	if (ttRequest)
		{
		struct Window *PrefsWindow = NULL;
		struct TagItem *AttrList;
		char FontName[MAX_TTFONTDESC];
		ULONG FontStyle, FontWeight, FontSize;
		STRPTR FontDesc = NULL;

		set(app->Obj[WINDOW_MAIN], MUIA_Window_Sleep, TRUE);

		get(app->Obj[WINDOW_MAIN], MUIA_Window_Window, &PrefsWindow);
		get(o, MUIA_String_Contents, &FontDesc);

		ParseTTFontFromDesc(FontDesc, &FontStyle, &FontWeight, 
			&FontSize, FontName, sizeof(FontName));

		//TT_RequestA()
		AttrList = TT_Request(ttRequest,
			TTRQ_Window, (ULONG) PrefsWindow,
			TTRQ_TitleText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTTEXTWINDOWFONT_ASLTITLE),
			TTRQ_PositiveText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTFONT_ASL_OKBUTTON),
			TTRQ_NegativeText, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SELECTFONT_ASL_CANCELBUTTON),
			TTRQ_DoSizes, TRUE,
			TTRQ_DoStyle, TRUE,
			TTRQ_DoWeight, TRUE,
			TTRQ_Activate, TRUE,
			TTRQ_DoPreview, TRUE,
			TTRQ_InitialName, (ULONG) FontName,
			TTRQ_InitialSize, FontSize,
			TTRQ_InitialStyle, FontStyle,
			TAG_END);

		d1(kprintf("%s/%s/%ld: AttrList=%08lx\n", __FILE__, __FUNC__, __LINE__, AttrList));

		if (AttrList)
			{
			char buffer[MAX_TTFONTDESC];

			BuildTTDescFromAttrList(buffer, sizeof(buffer), AttrList);

			d1(kprintf("%s/%s/%ld: FontDesc=<%s>\n", __FILE__, __FUNC__, __LINE__, buffer));

			set(app->Obj[POPSTRING_TTTEXTWINDOWFONT],  MUIA_String_Contents, buffer);
			set(app->Obj[POPSTRING_TEXTWINDOW_TTTEXTWINDOWFONT],  MUIA_String_Contents, buffer);

			set(app->Obj[MCC_TTTEXTWINDOWFONT_SAMPLE], MUIA_FontSample_TTFontDesc, buffer);
			set(app->Obj[MCC_TEXTWINDOW_TTTEXTWINDOWFONT_SAMPLE], MUIA_FontSample_TTFontDesc, buffer);

			Success = 1;
			}

		TT_FreeRequest(ttRequest);
		}

	DoMethod(o, MUIM_Popstring_Close, Success);
}

SAVEDS(void) INTERRUPT TextWindowTtfPopCloseHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	set(app->Obj[WINDOW_MAIN], MUIA_Window_Sleep, FALSE);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT IconDragTransparencyHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	ULONG active = 0;

	// Disable DragnDrop Icon transparency if transparent dragging is not enabled

	get(app->Obj[CYCLE_TRANSPMODE], MUIA_Cycle_Active, &active);
	set(app->Obj[SLIDER_ICONTRANSPARENCY_DRAG], MUIA_Disabled, TRANSPTYPE_Transparent != active);
}

//-----------------------------------------------------------------

static void BuildTTDescFromAttrList(char *buffer, size_t length, struct TagItem *AttrList)
{
	ULONG FontStyle;
	ULONG FontWeight;
	ULONG FontSize;
	const STRPTR *FontFamilyTable;

	FontStyle = GetTagData(TT_FontStyle, TT_FontStyle_Regular, AttrList);
	FontWeight = GetTagData(TT_FontWeight, TT_FontWeight_Normal, AttrList);
	FontSize = GetTagData(TT_FontSize, 12, AttrList);
	FontFamilyTable = (const STRPTR *) GetTagData(TT_FamilyTable, (ULONG) NULL, AttrList);

	sprintf(buffer, "%ld/%ld/%ld/%s", (long)FontStyle, (long)FontWeight, (long)FontSize, FontFamilyTable[0]);
}

//-----------------------------------------------------------------

static BOOL ParseTTFontFromDesc(CONST_STRPTR FontDesc, 
	ULONG *FontStyle, ULONG *FontWeight, ULONG *FontSize,
	STRPTR FontName, size_t FontNameSize)
{
	CONST_STRPTR lp;
	long long1,long2,long3;

	strcpy(FontName, "");
	*FontStyle = 0;
	*FontWeight = 0;
	*FontSize = 0;

	// Font Desc format:
	// "style/weight/size/fontname"

	if (3 != sscanf(FontDesc, "%ld/%ld/%ld", &long1, &long2, &long3))
		return FALSE;

	*FontStyle = long1;
	*FontWeight = long2;
	*FontSize = long3;

	lp = strchr(FontDesc, '/');	// Find "/" between style and weight
	if (NULL == lp)
		return FALSE;

	lp = strchr(lp + 1, '/');	// Find "/" between weight and size
	if (NULL == lp)
		return FALSE;

	lp = strchr(lp + 1, '/');	// Find "/" between size and name
	if (NULL == lp)
		return FALSE;

	// copy font name
	stccpy(FontName, 1 + lp, FontNameSize);

	return TRUE;
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT IconsMinSizeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct Rectangle SizeConstraints;

	GetIconSizeConstraints(app, &SizeConstraints);
	AdjustIconSizeSample(app, &SizeConstraints);

	if (SizeConstraints.MaxX < SizeConstraints.MinX)
		{
		// Adjust MaxSize if MaxSize is smaller than MinSize
		SizeConstraints.MaxX = SizeConstraints.MaxY = SizeConstraints.MinX;
		SetIconSizeConstraints(app, &SizeConstraints);
		}
}


SAVEDS(void) INTERRUPT IconsMaxSizeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct Rectangle SizeConstraints;

	GetIconSizeConstraints(app, &SizeConstraints);
	AdjustIconSizeSample(app, &SizeConstraints);

	if (SizeConstraints.MaxX < SizeConstraints.MinX)
		{
		// Adjust MinSize if MaxSize is smaller than MinSize
		SizeConstraints.MinX = SizeConstraints.MinY = SizeConstraints.MaxX;
		SetIconSizeConstraints(app, &SizeConstraints);
		}
}

//-----------------------------------------------------------------
/*** Removed! ***
SAVEDS(void) INTERRUPT ThumbnailsShowModeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	ULONG ShowMode;

	ShowMode = getv(app->Obj[CYCLE_SHOWTHUMBNAILS], MUIA_Cycle_Active);

	if (SHOWTHUMBNAILS_AS_DEFAULT != ShowMode)
		// Disable checkmark allowing thumbnail
		set(app->Obj[CHECK_SHOWTHUMBNAILS_AS_DEFAULT], MUIA_Disabled, TRUE);
	else
		set(app->Obj[CHECK_SHOWTHUMBNAILS_AS_DEFAULT], MUIA_Disabled, FALSE);
}
***/
//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT IconFrameHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	ULONG FrameTypeNormal = 0;
	ULONG FrameTypeSelected = 0;
	BOOL RecessedNormal;
	BOOL RecessedSelected;
	const struct FrameSize *fs;
	ULONG BorderLeft, BorderRight, BorderTop, BorderBottom;
	ULONG Border = 0;

	get(app->Obj[FRAME_ICONNORMAL], MUIA_MCPFrame_FrameType, &FrameTypeNormal);
	get(app->Obj[FRAME_ICONSELECTED], MUIA_MCPFrame_FrameType, &FrameTypeSelected);

	RecessedNormal = (FrameTypeNormal & MCP_FRAME_RECESSED) ? TRUE : FALSE;
	FrameTypeNormal &= ~MCP_FRAME_RECESSED;

	RecessedSelected = (FrameTypeSelected & MCP_FRAME_RECESSED) ? TRUE : FALSE;
	FrameTypeSelected &= ~MCP_FRAME_RECESSED;

	// find minimum icon borders

	BorderLeft = BorderRight = BorderTop = BorderBottom = 0;

	fs = McpGetFrameSize(FrameTypeNormal);
	if (fs)
		{
		if (RecessedNormal)
			{
			BorderLeft = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RELEFT]);
			BorderRight = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RERIGHT]);
			BorderTop = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RETOP]);
			BorderBottom = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_REBOTTOM]);
			}
		else
			{
			BorderLeft = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_LEFT]);
			BorderRight = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RIGHT]);
			BorderTop = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_TOP]);
			BorderBottom = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_BOTTOM]);
			}
		}

	fs = McpGetFrameSize(FrameTypeSelected);
	if (fs)
		{
		if (RecessedSelected)
			{
			BorderLeft = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RELEFT]);
			BorderRight = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RERIGHT]);
			BorderTop = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RETOP]);
			BorderBottom = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_REBOTTOM]);
			}
		else
			{
			BorderLeft = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_LEFT]);
			BorderRight = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RIGHT]);
			BorderTop = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_TOP]);
			BorderBottom = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_BOTTOM]);
			}
		}

	// now adjust icon borders if they are smaller than required

	get(app->Obj[ICONLEFT], MUIA_Numeric_Value, &Border);
	if (Border < BorderLeft)
		setslider(app->Obj[ICONLEFT], BorderLeft);

	get(app->Obj[ICONRIGHT], MUIA_Numeric_Value, &Border);
	if (Border < BorderRight)
		setslider(app->Obj[ICONRIGHT], BorderRight);

	get(app->Obj[ICONTOP], MUIA_Numeric_Value, &Border);
	if (Border < BorderTop)
		setslider(app->Obj[ICONTOP], BorderTop);

	get(app->Obj[ICONBOTTOM], MUIA_Numeric_Value, &Border);
	if (Border < BorderBottom)
		setslider(app->Obj[ICONBOTTOM], BorderBottom);

}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT ThumbnailFrameHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	ULONG FrameTypeNormal = 0;
	ULONG FrameTypeSelected = 0;
	BOOL RecessedNormal;
	BOOL RecessedSelected;
	const struct FrameSize *fs;
	ULONG BorderLeft, BorderRight, BorderTop, BorderBottom;
	ULONG Border = 0;

	get(app->Obj[FRAME_ICON_THUMBNAIL_NORMAL], MUIA_MCPFrame_FrameType, &FrameTypeNormal);
	get(app->Obj[FRAME_ICON_THUMBNAIL_SELECTED], MUIA_MCPFrame_FrameType, &FrameTypeSelected);

	d1(KPrintF("%s/%s/%ld: START FrameTypeNormal=%ld  FrameTypeSelected=%ld\n", \
		__FILE__, __FUNC__, __LINE__, FrameTypeNormal, FrameTypeSelected));

	RecessedNormal = (FrameTypeNormal & MCP_FRAME_RECESSED) ? TRUE : FALSE;
	FrameTypeNormal &= ~MCP_FRAME_RECESSED;

	RecessedSelected = (FrameTypeSelected & MCP_FRAME_RECESSED) ? TRUE : FALSE;
	FrameTypeSelected &= ~MCP_FRAME_RECESSED;

	// find minimum thumbnail borders

	BorderLeft = BorderRight = BorderTop = BorderBottom = 0;

	fs = McpGetFrameSize(FrameTypeNormal);
	if (fs)
		{
		if (RecessedNormal)
			{
			BorderLeft = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RELEFT]);
			BorderRight = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RERIGHT]);
			BorderTop = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RETOP]);
			BorderBottom = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_REBOTTOM]);
			}
		else
			{
			BorderLeft = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_LEFT]);
			BorderRight = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RIGHT]);
			BorderTop = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_TOP]);
			BorderBottom = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_BOTTOM]);
			}
		}

	fs = McpGetFrameSize(FrameTypeSelected);
	if (fs)
		{
		if (RecessedSelected)
			{
			BorderLeft = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RELEFT]);
			BorderRight = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RERIGHT]);
			BorderTop = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RETOP]);
			BorderBottom = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_REBOTTOM]);
			}
		else
			{
			BorderLeft = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_LEFT]);
			BorderRight = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_RIGHT]);
			BorderTop = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_TOP]);
			BorderBottom = max(BorderLeft, fs->fs_Sizes[FRAMESIZE_BOTTOM]);
			}
		}

	// now adjust thumbnail borders if they are smaller than required

	get(app->Obj[THUMBNAILS_LEFTBORDER], MUIA_Numeric_Value, &Border);
	if (Border < BorderLeft)
		setslider(app->Obj[THUMBNAILS_LEFTBORDER], BorderLeft);

	get(app->Obj[THUMBNAILS_RIGHTBORDER], MUIA_Numeric_Value, &Border);
	if (Border < BorderRight)
		setslider(app->Obj[THUMBNAILS_RIGHTBORDER], BorderRight);

	get(app->Obj[THUMBNAILS_TOPBORDER], MUIA_Numeric_Value, &Border);
	if (Border < BorderTop)
		setslider(app->Obj[THUMBNAILS_TOPBORDER], BorderTop);

	get(app->Obj[THUMBNAILS_BOTTOMBORDER], MUIA_Numeric_Value, &Border);
	if (Border < BorderBottom)
		setslider(app->Obj[THUMBNAILS_BOTTOMBORDER], BorderBottom);
}

//----------------------------------------------------------------------------

SAVEDS(void) INTERRUPT AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct IntuiMessage *iMsg = (struct IntuiMessage *) msg;

	if (IDCMP_REFRESHWINDOW == iMsg->Class)
		{
		DoMethod(app->Obj[APPLICATION], MUIM_Application_CheckRefresh);
		}
}

//----------------------------------------------------------------------------

SAVEDS(APTR) INTERRUPT BrowserCmdSelectedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct CommandTableEntry *cte = NULL;

	DoMethod(o, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &cte);
	if (cte)
		{
		setstring(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION], (ULONG) cte->cte_Command);

		DoMethod(app->Obj[POP_CONTROLBARGADGETS_BROWSER_ACTION], MUIM_Popstring_Close, 0);

		ControlBarGadgetBrowserChangedHookFunc(hook, o, msg);
		}

	return 0;
}

//----------------------------------------------------------------------------

SAVEDS(APTR) INTERRUPT NormalCmdSelectedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct CommandTableEntry *cte = NULL;

	DoMethod(o, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &cte);
	if (cte)
		{
		setstring(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION], (ULONG) cte->cte_Command);

		DoMethod(app->Obj[POP_CONTROLBARGADGETS_NORMAL_ACTION], MUIM_Popstring_Close, 0);

		ControlBarGadgetNormalChangedHookFunc(hook, o, msg);
		}

	return 0;
}

//----------------------------------------------------------------------------

SAVEDS(APTR) INTERRUPT ControlBarGadgetBrowserChangedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct ControlBarGadgetEntry *cgy = NULL;

	DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIM_NList_GetEntry, MUIV_List_GetEntry_Active, &cgy);

	d1(KPrintF("%s/%s/%ld: cgy=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy));

	if (cgy && SCPGadgetType_ActionButton == cgy->cgy_GadgetType)
		{
		STRPTR str = NULL;
		ULONG Changed = 0;

		get(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIA_String_Contents, &str);
		d1(KPrintF("%s/%s/%ld: str=%08lx\n", __FILE__, __FUNC__, __LINE__, str));
		if (0 != strcmp(str, cgy->cgy_NormalImage))
			{
			Changed++;

			if (cgy->cgy_NormalImage)
				free(cgy->cgy_NormalImage);
			cgy->cgy_NormalImage = strdup(str);

			DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIM_NList_UseImage,
				NULL, cgy->cgy_ImageObjectIndex, 0);
			MUI_DisposeObject(cgy->cgy_Image);

			if (ExistsObject(cgy->cgy_NormalImage))
				{
				cgy->cgy_Image = NewObject(DataTypesImageClass->mcc_Class, 0,
					MUIA_ScaDtpic_Name, (ULONG) cgy->cgy_NormalImage,
					TAG_END); //DataTypesMCC

				set(app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIA_ScaDtpic_Name,
					cgy->cgy_NormalImage);
				d1(KPrintF("%s/%s/%ld:  <%s>  cgy_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy->cgy_NormalImage, cgy->cgy_Image));
				}
			else
				{
				set(app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIA_ScaDtpic_Name,
					(STRPTR) "");
				cgy->cgy_Image = NULL;
				}

			DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIM_NList_UseImage,
				cgy->cgy_Image, cgy->cgy_ImageObjectIndex, 0);
			}

		get(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], MUIA_String_Contents, &str);
		d1(KPrintF("%s/%s/%ld: str=%08lx\n", __FILE__, __FUNC__, __LINE__, str));
		if (0 != strcmp(str, cgy->cgy_SelectedImage))
			{
			Changed++;

			if (cgy->cgy_SelectedImage)
				free(cgy->cgy_SelectedImage);
			cgy->cgy_SelectedImage = strdup(str);

			set(app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], MUIA_ScaDtpic_Name,
				cgy->cgy_SelectedImage);
			}

		get(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], MUIA_String_Contents, &str);
		d1(KPrintF("%s/%s/%ld: str=%08lx\n", __FILE__, __FUNC__, __LINE__, str));
		if (0 != strcmp(str, cgy->cgy_SelectedImage))
			{
			Changed++;

			if (cgy->cgy_DisabledImage)
				free(cgy->cgy_DisabledImage);
			cgy->cgy_DisabledImage = strdup(str);

			set(app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], MUIA_ScaDtpic_Name,
				cgy->cgy_DisabledImage);
			}

		get(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION], MUIA_String_Contents, &str);
		d1(KPrintF("%s/%s/%ld: str=%08lx\n", __FILE__, __FUNC__, __LINE__, str));
		if (0 != strcmp(str, cgy->cgy_Action))
			{
			Changed++;

			stccpy(cgy->cgy_Action, str, sizeof(cgy->cgy_Action));
			}

		set(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT],MUIA_TextEditor_HasChanged, FALSE);
		str = (STRPTR) DoMethod(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT], MUIM_TextEditor_ExportText);
		d1(KPrintF("%s/%s/%ld: str=%08lx  cgy_HelpText=%08lx\n", __FILE__, __FUNC__, __LINE__, str, cgy->cgy_HelpText));
		if ((NULL == cgy->cgy_HelpText) || (0 != strcmp(str, cgy->cgy_HelpText)))
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			Changed++;

			if (cgy->cgy_HelpText)
				free(cgy->cgy_HelpText);
			cgy->cgy_HelpText = strdup(str);
			}

		d1(KPrintF("%s/%s/%ld: Changed=%ld\n", __FILE__, __FUNC__, __LINE__, Changed));
		if (Changed)
			DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIM_NList_RedrawEntry, cgy);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//----------------------------------------------------------------------------

SAVEDS(APTR) INTERRUPT ControlBarGadgetNormalChangedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct ControlBarGadgetEntry *cgy = NULL;

	DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIM_NList_GetEntry, MUIV_List_GetEntry_Active, &cgy);

	d1(KPrintF("%s/%s/%ld: cgy=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy));

	if (cgy && SCPGadgetType_ActionButton == cgy->cgy_GadgetType)
		{
		STRPTR str = NULL;
		ULONG Changed = 0;

		get(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIA_String_Contents, &str);
		d1(KPrintF("%s/%s/%ld: str=%08lx\n", __FILE__, __FUNC__, __LINE__, str));
		if (0 != strcmp(str, cgy->cgy_NormalImage))
			{
			Changed++;

			if (cgy->cgy_NormalImage)
				free(cgy->cgy_NormalImage);
			cgy->cgy_NormalImage = strdup(str);

			DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIM_NList_UseImage,
				NULL, cgy->cgy_ImageObjectIndex, 0);
			MUI_DisposeObject(cgy->cgy_Image);

			if (ExistsObject(cgy->cgy_NormalImage))
				{
				cgy->cgy_Image = NewObject(DataTypesImageClass->mcc_Class, 0,
					MUIA_ScaDtpic_Name, (ULONG) cgy->cgy_NormalImage,
					TAG_END); //DataTypesMCC

				set(app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIA_ScaDtpic_Name,
					cgy->cgy_NormalImage);

				d1(KPrintF("%s/%s/%ld:  <%s>  cgy_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy->cgy_NormalImage, cgy->cgy_Image));
				}
			else
				{
				set(app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIA_ScaDtpic_Name,
					(STRPTR) "");
				cgy->cgy_Image = NULL;
				}

			DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIM_NList_UseImage,
				cgy->cgy_Image, cgy->cgy_ImageObjectIndex, 0);
			}

		get(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], MUIA_String_Contents, &str);
		d1(KPrintF("%s/%s/%ld: str=%08lx\n", __FILE__, __FUNC__, __LINE__, str));
		if (0 != strcmp(str, cgy->cgy_SelectedImage))
			{
			Changed++;

			if (cgy->cgy_SelectedImage)
				free(cgy->cgy_SelectedImage);
			cgy->cgy_SelectedImage = strdup(str);

			set(app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], MUIA_ScaDtpic_Name,
				cgy->cgy_SelectedImage);
			}

		get(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], MUIA_String_Contents, &str);
		d1(KPrintF("%s/%s/%ld: str=%08lx\n", __FILE__, __FUNC__, __LINE__, str));
		if (0 != strcmp(str, cgy->cgy_SelectedImage))
			{
			Changed++;

			if (cgy->cgy_DisabledImage)
				free(cgy->cgy_DisabledImage);
			cgy->cgy_DisabledImage = strdup(str);

			set(app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], MUIA_ScaDtpic_Name,
				cgy->cgy_DisabledImage);
			}

		get(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION], MUIA_String_Contents, &str);
		d1(KPrintF("%s/%s/%ld: str=%08lx\n", __FILE__, __FUNC__, __LINE__, str));
		if (0 != strcmp(str, cgy->cgy_Action))
			{
			Changed++;

			stccpy(cgy->cgy_Action, str, sizeof(cgy->cgy_Action));
			}

		set(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT],MUIA_TextEditor_HasChanged, FALSE);
		str = (STRPTR) DoMethod(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT], MUIM_TextEditor_ExportText);
		d1(KPrintF("%s/%s/%ld: str=%08lx  cgy_HelpText=%08lx\n", __FILE__, __FUNC__, __LINE__, str, cgy->cgy_HelpText));
		if ((NULL == cgy->cgy_HelpText) || (0 != strcmp(str, cgy->cgy_HelpText)))
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			Changed++;

			if (cgy->cgy_HelpText)
				free(cgy->cgy_HelpText);
			cgy->cgy_HelpText = strdup(str);
			}

		d1(KPrintF("%s/%s/%ld: Changed=%ld\n", __FILE__, __FUNC__, __LINE__, Changed));
		if (Changed)
			DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIM_NList_RedrawEntry, cgy);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT CalculateMaxRadiusHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	BOOL useTT;
	LONG YBorder;
	LONG FontHeight;
	LONG MaxRadius;
	CONST_STRPTR lp;

	useTT = getv(app->Obj[CHECK_TTICONFONT_ENABLE], MUIA_Selected);

	if (useTT)
		{
		lp = (CONST_STRPTR) getv(app->Obj[POPSTRING_TTICONFONT], MUIA_String_Contents);
		FontHeight = TTFontHeightFromDesc(lp);
		}
	else
		{
		lp = (CONST_STRPTR) getv(app->Obj[POP_ICONFONT], MUIA_String_Contents);
		FontHeight = DiskFontHeightFromDesc(lp);
		}


	YBorder = getv(app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERY], MUIA_Numeric_Value);

	MaxRadius = (YBorder + FontHeight) / 2;

	set(app->Obj[SLIDER_SEL_ICONTEXT_RECT_RADIUS], MUIA_Numeric_Max, MaxRadius);
}

//-----------------------------------------------------------------

static LONG TTFontHeightFromDesc(CONST_STRPTR FontDesc)
{
	// LONG FontStyle;
	// LONG FontWeight;
	LONG FontSize;
	CONST_STRPTR lp;
	long long1, long2, long3;

	// Font Desc format:
	// "style/weight/size/fontname"

	if (NULL == FontDesc)
		return 0;

	if (3 != sscanf(FontDesc, "%ld/%ld/%ld", &long1, &long2, &long3))
		return 0;

	// FontStyle = long1;
	// FontWeight = long2;
	FontSize = long3;

	lp = strchr(FontDesc, '/');	// Find "/" between style and weight
	if (NULL == lp)
		return 0;

	lp = strchr(lp + 1, '/');	// Find "/" between weight and size
	if (NULL == lp)
		return 0;

	lp = strchr(lp + 1, '/');	// Find "/" between size and name
	if (NULL == lp)
		return 0;

	return FontSize;
}

//-----------------------------------------------------------------

static LONG DiskFontHeightFromDesc(CONST_STRPTR FontDesc)
{
	// Font Desc format:
	// "fontname/size"
	struct TextAttr ta;
	CONST_STRPTR fp;
	size_t fnLen;
	LONG l;

	for (fp=FontDesc, fnLen=0; *fp && '/' != *fp; fnLen++)
		fp++;

	if ('/' == *fp)
		fp++;

	if (StrToLong(fp, &l))
		ta.ta_YSize = l;
	else
		ta.ta_YSize = 8;

	return ta.ta_YSize;
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT UpdateSelectMarkerSampleHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	struct ARGB argb;

	argb.Alpha = getv(app->Obj[SLIDER_TEXTWINDOWS_SELECTBORDERTRANSPARENCY], MUIA_Numeric_Value);

	argb.Red   = getv(app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK], MUIA_Coloradjust_Red) >> 24;
	argb.Green = getv(app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK], MUIA_Coloradjust_Green) >> 24;
	argb.Blue  = getv(app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK], MUIA_Coloradjust_Blue) >> 24;

	set(app->Obj[MCC_TEXTWINDOWS_SELECTMARKER_SAMPLE], TIHA_BaseColor, (ULONG) &argb);
}

//-----------------------------------------------------------------

SAVEDS(void) INTERRUPT ChangeThumbnailSizeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;
	ULONG SizeIndex;

	SizeIndex = getv(app->Obj[CYCLE_THUMBNAILSIZE], MUIA_Cycle_Active);

	set(app->Obj[GROUP_THUMBNAILS_ICON_SAMPLE], MUIA_Group_ActivePage, SizeIndex);
}

//-----------------------------------------------------------------




/// System includes
#define AROS_ALMOST_COMPATIBLE
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <libraries/asl.h>
#include <workbench/workbench.h>

#include <proto/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <exec/libraries.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <dos/dos.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>

#include <proto/alib.h>
#include <proto/utility.h>

#include <proto/datatypes.h>
#include <clib/dtclass_protos.h>
#include <datatypes/pictureclass.h>
#include <devices/rawkeycodes.h>

#include <libraries/gadtools.h>

#include <emul/emulregs.h>
#include <emul/emulinterface.h>
////

#include <private/vapor/vapor.h>
#include "util.h"
#include "poppler.h"
#include "application.h"
#include "window.h"
#include "logger.h"
#include "settings.h"
#include "printer.h"
#include "system/functions.h"

#include "mcc/documentview_class.h"
#include "mcc/toolbar_class.h"
#include "mcc/renderer_class.h"
#include "mcc/title_class.h"

#include <mui/Aboutbox_mcc.h>

#define MAXRECENT 10

struct Data
{
	Object *renderer;
	Object *menu;
	int windowid;
	Object *logger;
	Object *loggerwin;
	Object *settings;
	Object *printer;
	char *recent[MAXRECENT];
	Object *aboutbox;
};

/* MEnu description */

enum
{
	MEN_PROJECT = 1,
	MEN_ABOUT, MEN_OPEN_FILE, MEN_OPEN_URL, MEN_TAB_NEW, MEN_TAB_CLOSE, MEN_WINDOW_NEW, MEN_WINDOW_CLOSE,
	MEN_OPEN_RECENT, 
	MEN_PRINT,
	MEN_QUIT,
	MEN_SETTINGS, MEN_SETTINGS_SETTINGS, MEN_SETTINGS_MUI,
	MEN_OPEN_RECENT0 = 100 /* keep it last*/
};

#define	RB  CHECKIT
#define	TG  CHECKIT|MENUTOGGLE
#define	CK	CHECKIT

static struct NewMenu MenuData[] =
{
	{ NM_TITLE, "Project"                , 0,0,0            ,(APTR)MEN_PROJECT },
	{ NM_ITEM,  "New Window"             ,"N",0,0           ,(APTR)MEN_WINDOW_NEW},
	{ NM_ITEM,  "New Tab"                ,"T",0,0           ,(APTR)MEN_TAB_NEW},
	{ NM_ITEM,  "Open File..."           ,"O",0,0           ,(APTR)MEN_OPEN_FILE},
#if 0
	{ NM_ITEM,  "Open URL..."            ,"L",0,0           ,(APTR)MEN_OPEN_URL},
#endif
	{ NM_ITEM,  "Open Recent"            , 0,0,0            ,(APTR)MEN_OPEN_RECENT},
	{ NM_ITEM,  "Print..."               , "P",0,0            ,(APTR)MEN_PRINT},
	{ NM_ITEM,  NM_BARLABEL              , 0,0,0            ,(APTR)0},
	{ NM_ITEM,  "About..."               ,"?",0,0           ,(APTR)MEN_ABOUT},
	{ NM_ITEM,  NM_BARLABEL              , 0,0,0            ,(APTR)0},
	{ NM_ITEM,  "Quit"                   ,"Q",0,0           ,(APTR)MEN_QUIT},
	{ NM_TITLE, "Settings"               , 0,0,0            ,(APTR)MEN_SETTINGS },
	{ NM_ITEM,  "Settings..."            , 0,0,0            ,(APTR)MEN_SETTINGS_SETTINGS},
	{ NM_ITEM,  "MUI..."                 , 0,0,0            ,(APTR)MEN_SETTINGS_MUI},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};

/* rexx handling */

#define REXXHOOK(name, param)	static const struct Hook name = { { NULL, NULL }, (HOOKFUNC)&RexxEmul, NULL, (APTR)(param) }

enum
{
	REXX_ABOUT,
	REXX_SAVE,
	REXX_OPEN,
	REXX_APPTOFRONT
};

static LONG Rexx(void);
static const struct EmulLibEntry RexxEmul = { TRAP_LIB, 0, (void (*)())&Rexx };
REXXHOOK(RexxHookAbout, REXX_ABOUT);
REXXHOOK(RexxHookSave, REXX_SAVE);
REXXHOOK(RexxHookOpen, REXX_OPEN);
REXXHOOK(RexxHookAppToFront, REXX_APPTOFRONT);

static const struct MUI_Command rexxcommands[] =
{
	{ "ABOUT", 			NULL, 					0, (struct Hook *)&RexxHookAbout},
	{ "SAVE", 			NULL, 					0, (struct Hook *)&RexxHookSave },
	{ "OPEN", 			"FILE/K,NEWTAB/S",	    2, (struct Hook *)&RexxHookOpen },
	{ "APPTOFRONT",		"APPTOFRONT/S",			1, (struct Hook *)&RexxHookAppToFront 	 },
	{ NULL,				NULL,					0, NULL							}
};

/* credits */

static CONST_STRPTR __credits = 
"\33b%p\33n\n"   
"\tMichal 'kiero' Wozniak\n\n"  
"\33b%W\33n\n"  
"\twozniak_m@interia.pl\n\n" 
"\33b%P\33n\n"    
"\tMichal Zukowski\n\n"
"\33b%§\33n\n" 
"\tVPDF uses following libraries:\n"
"\tPoppler (http://www.poppler.freedesktop.org)\n"
"\tXPDF (http://www.foolabs.com/xpdf/)\n"
"\tFreeType (http://www.freetype.org)\n\n"
"\tThis software is based in part on the work of the\n"
"\tIndependent JPEG Group"
;


/* */

static void reqInit(char *path);
static void reqEnd(void);


DEFNEW
{
	Object *menu;
	Object *settings, *settingswin, *printerwin;
	Object *btnSave, *btnUse, *btnCancel;

	obj = DoSuperNew(cl, obj,
						MUIA_Application_Menustrip, menu = MUI_MakeObject(MUIO_MenustripNM, MenuData, TAG_DONE),
						MUIA_Application_Commands, &rexxcommands,
						MUIA_Application_Window, settingswin = WindowObject,
							MUIA_Window_Title,"VPDF · Setttings",
							MUIA_Window_Width, MUIV_Window_Width_Screen(30),
							MUIA_Window_Height, MUIV_Window_Height_Screen(40),
							MUIA_Window_ID, MAKE_ID('V', 'P', 'S', 'T'),
							MUIA_Window_RootObject,	VGroup,
								Child, settings = VPDFSettingsObject,
									MUIA_Frame, MUIV_Frame_Group,
									End,
								Child, HGroup,
									Child, btnSave = SimpleButton("_Save"),
									Child, btnUse = SimpleButton("_Use"),
									//Child, SimpleButton("_Test"),
									Child, HSpace(0),
									Child, btnCancel = SimpleButton("_Cancel"),
									End,
								End,						
							End,
							MUIA_Application_Window, printerwin = VPDFPrinterObject, End,	
						
						TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;
		int i;
		memset(data, 0, sizeof(struct Data));

		data->menu = menu;
		data->settings = settings;
		data->printer = printerwin;

		/* spawn document renderer common for all views */
		data->renderer = RendererObject, End;

		/* TODO: make some separate singleton for reqs? */
		reqInit("");

		/* menu notifications */

		DoMethod(obj, MUIM_Notify, MUIA_Application_MenuAction, MEN_ABOUT, obj, 1, MUIM_VPDF_About);
		DoMethod(obj, MUIM_Notify, MUIA_Application_MenuAction, MEN_QUIT, obj, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
		DoMethod(obj, MUIM_Notify, MUIA_Application_MenuAction, MEN_TAB_NEW, obj, 2, MUIM_VPDF_CreateTab, 0);
		DoMethod(obj, MUIM_Notify, MUIA_Application_MenuAction, MEN_WINDOW_NEW, obj, 1, MUIM_VPDF_CreateWindow);
		DoMethod(obj, MUIM_Notify, MUIA_Application_MenuAction, MEN_OPEN_FILE, obj, 4, MUIM_VPDF_OpenFile, 0, NULL, MUIV_VPDFWindow_OpenFile_CurrentTabIfEmpty);
		DoMethod(obj, MUIM_Notify, MUIA_Application_MenuAction, MEN_SETTINGS_SETTINGS, settingswin, 3, MUIM_Set, MUIA_Window_Open, TRUE);
		DoMethod(obj, MUIM_Notify, MUIA_Application_MenuAction, MEN_PRINT, obj, 1, MUIM_VPDF_PrintDocument);
		
		for(i=0; i<MAXRECENT; i++)
			DoMethod(obj, MUIM_Notify, MUIA_Application_MenuAction, MEN_OPEN_RECENT0+i, obj, 3, MUIM_VPDF_OpenRecentFile, 0, i);

		DoMethod(settingswin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, settingswin, 3, MUIM_Set, MUIA_Window_Open, FALSE);
		DoMethod(btnCancel, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_VPDF_SettingsCancel);
		DoMethod(btnSave, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_VPDF_SettingsUse);
		DoMethod(btnUse, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_VPDF_SettingsSave);

		DoMethod(printerwin, MUIM_Notify, MUIA_Window_Open, MUIV_EveryTime, obj, 3, MUIM_Set, MUIA_VPDF_Locked, MUIV_TriggerValue);

	}

	return (ULONG)obj;
}

DEFDISP
{
	GETDATA;

	/* delete all windows first as they might be using renderer object */

	{
		struct List *winlist = (struct List*)xget(obj, MUIA_Application_WindowList);
		Object *winobj_state = (Object*)winlist->lh_Head;
		Object *winobj;
		while((winobj = NextObject(&winobj_state)))
		{
			set(winobj, MUIA_Window_Open, FALSE);
			DoMethod(obj, OM_REMMEMBER, winobj);
			MUI_DisposeObject(winobj);
		}
	}

	/* dispose renderer and the rest */


	MUI_DisposeObject(data->renderer);
	reqEnd();

	return DOSUPER;
}

static void setuprecent(Object *obj, struct Data *data);

DEFMMETHOD(VPDF_OpenFile)
{
	GETDATA;
	char *filename = msg->filename;
	int mode = msg->mode;
	Object *window = (Object*)DoMethod(obj, MUIM_VPDF_FindWindowByID, msg->windowid == 0 ? MUIV_VPDF_FindWindowByID_Active : msg->windowid);

	if (window == NULL)
	{
		window = (Object*)DoMethod(obj, MUIM_VPDF_CreateWindow);
		mode = MUIV_VPDFWindow_OpenFile_CurrentTabIfEmpty;
	}

	if (filename == NULL)
	{
		filename = (char*)DoMethod(_app(obj), MUIM_VPDF_RequestFile,
						MUIV_VPDF_RequestFile_Load,
						NULL,
						NULL,
						NULL);

		if (filename == NULL)
			return FALSE;
	}

	if (window != NULL)
	{
		if (DoMethod(window, MUIM_VPDFWindow_OpenFile, filename, mode))
		{
			/* add to history */

			int i;
			int dupe = FALSE;

			for(i=0; i<MAXRECENT && data->recent[i] != NULL && *data->recent[i] != '\0' && dupe == FALSE; i++)
				dupe = 0 == stricmp(data->recent[i], filename);

			if (dupe == FALSE)
			{
				free(data->recent[MAXRECENT-1]);
				for(i=MAXRECENT-1;i>=1; i--)
					data->recent[i] = data->recent[i-1];

				data->recent[0] = strdup(filename);
				setuprecent(obj, data);
			}
		}
	}

	return TRUE;
}

DEFMMETHOD(VPDF_OpenRecentFile)
{
	GETDATA;
	if (data->recent[msg->idx] != NULL && *data->recent[msg->idx] != '\0')
		return DoMethod(obj, MUIM_VPDF_OpenFile, msg->windowid, data->recent[msg->idx], MUIV_VPDFWindow_OpenFile_CurrentTabIfEmpty);

	return FALSE;
}

DEFMMETHOD(VPDF_CreateTab)
{
	Object *window = (Object*)DoMethod(obj, MUIM_VPDF_FindWindowByID, msg->windowid == 0 ? MUIV_VPDF_FindWindowByID_Active :  msg->windowid);

	if (window != NULL)
	{
		return DoMethod(window, MUIM_VPDFWindow_CreateTab);
	}

	return (ULONG)-1;
}

DEFMMETHOD(VPDF_CreateWindow)
{
	GETDATA;
	Object *refwindow = DoMethod(obj, MUIM_VPDF_FindActiveWindow);
	Object *window = VPDFWindowObject,
							MUIA_VPDFWindow_ID, ++data->windowid,
							MUIA_Window_ID, MAKE_ID('V','P', 'W','O'), /* this is going to be nullified for all non-active windows */
							MUIA_Window_LeftEdge, 20,
							MUIA_Window_TopEdge, 20,
							MUIA_Window_RefWindow, refwindow, 
							MUIA_Window_OverrideStoredPosition, refwindow != NULL ? TRUE : FALSE,
							End;

	if (window != NULL)
	{
		int id = xget(window, MUIA_VPDFWindow_ID);
		DoMethod(obj, OM_ADDMEMBER, window);
		DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 2, MUIM_VPDF_CloseWindow, id);
		DoMethod(window, MUIM_Notify, MUIA_Window_Activate, TRUE, obj, 2, MUIM_VPDF_NewActiveWindow, window);
		set(window, MUIA_Window_Open, TRUE);
	}

	return (ULONG)window;
}

DEFMMETHOD(VPDF_HandleAppMessage)
{
	struct AppMessage *amsg = msg->appmessage;
	struct WBArg *ap = amsg->am_ArgList;
	char buf[1024];

	NameFromLock(ap->wa_Lock,buf,sizeof(buf));
	AddPart(buf,ap->wa_Name,sizeof(buf));

	if (strMatch(buf, "#?.pdf"))
		DoMethod(obj, MUIM_VPDF_OpenFile, msg->windowid, buf, MUIV_VPDFWindow_OpenFile_CurrentTabIfEmpty);

	return 0;
}

/// Helper Functions (requesters)

M_HOOK(AslRefresh, struct FileRequester *req,struct IntuiMessage *imsg)
{
	if (imsg->Class==IDCMP_REFRESHWINDOW)
		DoMethod(req->fr_UserData,MUIM_Application_CheckRefresh);
}

static struct FileRequester *reqLoad = NULL;
static struct FileRequester *reqSave = NULL;

static void reqInit(char *path)
{
	char *dir = strdup(path);
	char *file = FilePart(path);
	char *fPart = PathPart(dir);

	if (fPart)
		dir[ fPart - dir ] = 0;

	reqSave = MUI_AllocAslRequestTags(ASL_FileRequest,
		ASLFR_TitleText		, "Save image...",
		ASLFR_InitialPattern, "",
		ASLFR_DoSaveMode	, TRUE,
		ASLFR_InitialDrawer	, dir,
		ASLFR_InitialFile	, file,
		ASLFR_DoPatterns	, TRUE,
		ASLFR_RejectIcons	, TRUE,
		TAG_DONE);

	reqLoad = MUI_AllocAslRequestTags(ASL_FileRequest,
		ASLFR_TitleText		, "Load image...",
		ASLFR_InitialPattern, "",
		ASLFR_DoSaveMode	, FALSE,
		ASLFR_InitialDrawer	, dir,
		ASLFR_InitialFile	, file,
		ASLFR_DoPatterns	, TRUE,
		ASLFR_RejectIcons	, TRUE,
		TAG_DONE);

	free(dir);
}

static void reqEnd(void)
{
	if (reqSave)
		MUI_FreeAslRequest(reqSave);
	if (reqLoad)
		MUI_FreeAslRequest(reqLoad);
}

DEFMMETHOD(VPDF_RequestFile)
{
	static char buf[1024];
	char *res = NULL;
	struct FileRequester *req;
	Object *window = (Object*)DoMethod(obj, MUIM_VPDF_FindActiveWindow);

	if (msg->mode & MUIV_VPDF_RequestFile_Load)
		req	= reqLoad;
	else
		req = reqSave;

	if (req != NULL)
	{
		char *title = NULL;

		set(obj, MUIA_Application_Sleep, TRUE);

		/* will extract directory path first */

		buf[ 0 ] = 0;

		if (msg->initialdir != NULL)
			stccpy(buf, msg->initialdir, sizeof(buf));
		else if (msg->initialpath != NULL)
		{
			stccpy(buf, msg->initialpath, sizeof(buf));
			buf[ (int)PathPart(buf) - (int)&buf[ 0 ] ] = 0;
		}

		/* setup proper title */

		if (msg->mode & MUIV_VPDF_RequestFile_DirectoryOnly)
			title = "Select Drawer....";
		else if (msg->mode & MUIV_VPDF_RequestFile_Load)
			title = "Select File to Open...";
		else if (msg->mode & MUIV_VPDF_RequestFile_Save)
			title = "Select File to Save...";

		if (MUI_AslRequestTags(req,
						title != NULL ? ASLFR_TitleText : TAG_IGNORE, title,
						ASLFR_UserData,	obj,
						ASLFR_IntuiMsgFunc,	&AslRefreshHook,
						ASLFR_PrivateIDCMP,	FALSE,
						buf[ 0 ] ? ASLFR_InitialDrawer : TAG_IGNORE, buf,
						ASLFR_InitialFile, msg->initialfile ? msg->initialfile : msg->initialpath ? (char*)FilePart(msg->initialpath) : (char*)"",
						ASLFR_DrawersOnly, msg->mode & MUIV_VPDF_RequestFile_DirectoryOnly,
						ASLFR_Window, window != NULL ? xget(window, MUIA_Window_Window) : NULL,
						TAG_DONE))
		{
			if (*req->fr_File)
			{
				res = buf;
				stccpy(buf, req->fr_Drawer, sizeof(buf));
				if (!(msg->mode & MUIV_VPDF_RequestFile_DirectoryOnly))
					AddPart(buf, req->fr_File, sizeof(buf));
			}
			else if (*req->fr_Drawer && (msg->mode & MUIV_VPDF_RequestFile_DirectoryOnly))
			{
				res = buf;
				stccpy(buf, req->fr_Drawer, sizeof(buf));
			}
		}

		set(obj, MUIA_Application_Sleep, FALSE);
	}

	return (ULONG)res;

}

////

static void setuprecent(Object *obj, struct Data *data)
{
	int i;
	Object *menRecent = (Object*)DoMethod(data->menu, MUIM_FindUData, MEN_OPEN_RECENT);

	DoMethod(menRecent, MUIM_Menustrip_InitChange);

	/* cleanup recents */

	{
		Object *o;
		while((o = (Object*)DoMethod(menRecent, MUIM_Family_GetChild, MUIV_Family_GetChild_First)))
		{
			DoMethod(menRecent, MUIM_Family_Remove, o);
			MUI_DisposeObject(o);
		};
	}

	for(i=0; i<MAXRECENT && data->recent[i] != NULL && *data->recent[i] != '\0'; i++)
	{
		Object *item = MenuitemObject,
					MUIA_Menuitem_Title, data->recent[i],
					MUIA_UserData, MEN_OPEN_RECENT0 + i,
					MUIA_Menuitem_CopyStrings, TRUE,
					End;

		DoMethod(menRecent, MUIM_Family_AddTail, item);
	}

	DoMethod(data->menu, MUIM_Menustrip_ExitChange);
}

/* this method finds window which was last activated, not currently active one! */

DEFMMETHOD(VPDF_FindActiveWindow)
{
	GETDATA;
	struct List *winlist = (struct List*)xget(obj, MUIA_Application_WindowList);
	Object *winobj_state = (Object*)winlist->lh_Head;
	Object *winobj;
	ULONG v;

	while((winobj = NextObject(&winobj_state)))
	{
		//if (xget(winobj, MUIA_Window_Activate))
		if (xget(winobj, MUIA_Window_ID) != 0 && get(winobj, MUIA_VPDFWindow_ID, &v))
			return winobj;
	}

	return 0;
}

DEFMMETHOD(VPDF_FindWindowByID)
{
	GETDATA;

	if (msg->windowid == MUIV_VPDF_FindWindowByID_Active)
	{
		return DoMethod(obj, MUIM_VPDF_FindActiveWindow);
	}
	else
	{
		struct List *winlist = (struct List*)xget(obj, MUIA_Application_WindowList);
		Object *winobj_state = (Object*)winlist->lh_Head;
		Object *winobj;
		while((winobj = NextObject(&winobj_state)))
		{
			/* validate if it's document view window */
			int v;
			if (get(winobj, MUIA_VPDFWindow_ID, &v) == FALSE)
				continue;

			if (msg->windowid == MUIV_VPDF_FindWindowByID_First)
				return winobj;
			else if (msg->windowid == xget(winobj, MUIA_VPDFWindow_ID))
				return winobj;
		}
	}

	return 0;
}

DEFMMETHOD(VPDF_CloseWindow)
{
	Object *window = (Object*)DoMethod(obj, MUIM_VPDF_FindWindowByID, msg->windowid == 0 ? MUIV_VPDF_FindWindowByID_Active :  msg->windowid);

	if (window != NULL)
	{
		set(window, MUIA_Window_Open, FALSE);
		DoMethod(obj, OM_REMMEMBER, window);
		MUI_DisposeObject(window);

		if (DoMethod(obj, MUIM_VPDF_FindWindowByID, MUIV_VPDF_FindWindowByID_First) == NULL)
			DoMethod(obj, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		return TRUE;
	}
	return FALSE;
}

DEFMMETHOD(Export)
{
	GETDATA;
	int i;

	for(i=0; i<MAXRECENT && data->recent[i] != NULL && *data->recent[i] != '\0'; i++)
		DoMethod(msg->dataspace, MUIM_Dataspace_Add, data->recent[i], strlen(data->recent[i]) + 1, ID_VPDF_RECENT0 + i);

	return DOSUPER;
}

DEFMMETHOD(Import)
{
	GETDATA;
	int i;

	for(i=0; i<MAXRECENT; i++)
	{
		char *recent = (char*)DoMethod(msg->dataspace, MUIM_Dataspace_Find, ID_VPDF_RECENT0 + i);
		if (recent != NULL)
			data->recent[i] = strdup(recent);
	}

	setuprecent(obj, data);

	return DOSUPER;
}

DEFGET
{
	GETDATA;
	ULONG v;

	switch (msg->opg_AttrID)
	{
		case MUIA_VPDF_Renderer:
			*(ULONG*)msg->opg_Storage = (ULONG)data->renderer;
			return TRUE;
	}

	/* check for settings attribute */

	if (get(data->settings, msg->opg_AttrID, &v))
	{
		*(ULONG*)msg->opg_Storage = v;
		return TRUE;
	}

	/* */

	return DOSUPER;
}

DEFSET
{
	GETDATA;

	FORTAG(INITTAGS)
	{
		case MUIA_VPDF_Locked:
		{
			struct List *winlist = (struct List*)xget(obj, MUIA_Application_WindowList);
			Object *winobj_state = (Object*)winlist->lh_Head;
			Object *winobj;
			while((winobj = NextObject(&winobj_state)))
			{
				/* validate if it's document view window */
				int v;
				if (get(winobj, MUIA_VPDFWindow_ID, &v) == FALSE)
					continue;

				set(winobj, MUIA_Window_Sleep, tag->ti_Data);
			}
		}
		break;
	}
	NEXTTAG

	return DOSUPER;
}

DEFMMETHOD(VPDF_LogMessage)
{
	GETDATA;

	if (data->logger == NULL)
	{
		data->loggerwin = WindowObject,
						MUIA_Window_Title,"VPDF · Log",
						MUIA_Window_Width, MUIV_Window_Width_Screen(30),
						MUIA_Window_Height, MUIV_Window_Height_Screen(10),
						MUIA_Window_RootObject,
						data->logger = LoggerObject, End,
						End;

		if (data->loggerwin != NULL)
		{
			DoMethod(obj, OM_ADDMEMBER, data->loggerwin);
			DoMethod(data->loggerwin, MUIM_Notify, MUIA_Window_CloseRequest, MUIV_EveryTime, data->loggerwin, 3, MUIM_Set, MUIA_Window_Open, FALSE);
		}
	}

	if (data->logger != NULL && msg->message != NULL)
	{
		set(data->loggerwin, MUIA_Window_Open, TRUE);
		DoMethod(data->logger, MUIM_Logger_LogMessage, msg->severity, msg->message);

		/* message is always strdupped() so dispose it here. logger has to make a copy! */
		free(msg->message);
	}

	return 0;
}

DEFMMETHOD(VPDF_NewActiveWindow)
{
	/* only setup window id for active window so it will be used when the application is restarted*/

	struct List *winlist = (struct List*)xget(obj, MUIA_Application_WindowList);
	Object *winobj_state = (Object*)winlist->lh_Head;
	Object *winobj;
	while((winobj = NextObject(&winobj_state)))
	{
		/* validate if it's document view window */
		int v;
		if (get(winobj, MUIA_VPDFWindow_ID, &v) == FALSE)
			continue;

		set(winobj, MUIA_Window_ID, winobj == msg->window ? MAKE_ID('V','P', 'W','O') : 0);
	}

	return 0;
}

DEFMMETHOD(VPDF_SettingsCancel)
{
	GETDATA;
	DoMethod(data->settings, MUIM_VPDFSettings_Cancel);
	set(_win(data->settings), MUIA_Window_Open, FALSE);
	return 0;
}


DEFMMETHOD(VPDF_SettingsUse)
{
	GETDATA;
	DoMethod(obj, MUIM_VPDF_SettingsApply, DoMethod(data->settings, MUIM_VPDFSettings_Use));
	return 0;
}


DEFMMETHOD(VPDF_SettingsSave)
{
	GETDATA;
	DoMethod(obj, MUIM_VPDF_SettingsApply, DoMethod(data->settings, MUIM_VPDFSettings_Save));
	return 0;
}

DEFMMETHOD(VPDF_SettingsApply)
{
	GETDATA;
	set(_win(data->settings), MUIA_Window_Open, FALSE);
	return 0;
}

DEFMMETHOD(VPDF_About)
{
	GETDATA;

	if (data->aboutbox == NULL)
	{
		data->aboutbox = AboutboxObject,
			MUIA_Aboutbox_Credits, __credits,
		End;

		DoMethod(obj, OM_ADDMEMBER, data->aboutbox);
	}

	if (data->aboutbox != NULL)
		set(data->aboutbox, MUIA_Window_Open, TRUE);

	return 0;
}

DEFMMETHOD(VPDF_PrintDocument)
{
	GETDATA;
	Object *activewindow = (Object*)DoMethod(obj, MUIM_VPDF_FindActiveWindow);

	if (activewindow != NULL)
	{
		void *doc = (void*)xget(activewindow, MUIA_VPDFWindow_PDFDocument);
		if (doc != NULL)
			DoMethod(data->printer, MUIM_VPDFPrinter_PrintDocument, doc);
	}
	return TRUE;
}

DEFMMETHOD(DragDrop)
{
	GETDATA;
	int v;
	
	/* check if it was title object */
	
	if (_parent(msg->obj) != NULL && get(_parent(msg->obj), MUIA_VPDFTitle_IsTitle, &v))
	{
		struct List *winlist = (struct List*)xget(obj, MUIA_Application_WindowList);
		int ownwindow = FALSE;

		if (xget(_parent(msg->obj), MUIA_Group_ChildCount) <= 1)
			return DOSUPER;

		if (winlist != NULL)
		{
			Object *winobj_state = (Object*)winlist->lh_Head;
			Object *winobj;
			while((winobj = NextObject(&winobj_state)))
			{
				/* validate if it's document view window */
				int v;
				if (get(winobj, MUIA_VPDFWindow_ID, &v))
				{
					if (msg->x > xget(winobj, MUIA_Window_LeftEdge) && msg->x < xget(winobj, MUIA_Window_LeftEdge) + xget(winobj, MUIA_Window_Width)
						&& msg->y > xget(winobj, MUIA_Window_TopEdge) && msg->y < xget(winobj, MUIA_Window_TopEdge) + xget(winobj, MUIA_Window_Height))
					{
						ownwindow = TRUE;
					}
					break;
				}

			}
		}
		
		/* detach document from window (create new and add the document) */
		
		if (ownwindow == FALSE)
		{
			Object *oldwindow = _win(msg->obj);
			Object *newwindow = (Object*)DoMethod(obj, MUIM_VPDF_CreateWindow);
						
			if (newwindow != NULL)
			{
				struct MUIP_VPDFWindow_DetachView msg;
				msg.MethodID = MUIM_VPDFWindow_DetachView;
				msg.tabind = MUIV_VPDFWindow_DetachView_Active;
				
				if (DoMethodA(oldwindow, &msg))
				{
					
					DoMethod(newwindow, MUIM_VPDFWindow_AttachView, MUIV_VPDFWindow_AttachView_CurrentTabIfEmpty, msg.docview, msg.title);
				}
			}
		}		
	}
	
	return DOSUPER;
}

/* rexx handling */

static LONG Rexx(void)
{
	struct Hook *h = (struct Hook*)(APTR)REG_A0;
	Object *obj = (Object*)REG_A2;
	IPTR *params = (IPTR*)REG_A1;

	switch ((ULONG)h->h_Data)
	{
		case REXX_ABOUT:
			break;

		case REXX_OPEN:
		{
			char *file = (char*)*params;
			ULONG newtab = params[1];

			char *ftemp = file;
			while(*ftemp != '\0')
			{
				if(*ftemp == '\\')
					*ftemp = ' ';
				ftemp++;
			}

			DoMethod(obj, MUIM_VPDF_OpenFile, 0, file, newtab ? MUIV_VPDFWindow_OpenFile_CurrentTab : MUIV_VPDFWindow_OpenFile_CurrentTabIfEmpty);
			break;
		}

		case REXX_SAVE:
			break;

		case REXX_APPTOFRONT:
			break;
	}

	return 0;
}

/* */

BEGINMTABLE
	DECNEW
	DECDISP
	DECGET
	DECSET
	DECMMETHOD(Import)
	DECMMETHOD(Export)
	DECMMETHOD(DragDrop)
	DECMMETHOD(VPDF_OpenFile)
	DECMMETHOD(VPDF_CreateTab)
	DECMMETHOD(VPDF_CreateWindow)
	DECMMETHOD(VPDF_HandleAppMessage)
	DECMMETHOD(VPDF_RequestFile)
	DECMMETHOD(VPDF_OpenRecentFile)
	DECMMETHOD(VPDF_FindActiveWindow)
	DECMMETHOD(VPDF_FindWindowByID)
	DECMMETHOD(VPDF_CloseWindow)
	DECMMETHOD(VPDF_LogMessage)
	DECMMETHOD(VPDF_NewActiveWindow)
	DECMMETHOD(VPDF_SettingsCancel)
	DECMMETHOD(VPDF_SettingsSave)
	DECMMETHOD(VPDF_SettingsUse)
	DECMMETHOD(VPDF_SettingsApply)
	DECMMETHOD(VPDF_About)
	DECMMETHOD(VPDF_PrintDocument)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Application, VPDFClass)


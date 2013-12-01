// NewDrawer.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/alerts.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/classusr.h>
#include <libraries/gadtools.h>
#include <classes/window.h>
#include <gadgets/string.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/undo.h>
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/gadtools.h>
#include <proto/iconobject.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/resource.h>
#include <proto/scalos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include "NewDrawerGUI.h"

#define	CATCOMP_NUMBERS
#define	CATCOMP_STRINGS
#include "NewDrawerStrings.h"


#define	d1(x)	;
#define	d2(x)	x;

#define CATSTR(id)        GetCatalogStr(gb_Catalog, id, (STRPTR) id ## _STR)
#define CATSTRSTR(id,str) GetCatalogStr(gb_Catalog, id, (STRPTR) str)


extern int kprintf(CONST_STRPTR, ...);

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static ULONG MessageLoop(void);
static void UpdateGadgets(struct WBStartup *WBenchMsg);
static LONG MakeNewDrawer(void);
static void ReportError(LONG Result, CONST_STRPTR NewObjName);
static struct ScaWindowStruct *FindScalosWindow(BPTR dirLock);


#define PROGNAME "NewDrawer.module"
#define VERSIONR "1.1"
#define PROGDATE "29 Jun 2005 18:18:23"
#define COPYRIGT "© The Scalos Team, 2004" CURRENTYEAR

const char *version = "\0$VER: " PROGNAME " " VERSIONR " (" PROGDATE ") - " COPYRIGT "\n";


struct Library       *ResourceBase;
struct IntuitionBase *IntuitionBase;
struct Library       *GadToolsBase;
T_LOCALEBASE LocaleBase;
struct Library       *IconBase;
struct Library 	     *IconobjectBase = NULL;
struct ScalosBase    *ScalosBase = NULL;
T_UTILITYBASE UtilityBase;

static struct Screen        *gb_Screen;
static struct Window        *gb_Window;
static struct Gadget       **gb_Gadgets;
static struct Catalog       *gb_Catalog;
static struct MsgPort       *gb_IDCMPort;
static struct MsgPort       *gb_AppPort;
static Object               *gb_WindowObj;
RESOURCEFILE          gb_Resource;

static char LabelText[1024];


main()
{
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
		if (!OpenLibraries())
			break;

		gb_Catalog = OpenCatalogA(NULL, PROGNAME ".catalog",NULL);

		gb_Screen = LockPubScreen(NULL);
		if (NULL == gb_Screen)
			break;

		gb_IDCMPort = CreateMsgPort();
		if (NULL == gb_IDCMPort)
			break;

		gb_AppPort = CreateMsgPort();
		if (NULL == gb_AppPort)
			break;

		gb_Resource = RL_OpenResource(RCTResource, gb_Screen, gb_Catalog);

		gb_WindowObj = RL_NewObject(gb_Resource, WIN_NewDrawer_ID,
			WINDOW_SharedPort, gb_IDCMPort,
			WINDOW_AppPort,	gb_AppPort,
			TAG_DONE);
		if (NULL == gb_WindowObj)
			break;

		gb_Gadgets = (struct Gadget **) RL_GetObjectArray(gb_Resource, gb_WindowObj, GROUP_NewDrawer_ID);
		if (NULL == gb_Gadgets)
			break;

		UpdateGadgets(WBenchMsg);

		gb_Window = RA_OpenWindow(gb_WindowObj);
		if (NULL == gb_Window)
			break;

		MessageLoop();
		} while (0);

	if (gb_WindowObj)
		DoMethod(gb_WindowObj, WM_CLOSE);
	if (gb_Resource)
		RL_CloseResource(gb_Resource);
	if(gb_Catalog)
		CloseCatalog(gb_Catalog);
	if (gb_AppPort)
		DeleteMsgPort(gb_AppPort);
	if (gb_IDCMPort)
		DeleteMsgPort(gb_IDCMPort);
	if (gb_Screen)
		UnlockPubScreen(NULL, gb_Screen);
		
	CloseLibraries();
}


static BOOL OpenLibraries(void)
{
	ResourceBase = OpenLibrary("resource.library", 44);
	if (NULL == ResourceBase)
		return FALSE;

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;

	GadToolsBase = OpenLibrary("gadtools.library", 38);
	if (NULL == GadToolsBase)
		return FALSE;

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 38);
	if (NULL == LocaleBase)
		return FALSE;

	IconBase = OpenLibrary("icon.library", 39);
	if (NULL == IconBase)
		return FALSE;

	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;

	IconobjectBase = OpenLibrary(ICONOBJECTNAME, 39);
	if (NULL == IconobjectBase)
		return FALSE;

	ScalosBase = (struct ScalosBase *) OpenLibrary(SCALOSNAME, 40);
	if (NULL == ScalosBase)
		return FALSE;

	return TRUE;
}


static void CloseLibraries(void)
{
	if (ScalosBase)
		{
		CloseLibrary((struct Library *) ScalosBase);
		ScalosBase = NULL;
		}
	if (IconobjectBase)
		{
		CloseLibrary(IconobjectBase);
		IconobjectBase = NULL;
		}
	if (UtilityBase)
		{
		CloseLibrary(UtilityBase);
		UtilityBase = NULL;
		}
	if (IconBase)
		{
		CloseLibrary(IconBase);
		IconBase = NULL;
		}
	if (LocaleBase)
		{
		CloseLibrary(LocaleBase);
		LocaleBase = NULL;
		}
	if (GadToolsBase)
		{
		CloseLibrary(GadToolsBase);
		GadToolsBase = NULL;
		}
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase = NULL;
		}
	if (ResourceBase)
		{
		CloseLibrary(ResourceBase);
		ResourceBase = NULL;
		}
}


static ULONG MessageLoop(void)
{
	ULONG res = RETURN_FAIL;
	ULONG windowsignal, signals;

	GetAttr(WINDOW_SigMask, gb_WindowObj, &windowsignal);

	while (res == RETURN_FAIL)
		{
		ActivateGadget(gb_Gadgets[String_NewDrawer], gb_Window, NULL);

		signals = Wait(windowsignal);

		if (signals & windowsignal) 
			{
			ULONG result;
			WORD code;

			while ((result = RA_HandleInput(gb_WindowObj, &code)) != WMHI_LASTMSG)	
				{
				d1(kprintf("result=%08lx  code=%04lx\n", result, code));

				switch (result & WMHI_CLASSMASK)
					{
				case WMHI_CLOSEWINDOW:
					res = RETURN_OK;
					break;

				case WMHI_GADGETUP:
					{
//					const UWORD groupid = RL_GROUPID(result);
					const UWORD gadid = RL_GADGETID(result);

					switch (gadid)
						{
					case String_NewDrawer:
						d1(kprintf("WMHI_GADGETUP code=%04lx\n", code));
						if (0 == code)
							MakeNewDrawer();
						res = RETURN_OK;
						break;

					case OkButton:
						MakeNewDrawer();
						res = RETURN_OK;
						break;

					case Cancelbutton:
						res = RETURN_OK;
						break;

					default:
						break;
						}
					}
					break;
					}
				}
			}
		}

	return( res );
}


static void UpdateGadgets(struct WBStartup *WBenchMsg)
{
	char xName[512];
	STRPTR NewObjName = "";
	ULONG Mark;

	NameFromLock(WBenchMsg->sm_ArgList[1].wa_Lock, xName, sizeof(xName));

	sprintf(LabelText, CATSTR(MSGID_LABEL_ENTERNAME), xName);

	SetAttrs(gb_Gadgets[TextLine_EnterName],
		GA_Text, LabelText,
		TAG_END);

	GetAttr(STRINGA_TextVal, gb_Gadgets[String_NewDrawer], (ULONG *) &NewObjName);

	Mark = (0 << 16) + strlen(NewObjName);
	SetAttrs(gb_Gadgets[String_NewDrawer], 
		STRINGA_Mark, Mark,
		TAG_END);
}


static LONG MakeNewDrawer(void)
{
	BPTR oldDir = CurrentDir(WBenchMsg->sm_ArgList[1].wa_Lock);
	LONG Result = RETURN_OK;
	STRPTR NewObjName = NULL;
	struct ScaWindowStruct *ws;
	APTR UndoStep = NULL;
	BPTR newDirLock;
	ULONG CreateIcon = FALSE;

	ws = FindScalosWindow(WBenchMsg->sm_ArgList[1].wa_Lock);

	GetAttr(STRINGA_TextVal, gb_Gadgets[String_NewDrawer], (ULONG *) &NewObjName);
	GetAttr(GA_Selected, gb_Gadgets[CheckBox_CreateIcon], &CreateIcon);

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
				UNDOTAG_IconDirLock, WBenchMsg->sm_ArgList[1].wa_Lock,
				UNDOTAG_IconName, NewObjName,
				UNDOTAG_CreateIcon, CreateIcon,
				TAG_END
				);
			}
		newDirLock = CreateDir(NewObjName);
		if (newDirLock)
			{
			if (CreateIcon)
				{
				Object *IconObj;

				IconObj	= GetDefIconObject(WBDRAWER, NULL);
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

	if (ws)
		{
		DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_EndUndoStep, UndoStep);
		SCA_UnLockWindowList();
		}

	CurrentDir(oldDir);

	if (Result)
		ReportError(Result, NewObjName);

	return Result;
}


static void ReportError(LONG Result, CONST_STRPTR NewObjName)
{
	char FaultText[256];
	struct EasyStruct ezStruct;

	Fault(Result, "", FaultText, sizeof(FaultText));

	ezStruct.es_StructSize = sizeof(struct EasyStruct);
	ezStruct.es_Flags = 0;
	ezStruct.es_Title = CATSTR(MSGID_TITLE_ERRORREQ);
	ezStruct.es_TextFormat = CATSTR(MSGID_ERRORREQ_BODYTEXT);
	ezStruct.es_GadgetFormat = CATSTR(MSGID_REQ_OK);

	EasyRequest(gb_Window, &ezStruct, NULL,
		NewObjName, FaultText);
}

//----------------------------------------------------------------------------

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
			if (((BPTR)NULL == dirLock && (BPTR)NULL == ws->ws_Lock) || (LOCK_SAME == SameLock(dirLock, ws->ws_Lock)))
				{
				return ws;
				}
			}
		SCA_UnLockWindowList();
		}

	return NULL;
}

//----------------------------------------------------------------------------

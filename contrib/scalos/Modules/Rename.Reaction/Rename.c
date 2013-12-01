// Rename.c
// 09 Jan 2002 16:56:12

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
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/locale_protos.h>
#include <clib/utility_protos.h>
#include <clib/resource_protos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RenameGUI.h"

#define	CATCOMP_NUMBERS
#define	CATCOMP_STRINGS
#include "RenameStrings.h"


#define	d1(x)	;
#define	d2(x)	x;

#define CATSTR(id)        GetCatalogStr(gb_Catalog, id, (STRPTR) id ## _STR)
#define CATSTRSTR(id,str) GetCatalogStr(gb_Catalog, id, (STRPTR) str)


extern int kprintf(CONST_STRPTR, ...);

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static ULONG MessageLoop(struct WBArg *ArgList, LONG *n);
static void UpdateGadgets(struct WBArg *arg);
static void ReportError(LONG Result, CONST_STRPTR OldObjName, CONST_STRPTR NewObjName);
static LONG RenameObject(struct WBArg *arg);
ULONG RefreshSetGadgetAttrsA(struct Gadget *g, struct Window *w, struct Requester *r, struct TagItem *tags);
ULONG RefreshSetGadgetAttrs(struct Gadget *g, struct Window *w, struct Requester *r, Tag tag1, ...);
void StripTrailingColon(STRPTR Line);


#define PROGNAME "Rename.module"
#define VERSIONR "1.3"
#define PROGDATE "09 Jan 2002 16:56:28"
#define COPYRIGT "Copyright © Jürgen Lachmann"

CONST_STRPTR version = "\0$VER: " PROGNAME " " VERSIONR " (" PROGDATE ") - " COPYRIGT "\n";


struct Library       *ResourceBase;
struct IntuitionBase *IntuitionBase;
struct Library       *GadToolsBase;
struct Library       *LocaleBase;
struct Library       *UtilityBase;

static struct Screen        *gb_Screen;
static struct Window        *gb_Window;
static struct Gadget       **gb_Gadgets;
static struct Catalog       *gb_Catalog;
static struct MsgPort       *gb_IDCMPort;
static struct MsgPort       *gb_AppPort;
static Object               *gb_WindowObj;
RESOURCEFILE          gb_Resource;


static BOOL RenameVolume = FALSE;


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
		LONG Result;
		LONG n;

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

		gb_WindowObj = RL_NewObject(gb_Resource, WIN_Rename_ID,
			WINDOW_SharedPort, gb_IDCMPort,
			WINDOW_AppPort,	gb_AppPort,
			TAG_DONE);
		if (NULL == gb_WindowObj)
			break;

		gb_Gadgets = (struct Gadget **) RL_GetObjectArray(gb_Resource, gb_WindowObj, GROUP_Rename_ID);
		if (NULL == gb_Gadgets)
			break;

		UpdateGadgets(&WBenchMsg->sm_ArgList[1]);

		gb_Window = RA_OpenWindow(gb_WindowObj);
		if (NULL == gb_Window)
			break;

		n = 1;
		do	{
			Result = MessageLoop(WBenchMsg->sm_ArgList, &n);
			} while (n < WBenchMsg->sm_NumArgs && RETURN_OK == Result);
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

	LocaleBase = OpenLibrary("locale.library", 38);
	if (NULL == LocaleBase)
		return FALSE;

	UtilityBase = OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;

	return TRUE;
}


static void CloseLibraries(void)
{
	if (UtilityBase)
		{
		CloseLibrary(UtilityBase);
		UtilityBase = NULL;
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


static ULONG MessageLoop(struct WBArg *ArgList, LONG *n)
{
	ULONG res = RETURN_FAIL;
	ULONG windowsignal, signals;

	UpdateGadgets(&ArgList[*n]);

	GetAttr(WINDOW_SigMask, gb_WindowObj, &windowsignal);

	while (res == RETURN_FAIL)
		{
		ActivateGadget(gb_Gadgets[String_NewName], gb_Window, NULL);

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
					case String_NewName:
						d1(kprintf("WMHI_GADGETUP code=%04lx\n", code));
						if (0 == code)
							RenameObject(&ArgList[*n]);
						res = RETURN_OK;
						(*n)++;
						break;

					case OkButton:
						RenameObject(&ArgList[*n]);
						res = RETURN_OK;
						(*n)++;
						break;

					case SkipButton:
						(*n)++;
						res = RETURN_OK;
						break;

					case Cancelbutton:
						res = RETURN_ERROR;
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


static void UpdateGadgets(struct WBArg *arg)
{
	static char LabelText[1024];
	static char PathText[1024];
	STRPTR FileName;
	ULONG Mark;
	char xName[512];

	NameFromLock(arg->wa_Lock, xName, sizeof(xName));

	if (arg->wa_Name && strlen(arg->wa_Name) > 0)
		FileName = arg->wa_Name;
	else
		FileName = FilePart(xName);

	d1(kprintf("xName=<%s>  wa_Name=%08lx <%s>\n", xName, arg->wa_Name, arg->wa_Name ? arg->wa_Name : ""));

	if (':' == xName[strlen(xName)-1])
		{
		sprintf(PathText, CATSTR(MSGID_TEXTLINE_PATH), xName);

		StripTrailingColon(xName);

		FileName = xName;

		RenameVolume = TRUE;
		}
	else
		{
		STRPTR lp = PathPart(xName);
		if (lp)
			*lp = '\0';

		sprintf(PathText, CATSTR(MSGID_TEXTLINE_PATH), xName);
		}


	sprintf(LabelText, CATSTR(MSGID_TEXTLINE_ENTERNEWNAME), FileName);

	Mark = (0 << 16) + strlen(FileName);

	RefreshSetGadgetAttrs(gb_Gadgets[String_NewName], gb_Window, NULL,
		STRINGA_TextVal, FileName,
		TAG_END);

	RefreshSetGadgetAttrs(gb_Gadgets[String_NewName], gb_Window, NULL,
		STRINGA_Mark, Mark,
		TAG_END);

	RefreshSetGadgetAttrs(gb_Gadgets[TextLine_EnterName], gb_Window, NULL,
		GA_Text, LabelText,
		TAG_END);
	RefreshSetGadgetAttrs(gb_Gadgets[TextLine_Path], gb_Window, NULL,
		GA_Text, PathText,
		TAG_END);
}


static void ReportError(LONG Result, CONST_STRPTR OldObjName, CONST_STRPTR NewObjName)
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
		OldObjName, NewObjName, FaultText);
}


static LONG RenameObject(struct WBArg *arg)
{
	BPTR dirLock;
	LONG Result = RETURN_OK;
	STRPTR NewObjName = NULL;
	STRPTR OldObjName;
	char ObjName[512];
	BPTR oldDir;

	NameFromLock(arg->wa_Lock, ObjName, sizeof(ObjName));

	GetAttr(STRINGA_TextVal, gb_Gadgets[String_NewName], (ULONG *) &NewObjName);

	if (arg->wa_Name && strlen(arg->wa_Name) > 0)
		{
		OldObjName = arg->wa_Name;
		dirLock = DupLock(arg->wa_Lock);
		}
	else
		{
		OldObjName = FilePart(ObjName);
		dirLock = ParentDir(arg->wa_Lock);
		}

	oldDir = CurrentDir(dirLock);

	if (dirLock && NewObjName && strlen(NewObjName) > 0)
		{
		if (RenameVolume)
			{
			StripTrailingColon(OldObjName);
			StripTrailingColon(NewObjName);

			if (!Relabel(OldObjName, NewObjName))
				Result = IoErr();
			}
		else
			{
			if (!Rename(OldObjName, NewObjName))
				Result = IoErr();
			}

		if (RETURN_OK != Result)
			ReportError(Result, OldObjName, NewObjName);
		else
			{
			// try to rename associated icon
			char OldIconName[512];
			char NewIconName[512];

			stccpy(OldIconName, OldObjName, sizeof(OldIconName) - 5);
			strcat(OldIconName, ".info");

			stccpy(NewIconName, NewObjName, sizeof(NewIconName) - 5);
			strcat(NewIconName, ".info");

			if (!Rename(OldIconName, NewIconName))
				Result = IoErr();

			if (ERROR_OBJECT_NOT_FOUND == Result)
				Result = RETURN_OK;

			if (RETURN_OK != Result)
				ReportError(Result, OldIconName, NewIconName);
			}
		}

	CurrentDir(oldDir);

	UnLock(dirLock);

	return Result;
}


ULONG RefreshSetGadgetAttrsA(struct Gadget *g, struct Window *w, struct Requester *r, struct TagItem *tags)
{
	ULONG retval;
	BOOL changedisabled = FALSE;
	BOOL disabled = FALSE;

	if (w)
		{
		if (FindTagItem(GA_Disabled,tags))
			{
			changedisabled = TRUE;
 			disabled = g->Flags & GFLG_DISABLED;
 			}
	 	}
	retval = SetGadgetAttrsA(g,w,r,tags);
	if (w && (retval || (changedisabled && disabled != (g->Flags & GFLG_DISABLED))))
		{
		RefreshGList(g,w,r,1);
		retval = 1;
		}
	return retval;
}


ULONG RefreshSetGadgetAttrs(struct Gadget *g, struct Window *w, struct Requester *r, Tag tag1, ...)
{
	return RefreshSetGadgetAttrsA(g,w,r,(struct TagItem *) &tag1);
}


void StripTrailingColon(STRPTR Line)
{
	size_t Len = strlen(Line);

	Line += Len - 1;

	if (':' == *Line)
		*Line = '\0';
}



// Persist.c
// $Date$
// $Revision$


#include <devices/timer.h>
#include <intuition/classes.h>
#include <libraries/dos.h>
#include <workbench/icon.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>

#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/dos.h>
#include <proto/scalos.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/input.h>

#include <clib/alib_protos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include <defs.h>

#include "Persist.h"
#include "plugin.h"

#ifdef __AROS__
#include "plugin-common.c"
#endif

// moved revision history to file "History"

//----------------------------------------------------------------------------

// aus mempools.lib
#ifndef __amigaos4__
extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);
#endif /* __amigaos4__ */

static void GetScalosPath(Object *o, char *Path, size_t MaxLen);
static struct OpenNode *AddWindowPath(const struct ScaWindowStruct *ws, CONST_STRPTR Path);
static void RemoveWindowPath(CONST_STRPTR Path);
static void SetIconifyState_WindowTask(const struct ScaWindowTask *wt, BOOL Iconified);
static void SetWindowDimensions_WindowTask(const struct ScaWindowTask *wt,
	WORD Left, WORD Top, WORD Width, WORD Height);
// static void striplf(char *Line);
static void ScalosWindowOpen(Class *cl, Object *obj, Msg msg);
static void ScalosWindowClose(Class *cl, Object *obj, Msg msg);
static void ScalosWindowIconify(Class *cl, Object *obj, Msg msg);
static void ScalosWindowUnIconify(Class *cl, Object *obj, Msg msg);
static void ScalosWindowNewPath(Class *cl, Object *obj, Msg msg);
static void ScalosWindowSetSize(Class *cl, Object *obj, Msg msg);
static void DeleteNamedNode(CONST_STRPTR Path);
static void FreeNode(struct OpenNode *OldNode);
static void RewriteNodeListFile(void);
static BOOL StartReOpenProcess(void);
static SAVEDS(int) ReOpenProcess(void);
static void ReadNodes(void);
static BOOL OpenNodes(void);
static void CleanupNodes(void);
static void striplf(char *Line);
static void ReadConfig(CONST_STRPTR Filename);
static BOOL ExistsDevice(CONST_STRPTR Path);
static struct OpenNode *FindNamedNode(CONST_STRPTR Path);
static struct ScaWindowStruct *FindWindowByLock(BPTR xLock);
static BOOL StartUpdaterProcess(void);
static void KillUpdaterProcess(void);
static void TriggerUpdater(void);
static BOOL InitProcessTimer(struct ProcessTimerInfo *Timer);
static void CleanupProcessTimer(struct ProcessTimerInfo *Timer);
static SAVEDS(int) UpdaterProcess(void);

#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__AROS__)
static char *stpblk(CONST_STRPTR q);
#endif /* !defined(__SASC) &&!defined(__MORPHOS__) */

//----------------------------------------------------------------------------

struct DosLibrary *DOSBase;
struct ScalosBase *ScalosBase;
T_UTILITYBASE UtilityBase;
struct Library *IconBase;
struct Library *IconobjectBase;
struct IntuitionBase *IntuitionBase;
T_INPUTBASE InputBase;
#ifdef __amigaos4__
struct Library *NewlibBase;

struct DOSIFace *IDOS;
struct ScalosIFace *IScalos;
struct UtilityIFace *IUtility;
struct IconIFace *IIcon;
struct IconobjectIFace *IIconobject;
struct IntuitionIFace *IIntuition;
struct InputIFace *IInput;
struct Interface *INewlib;
#endif /* __amigaos4__ */

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
extern T_UTILITYBASE __UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && ! defined(__amigaos4__) */

STRPTR DefIconsPath = "ENV:sys/def_";

static BOOL fInit;

static BOOL ignoreWindowClose = FALSE;
static BOOL wbStartupFinished = FALSE;

static struct SignalSemaphore PersistSema;
static struct List OpenList;

static struct List ReOpenList;

static long OpenDelayTicks = 20l;	// number of Delay() ticks to wait before opening each window
static BOOL UseScaIconify = FALSE;	// Flag : use SCA_Iconify with OpenDrawerByNameTags()
static long UseWindowSizes = TRUE;	// Flag: size and position is retained for each window

static char Persist_FileName[256] = PERSIST_FILENAME;

static ULONG UpdaterProcSignal = 0L;
struct Process *UpdaterProc = NULL;

//----------------------------------------------------------------------------

BOOL initPlugin(struct PluginBase *pluginbase)
{
#ifdef __AROS__
	pluginbase->pl_PlugID = MAKE_ID('P','L','U','G');
#endif

	BOOL Success = FALSE;

	d(kprintf(__FUNC__ "/%ld \n", __LINE__);)

	if (fInit)
		return TRUE;

	do	{
		d(kprintf(__FUNC__ "/%ld \n", __LINE__);)

		NewList(&OpenList);

		InitSemaphore(&PersistSema);

		DOSBase = (struct DosLibrary *) OpenLibrary(DOSNAME, 39);
		if (NULL == DOSBase)
			break;
#ifdef __amigaos4__
		IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
		if (NULL == IDOS)
			break;
#endif /* __amigaos4__ */

		UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
		if (NULL == UtilityBase)
			break;
#ifdef __amigaos4__
		IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
		if (NULL == IUtility)
			break;
#endif /* __amigaos4__ */

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
		__UtilityBase = UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) */

		ScalosBase = (struct ScalosBase *) OpenLibrary(SCALOSNAME, 41);
		if (NULL == ScalosBase)
			break;
#ifdef __amigaos4__
		IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
		if (NULL == IScalos)
			break;
#endif /* __amigaos4__ */

		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
		if (NULL == IntuitionBase)
			break;
#ifdef __amigaos4__
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			break;

		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			break;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			break;
#endif /* __amigaos4__ */


#if !defined(__amigaos4__) && !defined(__AROS__)
		if (_STI_240_InitMemFunctions())
			break;
#endif /* __amigaos4__ */

		fInit = TRUE;

		if (ScalosBase->scb_LibNode.lib_Version >= 40)
			UseScaIconify = TRUE;

		ReadConfig(PERSIST_CONFIGFILENAME);

		if (!StartReOpenProcess())
			break;

		if (!StartUpdaterProcess())
			break;

		Success = TRUE;
		} while (0);

	if (!fInit)
		closePlugin(pluginbase);

	return Success;
}

//----------------------------------------------------------------------------

VOID closePlugin(struct PluginBase *pluginbase)
{
	(void) pluginbase;

	KillUpdaterProcess();

	ObtainSemaphore(&PersistSema);

	fInit = FALSE;

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif /* __amigaos4__ */


#ifdef __amigaos4__
	if (IScalos)
		{
		DropInterface((struct Interface *)IScalos);
		IScalos = NULL;
		}
#endif /* __amigaos4__ */
	if (ScalosBase)
		{
		CloseLibrary((struct Library *) ScalosBase);
		ScalosBase = NULL;
		}
#ifdef __amigaos4__
	if (INewlib)
		{
		DropInterface(INewlib);
		INewlib = NULL;
		}
	if (NewlibBase)
		{
		CloseLibrary(NewlibBase);
		NewlibBase = NULL;
		}
	if (IUtility)
		{
		DropInterface((struct Interface *)IUtility);
		IUtility = NULL;
		}
#endif /* __amigaos4__ */
	if (UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
#ifdef __amigaos4__
	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif /* __amigaos4__ */
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
#ifdef __amigaos4__
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif /* __amigaos4__ */
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
	ReleaseSemaphore(&PersistSema);
}

//----------------------------------------------------------------------------

M68KFUNC_P3(ULONG, persistHookFunc,
	A0, Class *, cl,
	A2, Object *, obj,
	A1, Msg, msg)
{
	switch (msg->MethodID)
		{
	case SCCM_Window_Open:
		d(kprintf(__FUNC__ "/%ld SCCM_Window_Open\n", __LINE__);)
		ScalosWindowOpen(cl, obj, msg);
		break;

	case SCCM_Window_Close:
		d(kprintf(__FUNC__ "/%ld SCCM_Window_Close\n", __LINE__);)
		ScalosWindowClose(cl, obj, msg);
		break;

	case SCCM_Window_Iconify:
		d(kprintf(__FUNC__ "/%ld SCCM_Window_Iconify\n", __LINE__);)
		ScalosWindowIconify(cl, obj, msg);
		break;

	case SCCM_Window_UnIconify:
		d(kprintf(__FUNC__ "/%ld SCCM_Window_UnIconify\n", __LINE__);)
		ScalosWindowUnIconify(cl, obj, msg);
		break;

	case SCCM_Window_NewPath:
		ScalosWindowNewPath(cl, obj, msg);
		break;

	case SCCM_Window_SetInnerSize:
		d(KPrintF(__FUNC__ "/%ld SCCM_Window_SetInnerSize\n", __LINE__);)
		ScalosWindowSetSize(cl, obj, msg);
		break;

	case SCCM_Window_WBStartupFinished:
		d1(KPrintF("%s/%ld SCCM_Window_WBStartupFinished\n", __FUNC__, __LINE__);)
		wbStartupFinished = TRUE;
		break;
		}

	d(kprintf(__FUNC__ "/%ld: MethodID=%08lx\n", __LINE__, msg->MethodID));

	return DoSuperMethodA(cl, obj, msg);
}
M68KFUNC_END

//----------------------------------------------------------------------------

static void GetScalosPath(Object *o, char *Path, size_t MaxLen)
{
	struct ScaRootList *rList = (struct ScaRootList *) o;
	struct ScaWindowStruct *ws = rList->rl_WindowTask->mt_WindowStruct;

	*Path = '\0';

	d(kprintf(__FUNC__ "/%ld: ScaWindowStruct=%08lx  Task=%08lx  Lock=%08lx  Window=%08lx\n", \
		__LINE__, ws, ws->ws_Task, ws->ws_Lock, ws->ws_Window));

	if (ws && ws->ws_Lock)
		{
		NameFromLock(ws->ws_Lock, Path, MaxLen);
		}
}

//----------------------------------------------------------------------------

static struct OpenNode *AddWindowPath(const struct ScaWindowStruct *ws, CONST_STRPTR Path)
{
	struct OpenNode *NewNode;

	d(kprintf(__FUNC__ "/%ld: Path = <%s>\n", __LINE__, Path));

	NewNode = FindNamedNode(Path);
	if (NewNode)
		{
		NewNode->on_Iconified = FALSE;
		}
	else
		{
		NewNode = malloc(sizeof(struct OpenNode));
		if (NewNode)
			{
			ObtainSemaphore(&PersistSema);

			NewNode->on_BrowserMode = FALSE;
			NewNode->on_Iconified = FALSE;
			NewNode->on_Path = strdup(Path);

			if (ws)
				{
				NewNode->on_WindowTask = ws->ws_WindowTask;

				NewNode->on_Left = ws->ws_Left;
				NewNode->on_Top = ws->ws_Top;
				NewNode->on_Width = ws->ws_Width;
				NewNode->on_Height = ws->ws_Height;
				}
			else
				{
				NewNode->on_WindowTask = NULL;

				NewNode->on_Left = 0;
				NewNode->on_Top = 0;
				NewNode->on_Width = 0;
				NewNode->on_Height = 0;
				}

			DeleteNamedNode(Path);
			AddTail(&OpenList, &NewNode->on_Node);

			ReleaseSemaphore(&PersistSema);
			}
		}

	if (ws && (ws->ws_Flags & WSV_FlagF_BrowserMode))
		NewNode->on_BrowserMode = TRUE;

	TriggerUpdater();

	return NewNode;
}

//----------------------------------------------------------------------------

static void RemoveWindowPath(CONST_STRPTR Path)
{
	d(kprintf(__FUNC__ "/%ld: Path = <%s>\n", __LINE__, Path));

	DeleteNamedNode(Path);

	TriggerUpdater();
}

//----------------------------------------------------------------------------

static void SetIconifyState_WindowTask(const struct ScaWindowTask *wt, BOOL Iconified)
{
	char Path[512];
	struct OpenNode *OldNode;

	ObtainSemaphoreShared(&PersistSema);

	if (!NameFromLock(wt->mt_WindowStruct->ws_Lock, Path, sizeof(Path)))
		{
		*Path = '\0';
		}

	d(kprintf(__FUNC__ "/%ld: Path = <%s>\n", __LINE__, Path));

	// Iconify-Zustand beim Eintrag mit Namen <Path> einstellen
	for (OldNode = (struct OpenNode *) OpenList.lh_Head;
		OldNode != (struct OpenNode *) &OpenList.lh_Tail;
		OldNode = (struct OpenNode *) OldNode->on_Node.ln_Succ )
		{
		if (wt == OldNode->on_WindowTask)
			{
			OldNode->on_Iconified = Iconified;
			break;
			}
		if (NULL == OldNode->on_WindowTask)
			{
			if (0 == Stricmp(OldNode->on_Path, Path))
				{
				OldNode->on_WindowTask = wt;
				OldNode->on_Iconified = Iconified;
				break;
				}
			}
		}

	ReleaseSemaphore(&PersistSema);

	TriggerUpdater();
}

//----------------------------------------------------------------------------

static void SetWindowDimensions_WindowTask(const struct ScaWindowTask *wt,
	WORD Left, WORD Top, WORD Width, WORD Height)
{
	struct OpenNode *OldNode;

	ObtainSemaphoreShared(&PersistSema);

	d(kprintf(__FUNC__ "/%ld: Path = <%s>\n", __LINE__, Path));

	// Iconify-Zustand beim Eintrag mit Namen <Path> einstellen
	for (OldNode = (struct OpenNode *) OpenList.lh_Head;
		OldNode != (struct OpenNode *) &OpenList.lh_Tail;
		OldNode = (struct OpenNode *) OldNode->on_Node.ln_Succ)
		{
		if (wt == OldNode->on_WindowTask)
			{
			OldNode->on_Left   = Left;
			OldNode->on_Top    = Top;
			OldNode->on_Width  = Width;
			OldNode->on_Height = Height;
			break;
			}
		}

	ReleaseSemaphore(&PersistSema);

	TriggerUpdater();
}

//----------------------------------------------------------------------------

static void ScalosWindowOpen(Class *cl, Object *obj, Msg msg)
{
	struct ScaRootList *rList = (struct ScaRootList *) obj;
	struct ScaWindowStruct *ws = rList->rl_WindowTask->mt_WindowStruct;
	char Filename[512];

	d(kprintf(__FUNC__ "/%ld: InstOffset=%08lx  InstSize=%08lx  SubclassCount=%lu  ObjectCount=%lu\n", \
		__LINE__, cl->cl_InstOffset, cl->cl_InstSize, cl->cl_SubclassCount, cl->cl_ObjectCount));

	d(kprintf(__FUNC__ "/%ld: ws=%08lx  name=%08lx  <%s>\n", __LINE__, \
		ws, ws->ws_Name, ws->ws_Name ? ws->ws_Name : (STRPTR) ""));

	if (NULL == ws->ws_Name)
		{
		// Root window is being opened (ws_Name == NULL)
		ignoreWindowClose = FALSE;
		}

	GetScalosPath(obj, Filename, sizeof(Filename));
	if (*Filename)
		AddWindowPath(ws, Filename);

	d(kprintf(__FUNC__ "/%ld: OM_NEW Result=%08lx  obj=%08lx  WindowStruct=%08lx\n", __LINE__, Result, obj, ws));
	d(kprintf(__FUNC__ "/%ld: ExecBase=%08lx  DosBase=%08lx\n", __LINE__, SysBase, DOSBase));
}

//----------------------------------------------------------------------------

static void ScalosWindowClose(Class *cl, Object *obj, Msg msg)
{
	struct ScaRootList *rList = (struct ScaRootList *) obj;
	struct ScaWindowStruct *ws = rList->rl_WindowTask->mt_WindowStruct;
	char Filename[512];

	d(kprintf(__FUNC__ "/%ld: InstOffset=%08lx  InstSize=%08lx  SubclassCount=%lu  ObjectCount=%lu\n", \
		__LINE__, cl->cl_InstOffset, cl->cl_InstSize, cl->cl_SubclassCount, cl->cl_ObjectCount));
	d(kprintf(__FUNC__ "/%ld: OM_DISPOSE Result=%08lx  obj=%08lx  MethodID=%08lx\n", __LINE__, Result, obj, msg->MethodID));
	d(kprintf(__FUNC__ "/%ld: ExecBase=%08lx  DosBase=%08lx\n", __LINE__, inst->pi_ExecBase, inst->pi_DosBase));

	d(kprintf(__FUNC__ "/%ld: ws=%08lx  name=%08lx  <%s>\n", __LINE__, \
		ws, ws->ws_Name, ws->ws_Name ? ws->ws_Name : (STRPTR) ""));

	if (NULL == ws->ws_Name)
		{
		// Root window is being closed (ws_Name == NULL)
		// Scalos is going to quit or CloseWorkBench()
		ignoreWindowClose = TRUE;
		}

	if (!ignoreWindowClose)
		{
		GetScalosPath(obj, Filename, sizeof(Filename));
		if (*Filename)
			RemoveWindowPath(Filename);
		}
}

//----------------------------------------------------------------------------

static void ScalosWindowIconify(Class *cl, Object *o, Msg msg)
{
	struct ScaWindowTask *wt = ((struct ScaRootList *) o)->rl_WindowTask;

	d(KPrintF(__FUNC__ "/%ld: wt=%08lx  name=%08lx  <%s>\n", __LINE__, \
		wt, wt->mt_WindowStruct->ws_Name, \
                wt->mt_WindowStruct->ws_Name ? wt->mt_WindowStruct->ws_Name : (STRPTR) ""));

	SetIconifyState_WindowTask(wt, TRUE);
}

//----------------------------------------------------------------------------

static void ScalosWindowUnIconify(Class *cl, Object *o, Msg msg)
{
	struct ScaWindowTask *wt = ((struct ScaRootList *) o)->rl_WindowTask;

	d(KPrintF(__FUNC__ "/%ld: wt=%08lx  name=%08lx  <%s>\n", __LINE__, \
		wt, wt->mt_WindowStruct->ws_Name, \
                wt->mt_WindowStruct->ws_Name ? wt->mt_WindowStruct->ws_Name : (STRPTR) ""));

	SetIconifyState_WindowTask(wt, FALSE);
}

//----------------------------------------------------------------------------

static void ScalosWindowNewPath(Class *cl, Object *obj, Msg msg)
{
	const struct msg_NewPath *npa = (const struct msg_NewPath *) msg;
	struct ScaRootList *rList = (struct ScaRootList *) obj;
	struct ScaWindowStruct *ws = rList->rl_WindowTask->mt_WindowStruct;
	char Filename[512];
	BPTR OldDir;
	BPTR pLock;

	d(kprintf(__FUNC__ "/%ld: InstOffset=%08lx  InstSize=%08lx  SubclassCount=%lu  ObjectCount=%lu\n", \
		__LINE__, cl->cl_InstOffset, cl->cl_InstSize, cl->cl_SubclassCount, cl->cl_ObjectCount));
	d(kprintf(__FUNC__ "/%ld: OM_DISPOSE Result=%08lx  obj=%08lx  MethodID=%08lx\n", __LINE__, Result, obj, msg->MethodID));
	d(kprintf(__FUNC__ "/%ld: ExecBase=%08lx  DosBase=%08lx\n", __LINE__, inst->pi_ExecBase, inst->pi_DosBase));

	d(kprintf(__FUNC__ "/%ld: ws=%08lx  name=%08lx  <%s>\n", __LINE__, \
		ws, ws->ws_Name, ws->ws_Name ? ws->ws_Name : (STRPTR) ""));

	GetScalosPath(obj, Filename, sizeof(Filename));
	if (*Filename)
		RemoveWindowPath(Filename);

	OldDir = CurrentDir(ws->ws_Lock);
	pLock = Lock(npa->npa_Path, ACCESS_READ);
	if (pLock)
		{
		NameFromLock(pLock, Filename, sizeof(Filename));
		(void) AddWindowPath(ws, Filename);
		UnLock(pLock);
		}
	CurrentDir(OldDir);

}

//----------------------------------------------------------------------------

static void ScalosWindowSetSize(Class *cl, Object *o, Msg msg)
{
	struct ScaWindowTask *wt = ((struct ScaRootList *) o)->rl_WindowTask;

	d1(KPrintF("%s/%s/%ld: ws=<%s>  ws_Left=%ld  ws_Top=%ld  ws_Width=%ld  ws_Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
		wt->mt_WindowStruct->ws_Name ? wt->mt_WindowStruct->ws_Name : (STRPTR) "", \
		wt->mt_WindowStruct->ws_Left, wt->mt_WindowStruct->ws_Top,\
		wt->mt_WindowStruct->ws_Width, wt->mt_WindowStruct->ws_Height ));

	SetWindowDimensions_WindowTask(wt,
		wt->mt_WindowStruct->ws_Left,
		wt->mt_WindowStruct->ws_Top,
		wt->mt_WindowStruct->ws_Width,
		wt->mt_WindowStruct->ws_Height);
}

//----------------------------------------------------------------------------

static void DeleteNamedNode(CONST_STRPTR Path)
{
	struct OpenNode *NextNode, *OldNode;

	ObtainSemaphore(&PersistSema);

	// alle Einträge mit gleichen Namen entfernen
	for (OldNode = (struct OpenNode *) OpenList.lh_Head; OldNode != (struct OpenNode *) &OpenList.lh_Tail; )
		{
		NextNode = (struct OpenNode *) OldNode->on_Node.ln_Succ;

		if (0 == Stricmp(OldNode->on_Path, Path) && !OldNode->on_Iconified)
			{
			Remove(&OldNode->on_Node);
			FreeNode(OldNode);
			}

		OldNode = NextNode;
		}

	ReleaseSemaphore(&PersistSema);
}

//----------------------------------------------------------------------------

static void FreeNode(struct OpenNode *OldNode)
{
	if (OldNode)
		{
		if (OldNode->on_Path)
			{
			free(OldNode->on_Path);
			OldNode->on_Path = NULL;
			}
		free(OldNode);
		}
}

//----------------------------------------------------------------------------

static void RewriteNodeListFile(void)
{
	static CONST_STRPTR NewSuffix = "-new";
	static CONST_STRPTR OldSuffix = "-old";
	STRPTR FNameNew;
	STRPTR FNameOld;

	FNameNew = malloc(1 + strlen(Persist_FileName) + strlen(NewSuffix));
	FNameOld = malloc(1 + strlen(Persist_FileName) + strlen(OldSuffix));
	if (FNameNew && FNameOld)
		{
		BOOL Success = TRUE;

		strcpy(FNameNew, Persist_FileName);
		strcat(FNameNew, NewSuffix);

		strcpy(FNameOld, Persist_FileName);
		strcat(FNameOld, OldSuffix);

		do	{
			BPTR fh;

			fh = Open(FNameNew, MODE_NEWFILE);
			if (fh)
				{
				LONG rc;
				struct OpenNode *OldNode;

				rc = FPuts(fh, "# written by persist.plugin\n");
				if (rc < 0)
					{
					Success = FALSE;
					break;
					}
				rc = FPuts(fh, "#\n");
				if (rc < 0)
					{
					Success = FALSE;
					break;
					}
				ObtainSemaphoreShared(&PersistSema);

				for (OldNode = (struct OpenNode *) OpenList.lh_Head; 
						Success && (OldNode != (struct OpenNode *) &OpenList.lh_Tail);
						OldNode = (struct OpenNode *) OldNode->on_Node.ln_Succ )
					{
					if (OldNode->on_Left)
						{
						rc = FPrintf(fh, "X=%ld ", OldNode->on_Left);
						if (rc < 0)
							{
							Success = FALSE;
							break;
							}
						}
					if (OldNode->on_Top)
						{
						rc = FPrintf(fh, "Y=%ld ", OldNode->on_Top);
						if (rc < 0)
							{
							Success = FALSE;
							break;
							}
						}
					if (OldNode->on_Width)
						{
						rc = FPrintf(fh, "W=%ld ", OldNode->on_Width);
						if (rc < 0)
							{
							Success = FALSE;
							break;
							}
						}
					if (OldNode->on_Height)
						{
						rc = FPrintf(fh, "H=%ld ", OldNode->on_Height);
						if (rc < 0)
							{
							Success = FALSE;
							break;
							}
						}
					rc = FPrintf(fh, "%s%sP=%s\n",
						(ULONG) (OldNode->on_Iconified ? "I " : ""), 
						(ULONG) (OldNode->on_BrowserMode ? "B " : ""),
						(ULONG) OldNode->on_Path);
					if (rc < 0)
						{
						Success = FALSE;
						break;
						}
					}

				ReleaseSemaphore(&PersistSema);

				if (!Close(fh))
					Success = FALSE;
				}
			else
				{
				Success = FALSE;
				}
			} while (0);

		if (Success)
			{
			(void) DeleteFile(FNameOld);
			(void) Rename(Persist_FileName, FNameOld);

			if (Rename(FNameNew, Persist_FileName))
				{
				(void) DeleteFile(FNameOld);
				}
			}
		else
			{
			(void) DeleteFile(FNameNew);
			}
		}
	if (FNameNew)
		free(FNameNew);
	if (FNameOld)
		free(FNameOld);
}

//----------------------------------------------------------------------------

static BOOL StartReOpenProcess(void)
{
	STATIC_PATCHFUNC(ReOpenProcess)
	struct Process *myProc;

	// CreateNewProc()
	myProc = CreateNewProcTags(NP_Name, (ULONG) "ReOpenScaloswindows",
			NP_Priority, 0,
			NP_Entry, (ULONG) PATCH_NEWFUNC(ReOpenProcess),
			NP_StackSize, 32768,
			TAG_END);
	if (myProc == NULL)
		{
//		alarm("StartReOpenProcess: CreateProc() failed");
		return FALSE;
		}

	return TRUE;
}

//----------------------------------------------------------------------------

static SAVEDS(int) ReOpenProcess(void)
{
	struct MsgPort   *inputPort = NULL;
	struct IOStdReq  *inputRequest = NULL;

	d(KPrintF(__FUNC__ "/%ld Start\n", __LINE__);)

	do	{
		NewList(&ReOpenList);

		IconBase = OpenLibrary("icon.library", 39);
		if (NULL == IconBase)
			break;
#ifdef __amigaos4__
		IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
		if (NULL == IIcon)
			break;
#endif /* __amigaos4__ */

		IconobjectBase = OpenLibrary("iconobject.library", 39);
		if (NULL == IconobjectBase)
			break;
#ifdef __amigaos4__
		IIconobject = (struct IconobjectIFace *)GetInterface((struct Library *)IconobjectBase, "main", 1, NULL);
		if (NULL == IIconobject)
			break;
#endif /* __amigaos4__ */

		d(kprintf(__FUNC__ "/%ld \n", __LINE__);)

		// open input.device for use by PeekQualifier()
		inputPort = CreateMsgPort();
		if (NULL == inputPort)
			break;
		inputRequest = (struct IOStdReq *) CreateExtIO(inputPort, sizeof(struct IOStdReq));
		if (NULL == inputRequest)
			break;

		if (0 != OpenDevice("input.device", 0, (struct IORequest *) inputRequest, 0))
			break;
		InputBase = (T_INPUTBASE) inputRequest->io_Device;
#ifdef __amigaos4__
		IInput  = (struct InputIFace *) GetInterface((struct Library *)InputBase, "main", 1, NULL);
		if (NULL == IInput)
			break;
#endif /* __amigaos4__ */

		d(kprintf("%s/%ld \n", __FUNC__, __LINE__);)

		ReadNodes();

		d1(kprintf("%s/%ld \n", __FUNC__, __LINE__);)

		while (!wbStartupFinished)
			{
			Delay(5);
			}

		d1(kprintf("%s/%ld \n", __FUNC__, __LINE__);)

		if (!OpenNodes())
			break;

		CleanupNodes();
		} while (0);

#ifdef __amigaos4__
	if (IInput)
		{
		DropInterface((struct Interface *)IInput);
		IInput = NULL;
		}
#endif /* __amigaos4__ */
	if (InputBase)
		{
		CloseDevice((struct IORequest *) inputRequest);
		InputBase = NULL;
		}
	if (inputRequest)
		DeleteExtIO((struct IORequest *) inputRequest);
	if (inputPort)
		DeleteMsgPort(inputPort);
#ifdef __amigaos4__
	if (IIconobject)
		{
		DropInterface((struct Interface *)IIconobject);
		IIconobject = NULL;
		}
#endif /* __amigaos4__ */
	if (IconobjectBase)
		{
		CloseLibrary(IconobjectBase);
		IconobjectBase = NULL;
		}
#ifdef __amigaos4__
	if (IIcon)
		{
		DropInterface((struct Interface *)IIcon);
		IIcon = NULL;
		}
#endif /* __amigaos4__ */
	if (IconBase)
		{
		CloseLibrary(IconBase);
		IconBase = NULL;
		}

	return 0;
}

//----------------------------------------------------------------------------

static void ReadNodes(void)
{
	BPTR fd;
	struct RDArgs *rdArgs;

	rdArgs = AllocDosObject(DOS_RDARGS, NULL);
	if (NULL == rdArgs)
		{
//		Printf("AllocDosObject(DOS_RDARG) failed,  err=%ld\n", IoErr());
		return;
		}

	fd = Open(Persist_FileName, MODE_OLDFILE);
	if (fd)
		{
		char Line[256];

		while (FGets(fd, Line, sizeof(Line)))
			{
			striplf(Line);

			d1(KPrintF(__FUNC__ "/%ld: Line = <%s>\n", __LINE__, Line));

			if (strlen(Line) > 0 && '#' != *Line)
				{
				static CONST_STRPTR Template = "I=ICONIFIED/S,B=BROWSERMODE/S,P=PATH/A/F,X=LEFT/K/N,Y=TOP/K/N,W=WIDTH/K/N,H=HEIGHT/K/N";
				enum	{
					ARG_Iconified,
					ARG_BrowserMode,
					ARG_Path,
					ARG_Left,
					ARG_Top,
					ARG_Width,
					ARG_Height
                                        };
				SIPTR ArgArray[7];
				struct RDArgs *rdArg;
				char *ArgString;

				memset(ArgArray, 0, sizeof(ArgArray));

				ArgString = malloc(strlen(Line) + 2);
				if (ArgString)
					{
					strcpy(ArgString, Line);
					strcat(ArgString, "\n");

					rdArgs->RDA_Source.CS_Buffer = ArgString;
					rdArgs->RDA_Source.CS_Length = strlen(ArgString);
					rdArgs->RDA_Source.CS_CurChr = 0;
					rdArgs->RDA_Flags |= RDAF_NOPROMPT;

					rdArg = ReadArgs((STRPTR) Template, ArgArray, rdArgs);

					d1(KPrintF(__FUNC__ "/%ld: rdArg=%08lx\n", __LINE__, rdArg));
					if (rdArg)
						{
						struct OpenNode *NewNode = malloc(sizeof(struct OpenNode));

						if (NewNode)
							{
							d1(KPrintF(__FUNC__ "/%ld: Iconified=%ld  Path=<%s>\n", __LINE__, ArgArray[ARG_Iconified], ArgArray[ARG_Path]));

							NewNode->on_Path = strdup((char *) ArgArray[ARG_Path]);

							if (ArgArray[ARG_Iconified])
								NewNode->on_Iconified = TRUE;
							else
								NewNode->on_Iconified = FALSE;

							if (ArgArray[ARG_BrowserMode])
								NewNode->on_BrowserMode = TRUE;
							else
								NewNode->on_BrowserMode = FALSE;

							if (ArgArray[ARG_Left])
								NewNode->on_Left = *((LONG *) ArgArray[ARG_Left]);
							else
								NewNode->on_Left = 0;
							if (ArgArray[ARG_Top])
								NewNode->on_Top = *((LONG *) ArgArray[ARG_Top]);
							else
								NewNode->on_Top = 0;
							if (ArgArray[ARG_Width])
								NewNode->on_Width = *((LONG *) ArgArray[ARG_Width]);
							else
								NewNode->on_Width = 0;
							if (ArgArray[ARG_Height])
								NewNode->on_Height = *((LONG *) ArgArray[ARG_Height]);
							else
								NewNode->on_Height = 0;

							d1(KPrintF("%s/%s/%ld: Path=<%s>  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
								NewNode->on_Path, \
								NewNode->on_Left, NewNode->on_Top, NewNode->on_Width, NewNode->on_Height ));

							AddTail(&ReOpenList, &NewNode->on_Node);
							}

						FreeArgs(rdArg);
						}
					else
						{
//						LONG Error = IoErr();

						d(kprintf(__FUNC__ "/%ld: ReadArgs() error = %ld\n", __LINE__, Error));
//						Printf("ReadArgs(%s) returned>  error=%ld\n", Line, Error);
						}
					free(ArgString);
					}
				}
			}

		Close(fd);
		// persist file autimatically gets rewritten when first window opens
		}
	else
		{
//		Printf("Failed to open file <%s>  err=%ld\n", Persist_FileName, IoErr());
		}

	FreeDosObject(DOS_RDARGS, rdArgs);
}

//----------------------------------------------------------------------------

static BOOL OpenNodes(void)
{
	struct OpenNode *OldNode;
	BOOL NormalFinished = TRUE;

	while (OldNode = (struct OpenNode *) RemHead(&ReOpenList))
		{
		UWORD Qualifier = PeekQualifier();

		if (Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
			{
			NormalFinished = FALSE;
			break;
			}

		Delay(OpenDelayTicks);

		if (ExistsDevice(OldNode->on_Path))
			{
			struct IBox WindowRect;

			d1(KPrintF("%s/%s/%ld: path=<%s>  iconified=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, OldNode->on_Path, OldNode->on_Iconified, \
				OldNode->on_Left, OldNode->on_Top, OldNode->on_Width, OldNode->on_Height));

			WindowRect.Left = OldNode->on_Left;
			WindowRect.Top = OldNode->on_Top;

			if (UseWindowSizes)
				{
				WindowRect.Width = OldNode->on_Width;
				WindowRect.Height = OldNode->on_Height;
				}
			else
				{
				// Do not use stored window position and size
				WindowRect.Width = WindowRect.Height = 0;
				}

			SCA_OpenDrawerByNameTags(OldNode->on_Path,
				UseScaIconify ? SCA_Iconify : TAG_IGNORE, OldNode->on_Iconified,
				SCA_NoActivateWindow, TRUE,
				SCA_BrowserMode, OldNode->on_BrowserMode,
				((0 != WindowRect.Width) && (0 != WindowRect.Height))
					? SCA_WindowRect : TAG_IGNORE, (ULONG) &WindowRect,
				TAG_END);
			}

		if (OldNode->on_Iconified)
			{
			if (UseScaIconify)
				{
				struct OpenNode *NewNode;

				NewNode = AddWindowPath(NULL, OldNode->on_Path);

				if (NewNode)
					{
					NewNode->on_Iconified = TRUE;
					TriggerUpdater();
					}
				}
			else
				{
				BPTR wLock = Lock(OldNode->on_Path, ACCESS_READ);

				if (wLock)
					{
					struct ScaWindowStruct *sw;

					Delay(2 * 50);		// allow window to open...

					sw = FindWindowByLock(wLock);
					if (sw)
						{
						SCA_UnLockWindowList();
						DoMethod(sw->ws_WindowTask->mt_WindowObject, SCCM_Window_Iconify);
						}

					UnLock(wLock);
					}
				}
			}

		if (OldNode->on_Path)
			{
			free(OldNode->on_Path);
			OldNode->on_Path = NULL;
			}
		free(OldNode);
		}

	return NormalFinished;
}

//----------------------------------------------------------------------------

static void CleanupNodes(void)
{
	struct OpenNode *OldNode;

	while (OldNode = (struct OpenNode *) RemTail(&ReOpenList))
		{
		if (OldNode->on_Path)
			{
			free(OldNode->on_Path);
			OldNode->on_Path = NULL;
			}
		free(OldNode);
		}
}

//----------------------------------------------------------------------------

static void striplf(char *Line)
{
	size_t Len = strlen(Line);

	if ('\n' == Line[Len - 1])
		Line[Len - 1] = '\0';
}

//----------------------------------------------------------------------------

static void ReadConfig(CONST_STRPTR Filename)
{
	BPTR fh;

	fh = Open(Filename, MODE_OLDFILE);
	if ((BPTR)NULL == fh)
		return;

	while (1)
		{
		char Line[160];
		char *lp;

		lp = FGets(fh, Line, sizeof(Line));

		if (lp)
			{
			char OptName[40];
			size_t len = strlen(lp);
			size_t Length;
			char *p;

			d1(kprintf(__FUNC__ "/%ld Line=<%s>\n", __LINE__, Line);)

			if (';' == *lp || '#' == *lp)
				continue;		// skip comment lines
			if (isspace((int) *lp))
				continue;		// skip lines beginning with spaces (or empty lines)
			if (0 == len)
				continue;

			if ('\n' == lp[len - 1])
				lp[len - 1] = '\0';

			// OptName lesen
			for (p=OptName, Length=sizeof(OptName)-1;
				*lp && !isspace((int) *lp) && *lp != ':' && *lp != '.' && Length;
				*p++ = *lp++, Length--);
			*p = '\0';

			while (*lp && *lp != '=')
				lp++;
			if (*lp == '\0')
				break;

			lp = stpblk(++lp);

			d1(kprintf(__FUNC__ "/%ld OptName=<%s>  Arg=<%s>\n", __LINE__, OptName, lp);)

			if (0 == Stricmp(OptName, "OpenDelay_Ticks"))
				{
				sscanf(lp, "%ld", &OpenDelayTicks);
				d1(kprintf(__FUNC__ "/%ld  OpenDelayTicks = %ld\n", __LINE__, OpenDelayTicks);)
				}
			else if (0 == Stricmp(OptName, "Use_SCA_Iconify"))
				{
				UseScaIconify = TRUE;
				d1(kprintf(__FUNC__ "/%ld  UseScaIconify = %ld\n", __LINE__, UseScaIconify);)
				}
			else if (0 == Stricmp(OptName, "Use_Window_Sizes"))
				{
				sscanf(lp, "%ld", &UseWindowSizes);
				d1(KPrintF(__FUNC__ "/%ld  UseWindowSizes = %ld\n", __LINE__, UseWindowSizes);)
				}
			else if (0 == Stricmp(OptName, "Persist_FileName"))
				{
				char *dp;
				size_t l;
				char StopChar = '\0';

				if ('"' == *lp)
					{
					lp++;
					StopChar = '"';
					}

				for (dp=Persist_FileName, l=sizeof(Persist_FileName);
					l > 1 && *lp && *lp != StopChar; )
					{
					*dp++ = *lp++;
					l--;
					}
				*dp = '\0';

				d1(kprintf(__FUNC__ "/%ld  Persist_FileName = <%s>\n", __LINE__, Persist_FileName);)
				}
			}
		else
			{
			break;
			}
		}

	Close(fh);
}

//----------------------------------------------------------------------------

static BOOL ExistsDevice(CONST_STRPTR Path)
{
	char DeviceName[256];
	short n;
	char *dp;
	struct DosList *dl, *dlFound;

	n = sizeof(DeviceName);
	dp = DeviceName;
	while (*Path && ':' != *Path && n > 1)
		{
		*dp++ = *Path++;
		n--;
		}
	*dp = '\0';

	dl = LockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_ASSIGNS | LDF_READ);

	dlFound = FindDosEntry(dl, DeviceName, LDF_VOLUMES | LDF_DEVICES | LDF_ASSIGNS);

	d(kprintf(__FUNC__ "/%ld Dev=<%s>  dlFound=%08lx\n", __LINE__, DeviceName, dlFound));

	UnLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_ASSIGNS | LDF_READ);

	return (BOOL) (dlFound != NULL);
}

//----------------------------------------------------------------------------

static struct OpenNode *FindNamedNode(CONST_STRPTR Path)
{
	struct OpenNode *OldNode;

	ObtainSemaphoreShared(&PersistSema);

	for (OldNode = (struct OpenNode *) OpenList.lh_Head; OldNode != (struct OpenNode *) &OpenList.lh_Tail; )
		{
		struct OpenNode *NextNode = (struct OpenNode *) OldNode->on_Node.ln_Succ;

		if (0 == Stricmp(OldNode->on_Path, Path) && !OldNode->on_Iconified)
			{
			ReleaseSemaphore(&PersistSema);
			return OldNode;
			}

		OldNode = NextNode;
		}

	ReleaseSemaphore(&PersistSema);

	return NULL;
}

//----------------------------------------------------------------------------

static struct ScaWindowStruct *FindWindowByLock(BPTR xLock)
{
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	struct ScaWindowStruct *Result = NULL;

	d(kprintf(__FUNC__ "/%ld: xLock=%08lx\n", __LINE__, xLock));

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; NULL == Result && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d(kprintf(__FUNC__ "/%ld: swi=%08lx Lock=%08lx  Same=%ld\n", __LINE__, \
				swi, swi->ws_Lock, SameLock(swi->ws_Lock, xLock)));

			if (LOCK_SAME == SameLock(swi->ws_Lock, xLock))
				{
				d(kprintf(__FUNC__ "/%ld: swi=%08lx\n", __LINE__, swi));
				Result = swi;
				}
			}

		if (NULL == Result)
			SCA_UnLockWindowList();
		}

	d(kprintf(__FUNC__ "/%ld: Result=%08lx\n", __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------

static BOOL StartUpdaterProcess(void)
{
	STATIC_PATCHFUNC(UpdaterProcess)

	// CreateNewProc()
	UpdaterProc = CreateNewProcTags(NP_Name, (ULONG) "Scalos_PersistentWindowsUpdater",
			NP_Priority, 0,
			NP_Entry, (ULONG) PATCH_NEWFUNC(UpdaterProcess),
			NP_StackSize, 32768,
			TAG_END);
	if (UpdaterProc == NULL)
		{
//		alarm("StartReOpenProcess: CreateProc() failed");
		return FALSE;
		}

	return TRUE;
}

//----------------------------------------------------------------------------

static void KillUpdaterProcess(void)
{
	if (UpdaterProc)
		{
		Signal(&UpdaterProc->pr_Task, SIGF_ABORT);
		UpdaterProc = NULL;
		Delay(4);
		}
}

//----------------------------------------------------------------------------

static void TriggerUpdater(void)
{
	d1(KPrintF("%s/%s/%ld UpdaterProc=%08lx  UpdaterProcSignal=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, UpdaterProc, UpdaterProcSignal));

	if (UpdaterProc && UpdaterProcSignal)
		{
		Signal(&UpdaterProc->pr_Task, UpdaterProcSignal);
		}
}

//----------------------------------------------------------------------------

static BOOL InitProcessTimer(struct ProcessTimerInfo *Timer)
{
///
	BYTE odError;

	memset(Timer, 0, sizeof(struct ProcessTimerInfo));

	Timer->ppt_ioPort = CreateMsgPort();
	if (NULL == Timer->ppt_ioPort)
		return FALSE;

	d1(kprintf("%s/%s/%ld: ioPort=%08lx\n", __FILE__, __FUNC__, __LINE__, Timer->ppt_ioPort));

	Timer->ppt_TimeReq = (T_TIMEREQUEST *) CreateIORequest(Timer->ppt_ioPort, sizeof(T_TIMEREQUEST ));
	if (NULL == Timer->ppt_TimeReq)
		return FALSE;

	d1(kprintf("%s/%s/%ld: TimeReq=%08lx\n", __FILE__, __FUNC__, __LINE__, Timer->ppt_TimeReq));

	odError = OpenDevice(TIMERNAME, UNIT_VBLANK, &Timer->ppt_TimeReq->tr_node, 0);
	d1(kprintf("%s/%s/%ld: OpenDevice returned %ld\n", __FILE__, __FUNC__, __LINE__, odError));
	if (0 != odError)
		return FALSE;

	Timer->ppt_timerOpen = TRUE;

	return TRUE;
///
}

//----------------------------------------------------------------------------

static void CleanupProcessTimer(struct ProcessTimerInfo *Timer)
{
///
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (Timer->ppt_TimeReq)
		{
		if (Timer->ppt_timerPending)
			{
			AbortIO((struct IORequest *) Timer->ppt_TimeReq);
			WaitIO((struct IORequest *) Timer->ppt_TimeReq);

			Timer->ppt_timerPending = FALSE;
			}
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		if (Timer->ppt_timerOpen)
			{
			CloseDevice(&Timer->ppt_TimeReq->tr_node);
			Timer->ppt_timerOpen = FALSE;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		DeleteIORequest((struct IORequest *) Timer->ppt_TimeReq);
		Timer->ppt_TimeReq = NULL;
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (Timer->ppt_ioPort)
		{
		DeleteMsgPort(Timer->ppt_ioPort);
		Timer->ppt_ioPort = NULL;
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
///
}

//----------------------------------------------------------------------------

static SAVEDS(int) UpdaterProcess(void)
{
	struct ProcessTimerInfo UpdateTimer;
	struct Process *myProc = (struct Process *) FindTask(NULL);
	APTR prWindowPtr;

	memset(&UpdateTimer, 0, sizeof(UpdateTimer));

	prWindowPtr = myProc->pr_WindowPtr;
	myProc->pr_WindowPtr = (APTR) ~0;    // suppress error requesters

	d1(KPrintF("%s/%s/%ld START\n", __FILE__, __FUNC__, __LINE__);)

	do	{
		ULONG ioMask;
		BYTE signal;

		signal = AllocSignal(-1);
		d1(KPrintF("%s/%s/%ld: signal=%ld\n", __FILE__, __FUNC__, __LINE__, signal));
		if (-1 == signal)
			break;
		UpdaterProcSignal = 1 << signal;
		d1(KPrintF("%s/%s/%ld: UpdaterProcSignal=%08lx\n", __FILE__, __FUNC__, __LINE__, UpdaterProcSignal));

		if (!InitProcessTimer(&UpdateTimer))
			break;

		d1(KPrintF("%s/%s/%ld: InitProcessTimer succeeded\n", __FILE__, __FUNC__, __LINE__));

		ioMask = 1 << UpdateTimer.ppt_ioPort->mp_SigBit;

		d1(KPrintF("%s/%s/%ld: ioMask=%08lx\n", __FILE__, __FUNC__, __LINE__, ioMask));

		do	{
			ULONG RcvdMask;

			d1(KPrintF("%s/%s/%ld: Mask=%08lx\n", __FILE__, __FUNC__, __LINE__, UpdaterProcSignal | ioMask | SIGF_ABORT));

			RcvdMask = Wait(UpdaterProcSignal | ioMask | SIGF_ABORT);

			d1(KPrintF("%s/%s/%ld: RcvdMask=%08lx\n", __FILE__, __FUNC__, __LINE__, RcvdMask));

			if (SIGF_ABORT & RcvdMask)
				{
				d1(KPrintF("%s/%s/%ld SIGF_ABORT\n", __FILE__, __FUNC__, __LINE__);)
				break;
				}

			if (RcvdMask & UpdaterProcSignal)
				{
				// Received new update trigger
				d1(KPrintF("%s/%s/%ld: Triggered! \n", __FILE__, __FUNC__, __LINE__));

				if (UpdateTimer.ppt_timerPending)
					{
					d1(KPrintF("%s/%s/%ld: Abort running timer\n", __FILE__, __FUNC__, __LINE__));

					AbortIO((struct IORequest *) UpdateTimer.ppt_TimeReq);
					WaitIO((struct IORequest *) UpdateTimer.ppt_TimeReq);

					UpdateTimer.ppt_timerPending = FALSE;
					RcvdMask &= ~ioMask;
					}

				// Restart timer
				UpdateTimer.ppt_TimeReq->tr_node.io_Command = TR_ADDREQUEST;
				UpdateTimer.ppt_TimeReq->tr_time.tv_micro = 0;
				UpdateTimer.ppt_TimeReq->tr_time.tv_secs = 1;

				BeginIO((struct IORequest *) UpdateTimer.ppt_TimeReq);
				UpdateTimer.ppt_timerPending = TRUE;
				}

			if (RcvdMask & ioMask)
				{
				// Update timer expired
				d1(KPrintF("%s/%s/%ld: Update timer expired, timerPending=%ld\n", __FILE__, __FUNC__, __LINE__, UpdateTimer.ppt_timerPending));

				if (UpdateTimer.ppt_timerPending)
					{
					WaitIO((struct IORequest *) UpdateTimer.ppt_TimeReq);
					UpdateTimer.ppt_timerPending = FALSE;
					}

				RewriteNodeListFile();
				}
			} while (1);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	// restore pr_WindowPtr
	myProc->pr_WindowPtr = prWindowPtr;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	CleanupProcessTimer(&UpdateTimer);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//----------------------------------------------------------------------------

#ifdef __SASC

void _XCEXIT(long x)
{
	(void) x;
}

#else /* __SASC */

#if !defined(__MORPHOS__) && !defined(__AROS__)
// Replacement for SAS/C library functions

static char *stpblk(CONST_STRPTR q)
{
	while (q && *q && isspace((int) *q))
		q++;

	return (char *) q;
}
#endif /* __MORPHOS__ */

#if !defined(__amigaos4__)

void exit(int x)
{
	(void) x;
	while (1)
		;
}

APTR _WBenchMsg;

#endif /* __amigaos4__ */

#endif /* __SASC */

//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------

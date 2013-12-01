// WBStartup.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions
static SAVEDS(LONG) CompareStartPriFunc(struct Hook * hook, struct ScaIconNode * in2, struct ScaIconNode * in1);
static void StartTool(BPTR DirLock, struct ScaIconNode *in);
static BOOL ReadWbStartupPrefs(void);
static void AddWbStartupEntries(struct internalScaWindowTask *iwt);

//----------------------------------------------------------------------------

// local data items

// ID for AmigaOS4.x WBStartup preferences
#define	OS4_WBSTARTUP_ID		0x000c6fbd	// ID at offset 0 of preferences file

// Bits in global flags of AmigaOS4.x WBStartup preferences
#define	OS4_WBSTART_TRANSPARENTWINDOW	(1 << 3)
#define	OS4_WBSTART_PROGRESSWINDOW	(1 << 0)

// AmigaOS4.x WBStartup preferences entry flags
#define	OS4_WBSTARTUP_DISABLED		(1 << 8)	// Flag: this entry is disabled

static struct Hook CompareStartPriHook = 
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(CompareStartPriFunc),	// h_Entry + h_SubEntry
	NULL,						// h_Data
	};

static struct BackDropList bdlWBStartup;

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------

SAVEDS(void) INTERRUPT WBStartup(void)
{
	struct ScaWindowStruct ws;
	struct internalScaWindowTask *iwt = &MainWindowTask->mwt;
	BPTR oldDir = NOT_A_LOCK;

	d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

	do	{
		struct ScaIconNode *in, *inNext;

		ws.ws_WindowTask = &iwt->iwt_WindowTask;
		iwt->iwt_WindowTask.mt_WindowStruct = &ws;
		iwt->iwt_ThumbnailMode = THUMBNAILS_Never;
		ws.ws_IconSizeConstraints = CurrentPrefs.pref_IconSizeConstraints;
		ws.ws_IconScaleFactor = CurrentPrefs.pref_IconScaleFactor;

		(void) ReadWbStartupPrefs();	// Read AmigaOS4.x WBStartup preferences

		ws.ws_Lock = Lock((STRPTR) CurrentPrefs.pref_WBStartupDir, ACCESS_READ);
		if ((BPTR)NULL == ws.ws_Lock)
			break;

		oldDir = CurrentDir(ws.ws_Lock);

		SetFileSysTask(DeviceProc((STRPTR) CurrentPrefs.pref_WBStartupDir));

		ReadIconList(iwt);

		AddWbStartupEntries(iwt);

		// move all nodes from LateIconList to IconList
		for (in = iwt->iwt_WindowTask.wt_LateIconList; in; in = inNext)
			{
			d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name));

			inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_LateIconList, &iwt->iwt_WindowTask.wt_IconList, in);
			}

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = inNext)
			{
			ULONG IconType;
			STRPTR tt;
			LONG StartPri;

			inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

			GetAttr(IDTA_Type, in->in_Icon, &IconType);

			d1(kprintf("%s/%s/%ld: in=%08lx  <%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name, IconType));

			if (WBPROJECT == IconType || WBTOOL == IconType)
				{
				tt = NULL;
				if (DoMethod(in->in_Icon, IDTM_FindToolType, "STARTPRI", &tt)
					&& DoMethod(in->in_Icon, IDTM_GetToolTypeValue, tt, &StartPri))
					{
					in->in_Userdata = StartPri;
					}
				}
			else
				{
				FreeIconNode(iwt, &iwt->iwt_WindowTask.wt_IconList, in);
				continue;
				}
			}

		SCA_SortNodes((struct ScalosNodeList *) &iwt->iwt_WindowTask.wt_IconList, &CompareStartPriHook, SCA_SortType_Best);

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			SplashDisplayProgress(GetLocString(MSGID_PROGRESS_WBSTARTUP_RUNPRG),
				1,
				(ULONG) in->in_Name);

			debugLock_d1(in->in_Lock);

			StartTool(in->in_Lock ? in->in_Lock : ws.ws_Lock, in);
			}
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (ws.ws_Lock)
		UnLock(ws.ws_Lock);

	WBStartupFinished();
}


// compare function for icon list sorting
static SAVEDS(LONG) CompareStartPriFunc(struct Hook * hook, struct ScaIconNode * in2, struct ScaIconNode * in1)
{
	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	(void) hook;

	return in1->in_Userdata - in2->in_Userdata;
}


static void StartTool(BPTR DirLock, struct ScaIconNode *in)
{
	struct WBArg ArgArray;
	STRPTR tt;
	LONG WaitTime = 0;
	ULONG Flags = SCAF_WBStart_Wait;

	debugLock_d1(DirLock);
	d1(kprintf("%s/%s/%ld: in=<%s>\n", __FILE__, __FUNC__, __LINE__, GetIconName(in)));

	tt = NULL;
	if (DoMethod(in->in_Icon, IDTM_FindToolType, "DONOTWAIT", &tt))
		Flags &= ~SCAF_WBStart_Wait;

	tt = NULL;
	if (DoMethod(in->in_Icon, IDTM_FindToolType, "WAIT", &tt))
		{
		DoMethod(in->in_Icon, IDTM_GetToolTypeValue, tt, &WaitTime);
		}

	ArgArray.wa_Name = in->in_Name;
	ArgArray.wa_Lock = DupLock(DirLock);

	d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, ArgArray.wa_Name));
	debugLock_d1(ArgArray.wa_Lock);

	SCA_WBStartTags(&ArgArray, 1,
		SCA_IconObject, (ULONG) in->in_Icon,
		SCA_Flags, Flags,
		TAG_END);

	if (WaitTime)
		Delay(WaitTime * 50);
}


void WBStartupFinished(void)
{
	struct ScaWindowList *winList;

	SplashDisplayProgress(GetLocString(MSGID_PROGRESS_WBSTARTUPFINISHED), 0);
	SplashRemoveUser();

	winList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	if (winList)
		{
		struct ScaWindowStruct *ws;

		for (ws = winList->wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			struct ScaWindowTask *wt = ws->ws_WindowTask;

			DoMethod(wt->mt_MainObject, SCCM_IconWin_WBStartupFinished);
			DoMethod(wt->mt_WindowObject, SCCM_Window_WBStartupFinished);
			}

		SCA_UnLockWindowList();
		}

	BackdropFreeList(&bdlWBStartup);
}


// Read AmigaOS4.x WBStartup preferences

static BOOL ReadWbStartupPrefs(void)
{
	CONST_STRPTR FileName = "ENV:Sys/wbstartup.prefs";
	BPTR fh;
	BOOL Success = FALSE;

	do	{
		ULONG LData;
		LONG Length;

		BackDropInitList(&bdlWBStartup);

		fh = Open(FileName, MODE_OLDFILE);
		d1(KPrintF("%s/%s/%ld:  fh=%08lx\n", __FILE__, __FUNC__, __LINE__, fh));
		if ((BPTR)NULL == fh)
			break;

		// read ID
		Length = Read(fh, &LData, sizeof(LData));
		d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
		if (sizeof(LData) != Length)
			break;

		d1(kprintf("%s/%s/%ld:  ID=%08lx, expect=%08lx\n", __FILE__, __FUNC__, __LINE__, LData, OS4_WBSTARTUP_ID));

		if (OS4_WBSTARTUP_ID != LData)
			break;

		// read global flags
		Length = Read(fh, &LData, sizeof(LData));
		d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
		if (sizeof(LData) != Length)
			break;

		Success = TRUE;

		// process entries
		while (1)
			{
			ULONG Flags;
			ULONG NameLength;
			STRPTR Name;

			// Read flags
			Length = Read(fh, &Flags, sizeof(Flags));
			d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
			if (sizeof(Flags) != Length)
				break;

			// Read Length of name
			Length = Read(fh, &NameLength, sizeof(NameLength));
			d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
			if (sizeof(NameLength) != Length)
				break;

			Name = ScalosAlloc(1 + NameLength);
			if (Name)
				{
				// Read name
				Length = Read(fh, Name, NameLength);
				d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
				if (NameLength != Length)
					{
					ScalosFree(Name);
					break;
					}

				Name[NameLength] = '\0';

				d1(kprintf("%s/%s/%ld:  Name=<%s>  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, Name, Flags));

				if (!(Flags & OS4_WBSTARTUP_DISABLED))
					BackdropAddLine(&bdlWBStartup, Name, NO_ICON_POSITION, NO_ICON_POSITION);

				ScalosFree(Name);
				}
			}
		} while (0);

	if (fh)
		Close(fh);

	return Success;
}


// Create icons for all entries configured in AmigaOS4.x WBStartup preferences
static void AddWbStartupEntries(struct internalScaWindowTask *iwt)
{
	do	{
		struct ChainedLine *cnl;

		for (cnl = (struct ChainedLine *) bdlWBStartup.bdl_LinesList.lh_Head;
			cnl != (struct ChainedLine *) &bdlWBStartup.bdl_LinesList.lh_Tail;
			cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ )
			{
			STRPTR IconPath;
			
			IconPath = AllocCopyString(cnl->cnl_Line);
			if (IconPath)
				{
				BPTR dirLock;
				STRPTR lp;

				lp = PathPart(IconPath);
				*lp = '\0';	 // strip file part for icon directory

				d1(kprintf("%s/%s/%ld:  IconPath=<%s>\n", __FILE__, __FUNC__, __LINE__, IconPath));

				dirLock = Lock(IconPath, ACCESS_READ);
				debugLock_d1(dirLock);
				if (dirLock)
					{
					struct ScaIconNode *in;
					struct ScaReadIconArg ria;
					CONST_STRPTR IconName;

					ria.ria_Lock = dirLock;
					ria.ria_x = ria.ria_y = NO_ICON_POSITION_SHORT;
					ria.ria_IconType = ICONTYPE_NONE;

					IconName = FilePart(cnl->cnl_Line);

					debugLock_d1(ria.ria_Lock);
					d1(kprintf("%s/%s/%ld:  IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, IconName));

					in = IconWindowReadIcon(iwt, IconName, &ria);
					if (in)
						{
						in->in_Lock = dirLock;
						(void) SetToolType(in->in_Icon, "DONOTWAIT", "", FALSE);
						}
					else
						UnLock(dirLock);
					}

				FreeCopyString(IconPath);
				}
			}
		} while (0);
}


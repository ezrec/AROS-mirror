// Backdrop.c
// $Date$
// $Revision$
// 04 Sep 2004 15:33:26


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <datatypes/pictureclass.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local Data definitions

//----------------------------------------------------------------------------

// local functions

static LONG BackdropWriteList(struct BackDropList *bdl);
static BOOL BackdropInternalAddLine(struct BackDropList *bdl, CONST_STRPTR NewLine, LONG PosX, LONG PosY);
static struct ChainedLine *BackdropAllocChainedLine(CONST_STRPTR TextLine);
static void BackdropFreeChainedLine(struct ChainedLine *cnl);
static void BackdropRemoveObsoleteEntriesFromBackdropFile(struct BackDropList *bdl);
static void BackdropRemoveDuplicateEntriesFromBackdropFile(struct BackDropList *bdl);
static void BackdropRedrawMainIconObject(struct internalScaWindowTask *iwtMain, Object *IconObj);

//----------------------------------------------------------------------------

// local Data

//----------------------------------------------------------------------------

void BackDropInitList(struct BackDropList *bdl)
{
	NewList(&bdl->bdl_LinesList);
	bdl->bdl_Loaded = FALSE;
	bdl->bdl_Changed = FALSE;
	bdl->bdl_Filtered = FALSE;
	bdl->bdl_NotPresent = FALSE;
	bdl->bdl_Initialized = TRUE;
}


LONG BackdropLoadList(struct BackDropList *bdl)
{
	const size_t LineSize = 300;
	LONG Result = RETURN_OK;
	char *Line;
	BPTR fh;

	BackDropInitList(bdl);

	ShortcutAddLines(bdl);		// add left-out icons from Ambient shortcuts

	fh = Open(".backdrop", MODE_OLDFILE);
	d1(kprintf("%s/%s/%ld: fh=%08lx\n", __FILE__, __FUNC__, __LINE__, fh));
	if ((BPTR)NULL == fh)
		{
		Result = IoErr();
		if (ERROR_OBJECT_NOT_FOUND == Result)
			{
			bdl->bdl_NotPresent = TRUE;
			Result = RETURN_OK;
			}

		return Result;
		}

	Line = ScalosAlloc(LineSize);
	if (NULL == Line)
		{
		Close(fh);
		return ERROR_NO_FREE_STORE;
		}

	bdl->bdl_Loaded = TRUE;

	while (FGets(fh, Line, LineSize))
		{
		if ('\n' == Line[strlen(Line) - 1])	// strip trailing LF
			Line[strlen(Line) - 1] = '\0';

		d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, Line));

		if (strlen(Line) >= 1)
			{
			BackdropAddLine(bdl, Line, NO_ICON_POSITION, NO_ICON_POSITION);
			}
		}

	ScalosFree(Line);

	if (!Close(fh))
		return IoErr();

	return Result;
}


void BackdropFreeList(struct BackDropList *bdl)
{
	struct ChainedLine *cnl;

	if (! bdl->bdl_Initialized)
		return;

	while ((cnl = (struct ChainedLine *) RemHead(&bdl->bdl_LinesList)))
		{
		BackdropFreeChainedLine(cnl);
		}
}


void BackdropFilterList(struct BackDropList *bdl, BPTR dirLock)
{
	struct ChainedLine *cnl;
	BPTR oldDir;
	STRPTR Path;

	d1(kprintf("%s/%s/%ld: START bdl=%08lx\n", __FILE__, __FUNC__, __LINE__, bdl));
	debugLock_d1(dirLock);

	if (BNULL == dirLock)
		return;

	if (bdl->bdl_Filtered)
		return;

	Path = AllocPathBuffer();
	if (NULL == Path)
		return;

	oldDir = CurrentDir(dirLock);

	if (!bdl->bdl_Loaded && !bdl->bdl_NotPresent)
		{
		BPTR RootDirLock;

		do	{
			RootDirLock = Lock(":", ACCESS_READ);
			debugLock_d1(RootDirLock);
			if (BNULL == RootDirLock)
				break;

			// change to root directory of icon drawer
			CurrentDir(RootDirLock);

			if (RETURN_OK != BackdropLoadList(bdl))
				break;
			} while (0);

		CurrentDir(dirLock);

		if (RootDirLock)
			UnLock(RootDirLock);
		}

	bdl->bdl_Filtered = TRUE;

	for (cnl = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
		cnl != (struct ChainedLine *) &bdl->bdl_LinesList.lh_Tail; )
		{
		BPTR tmpLock;
		STRPTR lp;
		struct ChainedLine *cnlNext = (struct ChainedLine *) cnl->cnl_Node.ln_Succ;;

		d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

		stccpy(Path, cnl->cnl_Line, Max_PathLen);

		// remove file part
		lp = PathPart(Path);
		if (lp)
			*lp = '\0';

		tmpLock = Lock(Path, ACCESS_READ);
		debugLock_d1(tmpLock);
		if (tmpLock)
			{
			if (LOCK_SAME != ScaSameLock(tmpLock, dirLock))
				{
				// delete entry from .backdrop
				d1(kprintf("%s/%s/%ld: Filter Line <%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

				Remove(&cnl->cnl_Node);

				BackdropFreeChainedLine(cnl);
				}
			UnLock(tmpLock);
			}

		cnl = cnlNext;
		}

	FreePathBuffer(Path);
	CurrentDir(oldDir);

	d1(kprintf("%s/%s/%ld: END bdl=%08lx\n", __FILE__, __FUNC__, __LINE__, bdl));
}


static LONG BackdropWriteList(struct BackDropList *bdl)
{
	struct ChainedLine *cnl;
	LONG Result = RETURN_OK;
	BPTR fh;

	d1(kprintf("%s/%s/%ld: bdl=%08lx\n", __FILE__, __FUNC__, __LINE__, bdl));

	// remove obsolete entries which refer to non-existing icons
	BackdropRemoveObsoleteEntriesFromBackdropFile(bdl);

	BackdropRemoveDuplicateEntriesFromBackdropFile(bdl);

	if (!bdl->bdl_Changed)
		return RETURN_OK;	// No changes

	fh = Open(".backdrop", MODE_NEWFILE);
	if ((BPTR)NULL == fh)
		return IoErr();

	while ((cnl = (struct ChainedLine *) RemHead(&bdl->bdl_LinesList)) && RETURN_OK == Result)
		{
		d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

		if (-1 == FPrintf(fh, "%s\n", (ULONG) cnl->cnl_Line))
			Result = IoErr();
		}

	Close(fh);

	bdl->bdl_Changed = FALSE;

	return Result;
}


BOOL BackdropAddLine(struct BackDropList *bdl, CONST_STRPTR NewLine, LONG PosX, LONG PosY)
{
	struct ChainedLine *cnl;

	for (cnl = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
		cnl != (struct ChainedLine *) &bdl->bdl_LinesList.lh_Tail;
		cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ )
		{
		if (0 == Stricmp(NewLine, cnl->cnl_Line))
			{
			// Duplicate line, ignore it silently
			return TRUE;
			}
		}

	return BackdropInternalAddLine(bdl, NewLine, PosX, PosY);
}


static BOOL BackdropInternalAddLine(struct BackDropList *bdl, CONST_STRPTR NewLine, LONG PosX, LONG PosY)
{
	BOOL Success = FALSE;
	struct ChainedLine *cnl;

	cnl = BackdropAllocChainedLine(NewLine);
	if (cnl)
		{
		AddTail(&bdl->bdl_LinesList, (struct Node *) &cnl->cnl_Node);
		bdl->bdl_Changed = TRUE;
		cnl->cnl_PosX = PosX;
		cnl->cnl_PosY = PosY;
		Success = TRUE;
		}

	return Success;
}


BOOL BackdropRemoveLine(struct BackDropList *bdl, BPTR iconLock)
{
	struct ChainedLine *cnl;
	BOOL Found = FALSE;
	STRPTR Path;

	d1(kprintf("%s/%s/%ld: bsl=%08lx\n", __FILE__, __FUNC__, __LINE__, bdl));
	debugLock_d1(iconLock);

	Path = AllocPathBuffer();
	if (NULL == Path)
		return FALSE;

	for (cnl = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
		cnl != (struct ChainedLine *) &bdl->bdl_LinesList.lh_Tail; )
		{
		BPTR tmpLock;
		struct ChainedLine *cnlNext = (struct ChainedLine *) cnl->cnl_Node.ln_Succ;

		d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

		stccpy(Path, cnl->cnl_Line, Max_PathLen);
		SafeStrCat(Path, ".info", Max_PathLen);

		tmpLock = Lock(Path, ACCESS_READ);
		if (tmpLock)
			{
			if (LOCK_SAME == ScaSameLock(tmpLock, iconLock))
				{
				// delete entry from .backdrop
				d1(kprintf("%s/%s/%ld: Delete Line <%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

				Found = bdl->bdl_Changed = TRUE;

				Remove(&cnl->cnl_Node);

				BackdropFreeChainedLine(cnl);
				}
			UnLock(tmpLock);
			}

		cnl = cnlNext;
		}

	FreePathBuffer(Path);

	return Found;
}


static struct ChainedLine *BackdropAllocChainedLine(CONST_STRPTR TextLine)
{
	struct ChainedLine *cnl = ScalosAlloc(sizeof(struct ChainedLine));

	if (NULL == cnl)
		return NULL;

	cnl->cnl_PosX = cnl->cnl_PosY = NO_ICON_POSITION;
	cnl->cnl_Line = AllocCopyString(TextLine);

	return cnl;
}


static void BackdropFreeChainedLine(struct ChainedLine *cnl)
{
	if (cnl->cnl_Line)
		FreeCopyString(cnl->cnl_Line);		// allocated by AllocCopyString()

	ScalosFree(cnl);
}


// rewrite ".backdrop" file
// <in> is only used to access the volume's root directory
BOOL RewriteBackdrop(struct ScaIconNode *in)
{
	BOOL Success = FALSE;
	BPTR rootDirLock;
	BPTR oldDir = CurrentDir(in->in_Lock);

	d1(kprintf("%s/%s/%ld: in=<%s>\n", __FILE__, __FUNC__, __LINE__, GetIconName(in)));

	rootDirLock = Lock(":", ACCESS_READ);
	debugLock_d1(rootDirLock);
	if (rootDirLock)
		{
		struct BackDropList bdl;

		CurrentDir(rootDirLock);

		d1(kprintf("%s/%s/%ld: rootDirLock OK\n", __FILE__, __FUNC__, __LINE__));

		if (RETURN_OK == BackdropLoadList(&bdl))
			{
			d1(kprintf("%s/%s/%ld: LoadBackdropFile OK - GO BackdropWriteList(&bdl)\n", __FILE__, __FUNC__, __LINE__));

			// Write .backdrop if it has any changes
			BackdropWriteList(&bdl);

			Success = TRUE;
			}

		BackdropFreeList(&bdl);

		UnLock(rootDirLock);
		}

	CurrentDir(oldDir);

	return Success;
}


// adjust ".backdrop" for changed name of object <oLock>
// <oLock> is Lock to object.
// <in> is the old backdrop icon
ULONG AdjustBackdropRenamed(BPTR oLock, struct ScaIconNode *in)
{
	CONST_STRPTR OldPathStart;
	STRPTR NewPathStart;
	STRPTR OldPath = NULL;
	STRPTR NewPath;
	ULONG Result;
	BPTR oldDir = NOT_A_LOCK;
	BPTR NewParentDir = (BPTR)NULL, rootLock = (BPTR)NULL;
	struct BackDropList bdl;
	struct ChainedLine *cnl;
	BOOL WasChanged = FALSE;

	d1(kprintf("%s/%s/%ld: oLock=%08lx  OldPath=<%s>\n", __FILE__, __FUNC__, __LINE__, oLock, OldPath));

	BackDropInitList(&bdl);

	do	{
		NewPath = AllocPathBuffer();
		if (NULL == NewPath)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		OldPath = AllocPathBuffer();
		if (NULL == OldPath)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		if (!NameFromLock(in->in_Lock, OldPath, Max_PathLen-1))
			{
			Result = IoErr();
			break;
			}

		if (!AddPart(OldPath, in->in_Name, Max_PathLen-1))
			{
			Result = IoErr();
			break;
			}

		NewParentDir = ParentDir(oLock);
		if ((BPTR)NULL == NewParentDir)
			{
			Result = IoErr();
			break;
			}

		if (!NameFromLock(oLock, NewPath, Max_PathLen-1))
			{
			Result = IoErr();
			break;
			}

		OldPathStart = strchr(OldPath, ':');
		if (NULL == OldPathStart)
			OldPathStart = OldPath;

		NewPathStart = strchr(NewPath, ':');
		if (NULL == NewPathStart)
			NewPathStart = NewPath;

		StripIconExtension(NewPath);

		d1(kprintf("%s/%s/%ld: OldPath=<%s>  NewPath=<%s>\n", __FILE__, __FUNC__, __LINE__, OldPath, NewPath));
		d1(kprintf("%s/%s/%ld: OldPathStart=<%s>  NewPathStart=<%s>\n", __FILE__, __FUNC__, __LINE__, OldPathStart, NewPathStart));

		oldDir = CurrentDir(NewParentDir);

		rootLock = Lock(":", ACCESS_READ);

		CurrentDir(oldDir);
		oldDir = NOT_A_LOCK;

		if ((BPTR)NULL == rootLock)
			{
			Result = IoErr();
			break;
			}

		oldDir = CurrentDir(rootLock);

		Result = BackdropLoadList(&bdl);
		if (RETURN_OK != Result)
			break;

		for (cnl = (struct ChainedLine *) bdl.bdl_LinesList.lh_Head;
			RETURN_OK == Result && cnl != (struct ChainedLine *) &bdl.bdl_LinesList.lh_Tail;
			cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

			if (0 == Stricmp((STRPTR) OldPathStart, cnl->cnl_Line))
				{
				// changed entry found
				FreeCopyString(cnl->cnl_Line);
				cnl->cnl_Line = AllocCopyString(NewPathStart);
				bdl.bdl_Changed = WasChanged = TRUE;
				}
			}

		BackdropWriteList(&bdl);
		} while (0);

	BackdropFreeList(&bdl);

	if (WasChanged)
		{
		struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
		struct ScaUpdateIcon_IW siw;
		struct ScaWindowStruct *sw;
		WORD x, y;

		siw.ui_iw_Lock = NewParentDir;
		siw.ui_iw_Name = FilePart(NewPath);
		//siw.ui_IconType = ICONTYPE_NONE;

		d1(KPrintF("%s/%s/%ld: Lock=%08lx  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, siw.ui_iw_Lock, siw.ui_iw_Name));

		x = gg->LeftEdge;
		y = gg->TopEdge;

		DoMethod(iwtMain->iwt_WindowTask.mt_MainObject, SCCM_DeviceWin_RemIcon, in);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		AddBackdropIcon(siw.ui_iw_Lock, siw.ui_iw_Name, x, y);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// remove icon from owning drawer window
		// search owning drawer window by Lock
		SCA_LockWindowList(SCA_LockWindowList_Shared);

		for (sw=winlist.wl_WindowStruct; sw; sw = (struct ScaWindowStruct *) sw->ws_Node.mln_Succ)
			{
			if (sw->ws_Lock && 
				LOCK_SAME == ScaSameLock(sw->ws_Lock, siw.ui_iw_Lock))
				{
				// owning drawer window found
				struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) sw->ws_WindowTask;

				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_RemIcon,
					siw.ui_iw_Lock,
                                        siw.ui_iw_Name);

				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				}
			}
		SCA_UnLockWindowList();
		}

	if (OldPath)
		FreePathBuffer(OldPath);
	if (NewPath)
		FreePathBuffer(NewPath);
	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (NewParentDir)
		UnLock(NewParentDir);
	if (rootLock)
		UnLock(rootLock);

	return Result;
}


struct ScaIconNode *AddBackdropIcon(BPTR iconDirLock, CONST_STRPTR iconName, WORD PosX, WORD PosY)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	struct ScaIconNode *inMain;
	struct DevProc *devproc;
	BOOL IconAlreadyPresent = FALSE;

	d1(KPrintF("%s/%s/%ld: START iconName=<%s>  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, iconName, PosX, PosY));
	debugLock_d1(iconDirLock);


	ScalosLockIconListExclusive(iwtMain);

	for (inMain = iwtMain->iwt_WindowTask.wt_IconList; inMain; inMain=(struct ScaIconNode *) inMain->in_Node.mln_Succ)
		{
		d1(KPrintF("%s/%s/%ld: in_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, inMain->in_Name));

		if (inMain->in_Lock && (LOCK_SAME == SameLock(inMain->in_Lock, iconDirLock))
			&& inMain->in_Name && 0 == Stricmp((STRPTR) iconName, inMain->in_Name))
			{
			IconAlreadyPresent = TRUE;
			d1(KPrintF("%s/%s/%ld: iconName=<%> inMain->in_Name=<%s> IconAlreadyPresent=[%ld]\n", __FILE__, __FUNC__, __LINE__, \
					iconName, inMain->in_Name, IconAlreadyPresent));
			}
		}

	ScalosUnLockIconList(iwtMain);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	// Abort here if we already have a backdrop icon with same lock and name
	if (IconAlreadyPresent)
		{
		d1(KPrintF("%s/%s/%ld: icon already present: iconName=<%s>\n", __FILE__, __FUNC__, __LINE__, iconName));
		return NULL;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	// !! PosX and PosY have to be concatenated from 2 WORD into 1 LONG parameter !!

	// add icon to desktop window
	inMain = (struct ScaIconNode *) DoMethod(iwtMain->iwt_WindowTask.mt_MainObject,
		SCCM_IconWin_AddIcon, 
		SCCM_ADDICON_MAKEXY(PosX, PosY),
		iconDirLock,
		iconName);

	d1(kprintf("%s/%s/%ld: iconName=<%> PosX=%ld  PosY=%ld  pos=%08lx\n", __FILE__, __FUNC__, __LINE__, iconName, PosX, PosY, (((UWORD) PosX) << 16) | ((UWORD) PosY)));

	devproc = GetDeviceProc(":", NULL);
	if (NULL == devproc)
		{
		d1(KPrintF("%s/%s/%ld: GetDeviceProc failed iconName=<%s>\n", __FILE__, __FUNC__, __LINE__, iconName));
		return NULL;
		}

	ScalosLockIconListExclusive(iwtMain);

	d1(kprintf("%s/%s/%ld: inMain=%08lx\n", __FILE__, __FUNC__, __LINE__, inMain));
	d1(kprintf("%s/%s/%ld: devproc=%08lx  Port=%08lx\n", __FILE__, __FUNC__, __LINE__, devproc, devproc->dvp_Port));
	debugLock_d1(iconDirLock);

	if (inMain)
		{
		BOOL VolumeIsWritable;

		d1(kprintf("%s/%s/%ld: inMain=%08lx  in_Icon=%08lx  in_Lock=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, inMain, inMain->in_Icon, inMain->in_Lock));

		VolumeIsWritable = ClassIsDiskWritable(iconDirLock);

		d1(kprintf("%s/%s/%ld: VolumeIsWritable=%ld\n", __FILE__, __FUNC__, __LINE__, VolumeIsWritable));
#if 0
		// Do not set ICONOVERLAYF_LeftOut here, icon was perhaps just dropped!
		// Check if is a real permanent backdrop icon before.
		SetAttrs(inMain->in_Icon,
			IDTA_OverlayType, ICONOVERLAYF_LeftOut,
			TAG_END);
#endif
		if (VolumeIsWritable)
			{
			struct BackDropList bdl;

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			// Check if is a real permanent backdrop icon before to set ICONOVERLAYF_LeftOut Overlay type flag.
			BackDropInitList(&bdl);

			
			if (IsPermanentBackDropIcon(iwtMain, &bdl, iconDirLock, GetIconName(inMain)))
				{
				d1(KPrintF("%s/%s/%ld: PermanentBackDropIcon=<%s>\n", __FILE__, __FUNC__, __LINE__, GetIconName(inMain)));

				SetAttrs(inMain->in_Icon,
					IDTA_OverlayType, ICONOVERLAYF_LeftOut,
					TAG_END);

				inMain->in_SupportFlags |= INF_SupportsPutAway;
				inMain->in_SupportFlags &= ~INF_SupportsLeaveOut;

				d1(KPrintF("%s/%s/%ld: Permanent BackDrop Icon=<%s> ICONOVERLAYF_LeftOut is SET - Icon Supportflags UPDATED!\n", \
						__FILE__, __FUNC__, __LINE__, GetIconName(inMain)));

				BackdropRedrawMainIconObject(iwtMain, inMain->in_Icon);
				}
			else
				{
				d1(kprintf("%s/%s/%ld: Not Permanent BackDrop Icon=<%s> ICONOVERLAYF_LeftOut NOT SET - Icon Supportflags UPDATED!\n",\
						 __FILE__, __FUNC__, __LINE__, GetIconName(inMain)));

				inMain->in_SupportFlags &= ~INF_SupportsPutAway;
				inMain->in_SupportFlags |= INF_SupportsLeaveOut;
				}

			BackdropFreeList(&bdl);
			}
		else
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			inMain->in_SupportFlags &= ~INF_SupportsPutAway;
			inMain->in_SupportFlags &= ~INF_SupportsLeaveOut;
			}

		CreateSbiForIcon(inMain);
		}

	ScalosUnLockIconList(iwtMain);

	FreeDeviceProc(devproc);

	d1(kprintf("%s/%s/%ld: END  inMain=%08lx\n", __FILE__, __FUNC__, __LINE__, inMain));

	return inMain;
}


struct ScaIconNode *FindBackdropIcon(BPTR dirLock, CONST_STRPTR iconName)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	struct ScaIconNode *in, *inFound = NULL;

	if (NULL == iwtMain)
		{
		d1(kprintf("%s/%s/%ld: iwtMain=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtMain));
		return inFound;
		}

	d1(KPrintF("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtMain));

	ScalosLockIconListShared(iwtMain);

	for (in=iwtMain->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (in->in_Lock &&
			LOCK_SAME == ScaSameLock(in->in_Lock, dirLock) &&
			0 == Stricmp(iconName, GetIconName(in)))
			{
			d1(kprintf("%s/%s/%ld: iwtMain=%08lx  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwtMain, in, GetIconName(in)));
			inFound = in;
			break;
			}
		}

	if (NULL == inFound)
		ScalosUnLockIconList(iwtMain);

	d1(kprintf("%s/%s/%ld: inFound=%08lx\n", __FILE__, __FUNC__, __LINE__, inFound));

	return inFound;
}


struct ScaIconNode *FindBackdropIconExclusive(BPTR dirLock, CONST_STRPTR iconName)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	struct ScaIconNode *in, *inFound = NULL;

	if (NULL == iwtMain)
		{
		d1(kprintf("%s/%s/%ld: iwtMain=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtMain));
		return inFound;
		}

	d1(KPrintF("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtMain));

	ScalosLockIconListExclusive(iwtMain);

	for (in=iwtMain->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (LOCK_SAME == ScaSameLock(in->in_Lock, dirLock) &&
			0 == Stricmp(iconName, GetIconName(in)))
			{
			d1(kprintf("%s/%s/%ld: iwtMain=%08lx  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwtMain, in, GetIconName(in)));
			inFound = in;
			break;
			}
		}

	if (NULL == inFound)
		ScalosUnLockIconList(iwtMain);

	d1(kprintf("%s/%s/%ld: inFound=%08lx\n", __FILE__, __FUNC__, __LINE__, inFound));

	return inFound;
}


static void BackdropRemoveObsoleteEntriesFromBackdropFile(struct BackDropList *bdl)
{
	char *IconPath;
	struct ChainedLine *cnl;

	IconPath = AllocPathBuffer();
	if (NULL == IconPath)
		return;

	for (cnl = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
		cnl != (struct ChainedLine *) &bdl->bdl_LinesList.lh_Tail; )
		{
		struct ChainedLine *cnlNext = (struct ChainedLine *) cnl->cnl_Node.ln_Succ;
		BPTR fLock;
		LONG LockResult;

		d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

		// clean up .backdrop
		// check if icon exists that this .backdrop line refers to 
		stccpy(IconPath, cnl->cnl_Line, Max_PathLen - 1);
		SafeStrCat(IconPath, ".info", Max_PathLen - 1);

		d1(kprintf("%s/%s/%ld: IconPath=<%s>\n", __FILE__, __FUNC__, __LINE__, IconPath));

		fLock = Lock(IconPath, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			{
			LockResult = IoErr();
			}
		else
			{
			LockResult = RETURN_OK;
			UnLock(fLock);
			}

		d1(kprintf("%s/%s/%ld: LockResult=%ld\n", __FILE__, __FUNC__, __LINE__, LockResult));

		if (ERROR_OBJECT_NOT_FOUND == LockResult)
			{
			d1(kprintf("%s/%s/%ld: LockResult=%ld - GO Remove <%s> \n", __FILE__, __FUNC__, __LINE__, LockResult, cnl->cnl_Line));
			Remove(&cnl->cnl_Node);
			BackdropFreeChainedLine(cnl);
			bdl->bdl_Changed = TRUE;
			}

		cnl = cnlNext;
		}

	FreePathBuffer(IconPath);
}

//----------------------------------------------------------------------------

static void BackdropRemoveDuplicateEntriesFromBackdropFile(struct BackDropList *bdl)
{
	char *IconPath;
	struct ChainedLine *cnl;

	IconPath = AllocPathBuffer();
	if (NULL == IconPath)
		return;

	for (cnl = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
		cnl != (struct ChainedLine *) &bdl->bdl_LinesList.lh_Tail;
		cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ )
		{
		ULONG Removed = 0;
		struct ChainedLine *cnl2;

		d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

		for (cnl2 = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
			cnl2 != (struct ChainedLine *) &bdl->bdl_LinesList.lh_Tail; )
			{
			struct ChainedLine *cnl2Next = (struct ChainedLine *) cnl2->cnl_Node.ln_Succ;

			if ((cnl2 != cnl) && (0 == Stricmp(cnl2->cnl_Line, cnl->cnl_Line)) )
				{
				// Remove Duplicate line
				d1(kprintf("%s/%s/%ld: LockResult=%ld - GO Remove <%s> \n", __FILE__, __FUNC__, __LINE__, LockResult, cnl->cnl_Line));
				Remove(&cnl2->cnl_Node);
				BackdropFreeChainedLine(cnl2);
				bdl->bdl_Changed = TRUE;
				Removed++;
				}

			cnl2 = cnl2Next;
			}
		if (Removed)
			cnl = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
		}

	FreePathBuffer(IconPath);
}

//----------------------------------------------------------------------------
void DoLeaveOutIcon(struct internalScaWindowTask *iwt, BPTR DirLock,
	CONST_STRPTR IconName, WORD x, WORD y)
{
	struct BackDropList bdl;
	BPTR oldDir = NOT_A_LOCK;
	BPTR rootLock = (BPTR)NULL;
	char *lp, *DirName = NULL;

	d1(KPrintF("%s/%s/%ld: START Icon=<%s>  Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, IconName, DirLock));
	debugLock_d1(DirLock);

	BackDropInitList(&bdl);

	do	{
		if (IsPermanentBackDropIcon(iwt, &bdl, DirLock, IconName))
			break;	// this is already a backdrop icon

		d1(KPrintF("%s/%s/%ld: ws_Lock=%08lx  DirLock=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, DirLock));

		DirName = AllocPathBuffer();
		if (NULL == DirName)
			break;

		d1(KPrintF("%s/%s/%ld: DirName=%08lx\n", __FILE__, __FUNC__, __LINE__, DirName));

		if (!NameFromLock(DirLock, DirName, Max_PathLen - 1))
			break;

		d1(KPrintF("%s/%s/%ld: DirName=<%s>\n", __FILE__, __FUNC__, __LINE__, DirName));

		if (!AddPart(DirName, (STRPTR) IconName, Max_PathLen - 1))
			break;

		d1(KPrintF("%s/%s/%ld: DirName = <%s>\n", __FILE__, __FUNC__, __LINE__, DirName));

		oldDir = CurrentDir(DirLock);

		rootLock = Lock(":", ACCESS_READ);
		if ((BPTR)NULL == rootLock)
			break;

		CurrentDir(oldDir);
		oldDir = NOT_A_LOCK;
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if ((BPTR)NULL == rootLock)
			break;

		oldDir = CurrentDir(rootLock);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (RETURN_OK != BackdropLoadList(&bdl))
			break;

		lp = strchr(DirName, ':');
		if (NULL == lp)
			lp = DirName;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (!BackdropAddLine(&bdl, lp, x, y))
			break;

		ShortcutAddEntry(DirName, x, y);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		BackdropWriteList(&bdl);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
			{
			d1(KPrintF("%s/%s/%ld: AddBackdropIcon(%08lx, %s, x, y)\n", __FILE__, __FUNC__, __LINE__, \
					iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, IconName));

			AddBackdropIcon(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
				IconName, x, y);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			// Special handling for "view all" textwindows.
			// here both object and icon are separate entries, which must be removed both!
			if (!IsIwtViewByIcon(iwt) &&
				IsShowAll(iwt->iwt_WindowTask.mt_WindowStruct))
				{
				STRPTR IconInfoName = ScalosAlloc(1 + strlen(IconName) + strlen(".info"));

				if (IconInfoName)
					{
					strcpy(IconInfoName, IconName);
					strcat(IconInfoName, ".info");

					// remove associated ".info" icon from owning drawer window
					DoMethod(iwt->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_RemIcon,
						iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
                                                IconInfoName);
					}
				}

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			// remove icon from owning drawer window
			DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_RemIcon,
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
                                IconName);
			}
		else
			{
			// For icon dropped on desktop:
			// If ScaIconNode lock and name = DirLock and IconName then
			// update INF_SupportsPutAway and INF_SupportsLeaveOut flags.

			struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
			struct ScaIconNode *in;

			ScalosLockIconListShared(iwtMain);

			for (in = iwtMain->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				if (LOCK_SAME == ScaSameLock(in->in_Lock, DirLock) &&
					0 == Stricmp(IconName, GetIconName(in)))
					{
					d1(KPrintF("%s/%s/%ld: in_Lock=%08lx DirLock=%08lx in_Name=<%s>  IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
							in->in_Lock, DirLock, GetIconName(in), IconName));

					CurrentDir(oldDir);
					oldDir = CurrentDir(DirLock);
					in->in_SupportFlags |= INF_SupportsPutAway;
					in->in_SupportFlags &= ~INF_SupportsLeaveOut;

					SetAttrs(in->in_Icon,
						IDTA_OverlayType, ICONOVERLAYF_LeftOut,
						TAG_END);

					d1(KPrintF("%s/%s/%ld: ICONOVERLAYF_LeftOut is SET IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, GetIconName(in)));
					BackdropRedrawMainIconObject(iwtMain, in->in_Icon);
					break;
					}
				}

			ScalosUnLockIconList(iwtMain);
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	BackdropFreeList(&bdl);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (rootLock)
		UnLock(rootLock);
	if (DirName)
		FreePathBuffer(DirName);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

BOOL IsPermanentBackDropIcon(struct internalScaWindowTask *iwt, struct BackDropList *bdl,
	BPTR fLock, CONST_STRPTR FileName)
{
	BOOL IsBackdrop = FALSE;

	debugLock_d1(fLock);
	d1(KPrintF("%s/%s/%ld: bdl=%08lx  Loaded=%ld  FileName=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, bdl, bdl->bdl_Loaded, FileName));

	if (!bdl->bdl_Loaded && !bdl->bdl_NotPresent)
		{
		BPTR oldDir;
		BPTR DirLock;

		do	{
			oldDir = CurrentDir(fLock);

			DirLock = Lock(":", ACCESS_READ);
			debugLock_d1(DirLock);
			if ((BPTR)NULL == DirLock)
				break;

			// change to root directory of icon drawer
			CurrentDir(DirLock);

			if (RETURN_OK != BackdropLoadList(bdl))
				break;
			} while (0);

		CurrentDir(oldDir);
		if (DirLock)
			UnLock(DirLock);
		}

	d1(kprintf("%s/%s/%ld: bdl=%08lx  Loaded=%ld  NotPresent=%ld  bdl_Filtered=%ld\n", \
		__FILE__, __FUNC__, __LINE__, bdl, bdl->bdl_Loaded, bdl->bdl_NotPresent, bdl->bdl_Filtered));

	if (bdl->bdl_Loaded)
		{
		if (bdl->bdl_Filtered)
			{
			struct ChainedLine *cnl;

			for (cnl = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
				cnl != (struct ChainedLine *) &bdl->bdl_LinesList.lh_Tail;
				cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ)
				{
				STRPTR lp = FilePart(cnl->cnl_Line);

				d1(kprintf("%s/%s/%ld: lp=<%s>\n", __FILE__, __FUNC__, __LINE__, lp));

				if (0 == Stricmp(FileName, lp))
					{
					// entry found
					d1(kprintf("%s/%s/%ld: Line=<%s> IsBackdrop=[ %ld ]\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line, IsBackdrop));
					IsBackdrop = TRUE;
					break;
					}
				}
			}
		else
			{
			char *IconPath;
			struct ChainedLine *cnl;
			STRPTR PathStart;

			do	{
				IconPath = AllocPathBuffer();
				if (NULL == IconPath)
					break;

				if (!NameFromLock(fLock, IconPath, Max_PathLen-1))
					break;

				if (!AddPart(IconPath, (STRPTR) FileName, Max_PathLen-1))
					break;

				d1(kprintf("%s/%s/%ld: IconPath=<%s>\n", __FILE__, __FUNC__, __LINE__, IconPath));

				PathStart = strchr(IconPath, ':');
				if (NULL == PathStart)
					PathStart = IconPath;

				for (cnl = (struct ChainedLine *) bdl->bdl_LinesList.lh_Head;
					cnl != (struct ChainedLine *) &bdl->bdl_LinesList.lh_Tail;
					cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ)
					{
					d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));

					if (0 == Stricmp((STRPTR) PathStart, cnl->cnl_Line))
						{
						// entry found
						d1(kprintf("%s/%s/%ld: Line=<%s> IsBackdrop=[ %ld ]\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line, IsBackdrop));
						IsBackdrop = TRUE;
						break;
						}
					}
				} while (0);

			if (IconPath)
				FreePathBuffer(IconPath);
			}
		}

	d1(KPrintF("%s/%s/%ld: IsBackdrop=%ld\n", __FILE__, __FUNC__, __LINE__, IsBackdrop));

	return IsBackdrop;
}

//----------------------------------------------------------------------------

void PutAwayIcon(struct internalScaWindowTask *iwt,
	BPTR iconDirLock, CONST_STRPTR IconName, BOOL RemovePermanent)
{
	BPTR oldDir;
	BPTR iconLock = (BPTR)NULL;
	BPTR rootLock = (BPTR)NULL;
	struct BackDropList bdl;
	STRPTR Path = NULL;
	struct ScaUpdateIcon_IW UpdateIcon = { (BPTR)NULL, NULL, ICONTYPE_NONE };

	BackDropInitList(&bdl);

	d1(KPrintF("%s/%s/%ld: Icon=<%s>\n", __FILE__, __FUNC__, __LINE__, IconName));
	debugLock_d1(iconDirLock);

	do	{
		debugLock_d1(iconDirLock);

		oldDir = CurrentDir(iconDirLock);

		if (!IsPermanentBackDropIcon(iwt, &bdl, iconDirLock, IconName))
			{
			// +jmc+ : This is not a backdrop icon(icon dropped) then remove object from main window.
			if (iwt->iwt_WindowTask.mt_WindowStruct == iInfos.xii_iinfos.ii_MainWindowStruct)
				{
				struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
				struct ScaIconNode *in;

				UpdateIcon.ui_iw_Name = AllocCopyString(IconName);

				ScalosLockIconListExclusive(iwtMain);
				for (in = iwtMain->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					if (in->in_Lock
						&& LOCK_SAME == SameLock(in->in_Lock, iconDirLock)
						&& 0 == Stricmp(IconName, GetIconName(in)))
						{
						DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_DeviceWin_RemIcon, in);
						d1(KPrintF("%s/%s/%ld: Not Permanent BackDrop Icon=<%s> removed from desktop\n", __FILE__, __FUNC__, __LINE__, IconName));
						break;
						}
					}
				ScalosUnLockIconList(iwtMain);
				}
			break;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		Path = AllocPathBuffer();
		if (NULL == Path)
			break;

		stccpy(Path, IconName, Max_PathLen);
		SafeStrCat(Path, ".info", Max_PathLen);

		iconLock = Lock(Path, ACCESS_READ);
		d1(KPrintF("%s/%s/%ld: IconName=<%s>  iconLock=%08lx\n", __FILE__, __FUNC__, __LINE__, IconName, iconLock));
		if ((BPTR)NULL == iconLock)
			break;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (RemovePermanent)
			{
			rootLock = Lock(":", ACCESS_READ);
			if ((BPTR)NULL == rootLock)
				break;

			CurrentDir(oldDir);

			oldDir = CurrentDir(rootLock);

			BackdropRemoveLine(&bdl, iconLock);

			ShortcutRemoveEntry(iconLock);

			CurrentDir(rootLock);

			if (bdl.bdl_Changed)
				BackdropWriteList(&bdl);
			}

		if (iwt->iwt_WindowTask.mt_WindowStruct == iInfos.xii_iinfos.ii_MainWindowStruct)
			{
			struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
			struct ScaIconNode *in;

			UpdateIcon.ui_iw_Name = AllocCopyString(IconName);

			// remove icon from main window
			ScalosLockIconListExclusive(iwtMain);
			for (in = iwtMain->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				if (in->in_Lock
					&& LOCK_SAME == SameLock(in->in_Lock, iconDirLock)
					&& 0 == Stricmp(IconName, GetIconName(in)))
					{
					DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_DeviceWin_RemIcon, in);
					d1(KPrintF("%s/%s/%ld: Permanent BackDrop Icon=<%s> removed from desktop - Icon SupportsFalgs UPDATED!\n", \
							 __FILE__, __FUNC__, __LINE__, IconName));
					break;
					}
				}
			ScalosUnLockIconList(iwtMain);
			}

		d1(KPrintF("%s/%s/%ld: Changed=%ld\n", __FILE__, __FUNC__, __LINE__, bdl.bdl_Changed));
		} while (0);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	BackdropFreeList(&bdl);

	if (Path)
		FreePathBuffer(Path);
	if (UpdateIcon.ui_iw_Name)
		FreeCopyString(UpdateIcon.ui_iw_Name);
	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (UpdateIcon.ui_iw_Lock)
		UnLock(UpdateIcon.ui_iw_Lock);
	if (iconLock)
		UnLock(iconLock);
	if (rootLock)
		UnLock(rootLock);
}


static void BackdropRedrawMainIconObject(struct internalScaWindowTask *iwtMain, Object *IconObj)
{
	struct SM_RedrawIconObj *sMsg;

	sMsg = (struct SM_RedrawIconObj *) SCA_AllocMessage(MTYP_RedrawIconObj, 0);
	d1(KPrintF("%s/%s/%ld: sMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, sMsg));
	if (sMsg)
		{
		sMsg->smrio_IconObject = IconObj;
		sMsg->smrio_Flags = SMRIOFLAGF_IconListLocked;
		PutMsg(iwtMain->iwt_WindowTask.wt_IconPort,
			&sMsg->ScalosMessage.sm_Message);
		}
}


// Wait up to 2 seconds for completion of backdrop icon generation on desktop
BOOL BackdropWait(BPTR dirLock)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	struct ScaIconNode *in;
	BOOL Found = FALSE;
	ULONG n = 20;

	if (NULL == iwtMain)
		{
		d1(kprintf("%s/%s/%ld: iwtMain=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtMain));
		return Found;
		}
	if (BNULL == dirLock)
		{
		d1(kprintf("%s/%s/%ld: dirLock=0\n", __FILE__, __FUNC__, __LINE__));
		return Found;
		}

	d1(KPrintF("%s/%s/%ld: START iwtMain=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtMain));

	do	{
		ScalosLockIconListShared(iwtMain);

		for (in=iwtMain->iwt_WindowTask.wt_IconList; in && !Found; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (in->in_DeviceIcon)
				{
				BPTR DeviceIconLock = DiskInfoLock(in);

				if (DeviceIconLock)
					{
					LONG IsSameLock;

					IsSameLock = ScaSameLock(DeviceIconLock, dirLock);

					if (LOCK_SAME_VOLUME == IsSameLock || LOCK_SAME == IsSameLock)
						{
						d1(kprintf("%s/%s/%ld: Found Device Icon in=<%s>  di_Flags=%08lx\n", \
							__FILE__, __FUNC__, __LINE__, GetIconName(in), \
							in->in_DeviceIcon->di_Flags));
						Found = in->in_DeviceIcon->di_Flags & DIBF_BackdropReadComplete;
						}

					UnLock(DeviceIconLock);
					}
				}
			}

		ScalosUnLockIconList(iwtMain);

		if (!Found)
			Delay(5);	// wait 100 ms

		} while (!Found && (n-- > 0));


	d1(kprintf("%s/%s/%ld: END Found=%ld\n", __FILE__, __FUNC__, __LINE__, Found));

	return Found;
}



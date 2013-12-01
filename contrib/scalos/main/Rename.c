// Rename.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

struct RenamedHistoryPath
	{
	struct Node rhp_Node;
	struct internalScaWindowTask *rhp_WindowTask;
	char rhp_OldName[1];	// variable length
	};

//----------------------------------------------------------------------------

// local functions

//----------------------------------------------------------------------------

BOOL CollectRenameAdjustHistoryPaths(struct List *HList, BPTR fLock)
{
	BOOL WinListLocked = TRUE;
	struct ScaWindowStruct *ws;

	// Window list stays locked until CleanupRenameAdjustHistoryPaths
	SCA_LockWindowList(SCA_LockWindowList_Shared);

	d1(kprintf("%s/%s/%ld:START\n", __FILE__, __FUNC__, __LINE__));
	debugLock_d1(fLock);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
		struct WindowHistoryEntry *whe;

		ScalosObtainSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

		// Walk through list of WindowHistoryEntry
		for (whe = (struct WindowHistoryEntry *) iwt->iwt_HistoryList.lh_Head;
			whe != (struct WindowHistoryEntry *) &iwt->iwt_HistoryList.lh_Tail;
			whe = (struct WindowHistoryEntry *) whe->whe_Node.ln_Succ)
			{
			BPTR pLock;

			d1(kprintf("%s/%s/%ld: whe_Path=<%s>\n", __FILE__, __FUNC__, __LINE__, whe->whe_Path));
			pLock = Lock(whe->whe_Path, ACCESS_READ);
			debugLock_d1(pLock);
			if (pLock)
				{
				if (LOCK_SAME == SameLock(pLock, fLock))
					{
					struct RenamedHistoryPath *rhp;

					d1(KPrintF("%s/%s/%ld: iwt=%08lx  ln_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, whe->whe_Node.ln_Name));

					rhp = ScalosAlloc(sizeof(struct RenamedHistoryPath) + strlen(whe->whe_Node.ln_Name));
					if (rhp)
						{
						rhp->rhp_WindowTask = iwt;
						strcpy(rhp->rhp_OldName, whe->whe_Node.ln_Name);

						AddTail(HList, &rhp->rhp_Node);
						}
					}
				UnLock(pLock);
				}
			}
		ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
		}

	d1(kprintf("%s/%s/%ld:END  WinListLocked=%ld\n", __FILE__, __FUNC__, __LINE__, WinListLocked));

	return WinListLocked;
}

//----------------------------------------------------------------------------

void AdjustRenameAdjustHistoryPaths(struct List *HList, BPTR fLock)
{
	STRPTR FullPath;

	d1(kprintf("%s/%s/%ld:START\n", __FILE__, __FUNC__, __LINE__));
	debugLock_d1(fLock);

	FullPath = AllocPathBuffer();
	d1(KPrintF("%s/%s/%ld: FullPath=%08lx\n", __FILE__, __FUNC__, __LINE__, FullPath));
	if (FullPath)
		{
		struct RenamedHistoryPath *rhp;

		if (NameFromLock(fLock, FullPath, Max_PathLen - 1))
			{
			d1(KPrintF("%s/%s/%ld: FullPath=<%s>\n", __FILE__, __FUNC__, __LINE__, FullPath));

			while ((rhp = (struct RenamedHistoryPath *) RemHead(HList)))
				{
				struct internalScaWindowTask *iwt = rhp->rhp_WindowTask;
				struct WindowHistoryEntry *whe;

				d1(kprintf("%s/%s/%ld: rhp_OldName=<%s>\n", __FILE__, __FUNC__, __LINE__, rhp->rhp_OldName));

				ScalosObtainSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

				// Walk through list of WindowHistoryEntry
				for (whe = (struct WindowHistoryEntry *) iwt->iwt_HistoryList.lh_Head;
					whe != (struct WindowHistoryEntry *) &iwt->iwt_HistoryList.lh_Tail;
					whe = (struct WindowHistoryEntry *) whe->whe_Node.ln_Succ)
					{
					d1(kprintf("%s/%s/%ld: rhp_OldName=<%s>  ln_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, rhp->rhp_OldName, whe->whe_Node.ln_Name));

					if (0 == Stricmp(rhp->rhp_OldName, whe->whe_Node.ln_Name))
						{
						struct WindowHistoryEntry *wheNew;

						wheNew = ScalosAlloc(sizeof(struct WindowHistoryEntry) + strlen(FullPath));
						d1(KPrintF("%s/%s/%ld: wheNew=%08lx\n", __FILE__, __FUNC__, __LINE__, wheNew));
						if (wheNew)
							{
							*wheNew = *whe;
							wheNew->whe_Node.ln_Name = wheNew->whe_Path;
							strcpy(wheNew->whe_Node.ln_Name, FullPath);

							// insert new WindowHistoryEntry after old one
							Insert(&iwt->iwt_HistoryList, &wheNew->whe_Node, &whe->whe_Node);
							}

						Remove(&whe->whe_Node);

						break;
						}
					}
				ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
				}
			}

		ScalosFree(FullPath);
		}
	d1(kprintf("%s/%s/%ld:END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

void CleanupRenameAdjustHistoryPaths(struct List *HList, BOOL WinListLocked)
{
	struct RenamedHistoryPath *rhp;

	if (WinListLocked)
		SCA_UnLockWindowList();

	while ((rhp = (struct RenamedHistoryPath *) RemHead(HList)))
		{
		ScalosFree(rhp);
		}
}

//----------------------------------------------------------------------------


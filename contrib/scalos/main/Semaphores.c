// Semaphores.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <utility/hooks.h>
#include <dos/dos.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <proto/utility.h>

#include <defs.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local functions

//----------------------------------------------------------------------------

// public Data

struct ScalosSemaphoreList GlobalSemaphoreList[] =
	{
	{ &WBStartListSema,		"WBStartListSema"		},
	{ &LayersSema,			"LayersSema"			},
	{ &MenuSema,			"MenuSema"			},
	{ &CopyHookSemaphore,		"CopyHookSemaphore"		},
	{ &DefIconsSemaphore,		"DefIconsSemaphore"		},
	{ &DefIconsCacheSemaphore,	"DefIconsCacheSemaphore"	},
	{ &DeleteHookSemaphore,		"DeleteHookSemaphore"		},
	{ &FileTypeListSema,		"FileTypeListSema"		},
	{ &TextInputHookSemaphore,	"TextInputHookSemaphore"	},
	{ &QuitSemaphore,		"QuitSemaphore"			},
	{ &CloseWBHookListSemaphore,	"CloseWBHookListSemaphore"	},
	{ &ClassListSemaphore,		"ClassListSemaphore"		},
	{ &WinListSemaphore,		"WinListSemaphore"		},
	{ &DoWaitSemaphore,		"DoWaitSemaphore"		},
	{ &PatternSema,			"PatternSema"			},
	{ &ParseMenuListSemaphore,	"ParseMenuListSemaphore"	},
	{ &ListenerSema,		"ListenerSema"			},
	{ &MenuSemaphore,		"MenuSemaphore"			},
	{ &AppMenuListSemaphore,	"AppMenuListSemaphore"		},
	{ &ThumbnailsCleanupSemaphore,	"ThumbnailsCleanupSemaphore"	},
	{ &tteSema,			"tteSema"			},
	{ &ClipboardSemaphore,		"ClipboardSemaphore"		},
	{ &UndoListListSemaphore,	"UndoListListSemaphore"		},
	{ &RedoListListSemaphore,	"RedoListListSemaphore"		},
	{ NULL, NULL }
	};

//----------------------------------------------------------------------------

// local Data

//----------------------------------------------------------------------------

// +jl+ 20010915
// extension to ScalosAttemptSemaphore()
// - prevents nesting when called more than once from same task
BOOL AttemptSemaphoreNoNest(SCALOSSEMAPHORE *sema)
{
	if (!ScalosAttemptSemaphore(sema))
		return FALSE;

	if (sema->Sema.ss_NestCount > 1)
		{
		ScalosReleaseSemaphore(sema);
		return FALSE;
		}

	return TRUE;
}


// Try to obtain multiple Semaphores without deadlock
// advantages over system ObtainSemaphoreList() :
// - supports both exclusive and shared Semaphore access.
// - does not use Semaphore Node data.
#if !defined(ScaObtainSemaphoreList)
void ScaObtainSemaphoreList(ULONG FirstTag, ...)
{
	va_list args;
	struct TagItem *TagList;

	va_start(args, FirstTag);

	TagList = ScalosVTagList(FirstTag, args);
	if (TagList)
		{
		ScaObtainSemaphoreListA(TagList);
		FreeTagItems(TagList);
		}

	va_end(args);
}
#endif /* ScaObtainSemaphoreList */

void ScaObtainSemaphoreListA(struct TagItem *OrigTagList)
{
	struct TagItem *Tag;
	BOOL GlobalSuccess;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct TagItem *TagList = OrigTagList;

		GlobalSuccess = TRUE;

		d1(kprintf("%s/%s/%ld: Loop START\n", __FILE__, __FUNC__, __LINE__));

		Forbid();
		while (GlobalSuccess && (Tag = NextTagItem(&TagList)))
			{
			BOOL Success = TRUE;

			d1(KPrintF("%s/%s/%ld: Tag=%08lx  ti_Tag=%08lx  Sema=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag, Tag->ti_Tag, Tag->ti_Data));

			switch (Tag->ti_Tag)
				{
			case SCA_SemaphoreExclusive:
				Success = ScalosAttemptSemaphore((SCALOSSEMAPHORE *) Tag->ti_Data);
				break;

			case SCA_SemaphoreShared:
				Success = ScalosAttemptSemaphoreShared((SCALOSSEMAPHORE *) Tag->ti_Data);
				break;
				}

			d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

			if (!Success)
				{
				struct TagItem *Tag2, *TagList2 = OrigTagList;

				GlobalSuccess = FALSE;

				// Release all previously obtained semaphores
				while ((Tag2 = NextTagItem(&TagList2)) && Tag2 != Tag)
					{
					d1(KPrintF("%s/%s/%ld: Tag2=%08lx  ti_Tag=%08lx  Sema=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag2, Tag2->ti_Tag, Tag2->ti_Data));
					ScalosReleaseSemaphore((SCALOSSEMAPHORE *) Tag2->ti_Data);
					}

				// wait until Semaphore available
				d1(kprintf("%s/%s/%ld: before Wait\n", __FILE__, __FUNC__, __LINE__));
				switch (Tag->ti_Tag)
					{
				case SCA_SemaphoreExclusive:
					ScalosObtainSemaphore((SCALOSSEMAPHORE *) Tag->ti_Data); // breaks Forbid()
					break;
				case SCA_SemaphoreShared:
					ScalosObtainSemaphoreShared((SCALOSSEMAPHORE *) Tag->ti_Data);	 // breaks Forbid()
					break;
					}

				d1(KPrintF("%s/%s/%ld: after Wait\n", __FILE__, __FUNC__, __LINE__));

				Permit();
				Forbid();
				ScalosReleaseSemaphore((SCALOSSEMAPHORE *) Tag->ti_Data);
				}
			}
		Permit();
		d1(KPrintF("%s/%s/%ld: GlobalSuccess=%ld\n", __FILE__, __FUNC__, __LINE__, GlobalSuccess));
		} while (!GlobalSuccess);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


// Try to obtain multiple Semaphores without deadlock
// advantages over system ObtainSemaphoreList() :
// - supports both exclusive and shared Semaphore access.
// - does not use Semaphore Node data.
#if !defined(ScaAttemptSemaphoreList)
BOOL ScaAttemptSemaphoreList(ULONG FirstTag, ...)
{
	va_list args;
	struct TagItem *TagList;
	BOOL Success = FALSE;

	va_start(args, FirstTag);

	TagList = ScalosVTagList(FirstTag, args);
	if (TagList)
		{
		Success = ScaAttemptSemaphoreListA(TagList);
		FreeTagItems(TagList);
		}

	va_end(args);

	return Success;
}
#endif /* ScaAttemptSemaphoreListA */


BOOL ScaAttemptSemaphoreListA(struct TagItem *OriginalTagList)
{
	struct TagItem *TagList;
	struct TagItem *Tag;
	BOOL Success = TRUE;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	TagList = OriginalTagList;

	Forbid();
	while (Success && (Tag = NextTagItem(&TagList)))
		{
		d1(kprintf("%s/%s/%ld: Tag=%08lx  ti_Tag=%08lx  Sema=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag, Tag->ti_Tag, Tag->ti_Data));

		switch (Tag->ti_Tag)
			{
		case SCA_SemaphoreExclusive:
			Success = ScalosAttemptSemaphore((SCALOSSEMAPHORE *) Tag->ti_Data);
			break;

		case SCA_SemaphoreShared:
			Success = ScalosAttemptSemaphoreShared((SCALOSSEMAPHORE *) Tag->ti_Data);
			break;

		case SCA_SemaphoreExclusiveNoNest:
			Success = AttemptSemaphoreNoNest((SCALOSSEMAPHORE *) Tag->ti_Data);
			break;
			}

		d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

		if (!Success)
			{
			struct TagItem *Tag2, *TagList2 = OriginalTagList;

			// Release all previously obtained semaphores
			while ((Tag2 = NextTagItem(&TagList2)) && Tag2 != Tag)
				{
				d1(kprintf("%s/%s/%ld: Tag2=%08lx  ti_Tag=%08lx  Sema=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag2, Tag2->ti_Tag, Tag2->ti_Data));
				ScalosReleaseSemaphore((SCALOSSEMAPHORE *) Tag2->ti_Data);
				}
			}
		}
	Permit();

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}


#ifdef	DEBUG_SEMAPHORES

#undef SignalSemaphore

void DebugScalosLockIconListShared(struct internalScaWindowTask *iwt,
	CONST_STRPTR CallingFile, CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle ? iwt->iwt_WinTitle : (STRPTR) ""));

	// Problem addressed: if the current process already holds an exclusive
	// lock on the semaphore, ObtainSemaphoreShared() is guaranteed to
	// create a deadlock.
	// Solution: is we already own this semaphore exclusively,
	// ScalosAttemptSemaphore() will succeed.
	if (DebugScalosAttemptSemaphoreShared(iwt->iwt_WindowTask.wt_IconSemaphore, CallingFile, CallingFunc, CallingLine))
		{
		if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
			{
			iwt->iwt_IconListLockedShared++;
			}
		d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
		return;
		}
	if (DebugScalosAttemptSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore, CallingFile, CallingFunc, CallingLine))
		{
		if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
			{
			iwt->iwt_IconListLockedExclusive++;
			}
		d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
		return;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DebugScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_IconSemaphore, CallingFile, CallingFunc, CallingLine);

	if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
		{
		iwt->iwt_IconListLockedShared++;
		}

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle ? iwt->iwt_WinTitle : (STRPTR) ""));
}


void DebugScalosLockIconListExclusive(struct internalScaWindowTask *iwt,
	CONST_STRPTR CallingFile, CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle ? iwt->iwt_WinTitle : (STRPTR) ""));

	if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL)
		&& iwt->iwt_IconListLockedShared)
		{
		// the window process already holds a shared lock on the icon list.
		// calling ScalosObtainSemaphore now guarantees a deadlock.
		ULONG Count = 0;

		d1(kprintf("%s/%s/%ld: iwt_IconListLockedShared=%lu  iwt_IconListLockedExclusive=%lu\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_IconListLockedShared, iwt->iwt_IconListLockedExclusive));

		Forbid();

		// release all shared locks, and establish exclusive lock(s)
		while (iwt->iwt_IconListLockedShared)
			{
			iwt->iwt_IconListLockedShared--;
			Count++;
			ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore);
			}
		while (Count--)
			{
			iwt->iwt_IconListLockedExclusive++;
			ScalosObtainSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore);
			}

		Permit();

		d1(kprintf("%s/%s/%ld: iwt_IconListLockedShared=%lu  iwt_IconListLockedExclusive=%lu\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_IconListLockedShared, iwt->iwt_IconListLockedExclusive));
		}

	DebugScalosObtainSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore, CallingFile, CallingFunc, CallingLine);

	if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
		{
		iwt->iwt_IconListLockedExclusive++;
		}

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle ? iwt->iwt_WinTitle : (STRPTR) ""));
}



struct ScaWindowList *DebugSCA_LockWindowList(LONG accessmode,
	CONST_STRPTR CallingFile, CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	switch (accessmode)
		{
	case SCA_LockWindowList_Shared:
		DebugScalosObtainSemaphoreShared(&WinListSemaphore,
			CallingFile, CallingFunc, CallingLine);
		break;
	case SCA_LockWindowList_Exclusiv:
		DebugScalosObtainSemaphore(&WinListSemaphore,
			CallingFile, CallingFunc, CallingLine);
		break;
	default:
		break;
		}

	return &winlist;
}


struct ScalosSemaphore *DebugScalosCreateSemaphore(CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	struct ScalosSemaphore *xsema;

	xsema = AllocVec(sizeof(struct ScalosSemaphore), MEMF_PUBLIC | MEMF_CLEAR);
	if (xsema)
		DebugScalosInitSemaphore(xsema, CallingFile, CallingFunc, CallingLine);

	return xsema;
}


void DebugScalosDeleteSemaphore(struct ScalosSemaphore *xsema,
	CONST_STRPTR CallingFile, CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	if (xsema)
		{
		DebugScalosObtainSemaphore(xsema, CallingFile, CallingFunc, CallingLine);
		Forbid();
		DebugScalosReleaseSemaphore(xsema, CallingFile, CallingFunc, CallingLine);
		FreeVec(xsema);
		Permit();
		}
}


void DebugScalosInitSemaphore(struct ScalosSemaphore *xsema,
	CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	d1(kprintf("%s/%s/%ld: Begin InitSemaphore(%08lx <%s>)  Task=%08lx <%s>\n", \
		CallingFile, CallingFunc, CallingLine, xsema, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));

	xsema->MagicNumber = DEBUG_SEMAPHORE_MAGIC;
	xsema->SafetyPtr = &xsema->Sema;

	NewList(&xsema->OwnerList);
	NewList(&xsema->BidderList);
	InitSemaphore(&xsema->Sema);

	d1(kprintf("%s/%s/%ld: End InitSemaphore(%08lx <%s>)\n", \
		CallingFile, CallingFunc, CallingLine, xsema));
}


void DebugScalosObtainSemaphore(struct ScalosSemaphore *xsema,
	CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	struct DebugSemaOwner *Owner;

	d1(kprintf("%s/%s/%ld: Begin ObtainSemaphore(%08lx <%s>)  Task=%08lx <%s>\n", \
		CallingFile, CallingFunc, CallingLine, xsema, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));

	Owner = AllocVec(sizeof(struct DebugSemaOwner), MEMF_PUBLIC);
	if (Owner)
		{
		Forbid();
		Owner->OwnMode = OWNMODE_EXCLUSIVE;
		Owner->FileName = CallingFile;
		Owner->Func = CallingFunc;
		Owner->Line = CallingLine;
		Owner->Proc = (struct Process *) FindTask(NULL);
		d1(if (Owner->Proc->pr_Task.tc_Node.ln_Name && 0 == strcmp(Owner->Proc->pr_Task.tc_Node.ln_Name, "input.device")) \
			KPrintF("%s/%s/%ld: ObtainSemaphore called from input.device!\n", CallingFile, CallingFunc, CallingLine));
		AddHead(&xsema->BidderList, &Owner->node);
		Permit();
		}

	ObtainSemaphore(&xsema->Sema);

	if (Owner)
		{
		Forbid();
		Remove(&Owner->node);
		AddHead(&xsema->OwnerList, &Owner->node);
		Permit();
		}

	d1(kprintf("%s/%s/%ld: End ObtainSemaphore(%08lx <%s>)\n", \
		CallingFile, CallingFunc, CallingLine, xsema));
}


void DebugScalosObtainSemaphoreShared(struct ScalosSemaphore *xsema,
	CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	struct DebugSemaOwner *Owner;

	d1(kprintf("%s/%s/%ld: Begin ObtainSemaphoreShared(%08lx <%s>)  Task=%08lx <%s>\n", \
		CallingFile, CallingFunc, CallingLine, xsema, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));

	Owner = AllocVec(sizeof(struct DebugSemaOwner), MEMF_PUBLIC);
	if (Owner)
		{
		Forbid();
		Owner->OwnMode = OWNMODE_SHARED;
		Owner->FileName = CallingFile;
		Owner->Func = CallingFunc;
		Owner->Line = CallingLine;
		Owner->Proc = (struct Process *) FindTask(NULL);
		d1(if (Owner->Proc->pr_Task.tc_Node.ln_Name && 0 == strcmp(Owner->Proc->pr_Task.tc_Node.ln_Name, "input.device")) \
			KPrintF("%s/%s/%ld: ObtainSemaphoreShared called from input.device!\n", CallingFile, CallingFunc, CallingLine));
		AddHead(&xsema->BidderList, &Owner->node);
		Permit();
		}

	ObtainSemaphoreShared(&xsema->Sema);

	if (Owner)
		{
		Forbid();
		Remove(&Owner->node);
		AddHead(&xsema->OwnerList, &Owner->node);
		Permit();
		}

	d1(kprintf("%s/%s/%ld: End ObtainSemaphoreShared(%08lx <%s>)\n", \
		CallingFile, CallingFunc, CallingLine, xsema));
}


void DebugScalosReleaseSemaphore(struct ScalosSemaphore *xsema,
	CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	struct DebugSemaOwner *Owner;
	struct Process *myProc = (struct Process *) FindTask(NULL);
	BOOL Found = FALSE;

	d1(kprintf("%s/%s/%ld: Begin ReleaseSemaphore(%08lx)  Task=%08lx <%s>\n", \
		CallingFile, CallingFunc, CallingLine, xsema, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));

	Forbid();
	for (Owner = (struct DebugSemaOwner *) xsema->OwnerList.lh_Head;
		Owner != (struct DebugSemaOwner *) &xsema->OwnerList.lh_Tail;
		Owner = (struct DebugSemaOwner *) Owner->node.ln_Succ)
		{
		if (Owner->Proc == myProc)
			{
			Found = TRUE;
			Remove(&Owner->node);
			FreeVec(Owner);
			break;
			}
		}
	Permit();

	if (!Found)
		{
		kprintf("%s/%s/%ld: FAILED ReleaseSemaphore(%08lx) - Task %08lx (%s) never locked this Semaphore!\n", \
			CallingFile, CallingFunc, CallingLine, xsema,
			FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name);
		}

	ReleaseSemaphore(&xsema->Sema);

	d1(kprintf("%s/%s/%ld: End ReleaseSemaphore(%08lx)\n", \
		CallingFile, CallingFunc, CallingLine, xsema));
}


ULONG DebugScalosAttemptSemaphore(struct ScalosSemaphore *xsema,
	CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld: Begin AttemptSemaphore(%08lx)  Task=%08lx <%s>\n", \
		CallingFile, CallingFunc, CallingLine, xsema, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));

	Result = AttemptSemaphore(&xsema->Sema);

	if (Result)
		{
		struct DebugSemaOwner *Owner;

		Owner = AllocVec(sizeof(struct DebugSemaOwner), MEMF_PUBLIC);
		if (Owner)
			{
			Forbid();
			Owner->OwnMode = OWNMODE_EXCLUSIVE;
			Owner->FileName = CallingFile;
			Owner->Func = CallingFunc;
			Owner->Line = CallingLine;
			Owner->Proc = (struct Process *) FindTask(NULL);
			AddHead(&xsema->OwnerList, &Owner->node);
			Permit();
			}
		}

	d1(kprintf("%s/%s/%ld: End AttemptSemaphore(%08lx) = %ld\n", \
		CallingFile, CallingFunc, CallingLine, xsema, Result));

	return Result;
}


ULONG DebugScalosAttemptSemaphoreShared(struct ScalosSemaphore *xsema,
	CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld: Begin AttemptSemaphoreShared(%08lx)  Task=%08lx <%s>\n", \
		CallingFile, CallingFunc, CallingLine, xsema, FindTask(NULL), \
		FindTask(NULL)->tc_Node.ln_Name ? FindTask(NULL)->tc_Node.ln_Name : (char *) ""));

	Result = AttemptSemaphoreShared(&xsema->Sema);

	if (Result)
		{
		struct DebugSemaOwner *Owner;

		d1(KPrintF("%s/%s/%ld: AttemptSemaphoreShared() succeeded\n", __FILE__, __FUNC__, __LINE__));

		Owner = AllocVec(sizeof(struct DebugSemaOwner), MEMF_PUBLIC);
		if (Owner)
			{
			Forbid();
			Owner->OwnMode = OWNMODE_SHARED;
			Owner->FileName = CallingFile;
			Owner->Func = CallingFunc;
			Owner->Line = CallingLine;
			Owner->Proc = (struct Process *) FindTask(NULL);
			AddHead(&xsema->OwnerList, &Owner->node);
			Permit();
			}
		}
	else
		{
		Result = DebugScalosAttemptSemaphore(xsema, CallingFile, CallingFunc, CallingLine);
		}

	d1(kprintf("%s/%s/%ld: End AttemptSemaphoreShared(%08lx) = %ld\n", \
		CallingFile, CallingFunc, CallingLine, xsema, Result));

	return Result;
}

#else /* DEBUG_SEMAPHORES */

#undef ScalosCreateSemaphore
#undef ScalosDeleteSemaphore
#undef ScalosInitSemaphore
#undef ScalosObtainSemaphore
#undef ScalosObtainSemaphoreShared
#undef ScalosReleaseSemaphore
#undef ScalosAttemptSemaphore
#undef ScalosAttemptSemaphoreShared

/*
struct ScaWindowList *SCA_LockWindowList(LONG accessmode)
{
	switch (accessmode)
		{
	case SCA_LockWindowList_Shared:
		ObtainSemaphoreShared_Debug(&WinListSemaphore,
			CallingFile, CallingFunc, CallingLine);
		break;
	case SCA_LockWindowList_Exclusiv:
		ObtainSemaphore_Debug(&WinListSemaphore,
			CallingFile, CallingFunc, CallingLine);
		break;
	default:
		break;
		}

	return &winlist;
}
*/

SCALOSSEMAPHORE	*ScalosCreateSemaphore(void)
{
	SCALOSSEMAPHORE	*sema;

	sema = AllocVec(sizeof(SCALOSSEMAPHORE), MEMF_PUBLIC | MEMF_CLEAR);
	if (sema)
		ScalosInitSemaphore(sema);

	return sema;
}


void ScalosDeleteSemaphore(SCALOSSEMAPHORE *sema)
{
	if (sema)
		{
		ScalosObtainSemaphore(sema);
		Forbid();
		ScalosReleaseSemaphore(sema);
		FreeVec(sema);
		Permit();
		}
}

void ScalosInitSemaphore(struct ScalosSemaphore *xsema)
{
	InitSemaphore(&xsema->Sema);
}


void ScalosObtainSemaphore(struct ScalosSemaphore *xsema)
{
	ObtainSemaphore(&xsema->Sema);
}


void ScalosObtainSemaphoreShared(struct ScalosSemaphore *xsema)
{
	ObtainSemaphoreShared(&xsema->Sema);
}


void ScalosReleaseSemaphore(struct ScalosSemaphore *xsema)
{
	ReleaseSemaphore(&xsema->Sema);
}


ULONG ScalosAttemptSemaphore(struct ScalosSemaphore *xsema)
{
	return AttemptSemaphore(&xsema->Sema);
}


ULONG ScalosAttemptSemaphoreShared(struct ScalosSemaphore *xsema)
{
	return AttemptSemaphoreShared(&xsema->Sema)
		|| AttemptSemaphore(&xsema->Sema);
}

#endif /* DEBUG_SEMAPHORES */


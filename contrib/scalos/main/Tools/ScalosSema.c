// ScalosSema.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/exec.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <intuition/classusr.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <libraries/dos.h>

#define DEBUG_SEMAPHORES

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#ifdef __SASC
#include "/debug.h"
#else //__SASC
#include "../debug.h"
#endif // __SASC
#undef	SCA_LockWindowList
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <scalos/scalos.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <Year.h> //+jmc+

#ifdef __SASC
#include "/Semaphores.h"
#include "/scalos_structures.h"
#else //__SASC
#include "../Semaphores.h"
#include "../scalos_structures.h"
#endif //__SASC


struct DebugSemaOwnerClone
	{
	struct Node node;
	ULONG OwnMode;
	ULONG Line;
	STRPTR FileName;
	STRPTR Func;
	STRPTR ProcName;
	};


//struct ScaWindowList *SCA_LockWindowList( LONG accessmode );

static void DoSema(const struct internalScaWindowTask *iwt, 
	const SCALOSSEMAPHORE *xSema, CONST_STRPTR SemaName, BOOL *WinTitlePrinted);
static STRPTR GetProcName(struct Process *Proc, STRPTR Buffer, size_t BuffLength);
static BOOL CloneList(struct List *destList, const struct List *srcList);
static void CleanupList(struct List *theList);

#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__)
size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !defined(__SASC) && !defined(__MORPHOS__)  */


struct ScalosBase *ScalosBase;
extern struct DosLibrary *DOSBase;

#ifdef __amigaos4__
struct ScalosIFace *IScalos;
#endif

CONST_STRPTR Ver = "\0$VER:ScalosSema 40.1 (29.5.2004) "
	COMPILER_STRING
	" ©2004" CURRENTYEAR " The Scalos Team";

static BOOL fListAll = FALSE;


int main(int argc, char *argv[])
{
	CONST_STRPTR ArgTemplate = "ALL/S";
	IPTR ArgOpts[2];
	struct RDArgs *Args;
	struct ScaWindowList *WinList;
	struct ScalosSemaphoreList *sspl;

	memset(ArgOpts, 0, sizeof(ArgOpts));

	ScalosBase = (struct ScalosBase *) OpenLibrary("scalos.library", 40);
	if (NULL == ScalosBase)
		{
		fprintf(stderr, "Failed to open scalos.library V40+\n");
		return RETURN_ERROR;
		}
#ifdef __amigaos4__
	IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
	if (NULL == IScalos)
		{
		fprintf(stderr, "Failed to get OS4 interface of scalos.library V40+\n");
		CloseLibrary((struct Library *)ScalosBase);
		return RETURN_ERROR;
		}
#endif

	if (((struct Library *)DOSBase)->lib_Version < 39)
		{
		printf("ScalosSema needs DOS.library v39+\n");
		return RETURN_ERROR;
		}

	Args = ReadArgs(ArgTemplate, ArgOpts, NULL);
	if (NULL == Args)
		{
		PrintFault(IoErr(), NULL);	/* prints the appropriate err message */

		return RETURN_ERROR;
		}

	if (ArgOpts[0])
		fListAll = TRUE;

	WinList = SCA_LockWindowList(-1);	// -1 causes no locking at all!
	if (WinList)
		{
		struct ScaWindowStruct *ws;

		for (ws = WinList->wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			struct internalScaWindowTask *iwt;
			BOOL WinTitlePrinted = FALSE;

			iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;

			DoSema(iwt, (struct ScalosSemaphore const *) iwt->iwt_WindowTask.wt_IconSemaphore, "wt_IconSemaphore", &WinTitlePrinted);
			DoSema(iwt, (struct ScalosSemaphore const *) iwt->iwt_WindowTask.wt_WindowSemaphore, "wt_WindowSemaphore", &WinTitlePrinted);
			DoSema(iwt, (struct ScalosSemaphore const *) iwt->iwt_AppListSemaphore, "iwt_AppListSemaphore", &WinTitlePrinted);
			DoSema(iwt, (struct ScalosSemaphore const *) &iwt->iwt_UpdateSemaphore, "iwt_UpdateSemaphore", &WinTitlePrinted);
			DoSema(iwt, (struct ScalosSemaphore const *) &iwt->iwt_ChildProcessSemaphore, "iwt_ChildProcessSemaphore", &WinTitlePrinted);
			DoSema(iwt, (struct ScalosSemaphore const *) &iwt->iwt_ScanDirSemaphore, "iwt_ScanDirSemaphore", &WinTitlePrinted);
			DoSema(iwt, (struct ScalosSemaphore const *) &iwt->iwt_ThumbnailIconSemaphore, "iwt_ThumbnailIconSemaphore", &WinTitlePrinted);
			DoSema(iwt, (struct ScalosSemaphore const *) &iwt->iwt_ThumbGenerateSemaphore, "iwt_ThumbGenerateSemaphore", &WinTitlePrinted);
			}
		}

	if (SCA_ScalosControl(NULL, SCALOSCTRLA_GetSemaphoreList, (ULONG) &sspl, TAG_END) > 0)
		{
		BOOL Dummy = TRUE;

		while (sspl && sspl->sspl_Semaphore)
			{
			DoSema(NULL, (struct ScalosSemaphore const *) sspl->sspl_Semaphore, sspl->sspl_Name, &Dummy);
			sspl++;
			}
		}

	FreeArgs(Args);
#ifdef __amigaos4__
	DropInterface((struct Interface *)IScalos);
#endif
	CloseLibrary((struct Library *) ScalosBase);

	return RETURN_OK;
}


static void DoSema(const struct internalScaWindowTask *iwt, 
	const SCALOSSEMAPHORE *xSema, CONST_STRPTR SemaName, BOOL *WinTitlePrinted)
{
	if (xSema && DEBUG_SEMAPHORE_MAGIC == xSema->MagicNumber
		&& xSema->SafetyPtr == &xSema->Sema)
		{
		if (fListAll || !IsListEmpty((struct List *) &xSema->OwnerList) || !IsListEmpty((struct List *) &xSema->BidderList))
			{
			CONST_STRPTR Prefix;
			const struct DebugSemaOwnerClone *Owner;

			if (iwt)
				{
				Prefix = "   ";
				if (!*WinTitlePrinted)
					{
					if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Name)
						printf("\nWindow <%s>\n", iwt->iwt_WindowTask.mt_WindowStruct->ws_Name);
					else
						printf("\nMain Window\n");
					*WinTitlePrinted = TRUE;
					}
				}
			else
				{
				Prefix = "";
				}

			printf("%sSemaphore <%s> %08lx\n", Prefix, SemaName, (unsigned long) &xSema->Sema);
			printf("%s   Owner:  ", Prefix);
			if (IsListEmpty((struct List *) &xSema->OwnerList))
				{
				printf("-\n");
				}
			else
				{
				struct List ListClone;

				NewList(&ListClone);
				CloneList(&ListClone, &xSema->OwnerList);

				printf("\n");

				for (Owner = (const struct DebugSemaOwnerClone *) ListClone.lh_Head;
					Owner != (const struct DebugSemaOwnerClone *) &ListClone.lh_Tail;
					Owner = (const struct DebugSemaOwnerClone *) Owner->node.ln_Succ)
					{
					CONST_STRPTR OwnModeString;

					if (OWNMODE_EXCLUSIVE == Owner->OwnMode || xSema->Sema.ss_Owner)
						OwnModeString = "Ex";
					else
						OwnModeString = "Sh";

					printf("%s      [%s] %s/%s/%ld  <%s>\n", Prefix, 
						OwnModeString,
						Owner->FileName, 
						Owner->Func, (long) Owner->Line,
						Owner->ProcName);
					}

				CleanupList(&ListClone);
				}

			printf("%s   Bidder:  ", Prefix);
			if (IsListEmpty((struct List *) &xSema->BidderList))
				{
				printf("-\n");
				}
			else
				{
				struct List ListClone;

				NewList(&ListClone);
				CloneList(&ListClone, &xSema->BidderList);

				printf("\n");

				for (Owner = (const struct DebugSemaOwnerClone *) ListClone.lh_Head;
					Owner != (const struct DebugSemaOwnerClone *) &ListClone.lh_Tail;
					Owner = (const struct DebugSemaOwnerClone *) Owner->node.ln_Succ)
					{
					printf("%s      [%s] %s/%s/%ld  <%s>\n", Prefix, 
						(OWNMODE_EXCLUSIVE == Owner->OwnMode) ? "Ex" : "Sh",
						Owner->FileName, 
						Owner->Func, (long) Owner->Line,
						Owner->ProcName);
					}

				CleanupList(&ListClone);
				}
			}
		}
}


static STRPTR GetProcName(struct Process *Proc, STRPTR Buffer, size_t BuffLength)
{
	strcpy(Buffer, "");
	if (Proc && Proc->pr_Task.tc_Node.ln_Name)
		stccpy(Buffer, Proc->pr_Task.tc_Node.ln_Name, BuffLength);

	return Buffer;
}


static BOOL CloneList(struct List *destList, const struct List *srcList)
{
	BOOL Success = TRUE;
	const struct DebugSemaOwner *Entry;

	Forbid();
	for (Entry = (const struct DebugSemaOwner *) srcList->lh_Head;
		Entry != (const struct DebugSemaOwner *) &srcList->lh_Tail;
		Entry = (const struct DebugSemaOwner *) Entry->node.ln_Succ)
		{
		struct DebugSemaOwnerClone *Clone;

		Clone = malloc(sizeof(struct DebugSemaOwnerClone));
		if (Clone)
			{
			char ProcName[256];

			Clone->OwnMode = Entry->OwnMode;
			Clone->Line = Entry->Line;
			Clone->FileName = strdup(Entry->FileName);
			Clone->Func = strdup(Entry->Func);
			GetProcName(Entry->Proc, ProcName, sizeof(ProcName));
			Clone->ProcName = strdup(ProcName);

			AddTail(destList, &Clone->node);
			}
		}
	Permit();

	return Success;
}


static void CleanupList(struct List *theList)
{
	struct DebugSemaOwnerClone *Clone;

	while ((Clone = (struct DebugSemaOwnerClone *) RemTail(theList)))
		{
		if (Clone->FileName)
			free(Clone->FileName);
		if (Clone->Func)
			free(Clone->Func);
		if (Clone->ProcName)
			free(Clone->ProcName);
		free(Clone);
		}
}


#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__)
// Replacement for SAS/C library functions

size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}
#endif /* !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__)  */

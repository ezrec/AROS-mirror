// Memory.c
// $Date$
// $Revision$

#include <exec/types.h>
#ifdef __amigaos4__
#include <dos/anchorpath.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#endif
#include <dos/dosasl.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/ttengine.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// macros and local data structures

#define	PUDDLESIZE_MAIN         65536
#define	THRESHOLD_MAIN		2048
#define	PUDDLESIZE_MSG		8192
#define	THRESHOLD_MSG		256
#define	PUDDLESIZE_NODES	8192
#define	THRESHOLD_NODES		256

// define DLMALLOC to use Doug Lea's memory allocator instead of OS's memory pools
#if !defined(__AROS__)
#define DLMALLOC	        1
#endif

// DLMALLOC_CLEAR to clear all allocated memory - ATM still required
#define DLMALLOC_CLEAR		1

//----------------------------------------------------------------------------

#if DLMALLOC
void* dlmalloc(size_t);
void  dlfree(void*);
void* dlrealloc(void*, size_t);
#endif //DLMALLOC

//----------------------------------------------------------------------------

// local Data

#if DLMALLOC
#else //DLMALLOC
static SCALOSSEMAPHORE MainMemPoolSemaphore;		// main memory pool semaphore
static SCALOSSEMAPHORE MsgMemPoolSemaphore;		// main memory pool semaphore
static SCALOSSEMAPHORE NodeMemPoolSemaphore;		// main memory pool semaphore
static void *MainMemPool = NULL;			// main memory pool
static void *MsgMemPool = NULL;				// main memory pool
static void *NodeMemPool = NULL;			// main memory pool
#endif //DLMALLOC

// ----------------------------------------------------------

BOOL MemoryInit(void)
{
#if DLMALLOC
#else //DLMALLOC
	ScalosInitSemaphore(&MainMemPoolSemaphore);
	ScalosInitSemaphore(&MsgMemPoolSemaphore);
	ScalosInitSemaphore(&NodeMemPoolSemaphore);

	MainMemPool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR, PUDDLESIZE_MAIN, THRESHOLD_MAIN);
	d1(KPrintF("%s/%s/%ld: MainMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMemPool));
	if (NULL == MainMemPool)
		return FALSE;

	MsgMemPool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR, PUDDLESIZE_MSG, THRESHOLD_MSG);
	d1(KPrintF("%s/%s/%ld: MsgMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, MsgMemPool));
	if (NULL == MsgMemPool)
		return FALSE;

	NodeMemPool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR, PUDDLESIZE_NODES, THRESHOLD_NODES);
	d1(KPrintF("%s/%s/%ld: MainMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, NodeMemPool));
	if (NULL == NodeMemPool)
		return FALSE;
#endif //DLMALLOC

	return TRUE;
}

// ----------------------------------------------------------

void MemoryCleanup(void)
{
#if DLMALLOC
#else //DLMALLOC
	d1(KPrintF("%s/%s/%ld: MainMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMemPool));
	if (MainMemPool)
		{
		DeletePool(MainMemPool);
		MainMemPool = NULL;
		}
	d1(KPrintF("%s/%s/%ld: MsgMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, MsgMemPool));
	if (MsgMemPool)
		{
		DeletePool(MsgMemPool);
		MsgMemPool = NULL;
		}
	d1(KPrintF("%s/%s/%ld: NodeMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, NodeMemPool));
	if (NodeMemPool)
		{
		DeletePool(NodeMemPool);
		NodeMemPool = NULL;
		}
#endif //DLMALLOC
}

// ----------------------------------------------------------

#ifndef DEBUG_MEMORY

#ifndef __GNUC__
#undef	ObtainSemaphore
#undef	ReleaseSemaphore
#endif /* __GNUC__ */

APTR ScalosAlloc(ULONG Size)
{
#if DLMALLOC
	APTR ptr;

	d1(kprintf("%s/%s/%ld: Size=%lu\n", __FILE__, __FUNC__, __LINE__, Size));

	ptr = (APTR) dlmalloc(Size);

#if DLMALLOC_CLEAR
	if (ptr)
		memset(ptr, 0, Size);
#endif //DLMALLOC_CLEAR

	d1(kprintf("%s/%s/%ld: END return %08lx\n", __FILE__, __FUNC__, __LINE__, ptr));
	return ptr;
#else //DLMALLOC
	struct AllocatedMemFromPool *ptr;

	d1(kprintf("%s/%s/%ld: Size=%lu  MemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, Size, MemPool));

	if (MainMemPool)
		{
		size_t AllocSize = Size + sizeof(struct AllocatedMemFromPool);

		d1(kprintf("%s/%s/%ld: AllocSize=%lu\n", __FILE__, __FUNC__, __LINE__, AllocSize));
		ScalosObtainSemaphore(&MainMemPoolSemaphore);
		ptr = AllocPooled(MainMemPool, AllocSize);
		ScalosReleaseSemaphore(&MainMemPoolSemaphore);
		d1(kprintf("%s/%s/%ld: ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, ptr));
		if (ptr)
			{
			ptr->amp_Size = AllocSize;

			d1(kprintf("%s/%s/%ld: return %08lx\n", __FILE__, __FUNC__, __LINE__, ptr->amp_UserData));
			return (APTR) &ptr->amp_UserData;
			}
		}

	d1(kprintf("%s/%s/%ld: return NULL\n", __FILE__, __FUNC__, __LINE__));

	return NULL;
#endif //DLMALLOC
}


void ScalosFree(APTR mem)
{
#if DLMALLOC
	d1(KPrintF("%s/%s/%ld: START OldMem=%08lx\n", __FILE__, __FUNC__, __LINE__, mem));
	dlfree(mem);
	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
#else //DLMALLOC
	if (MainMemPool && mem)
		{
		struct AllocatedMemFromPool *ptr;

		ptr = (struct AllocatedMemFromPool *) (((UBYTE *) mem) - offsetof(struct AllocatedMemFromPool, amp_UserData));

		ScalosObtainSemaphore(&MainMemPoolSemaphore);
		FreePooled(MainMemPool, ptr, ptr->amp_Size);
		ScalosReleaseSemaphore(&MainMemPoolSemaphore);
		}
#endif //DLMALLOC
}


APTR ScalosRealloc(APTR OldMem, ULONG NewSize)
{
#if DLMALLOC
	APTR ptr;

	d1(KPrintF("%s/%s/%ld: START OldMem=%08lx  NewSize=%lu\n", __FILE__, __FUNC__, __LINE__, OldMem, NewSize));
	ptr = dlrealloc(OldMem, NewSize);
	d1(kprintf("%s/%s/%ld: END return %08lx\n", __FILE__, __FUNC__, __LINE__, ptr));

	return ptr;
#else //DLMALLOC
	APTR NewMem = NULL;

	d1(KPrintF("%s/%s/%ld: OldMem=%08lx  NewSize=%lu\n", __FILE__, __FUNC__, __LINE__, OldMem, NewSize));
	if (OldMem)
		{
		struct AllocatedMemFromPool *ptr;
		ULONG OldSize;

		if (0 == NewSize)
			{
			ScalosFree(OldMem);
			return NULL;
			}

		ptr = (struct AllocatedMemFromPool *) (((UBYTE *) OldMem) - offsetof(struct AllocatedMemFromPool, amp_UserData));
		OldSize = ptr->amp_Size - sizeof(struct AllocatedMemFromPool);

		if (OldSize >= NewSize)
			return OldMem;

		NewMem = ScalosAlloc(NewSize);

		if (NewMem && OldMem)
			{
			memcpy(NewMem, OldMem, OldSize);
			ScalosFree(OldMem);
			}

		d1(KPrintF("%s/%s/%ld: NewMem=%08lx\n", __FILE__, __FUNC__, __LINE__, NewMem));
		}

	return NewMem;
#endif //DLMALLOC
}

#endif /* DEBUG_MEMORY */

// ----------------------------------------------------------

//#if defined(DEBUG_MEMORY) && !defined(__GNUC__)
#if defined(DEBUG_MEMORY)

APTR ScalosAlloc_Debug(ULONG Size, CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
#if DLMALLOC
	void *ptr;

	d1(KPrintF("%s/%s/%ld: START Size=%lu\n", CallingFile, CallingFunc, CallingLine, Size));
	ptr = dlmalloc(Size);
	d1(kprintf("%s/%s/%ld: END return %08lx\n", __FILE__, __FUNC__, __LINE__, ptr));

#if DLMALLOC_CLEAR
	if (ptr)
		memset(ptr, 0x00, Size);
#endif //DLMALLOC_CLEAR

	return ptr;
#else //DLMALLOC
	struct AllocatedMemFromPoolDebug *ptr;

	if (MainMemPool)
		{
		size_t AllocSize;

		AllocSize = Size + sizeof(struct AllocatedMemFromPoolDebug) + SCALOS_MEM_TRAILER * sizeof(ULONG);

		ScalosObtainSemaphore(&MainMemPoolSemaphore);
		ptr = AllocPooled(MainMemPool, AllocSize);
		ScalosReleaseSemaphore(&MainMemPoolSemaphore);
		if (ptr)
			{
			ULONG n;

			ptr->amp_Size = AllocSize;

			ptr->amp_Line = CallingLine;
			ptr->amp_File = CallingFile;
			ptr->amp_Function = CallingFunc;

			ptr->amp_Magic = SCALOS_MEM_START_MAGIC;

			for (n = 0; n < SCALOS_MEM_TRAILER; n++)
				*((ULONG *) &ptr->amp_UserData[Size + n * sizeof(ULONG)]) = SCALOS_MEM_END_MAGIC;

			return (APTR) &ptr->amp_UserData;
			}
		}

	return NULL;
#endif //DLMALLOC
}


void ScalosFree_Debug(APTR mem, CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
#if DLMALLOC
	d1(KPrintF("%s/%s/%ld: START OldMem=%08lx\n", CallingFile, CallingFunc, CallingLine, mem));
	dlfree(mem);
	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
#else //DLMALLOC
	if (MainMemPool && mem)
		{
		ULONG n;
		size_t OrigSize;
		struct AllocatedMemFromPoolDebug *ptr;

		ptr = (struct AllocatedMemFromPoolDebug *) (((UBYTE *) mem) - offsetof(struct AllocatedMemFromPoolDebug, amp_UserData));

		if (ptr->amp_Magic != SCALOS_MEM_START_MAGIC)
			{
			kprintf("ScalosFree: %08lx START_MAGIC not found, called from %s/%s/%ld\n",
				mem, CallingFile, CallingFunc, CallingLine);
			return;
			}

		OrigSize = ptr->amp_Size - sizeof(struct AllocatedMemFromPoolDebug) - SCALOS_MEM_TRAILER * sizeof(ULONG);

		// Check if block trailer is OK
		for (n = 0; n < SCALOS_MEM_TRAILER; n++)
			{
			if (*((ULONG *) &ptr->amp_UserData[OrigSize + n * sizeof(ULONG)]) != SCALOS_MEM_END_MAGIC)
				{
				kprintf("ScalosFree: %08lx trailer damaged, called from %s/%s/%ld\n",
					mem, CallingFile, CallingFunc, CallingLine);
				kprintf("               original Length=%lu, allocated from %s/%s/%ld\n",
					OrigSize, ptr->amp_File, ptr->amp_Function, ptr->amp_Line);
				}
			}

		ScalosObtainSemaphore(&MainMemPoolSemaphore);
		FreePooled(MainMemPool, ptr, ptr->amp_Size);
		ScalosReleaseSemaphore(&MainMemPoolSemaphore);
		}
#endif //DLMALLOC
}

#endif /* DEBUG_MEMORY */

// ----------------------------------------------------------

STRPTR AllocCopyString(CONST_STRPTR clp)
{
	STRPTR lp;

	d1(KPrintF("%s/%s/%ld: START clp=%08lx\n", __FILE__, __FUNC__, __LINE__, clp));

	if (NULL == clp)
		clp = "";

#if DLMALLOC
	lp = dlmalloc(1 + strlen(clp));
#else //DLMALLOC
	lp = AllocVec(1 + strlen(clp), MEMF_PUBLIC);
#endif //DLMALLOC
	if (lp)
		strcpy(lp, clp);

	d1(KPrintF("%s/%s/%ld: END lp=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, lp, clp));

	return lp;
}


void FreeCopyString(STRPTR lp)
{
	d1(KPrintF("%s/%s/%ld: START lp=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, lp, lp));
#if DLMALLOC
	dlfree(lp);
#else //DLMALLOC
	if (lp)
		FreeVec(lp);
#endif //DLMALLOC
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

// ----------------------------------------------------------

void *ScalosAllocNode(size_t Size)
{
#if DLMALLOC
	APTR ptr;

	d1(kprintf("%s/%s/%ld: START  Size=%lu\n", __FILE__, __FUNC__, __LINE__, Size));

	ptr = (APTR) dlmalloc(Size);

#if DLMALLOC_CLEAR
	if (ptr)
		memset(ptr, 0, Size);
#endif //DLMALLOC_CLEAR

	d1(KPrintF("%s/%s/%ld: END ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, ptr));
	return ptr;
#else //DLMALLOC
	struct AllocatedMemFromPool *ptr;

	d1(KPrintF("%s/%s/%ld: Size=%lu  NodeMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, Size, NodeMemPool));

	if (NodeMemPool)
		{
		size_t AllocSize;

		AllocSize = Size + sizeof(struct AllocatedMemFromPool);

		d1(kprintf("%s/%s/%ld: AllocSize=%lu\n", __FILE__, __FUNC__, __LINE__, AllocSize));
		ScalosObtainSemaphore(&NodeMemPoolSemaphore);
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		ptr = AllocPooled(NodeMemPool, AllocSize);
		d1(kprintf("%s/%s/%ld: ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, ptr));
		ScalosReleaseSemaphore(&NodeMemPoolSemaphore);
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		if (ptr)
			{
			ptr->amp_Size = AllocSize;

			d1(kprintf("%s/%s/%ld: return %08lx\n", __FILE__, __FUNC__, __LINE__, ptr->amp_UserData));
			return (APTR) &ptr->amp_UserData;
			}
		}

	d1(kprintf("%s/%s/%ld: return NULL\n", __FILE__, __FUNC__, __LINE__));

	return NULL;
#endif //DLMALLOC
}

void ScalosFreeNode(void *mem)
{
	d1(kprintf("%s/%s/%ld: START  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, mem));
#if DLMALLOC
	dlfree(mem);
#else //DLMALLOC
	if (NodeMemPool && mem)
		{
		struct AllocatedMemFromPool *ptr;

		ptr = (struct AllocatedMemFromPool *) (((UBYTE *) mem) - offsetof(struct AllocatedMemFromPool, amp_UserData));

		ScalosObtainSemaphore(&NodeMemPoolSemaphore);
		FreePooled(NodeMemPool, ptr, ptr->amp_Size);
		ScalosReleaseSemaphore(&NodeMemPoolSemaphore);
		}
#endif //DLMALLOC
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

// ----------------------------------------------------------

void *ScalosAllocMessage(size_t Size)
{
#if DLMALLOC
	APTR ptr;

	d1(kprintf("%s/%s/%ld: START  Size=%lu\n", __FILE__, __FUNC__, __LINE__, Size));

	ptr = (APTR) dlmalloc(Size);

#if DLMALLOC_CLEAR
	if (ptr)
		memset(ptr, 0, Size);
#endif

	d1(KPrintF("%s/%s/%ld: END ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, ptr));
	return ptr;
#else //DLMALLOC
	struct AllocatedMemFromPool *ptr;

	d1(KPrintF("%s/%s/%ld: Size=%lu  MsgMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, Size, MsgMemPool));

	if (MsgMemPool)
		{
		size_t AllocSize = Size + sizeof(struct AllocatedMemFromPool);

		d1(kprintf("%s/%s/%ld: AllocSize=%lu\n", __FILE__, __FUNC__, __LINE__, AllocSize));
		ScalosObtainSemaphore(&MsgMemPoolSemaphore);
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		ptr = AllocPooled(MsgMemPool, AllocSize);
		d1(kprintf("%s/%s/%ld: ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, ptr));
		ScalosReleaseSemaphore(&MsgMemPoolSemaphore);
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		if (ptr)
			{
			ptr->amp_Size = AllocSize;

			d1(kprintf("%s/%s/%ld: return %08lx\n", __FILE__, __FUNC__, __LINE__, ptr->amp_UserData));
			return (APTR) &ptr->amp_UserData;
			}
		}

	d1(kprintf("%s/%s/%ld: return NULL\n", __FILE__, __FUNC__, __LINE__));

	return NULL;
#endif //DLMALLOC
}

void ScalosFreeMessage(void *mem)
{
	d1(kprintf("%s/%s/%ld: START  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, mem));
#if DLMALLOC
	dlfree(mem);
#else //DLMALLOC
	if (MsgMemPool && mem)
		{
		struct AllocatedMemFromPool *ptr;

		ptr = (struct AllocatedMemFromPool *) (((UBYTE *) mem) - offsetof(struct AllocatedMemFromPool, amp_UserData));

		ScalosObtainSemaphore(&MsgMemPoolSemaphore);
		FreePooled(MsgMemPool, ptr, ptr->amp_Size);
		ScalosReleaseSemaphore(&MsgMemPoolSemaphore);
		}
#endif //DLMALLOC
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

// ----------------------------------------------------------

struct AnchorPath *ScalosAllocAnchorPath(ULONG Flags, size_t MaxPathLen)
{
	struct AnchorPath *ap;

#ifndef __amigaos4__
	ap = ScalosAlloc(sizeof(struct AnchorPath) + Max_PathLen);
	d1(KPrintF("%s/%s/%ld: ap=%08lx\n", __FILE__, __FUNC__, __LINE__, ap));
	if (ap)
		{
		memset(ap, 0, sizeof(struct AnchorPath));

		ap->ap_Flags = Flags;
		ap->ap_Strlen = MaxPathLen;
		}
#else /* __amigaos4__ */
	ap = AllocDosObjectTags(DOS_ANCHORPATH,
		ADO_Flags, Flags,
		ADO_Strlen, MaxPathLen,
		TAG_DONE
		);
	d1(KPrintF("%s/%s/%ld: ap=%08lx\n", __FILE__, __FUNC__, __LINE__, ap));
#endif /* __amigaos4__ */

	return ap;
}


void ScalosFreeAnchorPath(struct AnchorPath *ap)
{
	if (ap)
		{
#ifndef __amigaos4__
		ScalosFree(ap);
#else /* __amigaos4__ */
		FreeDosObject(DOS_ANCHORPATH, ap);
#endif /* __amigaos4__ */
		}
}

// ----------------------------------------------------------

struct InfoData *ScalosAllocInfoData(void)
{
#ifdef __amigaos4__
	return (struct InfoData *) AllocDosObject(DOS_INFODATA, NULL);
#else // __amigaos4__
	return (struct InfoData *) ScalosAlloc(sizeof(struct InfoData));
#endif //__amigaos4__
}

// ----------------------------------------------------------

void ScalosFreeInfoData(struct InfoData **pId)
{
	if (*pId)
		{
#ifdef __amigaos4__
		FreeDosObject(DOS_INFODATA, *pId);
#else // __amigaos4__
		ScalosFree(*pId);
#endif //__amigaos4__
		*pId = NULL;
		}
}

// ----------------------------------------------------------

STRPTR AllocPathBuffer(void)
{
	STRPTR Buffer;

	d1(kprintf("%s/%s/%ld: Max_PathLen=%ld Task=%08lx\n", __FILE__, __FUNC__, __LINE__,
		Max_PathLen, FindTask(NULL)));

	Buffer = (STRPTR) ScalosAlloc(Max_PathLen);

	d1(kprintf("%s/%s/%ld: String=%08lx\n", __FILE__, __FUNC__, __LINE__,
		Buffer));

	return Buffer;
}

// ----------------------------------------------------------

void FreePathBuffer(STRPTR Buffer)
{
	d1(kprintf("%s/%s/%ld: Buffer=%08lx\n", __FILE__, __FUNC__, __LINE__,
		Buffer));

	if (Buffer)
		ScalosFree(Buffer);
}

// ----------------------------------------------------------



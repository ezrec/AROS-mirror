// Preferences.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/memory.h>
#include <exec/semaphores.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <exec/resident.h>
#include <clib/alib_protos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/iffparse.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include <dos/dos.h>
#include <dos/dostags.h>
#include <libraries/iffparse.h>
#include <prefs/prefhdr.h>
#include <scalos/preferences.h>
#include <clib/alib_protos.h>

#include <proto/iffparse.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/preferences.h>

#include <string.h>

#include "Preferences_base.h"
#include "Preferences.h"
#include <defs.h>
#include <Year.h>

//----------------------------------------------------------------------------

#define	IS_NOT_EVEN(len)		((len) & 1)
#define	EVEN(len)			(((len) + 1) & ~1)
#define	MAGIC_PREFS_LIST_ENTRY_LIST	((UWORD) -1)

#define	min(a,b)			((a) < (b) ? (a) : (b))

#ifdef __amigaos4__
#define ExtLib(base) ((struct ExtendedLibrary *)((ULONG)base + ((struct Library *)base)->lib_PosSize))
#define IPreferences ((struct PreferencesIFace *)ExtLib(PreferencesBase)->MainIFace)
#endif

//----------------------------------------------------------------------------

struct PrefsListEntry
	{
	struct Node ple_Node;

	struct List ple_IdList;
	struct SignalSemaphore ple_Semaphore;

	char ple_Name[32];
	UWORD ple_AllocCount;
	};

struct PrefsId
	{
	struct Node pid_Node;
	ULONG pid_Id;
	struct List pid_PrefsTagList;
	};

struct PrefsTag
	{
	struct Node ptg_Node;
	ULONG ptg_Tag;

	UWORD ptg_DataSize;
	UBYTE ptg_Data[0];
	};

struct PrefsEntry
	{
	struct Node pre_Node;
	UWORD pre_DataSize;
	UBYTE pre_Data[0];
	};

#if defined(__GNUC__)
#pragma pack(2)
#endif /* __GNUC__ */

// preferences chunk, as stored on disk

struct PrefsChunk
	{
	ULONG	pck_Tag;
	UWORD	pck_DataSize;
	UBYTE	pck_Data[0];
	};

struct PrefsListChunk
	{
	UWORD	plc_DataSize;
	UBYTE	plc_Data[0];
	};

#if defined(__GNUC__)
#pragma pack()
#endif /* __GNUC__ */

//----------------------------------------------------------------------------

// Standard library functions

static struct PrefsListEntry *CreateNewPrefsListEntry(struct PreferencesBase *PreferencesBase,
	CONST_STRPTR name);
static void FreePrefsId(struct PreferencesBase *PreferencesBase, struct PrefsId *pid);
static void FreePrefsTag(struct PreferencesBase *PreferencesBase, struct PrefsTag *ptg);
static void FreePrefsEntry(struct PreferencesBase *PreferencesBase, struct PrefsEntry *pre);
static struct PrefsId *CreateNewPrefsId(struct PreferencesBase *PreferencesBase, 
	struct PrefsListEntry *ple, ULONG id);
static struct PrefsEntry *CreateNewPrefsTagListEntry(struct PreferencesBase *PreferencesBase, 
	const void *Data, size_t DataLen);
static struct PrefsTag *CreateNewPrefsTag(struct PreferencesBase *PreferencesBase, 
	struct PrefsId *pid, ULONG tag, const void *Data, UWORD StructSize);
static APTR MyAllocVecPooled(struct PreferencesBase *PreferencesBase, size_t Size);
static void MyFreeVecPooled(struct PreferencesBase *PreferencesBase, APTR mem);
static struct Node *GetNthListEntry(struct List *PrefsList, ULONG nEntry);
static LONG SameName(CONST_STRPTR Name1, CONST_STRPTR Name2, size_t MaxLen);

//----------------------------------------------------------------------------

struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase;
T_UTILITYBASE UtilityBase;
struct DosLibrary * DOSBase;
struct Library *IFFParseBase;
#ifdef __amigaos4__
struct Library *NewlibBase;
struct Interface *INewlib;
struct ExecIFace *IExec;
struct IntuitionIFace *IIntuition;
struct UtilityIFace *IUtility;
struct DOSIFace *IDOS;
struct IFFParseIFace *IIFFParse;
#endif

//----------------------------------------------------------------------------

char ALIGNED libName[] = "preferences.library";
char ALIGNED libIdString[] = "$VER: preferences.library "
        STR(LIB_VERSION) "." STR(LIB_REVISION)
	" (16 Dec 2005 21:16:25) "
	COMPILER_STRING 
	" ©2005" CURRENTYEAR " The Scalos Team";

//----------------------------------------------------------------------------


BOOL PreferencesInit(struct PreferencesBase *PreferencesBase)
{
	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  procName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, FindTask(NULL)->tc_Node.ln_Name));

	NewList(&PreferencesBase->prb_PrefsList);

	InitSemaphore(&PreferencesBase->prb_MemPoolSemaphore);
	InitSemaphore(&PreferencesBase->prb_PrefsListSem);

	d1(kprintf("%s/%s/%ld: END Success\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;	// Success
}

//-----------------------------------------------------------------------------

BOOL PreferencesOpen(struct PreferencesBase *PreferencesBase)
{
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld:   START PreferencesBase=%08lx  procName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, FindTask(NULL)->tc_Node.ln_Name));

	do	{
		IntuitionBase = (APTR) OpenLibrary( "intuition.library", 39 );
#ifdef __amigaos4__
		if (IntuitionBase)
			{
			IIntuition = (APTR) GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
			if (!IIntuition)
				{
				CloseLibrary((struct Library *)IntuitionBase);
				IntuitionBase = NULL;
				}
			}
#endif
		if (NULL == IntuitionBase)
			break;

		UtilityBase = (APTR) OpenLibrary( "utility.library", 39 );
#ifdef __amigaos4__
		if (UtilityBase)
			{
			IUtility = (APTR) GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
			if (!IUtility)
				{
				CloseLibrary((struct Library *)UtilityBase);
				UtilityBase = NULL;
				}
			}
#endif
		if (NULL == UtilityBase)
			break;

		IFFParseBase = (APTR) OpenLibrary( "iffparse.library", 39 );
#ifdef __amigaos4__
		if (IFFParseBase)
			{
			IIFFParse = (APTR) GetInterface((struct Library *)IFFParseBase, "main", 1, NULL);
			if (!IIFFParse)
				{
				CloseLibrary((struct Library *)IFFParseBase);
				IFFParseBase = NULL;
				}
			}
#endif
		if (NULL == IFFParseBase)
			break;

		DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
#ifdef __amigaos4__
		if (DOSBase)
			{
			IDOS = (APTR) GetInterface((struct Library *)DOSBase, "main", 1, NULL);
			if (!IDOS)
				{
				CloseLibrary((struct Library *)DOSBase);
				DOSBase = NULL;
				}
			}
#endif
		if (NULL == DOSBase)
			break;

#ifdef __amigaos4__
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			break;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			break;
#endif

		PreferencesBase->prb_MemPool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR, 8192, 256);
		if (NULL == PreferencesBase->prb_MemPool)
			break;

		Success = TRUE;
		} while (0);
	
	d1(kprintf("%s/%s/%ld: END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

void PreferencesCleanup(struct PreferencesBase *PreferencesBase)
{
	d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (PreferencesBase->prb_MemPool)
		{
		DeletePool(PreferencesBase->prb_MemPool);
		PreferencesBase->prb_MemPool = NULL;
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
	if (IIFFParse)
		{
		DropInterface((struct Interface *)IIFFParse);
		IIFFParse = NULL;
		}
#endif
	if (IFFParseBase)
		{
		CloseLibrary(IFFParseBase);
		IFFParseBase = NULL;
		}
#ifdef __amigaos4__
	if (IUtility)
		{
		DropInterface((struct Interface *)IUtility);
		IUtility = NULL;
		}
#endif
	if (UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
#ifdef __amigaos4__
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
#ifdef __amigaos4__
	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
}

//-----------------------------------------------------------------------------

//	prefshandle = AllocPrefsHandle( name )
//	D0                             A0
//
//	APTR AllocPrefsHandle( CONST_STRPTR );

LIBFUNC_P2(APTR, LIBAllocPrefsHandle,
	A0, CONST_STRPTR, Name,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *pleFound = NULL;

	d1(kprintf("%s/%s/%ld:   START PreferencesBase=%08lx  name=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, Name));

	ObtainSemaphore(&PreferencesBase->prb_PrefsListSem);

	if (Name)
		{
		struct PrefsListEntry *ple;

		for (ple = (struct PrefsListEntry *) PreferencesBase->prb_PrefsList.lh_Head;
			ple != (struct PrefsListEntry *) &PreferencesBase->prb_PrefsList.lh_Tail;
			ple = (struct PrefsListEntry *) ple->ple_Node.ln_Succ)
			{
			if (0 == SameName(Name, ple->ple_Name, sizeof(ple->ple_Name)))
				{
				pleFound = ple;
				break;
				}
			}
		}
	if (NULL == pleFound)
		{
		pleFound = CreateNewPrefsListEntry(PreferencesBase, Name);
		}

	if (pleFound)
		pleFound->ple_AllocCount++;

	ReleaseSemaphore(&PreferencesBase->prb_PrefsListSem);

	d1(KPrintF("%s/%s/%ld:   END PrefsHandle=%08lx\n", __FILE__, __FUNC__, __LINE__, pleFound));

	return pleFound;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	FreePrefsHandle(PrefsHandle);
//	                A0
//
//	void FreePrefsHandle(APTR);

LIBFUNC_P2(void, LIBFreePrefsHandle,
	A0, APTR, PrefsHandle,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;

	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, PrefsHandle));

	ObtainSemaphore(&PreferencesBase->prb_PrefsListSem);

	if (ple && 0 == --ple->ple_AllocCount)
		{
		struct PrefsId *pid;

		ObtainSemaphore(&ple->ple_Semaphore);

		while ((pid = (struct PrefsId *) RemHead(&ple->ple_IdList)))
			{
			FreePrefsId(PreferencesBase, pid);
			}
		ReleaseSemaphore(&ple->ple_Semaphore);
		}

	ReleaseSemaphore(&PreferencesBase->prb_PrefsListSem);

	d1(KPrintF("%s/%s/%ld:   END\n", __FILE__, __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	SetPreferences(PrefsHandle, ID, Tag, Struct, Struct_Size);
//	               A0,          D0, D1,  A1,     D2
//
//	void SetPreferences(APTR, ULONG, ULONG, const APTR, UWORD);

LIBFUNC_P6(void, LIBSetPreferences,
	A0, APTR, PrefsHandle,
	D0, ULONG, id,
	D1, ULONG, tag,
	A1, const APTR, Struct,
	D2, UWORD, StructSize,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;

	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle=%08lx  ID=%08lx  Tag=%08lx Struct=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, PrefsHandle, id, tag, Struct, StructSize));

	ObtainSemaphore(&ple->ple_Semaphore);

	do	{
		struct PrefsId *pid;
		struct PrefsId *pidFound = NULL;
		struct PrefsTag *ptg;
		struct PrefsTag *ptgFound = NULL;

		if (0 == id)
			break;
		if (0 == tag)
			break;

		for (pid = (struct PrefsId *) ple->ple_IdList.lh_Head;
			pid != (struct PrefsId *) &ple->ple_IdList.lh_Tail;
			pid = (struct PrefsId *) pid->pid_Node.ln_Succ)
			{
			if (pid->pid_Id == id)
				{
				pidFound = pid;
				break;
				}
			}
		if (NULL == pidFound)
			pidFound = CreateNewPrefsId(PreferencesBase, ple, id);

		if (NULL == pidFound)
			break;

		for (ptg = (struct PrefsTag *) pidFound->pid_PrefsTagList.lh_Head;
			ptg != (struct PrefsTag *) &pidFound->pid_PrefsTagList.lh_Tail;
			ptg = (struct PrefsTag *) ptg->ptg_Node.ln_Succ)
			{
			if (ptg->ptg_Tag == tag)
				{
				if (ptg->ptg_DataSize == StructSize)
					{
					ptgFound = ptg;
					}
				else
					{
					// correct tag, but wrong size
					// we remove the old PrefsTag and allocate a new one
					Remove(&ptg->ptg_Node);
					FreePrefsTag(PreferencesBase, ptg);
					}
				break;
				}
			}

		if (ptgFound)
			memcpy(ptg->ptg_Data, Struct, StructSize);
		else
			{
			ptgFound = CreateNewPrefsTag(PreferencesBase, pidFound, tag, Struct, StructSize);

			if (NULL == ptgFound)
				break;
			}

		} while (0);

	ReleaseSemaphore(&ple->ple_Semaphore);

	d1(KPrintF("%s/%s/%ld:   END\n", __FILE__, __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	bytescopied = GetPreferences(PrefsHandle, ID, Tag, Struct, Struct_Size);
//	D0                           A0           D0  D1   A1      D2
//
//	ULONG GetPreferences(APTR, ULONG, ULONG, const APTR, UWORD);

LIBFUNC_P6(ULONG, LIBGetPreferences,
	A0, APTR, PrefsHandle,
	D0, ULONG, id,
	D1, ULONG, tag,
	A1, APTR, Struct,
	D2, UWORD, StructSize,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;
	ULONG BytesCopied = 0;

	(void) PreferencesBase;

	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle=%08lx  ID=%08lx  Tag=%08lx Struct=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, PrefsHandle, id, tag, Struct, StructSize));

	ObtainSemaphoreShared(&ple->ple_Semaphore);

	do	{
		struct PrefsId *pid;
		struct PrefsId *pidFound = NULL;
		struct PrefsTag *ptg;
		struct PrefsTag *ptgFound = NULL;

		if (0 == id)
			break;
		if (0 == tag)
			break;

		for (pid = (struct PrefsId *) ple->ple_IdList.lh_Head;
			pid != (struct PrefsId *) &ple->ple_IdList.lh_Tail;
			pid = (struct PrefsId *) pid->pid_Node.ln_Succ)
			{
			if (pid->pid_Id == id)
				{
				pidFound = pid;
				break;
				}
			}
		if (NULL == pidFound)
			break;

		for (ptg = (struct PrefsTag *) pidFound->pid_PrefsTagList.lh_Head;
			ptg != (struct PrefsTag *) &pidFound->pid_PrefsTagList.lh_Tail;
			ptg = (struct PrefsTag *) ptg->ptg_Node.ln_Succ)
			{
			if (ptg->ptg_Tag == tag)
				{
				ptgFound = ptg;
				break;
				}
			}

		if (NULL == ptgFound)
			break;

		BytesCopied = min(StructSize, ptg->ptg_DataSize);
		memcpy(Struct, ptg->ptg_Data, BytesCopied);
		} while (0);

	ReleaseSemaphore(&ple->ple_Semaphore);

	d1(kprintf("%s/%s/%ld:   END  BytesCopied=%lu\n", __FILE__, __FUNC__, __LINE__, BytesCopied));

	return BytesCopied;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	ReadPrefsHandle(PrefsHandle, Filename);
//	                A0           A1
//
//	void ReadPrefsHandle(APTR, CONST_STRPTR);
LIBFUNC_P3(void, LIBReadPrefsHandle,
	A0, APTR, PrefsHandle,
	A1, CONST_STRPTR, Filename,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;
	struct IFFHandle *iff;
	BOOL IffOpen = FALSE;
	UBYTE *PrefsChunk = NULL;
	LONG Result;

	d1(kprintf("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle=%08lx  Filename=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, PrefsHandle, Filename));

	ObtainSemaphore(&ple->ple_Semaphore);

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			break;

		InitIFFasDOS(iff);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		iff->iff_Stream = (IPTR) Open(Filename, MODE_OLDFILE);
		if (0 == iff->iff_Stream)
			break;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		Result = OpenIFF(iff, IFFF_READ);
		if (RETURN_OK != Result)
			break;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		IffOpen = TRUE;
		} while (0);

	if (IffOpen)
		{
		d1(kprintf("%s/%s/%ld: IFF Opened\n", __FILE__, __FUNC__, __LINE__));

		do	{
			struct ContextNode *cn;
			const struct PrefsChunk *pck;
			ULONG Entry;

			Result = ParseIFF(iff, IFFPARSE_STEP);
			if (IFFERR_EOC == Result)
				continue;
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);
			if (NULL == cn)
				continue;
			if (ID_PREF != cn->cn_Type)
				continue;
			if (ID_FORM == cn->cn_ID)
				continue;
			if (ID_PRHD == cn->cn_ID)
				continue;

			PrefsChunk = MyAllocVecPooled(PreferencesBase, cn->cn_Size);
			if (NULL == PrefsChunk)
				continue;

			d1(kprintf("%s/%s/%ld: PrefsChunk=%08lx\n", __FILE__, __FUNC__, __LINE__,
				PrefsChunk));

			if (cn->cn_Size != ReadChunkBytes(iff, PrefsChunk, cn->cn_Size))
				break;

			pck = (struct PrefsChunk *) PrefsChunk;
			while (pck->pck_Tag)
				{
				d1(KPrintF("%s/%s/%ld:   pck=%08lx  Tag=%08lx  DataSize=%lu\n", \
					__FILE__, __FUNC__, __LINE__, pck, pck->pck_Tag, pck->pck_DataSize));

				if (MAGIC_PREFS_LIST_ENTRY_LIST == pck->pck_DataSize)
					{
					const struct PrefsListChunk *plc;

					d1(KPrintF("%s/%s/%ld:   MAGIC_PREFS_LIST_ENTRY_LIST\n",
						__FILE__, __FUNC__, __LINE__));

					plc = (const struct PrefsListChunk *) pck->pck_Data;
					Entry = 0;

					while (plc->plc_DataSize)
						{
						size_t len;

						d1(KPrintF("%s/%s/%ld:   plc=%08lx  DataSize=%lu\n",
							__FILE__, __FUNC__, __LINE__, plc, plc->plc_DataSize));

						SetEntry(ple, cn->cn_ID, pck->pck_Tag, (APTR) plc->plc_Data, plc->plc_DataSize, Entry);

						len = EVEN(sizeof(struct PrefsListChunk) + plc->plc_DataSize);
						plc = (const struct PrefsListChunk *) (((UBYTE *) plc) + len);
						Entry++;
						}

					pck = (const struct PrefsChunk *) (((UBYTE *) plc) + sizeof(UWORD));
					}
				else
					{
					// .stdnode
					size_t len;

					SetPreferences(ple, cn->cn_ID, pck->pck_Tag, (APTR) pck->pck_Data, pck->pck_DataSize);

					len = EVEN(sizeof(struct PrefsChunk) + pck->pck_DataSize);
					d1(KPrintF("%s/%s/%ld:   len=%lu\n", __FILE__, __FUNC__, __LINE__, len));

					pck = (const struct PrefsChunk *) (((UBYTE *) pck) + len);
					}
				}
			} while (1);
		}

	ReleaseSemaphore(&ple->ple_Semaphore);

	if (PrefsChunk)
		MyFreeVecPooled(PreferencesBase, PrefsChunk);
	if (iff)
		{
		if (IffOpen)
			CloseIFF(iff);
		if (iff->iff_Stream)
			{
			Close((BPTR) iff->iff_Stream);
			iff->iff_Stream = 0;
			}
		FreeIFF(iff);
		}

	d1(kprintf("%s/%s/%ld:   END\n", __FILE__, __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	WritePrefsHandle(PrefsHandle, Filename);
//	                A0           A1
//
//	void WritePrefsHandle(APTR, CONST_STRPTR);

LIBFUNC_P3(void, LIBWritePrefsHandle,
	A0, APTR, PrefsHandle,
	A1, CONST_STRPTR, Filename,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;
	struct IFFHandle *iff;
	BOOL IffOpen = FALSE;
	LONG Result;

	(void) PreferencesBase;

	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle=%08lx  Filename=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, PrefsHandle, Filename));

	ObtainSemaphore(&ple->ple_Semaphore);

	do	{
		static const struct PrefHeader prefHeader = { 1, 0, 0L };
		struct PrefsId *pid;

		iff = AllocIFF();
		if (NULL == iff)
			break;

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR) Open(Filename, MODE_NEWFILE);
		if (0 == iff->iff_Stream)
			break;

		Result = OpenIFF(iff, IFFF_WRITE);
		if (RETURN_OK != Result)
			break;

		IffOpen = TRUE;

		Result = PushChunk(iff, ID_PREF, ID_FORM, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		Result = PushChunk(iff, 0, ID_PRHD, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		if (WriteChunkBytes(iff, (APTR) &prefHeader, sizeof(prefHeader)) < 0)
			break;

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;

		for (pid = (struct PrefsId *) ple->ple_IdList.lh_Head;
			pid != (struct PrefsId *) &ple->ple_IdList.lh_Tail;
			pid = (struct PrefsId *) pid->pid_Node.ln_Succ)
			{
			ULONG NullDummy = 0;
			ULONG EndMark = 0;
			struct PrefsTag *ptg;

			d1(KPrintF("%s/%s/%ld: pid=%08lx  pid_Id=%08lx\n", __FILE__, __FUNC__, __LINE__, pid, pid->pid_Id));

			Result = PushChunk(iff, 0, pid->pid_Id, IFFSIZE_UNKNOWN);
			if (RETURN_OK != Result)
				break;

			for (ptg = (struct PrefsTag *) pid->pid_PrefsTagList.lh_Head;
				ptg != (struct PrefsTag *) &pid->pid_PrefsTagList.lh_Tail;
				ptg = (struct PrefsTag *) ptg->ptg_Node.ln_Succ)
				{
				UWORD PtgSize = (UWORD) ptg->ptg_DataSize;

				d1(KPrintF("%s/%s/%ld: ptg=%08lx  ptg_DataSize=%lu\n", __FILE__, __FUNC__, __LINE__, ptg, ptg->ptg_DataSize));

				if (MAGIC_PREFS_LIST_ENTRY_LIST == ptg->ptg_DataSize)
					{
					struct List *PrefsList = (struct List *) ptg->ptg_Data;;

					d1(KPrintF("%s/%s/%ld: PrefsList=%08lx\n", __FILE__, __FUNC__, __LINE__, PrefsList));

					if (!IsListEmpty(PrefsList))
						{
						struct PrefsEntry *pre;
						UWORD DataSize = 0;

						WriteChunkBytes(iff, &ptg->ptg_Tag, sizeof(ptg->ptg_Tag));
						WriteChunkBytes(iff, &PtgSize, sizeof(PtgSize));
					
						for (pre = (struct PrefsEntry *) PrefsList->lh_Head;
							pre != (struct PrefsEntry *) &PrefsList->lh_Tail;
							pre = (struct PrefsEntry *) pre->pre_Node.ln_Succ)
							{
							d1(KPrintF("%s/%s/%ld: pre=%08lx  pre_DataSize=%lu\n", __FILE__, __FUNC__, __LINE__, pre, pre->pre_DataSize));

							WriteChunkBytes(iff, &pre->pre_DataSize, sizeof(pre->pre_DataSize));
							WriteChunkBytes(iff, &pre->pre_Data, pre->pre_DataSize);
							if (IS_NOT_EVEN(pre->pre_DataSize))
								WriteChunkBytes(iff, &NullDummy, 1);
							}

						WriteChunkBytes(iff, &DataSize, sizeof(DataSize));
						}
					}
				else
					{
					// .stdnode
					WriteChunkBytes(iff, &ptg->ptg_Tag, sizeof(ptg->ptg_Tag));
					WriteChunkBytes(iff, &PtgSize, sizeof(PtgSize));
					WriteChunkBytes(iff, ptg->ptg_Data, ptg->ptg_DataSize);
					if (IS_NOT_EVEN(ptg->ptg_DataSize))
						WriteChunkBytes(iff, &NullDummy, 1);
					}
				}

			WriteChunkBytes(iff, &EndMark, sizeof(EndMark));

			Result = PopChunk(iff);
			if (RETURN_OK != Result)
				break;

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}

		if (RETURN_OK != Result)
			break;

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;
		} while (0);

	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	ReleaseSemaphore(&ple->ple_Semaphore);

	if (iff)
		{
		if (IffOpen)
			CloseIFF(iff);
		if (iff->iff_Stream)
			{
			Close((BPTR) iff->iff_Stream);
			iff->iff_Stream = 0;
			}
		FreeIFF(iff);
		}

	d1(KPrintF("%s/%s/%ld:   END\n", __FILE__, __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	prefsstruct = FindPreferences(prefshandle, ID, Tag);
//	D0                            A0           D0  D1
//
//	struct PrefsStruct *FindPreferences(APTR, ULONG, ULONG);

LIBFUNC_P4(struct PrefsStruct *, LIBFindPreferences,
	A0, APTR, PrefsHandle,
	D0, ULONG, ID,
	D1, ULONG, tag,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;
	struct PrefsStruct *ps = NULL;

	(void) PreferencesBase;

	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle=%08lx  ID=%08lx  Tag=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, PrefsHandle, ID, tag));

	ObtainSemaphoreShared(&ple->ple_Semaphore);

	do	{
		struct PrefsId *pid;
		struct PrefsId *pidFound = NULL;
		struct PrefsTag *ptg;
		struct PrefsTag *ptgFound = NULL;

		for (pid = (struct PrefsId *) ple->ple_IdList.lh_Head;
			pid != (struct PrefsId *) &ple->ple_IdList.lh_Tail;
			pid = (struct PrefsId *) pid->pid_Node.ln_Succ)
			{
			if (pid->pid_Id == ID)
				{
				pidFound = pid;
				break;
				}
			}

		if (NULL == pidFound)
			break;

		for (ptg = (struct PrefsTag *) pidFound->pid_PrefsTagList.lh_Head;
			ptg != (struct PrefsTag *) &pidFound->pid_PrefsTagList.lh_Tail;
			ptg = (struct PrefsTag *) ptg->ptg_Node.ln_Succ)
			{
			if (ptg->ptg_Tag == tag)
				{
				ptgFound = ptg;
				break;
				}
			}

		if (NULL == ptgFound)
			break;

		ps = (struct PrefsStruct *) &ptgFound->ptg_DataSize;
		} while (0);

	ReleaseSemaphore(&ple->ple_Semaphore);

	d1(KPrintF("%s/%s/%ld:   END  ps=%08lx\n", __FILE__, __FUNC__, __LINE__, ps));

	return ps;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	SetEntry(PrefsHandle, ID, Tag, Struct, Struct_Size, Entry)
//	         A0           D0  D1   A1      D2           D3
//
//	void SetEntry(APTR, ULONG, ULONG, const APTR, UWORD, ULONG);

LIBFUNC_P7(void, LIBSetEntry,
	A0, APTR, PrefsHandle,
	D0, ULONG, ID,
	D1, ULONG, tag,
	A1, const APTR, Struct,
	D2, UWORD, StructSize,
	D3, ULONG, Entry,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;

	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle=%08lx  ID=%08lx  Tag=%08lx Struct=%08lx  Size=%lu, Entry=%lu\n",
		__FILE__, __FUNC__, __LINE__, PreferencesBase, PrefsHandle, ID, tag, Struct, StructSize, Entry));

	ObtainSemaphore(&ple->ple_Semaphore);

	do	{
		struct PrefsId *pid;
		struct PrefsId *pidFound = NULL;
		struct PrefsTag *ptg;
		struct PrefsTag *ptgFound = NULL;
		struct PrefsEntry *pre;
		struct PrefsEntry *preFound;
		struct List *PrefsList;

		d1(KPrintF("%s/%s/%ld: ID=%08lx  tag=%08lx\n", __FILE__, __FUNC__, __LINE__, ID, tag));

		if (0 == ID)
			break;
		if (0 == tag)
			break;

		for (pid = (struct PrefsId *) ple->ple_IdList.lh_Head;
			pid != (struct PrefsId *) &ple->ple_IdList.lh_Tail;
			pid = (struct PrefsId *) pid->pid_Node.ln_Succ)
			{
			if (pid->pid_Id == ID)
				{
				pidFound = pid;
				break;
				}
			}

		d1(KPrintF("%s/%s/%ld: pidFound=%08lx\n", __FILE__, __FUNC__, __LINE__, pidFound));

		if (NULL == pidFound)
			pidFound = CreateNewPrefsId(PreferencesBase, ple, ID);

		d1(KPrintF("%s/%s/%ld: pidFound=%08lx\n", __FILE__, __FUNC__, __LINE__, pidFound));
		if (NULL == pidFound)
			break;

		for (ptg = (struct PrefsTag *) pidFound->pid_PrefsTagList.lh_Head;
			ptg != (struct PrefsTag *) &pidFound->pid_PrefsTagList.lh_Tail;
			ptg = (struct PrefsTag *) ptg->ptg_Node.ln_Succ)
			{
			if (ptg->ptg_Tag == tag)
				{
				ptgFound = ptg;
				break;
				}
			}

		d1(KPrintF("%s/%s/%ld: ptgFound=%08lx\n", __FILE__, __FUNC__, __LINE__, ptgFound));

		if (NULL == ptgFound)
			ptgFound = CreateNewPrefsTag(PreferencesBase, pidFound, tag, NULL, MAGIC_PREFS_LIST_ENTRY_LIST);

		d1(KPrintF("%s/%s/%ld: ptgFound=%08lx\n", __FILE__, __FUNC__, __LINE__, ptgFound));
		if (NULL == ptgFound)
			break;

		if (MAGIC_PREFS_LIST_ENTRY_LIST != ptgFound->ptg_DataSize)
			break;

		PrefsList = (struct List *) ptgFound->ptg_Data;

		preFound = (struct PrefsEntry *) GetNthListEntry(PrefsList, Entry);

		d1(KPrintF("%s/%s/%ld: PrefsList=%08lx\n", __FILE__, __FUNC__, __LINE__, PrefsList));
		d1(KPrintF("%s/%s/%ld: preFound=%08lx\n", __FILE__, __FUNC__, __LINE__, preFound));
		if (preFound)
			{
			Remove(&preFound->pre_Node);
			FreePrefsEntry(PreferencesBase, preFound);
			}
		preFound = CreateNewPrefsTagListEntry(PreferencesBase, Struct, StructSize);

		d1(KPrintF("%s/%s/%ld: preFound=%08lx\n", __FILE__, __FUNC__, __LINE__, preFound));
		if (preFound)
			{
			if (0 == Entry)
				{
				d1(KPrintF("%s/%s/%ld: Add to head\n", __FILE__, __FUNC__, __LINE__));
				AddHead(PrefsList, &preFound->pre_Node);
				}
			else
				{
				pre = (struct PrefsEntry *) GetNthListEntry(PrefsList, Entry);
				d1(KPrintF("%s/%s/%ld: pre=%08lx\n", __FILE__, __FUNC__, __LINE__, pre));
				if (pre)
					Insert(PrefsList, &preFound->pre_Node, &pre->pre_Node);
				else
					AddTail(PrefsList, &preFound->pre_Node);
				}
			}
		} while (0);

	ReleaseSemaphore(&ple->ple_Semaphore);

	d1(KPrintF("%s/%s/%ld:   END\n", __FILE__, __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	bytescopied = GetEntry(PrefsHandle, ID, Tag, Struct, Struct_Size, Entry)
//	D0                     A0           D0  D1   A1      D2           D3
//
//	ULONG GetEntry(APTR, ULONG, ULONG, APTR, UWORD, ULONG);

LIBFUNC_P7(ULONG, LIBGetEntry,
	A0, APTR, PrefsHandle,
	D0, ULONG, ID,
	D1, ULONG, tag,
	A1, APTR, Struct,
	D2, UWORD, StructSize,
	D3, ULONG, Entry,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;
	ULONG BytesCopied = 0;

	(void) PreferencesBase;

	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle=%08lx  ID=%08lx  Tag=%08lx Struct=%08lx  Size=%lu, Entry=%lu\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, PrefsHandle, ID, tag, Struct, StructSize, Entry));

	ObtainSemaphoreShared(&ple->ple_Semaphore);

	do	{
		struct PrefsId *pid;
		struct PrefsId *pidFound = NULL;
		struct PrefsTag *ptg;
		struct PrefsTag *ptgFound = NULL;
		struct PrefsEntry *preFound;
		struct List *PrefsList;

		for (pid = (struct PrefsId *) ple->ple_IdList.lh_Head;
			pid != (struct PrefsId *) &ple->ple_IdList.lh_Tail;
			pid = (struct PrefsId *) pid->pid_Node.ln_Succ)
			{
			if (pid->pid_Id == ID)
				{
				pidFound = pid;
				break;
				}
			}

		d1(KPrintF("%s/%s/%ld:   pidFound=%08lx\n", __FILE__, __FUNC__, __LINE__, pidFound));
		if (NULL == pidFound)
			break;

		for (ptg = (struct PrefsTag *) pidFound->pid_PrefsTagList.lh_Head;
			ptg != (struct PrefsTag *) &pidFound->pid_PrefsTagList.lh_Tail;
			ptg = (struct PrefsTag *) ptg->ptg_Node.ln_Succ)
			{
			if (ptg->ptg_Tag == tag)
				{
				ptgFound = ptg;
				break;
				}
			}

		d1(KPrintF("%s/%s/%ld:   ptgFound=%08lx\n", __FILE__, __FUNC__, __LINE__, ptgFound));
		if (NULL == ptgFound)
			break;

		d1(KPrintF("%s/%s/%ld:   DataSize=%lu\n", __FILE__, __FUNC__, __LINE__, ptgFound->ptg_DataSize));
		if (MAGIC_PREFS_LIST_ENTRY_LIST != ptgFound->ptg_DataSize)
			break;

		PrefsList = (struct List *) ptgFound->ptg_Data;;

		preFound = (struct PrefsEntry *) GetNthListEntry(PrefsList, Entry);
		d1(KPrintF("%s/%s/%ld:   preFound=%08lx\n", __FILE__, __FUNC__, __LINE__, preFound));
		if (preFound)
			{
			d1(KPrintF("%s/%s/%ld:   pre_Data=%08lx  pre_DataSize=%lu\n", __FILE__, __FUNC__, __LINE__, preFound->pre_Data, preFound->pre_DataSize));
			BytesCopied = min(preFound->pre_DataSize, StructSize);
			memcpy(Struct, preFound->pre_Data, BytesCopied);
			}
		} while (0);

	ReleaseSemaphore(&ple->ple_Semaphore);

	d1(KPrintF("%s/%s/%ld:   END  BytesCopied=%lu\n", __FILE__, __FUNC__, __LINE__, BytesCopied));

	return BytesCopied;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

//	success = RemEntry(PrefsHandle, ID, Tag, Entry);
//	D0                 A0           D0  D1   D2
//
//	ULONG RemEntry(APTR, ULONG, ULONG, ULONG);

LIBFUNC_P5(ULONG, LIBRemEntry,
	A0, APTR, PrefsHandle,
	D0, ULONG, ID,
	D1, ULONG, tag,
	D2, ULONG, Entry,
	A6, struct PreferencesBase *, PreferencesBase)
{
	struct PrefsListEntry *ple = (struct PrefsListEntry *) PrefsHandle;
	ULONG Success = 0;

	d1(KPrintF("%s/%s/%ld:   START PreferencesBase=%08lx  PrefsHandle=%08lx  ID=%08lx  Tag=%08lx  Entry=%lu\n", __FILE__, __FUNC__, __LINE__, \
		PreferencesBase, PrefsHandle, ID, tag, Entry));

	ObtainSemaphore(&ple->ple_Semaphore);

	do	{
		struct PrefsId *pid;
		struct PrefsId *pidFound = NULL;
		struct PrefsTag *ptg;
		struct PrefsTag *ptgFound = NULL;
		struct PrefsEntry *preFound;
		struct List *PrefsList;

		for (pid = (struct PrefsId *) ple->ple_IdList.lh_Head;
			pid != (struct PrefsId *) &ple->ple_IdList.lh_Tail;
			pid = (struct PrefsId *) pid->pid_Node.ln_Succ)
			{
			if (pid->pid_Id == ID)
				{
				pidFound = pid;
				break;
				}
			}

		if (NULL == pidFound)
			break;

		for (ptg = (struct PrefsTag *) pidFound->pid_PrefsTagList.lh_Head;
			ptg != (struct PrefsTag *) &pidFound->pid_PrefsTagList.lh_Tail;
			ptg = (struct PrefsTag *) ptg->ptg_Node.ln_Succ)
			{
			if (ptg->ptg_Tag == tag)
				{
				ptgFound = ptg;
				break;
				}
			}

		if (NULL == ptgFound)
			break;

		if (MAGIC_PREFS_LIST_ENTRY_LIST != ptgFound->ptg_DataSize)
			break;

		PrefsList = (struct List *) ptgFound->ptg_Data;;

		preFound = (struct PrefsEntry *) GetNthListEntry(PrefsList, Entry);
		if (preFound)
			{
			Success = 1;
			Remove(&preFound->pre_Node);
			FreePrefsEntry(PreferencesBase, preFound);
			}
		} while (0);

	ReleaseSemaphore(&ple->ple_Semaphore);

	d1(KPrintF("%s/%s/%ld:   END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

#if !defined(__SASC)
void exit(int x)
{
	(void) x;
	while (1)
		;
}

#if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)

static size_t stccpy(char *dest, const char *src, size_t MaxLen)
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

#endif /*__MORPHOS__*/

#endif /* !defined(__SASC) */

//-----------------------------------------------------------------------------

static struct PrefsListEntry *CreateNewPrefsListEntry(struct PreferencesBase *PreferencesBase, 
	CONST_STRPTR name)
{
	struct PrefsListEntry *ple;

	d1(KPrintF("%s/%s/%ld:   START  name=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

	ple = MyAllocVecPooled(PreferencesBase, sizeof(struct PrefsListEntry));
	if (ple)
		{
		memset(ple, 0, sizeof(struct PrefsListEntry));

		InitSemaphore(&ple->ple_Semaphore);
		NewList(&ple->ple_IdList);

		if (name)
			stccpy(ple->ple_Name, name, sizeof(ple->ple_Name));

		AddTail(&PreferencesBase->prb_PrefsList, &ple->ple_Node);
		}

	d1(KPrintF("%s/%s/%ld:   END  ple=%08lx\n", __FILE__, __FUNC__, __LINE__, ple));

	return ple;
}

//-----------------------------------------------------------------------------

static void FreePrefsId(struct PreferencesBase *PreferencesBase, struct PrefsId *pid)
{
	d1(KPrintF("%s/%s/%ld:   START  pid=%08lx\n", __FILE__, __FUNC__, __LINE__, pid));

	if (pid)
		{
		struct PrefsTag *ptg;

		while ((ptg = (struct PrefsTag *) RemHead(&pid->pid_PrefsTagList)))
			{
			FreePrefsTag(PreferencesBase, ptg);
			}

		MyFreeVecPooled(PreferencesBase, pid);
		}
	d1(KPrintF("%s/%s/%ld:   END\n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------------

static void FreePrefsTag(struct PreferencesBase *PreferencesBase, struct PrefsTag *ptg)
{
	d1(KPrintF("%s/%s/%ld:   START  ptg=%08lx\n", __FILE__, __FUNC__, __LINE__, ptg));

	if (ptg)
		{
		if (MAGIC_PREFS_LIST_ENTRY_LIST == ptg->ptg_DataSize)
			{
			struct PrefsEntry *pre;
			struct List *PrefsList = (struct List *) ptg->ptg_Data;

			while ((pre = (struct PrefsEntry *) RemTail(PrefsList)))
				FreePrefsEntry(PreferencesBase, pre);
			}

		MyFreeVecPooled(PreferencesBase, ptg);
		}
	d1(KPrintF("%s/%s/%ld:   END\n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------------

static void FreePrefsEntry(struct PreferencesBase *PreferencesBase, struct PrefsEntry *pre)
{
	d1(KPrintF("%s/%s/%ld:   START  pre=%08lx\n", __FILE__, __FUNC__, __LINE__, pre));

	if (pre)
		{
		MyFreeVecPooled(PreferencesBase, pre);
		}
	d1(kprintf("%s/%s/%ld:   END\n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------------

static struct PrefsId *CreateNewPrefsId(struct PreferencesBase *PreferencesBase, 
	struct PrefsListEntry *ple, ULONG id)
{
	struct PrefsId *pid;

	d1(KPrintF("%s/%s/%ld:   START  ple=%08lx  id=%08lx\n", __FILE__, __FUNC__, __LINE__, ple, id));

	pid = MyAllocVecPooled(PreferencesBase, sizeof(struct PrefsId));
	if (pid)
		{
		NewList(&pid->pid_PrefsTagList);
		pid->pid_Id = id;

		ObtainSemaphore(&ple->ple_Semaphore);
		AddTail(&ple->ple_IdList, &pid->pid_Node);
		ReleaseSemaphore(&ple->ple_Semaphore);
		}

	d1(KPrintF("%s/%s/%ld:   END  pid=%08lx\n", __FILE__, __FUNC__, __LINE__, pid));

	return pid;
}

//-----------------------------------------------------------------------------

static struct PrefsEntry *CreateNewPrefsTagListEntry(struct PreferencesBase *PreferencesBase, 
	const void *Data, size_t DataLen)
{
	struct PrefsEntry *pre;

	d1(kprintf("%s/%s/%ld:   END  Data=%08lx  Len=%lu\n", __FILE__, __FUNC__, __LINE__, Data, DataLen));

	pre = MyAllocVecPooled(PreferencesBase, sizeof(struct PrefsEntry) + DataLen);
	if (pre)
		{
		pre->pre_DataSize = DataLen;
		memcpy(pre->pre_Data, Data, DataLen);
		}

	d1(kprintf("%s/%s/%ld:   END  pre=%08lx\n", __FILE__, __FUNC__, __LINE__, pre));

	return pre;
}

//-----------------------------------------------------------------------------

static struct PrefsTag *CreateNewPrefsTag(struct PreferencesBase *PreferencesBase, 
	struct PrefsId *pid, ULONG tag, const void *Data, UWORD StructSize)
{
	struct PrefsTag *ptg;
	size_t DataSize;

	d1(kprintf("%s/%s/%ld:   END  pid=%08lx  tag=%08lx  Data=%08lx  Size=%lu\n", \
		__FILE__, __FUNC__, __LINE__, pid, tag, Data, StructSize));

	if (MAGIC_PREFS_LIST_ENTRY_LIST == StructSize)
		DataSize = sizeof(struct List);
	else
		DataSize = StructSize;

	ptg = MyAllocVecPooled(PreferencesBase, sizeof(struct PrefsTag) + DataSize);
	if (ptg)
		{
		ptg->ptg_DataSize = StructSize;
		ptg->ptg_Tag = tag;

		if (MAGIC_PREFS_LIST_ENTRY_LIST == StructSize)
			{
			NewList((struct List *) ptg->ptg_Data);
			}
		else
			{
			memcpy(ptg->ptg_Data, Data, StructSize);
			}

		AddTail(&pid->pid_PrefsTagList, &ptg->ptg_Node);
		}

	d1(KPrintF("%s/%s/%ld:   END  ptg=%08lx\n", __FILE__, __FUNC__, __LINE__, ptg));

	return ptg;
}

//-----------------------------------------------------------------------------

static APTR MyAllocVecPooled(struct PreferencesBase *PreferencesBase, size_t Size)
{
	APTR ptr;

	if (PreferencesBase->prb_MemPool)
		{
		ObtainSemaphore(&PreferencesBase->prb_MemPoolSemaphore);
		ptr = AllocPooled(PreferencesBase->prb_MemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&PreferencesBase->prb_MemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n",
				__FILE__, __FUNC__, __LINE__, PreferencesBase->prb_MemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, PreferencesBase->prb_MemPool, Size));

	return NULL;
}

//-----------------------------------------------------------------------------

static void MyFreeVecPooled(struct PreferencesBase *PreferencesBase, APTR mem)
{
	d1(KPrintF("%s/%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, PreferencesBase->prb_MemPool, mem));
	if (PreferencesBase->prb_MemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		d1(kprintf("%s/%s/%ld:  MemPool=%08lx  size=%lu  mem=%08lx\n",
			__FILE__, __FUNC__, __LINE__, PreferencesBase->prb_MemPool, size, &sptr[1]));

		ObtainSemaphore(&PreferencesBase->prb_MemPoolSemaphore);
		FreePooled(PreferencesBase->prb_MemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&PreferencesBase->prb_MemPoolSemaphore);
		}
}

//-----------------------------------------------------------------------------

static struct Node *GetNthListEntry(struct List *PrefsList, ULONG nEntry)
{
	ULONG Count = 0;
	struct Node *node;

	for (node = PrefsList->lh_Head;
		node != (struct Node *) &PrefsList->lh_Tail;
		node = node->ln_Succ, Count++)
		{
		if (Count == nEntry)
			{
			return node;
			}
		}

	// not found
	return NULL;
}

//-----------------------------------------------------------------------------

static LONG SameName(CONST_STRPTR Name1, CONST_STRPTR Name2, size_t MaxLen)
{
	(void) MaxLen;

	return Stricmp(Name1, Name2);
}

//-----------------------------------------------------------------------------


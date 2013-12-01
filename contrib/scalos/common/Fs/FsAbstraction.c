// FsAbstraction.c.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "int64.h"
#include "FsAbstraction.h"

//----------------------------------------------------------------------------

// local data definitions

//----------------------------------------------------------------------------

// local functions

//----------------------------------------------------------------------------

// local data

//----------------------------------------------------------------------------

BOOL ScalosExamineBegin(T_ExamineData **exd)
{
#ifdef __amigaos4__
	*exd = NULL;
	return TRUE;
#else //__amigaos4__
	*exd = AllocDosObject(DOS_FIB, NULL);
	return (BOOL) (NULL != *exd);
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

void ScalosExamineEnd(T_ExamineData **exd)
{
#ifdef __amigaos4__
	if (exd && *exd)
		{
		FreeDosObject(DOS_EXAMINEDATA, *exd);
		*exd = NULL;
		}
#else //__amigaos4__
	if (exd && *exd)
		{
		FreeDosObject(DOS_FIB, *exd);
		*exd = NULL;
		}
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

BOOL ScalosExamineLock(BPTR lock, T_ExamineData **exd)
{
#ifdef __amigaos4__
	*exd = ExamineObjectTags(EX_FileLockInput, lock,
		TAG_END);
	return (NULL != *exd);
#elif defined(__MORPHOS__) && defined(ACTION_EXAMINE_NEXT64)
	if (DOSBase->dl_lib.lib_Version >= 51)
		return Examine64(lock, *exd, NULL);
	else
		return Examine(lock, *exd);
#else //__amigaos4__
	return (BOOL) Examine(lock, *exd);
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

BOOL ScalosExamineFH(BPTR fh, T_ExamineData **exd)
{
#ifdef __amigaos4__
	*exd = ExamineObjectTags(EX_FileHandleInput, fh,
		TAG_END);
	return (NULL != *exd);
#elif defined(__MORPHOS__) && defined(ACTION_EXAMINE_NEXT64)
	if (DOSBase->dl_lib.lib_Version >= 51)
		return ExamineFH64(fh, *exd, NULL);
	else
		return ExamineFH(fh, *exd);
#else //__amigaos4__
	return ExamineFH(fh, *exd);
#endif //__amigaos4__
}
//----------------------------------------------------------------------------

struct DateStamp *ScalosExamineGetDate(T_ExamineData *exd)
{
#ifdef __amigaos4__
	return &exd->Date;
#else //__amigaos4__
	return &exd->fib_Date;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

ULONG ScalosExamineGetProtection(const T_ExamineData *exd)
{
#ifdef __amigaos4__
	return exd->Protection;
#else //__amigaos4__
	return (ULONG) exd->fib_Protection;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

ULONG64 ScalosExamineGetSize(const T_ExamineData *exd)
{
#ifdef __amigaos4__
	return exd->FileSize;
#elif defined(__MORPHOS__) && defined(ACTION_EXAMINE_NEXT64)
	if (DOSBase->dl_lib.lib_Version >= 51)
		{
		ULONG64 Size = (ULONG64) exd->fib_un.fib_un_ext.fib_un_ext_Size64;

		// workaround for MorphOS 2.1 sign-extension bug:
		// 64bit file size is negative for files with sizes between 2G and 4G
		if (0xffffffff == ULONG64_HIGH(Size))
			Size = MakeU64(ULONG64_LOW(Size));

		return Size;
		}
	else
		{
		return MakeU64(exd->fib_Size);
		}
#else //__amigaos4__
	return MakeU64(exd->fib_Size);
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

ULONG ScalosExamineGetBlockCount(const T_ExamineData *exd)
{
#ifdef __amigaos4__
	if (exd->FileSize >= 0)
		{
		// Real block count is not available, so we calculate an estimation
		return (511 + exd->FileSize) / 512;
		}
	else
		return 1;
#elif defined(__MORPHOS__)
	return exd->fib_NumBlocks;
#else //__amigaos4__
	return (ULONG) exd->fib_NumBlocks;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

CONST_STRPTR ScalosExamineGetName(const T_ExamineData *exd)
{
#ifdef __amigaos4__
	return exd->Name;
#else //__amigaos4__
	return exd->fib_FileName;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

CONST_STRPTR ScalosExamineGetComment(const T_ExamineData *exd)
{
#ifdef __amigaos4__
	return exd->Comment;
#else //__amigaos4__
	return exd->fib_Comment;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

LONG ScalosExamineGetDirEntryType(const T_ExamineData *exd)
{
	if (NULL == exd)
		return ST_FILE;
#ifdef __amigaos4__
	if( EXD_IS_FILE(exd) )
		return ST_FILE;
	else if ( EXD_IS_DIRECTORY(exd) )
		return ST_USERDIR;
	else if (EXD_IS_SOFTLINK(exd))
		return ST_SOFTLINK;
	else if (EXD_IS_PIPE(exd))
		return ST_PIPEFILE;
	else
		return ST_FILE;
#else //__amigaos4__
	return exd->fib_DirEntryType;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

LONG ScalosExamineGetDirEntryTypeRoot(const T_ExamineData *exd, BPTR fLock)
{
	if (NULL == exd)
		return ST_FILE;
#ifdef __amigaos4__
	if( EXD_IS_FILE(exd) )
		return ST_FILE;
	else if ( EXD_IS_DIRECTORY(exd) )
		{
		// Special handling (expensive!) since AmigaOS4 ExamineObject() has no means to recognize volumes
		BPTR parentLock = ParentDir(fLock);
		if (parentLock)
			{
			UnLock(parentLock);
			return ST_USERDIR;
			}
		else
			{
			return ST_ROOT;
			}
		}
	else if (EXD_IS_SOFTLINK(exd))
		return ST_SOFTLINK;
	else if (EXD_IS_PIPE(exd))
		return ST_PIPEFILE;
	else
		return ST_FILE;
#else //__amigaos4__
	return exd->fib_DirEntryType;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

LONG ScalosExamineIsDrawer(const T_ExamineData *exd)
{
#ifdef __amigaos4__
	return EXD_IS_DIRECTORY(exd) ? TRUE : FALSE;
#else //__amigaos4__
	return exd->fib_DirEntryType > 0;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

ULONG ScalosExamineGetDirUID(const T_ExamineData *exd)
{
#ifdef __amigaos4__
	return exd->OwnerUID;
#else //__amigaos4__
	return exd->fib_OwnerUID;
#endif //__amigaos4__
}
//----------------------------------------------------------------------------

ULONG ScalosExamineGetDirGID(const T_ExamineData *exd)
{
#ifdef __amigaos4__
	return exd->OwnerGID;
#else //__amigaos4__
	return exd->fib_OwnerGID;
#endif //__amigaos4__
}
//----------------------------------------------------------------------------

SLONG64 ScalosSeek(BPTR fh, SLONG64 pos, LONG mode)
{
#ifdef __amigaos4__
	int64 oldPos = GetFilePosition(fh);

	if (-1LL != oldPos && !ChangeFilePosition(fh, pos, mode))
		{
		oldPos = -1LL;
		}
	return oldPos;
#elif defined(__MORPHOS__) && defined(ACTION_EXAMINE_NEXT64)
	if (DOSBase->dl_lib.lib_Version >= 51)
		return Seek64(fh, pos, mode);
	else
		return Seek(fh, ULONG64_LOW(pos), mode);
#else //__amigaos4__

#ifdef __GNUC__
	return (SLONG64) Seek(fh, ULONG64_LOW(pos), mode);
#else //__GNUC__

	SLONG64 res64;

	SLONG64_HIGH(res64) = 0;
	SLONG64_LOW(res64) = Seek(fh, ULONG64_LOW(pos), mode);

	return res64;
#endif //__GNUC__

#endif //__amigaos4__
}

//----------------------------------------------------------------------------

BOOL ScalosExamineDirBegin(BPTR lock, T_ExamineDirHandle *edh)
{
#ifdef __amigaos4__
	*edh = ObtainDirContextTags(EX_FileLockInput, lock,
		EX_DataFields, EXF_ALL,
		TAG_END);
	return (NULL != *edh);
#elif defined(__MORPHOS__) && defined(ACTION_EXAMINE_NEXT64)
	*edh = AllocDosObject(DOS_FIB, NULL);
	if (NULL == *edh)
		return FALSE;
	if (DOSBase->dl_lib.lib_Version >= 51)
		{
		if (!Examine64(lock, *edh, NULL))
			{
			FreeDosObject(DOS_FIB, *edh);
			*edh = NULL;
			}
		}
	else
		{
		if (!Examine(lock, *edh))
			{
			FreeDosObject(DOS_FIB, *edh);
			*edh = NULL;
			}
		}
	d1(KPrintF("%s/%s/%ld:  fib=%08lx\n", __FILE__, __FUNC__, __LINE__, *edh));
	return (NULL != *edh);
#else //__amigaos4__
	*edh = AllocDosObject(DOS_FIB, NULL);
	if (NULL == *edh)
		return FALSE;
	if (!Examine(lock, *edh))
		{
		FreeDosObject(DOS_FIB, *edh);
		*edh = NULL;
		}
	return (BOOL) (NULL != *edh);
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

void ScalosExamineDirEnd(T_ExamineDirHandle *edh)
{
#ifdef __amigaos4__
	if (edh && *edh)
		{
		ReleaseDirContext(*edh);
		*edh = NULL;
		}
#else //__amigaos4__
	if (edh && *edh)
		{
		FreeDosObject(DOS_FIB, *edh);
		*edh = NULL;
		}
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

BOOL ScalosExamineDir(BPTR lock, T_ExamineDirHandle *edh, T_ExamineData **exd)
{
#ifdef __amigaos4__
	(void) lock;
	*exd = ExamineDir(*edh);
	return (NULL != *exd);
#elif defined(__MORPHOS__) && defined(ACTION_EXAMINE_NEXT64)
	if (DOSBase->dl_lib.lib_Version >= 51)
		{
		if (ExNext64(lock, *edh, NULL))
			{
			*exd = *edh;
			}
		else
			{
			return FALSE;
			}
		}
	else
		{
		if (ExNext(lock, *edh))
			{
			*exd = *edh;
			}
		else
			{
			return FALSE;
			}
		}
	return TRUE;
#else
	if (ExNext(lock, *edh))
		{
		*exd = *edh;
		}
	else
		{
		return FALSE;
		}
	return TRUE;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

BOOL ScalosReadLink(BPTR srcDirLock, CONST_STRPTR srcName, STRPTR Buffer, size_t BuffLen)
{
#ifdef __amigaos4__
	BOOL Success = FALSE;
	BPTR oldDir = CurrentDir(srcDirLock);
	struct ExamineData *exd = NULL;
	APTR dirContext= NULL;

	do	{
		if (ParsePatternNoCase(srcName, Buffer, BuffLen) < 0)
			break;

		dirContext = ObtainDirContextTags(EX_FileLockInput, srcDirLock,
				EX_DataFields, EXF_NAME | EXF_LINK | EXF_TYPE,
				EX_MatchString, Buffer,
				TAG_END);
		if (NULL == dirContext)
			break;

		exd = ExamineDir(dirContext);
		if (NULL == exd)
			break;

		if (!EXD_IS_LINK(exd))
			break;

		stccpy(Buffer, exd->Link, BuffLen);
		Success = TRUE;
		} while (0);

	if (dirContext)
		ReleaseDirContext(dirContext);

	CurrentDir(oldDir);

	return Success;
#else //__amigaos4__
	BOOL Success = FALSE;
	struct DevProc *devproc = NULL;

	do	{
		if (!NameFromLock(srcDirLock, Buffer, BuffLen))
			{
			break;
			}

		devproc = GetDeviceProc(Buffer, NULL);
		if (NULL == devproc)
			{
			break;
			}

		if (!ReadLink(devproc->dvp_Port, srcDirLock, (STRPTR) srcName,
				Buffer, BuffLen))
			{
			break;
			}
		Success = TRUE;
		} while (0);

	if (devproc)
		FreeDeviceProc(devproc);

	return Success;
#endif //__amigaos4__
}

//----------------------------------------------------------------------------

void ScalosDosPacketExamineDir(struct DosPacket *pkt, BPTR dirLock, T_ExamineDirHandle *edh)
{
#ifdef __amigaos4__
	pkt->dp_Type = ACTION_EXAMINEDATA_DIR;
	pkt->dp_Arg1 = MKBADDR(*edh);

#elif defined(__MORPHOS__) && defined(ACTION_EXAMINE_NEXT64)

	if (DOSBase->dl_lib.lib_Version >= 51)
		{
		pkt->dp_Type = ACTION_EXAMINE_NEXT64;
		pkt->dp_Arg1 = dirLock;
		pkt->dp_Arg2 = MKBADDR(*edh);
		pkt->dp_Arg3 = NULL;
		}
	else
		{
		pkt->dp_Type = ACTION_EXAMINE_NEXT;
		pkt->dp_Arg1 = dirLock;
		pkt->dp_Arg2 = MKBADDR(*edh);
		}
	return;
#else
	pkt->dp_Type = ACTION_EXAMINE_NEXT;
	pkt->dp_Arg1 = (IPTR) dirLock;
	pkt->dp_Arg2 = (IPTR) MKBADDR(*edh);
#endif
}

//----------------------------------------------------------------------------

BOOL ScalosDosPacketExamineDirResult(struct StandardPacket *pkt, T_ExamineData **exd,
	T_ExamineDirHandle *edh)
{
#ifdef __amigaos4__
	if (pkt->sp_Pkt.dp_Res1)
		{
		*exd = (T_ExamineData *) pkt->sp_Pkt.dp_Res1;
		}
	else
		*exd = NULL;

	return 0 != pkt->sp_Pkt.dp_Res1;
#else
	*exd = (T_ExamineData *) *edh;

	return (BOOL) (0 != pkt->sp_Pkt.dp_Res1);
#endif
}

//----------------------------------------------------------------------------

BOOL ScalosSupportsExamineDir(struct MsgPort *fileSysPort, BPTR dirLock, T_ExamineDirHandle *edh)
{
#ifdef __amigaos4__
	LONG rc;

	rc = DoPkt1(fileSysPort,
		ACTION_EXAMINEDATA_DIR,
		MKBADDR(edh));

	d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
	return (BOOL) (0 != rc);
#elif defined(__MORPHOS__) && defined(ACTION_EXAMINE_NEXT64)
	LONG rc;

	d1(KPrintF("%s/%s/%ld:  fib=%08lx\n", __FILE__, __FUNC__, __LINE__, *edh));

	rc = DoPkt3(fileSysPort,
		ACTION_EXAMINE_OBJECT64,
		dirLock,
		MKBADDR(*edh),
		NULL);

	d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
	return (BOOL) (0 != rc);
#else
	return FALSE;
#endif /* __MORPHOS__ */
}

//----------------------------------------------------------------------------



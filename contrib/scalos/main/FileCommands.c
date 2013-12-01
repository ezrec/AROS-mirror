// FileCommands.c
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
#include <proto/utility.h>
#include <proto/intuition.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"
#include "int64.h"
#include "FileTransClass.h"

//----------------------------------------------------------------------------

enum CopyErrReqResult { COPYERR_Retry = 1, COPYERR_Skip, COPYERR_Abort = 0, COPYERR_nop = ~0 };

struct GlobalCopyArgs
	{
	enum ExistsReqResult gca_Replace;
	};

typedef LONG (*ENTRYFUNC)(Class *cl, Object *o, 
	struct GlobalCopyArgs *gca, BPTR SrcDirLock, BPTR DestDirLock, 
	const T_ExamineData *fib, CONST_STRPTR DestName, LONG DirEntryType, CONST_STRPTR eName);

//----------------------------------------------------------------------------

// local functions

static LONG MoveCommand_Move(Class *cl, Object *o, 
	BPTR SrcDirLock, BPTR DestDirLock, CONST_STRPTR FileName);
static LONG MoveCommand_CopyAndDelete(Class *cl, Object *o, 
	BPTR SrcDirLock, BPTR DestDirLock, CONST_STRPTR FileName);
static LONG CopyFile(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcParentLock, BPTR DestParentLock, CONST_STRPTR SrcName, CONST_STRPTR DestName);
static LONG CopyLink(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcParentLock, BPTR DestParentLock, CONST_STRPTR SrcName, CONST_STRPTR DestName);
static LONG CopyDir(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcParentLock, BPTR DestParentLock, CONST_STRPTR SrcName, CONST_STRPTR DestName,
	ENTRYFUNC EntryFunc);
static LONG CopyVolume(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcParentLock, BPTR DestParentLock, CONST_STRPTR SrcName, CONST_STRPTR DestName);
static LONG CopyEntry(Class *cl, Object *o,
	struct GlobalCopyArgs *gca, BPTR SrcDirLock, BPTR DestDirLock, 
	const T_ExamineData *fib, CONST_STRPTR DestName, LONG DirEntryType, CONST_STRPTR eName);
static LONG ReportError(Class *cl, Object *o, LONG *Result, ULONG BodyTextID, ULONG GadgetTextID);
static LONG DeleteEntry(Class *cl, Object *o, BPTR parentLock, CONST_STRPTR Name, BOOL DoCount);
static LONG ScaDeleteFile(Class *cl, Object *o, BPTR ParentLock, CONST_STRPTR Name);
static LONG ScaDeleteDir(Class *cl, Object *o, BPTR ParentLock, CONST_STRPTR Name, BOOL DoCount);
static LONG CountEntry(Class *cl, Object *o, BPTR srcDirLock, T_ExamineData *fib,
	BOOL recursive, LONG DirEntryType, CONST_STRPTR eName);
static LONG FileTrans_CountDir(Class *cl, Object *o, BPTR parentLock, CONST_STRPTR Name);
static LONG CopyAndDeleteEntry(Class *cl, Object *o, 
	struct GlobalCopyArgs *gca, BPTR SrcDirLock, BPTR DestDirLock,
	const T_ExamineData *fib, CONST_STRPTR DestName, LONG DirEntryType, CONST_STRPTR eName);
static LONG RememberError(Class *cl, Object *o, CONST_STRPTR FileName,
	enum FileTransTypeAction LastAction, enum FileTransOperation LastOp);
static void ClearError(Class *cl, Object *o);
static LONG RememberExNextError(Class *cl, Object *o, CONST_STRPTR FileName,
	enum FileTransTypeAction LastAction, enum FileTransOperation LastOp);
static LONG HandleWriteProtectError(Class *cl, Object *o, BPTR DirLock,
	CONST_STRPTR FileName, enum WriteProtectedReqType Type, LONG ErroCode);
static LONG CopyLinkContents(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcDirLock, BPTR DestDirLock, CONST_STRPTR DestName, CONST_STRPTR LinkDest);

//----------------------------------------------------------------------------

// local Data

//----------------------------------------------------------------------------

LONG CreateLinkCommand(Class *cl, Object *o, BPTR SrcDirLock, BPTR DestDirLock,
	CONST_STRPTR SrcFileName, CONST_STRPTR DestFileName)
{
	LONG Result = RETURN_OK;
	LONG reqResult = COPYERR_nop;
	STRPTR Buffer = NULL;
	BPTR oldDir;
	BPTR fLock = BNULL;
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld: ftci=%08lx\n", __FILE__, __FUNC__, __LINE__, ftci));
	debugLock_d1(SrcDirLock);
	debugLock_d1(DestDirLock);
	d1(KPrintF("%s/%s/%ld: SrcFileName=<%s>\n", __FILE__, __FUNC__, __LINE__, SrcFileName));
	d1(KPrintF("%s/%s/%ld: DestFileName=<%s>\n", __FILE__, __FUNC__, __LINE__, DestFileName));

	if (!ExistsObject(SrcDirLock, SrcFileName))
		{
		// silently return if source object is non-existing
		d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, ERROR_OBJECT_NOT_FOUND));
		return ERROR_OBJECT_NOT_FOUND;
		}

	if (SCCV_ReplaceMode_Abort == ftci->ftci_ReplaceMode)
		{
		d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, RETURN_WARN));
		return RETURN_WARN;
		}
	
	if (ExistsObject(DestDirLock, DestFileName))
		{
		LONG existsResult = EXISTREQ_Replace;

		switch (ftci->ftci_ReplaceMode)
			{
		case SCCV_ReplaceMode_Ask:
			existsResult = DoMethod(o, SCCM_FileTrans_OverwriteRequest,
				OVERWRITEREQ_Move,
				SrcDirLock, SrcFileName,
				DestDirLock, DestFileName,
				ftci->ftci_Window, 
				MSGID_EXISTSNAME_MOVE, MSGID_EXISTS_GNAME_NEW);
			break;

		case SCCV_ReplaceMode_Never:
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
			return RETURN_OK;
			break;

		case SCCV_ReplaceMode_Always:
			existsResult = EXISTREQ_ReplaceAll;
			break;
			}

		d1(kprintf("%s/%s/%ld: existsResult=%ld\n", __FILE__, __FUNC__, __LINE__, existsResult));

		switch (existsResult)
			{
		case EXISTREQ_ReplaceAll:
			ftci->ftci_ReplaceMode = SCCV_ReplaceMode_Always;
			/* no break here !! */

		case EXISTREQ_Replace:
			do	{
				Result = DeleteEntry(cl, o, DestDirLock, DestFileName, FALSE);

				if (Result != RETURN_OK && Result != RESULT_UserAborted)
					reqResult = ReportError(cl, o, &Result, MSGID_DELETEERRORNAME, MSGID_COPYERRORGNAME);
				} while (RETURN_OK == Result && COPYERR_Retry == reqResult);

			if (COPYERR_Abort == reqResult)
				{
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, RETURN_WARN));
				return RETURN_WARN;
				}
			break;

		case EXISTREQ_SkipAll:
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
			ftci->ftci_ReplaceMode = SCCV_ReplaceMode_Never;
			return RETURN_OK;
			break;

		case EXISTREQ_Skip:
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
			return RETURN_OK;
			break;

		case EXISTREQ_Abort:
			ftci->ftci_ReplaceMode = SCCV_ReplaceMode_Abort;
			
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, RETURN_WARN));
			return RETURN_WARN;
			break;
			}
		}

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_EnterNewDir,
		SrcDirLock, DestDirLock, SrcFileName);

	oldDir = CurrentDir(DestDirLock);

	do	{
		BOOL createSoftLink = CurrentPrefs.pref_CreateSoftLinks;
		reqResult = COPYERR_nop;

		switch (ScaSameLock(SrcDirLock, DestDirLock))
			{
		case LOCK_SAME:
		case LOCK_SAME_VOLUME:
			break;
		case LOCK_DIFFERENT:
			// ALWAYS create soft links to different volumes!
			createSoftLink = TRUE;
			break;
			}

		do	{
			if (createSoftLink)
				{
				Buffer = AllocPathBuffer();
				d1(kprintf("%s/%s/%ld: Buffer=%08lx\n", __FILE__, __FUNC__, __LINE__, Buffer));
				if (NULL == Buffer)
					{
					Result = ERROR_NO_FREE_STORE;
					break;
					}

				if (!NameFromLock(SrcDirLock, Buffer, Max_PathLen))
					{
					Result = RememberError(cl, o, SrcFileName, ftta_CreateSoftLink, fto_Lock);
					break;
					}
				if (!AddPart(Buffer, (STRPTR) SrcFileName, Max_PathLen))
					{
					Result = RememberError(cl, o, SrcFileName, ftta_CreateSoftLink, fto_AddPart);
					break;
					}

				d1(kprintf("%s/%s/%ld: SrcFileName=<%s> Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, SrcFileName, Buffer));

				if (!MakeLink((STRPTR) DestFileName, Buffer, LINK_SOFT))
					{
					Result = RememberError(cl, o, DestFileName, ftta_CreateSoftLink, fto_MakeLink);
					break;
					}
				}
			else
				{
				CurrentDir(SrcDirLock);

				fLock = Lock((STRPTR) SrcFileName, ACCESS_READ);
				if ((BPTR)NULL == fLock)
					{
					Result = RememberError(cl, o, SrcFileName, ftta_CreateHardLink, fto_Lock);
					break;
					}

				CurrentDir(DestDirLock);

				if (!MakeLink((STRPTR) DestFileName, fLock, LINK_HARD))
					{
					Result = RememberError(cl, o, DestFileName, ftta_CreateHardLink, fto_MakeLink);
					break;
					}
				}
			} while (0);

		if (Result != RETURN_OK && Result != RESULT_UserAborted)
			reqResult = ReportError(cl, o, &Result, MSGID_CREATE_LINK_ERRORNAME, MSGID_COPYERRORGNAME);

		} while (RETURN_OK == Result && COPYERR_Retry == reqResult);

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Final,
		SrcDirLock, DestDirLock, SrcFileName);

	if (RESULT_UserAborted == Result)
		{
		d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, RETURN_WARN));
		Result = RETURN_WARN;
		}

	CurrentDir(oldDir);

	if (fLock)
		UnLock(fLock);
	if (Buffer)
		FreePathBuffer(Buffer);
		
	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------


LONG MoveCommand(Class *cl, Object *o, BPTR SrcDirLock, BPTR DestDirLock, CONST_STRPTR FileName)
{
	LONG Result = RETURN_OK;
	LONG reqResult = COPYERR_nop;
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);

	d1(kprintf("%s/%s/%ld: ftci=%08lx\n", __FILE__, __FUNC__, __LINE__, ftci));

	if (!ExistsObject(SrcDirLock, FileName))
		{
		// silently return if source object is non-existing
		d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
		return RETURN_OK;
		}

	if (SCCV_ReplaceMode_Abort == ftci->ftci_ReplaceMode)
		{
		d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_WARN));
		return RETURN_WARN;
		}
	
	if (ExistsObject(DestDirLock, FileName))
		{
		LONG existsResult = EXISTREQ_Replace;

		switch (ftci->ftci_ReplaceMode)
			{
		case SCCV_ReplaceMode_Ask:
			existsResult = DoMethod(o, SCCM_FileTrans_OverwriteRequest,
				OVERWRITEREQ_Move,
				SrcDirLock, FileName,
				DestDirLock, FileName,
				ftci->ftci_Window, 
				MSGID_EXISTSNAME_MOVE, MSGID_EXISTS_GNAME_NEW);
			break;

		case SCCV_ReplaceMode_Never:
			d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
			return RETURN_OK;
			break;

		case SCCV_ReplaceMode_Always:
			existsResult = EXISTREQ_ReplaceAll;
			break;
			}

		d1(kprintf("%s/%s/%ld: existsResult=%ld\n", __FILE__, __FUNC__, __LINE__, existsResult));

		switch (existsResult)
			{
		case EXISTREQ_ReplaceAll:
			ftci->ftci_ReplaceMode = SCCV_ReplaceMode_Always;
			/* no break here !! */

		case EXISTREQ_Replace:
			do	{
				Result = DeleteEntry(cl, o, DestDirLock, FileName, FALSE);

				if (Result != RETURN_OK && Result != RESULT_UserAborted)
					reqResult = ReportError(cl, o, &Result, MSGID_DELETEERRORNAME, MSGID_COPYERRORGNAME);
				} while (RETURN_OK == Result && COPYERR_Retry == reqResult);

			if (COPYERR_Abort == reqResult)
				{
				d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_WARN));
				return RETURN_WARN;
				}
			break;

		case EXISTREQ_SkipAll:
			d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
			ftci->ftci_ReplaceMode = SCCV_ReplaceMode_Never;
			return RETURN_OK;
			break;

		case EXISTREQ_Skip:
			d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
			return RETURN_OK;
			break;

		case EXISTREQ_Abort:
			ftci->ftci_ReplaceMode = SCCV_ReplaceMode_Abort;
			
			d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_WARN));
			return RETURN_WARN;
			break;
			}
		}

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_EnterNewDir,
		SrcDirLock, DestDirLock, FileName);

	do	{
		reqResult = COPYERR_nop;

		switch (ScaSameLock(SrcDirLock, DestDirLock))
			{
		case LOCK_SAME:
			break;
		case LOCK_SAME_VOLUME:
			Result = MoveCommand_Move(cl, o, SrcDirLock, DestDirLock, FileName);
			break;
		case LOCK_DIFFERENT:
			Result = MoveCommand_CopyAndDelete(cl, o, SrcDirLock, DestDirLock, FileName);
			break;
			}

		if (Result != RETURN_OK && Result != RESULT_UserAborted)
			reqResult = ReportError(cl, o, &Result, MSGID_MOVEERRORNAME, MSGID_COPYERRORGNAME);

		} while (RETURN_OK == Result && COPYERR_Retry == reqResult);

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Final,
		SrcDirLock, DestDirLock, FileName);

	if (RESULT_UserAborted == Result)
		{
		d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_WARN));
		return RETURN_WARN;
		}

	d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG MoveCommand_Move(Class *cl, Object *o, 
	BPTR SrcDirLock, BPTR DestDirLock, CONST_STRPTR FileName)
{
	LONG Result = RETURN_OK;
	char *OldFileName, *NewFileName = NULL;

	do	{
		OldFileName = AllocPathBuffer();
		if (NULL == OldFileName)
			{
			SetIoErr(ERROR_NO_FREE_STORE);
			Result = RememberError(cl, o, FileName, ftta_Move, fto_AllocPathBuffer);
			break;
			}

		NewFileName = AllocPathBuffer();
		if (NULL == NewFileName)
			{
			SetIoErr(ERROR_NO_FREE_STORE);
			Result = RememberError(cl, o, FileName, ftta_Move, fto_AllocPathBuffer);
			break;
			}

		if (!NameFromLock(SrcDirLock, OldFileName, Max_PathLen - 1))
			{
			Result = RememberError(cl, o, FileName, ftta_Move, fto_NameFromLock);
			break;
			}
		if (!NameFromLock(DestDirLock, NewFileName, Max_PathLen - 1))
			{
			Result = RememberError(cl, o, FileName, ftta_Move, fto_NameFromLock);
			break;
			}

		if (!AddPart(OldFileName, (STRPTR) FileName, Max_PathLen - 1))
			{
			Result = RememberError(cl, o, FileName, ftta_Move, fto_AddPart);
			break;
			}
		if (!AddPart(NewFileName, (STRPTR) FileName, Max_PathLen - 1))
			{
			Result = RememberError(cl, o, FileName, ftta_Move, fto_AddPart);
			break;
			}

		if (!Rename(OldFileName, NewFileName))
			{
			Result = RememberError(cl, o, FileName, ftta_Move, fto_Rename);
			if (ERROR_WRITE_PROTECTED == Result
				|| ERROR_DELETE_PROTECTED == Result)
				{
				enum WriteProtectedReqResult overwrite;

				overwrite = HandleWriteProtectError(cl, o, SrcDirLock, OldFileName, WRITEPROTREQ_Move, Result);
				Result = RETURN_OK;
				switch (overwrite)
					{
				case WRITEPROTREQ_ReplaceAll:
				case WRITEPROTREQ_Replace:
					SetProtection(OldFileName, 0);
					if (!Rename(OldFileName, NewFileName))
						Result = RememberError(cl, o, FileName, ftta_Move, fto_Rename);
					break;
				default:
					break;
					}
				if (WRITEPROTREQ_Abort == overwrite)
					Result = RESULT_UserAborted;
				}
			}
		if (RETURN_OK != Result)
			break;
		} while (0);

	if (OldFileName)
		FreePathBuffer(OldFileName);
	if (NewFileName)
		FreePathBuffer(NewFileName);

	return Result;
}


static LONG MoveCommand_CopyAndDelete(Class *cl, Object *o,
	BPTR SrcDirLock, BPTR DestDirLock, CONST_STRPTR FileName)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	STRPTR allocSrcFileName = NULL;
	BPTR oldDir;
	BPTR fLock = (BPTR)NULL;
	BPTR parentLock = (BPTR)NULL;
	T_ExamineData *fib;
	LONG Result;
	BOOL SourceIsVolume = FALSE;
	STRPTR allocatedName = NULL;
	struct GlobalCopyArgs gca;

	d1(kprintf("\n" "%s/%s/%ld: FileName=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
		FileName, FileName ? FileName : (CONST_STRPTR) ""));

	d1(kprintf("%s/%s/%ld: ftci=%08lx\n", __FILE__, __FUNC__, __LINE__, ftci));

	debugLock_d1(SrcDirLock);
	debugLock_d1(DestDirLock);

	if (SCCV_ReplaceMode_Abort == ftci->ftci_ReplaceMode)
		return RESULT_UserAborted;

	gca.gca_Replace = EXISTREQ_Skip;

	do	{
		CONST_STRPTR eName;
		LONG DirEntryType;

		oldDir = CurrentDir(SrcDirLock);

		if (!ScalosExamineBegin(&fib))
			{
			Result = RememberError(cl, o, FileName, ftta_CopyAndDelete, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (0 == strlen(FileName))
			{
			// Source is an absolute filename, i.e. "Volume:"
			SourceIsVolume = TRUE;
			}

		fLock = Lock((STRPTR) FileName, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			{
			Result = RememberError(cl, o, FileName, ftta_CopyAndDelete, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		d1(kprintf("%s/%s/%ld: FileName=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
			FileName, FileName ? FileName : (CONST_STRPTR) ""));

		debugLock_d1(fLock);

		if (!ScalosExamineLock(fLock, &fib))
			{
			Result = RememberError(cl, o, FileName, ftta_CopyAndDelete, fto_Examine);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (SourceIsVolume)
			{
			DirEntryType = ST_ROOT;
			eName = allocatedName = ScalosAlloc(2 + strlen(ScalosExamineGetName(fib)));
			if (eName)
				{
				strcpy(allocatedName, ScalosExamineGetName(fib));
				strcat(allocatedName, ":");
				}
			}
		else
			{
			eName = ScalosExamineGetName(fib);
			DirEntryType = ScalosExamineGetDirEntryType(fib);
			parentLock = ParentDir(fLock);
			if ((BPTR)NULL == parentLock)
				{
				Result = RememberError(cl, o, FileName, ftta_CopyAndDelete, fto_Lock);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}
			}

		UnLock(fLock);
		fLock = (BPTR)NULL;

		DoMethod(o, SCCM_FileTrans_UpdateWindow,
			FTUPDATE_EnterNewDir,
			SrcDirLock, DestDirLock,
			eName);

		Result = CopyAndDeleteEntry(cl, o, &gca, SrcDirLock,
			DestDirLock, fib,
			FileName, DirEntryType, eName);
		} while (0);

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Final,
		SrcDirLock, DestDirLock, FileName);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	CurrentDir(oldDir);
	if (fLock)
		UnLock(fLock);
	if (parentLock)
		UnLock(parentLock);
	ScalosExamineEnd(&fib);
	if (allocSrcFileName)
		FreePathBuffer(allocSrcFileName);
	if (allocatedName)
		ScalosFree(allocatedName);

	if (RESULT_UserAborted == Result)
		Result = RESULT_UserAborted;

	return Result;
}


// SrcFileName may be "", then SourceDirLock is an absolute filename, i.e. "Volume:"
// returns	RETURN_OK or error code
LONG CopyCommand(Class *cl, Object *o, BPTR SrcDirLock, BPTR DestDirLock,
	CONST_STRPTR SrcFileName, CONST_STRPTR DestFileName)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	STRPTR allocSrcFileName = NULL;
	BPTR oldDir;
	BPTR fLock = (BPTR)NULL;
	T_ExamineData *fib;
	LONG Result;
	BOOL SourceIsVolume = FALSE;
	STRPTR allocatedName = NULL;
	struct GlobalCopyArgs gca;

	d1(kprintf("\n" "%s/%s/%ld: SrcFileName=%08lx <%s>  DestFileName=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
		SrcFileName, SrcFileName ? SrcFileName : (CONST_STRPTR) "", \
		DestFileName, DestFileName ? DestFileName : (CONST_STRPTR) ""));

	d1(kprintf("%s/%s/%ld: ftci=%08lx\n", __FILE__, __FUNC__, __LINE__, ftci));

	debugLock_d1(SrcDirLock);
	debugLock_d1(DestDirLock);

	if (SCCV_ReplaceMode_Abort == ftci->ftci_ReplaceMode)
		return RETURN_WARN;

	if (!ExistsObject(SrcDirLock, SrcFileName))
		{
		// silently return if source object is non-existing
		d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
		return RETURN_OK;
		}

	gca.gca_Replace = EXISTREQ_Skip;

	do	{
		CONST_STRPTR eName;
		LONG DirEntryType;

		oldDir = CurrentDir(SrcDirLock);

		if (!ScalosExamineBegin(&fib))
			{
			Result = RememberError(cl, o, SrcFileName, ftta_Copy, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (0 == strlen(SrcFileName))
			{
			// Source is an absolute filename, i.e. "Volume:"
			SourceIsVolume = TRUE;
			}

		fLock = Lock((STRPTR) SrcFileName, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			{
			Result = RememberError(cl, o, SrcFileName, ftta_Copy, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		d1(kprintf("%s/%s/%ld: SrcFileName=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
			SrcFileName, SrcFileName ? SrcFileName : (CONST_STRPTR) ""));

		debugLock_d1(fLock);

		if (!ScalosExamineLock(fLock, &fib))
			{
			Result = RememberError(cl, o, SrcFileName, ftta_Copy, fto_Examine);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}
		eName = ScalosExamineGetName(fib);
		DirEntryType = ScalosExamineGetDirEntryType(fib);

		if (SourceIsVolume)
			{
			DirEntryType = ST_ROOT;
			eName = allocatedName = ScalosAlloc(2 + strlen(ScalosExamineGetName(fib)));
			if (eName)
				{
				strcpy(allocatedName, ScalosExamineGetName(fib));
				strcat(allocatedName, ":");
				}
			}
		else if (IsSoftLink(SrcFileName))
			{
			DirEntryType = ST_SOFTLINK;
			eName = allocatedName = ScalosAlloc(1 + strlen(SrcFileName));
			if (eName)
				{
				strcpy(allocatedName, SrcFileName);
				}
			}

		DoMethod(o, SCCM_FileTrans_UpdateWindow,
			FTUPDATE_EnterNewDir,
			SrcDirLock, DestDirLock,
			eName);

		d1(kprintf("%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));

		Result = CopyEntry(cl, o, &gca, SrcDirLock, 
				DestDirLock, fib, DestFileName,
				DirEntryType, eName);
		} while (0);

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Final,
		SrcDirLock, DestDirLock, DestFileName);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	CurrentDir(oldDir);
	if (fLock)
		UnLock(fLock);
	ScalosExamineEnd(&fib);
	if (allocSrcFileName)
		FreePathBuffer(allocSrcFileName);
	if (allocatedName)
		ScalosFree(allocatedName);

	if (RESULT_UserAborted == Result)
		Result = RETURN_WARN;

	return Result;
}


LONG DeleteCommand(Class *cl, Object *o, BPTR DirLock, CONST_STRPTR FileName)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	BPTR oldDir;
	BPTR fLock = (BPTR)NULL;
	T_ExamineData *fib;
	LONG Result;

	d1(kprintf("\n" "%s/%s/%ld: FileName=%08lx <%s>  \n", __FILE__, __FUNC__, __LINE__, \
		FileName, FileName ? FileName : (CONST_STRPTR) "" ));

	d1(kprintf("%s/%s/%ld: ftci=%08lx\n", __FILE__, __FUNC__, __LINE__, ftci));

	debugLock_d1(DirLock);

	if (SCCV_ReplaceMode_Abort == ftci->ftci_ReplaceMode)
		return RETURN_WARN;

	if (!ExistsObject(DirLock, FileName))
		{
		// silently return if source object is non-existing
		d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
		return RETURN_OK;
		}

	do	{
		CONST_STRPTR eName;

		oldDir = CurrentDir(DirLock);

		if (!ScalosExamineBegin(&fib))
			{
			Result = RememberError(cl, o, FileName, ftta_Delete, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		fLock = Lock((STRPTR) FileName, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			{
			Result = RememberError(cl, o, FileName, ftta_Delete, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		d1(kprintf("%s/%s/%ld: FileName=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
			FileName, FileName ? FileName : (CONST_STRPTR) ""));

		debugLock_d1(fLock);

		if (!ScalosExamineLock(fLock, &fib))
			{
			Result = RememberError(cl, o, FileName, ftta_Delete, fto_Examine);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}
		eName = ScalosExamineGetName(fib);

		DoMethod(o, SCCM_FileTrans_UpdateWindow,
			FTUPDATE_EnterNewDir,
			DirLock, DirLock,
			eName);

		d1(kprintf("%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));

		CurrentDir(oldDir);
		oldDir = BNULL;
		UnLock(fLock);
		fLock = BNULL;

		Result = DeleteEntry(cl, o, DirLock, FileName, TRUE);
		} while (0);

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Final,
		DirLock, DirLock, FileName);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	if (oldDir)
		CurrentDir(oldDir);
	if (fLock)
		UnLock(fLock);

	ScalosExamineEnd(&fib);

	if (RESULT_UserAborted == Result)
		Result = RETURN_WARN;

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


// SrcParentLock	: Lock to source parent directory
// DestParentLock	: Lock to destination parent directory
// Name			: Name of source dir (relative to SrcParentLock)

// returns 		: RETURN_OK or DOS error code
static LONG CopyFile(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcParentLock, BPTR DestParentLock, CONST_STRPTR SrcName, CONST_STRPTR DestName)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result = RETURN_OK;
	UBYTE *Buffer = NULL;
	BPTR fdSrc, fdDest = (BPTR)NULL;
	BPTR oldDir;

	d1(kprintf("%s/%s/%ld: SrcName=<%s>  DestName=<%s>\n", __FILE__, __FUNC__, __LINE__, SrcName, DestName));
	debugLock_d1(SrcParentLock);
	debugLock_d1(DestParentLock);

	do	{
		LONG Actual;

		oldDir = CurrentDir(SrcParentLock);

		fdSrc = Open((STRPTR) SrcName, MODE_OLDFILE);
		if ((BPTR)NULL == fdSrc)
			{
			Result = RememberError(cl, o, SrcName, ftta_CopyFile, fto_Open);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		Buffer = AllocVec(CurrentPrefs.pref_CopyBuffLen, MEMF_PUBLIC);
		if (NULL == Buffer)
			{
			Result = ERROR_NO_FREE_STORE;
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		CurrentDir(DestParentLock);

		fdDest = Open((STRPTR) DestName, MODE_NEWFILE);
		if ((BPTR)NULL == fdDest)
			{
			Result = RememberError(cl, o, DestName, ftta_CopyFile, fto_Open);
			d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (ERROR_WRITE_PROTECTED == Result
				|| ERROR_DELETE_PROTECTED == Result)
				{
				enum WriteProtectedReqResult overwrite;

				overwrite = HandleWriteProtectError(cl, o, DestParentLock, DestName, WRITEPROTREQ_Copy, Result);
				Result = RETURN_OK;
				switch (overwrite)
					{
				case WRITEPROTREQ_ReplaceAll:
				case WRITEPROTREQ_Replace:
					SetProtection(DestName, 0);
					fdDest = Open((STRPTR) DestName, MODE_NEWFILE);
					break;
				default:
					break;
					}
				if (WRITEPROTREQ_Abort == overwrite)
					{
					Result = RESULT_UserAborted;
					break;
					}
				}
			}

		if ((BPTR)NULL == fdDest)
			break;

		do	{
			Actual = Read(fdSrc, Buffer, CurrentPrefs.pref_CopyBuffLen);
			if (Actual < 0)
				{
				Result = RememberError(cl, o, SrcName, ftta_CopyFile, fto_Read);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}
			if (Actual > 0)
				{
				LONG Written = Write(fdDest, Buffer, Actual);

				ftci->ftci_CurrentBytes = Add64(ftci->ftci_CurrentBytes, MakeU64(Written));

				if (Written != Actual)
					{
					Result = RememberError(cl, o, DestName, ftta_CopyFile, fto_Write);
					d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
					break;
					}
				}

			DoMethod(o, SCCM_FileTrans_UpdateWindow,
				FTUPDATE_SameFile,
				SrcParentLock, DestParentLock, SrcName);
			if (RETURN_OK == Result)
				Result = DoMethod(o, SCCM_FileTrans_CheckAbort);
			} while (Actual && RETURN_OK == Result);

		if (RETURN_OK != Result)
			{
			// Remove destination after error

			// no need to check fdDest -- file MUST have been opened successfully!
			Close(fdDest);
			fdDest = (BPTR)NULL;

			// don't check for errors here
			(void) ScaDeleteFile(cl, o, DestParentLock, DestName);
			}
		} while (0);

	if (Buffer)
		FreeVec(Buffer);
	if (fdSrc)
		Close(fdSrc);
	if (fdDest)
		Close(fdDest);
	CurrentDir(oldDir);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


// SrcParentLock	: Lock to source parent directory
// DestParentLock	: Lock to destination parent directory
// Name			: Name of source dir (relative to SrcParentLock)

// returns 		: RETURN_OK or DOS error code
static LONG CopyLink(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcParentLock, BPTR DestParentLock, CONST_STRPTR SrcName, CONST_STRPTR DestName)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result = RETURN_OK;
	STRPTR Buffer;
	BPTR oldDir;

	d1(kprintf("%s/%s/%ld: SrcName=<%s>  DestName=<%s>\n", __FILE__, __FUNC__, __LINE__, SrcName, DestName));
	debugLock_d1(SrcParentLock);
	debugLock_d1(DestParentLock);

	do	{
		oldDir = CurrentDir(SrcParentLock);

		Buffer = AllocPathBuffer();
		d1(kprintf("%s/%s/%ld: Buffer=%08lx\n", __FILE__, __FUNC__, __LINE__, Buffer));
		if (NULL == Buffer)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		if (!ScalosReadLink(SrcParentLock, SrcName, Buffer, Max_PathLen))
			{
			Result = RememberError(cl, o, SrcName, ftta_CopyLink, fto_NameFromLock);
			break;
			}

		d1(kprintf("%s/%s/%ld: Link Dest=<%s>\n", __FILE__, __FUNC__, __LINE__, Buffer));

		CurrentDir(DestParentLock);

		if (!MakeLink((STRPTR) DestName, Buffer, LINK_SOFT))
			{
			Result = RememberError(cl, o, DestName, ftta_CreateSoftLink, fto_MakeLink);
			d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

			if (ERROR_OBJECT_EXISTS == Result)
				{
				// try to delete and re-MakeLink()
				d1(kprintf("%s/%s/%ld: Special error handling for ERROR_OBJECT_EXISTS\n", __FILE__, __FUNC__, __LINE__));

				Result = RETURN_OK;
				ClearError(cl, o);

				if (!DeleteFile((STRPTR) DestName))
					{
					Result = RememberError(cl, o, DestName, ftta_CopyLink, fto_DeleteFile);
					}

				d1(kprintf("%s/%s/%ld: after deleting link, Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

				if (RETURN_OK == Result)
					{
					if (!MakeLink((STRPTR) DestName, Buffer, LINK_SOFT))
						{
						Result = RememberError(cl, o, DestName, ftta_CreateSoftLink, fto_MakeLink);
						}

					d1(KPrintF("%s/%s/%ld: after MakeLink(), Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
					}
				}
			else if (ERROR_ACTION_NOT_KNOWN == Result)
				{
				// Links probably not supported by destination file system
				ULONG	LinksMode = ftci->ftci_LinksMode;

				if (SCCV_LinksNotSupportedMode_Ask == LinksMode)
					{
					enum LinksNotSupportedReqResult lnsResult;

					lnsResult = DoMethod(o, SCCM_FileTrans_LinksNotSupportedRequest,
						ftci->ftci_Window,
						SrcParentLock,
						Buffer,
						DestParentLock,
						DestName,
						MSGID_LINKSNOTSUPPORTED_BODY,
						MSGID_LINKSNOTSUPPORTED_GADGETS);
					
					switch (lnsResult)
						{
					case LINKSNOTSUPPORTEDREQ_Ignore:
						LinksMode = SCCV_LinksNotSupportedMode_Ignore;
						break;
					case LINKSNOTSUPPORTEDREQ_IgnoreAll:
						LinksMode = ftci->ftci_LinksMode = SCCV_LinksNotSupportedMode_IgnoreAll;
						break;
					case LINKSNOTSUPPORTEDREQ_Copy:
						LinksMode = SCCV_LinksNotSupportedMode_Copy;
						break;
					case LINKSNOTSUPPORTEDREQ_CopyAll:
						LinksMode = ftci->ftci_LinksMode = SCCV_LinksNotSupportedMode_CopyAll;
						break;
					case LINKSNOTSUPPORTEDREQ_Abort:
					default:
						LinksMode = SCCV_LinksNotSupportedMode_Abort;
						break;
						};
					}
				switch (LinksMode)
					{
				case SCCV_LinksNotSupportedMode_Copy:
				case SCCV_LinksNotSupportedMode_CopyAll:
					Result = CopyLinkContents(cl, o, gca,
						SrcParentLock,
						DestParentLock,
						DestName,
						Buffer);
					break;
				case SCCV_LinksNotSupportedMode_Ignore:
				case SCCV_LinksNotSupportedMode_IgnoreAll:
					// Silently ignore links
					Result = RETURN_OK;
					break;
				case SCCV_LinksNotSupportedMode_Abort:
					Result = RETURN_WARN;
					break;
					}
				}

			if (RETURN_OK != Result)
				break;
			}
		} while (0);

	if (Buffer)
		FreePathBuffer(Buffer);
	CurrentDir(oldDir);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


// SrcParentLock	: Lock to source parent directory
// DestParentLock	: Lock to destination parent directory
// Name			: Name of source dir (relative to SrcParentLock)

// returns 		: RETURN_OK or DOS error code
static LONG CopyDir(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcParentLock, BPTR DestParentLock, CONST_STRPTR SrcName, CONST_STRPTR DestName,
	ENTRYFUNC EntryFunc)
{
//	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result;
	T_ExamineDirHandle edh = NULL;
	T_ExamineData *fib;
	BPTR oldDir = CurrentDir(SrcParentLock);
	BPTR srcDirLock;			// lock to source directory
	BPTR DestDirLock = (BPTR)NULL;		// lock to (freshly-created) destination directory

	d1(kprintf("%s/%s/%ld: SrcName=<%s>  DestName=<%s>\n", __FILE__, __FUNC__, __LINE__, SrcName, DestName));
	debugLock_d1(SrcParentLock);
	debugLock_d1(DestParentLock);

	do	{
		srcDirLock = Lock((STRPTR) SrcName, ACCESS_READ);
		if ((BPTR)NULL == srcDirLock)
			{
			Result = RememberError(cl, o, SrcName, ftta_CopyDir, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (!ScalosExamineDirBegin(srcDirLock, &edh))
			{
			Result = RememberError(cl, o, SrcName, ftta_CopyDir, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		// create dest dir
		CurrentDir(DestParentLock);
		DestDirLock = CreateDir((STRPTR) DestName);
		if ((BPTR)NULL == DestDirLock)
			{
			Result = RememberError(cl, o, DestName, ftta_CopyDir, fto_CreateDir);
			d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

			if (ERROR_OBJECT_EXISTS == Result)
				{
				DestDirLock = Lock((STRPTR) DestName, ACCESS_READ);
				ClearError(cl, o);
				Result = RememberError(cl, o, DestName, ftta_CopyDir, fto_CreateDir);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				}

			if ((BPTR)NULL == DestDirLock)
				break;
			}
		else
			{
			// change ACCESS_WRITE lock to ACCESS_READ
			ChangeMode(CHANGE_LOCK, DestDirLock, ACCESS_READ);
			}

		DoMethod(o, SCCM_FileTrans_UpdateWindow,
			FTUPDATE_EnterNewDir,
			srcDirLock, DestDirLock, SrcName);

		do	{
			LONG DirEntryType;
			CONST_STRPTR eName;

			if (!ScalosExamineDir(srcDirLock, &edh, &fib))
				{
				Result = RememberExNextError(cl, o, SrcName, ftta_CopyDir, fto_ExNext);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}

			eName = ScalosExamineGetName(fib);
			DirEntryType = ScalosExamineGetDirEntryType(fib);

			Result = CopyEntry(cl, o, gca, srcDirLock,
				DestDirLock, fib, eName,
				DirEntryType, eName);
			} while (RETURN_OK == Result);

		if (ERROR_NO_MORE_ENTRIES == Result)
			Result = RETURN_OK;
		} while (0);

	CurrentDir(oldDir);

	if (DestDirLock)
		UnLock(DestDirLock);
	if (srcDirLock)
		UnLock(srcDirLock);
	ScalosExamineDirEnd(&edh);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


// SrcParentLock	: Lock to source parent directory
// DestParentLock	: Lock to destination parent directory
// Name			: Name of source dir (relative to SrcParentLock)

// returns 		: RETURN_OK or DOS error code
static LONG CopyVolume(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcParentLock, BPTR DestParentLock, CONST_STRPTR SrcName, CONST_STRPTR DestName)
{
//	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result;
	T_ExamineDirHandle edh = NULL;
	T_ExamineData *fib;
	BPTR oldDir = CurrentDir(SrcParentLock);
	BPTR srcDirLock;			// lock to source directory
	BPTR DestDirLock = (BPTR)NULL;		// lock to (freshly-created) destination directory

	d1(kprintf("%s/%s/%ld: SrcName=<%s>  DestName=<%s>\n", __FILE__, __FUNC__, __LINE__, SrcName, DestName));
	debugLock_d1(SrcParentLock);
	debugLock_d1(DestParentLock);

	do	{
		srcDirLock = DupLock(SrcParentLock);
		if ((BPTR)NULL == srcDirLock)
			{
			Result = RememberError(cl, o, SrcName, ftta_CopyVolume, fto_DupLock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (!ScalosExamineDirBegin(srcDirLock, &edh))
			{
			Result = RememberError(cl, o, SrcName, ftta_CopyVolume, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		CurrentDir(DestParentLock);

		// create dest dir
		CurrentDir(DestParentLock);
		DestDirLock = CreateDir((STRPTR) DestName);
		if ((BPTR)NULL == DestDirLock)
			{
			Result = RememberError(cl, o, DestName, ftta_CopyVolume, fto_CreateDir);
			d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

			if (ERROR_OBJECT_EXISTS == Result)
				{
				DestDirLock = Lock((STRPTR) DestName, ACCESS_READ);
				ClearError(cl, o);
				Result = RememberError(cl, o, DestName, ftta_CopyVolume, fto_CreateDir);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				}

			if ((BPTR)NULL == DestDirLock)
				break;
			}
		else
			{
			// change ACCESS_WRITE lock to ACCESS_READ
			ChangeMode(CHANGE_LOCK, DestDirLock, ACCESS_READ);
			}

		DoMethod(o, SCCM_FileTrans_UpdateWindow,
			FTUPDATE_EnterNewDir,
			srcDirLock, DestDirLock, SrcName);

		do	{
			LONG DirEntryType;
			CONST_STRPTR eName;

			if (!ScalosExamineDir(srcDirLock, &edh, &fib))
				{
				Result = RememberExNextError(cl, o, SrcName, ftta_CopyVolume, fto_ExNext);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}

			eName = ScalosExamineGetName(fib);
			DirEntryType = ScalosExamineGetDirEntryType(fib);

			Result = CopyEntry(cl, o, gca, srcDirLock,
				DestDirLock, fib,
				eName, DirEntryType, eName);
			} while (RETURN_OK == Result);
		} while (0);

	CurrentDir(oldDir);

	if (DestDirLock)
		UnLock(DestDirLock);
	if (srcDirLock)
		UnLock(srcDirLock);
	ScalosExamineDirEnd(&edh);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG CopyEntry(Class *cl, Object *o, 
	struct GlobalCopyArgs *gca, BPTR SrcDirLock, BPTR DestDirLock, 
	const T_ExamineData *fib, CONST_STRPTR DestName, LONG DirEntryType, CONST_STRPTR eName)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result = RETURN_OK;
	LONG reqResult;
	BOOL fExists;

	d1(kprintf("%s/%s/%ld: SrcName=<%s>  DestName=<%s>  Type=%ld  current=%lu\n", \
		__FILE__, __FUNC__, __LINE__, eName, DestName, DirEntryType, ftci->ftci_CurrentItems));
	d1(kprintf("%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));

	if (ST_ROOT == DirEntryType)
		{
		STRPTR VolName = AllocCopyString(DestName);

		if (VolName)
			{
			StripTrailingColon(VolName);
			fExists = ExistsObject(DestDirLock, VolName);

			FreeCopyString(VolName);
			}
		else
			fExists = ExistsObject(DestDirLock, DestName);
		}
	else
		{
		fExists = ExistsObject(DestDirLock, DestName);
		}

	ftci->ftci_MostCurrentReplaceMode = ftci->ftci_ReplaceMode;

	if (fExists)
		{
		LONG existsResult = EXISTREQ_Replace;

		switch (ftci->ftci_ReplaceMode)
			{
		case SCCV_ReplaceMode_Ask:
			if (EXISTREQ_ReplaceAll == gca->gca_Replace)
				existsResult = EXISTREQ_Replace;
			else
				{
				existsResult = ftci->ftci_MostCurrentReplaceMode = DoMethod(o, SCCM_FileTrans_OverwriteRequest,
					OVERWRITEREQ_Copy,
					SrcDirLock, eName,
					DestDirLock, DestName,
					ftci->ftci_Window, 
					MSGID_EXISTSNAME_COPY, MSGID_EXISTS_GNAME_NEW);
				}
			break;
		case SCCV_ReplaceMode_Never:
			return RETURN_OK;
			break;
		case SCCV_ReplaceMode_Always:
			existsResult = ftci->ftci_MostCurrentReplaceMode = EXISTREQ_Replace;
			break;
			}

		d1(kprintf("%s/%s/%ld: existsResult=%ld\n", __FILE__, __FUNC__, __LINE__, existsResult));

		switch (existsResult)
			{
		case EXISTREQ_Replace:
			break;

		case EXISTREQ_Skip:
			return RETURN_OK;
			break;

		case EXISTREQ_SkipAll:
			ftci->ftci_ReplaceMode = SCCV_ReplaceMode_Never;
			return RETURN_OK;
			break;

		case EXISTREQ_ReplaceAll:
			gca->gca_Replace = EXISTREQ_ReplaceAll;
			ftci->ftci_ReplaceMode = ftci->ftci_MostCurrentReplaceMode = SCCV_ReplaceMode_Always;
			break;

		case EXISTREQ_Abort:
			ftci->ftci_ReplaceMode = ftci->ftci_MostCurrentReplaceMode = SCCV_ReplaceMode_Abort;
			return RESULT_UserAborted;
			break;
			}
		}

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Entry,
		SrcDirLock, DestDirLock, eName);

	do	{
		d1(kprintf("%s/%s/%ld: Start of do loop\n", __FILE__, __FUNC__, __LINE__));
		reqResult = COPYERR_nop;

		switch (DirEntryType)
			{
		case ST_FILE:
		case ST_LINKFILE:
		case ST_PIPEFILE:
			d1(kprintf("%s/%s/%ld: Calling CopyFile for FILE, LINK or PIPE\n", __FILE__, __FUNC__, __LINE__));
			Result = CopyFile(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName);
			break;

		case ST_ROOT:
			d1(kprintf("%s/%s/%ld: Calling CopyVolume for ROOT\n", __FILE__, __FUNC__, __LINE__));
			Result = CopyVolume(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName);
			break;

		case ST_USERDIR:
		case ST_LINKDIR:
			d1(kprintf("%s/%s/%ld: Calling CopyDir for USERDIR or LINKDIR\n", __FILE__, __FUNC__, __LINE__));
			Result = CopyDir(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName, CopyEntry);
			break;

		case ST_SOFTLINK:
			d1(kprintf("%s/%s/%ld: Calling CopyLink for softlink\n", __FILE__, __FUNC__, __LINE__));
			Result = CopyLink(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName);
			break;

		/* Catch for types which are not any of the above values, but should still
		 * be handled (i.e. copying files across a Samba connection)
		 */
		default:
			if (DirEntryType < 0)
				{
				// generally speaking <0 is file
				d1(kprintf("%s/%s/%ld: DeFAULT: Calling CopyFile for FILE, LINK or PIPE\n", __FILE__, __FUNC__, __LINE__));
				Result = CopyFile(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName);
				}
			else
				{
				// and >=0 is a directory
				d1(kprintf("%s/%s/%ld: DEFAULT: Calling CopyDir for USERDIR or LINKDIR\n", __FILE__, __FUNC__, __LINE__));
				Result = CopyDir(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName, CopyEntry);
				}
			break;
			}

		if (RETURN_OK == Result)
			{
			BPTR oldDir = CurrentDir(DestDirLock);

			d1(kprintf("%s/%s/%ld: Result OK\n", __FILE__, __FUNC__, __LINE__));
			debugLock_d1(SrcDirLock);
			debugLock_d1(DestDirLock);
			debugLock_d1(oldDir);
			d1(kprintf("%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));
			d1(kprintf("%s/%s/%ld: Comment=<%s>\n", __FILE__, __FUNC__, __LINE__, ScalosExamineGetComment(fib)));

			(void) SetFileDate((STRPTR) DestName, ScalosExamineGetDate((T_ExamineData *) fib));
			(void) SetComment((STRPTR) DestName, (STRPTR) ScalosExamineGetComment(fib));
			(void) SetProtection((STRPTR) DestName, ScalosExamineGetProtection(fib) & ~FIBF_ARCHIVE);

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			CurrentDir(oldDir);
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (RETURN_OK == Result)
			Result = DoMethod(o, SCCM_FileTrans_CheckAbort);

		if (Result != RETURN_OK && Result != RESULT_UserAborted)
			{
			d1(kprintf("%s/%s/%ld: Weird error!\n", __FILE__, __FUNC__, __LINE__));
			reqResult = ReportError(cl, o, &Result, MSGID_COPYERRORNAME, MSGID_COPYERRORGNAME);
			}

		} while (RETURN_OK == Result && COPYERR_Retry == reqResult);

	ftci->ftci_CurrentItems++;

	return Result;
}


// check if named object (file/directory) exists
BOOL ExistsObject(BPTR DirLock, CONST_STRPTR Name)
{
	BOOL Exists = FALSE;
	BPTR oldDir = CurrentDir(DirLock);
	BPTR fLock;

	fLock = Lock((STRPTR) Name, ACCESS_READ);
	if (fLock)
		{
		Exists = TRUE;
		UnLock(fLock);
		}
	else
		{
		LONG Err = IoErr();

		switch (Err)
			{
		case ERROR_OBJECT_IN_USE:
			Exists = TRUE;
			break;
			}
		}

	CurrentDir(oldDir);

	return Exists;
}


static LONG ReportError(Class *cl, Object *o, LONG *Result, ULONG BodyTextID, ULONG GadgetTextID)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	enum ErrorReqResult reqResult;

	reqResult = DoMethod(o, SCCM_FileTrans_ErrorRequest,
		ftci->ftci_Window,
		ftci->ftci_LastAction,
		ftci->ftci_LastOp,
		ftci->ftci_LastErrorCode,
		ftci->ftci_LastErrorFileName,
		BodyTextID,
		GadgetTextID);

	d1(kprintf("%s/%s/%ld: UseRequest returned=%ld\n", __FILE__, __FUNC__, __LINE__, reqResult));

	switch (reqResult)
		{
	case ERRORREQ_Skip:
		// Skip : drop current object but try to continue copying
		*Result = RETURN_OK;
		break;

	case ERRORREQ_Retry:
		*Result = RETURN_OK;
		break;

	case ERRORREQ_Abort:
		*Result = RESULT_UserAborted;
		break;
		}

	if (ftci->ftci_LastErrorFileName)
		{
		FreeCopyString(ftci->ftci_LastErrorFileName);
		ftci->ftci_LastErrorFileName = NULL;
		}

	return reqResult;
}


static LONG DeleteEntry(Class *cl, Object *o, BPTR parentLock, CONST_STRPTR Name, BOOL DoCount)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result;
	T_ExamineData *fib;
	BPTR objLock = (BPTR)NULL;
	BPTR oldDir = CurrentDir(parentLock);

	d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));
	debugLock_d1(parentLock);

	do	{
		LONG DirEntryType;

		if (!ScalosExamineBegin(&fib))
			{
			Result = RememberError(cl, o, Name, ftta_Delete, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (IsSoftLink(Name))
			{
			DirEntryType = ST_SOFTLINK;
			}
		else
			{
			objLock = Lock((STRPTR) Name, ACCESS_READ);
			if ((BPTR)NULL == objLock)
				{
				Result = RememberError(cl, o, Name, ftta_Delete, fto_Lock);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}

			if (!ScalosExamineLock(objLock, &fib))
				{
				Result = RememberError(cl, o, Name, ftta_Delete, fto_Examine);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}

			DirEntryType = ScalosExamineGetDirEntryType(fib);

			d1(kprintf("%s/%s/%ld: FileName=<%s>  DirEntryType=%ld\n", \
				__FILE__, __FUNC__, __LINE__, ScalosExamineGetName(fib), DirEntryType));

			// if we don't UnLock() now, object cannot be deleted !
			UnLock(objLock);
			objLock = (BPTR)NULL;
			}

		switch (DirEntryType)
			{
		case ST_FILE:
		case ST_LINKFILE:
		case ST_PIPEFILE:
		case ST_SOFTLINK:
			if (DoCount)
				ftci->ftci_CurrentBytes = Add64(ftci->ftci_CurrentBytes, ScalosExamineGetSize(fib));
			Result = ScaDeleteFile(cl, o, parentLock, Name);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;

		case ST_ROOT:
		case ST_USERDIR:
		case ST_LINKDIR:
			Result = ScaDeleteDir(cl, o, parentLock, Name, DoCount); // delete directory contents
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK == Result)
				Result = ScaDeleteFile(cl, o, parentLock, Name);	// delete directory itself
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;

		default:
			if (!ScalosExamineIsDrawer(fib))
				{
				// generally speaking <0 is file
				if (DoCount)
					ftci->ftci_CurrentBytes = Add64(ftci->ftci_CurrentBytes, ScalosExamineGetSize(fib));
				Result = ScaDeleteFile(cl, o, parentLock, Name);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				}
			else
				{
				// and >=0 is a directory
				Result = ScaDeleteDir(cl, o, parentLock, Name, DoCount); // delete directory contents
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				if (RETURN_OK == Result)
					Result = ScaDeleteFile(cl, o, parentLock, Name);	// delete directory itself
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				}
			break;
			}
		} while (0);

	if (DoCount)
		ftci->ftci_CurrentItems++;

	CurrentDir(oldDir);

	if (objLock)
		UnLock(objLock);
	ScalosExamineEnd(&fib);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


// ParentLock		: Lock to parent directory
// Name			: Name of file (relative to ParentLock)

// returns 		: RETURN_OK or DOS error code
static LONG ScaDeleteFile(Class *cl, Object *o, BPTR parentLock, CONST_STRPTR Name)
{
//	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result = RETURN_OK;
	BPTR oldDir;

	d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));
	debugLock_d1(parentLock);

	do	{
		oldDir = CurrentDir(parentLock);

		if (!DeleteFile((STRPTR) Name))
			{
			Result = RememberError(cl, o, Name, ftta_DeleteFile, fto_DeleteFile);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (ERROR_WRITE_PROTECTED == Result
				|| ERROR_DELETE_PROTECTED == Result)
				{
				enum WriteProtectedReqResult overwrite;

				overwrite = HandleWriteProtectError(cl, o, parentLock, Name, WRITEPROTREQ_Move, Result);
				Result = RETURN_OK;
				switch (overwrite)
					{
				case WRITEPROTREQ_ReplaceAll:
				case WRITEPROTREQ_Replace:
					SetProtection(Name, 0);
					if (!DeleteFile((STRPTR) Name))
						Result = RememberError(cl, o, Name, ftta_DeleteFile, fto_SetProtection);
					break;
				default:
					break;
					}
				if (WRITEPROTREQ_Abort == overwrite)
					Result = RESULT_UserAborted;
				}
			}
		if (RETURN_OK != Result)
			break;
		} while (0);

	CurrentDir(oldDir);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


// parentLock		: Lock to parent directory
// Name			: Name of dir (relative to parentLock)

// returns 		: RETURN_OK or DOS error code
static LONG ScaDeleteDir(Class *cl, Object *o, BPTR parentLock, CONST_STRPTR Name, BOOL DoCount)
{
//	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result;
	T_ExamineDirHandle edh = NULL;
	T_ExamineData *fib;
	BPTR oldDir = CurrentDir(parentLock);
	BPTR dirLock;			// lock to source directory

	d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));
	debugLock_d1(parentLock);

	do	{
		dirLock = Lock((STRPTR) Name, ACCESS_READ);
		if ((BPTR)NULL == dirLock)
			{
			Result = RememberError(cl, o, Name, ftta_DeleteDir, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (!ScalosExamineDirBegin(dirLock, &edh))
			{
			Result = RememberError(cl, o, Name, ftta_DeleteDir, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		do	{
			if (!ScalosExamineDir(dirLock, &edh, &fib))
				{
				Result = RememberExNextError(cl, o, Name, ftta_DeleteDir, fto_ExNext);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}

			Result = DeleteEntry(cl, o, dirLock, ScalosExamineGetName(fib), DoCount);
			} while (RETURN_OK == Result);
		} while (0);

	CurrentDir(oldDir);
	if (dirLock)
		UnLock(dirLock);
	ScalosExamineDirEnd(&edh);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


LONG CountCommand(Class *cl, Object *o, BPTR SrcDirLock, CONST_STRPTR SrcFileName, BOOL Recursive)
{
	LONG Result = RETURN_OK;
	BOOL SourceIsVolume = FALSE;
	T_ExamineData *fib;
	BPTR fLock = (BPTR)NULL;
	STRPTR allocatedName = NULL;
	BPTR oldDir;

	d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, SrcFileName));
	debugLock_d1(SrcDirLock);

	DoMethod(o, SCCM_FileTrans_OpenWindowDelayed);

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Entry,
		SrcDirLock, SrcDirLock, SrcFileName);

	do	{
		CONST_STRPTR eName;
		LONG DirEntryType;

		oldDir = CurrentDir(SrcDirLock);

		if (!ScalosExamineBegin(&fib))
			{
			Result = RememberError(cl, o, SrcFileName, ftta_Count, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (0 == strlen(SrcFileName))
			{
			// Source is an absolute filename, i.e. "Volume:"
			SourceIsVolume = TRUE;
			}

		fLock = Lock((STRPTR) SrcFileName, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			{
			Result = RememberError(cl, o, SrcFileName, ftta_Count, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		d1(kprintf("%s/%s/%ld: SrcFileName=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
			SrcFileName, SrcFileName ? SrcFileName : (CONST_STRPTR) ""));

		debugLock_d1(fLock);

		if (!ScalosExamineLock(fLock, &fib))
			{
			Result = RememberError(cl, o, SrcFileName, ftta_Count, fto_Examine);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		eName = ScalosExamineGetName(fib);
		DirEntryType = ScalosExamineGetDirEntryType(fib);

		if (SourceIsVolume)
			{
			DirEntryType = ST_ROOT;
			eName = allocatedName = ScalosAlloc(2 + strlen(ScalosExamineGetName(fib)));
			if (eName)
				{
				strcpy(allocatedName, ScalosExamineGetName(fib));
				strcat(allocatedName, ":");
				}
			}
		else if (IsSoftLink(SrcFileName))
			{
			DirEntryType = ST_SOFTLINK;
			eName = allocatedName = ScalosAlloc(1 + strlen(SrcFileName));
			if (eName)
				{
				strcpy(allocatedName, SrcFileName);
				}
			}

		if (RETURN_OK == Result)
			Result = CountEntry(cl, o, SrcDirLock, fib, Recursive, DirEntryType, eName);
		} while (0);

	if (RETURN_OK == Result)
		Result = DoMethod(o, SCCM_FileTrans_CheckAbort);

	CurrentDir(oldDir);

	if (fLock)
		UnLock(fLock);
	ScalosExamineEnd(&fib);
	if (allocatedName)
		ScalosFree(allocatedName);

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Entry,
		SrcDirLock, SrcDirLock, SrcFileName);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG CountEntry(Class *cl, Object *o, BPTR srcDirLock, T_ExamineData *fib,
	BOOL recursive, LONG DirEntryType, CONST_STRPTR eName)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result = RETURN_OK;

	d1(kprintf("%s/%s/%ld: SrcName=<%s>  Type=%ld  Total=%lu\n", \
		__FILE__, __FUNC__, __LINE__, eName, DirEntryType, ftci->ftci_TotalFiles));

	DoMethod(o, SCCM_FileTrans_OpenWindowDelayed);

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Entry,
		srcDirLock, srcDirLock, eName);

	do	{
		switch (DirEntryType)
			{
		case ST_FILE:
		case ST_LINKFILE:
		case ST_PIPEFILE:
			ftci->ftci_CurrentItems++;

			ftci->ftci_CurrentBytes = Add64(ftci->ftci_CurrentBytes, ScalosExamineGetSize(fib));

			d1(kprintf("%s/%s/%ld: Name=<%s>  Size=%ld  Total=%lu\n", \
				__FILE__, __FUNC__, __LINE__, eName, ULONG64_LOW(ScalosExamineGetSize(fib)), ULONG64_LOW(ftci->ftci_TotalBytes)));
			break;

		case ST_SOFTLINK:
			ftci->ftci_TotalLinks++;
			break;

		case ST_LINKDIR:
		case ST_USERDIR:
			if (recursive)
				Result = FileTrans_CountDir(cl, o, srcDirLock, eName);	    // count directory contents
			d1(if (RETURN_OK != Result) kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			ftci->ftci_TotalDirs++;
			break;

		default:
			if (DirEntryType < 0)
				{
				// generally speaking <0 is file
				ftci->ftci_CurrentItems++;
				ftci->ftci_TotalFiles++;

				ftci->ftci_CurrentBytes = Add64(ftci->ftci_CurrentBytes, ScalosExamineGetSize(fib));

				d1(kprintf("%s/%s/%ld: Name=<%s>  Size=%ld  Current=%lu\n", \
					__FILE__, __FUNC__, __LINE__, eName, ULONG64_LOW(ScalosExamineGetSize(fib)), ULONG64_LOW(ftci->ftci_CurrentBytes)));
				}
			else
				{
				// and >=0 is a directory
				// count directory contents
				if (recursive)
					Result = FileTrans_CountDir(cl, o, srcDirLock, eName);
				d1(if (RETURN_OK != Result) kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				ftci->ftci_TotalDirs++;
				}
			break;
			}

		if (RETURN_OK == Result)
			{
			Result = DoMethod(o, SCCM_FileTrans_CheckAbort);
			d1(if (RETURN_OK != Result) kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			}

		} while (0);

	ftci->ftci_CurrentItems++;

	d1(if (RETURN_OK != Result) kprintf("%s/%s/%ld: SrcName=<%s> Result=%ld\n", __FILE__, __FUNC__, __LINE__, eName, Result));

	return Result;
}


// parentLock		: Lock to parent directory
// Name			: Name of dir (relative to parentLock)

// returns 		: RETURN_OK or DOS error code
static LONG FileTrans_CountDir(Class *cl, Object *o, BPTR parentLock, CONST_STRPTR Name)
{
//	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result;
	T_ExamineDirHandle edh = NULL;
	T_ExamineData *fib;
	BPTR oldDir = CurrentDir(parentLock);
	BPTR dirLock;			// lock to source directory

	d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));
	debugLock_d1(parentLock);

	if (DoMethod(o, SCCM_FileTrans_CountTimeout))
		{
		d1(kprintf("%s/%s/%ld: Timeout: Result=RETURN_WARN\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_WARN;
		}

	do	{
		dirLock = Lock((STRPTR) Name, ACCESS_READ);
		if ((BPTR)NULL == dirLock)
			{
			Result = RememberError(cl, o, Name, ftta_CountDir, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (!ScalosExamineDirBegin(dirLock, &edh))
			{
			Result = RememberError(cl, o, Name, ftta_CountDir, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		do	{
			CONST_STRPTR eName;
			LONG DirEntryType;

			if (!ScalosExamineDir(dirLock, &edh, &fib))
				{
				Result = RememberExNextError(cl, o, Name, ftta_CountDir, fto_ExNext);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				d1(if (RETURN_OK != Result) kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}

			eName = ScalosExamineGetName(fib);
			DirEntryType = ScalosExamineGetDirEntryType(fib);

			Result = CountEntry(cl, o, dirLock, fib, TRUE, DirEntryType, eName);
			d1(if (RETURN_OK != Result) kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			} while (RETURN_OK == Result);
		} while (0);

	CurrentDir(oldDir);
	if (dirLock)
		UnLock(dirLock);
	ScalosExamineDirEnd(&edh);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG CopyAndDeleteEntry(Class *cl, Object *o, 
	struct GlobalCopyArgs *gca, BPTR SrcDirLock, BPTR DestDirLock, 
	const T_ExamineData *fib, CONST_STRPTR DestName, LONG DirEntryType, CONST_STRPTR eName)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result = RETURN_OK;
	LONG reqResult;
	BOOL fExists;

	d1(kprintf("%s/%s/%ld: SrcName=<%s>  DestName=<%s>  Type=%ld\n", \
		__FILE__, __FUNC__, __LINE__, eName, DestName, DirEntryType));

	if (ST_ROOT == DirEntryType)
		{
		STRPTR VolName = AllocCopyString(DestName);

		if (VolName)
			{
			StripTrailingColon(VolName);
			fExists = ExistsObject(DestDirLock, VolName);

			FreeCopyString(VolName);
			}
		else
			fExists = ExistsObject(DestDirLock, DestName);
		}
	else
		{
		fExists = ExistsObject(DestDirLock, DestName);
		}

	if (fExists)
		{
		LONG existsResult = EXISTREQ_Replace;

		switch (ftci->ftci_ReplaceMode)
			{
		case SCCV_ReplaceMode_Ask:
			if (EXISTREQ_ReplaceAll == gca->gca_Replace)
				existsResult = EXISTREQ_Replace;
			else
				{
				existsResult = ftci->ftci_MostCurrentReplaceMode = DoMethod(o, SCCM_FileTrans_OverwriteRequest,
					OVERWRITEREQ_Copy,
					SrcDirLock, eName,
					DestDirLock, DestName,
					ftci->ftci_Window, 
					MSGID_EXISTSNAME_COPY, MSGID_EXISTS_GNAME_NEW);
				}
			break;
		case SCCV_ReplaceMode_Never:
			return RETURN_OK;
			break;
		case SCCV_ReplaceMode_Always:
			existsResult = ftci->ftci_MostCurrentReplaceMode = EXISTREQ_Replace;
			break;
			}

		d1(kprintf("%s/%s/%ld: existsResult=%ld\n", __FILE__, __FUNC__, __LINE__, existsResult));

		switch (existsResult)
			{
		case EXISTREQ_Replace:
			break;

		case EXISTREQ_Skip:
			return RETURN_OK;
			break;

		case EXISTREQ_SkipAll:
			ftci->ftci_ReplaceMode = ftci->ftci_MostCurrentReplaceMode = SCCV_ReplaceMode_Never;
			return RETURN_OK;
			break;

		case EXISTREQ_ReplaceAll:
			gca->gca_Replace = EXISTREQ_ReplaceAll;
			ftci->ftci_ReplaceMode = ftci->ftci_MostCurrentReplaceMode = SCCV_ReplaceMode_Always;
			break;

		case EXISTREQ_Abort:
			ftci->ftci_ReplaceMode = ftci->ftci_MostCurrentReplaceMode = SCCV_ReplaceMode_Abort;
			return RESULT_UserAborted;
			break;
			}
		}

	DoMethod(o, SCCM_FileTrans_UpdateWindow,
		FTUPDATE_Entry,
		SrcDirLock, DestDirLock, eName);

	do	{
		d1(kprintf("%s/%s/%ld: Start of do loop  DirEntryType=%ld\n", __FILE__, __FUNC__, __LINE__, DirEntryType));
		reqResult = COPYERR_nop;

		switch (DirEntryType)
			{
		case ST_FILE:
		case ST_LINKFILE:
		case ST_PIPEFILE:
			d1(kprintf("%s/%s/%ld: Calling CopyFile for FILE, LINK or PIPE\n", __FILE__, __FUNC__, __LINE__));
			Result = CopyFile(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName);
			if (RETURN_OK == Result)
				Result = ScaDeleteFile(cl, o, SrcDirLock, eName);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;

		case ST_ROOT:
			d1(kprintf("%s/%s/%ld: Calling CopyVolume for ROOT\n", __FILE__, __FUNC__, __LINE__));
			Result = CopyVolume(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;

		case ST_USERDIR:
		case ST_LINKDIR:
			d1(kprintf("%s/%s/%ld: Calling CopyDir for USERDIR or LINKDIR\n", __FILE__, __FUNC__, __LINE__));
			Result = CopyDir(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName, CopyAndDeleteEntry);
			if (RETURN_OK == Result)
				Result = ScaDeleteDir(cl, o, SrcDirLock, eName, FALSE);    // delete directory contents
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK == Result)
				Result = ScaDeleteFile(cl, o, SrcDirLock, eName);   // delete directory itself
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;

		case ST_SOFTLINK:
			d1(kprintf("%s/%s/%ld: Calling CopyLink for softlink\n", __FILE__, __FUNC__, __LINE__));
			Result = CopyLink(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK == Result)
				Result = ScaDeleteFile(cl, o, SrcDirLock, eName);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;

		/* Catch for types which are not any of the above values, but should still
		 * be handled (i.e. copying files across a Samba connection)
		 */
		default:
			if (DirEntryType < 0)
				{
				// generally speaking <0 is file
				d1(kprintf("%s/%s/%ld: DEFAULT: Calling CopyFile for FILE, LINK or PIPE\n", __FILE__, __FUNC__, __LINE__));
				Result = CopyFile(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				if (RETURN_OK == Result)
					Result = ScaDeleteFile(cl, o, SrcDirLock, eName);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				}
			else
				{
				// and >=0 is a directory
				d1(kprintf("%s/%s/%ld: DEFAULT: Calling CopyDir for USERDIR or LINKDIR\n", __FILE__, __FUNC__, __LINE__));
				Result = CopyDir(cl, o, gca, SrcDirLock, DestDirLock, eName, DestName, CopyAndDeleteEntry);
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				if (RETURN_OK == Result)
					Result = ScaDeleteDir(cl, o, SrcDirLock, eName, FALSE);    // delete directory contents
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				if (RETURN_OK == Result)
					Result = ScaDeleteFile(cl, o, SrcDirLock, eName);   // delete directory itself
				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				}
			break;
			}

		if (RETURN_OK == Result)
			{
			BPTR oldDir = CurrentDir(DestDirLock);

			d1(kprintf("%s/%s/%ld: Result OK\n", __FILE__, __FUNC__, __LINE__));

			SetFileDate((STRPTR) DestName, ScalosExamineGetDate((T_ExamineData *) fib));
			SetComment((STRPTR) DestName, (STRPTR) ScalosExamineGetComment(fib));
			SetProtection((STRPTR) DestName, ScalosExamineGetProtection(fib) & ~FIBF_ARCHIVE);

			CurrentDir(oldDir);
			}

		d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

		if (RETURN_OK == Result)
			Result = DoMethod(o, SCCM_FileTrans_CheckAbort);

		if (Result != RETURN_OK && Result != RESULT_UserAborted)
			{
			d1(kprintf("%s/%s/%ld: Weird error!\n", __FILE__, __FUNC__, __LINE__));
			reqResult = ReportError(cl, o, &Result, MSGID_MOVEERRORNAME, MSGID_COPYERRORGNAME);
			}

		} while (RETURN_OK == Result && COPYERR_Retry == reqResult);

	ftci->ftci_CurrentItems++;

	return Result;
}


static LONG RememberError(Class *cl, Object *o, CONST_STRPTR FileName,
	enum FileTransTypeAction LastAction, enum FileTransOperation LastOp)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG ErrorCode = IoErr();

	d1(kprintf("%s/%s/%ld: ErrorCode=%ld  FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, ErrorCode, FileName));

	if (RETURN_OK != ErrorCode)
		{
		if (NULL == ftci->ftci_LastErrorFileName)
			{
			ftci->ftci_LastOp = LastOp;
			ftci->ftci_LastAction = LastAction;
			ftci->ftci_LastErrorCode = ErrorCode;
			ftci->ftci_LastErrorFileName = AllocCopyString(FileName);
			}
		}

	return ErrorCode;
}


static void ClearError(Class *cl, Object *o)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);

	ftci->ftci_LastErrorCode = RETURN_OK;

	if (ftci->ftci_LastErrorFileName)
		{
		FreeCopyString(ftci->ftci_LastErrorFileName);
		ftci->ftci_LastErrorFileName = NULL;
		}
}


// just like RememberError() except that 
// ERROR_NO_MORE_ENTRIES get remapped to RETURN_OK
static LONG RememberExNextError(Class *cl, Object *o, CONST_STRPTR FileName,
	enum FileTransTypeAction LastAction, enum FileTransOperation LastOp)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG ErrorCode = IoErr();

	d1(kprintf("%s/%s/%ld: ErrorCode=%ld  FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, ErrorCode, FileName));

	if (ERROR_NO_MORE_ENTRIES == ErrorCode)
		ErrorCode = RETURN_OK;

	if (RETURN_OK != ErrorCode)
		{
		if (NULL == ftci->ftci_LastErrorFileName)
			{
			ftci->ftci_LastOp = LastOp;
			ftci->ftci_LastAction = LastAction;
			ftci->ftci_LastErrorCode = ErrorCode;
			ftci->ftci_LastErrorFileName = AllocCopyString(FileName);
			}
		}

	return ErrorCode;
}


static LONG HandleWriteProtectError(Class *cl, Object *o, BPTR DirLock,
	CONST_STRPTR FileName, enum WriteProtectedReqType Type, LONG ErrorCode)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	enum WriteProtectedReqResult existsResult = WRITEPROTREQ_Skip;
	ULONG ReqMsgId;

	d1(kprintf("%s/%s/%ld: ErrorCode=%ld  FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, ErrorCode, FileName));

	switch (ftci->ftci_OverwriteMode)
		{
	case SCCV_WriteProtectedMode_Ask:
		switch (Type)
			{
		case WRITEPROTREQ_Move:
			ReqMsgId = MSGID_WRITEPROTECTED_MOVE;
			break;
		case WRITEPROTREQ_CopyIcon:
			ReqMsgId = MSGID_WRITEPROTECTED_COPYICON;
			break;
		case WRITEPROTREQ_Copy:
		default:
			ReqMsgId = MSGID_WRITEPROTECTED_COPY;
			break;
			}

		existsResult = DoMethod(o, SCCM_FileTrans_WriteProtectedRequest,
			WRITEPROTREQ_Move,
			DirLock, FileName,
			ftci->ftci_Window,
			ErrorCode,
			ReqMsgId, MSGID_WRITEPROTECTED_GNAME_NEW);
		
		if (WRITEPROTREQ_ReplaceAll == existsResult
			|| WRITEPROTREQ_SkipAll == existsResult)
			ftci->ftci_OverwriteMode = existsResult;
                break;

	case SCCV_WriteProtectedMode_Never:
		d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, RETURN_OK));
		return RETURN_OK;
		break;

	case SCCV_WriteProtectedMode_Always:
		existsResult = WRITEPROTREQ_ReplaceAll;
		break;
		}

	switch (existsResult)
		{
	case WRITEPROTREQ_ReplaceAll:
	case WRITEPROTREQ_Replace:
	case WRITEPROTREQ_SkipAll:
	case WRITEPROTREQ_Skip:
		ClearError(cl, o);
		break;
	default:
		break;
		}

	return existsResult;
}


static LONG CopyLinkContents(Class *cl, Object *o, struct GlobalCopyArgs *gca,
	BPTR SrcDirLock, BPTR DestDirLock, CONST_STRPTR DestName, CONST_STRPTR LinkDest)
{
	BPTR oldDir;
	T_ExamineData *fib;
	LONG Result;
	BPTR LinkSrcDirLock = BNULL;
	BPTR fLock = BNULL;

	d1(kprintf("%s/%s/%ld: DestName=<%s>  LinkDest=<%s>\n", __FILE__, __FUNC__, __LINE__, DestName, LinkDest));
	debugLock_d1(SrcDirLock);
	debugLock_d1(DestDirLock);

	ClearError(cl, o);

	do	{
		LONG DirEntryType;

		oldDir = CurrentDir(DestDirLock);

		if (!ScalosExamineBegin(&fib))
			{
			Result = RememberError(cl, o, LinkDest, ftta_CopyLink, fto_AllocDosObject);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		fLock = Lock(LinkDest, ACCESS_READ);
		debugLock_d1(fLock);
		if (BNULL == fLock)
			{
			Result = RememberError(cl, o, LinkDest, ftta_CopyLink, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		LinkSrcDirLock = ParentDir(fLock);
		debugLock_d1(LinkSrcDirLock);
		if (BNULL == LinkSrcDirLock)
			{
			Result = RememberError(cl, o, LinkDest, ftta_CopyLink, fto_Lock);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		if (!ScalosExamineLock(fLock, &fib))
			{
			Result = RememberError(cl, o, LinkDest, ftta_CopyLink, fto_Examine);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		DirEntryType = ScalosExamineGetDirEntryType(fib);

		Result = CopyEntry(cl, o,
			gca,
			LinkSrcDirLock,
			DestDirLock,
			fib,
			DestName,
			DirEntryType,
			DestName);
		} while (0);

	ScalosExamineEnd(&fib);

	CurrentDir(oldDir);

	if (LinkSrcDirLock)
		UnLock(LinkSrcDirLock);
	if (fLock)
		UnLock(fLock);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}



// Preferences-aos4.c
// $Date$
// $Revision$


#include <exec/interfaces.h>
#include <exec/exectags.h>
#include <exec/resident.h>
#include <exec/emulation.h>

#include <proto/exec.h>
#include <interfaces/sqlite3.h>

#include <stdarg.h>

#include "sqlite3_base.h"
#include "LibSQLite3.h"
#include "sqlite3.h"

#include <defs.h>

int _start(void)
{
	return -1;
}

extern CONST_APTR VecTable68K[];

static struct Library *Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec);
static BPTR Expungelib(struct LibraryManagerInterface *Self);
static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version);
static BPTR Closelib(struct LibraryManagerInterface *Self);
static ULONG Obtainlib(struct LibraryManagerInterface *Self);
static ULONG Releaselib(struct LibraryManagerInterface *Self);

/* OS4.0 Library */

/* ------------------- OS4 Manager Interface ------------------------ */
static const APTR managerfunctable[] =
{
  (APTR)Obtainlib,
  (APTR)Releaselib,
  (APTR)NULL,
  (APTR)NULL,
  (APTR)Openlib,
  (APTR)Closelib,
  (APTR)Expungelib,
  (APTR)NULL,
  (APTR)-1
};

static const struct TagItem managertags[] =
{
  {MIT_Name,             (ULONG)"__library"},
  {MIT_VectorTable,      (ULONG)managerfunctable},
  {MIT_Version,          1},
  {TAG_DONE,             0}
};

/* ---------------------- OS4 Main Interface ------------------------ */
static APTR functable[] =
	{
	Obtainlib,
	Releaselib,
	NULL,
	NULL,
	LIBSQLite3Close,
	LIBSQLite3Exec,
	LIBSQLite3Changes,
	LIBSQLite3TotalChanges,
	LIBSQLite3Interrupt,
	LIBSQLite3Complete,
	LIBSQLite3BusyHandler,
	LIBSQLite3BusyTimeout,
	LIBSQLite3GetTable,
	LIBSQLite3FreeTable,
	LIBSQLite3Free,
	LIBSQLite3Trace,
	LIBSQLite3ProgressHandler,
	LIBSQLite3CommitHook,
	LIBSQLite3Open,
	LIBSQLite3Errcode,
	LIBSQLite3Errmsg,
	LIBSQLite3Prepare,
	LIBSQLite3BindBlob,
	LIBSQLite3BindInt,
	LIBSQLite3BindNull,
	LIBSQLite3BindText,
	LIBSQLite3BindValue,
	LIBSQLite3BindParameterCount,
	LIBSQLite3BindParameterName,
	LIBSQLite3BindParameterIndex,
	LIBSQLite3ClearBindings,
	LIBSQLite3ColumnCount,
	LIBSQLite3ColumnName,
	LIBSQLite3ColumnDecltype,
	LIBSQLite3Step,
	LIBSQLite3DataCount,
	LIBSQLite3ColumnBlob,
	LIBSQLite3ColumnBytes,
	LIBSQLite3ColumnInt,
	LIBSQLite3ColumnText,
	LIBSQLite3ColumnType,
	LIBSQLite3Finalize,
	LIBSQLite3Reset,
	LIBSQLite3AggregateCount,
	LIBSQLite3ValueBlob,
	LIBSQLite3ValueBytes,
	LIBSQLite3ValueInt,
	LIBSQLite3ValueText,
	LIBSQLite3ValueType,
	LIBSQLite3Aggregate_context,
	LIBSQLite3UserData,
	LIBSQLite3GetAuxdata,
	LIBSQLite3SetAuxdata,
	LIBSQLite3ResultBlob,
	LIBSQLite3ResultError,
	LIBSQLite3ResultInt,
	LIBSQLite3ResultNull,
	LIBSQLite3ResultText,
	LIBSQLite3ResultValue,
	LIBSQLite3CreateCollation,
	LIBSQLite3CollationNeeded,
	LIBSQLite3Sleep,
	LIBSQLite3Expired,
	LIBSQLite3TransferBindings,
	LIBSQLite3GlobalRecover,
	LIBSQLite3GetAutocommit,
	LIBSQLite3DbHandle,

	// new functions in V41
	LIBSQLite3RollbackHook,
	LIBSQLite3EnableSharedCache,
	LIBSQLite3ReleaseMemory,
	LIBSQLite3SoftHeapLimit,
	LIBSQLite3ThreadCleanup,

	// new function in V42
	LIBSQLite3PrepareV2,

	// new function in V43
	LIBSQLite3CreateFunction,
	LIBSQLite3CreateModule,
	LIBSQLite3DeclareVtab,
	LIBSQLite3OverloadFunction,
	LIBSQLite3BlobOpen,
	LIBSQLite3BlobClose,
	LIBSQLite3BlobBytes,
	LIBSQLite3BlobRead,
	LIBSQLite3BlobWrite,
	LIBSQLite3ExtendedResultCodes,
	LIBSQLite3BindZeroBlob,
	LIBSQLite3ColumnDatabaseName,
	LIBSQLite3ColumnTableName,
	LIBSQLite3ColumnOriginName,
	LIBSQLite3ColumnValue,
	LIBSQLite3CreateCollationV2,
	LIBSQLite3LibVersion,
	LIBSQLite3LibversionNumber,
	LIBSQLite3ResultErrorToobig,
	LIBSQLite3ResultZeroBlob,
	LIBSQLite3ValueNumericType,

	// new function in V44
	LIBSQLite3ConfigV,
	LIBSQLlite3DbConfigV,
	LIBSQLite3VfsFind,
	LIBSQLite3VfsRegister,
	LIBSQLite3VfsUnregister,
	LIBSQLite3FileControl,
	LIBSQLite3Status,
	LIBSQLite3DbStatus,

	(APTR) -1
	};

static const struct TagItem maintags[] =
	{
	{MIT_Name,             (ULONG)"main"},
	{MIT_VectorTable,      (ULONG)functable},
	{MIT_Version,          1},
	{TAG_DONE,             0}
	};

/* Init table used in library initialization. */
static const ULONG interfaces[] =
{
   (ULONG)managertags,
   (ULONG)maintags,
   (ULONG)0
};

static const struct TagItem inittab[] =
	{
	{CLT_DataSize, (ULONG)sizeof(struct SQLite3Base)},
	{CLT_Interfaces, (ULONG) interfaces},
	{CLT_Vector68K,  (ULONG)VecTable68K},
	{CLT_InitFunc, (ULONG) Initlib},
	{TAG_DONE, 0}
	};


/* The ROM tag */
struct Resident ALIGNED romtag =
	{
	RTC_MATCHWORD,
	&romtag,
	&romtag + 1,
	RTF_NATIVE | RTF_AUTOINIT,
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	inittab
	};

/* ------------------- OS4 Manager Functions ------------------------ */
static struct Library * Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec)
{
	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;
	struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)libbase + libbase->lib_PosSize);
	struct LibraryManagerInterface *Self = (struct LibraryManagerInterface *) ExtLib->ILibrary;

	SysBase = (struct ExecBase *)pIExec->Data.LibBase;
	IExec = pIExec;
	SQLite3LibBase->sql3_LibNode.lib_Revision = LIB_REVISION;
	SQLite3LibBase->sql3_SegList = (struct SegList *)seglist;

	if (!SQLite3Init(SQLite3LibBase))
		{
		Expungelib(Self);
		SQLite3LibBase = NULL;
		}

	return SQLite3LibBase ? &SQLite3LibBase->sql3_LibNode : NULL;
}

static BPTR Expungelib(struct LibraryManagerInterface *Self)
{
	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) Self->Data.LibBase;

	if (0 == SQLite3LibBase->sql3_LibNode.lib_OpenCnt)
		{
		struct SegList *libseglist = SQLite3LibBase->sql3_SegList;

		Remove((struct Node *) SQLite3LibBase);
		SQLite3Cleanup(SQLite3LibBase);
		DeleteLibrary((struct Library *)SQLite3LibBase);

		return (BPTR)libseglist;
		}

	SQLite3LibBase->sql3_LibNode.lib_Flags |= LIBF_DELEXP;

	return (BPTR)NULL;
}

static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version)
{
	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) Self->Data.LibBase;

	SQLite3LibBase->sql3_LibNode.lib_OpenCnt++;
	SQLite3LibBase->sql3_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!SQLite3LibBase->sql3_Initialized)
		{
		if (!SQLite3Open(SQLite3LibBase))
			{
			KPrintF("SQLite3Open failed\n");
			Closelib(Self);
			return NULL;
			}
		SQLite3LibBase->sql3_Initialized = TRUE;
		}

	return (struct LibraryHeader *)SQLite3LibBase;
}

static BPTR Closelib(struct LibraryManagerInterface *Self)
{
	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) Self->Data.LibBase;

	SQLite3LibBase->sql3_LibNode.lib_OpenCnt--;
/*
	if (0 == SQLite3LibBase->sql3_LibNode.lib_OpenCnt)
		{
		if (SQLite3LibBase->sql3_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return Expungelib(Self);
			}
		}
*/
	return (BPTR)NULL;
}

static ULONG Obtainlib(struct LibraryManagerInterface *Self)
{
  return(Self->Data.RefCount++);
}

static ULONG Releaselib(struct LibraryManagerInterface *Self)
{
  return(Self->Data.RefCount--);
}

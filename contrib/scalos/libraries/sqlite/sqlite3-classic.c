// Preferences-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>
#include <proto/preferences.h>


#include "sqlite3_base.h"
#include "LibSQLite3.h"
#include "sqlite3.h"

//----------------------------------------------------------------------------
// Standard library functions

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase));

static LIBFUNC_PROTO(Openlib, libbase, struct Library *);
static LIBFUNC_PROTO(Closelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Expungelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Extfunclib, libbase, ULONG);

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
#ifdef __MORPHOS__
	(APTR) FUNCARRAY_32BIT_NATIVE,
#endif
	Openlib,
	Closelib,
	Expungelib,
	Extfunclib,
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

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct SQLite3Base),
	(ULONG) functable,
	0,
	(ULONG) Initlib
	};


/* The ROM tag */
struct Resident ALIGNED romtag =
	{
	RTC_MATCHWORD,
	&romtag,
	&romtag + 1,
#ifdef __MORPHOS__
	RTF_PPC | RTF_AUTOINIT,
#else
	RTF_AUTOINIT,
#endif
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	inittab
	};

#ifdef __MORPHOS__
ULONG __abox__=1;
#endif

//----------------------------------------------------------------------------

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase))
{
	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;

	/* store pointer to execbase for global access */
	SysBase = sysbase;

	SQLite3LibBase->sql3_LibNode.lib_Revision = LIB_REVISION;
	SQLite3LibBase->sql3_SegList = seglist;

	SQLite3LibBase->sql3_Initialized = FALSE;

	if (!SQLite3Init(SQLite3LibBase))
		{
		CALLLIBFUNC(Expungelib, &SQLite3LibBase->sql3_LibNode);
		SQLite3LibBase = NULL;
		}

	return SQLite3LibBase ? &SQLite3LibBase->sql3_LibNode : NULL;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;

	SQLite3LibBase->sql3_LibNode.lib_OpenCnt++;
	SQLite3LibBase->sql3_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!SQLite3LibBase->sql3_Initialized)
		{
		if (!SQLite3Open(SQLite3LibBase))
			{
			CALLLIBFUNC(Closelib, &SQLite3LibBase->sql3_LibNode);
			return NULL;
			}

		SQLite3LibBase->sql3_Initialized = TRUE;
		}

	return &SQLite3LibBase->sql3_LibNode;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, struct SegList *)
{
	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;

	SQLite3LibBase->sql3_LibNode.lib_OpenCnt--;

	if (0 == SQLite3LibBase->sql3_LibNode.lib_OpenCnt)
		{
		if (SQLite3LibBase->sql3_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &SQLite3LibBase->sql3_LibNode);
			}
		}

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, struct SegList *)
{
	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;

	if (0 == SQLite3LibBase->sql3_LibNode.lib_OpenCnt)
		{
		ULONG size = SQLite3LibBase->sql3_LibNode.lib_NegSize + SQLite3LibBase->sql3_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) SQLite3LibBase - SQLite3LibBase->sql3_LibNode.lib_NegSize;
		struct SegList *libseglist = SQLite3LibBase->sql3_SegList;

		Remove((struct Node *) SQLite3LibBase);
		SQLite3Cleanup(SQLite3LibBase);
		FreeMem(ptr,size);

		return libseglist;
		}

	SQLite3LibBase->sql3_LibNode.lib_Flags |= LIBF_DELEXP;

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	(void) libbase;

	return 0;
}
LIBFUNC_END

//----------------------------------------------------------------------------


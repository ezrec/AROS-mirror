// Preferences-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <aros/libcall.h>

#include <proto/exec.h>
#include <proto/preferences.h>


#include "sqlite3_base.h"
#include "LibSQLite3.h"
#include "sqlite3.h"

struct Library *aroscbase;

//----------------------------------------------------------------------------
// Standard library functions

static AROS_UFP3 (struct Library *, Initlib,
		  AROS_UFPA(struct Library *, libbase, D0),
		  AROS_UFPA(struct SegList *, seglist, A0),
		  AROS_UFPA(struct ExecBase *, sysbase, A6)
);
static AROS_LD1 (struct Library *, Openlib,
		 AROS_LPA (__unused ULONG, version, D0),
		 struct Library *, libbase, 1, SQLite3
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, SQLite3
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, libbase, 3, SQLite3
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, SQLite3
);


//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
	SQLite3_1_Openlib,
	SQLite3_2_Closelib,
	SQLite3_3_Expungelib,
	SQLite3_4_Extfunclib,
	SQLite3Base_0_LIBSQLite3Close,
	SQLite3Base_0_LIBSQLite3Exec,
	SQLite3Base_0_LIBSQLite3Changes,
	SQLite3Base_0_LIBSQLite3TotalChanges,
	SQLite3Base_0_LIBSQLite3Interrupt,
	SQLite3Base_0_LIBSQLite3Complete,
	SQLite3Base_0_LIBSQLite3BusyHandler,
	SQLite3Base_0_LIBSQLite3BusyTimeout,
	SQLite3Base_0_LIBSQLite3GetTable,
	SQLite3Base_0_LIBSQLite3FreeTable,
	SQLite3Base_0_LIBSQLite3Free,
	SQLite3Base_0_LIBSQLite3Trace,
	SQLite3Base_0_LIBSQLite3ProgressHandler,
	SQLite3Base_0_LIBSQLite3CommitHook,
	SQLite3Base_0_LIBSQLite3Open,
	SQLite3Base_0_LIBSQLite3Errcode,
	SQLite3Base_0_LIBSQLite3Errmsg,
	SQLite3Base_0_LIBSQLite3Prepare,
	SQLite3Base_0_LIBSQLite3BindBlob,
	SQLite3Base_0_LIBSQLite3BindInt,
	SQLite3Base_0_LIBSQLite3BindNull,
	SQLite3Base_0_LIBSQLite3BindText,
	SQLite3Base_0_LIBSQLite3BindValue,
	SQLite3Base_0_LIBSQLite3BindParameterCount,
	SQLite3Base_0_LIBSQLite3BindParameterName,
	SQLite3Base_0_LIBSQLite3BindParameterIndex,
	SQLite3Base_0_LIBSQLite3ClearBindings,
	SQLite3Base_0_LIBSQLite3ColumnCount,
	SQLite3Base_0_LIBSQLite3ColumnName,
	SQLite3Base_0_LIBSQLite3ColumnDecltype,
	SQLite3Base_0_LIBSQLite3Step,
	SQLite3Base_0_LIBSQLite3DataCount,
	SQLite3Base_0_LIBSQLite3ColumnBlob,
	SQLite3Base_0_LIBSQLite3ColumnBytes,
	SQLite3Base_0_LIBSQLite3ColumnInt,
	SQLite3Base_0_LIBSQLite3ColumnText,
	SQLite3Base_0_LIBSQLite3ColumnType,
	SQLite3Base_0_LIBSQLite3Finalize,
	SQLite3Base_0_LIBSQLite3Reset,
	SQLite3Base_0_LIBSQLite3AggregateCount,
	SQLite3Base_0_LIBSQLite3ValueBlob,
	SQLite3Base_0_LIBSQLite3ValueBytes,
	SQLite3Base_0_LIBSQLite3ValueInt,
	SQLite3Base_0_LIBSQLite3ValueText,
	SQLite3Base_0_LIBSQLite3ValueType,
	SQLite3Base_0_LIBSQLite3Aggregate_context,
	SQLite3Base_0_LIBSQLite3UserData,
	SQLite3Base_0_LIBSQLite3GetAuxdata,
	SQLite3Base_0_LIBSQLite3SetAuxdata,
	SQLite3Base_0_LIBSQLite3ResultBlob,
	SQLite3Base_0_LIBSQLite3ResultError,
	SQLite3Base_0_LIBSQLite3ResultInt,
	SQLite3Base_0_LIBSQLite3ResultNull,
	SQLite3Base_0_LIBSQLite3ResultText,
	SQLite3Base_0_LIBSQLite3ResultValue,
	SQLite3Base_0_LIBSQLite3CreateCollation,
	SQLite3Base_0_LIBSQLite3CollationNeeded,
	SQLite3Base_0_LIBSQLite3Sleep,
	SQLite3Base_0_LIBSQLite3Expired,
	SQLite3Base_0_LIBSQLite3TransferBindings,
	SQLite3Base_0_LIBSQLite3GlobalRecover,
	SQLite3Base_0_LIBSQLite3GetAutocommit,
	SQLite3Base_0_LIBSQLite3DbHandle,

	// new functions in V41
	SQLite3Base_0_LIBSQLite3RollbackHook,
	SQLite3Base_0_LIBSQLite3EnableSharedCache,
	SQLite3Base_0_LIBSQLite3ReleaseMemory,
	SQLite3Base_0_LIBSQLite3SoftHeapLimit,
	SQLite3Base_0_LIBSQLite3ThreadCleanup,

	// new function in V42
	SQLite3Base_0_LIBSQLite3PrepareV2,

	// new function in V43
	SQLite3Base_0_LIBSQLite3CreateFunction,
	SQLite3Base_0_LIBSQLite3CreateModule,
	SQLite3Base_0_LIBSQLite3DeclareVtab,
	SQLite3Base_0_LIBSQLite3OverloadFunction,
	SQLite3Base_0_LIBSQLite3BlobOpen,
	SQLite3Base_0_LIBSQLite3BlobClose,
	SQLite3Base_0_LIBSQLite3BlobBytes,
	SQLite3Base_0_LIBSQLite3BlobRead,
	SQLite3Base_0_LIBSQLite3BlobWrite,
	SQLite3Base_0_LIBSQLite3ExtendedResultCodes,
	SQLite3Base_0_LIBSQLite3BindZeroBlob,
	SQLite3Base_0_LIBSQLite3ColumnDatabaseName,
	SQLite3Base_0_LIBSQLite3ColumnTableName,
	SQLite3Base_0_LIBSQLite3ColumnOriginName,
	SQLite3Base_0_LIBSQLite3ColumnValue,
	SQLite3Base_0_LIBSQLite3CreateCollationV2,
	SQLite3Base_0_LIBSQLite3LibVersion,
	SQLite3Base_0_LIBSQLite3LibversionNumber,
	SQLite3Base_0_LIBSQLite3ResultErrorToobig,
	SQLite3Base_0_LIBSQLite3ResultZeroBlob,
	SQLite3Base_0_LIBSQLite3ValueNumericType,

	// new function in V44
	SQLite3Base_0_LIBSQLite3ConfigV,
	SQLite3Base_0_LIBSQLlite3DbConfigV,
	SQLite3Base_0_LIBSQLite3VfsFind,
	SQLite3Base_0_LIBSQLite3VfsRegister,
	SQLite3Base_0_LIBSQLite3VfsUnregister,
	SQLite3Base_0_LIBSQLite3FileControl,
	SQLite3Base_0_LIBSQLite3Status,
	SQLite3Base_0_LIBSQLite3DbStatus,

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
	RTF_AUTOINIT,
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	inittab
	};


//----------------------------------------------------------------------------

	static AROS_UFH3(struct Library *, Initlib,
		 AROS_UFHA(struct Library *, libbase, D0),
		 AROS_UFHA(struct SegList *, seglist, A0),
		 AROS_UFHA(struct ExecBase *, sysbase, A6)
)
{
	AROS_USERFUNC_INIT

	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;

	/* store pointer to execbase for global access */
	SysBase = sysbase;

	SQLite3LibBase->sql3_LibNode.lib_Revision = LIB_REVISION;
	SQLite3LibBase->sql3_SegList = seglist;

	SQLite3LibBase->sql3_Initialized = FALSE;

	aroscbase = OpenLibrary("arosc.library", 0);

	if (!aroscbase || !SQLite3Init(SQLite3LibBase))
		{
		SQLite3_3_Expungelib(&SQLite3LibBase->sql3_LibNode, &SQLite3LibBase->sql3_LibNode);
		SQLite3LibBase = NULL;
		}

	return SQLite3LibBase ? &SQLite3LibBase->sql3_LibNode : NULL;

	AROS_USERFUNC_EXIT
}


static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, SQLite3
)
{
	AROS_LIBFUNC_INIT

	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;

	SQLite3LibBase->sql3_LibNode.lib_OpenCnt++;
	SQLite3LibBase->sql3_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!SQLite3LibBase->sql3_Initialized)
		{
		if (!SQLite3Open(SQLite3LibBase))
			{
			SQLite3_2_Closelib(&SQLite3LibBase->sql3_LibNode);
			return NULL;
			}

		SQLite3LibBase->sql3_Initialized = TRUE;
		}

	return &SQLite3LibBase->sql3_LibNode;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, SQLite3
)
{
	AROS_LIBFUNC_INIT

	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;

	SQLite3LibBase->sql3_LibNode.lib_OpenCnt--;

	if (0 == SQLite3LibBase->sql3_LibNode.lib_OpenCnt)
		{
		if (SQLite3LibBase->sql3_LibNode.lib_Flags & LIBF_DELEXP)
			{
			SQLite3_3_Expungelib(&SQLite3LibBase->sql3_LibNode, &SQLite3LibBase->sql3_LibNode);
			}
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, SQLite3
)
{
	AROS_LIBFUNC_INIT

	struct SQLite3Base *SQLite3LibBase = (struct SQLite3Base *) libbase;

	if (0 == SQLite3LibBase->sql3_LibNode.lib_OpenCnt)
		{
		ULONG size = SQLite3LibBase->sql3_LibNode.lib_NegSize + SQLite3LibBase->sql3_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) SQLite3LibBase - SQLite3LibBase->sql3_LibNode.lib_NegSize;
		struct SegList *libseglist = SQLite3LibBase->sql3_SegList;

		Remove((struct Node *) SQLite3LibBase);
		SQLite3Cleanup(SQLite3LibBase);
		FreeMem(ptr,size);

		CloseLibrary(aroscbase);

		return libseglist;
		}

	SQLite3LibBase->sql3_LibNode.lib_Flags |= LIBF_DELEXP;

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(ULONG, Extfunclib,
		__unused struct Library *, libbase, 4, SQLite3
)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}

//----------------------------------------------------------------------------

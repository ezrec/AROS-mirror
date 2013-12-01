// LibSQLite3.h
// $Date$
// $Revision$


#ifndef LIBSQLITE3_H
#define	LIBSQLITE3_H

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/libraries.h>
#include <sqlite3.h>

#include <defs.h>

//----------------------------------------------------------------------------

#define	LIB_VERSION	44
#define	LIB_REVISION	34

//----------------------------------------------------------------------------

// Name for public semaphores
#define SQLITE3_SHARED_SEMA_NAME	"SQLite3_Shared"
#define SQLITE3_PENDING_SEMA_NAME    	"SQLite3_Pending"
#define SQLITE3_RESERVED_SEMA_NAME	"SQLite3_Reserved"

//----------------------------------------------------------------------------

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

extern struct ExecBase *SysBase;

//----------------------------------------------------------------------------

typedef int (*xBusyFunc)(void*,int);
typedef void (*xTraceFunc)(void*, const char*);
typedef int (*xProgressFunc)(void *);
typedef int (*xCommitFunc)(void*);
typedef void (*xDeleteFunc)(void*);
typedef int (*xCompareFunc)(void*,int,const void*,int,const void*);
typedef void (*xCollationNeededFunc)(void*, sqlite3*, int eTextRep, const char*);
typedef VOID (*xRollbackCallbackFunc)(VOID *);
typedef VOID (*xFunctionFunc)(sqlite3_context *pCtx, int i, sqlite3_value **pVal);
typedef VOID (*xFunctionStep)(sqlite3_context *pCtx, int i, sqlite3_value **pVal);
typedef VOID (*xFunctionFinal)(sqlite3_context *pCtx);
typedef int (*xCreateCollationCompare)(APTR p, int i, CONST_APTR p2, int j, CONST_APTR p3);
typedef VOID (*xCreateCollationDestroy)(APTR p);

//----------------------------------------------------------------------------

BOOL SQLite3Init(struct SQLite3Base *SQLite3Base);
BOOL SQLite3Open(struct SQLite3Base *SQLite3Base);
void SQLite3Cleanup(struct SQLite3Base *SQLite3Base);

LIBFUNC_P2_PROTO(LONG, LIBSQLite3Close,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P6_PROTO(LONG, LIBSQLite3Exec,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, sql,
	A2, sqlite3_callback, xCallback,
	A3, APTR, pArg,
	D0, STRPTR *, errmsg,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3Changes,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3TotalChanges,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(VOID, LIBSQLite3Interrupt,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3Complete,
	A0, CONST_STRPTR, sql,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(LONG, LIBSQLite3BusyHandler,
	A0, sqlite3 *, db,
	A1, xBusyFunc, callback,
	A2, APTR, userdata,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3BusyTimeout,
	A0, sqlite3 *, db,
	D0, LONG, ms,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P7_PROTO(LONG, LIBSQLite3GetTable,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, sql,
	A2, STRPTR **, presult,
	A3, LONG *, nrow,
	D0, LONG *, ncolumn,
	D1, STRPTR *, errmsg,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(VOID, LIBSQLite3FreeTable,
	A0, STRPTR *, result,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(VOID, LIBSQLite3Free,
	A0, STRPTR, z,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(APTR, LIBSQLite3Trace,
	A0, sqlite3 *, db,
	A1, xTraceFunc, xTrace,
	A2, APTR, parg,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(VOID, LIBSQLite3ProgressHandler,
	A0, sqlite3 *, db,
	D0, LONG, nOps,
	A1, xProgressFunc, xProgress,
	A2, APTR, pArg,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(APTR, LIBSQLite3CommitHook,
	A0, sqlite3 *, db,
	A1, xCommitFunc, xCallback,
	A2, APTR, pArg,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3Open,
	A0, CONST_STRPTR, filename,
	A1, sqlite3 **, ppDb,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3Errcode,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(CONST_STRPTR, LIBSQLite3Errmsg,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P6_PROTO(LONG, LIBSQLite3Prepare,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zSql,
	D0, LONG, nBytes,
	A2, sqlite3_stmt **, ppStmt,
	A3, CONST_STRPTR *, pzTail,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P6_PROTO(LONG, LIBSQLite3BindBlob,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A1, CONST_APTR, zData,
	D1, LONG, nData,
	A2, xDeleteFunc, xDel,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(LONG, LIBSQLite3BindInt,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	D1, LONG, iValue,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3BindNull,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P6_PROTO(LONG, LIBSQLite3BindText,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A1, CONST_STRPTR, zData,
	D1, LONG, nData,
	A2, xDeleteFunc, xDel,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(LONG, LIBSQLite3BindValue,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A1, CONST sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3BindParameterCount,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(CONST_STRPTR, LIBSQLite3BindParameterName,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3BindParameterIndex,
	A0, sqlite3_stmt *, pStmt,
	A1, CONST_STRPTR, zName,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3ClearBindings,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3ColumnCount,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(CONST_STRPTR, LIBSQLite3ColumnName,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(CONST_STRPTR, LIBSQLite3ColumnDecltype,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3Step,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3DataCount,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(CONST_APTR, LIBSQLite3ColumnBlob,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3ColumnBytes,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3ColumnInt,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(CONST_STRPTR, LIBSQLite3ColumnText,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3ColumnType,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3Finalize,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3Reset,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG , LIBSQLite3AggregateCount,
	A0, sqlite3_context *, pCtx,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(CONST_APTR, LIBSQLite3ValueBlob,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3ValueBytes,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3ValueInt,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(CONST_STRPTR, LIBSQLite3ValueText,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3ValueType,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(APTR, LIBSQLite3Aggregate_context,
	A0, sqlite3_context *, pCtx,
	D0, LONG, nBytes,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(APTR, LIBSQLite3UserData,
	A0, sqlite3_context *, pCtx,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(APTR, LIBSQLite3GetAuxdata,
	A0, sqlite3_context *, pCtx,
	D0, LONG, iArg,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(VOID, LIBSQLite3SetAuxdata,
	A0, sqlite3_context *, pCtx,
	D0, LONG, iArg,
	A1, APTR, pAux,
	A2, xDeleteFunc, xDelete,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(VOID, LIBSQLite3ResultBlob,
	A0, sqlite3_context *, pCtx,
	A1, CONST_APTR, z,
	D1, LONG, n,
	A2, xDeleteFunc, xDelete,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(VOID, LIBSQLite3ResultError,
	A0, sqlite3_context *, pCtx,
	A1, CONST_STRPTR, z,
	D0, LONG, n,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(VOID, LIBSQLite3ResultInt,
	A0, sqlite3_context *, pCtx,
	D0, LONG, iVal,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(VOID, LIBSQLite3ResultNull,
	A0, sqlite3_context *, pCtx,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(VOID, LIBSQLite3ResultText,
	A0, sqlite3_context *, pCtx,
	A1, CONST_STRPTR, z,
	D1, LONG, n,
	A2, xDeleteFunc, xDelete,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(VOID, LIBSQLite3ResultValue,
	A0, sqlite3_context *, pCtx,
	A1, sqlite3_value *, pValue,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P6_PROTO(LONG, LIBSQLite3CreateCollation,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zName,
	D0, LONG, eTextRep,
	A2, APTR, pCtx,
	A3, xCompareFunc, xCompare,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(LONG, LIBSQLite3CollationNeeded,
	A0, sqlite3 *, db,
	D0, APTR, pCollNeededArg,
	A1, xCollationNeededFunc, xCollNeeded,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3Sleep,
	D0, LONG, ms,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3Expired,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3TransferBindings,
	A0, sqlite3_stmt *, pFromStmt,
	A1, sqlite3_stmt *, pToStmt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P1_PROTO(LONG, LIBSQLite3GlobalRecover,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3GetAutocommit,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(sqlite3 *, LIBSQLite3DbHandle,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base);

// new functions in V41
LIBFUNC_P4_PROTO(APTR, LIBSQLite3RollbackHook,
	A0, sqlite3 *, db,
	A1, xRollbackCallbackFunc, callback,
	A2, APTR, pUserData,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3EnableSharedCache,
	D0, BOOL, enable,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3ReleaseMemory,
	D0, LONG, bytesCount,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(VOID, LIBSQLite3SoftHeapLimit,
	D0, LONG, maxBytes,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P1_PROTO(VOID, LIBSQLite3ThreadCleanup,
	A6, struct SQLite3Base *, SQLite3Base);

// new function in V42
LIBFUNC_P6_PROTO(LONG, LIBSQLite3PrepareV2,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zSql,
	D0, LONG, nBytes,
	A2, sqlite3_stmt **, ppStmt,
	A3, CONST_STRPTR *, pzTail,
	A6, struct SQLite3Base *, SQLite3Base);

// new functions in V43
LIBFUNC_P9_PROTO(LONG, LIBSQLite3CreateFunction,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zFunctionName,
	D0, LONG, nArg,
	D1, LONG, eTextRep,
	A2, APTR, userdata,
	A3, xFunctionFunc, xFunc,
	D2, xFunctionStep, xStep,
	D3, xFunctionFinal, xFinal,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(LONG, LIBSQLite3CreateModule,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zName,
	A2, CONST sqlite3_module *, methods,
	A3, APTR, clientData,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3DeclareVtab,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zCreateTable,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(LONG, LIBSQLite3OverloadFunction,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zFuncName,
	D0, LONG, nArg,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P8_PROTO(LONG, LIBSQLite3BlobOpen,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zDb,
	A2, CONST_STRPTR, zTable,
	A3, CONST_STRPTR, zColumn,
	D0, LONG, iRow,
	D1, LONG, flags,
	A4, sqlite3_blob **, ppBlob,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3BlobClose,
	A0, sqlite3_blob *, blob,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3BlobBytes,
	A0, sqlite3_blob *, blob,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(LONG, LIBSQLite3BlobRead,
	A0, sqlite3_blob *, blob,
	A1, APTR, z,
	D0, LONG, n,
	D1, LONG, iOffset,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(LONG, LIBSQLite3BlobWrite,
	A0, sqlite3_blob *, blob,
	A1, CONST_APTR, z,
	D0, LONG, n,
	D1, LONG, iOffset,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3ExtendedResultCodes,
	A0, sqlite3 *, db,
	D0, LONG, onoff,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(LONG, LIBSQLite3BindZeroBlob,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	D1, LONG, n,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(CONST_STRPTR, LIBSQLite3ColumnDatabaseName,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, iCol,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(CONST_STRPTR, LIBSQLite3ColumnTableName,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, iCol,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(CONST_STRPTR, LIBSQLite3ColumnOriginName,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, iCol,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(sqlite3_value *, LIBSQLite3ColumnValue,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, iCol,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P7_PROTO(LONG, LIBSQLite3CreateCollationV2,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zName,
	D0, LONG, eTextRep,
	A2, APTR, pCtx,
	A3, xCreateCollationCompare, xCompare,
	D1, xCreateCollationDestroy, xDestroy,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P1_PROTO(CONST_STRPTR, LIBSQLite3LibVersion,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P1_PROTO(LONG, LIBSQLite3LibversionNumber,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(VOID, LIBSQLite3ResultErrorToobig,
	A0, sqlite3_context *, pCtx,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(VOID, LIBSQLite3ResultZeroBlob,
	A0, sqlite3_context *, pCtx,
	D0, LONG, n,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3ValueNumericType,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base);

// new functions in V43
LIBFUNC_P3_PROTO(LONG, LIBSQLite3ConfigV,
	D0, LONG, op,
	A0, APTR, ap,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P4_PROTO(LONG, LIBSQLlite3DbConfigV,
	A0, sqlite3 *, db,
	D0, LONG, op,
	A1, APTR, ap,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(sqlite3_vfs *, LIBSQLite3VfsFind,
	A0, CONST_STRPTR, zVfsName,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P3_PROTO(LONG, LIBSQLite3VfsRegister,
	A0, sqlite3_vfs *, vfs,
	D0, LONG, makeDflt,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P2_PROTO(LONG, LIBSQLite3VfsUnregister,
	A0, sqlite3_vfs *, vfs,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(LONG, LIBSQLite3FileControl,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zDbName,
	D0, LONG, op,
	A2, void *, arg,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P5_PROTO(LONG, LIBSQLite3Status,
	D0, LONG, op,
	A0, LONG *, pCurrent,
	A1, LONG *, pHighwater,
	D1, LONG, resetFlag,
	A6, struct SQLite3Base *, SQLite3Base);
LIBFUNC_P6_PROTO(LONG, LIBSQLite3DbStatus,
	A0, sqlite3 *, db,
	D0, LONG, op,
	A1, LONG *, pCur,
	A2, LONG *, pHiwtr,
	D1, LONG, resetFlg,
	A6, struct SQLite3Base *, SQLite3Base);

//-----------------------------------------------------------------------------

// defined in  mempools.lib

extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);

//----------------------------------------------------------------------------

// special Amiga-specific vararg functions:

int sqlite3_configv(int op, va_list ap);
int sqlite3_db_configv(sqlite3 *db, int op, va_list ap);

//----------------------------------------------------------------------------

sqlite3_mutex_methods *sqlite3OtherMutex(void);

//----------------------------------------------------------------------------

#endif /* LIBSQLITE3_H */

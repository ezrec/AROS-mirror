#ifndef CLIB_SQLITE3_PROTOS_H
#define CLIB_SQLITE3_PROTOS_H


/*
**	$VER: sqlite3_protos.h 2539 (17.08.2008)
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 2008 ©2006 The Scalos Team
**	All Rights Reserved
*/

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  SQLITE3_H
#include <sqlite3.h>
#endif

LONG SQLite3Close(sqlite3 * db);
LONG SQLite3Exec(sqlite3 * db, CONST_STRPTR sql, sqlite3_callback xCallback, APTR pArg,
	STRPTR * errmsg);
LONG SQLite3Changes(sqlite3 * db);
LONG SQLite3TotalChanges(sqlite3 * db);
VOID SQLite3Interrupt(sqlite3 * db);
LONG SQLite3Complete(CONST_STRPTR sql);
LONG SQLite3BusyHandler(sqlite3 * db, LONG (*callback)(APTR userdata, LONG l), APTR userdata);
LONG SQLite3BusyTimeout(sqlite3 * db, LONG ms);
LONG SQLite3GetTable(sqlite3 * db, CONST_STRPTR sql, STRPTR ** presult, LONG * nrow,
	LONG * ncolumn, STRPTR * errmsg);
VOID SQLite3FreeTable(STRPTR * result);
VOID SQLite3Free(STRPTR z);
APTR SQLite3Trace(sqlite3 * db, VOID (*xTrace)(APTR p, CONST_STRPTR z), APTR parg);
VOID SQLite3ProgressHandler(sqlite3 * db, LONG nOps, LONG (*xProgress)(APTR p), APTR pArg);
APTR SQLite3CommitHook(sqlite3 * db, LONG (*xCallback)(APTR p), APTR pArg);
LONG SQLite3Open(CONST_STRPTR filename, sqlite3 ** ppDb);
LONG SQLite3Errcode(sqlite3 * db);
CONST_STRPTR SQLite3Errmsg(sqlite3 * db);
LONG SQLite3Prepare(sqlite3 * db, CONST_STRPTR zSql, LONG nBytes, sqlite3_stmt ** ppStmt,
	CONST_STRPTR * pzTail);
LONG SQLite3BindBlob(sqlite3_stmt * pStmt, LONG i, CONST_APTR zData, LONG nData,
	VOID (*xDel)(APTR p));
LONG SQLite3BindInt(sqlite3_stmt * pStmt, LONG i, LONG iValue);
LONG SQLite3BindNull(sqlite3_stmt * pStmt, LONG i);
LONG SQLite3BindText(sqlite3_stmt * pStmt, LONG i, CONST_STRPTR zData, LONG nData,
	VOID (*xDel)(APTR p));
LONG SQLite3BindValue(sqlite3_stmt * pStmt, LONG i, CONST sqlite3_value * pVal);
LONG SQLite3BindParameterCount(sqlite3_stmt * pStmt);
CONST_STRPTR SQLite3BindParameterName(sqlite3_stmt * pStmt, LONG i);
LONG SQLite3BindParameterIndex(sqlite3_stmt * pStmt, CONST_STRPTR zName);
LONG SQLite3ClearBindings(sqlite3_stmt * pStmt);
LONG SQLite3ColumnCount(sqlite3_stmt * pStmt);
CONST_STRPTR SQLite3ColumnName(sqlite3_stmt * pStmt, LONG i);
CONST_STRPTR SQLite3ColumnDecltype(sqlite3_stmt * pStmt, LONG i);
LONG SQLite3Step(sqlite3_stmt * pStmt);
LONG SQLite3DataCount(sqlite3_stmt * pStmt);
CONST_APTR SQLite3ColumnBlob(sqlite3_stmt * pStmt, LONG iCol);
LONG SQLite3ColumnBytes(sqlite3_stmt * pStmt, LONG iCol);
LONG SQLite3ColumnInt(sqlite3_stmt * pStmt, LONG iCol);
CONST_STRPTR SQLite3ColumnText(sqlite3_stmt * pStmt, LONG iCol);
LONG SQLite3ColumnType(sqlite3_stmt * pStmt, LONG iCol);
LONG SQLite3Finalize(sqlite3_stmt * pStmt);
LONG SQLite3Reset(sqlite3_stmt * pStmt);
LONG SQLite3AggregateCount(sqlite3_context * pCtx);
CONST_APTR SQLite3ValueBlob(sqlite3_value * pVal);
LONG SQLite3ValueBytes(sqlite3_value * pVal);
LONG SQLite3ValueInt(sqlite3_value * pVal);
CONST_STRPTR SQLite3ValueText(sqlite3_value * pVal);
LONG SQLite3ValueType(sqlite3_value * pVal);
APTR SQLite3Aggregate_context(sqlite3_context * pCtx, LONG nBytes);
APTR SQLite3UserData(sqlite3_context * pCtx);
APTR SQLite3GetAuxdata(sqlite3_context * pCtx, LONG iArg);
VOID SQLite3SetAuxdata(sqlite3_context * pCtx, LONG iARg, APTR pAux, VOID (*xDelete)(APTR p));
VOID SQLite3ResultBlob(sqlite3_context * pCtx, CONST_APTR z, LONG n, VOID (*xDelete)(APTR p));
VOID SQLite3ResultError(sqlite3_context * pCtx, CONST_STRPTR z, LONG n);
VOID SQLite3ResultInt(sqlite3_context * pCtx, LONG iVal);
VOID SQLite3ResultNull(sqlite3_context * pCtx);
VOID SQLite3ResultText(sqlite3_context * pCtx, CONST_STRPTR z, LONG n, VOID (*xDelete)(APTR p));
VOID SQLite3ResultValue(sqlite3_context * pCtx, sqlite3_value * pValue);
LONG SQLite3CreateCollation(sqlite3 * db, CONST_STRPTR zName, LONG eTextRep, APTR pCtx,
	LONG (*xCompare)(APTR p, LONG i, CONST_APTR p2, LONG j, CONST_APTR p3));
LONG SQLite3CollationNeeded(sqlite3 * db, APTR pCollNeededArg,
	VOID (*xCollNeeded)(APTR p, sqlite3 *dv, LONG eTextRep, CONST_STRPTR z));
LONG SQLite3Sleep(LONG ms);
LONG SQLite3Expired(sqlite3_stmt * pStmt);
LONG SQLite3TransferBindings(sqlite3_stmt * pFromStmt, sqlite3_stmt * pToStmt);
LONG SQLite3GlobalRecover(void);
LONG SQLite3GetAutocommit(sqlite3 * db);
sqlite3 * SQLite3DbHandle(sqlite3_stmt * pStmt);
APTR SQLite3RollbackHook(sqlite3 * db, VOID (*callback)(APTR pUserData), APTR pUserData);
LONG SQLite3EnableSharedCache(LONG enable);
LONG SQLite3ReleaseMemory(LONG bytesCount);
VOID SQLite3SoftHeapLimit(LONG maxBytes);
VOID SQLite3ThreadCleanup(void);
LONG SQLite3PrepareV2(sqlite3 * db, CONST_STRPTR zSql, LONG nBytes, sqlite3_stmt ** ppStmt,
	CONST_STRPTR * pzTail);
LONG SQLite3CreateFunction(sqlite3 * db, CONST_STRPTR zFunctionName, LONG nArg, LONG eTextRep,
	APTR userdata,
	VOID (*xFunc)(sqlite3_context *pCtx, LONG i, sqlite3_value **pVal),
	VOID (*xStep)(sqlite3_context *pCtx, LONG i, sqlite3_value **pVal),
	VOID (*xFinal)(sqlite3_context *pCtx));
LONG SQLite3CreateModule(sqlite3 * db, CONST_STRPTR zName, CONST sqlite3_module * methods,
	APTR clientData);
LONG SQLite3DeclareVtab(sqlite3 * db, CONST_STRPTR zCreateTable);
LONG SQLite3OverloadFunction(sqlite3 * db, CONST_STRPTR zFuncName, LONG nArg);
LONG SQLite3BlobOpen(sqlite3 * db, CONST_STRPTR zDb, CONST_STRPTR zTable,
	CONST_STRPTR zColumn, LONG iRow, LONG flags, sqlite3_blob ** ppBlob);
LONG SQLite3BlobClose(sqlite3_blob * blob);
LONG SQLite3BlobBytes(sqlite3_blob * blob);
LONG SQLite3BlobRead(sqlite3_blob * blob, APTR z, LONG n, LONG iOffset);
LONG SQLite3BlobWrite(sqlite3_blob * blob, CONST_APTR z, LONG n, LONG iOffset);
LONG SQLite3ExtendedResultCodes(sqlite3 * db, LONG onoff);
LONG SQLite3BindZeroBlob(sqlite3_stmt * pStmt, LONG i, LONG n);
CONST_STRPTR SQLite3ColumnDatabaseName(sqlite3_stmt * stmt, LONG n);
CONST_STRPTR SQLite3ColumnTableName(sqlite3_stmt * stmt, LONG n);
CONST_STRPTR SQLite3ColumnOriginName(sqlite3_stmt * stmt, LONG n);
sqlite3_value * SQLite3ColumnValue(sqlite3_stmt * pStmt, LONG iCol);
LONG SQLite3CreateCollationV2(sqlite3 * db, CONST_STRPTR zName, LONG eTextRep, APTR pCtx,
	LONG (*xCompare)(APTR p, LONG i, CONST_APTR p2, LONG j, CONST_APTR p3),
	VOID (*xDestroy)(APTR));
CONST_STRPTR SQLite3LibVersion(void);
LONG SQLite3LibversionNumber(void);
VOID SQLite3ResultErrorToobig(sqlite3_context * pCtx);
VOID SQLite3ResultZeroBlob(sqlite3_context * pCtx, LONG n);
LONG SQLite3ValueNumericType(sqlite3_value * pVal);
LONG SQLite3ConfigV(LONG op, APTR ap);
LONG SQLite3Config(LONG op, ULONG ap, ...);
LONG SQLlite3DbConfigV(sqlite3 * db, LONG op, APTR ap);
LONG SQLlite3DbConfig(sqlite3 * db, LONG op, ULONG ap, ...);
sqlite3_vfs * SQLite3VfsFind(CONST_STRPTR zVfsName);
LONG SQLite3VfsRegister(sqlite3_vfs * vfs, LONG makeDflt);
LONG SQLite3VfsUnregister(sqlite3_vfs * vfs);
LONG SQLite3FileControl(sqlite3 * db, CONST_STRPTR zDbName, LONG op, void * arg);
LONG SQLite3Status(LONG op, LONG * pCurrent, LONG * pHighwater, LONG resetFlag);
LONG SQLite3DbStatus(sqlite3 * db, LONG op, LONG * pCur, LONG * pHiwtr, LONG resetFlg);

#endif	/*  CLIB_SQLITE3_PROTOS_H  */

#ifndef _INLINE_SQLITE3_H
#define _INLINE_SQLITE3_H

#ifndef CLIB_SQLITE3_PROTOS_H
#define CLIB_SQLITE3_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  SQLITE3_H
#include <sqlite3.h>
#endif

#ifndef SQLITE3_BASE_NAME
#define SQLITE3_BASE_NAME SQLite3Base
#endif

#define SQLite3Close(db) \
	LP1(0x1e, LONG, SQLite3Close, sqlite3 *, db, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Exec(db, sql, xCallback, pArg, errmsg) \
	LP5(0x24, LONG, SQLite3Exec, sqlite3 *, db, a0, CONST_STRPTR, sql, a1, sqlite3_callback, xCallback, a2, APTR, pArg, a3, STRPTR *, errmsg, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Changes(db) \
	LP1(0x2a, LONG, SQLite3Changes, sqlite3 *, db, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3TotalChanges(db) \
	LP1(0x30, LONG, SQLite3TotalChanges, sqlite3 *, db, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Interrupt(db) \
	LP1NR(0x36, SQLite3Interrupt, sqlite3 *, db, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Complete(sql) \
	LP1(0x3c, LONG, SQLite3Complete, CONST_STRPTR, sql, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3BusyHandler(db, callback, userdata) \
	LP3FP(0x42, LONG, SQLite3BusyHandler, sqlite3 *, db, a0, __fpt, callback, a1, APTR, userdata, a2, \
	, SQLITE3_BASE_NAME, LONG (*__fpt)(APTR userdata, LONG l))

#define SQLite3BusyTimeout(db, ms) \
	LP2(0x48, LONG, SQLite3BusyTimeout, sqlite3 *, db, a0, LONG, ms, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3GetTable(db, sql, presult, nrow, ncolumn, errmsg) \
	LP6(0x4e, LONG, SQLite3GetTable, sqlite3 *, db, a0, CONST_STRPTR, sql, a1, STRPTR **, presult, a2, LONG *, nrow, a3, LONG *, ncolumn, d0, STRPTR *, errmsg, d1, \
	, SQLITE3_BASE_NAME)

#define SQLite3FreeTable(result) \
	LP1NR(0x54, SQLite3FreeTable, STRPTR *, result, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Free(z) \
	LP1NR(0x5a, SQLite3Free, STRPTR, z, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Trace(db, xTrace, parg) \
	LP3FP(0x60, APTR, SQLite3Trace, sqlite3 *, db, a0, __fpt, xTrace, a1, APTR, parg, a2, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(APTR p, CONST_STRPTR z))

#define SQLite3ProgressHandler(db, nOps, xProgress, pArg) \
	LP4NRFP(0x66, SQLite3ProgressHandler, sqlite3 *, db, a0, LONG, nOps, d0, __fpt, xProgress, a1, APTR, pArg, a2, \
	, SQLITE3_BASE_NAME, LONG (*__fpt)(APTR p))

#define SQLite3CommitHook(db, xCallback, pArg) \
	LP3FP(0x6c, APTR, SQLite3CommitHook, sqlite3 *, db, a0, __fpt, xCallback, a1, APTR, pArg, a2, \
	, SQLITE3_BASE_NAME, LONG (*__fpt)(APTR p))

#define SQLite3Open(filename, ppDb) \
	LP2(0x72, LONG, SQLite3Open, CONST_STRPTR, filename, a0, sqlite3 **, ppDb, a1, \
	, SQLITE3_BASE_NAME)

#define SQLite3Errcode(db) \
	LP1(0x78, LONG, SQLite3Errcode, sqlite3 *, db, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Errmsg(db) \
	LP1(0x7e, CONST_STRPTR, SQLite3Errmsg, sqlite3 *, db, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Prepare(db, zSql, nBytes, ppStmt, pzTail) \
	LP5(0x84, LONG, SQLite3Prepare, sqlite3 *, db, a0, CONST_STRPTR, zSql, a1, LONG, nBytes, d0, sqlite3_stmt **, ppStmt, a2, CONST_STRPTR *, pzTail, a3, \
	, SQLITE3_BASE_NAME)

#define SQLite3BindBlob(pStmt, i, zData, nData, xDel) \
	LP5FP(0x8a, LONG, SQLite3BindBlob, sqlite3_stmt *, pStmt, a0, LONG, i, d0, CONST_APTR, zData, a1, LONG, nData, d1, __fpt, xDel, a2, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(APTR p))

#define SQLite3BindInt(pStmt, i, iValue) \
	LP3(0x90, LONG, SQLite3BindInt, sqlite3_stmt *, pStmt, a0, LONG, i, d0, LONG, iValue, d1, \
	, SQLITE3_BASE_NAME)

#define SQLite3BindNull(pStmt, i) \
	LP2(0x96, LONG, SQLite3BindNull, sqlite3_stmt *, pStmt, a0, LONG, i, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3BindText(pStmt, i, zData, nData, xDel) \
	LP5FP(0x9c, LONG, SQLite3BindText, sqlite3_stmt *, pStmt, a0, LONG, i, d0, CONST_STRPTR, zData, a1, LONG, nData, d1, __fpt, xDel, a2, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(APTR p))

#define SQLite3BindValue(pStmt, i, pVal) \
	LP3(0xa2, LONG, SQLite3BindValue, sqlite3_stmt *, pStmt, a0, LONG, i, d0, CONST sqlite3_value *, pVal, a1, \
	, SQLITE3_BASE_NAME)

#define SQLite3BindParameterCount(pStmt) \
	LP1(0xa8, LONG, SQLite3BindParameterCount, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3BindParameterName(pStmt, i) \
	LP2(0xae, CONST_STRPTR, SQLite3BindParameterName, sqlite3_stmt *, pStmt, a0, LONG, i, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3BindParameterIndex(pStmt, zName) \
	LP2(0xb4, LONG, SQLite3BindParameterIndex, sqlite3_stmt *, pStmt, a0, CONST_STRPTR, zName, a1, \
	, SQLITE3_BASE_NAME)

#define SQLite3ClearBindings(pStmt) \
	LP1(0xba, LONG, SQLite3ClearBindings, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnCount(pStmt) \
	LP1(0xc0, LONG, SQLite3ColumnCount, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnName(pStmt, i) \
	LP2(0xc6, CONST_STRPTR, SQLite3ColumnName, sqlite3_stmt *, pStmt, a0, LONG, i, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnDecltype(pStmt, i) \
	LP2(0xcc, CONST_STRPTR, SQLite3ColumnDecltype, sqlite3_stmt *, pStmt, a0, LONG, i, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Step(pStmt) \
	LP1(0xd2, LONG, SQLite3Step, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3DataCount(pStmt) \
	LP1(0xd8, LONG, SQLite3DataCount, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnBlob(pStmt, iCol) \
	LP2(0xde, CONST_APTR, SQLite3ColumnBlob, sqlite3_stmt *, pStmt, a0, LONG, iCol, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnBytes(pStmt, iCol) \
	LP2(0xe4, LONG, SQLite3ColumnBytes, sqlite3_stmt *, pStmt, a0, LONG, iCol, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnInt(pStmt, iCol) \
	LP2(0xea, LONG, SQLite3ColumnInt, sqlite3_stmt *, pStmt, a0, LONG, iCol, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnText(pStmt, iCol) \
	LP2(0xf0, CONST_STRPTR, SQLite3ColumnText, sqlite3_stmt *, pStmt, a0, LONG, iCol, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnType(pStmt, iCol) \
	LP2(0xf6, LONG, SQLite3ColumnType, sqlite3_stmt *, pStmt, a0, LONG, iCol, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Finalize(pStmt) \
	LP1(0xfc, LONG, SQLite3Finalize, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Reset(pStmt) \
	LP1(0x102, LONG, SQLite3Reset, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3AggregateCount(pCtx) \
	LP1(0x108, LONG, SQLite3AggregateCount, sqlite3_context *, pCtx, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ValueBlob(pVal) \
	LP1(0x10e, CONST_APTR, SQLite3ValueBlob, sqlite3_value *, pVal, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ValueBytes(pVal) \
	LP1(0x114, LONG, SQLite3ValueBytes, sqlite3_value *, pVal, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ValueInt(pVal) \
	LP1(0x11a, LONG, SQLite3ValueInt, sqlite3_value *, pVal, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ValueText(pVal) \
	LP1(0x120, CONST_STRPTR, SQLite3ValueText, sqlite3_value *, pVal, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ValueType(pVal) \
	LP1(0x126, LONG, SQLite3ValueType, sqlite3_value *, pVal, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Aggregate_context(pCtx, nBytes) \
	LP2(0x12c, APTR, SQLite3Aggregate_context, sqlite3_context *, pCtx, a0, LONG, nBytes, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3UserData(pCtx) \
	LP1(0x132, APTR, SQLite3UserData, sqlite3_context *, pCtx, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3GetAuxdata(pCtx, iArg) \
	LP2(0x138, APTR, SQLite3GetAuxdata, sqlite3_context *, pCtx, a0, LONG, iArg, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3SetAuxdata(pCtx, iARg, pAux, xDelete) \
	LP4NRFP(0x13e, SQLite3SetAuxdata, sqlite3_context *, pCtx, a0, LONG, iARg, d0, APTR, pAux, a1, __fpt, xDelete, a2, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(APTR p))

#define SQLite3ResultBlob(pCtx, z, n, xDelete) \
	LP4NRFP(0x144, SQLite3ResultBlob, sqlite3_context *, pCtx, a0, CONST_APTR, z, a1, LONG, n, d0, __fpt, xDelete, a2, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(APTR p))

#define SQLite3ResultError(pCtx, z, n) \
	LP3NR(0x14a, SQLite3ResultError, sqlite3_context *, pCtx, a0, CONST_STRPTR, z, a1, LONG, n, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ResultInt(pCtx, iVal) \
	LP2NR(0x150, SQLite3ResultInt, sqlite3_context *, pCtx, a0, LONG, iVal, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ResultNull(pCtx) \
	LP1NR(0x156, SQLite3ResultNull, sqlite3_context *, pCtx, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ResultText(pCtx, z, n, xDelete) \
	LP4NRFP(0x15c, SQLite3ResultText, sqlite3_context *, pCtx, a0, CONST_STRPTR, z, a1, LONG, n, d0, __fpt, xDelete, a2, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(APTR p))

#define SQLite3ResultValue(pCtx, pValue) \
	LP2NR(0x162, SQLite3ResultValue, sqlite3_context *, pCtx, a0, sqlite3_value *, pValue, a1, \
	, SQLITE3_BASE_NAME)

#define SQLite3CreateCollation(db, zName, eTextRep, pCtx, xCompare) \
	LP5FP(0x168, LONG, SQLite3CreateCollation, sqlite3 *, db, a0, CONST_STRPTR, zName, a1, LONG, eTextRep, d0, APTR, pCtx, a2, __fpt, xCompare, a3, \
	, SQLITE3_BASE_NAME, LONG (*__fpt)(APTR p, LONG i, CONST_APTR p2, LONG j, CONST_APTR p3))

#define SQLite3CollationNeeded(db, pCollNeededArg, xCollNeeded) \
	LP3FP(0x16e, LONG, SQLite3CollationNeeded, sqlite3 *, db, a0, APTR, pCollNeededArg, d0, __fpt, xCollNeeded, a1, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(APTR p, sqlite3 *dv, LONG eTextRep, CONST_STRPTR z))

#define SQLite3Sleep(ms) \
	LP1(0x174, LONG, SQLite3Sleep, LONG, ms, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3Expired(pStmt) \
	LP1(0x17a, LONG, SQLite3Expired, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3TransferBindings(pFromStmt, pToStmt) \
	LP2(0x180, LONG, SQLite3TransferBindings, sqlite3_stmt *, pFromStmt, a0, sqlite3_stmt *, pToStmt, a1, \
	, SQLITE3_BASE_NAME)

#define SQLite3GlobalRecover() \
	LP0(0x186, LONG, SQLite3GlobalRecover, \
	, SQLITE3_BASE_NAME)

#define SQLite3GetAutocommit(db) \
	LP1(0x18c, LONG, SQLite3GetAutocommit, sqlite3 *, db, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3DbHandle(pStmt) \
	LP1(0x192, sqlite3 *, SQLite3DbHandle, sqlite3_stmt *, pStmt, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3RollbackHook(db, callback, pUserData) \
	LP3FP(0x198, APTR, SQLite3RollbackHook, sqlite3 *, db, a0, __fpt, callback, a1, APTR, pUserData, a2, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(APTR pUserData))

#define SQLite3EnableSharedCache(enable) \
	LP1(0x19e, LONG, SQLite3EnableSharedCache, LONG, enable, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ReleaseMemory(bytesCount) \
	LP1(0x1a4, LONG, SQLite3ReleaseMemory, LONG, bytesCount, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3SoftHeapLimit(maxBytes) \
	LP1NR(0x1aa, SQLite3SoftHeapLimit, LONG, maxBytes, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ThreadCleanup() \
	LP0NR(0x1b0, SQLite3ThreadCleanup, \
	, SQLITE3_BASE_NAME)

#define SQLite3PrepareV2(db, zSql, nBytes, ppStmt, pzTail) \
	LP5(0x1b6, LONG, SQLite3PrepareV2, sqlite3 *, db, a0, CONST_STRPTR, zSql, a1, LONG, nBytes, d0, sqlite3_stmt **, ppStmt, a2, CONST_STRPTR *, pzTail, a3, \
	, SQLITE3_BASE_NAME)

#define SQLite3CreateFunction(db, zFunctionName, nArg, eTextRep, userdata, xFunc, xStep, xFinal) \
	LP8FP(0x1bc, LONG, SQLite3CreateFunction, sqlite3 *, db, a0, CONST_STRPTR, zFunctionName, a1, LONG, nArg, d0, LONG, eTextRep, d1, APTR, userdata, a2, __fpt, xFunc, a3, void *, xStep, d2, void *, xFinal, d3, \
	, SQLITE3_BASE_NAME, VOID (*__fpt)(sqlite3_context *pCtx, LONG i, sqlite3_value **pVal))

#define SQLite3CreateModule(db, zName, methods, clientData) \
	LP4(0x1c2, LONG, SQLite3CreateModule, sqlite3 *, db, a0, CONST_STRPTR, zName, a1, CONST sqlite3_module *, methods, a2, APTR, clientData, a3, \
	, SQLITE3_BASE_NAME)

#define SQLite3DeclareVtab(db, zCreateTable) \
	LP2(0x1c8, LONG, SQLite3DeclareVtab, sqlite3 *, db, a0, CONST_STRPTR, zCreateTable, a1, \
	, SQLITE3_BASE_NAME)

#define SQLite3OverloadFunction(db, zFuncName, nArg) \
	LP3(0x1ce, LONG, SQLite3OverloadFunction, sqlite3 *, db, a0, CONST_STRPTR, zFuncName, a1, LONG, nArg, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3BlobOpen(db, zDb, zTable, zColumn, iRow, flags, ppBlob) \
	LP7A4(0x1d4, LONG, SQLite3BlobOpen, sqlite3 *, db, a0, CONST_STRPTR, zDb, a1, CONST_STRPTR, zTable, a2, CONST_STRPTR, zColumn, a3, LONG, iRow, d0, LONG, flags, d1, sqlite3_blob **, ppBlob, d7, \
	, SQLITE3_BASE_NAME)

#define SQLite3BlobClose(blob) \
	LP1(0x1da, LONG, SQLite3BlobClose, sqlite3_blob *, blob, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3BlobBytes(blob) \
	LP1(0x1e0, LONG, SQLite3BlobBytes, sqlite3_blob *, blob, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3BlobRead(blob, z, n, iOffset) \
	LP4(0x1e6, LONG, SQLite3BlobRead, sqlite3_blob *, blob, a0, APTR, z, a1, LONG, n, d0, LONG, iOffset, d1, \
	, SQLITE3_BASE_NAME)

#define SQLite3BlobWrite(blob, z, n, iOffset) \
	LP4(0x1ec, LONG, SQLite3BlobWrite, sqlite3_blob *, blob, a0, CONST_APTR, z, a1, LONG, n, d0, LONG, iOffset, d1, \
	, SQLITE3_BASE_NAME)

#define SQLite3ExtendedResultCodes(db, onoff) \
	LP2(0x1f2, LONG, SQLite3ExtendedResultCodes, sqlite3 *, db, a0, LONG, onoff, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3BindZeroBlob(pStmt, i, n) \
	LP3(0x1f8, LONG, SQLite3BindZeroBlob, sqlite3_stmt *, pStmt, a0, LONG, i, d0, LONG, n, d1, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnDatabaseName(stmt, n) \
	LP2(0x1fe, CONST_STRPTR, SQLite3ColumnDatabaseName, sqlite3_stmt *, stmt, a0, LONG, n, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnTableName(stmt, n) \
	LP2(0x204, CONST_STRPTR, SQLite3ColumnTableName, sqlite3_stmt *, stmt, a0, LONG, n, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnOriginName(stmt, n) \
	LP2(0x20a, CONST_STRPTR, SQLite3ColumnOriginName, sqlite3_stmt *, stmt, a0, LONG, n, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ColumnValue(pStmt, iCol) \
	LP2(0x210, sqlite3_value *, SQLite3ColumnValue, sqlite3_stmt *, pStmt, a0, LONG, iCol, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3CreateCollationV2(db, zName, eTextRep, pCtx, xCompare, xDestroy) \
	LP6FP(0x216, LONG, SQLite3CreateCollationV2, sqlite3 *, db, a0, CONST_STRPTR, zName, a1, LONG, eTextRep, d0, APTR, pCtx, a2, __fpt, xCompare, a3, void *, xDestroy, d1, \
	, SQLITE3_BASE_NAME, LONG (*__fpt)(APTR p, LONG i, CONST_APTR p2, LONG j, CONST_APTR p3))

#define SQLite3LibVersion() \
	LP0(0x21c, CONST_STRPTR, SQLite3LibVersion, \
	, SQLITE3_BASE_NAME)

#define SQLite3LibversionNumber() \
	LP0(0x222, LONG, SQLite3LibversionNumber, \
	, SQLITE3_BASE_NAME)

#define SQLite3ResultErrorToobig(pCtx) \
	LP1NR(0x228, SQLite3ResultErrorToobig, sqlite3_context *, pCtx, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ResultZeroBlob(pCtx, n) \
	LP2NR(0x22e, SQLite3ResultZeroBlob, sqlite3_context *, pCtx, a0, LONG, n, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ValueNumericType(pVal) \
	LP1(0x234, LONG, SQLite3ValueNumericType, sqlite3_value *, pVal, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3ConfigV(op, ap) \
	LP2(0x23a, LONG, SQLite3ConfigV, LONG, op, d0, APTR, ap, a0, \
	, SQLITE3_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SQLite3Config(op, tags...) \
	({ULONG _tags[] = {tags}; SQLite3ConfigV((op), (APTR) _tags);})
#endif

#define SQLlite3DbConfigV(db, op, ap) \
	LP3(0x240, LONG, SQLlite3DbConfigV, sqlite3 *, db, a0, LONG, op, d0, APTR, ap, a1, \
	, SQLITE3_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SQLlite3DbConfig(db, op, tags...) \
	({ULONG _tags[] = {tags}; SQLlite3DbConfigV((db), (op), (APTR) _tags);})
#endif

#define SQLite3VfsFind(zVfsName) \
	LP1(0x246, sqlite3_vfs *, SQLite3VfsFind, CONST_STRPTR, zVfsName, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3VfsRegister(vfs, makeDflt) \
	LP2(0x24c, LONG, SQLite3VfsRegister, sqlite3_vfs *, vfs, a0, LONG, makeDflt, d0, \
	, SQLITE3_BASE_NAME)

#define SQLite3VfsUnregister(vfs) \
	LP1(0x252, LONG, SQLite3VfsUnregister, sqlite3_vfs *, vfs, a0, \
	, SQLITE3_BASE_NAME)

#define SQLite3FileControl(db, zDbName, op, arg) \
	LP4(0x258, LONG, SQLite3FileControl, sqlite3 *, db, a0, CONST_STRPTR, zDbName, a1, LONG, op, d0, void *, arg, a2, \
	, SQLITE3_BASE_NAME)

#define SQLite3Status(op, pCurrent, pHighwater, resetFlag) \
	LP4(0x25e, LONG, SQLite3Status, LONG, op, d0, LONG *, pCurrent, a0, LONG *, pHighwater, a1, LONG, resetFlag, d1, \
	, SQLITE3_BASE_NAME)

#define SQLite3DbStatus(db, op, pCur, pHiwtr, resetFlg) \
	LP5(0x264, LONG, SQLite3DbStatus, sqlite3 *, db, a0, LONG, op, d0, LONG *, pCur, a1, LONG *, pHiwtr, a2, LONG, resetFlg, d1, \
	, SQLITE3_BASE_NAME)

#endif /*  _INLINE_SQLITE3_H  */

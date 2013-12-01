/* Automatically generated header! Do not edit! */

#ifndef _INLINE_SQLITE3_LIB_SFD_H
#define _INLINE_SQLITE3_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef SQLITE3_LIB_SFD_BASE_NAME
#define SQLITE3_LIB_SFD_BASE_NAME SQLite3Base
#endif /* !SQLITE3_LIB_SFD_BASE_NAME */

#define SQLite3Close(___db) __SQLite3Close_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db)
#define __SQLite3Close_WB(___base, ___db) \
	AROS_LC1(LONG, SQLite3Close, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	struct Library *, (___base), 5, Sqlite3_lib_sfd)

#define SQLite3Exec(___db, ___sql, ___xCallback, ___pArg, ___errmsg) __SQLite3Exec_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___sql, ___xCallback, ___pArg, ___errmsg)
#define __SQLite3Exec_WB(___base, ___db, ___sql, ___xCallback, ___pArg, ___errmsg) \
	AROS_LC5(LONG, SQLite3Exec, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___sql), A1), \
	AROS_LCA(sqlite3_callback, (___xCallback), A2), \
	AROS_LCA(APTR, (___pArg), A3), \
	AROS_LCA(STRPTR *, (___errmsg), D0), \
	struct Library *, (___base), 6, Sqlite3_lib_sfd)

#define SQLite3Changes(___db) __SQLite3Changes_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db)
#define __SQLite3Changes_WB(___base, ___db) \
	AROS_LC1(LONG, SQLite3Changes, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	struct Library *, (___base), 7, Sqlite3_lib_sfd)

#define SQLite3TotalChanges(___db) __SQLite3TotalChanges_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db)
#define __SQLite3TotalChanges_WB(___base, ___db) \
	AROS_LC1(LONG, SQLite3TotalChanges, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	struct Library *, (___base), 8, Sqlite3_lib_sfd)

#define SQLite3Interrupt(___db) __SQLite3Interrupt_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db)
#define __SQLite3Interrupt_WB(___base, ___db) \
	AROS_LC1NR(VOID, SQLite3Interrupt, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	struct Library *, (___base), 9, Sqlite3_lib_sfd)

#define SQLite3Complete(___sql) __SQLite3Complete_WB(SQLITE3_LIB_SFD_BASE_NAME, ___sql)
#define __SQLite3Complete_WB(___base, ___sql) \
	AROS_LC1(LONG, SQLite3Complete, \
	AROS_LCA(CONST_STRPTR, (___sql), A0), \
	struct Library *, (___base), 10, Sqlite3_lib_sfd)

#define SQLite3BusyHandler(___db, ___callback, ___userdata) __SQLite3BusyHandler_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___callback, ___userdata)
#define __SQLite3BusyHandler_WB(___base, ___db, ___callback, ___userdata) \
	AROS_LC3(LONG, SQLite3BusyHandler, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(LONG(*)(APTR userdata,LONG l), (___callback), A1), \
	AROS_LCA(APTR, (___userdata), A2), \
	struct Library *, (___base), 11, Sqlite3_lib_sfd)

#define SQLite3BusyTimeout(___db, ___ms) __SQLite3BusyTimeout_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___ms)
#define __SQLite3BusyTimeout_WB(___base, ___db, ___ms) \
	AROS_LC2(LONG, SQLite3BusyTimeout, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(LONG, (___ms), D0), \
	struct Library *, (___base), 12, Sqlite3_lib_sfd)

#define SQLite3GetTable(___db, ___sql, ___presult, ___nrow, ___ncolumn, ___errmsg) __SQLite3GetTable_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___sql, ___presult, ___nrow, ___ncolumn, ___errmsg)
#define __SQLite3GetTable_WB(___base, ___db, ___sql, ___presult, ___nrow, ___ncolumn, ___errmsg) \
	AROS_LC6(LONG, SQLite3GetTable, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___sql), A1), \
	AROS_LCA(STRPTR **, (___presult), A2), \
	AROS_LCA(LONG *, (___nrow), A3), \
	AROS_LCA(LONG *, (___ncolumn), D0), \
	AROS_LCA(STRPTR *, (___errmsg), D1), \
	struct Library *, (___base), 13, Sqlite3_lib_sfd)

#define SQLite3FreeTable(___result) __SQLite3FreeTable_WB(SQLITE3_LIB_SFD_BASE_NAME, ___result)
#define __SQLite3FreeTable_WB(___base, ___result) \
	AROS_LC1NR(VOID, SQLite3FreeTable, \
	AROS_LCA(STRPTR *, (___result), A0), \
	struct Library *, (___base), 14, Sqlite3_lib_sfd)

#define SQLite3Free(___z) __SQLite3Free_WB(SQLITE3_LIB_SFD_BASE_NAME, ___z)
#define __SQLite3Free_WB(___base, ___z) \
	AROS_LC1NR(VOID, SQLite3Free, \
	AROS_LCA(STRPTR, (___z), A0), \
	struct Library *, (___base), 15, Sqlite3_lib_sfd)

#define SQLite3Trace(___db, ___xTrace, ___parg) __SQLite3Trace_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___xTrace, ___parg)
#define __SQLite3Trace_WB(___base, ___db, ___xTrace, ___parg) \
	AROS_LC3(APTR, SQLite3Trace, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(VOID (*)(APTR p,CONST_STRPTR z), (___xTrace), A1), \
	AROS_LCA(APTR, (___parg), A2), \
	struct Library *, (___base), 16, Sqlite3_lib_sfd)

#define SQLite3ProgressHandler(___db, ___nOps, ___xProgress, ___pArg) __SQLite3ProgressHandler_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___nOps, ___xProgress, ___pArg)
#define __SQLite3ProgressHandler_WB(___base, ___db, ___nOps, ___xProgress, ___pArg) \
	AROS_LC4NR(VOID, SQLite3ProgressHandler, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(LONG, (___nOps), D0), \
	AROS_LCA(LONG (*)(APTR p), (___xProgress), A1), \
	AROS_LCA(APTR, (___pArg), A2), \
	struct Library *, (___base), 17, Sqlite3_lib_sfd)

#define SQLite3CommitHook(___db, ___xCallback, ___pArg) __SQLite3CommitHook_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___xCallback, ___pArg)
#define __SQLite3CommitHook_WB(___base, ___db, ___xCallback, ___pArg) \
	AROS_LC3(APTR, SQLite3CommitHook, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(LONG (*)(APTR p), (___xCallback), A1), \
	AROS_LCA(APTR, (___pArg), A2), \
	struct Library *, (___base), 18, Sqlite3_lib_sfd)

#define SQLite3Open(___filename, ___ppDb) __SQLite3Open_WB(SQLITE3_LIB_SFD_BASE_NAME, ___filename, ___ppDb)
#define __SQLite3Open_WB(___base, ___filename, ___ppDb) \
	AROS_LC2(LONG, SQLite3Open, \
	AROS_LCA(CONST_STRPTR, (___filename), A0), \
	AROS_LCA(sqlite3 **, (___ppDb), A1), \
	struct Library *, (___base), 19, Sqlite3_lib_sfd)

#define SQLite3Errcode(___db) __SQLite3Errcode_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db)
#define __SQLite3Errcode_WB(___base, ___db) \
	AROS_LC1(LONG, SQLite3Errcode, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	struct Library *, (___base), 20, Sqlite3_lib_sfd)

#define SQLite3Errmsg(___db) __SQLite3Errmsg_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db)
#define __SQLite3Errmsg_WB(___base, ___db) \
	AROS_LC1(CONST_STRPTR, SQLite3Errmsg, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	struct Library *, (___base), 21, Sqlite3_lib_sfd)

#define SQLite3Prepare(___db, ___zSql, ___nBytes, ___ppStmt, ___pzTail) __SQLite3Prepare_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zSql, ___nBytes, ___ppStmt, ___pzTail)
#define __SQLite3Prepare_WB(___base, ___db, ___zSql, ___nBytes, ___ppStmt, ___pzTail) \
	AROS_LC5(LONG, SQLite3Prepare, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zSql), A1), \
	AROS_LCA(LONG, (___nBytes), D0), \
	AROS_LCA(sqlite3_stmt **, (___ppStmt), A2), \
	AROS_LCA(CONST_STRPTR *, (___pzTail), A3), \
	struct Library *, (___base), 22, Sqlite3_lib_sfd)

#define SQLite3BindBlob(___pStmt, ___i, ___zData, ___nData, ___xDel) __SQLite3BindBlob_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i, ___zData, ___nData, ___xDel)
#define __SQLite3BindBlob_WB(___base, ___pStmt, ___i, ___zData, ___nData, ___xDel) \
	AROS_LC5(LONG, SQLite3BindBlob, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	AROS_LCA(CONST_APTR, (___zData), A1), \
	AROS_LCA(LONG, (___nData), D1), \
	AROS_LCA(VOID (*)(APTR p), (___xDel), A2), \
	struct Library *, (___base), 23, Sqlite3_lib_sfd)

#define SQLite3BindInt(___pStmt, ___i, ___iValue) __SQLite3BindInt_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i, ___iValue)
#define __SQLite3BindInt_WB(___base, ___pStmt, ___i, ___iValue) \
	AROS_LC3(LONG, SQLite3BindInt, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	AROS_LCA(LONG, (___iValue), D1), \
	struct Library *, (___base), 24, Sqlite3_lib_sfd)

#define SQLite3BindNull(___pStmt, ___i) __SQLite3BindNull_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i)
#define __SQLite3BindNull_WB(___base, ___pStmt, ___i) \
	AROS_LC2(LONG, SQLite3BindNull, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	struct Library *, (___base), 25, Sqlite3_lib_sfd)

#define SQLite3BindText(___pStmt, ___i, ___zData, ___nData, ___xDel) __SQLite3BindText_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i, ___zData, ___nData, ___xDel)
#define __SQLite3BindText_WB(___base, ___pStmt, ___i, ___zData, ___nData, ___xDel) \
	AROS_LC5(LONG, SQLite3BindText, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	AROS_LCA(CONST_STRPTR, (___zData), A1), \
	AROS_LCA(LONG, (___nData), D1), \
	AROS_LCA(VOID (*)(APTR p), (___xDel), A2), \
	struct Library *, (___base), 26, Sqlite3_lib_sfd)

#define SQLite3BindValue(___pStmt, ___i, ___pVal) __SQLite3BindValue_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i, ___pVal)
#define __SQLite3BindValue_WB(___base, ___pStmt, ___i, ___pVal) \
	AROS_LC3(LONG, SQLite3BindValue, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	AROS_LCA(CONST sqlite3_value *, (___pVal), A1), \
	struct Library *, (___base), 27, Sqlite3_lib_sfd)

#define SQLite3BindParameterCount(___pStmt) __SQLite3BindParameterCount_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3BindParameterCount_WB(___base, ___pStmt) \
	AROS_LC1(LONG, SQLite3BindParameterCount, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 28, Sqlite3_lib_sfd)

#define SQLite3BindParameterName(___pStmt, ___i) __SQLite3BindParameterName_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i)
#define __SQLite3BindParameterName_WB(___base, ___pStmt, ___i) \
	AROS_LC2(CONST_STRPTR, SQLite3BindParameterName, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	struct Library *, (___base), 29, Sqlite3_lib_sfd)

#define SQLite3BindParameterIndex(___pStmt, ___zName) __SQLite3BindParameterIndex_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___zName)
#define __SQLite3BindParameterIndex_WB(___base, ___pStmt, ___zName) \
	AROS_LC2(LONG, SQLite3BindParameterIndex, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(CONST_STRPTR, (___zName), A1), \
	struct Library *, (___base), 30, Sqlite3_lib_sfd)

#define SQLite3ClearBindings(___pStmt) __SQLite3ClearBindings_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3ClearBindings_WB(___base, ___pStmt) \
	AROS_LC1(LONG, SQLite3ClearBindings, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 31, Sqlite3_lib_sfd)

#define SQLite3ColumnCount(___pStmt) __SQLite3ColumnCount_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3ColumnCount_WB(___base, ___pStmt) \
	AROS_LC1(LONG, SQLite3ColumnCount, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 32, Sqlite3_lib_sfd)

#define SQLite3ColumnName(___pStmt, ___i) __SQLite3ColumnName_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i)
#define __SQLite3ColumnName_WB(___base, ___pStmt, ___i) \
	AROS_LC2(CONST_STRPTR, SQLite3ColumnName, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	struct Library *, (___base), 33, Sqlite3_lib_sfd)

#define SQLite3ColumnDecltype(___pStmt, ___i) __SQLite3ColumnDecltype_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i)
#define __SQLite3ColumnDecltype_WB(___base, ___pStmt, ___i) \
	AROS_LC2(CONST_STRPTR, SQLite3ColumnDecltype, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	struct Library *, (___base), 34, Sqlite3_lib_sfd)

#define SQLite3Step(___pStmt) __SQLite3Step_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3Step_WB(___base, ___pStmt) \
	AROS_LC1(LONG, SQLite3Step, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 35, Sqlite3_lib_sfd)

#define SQLite3DataCount(___pStmt) __SQLite3DataCount_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3DataCount_WB(___base, ___pStmt) \
	AROS_LC1(LONG, SQLite3DataCount, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 36, Sqlite3_lib_sfd)

#define SQLite3ColumnBlob(___pStmt, ___iCol) __SQLite3ColumnBlob_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___iCol)
#define __SQLite3ColumnBlob_WB(___base, ___pStmt, ___iCol) \
	AROS_LC2(CONST_APTR, SQLite3ColumnBlob, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___iCol), D0), \
	struct Library *, (___base), 37, Sqlite3_lib_sfd)

#define SQLite3ColumnBytes(___pStmt, ___iCol) __SQLite3ColumnBytes_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___iCol)
#define __SQLite3ColumnBytes_WB(___base, ___pStmt, ___iCol) \
	AROS_LC2(LONG, SQLite3ColumnBytes, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___iCol), D0), \
	struct Library *, (___base), 38, Sqlite3_lib_sfd)

#define SQLite3ColumnInt(___pStmt, ___iCol) __SQLite3ColumnInt_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___iCol)
#define __SQLite3ColumnInt_WB(___base, ___pStmt, ___iCol) \
	AROS_LC2(LONG, SQLite3ColumnInt, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___iCol), D0), \
	struct Library *, (___base), 39, Sqlite3_lib_sfd)

#define SQLite3ColumnText(___pStmt, ___iCol) __SQLite3ColumnText_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___iCol)
#define __SQLite3ColumnText_WB(___base, ___pStmt, ___iCol) \
	AROS_LC2(CONST_STRPTR, SQLite3ColumnText, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___iCol), D0), \
	struct Library *, (___base), 40, Sqlite3_lib_sfd)

#define SQLite3ColumnType(___pStmt, ___iCol) __SQLite3ColumnType_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___iCol)
#define __SQLite3ColumnType_WB(___base, ___pStmt, ___iCol) \
	AROS_LC2(LONG, SQLite3ColumnType, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___iCol), D0), \
	struct Library *, (___base), 41, Sqlite3_lib_sfd)

#define SQLite3Finalize(___pStmt) __SQLite3Finalize_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3Finalize_WB(___base, ___pStmt) \
	AROS_LC1(LONG, SQLite3Finalize, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 42, Sqlite3_lib_sfd)

#define SQLite3Reset(___pStmt) __SQLite3Reset_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3Reset_WB(___base, ___pStmt) \
	AROS_LC1(LONG, SQLite3Reset, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 43, Sqlite3_lib_sfd)

#define SQLite3AggregateCount(___pCtx) __SQLite3AggregateCount_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx)
#define __SQLite3AggregateCount_WB(___base, ___pCtx) \
	AROS_LC1(LONG, SQLite3AggregateCount, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	struct Library *, (___base), 44, Sqlite3_lib_sfd)

#define SQLite3ValueBlob(___pVal) __SQLite3ValueBlob_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pVal)
#define __SQLite3ValueBlob_WB(___base, ___pVal) \
	AROS_LC1(CONST_APTR, SQLite3ValueBlob, \
	AROS_LCA(sqlite3_value *, (___pVal), A0), \
	struct Library *, (___base), 45, Sqlite3_lib_sfd)

#define SQLite3ValueBytes(___pVal) __SQLite3ValueBytes_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pVal)
#define __SQLite3ValueBytes_WB(___base, ___pVal) \
	AROS_LC1(LONG, SQLite3ValueBytes, \
	AROS_LCA(sqlite3_value *, (___pVal), A0), \
	struct Library *, (___base), 46, Sqlite3_lib_sfd)

#define SQLite3ValueInt(___pVal) __SQLite3ValueInt_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pVal)
#define __SQLite3ValueInt_WB(___base, ___pVal) \
	AROS_LC1(LONG, SQLite3ValueInt, \
	AROS_LCA(sqlite3_value *, (___pVal), A0), \
	struct Library *, (___base), 47, Sqlite3_lib_sfd)

#define SQLite3ValueText(___pVal) __SQLite3ValueText_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pVal)
#define __SQLite3ValueText_WB(___base, ___pVal) \
	AROS_LC1(CONST_STRPTR, SQLite3ValueText, \
	AROS_LCA(sqlite3_value *, (___pVal), A0), \
	struct Library *, (___base), 48, Sqlite3_lib_sfd)

#define SQLite3ValueType(___pVal) __SQLite3ValueType_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pVal)
#define __SQLite3ValueType_WB(___base, ___pVal) \
	AROS_LC1(LONG, SQLite3ValueType, \
	AROS_LCA(sqlite3_value *, (___pVal), A0), \
	struct Library *, (___base), 49, Sqlite3_lib_sfd)

#define SQLite3Aggregate_context(___pCtx, ___nBytes) __SQLite3Aggregate_context_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___nBytes)
#define __SQLite3Aggregate_context_WB(___base, ___pCtx, ___nBytes) \
	AROS_LC2(APTR, SQLite3Aggregate_context, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(LONG, (___nBytes), D0), \
	struct Library *, (___base), 50, Sqlite3_lib_sfd)

#define SQLite3UserData(___pCtx) __SQLite3UserData_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx)
#define __SQLite3UserData_WB(___base, ___pCtx) \
	AROS_LC1(APTR, SQLite3UserData, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	struct Library *, (___base), 51, Sqlite3_lib_sfd)

#define SQLite3GetAuxdata(___pCtx, ___iArg) __SQLite3GetAuxdata_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___iArg)
#define __SQLite3GetAuxdata_WB(___base, ___pCtx, ___iArg) \
	AROS_LC2(APTR, SQLite3GetAuxdata, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(LONG, (___iArg), D0), \
	struct Library *, (___base), 52, Sqlite3_lib_sfd)

#define SQLite3SetAuxdata(___pCtx, ___iARg, ___pAux, ___xDelete) __SQLite3SetAuxdata_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___iARg, ___pAux, ___xDelete)
#define __SQLite3SetAuxdata_WB(___base, ___pCtx, ___iARg, ___pAux, ___xDelete) \
	AROS_LC4NR(VOID, SQLite3SetAuxdata, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(LONG, (___iARg), D0), \
	AROS_LCA(APTR, (___pAux), A1), \
	AROS_LCA(VOID (*)(APTR p), (___xDelete), A2), \
	struct Library *, (___base), 53, Sqlite3_lib_sfd)

#define SQLite3ResultBlob(___pCtx, ___z, ___n, ___xDelete) __SQLite3ResultBlob_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___z, ___n, ___xDelete)
#define __SQLite3ResultBlob_WB(___base, ___pCtx, ___z, ___n, ___xDelete) \
	AROS_LC4NR(VOID, SQLite3ResultBlob, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(CONST_APTR, (___z), A1), \
	AROS_LCA(LONG, (___n), D0), \
	AROS_LCA(VOID (*)(APTR p), (___xDelete), A2), \
	struct Library *, (___base), 54, Sqlite3_lib_sfd)

#define SQLite3ResultError(___pCtx, ___z, ___n) __SQLite3ResultError_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___z, ___n)
#define __SQLite3ResultError_WB(___base, ___pCtx, ___z, ___n) \
	AROS_LC3NR(VOID, SQLite3ResultError, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(CONST_STRPTR, (___z), A1), \
	AROS_LCA(LONG, (___n), D0), \
	struct Library *, (___base), 55, Sqlite3_lib_sfd)

#define SQLite3ResultInt(___pCtx, ___iVal) __SQLite3ResultInt_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___iVal)
#define __SQLite3ResultInt_WB(___base, ___pCtx, ___iVal) \
	AROS_LC2NR(VOID, SQLite3ResultInt, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(LONG, (___iVal), D0), \
	struct Library *, (___base), 56, Sqlite3_lib_sfd)

#define SQLite3ResultNull(___pCtx) __SQLite3ResultNull_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx)
#define __SQLite3ResultNull_WB(___base, ___pCtx) \
	AROS_LC1NR(VOID, SQLite3ResultNull, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	struct Library *, (___base), 57, Sqlite3_lib_sfd)

#define SQLite3ResultText(___pCtx, ___z, ___n, ___xDelete) __SQLite3ResultText_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___z, ___n, ___xDelete)
#define __SQLite3ResultText_WB(___base, ___pCtx, ___z, ___n, ___xDelete) \
	AROS_LC4NR(VOID, SQLite3ResultText, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(CONST_STRPTR, (___z), A1), \
	AROS_LCA(LONG, (___n), D0), \
	AROS_LCA(VOID (*)(APTR p), (___xDelete), A2), \
	struct Library *, (___base), 58, Sqlite3_lib_sfd)

#define SQLite3ResultValue(___pCtx, ___pValue) __SQLite3ResultValue_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___pValue)
#define __SQLite3ResultValue_WB(___base, ___pCtx, ___pValue) \
	AROS_LC2NR(VOID, SQLite3ResultValue, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(sqlite3_value *, (___pValue), A1), \
	struct Library *, (___base), 59, Sqlite3_lib_sfd)

#define SQLite3CreateCollation(___db, ___zName, ___eTextRep, ___pCtx, ___xCompare) __SQLite3CreateCollation_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zName, ___eTextRep, ___pCtx, ___xCompare)
#define __SQLite3CreateCollation_WB(___base, ___db, ___zName, ___eTextRep, ___pCtx, ___xCompare) \
	AROS_LC5(LONG, SQLite3CreateCollation, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zName), A1), \
	AROS_LCA(LONG, (___eTextRep), D0), \
	AROS_LCA(APTR, (___pCtx), A2), \
	AROS_LCA(LONG(*)(APTR p,LONG i,CONST_APTR p2,LONG j,CONST_APTR p3), (___xCompare), A3), \
	struct Library *, (___base), 60, Sqlite3_lib_sfd)

#define SQLite3CollationNeeded(___db, ___pCollNeededArg, ___xCollNeeded) __SQLite3CollationNeeded_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___pCollNeededArg, ___xCollNeeded)
#define __SQLite3CollationNeeded_WB(___base, ___db, ___pCollNeededArg, ___xCollNeeded) \
	AROS_LC3(LONG, SQLite3CollationNeeded, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(APTR, (___pCollNeededArg), D0), \
	AROS_LCA(VOID (*)(APTR p,sqlite3 *dv,LONG eTextRep,CONST_STRPTR z), (___xCollNeeded), A1), \
	struct Library *, (___base), 61, Sqlite3_lib_sfd)

#define SQLite3Sleep(___ms) __SQLite3Sleep_WB(SQLITE3_LIB_SFD_BASE_NAME, ___ms)
#define __SQLite3Sleep_WB(___base, ___ms) \
	AROS_LC1(LONG, SQLite3Sleep, \
	AROS_LCA(LONG, (___ms), D0), \
	struct Library *, (___base), 62, Sqlite3_lib_sfd)

#define SQLite3Expired(___pStmt) __SQLite3Expired_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3Expired_WB(___base, ___pStmt) \
	AROS_LC1(LONG, SQLite3Expired, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 63, Sqlite3_lib_sfd)

#define SQLite3TransferBindings(___pFromStmt, ___pToStmt) __SQLite3TransferBindings_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pFromStmt, ___pToStmt)
#define __SQLite3TransferBindings_WB(___base, ___pFromStmt, ___pToStmt) \
	AROS_LC2(LONG, SQLite3TransferBindings, \
	AROS_LCA(sqlite3_stmt *, (___pFromStmt), A0), \
	AROS_LCA(sqlite3_stmt *, (___pToStmt), A1), \
	struct Library *, (___base), 64, Sqlite3_lib_sfd)

#define SQLite3GlobalRecover() __SQLite3GlobalRecover_WB(SQLITE3_LIB_SFD_BASE_NAME)
#define __SQLite3GlobalRecover_WB(___base) \
	AROS_LC0(LONG, SQLite3GlobalRecover, \
	struct Library *, (___base), 65, Sqlite3_lib_sfd)

#define SQLite3GetAutocommit(___db) __SQLite3GetAutocommit_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db)
#define __SQLite3GetAutocommit_WB(___base, ___db) \
	AROS_LC1(LONG, SQLite3GetAutocommit, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	struct Library *, (___base), 66, Sqlite3_lib_sfd)

#define SQLite3DbHandle(___pStmt) __SQLite3DbHandle_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt)
#define __SQLite3DbHandle_WB(___base, ___pStmt) \
	AROS_LC1(sqlite3 *, SQLite3DbHandle, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	struct Library *, (___base), 67, Sqlite3_lib_sfd)

#define SQLite3RollbackHook(___db, ___callback, ___pUserData) __SQLite3RollbackHook_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___callback, ___pUserData)
#define __SQLite3RollbackHook_WB(___base, ___db, ___callback, ___pUserData) \
	AROS_LC3(APTR, SQLite3RollbackHook, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(VOID (*)(APTR pUserData), (___callback), A1), \
	AROS_LCA(APTR, (___pUserData), A2), \
	struct Library *, (___base), 68, Sqlite3_lib_sfd)

#define SQLite3EnableSharedCache(___enable) __SQLite3EnableSharedCache_WB(SQLITE3_LIB_SFD_BASE_NAME, ___enable)
#define __SQLite3EnableSharedCache_WB(___base, ___enable) \
	AROS_LC1(LONG, SQLite3EnableSharedCache, \
	AROS_LCA(BOOL, (___enable), D0), \
	struct Library *, (___base), 69, Sqlite3_lib_sfd)

#define SQLite3ReleaseMemory(___bytesCount) __SQLite3ReleaseMemory_WB(SQLITE3_LIB_SFD_BASE_NAME, ___bytesCount)
#define __SQLite3ReleaseMemory_WB(___base, ___bytesCount) \
	AROS_LC1(LONG, SQLite3ReleaseMemory, \
	AROS_LCA(LONG, (___bytesCount), D0), \
	struct Library *, (___base), 70, Sqlite3_lib_sfd)

#define SQLite3SoftHeapLimit(___maxBytes) __SQLite3SoftHeapLimit_WB(SQLITE3_LIB_SFD_BASE_NAME, ___maxBytes)
#define __SQLite3SoftHeapLimit_WB(___base, ___maxBytes) \
	AROS_LC1NR(VOID, SQLite3SoftHeapLimit, \
	AROS_LCA(LONG, (___maxBytes), D0), \
	struct Library *, (___base), 71, Sqlite3_lib_sfd)

#define SQLite3ThreadCleanup() __SQLite3ThreadCleanup_WB(SQLITE3_LIB_SFD_BASE_NAME)
#define __SQLite3ThreadCleanup_WB(___base) \
	AROS_LC0NR(VOID, SQLite3ThreadCleanup, \
	struct Library *, (___base), 72, Sqlite3_lib_sfd)

#define SQLite3PrepareV2(___db, ___zSql, ___nBytes, ___ppStmt, ___pzTail) __SQLite3PrepareV2_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zSql, ___nBytes, ___ppStmt, ___pzTail)
#define __SQLite3PrepareV2_WB(___base, ___db, ___zSql, ___nBytes, ___ppStmt, ___pzTail) \
	AROS_LC5(LONG, SQLite3PrepareV2, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zSql), A1), \
	AROS_LCA(LONG, (___nBytes), D0), \
	AROS_LCA(sqlite3_stmt **, (___ppStmt), A2), \
	AROS_LCA(CONST_STRPTR *, (___pzTail), A3), \
	struct Library *, (___base), 73, Sqlite3_lib_sfd)

#define SQLite3CreateFunction(___db, ___zFunctionName, ___nArg, ___eTextRep, ___userdata, ___xFunc, ___xStep, ___xFinal) __SQLite3CreateFunction_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zFunctionName, ___nArg, ___eTextRep, ___userdata, ___xFunc, ___xStep, ___xFinal)
#define __SQLite3CreateFunction_WB(___base, ___db, ___zFunctionName, ___nArg, ___eTextRep, ___userdata, ___xFunc, ___xStep, ___xFinal) \
	AROS_LC8(LONG, SQLite3CreateFunction, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zFunctionName), A1), \
	AROS_LCA(LONG, (___nArg), D0), \
	AROS_LCA(LONG, (___eTextRep), D1), \
	AROS_LCA(APTR, (___userdata), A2), \
	AROS_LCA(VOID (*)(sqlite3_context *pCtx,LONG i,sqlite3_value **pVal), (___xFunc), A3), \
	AROS_LCA(VOID (*)(sqlite3_context *pCtx,LONG i,sqlite3_value **pVal), (___xStep), D2), \
	AROS_LCA(VOID (*)(sqlite3_context *pCtx), (___xFinal), D3), \
	struct Library *, (___base), 74, Sqlite3_lib_sfd)

#define SQLite3CreateModule(___db, ___zName, ___methods, ___clientData) __SQLite3CreateModule_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zName, ___methods, ___clientData)
#define __SQLite3CreateModule_WB(___base, ___db, ___zName, ___methods, ___clientData) \
	AROS_LC4(LONG, SQLite3CreateModule, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zName), A1), \
	AROS_LCA(CONST sqlite3_module *, (___methods), A2), \
	AROS_LCA(APTR, (___clientData), A3), \
	struct Library *, (___base), 75, Sqlite3_lib_sfd)

#define SQLite3DeclareVtab(___db, ___zCreateTable) __SQLite3DeclareVtab_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zCreateTable)
#define __SQLite3DeclareVtab_WB(___base, ___db, ___zCreateTable) \
	AROS_LC2(LONG, SQLite3DeclareVtab, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zCreateTable), A1), \
	struct Library *, (___base), 76, Sqlite3_lib_sfd)

#define SQLite3OverloadFunction(___db, ___zFuncName, ___nArg) __SQLite3OverloadFunction_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zFuncName, ___nArg)
#define __SQLite3OverloadFunction_WB(___base, ___db, ___zFuncName, ___nArg) \
	AROS_LC3(LONG, SQLite3OverloadFunction, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zFuncName), A1), \
	AROS_LCA(LONG, (___nArg), D0), \
	struct Library *, (___base), 77, Sqlite3_lib_sfd)

#define SQLite3BlobOpen(___db, ___zDb, ___zTable, ___zColumn, ___iRow, ___flags, ___ppBlob) __SQLite3BlobOpen_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zDb, ___zTable, ___zColumn, ___iRow, ___flags, ___ppBlob)
#define __SQLite3BlobOpen_WB(___base, ___db, ___zDb, ___zTable, ___zColumn, ___iRow, ___flags, ___ppBlob) \
	AROS_LC7(LONG, SQLite3BlobOpen, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zDb), A1), \
	AROS_LCA(CONST_STRPTR, (___zTable), A2), \
	AROS_LCA(CONST_STRPTR, (___zColumn), A3), \
	AROS_LCA(LONG, (___iRow), D0), \
	AROS_LCA(LONG, (___flags), D1), \
	AROS_LCA(sqlite3_blob **, (___ppBlob), A4), \
	struct Library *, (___base), 78, Sqlite3_lib_sfd)

#define SQLite3BlobClose(___blob) __SQLite3BlobClose_WB(SQLITE3_LIB_SFD_BASE_NAME, ___blob)
#define __SQLite3BlobClose_WB(___base, ___blob) \
	AROS_LC1(LONG, SQLite3BlobClose, \
	AROS_LCA(sqlite3_blob *, (___blob), A0), \
	struct Library *, (___base), 79, Sqlite3_lib_sfd)

#define SQLite3BlobBytes(___blob) __SQLite3BlobBytes_WB(SQLITE3_LIB_SFD_BASE_NAME, ___blob)
#define __SQLite3BlobBytes_WB(___base, ___blob) \
	AROS_LC1(LONG, SQLite3BlobBytes, \
	AROS_LCA(sqlite3_blob *, (___blob), A0), \
	struct Library *, (___base), 80, Sqlite3_lib_sfd)

#define SQLite3BlobRead(___blob, ___z, ___n, ___iOffset) __SQLite3BlobRead_WB(SQLITE3_LIB_SFD_BASE_NAME, ___blob, ___z, ___n, ___iOffset)
#define __SQLite3BlobRead_WB(___base, ___blob, ___z, ___n, ___iOffset) \
	AROS_LC4(LONG, SQLite3BlobRead, \
	AROS_LCA(sqlite3_blob *, (___blob), A0), \
	AROS_LCA(APTR, (___z), A1), \
	AROS_LCA(LONG, (___n), D0), \
	AROS_LCA(LONG, (___iOffset), D1), \
	struct Library *, (___base), 81, Sqlite3_lib_sfd)

#define SQLite3BlobWrite(___blob, ___z, ___n, ___iOffset) __SQLite3BlobWrite_WB(SQLITE3_LIB_SFD_BASE_NAME, ___blob, ___z, ___n, ___iOffset)
#define __SQLite3BlobWrite_WB(___base, ___blob, ___z, ___n, ___iOffset) \
	AROS_LC4(LONG, SQLite3BlobWrite, \
	AROS_LCA(sqlite3_blob *, (___blob), A0), \
	AROS_LCA(CONST_APTR, (___z), A1), \
	AROS_LCA(LONG, (___n), D0), \
	AROS_LCA(LONG, (___iOffset), D1), \
	struct Library *, (___base), 82, Sqlite3_lib_sfd)

#define SQLite3ExtendedResultCodes(___db, ___onoff) __SQLite3ExtendedResultCodes_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___onoff)
#define __SQLite3ExtendedResultCodes_WB(___base, ___db, ___onoff) \
	AROS_LC2(LONG, SQLite3ExtendedResultCodes, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(LONG, (___onoff), D0), \
	struct Library *, (___base), 83, Sqlite3_lib_sfd)

#define SQLite3BindZeroBlob(___pStmt, ___i, ___n) __SQLite3BindZeroBlob_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___i, ___n)
#define __SQLite3BindZeroBlob_WB(___base, ___pStmt, ___i, ___n) \
	AROS_LC3(LONG, SQLite3BindZeroBlob, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___i), D0), \
	AROS_LCA(LONG, (___n), D1), \
	struct Library *, (___base), 84, Sqlite3_lib_sfd)

#define SQLite3ColumnDatabaseName(___stmt, ___n) __SQLite3ColumnDatabaseName_WB(SQLITE3_LIB_SFD_BASE_NAME, ___stmt, ___n)
#define __SQLite3ColumnDatabaseName_WB(___base, ___stmt, ___n) \
	AROS_LC2(CONST_STRPTR, SQLite3ColumnDatabaseName, \
	AROS_LCA(sqlite3_stmt *, (___stmt), A0), \
	AROS_LCA(LONG, (___n), D0), \
	struct Library *, (___base), 85, Sqlite3_lib_sfd)

#define SQLite3ColumnTableName(___stmt, ___n) __SQLite3ColumnTableName_WB(SQLITE3_LIB_SFD_BASE_NAME, ___stmt, ___n)
#define __SQLite3ColumnTableName_WB(___base, ___stmt, ___n) \
	AROS_LC2(CONST_STRPTR, SQLite3ColumnTableName, \
	AROS_LCA(sqlite3_stmt *, (___stmt), A0), \
	AROS_LCA(LONG, (___n), D0), \
	struct Library *, (___base), 86, Sqlite3_lib_sfd)

#define SQLite3ColumnOriginName(___stmt, ___n) __SQLite3ColumnOriginName_WB(SQLITE3_LIB_SFD_BASE_NAME, ___stmt, ___n)
#define __SQLite3ColumnOriginName_WB(___base, ___stmt, ___n) \
	AROS_LC2(CONST_STRPTR, SQLite3ColumnOriginName, \
	AROS_LCA(sqlite3_stmt *, (___stmt), A0), \
	AROS_LCA(LONG, (___n), D0), \
	struct Library *, (___base), 87, Sqlite3_lib_sfd)

#define SQLite3ColumnValue(___pStmt, ___iCol) __SQLite3ColumnValue_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pStmt, ___iCol)
#define __SQLite3ColumnValue_WB(___base, ___pStmt, ___iCol) \
	AROS_LC2(sqlite3_value *, SQLite3ColumnValue, \
	AROS_LCA(sqlite3_stmt *, (___pStmt), A0), \
	AROS_LCA(LONG, (___iCol), D0), \
	struct Library *, (___base), 88, Sqlite3_lib_sfd)

#define SQLite3CreateCollationV2(___db, ___zName, ___eTextRep, ___pCtx, ___xCompare, ___xDestroy) __SQLite3CreateCollationV2_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zName, ___eTextRep, ___pCtx, ___xCompare, ___xDestroy)
#define __SQLite3CreateCollationV2_WB(___base, ___db, ___zName, ___eTextRep, ___pCtx, ___xCompare, ___xDestroy) \
	AROS_LC6(LONG, SQLite3CreateCollationV2, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zName), A1), \
	AROS_LCA(LONG, (___eTextRep), D0), \
	AROS_LCA(APTR, (___pCtx), A2), \
	AROS_LCA(LONG(*)(APTR p,LONG i,CONST_APTR p2,LONG j,CONST_APTR p3), (___xCompare), A3), \
	AROS_LCA(VOID (*)(APTR), (___xDestroy), D1), \
	struct Library *, (___base), 89, Sqlite3_lib_sfd)

#define SQLite3LibVersion() __SQLite3LibVersion_WB(SQLITE3_LIB_SFD_BASE_NAME)
#define __SQLite3LibVersion_WB(___base) \
	AROS_LC0(CONST_STRPTR, SQLite3LibVersion, \
	struct Library *, (___base), 90, Sqlite3_lib_sfd)

#define SQLite3LibversionNumber() __SQLite3LibversionNumber_WB(SQLITE3_LIB_SFD_BASE_NAME)
#define __SQLite3LibversionNumber_WB(___base) \
	AROS_LC0(LONG, SQLite3LibversionNumber, \
	struct Library *, (___base), 91, Sqlite3_lib_sfd)

#define SQLite3ResultErrorToobig(___pCtx) __SQLite3ResultErrorToobig_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx)
#define __SQLite3ResultErrorToobig_WB(___base, ___pCtx) \
	AROS_LC1NR(VOID, SQLite3ResultErrorToobig, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	struct Library *, (___base), 92, Sqlite3_lib_sfd)

#define SQLite3ResultZeroBlob(___pCtx, ___n) __SQLite3ResultZeroBlob_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pCtx, ___n)
#define __SQLite3ResultZeroBlob_WB(___base, ___pCtx, ___n) \
	AROS_LC2NR(VOID, SQLite3ResultZeroBlob, \
	AROS_LCA(sqlite3_context *, (___pCtx), A0), \
	AROS_LCA(LONG, (___n), D0), \
	struct Library *, (___base), 93, Sqlite3_lib_sfd)

#define SQLite3ValueNumericType(___pVal) __SQLite3ValueNumericType_WB(SQLITE3_LIB_SFD_BASE_NAME, ___pVal)
#define __SQLite3ValueNumericType_WB(___base, ___pVal) \
	AROS_LC1(LONG, SQLite3ValueNumericType, \
	AROS_LCA(sqlite3_value *, (___pVal), A0), \
	struct Library *, (___base), 94, Sqlite3_lib_sfd)

#define SQLite3ConfigV(___op, ___ap) __SQLite3ConfigV_WB(SQLITE3_LIB_SFD_BASE_NAME, ___op, ___ap)
#define __SQLite3ConfigV_WB(___base, ___op, ___ap) \
	AROS_LC2(LONG, SQLite3ConfigV, \
	AROS_LCA(LONG, (___op), D0), \
	AROS_LCA(APTR, (___ap), A0), \
	struct Library *, (___base), 95, Sqlite3_lib_sfd)

#ifndef NO_INLINE_VARARGS
#define SQLite3Config(___op, ___dummy, ...) __SQLite3Config_WB(SQLITE3_LIB_SFD_BASE_NAME, ___op, ___dummy, ## __VA_ARGS__)
#define __SQLite3Config_WB(___base, ___op, ___dummy, ...) \
	({IPTR _message[] = { (IPTR) ___dummy, ## __VA_ARGS__ }; __SQLite3ConfigV_WB((___base), (___op), (APTR) _message); })
#endif /* !NO_INLINE_VARARGS */

#define SQLlite3DbConfigV(___db, ___op, ___ap) __SQLlite3DbConfigV_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___op, ___ap)
#define __SQLlite3DbConfigV_WB(___base, ___db, ___op, ___ap) \
	AROS_LC3(LONG, SQLlite3DbConfigV, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(LONG, (___op), D0), \
	AROS_LCA(APTR, (___ap), A1), \
	struct Library *, (___base), 96, Sqlite3_lib_sfd)

#ifndef NO_INLINE_VARARGS
#define SQLlite3DbConfig(___db, ___op, ___dummy, ...) __SQLlite3DbConfig_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___op, ___dummy, ## __VA_ARGS__)
#define __SQLlite3DbConfig_WB(___base, ___db, ___op, ___dummy, ...) \
	({IPTR _message[] = { (IPTR) ___dummy, ## __VA_ARGS__ }; __SQLlite3DbConfigV_WB((___base), (___db), (___op), (APTR) _message); })
#endif /* !NO_INLINE_VARARGS */

#define SQLite3VfsFind(___zVfsName) __SQLite3VfsFind_WB(SQLITE3_LIB_SFD_BASE_NAME, ___zVfsName)
#define __SQLite3VfsFind_WB(___base, ___zVfsName) \
	AROS_LC1(sqlite3_vfs *, SQLite3VfsFind, \
	AROS_LCA(CONST_STRPTR, (___zVfsName), A0), \
	struct Library *, (___base), 97, Sqlite3_lib_sfd)

#define SQLite3VfsRegister(___vfs, ___makeDflt) __SQLite3VfsRegister_WB(SQLITE3_LIB_SFD_BASE_NAME, ___vfs, ___makeDflt)
#define __SQLite3VfsRegister_WB(___base, ___vfs, ___makeDflt) \
	AROS_LC2(LONG, SQLite3VfsRegister, \
	AROS_LCA(sqlite3_vfs *, (___vfs), A0), \
	AROS_LCA(LONG, (___makeDflt), D0), \
	struct Library *, (___base), 98, Sqlite3_lib_sfd)

#define SQLite3VfsUnregister(___vfs) __SQLite3VfsUnregister_WB(SQLITE3_LIB_SFD_BASE_NAME, ___vfs)
#define __SQLite3VfsUnregister_WB(___base, ___vfs) \
	AROS_LC1(LONG, SQLite3VfsUnregister, \
	AROS_LCA(sqlite3_vfs *, (___vfs), A0), \
	struct Library *, (___base), 99, Sqlite3_lib_sfd)

#define SQLite3FileControl(___db, ___zDbName, ___op, ___arg) __SQLite3FileControl_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___zDbName, ___op, ___arg)
#define __SQLite3FileControl_WB(___base, ___db, ___zDbName, ___op, ___arg) \
	AROS_LC4(LONG, SQLite3FileControl, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(CONST_STRPTR, (___zDbName), A1), \
	AROS_LCA(LONG, (___op), D0), \
	AROS_LCA(void *, (___arg), A2), \
	struct Library *, (___base), 100, Sqlite3_lib_sfd)

#define SQLite3Status(___op, ___pCurrent, ___pHighwater, ___resetFlag) __SQLite3Status_WB(SQLITE3_LIB_SFD_BASE_NAME, ___op, ___pCurrent, ___pHighwater, ___resetFlag)
#define __SQLite3Status_WB(___base, ___op, ___pCurrent, ___pHighwater, ___resetFlag) \
	AROS_LC4(LONG, SQLite3Status, \
	AROS_LCA(LONG, (___op), D0), \
	AROS_LCA(LONG *, (___pCurrent), A0), \
	AROS_LCA(LONG *, (___pHighwater), A1), \
	AROS_LCA(LONG, (___resetFlag), D1), \
	struct Library *, (___base), 101, Sqlite3_lib_sfd)

#define SQLite3DbStatus(___db, ___op, ___pCur, ___pHiwtr, ___resetFlg) __SQLite3DbStatus_WB(SQLITE3_LIB_SFD_BASE_NAME, ___db, ___op, ___pCur, ___pHiwtr, ___resetFlg)
#define __SQLite3DbStatus_WB(___base, ___db, ___op, ___pCur, ___pHiwtr, ___resetFlg) \
	AROS_LC5(LONG, SQLite3DbStatus, \
	AROS_LCA(sqlite3 *, (___db), A0), \
	AROS_LCA(LONG, (___op), D0), \
	AROS_LCA(LONG *, (___pCur), A1), \
	AROS_LCA(LONG *, (___pHiwtr), A2), \
	AROS_LCA(LONG, (___resetFlg), D1), \
	struct Library *, (___base), 102, Sqlite3_lib_sfd)

#endif /* !_INLINE_SQLITE3_LIB_SFD_H */

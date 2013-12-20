// LibSQLite3.c
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
#include <scalos/scalos.h>
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

#include "sqlite3_base.h"
#include "LibSQLite3.h"
#include <defs.h>
#include <Year.h>

//----------------------------------------------------------------------------

#ifdef __amigaos4__
#define ExtLib(base) ((struct ExtendedLibrary *)((ULONG)base + ((struct Library *)base)->lib_PosSize))
#define ISQLite3 ((struct SQLite3IFace *)ExtLib(SQLite3Base)->MainIFace)
#endif

//----------------------------------------------------------------------------

// Standard library functions

//----------------------------------------------------------------------------

struct ExecBase *SysBase;
T_UTILITYBASE UtilityBase;
struct DosLibrary * DOSBase;
T_TIMERBASE TimerBase;
#ifdef __amigaos4__
struct Library *NewlibBase;
struct Interface *INewlib;
struct ExecIFace *IExec;
struct UtilityIFace *IUtility;
struct DOSIFace *IDOS;
struct TimerIFace *ITimer;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__AROS__)
extern T_UTILITYBASE __UtilityBase;
extern struct Library *__MathIeeeDoubBasBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) */

//----------------------------------------------------------------------------

char ALIGNED libName[] = "sqlite3.library";
char ALIGNED libIdString[] = "$VER: sqlite3.library "
        STR(LIB_VERSION) "." STR(LIB_REVISION)
	" (" __DATE__ ") "
	COMPILER_STRING 
	" ©2006" CURRENTYEAR " The Scalos Team";

//----------------------------------------------------------------------------

BOOL SQLite3Init(struct SQLite3Base *SQLite3Base)
{
	d1(kprintf("%s/%ld:   START SQLite3Base=%08lx  procName=<%s>\n",__FUNC__ , __LINE__, \
		SQLite3Base, FindTask(NULL)->tc_Node.ln_Name));

#if !defined(__amigaos4__) && !defined(__AROS__)
	if (_STI_240_InitMemFunctions())
		return FALSE;
#endif

	d1(kprintf("%s/%ld: END Success\n",__FUNC__ , __LINE__));

	return TRUE;	// Success
}

//-----------------------------------------------------------------------------

BOOL SQLite3Open(struct SQLite3Base *SQLite3Base)
{
	BOOL Success = FALSE;

	d1(kprintf("%s/%ld:   START SQLite3Base=%08lx  procName=<%s>\n", __FUNC__, __LINE__, \
		SQLite3Base, FindTask(NULL)->tc_Node.ln_Name));

	do	{
		SQLite3Base->sql3_TimerIO = (T_TIMEREQUEST *)CreateIORequest(CreateMsgPort(), sizeof(T_TIMEREQUEST));
		if (NULL == SQLite3Base->sql3_TimerIO)
			break;

		if (0 != OpenDevice("timer.device", UNIT_VBLANK, &SQLite3Base->sql3_TimerIO->tr_node, 0))
			break;
		TimerBase = (T_TIMERBASE) SQLite3Base->sql3_TimerIO->tr_node.io_Device;
		if (NULL == TimerBase)
			break;

#ifdef __amigaos4__
		ITimer = (struct TimerIFace *)GetInterface(TimerBase, "main", 1, NULL);
		if (NULL == ITimer)
			break;
#endif /* __amigaos4__ */

		UtilityBase = (APTR) OpenLibrary( "utility.library", 39 );
		if (NULL == UtilityBase)
			break;
#ifdef __amigaos4__
		IUtility = (APTR) GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
		if (NULL == IUtility)
			break;
#endif /* __amigaos4__ */

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
		__UtilityBase = UtilityBase;

		__MathIeeeDoubBasBase = OpenLibrary("mathieeedoubbas.library", 0);
		if (NULL == __MathIeeeDoubBasBase)
			break;

#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__)*/

		DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
		if (NULL == DOSBase)
			break;
#ifdef __amigaos4__
		IDOS = (APTR) GetInterface((struct Library *)DOSBase, "main", 1, NULL);
		if (NULL == IDOS)
			break;
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			break;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			break;
//		  kprintf("%s/%s/%ld: NewlibBase=%lx, INewlib=%lx\n", __FILE__, __FUNC__, __LINE__,
//			  NewlibBase, INewlib
//		  );
#endif /* __amigaos4__ */

		if (SQLITE_OK != sqlite3_config(SQLITE_CONFIG_MUTEX, sqlite3OtherMutex()))
			break;

		Success = TRUE;
		} while (0);
	
	d1(kprintf("%s/%ld: END Success=%ld\n", __FUNC__,  __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

BOOL SQLite3Cleanup(struct SQLite3Base *SQLite3Base)
{
	d1(kprintf("%s/%ld:  START\n", __FUNC__, __LINE__));

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	if (__MathIeeeDoubBasBase)
		{
		CloseLibrary(__MathIeeeDoubBasBase);
		__MathIeeeDoubBasBase = NULL;
		}
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) */

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
	if (ITimer)
		{
		DropInterface((struct Interface *)ITimer);
		ITimer = NULL;
		}
#endif /* __amigaos4__ */
	if (SQLite3Base->sql3_TimerIO)
		{
		struct MsgPort *TimerPort = SQLite3Base->sql3_TimerIO->tr_node.io_Message.mn_ReplyPort;

		CloseDevice(&SQLite3Base->sql3_TimerIO->tr_node);
		DeleteIORequest(&SQLite3Base->sql3_TimerIO->tr_node);
		if (TimerPort)
			DeleteMsgPort(TimerPort);

		SQLite3Base->sql3_TimerIO = NULL;
		TimerBase = NULL;
		}
#ifdef __amigaos4__
	if (IUtility)
		{
		DropInterface((struct Interface *)IUtility);
		IUtility = NULL;
		}
#endif /* __amigaos4__ */
	if (UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
#ifdef __amigaos4__
	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif /* __amigaos4__ */
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
	return TRUE;
}

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Close,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(kprintf("%s/%ld:  START\n", __FUNC__, __LINE__));
	Result = sqlite3_close(db);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P6(LONG, LIBSQLite3Exec,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, sql,
	A2, sqlite3_callback, xCallback,
	A3, APTR, pArg,
	D0, STRPTR *, errmsg,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START  sql=<%s>\n", __FUNC__, __LINE__));
	Result = sqlite3_exec(db, sql, xCallback, pArg, (char **) errmsg);
	d1(KPrintF("%s/%ld:   END  Result=%ld  errmsg=<%s>\n", __FUNC__, __LINE__, Result, *errmsg));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Changes,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_changes(db);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3TotalChanges,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_total_changes(db);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(VOID, LIBSQLite3Interrupt,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_interrupt(db);
	d1(KPrintF("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Complete,
	A0, CONST_STRPTR, sql,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_complete(sql);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(LONG, LIBSQLite3BusyHandler,
	A0, sqlite3 *, db,
	A1, xBusyFunc, callback,
	A2, APTR, userdata,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_busy_handler(db, callback, userdata);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3BusyTimeout,
	A0, sqlite3 *, db,
	D0, LONG, ms,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_busy_timeout(db, ms);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P7(LONG, LIBSQLite3GetTable,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, sql,
	A2, STRPTR **, presult,
	A3, LONG *, nrow,
	D0, LONG *, ncolumn,
	D1, STRPTR *, errmsg,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;
	int NRow, NColumn;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_get_table(db, sql, (char ***) presult,
		&NRow, &NColumn, (char **) errmsg);
	*nrow = NRow;
	*ncolumn = NColumn;
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(VOID, LIBSQLite3FreeTable,
	A0, STRPTR *, result,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_free_table((char **) result);
	d1(KPrintF("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(VOID, LIBSQLite3Free,
	A0, STRPTR, z,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_free((char *) z);
	d1(KPrintF("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(APTR, LIBSQLite3Trace,
	A0, sqlite3 *, db,
	A1, xTraceFunc, xTrace,
	A2, APTR, parg,
	A6, struct SQLite3Base *, SQLite3Base)
{
	APTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_trace(db, xTrace, parg);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(VOID, LIBSQLite3ProgressHandler,
	A0, sqlite3 *, db,
	D0, LONG, nOps,
	A1, xProgressFunc, xProgress,
	A2, APTR, pArg,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_progress_handler(db, nOps, xProgress, pArg);
	d1(KPrintF("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(APTR, LIBSQLite3CommitHook,
	A0, sqlite3 *, db,
	A1, xCommitFunc, xCallback,
	A2, APTR, pArg,
	A6, struct SQLite3Base *, SQLite3Base)
{
	APTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_commit_hook(db, xCallback, pArg);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3Open,
	A0, CONST_STRPTR, filename,
	A1, sqlite3 **, ppDb,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_open(filename, ppDb);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Errcode,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_errcode(db);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(CONST_STRPTR, LIBSQLite3Errmsg,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_errmsg(db);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P6(LONG, LIBSQLite3Prepare,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zSql,
	D0, LONG, nBytes,
	A2, sqlite3_stmt **, ppStmt,
	A3, CONST_STRPTR *, pzTail,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START  zSql=<%s>\n", __FUNC__, __LINE__));
	Result = sqlite3_prepare(db, (const char *) zSql,
		nBytes, ppStmt, (const char **) pzTail);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P6(LONG, LIBSQLite3BindBlob,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A1, CONST_APTR, zData,
	D1, LONG, nData,
	A2, xDeleteFunc, xDel,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_bind_blob(pStmt, i, zData, nData, xDel);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(LONG, LIBSQLite3BindInt,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	D1, LONG, iValue,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_bind_int(pStmt, i, iValue);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3BindNull,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_bind_null(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P6(LONG, LIBSQLite3BindText,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A1, CONST_STRPTR, zData,
	D1, LONG, nData,
	A2, xDeleteFunc, xDel,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_bind_text(pStmt, i, (const char *) zData,
		nData, xDel);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(LONG, LIBSQLite3BindValue,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A1, CONST sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
//	Result = sqlite3_bind_value(pStmt, i, pVal);
	(void) pStmt;
	(void) i;
	(void) pVal;
	Result = SQLITE_ERROR;
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3BindParameterCount,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_bind_parameter_count(pStmt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(CONST_STRPTR, LIBSQLite3BindParameterName,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_bind_parameter_name(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3BindParameterIndex,
	A0, sqlite3_stmt *, pStmt,
	A1, CONST_STRPTR, zName,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_bind_parameter_index(pStmt, (const char *) zName);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3ClearBindings,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_clear_bindings(pStmt);
	(void) pStmt;
	Result = SQLITE_ERROR;
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3ColumnCount,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_count(pStmt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(CONST_STRPTR, LIBSQLite3ColumnName,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_name(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(CONST_STRPTR, LIBSQLite3ColumnDecltype,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_decltype(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Step,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_step(pStmt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3DataCount,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_data_count(pStmt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(CONST_APTR, LIBSQLite3ColumnBlob,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_APTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_blob(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3ColumnBytes,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_bytes(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3ColumnInt,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_int(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(CONST_STRPTR, LIBSQLite3ColumnText,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = (CONST_STRPTR)sqlite3_column_text(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3ColumnType,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_type(pStmt, i);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Finalize,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_finalize(pStmt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Reset,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_reset(pStmt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG , LIBSQLite3AggregateCount,
	A0, sqlite3_context *, pCtx,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_aggregate_count(pCtx);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(CONST_APTR, LIBSQLite3ValueBlob,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_APTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_value_blob(pVal);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3ValueBytes,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_value_bytes(pVal);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3ValueInt,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_value_int(pVal);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(CONST_STRPTR, LIBSQLite3ValueText,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = (CONST_STRPTR)sqlite3_value_text(pVal);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3ValueType,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_value_type(pVal);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(APTR, LIBSQLite3Aggregate_context,
	A0, sqlite3_context *, pCtx,
	D0, LONG, nBytes,
	A6, struct SQLite3Base *, SQLite3Base)
{
	APTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_aggregate_context(pCtx, nBytes);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(APTR, LIBSQLite3UserData,
	A0, sqlite3_context *, pCtx,
	A6, struct SQLite3Base *, SQLite3Base)
{
	APTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_user_data(pCtx);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(APTR, LIBSQLite3GetAuxdata,
	A0, sqlite3_context *, pCtx,
	D0, LONG, iArg,
	A6, struct SQLite3Base *, SQLite3Base)
{
	APTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_get_auxdata(pCtx, iArg);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(VOID, LIBSQLite3SetAuxdata,
	A0, sqlite3_context *, pCtx,
	D0, LONG, iArg,
	A1, APTR, pAux,
	A2, xDeleteFunc, xDelete,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_set_auxdata(pCtx, iArg, pAux, xDelete);
	d1(kprintf("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(VOID, LIBSQLite3ResultBlob,
	A0, sqlite3_context *, pCtx,
	A1, CONST_APTR, z,
	D1, LONG, n,
	A2, xDeleteFunc, xDelete,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_result_blob(pCtx, z, n, xDelete);
	d1(kprintf("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(VOID, LIBSQLite3ResultError,
	A0, sqlite3_context *, pCtx,
	A1, CONST_STRPTR, z,
	D0, LONG, n,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_result_error(pCtx, z, n);
	d1(kprintf("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(VOID, LIBSQLite3ResultInt,
	A0, sqlite3_context *, pCtx,
	D0, LONG, iVal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_result_int(pCtx, iVal);
	d1(kprintf("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(VOID, LIBSQLite3ResultNull,
	A0, sqlite3_context *, pCtx,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_result_null(pCtx);
	d1(kprintf("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(VOID, LIBSQLite3ResultText,
	A0, sqlite3_context *, pCtx,
	A1, CONST_STRPTR, z,
	D1, LONG, n,
	A2, xDeleteFunc, xDelete,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_result_text(pCtx, z, n, xDelete);
	d1(kprintf("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(VOID, LIBSQLite3ResultValue,
	A0, sqlite3_context *, pCtx,
	A1, sqlite3_value *, pValue,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_result_value(pCtx, pValue);
	d1(kprintf("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P6(LONG, LIBSQLite3CreateCollation,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zName,
	D0, LONG, eTextRep,
	A2, APTR, pCtx,
	A3, xCompareFunc, xCompare,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_create_collation(db, (const char *) zName,
		eTextRep, pCtx, xCompare);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(LONG, LIBSQLite3CollationNeeded,
	A0, sqlite3 *, db,
	D0, APTR, pCollNeededArg,
	A1, xCollationNeededFunc, xCollNeeded,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_collation_needed(db, pCollNeededArg, xCollNeeded);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Sleep,
	D0, LONG, ms,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_sleep(ms);
	(void) ms;
	Result = SQLITE_ERROR;
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3Expired,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_expired(pStmt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3TransferBindings,
	A0, sqlite3_stmt *, pFromStmt,
	A1, sqlite3_stmt *, pToStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_transfer_bindings(pFromStmt, pToStmt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P1(LONG, LIBSQLite3GlobalRecover,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_global_recover();
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3GetAutocommit,
	A0, sqlite3 *, db,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_get_autocommit(db);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(sqlite3 *, LIBSQLite3DbHandle,
	A0, sqlite3_stmt *, pStmt,
	A6, struct SQLite3Base *, SQLite3Base)
{
	sqlite3	*Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_db_handle(pStmt);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(APTR, LIBSQLite3RollbackHook,
	A0, sqlite3 *, db,
	A1, xRollbackCallbackFunc, callback,
	A2, APTR, pUserData,
	A6, struct SQLite3Base *, SQLite3Base)
{
	APTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_rollback_hook(db, callback, pUserData);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3EnableSharedCache,
	D0, BOOL, enable,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_enable_shared_cache(enable);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3ReleaseMemory,
	D0, LONG, bytesCount,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_release_memory(bytesCount);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(VOID, LIBSQLite3SoftHeapLimit,
	D0, LONG, maxBytes,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_soft_heap_limit(maxBytes);
	d1(KPrintF("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P1(VOID, LIBSQLite3ThreadCleanup,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_thread_cleanup();
	d1(KPrintF("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P6(LONG, LIBSQLite3PrepareV2,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zSql,
	D0, LONG, nBytes,
	A2, sqlite3_stmt **, ppStmt,
	A3, CONST_STRPTR *, pzTail,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START  zSql=<%s>\n", __FUNC__, __LINE__));
	Result = sqlite3_prepare_v2(db, (const char *) zSql,
		nBytes, ppStmt, (const char **) pzTail);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P9(LONG, LIBSQLite3CreateFunction,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zFunctionName,
	D0, LONG, nArg,
	D1, LONG, eTextRep,
	A2, APTR, userdata,
	A3, xFunctionFunc, xFunc,
	D2, xFunctionStep, xStep,
	D3, xFunctionFinal, xFinal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START  zFunctionName=<%s>\n", __FUNC__, __LINE__, (const char *) zFunctionName));
	Result = sqlite3_create_function(db, zFunctionName, nArg, eTextRep,
		userdata, xFunc, xStep, xFinal);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(LONG, LIBSQLite3CreateModule,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zName,
	A2, CONST sqlite3_module *, methods,
	A3, APTR, clientData,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START  zName=<%s>\n", __FUNC__, __LINE__));
	Result = sqlite3_create_module(db, zName, methods, clientData);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3DeclareVtab,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zCreateTable,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START  zCreateTable=<%s>\n", __FUNC__, __LINE__, zCreateTable));
	Result = sqlite3_declare_vtab(db, (const char *) zCreateTable);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(LONG, LIBSQLite3OverloadFunction,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zFuncName,
	D0, LONG, nArg,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START  zFuncName=<%s>\n", __FUNC__, __LINE__, zFuncName));
	Result = sqlite3_overload_function(db, (const char *) zFuncName, nArg);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P8(LONG, LIBSQLite3BlobOpen,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zDb,
	A2, CONST_STRPTR, zTable,
	A3, CONST_STRPTR, zColumn,
	D0, LONG, iRow,
	D1, LONG, flags,
	A4, sqlite3_blob **, ppBlob,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START  zTable=<%s>\n", __FUNC__, __LINE__, zTable));
	Result = sqlite3_blob_open(db, zDb, zTable, zColumn, iRow, flags, ppBlob);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3BlobClose,
	A0, sqlite3_blob *, blob,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_blob_close(blob);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3BlobBytes,
	A0, sqlite3_blob *, blob,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_blob_bytes(blob);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(LONG, LIBSQLite3BlobRead,
	A0, sqlite3_blob *, blob,
	A1, APTR, z,
	D0, LONG, n,
	D1, LONG, iOffset,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_blob_read(blob, z, n, iOffset);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(LONG, LIBSQLite3BlobWrite,
	A0, sqlite3_blob *, blob,
	A1, CONST_APTR, z,
	D0, LONG, n,
	D1, LONG, iOffset,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_blob_write(blob, z, n, iOffset);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3ExtendedResultCodes,
	A0, sqlite3 *, db,
	D0, LONG, onoff,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_extended_result_codes(db, onoff);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(LONG, LIBSQLite3BindZeroBlob,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, i,
	D1, LONG, n,
	A6, struct SQLite3Base *, SQLite3Base);
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_bind_zeroblob(pStmt, i, n);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(CONST_STRPTR, LIBSQLite3ColumnDatabaseName,
	A0, sqlite3_stmt *, stmt,
	D0, LONG, iCol,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_database_name(stmt, iCol);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(CONST_STRPTR, LIBSQLite3ColumnTableName,
	A0, sqlite3_stmt *, stmt,
	D0, LONG, iCol,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_table_name(stmt, iCol);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(CONST_STRPTR, LIBSQLite3ColumnOriginName,
	A0, sqlite3_stmt *, stmt,
	D0, LONG, iCol,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_origin_name(stmt, iCol);
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(sqlite3_value *, LIBSQLite3ColumnValue,
	A0, sqlite3_stmt *, pStmt,
	D0, LONG, iCol,
	A6, struct SQLite3Base *, SQLite3Base);
{
	sqlite3_value *Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_column_value(pStmt, iCol);
	d1(KPrintF("%s/%ld:   END  Result=%08lx\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P7(LONG, LIBSQLite3CreateCollationV2,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zName,
	D0, LONG, eTextRep,
	A2, APTR, pCtx,
	A3, xCreateCollationCompare, xCompare,
	D1, xCreateCollationDestroy, xDestroy,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_create_collation_v2(db, zName, eTextRep, pCtx, xCompare, xDestroy);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P1(CONST_STRPTR, LIBSQLite3LibVersion,
	A6, struct SQLite3Base *, SQLite3Base)
{
	CONST_STRPTR Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_libversion();
	d1(KPrintF("%s/%ld:   END  Result=<%s>\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P1(LONG, LIBSQLite3LibversionNumber,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_libversion_number();
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(VOID, LIBSQLite3ResultErrorToobig,
	A0, sqlite3_context *, pCtx,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_result_error_toobig(pCtx);
	d1(KPrintF("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(VOID, LIBSQLite3ResultZeroBlob,
	A0, sqlite3_context *, pCtx,
	D0, LONG, n,
	A6, struct SQLite3Base *, SQLite3Base)
{
	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	sqlite3_result_zeroblob(pCtx, n);
	d1(KPrintF("%s/%ld:   END\n", __FUNC__, __LINE__));
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3ValueNumericType,
	A0, sqlite3_value *, pVal,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_value_numeric_type(pVal);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3ConfigV,
	D0, LONG, op,
	A0, APTR, ap,
	A6, struct SQLite3Base *, SQLite3Base)
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_configv(op, ap);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(LONG, LIBSQLlite3DbConfigV,
	A0, sqlite3 *, db,
	D0, LONG, op,
	A1, APTR, ap,
	A6, struct SQLite3Base *, SQLite3Base);
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_db_configv(db, op, ap);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(sqlite3_vfs *, LIBSQLite3VfsFind,
	A0, CONST_STRPTR, zVfsName,
	A6, struct SQLite3Base *, SQLite3Base);
{
	sqlite3_vfs *Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_vfs_find(zVfsName);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(LONG, LIBSQLite3VfsRegister,
	A0, sqlite3_vfs *, vfs,
	D0, LONG, makeDflt,
	A6, struct SQLite3Base *, SQLite3Base);
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_vfs_register(vfs, makeDflt);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(LONG, LIBSQLite3VfsUnregister,
	A0, sqlite3_vfs *, vfs,
	A6, struct SQLite3Base *, SQLite3Base);
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_vfs_unregister(vfs);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(LONG, LIBSQLite3FileControl,
	A0, sqlite3 *, db,
	A1, CONST_STRPTR, zDbName,
	D0, LONG, op,
	A2, void *, arg,
	A6, struct SQLite3Base *, SQLite3Base);
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_file_control(db, zDbName, op, arg);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(LONG, LIBSQLite3Status,
	D0, LONG, op,
	A0, LONG *, pCurrent,
	A1, LONG *, pHighwater,
	D1, LONG, resetFlag,
	A6, struct SQLite3Base *, SQLite3Base);
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_status(op, (int *) pCurrent, (int *) pHighwater, resetFlag);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P6(LONG, LIBSQLite3DbStatus,
	A0, sqlite3 *, db,
	D0, LONG, op,
	A1, LONG *, pCur,
	A2, LONG *, pHiwtr,
	D1, LONG, resetFlg,
	A6, struct SQLite3Base *, SQLite3Base);
{
	LONG Result;

	(void) SQLite3Base;
	d1(KPrintF("%s/%ld:   START\n", __FUNC__, __LINE__));
	Result = sqlite3_db_status(db, op, (int *) pCur, (int *) pHiwtr, resetFlg);
	d1(KPrintF("%s/%ld:   END  Result=%ld\n", __FUNC__, __LINE__, Result));
	return Result;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

#if !defined(__SASC)
// Replacement for SAS/C library functions

#if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)

struct WBStartup *_WBenchMsg;

int __fpclassify_double(float x)
{
	return 0;
}

void exit(int x)
{
   (void) x;
   while (1)
	  ;
}
#endif /*__MORPHOS__*/

#endif /* !defined(__SASC) */
//-----------------------------------------------------------------------------

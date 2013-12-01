#ifndef PRAGMAS_SQLITE3_PRAGMAS_H
#define PRAGMAS_SQLITE3_PRAGMAS_H

#ifndef CLIB_SQLITE3_PROTOS_H
#include <clib/sqlite3_protos.h>
#endif

#pragma  libcall SQLite3Base SQLite3Close           01e 801
#pragma  libcall SQLite3Base SQLite3Exec            024 0ba9805
#pragma  libcall SQLite3Base SQLite3Changes         02a 801
#pragma  libcall SQLite3Base SQLite3TotalChanges    030 801
#pragma  libcall SQLite3Base SQLite3Interrupt       036 801
#pragma  libcall SQLite3Base SQLite3Complete        03c 801
#pragma  libcall SQLite3Base SQLite3BusyHandler     042 a9803
#pragma  libcall SQLite3Base SQLite3BusyTimeout     048 0802
#pragma  libcall SQLite3Base SQLite3GetTable        04e 10ba9806
#pragma  libcall SQLite3Base SQLite3FreeTable       054 801
#pragma  libcall SQLite3Base SQLite3Free            05a 801
#pragma  libcall SQLite3Base SQLite3Trace           060 a9803
#pragma  libcall SQLite3Base SQLite3ProgressHandler 066 a90804
#pragma  libcall SQLite3Base SQLite3CommitHook      06c a9803
#pragma  libcall SQLite3Base SQLite3Open            072 9802
#pragma  libcall SQLite3Base SQLite3Errcode         078 801
#pragma  libcall SQLite3Base SQLite3Errmsg          07e 801
#pragma  libcall SQLite3Base SQLite3Prepare         084 ba09805
#pragma  libcall SQLite3Base SQLite3BindBlob        08a a190805
#pragma  libcall SQLite3Base SQLite3BindInt         090 10803
#pragma  libcall SQLite3Base SQLite3BindNull        096 0802
#pragma  libcall SQLite3Base SQLite3BindText        09c a190805
#pragma  libcall SQLite3Base SQLite3BindValue       0a2 90803
#pragma  libcall SQLite3Base SQLite3BindParameterCount 0a8 801
#pragma  libcall SQLite3Base SQLite3BindParameterName 0ae 0802
#pragma  libcall SQLite3Base SQLite3BindParameterIndex 0b4 9802
#pragma  libcall SQLite3Base SQLite3ClearBindings   0ba 801
#pragma  libcall SQLite3Base SQLite3ColumnCount     0c0 801
#pragma  libcall SQLite3Base SQLite3ColumnName      0c6 0802
#pragma  libcall SQLite3Base SQLite3ColumnDecltype  0cc 0802
#pragma  libcall SQLite3Base SQLite3Step            0d2 801
#pragma  libcall SQLite3Base SQLite3DataCount       0d8 801
#pragma  libcall SQLite3Base SQLite3ColumnBlob      0de 0802
#pragma  libcall SQLite3Base SQLite3ColumnBytes     0e4 0802
#pragma  libcall SQLite3Base SQLite3ColumnInt       0ea 0802
#pragma  libcall SQLite3Base SQLite3ColumnText      0f0 0802
#pragma  libcall SQLite3Base SQLite3ColumnType      0f6 0802
#pragma  libcall SQLite3Base SQLite3Finalize        0fc 801
#pragma  libcall SQLite3Base SQLite3Reset           102 801
#pragma  libcall SQLite3Base SQLite3AggregateCount  108 801
#pragma  libcall SQLite3Base SQLite3ValueBlob       10e 801
#pragma  libcall SQLite3Base SQLite3ValueBytes      114 801
#pragma  libcall SQLite3Base SQLite3ValueInt        11a 801
#pragma  libcall SQLite3Base SQLite3ValueText       120 801
#pragma  libcall SQLite3Base SQLite3ValueType       126 801
#pragma  libcall SQLite3Base SQLite3Aggregate_context 12c 0802
#pragma  libcall SQLite3Base SQLite3UserData        132 801
#pragma  libcall SQLite3Base SQLite3GetAuxdata      138 0802
#pragma  libcall SQLite3Base SQLite3SetAuxdata      13e a90804
#pragma  libcall SQLite3Base SQLite3ResultBlob      144 a09804
#pragma  libcall SQLite3Base SQLite3ResultError     14a 09803
#pragma  libcall SQLite3Base SQLite3ResultInt       150 0802
#pragma  libcall SQLite3Base SQLite3ResultNull      156 801
#pragma  libcall SQLite3Base SQLite3ResultText      15c a09804
#pragma  libcall SQLite3Base SQLite3ResultValue     162 9802
#pragma  libcall SQLite3Base SQLite3CreateCollation 168 ba09805
#pragma  libcall SQLite3Base SQLite3CollationNeeded 16e 90803
#pragma  libcall SQLite3Base SQLite3Sleep           174 001
#pragma  libcall SQLite3Base SQLite3Expired         17a 801
#pragma  libcall SQLite3Base SQLite3TransferBindings 180 9802
#pragma  libcall SQLite3Base SQLite3GlobalRecover   186 00
#pragma  libcall SQLite3Base SQLite3GetAutocommit   18c 801
#pragma  libcall SQLite3Base SQLite3DbHandle        192 801
#pragma  libcall SQLite3Base SQLite3RollbackHook    198 a9803
#pragma  libcall SQLite3Base SQLite3EnableSharedCache 19e 001
#pragma  libcall SQLite3Base SQLite3ReleaseMemory   1a4 001
#pragma  libcall SQLite3Base SQLite3SoftHeapLimit   1aa 001
#pragma  libcall SQLite3Base SQLite3ThreadCleanup   1b0 00
#pragma  libcall SQLite3Base SQLite3PrepareV2       1b6 ba09805
#pragma  libcall SQLite3Base SQLite3CreateFunction  1bc 32ba109808
#pragma  libcall SQLite3Base SQLite3CreateModule    1c2 ba9804
#pragma  libcall SQLite3Base SQLite3DeclareVtab     1c8 9802
#pragma  libcall SQLite3Base SQLite3OverloadFunction 1ce 09803
#pragma  libcall SQLite3Base SQLite3BlobOpen        1d4 c10ba9807
#pragma  libcall SQLite3Base SQLite3BlobClose       1da 801
#pragma  libcall SQLite3Base SQLite3BlobBytes       1e0 801
#pragma  libcall SQLite3Base SQLite3BlobRead        1e6 109804
#pragma  libcall SQLite3Base SQLite3BlobWrite       1ec 109804
#pragma  libcall SQLite3Base SQLite3ExtendedResultCodes 1f2 0802
#pragma  libcall SQLite3Base SQLite3BindZeroBlob    1f8 10803
#pragma  libcall SQLite3Base SQLite3ColumnDatabaseName 1fe 0802
#pragma  libcall SQLite3Base SQLite3ColumnTableName 204 0802
#pragma  libcall SQLite3Base SQLite3ColumnOriginName 20a 0802
#pragma  libcall SQLite3Base SQLite3ColumnValue     210 0802
#pragma  libcall SQLite3Base SQLite3CreateCollationV2 216 1ba09806
#pragma  libcall SQLite3Base SQLite3LibVersion      21c 00
#pragma  libcall SQLite3Base SQLite3LibversionNumber 222 00
#pragma  libcall SQLite3Base SQLite3ResultErrorToobig 228 801
#pragma  libcall SQLite3Base SQLite3ResultZeroBlob  22e 0802
#pragma  libcall SQLite3Base SQLite3ValueNumericType 234 801
#pragma  libcall SQLite3Base SQLite3ConfigV         23a 8002
#pragma  libcall SQLite3Base SQLlite3DbConfigV      240 90803
#pragma  libcall SQLite3Base SQLite3VfsFind         246 801
#pragma  libcall SQLite3Base SQLite3VfsRegister     24c 0802
#pragma  libcall SQLite3Base SQLite3VfsUnregister   252 801
#pragma  libcall SQLite3Base SQLite3FileControl     258 a09804
#pragma  libcall SQLite3Base SQLite3Status          25e 198004
#pragma  libcall SQLite3Base SQLite3DbStatus        264 1a90805
#ifdef __SASC_60
#pragma  tagcall SQLite3Base SQLite3Config          23a 8002
#pragma  tagcall SQLite3Base SQLlite3DbConfig       240 90803
#endif

#endif	/*  PRAGMAS_SQLITE3_PRAGMA_H  */

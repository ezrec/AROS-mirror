		IFND LIBRARIES_SQLITE3_LIB_I
LIBRARIES_SQLITE3_LIB_I	SET	1

		XDEF	_LVOSQLite3Close
		XDEF	_LVOSQLite3Exec
		XDEF	_LVOSQLite3Changes
		XDEF	_LVOSQLite3TotalChanges
		XDEF	_LVOSQLite3Interrupt
		XDEF	_LVOSQLite3Complete
		XDEF	_LVOSQLite3BusyHandler
		XDEF	_LVOSQLite3BusyTimeout
		XDEF	_LVOSQLite3GetTable
		XDEF	_LVOSQLite3FreeTable
		XDEF	_LVOSQLite3Free
		XDEF	_LVOSQLite3Trace
		XDEF	_LVOSQLite3ProgressHandler
		XDEF	_LVOSQLite3CommitHook
		XDEF	_LVOSQLite3Open
		XDEF	_LVOSQLite3Errcode
		XDEF	_LVOSQLite3Errmsg
		XDEF	_LVOSQLite3Prepare
		XDEF	_LVOSQLite3BindBlob
		XDEF	_LVOSQLite3BindInt
		XDEF	_LVOSQLite3BindNull
		XDEF	_LVOSQLite3BindText
		XDEF	_LVOSQLite3BindValue
		XDEF	_LVOSQLite3BindParameterCount
		XDEF	_LVOSQLite3BindParameterName
		XDEF	_LVOSQLite3BindParameterIndex
		XDEF	_LVOSQLite3ClearBindings
		XDEF	_LVOSQLite3ColumnCount
		XDEF	_LVOSQLite3ColumnName
		XDEF	_LVOSQLite3ColumnDecltype
		XDEF	_LVOSQLite3Step
		XDEF	_LVOSQLite3DataCount
		XDEF	_LVOSQLite3ColumnBlob
		XDEF	_LVOSQLite3ColumnBytes
		XDEF	_LVOSQLite3ColumnInt
		XDEF	_LVOSQLite3ColumnText
		XDEF	_LVOSQLite3ColumnType
		XDEF	_LVOSQLite3Finalize
		XDEF	_LVOSQLite3Reset
		XDEF	_LVOSQLite3AggregateCount
		XDEF	_LVOSQLite3ValueBlob
		XDEF	_LVOSQLite3ValueBytes
		XDEF	_LVOSQLite3ValueInt
		XDEF	_LVOSQLite3ValueText
		XDEF	_LVOSQLite3ValueType
		XDEF	_LVOSQLite3Aggregate_context
		XDEF	_LVOSQLite3UserData
		XDEF	_LVOSQLite3GetAuxdata
		XDEF	_LVOSQLite3SetAuxdata
		XDEF	_LVOSQLite3ResultBlob
		XDEF	_LVOSQLite3ResultError
		XDEF	_LVOSQLite3ResultInt
		XDEF	_LVOSQLite3ResultNull
		XDEF	_LVOSQLite3ResultText
		XDEF	_LVOSQLite3ResultValue
		XDEF	_LVOSQLite3CreateCollation
		XDEF	_LVOSQLite3CollationNeeded
		XDEF	_LVOSQLite3Sleep
		XDEF	_LVOSQLite3Expired
		XDEF	_LVOSQLite3TransferBindings
		XDEF	_LVOSQLite3GlobalRecover
		XDEF	_LVOSQLite3GetAutocommit
		XDEF	_LVOSQLite3DbHandle
		XDEF	_LVOSQLite3RollbackHook
		XDEF	_LVOSQLite3EnableSharedCache
		XDEF	_LVOSQLite3ReleaseMemory
		XDEF	_LVOSQLite3SoftHeapLimit
		XDEF	_LVOSQLite3ThreadCleanup
		XDEF	_LVOSQLite3PrepareV2
		XDEF	_LVOSQLite3CreateFunction
		XDEF	_LVOSQLite3CreateModule
		XDEF	_LVOSQLite3DeclareVtab
		XDEF	_LVOSQLite3OverloadFunction
		XDEF	_LVOSQLite3BlobOpen
		XDEF	_LVOSQLite3BlobClose
		XDEF	_LVOSQLite3BlobBytes
		XDEF	_LVOSQLite3BlobRead
		XDEF	_LVOSQLite3BlobWrite
		XDEF	_LVOSQLite3ExtendedResultCodes
		XDEF	_LVOSQLite3BindZeroBlob
		XDEF	_LVOSQLite3ColumnDatabaseName
		XDEF	_LVOSQLite3ColumnTableName
		XDEF	_LVOSQLite3ColumnOriginName
		XDEF	_LVOSQLite3ColumnValue
		XDEF	_LVOSQLite3CreateCollationV2
		XDEF	_LVOSQLite3LibVersion
		XDEF	_LVOSQLite3LibversionNumber
		XDEF	_LVOSQLite3ResultErrorToobig
		XDEF	_LVOSQLite3ResultZeroBlob
		XDEF	_LVOSQLite3ValueNumericType
		XDEF	_LVOSQLite3ConfigV
		XDEF	_LVOSQLlite3DbConfigV
		XDEF	_LVOSQLite3VfsFind
		XDEF	_LVOSQLite3VfsRegister
		XDEF	_LVOSQLite3VfsUnregister
		XDEF	_LVOSQLite3FileControl
		XDEF	_LVOSQLite3Status
		XDEF	_LVOSQLite3DbStatus

_LVOSQLite3Close            	EQU	-30
_LVOSQLite3Exec             	EQU	-36
_LVOSQLite3Changes          	EQU	-42
_LVOSQLite3TotalChanges     	EQU	-48
_LVOSQLite3Interrupt        	EQU	-54
_LVOSQLite3Complete         	EQU	-60
_LVOSQLite3BusyHandler      	EQU	-66
_LVOSQLite3BusyTimeout      	EQU	-72
_LVOSQLite3GetTable         	EQU	-78
_LVOSQLite3FreeTable        	EQU	-84
_LVOSQLite3Free             	EQU	-90
_LVOSQLite3Trace            	EQU	-96
_LVOSQLite3ProgressHandler  	EQU	-102
_LVOSQLite3CommitHook       	EQU	-108
_LVOSQLite3Open             	EQU	-114
_LVOSQLite3Errcode          	EQU	-120
_LVOSQLite3Errmsg           	EQU	-126
_LVOSQLite3Prepare          	EQU	-132
_LVOSQLite3BindBlob         	EQU	-138
_LVOSQLite3BindInt          	EQU	-144
_LVOSQLite3BindNull         	EQU	-150
_LVOSQLite3BindText         	EQU	-156
_LVOSQLite3BindValue        	EQU	-162
_LVOSQLite3BindParameterCount	EQU	-168
_LVOSQLite3BindParameterName	EQU	-174
_LVOSQLite3BindParameterIndex	EQU	-180
_LVOSQLite3ClearBindings    	EQU	-186
_LVOSQLite3ColumnCount      	EQU	-192
_LVOSQLite3ColumnName       	EQU	-198
_LVOSQLite3ColumnDecltype   	EQU	-204
_LVOSQLite3Step             	EQU	-210
_LVOSQLite3DataCount        	EQU	-216
_LVOSQLite3ColumnBlob       	EQU	-222
_LVOSQLite3ColumnBytes      	EQU	-228
_LVOSQLite3ColumnInt        	EQU	-234
_LVOSQLite3ColumnText       	EQU	-240
_LVOSQLite3ColumnType       	EQU	-246
_LVOSQLite3Finalize         	EQU	-252
_LVOSQLite3Reset            	EQU	-258
_LVOSQLite3AggregateCount   	EQU	-264
_LVOSQLite3ValueBlob        	EQU	-270
_LVOSQLite3ValueBytes       	EQU	-276
_LVOSQLite3ValueInt         	EQU	-282
_LVOSQLite3ValueText        	EQU	-288
_LVOSQLite3ValueType        	EQU	-294
_LVOSQLite3Aggregate_context	EQU	-300
_LVOSQLite3UserData         	EQU	-306
_LVOSQLite3GetAuxdata       	EQU	-312
_LVOSQLite3SetAuxdata       	EQU	-318
_LVOSQLite3ResultBlob       	EQU	-324
_LVOSQLite3ResultError      	EQU	-330
_LVOSQLite3ResultInt        	EQU	-336
_LVOSQLite3ResultNull       	EQU	-342
_LVOSQLite3ResultText       	EQU	-348
_LVOSQLite3ResultValue      	EQU	-354
_LVOSQLite3CreateCollation  	EQU	-360
_LVOSQLite3CollationNeeded  	EQU	-366
_LVOSQLite3Sleep            	EQU	-372
_LVOSQLite3Expired          	EQU	-378
_LVOSQLite3TransferBindings 	EQU	-384
_LVOSQLite3GlobalRecover    	EQU	-390
_LVOSQLite3GetAutocommit    	EQU	-396
_LVOSQLite3DbHandle         	EQU	-402
_LVOSQLite3RollbackHook     	EQU	-408
_LVOSQLite3EnableSharedCache	EQU	-414
_LVOSQLite3ReleaseMemory    	EQU	-420
_LVOSQLite3SoftHeapLimit    	EQU	-426
_LVOSQLite3ThreadCleanup    	EQU	-432
_LVOSQLite3PrepareV2        	EQU	-438
_LVOSQLite3CreateFunction   	EQU	-444
_LVOSQLite3CreateModule     	EQU	-450
_LVOSQLite3DeclareVtab      	EQU	-456
_LVOSQLite3OverloadFunction 	EQU	-462
_LVOSQLite3BlobOpen         	EQU	-468
_LVOSQLite3BlobClose        	EQU	-474
_LVOSQLite3BlobBytes        	EQU	-480
_LVOSQLite3BlobRead         	EQU	-486
_LVOSQLite3BlobWrite        	EQU	-492
_LVOSQLite3ExtendedResultCodes	EQU	-498
_LVOSQLite3BindZeroBlob     	EQU	-504
_LVOSQLite3ColumnDatabaseName	EQU	-510
_LVOSQLite3ColumnTableName  	EQU	-516
_LVOSQLite3ColumnOriginName 	EQU	-522
_LVOSQLite3ColumnValue      	EQU	-528
_LVOSQLite3CreateCollationV2	EQU	-534
_LVOSQLite3LibVersion       	EQU	-540
_LVOSQLite3LibversionNumber 	EQU	-546
_LVOSQLite3ResultErrorToobig	EQU	-552
_LVOSQLite3ResultZeroBlob   	EQU	-558
_LVOSQLite3ValueNumericType 	EQU	-564
_LVOSQLite3ConfigV          	EQU	-570
_LVOSQLlite3DbConfigV       	EQU	-576
_LVOSQLite3VfsFind          	EQU	-582
_LVOSQLite3VfsRegister      	EQU	-588
_LVOSQLite3VfsUnregister    	EQU	-594
_LVOSQLite3FileControl      	EQU	-600
_LVOSQLite3Status           	EQU	-606
_LVOSQLite3DbStatus         	EQU	-612

		ENDC

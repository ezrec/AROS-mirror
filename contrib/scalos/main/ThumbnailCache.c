// ThumbnailCache.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <scalos/scalosgfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/timer.h>

#include "debug.h"
#include <proto/scalosgfx.h>
#include <proto/scalos.h>
#include <proto/sqlite3.h>

#include <clib/alib_protos.h>

#include <defs.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "functions.h"
#include "Variables.h"

//-----------------------------------------------------------------------

#define	THUMBNAIL_TABLE_NAME_OLD	"ScalosThumbNailTable"
#define	THUMBNAIL_TABLE_NAME		"ScalosThumbNailTableV2"

// column inidecs for use with SQLite3Column_***()
#define	COLUMN_THUMBNAIL_ENDOFLIFE	0
#define	COLUMN_THUMBNAIL_FILENAME	1
#define	COLUMN_THUMBNAIL_DEPTH		2
#define	COLUMN_THUMBNAIL_PREFSWIDTH	3
#define	COLUMN_THUMBNAIL_PREFSHEIGHT	4
#define	COLUMN_THUMBNAIL_FILEDATE	5
#define	COLUMN_THUMBNAIL_IMAGEDATA	6

//-----------------------------------------------------------------------

struct CachedThumbnail
	{
	struct DateStamp ctn_FileDate;
	ULONG ctn_EndOfLife;
	ULONG ctn_ThumbnailDepth;
	ULONG ctn_prefWidth;
	ULONG ctn_prefHeight;
	UBYTE *ctn_ThumbnailData;
	size_t ctn_ThumbnailDataSize;
	};

//-----------------------------------------------------------------------

// local data items

//-----------------------------------------------------------------------

static int ThumbnailCacheAddEntry(sqlite3 *db, CONST_STRPTR FileName,
	const struct DateStamp *FileDate, ULONG Depth,
	void *ThumbnailData, size_t ThumbnailDataSize, ULONG LifetimeDays);
static BOOL ThumbnailCacheFindEntry(sqlite3 *db, CONST_STRPTR FileName,
	struct CachedThumbnail *ctn, ULONG LifetimeDays);
static BOOL ThumbnailCacheDeleteEntry(sqlite3 *db, CONST_STRPTR FileName);
static BOOL ThumbnailCacheUpdateEndOfLife(sqlite3 *db, CONST_STRPTR FileName, const T_TIMEVAL *tv);
static STRPTR ThumbnailCacheGetFileNameFromLock(BPTR fLock);
static BOOL ThumbnailCacheIsSameDay(ULONG sec1, ULONG sec2);
static int ThumbnailCacheSetMode(sqlite3 *db);
static int ThumbnailCacheCreateTable(sqlite3 *db);
static void ThumbnailCacheGetEndOfLife(T_TIMEVAL *tv, ULONG LifetimeDays);
static void ThumbnailCacheFileExists(sqlite3_context *context, LONG x, sqlite3_value **pVal);
static int ThumbnailCacheSetTempDir(sqlite3 *db, CONST_STRPTR DirName);

//-----------------------------------------------------------------------

BOOL ThumbnailCacheInit(void)
{
	sqlite3 *db = NULL;
	BOOL Success = TRUE;

	d1(KPrintF("%s/%s/%ld: START  SQLite3Base=%08lx\n", __FILE__, __FUNC__, __LINE__, SQLite3Base));

	do	{
		int rc;

		if (NULL == SQLite3Base)
			break;
		if (strlen(CurrentPrefs.pref_ThumbDatabasePath) < 1)
			break;

		rc = SQLite3Open(CurrentPrefs.pref_ThumbDatabasePath, &db);
		d1(KPrintF("%s/%s/%ld: rc=%ld  db=%08lx\n", __FILE__, __FUNC__, __LINE__, rc, db));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3Open() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		rc = SQLite3BusyTimeout(db, 100);
		if (SQLITE_OK != rc )
			{
			d1(KPrintF("%s/%s/%ld: SQLite3BusyTimeout() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		// try to create our table
		rc = ThumbnailCacheCreateTable(db);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc )
			{
			d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, SQLite3Errcode(db)));
			break;
			}

		rc = ThumbnailCacheSetMode(db);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc )
			{
			d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, SQLite3Errcode(db)));
			break;
			}
                } while (0);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	if (SQLite3Base)
		{
		SQLite3Close(db);
		}

	return Success;
}


void CleanupThumbnailCache(void)
{
	d1(KPrintF("%s/%s/%ld: START  \n", __FILE__, __FUNC__, __LINE__));

	// currently No-Op

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------

BOOL ThumbnailCacheOpen(APTR *pThumbnailCacheHandle)
{
	BOOL Success = FALSE;
	sqlite3 *db = NULL;

	d1(KPrintF("%s/%s/%ld: START  \n", __FILE__, __FUNC__, __LINE__));
	*pThumbnailCacheHandle = NULL;

	do	{
		LONG rc;

		if (NULL == SQLite3Base)
			break;
		if (strlen(CurrentPrefs.pref_ThumbDatabasePath) < 1)
			break;

		rc = SQLite3Open(CurrentPrefs.pref_ThumbDatabasePath, &db);
		d1(KPrintF("%s/%s/%ld: db=%08lx\n", __FILE__, __FUNC__, __LINE__, db));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3Open() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		rc = ThumbnailCacheCreateTable(db);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: ThumbnailCacheCreateTable() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		rc = ThumbnailCacheSetTempDir(db, CurrentPrefs.pref_SQLiteTempDir);
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: ThumbnailCacheSetTempDir() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		Success = TRUE;
		*pThumbnailCacheHandle = (APTR) db;
		} while (0);

	if (!Success)
		{
		ThumbnailCacheClose(pThumbnailCacheHandle);
		}

	d1(KPrintF("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}


void ThumbnailCacheClose(APTR *pThumbnailCacheHandle)
{
	sqlite3 *db = (sqlite3 *) *pThumbnailCacheHandle;

	d1(KPrintF("%s/%s/%ld: START  db=%08lx\n", __FILE__, __FUNC__, __LINE__, db));
	if (SQLite3Base && db)
		{
		SQLite3Close(db);
		*pThumbnailCacheHandle = NULL;
		}
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------

static int ThumbnailCacheAddEntry(sqlite3 *db, CONST_STRPTR FileName,
	const struct DateStamp *FileDate, ULONG Depth,
	void *ThumbnailData, size_t ThumbnailDataSize, ULONG LifetimeDays)
{
	int rc;

	d1(KPrintF("%s/%s/%ld: START FileName=<%s> ThumbnailData=%08lx ThumbnailDataSize=%lu\n", \
		__FILE__, __FUNC__, __LINE__, FileName, ThumbnailData, ThumbnailDataSize));

	do	{
		T_TIMEVAL tv;
		sqlite3_stmt *pStmt = NULL;
		CONST_STRPTR Tail = NULL;
		static CONST_STRPTR Cmd =
			"INSERT OR REPLACE INTO " THUMBNAIL_TABLE_NAME " "
			"VALUES(:endoflife, :path, :depth, "
			":prefwidth, :prefheight, :filedate, :thumbnail);";

		rc = SQLite3PrepareV2(db,
			Cmd, strlen(Cmd),
			&pStmt,
			&Tail);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3PrepareV2() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		// endoflife
		ThumbnailCacheGetEndOfLife(&tv, LifetimeDays);

		rc = SQLite3BindInt(pStmt, 1, tv.tv_secs);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		// path
		rc = SQLite3BindText(pStmt, 2,
			FileName, strlen(FileName),
			SQLITE_STATIC);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		// depth
		rc = SQLite3BindInt(pStmt, 3, Depth);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		// prefwidth
		rc = SQLite3BindInt(pStmt, 4, CurrentPrefs.pref_ThumbNailWidth);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		// prefheight
		rc = SQLite3BindInt(pStmt, 5, CurrentPrefs.pref_ThumbNailHeight);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		// filedate
		rc = SQLite3BindBlob(pStmt, 6,
			(void *) FileDate, sizeof(struct DateStamp),
			SQLITE_STATIC);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		// thumbnail
		rc = SQLite3BindBlob(pStmt, 7,
			ThumbnailData, ThumbnailDataSize,
			SQLITE_STATIC);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		do	{
			rc = SQLite3Step(pStmt);
			if (SQLITE_BUSY == rc)
				Delay(2);
			} while (SQLITE_ROW == rc || SQLITE_BUSY == rc);

		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc)
			break;

		rc = SQLite3Finalize(pStmt);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;
		} while (0);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;
}

//-----------------------------------------------------------------------

static BOOL ThumbnailCacheFindEntry(sqlite3 *db, CONST_STRPTR FileName,
	struct CachedThumbnail *ctn, ULONG LifetimeDays)
{
	sqlite3_stmt *pStmt = NULL;
	BOOL Found = FALSE;
	int rc;

	ctn->ctn_ThumbnailDepth = 0;
	ctn->ctn_prefWidth = ctn->ctn_prefHeight = 0;
	ctn->ctn_ThumbnailDataSize = 0;
	ctn->ctn_ThumbnailData = NULL;

	d1(KPrintF("%s/%s/%ld: START FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

	do	{
		const void *BlobData;
		const struct DateStamp *CachedFileDate;
		int DataCount;
		int ColumnBytes;
		CONST_STRPTR Tail = NULL;
		static CONST_STRPTR SelectCmd =
			"SELECT * FROM " THUMBNAIL_TABLE_NAME " "
			"WHERE IconPath = :path;";

		rc = SQLite3PrepareV2(db,
			SelectCmd, strlen(SelectCmd),
			&pStmt,
			&Tail);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3PrepareV2() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		rc = SQLite3BindText(pStmt, 1,
			FileName, strlen(FileName),
			SQLITE_STATIC);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		do	{
			rc = SQLite3Step(pStmt);
			if (SQLITE_BUSY == rc)
				Delay(2);
			d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
			} while (SQLITE_BUSY == rc);

		// rc is SQLITE_ROW if a record had been found
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_ROW != rc)
			break;

		// check if there are rows available
		DataCount = SQLite3DataCount(pStmt);
		Found = DataCount > 0;
		d1(KPrintF("%s/%s/%ld: DataCount=%ld  Found=%ld\n", __FILE__, __FUNC__, __LINE__, DataCount, Found));
		if (!Found)
			break;

		// return last access
		ctn->ctn_EndOfLife = SQLite3ColumnInt(pStmt, COLUMN_THUMBNAIL_ENDOFLIFE);

		// return stored nominal thumbnail width+height
		ctn->ctn_prefWidth = SQLite3ColumnInt(pStmt, COLUMN_THUMBNAIL_PREFSWIDTH);
		ctn->ctn_prefHeight = SQLite3ColumnInt(pStmt, COLUMN_THUMBNAIL_PREFSHEIGHT);

		// return depth of stored thumbnail
		ctn->ctn_ThumbnailDepth = SQLite3ColumnInt(pStmt, COLUMN_THUMBNAIL_DEPTH);

		// return file date
		ColumnBytes = SQLite3ColumnBytes(pStmt, COLUMN_THUMBNAIL_FILEDATE);
		d1(KPrintF("%s/%s/%ld: ColumnBytes=%ld\n", __FILE__, __FUNC__, __LINE__, ColumnBytes));
		if (sizeof(struct DateStamp) != ColumnBytes)
			break;
		CachedFileDate = SQLite3ColumnBlob(pStmt, COLUMN_THUMBNAIL_FILEDATE);
		d1(KPrintF("%s/%s/%ld: CachedFileDate=%08lx\n", __FILE__, __FUNC__, __LINE__, CachedFileDate));
		if (CachedFileDate)
			ctn->ctn_FileDate = *CachedFileDate;
		else
			break;

		// return copy of thumbnail image data
		BlobData = SQLite3ColumnBlob(pStmt, COLUMN_THUMBNAIL_IMAGEDATA);
		ctn->ctn_ThumbnailDataSize = SQLite3ColumnBytes(pStmt, COLUMN_THUMBNAIL_IMAGEDATA);

		d1(KPrintF("%s/%s/%ld: ctn_ThumbnailDataSize=%lu\n", __FILE__, __FUNC__, __LINE__, ctn->ctn_ThumbnailDataSize));

		if (ctn->ctn_ThumbnailDataSize)
			{
			ctn->ctn_ThumbnailData = ScalosAlloc(ctn->ctn_ThumbnailDataSize);
			if (ctn->ctn_ThumbnailData)
				memcpy(ctn->ctn_ThumbnailData, BlobData, ctn->ctn_ThumbnailDataSize);
			else
				ctn->ctn_ThumbnailDataSize = 0;
			}
		} while (0);

	if (pStmt)
		{
		/*rc =*/ SQLite3Finalize(pStmt);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		(void) rc;
		}

	d1(KPrintF("%s/%s/%ld: END  Found=%ld\n", __FILE__, __FUNC__, __LINE__, Found));

	return (BOOL) (Found && ctn->ctn_ThumbnailDataSize);
}

//-----------------------------------------------------------------------

static BOOL ThumbnailCacheDeleteEntry(sqlite3 *db, CONST_STRPTR FileName)
{
	int rc;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

	do	{
		sqlite3_stmt *pStmt = NULL;
		CONST_STRPTR Tail = NULL;
		static CONST_STRPTR Cmd =
			"DELETE " THUMBNAIL_TABLE_NAME " "
			"WHERE IconPath = :path;";

		rc = SQLite3PrepareV2(db,
			Cmd, strlen(Cmd),
			&pStmt,
			&Tail);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3PrepareV2() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		// bind IconPath
		rc = SQLite3BindText(pStmt, 1,
			FileName, strlen(FileName),
			SQLITE_STATIC);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		do	{
			rc = SQLite3Step(pStmt);
			if (SQLITE_BUSY == rc)
				Delay(2);
			} while (SQLITE_ROW == rc || SQLITE_BUSY == rc);

		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc)
			break;

		rc = SQLite3Finalize(pStmt);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------

static BOOL ThumbnailCacheUpdateEndOfLife(sqlite3 *db, CONST_STRPTR FileName, const T_TIMEVAL *tv)
{
	int rc;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

	do	{
		CONST_STRPTR Tail = NULL;
		sqlite3_stmt *pStmt = NULL;
		static CONST_STRPTR Cmd =
			"UPDATE " THUMBNAIL_TABLE_NAME " "
			"SET EndOfLife = :endoflife "
			"WHERE IconPath = :path;";

		rc = SQLite3PrepareV2(db,
			Cmd, strlen(Cmd),
			&pStmt,
			&Tail);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3PrepareV2() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		// bind EndOfLife
		rc = SQLite3BindInt(pStmt, 1, tv->tv_secs);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		// bind IconPath
		rc = SQLite3BindText(pStmt, 2,
			FileName, strlen(FileName),
			SQLITE_STATIC);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		do	{
			rc = SQLite3Step(pStmt);
			if (SQLITE_BUSY == rc)
				Delay(2);
			} while (SQLITE_ROW == rc || SQLITE_BUSY == rc);

		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc)
			break;

		rc = SQLite3Finalize(pStmt);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------

void ThumbnailCacheAddARGB(BPTR fLock, APTR ThumbnailCacheHandle,
	const struct ARGBHeader *argbh, ULONG LifetimeDays)
{
	sqlite3 *db = (sqlite3 *) ThumbnailCacheHandle;
	T_ExamineData *fib = NULL;
	STRPTR FileName = NULL;
	UBYTE *Buffer = NULL;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	debugLock_d1(fLock);

	do	{
		size_t ImageDataSize;
		size_t BuffLength;

		if (NULL == db)
			break;	// fail if thumbnail database is not available

		if (!ScalosExamineBegin(&fib))
			break;

		if (!ScalosExamineLock(fLock, &fib))
			break;

		FileName = ThumbnailCacheGetFileNameFromLock(fLock);
		if (NULL == FileName)
			break;

		ImageDataSize = argbh->argb_Width * argbh->argb_Height * sizeof(struct ARGB);
		BuffLength = sizeof(struct ARGBHeader) + ImageDataSize;

		Buffer = ScalosAlloc(BuffLength);
		if (NULL == Buffer)
			break;

		memcpy(Buffer, argbh, sizeof(struct ARGBHeader));
		memcpy(Buffer + sizeof(struct ARGBHeader), argbh->argb_ImageData, ImageDataSize);

		ThumbnailCacheAddEntry(db, FileName,
			ScalosExamineGetDate(fib), 24,
			Buffer, BuffLength, LifetimeDays);
		} while (0);

	if (Buffer)
		ScalosFree(Buffer);
	if (FileName)
		FreePathBuffer(FileName);
	ScalosExamineEnd(&fib);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------

void ThumbnailCacheAddRemapped(BPTR fLock, APTR ThumbnailCacheHandle,
	const struct ScalosBitMapAndColor *sac, ULONG LifetimeDays)
{
	sqlite3 *db = (sqlite3 *) ThumbnailCacheHandle;
	T_ExamineData *fib = NULL;
	STRPTR FileName = NULL;
	UBYTE *Buffer = NULL;
	struct RastPort TempRp;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	memset(&TempRp, 0, sizeof(TempRp));

	do	{
		size_t BuffLength;
		size_t PixelArrayLength;
		size_t ColorTableLength;
		struct RastPort SrcRp;
		ULONG *ColorTable;

		if (NULL == db)
			break;	// fail if thumbnail database is not available

		if (!ScalosExamineBegin(&fib))
			break;

		if (!ScalosExamineLock(fLock, &fib))
			break;

		FileName = ThumbnailCacheGetFileNameFromLock(fLock);
		if (NULL == FileName)
			break;

		ColorTableLength = 3 * sizeof(ULONG) * sac->sac_NumColors;
		PixelArrayLength = PIXELARRAY8_BUFFERSIZE(sac->sac_Width, sac->sac_Height);
		BuffLength = sizeof(struct ScalosBitMapAndColor)
			+ PixelArrayLength + ColorTableLength;

		Buffer = ScalosAlloc(BuffLength);
		if (NULL == Buffer)
			break;

		InitRastPort(&SrcRp);
		SrcRp.BitMap = (struct BitMap *) sac->sac_BitMap;

		memcpy(Buffer, sac, sizeof(struct ScalosBitMapAndColor));

		InitRastPort(&TempRp);
		TempRp.BitMap = AllocBitMap(TEMPRP_WIDTH(sac->sac_Width), 1, 8, 0, NULL);
		if (NULL == TempRp.BitMap)
			break;

		// Blit BitMap data to buffer
		ReadPixelArray8(&SrcRp,
			0, 0,
			sac->sac_Width - 1, sac->sac_Height - 1,
			Buffer + sizeof(struct ScalosBitMapAndColor),
			&TempRp);

		// Copy ColorTable to buffer
		ColorTable = (ULONG *) (Buffer + sizeof(struct ScalosBitMapAndColor) + PixelArrayLength);
		memcpy(ColorTable, sac->sac_ColorTable, ColorTableLength);

		ThumbnailCacheAddEntry(db, FileName,
			ScalosExamineGetDate(fib), sac->sac_Depth,
			Buffer, BuffLength, LifetimeDays);
		} while (0);

	if (TempRp.BitMap)
		{
		FreeBitMap(TempRp.BitMap);
		TempRp.BitMap = NULL;
		}
	if (Buffer)
		ScalosFree(Buffer);
	if (FileName)
		FreePathBuffer(FileName);
	ScalosExamineEnd(&fib);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------

BOOL ThumbnailCacheFindARGB(BPTR fLock, APTR ThumbnailCacheHandle,
	struct ARGBHeader *argbh, ULONG LifetimeDays)
{
	sqlite3 *db = (sqlite3 *) ThumbnailCacheHandle;
	T_ExamineData *fib = NULL;
	STRPTR FileName = NULL;
	struct CachedThumbnail ctn;
	BOOL Found = FALSE;
	BOOL ShouldDeleteEntry = FALSE;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	debugLock_d1(fLock);

	memset(&ctn, 0, sizeof(ctn));

	do	{
		T_TIMEVAL tv;
		size_t ImageDataSize;

		if (NULL == db)
			break;	// fail if thumbnail database is not available

		FileName = ThumbnailCacheGetFileNameFromLock(fLock);
		if (NULL == FileName)
			break;

		if (!ScalosExamineBegin(&fib))
			break;

		if (!ScalosExamineLock(fLock, &fib))
			break;

		if (!ThumbnailCacheFindEntry(db, FileName, &ctn, LifetimeDays))
			break;

		if (0 == ctn.ctn_ThumbnailDataSize || NULL == ctn.ctn_ThumbnailData)
			break;

		// check if we have an ARGB thumbnail here
		d1(KPrintF("%s/%s/%ld: ThumbnailDepth=%lu\n", __FILE__, __FUNC__, __LINE__, ctn.ctn_ThumbnailDepth));
		if (ctn.ctn_ThumbnailDepth != 24)
			break;

		if (ctn.ctn_prefWidth != CurrentPrefs.pref_ThumbNailWidth
			|| ctn.ctn_prefHeight != CurrentPrefs.pref_ThumbNailHeight)
			{
			// Cached thumbnail size doesn't match current settings
			// mark it for deletion
			ShouldDeleteEntry = TRUE;
			d1(KPrintF("%s/%s/%ld: width/height mismatch  w=%lu  h=%lu\n", \
				__FILE__, __FUNC__, __LINE__, ctn.ctn_prefWidth, ctn.ctn_prefHeight));
			break;
			}

		if (0 != CompareDates(&ctn.ctn_FileDate, ScalosExamineGetDate(fib)))
			{
			// cache entry is outdated
			// mark it for deletion
			d1(KPrintF("%s/%s/%ld: FileDate mismatch\n", __FILE__, __FUNC__, __LINE__));
			ShouldDeleteEntry = TRUE;
			break;
			}

		// copy ARGB header
		memcpy(argbh, ctn.ctn_ThumbnailData, sizeof(struct ARGBHeader));

		// allocate ARGB image data
		argbh->argb_ImageData = ScalosGfxCreateARGB(argbh->argb_Width, argbh->argb_Height, NULL);
		if (NULL == argbh->argb_ImageData)
			break;

		ImageDataSize = argbh->argb_Width * argbh->argb_Height * sizeof(struct ARGB);

		// copy ARGB data
		memcpy(argbh->argb_ImageData, ctn.ctn_ThumbnailData + sizeof(struct ARGBHeader), ImageDataSize);

		// Check if database entry access date should be updated
		// Since updating takes time, entries are updated no more than once a day.
		ThumbnailCacheGetEndOfLife(&tv, LifetimeDays);

		d1(KPrintF("%s/%s/%ld:  tv_secs=%lu  ctn_EndOfLife=%lu\n", \
			__FILE__, __FUNC__, __LINE__, tv.tv_secs, ctn.ctn_EndOfLife));

		if (!ThumbnailCacheIsSameDay(tv.tv_secs, ctn.ctn_EndOfLife))
			ThumbnailCacheUpdateEndOfLife(db, FileName, &tv);

		Found = TRUE;
                } while (0);

	if (FileName && ShouldDeleteEntry)
		ThumbnailCacheDeleteEntry(db, FileName);

	if (ctn.ctn_ThumbnailData)
		ScalosFree(ctn.ctn_ThumbnailData);
	if (FileName)
		FreePathBuffer(FileName);
	ScalosExamineEnd(&fib);

	d1(KPrintF("%s/%s/%ld: END  Found=%ld\n", __FILE__, __FUNC__, __LINE__, Found));

	return Found;
}

//-----------------------------------------------------------------------

struct ScalosBitMapAndColor *ThumbnailCacheFindRemapped(BPTR fLock,
	APTR ThumbnailCacheHandle, ULONG LifetimeDays)
{
	sqlite3 *db = (sqlite3 *) ThumbnailCacheHandle;
	struct RastPort TempRp;
	T_ExamineData *fib = NULL;
	STRPTR FileName = NULL;
	BOOL Found = FALSE;
	BOOL ShouldDeleteEntry = FALSE;
	struct CachedThumbnail ctn;
	struct ScalosBitMapAndColor *newsac = NULL;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	debugLock_d1(fLock);

	memset(&TempRp, 0, sizeof(TempRp));
	memset(&ctn, 0, sizeof(ctn));

	do	{
		T_TIMEVAL tv;
		size_t ColorTableLength;
		size_t PixelArrayLength;
		const UBYTE *PixelArray;
		const ULONG *ColorTable;
		struct RastPort DestRp;
		struct ScalosBitMapAndColor sac;

		memset(&sac, 0, sizeof(struct ScalosBitMapAndColor));

		if (NULL == db)
			break;	// fail if thumbnail database is not available

		InitRastPort(&DestRp);

		FileName = ThumbnailCacheGetFileNameFromLock(fLock);
		if (NULL == FileName)
			break;

		if (!ScalosExamineBegin(&fib))
			break;

		if (!ScalosExamineLock(fLock, &fib))
			break;

		if (!ThumbnailCacheFindEntry(db, FileName, &ctn, LifetimeDays))
			break;

		if (0 == ctn.ctn_ThumbnailDataSize || NULL == ctn.ctn_ThumbnailData)
			break;

		// check if we have a SAC thumbnail here
		d1(KPrintF("%s/%s/%ld: ThumbnailDepth=%lu\n", __FILE__, __FUNC__, __LINE__, ctn.ctn_ThumbnailDepth));
		if (ctn.ctn_ThumbnailDepth > 8)
			break;

		if (ctn.ctn_prefWidth != CurrentPrefs.pref_ThumbNailWidth
			|| ctn.ctn_prefHeight != CurrentPrefs.pref_ThumbNailHeight)
			{
			// Cached thumbnail size doesn't match current settings
			// mark it for deletion
			ShouldDeleteEntry = TRUE;
			d1(KPrintF("%s/%s/%ld: width/height mismatch  w=%lu  h=%lu\n", \
				__FILE__, __FUNC__, __LINE__, ctn.ctn_prefWidth, ctn.ctn_prefHeight));
			break;
			}

		if (0 != CompareDates(&ctn.ctn_FileDate, ScalosExamineGetDate(fib)))
			{
			// cache entry is outdated
			// mark it for deletion
			d1(KPrintF("%s/%s/%ld: FileDate mismatch\n", __FILE__, __FUNC__, __LINE__));
			ShouldDeleteEntry = TRUE;
			break;
			}

		// copy ScalosBitMapAndColor
		memcpy(&sac, ctn.ctn_ThumbnailData, sizeof(struct ScalosBitMapAndColor));

		d1(KPrintF("%s/%s/%ld:  Width=%ld  Height=%ld  Depth=%ld  TransparentColor=%ld  NumColors=%ld\n", \
			__FILE__, __FUNC__, __LINE__, sac.sac_Width, sac.sac_Height, sac.sac_Depth, \
			sac.sac_TransparentColor, sac.sac_NumColors));

		ColorTableLength = 3 * sizeof(ULONG) * sac.sac_NumColors;
		PixelArrayLength = PIXELARRAY8_BUFFERSIZE(sac.sac_Width, sac.sac_Height);

		PixelArray = ctn.ctn_ThumbnailData + sizeof(struct ScalosBitMapAndColor);
		ColorTable = (ULONG *) (ctn.ctn_ThumbnailData + sizeof(struct ScalosBitMapAndColor) + PixelArrayLength);

		d1(KPrintF("%s/%s/%ld:  PixelArray=%08lx  ColorTable=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, PixelArray, ColorTable));
		d1(KPrintF("%s/%s/%ld:  PixelArrayLength=%lu  ColorTableLength=%lu\n", \
			__FILE__, __FUNC__, __LINE__, PixelArrayLength, ColorTableLength));

		newsac = ScalosGfxCreateSAC(sac.sac_Width, sac.sac_Height, sac.sac_Depth, NULL, NULL);
		d1(KPrintF("%s/%s/%ld:  newsac=%08lx\n", __FILE__, __FUNC__, __LINE__, newsac));
		if (NULL == newsac)
			break;

		newsac->sac_TransparentColor = sac.sac_TransparentColor;

		// Copy data into colortable
		memcpy(newsac->sac_ColorTable, ColorTable, ColorTableLength);

		// Initialize temporary RastPort
		InitRastPort(&TempRp);
		TempRp.BitMap = AllocBitMap(TEMPRP_WIDTH(newsac->sac_Width), 1, 8, 0, NULL);
		if (NULL == TempRp.BitMap)
			break;

		d1(KPrintF("%s/%s/%ld:  TempRp.BitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, TempRp.BitMap));

		// Blit image data to BitMap
		DestRp.BitMap = newsac->sac_BitMap;
		WritePixelArray8(&DestRp,
			0, 0,
			newsac->sac_Width - 1, newsac->sac_Height - 1,
			(UBYTE *) PixelArray,
			&TempRp);

		// Check if database entry access date should be updated
		// Since updating takes time, entries are updated only once a day.
		ThumbnailCacheGetEndOfLife(&tv, LifetimeDays);

		d1(KPrintF("%s/%s/%ld:  tv_secs=%lu  ctn_EndOfLife=%lu\n", \
			__FILE__, __FUNC__, __LINE__, tv.tv_secs, ctn.ctn_EndOfLife));

		if (!ThumbnailCacheIsSameDay(tv.tv_secs, ctn.ctn_EndOfLife))
			ThumbnailCacheUpdateEndOfLife(db, FileName, &tv);

		Found = TRUE;
		WaitBlit();
                } while (0);

	if (FileName && ShouldDeleteEntry)
		ThumbnailCacheDeleteEntry(db, FileName);

	if (TempRp.BitMap)
		FreeBitMap(TempRp.BitMap);
	if (ctn.ctn_ThumbnailData)
		ScalosFree(ctn.ctn_ThumbnailData);
	if (FileName)
		FreePathBuffer(FileName);
	ScalosExamineEnd(&fib);

	if (!Found)
		{
		// cleanup allocated newsac if not found
		ScalosGfxFreeSAC(newsac);
		newsac = NULL;
		}

	d1(KPrintF("%s/%s/%ld: END  Found=%ld\n", __FILE__, __FUNC__, __LINE__, Found));

	return newsac;
}

//-----------------------------------------------------------------------

static STRPTR ThumbnailCacheGetFileNameFromLock(BPTR fLock)
{
	STRPTR FileName;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	debugLock_d1(fLock);

	FileName = AllocPathBuffer();
	if (FileName)
		{
		if (!NameFromLock(fLock, FileName, Max_PathLen))
			{
			FreePathBuffer(FileName);
			FileName = NULL;
			}
		}

	d1(KPrintF("%s/%s/%ld: END  FileName=%08lx\n", __FILE__, __FUNC__, __LINE__, FileName));

	return FileName;
}

//-----------------------------------------------------------------------
	      
static BOOL ThumbnailCacheIsSameDay(ULONG sec1, ULONG sec2)
{
	return (BOOL) (abs(sec1 - sec2) <= 60 * 60 * 24);
}

//-----------------------------------------------------------------------

BOOL ThumbnailCacheRemoveEntry(BPTR DirLock, CONST_STRPTR FileName)
{
	APTR ThumbnailCacheHandle;
	BOOL Success = FALSE;
	STRPTR FullPath;

	d1(KPrintF("%s/%s/%ld: START  FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

	do	{
		sqlite3 *db;

		FullPath = AllocPathBuffer();
		if (NULL == FullPath)
			break;

		if (!ThumbnailCacheOpen(&ThumbnailCacheHandle))
			break;

		db = (sqlite3 *) ThumbnailCacheHandle;
		if (NULL == db)
			break;

		if (!NameFromLock(DirLock, FullPath, Max_PathLen))
			break;

		if (!AddPart(FullPath, FileName, Max_PathLen))
			break;

		Success = ThumbnailCacheDeleteEntry(db, FullPath);
		} while (0);

	if (FullPath)
		FreePathBuffer(FullPath);

	ThumbnailCacheClose(&ThumbnailCacheHandle);

	d1(KPrintF("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------

// Cleanup thumbnail cache -- remove all entries that
// have not been accessed for a defined time.

BOOL ThumbnailCacheCleanup(APTR ThumbnailCacheHandle)
{
	sqlite3 *db = (sqlite3 *) ThumbnailCacheHandle;
	sqlite3_stmt *pStmt = NULL;
	int rc = SQLITE_DONE;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		T_TIMEVAL tv;
		CONST_STRPTR Tail = NULL;
		LONG Changes;
		static CONST_STRPTR SelectCmd =
			"DELETE FROM " THUMBNAIL_TABLE_NAME " "
			"WHERE EndOfLife < :accessdate OR NOT FILEXISTS(IconPath);";

		if (NULL == db)
			break;	// fail if thumbnail database is not available

		d1(KPrintF("%s/%s/%ld:  db=%08lx\n", __FILE__, __FUNC__, __LINE__, db));

		rc = SQLite3ExtendedResultCodes(db, TRUE);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3ExtendedResultCodes() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		// Delete old V1 table if exists
		/* rc = */SQLite3Exec(db,
			"DROP TABLE IF EXISTS " THUMBNAIL_TABLE_NAME_OLD ";",
			NULL,
			NULL,
			NULL);
		d1(KPrintF("%s/%s/%ld:  rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

		GetSysTime(&tv);

		// Register custom "FILEEXISTS" function
		rc = SQLite3CreateFunction(db,
			"FILEXISTS",
			1,
			SQLITE_ANY,
			NULL,
			ThumbnailCacheFileExists,
			NULL,
			NULL);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3CreateFunction() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		rc = SQLite3PrepareV2(db,
			SelectCmd, strlen(SelectCmd),
			&pStmt,
			&Tail);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: SQLite3PrepareV2() failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}

		rc = SQLite3BindInt(pStmt, 1, tv.tv_secs);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		do	{
			rc = SQLite3Step(pStmt);
			d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
			if (SQLITE_BUSY == rc)
				Delay(2);
			} while (SQLITE_ROW == rc || SQLITE_BUSY == rc);

		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc)
			break;

		rc = SQLite3Finalize(pStmt);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_OK != rc)
			break;

		pStmt = NULL;

		// Check how many records have been deleted
		Changes	= SQLite3Changes(db);
		d1(KPrintF("%s/%s/%ld: Changes=%ld\n", __FILE__, __FUNC__, __LINE__, Changes));

		if (Changes < 1)
			break;

		// If records have been deleted, compress the database now

		rc = SQLite3Exec(db,
			"VACUUM;",
			NULL,
			NULL,
			NULL);
		if (SQLITE_DONE != rc && SQLITE_OK != rc)
			{
			d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
			d1(KPrintF("%s/%s/%ld: VACUUM failed: <%s>\n", __FILE__, __FUNC__, __LINE__, SQLite3Errmsg(db)));
			break;
			}
		} while (0);

	if (pStmt)
		{
		/*rc =*/ SQLite3Finalize(pStmt);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		(void) rc;
		}

	d1(KPrintF("%s/%s/%ld: END  rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

	return (BOOL) (SQLITE_DONE == rc || SQLITE_OK == rc);
}

//-----------------------------------------------------------------------

static int ThumbnailCacheSetMode(sqlite3 *db)
{
	int rc;

	d1(KPrintF("%s/%s/%ld: START  db=%08lx\n", __FILE__, __FUNC__, __LINE__, db));

	do	{
		rc = SQLite3Exec(db,
			"PRAGMA journal_mode=wal;",
			NULL,
			NULL,
			NULL);
		d1(KPrintF("%s/%s/%ld: PRAGMA journal_mode=wal returns rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc && SQLITE_OK != rc)
			break;

		rc = SQLite3EnableSharedCache(TRUE);
		d1(KPrintF("%s/%s/%ld: SQLite3EnableSharedCache() returns rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc && SQLITE_OK != rc)
			break;

                } while (0);

	d1(KPrintF("%s/%s/%ld: END  rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

	return rc;
}


//-----------------------------------------------------------------------

static int ThumbnailCacheCreateTable(sqlite3 *db)
{
	int rc;

	d1(KPrintF("%s/%s/%ld: START  db=%08lx\n", __FILE__, __FUNC__, __LINE__, db));

	do	{
		rc = SQLite3Exec(db,
			"CREATE TABLE IF NOT EXISTS " THUMBNAIL_TABLE_NAME "("
				"EndOfLife integer DEFAULT CURRENT_DATESTAMP,"
				"IconPath text PRIMARY KEY UNIQUE ON CONFLICT REPLACE,"
				"ThumbnailDepth integer,"
				"PrefThumbnailWidth integer,"
				"PrefThumbnailHeight integer,"
				"FileDate integer,"
				"Thumbnail blob"
				");",
			NULL,
			NULL,
			NULL);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc && SQLITE_OK != rc)
			break;

		rc = SQLite3Exec(db,
			"CREATE INDEX IF NOT EXISTS EolIndex "
			"ON " THUMBNAIL_TABLE_NAME "(EndOfLife);",
			NULL,
			NULL,
			NULL);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc && SQLITE_OK != rc)
			break;

		rc = SQLite3Exec(db,
			"CREATE UNIQUE INDEX IF NOT EXISTS PathIndex "
				"ON " THUMBNAIL_TABLE_NAME "(IconPath);",
			NULL,
			NULL,
			NULL);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (SQLITE_DONE != rc && SQLITE_OK != rc)
			break;
                } while (0);

	d1(KPrintF("%s/%s/%ld: END  rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

	return rc;
}

//-----------------------------------------------------------------------

BOOL ThumbnailCacheBegin(APTR ThumbnailCacheHandle)
{
#if 0
	sqlite3 *db = (sqlite3 *) ThumbnailCacheHandle;
	int rc = SQLITE_DONE;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	if (SQLite3Base && db)
		{
		rc = SQLite3Exec(db,
			"BEGIN DEFERRED TRANSACTION;",
			NULL,
			NULL,
			NULL);
		}
	d1(KPrintF("%s/%s/%ld: END  rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

	return (BOOL) (SQLITE_DONE == rc || SQLITE_OK == rc);
#else
	return TRUE;
#endif
}

//-----------------------------------------------------------------------

BOOL ThumbnailCacheFinish(APTR ThumbnailCacheHandle)
{
#if 0
	sqlite3 *db = (sqlite3 *) ThumbnailCacheHandle;
	int rc = SQLITE_DONE;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (SQLite3Base && db)
		{
		rc = SQLite3Exec(db,
			"END TRANSACTION;",
			NULL,
			NULL,
			NULL);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		}

	d1(KPrintF("%s/%s/%ld: END  rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

	return (BOOL) (SQLITE_DONE == rc || SQLITE_OK == rc);
#else
	return TRUE;
#endif
}

//-----------------------------------------------------------------------

static void ThumbnailCacheGetEndOfLife(T_TIMEVAL *tv, ULONG LifetimeDays)
{
	GetSysTime(tv);
	if (LifetimeDays)
		tv->tv_secs += 24 * 60 * 60 * LifetimeDays;
	else
		tv->tv_secs = ULONG_MAX;	// forever
}

//-----------------------------------------------------------------------

static void ThumbnailCacheFileExists(sqlite3_context *context, LONG x, sqlite3_value **pVal)
{
	BOOL fileExists = FALSE;
	CONST_STRPTR Path;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	(void) x;

	Path = SQLite3ValueText(*pVal);
	d1(KPrintF("%s/%s/%ld: Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path ? Path : (CONST_STRPTR) ""));

	if (Path && strlen(Path) > 0)
		{
		BPTR fLock;

		fLock = Lock(Path, ACCESS_READ);
		d1(KPrintF("%s/%s/%ld: fLock=%08lx\n", __FILE__, __FUNC__, __LINE__, fLock));
		if (fLock)
			{
			fileExists = TRUE;
			UnLock(fLock);
			}
		else
			{
			LONG ErrCode = IoErr();

			if (ERROR_DEVICE_NOT_MOUNTED == ErrCode)
				fileExists = TRUE;
			}
		}

	SQLite3ResultInt(context, fileExists);

	d1(KPrintF("%s/%s/%ld: END  fileExists=%ld\n", __FILE__, __FUNC__, __LINE__, fileExists));
}

//-----------------------------------------------------------------------

static int ThumbnailCacheSetTempDir(sqlite3 *db, CONST_STRPTR DirName)
{
	STRPTR CmdStr;
	int rc;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		size_t Length;
		static CONST_STRPTR Cmd =
			"PRAGMA temp_store_directory = \"%s\";";

		Length = 1 + strlen(Cmd) + strlen(DirName);

		CmdStr = ScalosAlloc(Length);
		if (NULL == CmdStr)
			{
			rc = SQLITE_NOMEM;
			break;
			}

		snprintf(CmdStr, Length, Cmd, DirName);

		rc = SQLite3Exec(db,
			CmdStr,
			NULL,
			NULL,
			NULL);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		} while (0);

	if (CmdStr)
		ScalosFree(CmdStr);

	d1(KPrintF("%s/%s/%ld: END  rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

	return rc;
}

//-----------------------------------------------------------------------



// Shortcuts.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dos.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include "debug.h"
#include <proto/scalos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Scalos_rev.h"
#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

#define	ID_SCT		MAKE_ID('S','C','T',0)
#define	MAGIC_SCT	0x800e0000

#define	FLAG_USE_CRC	(1 << 0)

#define	SHORTCUTS_FILENAME      "SYS:Prefs/Ambient/shortcuts.prefs"
#define	SHORTCUTS_FILENAME_BAK  SHORTCUTS_FILENAME ".bak"

struct SctPrefsField
	{
	ULONG spf_Number;	// ordinal number of this field
	ULONG spf_Length;	// Length of data in bytes
	UBYTE spf_Data[1];	// Field Data <spf_Length> Bytes
	};

struct SctShortcut
	{
	struct Node ssc_Node;
	struct SctPrefsField *ssc_Name;
	struct SctPrefsField *ssc_Left;
	struct SctPrefsField *ssc_Top;
	};

//----------------------------------------------------------------------------

// local functions

static BOOL ShortcutWritePrefs(void);
static BOOL WriteSctField(BPTR fh, ULONG *crc, ULONG ordinal, const void *data, ULONG dataLength);
static BOOL ReadDataCRC(BPTR fh, ULONG *crc, void *data, ULONG dataLength);
static BOOL WriteDataCRC(BPTR fh, ULONG *crc, const void *data, ULONG dataLength);
static struct SctPrefsField *ReadSctField(BPTR fh, ULONG *crc);

//----------------------------------------------------------------------------

// local Data items

static struct BackDropList bdl;
static ULONG ShortcutFileVersion;

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------

void ShortcutFreePrefs(void)
{
	BackdropFreeList(&bdl);
}

//----------------------------------------------------------------------------

// Add all lines from Shortcut bdl to bdlVolume which refer to the volume of the current directory
BOOL ShortcutAddLines(struct BackDropList *bdlVolume)
{
	BOOL Success = FALSE;
	BPTR VolumeLock = BNULL;

	do	{
		struct ChainedLine *cnl;

		if (!bdl.bdl_Loaded)
			{
			// No Ambient Shortcut Prefs present
			// e.g. on AmigaOS4.x or AmigaOS3.x machines
			Success = TRUE;
			break;
			}

		VolumeLock = Lock(":", ACCESS_READ);
		debugLock_d1(VolumeLock);
		if (BNULL == VolumeLock)
			break;

		for (cnl = (struct ChainedLine *) bdl.bdl_LinesList.lh_Head;
			cnl != (struct ChainedLine *) &bdl.bdl_LinesList.lh_Tail;
			cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ )
			{
			STRPTR Path;

			Path = ScalosAlloc(1 + strlen(cnl->cnl_Line));
			if (Path)
				{
				STRPTR lp;
				BPTR tmpLock;

				strcpy(Path, cnl->cnl_Line);

				// remove file part
				lp = PathPart(Path);
				if (lp)
					*lp = '\0';

				tmpLock = Lock(Path, ACCESS_READ);
				debugLock_d1(tmpLock);
				if (tmpLock)
					{
					LONG sameLock = SameLock(tmpLock, VolumeLock);

					if (LOCK_SAME == sameLock || LOCK_SAME_VOLUME == sameLock)
						{
						// Volume name must be omitted,
						// line shall start with ":"
						CONST_STRPTR lp = strchr(cnl->cnl_Line, ':');

						d1(KPrintF("%s/%s/%ld:  Add backdrop: <%s>  PosX=%ld  PosY=%ld\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line, cnl->cnl_PosX, cnl->cnl_PosY));

						if (lp)
							BackdropAddLine(bdlVolume, lp, cnl->cnl_PosX, cnl->cnl_PosY);
						}

					UnLock(tmpLock);
					}
				ScalosFree(Path);
				}
			}

		Success = TRUE;
		} while (0);

	if (BNULL != VolumeLock)
		UnLock(VolumeLock);

	return Success;
}

//----------------------------------------------------------------------------

// Read Ambient's Shortcut preferences

void ShortcutReadPrefs(void)
{
	BPTR fh = BNULL;
	BPTR IconDirLock = BNULL;
	STRPTR IconPath = NULL;

	do	{
		CONST_STRPTR FileName = SHORTCUTS_FILENAME;
		ULONG NumberOfEntries;
		ULONG LData;
		ULONG CRC = 0;
		BOOL Abort = FALSE;
		BOOL useCRC;

		if (bdl.bdl_Loaded)
			break;

		BackDropInitList(&bdl);

		fh = Open((STRPTR) FileName, MODE_OLDFILE);
		d1(KPrintF("%s/%s/%ld:  fh=%08lx\n", __FILE__, __FUNC__, __LINE__, fh));
		if ((BPTR)NULL == fh)
			break;

		IconPath = AllocPathBuffer();
		d1(KPrintF("%s/%s/%ld:  IconPath=%08lx\n", __FILE__, __FUNC__, __LINE__, IconPath));
		if (NULL == IconPath)
			break;

		if (!ReadDataCRC(fh, &CRC, &LData, sizeof(LData)))
			break;

		d1(KPrintF("%s/%s/%ld:  LData=%08lx\n", __FILE__, __FUNC__, __LINE__, LData));

		if ((LData & 0xffffff00) != ID_SCT)
			break;

		ShortcutFileVersion = LData & 0x000000ff;

		if (!ReadDataCRC(fh, &CRC, &LData, sizeof(LData)))
			break;

		d1(KPrintF("%s/%s/%ld:  LData=%08lx\n", __FILE__, __FUNC__, __LINE__, LData));

		useCRC = LData & FLAG_USE_CRC;

		if (1 == ShortcutFileVersion)
			{
			if (!ReadDataCRC(fh, &CRC, &LData, sizeof(LData)))
				break;
			}

		if (!ReadDataCRC(fh, &CRC, &LData, sizeof(LData)))
			break;

		d1(KPrintF("%s/%s/%ld:  LData=%08lx\n", __FILE__, __FUNC__, __LINE__, LData));
		if (LData != MAGIC_SCT)
		      break;

		if (!ReadDataCRC(fh, &CRC, &NumberOfEntries, sizeof(NumberOfEntries)))
			break;

		d1(KPrintF("%s/%s/%ld:  NumberOfEntries=%lu\n", __FILE__, __FUNC__, __LINE__, NumberOfEntries));

		while (NumberOfEntries--)
			{
			ULONG EntryNumber;
			ULONG NumberOfFields;
			struct SctShortcut ssc;

			memset(&ssc, 0, sizeof(ssc));

			if (!ReadDataCRC(fh, &CRC, &EntryNumber, sizeof(EntryNumber)))
				break;

			d1(KPrintF("%s/%s/%ld:  EntryNumber=%lu\n", __FILE__, __FUNC__, __LINE__, EntryNumber));

			if (!ReadDataCRC(fh, &CRC, &NumberOfFields, sizeof(NumberOfFields)))
				break;

			d1(KPrintF("%s/%s/%ld:  NumberOfFields=%lu\n", __FILE__, __FUNC__, __LINE__, NumberOfFields));

			// we expect 3 fields
			if (3 != NumberOfFields)
				break;

			while (!Abort && NumberOfFields--)
				{
				struct SctPrefsField *sct;

				sct = ReadSctField(fh, &CRC);
				if (NULL == sct)
					{
					Abort = TRUE;
					break;
					}

				switch (sct->spf_Number)
					{
				case 1:		// Path
					ssc.ssc_Name = sct;
					break;
				case 2:		// Left
					ssc.ssc_Left = sct;
					break;
				case 3:		// Top
					ssc.ssc_Top = sct;
					break;
				default:
					ScalosFree(sct);
					break;
					}
				}

			if (ssc.ssc_Name && ssc.ssc_Left && ssc.ssc_Top)
				{
				LONG PosX, PosY;

				PosX = (LONG) *((ULONG *) ssc.ssc_Left->spf_Data);
				PosY = (LONG) *((ULONG *) ssc.ssc_Top->spf_Data);

				d1(KPrintF("%s/%s/%ld:  Add backdrop: <%s>  PosX=%ld  PosY=%ld\n", __FILE__, __FUNC__, __LINE__, ssc.ssc_Name->spf_Data, PosX, PosY));

				if (!BackdropAddLine(&bdl, ssc.ssc_Name->spf_Data, PosX, PosY))
					break;
				}

			if (ssc.ssc_Name)
				ScalosFree(ssc.ssc_Name);
			if (ssc.ssc_Left)
				ScalosFree(ssc.ssc_Left);
			if (ssc.ssc_Top)
				ScalosFree(ssc.ssc_Top);
			}

		if (useCRC)
			{
			// read CRC
			if (!ReadDataCRC(fh, NULL, &LData, sizeof(LData)))
				break;
			d1(kprintf("%s/%s/%ld:  CRC  Read=%08lx  calculated=%08lx\n", __FILE__, __FUNC__, __LINE__, LData, CRC));

			if (useCRC && (CRC != LData))
				break;		// CRC mismatch
			}

		bdl.bdl_Changed = FALSE;
		bdl.bdl_Loaded = TRUE;
		} while (0);

	if (IconDirLock)
		UnLock(IconDirLock);
	if (IconPath)
		FreePathBuffer(IconPath);
	if (fh)
		Close(fh);
}

//----------------------------------------------------------------------------

static BOOL ShortcutWritePrefs(void)
{
	BOOL Success = FALSE;
	BPTR fh = BNULL;

	do	{
		static const char *VerString = "VER: Scalos_Prefs " VERS_REV_STR " (" DATE ")";
		CONST_STRPTR FileName = SHORTCUTS_FILENAME;
		struct ChainedLine *cnl;
		ULONG NumberOfEntries = 0;
		LONG Length;
		ULONG LData;
		ULONG EntryNumber = 0x80000000;
		ULONG CRC = 0;
		char ch;

		if (!bdl.bdl_Loaded)
			break;		// Shortcut file was never loaded

		// Try to rename old shortcut file
		(void) DeleteFile(SHORTCUTS_FILENAME_BAK);
		(void) Rename(SHORTCUTS_FILENAME, SHORTCUTS_FILENAME_BAK);

		fh = Open((STRPTR) FileName, MODE_NEWFILE);
		d1(KPrintF("%s/%s/%ld:  fh=%08lx\n", __FILE__, __FUNC__, __LINE__, fh));
		if (BNULL == fh)
			break;

		LData = (ID_SCT & 0xffffff00) | ShortcutFileVersion;
		if (!WriteDataCRC(fh, &CRC, &LData, sizeof(LData)))
			break;

		LData = 1;
		if (!WriteDataCRC(fh, &CRC, &LData, sizeof(LData)))
			break;

		if (1 == ShortcutFileVersion)
			{
			LData = 1;
			if (!WriteDataCRC(fh, &CRC, &LData, sizeof(LData)))
				break;
			}

		LData = MAGIC_SCT;
		if (!WriteDataCRC(fh, &CRC, &LData, sizeof(LData)))
			break;

		for (cnl = (struct ChainedLine *) bdl.bdl_LinesList.lh_Head;
			cnl != (struct ChainedLine *) &bdl.bdl_LinesList.lh_Tail;
			cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ )
			{
			NumberOfEntries++;
			}

		if (!WriteDataCRC(fh, &CRC, &NumberOfEntries, sizeof(NumberOfEntries)))
			break;

		d1(KPrintF("%s/%s/%ld:  NumberOfEntries=%lu\n", __FILE__, __FUNC__, __LINE__, NumberOfEntries));

		for (cnl = (struct ChainedLine *) bdl.bdl_LinesList.lh_Head;
			cnl != (struct ChainedLine *) &bdl.bdl_LinesList.lh_Tail;
			cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ, EntryNumber++ )
			{
			ULONG NumberOfFields = 3;

			if (!WriteDataCRC(fh, &CRC, &EntryNumber, sizeof(EntryNumber)))
				break;

			if (!WriteDataCRC(fh, &CRC, &NumberOfFields, sizeof(NumberOfFields)))
				break;

			// write path
			if (!WriteSctField(fh, &CRC, 1, cnl->cnl_Line, 1 + strlen(cnl->cnl_Line)))
				break;

			// write left position
			if (!WriteSctField(fh, &CRC, 2, &cnl->cnl_PosX, sizeof(cnl->cnl_PosX)))
				break;
			// write top position
			if (!WriteSctField(fh, &CRC, 3, &cnl->cnl_PosY, sizeof(cnl->cnl_PosY)))
				break;
			}

		// Write CRC
		Length = Write(fh, &CRC, sizeof(CRC));
		d1(KPrintF("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
		if (sizeof(CRC) != Length)
			break;

		// Write version string
		ch = '$';
		Length = Write(fh, &ch, sizeof(ch));
		d1(KPrintF("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
		if (sizeof(ch) != Length)
			break;

		Length = Write(fh, (APTR) VerString, strlen(VerString));
		d1(KPrintF("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
		if (strlen(VerString) != Length)
			break;

		Success = TRUE;
		} while (0);

	if (fh)
		Close(fh);

	return Success;
}

//----------------------------------------------------------------------------

static BOOL WriteSctField(BPTR fh, ULONG *crc, ULONG ordinal, const void *data, ULONG dataLength)
{
	// Write ordinal
	if (!WriteDataCRC(fh, crc, &ordinal, sizeof(ordinal)))
		return FALSE;

	// Write length
	if (!WriteDataCRC(fh, crc, &dataLength, sizeof(dataLength)))
		return FALSE;

	// Write data
	if (!WriteDataCRC(fh, crc, data, dataLength))
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------

static BOOL ReadDataCRC(BPTR fh, ULONG *crc, void *data, ULONG dataLength)
{
	LONG Length;

	Length = Read(fh, data, dataLength);
	d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
	if (dataLength != Length)
		return FALSE;

	if (crc)
		*crc = update_crc(*crc, data, dataLength);

	return TRUE;
}

//----------------------------------------------------------------------------

static BOOL WriteDataCRC(BPTR fh, ULONG *crc, const void *data, ULONG dataLength)
{
	LONG Length;

	*crc = update_crc(*crc, data, dataLength);
	Length = Write(fh, (APTR) data, dataLength);
	d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));

	return (BOOL) (dataLength == Length);
}

//----------------------------------------------------------------------------

static struct SctPrefsField *ReadSctField(BPTR fh, ULONG *crc)
{
	struct SctPrefsField *sct = NULL;
	ULONG FieldOrdinal;
	ULONG FieldDataSize;
	BOOL Success = FALSE;

	do	{
		if (!ReadDataCRC(fh, crc, &FieldOrdinal, sizeof(FieldOrdinal)))
			break;

		d1(KPrintF("%s/%s/%ld:  FieldOrdinal=%ld\n", __FILE__, __FUNC__, __LINE__, FieldOrdinal));

		if (!ReadDataCRC(fh, crc, &FieldDataSize, sizeof(FieldDataSize)))
			break;

		d1(KPrintF("%s/%s/%ld:  FieldDataSize=%ld\n", __FILE__, __FUNC__, __LINE__, FieldDataSize));

		sct = ScalosAlloc(sizeof(struct SctPrefsField) + FieldDataSize);
		d1(kprintf("%s/%s/%ld:  sct=%08lx\n", __FILE__, __FUNC__, __LINE__, sct));
		if (NULL == sct)
			break;

		sct->spf_Number = FieldOrdinal;
		sct->spf_Length = FieldDataSize;

		if (!ReadDataCRC(fh, crc, sct->spf_Data, FieldDataSize))
			break;

		Success = TRUE;
		} while (0);

	if (sct && !Success)
		{
		ScalosFree(sct);
		sct = NULL;
		}

	return sct;
}

//----------------------------------------------------------------------------

BOOL ShortcutAddEntry(STRPTR path, LONG PosX, LONG PosY)
{
	BOOL Success;

	d1(KPrintF("%s/%s/%ld:  Add Entry: <%s>  PosX=%ld  PosY=%ld\n", __FILE__, __FUNC__, __LINE__, path, PosX, PosY));

	Success = BackdropAddLine(&bdl, path, PosX, PosY);

	if (Success)
		ShortcutWritePrefs();

	return Success;
}

//----------------------------------------------------------------------------

BOOL ShortcutRemoveEntry(BPTR iconLock)
{
	BOOL Success;

	debugLock_d1(iconLock);

	Success = BackdropRemoveLine(&bdl, iconLock);

	if (Success)
		ShortcutWritePrefs();

	return Success;
}

//----------------------------------------------------------------------------



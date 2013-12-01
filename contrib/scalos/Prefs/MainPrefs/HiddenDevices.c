// HiddenDevices.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <scalos/preferences.h>
#include <scalos/scalos.h>
#include <intuition/intuition.h>
#include <devices/inputevent.h>
#include <libraries/iffparse.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <libraries/mcpgfx.h>
#include <libraries/ttengine.h>
#include <graphics/view.h>
#include <prefs/prefhdr.h>
#include <prefs/font.h>
#include <prefs/workbench.h>
#include <mui/NListview_mcc.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <proto/scalos.h>
#include <proto/utility.h>
#include <proto/wb.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <defs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <limits.h>

#include "HiddenDevices.h"
#include "ScalosPrefs.h"

//-----------------------------------------------------------------

struct HiddenDevice
	{
	struct Node hd_Node;
	char hd_Name[1];	// variable length
	};

struct UnknownPrefsChunk
	{
	struct Node upc_Node;
	LONG upc_ID;
	LONG upc_Type;
	LONG upc_Size;
	UBYTE upc_Data[1];	// variable length
	};

//-----------------------------------------------------------------

static void HandleWBHD(const UBYTE *data, size_t length);
static LONG CollectUnknownPrefsChunks(struct List *ChunksList, CONST_STRPTR Filename);
static LONG AddUnknownChunk(struct List *ChunksList, struct IFFHandle *iff, const struct ContextNode *cn);
static void CleanupUnknownChunks(struct List *ChunksList);
static void AddHiddenDeviceFromDosList(struct SCAModule *app, struct DosList *dl, struct InfoData *id, BOOL Hidden);
static void BtoCString(BPTR bstring, STRPTR Buffer, size_t Length);
static BOOL FindHiddenDevice(CONST_STRPTR Name);
static void StripTrailingColon(STRPTR Line);

//-----------------------------------------------------------------

static struct List HiddenDevicesList;

//-----------------------------------------------------------------

BOOL HiddenDevicesInit(void)
{
	NewList(&HiddenDevicesList);

	return TRUE;
}

//-----------------------------------------------------------------

void HiddenDevicesCleanup(void)
{
	struct HiddenDevice *hd;

	while ((hd = (struct HiddenDevice *) RemHead(&HiddenDevicesList)))
		{
		free(hd);
		}
}

//-----------------------------------------------------------------

// Read workbench.prefs and build list of hidden devices
BOOL ReadWorkbenchPrefs(CONST_STRPTR filename)
{
	struct IFFHandle *iff;
	LONG error;
	BOOL IffOpened = FALSE;
	static const ULONG pairs[] = {ID_PREF, ID_PREF, ID_WBHD};

	HiddenDevicesCleanup();

	d1(KPrintF(__FILE__ "/%s/%ld: filename=<%s>\n", __FUNC__, __LINE__, filename));

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			break;

		iff->iff_Stream = (ULONG) Open((STRPTR) filename, MODE_OLDFILE);
		d1(KPrintF(__FILE__ "/%s/%ld: iff_Stream=%08lx\n", __FUNC__, __LINE__, iff->iff_Stream));
		if (0 == iff->iff_Stream)
			break;

		InitIFFasDOS( iff );

		error = OpenIFF( iff, IFFF_READ );
		if (RETURN_OK != error)
			break;

		IffOpened = TRUE;

		error = CollectionChunks( iff, (LONG*) pairs, Sizeof(pairs) / 2);
		if (RETURN_OK != error)
			break;

		StopOnExit( iff, ID_PREF, ID_FORM );

		while(TRUE)
			{
			if( ( error = ParseIFF( iff, IFFPARSE_SCAN ) ) == IFFERR_EOC )
				{
				struct CollectionItem *ci;

				ci = FindCollection( iff, ID_PREF, ID_WBHD);
				while( ci )
					{
					HandleWBHD(ci->ci_Data, ci->ci_Size);
					ci = ci->ci_Next;
					}

				}
			else
				break;
			}
		} while (0);

	if (iff)
		{
		if (IffOpened)
			CloseIFF( iff );
		if (iff->iff_Stream)
			Close( (BPTR)iff->iff_Stream );
		FreeIFF( iff );
		}

	return TRUE;
}

//-----------------------------------------------------------------

static void HandleWBHD(const UBYTE *data, size_t length)
{
	d1(KPrintF(__FILE__ "/%s/%ld: data=<%s>  length=%lu\n", __FUNC__, __LINE__, data, length));

	if (!FindHiddenDevice((STRPTR)data))	// make sure we get no cuplicate entries
		AddHiddenDevice((STRPTR)data);
}

//-----------------------------------------------------------------

// save workbench.prefs, including WBHD entries for each hidden device
LONG WriteWorkbenchPrefs(CONST_STRPTR Filename)
{
	struct IFFHandle *iff = NULL;
	LONG Result;
	BOOL IffOpen = FALSE;
	struct List ChunksList;

	NewList(&ChunksList);

	do	{
		static const struct PrefHeader prefHeader = { 0, 0, 0L };
		struct HiddenDevice *hd;
		struct UnknownPrefsChunk *upc;

		d1(KPrintF(__FILE__ "/%s/%ld: Filename=<%s>\n", __FUNC__, __LINE__, Filename));

		Result = CollectUnknownPrefsChunks(&ChunksList, Filename);
		d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		iff = AllocIFF();
		if (NULL == iff)
			{
			Result = IoErr();
			break;
			}

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR)Open(Filename, MODE_NEWFILE);
		if (0 == iff->iff_Stream)
			{
			// ... try to create missing directories here
			STRPTR FilenameCopy;

			Result = IoErr();
			d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));

			FilenameCopy = AllocVec(1 + strlen(Filename), MEMF_PUBLIC);

			if (FilenameCopy)
				{
				STRPTR lp;

				strcpy(FilenameCopy, Filename);

				lp = PathPart(FilenameCopy);
				if (lp)
					{
					BPTR dirLock;

					*lp = '\0';
					dirLock = CreateDir(FilenameCopy);

					if (dirLock)
						UnLock(dirLock);

					iff->iff_Stream = (IPTR)Open(Filename, MODE_NEWFILE);
				if (0 == iff->iff_Stream)
						Result = IoErr();
					else
						Result = RETURN_OK;
					}

				FreeVec(FilenameCopy);
				}

			d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
			if (RETURN_OK != Result)
				break;
			}

		d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));

		Result = OpenIFF(iff, IFFF_WRITE);
		if (RETURN_OK != Result)
			break;

		IffOpen = TRUE;
		d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));

		Result = PushChunk(iff, ID_PREF, ID_FORM, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		Result = PushChunk(iff, 0, ID_PRHD, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		if (WriteChunkBytes(iff, (APTR) &prefHeader, sizeof(prefHeader)) < 0)
			{
			Result = IoErr();
			break;
			}

		Result = PopChunk(iff);		// PRHD
		if (RETURN_OK != Result)
			break;

		d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));

		for (hd = (struct HiddenDevice *) HiddenDevicesList.lh_Head;
			hd != (struct HiddenDevice *) &HiddenDevicesList.lh_Tail;
			hd = (struct HiddenDevice *) hd->hd_Node.ln_Succ)
			{
			size_t wbhdLength = 1 + strlen(hd->hd_Name);

			d1(KPrintF(__FILE__ "/%s/%ld: hd_Name=<%s>\n", __FUNC__, __LINE__, hd->hd_Name));

			Result = PushChunk(iff, 0, ID_WBHD, IFFSIZE_UNKNOWN);
			if (RETURN_OK != Result)
				break;

			if (wbhdLength != WriteChunkBytes(iff, (APTR) hd->hd_Name, wbhdLength))
				{
				Result = IoErr();
				break;
				}
			if (wbhdLength & 0x01)
				{
				// write dummy byte to get even chunk length
				ULONG Dummy = 0;

				if (1 != WriteChunkBytes(iff, (APTR) &Dummy, 1))
					{
					Result = IoErr();
					break;
					}
				}

			Result = PopChunk(iff);		// WBHD
			if (RETURN_OK != Result)
				break;
			}

		d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));

		if (RETURN_OK != Result)
			break;

		for (upc = (struct UnknownPrefsChunk *) ChunksList.lh_Head;
			upc != (struct UnknownPrefsChunk *) &ChunksList.lh_Tail;
			upc = (struct UnknownPrefsChunk *) upc->upc_Node.ln_Succ )
			{
			d1(KPrintF("%s/%s/%ld: Write Chunk Type=%08lx, ID=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, \
				upc->upc_Type, upc->upc_ID, upc->upc_Size));

			Result = PushChunk(iff, 0, upc->upc_ID, IFFSIZE_UNKNOWN);
			if (RETURN_OK != Result)
				break;

			if (upc->upc_Size != WriteChunkBytes(iff, (APTR) upc->upc_Data, upc->upc_Size))
				{
				Result = IoErr();
				break;
				}

			Result = PopChunk(iff);		// upc->upc_ID
			if (RETURN_OK != Result)
				break;
			}

		Result = PopChunk(iff);		// FORM
		if (RETURN_OK != Result)
			break;

		} while (0);

	CleanupUnknownChunks(&ChunksList);

	if (iff)
		{
		if (IffOpen)
			CloseIFF(iff);

		if (iff->iff_Stream)
			{
			Close((BPTR)iff->iff_Stream);
			iff->iff_Stream = 0;
			}

		FreeIFF(iff);
		}

	d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));

	if (RETURN_OK == Result && fCreateIcons)
		SaveIcon(Filename);

	return Result;
}

//-----------------------------------------------------------------

static LONG CollectUnknownPrefsChunks(struct List *ChunksList, CONST_STRPTR Filename)
{
	LONG Result;
	struct IFFHandle *iff;
	BOOL iffOpened = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: Filename=<%s>\n", __FUNC__, __LINE__, Filename));

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			{
			Result = IoErr();
			break;
			}

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR)Open(Filename, MODE_OLDFILE);
		if (0 == iff->iff_Stream)
			{
			Result = IoErr();
			break;
			}

		Result = OpenIFF(iff, IFFF_READ);
		if (RETURN_OK != Result)
			break;

		iffOpened = TRUE;

		while (RETURN_OK == Result)
			{
			struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_RAWSTEP);
			d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
			if (IFFERR_EOC == Result)
				{
				Result = RETURN_OK;
				continue;
				}
			if (IFFERR_EOF == Result)
				{
				Result = RETURN_OK;
				break;
				}
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);
			if (NULL == cn)
				continue;

			switch (cn->cn_ID)
				{
			case ID_FORM:
				d1(KPrintF(__FILE__ "/%s/%ld: ID_FORM  Size=%ld\n", __FUNC__, __LINE__, cn->cn_Size));
				break;
			case ID_PREF:
				d1(KPrintF(__FILE__ "/%s/%ld: ID_PREF  Size=%ld\n", __FUNC__, __LINE__, cn->cn_Size));
				break;
			case ID_PRHD:
				d1(KPrintF(__FILE__ "/%s/%ld: ID_PRHD  Size=%ld\n", __FUNC__, __LINE__, cn->cn_Size));
				break;
			default:
				d1(KPrintF("%s/%s/%ld: Found Chunk Type=%08lx, ID=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, \
					cn->cn_Type, cn->cn_ID, cn->cn_Size));
				Result = AddUnknownChunk(ChunksList, iff, cn);
				break;
				}
			}
		} while (0);

	if (iff)
		{
		if (iffOpened)
			CloseIFF(iff);

		if (iff->iff_Stream)
			Close((BPTR)iff->iff_Stream);

		FreeIFF(iff);
		}

	return Result;
}

//-----------------------------------------------------------------

static LONG AddUnknownChunk(struct List *ChunksList, struct IFFHandle *iff, const struct ContextNode *cn)
{
	struct UnknownPrefsChunk *upc;
	LONG Result = RETURN_OK;

	do	{
		LONG Length;

		upc = malloc(sizeof(struct UnknownPrefsChunk) + cn->cn_Size);
		if (NULL == upc)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		upc->upc_ID = cn->cn_ID;
		upc->upc_Type = cn->cn_Type;
		upc->upc_Size = cn->cn_Size;

		Length = ReadChunkBytes(iff, upc->upc_Data, upc->upc_Size);
		if (Length != upc->upc_Size)
			{
			Result = IoErr();
			break;
			}

		AddTail(ChunksList, &upc->upc_Node);
		upc = NULL;
		} while (0);

	if (upc)
		{
		free(upc);
		}

	d1(KPrintF(__FILE__ "/%s/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));

	return Result;
}

//-----------------------------------------------------------------

static void CleanupUnknownChunks(struct List *ChunksList)
{
	struct UnknownPrefsChunk *upc;

	while ((upc = (struct UnknownPrefsChunk *) RemTail(ChunksList)))
		{
		free(upc);
		}
}

//-----------------------------------------------------------------

// Traverse DosList for all devices, and build list of hideable devices
void FillHiddenDevicesList(struct SCAModule *app)
{
	ULONG LockDosListFlags = LDF_DEVICES | LDF_READ;
	struct HiddenDevice *hd;
	struct DosList *dl;
	struct InfoData *id;

	set(app->Obj[NLIST_HIDDENDEVICES], MUIA_NList_Quiet, TRUE);
	DoMethod(app->Obj[NLIST_HIDDENDEVICES], MUIM_NList_Clear);
	DoMethod(app->Obj[NLIST_HIDDENDEVICES], MUIM_NList_UseImage,
		NULL, MUIV_NList_UseImage_All, 0);

	id = malloc(sizeof(struct InfoData));
	if (id)
		{
		dl = LockDosList(LockDosListFlags);

		while (dl = NextDosEntry(dl, LockDosListFlags))
			{
			AddHiddenDeviceFromDosList(app, dl, id, FALSE);
			}

		UnLockDosList(LockDosListFlags);
		free(id);
		}

	// Now walk through the list of devices marked as hidden,
	// and add all entries.

	dl = LockDosList(LockDosListFlags);

	for (hd = (struct HiddenDevice *) HiddenDevicesList.lh_Head;
		hd != (struct HiddenDevice *) &HiddenDevicesList.lh_Tail;
		hd = (struct HiddenDevice *) hd->hd_Node.ln_Succ)
		{
		char HiddenDevName[128];
		struct DosList *dlFound;

		// Name must not have trailing ":" to use with FindDosEntry()
		stccpy(HiddenDevName, hd->hd_Name, sizeof(HiddenDevName));
		StripTrailingColon(HiddenDevName);
		d1(KPrintF(__FILE__ "/%s/%ld: HiddenDevName=<%s>\n", __FUNC__, __LINE__, HiddenDevName));

		dlFound = FindDosEntry(dl, HiddenDevName, LockDosListFlags);
		d1(KPrintF(__FILE__ "/%s/%ld: dl=%08lx\n", __FUNC__, __LINE__, dlFound));
		if (dlFound)
			{
			AddHiddenDeviceFromDosList(app, dlFound, id, TRUE);
			}
		else
			{
			struct NewHiddenDevice nhd;

			nhd.nhd_DeviceName = hd->hd_Name;
			nhd.nhd_VolumeName = "";
			nhd.nhd_Hidden = TRUE;

			d1(KPrintF(__FILE__ "/%s/%ld: DevName=<%s>  VolName=<%s>\n", \
				__FUNC__, __LINE__, nhd.nhd_DeviceName, nhd.nhd_VolumeName));

			DoMethod(app->Obj[NLIST_HIDDENDEVICES],
				MUIM_NList_InsertSingle,
				&nhd,
				MUIV_NList_Insert_Sorted);
			}
		}

	UnLockDosList(LockDosListFlags);

	set(app->Obj[NLIST_HIDDENDEVICES], MUIA_NList_Quiet, FALSE);
}

//-----------------------------------------------------------------

static void AddHiddenDeviceFromDosList(struct SCAModule *app, struct DosList *dl, struct InfoData *id, BOOL Hidden)
{
	char DevName[128];
	char VolName[128];
	struct DosList *VolumeNode;
	BOOL InfoDataValid = FALSE;

	strcpy(VolName, "");
	BtoCString(dl->dol_Name, DevName, sizeof(DevName));

	d1(KPrintF(__FILE__ "/%s/%ld: START  DevName=<%s>\n", __FUNC__, __LINE__, DevName));

#if defined(__amigaos4__)
	if (0 != Stricmp(DevName, "ENV"))
#endif //!defined(__amigaos4__)
		{
		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
		if (dl->dol_Task &&
			DoPkt(dl->dol_Task, ACTION_DISK_INFO,
				(IPTR)MKBADDR(id),
				0, 0, 0, 0))
			{
			InfoDataValid = TRUE;
			}
		d1(KPrintF(__FILE__ "/%s/%ld: InfoDataValid=%ld\n", __FUNC__, __LINE__, InfoDataValid));
		}

	if (!Hidden && !InfoDataValid)
		return;

	if (InfoDataValid)
		{
		switch (id->id_DiskState)
			{
		case ID_WRITE_PROTECTED:
		case ID_VALIDATING:
		case ID_VALIDATED:
			VolumeNode = BADDR(id->id_VolumeNode);
			if (VolumeNode)
				{
				BtoCString(VolumeNode->dol_Name, VolName, sizeof(VolName));
				StripTrailingColon(VolName);
				strcat(VolName, ":");
				}
			break;
		default:
			break;
			}
		}

	StripTrailingColon(DevName);
	strcat(DevName, ":");

	// add only visible devices here
	if (Hidden || !FindHiddenDevice(DevName))
		{
		struct NewHiddenDevice nhd;

		nhd.nhd_DeviceName = DevName;
		nhd.nhd_VolumeName = VolName;
		nhd.nhd_Hidden = Hidden;

		d1(KPrintF(__FILE__ "/%s/%ld: DevName=<%s>  VolName=<%s>\n", \
			__FUNC__, __LINE__, nhd.nhd_DeviceName, nhd.nhd_VolumeName));

		DoMethod(app->Obj[NLIST_HIDDENDEVICES],
			MUIM_NList_InsertSingle,
			&nhd,
			MUIV_NList_Insert_Sorted);
		}
}

//-----------------------------------------------------------------

static void BtoCString(BPTR bstring, STRPTR Buffer, size_t Length)
{
#ifdef __AROS__
	// AROS needs special handling because it uses NULL-terminated
	// strings on some platforms.
	size_t Len = AROS_BSTR_strlen(bstring);
	if (Len >= Length)
		Len = Length - 1;
	strncpy(Buffer, AROS_BSTR_ADDR(bstring), Len);
	Buffer[Len] = '\0';
#else
	CONST_STRPTR Src = BADDR(bstring);
	size_t bLength;

	// get BSTRING length (1st octet)
	bLength = *Src++;

	while (bLength-- && (Length-- > 1))
		*Buffer++ = *Src++;

	*Buffer = '\0';
#endif
}

//-----------------------------------------------------------------

// Add entry to  hidden devices list
BOOL AddHiddenDevice(CONST_STRPTR Name)
{
	struct HiddenDevice *hd;
	BOOL Success = FALSE;

	hd = malloc(sizeof(struct HiddenDevice) + 1 + strlen(Name));
	if (hd)
		{
		strcpy(hd->hd_Name, Name);

		// make sure device name is terminated with exactly one ":"
		StripTrailingColon(hd->hd_Name);
		strcat(hd->hd_Name, ":");

		AddHead(&HiddenDevicesList, &hd->hd_Node);
		Success = TRUE;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: Name=<%s>  hd=%08lx\n", __FUNC__, __LINE__, Name, hd));

	return Success;
}

//-----------------------------------------------------------------

// Remove entry from hidden devices list
BOOL RemoveHiddenDevice(CONST_STRPTR Name)
{
	struct HiddenDevice *hd;
	BOOL Found = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: Name=<%s>", __FUNC__, __LINE__, Name));

	for (hd = (struct HiddenDevice *) HiddenDevicesList.lh_Head;
		hd != (struct HiddenDevice *) &HiddenDevicesList.lh_Tail;
		hd = (struct HiddenDevice *) hd->hd_Node.ln_Succ)
		{
		if (0 == Stricmp(Name, hd->hd_Name))
			{
			Found = TRUE;
			Remove(&hd->hd_Node);
			free(hd);
			break;
			}
		}

	return Found;
}

//-----------------------------------------------------------------

// search for entry in hidden devices list
static BOOL FindHiddenDevice(CONST_STRPTR Name)
{
	const struct HiddenDevice *hd;
	BOOL Found = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: Name=<%s>", __FUNC__, __LINE__, Name));

	for (hd = (struct HiddenDevice *) HiddenDevicesList.lh_Head;
		hd != (struct HiddenDevice *) &HiddenDevicesList.lh_Tail;
		hd = (struct HiddenDevice *) hd->hd_Node.ln_Succ)
		{
		if (0 == Stricmp(Name, hd->hd_Name))
			{
			Found = TRUE;
			break;
			}
		}

	return Found;
}

//-----------------------------------------------------------------

static void StripTrailingColon(STRPTR Line)
{
	size_t Len = strlen(Line);

	Line += Len - 1;

	if (':' == *Line)
		*Line = '\0';
}

//-----------------------------------------------------------------


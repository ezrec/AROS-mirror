// devicefilter.c
// $Date$
// $Revision$

#include <exec/types.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <proto/utility.h>
#include <proto/intuition.h>

#include <exec/lists.h>
#include <exec/nodes.h>
#include <dos/dos.h>
#include <libraries/iffparse.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>
#include <scalos/scalos.h>
#include <utility/tagitem.h>
#include <dos/dostags.h>
#include <prefs/prefhdr.h>
#include <prefs/workbench.h>

#include <string.h>

#include <defs.h>
#include "devicefilter.h"

//-----------------------------------------------------------------

#define	WBPREFSNAME             "ENV:sys/workbench.prefs"

struct HiddenDevice
	{
	struct Node hd_Node;
	char hd_Name[1];	// variable length
	};

struct HiddenDevVol
	{
	struct Node hdv_Node;
	STRPTR hdv_DeviceName;
	STRPTR hdv_VolumeName;
	};

//-----------------------------------------------------------------

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static void FilterDevices(struct ScalosNodeList *devices);
static void HiddenDevicesCleanup(void);
static void HandleWBHD(CONST_STRPTR data, size_t length, struct List *HiddenDevsList);
static void FillHiddenDevicesList(void);
static void AddHiddenDeviceFromDosList(struct DosList *dl, struct InfoData *id,
	BOOL Hidden, struct List *HiddenDevsList);
static void BtoCString(BPTR bstring, STRPTR Buffer, size_t Length);
static BOOL AddHiddenDevice(CONST_STRPTR Name, struct List *HiddenDevsList);
static BOOL FindHiddenDevice(CONST_STRPTR DevName, CONST_STRPTR VolName);
static void StripTrailingColon(STRPTR Line);
static APTR MyAllocVecPooled(size_t Size);
static void MyFreeVecPooled(APTR mem);
static void AddHiddenDevVol(CONST_STRPTR DevName, CONST_STRPTR VolName);
static BOOL StartWBPrefsProcess(void);
static SAVEDS(int) WBPrefsProcess(void);

//----------------------------------------------------------------------------

// Some information for the library structure of the plugin. You should keep these
// variable names as they but feel free to change the contents


// Things we'll be using in this source
extern struct ExecBase	*SysBase;
struct DosLibrary	*DOSBase;
struct Library		*IFFParseBase;
T_UTILITYBASE		UtilityBase;

#ifdef __amigaos4__
struct IntuitionBase	*IntuitionBase;
struct Library 		*NewlibBase;
#endif /* __amigaos4__ */

#ifdef __amigaos4__
struct DOSIFace		*IDOS = NULL;
struct UtilityIFace	*IUtility = NULL;
struct IntuitionIFace	*IIntuition = NULL;
struct IFFParseIFace	*IIFFParse = NULL;
struct Interface 	*INewlib = NULL;
#endif /* __amigaos4__ */

static void *MemPool;

static struct List HiddenDevicesList;
static struct SignalSemaphore HiddenDevicesSema;

static struct SignalSemaphore PubMemPoolSemaphore;
static struct Process *WBPrefsProc;

//----------------------------------------------------------------------------

// Plugin/library initialisation routine. Called once at OpenLibrary.
BOOL initPlugin(struct PluginBase *pluginbase)
{
	if (!OpenLibraries())
		return FALSE;

	d1(KPrintF("%s/%s/%ld: SysBase=%08lx  DOSBase=%08lx\n", __FILE__, __FUNC__, __LINE__, SysBase, DOSBase));

	NewList(&HiddenDevicesList);
	InitSemaphore(&HiddenDevicesSema);
	InitSemaphore(&PubMemPoolSemaphore);

	MemPool = CreatePool(MEMPOOL_MEMFLAGS, MEMPOOL_PUDDLESIZE, MEMPOOL_THRESHSIZE);
	d1(KPrintF("%s/%ld: MemPool=%08lx\n", __FUNC__, __LINE__, MemPool));
	if (NULL == MemPool)
		return FALSE;

	if (!StartWBPrefsProcess())
		return FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: Success\n", __FUNC__, __LINE__));

	return TRUE;
}


// Plugin/library cleanup routine. Called at library expunge.
void closePlugin(struct PluginBase *pluginbase)
{
	(void)pluginbase;

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));

	if (WBPrefsProc)
		{
		Signal((struct Task *) WBPrefsProc, SIGF_ABORT);
		Delay(5);	// wait for WBPrefs process to finish
		}

	HiddenDevicesCleanup();

	if (NULL != MemPool)
		{
		DeletePool(MemPool);
		MemPool = NULL;
		}

	CloseLibraries();
}

//-----------------------------------------------------------------

// Opens all the libraries we need
static BOOL OpenLibraries(void)
{
	DOSBase = (struct DosLibrary *) OpenLibrary(DOSNAME, 39);
	if (NULL == DOSBase )
		return FALSE;
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *) GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		return FALSE;
#endif /* __amigaos4__ */

	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;
#ifdef __amigaos4__
	IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
	if (NULL == IUtility)
		return FALSE;
#endif /* __amigaos4__ */

#ifdef __amigaos4__
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
	if (NULL == IntuitionBase)
		return FALSE;
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
#endif /* __amigaos4__ */

	IFFParseBase = OpenLibrary("iffparse.library", 39);
	if (NULL == IFFParseBase)
		return FALSE;
#ifdef __amigaos4__
	IIFFParse = (struct IFFParseIFace *) GetInterface(IFFParseBase, "main", 1, NULL);
	if (NULL == IIFFParse)
		return FALSE;

	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif /* __amigaos4__ */

	return(TRUE);
}


// Closes all the opened libraries
static void CloseLibraries(void)
{
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
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase = NULL;
		}
#endif /* __amigaos4__ */

#ifdef __amigaos4__
	if (IIFFParse)
		{
		DropInterface((struct Interface *) IIFFParse);
		IIFFParse = NULL;
		}
#endif /* __amigaos4__ */
	if (IFFParseBase)
		{
		CloseLibrary(IFFParseBase);
		IFFParseBase = NULL;
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

#ifndef __amigaos4__
	if (DOSBase)
		{
		CloseLibrary((struct Library *)DOSBase);
		DOSBase = NULL;
		}
#endif /* __amigaos4__ */
}

//-----------------------------------------------------------------

/* The dispatcher hook which deals with the messages sent and calls the appropriate functions
 * In this case, the DeviceList.cla does not take a return value
 *
 * It might be an idea to have a switch() in the dispatcher to call functions
 * to deal with whatever the messages are (if you intend to respond to multiple
 * messages), but in this case, I'll just stick everything in the dispatcher.
 */
M68KFUNC_P3(ULONG, Dispatcher,
	A0, Class *, cl,
	A2, Object *, obj,
	A1, Msg, msg)
{
	ULONG Result = 0;

	if (msg)
		{
		/* Check message pointer, cure an enforcer hit? */

		if (SCCM_DeviceList_Filter == msg->MethodID)
			{
			FilterDevices(((struct DeviceList_Filter_Msg *)msg)->devicenodes);
			}

		// Call the original filter
		Result = DoSuperMethodA(cl, obj, msg);
		}

	return Result;
}
M68KFUNC_END

//-----------------------------------------------------------------

static void FilterDevices(struct ScalosNodeList *devices)
{
	struct ScaDeviceIcon *dev;      // Current node in the list of devices Scalos provides

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	ObtainSemaphoreShared(&HiddenDevicesSema);

	dev = (struct ScaDeviceIcon *) devices->snl_MinNode;
	while (dev)
		{
		// Make sure device will be shown, then we hide it if required
		dev->di_Flags &= ~(DIBF_DontDisplay);

		d1(KPrintF("%s/%s/%ld: Device=%08lx  DiskType=%08lx  Devname=<%s>  di_Flags=%04x\n", __FILE__, __FUNC__, __LINE__, dev, dev->di_Info->id_DiskType, dev->di_Device, dev->di_Flags));

		if ((dev->di_Info) && (ID_UNREADABLE_DISK == dev->di_Info->id_DiskType || ID_NOT_REALLY_DOS == dev->di_Info->id_DiskType))
			{
			d1(KPrintF("%s/%s/%ld: NDOS or BAD\n", __FILE__, __FUNC__, __LINE__));
			dev->di_Flags |= DIBF_DontDisplay;
			}
		else
			{
			const struct HiddenDevVol *hdv;

			for (hdv = (const struct HiddenDevVol *) HiddenDevicesList.lh_Head;
				hdv != (const struct HiddenDevVol *) &HiddenDevicesList.lh_Tail;
				hdv = (const struct HiddenDevVol *) hdv->hdv_Node.ln_Succ)
				{
				d1(KPrintF("%s/%s/%ld: hdv=%08lx  hdv_DeviceName=<%s>  hvd_VolumeName=<%s>\n", __FILE__, __FUNC__, __LINE__, hdv, hdv->hdv_DeviceName, hdv->hdv_VolumeName));

				if ((dev->di_Device && (0 == Stricmp(hdv->hdv_DeviceName, dev->di_Device)))
					|| (dev->di_Volume && (0 == Stricmp(hdv->hdv_VolumeName, dev->di_Volume))))
					{
					d1(KPrintF("%s/%s/%ld: Matched\n", __FILE__, __FUNC__, __LINE__));
					dev->di_Flags |= DIBF_DontDisplay;
					break;
					}
				}
			}
		dev = (struct ScaDeviceIcon *) dev->di_Node.mln_Succ;
		}

	ReleaseSemaphore(&HiddenDevicesSema);
}

//-----------------------------------------------------------------

static void HiddenDevicesCleanup(void)
{
	struct HiddenDevVol *hdv;

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	ObtainSemaphore(&HiddenDevicesSema);

	while ((hdv = (struct HiddenDevVol *) RemHead(&HiddenDevicesList)))
		{
		d1(KPrintF("%s/%ld:  hdv=%08lx  DevName=<%s>  VolName=<%s>\n", __FUNC__, __LINE__, hdv, hdv->hdv_DeviceName, hdv->hdv_VolumeName));
		d1(KPrintF("%s/%ld:  hdv=%08lx  DevName=%08lx  VolName=%08lx\n", __FUNC__, __LINE__, hdv, hdv->hdv_DeviceName, hdv->hdv_VolumeName));
		if (hdv->hdv_DeviceName)
			{
			MyFreeVecPooled(hdv->hdv_DeviceName);
			hdv->hdv_DeviceName = NULL;
			}
		if (hdv->hdv_VolumeName)
			{
			MyFreeVecPooled(hdv->hdv_VolumeName);
			hdv->hdv_VolumeName = NULL;
			}

		MyFreeVecPooled(hdv);
		}

	ReleaseSemaphore(&HiddenDevicesSema);

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
}

//-----------------------------------------------------------------

// Read workbench.prefs and build list of hidden devices
static BOOL ReadWorkbenchPrefs(CONST_STRPTR filename, struct List *HiddenDevsList)
{
	struct IFFHandle *iff;
	LONG error;
	BOOL IffOpened = FALSE;
	static const ULONG pairs[] = {ID_PREF, ID_WBHD};


	d1(KPrintF(__FILE__ "/%s/%ld: START filename=<%s>\n", __FUNC__, __LINE__, filename));

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
		d1(KPrintF(__FILE__ "/%s/%ld: error=%ld\n", __FUNC__, __LINE__, error));
		if (RETURN_OK != error)
			break;

		IffOpened = TRUE;

		error = CollectionChunks( iff, (LONG*) pairs, Sizeof(pairs) / 2);
		d1(KPrintF(__FILE__ "/%s/%ld: error=%ld\n", __FUNC__, __LINE__, error));
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
					HandleWBHD(ci->ci_Data, ci->ci_Size, HiddenDevsList);
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
			Close((BPTR) iff->iff_Stream );
		FreeIFF( iff );
		}

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));

	return TRUE;
}

//-----------------------------------------------------------------

static void HandleWBHD(CONST_STRPTR data, size_t length, struct List *HiddenDevsList)
{
	d1(KPrintF(__FILE__ "/%s/%ld: data=<%s>  length=%lu\n", __FUNC__, __LINE__, data, length));

	AddHiddenDevice(data, HiddenDevsList);
}

//-----------------------------------------------------------------

// Traverse DosList for all devices, and build list of hideable devices
static void FillHiddenDevicesList(void)
{
	ULONG LockDosListFlags = LDF_DEVICES | LDF_READ;
	struct HiddenDevice *hd;
	struct DosList *dl;
	struct InfoData *id;
	struct List WBHiddenDevList;

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	NewList(&WBHiddenDevList);
	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	HiddenDevicesCleanup();

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	ReadWorkbenchPrefs(WBPREFSNAME, &WBHiddenDevList);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	id = MyAllocVecPooled(sizeof(struct InfoData));
	d1(KPrintF(__FILE__ "/%s/%ld: id=%08lx\n", __FUNC__, __LINE__, id));
	if (NULL == id)
		{
		d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
		return;
		}

	dl = LockDosList(LockDosListFlags);

	while (dl = NextDosEntry(dl, LockDosListFlags))
		{
		AddHiddenDeviceFromDosList(dl, id, FALSE, &WBHiddenDevList);
		}

	UnLockDosList(LockDosListFlags);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// Now walk through the list of devices marked as hidden,
	// and add all entries.

	dl = LockDosList(LockDosListFlags);

	for (hd = (struct HiddenDevice *) WBHiddenDevList.lh_Head;
		hd != (struct HiddenDevice *) &WBHiddenDevList.lh_Tail;
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
			AddHiddenDeviceFromDosList(dlFound, id, TRUE, &WBHiddenDevList);
			}
		else
			{
			AddHiddenDevVol(hd->hd_Name, "");
			}
		}

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	MyFreeVecPooled(id);
	UnLockDosList(LockDosListFlags);

	while ((hd = (struct HiddenDevice *) RemHead(&WBHiddenDevList)))
		{
		MyFreeVecPooled(hd);
		}

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
}

//-----------------------------------------------------------------

static void AddHiddenDeviceFromDosList(struct DosList *dl, struct InfoData *id,
	BOOL Hidden, struct List *HiddenDevsList)
{
	char DevName[128];
	char VolName[128];
	struct DosList *VolumeNode;
	BOOL InfoDataValid = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	strcpy(VolName, "");

	if (dl->dol_Task &&
		DoPkt(dl->dol_Task, ACTION_DISK_INFO,
			(IPTR)MKBADDR(id),
			0, 0, 0, 0))
		{
		InfoDataValid = TRUE;
		}

	if (!Hidden && !InfoDataValid)
		{
		d1(KPrintF(__FILE__ "/%s/%ld: END  Hidden=%ld  InfoDataValid=%ld\n", __FUNC__, __LINE__, Hidden, InfoDataValid));
		return;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: ACTION_DISK_INFO Success\n", __FUNC__, __LINE__));

	BtoCString(dl->dol_Name, DevName, sizeof(DevName));

	d1(KPrintF(__FILE__ "/%s/%ld: DevName=<%s>\n", __FUNC__, __LINE__, DevName));

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

	d1(KPrintF(__FILE__ "/%s/%ld: VolName=<%s>\n", __FUNC__, __LINE__, VolName));

	// add only visible devices here
	if (Hidden)
		{
		AddHiddenDevVol(DevName, VolName);
		}

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
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
static BOOL AddHiddenDevice(CONST_STRPTR Name, struct List *HiddenDevsList)
{
	struct HiddenDevice *hd;
	BOOL Success = FALSE;

	hd = MyAllocVecPooled(sizeof(struct HiddenDevice) + 1 + strlen(Name));
	if (hd)
		{
		strcpy(hd->hd_Name, Name);

		// make sure device name is terminated with exactly one ":"
		StripTrailingColon(hd->hd_Name);
		strcat(hd->hd_Name, ":");

		AddHead(HiddenDevsList, &hd->hd_Node);
		Success = TRUE;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: Name=<%s>  hd=%08lx\n", __FUNC__, __LINE__, Name, hd));

	return Success;
}

//-----------------------------------------------------------------

// search for entry in hidden devices list
static BOOL FindHiddenDevice(CONST_STRPTR DevName, CONST_STRPTR VolName)
{
	const struct HiddenDevVol *hdv;
	BOOL Found = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: DevName=<%s>  VolName=<%s>\n", __FUNC__, __LINE__, DevName, VolName));

	for (hdv = (struct HiddenDevVol *) HiddenDevicesList.lh_Head;
		hdv->hdv_Node.ln_Succ;
		hdv = (struct HiddenDevVol *) hdv->hdv_Node.ln_Succ)
		{
		d1(KPrintF("%s/%ld:  hdv=%08lx  DevName=%08lx  VolName=%08lx\n", __FUNC__, __LINE__, hdv, hdv->hdv_DeviceName, hdv->hdv_VolumeName));
		d1(KPrintF(__FILE__ "/%s/%ld: hdv=%08lx  ln_Succ=%08lx\n", __FUNC__, __LINE__, hdv, hdv->hdv_Node.ln_Succ));
		if (0 == Stricmp(DevName, hdv->hdv_DeviceName)
			|| 0 == Stricmp(VolName, hdv->hdv_VolumeName))
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

static APTR MyAllocVecPooled(size_t Size)
{
	APTR ptr;

	if (MemPool)
		{
		ObtainSemaphore(&PubMemPoolSemaphore);
		ptr = AllocPooled(MemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&PubMemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(kprintf("%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FUNC__, __LINE__, MemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%ld:  MemPool=%08lx  Size=%lu\n", __FUNC__, __LINE__, MemPool, Size));

	return NULL;
}


static void MyFreeVecPooled(APTR mem)
{
	d1(kprintf("%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FUNC__, __LINE__, MemPool, mem));
	if (MemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		ObtainSemaphore(&PubMemPoolSemaphore);
		FreePooled(MemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&PubMemPoolSemaphore);
		}
}

//-----------------------------------------------------------------

static void AddHiddenDevVol(CONST_STRPTR DevName, CONST_STRPTR VolName)
{
	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	ObtainSemaphore(&HiddenDevicesSema);

	if (!FindHiddenDevice(DevName, VolName))
		{
		struct HiddenDevVol *hdv;

		hdv = MyAllocVecPooled(sizeof(struct HiddenDevVol));
		if (hdv)
			{
			d1(KPrintF("%s/%ld:  hdv=%08lx  DevName=<%s>  VolName=<%s>\n", __FUNC__, __LINE__, hdv, DevName, VolName));

			hdv->hdv_DeviceName = MyAllocVecPooled(1 + strlen(DevName));
			if (hdv->hdv_DeviceName)
				strcpy(hdv->hdv_DeviceName, DevName);

			hdv->hdv_VolumeName = MyAllocVecPooled(1 + strlen(VolName));
			if (hdv->hdv_VolumeName)
				strcpy(hdv->hdv_VolumeName, VolName);

			d1(KPrintF("%s/%ld:  hdv=%08lx  DevName=%08lx  VolName=%08lx\n", __FUNC__, __LINE__, hdv, hdv->hdv_DeviceName, hdv->hdv_VolumeName));

			AddTail(&HiddenDevicesList, &hdv->hdv_Node);
			}
		}

	ReleaseSemaphore(&HiddenDevicesSema);

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
}

//-----------------------------------------------------------------

static BOOL StartWBPrefsProcess(void)
{
	STATIC_PATCHFUNC(WBPrefsProcess)

	FillHiddenDevicesList();

	// CreateNewProc()
	WBPrefsProc = CreateNewProcTags(NP_Name, (ULONG) "Scalos_DeviceFilter_WBPrefs",
			NP_Priority, 0,
			NP_Entry, (ULONG) PATCH_NEWFUNC(WBPrefsProcess),
			NP_StackSize, 32768,
			TAG_END);
	d1(KPrintF(__FILE__ "/%s/%ld: WBPrefsProc=%08lx\n", __FUNC__, __LINE__, WBPrefsProc));
	if (WBPrefsProc == NULL)
		{
		return FALSE;
		}

	return TRUE;
}

//----------------------------------------------------------------------------

static SAVEDS(int) WBPrefsProcess(void)
{
	ULONG mySignalBit;
	struct NotifyRequest nr;
	BOOL NotifyStarted = FALSE;

	d1(KPrintF(__FUNC__ "/%ld START\n", __LINE__);)

	do	{
		mySignalBit = AllocSignal(-1);
		if (-1 == mySignalBit)
			break;

		d1(KPrintF(__FILE__ "/%s/%ld: mySignalBit=%ld\n", __FUNC__, __LINE__, mySignalBit));

		memset(&nr, 0, sizeof(nr));
		nr.nr_Name = (STRPTR) WBPREFSNAME;
		nr.nr_Flags = NRF_SEND_SIGNAL;
		nr.nr_stuff.nr_Signal.nr_Task = FindTask(NULL);
		nr.nr_stuff.nr_Signal.nr_SignalNum = mySignalBit;

		if (!StartNotify(&nr))
			break;

		d1(KPrintF(__FILE__ "/%s/%ld: StartNotify succeeded\n", __FUNC__, __LINE__));
		NotifyStarted = TRUE;

		while (1)
			{
			ULONG Signals = Wait((1UL << mySignalBit) | SIGF_ABORT);

			d1(KPrintF(__FILE__ "/%s/%ld: Signals=%08lx\n", __FUNC__, __LINE__, Signals));

			if (Signals & SIGF_ABORT)
				break;

			if (Signals & (1UL << mySignalBit))
				FillHiddenDevicesList();
			}

		} while (0);

	if (NotifyStarted)
		EndNotify(&nr);
	if (-1 != mySignalBit)
		FreeSignal(mySignalBit);

	d1(KPrintF(__FUNC__ "/%ld END\n", __LINE__);)

	return 0;
}

//----------------------------------------------------------------------------



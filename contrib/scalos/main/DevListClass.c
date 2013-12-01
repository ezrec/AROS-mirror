// DevListClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

struct DevListClassInstance
	{
	struct MsgPort *dlci_ReplyPort;			// DosPacket Replyport
	struct List dlci_DleList;
	struct ScaDeviceIcon *dlci_DevIconList;
	};

enum DevListState 
	{ 
	DLE_Initial,			// dle has just been created
	DLE_WaitInfo, 			// initial SendPkt() is pending, waiting for reply
	DLE_WaitInfo2,			// waiting for SendPkt() reply
	DLE_InfoFinished, 		// SendPkt() has replied, dle_InfoData is valid
	};

struct DevListEntry
	{
	struct Node dle_Node;
	struct InfoData	*dle_InfoData;		// Infodata from dos.library/Info()
	LONG dle_Type;				// type of entry, see DLT
	struct DosPacket *dle_DosPacket;
	struct MsgPort *dle_Handler;		// MessagePort of the HandlerTask
	STRPTR dle_DeviceName;			// Copy of device name (cannot be NULL)
	STRPTR dle_VolumeName;			// Copy of volume name or NULL
	enum DevListState dle_State;
	ULONG dle_WaitCount;
	BOOL dle_MayBeRemoved;
	BOOL dle_IconCreated;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) DevListClass_Dispatcher(Class *cl, Object *o, Msg msg);
static ULONG DevListClass_New(Class *cl, Object *o, Msg msg);
static ULONG DevListClass_Dispose(Class *cl, Object *o, Msg msg);
static ULONG DevListClass_Generate(Class *cl, Object *o, Msg msg);
static ULONG DevListClass_FreeDevNode(Class *cl, Object *o, Msg msg);
static ULONG DevListClass_Filter(Class *cl, Object *o, Msg msg);
static STRPTR AllocCopyBString(BPTR bString);
static STRPTR AllocCopyAString(CONST_STRPTR String);
static void CheckAddDosListEntry(struct DevListClassInstance *inst, struct DosList *dlist);
static void HandleRepliedPackets(Object *o, struct DevListClassInstance *inst, struct ScaDeviceIcon **diList);
static struct DevListEntry *CreateDevListEntry(const struct DosList *dlist);
static void DeleteDevListEntry(struct DevListEntry *dle);
static void CreateDeviceIcons(struct DevListClassInstance *inst, struct ScaDeviceIcon **diList);
static void RemoveObsoleteEntries(struct DevListClassInstance *inst);
static struct ScaDeviceIcon *CreateIconFromDle(struct DevListEntry *dle,
	struct ScaDeviceIcon **diList, struct DosList *origDlist);
static void SendInfoPacket(struct DevListClassInstance *inst, struct DevListEntry *dle);
static BOOL VerifyDosListEntry(struct DosList *dlStart, const struct DosList *dlTest);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------


struct ScalosClass *initDevListClass(const struct PluginClass *plug)
{
	struct ScalosClass *DevListClass;

	DevListClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct DevListClassInstance),
			NULL);

	if (DevListClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(DevListClass->sccl_class->cl_Dispatcher, DevListClass_Dispatcher);
		}

	return DevListClass;
}


static SAVEDS(ULONG) DevListClass_Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = DevListClass_New(cl, o, msg);
		break;

	case OM_DISPOSE:
		Result = DevListClass_Dispose(cl, o, msg);
		break;

	case SCCM_DeviceList_Generate:
		Result = DevListClass_Generate(cl, o, msg);
		break;

	case SCCM_DeviceList_FreeDevNode:
		Result = DevListClass_FreeDevNode(cl, o, msg);
		break;

	case SCCM_DeviceList_Filter:
		Result = DevListClass_Filter(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}


static ULONG DevListClass_New(Class *cl, Object *o, Msg msg)
{
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	o = (Object *) DoSuperMethodA(cl, o, msg);

	if (o)
		{
		struct DevListClassInstance *inst = INST_DATA(cl, o);

		d1(kprintf("%s/%s/%ld: o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

		inst->dlci_DevIconList = NULL;
		inst->dlci_ReplyPort = CreateMsgPort();
		NewList(&inst->dlci_DleList);

		if (NULL == inst->dlci_ReplyPort)
			{
			DoMethod(o, OM_DISPOSE);
			o = NULL;
			}
		}

	return (ULONG) o;
}


static ULONG DevListClass_Dispose(Class *cl, Object *o, Msg msg)
{
	struct DevListClassInstance *inst = INST_DATA(cl, o);
	struct DevListEntry *dle;

	d1(kprintf("%s/%s/%ld\n", __FILE__, __FUNC__, __LINE__));

	while ((dle = (struct DevListEntry *) RemHead(&inst->dlci_DleList)))
		{
		switch (dle->dle_State)
			{
		case DLE_WaitInfo:
		case DLE_WaitInfo2:
#ifndef __amigaos4__
			AbortPkt(dle->dle_Handler, dle->dle_DosPacket);
#endif
			break;
		default:
			break;
			}
		DeleteDevListEntry(dle);
		}

	if (inst->dlci_ReplyPort)
		{
		DeleteMsgPort(inst->dlci_ReplyPort);
		inst->dlci_ReplyPort = NULL;
		}

	return DoSuperMethodA(cl, o, msg);
}


static ULONG DevListClass_Generate(Class *cl, Object *o, Msg msg)
{
	struct DevListClassInstance *inst = INST_DATA(cl, o);
	struct msg_Generate *mge = (struct msg_Generate *) msg;
	struct DosList *dlist, *origDlist;
	struct DevListEntry *dle;

	for (dle = (struct DevListEntry *) inst->dlci_DleList.lh_Head;
		dle != (struct DevListEntry *) &inst->dlci_DleList.lh_Tail;
		dle = (struct DevListEntry *) dle->dle_Node.ln_Succ)
		{
		switch (dle->dle_State)
			{
		case DLE_WaitInfo:
		case DLE_WaitInfo2:
			dle->dle_WaitCount++;
			break;
		case DLE_Initial:
		case DLE_InfoFinished:
			dle->dle_MayBeRemoved = TRUE;
			break;
			}
		dle->dle_IconCreated = FALSE;
		}

	// now handle all device icons whose handlers have finished processing since last call
	HandleRepliedPackets(o, inst, mge->mge_DevIconList);

	origDlist = AttemptLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_READ);

	// work around bug in AttemptLockDosList()
	if ((struct DosList *) 0x00000001 == origDlist)
		origDlist = NULL;

	if (origDlist)
		{
		for (dlist=origDlist; dlist; )
			{
			dlist = NextDosEntry(dlist, LDF_VOLUMES);

			CheckAddDosListEntry(inst, dlist);
			}
		for (dlist=origDlist; dlist; )
			{
			dlist = NextDosEntry(dlist, LDF_DEVICES);

			CheckAddDosListEntry(inst, dlist);
			}

		RemoveObsoleteEntries(inst);

		for (dle = (struct DevListEntry *) inst->dlci_DleList.lh_Head;
			dle != (struct DevListEntry *) &inst->dlci_DleList.lh_Tail;
			dle = (struct DevListEntry *) dle->dle_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: dle=%08lx  dle_DeviceName=%08lx <%s>  dle_VolumeName=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
				dle, dle->dle_DeviceName, dle->dle_DeviceName ? dle->dle_DeviceName : (STRPTR) "",\
				dle->dle_VolumeName, dle->dle_VolumeName ? dle->dle_VolumeName : (STRPTR) ""));

			if (dle->dle_DeviceName)
				{
				switch (dle->dle_State)
					{
				case DLE_Initial:
				case DLE_InfoFinished:
					SendInfoPacket(inst, dle);
					break;
				default:
					break;
					}
				}
			}

		UnLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_READ);
		}

	// now handle all device icons whose handlers were able to reply in time
	HandleRepliedPackets(o, inst, mge->mge_DevIconList);

	// create ScaDeviceIcon's from inst->dlci_DleList
	CreateDeviceIcons(inst, mge->mge_DevIconList);

	DoMethod(o, SCCM_DeviceList_Filter, mge->mge_DevIconList);

	return 0;
}


static ULONG DevListClass_FreeDevNode(Class *cl, Object *o, Msg msg)
{
	struct msg_FreeDevNode *mfd = (struct msg_FreeDevNode *) msg;

	if (mfd->mfd_DevIcon->di_NotifyReq.nr_Name)
		{
		ScalosEndNotify(&mfd->mfd_DevIcon->di_NotifyReq);
		memset(&mfd->mfd_DevIcon->di_NotifyReq, 0, sizeof(mfd->mfd_DevIcon->di_NotifyReq));
		}
	if (mfd->mfd_DevIcon->di_DiskIconName)
		{
		FreePathBuffer(mfd->mfd_DevIcon->di_DiskIconName);
		mfd->mfd_DevIcon->di_DiskIconName = NULL;
		}
	if (mfd->mfd_DevIcon->di_Volume)
		ScalosFree(mfd->mfd_DevIcon->di_Volume);
	if (mfd->mfd_DevIcon->di_Device)
		ScalosFree(mfd->mfd_DevIcon->di_Device);

	return 0;
}


static ULONG DevListClass_Filter(Class *cl, Object *o, Msg msg)
{
	struct msg_Filter *mfi = (struct msg_Filter *) msg;
	struct ScaDeviceIcon *di;

	for (di=*mfi->mfi_DevIconList; di; di = (struct ScaDeviceIcon *) di->di_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: <%s>  InfoData=%08lx  diskType=%08lx  Flags=%04lx\n", \
			__FILE__, __FUNC__, __LINE__, di->di_Device, di->di_Info, di->di_Info->id_DiskType, di->di_Flags));

		if ((NULL == di->di_Volume
		     && ID_DOS_DISK == (di->di_Info->id_DiskType & 0xffffff00)
		    )
		    || (MAKE_ID('F','T','X','T') == di->di_Info->id_DiskType)
		    || (strcmp(di->di_Device, "ENV:") == 0)
		)
			{
			di->di_Flags |= DIBF_DontDisplay;
			}

		d1(kprintf("%s/%s/%ld: di=%08lx  di_Device=%08lx <%s>  di_Volume=%08lx <%s>  Flags=%04lx\n", __FILE__, __FUNC__, __LINE__, \
			di, di->di_Device, di->di_Device ? di->di_Device : (STRPTR) "",\
			di->di_Volume, di->di_Volume ? di->di_Volume : (STRPTR) "",\
			di->di_Flags));
		}

	return 1;
}


static STRPTR AllocCopyBString(BPTR bString)
{
#ifdef __AROS__
	// AROS needs special handling because it uses NULL-terminated
	// strings on some platforms.
	CONST_STRPTR Src = AROS_BSTR_ADDR(bString);
	size_t Len = AROS_BSTR_strlen(bString);
	STRPTR Name, lp;

	Name = lp = ScalosAlloc(Len + 3);
	if (NULL == Name)
		return NULL;
#else
	CONST_STRPTR Src = BADDR(bString);
	size_t Len = *Src;
	STRPTR Name, lp;

	Name = lp = ScalosAlloc(Len + 3);
	if (NULL == Name)
		return NULL;

	Len = *Src++;
#endif

	while (Len--)
		*lp++ = *Src++;

	*lp++ = ':';
	*lp = '\0';

	return Name;
}


static STRPTR AllocCopyAString(CONST_STRPTR Src)
{
	size_t Len;
	STRPTR Name;

	if (NULL == Src)
		return NULL;

	Len = strlen(Src);

	Name = ScalosAlloc(1 + Len);
	if (NULL == Name)
		return NULL;

	strcpy(Name, Src);

	return Name;
}


static void CheckAddDosListEntry(struct DevListClassInstance *inst, struct DosList *dlist)
{
	if (dlist && dlist->dol_Task)
		{
		BOOL Found = FALSE;
		struct DevListEntry *dle;

		for (dle = (struct DevListEntry *) inst->dlci_DleList.lh_Head;
			dle != (struct DevListEntry *) &inst->dlci_DleList.lh_Tail;
			dle = (struct DevListEntry *) dle->dle_Node.ln_Succ)
			{
			if (dle->dle_Handler == dlist->dol_Task)
				{
				dle->dle_MayBeRemoved = FALSE;

				switch (dlist->dol_Type)
					{
				case DLT_VOLUME:
					if (dle->dle_VolumeName)
						ScalosFree(dle->dle_VolumeName);

					dle->dle_VolumeName = AllocCopyBString(dlist->dol_Name);
					break;
				case DLT_DEVICE:
					if (dle->dle_DeviceName)
						ScalosFree(dle->dle_DeviceName);

					dle->dle_DeviceName = AllocCopyBString(dlist->dol_Name);
					break;
					}
				Found = TRUE;

				d1(kprintf("%s/%s/%ld: dle=%08lx  <%s>  disktype=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, dle, dle->dle_DeviceName, dle->dle_InfoData->id_DiskType));
				break;
				}
			}
		if (!Found)
			{
			dle = CreateDevListEntry(dlist);
			if (dle)
				AddTail(&inst->dlci_DleList, &dle->dle_Node);
			}
		}
}


static void HandleRepliedPackets(Object *o, struct DevListClassInstance *inst, struct ScaDeviceIcon **diList)
{
	struct DosList *origDlist;

	origDlist = AttemptLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_READ);

	// work around bug in AttemptLockDosList()
	if ((struct DosList *) 0x00000001 == origDlist)
		origDlist = NULL;

	if (origDlist)
		{
		struct StandardPacket *pkt;

		do	{
			pkt = (struct StandardPacket *) GetMsg(inst->dlci_ReplyPort);
			if (pkt)
				{
				struct DevListEntry *dle;

				d1(kprintf("%s/%s/%ld: pkt=%08lx  Res1=%08lx\n", __FILE__, __FUNC__, __LINE__, pkt, pkt->sp_Pkt.dp_Res1));

				for (dle = (struct DevListEntry *) inst->dlci_DleList.lh_Head;
					dle != (struct DevListEntry *) &inst->dlci_DleList.lh_Tail;
					dle = (struct DevListEntry *) dle->dle_Node.ln_Succ)
					{
					if (pkt->sp_Pkt.dp_Arg1 == (IPTR) MKBADDR(dle->dle_InfoData))
						{
						d1(kprintf("%s/%s/%ld: di=%08lx  <%s>  disktype=%08lx\n", \
							__FILE__, __FUNC__, __LINE__, dle, dle->dle_DeviceName, dle->dle_InfoData->id_DiskType));

						if (!pkt->sp_Pkt.dp_Res1)
							dle->dle_InfoData->id_DiskType = ID_NO_DISK_PRESENT;

						dle->dle_State = DLE_InfoFinished;
						dle->dle_WaitCount = 0;

						CreateIconFromDle(dle, diList, origDlist);
						}
					}
				}
			} while (pkt);

		UnLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_READ);
		}
}


static struct DevListEntry *CreateDevListEntry(const struct DosList *dlist)
{
	struct DevListEntry *dle;

	dle = ScalosAlloc(sizeof(struct DevListEntry));
	if (dle)
		{
		dle->dle_Type = dlist->dol_Type;
		dle->dle_Handler = dlist->dol_Task;
		dle->dle_State = DLE_Initial;
		dle->dle_MayBeRemoved = FALSE;
		dle->dle_IconCreated = FALSE;
		dle->dle_WaitCount = 0;
		dle->dle_InfoData = ScalosAllocInfoData();
		dle->dle_InfoData->id_DiskType = ID_NO_DISK_PRESENT;
		dle->dle_DosPacket = NULL;
		dle->dle_DeviceName = NULL;
		dle->dle_VolumeName = NULL;
		
		switch (dlist->dol_Type)
			{
		case DLT_VOLUME:
			dle->dle_VolumeName = AllocCopyBString(dlist->dol_Name);
			break;
		case DLT_DEVICE:
			dle->dle_DeviceName = AllocCopyBString(dlist->dol_Name);
			break;
			}

		dle->dle_DosPacket = AllocDosObject(DOS_STDPKT, NULL);

		if ((NULL == dle->dle_InfoData)
			|| (NULL == dle->dle_VolumeName && NULL == dle->dle_DeviceName)
			|| (NULL == dle->dle_DosPacket))
			{
			DeleteDevListEntry(dle);
			return NULL;
			}
		}

	return dle;
}


static void DeleteDevListEntry(struct DevListEntry *dle)
{
	if (dle)
		{
		ScalosFreeInfoData( &dle->dle_InfoData );
		if (dle->dle_DeviceName)
			{
			ScalosFree(dle->dle_DeviceName);
			dle->dle_DeviceName = NULL;
			}
		if (dle->dle_VolumeName)
			{
			ScalosFree(dle->dle_VolumeName);
			dle->dle_VolumeName = NULL;
			}
		if (dle->dle_DosPacket)
			{
			FreeDosObject(DOS_STDPKT, dle->dle_DosPacket);
			dle->dle_DosPacket = NULL;
			}

		ScalosFree(dle);
		}
}


static void CreateDeviceIcons(struct DevListClassInstance *inst, struct ScaDeviceIcon **diList)
{
	struct DosList *origDlist;

	origDlist = AttemptLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_READ);

	// work around bug in AttemptLockDosList()
	if ((struct DosList *) 0x00000001 == origDlist)
		origDlist = NULL;

	if (origDlist)
		{
		struct DevListEntry *dle;

		d1(kprintf("%s/%s/%ld\n", __FILE__, __FUNC__, __LINE__));

		for (dle = (struct DevListEntry *) inst->dlci_DleList.lh_Head;
			dle != (struct DevListEntry *) &inst->dlci_DleList.lh_Tail;
			dle = (struct DevListEntry *) dle->dle_Node.ln_Succ)
			{
			switch (dle->dle_State)
				{
			case DLE_InfoFinished:
				CreateIconFromDle(dle, diList, origDlist);
				break;
			case DLE_WaitInfo2:
				if (dle->dle_WaitCount < 2)
					CreateIconFromDle(dle, diList, origDlist);
				break;
			default:
				break;
				}
			}
		UnLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_READ);
		}
}


// Remove entries from dlci_DleList which are no longer in DosList
static void RemoveObsoleteEntries(struct DevListClassInstance *inst)
{
	struct DevListEntry *dle, *dleNext;

	d1(kprintf("%s/%s/%ld\n", __FILE__, __FUNC__, __LINE__));

	for (dle = (struct DevListEntry *) inst->dlci_DleList.lh_Head;
		dle != (struct DevListEntry *) &inst->dlci_DleList.lh_Tail;
		dle = dleNext)
		{
		dleNext = (struct DevListEntry *) dle->dle_Node.ln_Succ;

		d1(kprintf("%s/%s/%ld: dle=%08lx  <%s>  state=%ld  MayBeRemoved=%ld\n", \
			__FILE__, __FUNC__, __LINE__, dle, dle->dle_DeviceName, dle->dle_State, dle->dle_MayBeRemoved));

		if (dle->dle_MayBeRemoved)
			{
			d1(kprintf("%s/%s/%ld: di=%08lx  Device=<%s>  Volume=<%s> DiskType=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, dle, dle->dle_DeviceName, dle->dle_VolumeName, dle->dle_InfoData->id_DiskType));

			Remove(&dle->dle_Node);
			DeleteDevListEntry(dle);
			}
		}
}


static struct ScaDeviceIcon *CreateIconFromDle(struct DevListEntry *dle,
	struct ScaDeviceIcon **diList, struct DosList *origDlist)
{
	struct ScaDeviceIcon *di = NULL;

	if (dle->dle_DeviceName && !dle->dle_IconCreated &&
		ID_NO_DISK_PRESENT != dle->dle_InfoData->id_DiskType)
		{
		for (di=*diList; di; di = (struct ScaDeviceIcon *) di->di_Node.mln_Succ)
			{
			if (di->di_Handler == dle->dle_Handler)
				dle->dle_IconCreated = TRUE;
			}

		if (!dle->dle_IconCreated)
			{
			di = (struct ScaDeviceIcon *) SCA_AllocStdNode((struct ScalosNodeList *) diList, NTYP_DeviceIcon);
			if (di)
				{
				struct DosList *dlist;

				di->di_Info = BADDR(MKBADDR(di->di_InfoBuf + 3));
				*di->di_Info = *dle->dle_InfoData;
				di->di_Handler = dle->dle_Handler;

				dlist = BADDR(di->di_Info->id_VolumeNode);
				d1(kprintf("%s/%s/%ld: dlist=%08lx\n", __FILE__, __FUNC__, __LINE__, dlist));

				if (VerifyDosListEntry(origDlist, dlist) && dlist->dol_Name)
					{
					// Update volume name from DosList entry pointed to by InfoData
					d1(kprintf("%s/%s/%ld: dol_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, dlist->dol_Name));
					if (dle->dle_VolumeName)
						ScalosFree(dle->dle_VolumeName);

					dle->dle_VolumeName = AllocCopyBString(dlist->dol_Name);
					}

				d1(kprintf("%s/%s/%ld: dlist=%08lx\n", __FILE__, __FUNC__, __LINE__, dlist));
				di->di_Volume = AllocCopyAString(dle->dle_VolumeName);
				d1(kprintf("%s/%s/%ld: dlist=%08lx\n", __FILE__, __FUNC__, __LINE__, dlist));
				di->di_Device = AllocCopyAString(dle->dle_DeviceName);

				d1(kprintf("%s/%s/%ld: di=%08lx  Device=<%s>  Volume=<%s> DiskType=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, dle, dle->dle_DeviceName, dle->dle_VolumeName, dle->dle_InfoData->id_DiskType));

				if (DLE_WaitInfo2 == dle->dle_State)
					di->di_Flags |= DIBF_InfoPending;

				dle->dle_IconCreated = TRUE;
				}
			}
		}

	return di;
}


static void SendInfoPacket(struct DevListClassInstance *inst, struct DevListEntry *dle)
{
	struct DosList *origDlist = NULL;
	STRPTR DeviceName;

	d1(kprintf("%s/%s/%ld: dle=%08lx  <%s>  packet=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, dle, dle->dle_DeviceName, dle->dle_DosPacket));

	do	{
		struct DosList *dList;

		DeviceName = AllocCopyString(dle->dle_DeviceName);
		if (NULL == DeviceName)
			break;
		StripTrailingColon(DeviceName);

		origDlist = AttemptLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_READ);
		if (NULL == origDlist)
			break;

		dList = FindDosEntry(origDlist, DeviceName, LDF_VOLUMES | LDF_DEVICES);
		if (NULL == dList)
			{
			dle->dle_MayBeRemoved = TRUE;
			break;
			}

		dle->dle_DosPacket->dp_Port = inst->dlci_ReplyPort;
		dle->dle_DosPacket->dp_Type = ACTION_DISK_INFO;
		dle->dle_DosPacket->dp_Res1 = dle->dle_DosPacket->dp_Res2 = 0;
		dle->dle_DosPacket->dp_Arg1 = (SIPTR) MKBADDR(dle->dle_InfoData);

		SendPkt(dle->dle_DosPacket, dle->dle_Handler, inst->dlci_ReplyPort);

		if (DLE_InfoFinished == dle->dle_State)
			dle->dle_State = DLE_WaitInfo2;
		else
			dle->dle_State = DLE_WaitInfo;
		} while (0);

	if (origDlist)
		UnLockDosList(LDF_VOLUMES | LDF_DEVICES | LDF_READ);

	if (DeviceName)
		FreeCopyString(DeviceName);
}


static BOOL VerifyDosListEntry(struct DosList *dlStart, const struct DosList *dlTest)
{
	while (dlStart)
		{
		if (dlTest == dlStart)
			return TRUE;

		dlStart = NextDosEntry(dlStart, LDF_VOLUMES | LDF_DEVICES | LDF_READ);
		}

	d1(KPrintF("%s/%s/%ld: not found: dlTest=%08lx\n", __FILE__, __FUNC__, __LINE__, dlTest));

	return FALSE;	// dlTest not found!
}




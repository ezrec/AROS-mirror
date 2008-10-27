/*
    Copyright (C) 2006 by Michal Schulz
    $Id: hidclass.c 29012 2008-07-12 20:21:28Z schulz $

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this program; if not, write to the
    Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define DEBUG 1

#include <aros/debug.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>

#include <dos/dos.h>
#include <dos/dosextens.h>

#include <usb/usb.h>
#include <usb/usb_core.h>
#include <usb/hid.h>
#include <stdio.h>

#include "storage.h"

#include <proto/oop.h>
#include <proto/dos.h>

/*
 * Initialize the USBStorage instance
 */
OOP_Object *METHOD(Storage, Root, New)
{
    D(bug("[MSS] Storage::New()\n"));

    /* Prevent the class from being expunged */
    BASE(cl->UserData)->LibNode.lib_OpenCnt++;

    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);
    if (o)
    {
    	StorageData *mss = OOP_INST_DATA(cl, o);
        usb_config_descriptor_t cdesc;
        intptr_t iface;
    	int i;

        mss->sd = SD(cl);
        mss->o = o;

        /* Get the interface number */
        OOP_GetAttr(o, aHidd_USBDevice_Interface, &iface);

        /* Configure MSS device */
        HIDD_USBDevice_Configure(o, 0);

        /* Get device and config descriptors */
        D(bug("[MSS] Getting device descriptor\n"));
        HIDD_USBDevice_GetDeviceDescriptor(o, &mss->ddesc);
        D(bug("[MSS] Getting initial config descriptor\n"));
        HIDD_USBDevice_GetConfigDescriptor(o, 0, &cdesc);
        /* How many LUNs this device supports? */
        mss->maxLUN = HIDD_USBStorage_GetMaxLUN(o);
        D(bug("[MSS] GetMaxLUN returns %d\n", mss->maxLUN));

        if (AROS_LE2WORD(cdesc.wTotalLength))
        	mss->cdesc = AllocVecPooled(SD(cl)->MemPool, AROS_LE2WORD(cdesc.wTotalLength));

        if (mss->cdesc)
        {
        	D(bug("[MSS] Getting config descriptor of size %d\n", AROS_LE2WORD(cdesc.wTotalLength)));
			HIDD_USBDevice_GetDescriptor(o, UDESC_CONFIG, 0, AROS_LE2WORD(cdesc.wTotalLength), mss->cdesc);

			D(bug("[MSS] Getting descriptor of interface %d\n", iface));
			mss->iface = HIDD_USBDevice_GetInterface(o, iface);

			D(bug("[MSS] Interface has %d endpoints\n", mss->iface->bNumEndpoints));

			/*
			 * Iterate through the possible endpoints and look for IN and OUT endpoints for BULK
			 * transfers.
			 */
			for (i=0; i < mss->iface->bNumEndpoints; i++)
			{
				usb_endpoint_descriptor_t *ep;
				ep = HIDD_USBDevice_GetEndpoint(o, iface, i);

				D(bug("[MSS] endpoint %d: addr %02x, interval %02x, length %02x attr %02x maxpacket %04x\n", i,
						ep->bEndpointAddress,
						ep->bInterval,
						ep->bLength,
						ep->bmAttributes,
						AROS_LE2WORD(ep->wMaxPacketSize)));

				/* Use only BULK endpoints */
				if (UE_GET_XFERTYPE(ep->bmAttributes) == UE_BULK)
				{
					if (!mss->pipe_in && UE_GET_DIR(ep->bEndpointAddress) == UE_DIR_IN)
					{
						D(bug("[MSS] IN endpoint found\n"));
						mss->ep_in = ep;
						mss->pipe_in = HIDD_USBDevice_CreatePipe(o, PIPE_Bulk, ep->bEndpointAddress, 0, AROS_LE2WORD(ep->wMaxPacketSize), 10000);
					}
					else if (!mss->pipe_out && UE_GET_DIR(ep->bEndpointAddress) == UE_DIR_OUT)
					{
						D(bug("[MSS] OUT endpoint found\n"));
						mss->ep_out = ep;
						mss->pipe_out = HIDD_USBDevice_CreatePipe(o, PIPE_Bulk, ep->bEndpointAddress, 0, AROS_LE2WORD(ep->wMaxPacketSize), 10000);
					}
				}
			}

			/* Pipes are there. Let's start the handler task */
			if (mss->pipe_in && mss->pipe_out)
			{
				struct Task *t;
				struct MemList *ml;
				uint32_t unit_number;

				/*
				 * Unit number allocation.
				 *
				 * The unit number allocation proposed here is a very simple variant. It uses the
				 * counter which increases upon every query. The better approach would be to cache
				 * the assignment based on iProduct iManufacturer and iSerial strings.
				 */
				ObtainSemaphore(&SD(cl)->Lock);
				unit_number = SD(cl)->unitNum;
				SD(cl)->unitNum += 16;
				ReleaseSemaphore(&SD(cl)->Lock);

				/*
				 * For each LUN create a separate task. The unit number will be created as combination of
				 * unitNum and the LUN: unit 0x00 is Unit 0 LUN 0, 0x12 is Unit 1 LUN 2.
				 */
				for (i=0; i <= HIDD_USBStorage_GetMaxLUN(o); i++)
				{
					struct TagItem tags[] = {
										{ TASKTAG_ARG1,   (IPTR)cl },
										{ TASKTAG_ARG2,   (IPTR)o },
										{ TASKTAG_ARG3,	  i },
										{ TASKTAG_ARG4,	  (IPTR)FindTask(NULL) },
										{ TAG_DONE,       0UL }};

					t = AllocMem(sizeof(struct Task), MEMF_PUBLIC|MEMF_CLEAR);
					ml = AllocMem(sizeof(struct MemList) + 2*sizeof(struct MemEntry), MEMF_PUBLIC|MEMF_CLEAR);

					if (t && ml)
					{
						uint8_t *sp = AllocMem(10240, MEMF_PUBLIC|MEMF_CLEAR);
						uint8_t *name = AllocMem(16, MEMF_PUBLIC|MEMF_CLEAR);

						/* Create individual name */
						snprintf(name, 15, "USB MSS %02x.%x",unit_number >> 4, i);

						t->tc_SPLower = sp;
						t->tc_SPUpper = sp + 10240;
#if AROS_STACK_GROWS_DOWNWARDS
						t->tc_SPReg = (char *)t->tc_SPUpper - SP_OFFSET;
#else
						t->tc_SPReg = (char *)t->tc_SPLower + SP_OFFSET;
#endif

						ml->ml_NumEntries = 3;
						ml->ml_ME[0].me_Addr = t;
						ml->ml_ME[0].me_Length = sizeof(struct Task);
						ml->ml_ME[1].me_Addr = sp;
						ml->ml_ME[1].me_Length = 10240;
						ml->ml_ME[2].me_Addr = name;
						ml->ml_ME[2].me_Length = 16;

						NEWLIST(&t->tc_MemEntry);
						ADDHEAD(&t->tc_MemEntry, &ml->ml_Node);

						t->tc_Node.ln_Name = name;
						t->tc_Node.ln_Type = NT_TASK;
						t->tc_Node.ln_Pri = 20;     /* same priority as input.device */

						/* Add task. It will get back in touch soon */
						NewAddTask(t, StorageTask, NULL, &tags);
						/* Keep the initialization synchronous */
						Wait(SIGF_SINGLE);
						mss->handler[i] = t;
					}
				}
			}
        }
    }

    D(bug("[MSS] Storage::New() = %p\n", o));

    if (!o)
        BASE(cl->UserData)->LibNode.lib_OpenCnt--;

    return o;
}

struct pRoot_Dispose {
    OOP_MethodID        mID;
};

void METHOD(Storage, Root, Dispose)
{
    StorageData *mss = OOP_INST_DATA(cl, o);
    OOP_Object *drv = NULL;
    int i;
    struct Library *base = &BASE(cl->UserData)->LibNode;

    if (mss)
    {
    	for (i=0; i <= mss->maxLUN; i++)
    	{
    		if (mss->handler[i])
    	    	Signal(mss->handler[i], SIGBREAKF_CTRL_C);
    	}
    }

    OOP_GetAttr(o, aHidd_USBDevice_Bus, (intptr_t *)&drv);

    FreeVecPooled(SD(cl)->MemPool, mss->cdesc);

    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    base->lib_OpenCnt--;
}

BOOL METHOD(Storage, Hidd_USBStorage, Reset)
{
	USBDevice_Request req;
	intptr_t ifnr;

	OOP_GetAttr(o, aHidd_USBDevice_InterfaceNumber, &ifnr);

	req.bmRequestType = UT_WRITE_CLASS_INTERFACE;
	req.bRequest = 0xff;
	req.wValue = 0;
	req.wIndex = AROS_WORD2LE(ifnr);
	req.wLength = 0;

	HIDD_USBDevice_ControlMessage(o, NULL, &req, NULL, 0);
}

uint8_t METHOD(Storage, Hidd_USBStorage, GetMaxLUN)
{
	USBDevice_Request req;
	intptr_t ifnr;
	uint8_t maxlun;

	OOP_GetAttr(o, aHidd_USBDevice_InterfaceNumber, &ifnr);

	req.bmRequestType = UT_READ_CLASS_INTERFACE;
	req.bRequest = 0xfe;
	req.wValue = 0;
	req.wIndex = AROS_WORD2LE(ifnr);
	req.wLength = 1;

	if (HIDD_USBDevice_ControlMessage(o, NULL, &req, &maxlun, 1))
		return maxlun;
	else
		return 0xff;
}

uint32_t METHOD(Storage, Hidd_USBStorage, DirectSCSI)
{
	StorageData *mss = OOP_INST_DATA(cl, o);
	cbw_t cbw;
	csw_t csw;
	int i;
	uint32_t retval = 0;

	for (i=0; i < msg->cmdLen; i++)
		cbw.CBWCB[i] = msg->cmd[i];

	cbw.bCBWLUN = msg->lun;
	cbw.dCBWSignature = AROS_LONG2LE(CBW_SIGNATURE);
	cbw.dCBWTag = getTID(SD(cl));
	cbw.dCBWDataTransferLength = AROS_LONG2LE(msg->dataLen);
	cbw.bmCBWFlags = msg->read ? CBW_FLAGS_IN : CBW_FLAGS_OUT;
	cbw.bCBWCBLength = msg->cmdLen;

	ObtainSemaphore(&SD(cl)->Lock);

	D(bug("[MSS] DirectSCSI -> (%08x,%08x,%08x,%02x,%02x,%02x)\n",
			AROS_LONG2LE(cbw.dCBWSignature),
			cbw.dCBWTag,
			AROS_LONG2LE(cbw.dCBWDataTransferLength),
			cbw.bCBWLUN, cbw.bmCBWFlags, cbw.bCBWCBLength));

	if (HIDD_USBDevice_BulkTransfer(o, mss->pipe_out, &cbw, 31))
	{
		if (msg->dataLen)
		{
			if (msg->read)
				HIDD_USBDevice_BulkTransfer(o, mss->pipe_in, msg->data, msg->dataLen);
			else
				HIDD_USBDevice_BulkTransfer(o, mss->pipe_out, msg->data, msg->dataLen);
		}

		if (HIDD_USBDevice_BulkTransfer(o, mss->pipe_in, &csw, 13))
		{
			D(bug("[MSS] DirectSCSI <- (%08x,%08x,%08x,%02x)\n",
					AROS_LONG2LE(csw.dCSWSignature),
					csw.dCSWTag,
					AROS_LONG2LE(csw.dCSWDataResidue),
					csw.bCSWStatus));

			if (csw.dCSWSignature == AROS_LONG2LE(CSW_SIGNATURE))
			{
				if (csw.dCSWTag == cbw.dCBWTag)
				{
					retval = 1;
				}
			}
		}
	}

	ReleaseSemaphore(&SD(cl)->Lock);

	return retval;
}

BOOL METHOD(Storage, Hidd_USBStorage, Read)
{
	StorageData *mss = OOP_INST_DATA(cl, o);
	uint8_t cmd[10] = {0x28, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	if (msg->lun > mss->maxLUN)
		return FALSE;

	cmd[2] = msg->block >>24;
	cmd[3] = msg->block >>16;
	cmd[4] = msg->block >>8;
	cmd[5] = msg->block;

	cmd[7] = msg->count >> 8;
	cmd[8] = msg->count;

	if (msg->buffer)
		return HIDD_USBStorage_DirectSCSI(o, msg->lun, cmd, 10, msg->buffer, msg->count * mss->blocksize[msg->lun], 1);
}

BOOL METHOD(Storage, Hidd_USBStorage, Write)
{
	StorageData *mss = OOP_INST_DATA(cl, o);
	uint8_t cmd[10] = {0x2a, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	if (msg->lun > mss->maxLUN)
		return FALSE;

	cmd[2] = msg->block >>24;
	cmd[3] = msg->block >>16;
	cmd[4] = msg->block >>8;
	cmd[5] = msg->block;

	cmd[7] = msg->count >> 8;
	cmd[8] = msg->count;

	if (msg->buffer)
		return HIDD_USBStorage_DirectSCSI(o, msg->lun, cmd, 10, msg->buffer, msg->count * mss->blocksize[msg->lun], 0);

}

BOOL METHOD(Storage, Hidd_USBStorage, ReadCapacity)
{
	StorageData *mss = OOP_INST_DATA(cl, o);
	uint8_t cmd[10] = {0x25, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t capacity[8];
	BOOL retval = FALSE;

	if (msg->lun > mss->maxLUN)
		return FALSE;

	if (HIDD_USBStorage_DirectSCSI(o, msg->lun, cmd, 10, capacity, 8, 1))
	{
		mss->blocksize[msg->lun] = capacity[4] << 24 | capacity[5] << 16 | capacity[6] << 8 | capacity[7];

		if (msg->blockTotal)
			*msg->blockTotal = capacity[0] << 24 | capacity[1] << 16 | capacity[2] << 8 | capacity[3];
		if (msg->blockSize)
			*msg->blockSize = mss->blocksize[msg->lun];

		retval = TRUE;
	}

	return retval;
}

static usb_interface_descriptor_t *find_idesc(usb_config_descriptor_t *cd, int ifaceidx, int altidx)
{
    char *p = (char *)cd;
    char *end = p + AROS_LE2WORD(cd->wTotalLength);
    usb_interface_descriptor_t *d;
    int curidx, lastidx, curaidx = 0;

    for (curidx = lastidx = -1; p < end; ) {
        d = (usb_interface_descriptor_t *)p;

        if (d->bLength == 0) /* bad descriptor */
            break;
        p += d->bLength;
        if (p <= end && d->bDescriptorType == UDESC_INTERFACE) {
            if (d->bInterfaceNumber != lastidx) {
                lastidx = d->bInterfaceNumber;
                curidx++;
                curaidx = 0;
            } else
                curaidx++;
            if (ifaceidx == curidx && altidx == curaidx)
                return (d);
        }
    }
    return (NULL);
}

static const char *subclass[] = {
		NULL, "SCSI subset", "SFF-8020i", "QIC-157", "UFI", "SFF-8070i", "SCSI complete"
};

/*
 * The MatchCLID library call gets the device descriptor and full config
 * descriptor (including the interface and endpoint descriptors). It will
 * return CLID_Hidd_USBHID if the requested interface confirms to HID.
 */
AROS_LH3(void *, MatchCLID,
         AROS_LHA(usb_device_descriptor_t *, dev,       A0),
         AROS_LHA(usb_config_descriptor_t *, cfg,       A1),
         AROS_LHA(int,                       i,         D0),
         LIBBASETYPEPTR, LIBBASE, 5, Storage)
{
    AROS_LIBFUNC_INIT

    void *clid = NULL;

    D(bug("[MSS] MatchCLID(%p, %p)\n", dev, cfg));

    if (dev->bDeviceClass == UDCLASS_IN_INTERFACE)
    {
        usb_interface_descriptor_t *iface = find_idesc(cfg, i, 0);

        D(bug("[MSS] UDCLASS_IN_INTERFACE OK. Checking interface %d\n", i));
        D(bug("[MSS] iface %d @ %p class %d subclass %d protocol %d\n", i, iface,
        		iface->bInterfaceClass, iface->bInterfaceSubClass, iface->bInterfaceProtocol));

        if (iface->bInterfaceClass == 0x08)
        {
        	D(bug("[MSS] Interface may be handled by Mass Storage Class\n"));

        	if (iface->bInterfaceProtocol == 0x50)
        	{
        		D(bug("[MSS] Lucky you. The device supports Bulk Only transport\n"));

        		if (iface->bInterfaceSubClass > 0 && iface->bInterfaceSubClass < 7)
        		{
        			D(bug("[MSS] Protocol used: %s\n", subclass[iface->bInterfaceSubClass]));

        			if (iface->bInterfaceSubClass == 1 ||
						iface->bInterfaceSubClass == 2 ||
						iface->bInterfaceSubClass == 6)
        				clid = CLID_Hidd_USBStorage;
        		}
        	}
        }
    }

    if (clid)
    	D(bug("[MSS] Pick me! Pick me! Pick me! I can handle it!\n"));

    return clid;

    AROS_LIBFUNC_EXIT
}



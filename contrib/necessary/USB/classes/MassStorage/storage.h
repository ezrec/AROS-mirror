#ifndef STORAGE_H_
#define STORAGE_H_

/*
    Copyright (C) 2006 by Michal Schulz
    $Id: hid.h 29017 2008-07-12 22:25:06Z schulz $

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

#include <stdint.h>

#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/semaphores.h>
#include <exec/ports.h>
#include <exec/interrupts.h>
#include <dos/bptr.h>
#include <dos/dosextens.h>

#include <proto/exec.h>

#include <aros/libcall.h>
#include <aros/asmcall.h>

#include <oop/oop.h>

#include <aros/arossupportbase.h>
#include <exec/execbase.h>

#include LC_LIBDEFS_FILE

#undef HiddPCIDeviceAttrBase
#undef HiddUSBDeviceAttrBase
#undef HiddUSBHubAttrBase
#undef HiddUSBDrvAttrBase
#undef HiddUSBAttrBase
#undef HiddUSBStorageAttrBase
#undef HiddAttrBase

#define HiddPCIDeviceAttrBase (SD(cl)->HiddPCIDeviceAB)
#define HiddUSBDeviceAttrBase (SD(cl)->HiddUSBDeviceAB)
#define HiddUSBHubAttrBase (SD(cl)->HiddUSBHubAB)
#define HiddUSBDrvAttrBase (SD(cl)->HiddUSBDrvAB)
#define HiddAttrBase (SD(cl)->HiddAB)
#define HiddUSBAttrBase (SD(cl)->HiddUSBAB)
#define HiddUSBStorageAttrBase (SD(cl)->HiddUSBStorageAB)

typedef struct __attribute__((packed)) {
	uint32_t	dCBWSignature;
	uint32_t	dCBWTag;
	uint32_t	dCBWDataTransferLength;
	uint8_t		bmCBWFlags;
	uint8_t		bCBWLUN;
	uint8_t		bCBWCBLength;
	uint8_t		CBWCB[16];
} cbw_t;

#define CBW_SIGNATURE	0x43425355
#define CBW_FLAGS_IN	0x80
#define CBW_FLAGS_OUT	0x00

typedef struct __attribute__((packed)) {
	uint32_t	dCSWSignature;
	uint32_t	dCSWTag;
	uint32_t	dCSWDataResidue;
	uint8_t		bCSWStatus;
} csw_t;

#define CSW_SIGNATURE		0x53425355
#define CSW_STATUS_OK		0x00
#define CSW_STATUS_FAIL	0x01
#define CSW_STATUS_PHASE	0x02

struct mss_staticdata
{
    struct SignalSemaphore  Lock;
    void                    *MemPool;
    OOP_Class               *mssClass;

    uint32_t				tid;

    OOP_AttrBase            HiddPCIDeviceAB;
    OOP_AttrBase            HiddUSBDeviceAB;
    OOP_AttrBase            HiddUSBHubAB;
    OOP_AttrBase            HiddUSBDrvAB;
    OOP_AttrBase            HiddUSBAB;
    OOP_AttrBase            HiddUSBStorageAB;
    OOP_AttrBase            HiddAB;
};

typedef struct MSSData {
	struct mss_staticdata		*sd;
	OOP_Object					*o;

	usb_config_descriptor_t		*cdesc;
	usb_device_descriptor_t 	ddesc;
	usb_interface_descriptor_t	*iface;

	usb_endpoint_descriptor_t 	*ep_in;
	usb_endpoint_descriptor_t	*ep_out;

	void						*pipe_in;
	void						*pipe_out;

} StorageData;

struct mssbase
{
    struct Library          LibNode;
    struct mss_staticdata   sd;
};

#define METHOD(base, id, name) \
    base ## __ ## id ## __ ## name (OOP_Class *cl, OOP_Object *o, struct p ## id ## _ ## name *msg)

#define BASE(lib)((struct mssbase*)(lib))
#define SD(cl) (&BASE(cl->UserData)->sd)

static volatile uint32_t getTID(struct mss_staticdata *sd)
{
	uint32_t id;
	Disable(); id = sd->tid++; Enable();
	return id;
}

#endif /* STORAGE_H_ */

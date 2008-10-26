#ifndef USB_STORAGE_H
#define USB_STORAGE_H

/*
    Copyright (C) 2006 by Michal Schulz
    $Id: hid.h 28727 2008-05-17 22:02:30Z schulz $

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
#include <oop/oop.h>
#include <usb/usb.h>

#define CLID_Hidd_USBStorage "Bus::USB::MassStorage"
#define IID_Hidd_USBStorage  "IBus::USB::MassStorage"

#define HiddUSBStorageAttrBase __IHidd_USBStorage

enum {
	moHidd_USBStorage_GetMaxLUN,
	moHidd_USBStorage_Reset,
	moHidd_USBStorage_DirectSCSI,

    NUM_HIDD_USBStorage_METHODS
};

struct pHidd_USBStorage_GetMaxLUN {
	OOP_MethodID	mID;
};

struct pHidd_USBStorage_Reset {
	OOP_MethodID	mID;
};

struct pHidd_USBStorage_DirectSCSI {
	OOP_MethodID	mID;
	uint8_t			lun;
	uint8_t			*cmd;
	uint8_t			cmdLen;
	void			*data;
	uint32_t		dataLen;
	uint8_t			read;
};

BOOL HIDD_USBStorage_Reset(OOP_Object *o);
uint8_t HIDD_USBStorage_GetMaxLUN(OOP_Object *o);
uint32_t HIDD_USBStorage_DirectSCSI(OOP_Object *o, uint8_t lun, uint8_t *cmd, uint8_t cmdLen, void *data, uint32_t dataLen, uint8_t read);

#endif /* USB_STORAGE_H */

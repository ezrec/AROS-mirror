/*
    Copyright (C) 2006 by Michal Schulz
    $Id$

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

#include <aros/symbolsets.h>

#include <exec/types.h>
#include <oop/oop.h>
#include <dos/dos.h>
#include <dos/dosextens.h>

#include <hidd/hidd.h>
#include <usb/usb.h>

#define DEBUG 1

#include <proto/exec.h>
#include <proto/oop.h>
#include <proto/dos.h>
#include <aros/debug.h>

#include "usb.h"
#include LC_LIBDEFS_FILE

OOP_AttrBase HiddUSBAttrBase;
OOP_AttrBase HiddUSBDeviceAttrBase;
OOP_AttrBase HiddUSBHubAttrBase;
OOP_AttrBase HiddUSBDrvAttrBase;
OOP_AttrBase HiddAttrBase;

void usb_process();

static int USB_Init(LIBBASETYPEPTR LIBBASE)
{
    struct OOP_ABDescr attrbases[] = {
            { (STRPTR)IID_Hidd,             &HiddAttrBase },
            { (STRPTR)IID_Hidd_USB,         &HiddUSBAttrBase },
            { (STRPTR)IID_Hidd_USBDevice,   &HiddUSBDeviceAttrBase },
            { (STRPTR)IID_Hidd_USBHub,      &HiddUSBHubAttrBase },
            { (STRPTR)IID_Hidd_USBDrv,      &HiddUSBDrvAttrBase },
            { NULL, NULL }
    };

    D(bug("[USB] USB Init\n"));

    NEWLIST(&LIBBASE->sd.driverList);
    NEWLIST(&LIBBASE->sd.extClassList);
    InitSemaphore(&LIBBASE->sd.global_lock);
    InitSemaphore(&LIBBASE->sd.driverListLock);

    if (OOP_ObtainAttrBases(attrbases))
    {
        if ((LIBBASE->sd.MemPool = CreatePool(MEMF_PUBLIC|MEMF_CLEAR|MEMF_SEM_PROTECTED, 8192, 4096)) != NULL)
        {
            D(bug("[USB] USB::New(): Creating the main USB process\n"));
            struct usbEvent message;

            struct TagItem tags[] = {
                    { NP_Entry,		(IPTR)usb_process },
                    { NP_UserData,	(IPTR)&LIBBASE->sd },		
                    { NP_Priority,	5 },
                    { NP_Name,		(IPTR)"USB" },
                    { TAG_DONE, 	0UL },
            };

            message.ev_Message.mn_ReplyPort = CreateMsgPort();
            message.ev_Type = evt_Startup;

            LIBBASE->sd.usbProcess = CreateNewProc(tags);
            PutMsg(&LIBBASE->sd.usbProcess->pr_MsgPort, (struct Message *)&message);
            WaitPort(message.ev_Message.mn_ReplyPort);
            DeleteMsgPort(message.ev_Message.mn_ReplyPort); 

            return TRUE;
        }
        OOP_ReleaseAttrBases(attrbases);
    }

    return FALSE;
}

static int USB_Expunge(LIBBASETYPEPTR LIBBASE)
{ 
    struct OOP_ABDescr attrbases[] = {
            { (STRPTR)IID_Hidd,                 &HiddAttrBase },
            { (STRPTR)IID_Hidd_USB,             &HiddUSBAttrBase },
            { (STRPTR)IID_Hidd_USBDevice,       &HiddUSBDeviceAttrBase },
            { (STRPTR)IID_Hidd_USBHub,          &HiddUSBHubAttrBase },
            { (STRPTR)IID_Hidd_USBDrv,          &HiddUSBDrvAttrBase },
            { NULL, NULL }
    };
    struct usbEvent message;

    D(bug("[USB] USB Expunge\n"));

    message.ev_Message.mn_ReplyPort = CreateMsgPort();
    message.ev_Type = evt_Cleanup;

    PutMsg(&LIBBASE->sd.usbProcess->pr_MsgPort, (struct Message *)&message);
    WaitPort(message.ev_Message.mn_ReplyPort);
    DeleteMsgPort(message.ev_Message.mn_ReplyPort); 

    OOP_ReleaseAttrBases(attrbases);

    DeletePool(LIBBASE->sd.MemPool);

    return TRUE;
}

ADD2INITLIB(USB_Init, 0)
ADD2EXPUNGELIB(USB_Expunge, 0)

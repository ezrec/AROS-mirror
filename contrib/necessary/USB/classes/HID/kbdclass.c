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

#define DEBUG 1

#include <aros/debug.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>

#include <dos/dos.h>
#include <dos/dosextens.h>

#include <devices/inputevent.h>
#include <devices/input.h>

#include <usb/usb.h>
#include <usb/usb_core.h>
#include <usb/hid.h>
#include "hid.h"

#include <proto/oop.h>
#include <proto/dos.h>
#include <proto/input.h>

static void kbd_process();

void METHOD(USBKbd, Hidd_USBHID, ParseReport)
{
    KbdData *kbd = OOP_INST_DATA(cl, o);
    
    OOP_DoSuperMethod(cl, o);
}

OOP_Object *METHOD(USBKbd, Root, New)
{
    D(bug("[USBKbd] USBKeyboard::New()\n"));

    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);
    if (o)
    {
        KbdData *kbd = OOP_INST_DATA(cl, o);
        kbd->sd = SD(cl);
        kbd->o = o;
        kbd->hd = HIDD_USBHID_GetHidDescriptor(o);
        uint32_t flags;

        D(bug("[USBKbd::New()] Hid descriptor @ %p\n", kbd->hd));
        D(bug("[USBKbd::New()] Number of Report descriptors: %d\n", kbd->hd->bNumDescriptors));
        
        kbd->reportLength = AROS_LE2WORD(kbd->hd->descrs[0].wDescriptorLength);
        kbd->report = AllocVecPooled(SD(cl)->MemPool, kbd->reportLength);
        
        D(bug("[USBKbd::New()] Getting report descriptor of size %d\n", kbd->reportLength));
        
        HIDD_USBHID_GetReportDescriptor(o, kbd->reportLength, kbd->report);
      
        struct TagItem tags[] = {
                { NP_Entry,     (intptr_t)kbd_process },
                { NP_UserData,  (intptr_t)kbd },                
                { NP_Priority,  50 },
                { NP_Name,      (intptr_t)"HID Mouse" },
                { TAG_DONE,     0UL },
        };

        kbd->kbd_task = CreateNewProc(tags);
    }
    
    return o;
}

struct pRoot_Dispose {
    OOP_MethodID        mID;
};

void METHOD(USBKbd, Root, Dispose)
{
    KbdData *kbd = OOP_INST_DATA(cl, o);
    
    Signal(kbd->kbd_task, SIGBREAKF_CTRL_C);
    
    if (kbd->report)
        FreeVecPooled(SD(cl)->MemPool, kbd->report);
    
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

static void kbd_process()
{
    KbdData *kbd = (KbdData *)(FindTask(NULL)->tc_UserData);
    struct hid_staticdata *sd = kbd->sd;
    OOP_Object *o = kbd->o;
    OOP_Class *cl = sd->hidClass;
    uint32_t sigset;
    
    struct MsgPort *port = CreateMsgPort();
    struct IOStdReq *req = (struct IOStdReq *)CreateIORequest(port, sizeof(struct IOStdReq));
    struct InputEvent ie;
    struct Device *InputBase;
    
    if (OpenDevice("input.device", 0, (struct IORequest *)req, 0))
    {
        DeleteIORequest((struct IORequest *)req);
        DeleteMsgPort(port);
        kbd->kbd_task = NULL;
        
        bug("[Kbd] Failed to open input.device\n");
        
        return;
    }
    
    InputBase = req->io_Device;
    
    for (;;)
    {
        sigset = Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F);
        
        if (sigset & SIGBREAKF_CTRL_C)
        {
            D(bug("[Kbd] USB mouse detached. Cleaning up\n"));
            
            CloseDevice((struct IORequest *)req);
            DeleteIORequest((struct IORequest *)req);
            DeleteMsgPort(port);
            return;
        }
        
        if (sigset & SIGBREAKF_CTRL_F)
        {
        }
    }    
}

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

#include <inttypes.h>

#include <exec/types.h>
#include <oop/oop.h>
#include <usb/usb.h>
#include <utility/tagitem.h>
#include <aros/debug.h>
#include <aros/symbolsets.h>

#include <devices/timer.h>

#include <hidd/hidd.h>
#include <hidd/pci.h>
#include <hidd/irq.h>

#include <proto/oop.h>
#include <proto/utility.h>

#include <stdio.h>

#include "ohci.h"

BOOL METHOD(OHCI, Hidd_USBHub, GetHubDescriptor)
{
    ohci_data_t *ohci = OOP_INST_DATA(cl, o);
    
    if (ohci->hubDescr.bDescriptorType) {
        CopyMem(&ohci->hubDescr, msg->descriptor, sizeof(ohci->hubDescr));
        return TRUE;
    }
    else
        return FALSE;
}

BOOL METHOD(OHCI, Hidd_USBHub, OnOff)
{
    ohci_data_t *ohci = OOP_INST_DATA(cl, o);
    BOOL retval = FALSE;
    D(bug("[OHCI] USBHub::OnOff(%d)\n", msg->on));
    
    if (!CheckIO((struct IORequest *)ohci->timerReq))
        AbortIO((struct IORequest *)ohci->timerReq);

    GetMsg(&ohci->timerPort);
    
    if (msg->on)
    {
        ohci->timerReq->tr_node.io_Command = TR_ADDREQUEST;
        ohci->timerReq->tr_time.tv_secs = 0;
        ohci->timerReq->tr_time.tv_micro = 510000;
        SendIO((struct IORequest *)ohci->timerReq);
    }
    
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

//    retval = uhci_run(cl, o, msg->on);
//    uhci_sleep(cl, o, 100);
//    
//    uhci->running = msg->on;
    
    return retval;
}

BOOL METHOD(OHCI, Hidd_USBHub, PortReset)
{
    return FALSE;
}

BOOL METHOD(OHCI, Hidd_USBHub, GetHubStatus)
{
    return TRUE;
}

BOOL METHOD(OHCI, Hidd_USBHub, ClearHubFeature)
{
    return TRUE;
}

BOOL METHOD(OHCI, Hidd_USBHub, SetHubFeature)
{
    return TRUE;
}

BOOL METHOD(OHCI, Hidd_USBHub, GetPortStatus)
{
    ohci_data_t *ohci = OOP_INST_DATA(cl, o);
    BOOL retval = FALSE;
    
    if (msg->port >= 1 && msg->port <= ohci->hubDescr.bNbrPorts)
    {
        uint32_t val = mmio(ohci->regs->HcRhPortStatus[msg->port-1]);
        msg->status->wPortStatus = val;
        msg->status->wPortChange = val >> 16;
        retval = TRUE;
    }
    
    D(bug("[OHCI] GetPortStatus: sts=%04x chg=%04x\n", msg->status->wPortStatus, msg->status->wPortChange));
    
    return retval;
}

BOOL METHOD(OHCI, Hidd_USBHub, ClearPortFeature)
{
    return FALSE;
}

BOOL METHOD(OHCI, Hidd_USBHub, SetPortFeature)
{
    return FALSE;
}

AROS_UFH3(void, OHCI_HubInterrupt,
                 AROS_UFHA(APTR, interruptData, A1),
                 AROS_UFHA(APTR, interruptCode, A5),
                 AROS_UFHA(struct ExecBase *, SysBase, A6))
{
    AROS_USERFUNC_INIT
 
    ohci_data_t *ohci = interruptData;  
 
    /* Remove self from the msg queue */
    GetMsg(&ohci->timerPort);
    
    /* ... */
    
    
    /* Restart the timer */
    ohci->timerReq->tr_node.io_Command = TR_ADDREQUEST;
    ohci->timerReq->tr_time.tv_secs = 0;
    ohci->timerReq->tr_time.tv_micro = 255000;
    SendIO((struct IORequest *)ohci->timerReq);
    
    AROS_USERFUNC_EXIT
}

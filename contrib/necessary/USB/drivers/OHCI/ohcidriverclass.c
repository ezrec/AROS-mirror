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
#include <usb/usb_core.h>
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

/*
 * usb_delay() stops waits for specified amount of miliseconds. It uses the timerequest
 * of specified USB device. No pre-allocation of signals is required.
 */
void ohci_Delay(struct timerequest *tr, uint32_t msec)
{
    /* Allocate a signal within this task context */
    tr->tr_node.io_Message.mn_ReplyPort->mp_SigBit = AllocSignal(-1);
    tr->tr_node.io_Message.mn_ReplyPort->mp_SigTask = FindTask(NULL);
    
    /* Specify the request */
    tr->tr_node.io_Command = TR_ADDREQUEST;
    tr->tr_time.tv_secs = msec / 1000;
    tr->tr_time.tv_micro = 1000 * (msec % 1000);

    /* Wait */
    DoIO((struct IORequest *)tr);
    
    /* The signal is not needed anymore */
    FreeSignal(tr->tr_node.io_Message.mn_ReplyPort->mp_SigBit);
    tr->tr_node.io_Message.mn_ReplyPort->mp_SigTask = NULL;
}


struct timerequest *ohci_CreateTimer()
{
    struct timerequest *tr = NULL;
    struct MsgPort *mp = NULL;
    
    mp = CreateMsgPort();
    if (mp)
    {        
        tr = (struct timerequest *)CreateIORequest(mp, sizeof(struct timerequest));
        if (tr)
        {
            FreeSignal(mp->mp_SigBit);
            if (!OpenDevice((STRPTR)"timer.device", UNIT_MICROHZ, (struct IORequest *)tr, 0))
                return tr;
            
            DeleteIORequest((struct IORequest *)tr);
            mp->mp_SigBit = AllocSignal(-1);
        }
        DeleteMsgPort(mp);
    }
    
    return NULL;
}

void ohci_DeleteTimer(struct timerequest *tr)
{
    if (tr)
    {
        tr->tr_node.io_Message.mn_ReplyPort->mp_SigBit = AllocSignal(-1);
        CloseDevice(tr);
        DeleteMsgPort(tr->tr_node.io_Message.mn_ReplyPort);
        DeleteIORequest((struct IORequest *)tr);
    }
}

/*
 * This function allocats new 32-byte Transfer Descriptor from the 
 * pool of 4K-aligned PCI-accessible memory regions. Within each 4K page, 
 * a bitmap is used to determine, which of the TD elements are available
 * for use.
 * 
 * This function returns NULL if no free TD's are found and no more memory
 * is available.
 */
ohci_td_t *ohci_AllocTD(OOP_Class *cl, OOP_Object *o)
{
    ohci_data_t *ohci = OOP_INST_DATA(cl, o);
    td_node_t *n;
    uint8_t node_num = 32;
    uint8_t bmp_pos = 4;

    ObtainSemaphore(&SD(cl)->tdLock);

    /* Walk through the list of already available 4K pages */
    ForeachNode(&SD(cl)->tdList, n)
    {
        /*
         * For each 4K page, search the first free node (cleared bit) and alloc it.
         */
        for (bmp_pos=0; bmp_pos < 4; bmp_pos++)
        {
            if (n->tdBitmap[bmp_pos] != 0xffffffff)
            {
                for (node_num = 0; node_num < 32; node_num++)
                {
                    /* Free TD */
                    if (!(n->tdBitmap[bmp_pos] & (1 << node_num)))
                    {
                        ohci_td_t * td = &n->tdPage[bmp_pos*32 + node_num];
                        /* Mark the TD as used and return a pointer to it */
                        n->tdBitmap[bmp_pos] |= 1 << node_num;

                        ReleaseSemaphore(&SD(cl)->tdLock);

                        return td;
                    }
                }
            }
        }
    }

    /*
     * No free TDs have been found on the list of 4K pages. Create new Page node
     * and alloc 4K PCI accessible memory region for it
     */
    if ((n = AllocPooled(SD(cl)->memPool, sizeof(td_node_t))))
    {
        if ((n->tdPage = HIDD_PCIDriver_AllocPCIMem(ohci->pciDriver, 4096)))
        {
            /* Make 4K node available for future allocations */
            AddHead(&SD(cl)->tdList, (struct Node*)n);
            ohci_td_t * td = &n->tdPage[0];

            /* Mark first TD as used and return a pointer to it */
            n->tdBitmap[0] |= 1;

            ReleaseSemaphore(&SD(cl)->tdLock);

            return td;
        }
        FreePooled(SD(cl)->memPool, n, sizeof(td_node_t));
    }

    ReleaseSemaphore(&SD(cl)->tdLock);

    /* Everything failed? Out of memory, most likely */
    return NULL;
}

/*
 * This function allocats new 16-byte Endpoint Descriptor aligned at the 16-byte boundary.
 * See ohci_AllocTD for more details.
 */
ohci_ed_t *ohci_AllocED(OOP_Class *cl, OOP_Object *o)
{
    /*
     * Since the Endpoint Descriptors have to be aligned at the 16-byte boundary, they may
     * be allocated from the same pool TD's are
     */
    return (ohci_ed_t *)ohci_AllocTD(cl, o);
}

/*
 * Mark the Transfer Descriptor free, so that it may be allocated by another one.
 * A quick version which may be called from interrupts.
 */
void ohci_FreeTDQuick(ohci_data_t *ohci, ohci_td_t *td)
{
    td_node_t *t;
    
    /* We're inside interrupt probably. Don't ObtainSemaphore. Just lock interrupts instead */
    Disable();

    /* traverse through the list of 4K pages */
    ForeachNode(&ohci->sd->tdList, t)
    {
        /* Address match? */
        if ((intptr_t)t->tdPage == ((intptr_t)td & ~0xfff))
        {
            /* extract the correct location of the TD within the bitmap */
            int bmp = (((intptr_t)td & 0xc00) >> 10);
            int node = (((intptr_t)td & 0x3e0) >> 5);

            /* Free the node */
            t->tdBitmap[bmp] &= ~(1 << node);

            break;
        }
    }

    Enable();
}

void ohci_FreeEDQuick(ohci_data_t *ohci, ohci_ed_t *ed)
{
    ohci_FreeTDQuick(ohci, (ohci_td_t *)ed);
}

/*
 * Mark the Transfer Descriptor free, so that it may be allocated by another one.
 * If the 4K page contains no used descriptors, the page will be freed
 */
void ohci_FreeTD(OOP_Class *cl, OOP_Object *o, ohci_td_t *td)
{
    td_node_t *t, *next;
    ohci_data_t *ohci = OOP_INST_DATA(cl, o);

    ObtainSemaphore(&SD(cl)->tdLock);

    /* traverse through the list of 4K pages */
    ForeachNodeSafe(&SD(cl)->tdList, t, next)
    {
        /* Address match? */
        if ((intptr_t)t->tdPage == ((intptr_t)td & ~0xfff))
        {
            /* extract the correct location of the TD within the bitmap */
            int bmp = (((intptr_t)td & 0xc00) >> 10);
            int node = (((intptr_t)td & 0x3e0) >> 5);

            /* Free the node */
            t->tdBitmap[bmp] &= ~(1 << node);

            /* Check, if all TD nodes are freee within the 4K page */
            int i;
            for (i=0; i < 4; i++)
                if (t->tdBitmap[i] != 0)
                    break;

            /* So is it. Free the 4K page */
            if (i==4)
            {
                Remove((struct Node*)t);
                HIDD_PCIDriver_FreePCIMem(ohci->pciDriver, t->tdPage);
                FreePooled(SD(cl)->memPool, t, sizeof(td_node_t));
            }

            break;
        }
    }
    ReleaseSemaphore(&SD(cl)->tdLock);
}

void ohci_FreeED(OOP_Class *cl, OOP_Object *o, ohci_ed_t *ed)
{
    ohci_FreeTD(cl, o, (ohci_td_t *)ed);
}

void ohci_Handler(HIDDT_IRQ_Handler *irq, HIDDT_IRQ_HwInfo *hw)
{
    ohci_data_t *ohci = (ohci_data_t *)irq->h_Data;
    uint32_t intrs = 0;
    uint32_t done;
    
    done = ohci->hcca->hccaDoneHead;
    
    if (done != 0)
    {
        if (done & ~1)
            intrs = HC_INTR_WDH;
        if (done & 1)
            intrs |= mmio(ohci->regs->HcInterruptStatus);
        ohci->hcca->hccaDoneHead = 0;
    }
    else
    {
        intrs = mmio(ohci->regs->HcInterruptStatus);
        if (intrs & HC_INTR_WDH)
        {
            done = ohci->hcca->hccaDoneHead;
            ohci->hcca->hccaDoneHead = 0;
        }
    }
    
    D(bug("[OHCI] Intr handler %08x\n", intrs));
    intrs &= ~HC_INTR_MIE;

    if (intrs & HC_INTR_WDH)
    {
        D(bug("[OHCI] WDH Interrupt, hccaDoneHead=%p\n[OHCI] TD's in hccaDoneHead:\n", done));
        ohci_td_t *td = (ohci_td_t*)(done & 0xfffffff0);
        
        while (td)
        {
            ohci_td_t *tmp = td;
            
            /* 
             * For each TD issue the signal for corresponding pipe if the 
             * edHeadP=edTailP (thus, the pipe is ready).
             */
            if (td->tdPipe)
            {
                ohci_pipe_t *pipe = td->tdPipe;
                if (pipe->ed->edHeadP == pipe->ed->edTailP)
                {
                    if (pipe->sigTask)
                        Signal(pipe->sigTask, 1 << pipe->signal);
                }
            }
            
            D(bug("[OHCI]   TD @ %p (%p %p %p %p)\n", td, td->tdFlags, td->tdCurrentBufferPointer, td->tdBufferEnd, td->tdNextTD));

            td = (ohci_td_t *)td->tdNextTD;

            if (tmp->tdPipe->type == PIPE_Interrupt)
            {
                ohci_td_t *itd = (ohci_td_t *)tmp->tdPipe->ed->edTailP;
                
                D(bug("[OHCI]   TD belongs to the interrupt pipe. Intr = %p\n", tmp->tdPipe->interrupt));
                
                if (tmp->tdPipe->interrupt)
                {
                    Cause(tmp->tdPipe->interrupt->intr);
                }
                
                D(bug("[OHCI]   restarting TD\n"));
                
                /* Restart the pipe's interrupt */
                itd->tdFlags = tmp->tdFlags;
                itd->tdPipe = tmp->tdPipe;
                itd->tdBufferEnd = tmp->tdBufferEnd;
                itd->tdCurrentBufferPointer = (uint32_t)(tmp->tdPipe->interrupt->buffer);
                itd->tdNextTD = (uint32_t)tmp;
                itd->tdPipe->ed->edTailP = (uint32_t)tmp;
                
                D(bug("[OHCI]   Pipe=%p, HeadP=%p, TailP=%p\n", itd->tdPipe,
                      itd->tdPipe->ed->edHeadP, itd->tdPipe->ed->edTailP));
                
                ohci_td_t *_td = (ohci_td_t*)(itd->tdPipe->ed->edHeadP & 0xfffffff0);
                while (_td)
                {
                    D(bug("[OHCI]   TD @ %p (%p %p %p %p)\n", _td, _td->tdFlags, _td->tdCurrentBufferPointer, _td->tdBufferEnd, _td->tdNextTD));
                    _td = (ohci_td_t *)(_td->tdNextTD & 0xfffffff0);
                }
            }
            else
                ohci_FreeTDQuick(ohci, tmp);

        }
        
        ohci->hcca->hccaDoneHead = 0;
        mmio(ohci->regs->HcInterruptStatus) = HC_INTR_WDH;
        intrs &= ~HC_INTR_WDH;
    }
    
    if (intrs & HC_INTR_RHSC)
    {
        D(bug("[OHCI] RHSC interrupt. Disabling it for 1 second\n"));
        
        mmio(ohci->regs->HcInterruptDisable) = HC_INTR_RHSC;
        mmio(ohci->regs->HcInterruptStatus) = HC_INTR_RHSC;
        
        intrs &= ~HC_INTR_RHSC;
        
        /* Restart the RHSC enable timer */
        ohci->timerReq->tr_node.io_Command = TR_ADDREQUEST;
        ohci->timerReq->tr_time.tv_secs = 0;
        ohci->timerReq->tr_time.tv_micro = 900000;
        SendIO((struct IORequest *)ohci->timerReq);
        
        Disable();
        if (ohci->running)
        {
            struct Interrupt *intr;
            
            ForeachNode(&ohci->intList, intr)
            {
                Cause(intr);
            }
        }
        else
        {
            /*
             * OHCI is not yet in running state, thus posting any interrupts 
             * down the classes is forbidden. The pending flag will be set in 
             * order to issue the interrupts as soon as the OHCI will be switched
             * back to the running state
             */
            ohci->pendingRHSC = 1;
        }
        Enable();
    }
    
    if (intrs)
    {
        mmio(ohci->regs->HcInterruptDisable) = intrs;
        D(bug("[OHCI] Disabling interrupts %p\n", intrs));
    }
}



BOOL METHOD(OHCI, Hidd_USBDrv, AddInterrupt)
{
    BOOL retval = FALSE;
    
    if (msg->pipe == (void *)0xdeadbeef)
    {
        ohci_data_t *ohci = OOP_INST_DATA(cl, o);
        D(bug("[OHCI] AddInterrupt() local for the OHCI. Intr %p, list %p\n", msg->interrupt, &ohci->intList));
        
        if (!ohci->regs)
            ohci->tmp = msg->interrupt;
        else
            AddTail(&ohci->intList, &msg->interrupt->is_Node);
        
        retval = TRUE;    
    }
    else
    {
        ohci_intr_t *intr = AllocVecPooled(SD(cl)->memPool, sizeof(ohci_intr_t));
        ohci_pipe_t *pipe = msg->pipe;
        ohci_td_t *tail; 
        
        D(bug("[OHCI] AddInterrupt() intr = %p\n", intr));
        
        if (intr)
        {
            intr->intr = msg->interrupt;

            tail = ohci_AllocTD(cl, o);
            
            intr->td = pipe->tail;
            intr->td->tdPipe = pipe;
            intr->buffer = msg->buffer;
            intr->length = msg->length;
            
            if (msg->buffer)
            {
                intr->td->tdCurrentBufferPointer = (uint32_t)msg->buffer;
                intr->td->tdBufferEnd = (uint32_t)msg->buffer + msg->length - 1;
            }
            else
                intr->td->tdBufferEnd = intr->td->tdCurrentBufferPointer = 0;
                
            if (UE_GET_DIR(pipe->endpoint) == UE_DIR_IN)
            {
                intr->td->tdFlags = 0x00100000;
            }
            else
            {
                intr->td->tdFlags = 0x00080000;                
            }
            
            intr->td->tdNextTD = (uint32_t)tail;
            
            retval = TRUE;
            
            pipe->interrupt = intr;
            
            pipe->ed->edTailP = (uint32_t)tail;
        }
    }
    D(bug("[OHCI::AddInterrupt] %s\n", retval ? "success":"failure"));
    
    return retval;
}

BOOL METHOD(OHCI, Hidd_USBDrv, RemInterrupt)
{
    if (msg->pipe == (void *)0xdeadbeef)
    {
        Remove(msg->interrupt);
        return TRUE;
    }
    else
    {
#warning TODO:
        return FALSE;
    }
}

static ohci_ed_t *ohci_SelectIntrED(ohci_data_t *ohci, uint8_t interval)
{
    ohci_ed_t   **ed = NULL;
    uint8_t     i, count, minimum, bestat;
    
    if (interval < 2)
    {
        ed = &ohci->int01;
        count = 1;
    }
    else if (interval < 4)
    {
        ed = ohci->int02;
        count = 2;
    }
    else if (interval < 8)
    {
        ed = ohci->int04;
        count = 4;
    }
    else if (interval < 16)
    {
        ed = ohci->int08;
        count = 8;
    }
    else if (interval < 32)
    {
        ed = ohci->int16;
        count = 16;
    }
    else
    {
        ed = ohci->int32;
        count = 32;
    }

    i=0;
    minimum = ED_GET_USAGE(ed[0]);
    bestat = 0;
    do {
        if (minimum > ED_GET_USAGE(ed[i]))
        {
            minimum = ED_GET_USAGE(ed[i]);
            bestat = i;
        }
    } while (++i < count);
    
    (bug("[OHCI] Best node (%d uses) %d from %d\n", minimum, bestat, count));

    ED_SET_USAGE(ed[bestat], ED_GET_USAGE(ed[bestat]) + 1);
    
    return ed[bestat];
}

void *METHOD(OHCI, Hidd_USBDrv, CreatePipe)
{
    ohci_data_t *ohci = OOP_INST_DATA(cl, o);
    ohci_pipe_t *pipe = NULL;

    D({
        bug("[OHCI] CreatePipe(");
        switch (msg->type)
        {
            case PIPE_Bulk:
                bug("BULK");
                break;
            case PIPE_Control:
                bug("CTRL");
                break;
            case PIPE_Interrupt:
                bug("INTR");
                break;
            case PIPE_Isochronous:
                bug("ISOC");
                break;
            default:
                bug("????");
        }
        bug(", %s, %02x, %02x, %d, %dms)\n", msg->fullspeed ? "FAST":"SLOW", 
                msg->address, msg->endpoint, msg->maxpacket, msg->timeout);
    });
    
    pipe = AllocVecPooled(SD(cl)->memPool,sizeof(ohci_pipe_t));
    
    if (pipe)
    {
        pipe->tail = ohci_AllocTD(cl, o);
        pipe->tail->tdBufferEnd = 0;
        pipe->tail->tdCurrentBufferPointer = 0;
        pipe->tail->tdFlags = 0;
        pipe->tail->tdNextTD = 0;
        pipe->tail->tdPipe = pipe;
        
        pipe->ed = ohci_AllocED(cl, o);
        pipe->type = msg->type;
        pipe->address = msg->address;
        pipe->endpoint = msg->endpoint;
        pipe->interval = msg->period;
      
        pipe->ed->edPipe = pipe;
        pipe->ed->edFlags = (msg->address & ED_FA_MASK) |
                            ((msg->endpoint << 7) & ED_EN_MASK) |
                            ((msg->maxpacket << 16) & ED_MPS_MASK) | ED_K;
        
        if (!msg->fullspeed)
            pipe->ed->edFlags |= ED_S;

        pipe->ed->edHeadP = pipe->ed->edTailP = (uint32_t)pipe->tail;
            
        //NEWLIST(&pipe->intList);
        InitSemaphore(&pipe->lock);
        
        pipe->timeoutVal = msg->timeout;
        pipe->timeout = (struct timerequest *)CreateIORequest(
                CreateMsgPort(), sizeof(struct timerequest));

        FreeSignal(pipe->timeout->tr_node.io_Message.mn_ReplyPort->mp_SigBit);
        OpenDevice((STRPTR)"timer.device", UNIT_MICROHZ, (struct IORequest *)pipe->timeout, 0);
        
        /* According to the pipe's type, add it to proper list */
        switch (msg->type)
        {
            case PIPE_Bulk:
                pipe->location = ohci->bulk_head;
                break;

            case PIPE_Control:
                pipe->location = ohci->ctrl_head;
                break;

            case PIPE_Isochronous:
                pipe->location = ohci->isoc_head;
                pipe->ed->edFlags |= ED_F;
                break;

            case PIPE_Interrupt:
                pipe->location = ohci_SelectIntrED(ohci, msg->period);
                break;
        }
        
        if (pipe->location)
        {
            pipe->ed->edNextED = pipe->location->edNextED & 0xfffffff0;
            pipe->location->edNextED = (pipe->location->edNextED & 0x0000000f) | (uint32_t)(pipe->ed);

            D(bug("[OHCI] Master ED at %p\n", pipe->location));
        }
        
        pipe->ed->edFlags &= ~ED_K;
    }
    
    D(bug("[OHCI] CreatePipe()=%p\n", pipe));
    return pipe;
}

void METHOD(OHCI, Hidd_USBDrv, DeletePipe)
{
    ohci_data_t *ohci = OOP_INST_DATA(cl, o);
    ohci_pipe_t *pipe = msg->pipe;
    
    D(bug("[OHCI] DeletePipe(%p, ed=%p)\n", pipe, pipe->ed));
    
    if (pipe)
    {
        ohci_ed_t *ed = pipe->location;
        
        D(bug("[OHCI]  location=%p\n", ed));
        
        /* Stop pipe from beeing processed */
        pipe->ed->edFlags |= ED_K;
        
        if (pipe->type == PIPE_Interrupt)
        {
            Disable();
            ED_SET_USAGE(ed, ED_GET_USAGE(ed) - 1);
            Enable();
        }
        
        /* Remove pipe from the proper list */
        while ((uint32_t)ed & 0xfffffff0)
        {
            D(bug("[OHCI]  ed->edNextED=%p\n", ed->edNextED));
            
            if ((ed->edNextED & 0xfffffff0) == ((uint32_t)pipe->ed))
            {
                D(bug("[OHCI]  ED match the pipe (pipe->edNextED=%p)\n", pipe->ed->edNextED));
                Disable();
                uint32_t next = (ed->edNextED & 0x0000000f) | (pipe->ed->edNextED & 0xfffffff0);
                ed->edNextED = next;
                Enable();
                
                D(bug("[OHCI]  ed->edNextED=%p (fixed)\n", ed->edNextED));
                break;
            }
            
            ed = (ohci_ed_t *)(ed->edNextED & 0xfffffff0);
        }
        
        /* Remove any TD's pending */
        while((pipe->ed->edTailP & 0xfffffff0) != (pipe->ed->edHeadP & 0xfffffff0))
        {
            ohci_td_t *t = (ohci_td_t*)(pipe->ed->edHeadP & 0xfffffff0);
            pipe->ed->edHeadP = t->tdNextTD & 0xfffffff0;
            ohci_FreeTD(cl, o, t);
        }
            
        if (pipe->ed->edTailP)
            ohci_FreeTD(cl, o, (ohci_td_t *)pipe->ed->edTailP);
        
        /* Close timer device */
        pipe->timeout->tr_node.io_Message.mn_ReplyPort->mp_SigBit = AllocSignal(-1);
        CloseDevice((struct IORequest *)pipe->timeout);
        DeleteMsgPort(pipe->timeout->tr_node.io_Message.mn_ReplyPort);
        DeleteIORequest((struct IORequest *)pipe->timeout);

        ohci_FreeED(cl, o, pipe->ed);
        FreeVecPooled(SD(cl)->memPool, pipe);
    }
}

BOOL METHOD(OHCI, Hidd_USBDrv, ControlTransfer)
{
    ohci_data_t *ohci = OOP_INST_DATA(cl, o);
    ohci_pipe_t *pipe = msg->pipe;
    int8_t sig = AllocSignal(-1);
    int8_t toutsig = AllocSignal(-1);
    int32_t msec = pipe->timeoutVal;
    BOOL retval = FALSE;
    
    D(bug("[OHCI] ControlTransfer()\n"));
    
    ObtainSemaphore(&pipe->lock);
    
    if (sig >= 0 && toutsig >= 0)
    {
        ohci_td_t *setup;
        ohci_td_t *status;
        ohci_td_t *td, *tail;
        
        pipe->signal = sig;
        pipe->sigTask = FindTask(NULL);
        
        if (pipe->timeoutVal != 0)
        {
            pipe->timeout->tr_node.io_Message.mn_ReplyPort->mp_SigBit = toutsig;
            pipe->timeout->tr_node.io_Message.mn_ReplyPort->mp_SigTask = FindTask(NULL);

            pipe->timeout->tr_node.io_Command = TR_ADDREQUEST;
            pipe->timeout->tr_time.tv_secs = msec / 1000;
            pipe->timeout->tr_time.tv_micro = 1000 * (msec % 1000);

            SendIO((struct IORequest *)pipe->timeout);
        }
        
        tail = ohci_AllocTD(cl, o);
        tail->tdFlags = 0;
        tail->tdCurrentBufferPointer = 0;
        tail->tdBufferEnd = 0;
        tail->tdNextTD = 0;
        tail->tdPipe = pipe;
        
        /* Do the right control transfer */
        setup = td = pipe->tail;
        setup->tdFlags = 0x02000000 | 0x00e00000;
        setup->tdCurrentBufferPointer = (uint32_t)msg->request;
        setup->tdBufferEnd = (uint32_t)msg->request + sizeof(USBDevice_Request) - 1;
        setup->tdPipe = pipe;
        
        status = ohci_AllocTD(cl, o);
        status->tdFlags = 0x03000000 | ((msg->request->bmRequestType & UT_READ) ? 0x00080000 : 0x00100000);
        status->tdBufferEnd = 0;
        status->tdCurrentBufferPointer = 0;
        status->tdNextTD = (uint32_t)tail;
        status->tdPipe = pipe;
        
        if (msg->buffer && msg->length < 8192)
        {
            /* Get new TD and link it immediatelly with the previous one */
            td->tdNextTD = (uint32_t)ohci_AllocTD(cl, o);
            td = (ohci_td_t *)td->tdNextTD;
            
            td->tdFlags = 0x03e00000 | ((msg->request->bmRequestType & UT_READ) ? 0x00100000 : 0x00080000);
            td->tdCurrentBufferPointer = (uint32_t)msg->buffer;
            td->tdBufferEnd = (uint32_t)msg->buffer + msg->length - 1;
            td->tdPipe = pipe;
        }

        /* Link last TD with the status TD */
        td->tdNextTD = (uint32_t)status;

        D(bug("[OHCI] Transfer:\n[OHCI]   SETUP=%p (%p %p %p %p)\n", setup, setup->tdFlags, setup->tdCurrentBufferPointer, setup->tdBufferEnd, setup->tdNextTD));
        if (td != setup)
            D(bug("[OHCI]   DATA=%p (%p %p %p %p)\n", td, td->tdFlags, td->tdCurrentBufferPointer, td->tdBufferEnd, td->tdNextTD));
        D(bug("[OHCI]   STAT=%p (%p %p %p %p)\n", status, status->tdFlags, status->tdCurrentBufferPointer, status->tdBufferEnd, status->tdNextTD));
        
        pipe->errorCode = 0;
        
        /* Link the first TD with end of the ED's transfer queue */
        pipe->ed->edTailP = (uint32_t)tail;
        pipe->tail = tail;
        
        mmio(ohci->regs->HcCommandStatus) |= HC_CS_CLF;
        
        /* Wait for completion signals */
        if (Wait((1 << sig) | (1 << toutsig)) & (1 << toutsig))
        {
            /* Timeout. Pipe stall. */
            bug("[OHCI] !!!TIMEOUT!!!\n");
            
            GetMsg(pipe->timeout->tr_node.io_Message.mn_ReplyPort);
            
            /* Remove any TD's pending */
            
            D(bug("[OHCI] HeadP=%p TailP=%p\n", pipe->ed->edHeadP, pipe->ed->edTailP));
            D(bug("[OHCI] TD's in ED: \n"));
            ohci_td_t *td = (ohci_td_t*)(pipe->ed->edHeadP & 0xfffffff0);
            while (td)
            {
                D(bug("[OHCI] TD @ %p (%p %p %p %p)\n", td, td->tdFlags, td->tdCurrentBufferPointer, td->tdBufferEnd, td->tdNextTD));
                td = (ohci_td_t *)(td->tdNextTD & 0xfffffff0);
            }

            while((pipe->ed->edTailP & 0xfffffff0) != (pipe->ed->edHeadP & 0xfffffff0))
            {
                ohci_td_t *t = (ohci_td_t*)(pipe->ed->edHeadP & 0xfffffff0);
                pipe->ed->edHeadP = t->tdNextTD & 0xfffffff0;
                ohci_FreeTD(cl, o, t);
            }

            /* Reenable the ED */
            pipe->ed->edHeadP &= 0xfffffff0;
            
            retval = FALSE;                
        }
        else
        {
            /* 
             * No timeout occured. Remove the timeout request and report success. In this case,
             * the TD's are freed automatically by the interrupt handler.
             */
            if (!CheckIO((struct IORequest *)pipe->timeout))
                AbortIO((struct IORequest *)pipe->timeout);
            WaitIO((struct IORequest *)pipe->timeout);
            
            if (!pipe->errorCode)
                retval = TRUE;
        }
        FreeSignal(sig);
        FreeSignal(toutsig);
    }
    
    D(bug("[OHCI] ControlTransfer() %s\n", retval?"OK":"FAILED"));
    
    ReleaseSemaphore(&pipe->lock);
    
    return retval;
}

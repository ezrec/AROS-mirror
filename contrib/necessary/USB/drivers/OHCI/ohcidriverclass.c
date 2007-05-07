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

#include <hidd/hidd.h>
#include <hidd/pci.h>
#include <hidd/irq.h>

#include <proto/oop.h>
#include <proto/utility.h>

#include <stdio.h>

#include "ohci.h"

/*
 * This function allocats new 16-byte Transfer Descriptor from the 
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
    uint8_t bmp_pos = 8;

    ObtainSemaphore(&SD(cl)->tdLock);

    /* Walk through the list of already available 4K pages */
    ForeachNode(&SD(cl)->tdList, n)
    {
        /*
         * For each 4K page, search the first free node (cleared bit) and alloc it.
         */
        for (bmp_pos=0; bmp_pos < 8; bmp_pos++)
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
            int bmp = (((intptr_t)td & 0xe00) >> 9);
            int node = (((intptr_t)td & 0x1f0) >> 4);

            /* Free the node */
            t->tdBitmap[bmp] &= ~(1 << node);

            break;
        }
    }

    Enable();
}

void ohci_FreeEDQuick(ohci_data_t *uhci, ohci_ed_t *ed)
{
    ohci_FreeTDQuick(uhci, (ohci_td_t *)ed);
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
            int bmp = (((intptr_t)td & 0xe00) >> 9);
            int node = (((intptr_t)td & 0x1f0) >> 4);

            /* Free the node */
            t->tdBitmap[bmp] &= ~(1 << node);

            /* Check, if all TD nodes are freee within the 4K page */
            int i;
            for (i=0; i < 8; i++)
                if (t->tdBitmap[i] != 0)
                    break;

            /* So is it. Free the 4K page */
            if (i==8)
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
    
}

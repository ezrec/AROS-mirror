/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <proto/exec.h>

#include "ahci_intern.h"

/* Allocate memory.
 * Memory will be at least 'size', and the aligned start address will
 * be placed in *dma
 */
static APTR AllocMemDMA(ULONG size, ULONG flags, ULONG align, APTR *dma)
{
    APTR raw;

    raw = AllocVec(size + align - 1, flags);
    if (!raw)
        return NULL;

    *dma = raw + (align - (((IPTR)raw) & ~(align-1)));
    return raw;
}

static void FreeMemDMA(APTR raw)
{
    FreeVec(raw);
}

/* Initialize host, and spin up the ports' devices
 */
BOOL ahci_InitHost(struct ahci_Host *ah)
{
    volatile struct ahci_hwhba *host = ah->ah_MMIO;
    int i, cmds;
    int units = 0;
    ULONG val;
    BOOL is64;

    /* Let SMM and SMBIOS know that we are controlling the port */
    mmio_outl_le(mmio_inl_le(&host->ghc) | GHC_AE, &host->ghc);

    /* Make sure all ports are idle */
    for (i = 0; i <  ah->nr_Ports; i++) {
        struct ahci_Port *ap = &ah->ah_Port[i];

        val = mmio_inl_le(&host->cmd);
        if (val & (PORT_CMD_ST | PORT_CMD_CD | PORT_CMD_FRE | PORT_CMD_FR)) {
            /* Reset the controller - it was busy */
            D(bug("[ahci] Port %d.%d: Idling busy port\n", ah->ah_HostNum, ap->ap_PortNum));

            /* Clear CR */
            if (val & PORT_CMD_CR) {
                mmio_outl_le(val & ~PORT_CMD_ST, &host->cmd);
                /* FIXME: Wait for 500ms */
                val = mmio_inl_le(&host->cmd);
                if ((val & PORT_CMD_CR)) {
                    D(bug("[ahci] Port %d.%d: Can't idle port (CR).\n", ah->ah_HostNum, ap->ap_PortNum));
                    ap->ap_Dev = DEV_BROKEN;
                    continue;
                }
            }

            /* Clear FR */
            if (val & PORT_CMD_FRE) {
                mmio_outl_le(val & ~PORT_CMD_FRE, &host->cmd);
                /* FIXME: Wait for 500ms */
                val = mmio_inl_le(&host->cmd);
                if ((val & PORT_CMD_FR)) {
                    D(bug("[ahci] Port %d.%d: Can't idle port (FR).\n", ah->ah_HostNum, ap->ap_PortNum));
                    ap->ap_Dev = DEV_BROKEN;
                    continue;
                }
            }

            ap->ap_Dev = DEV_NONE;
        }
    }

    /* Determine # of command slots */
    val = mmio_inl_le(&host->cap);
    cmds = (val >> CAP_NCS_SHIFT) & CAP_NCS_MASK;
    is64 = (val & CAP_S64A) != 0;

    /* Allocate memory for all ports
     */
    for (i = 0; i < ah->ah_Ports; i++) {
        struct ahci_Port *ap = &ah->ah_Port[i];
        volatile struct ahci_hwport *port = ap->ap_MMIO;
        IPTR tmp;
        ULONG fis_offset;

        if (ap->ap_Dev == DEV_BROKEN)
            continue;

        mmio_outl_le(0, &port->cmd);

        fis_offset = cmds * sizeof(struct ahci_cmdhdr);

        /* Align FIS offset to next 256 byte boundary */
        fis_offset = (fis_offset + 128 - 1) & ~(128 - 1);

        /* Allocate 1K aligned memory for the command list and FISrx */
        ap->ap_RawMemory = AllocMemDMA(fis_offset + sizeof(struct ahci_fis), MEMF_ANY | MEMF_CLEAR, 1024, &ap->ap_DMAMemory);
        if (ap->ap_RawMemory == 0) {
            D(bug("[ahci] Port %d.%d: No DMA memory\n", ah->ah_HostNum, ap->ap_PortNum));
            ap->ap_Dev = DEV_BROKEN;
            continue;
        }
      
        ap->ap_CmdList = ap->ap_DMAMemory;
        ap->ap_FISrx   = ap->ap_DMAMemory + fis_offset;

        tmp = (IPTR)ap->ap_CmdList;
        mmio_outl_le((ULONG)tmp & 0xffffffff, &port->clb);
        if (is64)
            mmio_outl_le((ULONG)(tmp >> 32) & 0xffffffff, &port->clbu);

        tmp = (IPTR)ap->ap_FISrx;
        mmio_outl_le((ULONG)tmp & 0xffffffff, &port->fb);
        if (is64)
            mmio_outl_le((ULONG)(tmp >> 32) & 0xffffffff, &port->fbu);

        mmio_outl_le(PORT_CMD_FRE, &port->cmd);

        /* Wait for the port to spin up */
        D(bug("[ahci] Port %d.%d: Check for attachment...\n", ah->ah_HostNum, ap->ap_PortNum));
        for (timer = 100; timer > 0; timer--) {
            val = mmio_inl_le(&port->ssts);
            if ((val & PORT_SSTS_DET) == 1 ||
                (val & PORT_SSTS_DET) == 3)
                break;
            /* FIXME: Delay 10ms */
        }

        if (timer == 0) {
                D(bug("[ahci] Port %d.%d: Timed out!\n", ah->ah_HostNum, ap->ap_PortNum));
                continue;
        }

        /* Clear any errors */
        mmio_outl_le(~0, &port->serr);

        D(bug("[ahci] Port %d.%d: Waiting spin up...\n", ah->ah_HostNum, ap->ap_PortNum));
        for (timer = 100; timer > 0; timer--) {
            val = mmio_inl_le(&port->tfd);
            if ((val & (PORT_TFD_STS_BSY | PORT_TFD_STS_DRQ | PORT_TFD_STS_ERR)) == 0)
                break;
            /* FIXME: Delay 100ms */
        }

        if (timer == 0) {
                D(bug("[ahci] Port %d.%d: Timed out!\n", ah->ah_HostNum, ap->ap_PortNum));
                continue;
        }

        ap->ap_Dev = DEV_SATA;

        /* Excellent. The port is ready */
        ahci_setup_unit(ap);
        units++;
    }

    return (units) ? TRUE : FALSE;
}

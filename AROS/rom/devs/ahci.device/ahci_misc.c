/*
    Copyright © 2010, The AROS Development Team. All rights reserved
    $Id$
*/

#define DEBUG 1
#include <aros/debug.h>

#include "ahci_header.h"

//#include LC_LIBDEFS_FILE


void delay_ms(struct ahci_hba_chip *hba_chip, uint32_t msec) {
    /* Allocate a signal within this task context */
    hba_chip->tr->tr_node.io_Message.mn_ReplyPort->mp_SigBit = AllocSignal(-1);
    hba_chip->tr->tr_node.io_Message.mn_ReplyPort->mp_SigTask = FindTask(NULL);
    
    /* Specify the request */
    hba_chip->tr->tr_node.io_Command = TR_ADDREQUEST;
    hba_chip->tr->tr_time.tv_secs = msec / 1000;
    hba_chip->tr->tr_time.tv_micro = 1000 * (msec % 1000);

    /* Wait */
    DoIO((struct IORequest *)hba_chip->tr);
    
    /* The signal is not needed anymore */
    FreeSignal(hba_chip->tr->tr_node.io_Message.mn_ReplyPort->mp_SigBit);
    hba_chip->tr->tr_node.io_Message.mn_ReplyPort->mp_SigTask = NULL;
}

void delay_us(struct ahci_hba_chip *hba_chip, uint32_t usec) {
    /* Allocate a signal within this task context */
    hba_chip->tr->tr_node.io_Message.mn_ReplyPort->mp_SigBit = AllocSignal(-1);
    hba_chip->tr->tr_node.io_Message.mn_ReplyPort->mp_SigTask = FindTask(NULL);

	/* Specify the request */
	hba_chip->tr->tr_node.io_Command = TR_ADDREQUEST;
	hba_chip->tr->tr_time.tv_secs = usec / 1000000;
	hba_chip->tr->tr_time.tv_micro = (usec % 1000000);

	/* Wait */
	DoIO((struct IORequest *)hba_chip->tr);

    /* The signal is not needed anymore */
    FreeSignal(hba_chip->tr->tr_node.io_Message.mn_ReplyPort->mp_SigBit);
    hba_chip->tr->tr_node.io_Message.mn_ReplyPort->mp_SigTask = NULL;
}

BOOL sleep2_bitmask_set(struct ahci_hba_chip *hba_chip, volatile uint32_t *reg, uint32_t bits, uint32_t timeout) {
	while (timeout--) {
		if (((*reg) & bits) == bits)
			return TRUE;
        delay_ms(hba_chip, 1);
	}
	return FALSE;
}

BOOL sleep2_bitmask_clr(struct ahci_hba_chip *hba_chip, volatile uint32_t *reg, uint32_t bits, uint32_t timeout) {
	while (timeout--) {
		if (((*reg) & bits) == 0)
			return TRUE;
        delay_ms(hba_chip, 1);
	}
	return FALSE;
}


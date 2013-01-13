/*
 * host_thread.h
 *
 *  Created on: Sep 29, 2012
 *      Author: michal
 */

#ifndef HOST_THREAD_H_
#define HOST_THREAD_H_

#include <exec/types.h>
#include <exec/io.h>

struct HostInterface;

/* Virtual MMIO interface to store all information required by the child thread */
struct HostMMIO {
    UWORD   mmio_Command;
    UBYTE   mmio_IRQ;
    APTR    mmio_Buffer;
    ULONG   mmio_Size;
    int    mmio_File;
    int    mmio_Ret;
    APTR    mmio_Task;
    int    mmio_Signal;
};

/* All information about and for the child unix thread */
struct ThreadData {
    int                         td_pid;         /* pid of the child */
    BYTE *                      td_stack;       /* stack of the child process */
    APTR                        td_irqHandler;  /* IRQ handler attached to the SIGUSR2 */
    LONG                        td_stacksize;   /* Size of the stack */
    struct HostInterface *      td_iface;       /* Host interface to do clib calls */
    volatile struct HostMMIO *  td_mmio;        /* virtual MMIO interface */
};

int host_thread(struct ThreadData *td);

#endif /* HOST_THREAD_H_ */

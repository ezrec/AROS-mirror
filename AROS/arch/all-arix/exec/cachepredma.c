/*
    Copyright � 1995-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc: CachePreDMA() - Do what is necessary for DMA.
    Lang: english
*/

#define DEBUG 0

#include <aros/debug.h>
#include <exec/types.h>
#include <aros/libcall.h>

#include "exec_intern.h"

#define PM_ENTRY_BYTES      sizeof(UQUAD)
#define PM_STATUS_BITS      3
#define PM_STATUS_OFFSET    (64 - PM_STATUS_BITS)
#define PM_STATUS_MASK      (((1LL << PM_STATUS_BITS) - 1) << PM_STATUS_OFFSET)
#define PM_STATUS(nr)       (((nr) << PM_STATUS_OFFSET) & PM_STATUS_MASK)
#define PM_PSHIFT_BITS      6
#define PM_PSHIFT_OFFSET    (PM_STATUS_OFFSET - PM_PSHIFT_BITS)
#define PM_PSHIFT_MASK      (((1LL << PM_PSHIFT_BITS) - 1) << PM_PSHIFT_OFFSET)
#define PM_PSHIFT(x)        (((u64) (x) << PM_PSHIFT_OFFSET) & PM_PSHIFT_MASK)
#define PM_PFRAME_MASK      ((1LL << PM_PSHIFT_OFFSET) - 1)
#define PM_PFRAME(x)        ((x) & PM_PFRAME_MASK)

#define PM_PRESENT          PM_STATUS(4LL)
#define PM_SWAP             PM_STATUS(2LL)
#define PM_FILE             PM_STATUS(1LL)

/*****************************************************************************

    NAME */
#include <proto/exec.h>

	AROS_LH3(APTR, CachePreDMA,

/*  SYNOPSIS */
	AROS_LHA(APTR,    address, A0),
	AROS_LHA(ULONG *, length,  A1),
	AROS_LHA(ULONG,   flags,  D0),

/*  LOCATION */
	struct ExecBase *, SysBase, 127, Exec)

/*  FUNCTION
	Do everything necessary to make CPU caches aware that a DMA
	will happen. Virtual memory systems will make it possible
	that your memory is not at one block and not at the address
	you thought. This function gives you all the information
	you need to split the DMA request up and to convert virtual
	to physical addresses.

    INPUTS
	address - Virtual address of memory affected by the DMA
	*length - Number of bytes affected
	flags   - DMA_Continue    - This is a call to continue a
				    request that was broken up.
		  DMA_ReadFromRAM - Indicate that the DMA goes from
				    RAM to the device. Set this bit
				    in both calls.

    RESULT
	The physical address in memory.
	*length contains the number of contiguous bytes in physical
	memory.

    NOTES
	DMA must follow a call to CachePreDMA() and must be followed
	by a call to CachePostDMA().

    EXAMPLE

    BUGS

    SEE ALSO
	CachePostDMA()

    INTERNALS
	This function should be replaced by a function in $(KERNEL).

******************************************************************************/
{
    AROS_LIBFUNC_INIT
    void *phys_addr;

    int fd;

    /*
     * We're in hosted environment where virtual address does not correspond to
     * a physical address. Therefore, before issuing any DMA transaction we have
     * to lock the memory region of interest and find out it's physical address.
     */

    /*
     * First, the memory has to be locked so that it does not get swapped away.
     * If the address range was in swap, it will be back in RAM after this call
     */
    if (!(flags & DMA_Continue))
    {
        PD(SysBase).SysIFace->mlock(address, *length);
    }

    /* Open pagemap file */
    if ((fd = PD(SysBase).SysIFace->open("/proc/self/pagemap", 0)))
    {

        PD(SysBase).SysIFace->close(fd);
    }

    return address;

    AROS_LIBFUNC_EXIT
} /* CachePreDMA() */

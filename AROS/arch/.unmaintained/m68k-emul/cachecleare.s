/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc:
    Lang:
*/

/******************************************************************************

    NAME
        AROS_LH3(void, CacheClearE,

    SYNOPSIS
        AROS_LHA(APTR,  address, A0),
        AROS_LHA(ULONG, length,  D0),
        AROS_LHA(ULONG, caches,  D1),

    LOCATION
        struct ExecBase *, SysBase, 107, Exec)

    FUNCTION
        Flushes the contents of the CPU caches for a given area of memory
        and a given cache.

    INPUTS
        address - First byte in memory
        length  - number of bytes (~0 means all addresses).
        caches  - CPU caches affected
                    CACRF_ClearI - Instruction cache
                    CACRF_ClearD - Data cache

    RESULT

    NOTES

    EXAMPLE

    BUGS
	Currently this only works on Linux/m68k.
	It ignores the caches argument and clears all caches.
	
    SEE ALSO

    INTERNALS

    HISTORY

******************************************************************************/

        #include "machine.i"

	FirstArg	= 8
	address		= FirstArg
	length		= address+4
	caches		= length+4

	.text
	.balign 16
	.globl	AROS_SLIB_ENTRY(CacheClearE,Exec)
	.type	AROS_SLIB_ENTRY(CacheClearE,Exec),@function
AROS_SLIB_ENTRY(CacheClearE,Exec):
	linkw	%fp,#0
	movem.l	%d2-%d4,-(%sp)
	moveq	#123,%d0
	move.l	address(%fp),%d1
	move.l	#1,%d2
	move.l	#3,%d3
	move.l	length(%fp),%d4
	trap	#0
	movem.l	-12(%fp),%d2-%d4
	unlk	%fp
	rts


/*
    Copyright � 2013, The AROS Development Team. All rights reserved
    $Id$
*/

#include <exec/types.h>
#include <exec/io.h>
#include <hardware/cia.h>
#include <aros/debug.h>

#include "timer.h"
#include "ata.h"

BOOL ata_Calibrate(struct IORequest* tmr, struct ataBase *base)
{
    base->ata_ItersPer100ns = 1;
    return TRUE;
}

static inline void wait_1EClock()
{
    asm volatile("tst.b (%0)"::"a"((void*)0xdbf001));
}

static void wait_EClock(UWORD cnt)
{
    do {
        wait_1EClock();
    } while(cnt-- > 0);
}

void ata_WaitNano(ULONG ns, struct ataBase *base)
{
    (void)base;
    ULONG e_cycle_cnt = 0;
    UWORD short_delay_cnt = 0;

    /* 
        Convert ns to number of E-clock cycles (about 700kHz, 709379 on PAL, 715909 on NTSC)
        Assume 732kHz in order to simplify calculations
    */

    e_cycle_cnt = ns >> 11;     /* /2048 */
    e_cycle_cnt += (ns + 4095) >> 12;
    
    while (e_cycle_cnt > 65535)
    {
        wait_EClock(65535);
        e_cycle_cnt -= 65535;
    }

    short_delay_cnt = e_cycle_cnt & 0xffff;
    while (short_delay_cnt-- > 0)
    {
        wait_1EClock();
    }
}

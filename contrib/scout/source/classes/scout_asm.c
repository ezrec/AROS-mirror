/*
* Scout - The Amiga System Monitor
*
*------------------------------------------------------------------
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*------------------------------------------------------------------
*
* @author Andreas Gelhausen
* @author Richard Körber <rkoerber@gmx.de>
*/

#include "system_headers.h"
#if defined(__amigaos4__)
    #include <exec/emulation.h>
#elif defined(__MORPHOS__)
    #include <exec/system.h>
#endif

FAR struct Library *AsmTimerBase;
FAR ULONG TotalMicros1;
FAR ULONG TotalMicros2;
FAR UBYTE SwitchState;
FAR UBYTE PatchesInstalled;
#ifdef __MORPHOS__
STATIC UQUAD lastsecticks;
#endif

/**********************************************************************
    FreeTaskBuffer
**********************************************************************/

void ASM FAR FreeTaskBuffer( void )
{
}

/**********************************************************************
    InstallPatches
**********************************************************************/

void ASM FAR InstallPatches( void )
{
    PatchesInstalled = TRUE;
}

/**********************************************************************
    UninstallPatches
**********************************************************************/

void ASM FAR UninstallPatches( void )
{
}

/**********************************************************************
    GetTaskData
**********************************************************************/

ULONG ASM FAR GetTaskData( REG( d0, UNUSED struct Task *task) )
{
#ifdef __MORPHOS__
    UQUAD lastseccputime;
    ULONG cpu_p;

    if (task == NULL) {
        NewGetSystemAttrs(&lastseccputime,  sizeof(lastseccputime),  SYSTEMINFOTYPE_LASTSECCPUTIME, TAG_DONE);
        cpu_p = ((DOUBLE)lastseccputime / (DOUBLE)lastsecticks) * 100.0;
    } else {
        NewGetTaskAttrs(task, &lastseccputime, sizeof(lastseccputime), TASKINFOTYPE_LASTSECCPUTIME, TAG_DONE);
        cpu_p = ((DOUBLE)lastseccputime / (DOUBLE)lastsecticks) * 10000.0;
    }

    return cpu_p;
#else
    return 0;
#endif
}

/**********************************************************************
    ClearTaskData
**********************************************************************/

void ASM FAR ClearTaskData( void )
{
    /* fill into global value */
#ifdef  __MORPHOS__
    NewGetSystemAttrsA(&lastsecticks, sizeof(lastsecticks), SYSTEMINFOTYPE_LASTSECTICKS, NULL);
    TotalMicros2 = 1;
#endif
}

/**********************************************************************
    GetAddedTask
**********************************************************************/

struct Task * ASM FAR GetAddedTask( void )
{
    return NULL;
}

/**********************************************************************
    GetCACR (68k)
**********************************************************************/

STATIC CONST UWORD getcacr[] = { 0x4e7a, 0x0002, 0x4e73 };

ULONG ASM FAR GetCACR( void )
{
    return Supervisor((ULONG (*)())&getcacr);
}

/**********************************************************************
    GetVBR (68k)
**********************************************************************/

STATIC CONST UWORD getvbr[] = { 0x4e7a, 0x0801, 0x4e73 };

APTR ASM FAR GetVBR( void )
{
    return (APTR)Supervisor((ULONG (*)())&getvbr);
}

/**********************************************************************
    GetPCR (68k)

    MorphOS 68k emulation is 68060 so...
**********************************************************************/

STATIC CONST UWORD getpcr[] = { 0x4e7a, 0x0808, 0x4e73 };

ULONG ASM FAR GetPCR( void )
{
    return Supervisor((ULONG (*)())&getpcr);
}

/**********************************************************************
    MyCause (68k)
**********************************************************************/

LONG ASM FAR MyCause( REG(a1, struct Interrupt *is), REG(a0, APTR data) )
{
#if defined(__amigaos4__)
    if (IsNative(is->is_Code)) {
        LONG (* irqCode)( APTR ) = (LONG (*)(APTR))is->is_Code;

        return irqCode(data);
    } else {
        return (LONG)EmulateTags(is->is_Code, ET_RegisterA0, data,
                                              ET_RegisterA1, is->is_Data,
                                              ET_RegisterA6, SysBase,
                                              ET_SaveRegs, TRUE,
                                              TAG_DONE);
    }
#elif defined(__MORPHOS__)
    REG_A1 = (ULONG)is->is_Data;
    REG_A0 = (ULONG)data;
    REG_A6 = (ULONG)SysBase;

    return MyEmulHandle->EmulCallDirect68k(is->is_Code);
#else
    return 0;
#endif
}


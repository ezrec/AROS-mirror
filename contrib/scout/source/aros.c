/**
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
 */

#include "system_headers.h"
#include <aros/debug.h>

/* Identify.library *******************************************************/

struct Library *IdentifyBase;

LONG IdAlert(ULONG a, struct TagItem *b)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

LONG IdAlertTags(ULONG a, ULONG b, ...)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

ULONG IdEstimateFormatSize(STRPTR a, struct TagItem *b)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

ULONG IdEstimateFormatSizeTags(STRPTR a, ULONG b, ...)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

LONG IdExpansion(struct TagItem *a)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

LONG IdExpansionTags(ULONG a, ...)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

ULONG IdFormatString(STRPTR a, STRPTR b, ULONG c, struct TagItem *d)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

ULONG IdFormatStringTags(STRPTR a, STRPTR b, ULONG c, ...)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

LONG IdFunction(STRPTR a, LONG b, struct TagItem *c)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

LONG IdFunctionTags(STRPTR a, LONG b, ULONG c, ...)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

STRPTR IdHardware(ULONG a, struct TagItem *b)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

STRPTR IdHardwareTags(ULONG a, ULONG b, ...)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

ULONG IdHardwareNum(ULONG a, struct TagItem *b)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

ULONG IdHardwareNumTags(ULONG a, ULONG b, ...)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}


/* Disassembler.library ***************************************************/

void IdHardwareUpdate(void)
{
    bug("%s not implemented!\n", __func__);
}

APTR Disassemble(struct DisData *ds)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

APTR FindStartPosition(APTR startpc, UWORD min, UWORD max)
{
    bug("%s not implemented!\n", __func__);
    return 0;
}


/* UserGroup.library ******************************************************/

STRPTR getlogin( VOID )
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

STRPTR getpass( STRPTR prompt )
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

UBYTE *crypt( UBYTE *key, UBYTE *set )
{
    bug("%s not implemented!\n", __func__);
    return 0;
}


/* CIA ********************************************************************/

struct Interrupt *AddICRVector( struct Library *resource, LONG iCRBit, struct Interrupt *interrupt )
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

VOID RemICRVector( struct Library *resource, LONG iCRBit, struct Interrupt *interrupt )
{
    bug("%s not implemented!\n", __func__);
}


/* Misc *******************************************************************/

UBYTE *AllocMiscResource( ULONG unitNum, CONST_STRPTR name )
{
    bug("%s not implemented!\n", __func__);
    return 0;
}

VOID FreeMiscResource( ULONG unitNum )
{
    bug("%s not implemented!\n", __func__);
}


/* main *******************************************************************/

struct Args opts;

int main(int argc, char **argv)
{
    ULONG rc = RETURN_FAIL;
    struct Process *me;
    struct WBStartup *wbmsg = NULL;

    me = (struct Process *)FindTask(NULL);

    if (!me->pr_CLI) {
        wbmsg = (struct WBStartup *)argv;
    }

    struct SmartArgs sa;
    memset(&sa, 0x00, sizeof(struct SmartArgs));
    sa.sa_Template = TEMPLATE;
    sa.sa_Parameter = (IPTR *)&opts;
    sa.sa_FileParameter = -1;
    sa.sa_Window = "CON:20/20/400/100/Scout/AUTO/CLOSE/WAIT";

    if (SmartReadArgs(wbmsg, &sa) == 0) {
        if (opts.ToolPri != NULL && (*opts.ToolPri < -128 || *opts.ToolPri > 127)) {
            FPrintf(Output(), "ToolPri is not in range (-128..127).\n");
        } else if (opts.CpuDisplay != NULL && (*opts.CpuDisplay < 0 || *opts.CpuDisplay > 2)) {
            FPrintf(Output(), "CpuDisplay is not in range (0..2).\n");
        } else {
            LONG oldtaskpri = 0;

            if (opts.ToolPri) {
                oldtaskpri = SetTaskPri((struct Task *)me, *opts.ToolPri);
            }

            // Call the main program
            rc = scout_main();

            // Reset program priority if necessary
            if (opts.ToolPri) {
                SetTaskPri((struct Task *)me, (LONG)oldtaskpri);
            }
        }
    }
    SmartFreeArgs(&sa);

    return rc;
}


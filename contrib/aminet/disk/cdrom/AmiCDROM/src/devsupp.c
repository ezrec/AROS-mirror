/* devsupp.c:
 *
 * Support routines for the device handler.
 * - debugging
 * - Mountlist "Startup" field parsing
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 * 
 * 12-Oct-94   fmu   Get_Startup() modified.
 * 17-May-94   fmu   New option MAYBELOWERCASE (=ML).
 * 09-Apr-94   fmu   Larger buffer for startup strings.
 * 02-Jan-94   fmu   New options XPOS and YPOS.
 * 11-Dec-93   fmu   Memory type can now be chosen by the user:
 *                   new options CHIP, DMA and ANY.
 * 11-Dec-93   fmu   The assembly code stubs for the debugging process
 *                   are no longer necessary; the debugger code is now
 *                   called with CreateNewProcTags().
 * 21-Nov-93   fmu   New option SCANINTERVAL.
 * 14-Nov-93   fmu   Added Handle_Control_Packet for 'cdcontrol' program.
 * 10-Nov-93   fmu   New options SHOWVERSION and HFSFIRST.
 * 23-Oct-93   fmu   MacHFS options added.
 * 15-Oct-93   fmu   Adapted to new VOLUME structure.
 */

/*
 * Extract information from Mountlist "Startup" field.
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <exec/types.h>
#include <dos/dostags.h>

#include "device.h"
#include "devsupp.h"
#include "intui.h"
#include "params.h"
#include "cdcontrol.h"

static char *TheVersion = "$VER: " VERSION;

unsigned long g_memory_type;
short g_retry_mode;

int Handle_Control_Packet (ULONG p_type, ULONG p_par1, ULONG p_par2)
{
  switch (p_type) {
  case CDCMD_LOWERCASE:
    g_map_to_lowercase = p_par1;
    break;
  case CDCMD_MACTOISO:
    g_convert_hfs_filenames = p_par1;
    break;
  case CDCMD_CONVERTSPACES:
    g_convert_hfs_spaces = p_par1;
    break;
  case CDCMD_SHOWVERSION:
    g_show_version_numbers = p_par1;
    break;
  case CDCMD_HFSFIRST:
    g_hfs_first = p_par1;
    break;
  case CDCMD_DATAEXT:
    strcpy (g_data_fork_extension, (char *) p_par1);
    break;
  case CDCMD_RESOURCEEXT:
    strcpy (g_resource_fork_extension, (char *) p_par1);
    break;
  default:
    return 999;
  }
  return 0;
}


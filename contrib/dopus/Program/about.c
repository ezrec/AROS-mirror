
/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

/*
#include "DOpus.h"
*/
#include "dopus.h"

#include <dos/var.h>
#include <exec/execbase.h>

void
about()
{
    simplerequest("Directory Opus 4\n"
                  "Original GPL release version 4.12\n"
                  "Copyright 1993-2000 Jonathan Potter\n"
                  "\n"
                  "This program is free software; you can redistribute it and/or\n"
                  "modify it under the terms of the GNU General Public License\n"
                  "as published by the Free Software Foundation; either version 2\n"
                  "of the License, or (at your option) any later version.\n"
                  "\n"
                  "This program is distributed in the hope that it will be useful,\n"
                  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                  "GNU General Public License for more details.\n"
                  "\n"
                  "All users of Directory Opus 4 (including versions distributed\n"
                  "under the GPL) are entitled to upgrade to the latest version of\n"
                  "Directory Opus version 5 at a reduced price. Please see\n"
                  "http://www.gpsoft.com.au for more information.",
                  globstring[STR_CONTINUE], NULL);
}


char *comp_date = __DATE__, *comp_time = __TIME__;

void
give_version_info()
{
    char buf[700];
    char prog_ver_buf[20],
        lib_ver_buf[20],
        kick_ver_buf[20],
        wb_ver_buf[20], proc_buf[10], coproc_buf[10];
    UWORD ver, rev;
    int proc;

    lsprintf(prog_ver_buf, "v%s", str_version_string);
    lsprintf(lib_ver_buf, "v%ld.%ld",
             DOpusBase->LibNode.lib_Version,
             DOpusBase->LibNode.lib_Revision);

#ifndef __AROS__
    if (DOSBase->dl_lib.lib_Version < 36 ||
        (GetVar("Kickstart", &kick_ver_buf[1], 19, GVF_GLOBAL_ONLY)) < 1)
    {

        char *ptr3;
        ULONG ptr, *ptr2;

        ptr2 = (ULONG *) 0xffffec;
        ptr = 0x1000000 - (*ptr2);
        ptr3 = (char *) ptr + 12;
        ptr2 = (ULONG *) ptr3;
        ptr = *ptr2;

        ver = ptr >> 16;
        /* AROS FIX: check!! */
        /* rev=ptr&(((1L<<32)-(1L<<16))-1); */
        rev = ptr & 0xFFFEFFFF; /* ??? */

        lsprintf(kick_ver_buf, "v%ld.%ld", ver, rev);
    }
    else
#endif
        kick_ver_buf[0] = 'v';

    if (DOSBase->dl_lib.lib_Version < 36 ||
        (GetVar("Workbench", &wb_ver_buf[1], 19, GVF_GLOBAL_ONLY)) < 1)
    {

        struct Library *VersionBase;

        if ((VersionBase = OpenLibrary("version.library", 0)))
        {
            ver = VersionBase->lib_Version;
            rev = VersionBase->lib_Revision;
            CloseLibrary(VersionBase);
        }
        else
            ver = rev = 0;

        lsprintf(wb_ver_buf, "v%ld.%ld", ver, rev);
    }
    else
        wb_ver_buf[0] = 'v';

    if (SysBase->AttnFlags & AFF_68010)
    {
        if (SysBase->AttnFlags & AFF_68020)
        {
            if (SysBase->AttnFlags & AFF_68030)
            {
                if (SysBase->AttnFlags & AFF_68040)
                {
                    proc = 40;
                }
                else
                    proc = 30;
            }
            else
                proc = 20;
        }
        else
            proc = 10;
    }
    else
        proc = 0;

    lsprintf(proc_buf, "680%02ld", proc);

    if (SysBase->AttnFlags & AFF_68040
        && SysBase->AttnFlags & AFF_FPU40)
    {
        strcpy(coproc_buf, "68040 FPU");
    }
    else if (SysBase->AttnFlags & AFF_68881)
    {
        if (SysBase->AttnFlags & AFF_68882)
        {
            strcpy(coproc_buf, "68882");
        }
        else
            strcpy(coproc_buf, "68881");
    }
    else
        strcpy(coproc_buf, "None");

    lsprintf(buf,
             "%-42s\n\n"
             "  Directory Opus%24s  \n"
             "  DOpus Library%25s  \n\n"
             "  Compile time%26s  \n"
             "  Compile date%26s  \n\n"
             "  Port/screen name%22s  \n\n"
             "  Kickstart%29s  \n"
             "  Workbench%29s  \n\n"
             "  Processor%29s  \n"
             "  Math co-processor%21s  \n"
             "  Graphics chipset%22s  \n"
             "  Video system%26s  ",
             "Version information...",
             prog_ver_buf,
             lib_ver_buf,
             comp_time, comp_date,
             str_arexx_portname,
             kick_ver_buf,
             wb_ver_buf,
             proc_buf,
             coproc_buf,
             (GfxBase->ChipRevBits0 & GFXF_AA_ALICE) ? "AGA" :
             ((GfxBase->
               ChipRevBits0 & GFXF_HR_AGNUS) ? "ECS" : "OLD"),
             (scrdata_is_pal) ? "PAL" : "NTSC");

    simplerequest(buf, globstring[STR_CONTINUE], NULL);
}

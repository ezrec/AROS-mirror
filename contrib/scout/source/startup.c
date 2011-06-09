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
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#include "system_headers.h"

#ifndef __AROS__
#define EXECBASE        (*(struct ExecBase **)4)
#endif

struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
struct Library *IconBase;
struct GfxBase *GfxBase;
#if defined(__MORPHOS__)
struct Library *CxBase;
struct Library *RexxSysBase;
struct Library *AslBase;
#endif
#if defined(__MORPHOS__) || !defined(__amigaos4__) || !defined(__AROS__)
struct Library *SocketBase;
struct Library *UserGroupBase;
#endif
#if defined(__amigaos4__) || defined(__AROS__) || !defined(__GNUC__)
struct UtilityBase *UtilityBase;
#else
struct Library *UtilityBase;
#endif
#if defined(__amigaos4__)
struct ExecIFace *IExec;
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
struct UtilityIFace *IUtility;
struct IconIFace *IIcon;
#if defined(__NEWLIB__)
struct Library *NewlibBase;
struct Interface *INewlib __attribute__((force_no_baserel));
#endif
#endif

#if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__) && !defined(__SASC)
extern struct Library *__UtilityBase;   // clib2
#endif

struct Args opts;
#if defined(__MORPHOS__)
const ULONG __abox__ = 1;
#elif defined(__amigaos4__)
const UBYTE USED_VAR stack[] = "$STACK: 16384\n";
#elif defined(__SASC)
long __stack = 16384;
#endif

#if defined(__AROS__)
AROS_UFH3(__startup static ULONG, _start,
	  AROS_UFHA(char *, argstr, A0),
	  AROS_UFHA(ULONG, argsize, D0),
	  AROS_UFHA(struct ExecBase *, sBase, A6))
{
    AROS_USERFUNC_INIT

#if defined(__mc68000)
    /* A6 does not contain ExecBase but BCPL stuff */
    sBase = (*(struct ExecBase **)4);
#endif
    
    return startup(sBase);
    
    AROS_USERFUNC_EXIT
}

ULONG startup(struct ExecBase *EXECBASE)
#elif defined(__amigaos4__)
ULONG _start(void)
#else
ULONG SAVEDS realstartup(void)
#endif
{
    ULONG rc = RETURN_FAIL;
    struct Process *me;
    struct WBStartup *wbmsg = NULL;

    SysBase = EXECBASE;
#if defined(__amigaos4__)
    IExec = (struct ExecIFace *)SysBase->MainInterface;
#endif

    me = (struct Process *)FindTask(NULL);

    if (!me->pr_CLI) {
        WaitPort(&me->pr_MsgPort);
        wbmsg = (struct WBStartup *)GetMsg(&me->pr_MsgPort);
    }

#if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__)
    if (FLAG_IS_SET(SysBase->AttnFlags, AFF_68020)) {
#endif
        if ((DOSBase = (struct DosLibrary *)OpenLibrary(DOSNAME, 37)) != NULL) {
            if (GETINTERFACE(IDOS, DOSBase)) {
                if ((IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37)) != NULL) {
                    if (GETINTERFACE(IIntuition, IntuitionBase)) {
                        if ((UtilityBase = (APTR)OpenLibrary(UTILITYNAME, 37)) != NULL) {
                            if (GETINTERFACE(IUtility, UtilityBase)) {
                                if ((IconBase = OpenLibrary(ICONNAME, 37)) != NULL) {
                                    if (GETINTERFACE(IIcon, IconBase)) {
                                    #if defined(__NEWLIB__)
                                        if ((NewlibBase = OpenLibrary("newlib.library", 4)) != NULL) {
                                            if (GETINTERFACE(INewlib, NewlibBase)) {
                                    #endif
                                                struct SmartArgs sa;

                                           #if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__)  && !defined(__SASC)
                                                __UtilityBase = UtilityBase;
                                           #endif

                                                memset(&sa, 0x00, sizeof(struct SmartArgs));
                                                sa.sa_Template = (STRPTR)TEMPLATE;
                                                sa.sa_Parameter = (IPTR *)&opts;
                                                sa.sa_FileParameter = -1;
                                                sa.sa_Window = (STRPTR)"CON:20/20/400/100/Scout/AUTO/CLOSE/WAIT";

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

                                    #if defined(__NEWLIB__)
                                                DROPINTERFACE(INewlib);
                                            }

                                            CloseLibrary(NewlibBase);
                                        }
                                    #endif

                                        DROPINTERFACE(IIcon);
                                    }

                                    CloseLibrary(IconBase);
                                }

                                DROPINTERFACE(IUtility);
                            }

                            CloseLibrary((struct Library *)UtilityBase);
                        }

                        DROPINTERFACE(IIntuition);
                    }

                    CloseLibrary((struct Library *)IntuitionBase);
                }

                DROPINTERFACE(IDOS);
            }

            CloseLibrary((struct Library *)DOSBase);
        }

        if (wbmsg)
        {
            Forbid();
            ReplyMsg((struct Message *)wbmsg);
        }

#if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__)
    }
#endif

    return rc;
}

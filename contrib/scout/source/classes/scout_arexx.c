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
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#include "system_headers.h"

/********************************************************************/
/*                        ARexx-Funktionen                          */
/********************************************************************/
HOOKPROTONHNO(printlist_rxfunc, LONG, ULONG *arg)
{
   if (arg[2])
      PrintHandleMode = MODE_OLDFILE;
   else
      PrintHandleMode = MODE_NEWFILE;

   if (stricmp ("L", (STRPTR)arg[0]) == 0)
      PrintLibraries ((STRPTR)arg[1]);
   else if (stricmp ("D", (STRPTR)arg[0]) == 0)
      PrintDevices ((STRPTR)arg[1]);
   else if (stricmp ("U", (STRPTR)arg[0]) == 0)
      PrintResources ((STRPTR)arg[1]);
   else if (stricmp ("T", (STRPTR)arg[0]) == 0)
      PrintTasks ((STRPTR)arg[1]);
   else if (stricmp ("P", (STRPTR)arg[0]) == 0)
      PrintPorts ((STRPTR)arg[1]);
   else if (stricmp ("C", (STRPTR)arg[0]) == 0)
      PrintCommands ((STRPTR)arg[1]);
   else if (stricmp ("X", (STRPTR)arg[0]) == 0)
      PrintExpansions ((STRPTR)arg[1]);
   else if (stricmp ("M", (STRPTR)arg[0]) == 0)
      PrintMemory ((STRPTR)arg[1]);
   else if (stricmp ("R", (STRPTR)arg[0]) == 0)
      PrintResidents ((STRPTR)arg[1]);
   else if (stricmp ("G", (STRPTR)arg[0]) == 0)
      PrintAssigns ((STRPTR)arg[1]);
   else if (stricmp ("O", (STRPTR)arg[0]) == 0)
      PrintLocks ((STRPTR)arg[1]);
   else if (stricmp ("N", (STRPTR)arg[0]) == 0)
      PrintMounts ((STRPTR)arg[1]);
   else if (stricmp ("H", (STRPTR)arg[0]) == 0)
      PrintInputHandlers ((STRPTR)arg[1]);
   else if (stricmp ("I", (STRPTR)arg[0]) == 0)
      PrintInterrupts ((STRPTR)arg[1]);
   else if (stricmp ("V", (STRPTR)arg[0]) == 0)
      PrintVectors ((STRPTR)arg[1]);
   else if (stricmp ("F", (STRPTR)arg[0]) == 0)
      PrintFonts ((STRPTR)arg[1]);
   else if (stricmp ("S", (STRPTR)arg[0]) == 0)
      PrintSemaphores ((STRPTR)arg[1]);
   else if (stricmp ("W", (STRPTR)arg[0]) == 0)
      PrintWindows ((STRPTR)arg[1]);
   else if (stricmp ("J", (STRPTR)arg[0]) == 0)
      PrintLowMemory ((STRPTR)arg[1]);
   else if (stricmp ("K", (STRPTR)arg[0]) == 0)
      PrintCx ((STRPTR)arg[1]);
   else if (stricmp ("Z", (STRPTR)arg[0]) == 0)
      PrintSMode ((STRPTR)arg[1]);
   else if (stricmp ("Y", (STRPTR)arg[0]) == 0)
      PrintSystem ((STRPTR)arg[1]);
   else if (stricmp ("A", (STRPTR)arg[0]) == 0)
      PrintAllocations ((STRPTR)arg[1]);
   else if (stricmp ("B", (STRPTR)arg[0]) == 0)
      PrintClass ((STRPTR)arg[1]);
   return (RETURN_OK);
}
MakeStaticHook(printlist_rxhook, printlist_rxfunc);

HOOKPROTONHNO(findtask_rxfunc, LONG, ULONG *arg)
{
   struct   Task     *task;
   long result = RETURN_ERROR;

   Forbid();
   if ((task = MyFindTask((STRPTR)arg[0])) != NULL) {
       if (SendResultString("0x%08lx", task)) result = RETURN_OK;
   }
   Permit();
   return result;
}
MakeStaticHook(findtask_rxhook, findtask_rxfunc);

HOOKPROTONHNO(removetask_rxfunc, LONG, ULONG *arg)
{
   if (MyRemoveTask ((STRPTR)arg[0], (BOOL) arg[1]))
      return (RETURN_OK);
   else
      return (RETURN_ERROR);
}
MakeStaticHook(removetask_rxhook, removetask_rxfunc);

HOOKPROTONHNO(freezetask_rxfunc, LONG, ULONG *arg)
{
   if (MyFreezeTask ((STRPTR)arg[0]))
      return (RETURN_OK);
   else
      return (RETURN_ERROR);
}
MakeStaticHook(freezetask_rxhook, freezetask_rxfunc);

HOOKPROTONHNO(activatetask_rxfunc, LONG, ULONG *arg)
{
   if (MyActivateTask ((STRPTR)arg[0]))
      return (RETURN_OK);
   else
      return (RETURN_ERROR);
}
MakeStaticHook(activatetask_rxhook, activatetask_rxfunc);

HOOKPROTONHNO(signaltask_rxfunc, LONG, ULONG *arg)
{
   if (MySignalTask ((STRPTR)arg[0], (STRPTR)arg[1]))
      return (RETURN_OK);
   else
      return (RETURN_ERROR);
}
MakeStaticHook(signaltask_rxhook, signaltask_rxfunc);

HOOKPROTONHNO(breaktask_rxfunc, LONG, ULONG *arg)
{
   if (MySignalTask ((STRPTR)arg[0], "0x1000"))
      return (RETURN_OK);
   else
      return (RETURN_ERROR);
}
MakeStaticHook(breaktask_rxhook, breaktask_rxfunc);

HOOKPROTONHNO(settaskpri_rxfunc, LONG, ULONG *arg)
{
   struct   Task     *task;
   LONG     pri, result = RETURN_ERROR;

   Forbid();
   if ((task = MyFindTask ((STRPTR)arg[0])) != NULL && (IsDec ((STRPTR)arg[1], &pri))) {
      SetTaskPri (task, (int) pri);
      result = RETURN_OK;
   }
   Permit();

   return result;
}
MakeStaticHook(settaskpri_rxhook, settaskpri_rxfunc);

HOOKPROTONHNO(removeport_rxfunc, LONG, ULONG *arg)
{
   struct   MsgPort  *port;

   if ((port = (struct MsgPort *) MyFindName ("PORT", (STRPTR)arg[0])) != NULL) {
      RemPort (port);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removeport_rxhook, removeport_rxfunc);

HOOKPROTONHNO(getlocknumber_rxfunc, LONG, ULONG *arg)
{
   if (SendResultString("%ld", CountLocks((STRPTR)*arg))) return RETURN_OK;

   return RETURN_ERROR;
}
MakeStaticHook(getlocknumber_rxhook, getlocknumber_rxfunc);

HOOKPROTONHNO(removelocks_rxfunc, LONG, ULONG *arg)
{
   RemoveLock((STRPTR)*arg);

   return RETURN_OK;
}
MakeStaticHook(removelocks_rxhook, removelocks_rxfunc);

HOOKPROTONHNO(removelock_rxfunc, LONG, ULONG *arg)
{
   LONG  nobptr;

   if (IsHex ((STRPTR)arg[0], &nobptr)) {
      UnLock ((BPTR) MKBADDR(nobptr));
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removelock_rxhook, removelock_rxfunc);

HOOKPROTONHNO(getpriority_rxfunc, LONG, ULONG *arg)
{
   LONG  address;

   if (IsHex ((STRPTR)arg[0], &address)) {
      if (SendResultString ("%d", ((struct Node *) address)->ln_Pri))
         return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(getpriority_rxhook, getpriority_rxfunc);

HOOKPROTONHNO(setpriority_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;
   struct   List  *list;
   int      value;

   if ((list = MyGetList ((STRPTR)arg[0])) != NULL \
     && (node = MyFindName ((STRPTR)arg[0], (STRPTR)arg[1]))) {

      value = (int) *(ULONG *) arg[2];
      if ((value < 128) && (value > -129)) {

         Remove (node);
         node->ln_Pri = value;
         Enqueue (list, node);

         return (RETURN_OK);
      }
   }
   return (RETURN_ERROR);
}
MakeStaticHook(setpriority_rxhook, setpriority_rxfunc);

HOOKPROTONHNO(findnode_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;

   if ((node = MyFindName ((STRPTR)arg[0], (STRPTR)arg[1])) != NULL) {
      if (SendResultString ("0x%08lx", node))
         return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(findnode_rxhook, findnode_rxfunc);

HOOKPROTONHNO(closelib_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;

   if ((node = MyFindName("LIBRARY", (STRPTR)arg[0])) != NULL) {
      CloseLibrary((struct Library *)node);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(closelib_rxhook, closelib_rxfunc);

HOOKPROTONHNO(removelib_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;

   if ((node = MyFindName("LIBRARY", (STRPTR)arg[0])) != NULL) {
      ((struct Library *)node)->lib_OpenCnt = 0;
      RemLibrary((struct Library *)node);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removelib_rxhook, removelib_rxfunc);

HOOKPROTONHNO(removedev_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;

    if ((node = MyFindName ("DEVICE", (STRPTR)arg[0])) != NULL) {
        ((struct Device *)node)->dd_Library.lib_OpenCnt = 0;
        RemDevice((struct Device *) node);
        return (RETURN_OK);
    }
    return (RETURN_ERROR);
}
MakeStaticHook(removedev_rxhook, removedev_rxfunc);

HOOKPROTONHNO(removeres_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;

   if ((node = MyFindName ("RESOURCE", (STRPTR)arg[0])) != NULL) {
      RemResource((struct Library *) node);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removeres_rxhook, removeres_rxfunc);

HOOKPROTONHNO(obtainsem_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;

   if ((node = MyFindName ("SEMAPHORE", (STRPTR)arg[0])) != NULL) {
      ObtainSemaphore ((struct SignalSemaphore *) node);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(obtainsem_rxhook, obtainsem_rxfunc);

HOOKPROTONHNO(releasesem_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;

   if ((node = MyFindName ("SEMAPHORE", (STRPTR)arg[0])) != NULL \
      && (((struct SignalSemaphore *) node)->ss_NestCount)) {
      ReleaseSemaphore ((struct SignalSemaphore *) node);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(releasesem_rxhook, releasesem_rxfunc);

HOOKPROTONHNO(removesem_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;

   if ((node = MyFindName ("SEMAPHORE", (STRPTR)arg[0])) != NULL) {
      RemSemaphore ((struct SignalSemaphore *) node);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removesem_rxhook, removesem_rxfunc);

HOOKPROTONHNO(removeinput_rxfunc, LONG, ULONG *arg)
{
    LONG result = RETURN_ERROR;
    struct MsgPort *port;

    if ((port = CreateMsgPort()) != NULL) {
        struct IOStdReq *io;

        if ((io = (struct IOStdReq *)CreateIORequest(port, sizeof(struct IOStdReq))) != NULL) {
            if (OpenDevice("input.device", 0, (struct IORequest *)io, 0) == 0) {
                struct Node *node;

                if ((node = MyFindName ("INPUTHANDLER", (STRPTR)arg[0])) != NULL) {
                    io->io_Command = IND_REMHANDLER;
                    io->io_Data = (void *)node;
                    DoIO((struct IORequest *)io);
                    result = RETURN_OK;
                }

                CloseDevice((struct IORequest *)io);
            }

            DeleteIORequest((struct IORequest *)io);
        }

        DeleteMsgPort(port);
    }

    return result;
}
MakeStaticHook(removeinput_rxhook, removeinput_rxfunc);

HOOKPROTONHNO(removelowmemory_rxfunc, LONG, ULONG *arg)
{
   struct Interrupt *intr;

   if ((intr = (struct Interrupt *)MyFindName ("LOWMEMORY", (STRPTR)arg[0])) != NULL) {
      if (SysBase->LibNode.lib_Version >= 39)
         RemMemHandler(intr);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removelowmemory_rxhook, removelowmemory_rxfunc);

HOOKPROTONHNO(causelowmemory_rxfunc, LONG, ULONG *arg)
{
   struct Interrupt *intr;
   struct MemHandlerData mhd = {1000, MEMF_ANY, 0};

   if ((intr = (struct Interrupt *)MyFindName("LOWMEMORY", (STRPTR)arg[0])) != NULL) {
      MyCause(intr, &mhd);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(causelowmemory_rxhook, causelowmemory_rxfunc);

HOOKPROTONHNO(findresident_rxfunc, LONG, ULONG *arg)
{
   struct   Resident *resi;

   if ((resi = FindResident ((STRPTR)arg[0])) != NULL) {
      if (SendResultString ("0x%08lx", resi))
         return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(findresident_rxhook, findresident_rxfunc);

HOOKPROTONHNO(findinterrupt_rxfunc, LONG, ULONG *arg)
{
    struct Interrupt *intr;

    if ((intr = FindInterrupt((STRPTR)arg[0])) != NULL) {
        if (SendResultString("0x%08lx", intr)) {
            return RETURN_OK;
        }
    }

    return RETURN_ERROR;
}
MakeStaticHook(findinterrupt_rxhook, findinterrupt_rxfunc);

HOOKPROTONHNO(removeinterrupt_rxfunc, LONG, ULONG *arg)
{
    struct Interrupt *intr;

    if ((intr = RemoveInterrupt((STRPTR)arg[0])) != NULL) {
        return RETURN_OK;
    }

    return RETURN_ERROR;
}
MakeStaticHook(removeinterrupt_rxhook, removeinterrupt_rxfunc);

HOOKPROTONHNO(removeclass_rxfunc, LONG, ULONG *arg)
{
   struct IClass *node;
   BOOL ret;

   if ((node = (struct IClass *)MyFindName ("CLASSES", (STRPTR)arg[0])) != NULL) {
      if((!node->cl_ObjectCount) && (!node->cl_SubclassCount))
      {
        ret = FreeClass((struct IClass *)node);
        return (ret ? RETURN_OK : RETURN_ERROR);
      }
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removeclass_rxhook, removeclass_rxfunc);

HOOKPROTONHNO(aborttimer_rxfunc, LONG, ULONG *arg)
{
    struct timerequest *tr;

    tr = (struct timerequest *)MyFindName("TIMER_MICROHZ", (STRPTR)arg[0]);
    if (tr == NULL) tr = (struct timerequest *)MyFindName("TIMER_VBLANK", (STRPTR)arg[0]);
    if (tr) {
        Forbid();
        AbortIO((struct IORequest *)tr);
        Permit();
        return RETURN_OK;
    }

    return RETURN_ERROR;
}
MakeStaticHook(aborttimer_rxhook, aborttimer_rxfunc);

HOOKPROTONHNONP(flushdevs_rxfunc, LONG)
{
   FlushDevices();

   return (RETURN_OK);
}
MakeStaticHook(flushdevs_rxhook, flushdevs_rxfunc);

HOOKPROTONHNONP(flushfonts_rxfunc, LONG)
{
   FlushFonts();

   return (RETURN_OK);
}
MakeStaticHook(flushfonts_rxhook, flushfonts_rxfunc);

HOOKPROTONHNONP(flushlibs_rxfunc, LONG)
{
   FlushLibraries();

   return (RETURN_OK);
}
MakeStaticHook(flushlibs_rxhook, flushlibs_rxfunc);

HOOKPROTONHNONP(flushall_rxfunc, LONG)
{
   FlushDevices();
   FlushFonts();
   FlushLibraries();

   return (RETURN_OK);
}
MakeStaticHook(flushall_rxhook, flushall_rxfunc);

HOOKPROTONHNONP(clearreset_rxfunc, LONG)
{
   ClearResetVectors();

   return (RETURN_OK);
}
MakeStaticHook(clearreset_rxhook, clearreset_rxfunc);

HOOKPROTONHNO(poptofront_rxfunc, LONG, ULONG *arg)
{
   struct   Window   *window;
   struct   Screen   *screen;

   if ((screen = MyFindScreen ((STRPTR)arg[0])) != NULL) {
      ScreenToFront (screen);
      return (RETURN_OK);
   } else if ((window = MyFindWindow ((STRPTR)arg[0])) != NULL) {
      ScreenToFront (window->WScreen);
      WindowToFront (window);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(poptofront_rxhook, poptofront_rxfunc);

HOOKPROTONHNO(makevisible_rxfunc, LONG, ULONG *arg)
{
    struct   Window   *window;
    struct   Screen   *screen;

    if ((window = MyFindWindow ((STRPTR)arg[0])) != NULL) {
        ULONG id;
        LONG minX, minY, maxX, maxY;
        LONG moveX, moveY;

        screen = window->WScreen;

        ScreenToFront (screen);
        WindowToFront (window);

        // default max. dimensions of the screen
        minX = 0;
        minY = 0;
        maxX = screen->Width;
        maxY = screen->Height;

        // now try to get the dimensions by asking the apropriate screen mode
        if ((id = GetVPModeID(&screen->ViewPort)) != 0) {
            APTR handle;

            if ((handle = FindDisplayInfo(id)) != NULL) {
                struct DimensionInfo diminfo;

                GetDisplayInfoData(handle, (UBYTE *)&diminfo, sizeof(diminfo), DTAG_DIMS, 0);
                minX = diminfo.Nominal.MinX;
                minY = diminfo.Nominal.MinY;
                maxX = diminfo.Nominal.MaxX;
                maxY = diminfo.Nominal.MaxY;
            }
        }

        // calculate how far the window has to be moved to be made visible
        if (window->LeftEdge < minX) {
            moveX = minX;
        } else if (window->LeftEdge + window->Width >= maxX) {
            moveX = maxX - window->Width;
        } else {
            moveX = window->LeftEdge;
        }

        if (window->TopEdge < minY) {
            moveY = minY;
        } else if (window->TopEdge + window->Height >= maxY) {
            moveY = maxY - window->Height;
        } else {
            moveY = window->TopEdge;
        }

        if (moveX != window->LeftEdge || moveY != window->TopEdge) {
            ChangeWindowBox(window, moveX, moveY, window->Width, window->Height);
        }

        return (RETURN_OK);
    }
    return (RETURN_ERROR);
}
MakeStaticHook(makevisible_rxhook, makevisible_rxfunc);

HOOKPROTONHNO(closewindow_rxfunc, LONG, ULONG *arg)
{
   struct   Window   *window;

   if ((window = MyFindWindow ((STRPTR)arg[0])) != NULL) {
      CloseWindow (window);
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(closewindow_rxhook, closewindow_rxfunc);

HOOKPROTONHNO(closescreen_rxfunc, LONG, ULONG *arg)
{
   struct   Screen   *screen;

   if ((screen = MyFindScreen ((STRPTR)arg[0])) != NULL) {
      if (CloseScreenAll(screen)) {
          return (RETURN_OK);
      }
   }
   return (RETURN_ERROR);
}
MakeStaticHook(closescreen_rxhook, closescreen_rxfunc);

HOOKPROTONHNO(closefont_rxfunc, LONG, ULONG *arg)
{
    LONG font;

    if (IsHex((STRPTR)arg[0], &font)) {
        CloseFont((struct TextFont *)&font);
        return (RETURN_OK);
    }
    return (RETURN_ERROR);
}
MakeStaticHook(closefont_rxhook, closefont_rxfunc);

HOOKPROTONHNO(removefont_rxfunc, LONG, ULONG *arg)
{
    LONG font;

    if (IsHex((STRPTR)arg[0], &font)) {
        RemFont((struct TextFont *)&font);
        Remove((struct Node *)&font);
        return (RETURN_OK);
    }
    return (RETURN_ERROR);
}
MakeStaticHook(removefont_rxhook, removefont_rxfunc);

HOOKPROTONHNO(removecommand_rxfunc, LONG, ULONG *arg)
{
    LONG seg;

    if (IsHex((STRPTR)arg[0], &seg)) {
        if (RemSegment((struct Segment *)seg)) {
            return (RETURN_OK);
        }
    }
    return (RETURN_ERROR);
}
MakeStaticHook(removecommand_rxhook, removecommand_rxfunc);

HOOKPROTONHNO(removeassign_rxfunc, LONG, ULONG *arg)
{

   if (AssignLock((STRPTR)arg[0], (BPTR)NULL)) {
      return (RETURN_OK);
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removeassign_rxhook, removeassign_rxfunc);

HOOKPROTONHNO(removeassignlist_rxfunc, LONG, ULONG *arg)
{
   BPTR  address;

   if (IsHex ((STRPTR)arg[1], (LONG *) &address)) {
      if (RemAssignList((STRPTR)arg[0], address)) {
         return (RETURN_OK);
      }
   }
   return (RETURN_ERROR);
}
MakeStaticHook(removeassignlist_rxhook, removeassignlist_rxfunc);

STATIC struct WindowCmd {
    CONST_STRPTR wc_Name;
    ULONG wc_Method;
} wincmds[] = {
    { "Allocations",   MUIM_MainWin_ShowAllocations   },
    { "Assigns",       MUIM_MainWin_ShowAssigns       },
    { "Classes",       MUIM_MainWin_ShowClasses       },
    { "Commodities",   MUIM_MainWin_ShowCommodities   },
    { "Devices",       MUIM_MainWin_ShowDevices       },
    { "Expansions",    MUIM_MainWin_ShowExpansions    },
    { "Fonts",         MUIM_MainWin_ShowFonts         },
    { "InputHandlers", MUIM_MainWin_ShowInputHandlers },
    { "Interrupts",    MUIM_MainWin_ShowInterrupts    },
    { "Libraries",     MUIM_MainWin_ShowLibraries     },
    { "Locks",         MUIM_MainWin_ShowLocks         },
    { "LowMemory",     MUIM_MainWin_ShowLowMemory     },
    { "Memory",        MUIM_MainWin_ShowMemory        },
    { "Mount",         MUIM_MainWin_ShowMounts        },
    { "Ports",         MUIM_MainWin_ShowPorts         },
    { "Residents",     MUIM_MainWin_ShowResidents     },
    { "Commands",      MUIM_MainWin_ShowCommands      },
    { "Resources",     MUIM_MainWin_ShowResources     },
    { "ScreenMode",    MUIM_MainWin_ShowScreenModes   },
    { "Semaphores",    MUIM_MainWin_ShowSemaphores    },
    { "System",        MUIM_MainWin_ShowSystem        },
    { "Tasks",         MUIM_MainWin_ShowTasks         },
    { "Timer",         MUIM_MainWin_ShowTimers        },
    { "Vectors",       MUIM_MainWin_ShowVectors       },
    { "Windows",       MUIM_MainWin_ShowWindows       },
    { "Patches",       MUIM_MainWin_ShowPatches       },
    { "Catalogs",      MUIM_MainWin_ShowCatalogs      },
    { "ResetHandlers", MUIM_MainWin_ShowResetHandlers },
    { NULL,            0                              }
};

HOOKPROTONHNO(openwindow_rxfunc, LONG, ULONG *arg)
{
    if (AP_Scout) {
        ULONG i = 0;

        while (wincmds[i].wc_Name) {
            if (stricmp((STRPTR)arg[0], wincmds[i].wc_Name) == 0) {
                DoMethod(WI_Main, wincmds[i].wc_Method);
                return RETURN_OK;
            }
            i++;
        }
    }

    return RETURN_ERROR;
}
MakeStaticHook(openwindow_rxhook, openwindow_rxfunc);


STATIC LONG cxfunc( STRPTR cx,
                    ULONG func )
{
    struct Library *CxBase;
    LONG result = RETURN_ERROR;

    if ((CxBase = OpenLibrary("commodities.library", 37)) != NULL) {
    #if defined(__amigaos4__)
        struct CommoditiesIFace *ICommodities;
    #endif

        if (GETINTERFACE(ICommodities, CxBase)) {
            if (BrokerCommand(cx, func) == 0) {
                result = RETURN_OK;
            }

            DROPINTERFACE(ICommodities);
        }

        CloseLibrary(CxBase);
    }

    return result;
}

HOOKPROTONHNO(cxappear_rxfunc, LONG, ULONG *arg)
{
    return cxfunc((STRPTR)arg[0], CXCMD_APPEAR);
}
MakeStaticHook(cxappear_rxhook, cxappear_rxfunc);

HOOKPROTONHNO(cxdisappear_rxfunc, LONG, ULONG *arg)
{
    return cxfunc((STRPTR)arg[0], CXCMD_DISAPPEAR);
}
MakeStaticHook(cxdisappear_rxhook, cxdisappear_rxfunc);

HOOKPROTONHNO(cxenable_rxfunc, LONG, ULONG *arg)
{
    return cxfunc((STRPTR)arg[0], CXCMD_ENABLE);
}
MakeStaticHook(cxenable_rxhook, cxenable_rxfunc);

HOOKPROTONHNO(cxdisable_rxfunc, LONG, ULONG *arg)
{
    return cxfunc((STRPTR)arg[0], CXCMD_DISABLE);
}
MakeStaticHook(cxdisable_rxhook, cxdisable_rxfunc);

HOOKPROTONHNO(cxkill_rxfunc, LONG, ULONG *arg)
{
    return cxfunc((STRPTR)arg[0], CXCMD_KILL);
}
MakeStaticHook(cxkill_rxhook, cxkill_rxfunc);

HOOKPROTONHNO(cxlistchg_rxfunc, LONG, ULONG *arg)
{
    return cxfunc((STRPTR)arg[0], CXCMD_LIST_CHG);
}
MakeStaticHook(cxlistchg_rxhook, cxlistchg_rxfunc);

HOOKPROTONHNO(cxunique_rxfunc, LONG, ULONG *arg)
{
    return cxfunc((STRPTR)arg[0], CXCMD_UNIQUE);
}
MakeStaticHook(cxunique_rxhook, cxunique_rxfunc);

HOOKPROTONHNO(removecx_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;
    LONG result = RETURN_ERROR;

    if ((node = MyFindName ("COMMODITIES", (STRPTR)arg[0])) != NULL) {
        struct Library *CxBase;

        if ((CxBase = OpenLibrary("commodities.library", 37)) != NULL) {
        #if defined(__amigaos4__)
            struct CommoditiesIFace *ICommodities;
        #endif

            if (GETINTERFACE(ICommodities, CxBase)) {
                RemoveCxObj((CxObj *)node);

                result = RETURN_OK;

                DROPINTERFACE(ICommodities);
            }
        }

        CloseLibrary(CxBase);
   }

   return result;
}
MakeStaticHook(removecx_rxhook, removecx_rxfunc);

HOOKPROTONHNO(setcxpri_rxfunc, LONG, ULONG *arg)
{
    struct Node *node;
   LONG   pri;

   if ((node = MyFindName ("COMMODITIES", (STRPTR)arg[0])) != NULL && (IsDec ((STRPTR)arg[1], &pri))) {
      if((pri>=-128) && (pri<=127)) {
        struct List *list = MyGetList("COMMODITIES");
        Forbid();
        Remove(node);
        node->ln_Pri = pri;
        Enqueue(list,node);
        Permit();
        return (RETURN_OK);
      }
   }
   return (RETURN_ERROR);
}
MakeStaticHook(setcxpri_rxhook, setcxpri_rxfunc);

HOOKPROTONHNO(removereset_rxfunc, LONG, ULONG *arg)
{
    LONG result = RETURN_ERROR;
    struct MsgPort *port;

    if ((port = CreateMsgPort()) != NULL) {
        struct IOStdReq *io;

        if ((io = (struct IOStdReq *)CreateIORequest(port, sizeof(struct IOStdReq))) != NULL) {
            if (OpenDevice("keyboard.device", 0, (struct IORequest *)io, 0) == 0) {
                struct Node *node;

                if ((node = MyFindName("RESETHANDLER", (STRPTR)arg[0])) != NULL) {
                    io->io_Command = KBD_REMRESETHANDLER;
                    io->io_Data = (void *)node;
                    DoIO((struct IORequest *)io);

                    result = RETURN_OK;
                }

                CloseDevice((struct IORequest *)io);
            }

            DeleteIORequest((struct IORequest *)io);
        }

        DeleteMsgPort(port);
    }

#if defined(__amigaos4__)
    if (result != RETURN_OK) {
        struct Interrupt *irq;

        if ((irq = (struct Interrupt *)MyFindName("RESETCALLBACK", (STRPTR)arg[0])) != NULL) {
            RemResetCallback(irq);

            result = RETURN_OK;
        }
    }
#endif

    return result;
}
MakeStaticHook(removereset_rxhook, removereset_rxfunc);


const struct MUI_Command arexx_list[] = {
   {(STRPTR)"PrintList"         , (STRPTR)"LIST/A,FILENAME/A,APPEND/S", 3, &printlist_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FindTask"          , (STRPTR)"NAME=ADDRESS/A"            , 1, &findtask_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FreezeTask"        , (STRPTR)"NAME=ADDRESS/A"            , 1, &freezetask_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"ActivateTask"      , (STRPTR)"NAME=ADDRESS/A"            , 1, &activatetask_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveTask"        , (STRPTR)"NAME=ADDRESS/A,WITHPORTS/S", 2, &removetask_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"BreakTask"         , (STRPTR)"NAME=ADDRESS/A"            , 1, &breaktask_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"SignalTask"        , (STRPTR)"NAME=ADDRESS/A,HEXSIGNAL/A", 2, &signaltask_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"SetTaskPri"        , (STRPTR)"NAME=ADDRESS/A,PRIORITY/A" , 2, &settaskpri_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemovePort"        , (STRPTR)"NAME=ADDRESS/A"            , 1, &removeport_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"GetLockNumber"     , (STRPTR)"LOCKPATTERN/F"             , 1, &getlocknumber_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveLocks"       , (STRPTR)"LOCKPATTERN/F"             , 1, &removelocks_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveLock"        , (STRPTR)"ADDRESS/A"                 , 1, &removelock_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"GetPriority"       , (STRPTR)"ADDRESS/A"                 , 1, &getpriority_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"SetPriority"       , (STRPTR)"TYPE/A,NAME=ADDRESS/A,PRIORITY/N", 3, &setpriority_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FindNode"          , (STRPTR)"TYPE/A,NAME=ADDRESS/A"     , 2, &findnode_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CloseLibrary"      , (STRPTR)"NAME=ADDRESS/A"            , 1, &closelib_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveLibrary"     , (STRPTR)"NAME=ADDRESS/A"            , 1, &removelib_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveDevice"      , (STRPTR)"NAME=ADDRESS/A"            , 1, &removedev_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveResource"    , (STRPTR)"NAME=ADDRESS/A"            , 1, &removeres_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"ObtainSemaphore"   , (STRPTR)"NAME=ADDRESS/A"            , 1, &obtainsem_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"ReleaseSemaphore"  , (STRPTR)"NAME=ADDRESS/A"            , 1, &releasesem_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveSemaphore"   , (STRPTR)"NAME=ADDRESS/A"            , 1, &removesem_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveInputHandler", (STRPTR)"NAME=ADDRESS/A"            , 1, &removeinput_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveLowMemory"   , (STRPTR)"NAME=ADDRESS/A"            , 1, &removelowmemory_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CauseLowMemory"    , (STRPTR)"NAME=ADDRESS/A"            , 1, &causelowmemory_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FindResident"      , (STRPTR)"NAME=ADDRESS/A"            , 1, &findresident_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FindInterrupt"     , (STRPTR)"NAME/A"                    , 1, &findinterrupt_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveInterrupt"   , (STRPTR)"NAME/A"                    , 1, &removeinterrupt_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FlushDevs"         , NULL                                , 0, &flushdevs_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FlushFonts"        , NULL                                , 0, &flushfonts_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FlushLibs"         , NULL                                , 0, &flushlibs_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"FlushAll"          , NULL                                , 0, &flushall_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"ClearResetVectors" , NULL                                , 0, &clearreset_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"PopToFront"        , (STRPTR)"TITLE=ADDRESS/A"           , 1, &poptofront_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"MakeVisible"       , (STRPTR)"TITLE=ADDRESS/A"           , 1, &makevisible_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CloseWindow"       , (STRPTR)"TITLE=ADDRESS/A"           , 1, &closewindow_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CloseScreen"       , (STRPTR)"TITLE=ADDRESS/A"           , 1, &closescreen_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CloseFont"         , (STRPTR)"ADDRESS/A"                 , 1, &closefont_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveFont"        , (STRPTR)"ADDRESS/A"                 , 1, &removefont_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveCommand"     , (STRPTR)"ADDRESS/A"                 , 1, &removecommand_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveAssign"      , (STRPTR)"NAME/A"                    , 1, &removeassign_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveAssignList"  , (STRPTR)"NAME/A,ADDRESS/A"          , 2, &removeassignlist_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"OpenWindow"        , (STRPTR)"WINDOWID/F"                , 1, &openwindow_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CxAppear"          , (STRPTR)"NAME/A"                    , 1, &cxappear_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CxDisappear"       , (STRPTR)"NAME/A"                    , 1, &cxdisappear_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CxEnable"          , (STRPTR)"NAME/A"                    , 1, &cxenable_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CxDisable"         , (STRPTR)"NAME/A"                    , 1, &cxdisable_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CxKill"            , (STRPTR)"NAME/A"                    , 1, &cxkill_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CxListChg"         , (STRPTR)"NAME/A"                    , 1, &cxlistchg_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"CxUnique"          , (STRPTR)"NAME/A"                    , 1, &cxunique_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveCx"          , (STRPTR)"NAME=ADDRESS/A"            , 1, &removecx_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"SetCxPri"          , (STRPTR)"NAME=ADDRESS/A,PRIORITY/A" , 2, &setcxpri_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveClass"       , (STRPTR)"NAME/A"                    , 1, &removeclass_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"AbortTimer"        , (STRPTR)"ADDRESS/A"                 , 1, &aborttimer_rxhook, {0, 0, 0, 0, 0} },
   {(STRPTR)"RemoveResetHandler", (STRPTR)"NAME=ADDRESS/A"            , 1, &removereset_rxhook, {0, 0, 0, 0, 0} },
   { NULL, NULL, 0, NULL, {0, 0, 0, 0, 0}  }
};

STRPTR FindMyARexxPort( CONST_STRPTR name )
{
   STRPTR result = NULL;
   STRPTR tmp;
   struct   MsgPort  *port;
   struct   Task     *task;
   int      i;

   if ((tmp = tbAllocVecPooled(globalPool, PATH_LENGTH)) != NULL) {
       task = FindTask (NULL);
       for (i = 0; i < 10; i++) {
          _snprintf(tmp, PATH_LENGTH, "%s.%ld", name, i);

          Forbid();
          port = FindPort (tmp);
          Permit();

          if (port != NULL && port->mp_SigTask == task) {
             result = port->mp_Node.ln_Name;
             break;
          }
       }
       tbFreeVecPooled(globalPool, tmp);
   }

   return result;
}

ULONG SafePutToPort(struct Message *message, CONST_STRPTR portname) {
   struct MsgPort *port;

   Forbid();
   if ((port = FindPort(portname)) != NULL)
      PutMsg (port,message);
   Permit();
   return ((ULONG) port); /* If zero, the port has gone away */
}

short SendStartupMsg( CONST_STRPTR PortName, CONST_STRPTR RString, BOOL IsFileName )
{
   register struct   RexxMsg  *rmsg;
   register short    flag = FALSE;
   STRPTR buffer = NULL;
   ULONG    buflen;

   if ((RString) && ((rmsg = CreateRexxMsg(ScoutPort,NULL,(char *)PortName))) != NULL) {

      rmsg->rm_Action = (RXCOMM|RXFF_STRING);

      if (IsFileName) {
         buflen = strlen (RString) + 4;

         if ((buffer = tbAllocVecPooled(globalPool, buflen)) != NULL) {
            /*
             * Fill in the hostname and the command
             */
            _snprintf(buffer, buflen, "'%s'", RString);

            rmsg->rm_Args[0] = (IPTR)CreateArgstring (buffer, buflen);
         }
      } else {
         rmsg->rm_Args[0] = (IPTR)CreateArgstring (RString,(LONG) strlen (RString));
      }

      if (rmsg->rm_Args[0]) {
         if (SafePutToPort ((struct Message *) rmsg, RXSDIR)) {
            flag=TRUE;
         } else {
            DeleteArgstring ((UBYTE *)rmsg->rm_Args[0]);
            DeleteRexxMsg (rmsg);
         }
      } else {
            DeleteRexxMsg (rmsg);
      }

      if (buffer) tbFreeVecPooled(globalPool, buffer);
   }
   return (flag);
}


/********************************************************************/
/*         Neue Hilfsroutinen für die ARexx-Funktionen              */
/********************************************************************/

struct Task *MyFindTask( CONST_STRPTR stask )
{
   struct Task  *lauf, *task, *result = NULL;
   STRPTR tmp;

   if ((tmp = tbAllocVecPooled(globalPool, TMP_STRING_LENGTH)) != NULL) {
       IPTR _task;

       if (IsUHex(stask, &_task)) {
          task = (struct Task *)_task;

          Forbid();

          ITERATE_LIST(&SysBase->TaskReady, struct Task *, lauf) {
             if (task == lauf) {
                result = lauf;
                break;
             }
          }

          if (!result) {
             ITERATE_LIST(&SysBase->TaskWait, struct Task *, lauf) {
                if (task == lauf) {
                    result = lauf;
                    break;
                }
             }
          }

          Permit();

          if ((! result) && (task == FindTask (NULL)))
             result = task;

       } else {
          Forbid();

          ITERATE_LIST(&SysBase->TaskReady, struct Task *, lauf) {
             GetTaskName (lauf, tmp, TMP_STRING_LENGTH);
             if (stricmp (stask, tmp) == 0) {
                result = lauf;
                break;
             }
          }

          if (!result) {
             ITERATE_LIST(&SysBase->TaskWait, struct Task *, lauf) {
                GetTaskName (lauf, tmp, TMP_STRING_LENGTH);
                if (stricmp (stask, tmp) == 0) {
                   result = lauf;
                   break;
                }
             }
          }

          Permit();

          if (!result) {
            GetTaskName (lauf = FindTask(NULL), tmp, TMP_STRING_LENGTH);
            if (stricmp (stask, tmp) == 0)
               result = lauf;
          }
       }

       tbFreeVecPooled(globalPool, tmp);
   }

   return (result);
}

BOOL MyRemoveTask( CONST_STRPTR stask, BOOL withports )
{
   struct   Task     *task;
   struct   Window   *window, *nextwindow;
   struct   Screen   *screen, *nextscreen;
   BOOL     allwins = FALSE;

   Forbid();
   if ((task = MyFindTask (stask)) != NULL) {

   // Wegen groben Unfugs gestrichen!

      if (MyFreezeTask(stask) && withports) {
         struct MsgPort *mp;
         struct SignalSemaphore *ss;

         mp = (struct MsgPort *)SysBase->PortList.lh_Head;
         while (mp) {
            struct MsgPort *_mp;

            _mp = (struct MsgPort *)mp->mp_Node.ln_Succ;

            if (mp->mp_SigTask == task) {
                RemPort(mp);
            }

            mp = _mp;
         }

         ss = (struct SignalSemaphore *)SysBase->SemaphoreList.lh_Head;
         while (ss) {
            struct SignalSemaphore *_ss;

            _ss = (struct SignalSemaphore *)ss->ss_Link.ln_Succ;

            if (ss->ss_Owner == task) {
               int tmpint = ss->ss_NestCount;

               while (tmpint-- > 0) {
                  ReleaseSemaphore(ss);
               }
            }

            ss = _ss;
         }

         screen = IntuitionBase->FirstScreen;
         while (screen) {
            nextscreen = screen->NextScreen;

            if ((window = screen->FirstWindow) != NULL) {
               allwins = TRUE;

               while (window) {
                  nextwindow = window->NextWindow;

                  if (window->UserPort->mp_SigTask == task) {
                     CloseWindow (window);
                  } else {
                     allwins = FALSE;
                  }
                  window = nextwindow;
               }
            }

            if (allwins && !screen->FirstWindow)
               CloseScreen (screen);

            screen = nextscreen;
         }
      }

      #if defined(__MORPHOS__)
      FlushPool(task->tc_ETask->MemPool);
      #endif
      RemTask (task);
      Permit();
      return (TRUE);
   }
   Permit();
   return (FALSE);
}

BOOL MyFreezeTask( CONST_STRPTR stask )
{
   BOOL result = FALSE;
   struct   Task     *task;

   Forbid();
   if ((task = MyFindTask (stask)) != NULL) {
      Remove ((struct Node *) task);
      task->tc_State = (BYTE) TS_FROZEN;
      Enqueue ((struct List *) &SysBase->TaskWait, (struct Node *) task);
      result = TRUE;
   }
   Permit();

   return result;
}

BOOL MyActivateTask( CONST_STRPTR stask )
{
   BOOL result = FALSE;
   struct   Task     *task;

   Forbid();
   if ((task = MyFindTask (stask)) != NULL) {
      Remove ((struct Node *) task);
      task->tc_State = (BYTE) TS_READY;
      Enqueue ((struct List *) &SysBase->TaskReady, (struct Node *) task);
      result = TRUE;
   }
   Permit();

   return result;
}

BOOL MySignalTask (CONST_STRPTR stask, CONST_STRPTR ssignal )
{
   BOOL result = FALSE;
   struct Task *task;
   ULONG signal;

   Forbid();
   if ((task = MyFindTask (stask)) != NULL && (IsUHex(ssignal, &signal))) {
      Signal (task, signal);
      result = TRUE;
   }
   Permit();

   return result;
}

STATIC CONST_STRPTR nodetype[] = {
   "LIBRARY", "DEVICE", "RESOURCE", "MEMORY", "SEMAPHORE",
   "PORT", "INPUTHANDLER", "LOWMEMORY", "COMMODITIES",
   "CLASSES", "TIMER_MICRO", "TIMER_VBLANK", "RESETHANDLER",
   "RESETCALLBACK", "MONITOR", NULL
};

struct List *MyGetList( CONST_STRPTR type )
{
   int i = 0;
   struct List *list = NULL;

   while (nodetype[i]) {
      if (! stricmp (nodetype[i], type)) {
         switch (i) {
            case 0:
               list = &SysBase->LibList;
               break;

            case 1:
               list = &SysBase->DeviceList;
               break;

            case 2:
               list = &SysBase->ResourceList;
               break;

            case 3:
               list = &SysBase->MemList;
               break;

            case 4:
               list = &SysBase->SemaphoreList;
               break;

            case 5:
               list = &SysBase->PortList;
               break;

            case 6:
               {
                    struct MsgPort *mp;

                    if ((mp = CreateMsgPort()) != NULL) {
                        struct IOStdReq *io;

                        if ((io = (struct IOStdReq *)CreateIORequest(mp, sizeof(*io))) != NULL) {
                            if (OpenDevice("input.device", 0, (struct IORequest *)io, 0) == 0) {
                                struct Interrupt *myirq;

                                if ((myirq = tbAllocVecPooled(globalPool, sizeof(struct Interrupt))) != NULL) {
                                    io->io_Command = IND_ADDHANDLER;
                                    io->io_Data = myirq;
                                    DoIO((struct IORequest *)io);

                                    Forbid();

                                    list = FindListOfNode((struct Node *)myirq);

                                    Permit();

                                    io->io_Command = IND_REMHANDLER;
                                    io->io_Data = myirq;
                                    DoIO((struct IORequest *)io);

                                    tbFreeVecPooled(globalPool, myirq);
                                }

                                CloseDevice((struct IORequest *)io);
                            }

                            DeleteIORequest((struct IORequest *)io);
                        }

                        DeleteMsgPort(mp);
                    }
               }
               break;

            case 7:
               if (SysBase->LibNode.lib_Version >= 39) {
                 list = (struct List *)&SysBase->ex_MemHandlers;
               }
               break;

            case 8:
               {
                  struct MsgPort *cxport;
                  struct NewBroker cxnewbroker =
                  {
                    NB_VERSION,
                    (STRPTR)"« Scout Dummy »",
                    (STRPTR)"« Scout Dummy »",
                    (STRPTR)"Dummy Broker",
                    0,
                    0,
                    0,
                    NULL,
                    0
                  };
                  struct Node *cxbroker;
                  struct Library *CxBase;

                  if ((CxBase = OpenLibrary("commodities.library", 37)) != NULL) {
                  #if defined(__amigaos4__)
                    struct CommoditiesIFace *ICommodities;
                  #endif

                    if (GETINTERFACE(ICommodities, CxBase)) {
                      if ((cxport = CreateMsgPort()) != NULL) {
                         cxnewbroker.nb_Port = cxport;
                         if ((cxbroker = (struct Node *)CxBroker(&cxnewbroker,NULL)) != NULL) {
                              Forbid();

                              list = FindListOfNode(cxbroker);

                              Permit();

                              DeleteCxObjAll((CxObj *)cxbroker);
                         }
                         DeleteMsgPort(cxport);
                      }
                      DROPINTERFACE(ICommodities);
                    }
                    CloseLibrary(CxBase);
                  }
               }
               break;

            case 9:
               {
                  struct IClass *myclass = MakeClass("« Scout Dummy Class »","rootclass",NULL,0,0);
                  if(myclass) {
                     AddClass(myclass);
                     Forbid();
                     list = FindListOfNode((struct Node *)&myclass->cl_Dispatcher.h_MinNode);
                     Permit();
                     FreeClass(myclass);
                  }
               }
               break;

            case 10:
            case 11:
               {
                    struct MsgPort *mp;

                    if ((mp = CreateMsgPort()) != NULL) {
                        struct TimeRequest *mytr;

                        if ((mytr = (struct TimeRequest *)CreateIORequest(mp, sizeof(struct TimeRequest))) != NULL) {
                            ULONG unit;

                            unit = (i == 10) ? UNIT_MICROHZ : UNIT_VBLANK;
                            if (OpenDevice(TIMERNAME, unit, (struct IORequest *)mytr, 0) == 0) {
                                mytr->Request.io_Command = TR_ADDREQUEST;
                                mytr->Time.Seconds = 0;
                                mytr->Time.Microseconds = 10000;

                                Forbid();

                                SendIO((struct IORequest *)mytr);

                                list = FindListOfNode((struct Node *)mytr);

                                Permit();

                                WaitIO((struct IORequest *)mytr);
                                CloseDevice((struct IORequest *)mytr);
                            }

                            DeleteIORequest((struct IORequest *)mytr);
                        }
                        DeleteMsgPort(mp);
                    }
               }
               break;

            case 12:
               {
                    struct MsgPort *mp;

                    if ((mp = CreateMsgPort()) != NULL) {
                        struct IOStdReq *io;

                        if ((io = (struct IOStdReq *)CreateIORequest(mp, sizeof(*io))) != NULL) {
                            if (OpenDevice("keyboard.device", 0, (struct IORequest *)io, 0) == 0) {
                                struct Interrupt *myirq;

                                if ((myirq = tbAllocVecPooled(globalPool, sizeof(struct Interrupt))) != NULL) {
                                    io->io_Command = KBD_ADDRESETHANDLER;
                                    io->io_Data = myirq;
                                    DoIO((struct IORequest *)io);

                                    Forbid();

                                    list = FindListOfNode((struct Node *)myirq);

                                    Permit();

                                    io->io_Command = KBD_REMRESETHANDLER;
                                    io->io_Data = myirq;
                                    DoIO((struct IORequest *)io);

                                    tbFreeVecPooled(globalPool, myirq);
                                }

                                CloseDevice((struct IORequest *)io);
                            }

                            DeleteIORequest((struct IORequest *)io);
                        }

                        DeleteMsgPort(mp);
                    }
               }
               break;

            case 13:
				#if defined(__amigaos4__)
               {
                    struct Interrupt *myirq;

                    if ((myirq = tbAllocVecPooled(globalPool, sizeof(struct Interrupt))) != NULL) {
                        myirq->is_Node.ln_Type = NT_EXTINTERRUPT;
                        myirq->is_Node.ln_Pri = 127;
                        myirq->is_Node.ln_Name = (STRPTR)"Scout";
                        myirq->is_Code = (void (*)())NULL;
                        myirq->is_Data = NULL;

                        if (AddResetCallback(myirq)) {
                            Forbid();

                            list = FindListOfNode((struct Node *)myirq);

                            Permit();

                            RemResetCallback(myirq);
                        }

                        tbFreeVecPooled(globalPool, myirq);
                    }
               }
				#endif
               break;
	    case 14:
	       list = &GfxBase->MonitorList;
	       break;
          }
      }
      i++;
   }
   return (list);
}

struct Node *MyFindName (CONST_STRPTR type, CONST_STRPTR sname)
{
   struct   List  *list;
   struct   Node  *lauf, *node, *result = NULL;

   if ((list = MyGetList (type)) != NULL) {
      LONG _node;

      if (IsHex (sname, &_node)) {
         node = (struct Node *)_node;

         Forbid();

         ITERATE_LIST(list, struct Node *, lauf) {
            if (node == lauf) {
               result = lauf;
               break;
            }
         }

         Permit();
      } else {
         result = FindName (list, sname);
      }
   }

   return (result);
}

struct Screen *MyFindScreen( CONST_STRPTR sname )
{
    struct Screen *screen, *lauf;
    IPTR _screen;

    lauf = IntuitionBase->FirstScreen;
    if (IsUHex(sname, &_screen)) {
        screen = (struct Screen *)_screen;

        while (lauf) {
            if (screen) {
                if (lauf == screen) {
                    return lauf;
                }
            } else {
                if (stricmp (sname, nonetest(lauf->Title)) == 0) {
                    return lauf;
                }
            }

            lauf = lauf->NextScreen;
        }
    }

    return NULL;
}

struct Window *MyFindWindow( CONST_STRPTR sname )
{
    struct Window *window, *lauf, *result = NULL;
    struct Screen *screen;
    ULONG lock;
    IPTR _window;

    lock = LockIBase(0);

    screen = IntuitionBase->FirstScreen;
    if (IsUHex(sname, &_window)) {
        window = (struct Window *)_window;

        while (screen) {
            lauf = screen->FirstWindow;

            while (lauf) {
                if (window) {
                    if (lauf == window) {
                        result = lauf;
                        break;
                    }
                } else {
                    if (stricmp(sname, nonetest(lauf->Title)) == 0) {
                        result = lauf;
                    }
                }

                lauf = lauf->NextWindow;
            }

            screen = screen->NextScreen;
            if (result) {
                break;
            }
        }
    }

    UnlockIBase(lock);

    return result;
}

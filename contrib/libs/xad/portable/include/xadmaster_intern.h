#ifndef XADMASTER_INTERN_H
#define XADMASTER_INTERN_H

/*  $Id$
    internal libbase structure

    XAD library system for archive handling
    Copyright (C) 2011 and later by Dirk Stöcker <soft@dstoecker.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <utility/hooks.h>
#include "xadmaster.h"

#if defined(__GNUC__) && defined(__PPC__)
#pragma pack(2)
#endif

struct xadMasterBaseP {
#ifdef AMIGA
  /* first 6 elements need to be unchanged for compatibility */
  struct Library         xmb_LibNode;
#if !defined(__AROS__)
  struct ExecBase *      xmb_SysBase;
#else
  void *                 xmb_Reserved0;
#endif
  struct DosLibrary *    xmb_DOSBase;
  struct UtilityBase *   xmb_UtilityBase;
#endif
  xadSize                xmb_RecogSize;
  xadSTRPTR              xmb_DefaultName;
  /* totally private stuff */
#ifdef AMIGA
  xadUINT16              xmb_Pad;
  BPTR                   xmb_ClientSegList;
  BPTR                   xmb_SegList;
  struct Hook            xmb_InHookDisk;
  struct Hook            xmb_OutHookDisk;
#endif
  struct Hook            xmb_InHookFH;
  struct Hook            xmb_InHookMem;
  struct Hook            xmb_InHookSplitted;
  struct Hook            xmb_InHookDiskArc;
  struct Hook            xmb_InHookStream;
  struct Hook            xmb_OutHookFH;
  struct Hook            xmb_OutHookMem;
  struct Hook            xmb_OutHookStream;
  struct xadSystemInfo   xmb_System;
  struct xadClient *     xmb_FirstClient;
  xadUINT16              xmb_CRCTable1[256];
  xadUINT32              xmb_CRCTable2[256];
#ifdef UNIX
  void **                xmb_ExtClients;
  xadUINT                xmb_NumExtClients;
  xadINT32               xmb_Unix_AccessCount;
#endif

#if defined(AMIGA) && defined(DEBUGRESOURCE)
  struct xadObject   *   xmb_Resource;
  struct SignalSemaphore xmb_ResourceLock; /* access lock */
#endif

#if defined(__amigaos4__)
  struct ExecIFace *     xmb_IExec;
  struct DOSIFace *      xmb_IDOS;
  struct UtilityIFace *  xmb_IUtility;
  struct ElfIFace *      xmb_IElf;
  struct Library *       xmb_ElfBase;
  struct MinList         xmb_ElfList;
#endif
};

#if defined(__AROS__)
#define XADB(x) ((struct xadMasterBaseP *)x)
#endif

#if defined(__amigaos4__)
#define IExec           ((struct xadMasterBaseP *)xadMasterBase)->xmb_IExec
#define IDOS            ((struct xadMasterBaseP *)xadMasterBase)->xmb_IDOS
#define IUtility        ((struct xadMasterBaseP *)xadMasterBase)->xmb_IUtility
#endif

#if defined(__GNUC__) && defined(__PPC__)
#pragma pack()
#endif

#endif /* XADMASTER_INTERN_H */

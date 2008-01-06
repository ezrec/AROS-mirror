/*

File: resident.c
Author: Neil Cafferkey
Copyright (C) 2008 Neil Cafferkey

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this file; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.

*/

#include <exec/types.h>
#include <exec/resident.h>
#include <exec/alerts.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "handler.h"

#define _STR(A) #A
#define STR(A) _STR(A)

#ifndef REG
#ifdef __mc68000
#define _REG(A, B) B __asm(#A)
#define REG(A, B) _REG(A, B)
#else
#define REG(A, B) B
#endif
#endif

#ifndef BASE_REG
#define BASE_REG a6
#endif

#define HANDLER_NAME "amberram.handler"
#define VERSION 1
#define REVISION 9
#define DATE "6.1.2008"

extern LONG Main();
static VOID Init(REG(d0, ULONG unused), REG(a0, APTR seg_list),
   REG(BASE_REG, struct ExecBase *sys_base));

static const TEXT handler_name[] = HANDLER_NAME;
static const TEXT version_string[] =
   HANDLER_NAME " " STR(VERSION) "." STR(REVISION) " (" DATE ")\n";
static const TEXT dev_name[] = "RAM";

extern const TEXT dos_name[];

const struct Resident rom_tag =
{
   RTC_MATCHWORD,
   (struct Resident *)&rom_tag,
   (APTR)(&rom_tag + 1),
   RTF_AFTERDOS,
   VERSION,
   NT_PROCESS,
   -127,
   (STRPTR)handler_name,
   (STRPTR)version_string,
   (APTR)Init
};



static VOID Init(REG(d0, ULONG unused), REG(a0, APTR seg_list),
   REG(BASE_REG, struct ExecBase *sys_base))
{
   struct DosLibrary *DOSBase;
   struct DeviceNode *dev_node;

   /* Create device node and add it to the system. The handler will then be
      started when it is first accessed */

   DOSBase = (struct DosLibrary *)OpenLibrary(dos_name, DOS_VERSION);
   dev_node = (APTR)MakeDosEntry(dev_name, DLT_DEVICE);
   if(dev_node == NULL)
      Alert(AT_DeadEnd | AG_NoMemory);
   dev_node->dn_Handler = (STRPTR)handler_name;
   dev_node->dn_StackSize = 100000;
   dev_node->dn_SegList = MKBADDR(Main) - 1;
   if(!AddDosEntry((APTR)dev_node))
      Alert(AT_DeadEnd);

   return;
}


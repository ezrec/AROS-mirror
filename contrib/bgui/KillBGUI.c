/*
 * @(#) $Header$
 *
 * KillBGUI.c
 *
 * This program will flush bgui.library out of memory, if this is not possible;
 * then the existing one is renamed "bgui.flushed" so it will not be found again.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:08:00  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:53:33  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:11:21  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:05  mlemos
 * Ian sources
 *
 *
 */
/* KillBGUI.c
 *
 * This program will flush bgui.library out of memory, if this is not possible;
 * then the existing one is renamed "bgui.flushed" so it will not be found again.
 */

#include <proto/exec.h>
#include <exec/execbase.h>
#include <strings.h>
#include <stdio.h>

void main(void)
{
   struct Node *BGUIBase;
   
   AllocMem(0x10000000, 0);
   Forbid();
   BGUIBase = FindName(&((struct ExecBase *)SysBase)->LibList, "bgui.library");
   Permit();
   if (BGUIBase) strcpy(BGUIBase->ln_Name + 5, "flushed");
}

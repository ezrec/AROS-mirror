/*
 * BGUI library
 * libtag.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 1.2  2000/07/03 20:45:44  bergers
 * Update. Test1 now successfully opens the library. In LibOpen the AddTaskMember function seems to trash the stack somehow (return address is trashed) and therefore I had to take it out.
 *
 * Revision 1.1  2000/05/30 03:25:24  bergers
 * Instead of libtag.asm.
 *
 * Revision 41.10  1998/02/25 21:12:28  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:54  mlemos
 * Ian sources
 *
 *
 */

#include <exec/libraries.h>
#include <exec/resident.h>

extern unsigned char LibName[];
extern const unsigned char LibID[];;
extern const unsigned char LibVer[];
extern struct Library * LibInit();
extern int BGUI_end;

int entry(void)
{
  printf("entry called. Doing nothing!\n");
  return -1;
}

struct Resident ROMTag =
{
  RTC_MATCHWORD,
  (struct Resident *)&ROMTag,
  &BGUI_end,
  0,
  41,
  NT_LIBRARY,
  0,
  LibName,
  LibID,
  LibInit
};


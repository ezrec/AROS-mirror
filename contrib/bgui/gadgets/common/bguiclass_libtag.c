/*
 * $Log$
 * Revision 1.3  2004/01/09 19:06:06  stegerg
 * If compiled for AROS, don't printf() in entry().
 *
 * Revision 1.2  2002/04/28 11:35:44  verhaegs
 * include stdio.h for printf
 *
 * Revision 1.1  2000/07/09 03:06:50  bergers
 * Added some files.
 *
 * Revision 1.2  2000/07/03 20:45:44  bergers
 * Update. Test1 now successfully opens the library. In LibOpen the AddTaskMember function seems to trash the stack somehow (return address is trashed) and therefore I had to take it out.
 *
 */

#include <exec/libraries.h>
#include <exec/resident.h>
#include <exec/types.h>

#include <stdio.h>

extern UBYTE _LibName[];
extern UBYTE _LibID[];;
extern UWORD _LibVersion;
extern ULONG _LibInit[];
extern int BGUIGadget_end;

int entry(void)
{
#ifndef __AROS__
  printf("entry called. Doing nothing!\n");
#endif
  return -1;
}

struct Resident ROMTag =
{
  RTC_MATCHWORD,
  (struct Resident *)&ROMTag,
  &BGUIGadget_end,
  RTF_AUTOINIT,
  41,
  NT_LIBRARY,
  0,
  _LibName,
  _LibID,
  _LibInit
};


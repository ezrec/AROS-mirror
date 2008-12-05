/*
  NDirlist.mcc (c) Copyright 1996 by Gilles Masson
  Registered MUI class, Serial Number: 1d51                            0x9d5100b0 to 0x9d5100cF
  *** use only YOUR OWN Serial Number for your public custom class ***
  NDirlist_priv_mcc.h
*/

#ifndef MUI_NDirlist_priv_MCC_H
#define MUI_NDirlist_priv_MCC_H

#ifndef MUI_MUI_H
#include <mui/mui.h>
#endif

#include <MUI/NDirlist_mcc.h>

#define ALIGN_MASK      (0x0700)


struct DirEntry {
  UBYTE  *Name;
  UBYTE  *Size;
  UBYTE  *Date;
  UBYTE  *Time;
  UBYTE  *Prot;
  UBYTE  *Comment;
  LONG    ed_Type;
  ULONG   ed_Size;
  ULONG   ed_Prot;
  ULONG   ed_Days;
  ULONG   ed_Mins;
  ULONG   ed_Ticks;
};


struct NDLData
{
  LONG SETUP;
  char *NDirlist_Directory;
  struct Hook *NDirlist_Hook;
  struct ExAllControl *NDirlist_eac;
  struct ExAllData *NDirlist_ed;
  ULONG  NDirlist_Total;
  struct DirEntry **NDirlist_DirEntry;
  LONG   NDirlist_DirEntryCurrent;
  LONG   NDirlist_DirEntryLast;
  struct FileInfoBlock *NDirlist_fib;
};

#endif /* MUI_NDirlist_priv_MCC_H */


// :ts=4 (Tabsize)

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
// we need to export our struct instance data because maintask needs its size
#include <dos/dos.h>

struct DOSIconInst
{
        BPTR currentdir;
        ULONG size;
        ULONG prot;
        struct DateStamp date;
        char *comment;
        UWORD owneruid;
        UWORD ownergid;
        LONG type;
        UWORD flags;
        APTR path;
};

// possible values of flags:

#define DOSIF_SIZE      (1<<0)
#define DOSIF_PROT      (1<<1)
#define DOSIF_DATE      (1<<2)
#define DOSIF_COMMENT   (1<<3)
#define DOSIF_UID       (1<<4)
#define DOSIF_GID       (1<<5)
#define DOSIF_TYPE      (1<<6)


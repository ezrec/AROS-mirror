// :ts=4 (Tabsize)
// ---------------- internal Structures -------------

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
#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#ifndef SCALOS_H
#include "scalos.h"
#endif /* SCALOS_H */


#define SCALOS_MSGID        MAKE_ID('S','M','S','G')

struct SCMSGP_Quit
{
        struct  SC_Message scmsg;
};

struct SCMSGP_Method
{
        struct  SC_Message scmsg;
        struct  SC_MethodData *methoddata;
        ULONG   methodarg1;
};

struct SCMSGP_Startup
{
        struct  SC_Message scmsg;
        Object  *threadobj;
        struct  TagItem *taglist;
};

struct SCMSGP_SetIDCMP
{
        struct  SC_Message scmsg;
        ULONG   IDCMP;
        Object  *obj;
};

struct SCMSGP_Notify
{
        struct SC_Message scmsg;
        ULONG  arg1;
};                                      // that's not the whole size of this message

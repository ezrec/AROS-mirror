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
#ifndef SCALOS_H
#include "Scalos.h"
#endif /* SCALOS_H */

// not real instance data. This struct is located before the object data.
struct RootInst
{
        struct SignalSemaphore  objectlistsem;
        struct MinList                  objectlist;
        struct SignalSemaphore  notilistsem;
        struct MinList                  notilist;
        struct MinNode                  node;
        struct SC_Class                 *oclass;
};

#define myRootInst(obj) (((struct RootInst *)(obj)) - 1)

// refer to SCCP_Notify also
struct NotifyNode                               // node of notilist
{
        struct  MinNode node;
        ULONG   TriggerAttr;
        ULONG   TriggerVal;
        Object  *DestObject;
        ULONG   numargs;
        ULONG   arg_method;
        ULONG   arg_attr;
        ULONG   arg_value;
        // args follow here
};

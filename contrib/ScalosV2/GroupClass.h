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

struct GroupInst
{
//  struct SignalSemaphore   Lock;              // lock for this structure
        struct MsgPort          *childsreplyport;
        ULONG                    GroupType;
        ULONG                    NumChilds;
        ULONG                    GroupDefHeight;
        ULONG                    GroupDefWidth;
//  ULONG                    GroupHeight;
//  ULONG                    GroupWidth;
        struct MinList           childlist;
        ULONG                    action;
};

struct ChildEntry
{
        struct MinNode               node;
        Object                      *child;
        // the important data of SCCP_Area_AskMinMax ...
        struct  SC_MinMax            sizes;
        // the important data of SCCP_Area_Show ...
        struct IBox                  bounds;
        struct IBox                  oldbounds;
};



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

struct IconPanelInst
{
        struct  SignalSemaphore iconsem;    // Semaphore to protect the icon list
        struct  SignalSemaphore newiconsem; // Semaphore to protect the new icon list
        struct  MinList iconlist;           // Icons already displayed
        struct  MinList newiconlist;        // Icons that have no position currently
        struct  IconNode *activenode;       // current active iconnode
        ULONG   itemid;                     // current highest itemid
        ULONG   idcmp;                      // idcmp flags of this object
        struct  IconNode *lastclicked;      // noded that was clicked lasttime
        ULONG   lastseconds,lastmicros;     // time from last click
};

struct IconNode
{
        struct MinNode node;
        Object *icon;
        Object *text;
        ULONG  itemid;
};


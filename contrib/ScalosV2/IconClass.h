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

struct IconInst
{
        Object  *normimage;                 // normal image
        Object  *selimage;                  // selected image
        Object  *tooltypes;
        char    *name;
        char    *defaulttool;
        ULONG   stacksize;
        WORD    windowleft;                 // values for the window of the icon
        WORD    windowtop;
        UWORD   windowwidth;
        UWORD   windowheight;
        LONG    windowoffsetx;
        LONG    windowoffsety;
        UWORD   windowviewmode;
        UWORD   flags;
        ULONG   rawtype;                    // which type was the diskobject
        UBYTE   type;                       // old icon type
};

#define ICONF_Selected          (1<<0)  // display selected or non-selected image?
#define ICONF_WindowAllFiles    (1<<1)  // ShowAllFiles for the icon-window ?
#define ICONF_NoPosition        (1<<2)  // this icon has no position


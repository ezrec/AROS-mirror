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
#include "scalos.h"
#endif /* SCALOS_H */

struct WindowInst
{
        struct SC_RenderInfo rinfo;
        char                    *title;
        WORD                    left;
        WORD                    top;
        UWORD                   width;
        UWORD                   height;                         // only valid if window is closed
        UWORD                   borderleft;
        UWORD                   bordertop;
        UWORD                   borderright;
        UWORD                   borderbottom;
        ULONG                   flags;                          // WindowFlags
        ULONG                   specialflags;
        struct                  SC_MinMax minmax;
        Object                  *rootobj;
        struct SignalSemaphore  inputsem;
        struct MinList  inputlist;
        struct MinList  regionlist;                     // list of regions for clipping
        struct Region   *oldregion;
        struct SignalSemaphore  dshowsem;       // delayed show
        struct MinList  dshowlist;

        struct Gadget   *sliderright;
        struct Gadget   *sliderbottom;
        struct Gadget   *arrowup;
        struct Gadget   *arrowdown;
        struct Gadget   *arrowleft;
        struct Gadget   *arrowright;
        struct Image    *arrowupimg;
        struct Image    *arrowdownimg;
        struct Image    *arrowleftimg;
        struct Image    *arrowrightimg;
};

struct InputNode
{
        struct MinNode  node;
        ULONG                   IDCMP;
        Object                  *obj;
};

struct RegionNode
{
        struct MinNode  node;
        struct Region   *region;
};

struct DShowNode
{
        struct MinNode  node;
        Object                  *obj;
};

#define SWSB_SliderRight                        0               // SpecialFlags
#define SWSB_SliderBottom                       1
#define SWSB_IsRefreshing                       2

#define SWSF_SliderRight                        (1L<<SWSB_SliderRight)
#define SWSF_SliderBottom                       (1L<<SWSB_SliderBottom)
#define SWSF_IsRefreshing                       (1L<<SWSB_IsRefreshing)

#define WinIDCMP  IDCMP_CLOSEWINDOW | IDCMP_REFRESHWINDOW | IDCMP_NEWSIZE

#define ARROWGADID_LEFT         1
#define ARROWGADID_RIGHT        2
#define ARROWGADID_UP           3
#define ARROWGADID_DOWN         4
#define SLIDERGADID_RIGHT       5
#define SLIDERGADID_BOTTOM      6


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

struct PanelInst
{
        ULONG   idcmp;
        struct  Region  *clipregion;
        Object  *backbitmap;
        struct  Region  *refreshregion;

        LONG    left;
        LONG    top;            // inner offset
        LONG    oldleft;
        LONG    oldtop;         // inner offset

        WORD    oldleftpos;
        WORD    oldtoppos;
        WORD    oldright;
        WORD    oldbottom;      // old sizes after resize
};



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
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <exec/memory.h>
#include <intuition/classusr.h>
#include <string.h>

#include "scalos_protos.h"
#include "ScalosIntern.h"
#include "MethodSenderClass.h"

// Allocate mem for the string and copy it into it
// returns NULL on failure
APTR AllocCopyString( char *string )
{
        int i;
        APTR mem;

        for (i=0;string[i] != 0;i++);
        if ((mem = AllocVec( ++i,1 )))
        {
                strcpy( mem,string );
        }
        return( mem );
}

// free the allocated mem
void FreeString( APTR string )
{
        if (string)
                FreeVec( string );
}

/*---------------------------------------------------------------- */
// BOOPSI Support
// it's a GetAttr replacement with returnvalue
ULONG get(Object *obj, ULONG id)
{
        ULONG   store;

        if (SC_GetAttr(id,obj,&store))
                return(store);
        return(0);
}


/* ---------------------------------------------------------------- */
// Nodehandling routines

APTR AllocNode(struct MinList *list, ULONG size)
{
        APTR ret;

        if ((ret = AllocVec(size, MEMF_CLEAR)))
        {
                AddTail((struct List *) list, (struct Node *) ret);
                return(ret);
        }
        return(NULL);
}

void FreeNode(APTR node)
{
        if (node)
        {
                Remove((struct Node *) node);
                FreeVec(node);
        }
}

void FreeAllNodes(struct MinList *list)
{
        struct Node *realnode;

        while (!(IsListEmpty((struct List *) list)))
        {
                realnode = (struct Node *) list->mlh_Head;
                Remove(realnode);
                FreeVec(realnode);
        }
}

/* ---------------------------------------------------------------- */
// RectRegion support

void OrRectRegionArgs(struct Region *region, WORD minx, WORD miny, WORD maxx, WORD maxy)
{
        struct Rectangle rect;

        rect.MinX = minx;
        rect.MinY = miny;
        rect.MaxX = maxx;
        rect.MaxY = maxy;

        OrRectRegion(region, &rect);
}

void XorRectRegionArgs(struct Region *region, WORD minx, WORD miny, WORD maxx, WORD maxy)
{
        struct Rectangle rect;

        rect.MinX = minx;
        rect.MinY = miny;
        rect.MaxX = maxx;
        rect.MaxY = maxy;

        XorRectRegion(region, &rect);
}

void AndRectRegionArgs(struct Region *region, WORD minx, WORD miny, WORD maxx, WORD maxy)
{
        struct Rectangle rect;

        rect.MinX = minx;
        rect.MinY = miny;
        rect.MaxX = maxx;
        rect.MaxY = maxy;

        AndRectRegion(region, &rect);
}

void ClearRectRegionArgs(struct Region *region, WORD minx, WORD miny, WORD maxx, WORD maxy)
{
        struct Rectangle rect;

        rect.MinX = minx;
        rect.MinY = miny;
        rect.MaxX = maxx;
        rect.MaxY = maxy;

        ClearRectRegion(region, &rect);
}


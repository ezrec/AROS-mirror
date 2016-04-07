#ifndef BWIN_PRIVATE_H
#define BWIN_PRIVATE_H

#include <exec/types.h>
#include <intuition/classusr.h>

/****************************************************************************/

struct data
{
    Object *root;
    Object *contents;
    ULONG  ID;
    ULONG  lastid;
    ULONG  save;
    ULONG  flags;

    LONG   ls;
    LONG   rs;
    LONG   ts;
    LONG   bs;
};

/***********************************************************************/

#endif

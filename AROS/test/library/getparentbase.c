/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/
#include <exec/libraries.h>
#include <aros/libcall.h>

#include LC_LIBDEFS_FILE

struct Library *GetParentBase(void)
{
    struct Library *lh = AROS_GET_LIBBASE;
    
    return GM_GETPARENTBASEID2(lh);
}

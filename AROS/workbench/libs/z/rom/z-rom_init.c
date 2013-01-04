/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$

    Desc: z-rom library
    Lang: english
*/

#include <proto/exec.h>

#include LC_LIBDEFS_FILE

static LONG ZROMInit(LIBBASETYPE *LIBBASE)
{
    DOSBase = (struct DosLibrary *)TaggedOpenLibrary(TAGGEDOPEN_DOS);

    if (!DOSBase)
        return 0;

    InitSemaphore(&LIBBASE->lock);
    NEWLIST(&LIBBASE->bptr2gzf);

    return 1;
}

ADD2INITLIB(ZROMInit, 0);

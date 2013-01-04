/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$

    Desc: z-rom.library api entry points
    Lang: english
*/

#include <proto/dos.h>
#include <aros/debug.h>

#include "z-rom_intern.h"
#include "gzguts.h"

gzFile gz_open(const void *, IPTR, const char *, struct DosLibrary * dosBase);

struct BPTR2GZFNode
{
    struct Node n;
    BPTR key;
    APTR val;
};

VOID gzRegister(BPTR file, gzFile gzf, struct ZROMLibrary * ZROMBase)
{
    ObtainSemaphore(&ZROMBase->lock);

    struct BPTR2GZFNode * n = AllocMem(sizeof(struct BPTR2GZFNode), MEMF_ANY);
    n->key = file;
    n->val = gzf;

    AddHead(&ZROMBase->bptr2gzf, (struct Node *)n);
    ZROMBase->lastUsed = n;

    ReleaseSemaphore(&ZROMBase->lock);
}

VOID gzUnregister(BPTR file, struct ZROMLibrary * ZROMBase)
{
    struct BPTR2GZFNode * n = NULL;

    ObtainSemaphore(&ZROMBase->lock);
    ForeachNode(&ZROMBase->bptr2gzf, n)
    {
        if (n->key == file)
        {
            Remove((struct Node *)n);
            if (ZROMBase->lastUsed == n)
                ZROMBase->lastUsed = NULL;
            FreeMem(n, sizeof(struct BPTR2GZFNode));

            break;
        }
    }
    ReleaseSemaphore(&ZROMBase->lock);
}

gzFile gzGetGZFile(BPTR file, struct ZROMLibrary * ZROMBase)
{
    /* Use local variable to snapshot global last used */
    struct BPTR2GZFNode * n = (struct BPTR2GZFNode *)ZROMBase->lastUsed;

    /* Try quick path */
    if (n && n->key == file)
        return (gzFile)n->val;

    /* Scan the complete list */
    ObtainSemaphore(&ZROMBase->lock);
    ForeachNode(&ZROMBase->bptr2gzf, n)
    {
        if (n->key == file)
        {
            ZROMBase->lastUsed = n;
            ReleaseSemaphore(&ZROMBase->lock);
            return (gzFile)n->val;
        }
    }
    ReleaseSemaphore(&ZROMBase->lock);

    return NULL;
}

#define GZGETFILE(f)    gzFile gzf = gzGetGZFile(f, ZROMBase);

/*****************************************************************************

    NAME */
#include <proto/z-rom.h>

        AROS_LH2(BPTR, gzOpen,

/*  SYNOPSIS */
        AROS_LHA(CONST_STRPTR, name,       D1),
        AROS_LHA(LONG,         accessMode, D2),

/*  LOCATION */
        struct ZROMLibrary *, ZROMBase, 5, ZROM)

/*  FUNCTION
        Opens a file for read.

    INPUTS
        name       - NUL terminated name of the file.
        accessMode - MODE_OLDFILE   - open existing file.

    RESULT
        Handle to the file or 0 if the file couldn't be opened.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    gzFile gzf = gz_open(name, -1, "r", DOSBase);

    if (gzf != NULL)
    {
        gz_statep state = (gz_statep)gzf;
        gzRegister(state->fd, gzf, ZROMBase);
        return state->fd;
    }

    return BNULL;

    AROS_LIBFUNC_EXIT
} /* gzOpen */

/*****************************************************************************

    NAME */
#include <proto/z-rom.h>

        AROS_LH1(BOOL, gzClose,

/*  SYNOPSIS */
        AROS_LHA(BPTR, file, D1),

/*  LOCATION */
        struct ZROMLibrary *, ZROMBase, 6, ZROM)

/*  FUNCTION
        Close a filehandle opened with gzOpen().

    INPUTS
        file  --  filehandle

    RESULT
        0 if there was an error. != 0 on success.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    GZGETFILE(file)

    gzUnregister(file, ZROMBase);

    return !!gzclose_r(gzf);

    AROS_LIBFUNC_EXIT
} /* gzClose */

/*****************************************************************************

    NAME */
#include <proto/z-rom.h>

        AROS_LH3(LONG, gzRead,

/*  SYNOPSIS */
        AROS_LHA(BPTR, file,   D1),
        AROS_LHA(APTR, buffer, D2),
        AROS_LHA(LONG, length, D3),

/*  LOCATION */
        struct ZROMLibrary *, ZROMBase, 7, ZROM)

/*  FUNCTION
        Read some data from a given file.

    INPUTS
        file   - filehandle
        buffer - pointer to buffer for the data
        length - number of bytes to read. The filesystem is
                 advised to try to fulfill the request as good
                 as possible.

    RESULT
        The number of bytes actually read, 0 if the end of the
        file was reached, -1 if an error happened. IoErr() will
        give additional information in that case.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    GZGETFILE(file)

    return gzread(gzf, buffer, length);

    AROS_LIBFUNC_EXIT
} /* gzRead */

/*****************************************************************************

    NAME */
#include <proto/z-rom.h>

        AROS_LH3(LONG, gzSeek,

/*  SYNOPSIS */
        AROS_LHA(BPTR, file,     D1),
        AROS_LHA(LONG, position, D2),
        AROS_LHA(LONG, mode,     D3),

/*  LOCATION */
        struct ZROMLibrary *, ZROMBase, 11, ZROM)

/*  FUNCTION
        Changes the current read/write position in a file and/or
        reads the current position, e.g to get the current position
        do a Seek(file,0,OFFSET_CURRENT).

        This function may fail (obviously) on certain devices such
        as pipes or console handlers.

    INPUTS
        file     - filehandle
        position - relative offset in bytes (positive, negative or 0).
        mode     - Where to count from. Either OFFSET_BEGINNING,
                   OFFSET_CURRENT or OFFSET_END.

    RESULT
        Absolute position in bytes before the Seek(), -1 if an error
        happened. IoErr() will give additional information in that case.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    GZGETFILE(file)
    int whence;
    LONG ret;
    LONG oldpos;

    switch (mode) {
    case OFFSET_CURRENT  : whence = SEEK_CUR; break;
    case OFFSET_END      : whence = SEEK_END; break;
    case OFFSET_BEGINNING: whence = SEEK_SET; break;
    default: return -1;
    }

    oldpos = (LONG)gzseek(gzf, 0, SEEK_CUR);

    ret = (LONG)gzseek(gzf, (z_off_t)position, whence);
    if (ret < 0)
        return -1;

    return oldpos;

    AROS_LIBFUNC_EXIT
} /* gzSeek */

/*****************************************************************************

    NAME */
#include <proto/z-rom.h>

        AROS_LH4(LONG, gzSetVBuf,

/*  SYNOPSIS */
        AROS_LHA(BPTR  , file, D1),
        AROS_LHA(STRPTR, buff, D2),
        AROS_LHA(LONG  , type, D3),
        AROS_LHA(LONG  , size, D4),

/*  LOCATION */
        struct ZROMLibrary *, ZROMBase, 61, ZROM)

/*  FUNCTION
        Changes the buffer size for a filehandle.

    INPUTS
        file - Filehandle
        buff - ignored
        type - ignored
        size - size of buffer

    RESULT
        0 if operation succeeded.

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    GZGETFILE(file)

    SetVBuf(file, buff, type, size);

    gzbuffer(gzf, size);

    return 0;

    AROS_LIBFUNC_EXIT
} /* gzSetVBuf */

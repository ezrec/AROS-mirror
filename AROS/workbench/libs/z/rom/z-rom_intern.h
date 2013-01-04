#ifndef Z_ROM_INTERN_H
#define Z_ROM_INTERN_H

#include <exec/libraries.h>
#include <dos/dosextens.h>

struct ZROMLibrary
{
    struct Library          Base;
    struct DosLibrary       *dosBase;

    struct SignalSemaphore  lock;
    struct List             bptr2gzf;
    APTR                    lastUsed;
};

#define DOSBase     (ZROMBase->dosBase)

#endif /* Z_ROM_INTERN_H */

#ifndef CLIB_PTPLAY_PROTOS_H
#define CLIB_PTPLAY_PROTOS_H

/*
    *** Automatically generated from 'ptplay.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <libraries/ptplay.h>
#include <utility/tagitem.h>
AROS_LP4(APTR, PtInit,
         AROS_LPA(UBYTE *, buf, A1),
         AROS_LPA(LONG, bufsize, D0),
         AROS_LPA(LONG, freq, D1),
         AROS_LPA(ULONG, modtype, D2),
         LIBBASETYPEPTR, PtplayBase, 5, Ptplay
);
AROS_LP8(VOID, PtRender,
         AROS_LPA(APTR, mod, A0),
         AROS_LPA(BYTE *, destbuf1, A1),
         AROS_LPA(BYTE *, destbuf2, A2),
         AROS_LPA(LONG, bufmodulo, D0),
         AROS_LPA(LONG, numsmp, D1),
         AROS_LPA(LONG, scale, D2),
         AROS_LPA(LONG, depth, D3),
         AROS_LPA(LONG, channels, D4),
         LIBBASETYPEPTR, PtplayBase, 6, Ptplay
);
AROS_LP3(ULONG, PtTest,
         AROS_LPA(CONST_STRPTR, filename, A0),
         AROS_LPA(UBYTE *, buf, A1),
         AROS_LPA(LONG, bufsize, D0),
         LIBBASETYPEPTR, PtplayBase, 7, Ptplay
);
AROS_LP1(VOID, PtCleanup,
         AROS_LPA(APTR, mod, A0),
         LIBBASETYPEPTR, PtplayBase, 8, Ptplay
);
AROS_LP2(VOID, PtSetAttrs,
         AROS_LPA(APTR, mod, A0),
         AROS_LPA(struct TagItem *, taglist, A1),
         LIBBASETYPEPTR, PtplayBase, 9, Ptplay
);
AROS_LP3(ULONG, PtGetAttr,
         AROS_LPA(APTR, mod, A0),
         AROS_LPA(ULONG, tagitem, D0),
         AROS_LPA(ULONG *, StoragePtr, A1),
         LIBBASETYPEPTR, PtplayBase, 10, Ptplay
);
AROS_LP2(ULONG, PtSeek,
         AROS_LPA(APTR, mod, A0),
         AROS_LPA(ULONG, time, D0),
         LIBBASETYPEPTR, PtplayBase, 11, Ptplay
);

#endif /* CLIB_PTPLAY_PROTOS_H */

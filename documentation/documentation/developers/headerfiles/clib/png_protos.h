#ifndef CLIB_PNG_PROTOS_H
#define CLIB_PNG_PROTOS_H

/*
    *** Automatically generated from 'png.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
AROS_LP1(LONG, PNG_CheckSig,
         AROS_LPA(STRPTR, name, A0),
         LIBBASETYPEPTR, PNGBase, 6, PNG
);
AROS_LP4(APTR, PNG_LoadImageFH,
         AROS_LPA(BPTR, fh, A0),
         AROS_LPA(STRPTR*, chunkstoread, A1),
         AROS_LPA(APTR*, chunkstore, A2),
         AROS_LPA(BOOL, makeARGB, D0),
         LIBBASETYPEPTR, PNGBase, 7, PNG
);
AROS_LP4(APTR, PNG_LoadImage,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(STRPTR*, chunkstoread, A1),
         AROS_LPA(APTR*, chunkstore, A2),
         AROS_LPA(BOOL, makeARGB, D0),
         LIBBASETYPEPTR, PNGBase, 8, PNG
);
AROS_LP5(APTR, PNG_LoadImageMEM,
         AROS_LPA(APTR, mem, A0),
         AROS_LPA(ULONG, memsize, D0),
         AROS_LPA(STRPTR*, chunkstoread, A1),
         AROS_LPA(APTR*, chunkstore, A2),
         AROS_LPA(BOOL, makeARGB, D1),
         LIBBASETYPEPTR, PNGBase, 9, PNG
);
AROS_LP5(void, PNG_GetImageInfo,
         AROS_LPA(APTR, pnghandle, A0),
         AROS_LPA(LONG*, widthptr, A1),
         AROS_LPA(LONG*, heightptr, A2),
         AROS_LPA(LONG*, depthptr, A3),
         AROS_LPA(LONG*, typeptr, A4),
         LIBBASETYPEPTR, PNGBase, 10, PNG
);
AROS_LP3(void, PNG_GetImageData,
         AROS_LPA(APTR, pnghandle, A0),
         AROS_LPA(APTR*, gfxdataptr, A1),
         AROS_LPA(APTR*, paldataptr, A2),
         LIBBASETYPEPTR, PNGBase, 11, PNG
);
AROS_LP1(void, PNG_FreeImage,
         AROS_LPA(APTR, pnghandle, A0),
         LIBBASETYPEPTR, PNGBase, 12, PNG
);
AROS_LP1(void, PNG_FreeChunk,
         AROS_LPA(APTR, chunk, A0),
         LIBBASETYPEPTR, PNGBase, 13, PNG
);
AROS_LP3(void, PNG_GetChunkInfo,
         AROS_LPA(APTR, chunk, A0),
         AROS_LPA(APTR*, dataptr, A1),
         AROS_LPA(ULONG*, sizeptr, A2),
         LIBBASETYPEPTR, PNGBase, 14, PNG
);

#endif /* CLIB_PNG_PROTOS_H */

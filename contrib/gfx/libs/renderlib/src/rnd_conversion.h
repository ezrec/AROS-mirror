
#ifndef _RND_CONV_H
#define _RND_CONV_H

#include <utility/tagitem.h>
#include <exec/memory.h>
#include <render/renderhooks.h>
#include <graphics/gfx.h>
#include "rnd_palette.h"

LIBAPI ULONG Chunky2RGBA(UBYTE *src, UWORD w, UWORD h, ULONG *dst, RNDPAL *pal, struct TagItem *tags);
LIBAPI void Planar2ChunkyA(UWORD **planetab, UWORD bytewidth, UWORD rows, UWORD depth, UWORD bytesperrow, UBYTE *dst, struct TagItem *tags);
LIBAPI void Chunky2BitMapA(UBYTE *src, UWORD sx, UWORD sy, UWORD width, UWORD height, struct BitMap *bm, UWORD dx, UWORD dy, struct TagItem *tags);
LIBAPI void RemapArrayA(UBYTE *src, UWORD width, UWORD height, UBYTE *dst, UBYTE *pentab, struct TagItem *tags);
LIBAPI void CreatePenTableA(UBYTE *src, RNDPAL *srcpal, UWORD width, UWORD height, RNDPAL *dstpal, UBYTE *pentab, struct TagItem *tags);

#endif

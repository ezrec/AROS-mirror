
#ifndef _RND_ALPHA_H
#define _RND_ALPHA_H

#include <utility/tagitem.h>
#include <exec/memory.h>
#include <exec/nodes.h>
#include "rnd_palette.h"

LIBAPI void InsertAlphaChannelA(UBYTE *chunky, UWORD width, UWORD height, ULONG *rgb, struct TagItem *tags);
LIBAPI void ExtractAlphaChannelA(ULONG *rgb, UWORD width, UWORD height, UBYTE *chunky, struct TagItem *tags);
LIBAPI void ApplyAlphaChannelA(ULONG *src, UWORD width, UWORD height, ULONG *dst, struct TagItem *tags);
LIBAPI void MixRGBArrayA(ULONG *src, UWORD width, UWORD height, ULONG *dst, UWORD ratio, struct TagItem *tags);
LIBAPI void CreateAlphaArrayA(ULONG *rgb, UWORD width, UWORD height, struct TagItem *tags);
LIBAPI void MixAlphaChannelA(ULONG *rgb1, ULONG *rgb2, UWORD width, UWORD height, ULONG *dest, struct TagItem *tags);
LIBAPI void TintRGBArrayA(ULONG *src, UWORD width, UWORD height, ULONG rgb, UWORD ratio, ULONG *dest, struct TagItem *tags);

#endif


#ifndef _RND_MAPPING_H
#define _RND_MAPPING_H

#include <utility/tagitem.h>
#include <exec/memory.h>
#include <exec/nodes.h>
#include "rnd_palette.h"

typedef RNDPAL RNDMAP;

LIBAPI RNDMAP *CreateMapEngineA(RNDPAL *palette, struct TagItem *tags);
LIBAPI void DeleteMapEngine(RNDMAP *mapengine);
LIBAPI ULONG MapRGBArrayA(RNDMAP *engine, ULONG *rgb, UWORD width, UWORD height, UBYTE *chunky, struct TagItem *tags);
LIBAPI ULONG MapChunkyArrayA(RNDMAP *engine, UBYTE *src, RNDPAL *palette, UWORD width, UWORD height, UBYTE *dest, struct TagItem *tags);

#endif

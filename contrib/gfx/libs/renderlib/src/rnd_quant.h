
#ifndef _RND_QUANT_H
#define _RND_QUANT_H

#include <utility/tagitem.h>
#include <exec/memory.h>
#include "rnd_histogram.h"
#include "rnd_palette.h"

LIBAPI ULONG ExtractPaletteA(RNDHISTO *h, RNDPAL *palette, UWORD numcol, struct TagItem *tags);

#endif

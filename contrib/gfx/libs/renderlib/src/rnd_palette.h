
#ifndef _RND_PALETTE_H
#define _RND_PALETTE_H

#include <utility/tagitem.h>
#include <exec/memory.h>

typedef struct Palette
{
	ULONG table[256];
	struct SignalSemaphore lock;
	APTR rmh;
	WORD *p2table;
	ULONG p2mask, p2mask2;
	UWORD numcolors;
	UWORD type;

} RNDPAL;

LIBAPI RNDPAL *CreatePaletteA(struct TagItem *tags);
LIBAPI void DeletePalette(RNDPAL *palette);
LIBAPI void ImportPaletteA(RNDPAL *palette, APTR coltab, UWORD numcol, struct TagItem *tags);
LIBAPI void ExportPaletteA(RNDPAL *palette, ULONG *coltab, struct TagItem *tags);
LIBAPI void FlushPalette(RNDPAL *palette);
LIBAPI WORD *GetP2Table(RNDPAL *p);
LIBAPI LONG BestPen(RNDPAL *p, ULONG rgb);
LIBAPI WORD P2Lookup(RNDPAL *pal, ULONG rgb);
LIBAPI WORD P2Lookup2(RNDPAL *pal, WORD r, WORD g, WORD b);
LIBAPI ULONG SortPaletteA(RNDPAL *p, ULONG mode, struct TagItem *tags);
LIBAPI ULONG GetPaletteAttrs(RNDPAL *p, ULONG args);


#endif

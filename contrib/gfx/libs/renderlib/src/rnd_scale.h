
#ifndef _RND_SCALE_H
#define _RND_SCALE_H

#include <utility/tagitem.h>
#include <exec/memory.h>
#include "rnd_memory.h"

struct ScaleData
{
	APTR data;
	LONG cy;
	FLOAT sourcey;
	FLOAT deltax, deltay;
};

typedef struct ScaleEngine
{
	RNDMH *rmh;
	void (*scalefunc)(struct ScaleEngine *, struct ScaleData *, APTR, APTR, LONG, WORD);
	BOOL (*initfunc)(struct ScaleEngine *, struct ScaleData *);
	void (*exitfunc)(struct ScaleEngine *, struct ScaleData *);
	UWORD sw, sh, dw, dh;
	WORD coords[8];
	UWORD format;
	UWORD usecoords;
	UWORD bpp;			/* bytes per pixel */

} RNDSCALE;

LIBAPI RNDSCALE *CreateScaleEngineA(UWORD sw, UWORD sh, UWORD dw, UWORD dh, struct TagItem *tags);
LIBAPI void DeleteScaleEngine(RNDSCALE *engine);
LIBAPI ULONG ScaleA(RNDSCALE *sce, UBYTE *src, UBYTE *dst, struct TagItem *tags);
LIBAPI UWORD ScaleOrdinate(UWORD start, UWORD dest, UWORD ordinate);

#endif

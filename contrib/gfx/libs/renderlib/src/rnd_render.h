
#ifndef _RND_RENDER_H
#define _RND_RENDER_H

#include <utility/tagitem.h>
#include <exec/memory.h>
#include <render/renderhooks.h>
#include <graphics/gfx.h>
#include "rnd_scale.h"
#include "rnd_palette.h"

LIBAPI ULONG RenderA(ULONG *src, UWORD w, UWORD h, UBYTE *dst, RNDPAL *pal, struct TagItem *tags);
LIBAPI ULONG ConvertChunkyA(UBYTE *src, RNDPAL *srcpal, UWORD width, UWORD height, UBYTE *dst, RNDPAL *dstpal, struct TagItem *tags);

struct RenderData
{
	APTR src;				/* source data ptr */
	APTR dst;				/* dest data ptr */
	RNDPAL *srcpal;			/* src palette */
	RNDPAL *dstpal;			/* dst palette */
	UBYTE *pentab;			/* pentable for secodary pen conversion */
	RNDSCALE *scaleengine;	/* optional scaleengine */
	APTR rmh;				/* preferred memory manager */
	LONG width;
	LONG height;
	LONG srcbpp;			/* bytes per pixel, source */
	LONG dstbpp;			/* bytes per pixel, dest */
	LONG tsw, tdw;			/* total source/dest buffer width */

	struct Hook *linehook;
	struct Hook *proghook;
	struct RND_ProgressMessage progmsg;
	struct RND_LineMessage fetchmsg;
	struct RND_LineMessage rendermsg;

	WORD *ditherbuf;

	ULONG (*renderyfunc)(struct RenderData *data);
	void (*renderxfunc)(struct RenderData *data, APTR src, APTR dst, LONG width);

	UWORD flipflop;		/* for zig-zag dithering */
	WORD Ar, Ag, Ab, Br, Bg, Bb, Cr, Cg, Cb;

	struct ScaleData scdata;
	UBYTE pens[256];
	
	ULONG bgcolor;
	ULONG bgpen;
	
	BYTE *randtab;
	WORD randi;
	UWORD dithermode;
	
};

#endif


#ifndef _RND_HISTO_H
#define _RND_HISTO_H

#include <utility/tagitem.h>
#include <exec/memory.h>
#include "rnd_palette.h"

#define HSTYPE_MASK	15
#define RNDTREEMEMBLOCK_NUMNODES	64

struct RNDHistoEntry
{
	ULONG rgb;
	ULONG count;
};

struct RNDTreeNode
{
	struct RNDTreeNode *left, *right;
	struct RNDHistoEntry entry;
};

struct RNDTreeMemBlock
{
	struct RNDTreeMemBlock *next;
	ULONG free;		/* number of remaining free nodes in this block */
	struct RNDTreeNode nodes[RNDTREEMEMBLOCK_NUMNODES];
};

typedef struct Histogram
{
	struct SignalSemaphore lock;
	APTR rmh;
	APTR table;
	struct RNDTreeMemBlock *memlist;
	struct RNDTreeNode *root;
	ULONG numcolors;
	ULONG numpixels;
	ULONG rgbmask;
	UWORD type;
	UWORD bitspergun;

} RNDHISTO;


LIBAPI RNDHISTO *CreateHistogramA(struct TagItem *tags);
LIBAPI void DeleteHistogram(RNDHISTO *h);
LIBAPI ULONG AddRGB(RNDHISTO *h, ULONG rgb, ULONG count);
LIBAPI ULONG QueryHistogram(RNDHISTO *h, Tag tag);
LIBAPI ULONG AddRGBImageA(RNDHISTO *h, ULONG *rgb, UWORD width, UWORD height, struct TagItem *tags);
LIBAPI ULONG AddChunkyImageA(RNDHISTO *histogram, UBYTE *chunky, UWORD width, UWORD height, RNDPAL *palette, struct TagItem *tags);
LIBAPI struct RNDHistoEntry **CreateHistogramPointerArray(RNDHISTO *histogram);
LIBAPI ULONG CountRGB(RNDHISTO *h, ULONG rgb);
LIBAPI ULONG CountHistogram(RNDHISTO *h);
LIBAPI ULONG AddHistogramA(RNDHISTO *dst, RNDHISTO *src, struct TagItem *tags);

#endif

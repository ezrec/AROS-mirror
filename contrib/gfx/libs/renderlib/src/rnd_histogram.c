
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>


/************************************************************************** 
**
**	createhistogram
*/

LIBAPI RNDHISTO *CreateHistogramA(struct TagItem *tags)
{
	APTR rmh;
	RNDHISTO *h;
	UWORD type;
	
	type = (UWORD) GetTagData(RND_HSType, HSTYPE_15BIT_TURBO, tags);
	switch (type)
	{
		case HSTYPE_12BIT:
		case HSTYPE_15BIT:
		case HSTYPE_18BIT:
		case HSTYPE_21BIT:
		case HSTYPE_24BIT:
			break;

		case HSTYPE_12BIT_TURBO:
		case HSTYPE_15BIT_TURBO:
		case HSTYPE_18BIT_TURBO:
			type &= HSTYPE_MASK;		/* turbo currently not supported */
			break;

		default:
			return NULL;
	}

	rmh = (APTR) GetTagData(RND_RMHandler, NULL, tags);
	h = AllocRenderVecClear(rmh, sizeof(RNDHISTO));
	if (h)
	{
		ULONG mask;
		InitSemaphore(&h->lock);
		h->type = type;
		h->rmh = rmh;
		h->bitspergun = type & HSTYPE_MASK;
		mask = (0xff << (8 - h->bitspergun)) & 0xff;
		h->rgbmask = (mask << 16) | (mask << 8) | mask;
	}
	return h;
}


/************************************************************************** 
**
**	deletehistogram
*/

LIBAPI void DeleteHistogram(RNDHISTO *h)
{
	if (h)
	{
		struct RNDTreeMemBlock *node, *nextnode;
	
		node = h->memlist;
		while (node)
		{
			nextnode = node->next;
			FreeRenderMem(h->rmh, node, sizeof(struct RNDTreeMemBlock));
			node = nextnode;
		}
	
		FreeRenderVec((ULONG *) h);
	}
}


/************************************************************************** 
**
**	queryhistogram
*/

LIBAPI ULONG QueryHistogram(RNDHISTO *h, Tag tag)
{
	ULONG result = 0xffffffff;
	switch (tag)
	{
		case RND_NumColors:
			result = h->numcolors;
			break;
		case RND_NumPixels:
			result = h->numpixels;
			break;
		case RND_RMHandler:
			result = (ULONG) h->rmh;
			break;
		case RND_HSType:
			result = (ULONG) h->type;
			break;
	}
	return result;
}



/* 
**	tree histogram
*/

static struct RNDTreeNode *alloctreenode(RNDHISTO *h)
{
	struct RNDTreeMemBlock *mn = h->memlist;

	if (mn)
	{
		if (mn->free)
		{
			return mn->nodes + --mn->free;
		}
	}

	mn = AllocRenderMem(h->rmh, sizeof(struct RNDTreeMemBlock));
	if (mn)
	{
		mn->next = h->memlist;
		h->memlist = mn;
		mn->free = RNDTREEMEMBLOCK_NUMNODES - 1;
		return mn->nodes + RNDTREEMEMBLOCK_NUMNODES - 1;
	}
	
	return NULL;
}


static LONG addrgb_tree(RNDHISTO *h, ULONG rgb, ULONG count)
{
	struct RNDTreeNode **nodeptr = &h->root;
	struct RNDTreeNode *node;

	ULONG rgbmask = h->rgbmask;	
	ULONG chkmask = 0x800000;
	
	rgb &= rgbmask;

	while ((node = *nodeptr))
	{
		if (node->entry.rgb == rgb)
		{
			node->entry.count += count;
			h->numpixels += count;
			return ADDH_SUCCESS;
		}

		while (!(chkmask & rgbmask)) chkmask >>= 1;
	
		if (rgb & chkmask)
		{
			nodeptr = &node->left;
		}
		else
		{
			nodeptr = &node->right;
		}

		chkmask >>= 1;
	}

	node = alloctreenode(h);
	if (node)
	{
		*nodeptr = node;
		node->left = NULL;
		node->right = NULL;
		node->entry.rgb = rgb;
		node->entry.count = count;
		h->numpixels += count;
		h->numcolors++;
		return ADDH_SUCCESS;
	}
	
	return ADDH_NOT_ENOUGH_MEMORY;
}


/************************************************************************** 
**
**	addrgb
*/

LIBAPI ULONG AddRGB(RNDHISTO *h, ULONG rgb, ULONG count)
{
	ULONG result = ADDH_NO_DATA;
	if (h && count)
	{
		ObtainSemaphore(&h->lock);
		result = addrgb_tree(h, rgb, count);
		ReleaseSemaphore(&h->lock);
	}
	return result;
}

/************************************************************************** 
**
**	addrgbimage
*/

LIBAPI ULONG AddRGBImageA(RNDHISTO *h, ULONG *rgb, UWORD width, UWORD height, struct TagItem *tags)
{
	ULONG result = ADDH_NO_DATA;
	if (h && rgb && width && height)
	{
		struct RND_ProgressMessage progmsg;
		struct Hook *proghook = (struct Hook *) GetTagData(RND_ProgressHook, NULL, tags);
		UWORD totw = (UWORD) GetTagData(RND_SourceWidth, width, tags);
		UWORD y, x;

		progmsg.RND_PMsg_type = PMSGTYPE_LINES_ADDED;
		progmsg.RND_PMsg_total = height;

		ObtainSemaphore(&h->lock);
	
		for (y = 0; y < height; ++y)
		{
			for (x = 0; x < width; ++x)
			{
			#ifdef RENDER_LITTLE_ENDIAN
			    	ULONG pix = *rgb++;
				result = addrgb_tree(h, AROS_BE2LONG(pix), 1);
			#else
				result = addrgb_tree(h, *rgb++, 1);
			#endif
				if (result != ADDH_SUCCESS) goto fail;
			}

			if (proghook)
			{
				progmsg.RND_PMsg_count = y;
				if (!CallHookPkt(proghook, h, &progmsg))
				{
					result = ADDH_CALLBACK_ABORTED;
					goto fail;
				}
			}
			
			rgb += totw - width;
		}			
fail:
		ReleaseSemaphore(&h->lock);
	}
	return result;
}


/************************************************************************** 
**
**	addchunkyimage
*/

LIBAPI ULONG AddChunkyImageA(RNDHISTO *h, UBYTE *chunky, 
	UWORD width, UWORD height, RNDPAL *p, struct TagItem *tags)
{
	ULONG result = ADDH_NO_DATA;
	if (h && p && chunky && width && height)
	{
		struct RND_ProgressMessage progmsg;
		struct Hook *proghook = (struct Hook *) GetTagData(RND_ProgressHook, NULL, tags);
		UWORD totw = (UWORD) GetTagData(RND_SourceWidth, width, tags);
		UWORD y, x;

		progmsg.RND_PMsg_type = PMSGTYPE_LINES_ADDED;
		progmsg.RND_PMsg_total = height;

		ObtainSemaphore(&h->lock);
	
		for (y = 0; y < height; ++y)
		{
			for (x = 0; x < width; ++x)
			{
				result = addrgb_tree(h, p->table[*chunky++], 1);
				if (result != ADDH_SUCCESS) goto fail;
			}

			if (proghook)
			{
				progmsg.RND_PMsg_count = y;
				if (!CallHookPkt(proghook, h, &progmsg))
				{
					result = ADDH_CALLBACK_ABORTED;
					goto fail;
				}
			}

			chunky += totw - width;
		}			
fail:
		ReleaseSemaphore(&h->lock);
	}
	return result;
}


/************************************************************************** 
**
**	createhistogrampointerarray
*/

static struct RNDHistoEntry **scantree(struct RNDTreeNode *node, struct RNDHistoEntry **destp)
{
	if (node->left) destp = scantree(node->left, destp);
	if (node->right) destp = scantree(node->right, destp);
	*destp++ = &node->entry;
	return destp;
}

LIBAPI struct RNDHistoEntry **CreateHistogramPointerArray(RNDHISTO *h)
{
	struct RNDHistoEntry **array = NULL;
	ULONG numcolors;
	ObtainSemaphoreShared(&h->lock);
	numcolors = QueryHistogram(h, RND_NumColors);	
	if (numcolors > 0 && numcolors != 0xffffffff)
	{
		array = AllocRenderVec(h->rmh, numcolors * sizeof(struct RNDHistoEntry *));
		if (array)
		{
			scantree(h->root, array);
		}
	}
	ReleaseSemaphore(&h->lock);
	return array;
}


/************************************************************************** 
**
**	countrgb
*/

static ULONG findrgb_tree(RNDHISTO *h, ULONG rgb)
{
	struct RNDTreeNode *node = h->root;

	ULONG rgbmask = h->rgbmask;	
	ULONG chkmask = 0x800000;

	rgb &= rgbmask;
	
	while (node)
	{
		if (node->entry.rgb == rgb)
		{
			return (ULONG) node->entry.count;
		}

		while (!(chkmask & rgbmask)) chkmask >>= 1;
		
		if (rgb & chkmask)
		{
			node = node->left;
		}
		else
		{
			node = node->right;
		}
		
		chkmask >>= 1;
	}
	
	return 0;
}

LIBAPI ULONG CountRGB(RNDHISTO *h, ULONG rgb)
{
	ULONG result;
	ObtainSemaphoreShared(&h->lock);
	result = findrgb_tree(h, rgb);
	ReleaseSemaphore(&h->lock);
	return result;
}


/************************************************************************** 
**
**	counthistogram
*/

LIBAPI ULONG CountHistogram(RNDHISTO *h)
{
	return h->numcolors;
}


/************************************************************************** 
**
**	addhistogram
*/

static ULONG addtree(struct RNDTreeNode *node, RNDHISTO *dest, ULONG weight)
{
	ULONG result = ADDH_SUCCESS;

	if (node->left)
	{
		result = addtree(node->left, dest, weight);
	}

	if (result == ADDH_SUCCESS)
	{
		if (node->right) result = addtree(node->right, dest, weight);
	}
	
	if (result == ADDH_SUCCESS)
	{
		result = addrgb_tree(dest, node->entry.rgb, node->entry.count * weight);
	}

	return result;
}

LIBAPI ULONG AddHistogramA(RNDHISTO *dst, RNDHISTO *src, struct TagItem *tags)
{
	ULONG result = ADDH_NO_DATA;
	if (dst && src)
	{
		ULONG weight = GetTagData(RND_Weight, 1, tags);
		ObtainSemaphoreShared(&src->lock);
		ObtainSemaphore(&dst->lock);
		result = addtree(src->root, dst, weight);
		ReleaseSemaphore(&dst->lock);
		ReleaseSemaphore(&src->lock);
	}
	return result;
}


#include <math.h>
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>

/*
**	quantization tree node
*/

struct quantizenode
{
	struct quantizenode *left, *right;			/* child nodes */
	struct RNDHistoEntry **lower, **upper;		/* range */
	ULONG numentries;	/* number of entries represented by this node */
	ULONG min, max;		/* min max values of this component */
	FLOAT diversity;	/* separation criterium */
	FLOAT cc_mid[3];	/* average color represented by this node */
	ULONG shift, mask;
};


/*-------------------------------------------------------------------

	quantizenode = CreateNode(memhandler, lower, upper)
	create a new quantization node for the given field.

-------------------------------------------------------------------*/

static struct quantizenode *CreateNode(RNDMH *rmh, struct RNDHistoEntry **lower, struct RNDHistoEntry **upper)
{
	struct quantizenode *qn = AllocRenderMem(rmh, sizeof(struct quantizenode));
	if (qn)
	{
		FLOAT diversity[3];
		FLOAT a;
		LONG c1, c2, c3;
		LONG min[3];
		LONG max[3];
		FLOAT numpixel = 0;
		struct RNDHistoEntry **htp;
		FLOAT maxd;
		ULONG shift, mask;
		ULONG component;

		qn->cc_mid[0] = 0;
		qn->cc_mid[1] = 0;
		qn->cc_mid[2] = 0;
	
		qn->left = NULL;
		qn->right = NULL;
		qn->lower = lower;
		qn->upper = upper;
		qn->numentries = upper - lower + 1;

		diversity[0] = 0;
		diversity[1] = 0;
		diversity[2] = 0;
		
		min[0] = 256;
		min[1] = 256;
		min[2] = 256;

		max[0] = -1;
		max[1] = -1;
		max[2] = -1;

		htp = lower;
		while (htp <= upper)
		{
			ULONG rgb = (*htp)->rgb;
			ULONG count = (*htp)->count;
			c1 = (rgb & 0xff0000) >> 16;
			c2 = (rgb & 0x00ff00) >> 8;
			c3 = rgb & 0x0000ff;
			
			if (c1 < min[0]) min[0] = c1;
			if (c1 > max[0]) max[0] = c1;
			if (c2 < min[1]) min[1] = c2;
			if (c2 > max[1]) max[1] = c2;
			if (c3 < min[2]) min[2] = c3;
			if (c3 > max[2]) max[2] = c3;

			qn->cc_mid[0] += (FLOAT) c1 * count;
			qn->cc_mid[1] += (FLOAT) c2 * count;
			qn->cc_mid[2] += (FLOAT) c3 * count;

			htp++;
			numpixel += count;
		}
		
		qn->cc_mid[0] /= numpixel;
		qn->cc_mid[1] /= numpixel;
		qn->cc_mid[2] /= numpixel;

		htp = lower;
		while (htp <= upper)
		{
			ULONG rgb = (*htp)->rgb;
			c1 = (rgb & 0xff0000) >> 16;
			c2 = (rgb & 0x00ff00) >> 8;
			c3 = rgb & 0x0000ff;
			
			c1 -= qn->cc_mid[0];
			c2 -= qn->cc_mid[1];
			c3 -= qn->cc_mid[2];

		/*	a = sqrt((DOUBLE) (*htp)->count);	*/
			a = pow((DOUBLE) (*htp)->count, 0.35);

			diversity[0] += (FLOAT) (c1 * c1) * a;
			diversity[1] += (FLOAT) (c2 * c2) * a;
			diversity[2] += (FLOAT) (c3 * c3) * a;
			htp++;
		}

		maxd = diversity[0]; component = 0; mask = 0xff0000; shift = 16;
		if (diversity[1] > maxd) { maxd = diversity[1]; component = 1; mask = 0x00ff00; shift = 8; }
		if (diversity[2] > maxd) { maxd = diversity[2]; component = 2; mask = 0x0000ff; shift = 0; }
		qn->diversity = maxd;
		qn->shift = shift;
		qn->mask = mask;
		qn->min = min[component];
		qn->max = max[component];
	}

	return qn;
}


/*-------------------------------------------------------------------

	cutindex = Separate(quantizenode)

	separate a node's array and return the cut index.
	(cutindex := new upper node's lower index)
	
-------------------------------------------------------------------*/

static ULONG Separate(struct quantizenode *node)
{
	struct RNDHistoEntry **lentry = node->lower;
	struct RNDHistoEntry **uentry = node->upper;
	struct RNDHistoEntry *temp;
	
	ULONG mid = node->min + (node->max - node->min) / 2;
	ULONG cutindex = 0;

	while (lentry < uentry)
	{
		if ((((*lentry)->rgb & node->mask) >> node->shift) > mid)
		{
			while ((((*uentry)->rgb & node->mask) >> node->shift) > mid) uentry--;
			temp = *lentry;
			*lentry = *uentry;
			*uentry = temp;
			uentry--;
		}		
		lentry++;
		cutindex++;
	}

	return cutindex;
}


/*-------------------------------------------------------------------

	FreeTree(node)
	free tree

-------------------------------------------------------------------*/

static void FreeTree(RNDMH *rmh, struct quantizenode *node)
{
	if (node->left) FreeTree(rmh, node->left);
	if (node->right) FreeTree(rmh, node->right);
	FreeRenderMem(rmh, node, sizeof(struct quantizenode));
}


/*-------------------------------------------------------------------

	newtableptr = DecomposeTree(node, ctableptr)
	decompose tree to colortable

-------------------------------------------------------------------*/

static ULONG *DecomposeTree(struct quantizenode *node, ULONG *tabentry)
{
	if (node->left && node->right)
	{
		tabentry = DecomposeTree(node->left, tabentry);
		tabentry = DecomposeTree(node->right, tabentry);
	}
	else
	{
		ULONG rgb;
		rgb = (ULONG) node->cc_mid[0];
		rgb <<= 8;
		rgb |= (ULONG) node->cc_mid[1];
		rgb <<= 8;
		rgb |= (ULONG) node->cc_mid[2];
		*tabentry++ = rgb;
	}
	return tabentry;
}


/*-------------------------------------------------------------------

	dmaxnode = FindNode(node, dmaxnode)
	recurse tree and find end node with diversity[max].
	at initial call, dmaxnode must point to a dummy node
	with node->diversity = 0

-------------------------------------------------------------------*/

static struct quantizenode *FindNode(struct quantizenode *node, struct quantizenode *dmaxnode)
{
	if (node->left && node->right)
	{
		dmaxnode = FindNode(node->left, dmaxnode);
		dmaxnode = FindNode(node->right, dmaxnode);
	}
	else
	{
		if (node->diversity > dmaxnode->diversity)
		{
			dmaxnode = node;
		}
	}

	return dmaxnode;
}


/*-------------------------------------------------------------------

	result = Quantize(memhandler, histoptrarray, numhist, palptr, numcolors)
	quantize main

-------------------------------------------------------------------*/

static ULONG Quantize(RNDMH *rmh, struct RNDHistoEntry **histoptrtab, ULONG numhist, ULONG *pptr, ULONG numcol, struct Hook *proghook, RNDHISTO *h)
{
	ULONG result = EXTP_NOT_ENOUGH_MEMORY;
	struct quantizenode *rootnode;

	struct RND_ProgressMessage progmsg;
	progmsg.RND_PMsg_type = PMSGTYPE_COLORS_CHOSEN;
	progmsg.RND_PMsg_total = numcol;

	rootnode = CreateNode(rmh, histoptrtab, histoptrtab + numhist - 1);
	if (rootnode)
	{
		struct quantizenode *maxdnode = rootnode, dummynode;
		ULONG numnodes = 1, cutindex;
	
		dummynode.diversity = 0;
		result = EXTP_SUCCESS;
		
		while (numnodes < numcol)
		{
			cutindex = Separate(maxdnode);
	
			maxdnode->left = CreateNode(rmh, maxdnode->lower, maxdnode->lower + cutindex - 1);
			maxdnode->right = CreateNode(rmh, maxdnode->lower + cutindex, maxdnode->upper);
			
			if (proghook)
			{
				progmsg.RND_PMsg_count = numnodes;
				if (!CallHookPkt(proghook, h, &progmsg))
				{
					result = EXTP_CALLBACK_ABORTED;
					goto abort;
				}
			}
	
			if (maxdnode->left && maxdnode->right)
			{
				maxdnode = FindNode(rootnode, &dummynode);
				numnodes++;
				continue;
			}
			
			result = EXTP_NOT_ENOUGH_MEMORY;
			break;
		}
		
		if (result == EXTP_SUCCESS)
		{
			DecomposeTree(rootnode, pptr);
		}
abort:		
		FreeTree(rmh, rootnode);
	}

	return result;
}


/*****************************************************************************
**
**	extractpalette
*/

LIBAPI ULONG ExtractPaletteA(RNDHISTO *h, RNDPAL *p, UWORD numcol, struct TagItem *tags)
{
	ULONG result = EXTP_NO_DATA;
	
	if (h && p && numcol)
	{
		ULONG numhcol;
		ObtainSemaphoreShared(&h->lock);
	
		numhcol = QueryHistogram(h, RND_NumColors);
		if (numhcol > 0)
		{
			struct RNDHistoEntry **hparray = CreateHistogramPointerArray(h);
			if (hparray)
			{
				BOOL newpal = (BOOL) GetTagData(RND_NewPalette, TRUE, tags);
				ULONG firstcol = GetTagData(RND_FirstColor, 0, tags);
				ULONG *pptr = p->table + firstcol;
		
				ObtainSemaphore(&p->lock);
				FlushPalette(p);
			
				if (newpal)
				{
					p->numcolors = 0;
					memfill32(p->table, 256*4, 0);
				}
	
				if (firstcol + numcol > p->numcolors)
				{
					p->numcolors = firstcol + numcol;
				}
				
				if (numhcol > numcol)
				{
					RNDMH *rmh = (APTR) GetTagData(RND_RMHandler, (ULONG) h->rmh, tags);
					struct Hook *proghook = (struct Hook *) GetTagData(RND_ProgressHook, NULL, tags);

					result = Quantize(rmh, hparray, numhcol, pptr, numcol, proghook, h);
				}
				else
				{
					LONG i;
					for (i = 0; i < numhcol; ++i)
					{
						*pptr++ = hparray[i]->rgb;
					}
					result = EXTP_SUCCESS;
				}
	
				ReleaseSemaphore(&p->lock);
				FreeRenderVec((ULONG *) hparray);
			}
			else
			{
				result = EXTP_NOT_ENOUGH_MEMORY;
			}
		}
		ReleaseSemaphore(&h->lock);
	}

	return result;
}

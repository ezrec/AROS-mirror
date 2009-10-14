#include "Private.h"

///typedefs

struct quantizenode
{
    struct quantizenode *left, *right;          /* child nodes */
    FHEntry **lower;
    FHEntry **upper;      /* range */
    uint32 numentries;   /* number of entries represented by this node */
    uint32 min, max;     /* min max values of this component */
    uint32 diversity;    /* separation criterium */
    uint32 cc_mid[3];
    uint32 shift, mask;
};

//+

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///histogram_quantize_create
static struct quantizenode * histogram_quantize_create(APTR Pool, FHEntry **lower, FHEntry **upper)
{
    struct quantizenode *qn = F_NewP(Pool, sizeof(struct quantizenode));

    if (qn)
    {
        uint32 diversity[3];
        uint32 maxd;
        LONG min[3];
        LONG max[3];
        uint32 numpixel=0;
        uint32 shift, mask;
        uint32 component;
        FHEntry **htp = lower;

        qn -> left = NULL;
        qn -> right = NULL;
        qn -> lower = lower;
        qn -> upper = upper;
        qn -> numentries = upper - lower + 1;

        qn -> cc_mid[0] = qn -> cc_mid[1] = qn -> cc_mid[2] = 0;
        diversity[0] = diversity[1] = diversity[2] = 0;
        min[0] = min[1] = min[2] = 256;
        max[0] = max[1] = max[2] = -1;

        while (htp <= upper)
        {
            uint32 rgb = (*htp)->rgb;
            uint32 count = (*htp)->count;
            int32 c1 = (rgb & 0xff0000) >> 16;
            int32 c2 = (rgb & 0x00ff00) >> 8;
            int32 c3 = rgb & 0x0000ff;

            if (c1 < min[0]) min[0] = c1;
            if (c1 > max[0]) max[0] = c1;
            if (c2 < min[1]) min[1] = c2;
            if (c2 > max[1]) max[1] = c2;
            if (c3 < min[2]) min[2] = c3;
            if (c3 > max[2]) max[2] = c3;

            qn -> cc_mid[0] += c1 * count;
            qn -> cc_mid[1] += c2 * count;
            qn -> cc_mid[2] += c3 * count;

            htp++;
            numpixel += count;
        }

        qn->cc_mid[0] /= numpixel;
        qn->cc_mid[1] /= numpixel;
        qn->cc_mid[2] /= numpixel;

        htp = lower;

        while (htp <= upper)
        {
            uint32 rgb = (*htp)->rgb;

#warning "TODO: FIXME! These defines cause problems compiling ... pls correct"
//    	#ifdef __AROS__
//	#warning "CHECKME: IEEE crap"
//	#define IEEEDPFlt   	(double)
//	#define IEEEDPPow   	pow
//	#define IEEEDPMul(a,b)  a * b
//	#define IEEEDPFix   	(uint32)
//	#endif

            double c1 = IEEEDPFlt(((rgb & 0x00FF0000) >> 16) - qn -> cc_mid[0]);
            double c2 = IEEEDPFlt(((rgb & 0x0000FF00) >>  8) - qn -> cc_mid[1]);
            double c3 = IEEEDPFlt( (rgb & 0x000000FF)        - qn -> cc_mid[2]);

            double a = IEEEDPPow(IEEEDPFlt((*htp)->count), 0.35);

            c1 = IEEEDPMul(IEEEDPMul(c1, c1), a);
            c2 = IEEEDPMul(IEEEDPMul(c2, c2), a);
            c3 = IEEEDPMul(IEEEDPMul(c3, c3), a);

            diversity[0] += IEEEDPFix(c1);
            diversity[1] += IEEEDPFix(c2);
            diversity[2] += IEEEDPFix(c3);

            htp++;
        }

        //F_Log(0,"div 0x%08lx 0x%08lx 0x%08lx",diversity[0],diversity[1],diversity[2]);

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
//+
///histogram_quantize_separate
/*-------------------------------------------------------------------

    cutindex = Separate(quantizenode)

    separate a node's array and return the cut index.
    (cutindex := new upper node's lower index)

-------------------------------------------------------------------*/

static uint32 histogram_quantize_separate(struct quantizenode *node)
{
    FHEntry **lentry = node->lower;
    FHEntry **uentry = node->upper;
    FHEntry *temp;

    uint32 mid = node->min + (node->max - node->min) / 2;
    uint32 cutindex = 0;

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
//+
///histogram_quantize_find
/*-------------------------------------------------------------------

    dmaxnode = FindNode(node, dmaxnode)
    recurse tree and find end node with diversity[max].
    at initial call, dmaxnode must point to a dummy node
    with node->diversity = 0

-------------------------------------------------------------------*/

static struct quantizenode *histogram_quantize_find(struct quantizenode *node, struct quantizenode *dmaxnode)
{
    if (node->left && node->right)
    {
        dmaxnode = histogram_quantize_find(node->left, dmaxnode);
        dmaxnode = histogram_quantize_find(node->right, dmaxnode);
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
//+
///histogram_quantize_decompose
/*-------------------------------------------------------------------

    newtableptr = DecomposeTree(node, ctableptr)
    decompose tree to colortable

-------------------------------------------------------------------*/

static uint32 *histogram_quantize_decompose(struct quantizenode *node, uint32 *tabentry)
{
    if (node->left && node->right)
    {
        tabentry = histogram_quantize_decompose(node->left, tabentry);
        tabentry = histogram_quantize_decompose(node->right, tabentry);
    }
    else
    {
        uint32 rgb;
        rgb = (uint32) node->cc_mid[0];
        rgb <<= 8;
        rgb |= (uint32) node->cc_mid[1];
        rgb <<= 8;
        rgb |= (uint32) node->cc_mid[2];
        *tabentry++ = rgb;
    }
    return tabentry;
}
//+
///histogram_quantize_free
static void histogram_quantize_free(struct quantizenode *node)
{
    if (node->left)
    {
        histogram_quantize_free(node->left);
    }

    if (node->right)
    {
        histogram_quantize_free(node->right);
    }

    F_Dispose(node);
}
//+
///histogram_quantize
static uint32 histogram_quantize(APTR Pool, FHEntry **histoptrtab, uint32 numhist, uint32 *pptr, uint32 numcol)
{
    uint32 rc = FALSE;
    struct quantizenode *rootnode;

    rootnode = histogram_quantize_create(Pool, histoptrtab, histoptrtab + numhist - 1);

    if (rootnode)
    {
        struct quantizenode *maxdnode = rootnode, dummynode;
        uint32 numnodes = 1, cutindex;

        dummynode.diversity = 0;
        rc = TRUE;

        while (numnodes < numcol)
        {
            cutindex = histogram_quantize_separate(maxdnode);

            maxdnode->left = histogram_quantize_create(Pool, maxdnode->lower, maxdnode->lower + cutindex - 1);
            maxdnode->right = histogram_quantize_create(Pool, maxdnode->lower + cutindex, maxdnode->upper);

            if (maxdnode->left && maxdnode->right)
            {
                maxdnode = histogram_quantize_find(rootnode, &dummynode);
                numnodes++;
                continue;
            }

            rc = FALSE;

            break;
        }

        if (rc)
        {
            histogram_quantize_decompose(rootnode, pptr);
        }

        histogram_quantize_free(rootnode);
    }

    return rc;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

struct FS_Histogram_Extract                     { uint32 NumColors; };

///Histogram_Extract
F_METHODM(uint32 *,Histogram_Extract,FS_Histogram_Extract)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 *table = NULL;

    if (Msg -> NumColors)
    {
        F_Do(Obj,FM_Lock,FF_Lock_Shared);

        if (LOD -> numcolors)
        {
            FHEntry **array = (FHEntry **) F_Do(Obj,F_IDM(FM_Histogram_CreateArray));

            if (array)
            {
                table = F_New(sizeof (uint32) * Msg -> NumColors);

                if (table)
                {
                    if (Msg -> NumColors >= LOD -> numcolors)
                    {
                        uint32 i;

                        for (i = 0 ; i < LOD -> numcolors ; i++)
                        {
                            table[i] = array[i] -> rgb;
                        }
                    }
                    else
                    {
                        if (!histogram_quantize(LOD -> pool, array, LOD -> numcolors, table, MIN(Msg -> NumColors, LOD -> numcolors)))
                        {
                            F_Dispose(table); table = NULL;
                        }
                    }
                }

                F_Dispose(array);
            }
        }

        F_Do(Obj,FM_Unlock);
    }
    return table;
}
//+


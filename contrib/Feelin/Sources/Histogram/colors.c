#include "Private.h"

typedef int32 (*CMPFUNC)(APTR userdata, FHEntry *e1, FHEntry *e2);

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///histogram_add
int32 histogram_add(struct LocalObjectData *LOD, uint32 ARGB, uint32 Count)
{
    struct RNDTreeNode **nodeptr = &LOD -> root;
    struct RNDTreeNode *node;

    uint32 rgbmask = LOD -> rgbmask;
    uint32 chkmask = 0x800000;

    ARGB &= rgbmask;

    while (node = *nodeptr)
    {
        if (node -> entry.rgb == ARGB)
        {
            node -> entry.count += Count;
            LOD -> numpixels += Count;

//            F_Log(0,"more 0x%08lx (%ld/%ld)",ARGB,node -> entry.count,LOD -> numpixels);

            return TRUE;
        }

        while (!(chkmask & rgbmask)) chkmask >>= 1;

        if (ARGB & chkmask)
        {
            nodeptr = &node->left;
        }
        else
        {
            nodeptr = &node->right;
        }

        chkmask >>= 1;
    }

    node = F_NewP(LOD -> pool,sizeof (struct RNDTreeNode));

    if (node)
    {
        *nodeptr = node;

        node->left = NULL;
        node->right = NULL;

        node->entry.rgb = ARGB;
        node->entry.count = Count;

        LOD -> numpixels += Count;
        LOD -> numcolors++;

//        F_Log(0,"new 0x%08lx",ARGB);

        return TRUE;
    }

    return FALSE;
}
//+
///histogram_heapsort
BOOL histogram_heapsort(APTR userdata, APTR *refarray, uint32 length, CMPFUNC cmpfunc)
{
    uint32 indx, k, j, half, limit;
    APTR temp;

    if (refarray && cmpfunc && length > 1)
    {
        indx = (length >> 1) - 1;

        do
        {
            k = indx;
            temp = refarray[k];
            limit = length - 1;
            half = length >> 1;

            while (k < half)
            {
                j = k + k + 1;

                if ((j < limit) && ((*cmpfunc)(userdata, refarray[j + 1], refarray[j]) > 0))
                {
                   ++j;
                }

                if ((*cmpfunc)(userdata, temp, refarray[j]) >= 0)
                {
                   break;
                }

                refarray[k] = refarray[j];
                k = j;
            }
            refarray[k] = temp;
        }
        while (indx-- != 0);

        while (--length > 0)
        {
            temp = refarray[0];
            refarray[0] = refarray[length];
            refarray[length] = temp;
            k = 0;
            temp = refarray[k];
            limit = length - 1;
            half = length >> 1;

            while (k < half)
            {
               j = k + k + 1;

               if ((j < limit) && ((*cmpfunc)(userdata, refarray[j + 1], refarray[j]) > 0))
               {
                  ++j;
               }

               if ((*cmpfunc)(userdata, temp, refarray[j]) >= 0)
               {
                  break;
               }

               refarray[k] = refarray[j];
               k = j;
            }
            refarray[k] = temp;
        }
        return TRUE;
    }
    return FALSE;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

struct  FS_Histogram_AddRGB                     { uint32 ARGB; uint32 Weight; };
struct  FS_Histogram_AddPixels                  { uint32 *ARGB; uint32 Width; uint32 Height; };
struct  FS_Histogram_Sort                       { uint32 Mode; };

///Histogram_AddRGB
F_METHODM(uint32,Histogram_AddRGB,FS_Histogram_AddRGB)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 n = 0;
    
    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);
    
    if (histogram_add(LOD, Msg -> ARGB, Msg -> Weight))
    {
        n++;
    }
    
    F_Do(Obj,FM_Unlock);

    return n;
}
//+
///Histogram_AddPixels
F_METHODM(uint32,Histogram_AddPixels,FS_Histogram_AddPixels)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
     
    uint32 n=0;

    if (Msg -> ARGB && Msg -> Width && Msg -> Height)
    {
        uint32 *argb = Msg -> ARGB;
        uint32 i,j = Msg -> Width * Msg -> Height;

        F_Do(Obj,FM_Lock,FF_Lock_Exclusive);
        
        for (i = 0 ; i < j ; i++)
        {
            if (histogram_add(LOD, *argb++, 1))
            {
                n++;
            }
            else break;
        }
        
        F_Do(Obj,FM_Unlock);
    }

    return n;
}
//+
///Histogram_CreateArray

///histogram_scan
FHEntry **histogram_scan(struct RNDTreeNode *node, FHEntry **Array)
{
    if (node->left)
    {
        Array = histogram_scan(node -> left, Array);
    }

    if (node->right)
    {
        Array = histogram_scan(node -> right, Array);
    }

    *Array++ = &node -> entry;

    return Array;
}
//+

F_METHOD(FHEntry **,Histogram_CreateArray)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    FHEntry **array = NULL;

    F_Do(Obj,FM_Lock,FF_Lock_Shared);

    if ((LOD -> numcolors > 0) &&
        (LOD -> numcolors != 0xFFFFFFFF) &&
        (LOD -> root != NULL))
    {
        array = F_New(LOD -> numcolors * sizeof(FHEntry *));

        if (array)
        {
            histogram_scan(LOD -> root, array);
        }
    }

    F_Do(Obj,FM_Unlock);

    return array;
}
//+
///Histogram_Sort
///cmpfunc
///cmpfunc_lum_desc
int32 cmpfunc_lum_desc(APTR data, FHEntry *e1, FHEntry *e2)
{
    uint32 rgb1 = e1 -> rgb;
    uint32 rgb2 = e2 -> rgb;

    uint32 Y1 =
        299 * ((rgb1 & 0xff0000) >> 16) +
        587 * ((rgb1 & 0x00ff00) >> 8) +
        114 * (rgb1 & 0x0000ff);

    uint32 Y2 =
        299 * ((rgb2 & 0xff0000) >> 16) +
        587 * ((rgb2 & 0x00ff00) >> 8) +
        114 * (rgb2 & 0x0000ff);

    if (Y1 > Y2) return -1;
    if (Y1 == Y2) return 0;

    return 1;
}
//+
///cmpfunc_lum_asc
int32 cmpfunc_lum_asc(APTR data, FHEntry *e1, FHEntry *e2)
{
   return -cmpfunc_lum_desc(data,e1,e2);
}
//+
///cmpfunc_sat_desc
int32 cmpfunc_sat_desc(APTR data, FHEntry *e1, FHEntry *e2)
{
    uint32 rgb1 = e1 -> rgb;
    uint32 rgb2 = e2 -> rgb;

    uint32 S1, S2;
    int32 r = ((rgb1 & 0xff0000) >> 16);
    int32 g = ((rgb1 & 0x00ff00) >> 8);
    int32 b = (rgb1 & 0x0000ff);
    S1 = (r-g)*(r-g);
    S1 += (g-b)*(g-b);
    S1 += (b-r)*(b-r);
    r = ((rgb2 & 0xff0000) >> 16);
    g = ((rgb2 & 0x00ff00) >> 8);
    b = (rgb2 & 0x0000ff);
    S2 = (r-g)*(r-g);
    S2 += (g-b)*(g-b);
    S2 += (b-r)*(b-r);

    if (S1 > S2) return -1;
    if (S1 == S2) return 0;
    return 1;
}
//+
///cmpfunc_sat_asc
int32 cmpfunc_sat_asc(APTR data, FHEntry *e1, FHEntry *e2)
{
   return -cmpfunc_sat_desc(data,e1,e2);
}
//+
///cmpfunc_rep_desc
int32 cmpfunc_rep_desc(APTR data, FHEntry *e1, FHEntry *e2)
{
   if (e1 -> count > e2 -> count) return -1;
   if (e1 -> count == e2 -> count) return 0;

   return 1;
}
//+
///cmpfunc_rep_asc
int32 cmpfunc_rep_asc(APTR data, FHEntry *ref1, FHEntry *ref2)
{
   if (ref1 -> count > ref2 -> count) return 1;
   if (ref1 -> count == ref2 -> count) return 0;

   return -1;
}
//+
///cmpfunc_sig_desc
int32 cmpfunc_sig_desc(APTR data, FHEntry *ref1, FHEntry *ref2)
{
    uint32 sig1,sig2, r,g,b;

    r = (ref1 -> rgb & 0xff0000) >> 16;
    g = (ref1 -> rgb & 0x00ff00) >> 8;
    b = (ref1 -> rgb & 0x0000ff);

    sig1  = (r-g)*(r-g);
    sig1 += (g-b)*(g-b);
    sig1 += (b-r)*(b-r);
    sig1 *= ref1 -> count;

    r = (ref2 -> rgb & 0xff0000) >> 16;
    g = (ref2 -> rgb & 0x00ff00) >> 8;
    b = (ref2 -> rgb & 0x0000ff);

    sig2  = (r-g)*(r-g);
    sig2 += (g-b)*(g-b);
    sig2 += (b-r)*(b-r);
    sig2 *= ref2 -> count;

    if (sig1 > sig2) return -1;
    if (sig1 == sig2) return 0;

    return 1;
}
//+
///cmpfunc_sig_asc
int32 cmpfunc_sig_asc(APTR data, FHEntry *ref1, FHEntry *ref2)
{
    return -cmpfunc_sig_desc(data,ref1,ref2);
}
//+
//+

F_METHODM(uint32 *,Histogram_Sort,FS_Histogram_Sort)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
    FHEntry **sortdata;
    int32 (*cmpfunc)(APTR, FHEntry *, FHEntry *) = NULL;

    F_Log(0,"Mode (%ld) - NumColors (%ld)",Msg -> Mode,LOD -> numcolors);
    
    switch (Msg -> Mode)
    {
        case FV_HISTOGRAM_SORT_BRIGHTNESS:
        {
            cmpfunc = cmpfunc_lum_desc;
        }
        break;

        case FV_HISTOGRAM_SORT_BRIGHTNESS | FF_HISTOGRAM_SORT_ASCENDING:
        {
            cmpfunc = cmpfunc_lum_asc;
        }
        break;

        case FV_HISTOGRAM_SORT_SATURATION:
        {
            cmpfunc = cmpfunc_sat_desc;
        }
        break;

        case FV_HISTOGRAM_SORT_SATURATION | FF_HISTOGRAM_SORT_ASCENDING:
        {
            cmpfunc = cmpfunc_sat_asc;
        }
        break;

        /* sort modes which realy need a histogram */

        case FV_HISTOGRAM_SORT_REPRESENTATION:
        {
            cmpfunc = cmpfunc_rep_desc;
        }
        break;

        case FV_HISTOGRAM_SORT_REPRESENTATION | FF_HISTOGRAM_SORT_ASCENDING:
        {
            cmpfunc = cmpfunc_rep_asc;
        }
        break;

        case FV_HISTOGRAM_SORT_SIGNIFICANCE:
        {
            cmpfunc = cmpfunc_sig_desc;
        }
        break;

        case FV_HISTOGRAM_SORT_SIGNIFICANCE | FF_HISTOGRAM_SORT_ASCENDING:
        {
            cmpfunc = cmpfunc_sig_asc;
        }
        break;

        default:
        {
            F_Log(0,"unknown sort mode (%ld)",Msg -> Mode);
        }
        return NULL;
    }

    sortdata = (FHEntry **) F_Do(Obj,F_IDM(FM_Histogram_CreateArray));

    if (sortdata && cmpfunc)
    {
//        uint32 i;

        histogram_heapsort(NULL, (APTR) sortdata, LOD -> numcolors, cmpfunc);
/*
        for (i = 0 ; i < LOD -> numcolors ; i++)
        {
            FPrintf(FeelinBase -> Console,"%06lxx%04ld ",sortdata[i] -> rgb,sortdata[i] -> count);
        }

        FPrintf(FeelinBase -> Console,"\n\n");
*/
        return (uint32 *) sortdata;
    }
    return NULL;
}
//+


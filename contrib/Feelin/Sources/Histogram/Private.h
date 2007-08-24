/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/mathieeedoubbas.h>
#include <proto/mathieeedoubtrans.h>
#include <proto/feelin.h>

/****************************************************************************
*** Attributes, Methods, Values, Flags **************************************
****************************************************************************/

enum    { // attributes

        FA_Histogram_Resolution,
        FA_Histogram_Pool,
        FA_Histogram_NumColors,
        FA_Histogram_NumPixels

        };

enum    {

        FM_Histogram_AddRGB,
        FM_Histogram_AddPixels,
        FM_Histogram_CreateArray,
        FM_Histogram_Sort,
        FM_Histogram_Extract

        };

#define FV_HISTOGRAM_TYPE_12BIT                 4 // bits per guns
#define FV_HISTOGRAM_TYPE_15BIT                 5
#define FV_HISTOGRAM_TYPE_18BIT                 6
#define FV_HISTOGRAM_TYPE_21BIT                 7
#define FV_HISTOGRAM_TYPE_24BIT                 8

enum    {

        FV_HISTOGRAM_SORT_NONE,

        /* don't sort anything */

        FV_HISTOGRAM_SORT_BRIGHTNESS,

        /* sort palette entries by brightness */

        FV_HISTOGRAM_SORT_SATURATION,

        /* sort palette entries by color intensity */

        FV_HISTOGRAM_SORT_REPRESENTATION,

        /* sort palette entries by the number  of  histogram  entries  that
        they represent. You must supply the RND_Histogram taglist argument.
        */

        FV_HISTOGRAM_SORT_SIGNIFICANCE,

        /* sort palette entries by their optical significance for the human
        eye.  Implementation  is  unknown  to  you and may change. You must
        supply the RND_Histogram taglist argument. */

        FV_HISTOGRAM_SORT_END

        };

        /* By default, sort direction is descending, i.e. the precedence is
        more-to-less.  Combine with this flag to invert the sort direction.
        */

#define FF_HISTOGRAM_SORT_ASCENDING       (1 << FV_HISTOGRAM_SORT_END)

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

typedef struct FeelinHistogramEntry
{
    uint32                          rgb;
    uint32                          count;
}
FHEntry;

struct RNDTreeNode
{
    struct RNDTreeNode             *left;
    struct RNDTreeNode             *right;
    FHEntry                         entry;
};

struct LocalObjectData
{
    APTR                            pool;

    struct RNDTreeNode             *root;
    uint32                          numcolors;
    uint32                          numpixels;
    uint32                          rgbmask;
    uint8                           bitspergun;
    uint8                           _pad0;
    uint16                          _pad1;
};


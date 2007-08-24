/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/feelin.h>

#include <cybergraphx/cybergraphics.h>
#include <libraries/feelin.h>

#include <feelin/coremedia.h>

#define GfxBase                                 FeelinBase -> Graphics

/****************************************************************************
*** Attributes, Methods, Values, Flags **************************************
****************************************************************************/

enum    {

        FA_BitMap_Width,
        FA_BitMap_Height,
        FA_BitMap_Depth,                    //FIXME: not implemented

        FA_BitMap_PixelSize,
        FA_BitMap_PixelArray,
        FA_BitMap_PixelArrayMod,

        FA_BitMap_ColorType,
        FA_BitMap_ColorCount,
        FA_BitMap_ColorArray,

        FA_BitMap_Pens,
        FA_BitMap_NumPens,

        FA_BitMap_Rendered,
        FA_BitMap_RenderedType,
        FA_BitMap_RenderedWidth,
        FA_BitMap_RenderedHeight,

        FA_BitMap_TargetWidth,
        FA_BitMap_TargetHeight,
        FA_BitMap_TargetScreen,
        FA_BitMap_TargetMaxColors,

        FA_BitMap_BlitMode,
        FA_BitMap_ScaleFilter,

        FA_BitMap_Margins

        };

enum    {

        FM_BitMap_Blit,
        FM_BitMap_Render

        };

enum    {

        FV_BitMap_BlitMode_Copy,
        FV_BitMap_BlitMode_Tile,
        FV_BitMap_BlitMode_Scale,
        FV_BitMap_BlitMode_Frame

        };

enum    {

        FV_BitMap_ScaleFilter_Nearest,
        FV_BitMap_ScaleFilter_Bilinear,
        FV_BitMap_ScaleFilter_Average

        };

#define FV_PIXEL_TYPE_BITMAP                    0xFFFFFFFF

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
    uint32                          flags;

    FCorePicturePublic              source;

    // rendered

    APTR                            rendered;
    uint16                          rendered_w;
    uint16                          rendered_h;
    uint32                          rendered_pixel_type;

    // remap

    uint32                         *pens;
    uint32                          numpens;

    struct ColorMap                *cm;
};

#define FF_BITMAP_RENDERED_BITMAP               (1 << 0)

#define GSIMUL 4096

#define F_PIXEL_R(x)                            ((x & 0xFF000000) >> 24)
#define F_PIXEL_G(x)                            ((x & 0x00FF0000) >> 16)
#define F_PIXEL_B(x)                            ((x & 0x0000FF00) >>  8)
#define F_PIXEL_A(x)                            (x & 0x000000FF)

/****************************************************************************
*** Prototypes **************************************************************
****************************************************************************/

void bitmap_render_free(FClass *Class, FObject Obj);


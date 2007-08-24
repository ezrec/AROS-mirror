#ifndef FEELIN_COREMEDIA_H
#define FEELIN_COREMEDIA_H

/*
**  feelin/coremedia.h
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
********************************************************************************

$VER: 01.00 (2005/08/20)

*/

/****************************************************************************
*** Generation **************************************************************
****************************************************************************/

#define FC_CorePNG                              "CorePNG"
#define CorePNGObject                           F_NewObj(FC_CorePNG

/****************************************************************************
*** Types *******************************************************************
****************************************************************************/

typedef struct FeelinCorePicturePublic
{
    uint16                          Width;
    uint16                          Height;
    uint8                           Depth;

    bits16                          Flags;

    uint8                           PixelSize;
    APTR                            PixelArray;
    uint16                          PixelArrayMod;

    uint8                           ColorType;
    uint8                           ColorCount;
    uint32                         *ColorArray;
}
FCorePicturePublic;

#define FV_COLOR_TYPE_GREY                      (1 << 0)
#define FV_COLOR_TYPE_PALETTE                   (1 << 1)
#define FV_COLOR_TYPE_RGB                       (1 << 2)
#define FF_COLOR_TYPE_ALPHA                     (1 << 7)
#define FV_COLOR_TYPE_MASK                      0x0F
#define FV_COLOR_TYPE_GREY_ALPHA                (FF_COLOR_TYPE_ALPHA | FV_COLOR_TYPE_GREY)
#define FV_COLOR_TYPE_PALETTE_ALPHA             (FF_COLOR_TYPE_ALPHA | FV_COLOR_TYPE_PALETTE)
#define FV_COLOR_TYPE_RGB_ALPHA                 (FF_COLOR_TYPE_ALPHA | FV_COLOR_TYPE_RGB)

/****************************************************************************
*** Values ******************************************************************
****************************************************************************/

enum    {

        FV_CoreMedia_SourceType_None,
        FV_CoreMedia_SourceType_File,
        FV_CoreMedia_SourceType_Handle

        };

#endif

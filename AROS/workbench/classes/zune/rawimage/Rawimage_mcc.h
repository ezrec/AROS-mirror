#ifndef RAWIMAGE_MCC_H
#define RAWIMAGE_MCC_H

/*
 *  Rawimage.mcc
 *
 *  Copyright © 2008-2010 Christian Rosentreter <tokai@binaryriot.org>
 *  All rights reserved.
 */

#ifndef LIBRARIES_IFFPARSE_H
  #include <libraries/iffparse.h> /* for MAKE_ID */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif


#define MUIC_Rawimage  "Rawimage.mcc"
#define RawimageObject MUI_NewObject(MUIC_Rawimage


struct MUI_RawimageData
{
	ULONG ri_Width;
	ULONG ri_Height;
	ULONG ri_Format;
	ULONG ri_Size;
	ULONG ri_Data[0];
};

#define RAWIMAGE_PIXELFORMAT_ARGB    0
#define RAWIMAGE_PIXELFORMAT_RGB     1

#define RAWIMAGE_FORMAT_RAW_ARGB_ID  RAWIMAGE_PIXELFORMAT_ARGB
#define RAWIMAGE_FORMAT_RAW_RGB_ID   RAWIMAGE_PIXELFORMAT_RGB
#define RAWIMAGE_FORMAT_BZ2_ARGB_ID  MAKE_ID('B', 'Z', '2', RAWIMAGE_PIXELFORMAT_ARGB)
#define RAWIMAGE_FORMAT_BZ2_RGB_ID   MAKE_ID('B', 'Z', '2', RAWIMAGE_PIXELFORMAT_RGB )
#define RAWIMAGE_FORMAT_Z_ARGB_ID    MAKE_ID('Z', 0x0, 0x0, RAWIMAGE_PIXELFORMAT_ARGB)
#define RAWIMAGE_FORMAT_Z_RGB_ID     MAKE_ID('Z', 0x0, 0x0, RAWIMAGE_PIXELFORMAT_RGB )
#define RAWIMAGE_FORMAT_RLE_ARGB_ID  MAKE_ID('R', 'L', 'E', RAWIMAGE_PIXELFORMAT_ARGB)
#define RAWIMAGE_FORMAT_RLE_RGB_ID   MAKE_ID('R', 'L', 'E', RAWIMAGE_PIXELFORMAT_RGB )
#define RAWIMAGE_FORMAT_LZMA_ARGB_ID MAKE_ID('L', 'Z', 'M', RAWIMAGE_PIXELFORMAT_ARGB)
#define RAWIMAGE_FORMAT_LZMA_RGB_ID  MAKE_ID('L', 'Z', 'M', RAWIMAGE_PIXELFORMAT_RGB )


/*  attributes
 */
#define MUIA_Rawimage_Data  0xFED10014   /* [IS.]  struct MUI_RawimageData *   v20.1 (06.01.2007)   */


#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack()
  #elif defined(__VBCC__)
    #pragma default-align
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* RAWIMAGE_MCC_H */


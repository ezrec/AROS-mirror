/*
**	$VER: cybergraphics.h 50.12 (13.08.2008)
**
**	include file for cybergraphics.library
**
**	Copyright © 1996-1998 by phase5 digital products
**      All Rights reserved.
**
*/

#ifndef CYBERGRAPHX_CYBERGRAPHICS_H
#define CYBERGRAPHX_CYBERGRAPHICS_H 1

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#ifndef GRAPHICS_DISPLAYINFO_H
#include <graphics/displayinfo.h>
#endif

#if defined(__GNUC__) && __GNUC__ > 2
#pragma pack(2)
#endif //defined(__GNUC__) && __GNUC__ > 2


#define CYBERGFXNAME	"cybergraphics.library"
#define CYBERGFX_INCLUDE_VERSION  (41UL)

/*                                                          *
 *  Definition of CyberModeNode (Returned in AllocModeList) *
 *                                                          */

struct CyberModeNode
 {
  struct Node Node;
  char   ModeText[DISPLAYNAMELEN];	/* name for this mode */
  ULONG  DisplayID;			/* display id associated with the node */
  UWORD  Width;				/* visible width */
  UWORD  Height;			/* visible height */
  UWORD  Depth;				/* display depth */
  struct TagItem *DisplayTagList;	/* taglist with extended ModeID information */
 };


/*                                  *
 * Parameters for GetCyberMapAttr() *
 *                                  */

#define CYBRMATTR_XMOD        (0x80000001) /* function returns BytesPerRow if its called with this parameter */
#define CYBRMATTR_BPPIX       (0x80000002) /* BytesPerPixel shall be returned */
#define CYBRMATTR_DISPADR     (0x80000003) /* do not use this ! private tag */
#define CYBRMATTR_PIXFMT      (0x80000004) /* the pixel format is returned */
#define CYBRMATTR_WIDTH       (0x80000005) /* returns width in pixels */
#define CYBRMATTR_HEIGHT      (0x80000006) /* returns height in lines */
#define CYBRMATTR_DEPTH       (0x80000007) /* returns bits per pixel */
#define CYBRMATTR_ISCYBERGFX  (0x80000008) /* returns -1 if supplied bitmap is a cybergfx one */
#define CYBRMATTR_ISLINEARMEM (0x80000009) /* returns -1 if supplied bitmap is linear accessable */

/*                                 *
 * Parameters for GetCyberIDAttr() *
 *                                 */

#define CYBRIDATTR_PIXFMT (0x80000001) /* the pixel format is returned */
#define CYBRIDATTR_WIDTH  (0x80000002) /* returns visible width in pixels */
#define CYBRIDATTR_HEIGHT (0x80000003) /* returns visible height in lines */
#define CYBRIDATTR_DEPTH  (0x80000004) /* returns bits per pixel */
#define CYBRIDATTR_BPPIX  (0x80000005) /* BytesPerPixel shall be returned */

/*                              *
 * Tags for CyberModeRequest()  *
 *                              */

#define CYBRMREQ_TB          (TAG_USER+0x40000)

/*            *
 * FilterTags *
 *            */

#define CYBRMREQ_MinDepth    (CYBRMREQ_TB+0)  /* Minimum depth for displayed screenmode */
#define CYBRMREQ_MaxDepth    (CYBRMREQ_TB+1)  /* Maximum depth  "       "        " */
#define CYBRMREQ_MinWidth    (CYBRMREQ_TB+2)  /* Minumum width  "       "        " */
#define CYBRMREQ_MaxWidth    (CYBRMREQ_TB+3)  /* Maximum width  "       "        " */
#define CYBRMREQ_MinHeight   (CYBRMREQ_TB+4)  /* Minumum height "       "        " */
#define CYBRMREQ_MaxHeight   (CYBRMREQ_TB+5)  /* Minumum height "       "        " */
#define CYBRMREQ_CModelArray (CYBRMREQ_TB+6)

#define CYBRMREQ_WinTitle    (CYBRMREQ_TB+20)
#define CYBRMREQ_OKText      (CYBRMREQ_TB+21)
#define CYBRMREQ_CancelText  (CYBRMREQ_TB+22)

#define CYBRMREQ_Screen      (CYBRMREQ_TB+30)  /* Screen you wish the Requester to open on */

/*                            *
 * Tags for BestCyberModeID() *
 *                            */

#define CYBRBIDTG_TB  (TAG_USER+0x50000)

/* FilterTags */

#define CYBRBIDTG_Depth         (CYBRBIDTG_TB+0)
#define CYBRBIDTG_NominalWidth  (CYBRBIDTG_TB+1)
#define CYBRBIDTG_NominalHeight (CYBRBIDTG_TB+2)
#define CYBRBIDTG_MonitorID     (CYBRBIDTG_TB+3)
#define CYBRBIDTG_BoardName     (CYBRBIDTG_TB+5)

/*                                    *
 * definition of divers pixel formats *
 *                                    */

#define PIXFMT_LUT8	(0UL)
#define PIXFMT_RGB15	(1UL)
#define PIXFMT_RGB15X   (2UL)  /* CGX4 used it as RRRRRGGG GGBBBBBX, CGX3 did not support it at all */
#define PIXFMT_BGR15	(2UL)
#define PIXFMT_RGB15PC	(3UL)
#define PIXFMT_BGR15PC	(4UL)
#define PIXFMT_RGB16	(5UL)
#define PIXFMT_BGR16	(6UL)
#define PIXFMT_RGB16PC	(7UL)
#define PIXFMT_BGR16PC	(8UL)
#define PIXFMT_RGB24	(9UL)
#define PIXFMT_BGR24	(10UL)
#define PIXFMT_ARGB32	(11UL)
#define PIXFMT_BGRA32	(12UL)
#define PIXFMT_RGBA32	(13UL)

/*                                                        *
 * SrcRectangle formats defines for xxxPixelArray calls() * 
 *                                                        */

#define RECTFMT_RGB	(0UL)
#define RECTFMT_RGBA	(1UL)
#define RECTFMT_ARGB	(2UL)
#define RECTFMT_LUT8	(3UL)
#define RECTFMT_GREY8	(4UL)
#define RECTFMT_RAW     (5UL)

/*                                    *
 * Parameters for CVideoCtrlTagList() *
 *                                    */

#define SETVC_DPMSLevel  (0x88002001)

#define DPMS_ON      (0UL)   /* Full operation                             */
#define DPMS_STANDBY (1UL)   /* Optional state of minimal power reduction  */
#define DPMS_SUSPEND (2UL)   /* Significant reduction of power consumption */
#define DPMS_OFF     (3UL)   /* Lowest level of power consumption          */

/*                              *
 * Tags for LockBitMapTagList() *
 *                              */

#define LBMI_WIDTH       (0x84001001)
#define LBMI_HEIGHT      (0x84001002)
#define LBMI_DEPTH       (0x84001003)
#define LBMI_PIXFMT      (0x84001004)
#define LBMI_BYTESPERPIX (0x84001005)
#define LBMI_BYTESPERROW (0x84001006)
#define LBMI_BASEADDRESS (0x84001007)

/*                                *
 * Tags for UnLockBitMapTagList() *
 *                                */

#define	UBMI_UPDATERECTS	(0x85001001)
#define	UBMI_REALLYUNLOCK	(0x85001002)



/*								*
 * Message passed to the DoCDrawMethodTagList() hook function	*
 *								*/


struct  CDrawMsg
{
        APTR    cdm_MemPtr;
        ULONG   cdm_offx;
        ULONG   cdm_offy;
        ULONG   cdm_xsize;
        ULONG   cdm_ysize;
        UWORD   cdm_BytesPerRow;
        UWORD   cdm_BytesPerPix;
        UWORD   cdm_ColorModel;
};

/*							*
 * Colour Table source formats for WriteLUTPixelArray()	*
 *							*/

#define CTABFMT_XRGB8	(0UL)	/* ULONG [] table */


/*							*
 *	graphics.library/AllocBitMap() extended flags	*
 *							*/

#ifndef BMB_SPECIALFMT
#define BMB_SPECIALFMT (7UL)
#define BMF_SPECIALFMT (1UL << BMB_SPECIALFMT)
#endif /* BMB_SPECIALFMT */

#define SHIFT_PIXFMT( fmt ) (((ULONG)(fmt)) << 24UL)


#ifndef BMF_REQUESTVMEM
#define BMF_REQUESTVMEM  (BMF_MINPLANES|BMF_DISPLAYABLE)
#endif

#ifndef BMB_ROOTMAP
#define BMB_ROOTMAP (5UL)
#define BMF_ROOTMAP (1UL << BMB_ROOTMAP)
#endif /* BMB_ROOTMAP */

#ifndef BMB_3DTARGET
#define BMB_3DTARGET (8UL)
#define BMF_3DTARGET (1UL << BMB_3DTARGET)
#endif /* BMB_3DTARGET */

/*
 * Operations for ProcessPixelArray() (v50)
 *
 */

#define POP_BRIGHTEN            0
#define POP_DARKEN              1
#define POP_SETALPHA            2
#define POP_TINT                3
#define POP_BLUR                4
#define POP_COLOR2GREY          5
#define POP_NEGATIVE            6
#define POP_NEGFADE             7
#define POP_TINTFADE            8
#define POP_GRADIENT            9
#define POP_SHIFTRGB            10

/*
 * Values for POP_SHIFTRGB
 *
 */

#define RGBSHIFT_BGR             (1UL)
#define RGBSHIFT_BRG             (2UL)
#define RGBSHIFT_GBR             (3UL)
#define RGBSHIFT_GRB             (4UL)
#define RGBSHIFT_RBG             (5UL)


/*
 * Tags for ProcessPixelArray() ops
 *
 */

#define PPAOPTAG_FADEFULLSCALE  0x85231020
#define PPAOPTAG_FADEOFFSET     0x85231021


#define PPAOPTAG_GRADIENTTYPE                   0x85231022

#define GRADTYPE_HORIZONTAL                     0
#define GRADTYPE_VERTICAL                       1

/* yet unsupported gradient types follow */
#define GRADTYPE_RECTANGLE                      2
#define GRADTYPE_LINEAR_ANGLE                   3
#define GRADTYPE_RADIAL                         4 /* "circle" center-based */

#define GRADIENT_NUMTYPES                       2

#define PPAOPTAG_GRADCOLOR1                     0x85231023
#define PPAOPTAG_GRADCOLOR2                     0x85231024

#define PPAOPTAG_GRADFULLSCALE                  PPAOPTAG_FADEFULLSCALE
#define PPAOPTAG_GRADOFFSET                     PPAOPTAG_FADEOFFSET

#define PPAOPTAG_RGBMASK                        0x85231025

#define PPAOPTAG_GRADSYMCENTER                  0x85231026

/*
 * Tags for BltBitMap(RastPort)Alpha() (v50)
 *
 */

#define BLTBMA_MIXLEVEL         0x88802000      /* from 0(0%) to 0xFFFFFFFF (100%) */
#define BLTBMA_USESOURCEALPHA   0x88802001
#define BLTBMA_GLOBALALPHA      BLTBMA_MIXLEVEL
#define BLTBMA_DESTALPHAVALUE   0x88802002

#define DESTALPHAVALUE_UNDEFINED    0 /* default */
#define DESTALPHAVALUE_ONE          1
#define DESTALPHAVALUE_USESOURCE    2
#define DESTALPHAVALUE_USEDEST      3

#if defined(__GNUC__) && __GNUC__ > 2
#pragma pack()
#endif //defined(__GNUC__) && __GNUC__ > 2

#endif  /* !CYBERGRAPHX_CYBERGRAPHICS_H */

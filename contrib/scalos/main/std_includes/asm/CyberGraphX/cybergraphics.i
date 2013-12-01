;*
;*	$VER: cybergraphics.i 41.18 (21.02.1998)
;*
;*	include file for cybergraphics.library
;*
;*	Copyright © 1996-1998 by phase5 digital products
;*      All Rights reserved.
;*
;*

			IFND	CYBERGRAPHX_CYBERGRAPHICS_I
CYBERGRAPHX_CYBERGRAPHICS_I	SET	1


			INCLUDE "exec/nodes.i"
			INCLUDE "utility/tagitem.i"
			INCLUDE	"graphics/displayinfo.i"

;
; Definition of CyberModeNode (Returned in AllocModeList)
;
			STRUCTURE	CyberModeNode,0
			STRUCT	cmn_Node,LN_SIZE
			STRUCT	cmn_ModeText,DISPLAYNAMELEN	; name for this mode
			ULONG	cmn_DisplayID			; display id associated with the node
			UWORD	cmn_Width			; visible width
			UWORD	cmn_Height			; visible height
			UWORD	cmn_Depth			; display depth
			APTR	cmn_DisplayTagList		; taglist with extended ModeID information
			LABEL	cmn_SIZEOF

		
;
; Parameters for GetCyberMapAttr()
;

CYBRMATTR_XMOD		EQU	($80000001)	; function returns BytesPerRow if its called with this parameter
CYBRMATTR_BPPIX		EQU	($80000002)	; BytesPerPixel shall be returned
CYBRMATTR_DISPADR	EQU	($80000003)	; do not use this ! private tag
CYBRMATTR_PIXFMT	EQU	($80000004)	; the pixel format is returned
CYBRMATTR_WIDTH		EQU	($80000005)	; returns width in pixels
CYBRMATTR_HEIGHT	EQU	($80000006)	; returns height in lines
CYBRMATTR_DEPTH		EQU	($80000007)	; returns bits per pixel
CYBRMATTR_ISCYBERGFX	EQU	($80000008)	; returns if supplied bitmap is a cybergfx one
CYBRMATTR_ISLINEARMEM	EQU	($80000009)	; returns -1 if supplied bitmap is linear accessable

;
; Parameters for GetCyberIDAttr()
;

CYBRIDATTR_PIXFMT	EQU	($80000001)	; the pixel format is returned
CYBRIDATTR_WIDTH	EQU	($80000002)	; returns visible width in pixels
CYBRIDATTR_HEIGHT	EQU	($80000003)	; returns visible height in lines
CYBRIDATTR_DEPTH	EQU	($80000004)	; returns bits per pixel
CYBRIDATTR_BPPIX	EQU	($80000005)	; BytesPerPixel shall be returned

;
; Tags for CModeRequestTagList()
;

CYBRMREQ_TB		EQU	(TAG_USER+$40000)
;
; FilterTags
;
CYBRMREQ_MinDepth	EQU	(CYBRMREQ_TB+0)		; Minimum depth for displayed screenmode
CYBRMREQ_MaxDepth	EQU	(CYBRMREQ_TB+1)		; Maximum depth  "       "        "
CYBRMREQ_MinWidth	EQU	(CYBRMREQ_TB+2)		; Minumum width  "       "        "
CYBRMREQ_MaxWidth	EQU	(CYBRMREQ_TB+3)		; Maximum width  "       "        "
CYBRMREQ_MinHeight	EQU	(CYBRMREQ_TB+4)		; Minumum height "       "        "
CYBRMREQ_MaxHeight	EQU	(CYBRMREQ_TB+5)		; Minumum height "       "        "
CYBRMREQ_CModelArray	EQU	(CYBRMREQ_TB+6)		; Filters certain color models

CYBRMREQ_WinTitle	EQU	(CYBRMREQ_TB+20)
CYBRMREQ_OKText		EQU	(CYBRMREQ_TB+21)
CYBRMREQ_CancelText	EQU	(CYBRMREQ_TB+22)

CYBRMREQ_Screen		EQU	(CYBRMREQ_TB+30)	; Screen you wish the Requester to opened on

;
; Tags for BestCyberModeID()
;

CYBRBIDTG_TB		EQU	(TAG_USER+$50000)
;
; FilterTags
;
CYBRBIDTG_Depth		EQU	(CYBRBIDTG_TB+0)
CYBRBIDTG_NominalWidth	EQU	(CYBRBIDTG_TB+1)
CYBRBIDTG_NominalHeight	EQU	(CYBRBIDTG_TB+2)
CYBRBIDTG_MonitorID	EQU	(CYBRBIDTG_TB+3)
CYBRBIDTG_BoardName	EQU	(CYBRBIDTG_TB+5)


PIXFMT_LUT8		EQU	(0)
PIXFMT_RGB15		EQU	(1)
PIXFMT_BGR15		EQU	(2)
PIXFMT_RGB15PC		EQU	(3)
PIXFMT_BGR15PC		EQU	(4)
PIXFMT_RGB16		EQU	(5)
PIXFMT_BGR16		EQU	(6)
PIXFMT_RGB16PC		EQU	(7)
PIXFMT_BGR16PC		EQU	(8)
PIXFMT_RGB24		EQU	(9)
PIXFMT_BGR24		EQU	(10)
PIXFMT_ARGB32		EQU	(11)
PIXFMT_BGRA32		EQU	(12)
PIXFMT_RGBA32		EQU	(13)

PIXFMT_CNT		EQU	(14)

;
; SrcRectangle formats defines for xxxPixelArray() calls
;

RECTFMT_RGB		EQU	(0)
RECTFMT_RGBA		EQU	(1)
RECTFMT_ARGB		EQU	(2)
RECTFMT_LUT8		EQU	(3)
RECTFMT_GREY8		EQU	(4)


;
; Parameters for CVideoCtrlTagList()
;

SETVC_DPMSLevel		EQU	($88002001)

DPMS_ON			EQU	(0)	; Full operation
DPMS_STANDBY		EQU	(1)	; Optional state of minimal power reduction
DPMS_SUSPEND		EQU	(2)	; Significant reduction of power consumption
DPMS_OFF		EQU	(3)	; Lowest level of power consumption


;
; Parameters for LockBitMapTagList()
;

LBMI_WIDTH		EQU	($84001001)
LBMI_HEIGHT		EQU	($84001002)
LBMI_DEPTH		EQU	($84001003)
LBMI_PIXFMT		EQU	($84001004)
LBMI_BYTESPERPIX	EQU	($84001005)
LBMI_BYTESPERROW	EQU	($84001006)
LBMI_BASEADDRESS	EQU	($84001007)


;
; Parameters for UnLockBitMapTagList()
;

UBMI_UPDATERECTS	EQU	($85001001)
UBMI_REALLYUNLOCK	EQU	($85001002)


;							  
; Message passed to the DoCDrawMethodTagList()
; hook function
;							  

	STRUCTURE	CDrawMsg,0
	        APTR    cdm_MemPtr
        	ULONG   cdm_offx
	        ULONG   cdm_offy
        	ULONG   cdm_xsize
	        ULONG   cdm_ysize
        	UWORD   cdm_BytesPerRow
	        UWORD   cdm_BytesPerPix
        	UWORD   cdm_ColorModel
		LABEL	cdm_SIZEOF

;
; Colour Table source formats for WriteLUTPixelArray()
;

CTABFMT_XRGB8	EQU	(0)	; ULONG [] table
				; X,R,G,B order, 8-bits-per-gun



;
; graphics.library/AllocBitmap() extended flags
;


BMB_SPECIALFMT	EQU	(7)
BMF_SPECIALFMT	EQU	($80)

			ENDC


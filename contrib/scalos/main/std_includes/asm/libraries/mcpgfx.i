	IFND	LIBRARIES_MCPGFX_I
LIBRARIES_MCPGFX_I	set	1

	IFND	EXEC_TYPES_I
	include "exec/types.i"
	ENDC

	IFND	UTILITY_TAGITEM_I
	include "utility/tagitem.i"
	ENDC

	IFND	INTUITION_SCREENS_I
	include "intuition/screens.i"
	ENDC


; Types of frames available
MF_FRAME_NONE:		equ	0	; No frame
MF_FRAME_BUTTON:	equ	1	; Standard 3D frame used for buttons
MF_FRAME_BORDER:	equ	2	; Standard 2D frame used for
MF_FRAME_STRING:	equ	3	; String
MF_FRAME_DROPBOX:	equ	4	; Dropbox [String (with space)?]
MF_FRAME_XEN:		equ	5	; Standard XEN button
MF_FRAME_MWB:		equ	6	; Standard MWB
MF_FRAME_THICK:		equ	7	; Standard Thick
MF_FRAME_XWIN:		equ	8	; Standard XWIN
MF_FRAME_MAXIMUM:	equ	9	; Maximum number of frame types


; Sizes of frame egdes. Always in the order x1,y1,x2,y2,(recessed)x1,y1,x2,y2.
   STRUCTURE FrameSize,0
	STRUCT	fs_Sizes,2*8
   LABEL FrameSize_SIZEOF


; Some constants for acessing the values from the FrameSize structure
FRAMESIZE_LEFT		equ	0	; Normal left edge width
FRAMESIZE_TOP		equ	1	; Normal top edge width
FRAMESIZE_RIGHT		equ	2	; Normal right edge width
FRAMESIZE_BOTTOM	equ	3	; Normal bottom edge width
FRAMESIZE_RELEFT	equ	4	; Recessed left edge width
FRAMESIZE_RETOP		equ	5	; Recessed top edge width
FRAMESIZE_RERIGHT	equ	6	; Recessed right edge width
FRAMESIZE_REBOTTOM	equ	7	; Recessed bottom edge width


; Tags for the various function in mcpgfx.library
MCP_TagBase:		equ	TAG_USER+$20000

;IA_Left:		equ	MCP_TagBase+$01
;IA_Top			equ	MCP_TagBase+$02
;IA_Width		equ	MCP_TagBase+$03
;IA_Height		equ	MCP_TagBase+$04
;IA_FGPen		equ	MCP_TagBase+$05
;IA_BGPen		equ	MCP_TagBase+$06
;IA_Data		equ	MCP_TagBase+$07
;IA_ShadowPen		equ	MCP_TagBase+$09
;IA_ShinePen		equ	MCP_TagBase+$0a
;IA_APattern		equ	MCP_TagBase+$10
;IA_APatSize		equ	MCP_TagBase+$11
;IA_Mode		equ	MCP_TagBase+$12
;IA_Recessed		equ	MCP_TagBase+$15
IA_DrawInfo		equ	MCP_TagBase+$18
;IA_FrameType		equ	MCP_TagBase+$1b
IA_HalfShadowPen	equ	MCP_TagBase+$1c
IA_HalfShinePen		equ	MCP_TagBase+$1d


; A little something
   STRUCTURE ExtDrawInfo,0
	STRUCT	edi_DrawInfo,dri_longreserved+20	; Original screen DrawInfo (copy of)
	APTR	edi_Pens;			; Pointer to an array of pens to use for MWB colours
	STRUCT	edi_dunno,2*4;			; like I say
   LABEL ExtDrawInfo_SIZEOF


	ENDC	; LIBRARIES_MCPGFX_H

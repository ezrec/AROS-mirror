**
**  $VER: iconobject.i 41.6 (23 Jan 2008 21:30:36)
**
**  (C) Copyright 1996-1999 ALiENDESiGN
**  (C) Copyright 2000-2008 The Scalos Team
**  All Rights Reserved

	IFND DATATYPES_ICONOBJECT_I
DATATYPES_ICONOBJECT_I	SET 1

;------------------------------------------------------------------------------

	IFND EXEC_TYPES_I
	INCLUDE "exec/types.i"
	ENDC

	IFND INTUITION_IMAGECLASS_I
	INCLUDE "intuition/imageclass.i"
	ENDC

	IFND DATATYPES_DATATYPESCLASS_I
	INCLUDE "datatypes/datatypesclass.i"
	ENDC

;------------------------------------------------------------------------------


;DTA_Dummy	 EQU $80001000	     ;TAGUSER+$1000

;DTA_Name	 EQU DTA_Dummy+100

;diskobject.datatype

;Attributes
IDTA_Type		EQU	DTA_Dummy+901	;(.SG) Icon Type
IDTA_Text		EQU	DTA_Dummy+902	;(ISG) Icon Text
IDTA_DefaultTool	EQU	DTA_Dummy+903	;(.SG) DefaultTool
IDTA_WindowRect		EQU	DTA_Dummy+904	;(.SG) WindowSize
IDTA_ToolTypes		EQU	DTA_Dummy+905	;(.SG) Tooltype Array
IDTA_Stacksize		EQU	DTA_Dummy+906	;(.SG) Stacksize
IDTA_InnerLeft		EQU	DTA_Dummy+907	;(ISG)
IDTA_InnerTop		EQU	DTA_Dummy+908	;(ISG)
IDTA_InnerRight		EQU	DTA_Dummy+909	;(ISG)
IDTA_InnerBottom	EQU	DTA_Dummy+910	;(ISG)
IDTA_FrameType		EQU	DTA_Dummy+911	;(ISG) NormalFrame
IDTA_WinCurrentX	EQU	DTA_Dummy+912	;(.SG)
IDTA_WinCurrentY	EQU	DTA_Dummy+913	;(.SG)
IDTA_LayoutFlags	EQU	DTA_Dummy+914	;(..G)
IDTA_Extention		EQU	DTA_Dummy+915	;(..G) e.g. ".info"
IDTA_Mask_Normal	EQU	DTA_Dummy+916	;(.SG) BlitMask
IDTA_Mask_Selected	EQU	DTA_Dummy+917	;(.SG) BlitMask
IDTA_Flags		EQU	DTA_Dummy+918	;(.SG) DrawerData Flags
IDTA_ViewModes		EQU	DTA_Dummy+919	;(.SG) DrawerData ViewModes
IDTA_DefType		EQU	DTA_Dummy+920	;(I..) DefIcon Type
IDTA_MaxDepth		EQU	DTA_Dummy+921	;(.S.) of the bitmap
IDTA_TextPenOutline	EQU	DTA_Dummy+922	;(ISG) textpen for outline text
IDTA_TextMode		EQU	DTA_Dummy+923	;(IS.) textmode
IDTA_TextSkip		EQU	DTA_Dummy+924	;(IS.) space between text and image
IDTA_FrameTypeSel	EQU	DTA_Dummy+925	;(ISG) SelectedFrame
IDTA_HalfShadowPen	EQU	DTA_Dummy+926	;(IS.) needed for some frames
IDTA_HalfShinePen	EQU	DTA_Dummy+927	;(IS.) needed for some frames
IDTA_RenderHook:	EQU	DTA_Dummy+928	;(IS.) needed for animated AppIcons +jl+
IDTA_DoFreeDiskObject:	EQU	DTA_Dummy+929	;(I..) Free DiskObject delivered by AIDTA_Icon +jl+
IDTA_TextStyle:		EQU	DTA_Dummy+930	;(ISG) ULONG +jl+ 20011231 Text style for icon text
IDTA_Font:		EQU	DTA_Dummy+931	;(ISG) (struct TextFont *) +jl+ 20010818 font to render icon text in */
IDTA_SupportedIconTypes: EQU	DTA_Dummy+932	;(I..) ULONG bits enable specific icon types
IDTA_TextPen:		EQU	DTA_Dummy+933	;(ISG) icon text pen
IDTA_TextPenSel:	EQU	DTA_Dummy+934	;(ISG) selected icon text pen
IDTA_TextPenShadow:	EQU	DTA_Dummy+935	;(ISG) icon text shadow pen
IDTA_MaskBM_Normal	EQU	DTA_Dummy+936	;(..G) struct BitMap * +jl+ 20020201
IDTA_MaskBM_Selected	EQU	DTA_Dummy+937	;(..G) struct BitMap * +jl+ 20020201
IDTA_UserFlags		EQU	DTA_Dummy+938	;(ISG) ULONG
IDTA_Borderless		EQU	DTA_Dummy+939	;(ISG) ULONG always draw icon without border
IDTA_AlphaChannel	EQU	DTA_Dummy+940	;(..G) UBYTE * byte array of alpha channel information 
						;       or NULL if no alpha channel supported
IDTA_Fonthandle		EQU	DTA_Dummy+942	;(ISG) (APTR) opaque font handle to use with font function vector
IDTA_FontHook		EQU	DTA_Dummy+943	;(ISG) (struct Hook *) font function Hook
IDTA_MultiLineText	EQU	DTA_Dummy+944	;(ISG) ULONG - Flag: allow splitting of icon text into multiple lines 
IDTA_ARGBImageData	EQU	DTA_Dummy+945	;(..G) struct ARGBHeader * image data in ARGB format. Not available for all icon types */
IDTA_SizeConstraints	EQU	DTA_Dummy+946	;(I..) struct Rectangle * - set min/max sizes for icon
IDTA_Width_Mask_Normal	EQU	DTA_Dummy+947	;(ISG) ULONG - Witdh of non-selected mask bitplane
IDTA_Height_Mask_Normal	EQU	DTA_Dummy+948	;(ISG) UBYTE * - Height of non-selected mask bitplane
IDTA_Width_Mask_Selected EQU	DTA_Dummy+949	;(ISG) UBYTE * - Width of selected mask bitplane
IDTA_Height_Mask_Selected EQU	DTA_Dummy+950	;(ISG) UBYTE * - Height of non-selected mask bitplane
IDTA_CopyARGBImageData	EQU	DTA_Dummy+951	;(ISG) ULONG - If TRUE, copy IDTA_ARGBImageData
IDTA_IconType		EQU	DTA_Dummy+952	;(..G) ULONG - type of icon */
IDTA_NumberOfColorsSupported EQU DTA_Dummy+953	;(..G) ULONG - number of colors supported by icon type (e.g. 256 by GlowIcons + NewIcons) */
IDTA_OverlayType	EQU	DTA_Dummy+954	;(ISG) ULONG - type of overlay to draw over icon - not used, but only stored by datatype */
IDTA_Backfill		EQU	DTA_Dummy+955	; (ISG) ULONG - Pen number to use for unselected background fill
IDTA_BackfillSel	EQU	DTA_Dummy+956	; (ISG) ULONG - Pen number to use for selected background fill
IDTA_TextDrawMode	EQU	DTA_Dummy+957	; (ISG) UBYTE - draw mode for icon Text (JAM1 or JAM2)
IDTA_TextBackPen	EQU	DTA_Dummy+958	; (ISG) UBYTE - icon text background color for IDTA_TextDrawMode = IDTA_TextDrawMode
IDTA_CopySelARGBImageData EQU	DTA_Dummy+959	; (ISG) ULONG - If TRUE, copy IDTA_SelARGBImageData
IDTA_SelARGBImageData	EQU	DTA_Dummy+960	; (..G) struct ARGBHeader * selected image data in ARGB format. Not available for all icon types
IDTA_SelAlphaChannel	EQU	DTA_Dummy+961	; (..G) UBYTE * - byte array of alpha channel information
IDTA_ToolWindow		EQU	DTA_Dummy+962	; (.SG) STRPTR - Tool Window
						;		   or NULL if no alpha channel supported
IDTA_CloneIconObject	EQU	DTA_Dummy+963	; (I..)
IDTA_TextPenBgSel	EQU	DTA_Dummy+964	; (ISG) UWORD - pen for rectangle around selected icon text
IDTA_SelectedTextRectangle EQU	DTA_Dummy+965	; (I.G) ULONG - Flag: draw rectangle around selected icon text
IDTA_SelTextRectBorderX EQU     DTA_Dummy+966	; (ISG) UWORD - Additional horizontal border around selected icon text rectangle
IDTA_SelTextRectBorderY EQU     DTA_Dummy+967	; (ISG) UWORD - Additional vertical border around selected icon text rectangle
IDTA_SelTextRectRadius  EQU     DTA_Dummy+968	; (ISG) UWORD - Radius for selected icon text rectangle corners

IDTA_UnscaledWidth      EQU	DTA_Dummy+969	; (ISG) ULONG - unscaled width of icon
IDTA_UnscaledHeight     EQU     DTA_Dummy+970	; (ISG) ULONG - unscaled width of icon

IDTA_ScalePercentage	EQU	DTA_Dummy+971	; (I..) UWORD - Scale factor in percent

; allowed range for IDTA_ScalePercentage
IDTA_ScalePercentage_MIN	EQU	25
IDTA_ScalePercentage_MAX	EQU	400

;amigaicon.datatype
AIDTA_Icon		EQU	DTA_Dummy+1101	;(I.G) original Icon

;IDTA_TextMode
IDTV_TextMode_Normal	EQU	0
IDTV_TextMode_Outline	EQU	1
IDTV_TextMode_Shadow	EQU	2

;IDTA_ViewModes
IDTV_ViewModes_Default		EQU	0
IDTV_ViewModes_Icon		EQU	1
IDTV_ViewModes_Name		EQU	2
IDTV_ViewModes_Size		EQU	3
IDTV_ViewModes_Date		EQU	4
IDTV_ViewModes_Time		EQU	5
IDTV_ViewModes_Comment		EQU	6
IDTV_ViewModes_Protection	EQU	7
IDTV_ViewModes_Owner		EQU	8
IDTV_ViewModes_Group		EQU	9
IDTV_ViewModes_Type		EQU	10
IDTV_ViewModes_Version		EQU	11
IDTV_ViewModes_MiniIcon		EQU	12

; IDTA_SupportedIconTypes values (bit mask)
IDTV_IconType_NewIcon	equ	$00000001
IDTV_IconType_ColorIcon	equ	$00000002

;/* IDTA_UserFlags flag bits */
	; this icon should be rendered in shadowed state
	BITDEF	ICONOBJ_USERFLAG,DrawShadowed,0
	BITDEF	ICONOBJ_USERFLAG,DefaultIcon,1
	BITDEF	ICONOBJ_USERFLAG,DrawHighlite,2

;------------------------------------------------------------------------------
    STRUCTURE   ARGB,0
	UBYTE	Alpha
	UBYTE	Red
	UBYTE	Green
	UBYTE	Blue
    LABEL	ARGB_SIZEOF

    STRUCTURE	ARGBHeader,0
	ULONG	argb_Width
	ULONG	argb_Height
	APTR	argb_ImageData		; struct ARGB *
    LABEL	ARGBHeader_SIZEOF

;------------------------------------------------------------------------------

; Icon types - parameter for IDTA_IconType

ioICONTYPE_Standard	EQU	0     ; standard icon
ioICONTYPE_NewIcon	EQU	1
ioICONTYPE_GlowIcon	EQU	2     ; OS3.5+ GlowIcon
ioICONTYPE_PngIcon	EQU	3

;------------------------------------------------------------------------------

    STRUCTURE   ScalosBitMapAndColor,0
	ULONG		sac_Width
	ULONG 		sac_Height
	ULONG 		sac_Depth
	APTR		sac_BitMap		; struct BitMap *
	ULONG 		sac_TransparentColor	; number of pen for transparent color
	ULONG 		sac_NumColors		; number of entries in sac_ColorTable
	APTR		sac_ColorTable		; ULONG * -- color table, 3 longwords per entry
    LABEL       ScalosBitMapAndColor_SIZEOF

; set sac_TransparentColor to this value for no transparency
SAC_TRANSPARENT_NONE	equ	$ffffffff

;------------------------------------------------------------------------------

; parameter for IDTA_FontFunctions 

iofMsg_DONERASTPORT	equ	0
iofMsg_SETFONT		equ	1
iofMsg_TEXT		equ	2
iofMsg_SETFONTSTYLE	equ	3
iofMsg_TEXTEXTENT	equ	4
iofMsg_TEXTFIT		equ	5
iofMsg_GETFONTHEIGHT	equ	6
iofMsg_GETFONTBASELINE	equ	7
iofMsg_SETTRANSPARENCY	equ	8


   STRUCTURE	ioFontMsg_DoneRastPort,4
;	ULONG 	MsgID;		; iofMsg_DONERASTPORT
	APTR	iofdr_RastPort
	APTR	iofdr_FontHandle
   LABEL	iofdr_SIZEOF

   STRUCTURE	ioFontMsg_SetFont,4
;	ULONG	MsgID		; iofMsg_SETFONT
	APTR	iofsf_RastPort
	APTR	iofsf_FontHandle
	APTR	iofsf_TextFont
   LABEL	iofsf_SIZEOF

   STRUCTURE	ioFontMsg_Text,4
;	ULONG	MsgID		; iofMsg_TEXT
	APTR	ioft_RastPort
	APTR	ioft_FontHandle
	APTR	ioft_String
	WORD	ioft_Length
   LABEL	ioft_SIZEOF

   STRUCTURE	ioFontMsg_SetFontStyle,4
;	ULONG	MsgID		; iofMsg_SETFONTSTYLE
	APTR	iofsfs_RastPort
	APTR	iofsfs_FontHandle
	ULONG	iofsfs_Style
	ULONG	iofsfs_Enable
   LABEL	iofsfs_SIZEOF

   STRUCTURE	ioFontMsg_TextExtent,4
;	ULONG	MsgID		; iofMsg_TEXTEXTENT
	APTR	iofte_RastPort
	APTR	iofte_FontHandle
	APTR	iofte_String
	WORD	iofte_Length
	APTR	iofte_TextExtent
   LABEL	iofte_SIZEOF

   STRUCTURE	ioFontMsg_TextFit,4
;	ULONG	MsgID		; iofMsg_TEXTFIT
	APTR	ioftf_RastPort
	APTR	ioftf_FontHandle
	APTR	ioftf_String
	WORD	ioftf_Length
	APTR	ioftf_TextExtent
	APTR	ioftf_ConstrainingTextExtent
	WORD	ioftf_StrDirection
	UWORD	ioftf_ConstrainingBitWidth
	UWORD	ioftf_ConstrainingBitHeight
   LABEL	ioftf_SIZEOF

   STRUCTURE	ioFontMsg_GetFontHeight,4
;	ULONG 	MsgID		; iofMsg_GETFONTHEIGHT
	APTR	iofgfh_RastPort
	APTR	iofgfh_FontHandle
   LABEL	iofgfh_SIZEOF

   STRUCTURE	ioFontMsg_GetFontBaseline,4
;	ULONG	MsgID		; iofMsg_GETFONTBASELINE
	APTR	iofgfb_RastPort
	APTR	iofgfb_FontHandle
   LABEL	iofgfb_SIZEOF

   STRUCTURE	ioFontMsg_SetTransparency,4
;	ULONG MsgID;			; iofMsg_SETTRANSPARENCY
	APTR	iofst_RastPort
	APTR	iofst_FontHandle
	ULONG	iofst_Transparency	; 0=no transparency (opaque), 255=maximum transparency
   LABEL	iofst_SIZEOF

;------------------------------------------------------------------------------

;Icon object methods
IDTM_Layout		EQU	DTA_Dummy+1001
IDTM_Draw		EQU	DTA_Dummy+1002
IDTM_Write		EQU	DTA_Dummy+1003	;PutDiskObject
IDTM_FindToolType	EQU	DTA_Dummy+1004
IDTM_GetToolTypeValue	EQU	DTA_Dummy+1005
IDTM_FreeLayout		EQU	DTA_Dummy+1006
IDTM_Erase		EQU	DTA_Dummy+1007
ITDM_ScaleIcon		EQU	DTA_Dummy+1008
IDTM_NewImage		EQU	DTA_Dummy+1009
IDTM_CloneIconObject	EQU	DTA_Dummy+1010

;------------------------------------------------------------------------------

;IDTM_Layout method

    STRUCTURE iopLayout, 4
	;ULONG	     MethodID
	APTR	    iopl_Screen
	APTR	    iopl_Window
	APTR	    iopl_RastPort
	APTR	    iopl_DrawInfo
	ULONG	    iopl_Flags
    LABEL iopl_SIZEOF

;IDTM_Layout flags
    BITDEF  IOLAYOUT,NormalImage,0
    BITDEF  IOLAYOUT,SelectedImage,1

;------------------------------------------------------------------------------

;IDTM_Draw method

    STRUCTURE iopDraw, 4
	;ULONG	     MethodID
	APTR	    iopd_Screen
	APTR	    iopd_Window
	APTR	    iopd_RastPort
	APTR	    iopd_DrawInfo
;----	     ULONG	 iopd_Flags  --- WRONG +jl+
	ULONG	    iopd_XOffset
	ULONG	    iopd_YOffset
	ULONG	    iopd_DrawFlags
    LABEL	    iopd_SIZEOF

;IDTM_Draw draw flags
    BITDEF  IODRAW,NoText,0
    BITDEF  IODRAW,AbsolutePos,1    ; X/YOffset is absolute position
    BITDEF  IODRAW,NoImage,2
    BITDEF  IODRAW,NoEraseBg,4		; Do not erase background

;------------------------------------------------------------------------------

;IDTM_Erase method

    STRUCTURE iopErase, 4
	;ULONG	     MethodID
	APTR	    iope_Screen
	APTR	    iope_Window
	APTR	    iope_RastPort
	ULONG	    iope_XOffset
	ULONG	    iope_YOffset
	ULONG	    iope_EraseFlags
    LABEL	    iope_SIZEOF

;IDTM_Erase draw flags

;----------------------------------------------------------------------------------

;IDTM_FindToolType method

    STRUCTURE iopFindToolType,4
	;ULONG	MethodID;
	APTR	ioftt_ToolTypeName	;Name of tooltype to search
	APTR	ioftt_ToolTypeValue	;On return: Contents of tooltype or NULL if not found.
    					;on Call: NULL to search from first tooltype, 
					;or result of last IDTM_FindToolType.
    LABEL	ioftt_SIZEOF

;----------------------------------------------------------------------------------

;IDTM_GetToolTypeValue method

    STRUCTURE iopGetToolTypeValue,4
	;ULONG	MethodID
	APTR	iotv_ToolTypeValue	; result from IDTM_FindToolType
	APTR	iotv_Value		; Pointer to LONG for storing the result
    LABEL	iotv_SIZEOF

;------------------------------------------------------------------------------

;IDTM_Write method

    STRUCTURE iopWrite,4
	;ULONG	MethodID
	APTR	iopw_Path
	APTR	iopw_SourceObject
	APTR	iopw_Tags
    LABEL 	iopw_SIZEOF

;Tags for IDTM_Write
ICONA_NoNewImage	EQU	$80000001
ICONA_NoPosition	EQU	$80000002
ICONA_NoUsePosition	EQU	$80000003
ICONA_NotifyWorkbench	EQU	$80000004	; ULONG - Flag: notify Scalos that the saved icon needs to be updated. Default is FALSE.

;----------------------------------------------------------------------------------

;IDTM_FreeLayout method

  STRUCTURE iopFreeLayout,4
	;ULONG	MethodID;		; IDTM_FreeLayout
	ULONG	iopf_Flags;		; Additional flags
  LABEL 	iopf_SIZEOF

; Flags
    BITDEF  IOFREELAYOUT,ScreenAvailable,0

;----------------------------------------------------------------------------------

;ITDM_ScaleIcon method

   STRUCTURE iopScaleIcon,4
	;ULONG	MethodID;		; ITDM_ScaleIcon
	ULONG	iops_NewWidth;		; Scaled icon width (Gadget->Width, BoundsWidth will be adjusted accordingly)
	ULONG	iops_NewHeight;		; Scaled icon height (Gadget->Height, BoundsHeight will be adjusted accordingly)
	ULONG	iops_Flags;		; flags
  LABEL 	iops_SIZEOF

; Flags
; none defined ATM

;----------------------------------------------------------------------------------

; IDTM_NewImage method

    STRUCTURE	iopNewImage,4
	;ULONG	MethodID		; IDTM_NewImage
	APTR	ioni_NormalImage	; struct ScalosBitMapAndColor *
	APTR	ioni_SelectedImage	; struct ScalosBitMapAndColor * -- may be NULL
    LABEL       iopNewImage_SIZEOF

;------------------------------------------------------------------------------

; IDTM_CloneIconObject method

    STRUCTURE	iopCloneIconObject,4
	;ULONG	MethodID		; IDTM_CloneIconObject
	APTR	iocio_TagList;		; struct TagItem *
    LABEL       iopCloneIconObject_SIZEOF

;------------------------------------------------------------------------------

;texticon.datatype (Scalos internal)

;TIDTA_ExAllData		 EQU	 DTA_Dummy+1151	 ;(IS.) deprecated
;TIDTA_ExAllType		 EQU	 DTA_Dummy+1152	 ;(IS.) deprecated
TIDTA_WidthArray		EQU	DTA_Dummy+1153	;(ISG)
;TIDTA_ExNextData		 EQU	 DTA_Dummy+1154	 ;(IS.) deprecated
TIDTA_FileType			EQU	DTA_Dummy+1155	;(..G)
TIDTA_Size			EQU	DTA_Dummy+1156	;(..G)
TIDTA_Days			EQU	DTA_Dummy+1157	;(..G)
TIDTA_Mins			EQU	DTA_Dummy+1158	;(..G)
TIDTA_Ticks			EQU	DTA_Dummy+1159	;(..G)
TIDTA_Comment			EQU	DTA_Dummy+1160	;(..G)
TIDTA_Owner_UID			EQU	DTA_Dummy+1161	;(..G) UWORD
TIDTA_Owner_GID			EQU	DTA_Dummy+1162	;(..G) UWORD
TIDTA_Protection		EQU	DTA_Dummy+1163	;(..G)
TIDTA_IsLink			equ	DTA_Dummy+1164	;(ISG) BOOL
TIDTA_Font			equ	DTA_Dummy+1165	;(ISG) (struct TextFont *) - font to render icon text in +jl+ 20010903
TIDTA_IconType			equ	DTA_Dummy+1167	;(ISG) ULONG - Workbench icon type +jl+ 20010906
TIDTA_TTFont			equ	DTA_Dummy+1168	;(ISG) APTR - ttEngine font to render icon text in +jl+ 20030110
TIDTA_WindowTask		equ	DTA_Dummy+1169	;(I..) struct ScaWindowTask * - parent WindowTask
TIDTA_IconObject		equ	DTA_Dummy+1170	;(ISG) Object *
TIDTA_SoloIcon			equ	DTA_Dummy+1171	;(ISG) ULONG (Boolean) TRUE if this an icon w/o object
TIDTA_ColumnWidthChangeHook	equ	DTA_Dummy+1172	;(ISG) struct Hook * - Hook is called when icon layout causes column width to change
TIDTA_SelectNameOnly		equ	DTA_Dummy+1173	;(ISG) ULONG - if TRUE, icon can only be selected at name column
TIDTA_TypeNode			equ	DTA_Dummy+1174	;(ISG) struct TypeNode * - TypeNode Workbench icon type
TIDTA_TextPenDrawerNormal 	EQU	DTA_Dummy+1175	;(ISG) UWORD - text window normal  icon text pen for drawers
TIDTA_TextPenDrawerSelected 	EQU	DTA_Dummy+1176	;(ISG) UWORD - text window selected icon text pen  for drawers
TIDTA_TextPenFileNormal		equ	DTA_Dummy+1177	;(ISG) UWORD - text window normal  icon text pen for files
TIDTA_TextPenFileSelected	equ	DTA_Dummy+1178	;(ISG) UWORD - text window selected icon text pen for files
TIDTA_ReadIconListData		equ	DTA_Dummy+1180	;(IS.) struct ReadIconListData *

;------------------------------------------------------------------------------

;FrameTags mcpgfx.library
;IA_FrameType		 EQU	$8002001b
;IA_Recessed		 EQU	$80020015
;IA_ShadowPen		 EQU	$80020009
;IA_ShinePen		 EQU	$8002000a
;IA_HalfShadowPen	 EQU	$8002001c	; commented out +jl+ because of duplicate definition
;IA_HalfShinePen	 EQU	$8002001d	; commented out +jl+ because of duplicate definition
;IA_BGPen		 EQU	$80020006
;IA_FGPen		 EQU	$80020005
;IA_APattern		 EQU	$80020010
;IA_APatSize		 EQU	$80020011
;IA_Left		 EQU	$80020001
;IA_Top			 EQU	$80020002
;IA_Width		 EQU	$80020003
;IA_Height		 EQU	$80020004
;IA_Mode		 EQU	$80020012
;IA_Data		 EQU	$80020007
;IA_DrawInfo		 EQU	$80020018

;------------------------------------------------------------------------------

	endc

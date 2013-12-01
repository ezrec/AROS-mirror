#ifndef DATATYPES_ICONOBJECT_H
#define DATATYPES_ICONOBJECT_H
/*
**  $VER: iconobject.h 41.6 (23 Jan 2008 21:30:36)
**
**	iconobject.library include
**
**  (C) Copyright 1996-1999 ALiENDESiGN
**  (C) Copyright 2000-2008 The Scalos Team
**  All Rights Reserved
*/

#ifndef	 EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef	 INTUITION_IMAGECLASS_H
#include <intuition/imageclass.h>
#endif

#ifndef	 DATATYPES_DATATYPESCLASS_H
#include <datatypes/datatypesclass.h>
#endif


#define	ICONOBJECTNAME		"iconobject.library"


/* #define DTA_Dummy		       0x80001000      // TAGUSER+0x1000 */


/*
 * Icon object attributes
 */

#define IDTA_Type			DTA_Dummy+901	/* (.SG) ULONG - Icon Type */
#define IDTA_Text			DTA_Dummy+902	/* (ISG) STRPTR - Icon Text */
#define IDTA_DefaultTool		DTA_Dummy+903	/* (.SG) STRPTR - DefaultTool */
#define IDTA_WindowRect			DTA_Dummy+904	/* (.SG) struct IBox * - WindowSize - Get can return NULL if icon has no DrawerData information */

#define IDTA_ToolTypes			DTA_Dummy+905	/* (.SG) STRPTR * - Tooltype Array */
		/*    Beware: a tooltype pointer received by GetAttr() will get invalid */
		/*    (memory gets freed) as soon as tooltypes are changed with */
		/*    SetAttrs(IDTA_ToolTypes...). */

#define IDTA_Stacksize			DTA_Dummy+906	/* (.SG) Stacksize */
#define IDTA_InnerLeft			DTA_Dummy+907	/* (ISG) */
#define IDTA_InnerTop			DTA_Dummy+908	/* (ISG) */
#define IDTA_InnerRight			DTA_Dummy+909	/* (ISG) */
#define IDTA_InnerBottom		DTA_Dummy+910	/* (ISG) */
#define IDTA_FrameType			DTA_Dummy+911	/* (ISG) */
#define IDTA_WinCurrentX		DTA_Dummy+912	/* (.SG) */
#define IDTA_WinCurrentY		DTA_Dummy+913	/* (.SG) */
#define IDTA_LayoutFlags		DTA_Dummy+914	/* (..G) */
#define IDTA_Extention			DTA_Dummy+915	/* (..G) e.g. ".info" */
#define IDTA_Mask_Normal		DTA_Dummy+916	/* (.SG) UBYTE * - BlitMask */
#define IDTA_Mask_Selected		DTA_Dummy+917	/* (.SG) UBYTE * - BlitMask */
#define IDTA_Flags			DTA_Dummy+918	/* (.SG) DrawerData Flags */
#define IDTA_ViewModes			DTA_Dummy+919	/* (.SG) DrawerData ViewModes */
#define IDTA_DefType			DTA_Dummy+920	/* (I..) ULONG - DefIcon Type */
//#define IDTA_MaxDepth			DTA_Dummy+921	/* (.S.) of the bitmap */	UNUSED
#define IDTA_TextPenOutline		DTA_Dummy+922	/* (ISG) UWORD - textpen for outline text */
#define IDTA_TextMode			DTA_Dummy+923	/* (IS.) UBYTE - textmode - IDTV_TextMode_*** */
#define IDTA_TextSkip			DTA_Dummy+924	/* (IS.) UBYTE - space between text and image */
#define IDTA_FrameTypeSel		DTA_Dummy+925	/* (ISG) UWORD - SelectedFrame */
#define IDTA_HalfShadowPen		DTA_Dummy+926	/* (ISG) UWORD - needed for some frames */
#define IDTA_HalfShinePen		DTA_Dummy+927	/* (ISG) UWORD - needed for some frames */
#define IDTA_RenderHook			DTA_Dummy+928	/* (IS.) struct Hook * - needed for animated AppIcons +jl+ */
#define IDTA_DoFreeDiskObject		DTA_Dummy+929	/* (I..) ULONG - Flag: free DiskObject delivered by AIDTA_Icon +jl+ */
#define IDTA_TextStyle			DTA_Dummy+930	/* (ISG) ULONG - +jl+ 20011231 Text style for icon text */
#define	IDTA_Font			DTA_Dummy+931	/* (ISG) struct TextFont * - +jl+ 20010818 font to render icon text in */
#define	IDTA_SupportedIconTypes		DTA_Dummy+932	/* (I..) ULONG - bits enable specific icon types */
#define	IDTA_TextPen			DTA_Dummy+933	/* (ISG) UWORD - icon text pen */
#define	IDTA_TextPenSel			DTA_Dummy+934	/* (ISG) UWORD - selected icon text pen */
#define	IDTA_TextPenShadow		DTA_Dummy+935	/* (ISG) UWORD - icon text shadow pen */
#define	IDTA_MaskBM_Normal		DTA_Dummy+936	/* (..G) struct BitMap * - +jl+ 20020201 */
#define	IDTA_MaskBM_Selected		DTA_Dummy+937	/* (..G) struct BitMap * - +jl+ 20020201 */
#define	IDTA_UserFlags			DTA_Dummy+938	/* (ISG) ULONG - +jl+ 20020928 */
#define	IDTA_Borderless			DTA_Dummy+939	/* (ISG) ULONG - always draw icon without border */
#define	IDTA_AlphaChannel		DTA_Dummy+940	/* (..G) UBYTE * - byte array of alpha channel information
									or NULL if no alpha channel supported */
#define	IDTA_IconLocation		DTA_Dummy+941	/* (I..) const struct WBArg * - icon location of object for this icon */
#define	IDTA_Fonthandle			DTA_Dummy+942	/* (ISG) APTR - opaque font handle to use with font function vector */
#define	IDTA_FontHook			DTA_Dummy+943	/* (ISG) struct Hook * - font function Hook */
#define	IDTA_MultiLineText		DTA_Dummy+944	/* (ISG) ULONG - Flag: allow splitting of icon text into multiple lines */
#define IDTA_ARGBImageData		DTA_Dummy+945	/* (..G) struct ARGBHeader * image data in ARGB format. Not available for all icon types */
#define IDTA_SizeConstraints		DTA_Dummy+946	/* (I..) struct Rectangle * - set min/max sizes for icon */
#define IDTA_Width_Mask_Normal		DTA_Dummy+947	/* (ISG) ULONG - Witdh of non-selected mask bitplane */
#define IDTA_Height_Mask_Normal		DTA_Dummy+948	/* (ISG) UBYTE * - Height of non-selected mask bitplane */
#define IDTA_Width_Mask_Selected	DTA_Dummy+949	/* (ISG) UBYTE * - Width of selected mask bitplane */
#define IDTA_Height_Mask_Selected	DTA_Dummy+950	/* (ISG) UBYTE * - Height of non-selected mask bitplane */
#define IDTA_CopyARGBImageData		DTA_Dummy+951	/* (ISG) ULONG - If TRUE, copy IDTA_ARGBImageData */
#define IDTA_IconType			DTA_Dummy+952	/* (..G) ULONG - type of icon */
#define IDTA_NumberOfColorsSupported	DTA_Dummy+953	/* (..G) ULONG - number of colors supported by icon type (e.g. 256 by GlowIcons + NewIcons) */
#define IDTA_OverlayType		DTA_Dummy+954	/* (ISG) ULONG - type of overlay to draw over icon - not used, but only stored by datatype */
#define IDTA_Backfill			DTA_Dummy+955	/* (ISG) ULONG - Pen number to use for unselected background fill */
#define IDTA_BackfillSel		DTA_Dummy+956	/* (ISG) ULONG - Pen number to use for selected background fill */
#define IDTA_TextDrawMode		DTA_Dummy+957	/* (ISG) UBYTE - draw mode for icon Text (JAM1 or JAM2) */
#define IDTA_TextBackPen		DTA_Dummy+958	/* (ISG) UBYTE - icon text background color for IDTA_TextDrawMode = IDTA_TextDrawMode */
#define IDTA_CopySelARGBImageData	DTA_Dummy+959	/* (ISG) ULONG - If TRUE, copy IDTA_SelARGBImageData */
#define IDTA_SelARGBImageData		DTA_Dummy+960	/* (..G) struct ARGBHeader * selected image data in ARGB format. Not available for all icon types */
#define	IDTA_SelAlphaChannel		DTA_Dummy+961	/* (..G) UBYTE * - byte array of alpha channel information
							  	         or NULL if no alpha channel supported */
#define IDTA_ToolWindow			DTA_Dummy+962	/* (.SG) STRPTR - Tool Window */
#define	IDTA_CloneIconObject		DTA_Dummy+963	/* (I..) */
#define IDTA_TextPenBgSel		DTA_Dummy+964	/* (ISG) UWORD - pen for rectangle around selected icon text */
#define	IDTA_SelectedTextRectangle	DTA_Dummy+965	/* (I.G) ULONG - Flag: draw rectangle around selected icon text */
#define IDTA_SelTextRectBorderX         DTA_Dummy+966	/* (ISG) UWORD - Additional horizontal border around selected icon text rectangle */
#define IDTA_SelTextRectBorderY         DTA_Dummy+967	/* (ISG) UWORD - Additional vertical border around selected icon text rectangle */
#define	IDTA_SelTextRectRadius          DTA_Dummy+968	/* (ISG) UWORD - Radius for selected icon text rectangle corners */

#define	IDTA_UnscaledWidth              DTA_Dummy+969	/* (ISG) ULONG - unscaled width of icon */
#define	IDTA_UnscaledHeight             DTA_Dummy+970	/* (ISG) ULONG - unscaled width of icon */

#define IDTA_ScalePercentage		DTA_Dummy+971	/* (I..) UWORD - Scale factor in percent */

//amigaicon.datatype
#define AIDTA_Icon			DTA_Dummy+1101	/* (I.G) orginal diskobject */

// allowed range for IDTA_ScalePercentage
#define IDTA_ScalePercentage_MIN	25
#define IDTA_ScalePercentage_MAX	400

/* IDTA_SupportedIconTypes values (bit mask) */
#define IDTV_IconType_NewIcon		0x00000001
#define IDTV_IconType_ColorIcon		0x00000002


/* IDTA_TextMode values */
#define IDTV_TextMode_Normal		0
#define IDTV_TextMode_Outline		1
#define IDTV_TextMode_Shadow		2


/* IDTA_Viewmodes values - also used in ws_Viewmodes */
#define IDTV_ViewModes_Default		0
#define IDTV_ViewModes_Icon		1
#define IDTV_ViewModes_Name		2
#define IDTV_ViewModes_Size		3
#define IDTV_ViewModes_Date		4
#define IDTV_ViewModes_Time		5
#define IDTV_ViewModes_Comment		6
#define IDTV_ViewModes_Protection	7
#define IDTV_ViewModes_Owner		8
#define IDTV_ViewModes_Group		9
#define IDTV_ViewModes_Type		10
#define IDTV_ViewModes_Version		11
#define IDTV_ViewModes_MiniIcon		12


/* IDTA_UserFlags flag bits */
#define ICONOBJ_USERFLAGB_DrawShadowed	0		// this icon should be rendered in shadowed state
#define ICONOBJ_USERFLAGB_DefaultIcon	1		// this is a default icon and may be rendered specially
#define ICONOBJ_USERFLAGB_DrawHighlite	2		// Draw icon in highlighted state
#define ICONOBJ_USERFLAGB_Thumbnail	3		// This is an image thumbnail

#define ICONOBJ_USERFLAGF_DrawShadowed	(1L << ICONOBJ_USERFLAGB_DrawShadowed)
#define ICONOBJ_USERFLAGF_DefaultIcon	(1L << ICONOBJ_USERFLAGB_DefaultIcon)
#define ICONOBJ_USERFLAGF_DrawHighlite	(1L << ICONOBJ_USERFLAGB_DrawHighlite)
#define ICONOBJ_USERFLAGF_Thumbnail	(1L << ICONOBJ_USERFLAGB_Thumbnail)

/*----------------------------------------------------------------------------------*/

/* Icon types - parameter for IDTA_IconType */
enum ioIconTypes
	{
	ioICONTYPE_Standard,		// standard icon
	ioICONTYPE_NewIcon,
	ioICONTYPE_GlowIcon,		// OS3.5+ GlowIcon
	ioICONTYPE_PngIcon,
	};

/*----------------------------------------------------------------------------------*/

/* parameter for IDTA_FontFunctions */

enum ioFontHookMsgTypes
	{
	iofMsg_DONERASTPORT,
	iofMsg_SETFONT,
	iofMsg_TEXT,
	iofMsg_SETFONTSTYLE,
	iofMsg_TEXTEXTENT,
	iofMsg_TEXTFIT,
	iofMsg_GETFONTHEIGHT,
	iofMsg_GETFONTBASELINE,
	iofMsg_SETTRANSPARENCY,
	};

#if defined(__MORPHOS__)
#pragma pack(2)
#endif

struct ioFontMsg_DoneRastPort
	{
	ULONG MsgID;		// iofMsg_DONERASTPORT
	struct RastPort *iofdr_RastPort;
	void *iofdr_FontHandle;
	};

struct ioFontMsg_SetFont
	{
	ULONG MsgID;		// iofMsg_SETFONT
	struct RastPort *iofsf_RastPort;
	void *iofsf_FontHandle;
	struct TextFont *iofsf_TextFont;
	};

struct ioFontMsg_Text
	{
	ULONG MsgID;		// iofMsg_TEXT
	struct RastPort *ioft_RastPort;
	void *ioft_FontHandle;
	CONST_STRPTR ioft_String;
	WORD ioft_Length;
	};

struct ioFontMsg_SetFontStyle
	{
	ULONG MsgID;		// iofMsg_SETFONTSTYLE
	struct RastPort *iofsfs_RastPort;
	void *iofsfs_FontHandle;
	ULONG iofsfs_Style;
	ULONG iofsfs_Enable;
	};

struct ioFontMsg_TextExtent
	{
	ULONG MsgID;		// iofMsg_TEXTEXTENT
	struct RastPort *iofte_RastPort;
	void *iofte_FontHandle;
	CONST_STRPTR iofte_String;
	WORD iofte_Length;
	struct TextExtent *iofte_TextExtent;
	};

struct ioFontMsg_TextFit
	{
	ULONG MsgID;		// iofMsg_TEXTFIT
	struct RastPort *ioftf_RastPort;
	void *ioftf_FontHandle;
	CONST_STRPTR ioftf_String;
	WORD ioftf_Length;
	struct TextExtent *ioftf_TextExtent;
	struct TextExtent *ioftf_ConstrainingTextExtent;
	WORD ioftf_StrDirection;
	UWORD ioftf_ConstrainingBitWidth;
	UWORD ioftf_ConstrainingBitHeight;
	};

struct ioFontMsg_GetFontHeight
	{
	ULONG MsgID;		// iofMsg_GETFONTHEIGHT
	struct RastPort *iofgfh_RastPort;
	void *iofgfh_FontHandle;
	};

struct ioFontMsg_GetFontBaseline
	{
	ULONG MsgID;		// iofMsg_GETFONTBASELINE
	struct RastPort *iofgfb_RastPort;
	void *iofgfb_FontHandle;
	};

struct ioFontMsg_SetTransparency
	{
	ULONG MsgID;		// iofMsg_SETTRANSPARENCY
	struct RastPort *iofst_RastPort;
	void *iofst_FontHandle;
	ULONG iofst_Transparency;	// 0=no transparency (opaque), 255=maximum transparency
	};

#if defined(__MORPHOS__)
#pragma pack()
#endif

/*----------------------------------------------------------------------------------*/

/*
 * Icon object methods
 */

#define IDTM_Layout			(DTA_Dummy+1001)
#define IDTM_Draw			(DTA_Dummy+1002)
#define IDTM_Write			(DTA_Dummy+1003)	/* PutDiskObject */
#define IDTM_FindToolType		(DTA_Dummy+1004)
#define IDTM_GetToolTypeValue		(DTA_Dummy+1005)
#define IDTM_FreeLayout			(DTA_Dummy+1006)
#define IDTM_Erase			(DTA_Dummy+1007)	// CURRENTLY UNUSED
#define IDTM_ScaleIcon			(DTA_Dummy+1008)
#define IDTM_NewImage			(DTA_Dummy+1009)
#define IDTM_CloneIconObject		(DTA_Dummy+1010)

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_Layout method
 */

struct iopLayout
{
    ULONG	MethodID;
    struct	Screen *iopl_Screen;
    struct	Window *iopl_Window;
    struct	RastPort *iopl_RastPort;
    struct	DrawInfo *iopl_DrawInfo;
    ULONG	iopl_Flags;
};

/* Flags */
#define IOLAYOUTB_NormalImage		0
#define IOLAYOUTB_SelectedImage		1

#define IOLAYOUTF_NormalImage		(1L<<IOLAYOUTB_NormalImage)
#define IOLAYOUTF_SelectedImage		(1L<<IOLAYOUTB_SelectedImage)

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_Draw method
 */

struct iopDraw
{
    ULONG	MethodID;
    struct	Screen *iopd_Screen;
    struct	Window *iopd_Window;
    struct	RastPort *iopd_RastPort;
    struct	DrawInfo *iopd_DrawInfo;
    ULONG	iopd_XOffset;
    ULONG	iopd_YOffset;
    ULONG	iopd_DrawFlags;
};

/* DrawFlags */
#define IODRAWB_NoText		0
#define IODRAWB_AbsolutePos	1	/* iopd_XOffset / iopd_XOffset is absolute position */
#define IODRAWB_NoImage		2
#define IODRAWB_NoAlpha		3		// ignore any Alpha channel
#define IODRAWB_NoEraseBg	4		// Do not erase background
#define IODRAWB_HighlightChg	5		// only change in icon highlight state

#define IODRAWF_NoText		(1L << IODRAWB_NoText)
#define IODRAWF_AbsolutePos	(1L << IODRAWB_AbsolutePos)
#define IODRAWF_NoImage		(1L << IODRAWB_NoImage)
#define IODRAWF_NoAlpha		(1L << IODRAWB_NoAlpha)
#define IODRAWF_NoEraseBg	(1L << IODRAWB_NoEraseBg)
#define IODRAWF_HighlightChg	(1L << IODRAWB_HighlightChg)

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_Erase method
 */

struct iopErase
{
    ULONG	MethodID;
    struct	Screen *iope_Screen;
    struct	Window *iope_Window;
    struct	RastPort *iope_RastPort;
    ULONG	iope_XOffset;
    ULONG	iope_YOffset;
    ULONG	iope_EraseFlags;
};

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_FindToolType method
 */

struct iopFindToolType
{
    ULONG		MethodID;
    CONST_STRPTR	ioftt_ToolTypeName;	// Name of tooltype to search
    STRPTR		*ioftt_ToolTypeValue;   // on Call: point to NULL to search from first tooltype,
						// or result of last IDTM_FindToolType.
};

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_GetToolTypeValue method
 */

struct iopGetToolTypeValue
{
    ULONG		MethodID;
    STRPTR		iotv_ToolTypeValue;    // result from IDTM_FindToolType
    LONG		*iotv_Value;		// Pointer to LONG for storing the result
};

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_Write method
 */

struct iopWrite
{
    ULONG		MethodID;
    STRPTR		iopw_Path;
    APTR		iopw_SourceObject;	/* Object to get attrs from */
    struct TagItem 	*iopw_Tags;		/* Additional Tags */
};

/* Tags for IDTM_Write */
#define ICONA_NoNewImage	0x80000001	// ULONG
#define ICONA_NoPosition	0x80000002	// ULONG
#define ICONA_NoUsePosition	0x80000003	// ULONG
#define ICONA_NotifyWorkbench	0x80000004	// ULONG - Flag: notify Scalos that the saved icon needs to be updated. Default is FALSE.

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_FreeLayout method
 */

struct iopFreeLayout
{
    ULONG	MethodID;		/* IDTM_FreeLayout */
    ULONG	iopf_Flags;		/* Additional flags */
};

/* Flags */
#define IOFREELAYOUTB_ScreenAvailable	0
#define IOFREELAYOUTF_ScreenAvailable	(1<<IOFREELAYOUTB_ScreenAvailable)

/*----------------------------------------------------------------------------------*/

/*
 * ITDM_ScaleIcon method
 */

struct iopScaleIcon
{
	ULONG	    MethodID;		/* ITDM_ScaleIcon */
	ULONG	    iops_NewWidth;	// Scaled icon width (Gadget->Width, BoundsWidth will be adjusted accordingly)
	ULONG	    iops_NewHeight;	// Scaled icon height (Gadget->Height, BoundsHeight will be adjusted accordingly)
	ULONG	    iops_Flags;		/* flags */
};

/* Flags */
// same as for IDTM_Layout
//IOLAYOUTF_NormalImage
//IOLAYOUTF_SelectedImage

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_NewImage method
 */

struct iopNewImage
{
	ULONG	    MethodID;			/* IDTM_NewImage */
	struct ScalosBitMapAndColor *ioni_NormalImage;
	struct ScalosBitMapAndColor *ioni_SelectedImage;	/* may be NULL for IDTM_NewSelectedImage */
};

/*----------------------------------------------------------------------------------*/

/*
 * IDTM_CloneIconObject method
 */

struct iopCloneIconObject
{
    ULONG		MethodID;
    struct TagItem 	*iocio_TagList;
};

/*----------------------------------------------------------------------------------*/

// values for IDTA_OverlayType

#define ICONOVERLAY_None		0

#define ICONOVERLAYB_LeftOut		0
#define ICONOVERLAYB_ReadOnly		1
#define ICONOVERLAYB_Thumbnail		2

#define	ICONOVERLAY_MAX			3

#define ICONOVERLAYF_LeftOut             (1 << ICONOVERLAYB_LeftOut)
#define ICONOVERLAYF_ReadOnly            (1 << ICONOVERLAYB_ReadOnly)
#define ICONOVERLAYF_Thumbnail           (1 << ICONOVERLAYB_Thumbnail)

/*----------------------------------------------------------------------------------*/

// special value for IDTA_Backfill and IDTA_BackfillSel

#define IDTA_BACKFILL_NONE		((ULONG) ~0)

/*----------------------------------------------------------------------------------*/

/*
 * texticon.datatype (scalos internal)
 */

//#define TIDTA_ExAllData		DTA_Dummy+1151		/* (IS.) deprecated */
//#define TIDTA_ExAllType		DTA_Dummy+1152		/* (IS.) deprecated */
#define TIDTA_WidthArray		DTA_Dummy+1153		/* (ISG) */
//#define TIDTA_ExNextData		DTA_Dummy+1154		/* (IS.) deprecated */
#define TIDTA_FileType			DTA_Dummy+1155		/* (..G) */
#define TIDTA_Size			DTA_Dummy+1156		/* (..G) */
#define TIDTA_Days			DTA_Dummy+1157		/* (..G) */
#define TIDTA_Mins			DTA_Dummy+1158		/* (..G) */
#define TIDTA_Ticks			DTA_Dummy+1159		/* (..G) */
#define TIDTA_Comment			DTA_Dummy+1160		/* (..G) */
#define TIDTA_Owner_UID			DTA_Dummy+1161		/* (..G) UWORD */
#define TIDTA_Owner_GID			DTA_Dummy+1162		/* (..G) UWORD */
#define TIDTA_Protection		DTA_Dummy+1163		/* (..G) */
#define TIDTA_TextStyle			DTA_Dummy+1164		/* (ISG) ULONG text style for text icons, see SetSoftStyle() +jl+ 20011231 */
#define TIDTA_Font			DTA_Dummy+1165		/* (ISG) (struct TextFont *) - font to render icon text in +jl+ 20010903 */
#define TIDTA_IconType			DTA_Dummy+1167		/* (ISG) ULONG - Workbench icon type +jl+ 20010906 */
#define TIDTA_TTFont			DTA_Dummy+1168		/* (ISG) APTR - ttEngine font to render icon text in +jl+ 20030110 */
#define TIDTA_WindowTask		DTA_Dummy+1169		/* (I..) struct ScaWindowTask * - parent WindowTask */
#define TIDTA_IconObject		DTA_Dummy+1170		/* (ISG) Object * */
#define TIDTA_SoloIcon			DTA_Dummy+1171		/* (ISG) ULONG (Boolean) TRUE if this an icon w/o object */
#define TIDTA_ColumnWidthChangeHook	DTA_Dummy+1172		/* (ISG) struct Hook * - Hook is called when icon layout causes column width to change */
#define TIDTA_SelectNameOnly		DTA_Dummy+1173		/* (ISG) ULONG - if TRUE, icon can only be selected at name column */
#define TIDTA_TypeNode			DTA_Dummy+1174		/* (ISG) struct TypeNode * - TypeNode Workbench icon type */
#define TIDTA_TextPenDrawerNormal	DTA_Dummy+1175		/* (ISG) UWORD - text window normal  icon text pen for drawers */
#define TIDTA_TextPenDrawerSelected	DTA_Dummy+1176		/* (ISG) UWORD - text window selected icon text pen  for drawers */
#define TIDTA_TextPenFileNormal		DTA_Dummy+1177		/* (ISG) UWORD - text window normal  icon text pen for files */
#define TIDTA_TextPenFileSelected	DTA_Dummy+1178		/* (ISG) UWORD - text window selected icon text pen for files */
#define TIDTA_ReadIconListData		DTA_Dummy+1180		/* (IS.) struct ReadIconListData * */

/*----------------------------------------------------------------------------------*/

#endif	/* DATATYPES_ICONOBJECT_H */

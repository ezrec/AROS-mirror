#ifndef DATATYPES_ICONOBJECT_H
#define DATATYPES_ICONOBJECT_H
/*
**	$VER: iconobject.h 39.7 (13.2.97)
**
**      iconobject.library include
**
**	(C) Copyright 1996-1997 ALiENDESiGN
**	All Rights Reserved
*/

//#define DTA_Dummy					0x80001000		// TAGUSER+0x1000

// iconobject.datatype

// Attributes
#define IDTA_Type					DTA_Dummy+901		// (.SG) Icon Type
#define IDTA_Text					DTA_Dummy+902		// (ISG) Icon Text
#define IDTA_DefaultTool		DTA_Dummy+903		// (.SG) DefaultTool
#define IDTA_WindowRect			DTA_Dummy+904		// (.SG) WindowSize
#define IDTA_ToolTypes			DTA_Dummy+905		// (.SG) Tooltype Array
#define IDTA_Stacksize			DTA_Dummy+906		// (.SG) Stacksize
#define IDTA_InnerLeft			DTA_Dummy+907		// (ISG)
#define IDTA_InnerTop			DTA_Dummy+908		// (ISG)
#define IDTA_InnerRight			DTA_Dummy+909		// (ISG)
#define IDTA_InnerBottom		DTA_Dummy+910		// (ISG)
#define IDTA_FrameType			DTA_Dummy+911		// (ISG)
#define IDTA_WinCurrentX		DTA_Dummy+912		// (.SG)
#define IDTA_WinCurrentY		DTA_Dummy+913		// (.SG)
#define IDTA_LayoutFlags		DTA_Dummy+914		// (..G)
#define IDTA_Extention			DTA_Dummy+915		// (..G) e.g. ".info"
#define IDTA_Mask_Normal		DTA_Dummy+916		// (.SG) BlitMask
#define IDTA_Mask_Selected		DTA_Dummy+917		// (.SG) BlitMask
#define IDTA_Flags				DTA_Dummy+918		// (.SG) DrawerData Flags
#define IDTA_ViewModes			DTA_Dummy+919		// (.SG) DrawerData ViewModes
#define IDTA_DefType				DTA_Dummy+920		// (I..) DefIcon Type
#define IDTA_MaxDepth			DTA_Dummy+921		// (.S.) of the bitmap
#define IDTA_TextPen				DTA_Dummy+922		// (IS.) textpen for outline text
#define IDTA_TextMode			DTA_Dummy+923		// (IS.) textmode
#define IDTA_TextSkip			DTA_Dummy+924		// (IS.) space between text and image
#define IDTA_FrameTypeSel		DTA_Dummy+925		// (ISG) SelectedFrame
#define IDTA_HalfShadowPen		DTA_Dummy+926		// (IS.) needed for some frames
#define IDTA_HalfShinePen		DTA_Dummy+927		// (IS.) needed for some frames

#define IDTV_TextMode_Normal		0
#define IDTV_TextMode_Outline		1
#define IDTV_TextMode_Shadow		2

#define IDTV_ViewModes_Icon		0
#define IDTV_ViewModes_Name		2
#define IDTV_ViewModes_Size		3
#define IDTV_ViewModes_Date		4

// Methods
#define IDTM_Layout				DTA_Dummy+1001
#define IDTM_Draw					DTA_Dummy+1002
#define IDTM_Write				DTA_Dummy+1003		// PutDiskObject
#define IDTM_FindToolType		DTA_Dummy+1004
#define IDTM_GetToolTypeValue	DTA_Dummy+1005
#define IDTM_FreeLayout			DTA_Dummy+1006

// amigaiconobject.datatype
#define AIDTA_Icon				DTA_Dummy+1101		// (I.G) original Icon

// texticon.datatype (scalos intern)
#define TIDTA_FileType			DTA_Dummy+1151		// (ISG)
#define TIDTA_FileSize			DTA_Dummy+1152		// (ISG)
#define TIDTA_Protection		DTA_Dummy+1153		// (ISG)
#define TIDTA_Days				DTA_Dummy+1154		// (ISG)
#define TIDTA_Mins				DTA_Dummy+1155		// (ISG)
#define TIDTA_Ticks				DTA_Dummy+1156		// (ISG)
#define TIDTA_Comment			DTA_Dummy+1157		// (ISG)
#define TIDTA_Owner_UID			DTA_Dummy+1158		// (ISG) UWORD
#define TIDTA_Owner_GID			DTA_Dummy+1159		// (ISG) UWORD
#define TIDTA_Name_Width		DTA_Dummy+1160		// (ISG) Pixel width
#define TIDTA_FileSize_Width	DTA_Dummy+1161		// (ISG) Pixel width
#define TIDTA_Protection_Width DTA_Dummy+1162	// (ISG)
#define TIDTA_Date_Width		DTA_Dummy+1163		// (ISG)
#define TIDTA_Time_Width		DTA_Dummy+1164		// (ISG)
#define TIDTA_Comment_Width	DTA_Dummy+1165		// (ISG)
#define TIDTA_Owner_Width		DTA_Dummy+1166		// (ISG)

// PutDiskObject Tags
#define ICONA_NoNewImage		0x80000001
#define ICONA_NoPosition		0x80000002
#define ICONA_NoUsePosition	0x80000003

#endif	/* DATATYPES_ICONOBJECT_H */

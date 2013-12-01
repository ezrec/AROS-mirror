#ifndef LIBRARIES_MCPGFX_H
#define LIBRARIES_MCPGFX_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif


/* Types of frames available */
#define MF_FRAME_NONE		0	/* No frame */
#define MF_FRAME_BUTTON		1	/* Standard 3D frame used for buttons */
#define MF_FRAME_BORDER		2	/* Standard 2D frame used for */
#define MF_FRAME_STRING		3	/* String */
#define MF_FRAME_DROPBOX	4	/* Dropbox [String (with space)?] */
#define MF_FRAME_XEN		5	/* Standard XEN button */
#define MF_FRAME_MWB		6	/* Standard MWB */
#define MF_FRAME_THICK		7	/* Standard Thick */
#define MF_FRAME_XWIN		8	/* Standard XWIN */
#define MF_FRAME_MAXIMUM	9	/* Maximum number of frame types */


/* Sizes of frame egdes. Always in the order x1,y1,x2,y2,(recessed)x1,y1,x2,y2. */
struct FrameSize
{
	WORD	fs_Sizes[8];
};


/* Some constants for acessing the values from the FrameSize structure */
#define FRAMESIZE_LEFT		0	/* Normal left edge width */
#define FRAMESIZE_TOP		1	/* Normal top edge width */
#define FRAMESIZE_RIGHT		2	/* Normal right edge width */
#define FRAMESIZE_BOTTOM	3	/* Normal bottom edge width */
#define FRAMESIZE_RELEFT	4	/* Recessed left edge width */
#define FRAMESIZE_RETOP		5	/* Recessed top edge width */
#define FRAMESIZE_RERIGHT	6	/* Recessed right edge width */
#define FRAMESIZE_REBOTTOM	7	/* Recessed bottom edge width */


/* Tags for the various function in mcpgfx.library */
#define MCP_TagBase			TAG_USER + 0x20000

#if 0

#define IA_Left				MCP_TagBase + 0x01
#define IA_Top				MCP_TagBase + 0x02
#define IA_Width			MCP_TagBase + 0x03
#define IA_Height			MCP_TagBase + 0x04
#define IA_FGPen			MCP_TagBase + 0x05
#define IA_BGPen			MCP_TagBase + 0x06
#define IA_Data				MCP_TagBase + 0x07
#define IA_ShadowPen		MCP_TagBase + 0x09

#endif

#define IA_ShinePen			MCP_TagBase + 0x0a

#if 0

#define IA_APattern			MCP_TagBase + 0x10
#define IA_APatSize			MCP_TagBase + 0x11
#define IA_Mode				MCP_TagBase + 0x12
#define IA_Recessed			MCP_TagBase + 0x15
#define IA_DrawInfo			MCP_TagBase + 0x18
#define IA_FrameType		MCP_TagBase + 0x1b

#endif


#define IA_HalfShadowPen	MCP_TagBase + 0x1c
#define IA_HalfShinePen		MCP_TagBase + 0x1d


/* A little something */
struct ExtDrawInfo
{
	struct DrawInfo	edi_DrawInfo;	/* Original screen DrawInfo (copy of) */
	WORD			*edi_Pens;		/* Pointer to an array of pens to use for MWB colours */
	ULONG			edi_dunno[2];	/* like I say */
};


#endif /* LIBRARIES_MCPGFX_H */


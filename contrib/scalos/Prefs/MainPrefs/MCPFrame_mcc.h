// MCPFrame_mcc.h
// $Date$


#ifndef MUI_MCPFRAME_H
#define MUI_MCPFRAME_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#define MUIC_MCPFrame "MCPFrame.mcc"

#ifdef __AROS__
#define McpFrameObject BOOPSIOBJMACRO_START(McpFrameClass->mcc_Class)
#else
#define McpFrameObject NewObject(McpFrameClass->mcc_Class, 0
#endif


#define MUIA_MCPFrame_FrameType		0x804d0009	/* isg LONG              */
#define MUIA_MCPFrame_Unknown		0x804d0008	/* isg LONG              */


/* Types of frames available */
#define MCP_FRAME_NONE		0	/* No frame */
#define MCP_FRAME_BUTTON	1	/* Standard 3D frame used for buttons */
#define MCP_FRAME_BORDER	2	/* Standard 2D frame used for */
#define MCP_FRAME_STRING	3	/* String */
#define MCP_FRAME_DROPBOX	4	/* Dropbox [String (with space)?] */
#define MCP_FRAME_XEN		5	/* Standard XEN button */
#define MCP_FRAME_MWB		6	/* Standard MWB */
#define MCP_FRAME_THICK		7	/* Standard Thick */
#define MCP_FRAME_XWIN		8	/* Standard XWIN */
#define MCP_FRAME_MAXIMUM	9	/* Maximum number of frame types */

#define	MCP_FRAME_RECESSED	0x8000

#endif /* MUI_MCPFRAME_H */

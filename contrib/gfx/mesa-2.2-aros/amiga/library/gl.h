/* $Id$ */

/*
 * Mesa 3-D graphics library
 * Version:  1.2
 * Copyright (C) 1995-1996  Brian Paul  (brianp@ssec.wisc.edu)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
$Log$
Revision 1.2  2004/02/25 02:51:02  NicJA
updates updates updates... now builds with the standard aros mmakefile.src system - use make contrib-mesa2 to build

N.B - theres something wrong somewhere causing some demos,samples and book files to not compile (if you can see the problem please fix it ;))

Revision 1.1.1.1  2003/08/09 00:23:30  chodorowski
Amiga Mesa 2.2 ported to AROS by Nic Andrews. Build with 'make aros'. Not built by default.

 * Revision 1.16	1996/02/15	16:00:00  brianp
 * added vertex array extension symbols and functions
 *
 * Revision 1.15	1995/11/03	14:29:02  brianp
 * added patches for CenterLine C++ per Steven Spitz
 * cleaned up some comments
 *
 * Revision 1.14	1995/10/27	20:26:53  brianp
 * added glPolygonOffsetEXT() and related GLenums
 *
 * Revision 1.13	1995/10/19	13:47:23  brianp
 * GL_LINEAR_MIPMAP_NEAREST had wrong value
 *
 * Revision 1.12	1995/10/02	13:13:56  brianp
 * removed an extra glColor4B prototype
 *
 * Revision 1.11	1995/09/28	13:17:48  brianp
 * renamed GL_ALL_ATTRIB_BIT to GL_ALL_ATTRIB_BITS
 *
 * Revision 1.10	1995/08/13	17:26:02  brianp
 * assign explicit values to all GLenum symbols to match OpenGL
 *
 * Revision 1.9  1995/05/30  13:17:38	brianp
 * added glGetTexImage(), glGetMap[dfi]v() protos, GL_AUX[123], GL_COEF,
 * GL_DOMAIN, GL_ORDER symbols
 *
 * Revision 1.8  1995/05/29  21:19:54	brianp
 * added GL_TEXTURE_WIDTH, HEIGHT, BORDER, COMPONENTS symbols
 * added glGetMaterial*, glGetPixelMap*, glGetTexGen*, glGetTexParameter* protos
 *
 * Revision 1.7  1995/05/22  17:03:21	brianp
 * Release 1.2
 *
 * Revision 1.6  1995/04/28  20:04:03	brianp
 * added GL_REPLACE_EXT texture symbol
 * fixed glGetString and glGetLighiv prototypes
 *
 * Revision 1.5  1995/04/19  13:47:05	brianp
 * renamed occurances of near and far for SCO x86 Unix
 *
 * Revision 1.4  1995/04/05  18:29:04	brianp
 * added GL_EXT_blend_* compile-time symbols
 *
 * Revision 1.3  1995/03/10  16:28:41	brianp
 * updated for blending extensions
 *
 * Revision 1.2  1995/03/04  19:45:47	brianp
 * 1.1 beta revision
 *
 * Revision 1.1  1995/02/28  21:21:03	brianp
 * Initial revision
 *
 */

#ifdef AMIWIN

#include "gl_org.h"

#else


#ifndef GL_H
#define GL_H


#ifdef __cplusplus
extern "C" {
#endif




/*
 * Apps can test for this symbol to do conditional compilation if needed.
 */
#define MESA



/*
 *
 * Enumerations
 *
 */

typedef enum {
	/* Boolean values */
	GL_FALSE 		= 0,
	GL_TRUE				= 1,

	/* Data types */
	GL_BYTE				= 0x1400,
	GL_UNSIGNED_BYTE		= 0x1401,
	GL_SHORT 		= 0x1402,
	GL_UNSIGNED_SHORT 	= 0x1403,
	GL_INT				= 0x1404,
	GL_UNSIGNED_INT			= 0x1405,
	GL_FLOAT 		= 0x1406,
	GL_2_BYTES			= 0x1407,
	GL_3_BYTES			= 0x1408,
	GL_4_BYTES			= 0x1409,
	GL_DOUBLE_EXT			= 0x140A,

	/* Primitives */
	GL_LINES 		= 0x0001,
	GL_POINTS			= 0x0000,
	GL_LINE_STRIP			= 0x0003,
	GL_LINE_LOOP			= 0x0002,
	GL_TRIANGLES			= 0x0004,
	GL_TRIANGLE_STRIP 	= 0x0005,
	GL_TRIANGLE_FAN			= 0x0006,
	GL_QUADS 		= 0x0007,
	GL_QUAD_STRIP			= 0x0008,
	GL_POLYGON			= 0x0009,
	GL_EDGE_FLAG			= 0x0B43,

	/* Matrix Mode */
	GL_MATRIX_MODE 		= 0x0BA0,
	GL_MODELVIEW			= 0x1700,
	GL_PROJECTION			= 0x1701,
	GL_TEXTURE			= 0x1702,

	/* Points */
	GL_POINT_SMOOTH			= 0x0B10,
	GL_POINT_SIZE			= 0x0B11,
	GL_POINT_SIZE_GRANULARITY	= 0x0B13,
	GL_POINT_SIZE_RANGE		= 0x0B12,

	/* Lines */
	GL_LINE_SMOOTH 		= 0x0B20,
	GL_LINE_STIPPLE			= 0x0B24,
	GL_LINE_STIPPLE_PATTERN 	= 0x0B25,
	GL_LINE_STIPPLE_REPEAT		= 0x0B26,
	GL_LINE_WIDTH			= 0x0B21,
	GL_LINE_WIDTH_GRANULARITY	= 0x0B23,
	GL_LINE_WIDTH_RANGE		= 0x0B22,

	/* Polygons */
	GL_POINT 		= 0x1B00,
	GL_LINE				= 0x1B01,
	GL_FILL				= 0x1B02,
	GL_CCW				= 0x0901,
	GL_CW 			= 0x0900,
	GL_FRONT 		= 0x0404,
	GL_BACK				= 0x0405,
	GL_CULL_FACE			= 0x0B44,
	GL_CULL_FACE_MODE 	= 0x0B45,
	GL_POLYGON_SMOOTH 	= 0x0B41,
	GL_POLYGON_STIPPLE		= 0x0B42,
	GL_FRONT_FACE			= 0x0B46,
	GL_POLYGON_MODE			= 0x0B40,

	/* Display Lists */
	GL_COMPILE			= 0x1300,
	GL_COMPILE_AND_EXECUTE		= 0x1301,
	GL_LIST_BASE			= 0x0B32,
	GL_LIST_INDEX			= 0x0B33,
	GL_LIST_MODE			= 0x0B30,

	/* Depth buffer */
	GL_NEVER 		= 0x0200,
	GL_LESS				= 0x0201,
	GL_GEQUAL			= 0x0206,
	GL_LEQUAL			= 0x0203,
	GL_GREATER			= 0x0204,
	GL_NOTEQUAL 		= 0x0205,
	GL_EQUAL 		= 0x0202,
	GL_ALWAYS			= 0x0207,
	GL_DEPTH_TEST			= 0x0B71,
	GL_DEPTH_BITS			= 0x0D56,
	GL_DEPTH_CLEAR_VALUE 	= 0x0B73,
	GL_DEPTH_FUNC			= 0x0B74,
	GL_DEPTH_RANGE 		= 0x0B70,
	GL_DEPTH_WRITEMASK		= 0x0B72,
	GL_DEPTH_COMPONENT		= 0x1902,

	/* Lighting */
	GL_LIGHTING 		= 0x0B50,
	GL_LIGHT0			= 0x4000,
	GL_LIGHT1			= 0x4001,
	GL_LIGHT2			= 0x4002,
	GL_LIGHT3			= 0x4003,
	GL_LIGHT4			= 0x4004,
	GL_LIGHT5			= 0x4005,
	GL_LIGHT6			= 0x4006,
	GL_LIGHT7			= 0x4007,
	GL_SPOT_EXPONENT		= 0x1205,
	GL_SPOT_CUTOFF 		= 0x1206,
	GL_CONSTANT_ATTENUATION 	= 0x1207,
	GL_LINEAR_ATTENUATION		= 0x1208,
	GL_QUADRATIC_ATTENUATION	= 0x1209,
	GL_AMBIENT			= 0x1200,
	GL_DIFFUSE			= 0x1201,
	GL_SPECULAR 		= 0x1202,
	GL_SHININESS			= 0x1601,
	GL_EMISSION 		= 0x1600,
	GL_POSITION 		= 0x1203,
	GL_SPOT_DIRECTION 	= 0x1204,
	GL_AMBIENT_AND_DIFFUSE		= 0x1602,
	GL_COLOR_INDEXES		= 0x1603,
	GL_LIGHT_MODEL_TWO_SIDE 	= 0x0B52,
	GL_LIGHT_MODEL_LOCAL_VIEWER	= 0x0B51,
	GL_LIGHT_MODEL_AMBIENT		= 0x0B53,
	GL_FRONT_AND_BACK 	= 0x0408,
	GL_SHADE_MODEL 		= 0x0B54,
	GL_FLAT				= 0x1D00,
	GL_SMOOTH			= 0x1D01,
	GL_COLOR_MATERIAL 	= 0x0B57,
	GL_COLOR_MATERIAL_FACE		= 0x0B55,
	GL_COLOR_MATERIAL_PARAMETER	= 0x0B56,
	GL_NORMALIZE			= 0x0BA1,

	/* User clipping planes */
	GL_CLIP_PLANE0 		= 0x3000,
	GL_CLIP_PLANE1 		= 0x3001,
	GL_CLIP_PLANE2 		= 0x3002,
	GL_CLIP_PLANE3 		= 0x3003,
	GL_CLIP_PLANE4 		= 0x3004,
	GL_CLIP_PLANE5 		= 0x3005,

	/* Accumulation buffer */
	GL_ACCUM_RED_BITS 	= 0x0D58,
	GL_ACCUM_GREEN_BITS		= 0x0D59,
	GL_ACCUM_BLUE_BITS		= 0x0D5A,
	GL_ACCUM_ALPHA_BITS		= 0x0D5B,
	GL_ACCUM_CLEAR_VALUE 	= 0x0B80,
	GL_ACCUM 		= 0x0100,
	GL_ADD				= 0x0104,
	GL_LOAD				= 0x0101,
	GL_MULT				= 0x0103,
	GL_RETURN			= 0x0102,

	/* Alpha testing */
	GL_ALPHA_TEST			= 0x0BC0,
	GL_ALPHA_TEST_REF 	= 0x0BC2,
	GL_ALPHA_TEST_FUNC		= 0x0BC1,

	/* Blending */
	GL_BLEND 		= 0x0BE2,
	GL_BLEND_SRC			= 0x0BE1,
	GL_BLEND_DST			= 0x0BE0,
	GL_ZERO				= 0,
	GL_ONE				= 1,
	GL_SRC_COLOR			= 0x0300,
	GL_ONE_MINUS_SRC_COLOR		= 0x0301,
	GL_DST_COLOR			= 0x0306,
	GL_ONE_MINUS_DST_COLOR		= 0x0307,
	GL_SRC_ALPHA			= 0x0302,
	GL_ONE_MINUS_SRC_ALPHA		= 0x0303,
	GL_DST_ALPHA			= 0x0304,
	GL_ONE_MINUS_DST_ALPHA		= 0x0305,
	GL_SRC_ALPHA_SATURATE		= 0x0308,

	/* Render Mode */
	GL_FEEDBACK 		= 0x1C01,
	GL_RENDER			= 0x1C00,
	GL_SELECT			= 0x1C02,

	/* Feedback */
	GL_2D 			= 0x0600,
	GL_3D 			= 0x0601,
	GL_3D_COLOR 		= 0x0602,
	GL_3D_COLOR_TEXTURE		= 0x0603,
	GL_4D_COLOR_TEXTURE		= 0x0604,
	GL_POINT_TOKEN 		= 0x0701,
	GL_LINE_TOKEN			= 0x0702,
	GL_LINE_RESET_TOKEN		= 0x0707,
	GL_POLYGON_TOKEN		= 0x0703,
	GL_BITMAP_TOKEN			= 0x0704,
	GL_DRAW_PIXEL_TOKEN		= 0x0705,
	GL_COPY_PIXEL_TOKEN		= 0x0706,
	GL_PASS_THROUGH_TOKEN		= 0x0700,

	/* Fog */
	GL_FOG				= 0x0B60,
	GL_FOG_MODE 		= 0x0B65,
	GL_FOG_DENSITY 		= 0x0B62,
	GL_FOG_COLOR			= 0x0B66,
	GL_FOG_INDEX			= 0x0B61,
	GL_FOG_START			= 0x0B63,
	GL_FOG_END			= 0x0B64,
	GL_LINEAR			= 0x2601,
	GL_EXP				= 0x0800,
	GL_EXP2				= 0x0801,

	/* Logic Ops */
	GL_LOGIC_OP 		= 0x0BF1,
	GL_LOGIC_OP_MODE		= 0x0BF0,
	GL_CLEAR 		= 0x1500,
	GL_SET				= 0x150F,
	GL_COPY				= 0x1503,
	GL_COPY_INVERTED		= 0x150C,
	GL_NOOP				= 0x1505,
	GL_INVERT			= 0x150A,
	GL_AND				= 0x1501,
	GL_NAND				= 0x150E,
	GL_OR 			= 0x1507,
	GL_NOR				= 0x1508,
	GL_XOR				= 0x1506,
	GL_EQUIV 		= 0x1509,
	GL_AND_REVERSE 		= 0x1502,
	GL_AND_INVERTED			= 0x1504,
	GL_OR_REVERSE			= 0x150B,
	GL_OR_INVERTED 		= 0x150D,

	/* Stencil */
	GL_STENCIL_TEST			= 0x0B90,
	GL_STENCIL_WRITEMASK 	= 0x0B98,
	GL_STENCIL_BITS			= 0x0D57,
	GL_STENCIL_FUNC			= 0x0B92,
	GL_STENCIL_VALUE_MASK		= 0x0B93,
	GL_STENCIL_REF 		= 0x0B97,
	GL_STENCIL_FAIL			= 0x0B94,
	GL_STENCIL_PASS_DEPTH_PASS = 0x0B96,
	GL_STENCIL_PASS_DEPTH_FAIL = 0x0B95,
	GL_STENCIL_CLEAR_VALUE		= 0x0B91,
	GL_STENCIL_INDEX		= 0x1901,
	GL_KEEP				= 0x1E00,
	GL_REPLACE			= 0x1E01,
	GL_INCR				= 0x1E02,
	GL_DECR				= 0x1E03,

	/* Buffers, Pixel Drawing/Reading */
	GL_NONE				= 0,
	GL_LEFT				= 0x0406,
	GL_RIGHT 		= 0x0407,
	/*GL_FRONT			= 0x0404, */
	/*GL_BACK			= 0x0405, */
	/*GL_FRONT_AND_BACK		= 0x0408, */
	GL_FRONT_LEFT			= 0x0400,
	GL_FRONT_RIGHT 		= 0x0401,
	GL_BACK_LEFT			= 0x0402,
	GL_BACK_RIGHT			= 0x0403,
	GL_AUX0				= 0x0409,
	GL_AUX1				= 0x040A,
	GL_AUX2				= 0x040B,
	GL_AUX3				= 0x040C,
	GL_COLOR_INDEX 		= 0x1900,
	GL_RED				= 0x1903,
	GL_GREEN 		= 0x1904,
	GL_BLUE				= 0x1905,
	GL_ALPHA 		= 0x1906,
	GL_LUMINANCE			= 0x1909,
	GL_LUMINANCE_ALPHA		= 0x190A,
	GL_ALPHA_BITS			= 0x0D55,
	GL_RED_BITS 		= 0x0D52,
	GL_GREEN_BITS			= 0x0D53,
	GL_BLUE_BITS			= 0x0D54,
	GL_INDEX_BITS			= 0x0D51,
	GL_SUBPIXEL_BITS		= 0x0D50,
	GL_AUX_BUFFERS 		= 0x0C00,
	GL_READ_BUFFER 		= 0x0C02,
	GL_DRAW_BUFFER 		= 0x0C01,
	GL_DOUBLEBUFFER			= 0x0C32,
	GL_STEREO			= 0x0C33,
	GL_BITMAP			= 0x1A00,
	GL_COLOR 		= 0x1800,
	GL_DEPTH 		= 0x1801,
	GL_STENCIL			= 0x1802,
	GL_DITHER			= 0x0BD0,
	GL_RGB				= 0x1907,
	GL_RGBA				= 0x1908,

	/* Implementation limits */
	GL_MAX_MODELVIEW_STACK_DEPTH	= 0x0D36,
	GL_MAX_PROJECTION_STACK_DEPTH = 0x0D38,
	GL_MAX_TEXTURE_STACK_DEPTH = 0x0D39,
	GL_MAX_ATTRIB_STACK_DEPTH	= 0x0D35,
	GL_MAX_NAME_STACK_DEPTH 	= 0x0D37,
	GL_MAX_LIST_NESTING		= 0x0B31,
	GL_MAX_LIGHTS			= 0x0D31,
	GL_MAX_CLIP_PLANES		= 0x0D32,
	GL_MAX_VIEWPORT_DIMS 	= 0x0D3A,
	GL_MAX_PIXEL_MAP_TABLE		= 0x0D34,
	GL_MAX_EVAL_ORDER 	= 0x0D30,
	GL_MAX_TEXTURE_SIZE		= 0x0D33,

	/* Gets */
	GL_ATTRIB_STACK_DEPTH		= 0x0BB0,
	GL_COLOR_CLEAR_VALUE 	= 0x0C22,
	GL_COLOR_WRITEMASK		= 0x0C23,
	GL_CURRENT_INDEX		= 0x0B01,
	GL_CURRENT_COLOR		= 0x0B00,
	GL_CURRENT_NORMAL 	= 0x0B02,
	GL_CURRENT_RASTER_COLOR 	= 0x0B04,
	GL_CURRENT_RASTER_DISTANCE = 0x0B09,
	GL_CURRENT_RASTER_INDEX 	= 0x0B05,
	GL_CURRENT_RASTER_POSITION = 0x0B07,
	GL_CURRENT_RASTER_TEXTURE_COORDS = 0x0B06,
	GL_CURRENT_RASTER_POSITION_VALID = 0x0B08,
	GL_CURRENT_TEXTURE_COORDS	= 0x0B03,
	GL_INDEX_CLEAR_VALUE 	= 0x0C20,
	GL_INDEX_MODE			= 0x0C30,
	GL_INDEX_WRITEMASK		= 0x0C21,
	GL_MODELVIEW_MATRIX		= 0x0BA6,
	GL_MODELVIEW_STACK_DEPTH	= 0x0BA3,
	GL_NAME_STACK_DEPTH		= 0x0D70,
	GL_PROJECTION_MATRIX 	= 0x0BA7,
	GL_PROJECTION_STACK_DEPTH	= 0x0BA4,
	GL_RENDER_MODE 		= 0x0C40,
	GL_RGBA_MODE			= 0x0C31,
	GL_TEXTURE_MATRIX 	= 0x0BA8,
	GL_TEXTURE_STACK_DEPTH		= 0x0BA5,
	GL_VIEWPORT 		= 0x0BA2,


	/* Evaluators */
	GL_AUTO_NORMAL 		= 0x0D80,
	GL_MAP1_COLOR_4			= 0x0D90,
	GL_MAP1_GRID_DOMAIN		= 0x0DD0,
	GL_MAP1_GRID_SEGMENTS		= 0x0DD1,
	GL_MAP1_INDEX			= 0x0D91,
	GL_MAP1_NORMAL 		= 0x0D92,
	GL_MAP1_TEXTURE_COORD_1 	= 0x0D93,
	GL_MAP1_TEXTURE_COORD_2 	= 0x0D94,
	GL_MAP1_TEXTURE_COORD_3 	= 0x0D95,
	GL_MAP1_TEXTURE_COORD_4 	= 0x0D96,
	GL_MAP1_VERTEX_3		= 0x0D97,
	GL_MAP1_VERTEX_4		= 0x0D98,
	GL_MAP2_COLOR_4			= 0x0DB0,
	GL_MAP2_GRID_DOMAIN		= 0x0DD2,
	GL_MAP2_GRID_SEGMENTS		= 0x0DD3,
	GL_MAP2_INDEX			= 0x0DB1,
	GL_MAP2_NORMAL 		= 0x0DB2,
	GL_MAP2_TEXTURE_COORD_1 	= 0x0DB3,
	GL_MAP2_TEXTURE_COORD_2 	= 0x0DB4,
	GL_MAP2_TEXTURE_COORD_3 	= 0x0DB5,
	GL_MAP2_TEXTURE_COORD_4 	= 0x0DB6,
	GL_MAP2_VERTEX_3		= 0x0DB7,
	GL_MAP2_VERTEX_4		= 0x0DB8,
	GL_COEFF 		= 0x0A00,
	GL_DOMAIN			= 0x0A02,
	GL_ORDER 		= 0x0A01,

	/* Hints */
	GL_FOG_HINT 		= 0x0C54,
	GL_LINE_SMOOTH_HINT		= 0x0C52,
	GL_PERSPECTIVE_CORRECTION_HINT	= 0x0C50,
	GL_POINT_SMOOTH_HINT 	= 0x0C51,
	GL_POLYGON_SMOOTH_HINT		= 0x0C53,
	GL_DONT_CARE			= 0x1100,
	GL_FASTEST			= 0x1101,
	GL_NICEST			= 0x1102,

	/* Scissor box */
	GL_SCISSOR_TEST			= 0x0C11,
	GL_SCISSOR_BOX 		= 0x0C10,

	/* Pixel Mode / Transfer */
	GL_MAP_COLOR			= 0x0D10,
	GL_MAP_STENCIL 		= 0x0D11,
	GL_INDEX_SHIFT 		= 0x0D12,
	GL_INDEX_OFFSET			= 0x0D13,
	GL_RED_SCALE			= 0x0D14,
	GL_RED_BIAS 		= 0x0D15,
	GL_GREEN_SCALE 		= 0x0D18,
	GL_GREEN_BIAS			= 0x0D19,
	GL_BLUE_SCALE			= 0x0D1A,
	GL_BLUE_BIAS			= 0x0D1B,
	GL_ALPHA_SCALE 		= 0x0D1C,
	GL_ALPHA_BIAS			= 0x0D1D,
	GL_DEPTH_SCALE 		= 0x0D1E,
	GL_DEPTH_BIAS			= 0x0D1F,
	GL_PIXEL_MAP_S_TO_S_SIZE	= 0x0CB1,
	GL_PIXEL_MAP_I_TO_I_SIZE	= 0x0CB0,
	GL_PIXEL_MAP_I_TO_R_SIZE	= 0x0CB2,
	GL_PIXEL_MAP_I_TO_G_SIZE	= 0x0CB3,
	GL_PIXEL_MAP_I_TO_B_SIZE	= 0x0CB4,
	GL_PIXEL_MAP_I_TO_A_SIZE	= 0x0CB5,
	GL_PIXEL_MAP_R_TO_R_SIZE	= 0x0CB6,
	GL_PIXEL_MAP_G_TO_G_SIZE	= 0x0CB7,
	GL_PIXEL_MAP_B_TO_B_SIZE	= 0x0CB8,
	GL_PIXEL_MAP_A_TO_A_SIZE	= 0x0CB9,
	GL_PIXEL_MAP_S_TO_S		= 0x0C71,
	GL_PIXEL_MAP_I_TO_I		= 0x0C70,
	GL_PIXEL_MAP_I_TO_R		= 0x0C72,
	GL_PIXEL_MAP_I_TO_G		= 0x0C73,
	GL_PIXEL_MAP_I_TO_B		= 0x0C74,
	GL_PIXEL_MAP_I_TO_A		= 0x0C75,
	GL_PIXEL_MAP_R_TO_R		= 0x0C76,
	GL_PIXEL_MAP_G_TO_G		= 0x0C77,
	GL_PIXEL_MAP_B_TO_B		= 0x0C78,
	GL_PIXEL_MAP_A_TO_A		= 0x0C79,
	GL_PACK_ALIGNMENT 	= 0x0D05,
	GL_PACK_LSB_FIRST 	= 0x0D01,
	GL_PACK_ROW_LENGTH		= 0x0D02,
	GL_PACK_SKIP_PIXELS		= 0x0D04,
	GL_PACK_SKIP_ROWS 	= 0x0D03,
	GL_PACK_SWAP_BYTES		= 0x0D00,
	GL_UNPACK_ALIGNMENT		= 0x0CF5,
	GL_UNPACK_LSB_FIRST		= 0x0CF1,
	GL_UNPACK_ROW_LENGTH 	= 0x0CF2,
	GL_UNPACK_SKIP_PIXELS		= 0x0CF4,
	GL_UNPACK_SKIP_ROWS		= 0x0CF3,
	GL_UNPACK_SWAP_BYTES 	= 0x0CF0,
	GL_ZOOM_X			= 0x0D16,
	GL_ZOOM_Y			= 0x0D17,

	/* Texture mapping */
	GL_TEXTURE_ENV 		= 0x2300,
	GL_TEXTURE_ENV_MODE		= 0x2200,
	GL_TEXTURE_1D			= 0x0DE0,
	GL_TEXTURE_2D			= 0x0DE1,
	GL_TEXTURE_WRAP_S 	= 0x2802,
	GL_TEXTURE_WRAP_T 	= 0x2803,
	GL_TEXTURE_MAG_FILTER		= 0x2800,
	GL_TEXTURE_MIN_FILTER		= 0x2801,
	GL_TEXTURE_ENV_COLOR 	= 0x2201,
	GL_TEXTURE_GEN_S		= 0x0C60,
	GL_TEXTURE_GEN_T		= 0x0C61,
	GL_TEXTURE_GEN_MODE		= 0x2500,
	GL_TEXTURE_BORDER_COLOR 	= 0x1004,
	GL_TEXTURE_WIDTH		= 0x1000,
	GL_TEXTURE_HEIGHT 	= 0x1001,
	GL_TEXTURE_BORDER 	= 0x1005,
	GL_TEXTURE_COMPONENTS		= 0x1003,
	GL_NEAREST_MIPMAP_NEAREST	= 0x2700,
	GL_NEAREST_MIPMAP_LINEAR	= 0x2702,
	GL_LINEAR_MIPMAP_NEAREST	= 0x2701,
	GL_LINEAR_MIPMAP_LINEAR 	= 0x2703,
	GL_OBJECT_LINEAR		= 0x2401,
	GL_OBJECT_PLANE			= 0x2501,
	GL_EYE_LINEAR			= 0x2400,
	GL_EYE_PLANE			= 0x2502,
	GL_SPHERE_MAP			= 0x2402,
	GL_DECAL 		= 0x2101,
	GL_MODULATE 		= 0x2100,
	GL_NEAREST			= 0x2600,
	GL_REPEAT			= 0x2901,
	GL_CLAMP 		= 0x2900,
	GL_S				= 0x2000,
	GL_T				= 0x2001,
	GL_R				= 0x2002,
	GL_Q				= 0x2003,
	GL_TEXTURE_GEN_R		= 0x0C62,
	GL_TEXTURE_GEN_Q		= 0x0C63,

	/* Utility */
	GL_VENDOR			= 0x1F00,
	GL_RENDERER 		= 0x1F01,
	GL_VERSION			= 0x1F02,
	GL_EXTENSIONS			= 0x1F03,

	/* Errors */
	GL_INVALID_VALUE		= 0x0501,
	GL_INVALID_ENUM			= 0x0500,
	GL_INVALID_OPERATION 	= 0x0502,
	GL_STACK_OVERFLOW 	= 0x0503,
	GL_STACK_UNDERFLOW		= 0x0504,
	GL_OUT_OF_MEMORY		= 0x0505,

	/* Blending Extensions */
	GL_CONSTANT_COLOR_EXT		= 0x8001,
	GL_ONE_MINUS_CONSTANT_COLOR_EXT	= 0x8002,
	GL_CONSTANT_ALPHA_EXT		= 0x8003,
	GL_ONE_MINUS_CONSTANT_ALPHA_EXT	= 0x8004,
	GL_BLEND_EQUATION_EXT		= 0x8009,
	GL_MIN_EXT			= 0x8007,
	GL_MAX_EXT			= 0x8008,
	GL_FUNC_ADD_EXT			= 0x8006,
	GL_FUNC_SUBTRACT_EXT 	= 0x800A,
	GL_FUNC_REVERSE_SUBTRACT_EXT	= 0x800B,
	GL_BLEND_COLOR_EXT		= 0x8005,
	GL_REPLACE_EXT 		= 0x8062,

	/* Polygon Offset Extension */
		  GL_POLYGON_OFFSET_EXT 			 = 0x8037,
		  GL_POLYGON_OFFSET_FACTOR_EXT	 = 0x8038,
		  GL_POLYGON_OFFSET_BIAS_EXT		 = 0x8039,

	/* Vertex Array Extension */
	GL_VERTEX_ARRAY_EXT		= 0x8074,
	GL_NORMAL_ARRAY_EXT		= 0x8075,
	GL_COLOR_ARRAY_EXT		= 0x8076,
	GL_INDEX_ARRAY_EXT		= 0x8077,
	GL_TEXTURE_COORD_ARRAY_EXT = 0x8078,
	GL_EDGE_FLAG_ARRAY_EXT		= 0x8079,
	GL_VERTEX_ARRAY_SIZE_EXT	= 0x807A,
	GL_VERTEX_ARRAY_TYPE_EXT	= 0x807B,
	GL_VERTEX_ARRAY_STRIDE_EXT = 0x807C,
	GL_VERTEX_ARRAY_COUNT_EXT	= 0x807D,
	GL_NORMAL_ARRAY_TYPE_EXT	= 0x807E,
	GL_NORMAL_ARRAY_STRIDE_EXT = 0x807F,
	GL_NORMAL_ARRAY_COUNT_EXT	= 0x8080,
	GL_COLOR_ARRAY_SIZE_EXT 	= 0x8081,
	GL_COLOR_ARRAY_TYPE_EXT 	= 0x8082,
	GL_COLOR_ARRAY_STRIDE_EXT	= 0x8083,
	GL_COLOR_ARRAY_COUNT_EXT	= 0x8084,
	GL_INDEX_ARRAY_TYPE_EXT 	= 0x8085,
	GL_INDEX_ARRAY_STRIDE_EXT	= 0x8086,
	GL_INDEX_ARRAY_COUNT_EXT	= 0x8087,
	GL_TEXTURE_COORD_ARRAY_SIZE_EXT	= 0x8088,
	GL_TEXTURE_COORD_ARRAY_TYPE_EXT	= 0x8089,
	GL_TEXTURE_COORD_ARRAY_STRIDE_EXT= 0x808A,
	GL_TEXTURE_COORD_ARRAY_COUNT_EXT= 0x808B,
	GL_EDGE_FLAG_ARRAY_STRIDE_EXT = 0x808C,
	GL_EDGE_FLAG_ARRAY_COUNT_EXT	= 0x808D,
	GL_VERTEX_ARRAY_POINTER_EXT	= 0x808E,
	GL_NORMAL_ARRAY_POINTER_EXT	= 0x808F,
	GL_COLOR_ARRAY_POINTER_EXT = 0x8090,
	GL_INDEX_ARRAY_POINTER_EXT = 0x8091,
	GL_TEXTURE_COORD_ARRAY_POINTER_EXT= 0x8092,
	GL_EDGE_FLAG_ARRAY_POINTER_EXT	= 0x8093
}
#ifdef CENTERLINE_CLPP
  /* CenterLine C++ workaround: */
  gl_enum;
  typedef int GLenum;
#else
  /* all other compilers */
  GLenum;
#endif


/* GL_NO_ERROR must be zero */
#define GL_NO_ERROR GL_FALSE



typedef enum {
	GL_CURRENT_BIT 	= 0x00000001,
	GL_POINT_BIT		= 0x00000002,
	GL_LINE_BIT 	= 0x00000004,
	GL_POLYGON_BIT 	= 0x00000008,
	GL_POLYGON_STIPPLE_BIT	= 0x00000010,
	GL_PIXEL_MODE_BIT = 0x00000020,
	GL_LIGHTING_BIT		= 0x00000040,
	GL_FOG_BIT		= 0x00000080,
	GL_DEPTH_BUFFER_BIT	= 0x00000100,
	GL_ACCUM_BUFFER_BIT	= 0x00000200,
	GL_STENCIL_BUFFER_BIT	= 0x00000400,
	GL_VIEWPORT_BIT		= 0x00000800,
	GL_TRANSFORM_BIT	= 0x00001000,
	GL_ENABLE_BIT		= 0x00002000,
	GL_COLOR_BUFFER_BIT	= 0x00004000,
	GL_HINT_BIT 	= 0x00008000,
	GL_EVAL_BIT 	= 0x00010000,
	GL_LIST_BIT 	= 0x00020000,
	GL_TEXTURE_BIT 	= 0x00040000,
	GL_SCISSOR_BIT 	= 0x00080000,
	GL_ALL_ATTRIB_BITS	= 0x000fffff
} GLbitfield;


#ifdef CENTERLINE_CLPP
#define signed
#endif



/*
 *
 * Data types (may be architecture dependent in some cases)
 *
 */



/*  C type		GL type		storage									  */
/*-------------------------------------------------------------------------*/
typedef void		GLvoid;
typedef unsigned char	GLboolean;
typedef signed char	GLbyte;		/* 1-byte signed */
typedef short		GLshort; /* 2-byte signed */
typedef int 	GLint;		/* 4-byte signed */
typedef unsigned char	GLubyte; /* 1-byte unsigned */
typedef unsigned short	GLushort;	/* 2-byte unsigned */
typedef unsigned int GLuint;		/* 4-byte unsigned */
typedef int 	GLsizei; /* 4-byte signed */
typedef float		GLfloat; /* single precision float */
typedef float		GLclampf;	/* single precision float in [0,1] */
typedef float		GLdouble;	/* double precision float @@@ TODO should be double*/
typedef float		GLclampd;	/* double precision float in [0,1] @@@ TODO should be double*/

/*
 *
 * Amiga Mesa init and closing rutines
 *
 */


__regargs __asm __saveds struct amigamesa_context *AmigaMesaCreateContext(register __a1 struct Window *window,register __d0 GLboolean rgb_flag,register __d1 GLboolean db_flag );
__regargs __asm __saveds void AmigaMesaDestroyContext(register __a0 struct amigamesa_context *c );
__regargs __asm __saveds void AmigaMesaMakeCurrent(register __a0 struct amigamesa_context *c );
__regargs __asm __saveds void AmigaMesaSwapBuffers(void);
/* This is on the drawingboard */
__regargs __asm __saveds BOOL AmigaMesaSetDefs(register __a0 struct TagItem *tagList);


/*
 * Miscellaneous
 */

__asm __saveds extern void glClearIndex(register __d0 GLfloat c );
__asm __saveds extern void glClearColor(register __d0 GLclampf red,register __d1 GLclampf green,register __d2 GLclampf blue,register __d3 GLclampf alpha );
__asm __saveds extern void glClear(register __d0 GLbitfield mask );
__asm __saveds extern void glIndexMask(register __d0 GLuint mask );

__asm __saveds extern void glColorMask(register __d0 GLboolean red,register __d1 GLboolean green,register __d2 GLboolean blue,register __d3	GLboolean alpha );
__asm __saveds extern void glAlphaFunc(register __d0 GLenum func,register __d1  GLclampf ref );
__asm __saveds extern void glBlendFunc(register __d0 GLenum sfactor,register __d1  GLenum dfactor );
__asm __saveds extern void glLogicOp(register __d0 GLenum opcode );
__asm __saveds extern void glCullFace(register __d0 GLenum mode );
__asm __saveds extern void glFrontFace(register __d0 GLenum mode );
__asm __saveds extern void glPointSize(register __d0 GLfloat size );
__asm __saveds extern void glLineWidth(register __d0 GLfloat width );
__asm __saveds extern void glLineStipple(register __d0 GLint factor,register __d1  GLushort pattern );
__asm __saveds extern void glPolygonMode(register __d0 GLenum face,register __d1  GLenum mode );
__asm __saveds extern void glPolygonStipple(register __a0 const GLubyte *mask );
__asm __saveds extern void glGetPolygonStipple(register __a0 GLubyte *mask );
__asm __saveds extern void glEdgeFlag(register __d0 GLboolean flag );
__asm __saveds extern void glEdgeFlagv(register __a0 const GLboolean *flag );
__asm __saveds extern void glScissor(register __d0 GLint x,register __d1  GLint y,register __d2  GLsizei width,register __d3	GLsizei height);
__asm __saveds extern void glClipPlane(register __d0 GLenum plane,register __a0 const GLdouble *equation );
__asm __saveds extern void glGetClipPlane(register __d0 GLenum plane,register __a0 GLdouble *equation );
__asm __saveds extern void glDrawBuffer(register __d0 GLenum mode );
__asm __saveds extern void glReadBuffer(register __d0 GLenum mode );
__asm __saveds extern void glEnable(register __d0 GLenum cap );
__asm __saveds extern void glDisable(register __d0 GLenum cap );
__asm __saveds extern GLboolean glIsEnabled(register __d0 GLenum cap );

__asm __saveds extern void glGetBooleanv(register __d0 GLenum pname,register __a0 GLboolean *params );
__asm __saveds extern void glGetDoublev(register __d0 GLenum pname,register __a0 GLdouble *params );
__asm __saveds extern void glGetFloatv(register __d0 GLenum pname,register __a0 GLfloat *params );
__asm __saveds extern void glGetIntegerv(register __d0 GLenum pname,register __a0 GLint *params );

__asm __saveds extern void glPushAttrib(register __d0 GLbitfield mask );
__asm __saveds extern void glPopAttrib( void );
__asm __saveds extern GLint glRenderMode(register __d0 GLenum mode );
__asm __saveds extern GLenum glGetError( void );
__asm __saveds extern const GLubyte *glGetString(register __d0 GLenum name );

__asm __saveds extern void glFinish( void );
__asm __saveds extern void glFlush( void );
__asm __saveds extern void glHint(register __d0 GLenum target,register __d1  GLenum mode );



/*
 * Depth Buffer
 */

__asm __saveds extern void glClearDepth(register __d0 GLclampd depth );
__asm __saveds extern void glDepthFunc(register __d0 GLenum func );
__asm __saveds extern void glDepthMask(register __d0 GLboolean flag );
__asm __saveds extern void glDepthRange(register __d0 GLclampd near_val,register __d1	GLclampd far_val );


/*
 * Accumulation Buffer
 */

__asm __saveds extern void glClearAccum(register __d0 GLfloat red,register __d1	GLfloat green,register __d2 GLfloat blue,register __d3  GLfloat alpha );
__asm __saveds extern void glAccum(register __d0 GLenum op,register __d1  GLfloat value );



/*
 * Transformation
 */

__asm __saveds extern void glMatrixMode(register __d0 GLenum mode );
__asm __saveds extern void glOrtho(register __d0 GLdouble left,register __d1	GLdouble right,register __d2
			  GLdouble bottom,register __d3	GLdouble top,register __d4
			  GLdouble nearval,register __d5  GLdouble farval );

__asm __saveds extern void glFrustum(register __d0 GLdouble left,register __d1  GLdouble right,register __d2
				 GLdouble bottom,register __d3  GLdouble top,register __d4
				 GLdouble nearval,register __d5	GLdouble farval );
__asm __saveds extern void glViewport(register __d0 GLint x,register __d1	GLint y,register __d2  GLsizei width,register __d3  GLsizei height );
__asm __saveds extern void glPushMatrix( void );
__asm __saveds extern void glPopMatrix( void );
__asm __saveds extern void glLoadIdentity( void );

__asm __saveds extern void glLoadMatrixd(register __a0 const GLdouble *m );
__asm __saveds extern void glLoadMatrixf(register __a0 const GLfloat *m );

__asm __saveds extern void glMultMatrixd(register __a0 const GLdouble *m );
__asm __saveds extern void glMultMatrixf(register __a0 const GLfloat *m );

__asm __saveds extern void glRotated(register __d0 GLdouble angle,register __d1	GLdouble x,register __d2  GLdouble y,register __d3  GLdouble z );
__asm __saveds extern void glRotatef(register __d0 GLfloat angle,register __d1 GLfloat x,register __d2  GLfloat y,register __d3	GLfloat z );

__asm __saveds extern void glScaled(register __d0 GLdouble x,register __d1  GLdouble y,register __d2	GLdouble z );
__asm __saveds extern void glScalef(register __d0 GLfloat x,register __d1	GLfloat y,register __d2  GLfloat z );

__asm __saveds extern void glTranslated(register __d0 GLdouble x,register __d1  GLdouble y,register __d2  GLdouble z );
__asm __saveds extern void glTranslatef(register __d0 GLfloat x,register __d1  GLfloat y,register __d2  GLfloat z );



/*
 * Display Lists
 */

__asm __saveds extern GLboolean glIsList(register __d0 GLuint list );
__asm __saveds extern void glDeleteLists(register __d0 GLuint list,register __d1  GLsizei range );
__asm __saveds extern GLuint glGenLists(register __d0 GLsizei range );
__asm __saveds extern void glNewList(register __d0 GLuint list,register __d1	GLenum mode );
__asm __saveds extern void glEndList( void );
__asm __saveds extern void glCallList(register __d0 GLuint list );
__asm __saveds extern void glCallLists(register __d0 GLsizei n,register __d1	GLenum type,register __a0	const GLvoid *lists );
__asm __saveds extern void glListBase(register __d0 GLuint base );



/*
 * Drawing Functions
 */

__asm __saveds extern void glBegin(register __d0 GLenum p );
__asm __saveds extern void glEnd( void );

__asm __saveds extern void glVertex2d(register __d0 GLdouble x,register __d1	GLdouble y );
__asm __saveds extern void glVertex2f(register __d0 GLfloat x,register __d1  GLfloat y );
__asm __saveds extern void glVertex2i(register __d0 GLint x,register __d1	GLint y );
__asm __saveds extern void glVertex2s(register __d0 GLshort x,register __d1  GLshort y );

__asm __saveds extern void glVertex3d(register __d0 GLdouble x,register __d1	GLdouble y,register __d2  GLdouble z );
__asm __saveds extern void glVertex3f(register __d0 GLfloat x,register __d1  GLfloat y,register __d2	GLfloat z );
__asm __saveds extern void glVertex3i(register __d0 GLint x,register __d1	GLint y,register __d2  GLint z );
__asm __saveds extern void glVertex3s(register __d0 GLshort x,register __d1  GLshort y,register __d2	GLshort z );

__asm __saveds extern void glVertex4d(register __d0 GLdouble x,register __d1	GLdouble y,register __d2  GLdouble z,register __d3  GLdouble w );
__asm __saveds extern void glVertex4f(register __d0 GLfloat x,register __d1  GLfloat y,register __d2	GLfloat z,register __d3  GLfloat w );
__asm __saveds extern void glVertex4i(register __d0 GLint x,register __d1	GLint y,register __d2  GLint z,register __d3  GLint w );
__asm __saveds extern void glVertex4s(register __d0 GLshort x,register __d1  GLshort y,register __d2	GLshort z,register __d3  GLshort w );

__asm __saveds extern void glVertex2dv(register __a0 const GLdouble *v );
__asm __saveds extern void glVertex2fv(register __a0 const GLfloat *v );
__asm __saveds extern void glVertex2iv(register __a0 const GLint *v );
__asm __saveds extern void glVertex2sv(register __a0 const GLshort *v );

__asm __saveds extern void glVertex3dv(register __a0 const GLdouble *v );
__asm __saveds extern void glVertex3fv(register __a0 const GLfloat *v );
__asm __saveds extern void glVertex3iv(register __a0 const GLint *v );
__asm __saveds extern void glVertex3sv(register __a0 const GLshort *v );

__asm __saveds extern void glVertex4dv(register __a0 const GLdouble *v );
__asm __saveds extern void glVertex4fv(register __a0 const GLfloat *v );
__asm __saveds extern void glVertex4iv(register __a0 const GLint *v );
__asm __saveds extern void glVertex4sv(register __a0 const GLshort *v );

__asm __saveds extern void glNormal3b(register __d0 GLbyte nx,register __d1  GLbyte ny,register __d2	GLbyte nz );
__asm __saveds extern void glNormal3d(register __d0 GLdouble nx,register __d1  GLdouble ny,register __d2  GLdouble nz );
__asm __saveds extern void glNormal3f(register __d0 GLfloat nx,register __d1	GLfloat ny,register __d2  GLfloat nz );
__asm __saveds extern void glNormal3i(register __d0 GLint nx,register __d1  GLint ny,register __d2  GLint nz );
__asm __saveds extern void glNormal3s(register __d0 GLshort nx,register __d1	GLshort ny,register __d2  GLshort nz );

__asm __saveds extern void glNormal3bv(register __a0 const GLbyte *v );
__asm __saveds extern void glNormal3dv(register __a0 const GLdouble *v );
__asm __saveds extern void glNormal3fv(register __a0 const GLfloat *v );
__asm __saveds extern void glNormal3iv(register __a0 const GLint *v );
__asm __saveds extern void glNormal3sv(register __a0 const GLshort *v );

__asm __saveds extern void glIndexd(register __d0 GLdouble c );
__asm __saveds extern void glIndexf(register __d0 GLfloat c );
__asm __saveds extern void glIndexi(register __d0 GLint c );
__asm __saveds extern void glIndexs(register __d0 GLshort c );

__asm __saveds extern void glIndexdv(register __a0 const GLdouble *c );
__asm __saveds extern void glIndexfv(register __a0 const GLfloat *c );
__asm __saveds extern void glIndexiv(register __a0 const GLint *c );
__asm __saveds extern void glIndexsv(register __a0 const GLshort *c );

__asm __saveds extern void glColor3b(register __d0 GLbyte red,register __d1  GLbyte green,register __d2	GLbyte blue );
__asm __saveds extern void glColor3d(register __d0 GLdouble red,register __d1  GLdouble green,register __d2  GLdouble blue );
__asm __saveds extern void glColor3f(register __d0 GLfloat red,register __d1	GLfloat green,register __d2  GLfloat blue );
__asm __saveds extern void glColor3i(register __d0 GLint red,register __d1  GLint green,register __d2  GLint blue );
__asm __saveds extern void glColor3s(register __d0 GLshort red,register __d1	GLshort green,register __d2  GLshort blue );
__asm __saveds extern void glColor3ub(register __d0 GLubyte red,register __d1  GLubyte green,register __d2	GLubyte blue );
__asm __saveds extern void glColor3ui(register __d0 GLuint red,register __d1	GLuint green,register __d2  GLuint blue );
__asm __saveds extern void glColor3us(register __d0 GLushort red,register __d1  GLushort green,register __d2  GLushort blue );

__asm __saveds extern void glColor4b(register __d0 GLbyte red,register __d1  GLbyte green,register __d2	GLbyte blue,register __d3	GLbyte alpha );
__asm __saveds extern void glColor4d(register __d0 GLdouble red,register __d1  GLdouble green,register __d2 GLdouble blue,register __d3  GLdouble alpha );
__asm __saveds extern void glColor4f(register __d0 GLfloat red,register __d1	GLfloat green,register __d2 GLfloat blue,register __d3  GLfloat alpha );
__asm __saveds extern void glColor4i(register __d0 GLint red,register __d1  GLint green,register __d2  GLint blue,register __d3	GLint alpha );
__asm __saveds extern void glColor4s(register __d0 GLshort red,register __d1	GLshort green,register __d2 GLshort blue,register __d3  GLshort alpha );
__asm __saveds extern void glColor4ub(register __d0 GLubyte red,register __d1  GLubyte green,register __d2 GLubyte blue,register __d3	GLubyte alpha );
__asm __saveds extern void glColor4ui(register __d0 GLuint red,register __d1	GLuint green,register __d2  GLuint blue,register __d3  GLuint alpha );
__asm __saveds extern void glColor4us(register __d0 GLushort red,register __d1  GLushort green,register __d2 GLushort blue,register __d3	GLushort alpha );

__asm __saveds extern void glColor3bv(register __a0 const GLbyte *v );
__asm __saveds extern void glColor3dv(register __a0 const GLdouble *v );
__asm __saveds extern void glColor3fv(register __a0 const GLfloat *v );
__asm __saveds extern void glColor3iv(register __a0 const GLint *v );
__asm __saveds extern void glColor3sv(register __a0 const GLshort *v );
__asm __saveds extern void glColor3ubv(register __a0 const GLubyte *v );
__asm __saveds extern void glColor3uiv(register __a0 const GLuint *v );
__asm __saveds extern void glColor3usv(register __a0 const GLushort *v );

__asm __saveds extern void glColor4bv(register __a0 const GLbyte *v );
__asm __saveds extern void glColor4dv(register __a0 const GLdouble *v );
__asm __saveds extern void glColor4fv(register __a0 const GLfloat *v );
__asm __saveds extern void glColor4iv(register __a0 const GLint *v );
__asm __saveds extern void glColor4sv(register __a0 const GLshort *v );
__asm __saveds extern void glColor4ubv(register __a0 const GLubyte *v );
__asm __saveds extern void glColor4uiv(register __a0 const GLuint *v );
__asm __saveds extern void glColor4usv(register __a0 const GLushort *v );

__asm __saveds extern void glTexCoord1d(register __d0 GLdouble s );
__asm __saveds extern void glTexCoord1f(register __d0 GLfloat s );
__asm __saveds extern void glTexCoord1i(register __d0 GLint s );
__asm __saveds extern void glTexCoord1s(register __d0 GLshort s );

__asm __saveds extern void glTexCoord2d(register __d0 GLdouble s,register __d1  GLdouble t );
__asm __saveds extern void glTexCoord2f(register __d0 GLfloat s,register __d1  GLfloat t );
__asm __saveds extern void glTexCoord2i(register __d0 GLint s,register __d1  GLint t );
__asm __saveds extern void glTexCoord2s(register __d0 GLshort s,register __d1  GLshort t );

__asm __saveds extern void glTexCoord3d(register __d0 GLdouble s,register __d1  GLdouble t,register __d2  GLdouble r );
__asm __saveds extern void glTexCoord3f(register __d0 GLfloat s,register __d1  GLfloat t,register __d2  GLfloat r );
__asm __saveds extern void glTexCoord3i(register __d0 GLint s,register __d1  GLint t,register __d2  GLint r );
__asm __saveds extern void glTexCoord3s(register __d0 GLshort s,register __d1  GLshort t,register __d2  GLshort r );

__asm __saveds extern void glTexCoord4d(register __d0 GLdouble s,register __d1  GLdouble t,register __d2  GLdouble r,register __d3	GLdouble q );
__asm __saveds extern void glTexCoord4f(register __d0 GLfloat s,register __d1  GLfloat t,register __d2  GLfloat r,register __d3	GLfloat q );
__asm __saveds extern void glTexCoord4i(register __d0 GLint s,register __d1  GLint t,register __d2  GLint r,register __d3	GLint q );
__asm __saveds extern void glTexCoord4s(register __d0 GLshort s,register __d1  GLshort t,register __d2  GLshort r,register __d3	GLshort q );

__asm __saveds extern void glTexCoord1dv(register __a0 const GLdouble *v );
__asm __saveds extern void glTexCoord1fv(register __a0 const GLfloat *v );
__asm __saveds extern void glTexCoord1iv(register __a0 const GLint *v );
__asm __saveds extern void glTexCoord1sv(register __a0 const GLshort *v );

__asm __saveds extern void glTexCoord2dv(register __a0 const GLdouble *v );
__asm __saveds extern void glTexCoord2fv(register __a0 const GLfloat *v );
__asm __saveds extern void glTexCoord2iv(register __a0 const GLint *v );
__asm __saveds extern void glTexCoord2sv(register __a0 const GLshort *v );

__asm __saveds extern void glTexCoord3dv(register __a0 const GLdouble *v );
__asm __saveds extern void glTexCoord3fv(register __a0 const GLfloat *v );
__asm __saveds extern void glTexCoord3iv(register __a0 const GLint *v );
__asm __saveds extern void glTexCoord3sv(register __a0 const GLshort *v );

__asm __saveds extern void glTexCoord4dv(register __a0 const GLdouble *v );
__asm __saveds extern void glTexCoord4fv(register __a0 const GLfloat *v );
__asm __saveds extern void glTexCoord4iv(register __a0 const GLint *v );
__asm __saveds extern void glTexCoord4sv(register __a0 const GLshort *v );

__asm __saveds extern void glRasterPos2d(register __d0 GLdouble x,register __d1	GLdouble y );
__asm __saveds extern void glRasterPos2f(register __d0 GLfloat x,register __d1  GLfloat y );
__asm __saveds extern void glRasterPos2i(register __d0 GLint x,register __d1	GLint y );
__asm __saveds extern void glRasterPos2s(register __d0 GLshort x,register __d1  GLshort y );

__asm __saveds extern void glRasterPos3d(register __d0 GLdouble x,register __d1	GLdouble y,register __d2  GLdouble z );
__asm __saveds extern void glRasterPos3f(register __d0 GLfloat x,register __d1  GLfloat y,register __d2	GLfloat z );
__asm __saveds extern void glRasterPos3i(register __d0 GLint x,register __d1	GLint y,register __d2  GLint z );
__asm __saveds extern void glRasterPos3s(register __d0 GLshort x,register __d1  GLshort y,register __d2	GLshort z );

__asm __saveds extern void glRasterPos4d(register __d0 GLdouble x,register __d1	GLdouble y,register __d2  GLdouble z,register __d3  GLdouble w );
__asm __saveds extern void glRasterPos4f(register __d0 GLfloat x,register __d1  GLfloat y,register __d2	GLfloat z,register __d3  GLfloat w );
__asm __saveds extern void glRasterPos4i(register __d0 GLint x,register __d1	GLint y,register __d2  GLint z,register __d3  GLint w );
__asm __saveds extern void glRasterPos4s(register __d0 GLshort x,register __d1  GLshort y,register __d2	GLshort z,register __d3  GLshort w );

__asm __saveds extern void glRasterPos2dv(register __a0 const GLdouble *v );
__asm __saveds extern void glRasterPos2fv(register __a0 const GLfloat *v );
__asm __saveds extern void glRasterPos2iv(register __a0 const GLint *v );
__asm __saveds extern void glRasterPos2sv(register __a0 const GLshort *v );

__asm __saveds extern void glRasterPos3dv(register __a0 const GLdouble *v );
__asm __saveds extern void glRasterPos3fv(register __a0 const GLfloat *v );
__asm __saveds extern void glRasterPos3iv(register __a0 const GLint *v );
__asm __saveds extern void glRasterPos3sv(register __a0 const GLshort *v );

__asm __saveds extern void glRasterPos4dv(register __a0 const GLdouble *v );
__asm __saveds extern void glRasterPos4fv(register __a0 const GLfloat *v );
__asm __saveds extern void glRasterPos4iv(register __a0 const GLint *v );
__asm __saveds extern void glRasterPos4sv(register __a0 const GLshort *v );

__asm __saveds extern void glRectd(register __d0 GLdouble x1,register __d1  GLdouble y1,register __d2  GLdouble x2,register __d3  GLdouble y2 );
__asm __saveds extern void glRectf(register __d0 GLfloat x1,register __d1	GLfloat y1,register __d2  GLfloat x2,register __d3  GLfloat y2 );
__asm __saveds extern void glRecti(register __d0 GLint x1,register __d1  GLint y1,register __d2  GLint x2,register __d3  GLint y2 );
__asm __saveds extern void glRects(register __d0 GLshort x1,register __d1	GLshort y1,register __d2  GLshort x2,register __d3  GLshort y2 );

__asm __saveds extern void glRectdv(register __a0 const GLdouble *v1,register __a1	const GLdouble *v2 );
__asm __saveds extern void glRectfv(register __a0 const GLfloat *v1,register __a1  const GLfloat *v2 );
__asm __saveds extern void glRectiv(register __a0 const GLint *v1,register __a1	const GLint *v2 );
__asm __saveds extern void glRectsv(register __a0 const GLshort *v1,register __a1  const GLshort *v2 );



/*
 * Lighting
 */

__asm __saveds extern void glShadeModel(register __d0 GLenum mode );

__asm __saveds extern void glLightf(register __d0 GLenum light,register __d1	GLenum pname,register __d2  GLfloat param );
__asm __saveds extern void glLighti(register __d0 GLenum light,register __d1 GLenum pname,register __d2	GLint param );
__asm __saveds extern void glLightfv(register __d0 GLenum light,register __d1  GLenum pname,register __d2  const GLfloat *params );
__asm __saveds extern void glLightiv(register __d0 GLenum light,register __d1  GLenum pname,register __d2  const GLint *params );

__asm __saveds extern void glGetLightfv(register __d0 GLenum light,register __d1  GLenum pname,register __d2  GLfloat *params );
__asm __saveds extern void glGetLightiv(register __d0 GLenum light,register __d1  GLenum pname,register __d2  GLint *params );

__asm __saveds extern void glLightModelf(register __d0 GLenum pname,register __d1  GLfloat param );
__asm __saveds extern void glLightModeli(register __d0 GLenum pname,register __d1  GLint param );
__asm __saveds extern void glLightModelfv(register __d0 GLenum pname,register __a0	const GLfloat *params );
__asm __saveds extern void glLightModeliv(register __d0 GLenum pname,register __a0	const GLint *params );

__asm __saveds extern void glMaterialf(register __d0 GLenum face,register __d1  GLenum pname,register __d2	GLfloat param );
__asm __saveds extern void glMateriali(register __d0 GLenum face,register __d1  GLenum pname,register __d2	GLint param );
__asm __saveds extern void glMaterialfv(register __d0 GLenum face,register __d1	GLenum pname,register __a0  const GLfloat *params );
__asm __saveds extern void glMaterialiv(register __d0 GLenum face,register __d1	GLenum pname,register __a0  const GLint *params );

__asm __saveds extern void glGetMaterialfv(register __d0 GLenum face,register __d1	GLenum pname,register __a0  GLfloat *params );
__asm __saveds extern void glGetMaterialiv(register __d0 GLenum face,register __d1	GLenum pname,register __a0  GLint *params );

__asm __saveds extern void glColorMaterial(register __d0 GLenum face,register __d1	GLenum mode );




/*
 * Raster functions
 */

__asm __saveds extern void glPixelZoom(register __d0 GLfloat xfactor,register __d1	GLfloat yfactor );

__asm __saveds extern void glPixelStoref(register __d0 GLenum pname,register __d1  GLfloat param );
__asm __saveds extern void glPixelStorei(register __d0 GLenum pname,register __d1  GLint param );

__asm __saveds extern void glPixelTransferf(register __d0 GLenum pname,register __d1  GLfloat param );
__asm __saveds extern void glPixelTransferi(register __d0 GLenum pname,register __d1  GLint param );

__asm __saveds extern void glPixelMapfv(register __d0 GLenum map,register __d1  GLint mapsize,register __a0  const GLfloat *values );
__asm __saveds extern void glPixelMapuiv(register __d0 GLenum map,register __d1	GLint mapsize,register __a0  const GLuint *values );
__asm __saveds extern void glPixelMapusv(register __d0 GLenum map,register __d1	GLint mapsize,register __a0  const GLushort *values );

__asm __saveds extern void glGetPixelMapfv(register __d0 GLenum map,register __a0  GLfloat *values );
__asm __saveds extern void glGetPixelMapuiv(register __d0 GLenum map,register __a0	GLuint *values );
__asm __saveds extern void glGetPixelMapusv(register __d0 GLenum map,register __a0	GLushort *values );

__asm __saveds extern void glBitmap(register __d0 GLsizei width,register __d1  GLsizei height,register __d2
				GLfloat xorig,register __d3  GLfloat yorig,register __d4
				GLfloat xmove,register __d5  GLfloat ymove,register __a0
				const GLubyte *bitmap );

__asm __saveds extern void glReadPixels(register __d0 GLint x,register __d1  GLint y,register __d2  GLsizei width,register __d3	GLsizei height,register __d4
			  GLenum format,register __d5  GLenum type,register __a0  GLvoid *pixels );

__asm __saveds extern void glDrawPixels(register __d0 GLsizei width,register __d1  GLsizei height,register __d2
			  GLenum format,register __d3  GLenum type,register __a0  const GLvoid *pixels );

__asm __saveds extern void glCopyPixels(register __d0 GLint x,register __d1  GLint y,register __d2  GLsizei width,register __d3	GLsizei height,register __d4
			  GLenum type );



/*
 * Stenciling
 */

__asm __saveds extern void glStencilFunc(register __d0 GLenum func,register __d1  GLint ref,register __d2  GLuint mask );
__asm __saveds extern void glStencilMask(register __d0 GLuint mask );
__asm __saveds extern void glStencilOp(register __d0 GLenum fail,register __d1  GLenum zfail,register __d2	GLenum zpass );
__asm __saveds extern void glClearStencil(register __d0 GLint s );



/*
 * Texture mapping
 */

__asm __saveds extern void glTexGend(register __d0 GLenum coord,register __d1  GLenum pname,register __d2  GLdouble param );
__asm __saveds extern void glTexGenf(register __d0 GLenum coord,register __d1  GLenum pname,register __d2  GLfloat param );
__asm __saveds extern void glTexGeni(register __d0 GLenum coord,register __d1  GLenum pname,register __d2  GLint param );

__asm __saveds extern void glTexGendv(register __d0 GLenum coord,register __d1  GLenum pname,register __a0	const GLdouble *params );
__asm __saveds extern void glTexGenfv(register __d0 GLenum coord,register __d1  GLenum pname,register __a0	const GLfloat *params );
__asm __saveds extern void glTexGeniv(register __d0 GLenum coord,register __d1  GLenum pname,register __a0	const GLint *params );

__asm __saveds extern void glGetTexGendv(register __d0 GLenum coord,register __d1  GLenum pname,register __a0	GLdouble *params );
__asm __saveds extern void glGetTexGenfv(register __d0 GLenum coord,register __d1  GLenum pname,register __a0	GLfloat *params );
__asm __saveds extern void glGetTexGeniv(register __d0 GLenum coord,register __d1  GLenum pname,register __a0	GLint *params );


__asm __saveds extern void glTexEnvf(register __d0 GLenum target,register __d1  GLenum pname,register __d2	GLfloat param );
__asm __saveds extern void glTexEnvi(register __d0 GLenum target,register __d1  GLenum pname,register __d2	GLint param );

__asm __saveds extern void glTexEnvfv(register __d0 GLenum target,register __d1	GLenum pname,register __a0  const GLfloat *params );
__asm __saveds extern void glTexEnviv(register __d0 GLenum target,register __d1	GLenum pname,register __a0  const GLint *params );

__asm __saveds extern void glGetTexEnvfv(register __d0 GLenum target,register __d1	GLenum pname,register __a0  GLfloat *params );
__asm __saveds extern void glGetTexEnviv(register __d0 GLenum target,register __d1	GLenum pname,register __a0  GLint *params );


__asm __saveds extern void glTexParameterf(register __d0 GLenum target,register __d1  GLenum pname,register __d2	GLfloat param );
__asm __saveds extern void glTexParameteri(register __d0 GLenum target,register __d1  GLenum pname,register __d2	GLint param );

__asm __saveds extern void glTexParameterfv(register __d0 GLenum target,register __d1	GLenum pname,register __a0
					const GLfloat *params );
__asm __saveds extern void glTexParameteriv(register __d0 GLenum target,register __d1	GLenum pname,register __a0
					const GLint *params );

__asm __saveds extern void glGetTexParameterfv(register __d0 GLenum target,register __d1	GLenum pname,register __a0  GLfloat *params);
__asm __saveds extern void glGetTexParameteriv(register __d0 GLenum target,register __d1	GLenum pname,register __a0  GLint *params );

__asm __saveds extern void glGetTexLevelParameterfv(register __d0 GLenum target,register __d1  GLint level,register __d2
						GLenum pname,register __a0  GLfloat *params );
__asm __saveds extern void glGetTexLevelParameteriv(register __d0 GLenum target,register __d1  GLint level,register __d2
						GLenum pname,register __a0  GLint *params );


__asm __saveds extern void glTexImage1D(register __d0 GLenum target,register __d1  GLint level,register __d2  GLint components,register __d3
			  GLsizei width,register __d4  GLint border,register __d5
			  GLenum format,register __d6  GLenum type,register __a0  const GLvoid *pixels );

__asm __saveds extern void glTexImage2D(register __d0 GLenum target,register __d1  GLint level,register __d2 GLint components,register __d3
			  GLsizei width,register __d4  GLsizei height,register __d5  GLint border,register __d6
			  GLenum format,register __d7  GLenum type,register __a0  const GLvoid *pixels );

__asm __saveds extern void glGetTexImage(register __d0 GLenum target,register __d1	GLint level,register __d2	GLenum format,register __d3
				GLenum type,register __a0	GLvoid *pixels );



/*
 * Evaluators
 */

__asm __saveds extern void glMap1d(register __d0 GLenum target,register __d1	GLdouble u1,register __d2	GLdouble u2,register __d3	GLint stride,register __d4
			  GLint order,register __a0  const GLdouble *points );
__asm __saveds extern void glMap1f(register __d0 GLenum target,register __d1	GLfloat u1,register __d2  GLfloat u2,register __d3  GLint stride,register __d4
			  GLint order,register __a0  const GLfloat *points );

__asm __saveds extern void glMap2d(register __d0 GLenum target,register __d1
			  GLdouble u1,register __d2  GLdouble u2,register __d3  GLint ustride,register __d4  GLint uorder,register __d5
			  GLdouble v1,register __d6  GLdouble v2,register __d7  GLint vstride,register __a1  GLint vorder,register __a0
			  const GLdouble *points );
			  
			  
			  
__asm __saveds extern void glMap2f(register __d0 GLenum target,register __d1
			  GLfloat u1,register __d2  GLfloat u2,register __d3	GLint ustride,register __d4  GLint uorder,register __d5
			  GLfloat v1,register __d6  GLfloat v2,register __d7	GLint vstride,register __a1  GLint vorder,register __a0
			  const GLfloat *points );





__asm __saveds extern void glGetMapdv(register __d0 GLenum target,register __d1	GLenum query,register __a0  GLdouble *v );
__asm __saveds extern void glGetMapfv(register __d0 GLenum target,register __d1	GLenum query,register __a0  GLfloat *v );
__asm __saveds extern void glGetMapiv(register __d0 GLenum target,register __d1	GLenum query,register __a0  GLint *v );

__asm __saveds extern void glEvalCoord1d(register __d0 GLdouble u );
__asm __saveds extern void glEvalCoord1f(register __d0 GLfloat u );

__asm __saveds extern void glEvalCoord1dv(register __a0 const GLdouble *u );
__asm __saveds extern void glEvalCoord1fv(register __a0 const GLfloat *u );

__asm __saveds extern void glEvalCoord2d(register __d0 GLdouble u,register __d1	GLdouble v );
__asm __saveds extern void glEvalCoord2f(register __d0 GLfloat u,register __d1  GLfloat v );

__asm __saveds extern void glEvalCoord2dv(register __a0 const GLdouble *u );
__asm __saveds extern void glEvalCoord2fv(register __a0 const GLfloat *u );

__asm __saveds extern void glMapGrid1d(register __d0 GLint un,register __d1  GLdouble u1,register __d2  GLdouble u2 );
__asm __saveds extern void glMapGrid1f(register __d0 GLint un,register __d1  GLfloat u1,register __d2  GLfloat u2 );

__asm __saveds extern void glMapGrid2d(register __d0 GLint un,register __d1  GLdouble u1,register __d2  GLdouble u2,register __d3
			 GLint vn,register __d4  GLdouble v1,register __d5  GLdouble v2 );
__asm __saveds extern void glMapGrid2f(register __d0 GLint un,register __d1  GLfloat u1,register __d2  GLfloat u2,register __d3
			 GLint vn,register __d4  GLfloat v1,register __d5	GLfloat v2 );

__asm __saveds extern void glEvalPoint1(register __d0 GLint i );

__asm __saveds extern void glEvalPoint2(register __d0 GLint i,register __d1  GLint j );

__asm __saveds extern void glEvalMesh1(register __d0 GLenum mode,register __d1  GLint i1,register __d2  GLint i2 );

__asm __saveds extern void glEvalMesh2(register __d0 GLenum mode,register __d1  GLint i1,register __d2  GLint i2,register __d3  GLint j1,register __d4  GLint j2 );



/*
 * Fog
 */

__asm __saveds extern void glFogf(register __d0 GLenum pnameR,register __d1  GLfloat paramR );

__asm __saveds extern void glFogi(register __d0 GLenum pname,register __d1  GLint param );

__asm __saveds extern void glFogfv(register __d0 GLenum pname,register __a0  const GLfloat *params );

__asm __saveds extern void glFogiv(register __d0 GLenum pname,register __a0  const GLint *params );



/*
 * Selection and Feedback
 */

__asm __saveds extern void glFeedbackBuffer(register __d0 GLsizei size,register __d1  GLenum type,register __a0  GLfloat *buffer );

__asm __saveds extern void glPassThrough(register __d0 GLfloat token );

__asm __saveds extern void glSelectBuffer(register __d0 GLsizei size,register __d1	GLuint *buffer );

__asm __saveds extern void glInitNames( void );

__asm __saveds extern void glLoadName(register __d0 GLuint name );

__asm __saveds extern void glPushName(register __d0 GLuint name );

__asm __saveds extern void glPopName( void );



/*
 * Extensions
 */

__asm __saveds extern void glBlendEquationEXT(register __d0 GLenum mode );

__asm __saveds extern void glBlendColorEXT(register __d0 GLclampf red,register __d1  GLclampf green,register __d2
				  GLclampf blue,register __d3  GLclampf alpha );


__asm __saveds extern void glPolygonOffsetEXT(register __d0 GLfloat factor,register __d1	GLfloat bias );


__asm __saveds extern void glVertexPointerEXT(register __d0 GLint size,register __d1  GLenum type,register __d2  GLsizei stride,register __d3
											GLsizei count,register __a0  const void *ptr );

__asm __saveds extern void glNormalPointerEXT(register __d0 GLenum type,register __d1	GLsizei stride,register __d2
											GLsizei count,register __a0  const void *ptr );

__asm __saveds extern void glColorPointerEXT(register __d0 GLint size,register __d1  GLenum type,register __d2  GLsizei stride,register __d3
										 GLsizei count,register __a0	const void *ptr );

__asm __saveds extern void glIndexPointerEXT(register __d0 GLenum type,register __d1  GLsizei stride,register __d2  GLsizei count,register __a0
										 const void *ptr );
__asm __saveds extern void glTexCoordPointerEXT(register __d0 GLint size,register __d1  GLenum type,register __d2  GLsizei stride,register __d3
											 GLsizei count,register __a0	const void *ptr );
__asm __saveds extern void glEdgeFlagPointerEXT(register __d0 GLsizei stride,register __d1  GLsizei count,register __a0 const GLboolean *ptr );
__asm __saveds extern void glGetPointervEXT(register __d0 GLenum pname,register __a0  void **params );
__asm __saveds extern void glArrayElementEXT(register __d0 GLint i );
__asm __saveds extern void glDrawArraysEXT(register __d0 GLenum mode,register __d1	GLint first,register __d2	GLsizei count );



#ifdef AMIGALIB

#else
#ifdef __AROS__

#else
extern struct Library *glBase;
#include "gl_pragma.h"
#endif
#endif




/*
 * Compile-time tests for extensions:
 */
#define GL_EXT_blend_color 1
#define GL_EXT_blend_logic_op 1
#define GL_EXT_blend_minmax	1
#define GL_EXT_blend_subtract 1
#define GL_EXT_polygon_offset 1
#define GL_EXT_vertex_array	1



#ifdef __cplusplus
}
#endif

#endif

#endif
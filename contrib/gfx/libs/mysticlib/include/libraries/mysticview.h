#ifndef	MYSTICV_H
#define MYSTICV_H	1
/*
**    $VER: mysticview.h 4.2 (1.6.99)
**
**    mysticview.library definitions
**
**    © 1997-99 TEK neoscientists
*/

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif


/*
 *	Tags
 */

#define	MVIEW_Dummy				(TAG_USER+765432)
#define	MVIEW_DestX				(MVIEW_Dummy+1)			// left edge in rastport
#define	MVIEW_DestY				(MVIEW_Dummy+2)			// top edge in rastport
#define	MVIEW_DestWidth			(MVIEW_Dummy+3)			// width in rastport
#define	MVIEW_DestHeight		(MVIEW_Dummy+4)			// height in rastport
#define	MVIEW_BackColor			(MVIEW_Dummy+5)			// background color
#define	MVIEW_Precision			(MVIEW_Dummy+6)			// OBP_Precision (view.h)
#define	MVIEW_DisplayMode		(MVIEW_Dummy+7)			// aspect mode - see definitions below
#define	MVIEW_PreviewMode		(MVIEW_Dummy+8)			// preview mode - see definitions below
#define	MVIEW_Picture			(MVIEW_Dummy+9)			// a guigfx.library picture
#define	MVIEW_Text				(MVIEW_Dummy+10)		// simple text line
#define	MVIEW_StaticPalette		(MVIEW_Dummy+11)		// static palette
#define	MVIEW_Dither			(MVIEW_Dummy+12)		// dither activation mode (see below)
#define MVIEW_HSType			(MVIEW_Dummy+13)		// histogram type
#define MVIEW_ScreenAspectX		(MVIEW_Dummy+14)		// screen aspect horizontal
#define MVIEW_ScreenAspectY		(MVIEW_Dummy+15)		// screen aspect vertical
#define MVIEW_DitherMode		(MVIEW_Dummy+16)		// dither mode
#define MVIEW_ShowCursor		(MVIEW_Dummy+17)		// display cursor
#define	MVIEW_Zoom				(MVIEW_Dummy+18)		// zoom factor (0.1 ... 10)
#define	MVIEW_XPos				(MVIEW_Dummy+19)		// x position (0 ... 1)
#define	MVIEW_YPos				(MVIEW_Dummy+20)		// y position (0 ... 1)
#define	MVIEW_Rotation			(MVIEW_Dummy+21)		// rotation (0 ... 1)
#define MVIEW_AutoDither		(MVIEW_Dummy+22)		// do not use
#define MVIEW_ReadySignal		(MVIEW_Dummy+23)		// picture fully drawn
#define MVIEW_PictureX			(MVIEW_Dummy+24)		// picture X inside the rastport
#define MVIEW_PictureY			(MVIEW_Dummy+25)		// picture Y inside the rastport
#define MVIEW_PictureWidth		(MVIEW_Dummy+26)		// picture Width inside the rastport
#define MVIEW_PictureHeight		(MVIEW_Dummy+27)		// picture Height inside the rastport
#define MVIEW_DrawArrows		(MVIEW_Dummy+28)		// indicate scrollability
#define MVIEW_ShowPip			(MVIEW_Dummy+29)		// show PIP layer
#define	MVIEW_TextColor			(MVIEW_Dummy+30)		// text/grid color
#define	MVIEW_MarkColor			(MVIEW_Dummy+31)		// color for pip-border, cursor, arrows...
#define	MVIEW_RPSemaphore		(MVIEW_Dummy+32)		// rastport semaphore (MV_Create() only)
#define	MVIEW_Priority			(MVIEW_Dummy+33)		// set task priority (MV_Create() only)


/*
 *	Types
 */

#define	MVDISPMODE_FIT					0		// image fits exactly into view
#define	MVDISPMODE_KEEPASPECT_MIN		1		// image is fully visible
#define	MVDISPMODE_KEEPASPECT_MAX		2		// width or height is fully visible
#define	MVDISPMODE_ONEPIXEL				3		// the image aspect is ignored
#define	MVDISPMODE_IGNOREASPECT			4		// aspect ratios are ignored

#define	MVPREVMODE_NONE					0		// no realtime refresh
#define	MVPREVMODE_GRID					1		// grid realtime refresh
#define	MVPREVMODE_OPAQUE				2		// opaque realtime refresh

#define	MVDITHERMODE_ON					0		// dithering on
#define	MVDITHERMODE_OFF				1		// dithering off
#define	MVDITHERMODE_AUTO				2		// auto dithering 

#endif

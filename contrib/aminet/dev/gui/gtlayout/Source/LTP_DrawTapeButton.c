/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include "Assert.h"

#ifdef DO_TAPEDECK_KIND	/* Support code */

VOID
LTP_DrawTapeButton(struct RastPort *RPort,ImageInfo *imageInfo,LONG Left,LONG Top,LONG Width,LONG Height,LONG AspectX,LONG AspectY,LONG Background)
{
	LONG	SizeX,SizeY,Mid,Size;
	LONG	ImageType;

	ImageType	= imageInfo -> ImageType;
	SizeX		= imageInfo -> GlyphWidth;
	SizeY		= imageInfo -> GlyphHeight;

	Left	+= 6;
	Top		+= 3;

	Width	-= 2 * 6;
	Height	-= 2 * 3;

	if(!SizeY)
		SizeY = Height;

	if(!SizeX)
	{
		SizeX = (SizeY * AspectY) / AspectX;

		if(ImageType == IMAGECLASS_BACKWARD || ImageType == IMAGECLASS_FORWARD)
			SizeX *= 2;
	}

	if(SizeX > 0 && SizeY > 0)
	{
		switch(ImageType)
		{
			case IMAGECLASS_BACKWARD:
			case IMAGECLASS_FORWARD:

				Left	= Left + (Width - SizeX) / 2;
				Top		= Top + (Height - SizeY) / 2;

				LTP_DrawBackFore(RPort,ImageType == IMAGECLASS_BACKWARD,Left,Top,SizeX,SizeY);
				break;

			case IMAGECLASS_PREVIOUS:
			case IMAGECLASS_NEXT:

				Left	= Left + (Width - SizeX) / 2;
				Top		= Top + (Height - SizeY) / 2;

				LTP_DrawPrevNext(RPort,ImageType == IMAGECLASS_PREVIOUS,Left,Top,SizeX,SizeY);
				break;

			case IMAGECLASS_EJECT:

				Left	= Left + (Width - SizeX) / 2;
				Top		= Top + (Height - SizeY) / 2;

				LTP_DrawPicker(RPort,TRUE,Left,Top,SizeX,SizeY);
				break;

			case IMAGECLASS_RECORD:

				LTP_RenderCircle(RPort,Left + Width / 2,Top + Height / 2,SizeY / 2,AspectX,AspectY);
				break;

			case IMAGECLASS_PLAY:
			case IMAGECLASS_REWIND:

				Left	= Left + (Width - SizeX) / 2;
				Top		= Top + (Height - SizeY) / 2;

				LTP_RenderArrow(RPort,ImageType == IMAGECLASS_REWIND,Left,Top,SizeX,SizeY);
				break;

			case IMAGECLASS_STOP:

				Left	= Left + (Width - SizeX) / 2;
				Top		= Top + (Height - SizeY) / 2;

				LTP_FillBox(RPort,Left,Top,SizeX,SizeY);
				break;

			case IMAGECLASS_PAUSE:

				Left	= Left + (Width - SizeX) / 2;
				Top		= Top + (Height - SizeY) / 2;

				LTP_FillBox(RPort,Left,Top,SizeX,SizeY);

				Size	= SizeX - (2 * SizeX) / 3;
				Mid		= SizeX / 3;

				LTP_SetAPen(RPort,Background);
				LTP_FillBox(RPort,Left + Mid,Top,Size,SizeY);

				break;
		}
	}
}

#endif	/* DO_TAPEDECK_KIND */

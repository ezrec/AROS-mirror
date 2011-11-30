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


/*****************************************************************************/


STATIC VOID
LTP_GetSizeDimensions(LayoutHandle *Handle,IPTR *SizeWidth,IPTR *SizeHeight)
{
	Object	*SizeImage;
	LONG	 SizeType;

	if(Handle->Screen->Flags & SCREENHIRES)
	{
		if(SizeWidth)
			*SizeWidth = 18;

		if(SizeHeight)
			*SizeHeight = 10;

		SizeType = SYSISIZE_MEDRES;
	}
	else
	{
		if(SizeWidth)
			*SizeWidth = 13;

		if(SizeHeight)
			*SizeHeight = 11;

		SizeType = SYSISIZE_LOWRES;
	}

	if(SizeImage = NewObject(NULL,SYSICLASS,
		SYSIA_Size,		SizeType,
		SYSIA_Which,	SIZEIMAGE,
		SYSIA_DrawInfo,	Handle->DrawInfo,
	TAG_DONE))
	{
		if(SizeWidth)
			GetAttr(IA_Width,SizeImage,SizeWidth);

		if(SizeHeight)
			GetAttr(IA_Height,SizeImage,SizeHeight);

		DisposeObject(SizeImage);
	}
}


/*****************************************************************************/


ULONG
LTP_GetSizeWidth(struct LayoutHandle *handle)
{
	IPTR SizeWidth;

	LTP_GetSizeDimensions(handle,&SizeWidth,NULL);

	if(SizeWidth < handle->Screen->WBorRight)
		return(handle->Screen->WBorRight);
	else
		return(SizeWidth);
}


/*****************************************************************************/


ULONG
LTP_GetSizeHeight(struct LayoutHandle *handle)
{
	IPTR SizeHeight;

	LTP_GetSizeDimensions(handle,NULL,&SizeHeight);

	if(SizeHeight < handle->Screen->WBorBottom)
		return(handle->Screen->WBorBottom);
	else
		return(SizeHeight);
}

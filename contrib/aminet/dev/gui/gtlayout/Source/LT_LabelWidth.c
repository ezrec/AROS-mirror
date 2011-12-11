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

/****** gtlayout.library/LT_LabelWidth ******************************************
*
*   NAME
*	LT_LabelWidth -- Calculate the width of a string according
*	                 to the user interface font used.
*
*   SYNOPSIS
*	Width = LT_LabelWidth(Handle,Label);
*	  D0                    A0    A1
*
*	LONG LT_LabelWidth(LayoutHandle *,CONST_STRPTR);
*
*   FUNCTION
*	This routine calculates the width of strings in
*	terms of pixels according to the user interface
*	font associated with the LayoutHandle.
*
*	You can pass multi-line label texts to this routine,
*	it will calculate the maximum length of the single
*	lines. (V12)
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure.
*
*   RESULT
*	Width - Width of the text string in pixels.
*
******************************************************************************
*
*/

LONG LIBENT
LT_LabelWidth(REG(a0) LayoutHandle *handle,REG(a1) CONST_STRPTR label)
{
	if(handle)
	{
		struct RastPort *rp = &handle->RPort;
		LONG start;
		LONG len;
		LONG cnt;
		LONG width,maxWidth;
		LONG underscore;

		start = 0;
		maxWidth = 0;
		underscore = 0;

		cnt = 0;
		len = 0;

		while(label[len])
		{
			if(label[len] == '_')
			{
				if(!underscore)
					underscore = TextLength(rp,"_",1);

				cnt += underscore;
			}

			if(label[len] == '\n')
			{
				if(len > start)
					width = TextLength(rp,&label[start],len - start) - cnt;
				else
					width = 0;

				cnt = 0;
				start = len + 1;

				if(width > maxWidth)
					maxWidth = width;
			}

			len++;
		}

		if(len > start)
			width = TextLength(rp,&label[start],len - start) - cnt;
		else
			width = 0;

		if(width > maxWidth)
			maxWidth = width;

		return(maxWidth);
	}
	else
		return(0);
}


/*****************************************************************************/


/****** gtlayout.library/LT_LabelChars ******************************************
*
*   NAME
*	LT_LabelChars -- Calculate the width of a a string
*	                 according to the user interface font
*	                 associated with a LayoutHandle.
*
*   SYNOPSIS
*	Length = LT_LabelChars(Handle,Label);
*	  D0                     A0    A1
*
*	LONG LT_LabelChars(struct LayoutHandle *,STRPTR);
*
*   FUNCTION
*	Calculates the width of a string according to the user
*	interface font used. The width is then converted to a number
*	of characters suitable for using with the LA_Chars tag
*	item when calling LT_New().
*
*	You can pass multi-line label texts to this routine,
*	it will calculate the maximum length of the single
*	lines. (V12)
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure.
*
*	Label - Pointer to string.
*
*   RESULT
*	Length - Number of characters that are considered
*	    equivalent to the string length in pixels.
*
******************************************************************************
*
*/

LONG LIBENT
LT_LabelChars(REG(a0) LayoutHandle *handle,REG(a1) CONST_STRPTR label)
{
	if(handle)
		return((LT_LabelWidth(handle,label) + handle->GlyphWidth - 1) / handle->GlyphWidth);
	else
		return(0);
}

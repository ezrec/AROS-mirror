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


STATIC LONG __stdargs
DefaultDispFunc(struct Gadget *gad,LONG value)
{
	return(value);
}


/*****************************************************************************/


VOID
LTP_LevelWidth(LayoutHandle *handle,STRPTR levelFormat,DISPFUNC dispFunc,LONG min,LONG max,LONG *maxWidth,LONG *maxLen,BOOL fullCheck)
{
	UBYTE buffer[80];
	LONG localMaxWidth;
	LONG localMaxLen;
	LONG len;

	localMaxWidth	= 0;
	localMaxLen	= 0;

	if(handle)
	{
		if(!dispFunc)
			dispFunc = (DISPFUNC)DefaultDispFunc;

		if(levelFormat == NULL)
			levelFormat = LTP_NumberFormat;	/* either %ld or %lD, according to locale */

		if(fullCheck)
		{
			LONG i;

			for(i = min ; i <= max ; i++)
			{
				SPrintf(buffer,levelFormat,(LONG)(*dispFunc)(NULL,i));

				len = strlen(buffer);

				if(len > localMaxLen)
					localMaxLen = len;

				len = TextLength(&handle->RPort,buffer,len);

				if(len > localMaxWidth)
					localMaxWidth = len;
			}
		}
		else
		{
			SPrintf(buffer,levelFormat,(LONG)(*dispFunc)(NULL,min));

			len = strlen(buffer);

			if(len > localMaxLen)
				localMaxLen = len;

			len = TextLength(&handle->RPort,buffer,len);

			if(len > localMaxWidth)
				localMaxWidth = len;

			SPrintf(buffer,levelFormat,(LONG)(*dispFunc)(NULL,max));

			len = strlen(buffer);

			if(len > localMaxLen)
				localMaxLen = len;

			len = TextLength(&handle->RPort,buffer,len);

			if(len > localMaxWidth)
				localMaxWidth = len;
		}
	}

	if(maxLen)
		*maxLen = localMaxLen;

	if(maxWidth)
		*maxWidth = localMaxWidth + handle->GlyphWidth;
}


/*****************************************************************************/


/****** gtlayout.library/LT_LevelWidth ******************************************
*
*   NAME
*	LT_LevelWidth -- Determine the maximum width of a SLIDER_KIND
*	                 level string.
*
*   SYNOPSIS
*	Index = LT_LevelWidth(Handle,FormatString,DispFunc,Min,Max,MaxWidth,
*	  D0                    A0        A1         A2     D0  D1    A3
*
*	                          MaxLen,FullCheck);
*	                            A5      D2
*
*	LONG LT_LevelWidth(LayoutHandle *,STRPTR,
*	                   LONG (*)(struct Gadget *,WORD),LONG,LONG,LONG *,
*	                   LONG *,BOOL);
*
*   FUNCTION
*	In order to make room for the level text displayed by a
*	SLIDER_KIND object one needs to know how much space the
*	longest level string will occupy. Otherwise, the level
*	text may overwrite the gadget label text or the slider
*	container. This routine will rattle through all possible
*	slider settings (as given via the Min and the Max
*	level values) and determine the longest label string
*	according to the font used.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure, as returned by
*	    a call to LT_CreateHandleTags().
*
*	FormatString - The sprintf() style formatting string to be used
*	    to format the slider level settings into text.
*	    This is the same string you would pass in via
*	    the GTSL_LevelFormat tag when creating the
*	    slider object.
*	    Default: "%lD" for systems which have locale.library
*	        installed, "%ld" otherwise.
*
*	DispFunc - A pointer to the function to filter the slider level
*	    values. The result of this function will then be
*	    used to format a string into the slider level text.
*	    This is the same parameter you would pass in via
*	    the GTSL_DispFunc tag when creating the slider
*	    object.
*
*	        NOTE: the routine will be called with a NULL Gadget
*	            parameter, make sure your code will handle
*	            this nicely.
*
*	    Default: no display function
*
*	Min - The smallest value the slider can be set to. This is
*	    same value you would pass in via GTSL_Min when creating
*	    the slider object.
*
*	Max - The largest value the slider can be set to. This is
*	    same value you would pass in via GTSL_Max when creating
*	    the slider object.
*
*	MaxWidth - Pointer to a place to store the width of the
*	    longest level string in pixels. If you pass
*	    in NULL instead of the address of a variable
*	    no harm will be done.
*
*	MaxLen - Pointer to a place to store the length of the
*	    longest level string in characters. If you pass
*	    in NULL instead of the address of a variable
*	    no harm will be done.
*
*	FullCheck - TRUE will cause the code to rattle through all
*	    possible slider settings, starting from the
*	    minimum value, ending at the maximum value.
*	    While this may be a good idea for a display
*	    function to map slider levels to text strings
*	    of varying length it might be a problem when
*	    it comes to display a range of numbers from
*	    1 to 40,000: the code will loop through
*	    40,000 iterations trying to find the longest
*	    string.
*
*	    FALSE will cause the code to calculate the
*	    longest level string based only on the
*	    minimum and the maximum value to check.
*	    While this is certainly a good a idea when
*	    it comes to display a range of numbers from
*	    1 to 40,000 as only two values will be
*	    checked the code may fail to produce
*	    accurate results for sliders using display
*	    functions mapping slider levels to strings.
*
*   RESULT
*	Index - The slider level which gives the longest
*	    level string.
*
*   NOTES
*	Some compilers have trouble passing parameters in A5. In such
*	a case it is recommended to use gtlayout.library/LT_NewLevelWidth
*	instead.
*
*   SEE ALSO
*	gtlayout.library/LT_NewLevelWidth
*
******************************************************************************
*
*/

VOID LIBENT
LT_LevelWidth(REG(a0) LayoutHandle *handle,REG(a1) STRPTR levelFormat,REG(a2) DISPFUNC dispFunc,REG(d0) LONG min,REG(d1) LONG max,REG(a3) LONG *maxWidth,REG(a5) LONG *maxLen,REG(d2) BOOL fullCheck)
{
	LTP_LevelWidth(handle,levelFormat,dispFunc,min,max,maxWidth,maxLen,fullCheck);
}

/****** gtlayout.library/LT_NewLevelWidth ******************************************
*
*   NAME
*	LT_NewLevelWidth -- Determine the maximum width of a SLIDER_KIND
*	                    level string. (V14)
*
*   SYNOPSIS
*	Index = LT_LevelWidth(Handle,FormatString,DispFunc,Min,Max,MaxWidth,
*	  D0                    A0        A1         A2     D0  D1    A3
*
*	                          MaxLen,FullCheck);
*	                            D3      D2
*
*	LONG LT_LevelWidth(LayoutHandle *,STRPTR,
*	                   LONG (*)(struct Gadget *,WORD),LONG,LONG,LONG *,
*	                   LONG *,BOOL);
*
*   FUNCTION
*	In order to make room for the level text displayed by a
*	SLIDER_KIND object one needs to know how much space the
*	longest level string will occupy. Otherwise, the level
*	text may overwrite the gadget label text or the slider
*	container. This routine will rattle through all possible
*	slider settings (as given via the Min and the Max
*	level values) and determine the longest label string
*	according to the font used.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure, as returned by
*	    a call to LT_CreateHandleTags().
*
*	FormatString - The sprintf() style formatting string to be used
*	    to format the slider level settings into text.
*	    This is the same string you would pass in via
*	    the GTSL_LevelFormat tag when creating the
*	    slider object.
*	    Default: "%lD" for systems which have locale.library
*	        installed, "%ld" otherwise.
*
*	DispFunc - A pointer to the function to filter the slider level
*	    values. The result of this function will then be
*	    used to format a string into the slider level text.
*	    This is the same parameter you would pass in via
*	    the GTSL_DispFunc tag when creating the slider
*	    object.
*
*	    NOTE: the routine will be called with a NULL Gadget
*	        parameter, make sure your code will handle
*	        this nicely.
*
*	    Default: no display function
*
*	Min - The smallest value the slider can be set to. This is
*	    same value you would pass in via GTSL_Min when creating
*	    the slider object.
*
*	Max - The largest value the slider can be set to. This is
*	    same value you would pass in via GTSL_Max when creating
*	    the slider object.
*
*	MaxWidth - Pointer to a place to store the width of the
*	    longest level string in pixels. If you pass
*	    in NULL instead of the address of a variable
*	    no harm will be done.
*
*	MaxLen - Pointer to a place to store the length of the
*	    longest level string in characters. If you pass
*	    in NULL instead of the address of a variable
*	    no harm will be done.
*
*	FullCheck - TRUE will cause the code to rattle through all
*	    possible slider settings, starting from the
*	    minimum value, ending at the maximum value.
*	    While this may be a good idea for a display
*	    function to map slider levels to text strings
*	    of varying length it might be a problem when
*	    it comes to display a range of numbers from
*	    1 to 40,000: the code will loop through
*	    40,000 iterations trying to find the longest
*	    string.
*
*	    FALSE will cause the code to calculate the
*	    longest level string based only on the
*	    minimum and the maximum value to check.
*	    While this is certainly a good a idea when
*	    it comes to display a range of numbers from
*	    1 to 40,000 as only two values will be
*	    checked the code may fail to produce
*	    accurate results for sliders using display
*	    functions mapping slider levels to strings.
*
*   RESULT
*	Index - The slider level which gives the longest
*	    level string.
*
*   NOTES
*	This function does exactly what gtlayout.library/LT_LevelWidth
*	does, but uses a slightly different register ordering. Namely,
*	the MaxLen pointer is passed in D3 instead of A5.
*
*   SEE ALSO
*	gtlayout.library/LT_LevelWidth
*
******************************************************************************
*
*/

VOID LIBENT
LT_NewLevelWidth(REG(a0) LayoutHandle *handle,REG(a1) STRPTR levelFormat,REG(a2) DISPFUNC dispFunc,REG(d0) LONG min,REG(d1) LONG max,REG(a3) LONG *maxWidth,REG(d3) LONG *maxLen,REG(d2) BOOL fullCheck)
{
	LTP_LevelWidth(handle,levelFormat,dispFunc,min,max,maxWidth,maxLen,fullCheck);
}

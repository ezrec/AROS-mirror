/*
**	Boxes.c
**
**	Text box management support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Sizing data. */

STATIC struct RastPort	*SZ_RPort;
STATIC struct TextFont	*SZ_TextFont;
STATIC struct Screen	*SZ_Screen;

STATIC LONG				 SZ_Left,
						 SZ_Top,
						 SZ_CurrentLeft,
						 SZ_CurrentTop,
						 SZ_CurrentWidth,
						 SZ_MaxWidth,
						 SZ_TextPen,
						 SZ_BackPen,
						 SZ_AverageGlyphWidth;

	/* SZ_SetTopEdge(LONG Top):
	 *
	 *	Set the current object top edge.
	 */

VOID
SZ_SetTopEdge(LONG Top)
{
	SZ_CurrentTop = Top;
}

	/* SZ_SetLeftEdge(LONG Left):
	 *
	 *	Set the current object left edge.
	 */

VOID
SZ_SetLeftEdge(LONG Left)
{
	SZ_CurrentLeft = Left;
}

	/* SZ_SetAbsoluteTop(LONG Top):
	 *
	 *	Set new inner window top edge.
	 */

VOID
SZ_SetAbsoluteTop(LONG Top)
{
	SZ_Top = Top;
}

	/* SZ_SetWidth(LONG Width):
	 *
	 *	Set current object width.
	 */

VOID
SZ_SetWidth(LONG Width)
{
	SZ_CurrentWidth = Width;
}

	/* SZ_AddLeftOffset(LONG Offset):
	 *
	 *	Update current object left offset.
	 */

VOID
SZ_AddLeftOffset(LONG Offset)
{
	SZ_CurrentLeft += Offset;
}

	/* SZ_LeftOffsetN(LONG DataArray,...):
	 *
	 *	Determine the maximum length of a number of
	 *	gadget labels (first, second, third item, -1 terminates
	 *	the array).
	 */

LONG
SZ_LeftOffsetN(LONG DataArray,...)
{
	LONG	*Data = &DataArray,
			 Len,
			 Max = 0;
	CONST_STRPTR	 String;

	while(*Data != -1)
	{
		String = LocaleString(*Data++);

		if((Len = TextLength(SZ_RPort,String,strlen(String))) > Max)
			Max = Len;
	}

	return(Max + INTERWIDTH);
}

	/* SZ_SizeCleanup():
	 *
	 *	Free data allocated by SZ_SizeSetup().
	 */

VOID
SZ_SizeCleanup()
{
	if(SZ_TextFont)
	{
		CloseFont(SZ_TextFont);

		SZ_TextFont = NULL;
	}

	FreeVecPooled(SZ_RPort);
	SZ_RPort = NULL;
}

	/* SZ_SizeSetup(struct Screen *Screen,struct TextAttr *TextAttr):
	 *
	 *	Perform setups for gadget creation.
	 */

BOOL
SZ_SizeSetup(struct Screen *Screen,struct TextAttr *TextAttr)
{
	SZ_SizeCleanup();

	if(Screen)
	{
		struct DrawInfo *DrawInfo;

		if(DrawInfo = GetScreenDrawInfo(Screen))
		{
			SZ_TextPen = DrawInfo->dri_Pens[TEXTPEN];
			SZ_BackPen = DrawInfo->dri_Pens[BACKGROUNDPEN];

			FreeScreenDrawInfo(Screen,DrawInfo);
		}
	}
	else
	{
		if(!TextAttr)
			return(FALSE);
	}

	SZ_Screen = Screen;

	if(SZ_RPort = (struct RastPort *)AllocVecPooled(sizeof(struct RastPort),MEMF_ANY | MEMF_CLEAR))
	{
		InitRastPort(SZ_RPort);

		if(!TextAttr)
			TextAttr = Screen->Font;

		if(SZ_TextFont = SmartOpenDiskFont(TextAttr))
		{
			LONG	i,Width,Counted = 0,NumericWidth = 0;
			UBYTE	Char;

			InterWidth = INTERWIDTH;

			if(SZ_TextFont->tf_YSize <= 8)
				InterHeight = 1;
			else
				InterHeight = SZ_TextFont->tf_YSize / 4;

			SetFont(SZ_RPort,SZ_TextFont);

			SZ_AverageGlyphWidth = 0;

			for(i = 32 ; i < 127 ; i++)
			{
				Char = i;

				Width = TextLength(SZ_RPort,&Char,1);

					/* For really pathologic fonts... */

				if(Width < 0 || Width > 32767)
					continue;

				SZ_AverageGlyphWidth += Width;

				Counted++;
			}

			for(i = 160 ; i < 256 ; i++)
			{
				Char = i;

				Width = TextLength(SZ_RPort,&Char,1);

					/* For really pathologic fonts... */

				if(Width < 0 || Width > 32767)
					continue;

				SZ_AverageGlyphWidth += Width;

				Counted++;
			}

			for(i = '0' ; i < '9' ; i++)
			{
				Char = i;

				Width = TextLength(SZ_RPort,&Char,1);

				if(Width > NumericWidth)
					NumericWidth = Width;
			}

			Char = ' ';

			Width = TextLength(SZ_RPort,&Char,1);

			if(Width > NumericWidth)
				NumericWidth = Width;

			SZ_AverageGlyphWidth /= Counted;

			if(SZ_AverageGlyphWidth < NumericWidth)
				SZ_AverageGlyphWidth = NumericWidth;

			SZ_Left = InterWidth;

			if(Screen)
				SZ_Top = Screen->WBorTop + Screen->Font->ta_YSize + 1 + InterHeight;
			else
				SZ_Top = 0;

			SZ_CurrentLeft	= InterWidth;
			SZ_CurrentTop	= SZ_Top;

			SZ_CurrentWidth	= 0;
			SZ_MaxWidth	= 0;

			return(TRUE);
		}
	}

	SZ_SizeCleanup();

	return(FALSE);
}

	/* SZ_GetLen(STRPTR String):
	 *
	 *	Get the string pixel width, measured using the average
	 *	glyph width.
	 */

ULONG
SZ_GetLen(STRPTR String)
{
	return(strlen(String) * SZ_AverageGlyphWidth);
}

	/* SZ_FreeBox(struct TextBox *Box):
	 *
	 *	Free a text box.
	 */

STATIC VOID
SZ_FreeBox(struct TextBox *Box)
{
	if(Box)
	{
		if(Box->Text)
		{
			LONG i;

			for(i = 0 ; i < Box->NumLines ; i++)
			{
				if(Box->Text[i])
					FreeVecPooled(Box->Text[i]);
			}

			FreeVecPooled(Box->Text);
		}

		FreeVecPooled(Box->Title);

		FreeVecPooled(Box);
	}
}

	/* SZ_FreeBoxes(struct TextBox *FirstBox):
	 *
	 *	Free a number of text boxes.
	 */

VOID
SZ_FreeBoxes(struct TextBox *FirstBox)
{
	if(FirstBox)
	{
		struct TextBox *NextBox;

		do
		{
			NextBox = FirstBox->NextBox;

			SZ_FreeBox(FirstBox);

			FirstBox = NextBox;
		}
		while(FirstBox);
	}
}

	/* SZ_BoxWidth(LONG Chars):
	 *
	 *	Determine the width of a text box.
	 */

LONG
SZ_BoxWidth(LONG Chars)
{
	return((LONG)(4 + SZ_AverageGlyphWidth * Chars + 4));
}

	/* SZ_BoxHeight(LONG Lines):
	 *
	 *	Determine the height of a text box.
	 */

LONG
SZ_BoxHeight(LONG Lines)
{
	return(2 + SZ_TextFont->tf_YSize * Lines + 2);
}

	/* SZ_CreateTextBox(struct TextBox **FirstBox,...):
	 *
	 *	Create a text box, this routine works similar
	 *	to the CreateGadget() frontend.
	 */

struct TextBox *
SZ_CreateTextBox(struct TextBox **FirstBox, IPTR Tag1, ...)
{
#ifdef __AROS__
	AROS_SLOWSTACKTAGS_PRE_AS(Tag1, struct TextBox *)
#else
	struct TextBox *retval;
#endif
	struct TagItem	*TagList,
					*ThisTag;
	LONG			 Chars,Lines,
					 Width,
					 Height,
					 Left = SZ_CurrentLeft;
	BOOL			 AutoWidth	= FALSE,
					 MoveDown	= TRUE,
					 SetLeft	= FALSE,
					 SetBelow	= FALSE;

	struct TextBox	*Box;
	LONG			 i;

#ifdef __AROS__
	TagList = AROS_SLOWSTACKTAGS_ARG(Tag1);
#else
	TagList = (struct TagItem *)&Tag1;
#endif

	if(ThisTag = FindTagItem(SZ_Lines,TagList))
		Lines = (LONG)ThisTag->ti_Data;
	else
		return(NULL);

	Height = 2 + SZ_TextFont->tf_YSize * Lines + 2;

	if(ThisTag = FindTagItem(SZ_AutoWidth,TagList))
		AutoWidth = ThisTag->ti_Data;

	if(!AutoWidth) {
		retval = NULL;
		goto end;
	} else
		Chars = (SZ_CurrentWidth - 8) / SZ_AverageGlyphWidth;

	if(!(Box = (struct TextBox *)AllocVecPooled(sizeof(struct TextBox),MEMF_ANY | MEMF_CLEAR))) {
		retval = NULL;
		goto end;
	}

	if(ThisTag = FindTagItem(SZ_NewColumn,TagList))
	{
		if(ThisTag->ti_Data)
		{
			SZ_CurrentTop	= SZ_Top;
			Left			= Left + SZ_MaxWidth + InterWidth;

			SZ_MaxWidth	= 0;
		}
	}

	if(ThisTag = FindTagItem(SZ_AutoWidth,TagList))
		AutoWidth = ThisTag->ti_Data;

	if(!AutoWidth)
		Width = SZ_BoxWidth(Chars);
	else
		Width = SZ_CurrentWidth;

	Box->Left		= Left;
	Box->Top		= SZ_CurrentTop;
	Box->Width		= Width;
	Box->Height		= Height;

	Box->LineWidth	= Chars * SZ_AverageGlyphWidth;
	Box->LineHeight	= SZ_TextFont->tf_YSize;

	Box->NumChars	= Chars;
	Box->NumLines	= Lines;

	Box->TitleFgPen	= SZ_TextPen;
	Box->TitleBgPen	= SZ_BackPen;
	Box->TextPen	= SZ_TextPen;

	if(!(Box->Title = (CONST_STRPTR *)AllocVecPooled(sizeof(CONST_STRPTR) * Lines,MEMF_ANY | MEMF_CLEAR)))
	{
		SZ_FreeBox(Box);

		retval = NULL;
		goto end;
	}

	if(!(Box->Text = (STRPTR *)AllocVecPooled(sizeof(STRPTR) * Lines,MEMF_ANY | MEMF_CLEAR)))
	{
		SZ_FreeBox(Box);

		retval = NULL;
		goto end;
	}

	for(i = 0 ; i < Lines ; i++)
	{
		if(!(Box->Text[i] = (STRPTR)AllocVecPooled(Chars + 1,MEMF_ANY | MEMF_CLEAR)))
		{
			SZ_FreeBox(Box);

			retval = NULL;
			goto end;
		}
	}

	if(SetBelow)
		MoveDown = FALSE;

	if(MoveDown)
		SZ_CurrentTop = SZ_CurrentTop + Height + InterHeight;
	else
		SZ_MaxWidth = 0;

	if(Width > SZ_MaxWidth)
		SZ_MaxWidth = Width;

	if(!SetLeft)
		SZ_CurrentLeft = Left;

	if(!(*FirstBox))
		*FirstBox = Box;
	else
	{
		struct TextBox *CurrentBox = *FirstBox;

		while(CurrentBox->NextBox)
			CurrentBox = CurrentBox->NextBox;

		CurrentBox->NextBox = Box;
	}

	retval = Box;

end:
#ifdef __AROS__
	AROS_SLOWSTACKTAGS_POST
#else
	return retval;
#endif
}

	/* SZ_SetBoxTitles(struct TextBox *Box,STRPTR Array,...):
	 *
	 *	Set the titles displayed in a text box.
	 */

VOID
SZ_SetBoxTitles(struct TextBox *Box,CONST_STRPTR Array,...)
{
	if(Box)
	{
		CONST_STRPTR	*Data = &Array;
		LONG	 i = 0;

		while(*Data != NULL)
		{
			if(i < Box->NumLines)
				Box->Title[i++] = *Data;

			Data++;
		}
	}
}

	/* SZ_SetLine():
	 *
	 *	Print a string into a text box, plain version.
	 */

VOID
SZ_SetLine(struct RastPort *RPort,struct TextBox *Box,LONG Line,STRPTR String)
{
	LONG			 FgPen	= ReadAPen(RPort),
					 BgPen	= ReadBPen(RPort),
					 DrMd	= ReadDrMd(RPort);
	struct TextFont	*Font	= RPort->Font;

	LONG			 Width,Len,
					 Left	= Box->Left + 4,
					 Top	= Box->Top + 2 + Line * Box->LineHeight;

	SetPens(RPort,Box->TextPen,SZ_BackPen,JAM2);

	if(Font != UserTextFont)
		SetFont(RPort,UserTextFont);

	if(Len = strlen(String))
	{
		if(Len > Box->NumChars)
			Len = Box->NumChars;

		Len = FitText(RPort,Box->LineWidth,String,Len);

		if(Len)
		{
			Width = TextLength(RPort,String,Len);

			PlaceText(RPort,Left,Top,String,Len);
		}
		else
			Width = 0;
	}
	else
		Width = 0;

	if(Width != Box->LineWidth)
	{
		if(FgPen != SZ_BackPen)
		{
			SetAPen(RPort,SZ_BackPen);

			RectFill(RPort,Left + Width,Top,Left + Box->LineWidth - 1,Top + Box->LineHeight - 1);

			SetAPen(RPort,FgPen);
		}
		else
			RectFill(RPort,Left + Width,Top,Left + Box->LineWidth - 1,Top + Box->LineHeight - 1);
	}

	SetPens(RPort,FgPen,BgPen,DrMd);

	if(String != Box->Text[Line])
	{
		if(Len > 0)
			CopyMem(String,Box->Text[Line],Len);

		Box->Text[Line][Len] = 0;
	}

	if(Font != UserTextFont)
		SetFont(RPort,Font);
}

	/* SZ_PrintLine():
	 *
	 *	Print a string into a text box, varargs version.
	 */

VOID
SZ_PrintLine(struct RastPort *RPort,struct TextBox *Box,LONG Line,STRPTR String,...)
{
	va_list	VarArgs;
	UBYTE	Buffer[256];

	va_start(VarArgs,String);
	LimitedVSPrintf(sizeof(Buffer),Buffer,String,VarArgs);
	va_end(VarArgs);

	SZ_SetLine(RPort,Box,Line,Buffer);
}

	/* SZ_DrawBox(struct RastPort *RPort,struct TextBox *Box):
	 *
	 *	(Re-)Draw a text box.
	 */

STATIC VOID
SZ_DrawBox(struct RastPort *RPort,struct TextBox *Box)
{
	if(Box)
	{
		LONG			 LineY,i,Len,FgPen = ReadAPen(RPort),BgPen = ReadBPen(RPort),DrMd = ReadDrMd(RPort);
		struct TextFont	*Font = RPort->Font;

		if(Font != UserTextFont)
			SetFont(RPort,UserTextFont);

		if(FgPen != SZ_BackPen)
		{
			SetAPen(RPort,SZ_BackPen);

			FillBox(RPort,Box->Left,Box->Top,Box->Width,Box->Height);

			SetAPen(RPort,FgPen);
		}
		else
			FillBox(RPort,Box->Left,Box->Top,Box->Width,Box->Height);

		DrawBevelBox(RPort,Box->Left,Box->Top,Box->Width,Box->Height,
			GT_VisualInfo,	VisualInfo,
			GTBB_Recessed,	TRUE,
		TAG_DONE);

		LineY = Box->Top + 2;

		SetPens(RPort,Box->TitleFgPen,Box->TitleBgPen,JAM2);

		for(i = 0 ; i < Box->NumLines ; i++)
		{
			if(Len = strlen(Box->Title[i]))
				PlaceText(RPort,Box->Left - INTERWIDTH - TextLength(RPort,Box->Title[i],Len),LineY,Box->Title[i],Len);

			LineY += Box->LineHeight;
		}

		for(i = 0 ; i < Box->NumLines ; i++)
			SZ_PrintLine(RPort,Box,i,Box->Text[i]);

		SetPens(RPort,FgPen,BgPen,DrMd);

		if(Font != UserTextFont)
			SetFont(RPort,Font);
	}
}

	/* SZ_DrawBoxes(struct RastPort *RPort,struct TextBox *FirstBox):
	 *
	 *	(Re-)Draw a number of text boxes.
	 */

VOID
SZ_DrawBoxes(struct RastPort *RPort,struct TextBox *FirstBox)
{
	do
		SZ_DrawBox(RPort,FirstBox);
	while(FirstBox = FirstBox->NextBox);
}

	/* SZ_MoveBox(struct TextBox *Box,LONG Left,LONG Top):
	 *
	 *	Change the location of a text box.
	 */

STATIC VOID
SZ_MoveBox(struct TextBox *Box,LONG Left,LONG Top)
{
	Box->Left	+= Left;
	Box->Top	+= Top;
}

	/* SZ_MoveBoxes(struct TextBox *FirstBox,LONG Left,LONG Top):
	 *
	 *	Change the locations of a number of text boxes.
	 */

VOID
SZ_MoveBoxes(struct TextBox *FirstBox,LONG Left,LONG Top)
{
	do
		SZ_MoveBox(FirstBox,Left,Top);
	while(FirstBox = FirstBox->NextBox);
}

	/* SZ_SetBox(struct TextBox *Box,LONG Left,LONG Top):
	 *
	 *	Set the location of a text box.
	 */

STATIC VOID
SZ_SetBox(struct TextBox *Box,LONG Left,LONG Top)
{
	if(Left >= 0)
		Box->Left = Left;

	if(Top >= 0)
		Box->Top = Top;
}

	/* SZ_SetBoxes(struct TextBox *FirstBox,LONG Left,LONG Top):
	 *
	 *	Set the locations of a number of text boxes.
	 */

VOID
SZ_SetBoxes(struct TextBox *FirstBox,LONG Left,LONG Top)
{
	do
		SZ_SetBox(FirstBox,Left,Top);
	while(FirstBox = FirstBox->NextBox);
}

	/* SZ_GetBoxInfo(struct TextBox *Box,LONG Type):
	 *
	 *	Query information on a certain text box.
	 */

LONG
SZ_GetBoxInfo(struct TextBox *Box,LONG Type)
{
	switch(Type)
	{
		case BOX_LEFT:

			return(Box->Left);

		case BOX_TOP:

			return(Box->Top);

		case BOX_WIDTH:

			return(Box->Width);

		case BOX_HEIGHT:

			return(Box->Height);

		default:

			return(0);
	}
}

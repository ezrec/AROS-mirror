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

VOID
LTP_DrawMultiLineButton(struct RastPort *RPort,LONG Left,LONG Top,LONG Width,LONG Height,struct ImageInfo *ImageInfo,BOOL Bold)
{
	LONG Len,ThisTop,ThisLeft;
	STRPTR *Lines = ImageInfo->Lines;
	LONG LineCount = ImageInfo->LineCount;
	STRPTR KeyStroke = ImageInfo->KeyStroke;
	STRPTR Label;
	ULONG OldStyle;
	struct TextFont *OldFont;

	if(ImageInfo->Font)
	{
		OldFont = RPort->Font;

		SetFont(RPort,ImageInfo->Font);
	}
	else
		OldFont = NULL;

	OldStyle = RPort->AlgoStyle;

	if(Bold)
		SetSoftStyle(RPort,FSF_BOLD,FSF_BOLD);

	if(!LineCount)
		LineCount = 1;

	ThisTop = Top + (Height - LineCount * RPort->TxHeight + 1) / 2;

	do
	{
		if(LineCount && Lines)
			Label = *Lines++;
		else
			Label = ImageInfo->Label;

		Len = strlen(Label);

		ThisLeft = Left + (Width - TextLength(RPort,Label,Len)) / 2;

		LTP_PrintText(RPort,Label,Len,ThisLeft,ThisTop);

		if(KeyStroke)
		{
			if(KeyStroke >= Label && KeyStroke < Label + Len)
			{
				if(KeyStroke != Label)
					ThisLeft += TextLength(RPort,Label,(IPTR)KeyStroke - (IPTR)(Label));

				SetSoftStyle(RPort,FSF_UNDERLINED,FSF_UNDERLINED);

				LTP_PrintText(RPort,KeyStroke,1,ThisLeft,ThisTop);

				SetSoftStyle(RPort,0,FSF_UNDERLINED);

				KeyStroke = NULL;
			}
		}

		ThisTop += RPort->TxHeight;
	}
	while(--LineCount > 0);

	if(Bold)
		SetSoftStyle(RPort,OldStyle,FSF_BOLD);

	if(OldFont)
		SetFont(RPort,OldFont);
}

VOID
LTP_DrawBox(struct RastPort *rp,struct DrawInfo *drawInfo,LONG left,LONG top,LONG width,LONG height,BOOL selected,BOOL ghosted,ImageInfo *imageInfo)
{
	LONG pen3,pen4;
	LONG imageType;
	LONG Width,Height;
	UWORD *pens;

		// It doesn't work reliably under v2.04 without these

	if(!V39)
		LTP_ResetRenderInfo(rp);

	pens = drawInfo->dri_Pens;

	if(selected)
	{
		pen3 = pens[FILLPEN];
		pen4 = pens[FILLTEXTPEN];
	}
	else
	{
		pen3 = pens[BACKGROUNDPEN];
		pen4 = pens[TEXTPEN];
	}

	if(imageInfo->Emboss)
	{
		Width	= 3;
		Height	= 2;
	}
	else
	{
		Width	= 2;
		Height	= 1;
	}

	if(!imageInfo->UseFrame)
	{
		LTP_RenderBevel(rp,pens,left,top,width,height,selected,imageInfo->Emboss ? 3 : 2);

		LTP_SetAPen(rp,pen3);

		LTP_FillBox(rp,left + Width,top + Height,width - 2 * Width,height - 2 * Height);
	}
	else
	{
		ULONG state;

		if(selected)
			state = IDS_SELECTED;
		else
		{
			if(ghosted)
				state = IDS_DISABLED;
			else
				state = IDS_NORMAL;
		}

		DrawImageState(rp,imageInfo->FrameImage,left,top,state,drawInfo);
	}

	LTP_SetPens(rp,pen4,0,JAM1);

	switch(imageType = imageInfo->ImageType)
	{
		case IMAGECLASS_PICKER:

			LTP_DrawAdjustedPicker(rp,FALSE,left,top,width,height,drawInfo->dri_Resolution.X,drawInfo->dri_Resolution.Y);
			break;

		case IMAGECLASS_MULTILINEBUTTON:

			LTP_DrawMultiLineButton(rp,left + Width,top + Height,width - 2 * Width,height - 2 * Height,imageInfo,imageInfo->Emboss);
			break;

		case IMAGECLASS_LEFTINCREMENTER:
		case IMAGECLASS_RIGHTINCREMENTER:

			LTP_DrawIncrementer(rp,drawInfo,imageType == IMAGECLASS_LEFTINCREMENTER,left + 2,top + 1,width - 4,height - 2);
			break;

		#ifdef DO_TAPEDECK_KIND
		{
			case IMAGECLASS_BACKWARD:
			case IMAGECLASS_FORWARD:
			case IMAGECLASS_PREVIOUS:
			case IMAGECLASS_NEXT:
			case IMAGECLASS_RECORD:
			case IMAGECLASS_PLAY:
			case IMAGECLASS_STOP:
			case IMAGECLASS_PAUSE:
			case IMAGECLASS_EJECT:
			case IMAGECLASS_REWIND:

				LTP_DrawTapeButton(rp,imageInfo,left,top,width,height,drawInfo->dri_Resolution.X,drawInfo->dri_Resolution.Y,pen3);
				break;
		}
		#endif	/* DO_TAPEDECK_KIND */
	}
}

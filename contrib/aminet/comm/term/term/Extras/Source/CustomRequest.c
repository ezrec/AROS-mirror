/*
**	CustomRequest.c
**
**	EasyRequest like custom requester
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* GetScreenInfo():
	 *
	 *	Obtain the currently visible screen area.
	 */

STATIC VOID
GetScreenInfo(struct Screen *Screen,WORD *Left,WORD *Top,WORD *Width,WORD *Height)
{
	struct ViewPortExtra	*Extra;
	struct TagItem			 Tags[2] = { VTAG_VIEWPORTEXTRA_GET, NULL, TAG_DONE };

	if(!VideoControl(Screen->ViewPort.ColorMap,Tags))
		Extra = (struct ViewPortExtra *)Tags[0].ti_Data;
	else
		Extra = NULL;

	if(Extra)
	{
		struct Rectangle Clip;

		QueryOverscan(GetVPModeID(&Screen->ViewPort),&Clip,OSCAN_TEXT);

		*Width	= Extra->DisplayClip.MaxX - Extra->DisplayClip.MinX + 1;
		*Height	= Extra->DisplayClip.MaxY - Extra->DisplayClip.MinY + 1;

		if(*Width < Clip.MaxX - Clip.MinX + 1)
			*Width = Clip.MaxX - Clip.MinX + 1;

		if(*Height < Clip.MaxY - Clip.MinY + 1)
			*Height = Clip.MaxY - Clip.MinY + 1;
	}
	else
	{
		*Width	= Screen->Width;
		*Height	= Screen->Height;
	}

	*Left	= ABS(Screen->LeftEdge);
	*Top	= ABS(Screen->TopEdge);

	if(*Left > Screen->Width || *Left < 0)
		*Left = 0;

	if(*Top > Screen->Height || *Top < 0)
		*Top = 0;
}

	/* CentreWindow():
	 *
	 *	Centre a window within the visible bounds of
	 *	a screen.
	 */

STATIC VOID
CentreWindow(struct Screen *Screen,WORD Width,WORD Height,WORD *Left,WORD *Top)
{
	WORD ScreenWidth,ScreenHeight,ScreenLeft,ScreenTop;

	GetScreenInfo(Screen,&ScreenLeft,&ScreenTop,&ScreenWidth,&ScreenHeight);

	*Left	= ScreenLeft + (ScreenWidth - Width) / 2;
	*Top	= ScreenTop + (ScreenHeight - Height) / 2;
}

#ifndef __AROS__
VOID SAVE_DS ASM
CustomStuffText(REG(a3) LONG *Data,REG(d0) UBYTE Char)
#else
AROS_UFH2(VOID, CustomStuffText,
 AROS_UFHA(UBYTE          , Char, D0),
 AROS_UFHA(LONG *         , Data, A3))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	struct IntuiText *IText;
	STRPTR Buffer;

	Buffer	= (STRPTR)Data[0];	/* Space allocated for the char buffer */
	IText	= (APTR)Data[1];	/* Space allocated for the IntuiTexts */

	if(Char == Data[2] || Char == '\0')
	{
		LONG Width;

		IText->LeftEdge	= 0;
		IText->TopEdge	= Data[3];	/* Text top edge */
		IText->ITextFont	= NULL;

			/* Line height, derived from font size */

		Data[3] += Data[4];

			/* Chop off the rest */

		*Buffer++ = '\0';

			/* Measure the length in pixels */

		if(Data[6])
		{
			Width = TextLength((APTR)Data[6],IText->IText,(LONG)Buffer - (LONG)IText->IText - 1);

				/* More than we got before? */

			if(Width > Data[5])
				Data[5] = Width;
		}

		if(Char != '\0')
		{
				/* Link to following text */

			IText->NextText = IText + 1;

				/* Get ready for the next line */

			Data[1] = (LONG)(++IText);

				/* Start of next line */

			IText->IText = Buffer;
		}
		else
			IText->NextText = NULL;			/* Terminate the chain */
	}
	else
		*Buffer++ = Char;						/* Just store the character */

	Data[0] = (LONG)Buffer;						/* Remember for next char */

#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

#ifndef __AROS__
VOID ASM
CustomCountChar(REG(a3) LONG *Count,REG(d0) UBYTE Char)
#else
AROS_UFH2(VOID, CustomCountChar,
 AROS_UFHA(UBYTE         , Char , D0),
 AROS_UFHA(LONG *        , Count, A3))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	if(Char == Count[2] || Char == '\0')	/* Count the number of lines */
		Count[1]++;

	Count[0]++;								/* Count the number of characters in total */
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

STATIC struct IntuiText *
BuildITextTable(STRPTR FormatString,va_list VarArgs,UBYTE Terminator,LONG *Information,LONG *MoreData)
{
	struct IntuiText *Primitive;

	Information[0] = Information[1] = 0;
	Information[2] = Terminator;

	#ifdef USE_GLUE
		RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)CustomCountCharGlue,(APTR)Information);
	#else
		RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)CustomCountChar,(APTR)Information);
	#endif

	if(Primitive = (struct IntuiText *)AllocVec(Information[1] * sizeof(struct IntuiText) + Information[0],MEMF_ANY))
	{
			/* Room for the char buffer */

		MoreData[0]	= (LONG)(Primitive + Information[1]);

			/* Room for the IntuiTexts */

		MoreData[1] = (LONG)Primitive;

			/* Separator char to use */

		MoreData[2] = Terminator;

			/* Top edge of first text, the following lines get placed below */

		MoreData[3] = 0;

			/* The maximum width of all lines will end up here */

		MoreData[5] = 0;

			/* Now this is important; the first IText buffer must */
			/* be set up here, the formatting code will do the rest */
			/* for all the others */

		Primitive[0].IText = (STRPTR)MoreData[0];
	}

	return(Primitive);
}

	/* ShowInfo():
	 *
	 *	Display an EasyRequest like information requester.
	 */

LONG
ShowInfo(struct Window *Parent,STRPTR Title,STRPTR Continue,STRPTR FormatString,...)
{
	struct Screen *Screen;
	ULONG IntuiLock;
	LONG Result;
	BOOL PopBack;

	Result = 0;

	if(Parent)
		Screen = Parent->WScreen;
	else
		Screen = LockPubScreen(NULL);

	IntuiLock = LockIBase(NULL);

	PopBack = (BOOL)(IntuitionBase->FirstScreen != Screen);

	UnlockIBase(IntuiLock);

	if(Screen)
	{
		struct DrawInfo *DrawInfo;

		if(PopBack)
			ScreenToFront(Screen);

		if(DrawInfo = GetScreenDrawInfo(Screen))
		{
			LONG Information[3],GadgetInformation[3];
			LONG MoreData[7],GadgetData[7];
			struct Image *GadgetFrameImage;
			struct IntuiText *GadgetText;
			struct TextAttr FontTemplate;
			struct Gadget **GadgetTable;
			struct IntuiText *BodyText;
			struct TextFont *Font;
			LONG FullButtonWidth;
			LONG DistX,DistY;
			LONG RoomX,RoomY;
			va_list VarArgs;

				/* We'll need this one later */

			Font = DrawInfo->dri_Font;

				/* Set up the spacing information */

			RoomY = (Font->tf_YSize + 1) / 2;
			RoomX = (RoomY * DrawInfo->dri_Resolution.Y) / DrawInfo->dri_Resolution.X;

				/* Make a copy of the screen font */

			AskFont(&Screen->RastPort,&FontTemplate);

				/* Request that it be boldfaced, the leftmost */
				/* button will receive this type of label */

			FontTemplate.ta_Style |= FSF_BOLD;

			va_start(VarArgs,FormatString);
			BodyText = BuildITextTable(FormatString,VarArgs,'\n',Information,MoreData);
			va_end(VarArgs);

			GadgetText = BuildITextTable(Continue,NULL,'|',GadgetInformation,GadgetData);

			GadgetTable = (struct Gadget **)AllocVec(sizeof(struct Gadget *) * GadgetInformation[1],MEMF_ANY|MEMF_CLEAR);

			GadgetFrameImage = (struct Image *)NewObjectA(NULL,FRAMEICLASS,NULL);

			if(GadgetFrameImage && GadgetTable && GadgetText)
			{
				struct Gadget *Previous;
				LONG ID,Last,i;
				ULONG PrevTag;

				ID			= 0;
				Last		= GadgetInformation[1] - 1;

				Previous	= NULL;
				PrevTag		= TAG_IGNORE;

					/* We'll work with the button sizes in a minute... */

				FullButtonWidth = 0;

					/* We don't want fonts to be measured */

				GadgetData[6] = 0;

				#ifdef USE_GLUE
					RawDoFmt(Continue,NULL,(PUTCHAR)CustomStuffTextGlue,GadgetData);
				#else
					RawDoFmt(Continue,NULL,(PUTCHAR)CustomStuffText,GadgetData);
				#endif	/* USE_GLUE */

				DistY = 2;
				DistX = (DistY * DrawInfo->dri_Resolution.Y) / DrawInfo->dri_Resolution.X;

				GadgetText[0].ITextFont = &FontTemplate;

				for(i = 0 ; i <= Last ; i++)
				{
					if(i == Last)
						ID = 0;
					else
						ID++;

					GadgetText[i].NextText = NULL;

					if(GadgetTable[i] = NewObject(NULL,FRBUTTONCLASS,
						GA_IntuiText,	&GadgetText[i],
						GA_Image,		GadgetFrameImage,
						GA_RelVerify,	TRUE,
						GA_ID,			ID,
						GA_DrawInfo,	DrawInfo,
						PrevTag,		Previous,
					TAG_DONE))
					{
						Previous	= GadgetTable[i];
						PrevTag		= GA_Previous;

						SetAttrs(Previous,
							GA_Width,	2*DistX + Previous->Width,
							GA_Height,	2*DistY + Previous->Height,
							GA_DrawInfo,DrawInfo,
						TAG_DONE);

							/* Add the width of the button, plus */
							/* some room in between */

						FullButtonWidth += Previous->Width + RoomX;
					}
					else
					{
						LONG j;

						for(j = 0 ; j < i ; j++)
							DisposeObject(GadgetTable[i]);

						FreeVec(GadgetTable);
						GadgetTable = NULL;

						break;
					}
				}

					/* Subtract the trailing space, add the border space */

				FullButtonWidth += Screen->WBorRight + RoomX + Screen->WBorLeft;
			}

				/* Did we get what we wanted and will the buttons fit on */
				/* the screen? */

			if(BodyText && GadgetText && GadgetTable && FullButtonWidth <= Screen->Width)
			{
				WORD WindowWidth,WindowHeight,ScreenWidth,ScreenHeight,ScreenLeft,ScreenTop;
				struct Image *FillImage,*FrameImage,*TextImage;
				WORD Width,Height,Left,Top;
				struct Image *DepthImage;
				LONG FullTitleWidth;
				ULONG DepthWidth;

					/* Before we do anything else, set up the */
					/* full width of the window title; this includes */
					/* both the text and the depth arrangement gadget */

				if(Title)
					FullTitleWidth = TextLength(&Screen->RastPort,Title,strlen(Title));
				else
					FullTitleWidth = 0;

					/* Get the width of the window depth gadget for this screen */

				if(DepthImage = NewObject(NULL,SYSICLASS,
					SYSIA_Size,		(Screen->Flags & SCREENHIRES) ? SYSISIZE_MEDRES : SYSISIZE_LOWRES,
					SYSIA_Which,	DEPTHIMAGE,
					SYSIA_DrawInfo,	DrawInfo,
				TAG_DONE))
				{
					GetAttr(IA_Width,DepthImage,&DepthWidth);

					DisposeObject(DepthImage);
				}
				else
					DepthWidth = (Screen->Flags & SCREENHIRES) ? 23 : 17;

				FullTitleWidth += DepthWidth + 2;

				if(FullTitleWidth > Screen->Width)
					FullTitleWidth = Screen->Width;

					/* Increase the size to fit all the buttons into the window */

				if(FullButtonWidth > FullTitleWidth)
					FullTitleWidth = FullButtonWidth;

					/* Height of each line */

				MoreData[4] = Font->tf_YSize;

					/* Pointer to screen rastport used for measuring the text */

				MoreData[6] = (LONG)&Screen->RastPort;

					/* Now do the magic formatting... */

				va_start(VarArgs,FormatString);

				#ifdef USE_GLUE
					RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)CustomStuffTextGlue,(APTR)MoreData);
				#else
					RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)CustomStuffText,(APTR)MoreData);
				#endif	/* USE_GLUE */

				va_end(VarArgs);

					/* The maximum width of all lines is now in MoreData[5], */
					/* the effective height of the block in MoreData[3]; */

				TextImage = NewObject(NULL,ITEXTICLASS,
					IA_FGPen,		DrawInfo->dri_Pens[TEXTPEN],
					IA_Data,		BodyText,
					IA_Width,		MoreData[5] + 2*DistX,	/* Add a little room around the text */
					IA_Height,		MoreData[3] + 2*DistY,
				TAG_DONE);

					/* Build the checkered background fill pattern */

				FillImage = NewObject(NULL,FILLRECTCLASS,
					IA_APattern,	&Crosshatch,
					IA_APatSize,	1,
					IA_Mode,		JAM2,
					IA_FGPen,		DrawInfo->dri_Pens[SHINEPEN],
					IA_BGPen,		DrawInfo->dri_Pens[BACKGROUNDPEN],
				TAG_DONE);

					/* Do the same for the frame around the text */

				FrameImage = NewObject(NULL,FRAMEICLASS,
					IA_Recessed,	TRUE,
				TAG_DONE);

					/* Did we get 'em all? */

				if(FillImage && FrameImage && TextImage)
				{
					LONG WindowLeft,WindowTop;
					struct IBox FramedSize;
					struct Window *Window;
					struct Gadget Dummy;

						/* We'll need these offsets pretty soon */

					WindowLeft	= Screen->WBorLeft;
					WindowTop	= Screen->WBorTop + Screen->Font->ta_YSize + 1;

						/* Now check how large the surrounding frame would be */

					DoMethod((Object *)FrameImage,IM_FRAMEBOX,&TextImage->LeftEdge,&FramedSize,DrawInfo,NULL);

						/* Pad the space around the frame a bit */

					FramedSize.Width	+= 2 * RoomX;
					FramedSize.Height	+= 2 * RoomY;

						/* Save these for the window open stuff */

					Width	= WindowLeft +	FramedSize.Width +	Screen->WBorRight;
					Height	= WindowTop +	FramedSize.Height +	Screen->WBorBottom;

						/* If the window title won't fit on the screen, */
						/* expand the fillimage a bit; do the same to the */
						/* textimage. */

					if(Width < FullTitleWidth)
					{
						LONG Adapt = (FullTitleWidth - Width + 1) & ~1;

						FramedSize.Width	+= Adapt;
						Width				+= Adapt;

						DistX				+= Adapt / 2;

						SetAttrs((Object *)TextImage,
							IA_Width,MoreData[5] + 2*DistX,
						TAG_DONE);
					}

						/* Adjust size and position of the background pattern */

					SetAttrs((Object *)FillImage,
						IA_Left,	WindowLeft,
						IA_Top,		WindowTop,
						IA_Width,	FramedSize.Width,
						IA_Height,	FramedSize.Height,
					TAG_DONE);

						/* Do the whole thing all over again for the framed text; */
						/* this will put the right coords to centre the text within */
						/* the frame into FramedSize */

					DoMethod((Object *)FrameImage,IM_FRAMEBOX,&TextImage->LeftEdge,&FramedSize,DrawInfo,FRAMEF_SPECIFY);

						/* Move the text around */

					SetAttrs((Object *)TextImage,
						IA_Left,	WindowLeft	- FramedSize.Left	+ DistX,
						IA_Top,		WindowTop	- FramedSize.Top	+ DistY,
					TAG_DONE);

						/* Almost finished, now centre the frame around the text */

					DoMethod((Object *)FrameImage,IM_FRAMEBOX,&TextImage->LeftEdge,&FramedSize,DrawInfo,NULL);

					SetAttrs((Object *)FrameImage,
						IA_Left,	FramedSize.Left		- DistX,
						IA_Top,		FramedSize.Top		- DistY,
						IA_Width,	FramedSize.Width,
						IA_Height,	FramedSize.Height,
					TAG_DONE);

						/* Now link the objects together */

					FillImage->NextImage	= FrameImage;
					FrameImage->NextImage	= TextImage;

						/* Zap the dummy gadget and put the image inside */

					memset(&Dummy,0,sizeof(Dummy));

					Dummy.Flags			= GFLG_GADGIMAGE|GFLG_GADGHNONE;
					Dummy.GadgetRender	= FillImage;

						/* Put the gadget in */

					/* ALWAYS */
					{
						LONG LocalLeft	= FrameImage->LeftEdge;
						LONG LocalTop	= FillImage->TopEdge + FillImage->Height;

						GadgetTable[GadgetInformation[1] - 1]->NextGadget = &Dummy;

						SetAttrs((Object *)FillImage,
							IA_Height,	FillImage->Height + GadgetTable[0]->Height + RoomY,
						TAG_DONE);

						Height += GadgetTable[0]->Height + RoomY;

						if(GadgetInformation[1] > 1)
						{
							LONG Room,Count,i;

							Room		 = Width - FullButtonWidth;
							Count		 = GadgetInformation[1] - 1;
							LocalLeft	 = FrameImage->LeftEdge;
							Room		+= Count * RoomX;

							for(i = 0 ; i <= Count ; i++)
							{
								SetAttrs((Object *)GadgetTable[i],
									GA_Top,		LocalTop,
									GA_Left,	LocalLeft,
								TAG_DONE);

								LocalLeft += GadgetTable[i]->Width + ((Room * (i + 1)) / Count - (Room * i) / Count);
							}
						}
						else
						{
							SetAttrs((Object *)GadgetTable[0],
								GA_Top,		LocalTop,
								GA_Left,	LocalLeft + (FillImage->Width - GadgetTable[0]->Width) / 2,
							TAG_DONE);
						}
					}

						/* Check if the window will fit on the screen */

					if(Width <= Screen->Width && Height <= Screen->Height)
					{
							/* Centre the body text strings */

						/* ALWAYS */
						{
							struct IntuiText *This;

							for(This = BodyText ; This ; This = This->NextText)
							{
								This->ITextFont = Screen->Font;
								This->LeftEdge = (MoreData[5] - IntuiTextLength(This)) / 2;
								This->ITextFont = NULL;
							}
						}

							/* Centre the window to open */

						if(Parent)
						{
							WindowLeft		= Parent->LeftEdge + Parent->BorderLeft;
							WindowTop		= Parent->TopEdge + Parent->BorderTop;
							WindowWidth		= Parent->Width - (Parent->BorderLeft + Parent->BorderRight);
							WindowHeight	= Parent->Height - (Parent ->BorderTop + Parent->BorderBottom);

							if((Left = WindowLeft + (WindowWidth - Width) / 2) < 0)
								Left = 0;

							if((Top = WindowTop + (WindowHeight - Height) / 2) < 0)
								Top = 0;

							GetScreenInfo(Screen,&ScreenLeft,&ScreenTop,&ScreenWidth,&ScreenHeight);

							if(Left < ScreenLeft || Left + Width > ScreenLeft + ScreenWidth)
								Left = -1;

							if(Top < ScreenTop || Top + Height > ScreenTop + ScreenHeight)
								Top = -1;

							if(Top == -1 || Left == -1)
								CentreWindow(Screen,Width,Height,&Left,&Top);
						}
						else
							CentreWindow(Screen,Width,Height,&Left,&Top);

							/* Finally, open the window */

						if(Window = OpenWindowTags(NULL,
							WA_Left,			Left,
							WA_Top,				Top,
							WA_Width,			Width,
							WA_Height,			Height,
							WA_Title,			Title,
							WA_Flags,			WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_RMBTRAP | WFLG_NOCAREREFRESH | WFLG_SIMPLE_REFRESH | WFLG_ACTIVATE,
							WA_IDCMP,			IDCMP_GADGETUP | IDCMP_VANILLAKEY | IDCMP_RAWKEY,
							WA_CustomScreen,	Screen,
							WA_Gadgets,			GadgetTable[0],
						TAG_DONE))
						{
							struct IntuiMessage	*Message;
							struct Gadget *MsgGadget;
							struct Gadget *Gadget;
							BOOL Terminated;
							ULONG MsgClass;
							LONG MsgCode;

							Gadget		= NULL;
							Terminated	= FALSE;

							do
							{
								WaitPort(Window->UserPort);

								while(Message = (struct IntuiMessage *)GetMsg(Window->UserPort))
								{
									MsgClass	= Message->Class;
									MsgCode		= Message->Code;
									MsgGadget	= Message->IAddress;

									ReplyMsg((struct Message *)Message);

									switch(MsgClass)
									{
										case IDCMP_GADGETUP:

											Terminated	= TRUE;
											Result		= MsgGadget->GadgetID;

											break;

										case IDCMP_VANILLAKEY:

											MsgCode = ToUpper(MsgCode);

											switch(MsgCode)
											{
												case '\033':
												case 'B':
												case CONTROL_('C'):

													Terminated	= TRUE;
													Gadget		= GadgetTable[GadgetInformation[1] - 1];

													break;

												case '\r':
												case 'V':

													Terminated	= TRUE;
													Gadget		= GadgetTable[0];

													break;
											}

											break;

										case IDCMP_RAWKEY:

											switch(MsgCode)
											{
												case HELP_CODE:

													GuideDisplay(CONTEXT_MAIN);
													break;

												default:

													if(MsgCode >= F01_CODE && MsgCode <= F10_CODE)
													{
														MsgCode -= F01_CODE;

														if(MsgCode < GadgetInformation[1])
														{
															Terminated = TRUE;

															Gadget = GadgetTable[MsgCode];
														}
													}

													break;
											}

											break;
									}
								}
							}
							while(!Terminated);

							if(Gadget)
							{
								STATIC ULONG SelectedTags[]		= { GA_Selected,TRUE,	TAG_DONE };
								STATIC ULONG DeselectedTags[]	= { GA_Selected,FALSE,	TAG_DONE };

								SetGadgetAttrsA(Gadget,Window,NULL,(struct TagItem *)SelectedTags);
								RefreshGList(Gadget,Window,NULL,1);

								DelayTime(0,80000);

								SetGadgetAttrsA(Gadget,Window,NULL,(struct TagItem *)DeselectedTags);
								RefreshGList(Gadget,Window,NULL,1);
							}

							CloseWindow(Window);
						}
					}
				}

					/* Get rid of the images */

				DisposeObject(FillImage);
				DisposeObject(FrameImage);
				DisposeObject(TextImage);
			}

				/* And the buffers so much work went into... */

			FreeVec(BodyText);
			FreeVec(GadgetText);

			if(GadgetTable)
			{
				LONG i;

				for(i = 0 ; i < GadgetInformation[1] ; i++)
					DisposeObject(GadgetTable[i]);

				FreeVec(GadgetTable);
			}

			DisposeObject(GadgetFrameImage);

			FreeScreenDrawInfo(Screen,DrawInfo);
		}

		if(PopBack)
			ScreenToBack(Screen);

		if(!Parent)
			UnlockPubScreen(NULL,Screen);
	}

	return(Result);
}

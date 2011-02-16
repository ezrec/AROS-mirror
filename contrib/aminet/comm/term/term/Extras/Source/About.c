/*
**	About.c
**
**	Support routines for the `About' window.
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

#define HEADING 4

enum	{	GAD_FRAME=1000,GAD_SCROLL,GAD_BUTTON };

	/* LocalDeleteBitMap(struct BitMap *BitMap,LONG Width,LONG Height):
	 *
	 *	Delete yet another screen bitmap.
	 */

STATIC VOID
LocalDeleteBitMap(struct BitMap *BitMap,LONG Width,LONG Height)
{
	if(BitMap)
	{
		LONG i;

		WaitBlit();

		for(i = 0 ; i < BitMap->Depth ; i++)
		{
			if(BitMap->Planes[i])
				FreeRaster(BitMap->Planes[i],Width,Height);
		}

		FreeVecPooled(BitMap);
	}
}

	/* LocalCreateBitMap(LONG Depth,LONG Width,LONG Height):
	 *
	 *	Create yet another screen bitmap.
	 */

STATIC struct BitMap *
LocalCreateBitMap(LONG Depth,LONG Width,LONG Height)
{
	struct BitMap *BitMap;

	if(BitMap = (struct BitMap *)AllocVecPooled(sizeof(struct BitMap),MEMF_ANY))
	{
		BOOL	Success = TRUE;
		LONG	i;

		InitBitMap(BitMap,Depth,Width,Height);

		for(i = 0 ; Success && i < Depth ; i++)
		{
			if(!(BitMap->Planes[i] = AllocRaster(Width,Height)))
				Success = FALSE;
		}

		if(Success)
			return(BitMap);

		LocalDeleteBitMap(BitMap,Width,Height);
	}

	return(NULL);
}

	/* CreateBitMapFromImage(struct Image *Image,struct BitMap *BitMap):
	 *
	 *	Turn an Intuition Image into a Gfx BitMap.
	 */

STATIC VOID
CreateBitMapFromImage(struct Image *Image,struct BitMap *BitMap)
{
	PLANEPTR	Data	= (PLANEPTR)Image->ImageData;
	ULONG		Modulo	= ((((ULONG)Image->Width) + 15) >> 3) & ~1;
	LONG		i;

	InitBitMap(BitMap,Image->Depth,Image->Width,Image->Height);

	for(i = 0 ; i < Image->Depth ; i++, Data += Modulo * Image->Height)
		BitMap->Planes[i] = Data;
}

	/* RecolourBitMap():
	 *
	 *	Remap a BitMap to use a different colour selection.
	 */

STATIC struct BitMap *
RecolourBitMap (struct BitMap *Src, UBYTE * Mapping, UBYTE DestDepth, LONG Width, LONG Height)
{
	struct BitMap *Dst;

		/* Create the bitmap to hold the remapped data. */

	if(Dst = LocalCreateBitMap(DestDepth,Width,Height))
	{
		struct BitMap *SingleMap;

			/* Create a single bitplane bitmap. */

		if(SingleMap = LocalCreateBitMap(1,Width,Height))
		{
			struct BitMap *FullMap;

				/* Create a dummy bitmap. */

			if(FullMap = (struct BitMap *)AllocVecPooled(sizeof(struct BitMap),MEMF_ANY))
			{
				LONG i,Mask = (1L << Src->Depth) - 1;

					/* Make the dummy bitmap use the
					 * single bitmap in all planes.
					 */

				InitBitMap(FullMap,DestDepth,Width,Height);

				for(i = 0 ; i < DestDepth ; i++)
					FullMap->Planes[i] = SingleMap->Planes[0];

					/* Clear the destination bitmap. */

				BltBitMap(Dst,0,0,Dst,0,0,Width,Height,MINTERM_ZERO,0xFF,NULL);

					/* Is colour zero to be mapped to a non-zero colour? */

				if(Mapping[0])
				{
						/* Clear the single plane bitmap. */

					BltBitMap(SingleMap,0,0,SingleMap,0,0,Width,Height,MINTERM_ZERO,1,NULL);

						/* Merge all source bitplane data. */

					BltBitMap(Src,0,0,FullMap,0,0,Width,Height,MINTERM_B_OR_C,Mask,NULL);

						/* Invert the single plane bitmap, to give us
						 * the zero colour bitmap we can work with.
						 */

					BltBitMap(SingleMap,0,0,SingleMap,0,0,Width,Height,MINTERM_NOT_C,1,NULL);

						/* Now set all the bits for colour zero. */

					BltBitMap(FullMap,0,0,Dst,0,0,Width,Height,MINTERM_B_OR_C,Mapping[0],NULL);
				}

					/* Run down the colours. */

				for(i = 1 ; i <= Mask ; i++)
				{
					ULONG Bits = Mask ^ i;

						/* Set the single plane bitmap to all 1's. */

					BltBitMap(SingleMap,0,0,SingleMap,0,0,Width,Height,MINTERM_ONE,1,NULL);

						/* Isolate the pixels to match the colour
						 * specified in `i'.
						 */

					BltBitMap(Src,0,0,FullMap,0,0,Width,Height,MINTERM_B_AND_C,i,NULL);

					if (Bits) {
						BltBitMap(Src,0,0,FullMap,0,0,Width,Height,MINTERM_NOT_B_AND_C,Bits,NULL);
					}

						/* Set the pixels in the destination bitmap,
						 * use the designated colour.
						 */

					BltBitMap(FullMap,0,0,Dst,0,0,Width,Height,MINTERM_B_OR_C,Mapping[i],NULL);
				}

					/* Free the temporary bitmap. */

				FreeVecPooled(FullMap);

					/* Free the single plane bitmap. */

				LocalDeleteBitMap(SingleMap,Width,Height);

					/* Return the result. */

				return(Dst);
			}

			LocalDeleteBitMap(SingleMap,Width,Height);
		}

		LocalDeleteBitMap(Dst,Width,Height);
	}

	return(NULL);
}

STATIC STRPTR Table[] =
{
	"\bBeta testing:",
	"",
	"Göran Åberg   Peter L. Banville Jr.",
	"Stefan Becker   Abdelkader Benbachir",
	"Sebastian Bergmann   Martin Berndt",
	"Gregory A. Chance   Keith Christopher",
	"Mark Constable   Steve Corder",
	"Sebastian Delmont   Marcel Döring",
	"Klaus Dürr   Frank Dürring",
	"Bernd Ernesti   Kenneth Fribert",
	"Kay Gehrke   Jay Grizzard",
	"Stefan Gybas   Christoph Gülicher",
	"Chris Hanson   Peer Hasselmeyer",
	"Christian Hechelmann   Holger Heinrich",
	"Rodney Hester   Florian Hinzmann",
	"Hung-Tung Hsu   Stefan Hudson",
	"Kai Iske   Piotr Kaminski",
	"Jari Karjalainen   Andreas M. Kirchwitz",
	"Tony Kirkland   Stellan Klebom",
	"Simo Koivukoski   Jens Langner",
	"Russell John LeBar   Jason C. Leach",
	"Michael Leun   Holger Lubitz",
	"Daniel M. Makovec   Bob Maple",
	"Julian Matthew   Chris Mattingly",
	"Matthias Merkel   Dabe Murphy",
	"William Michael Mushkin   Christopher G. Newby",
	"Dean S. Pemberton   Yves Perrenoud",
	"Olaf Peters   Sven Reger",
	"Robert Reiswig   Matti Rintala",
	"Alfredo Rojas   Karsten Rother",
	"Ottmar Röhrig   Matthias Scheler",
	"Markus Schmall   Robert L. Shady",
	"Leon D. Shaner   Eric W. Sommer",
	"Jason Soukeras   Gary B. Standen",
	"Keith A. Stewart   Joel E. Swan",
	"Jonathan Tew   Bodo Thevissen",
	"Alexander Wild   Jürgen Zeschky",
	"Michael Zielesny",
	"",
	"\bForeign language translations:",
	"",
	"Olaf Barthel (Deutsch)",
	"Phillippe Brand (Français)",
	"Thomas Egrelius (Svenska)",
	"Flemming Lindeblad (Dansk)",
	"Andrea Suatoni (Italiano)",
	"Edmund Vermeulen (Nederlands)",
	"«Sorry, I lost your name» (Español)",
	"",
	"\bDocumentation:",
	"",
	"Olaf Barthel   Garry Glendown",
	"Henning Hucke   Mike Safer",
	"Mark Schröer",
	"",
	"\bXPR Libraries by:",
	"",
	"Marc Boucher (xmodem)",
	"Terence Finney (bplus)",
	"Rick Huebner & William M. Perkins (zmodem)",
	"Ueli Kaufmann (ascii, ymodem, vms)",
	"Marco Papa & Stephen Walton (kermit)",
	"Jack Rouse (quickb)",
	"",
	"\bXEM Libraries by:",
	"",
	"Ueli Kaufmann (amiga, ascii, bbs, vt340)",
	"Stef Rave (rip)",
	"",
	"\bPeople who will deny that",
	"\bthey were ever involved:",
	"",
	"John Burton   Peter Fischer",
	"David Göhler   Michael-Wolfgang Hohmann",
	"David Jones   Marko Küchmann",
	"Bernd Lambracht   Roby Leeman & AUGS",
	"Frank Mariak   Germar Morgenthaler",
	"Jürgen Otte   Till `Dill-Prince' Prinzler",
	"Nicola Salmoria   Ralph Schmidt",
	"Veith Schörgenhummer   Thorsten Seidel",
	"Markus Stoll   Martin Taillefer",
	"Christoph Teuber   Ralf Thanner",
	"Volker Ulle & the Aquila Sysop Team   Michael Vaeth",
	"Oliver Wagner   Christopher Wichura",
	"Udo Wolt   Matthias Zepf",
	"",
	"\aDon't switch off, it's almost over!",
	"",
	NULL
};

STATIC VOID
PrintThisLine(struct RastPort *RPort,struct Rectangle *Clip,LONG Top,STRPTR Line)
{
	LONG Len,Width;

	Len = strlen(Line);

	if(*Line == '\b' || *Line == '\a')
	{
		ULONG Style;

		if(*Line == '\b')
			Style = FSF_BOLD;
		else
			Style = FSF_ITALIC;

		SetSoftStyle(RPort,Style,FSF_BOLD | FSF_ITALIC);
		Line++;
		Len--;
	}
	else
		SetSoftStyle(RPort,0,FSF_BOLD | FSF_ITALIC);

	if(Len > 0)
	{
		Width = TextLength(RPort,Line,Len);

		PlaceText(RPort,Clip->MinX + (Clip->MaxX - Clip->MinX + 1 - Width) / 2,Top,Line,Len);
	}
}


	/* ShowAbout():
	 *
	 *	Open a window, draw the `term' logo, show some text
	 *	and wait for user reaction.
	 */

BOOL
ShowAbout(LONG Ticks)
{
	struct BitMap		*ImageBitMap = NULL;
	LONG				 ImageWidth,
						 ImageHeight;
	struct LayoutHandle	*Handle;
	BOOL				 GotRexxMessage = FALSE;

	if(IconBase)
	{
		struct DiskObject *Icon;

		if(Icon = GetProgramIcon())
		{
			STATIC UWORD DefaultColours[4] =
			{
				0xAAA,
				0x000,
				0xFFF,
				0x68B
			};

			UBYTE	Mapping[4];
			LONG	Colour1,Colour2;
			LONG	ChannelDistance;
			LONG	Distance,BestDistance,BestIndex,Depth,Count,i,j;
			BOOL	Duplicates = FALSE;

			Depth = GetBitMapDepth(Window->RPort->BitMap);
			Count = Window->WScreen->ViewPort.ColorMap->Count;

			if(Depth > 8)
				Depth = 8;

			if(Count > (1L << Depth))
				Count = 1L << Depth;

			if(Count > 256)
				Count = 256;

			if(Count >= 4)
			{
				for(i = 0 ; i < 4 ; i++)
				{
					Colour2 = DefaultColours[i];

					BestDistance	= 3 * 15 * 15;
					BestIndex		= 0;

					for(j = 0 ; j < Count ; j++)
					{
						Colour1 = GetRGB4(Window->WScreen->ViewPort.ColorMap,j);

						ChannelDistance = ((LONG)((Colour1 >> 8) & 0xF)) - ((LONG)((Colour2 >> 8) & 0xF));

						Distance = ChannelDistance * ChannelDistance;

						ChannelDistance = ((LONG)((Colour1 >> 4) & 0xF)) - ((LONG)((Colour2 >> 4) & 0xF));

						Distance += ChannelDistance * ChannelDistance;

						ChannelDistance = ((LONG)(Colour1 & 0xF)) - ((LONG)(Colour2 & 0xF));

						Distance += ChannelDistance * ChannelDistance;

						if(Distance < BestDistance)
						{
							BestDistance	= Distance;
							BestIndex		= j;
						}
					}

					Mapping[i] = BestIndex;
				}

				for(i = 0 ; !Duplicates && i < 4 ; i++)
				{
					for(j = i + 1 ; !Duplicates && j < 4 ; j++)
						Duplicates = (Mapping[i] == Mapping[j]);
				}

				if(!Duplicates)
				{
					struct BitMap	 LocalBitMap;
					struct Image	*Image;

					Image = Icon->do_Gadget.GadgetRender;

					if(Image->Depth == 2)
					{
						CreateBitMapFromImage(Image,&LocalBitMap);

						if(ImageBitMap = RecolourBitMap(&LocalBitMap,Mapping,Depth,Image->Width,Image->Height))
						{
							ImageWidth	= Image->Width;
							ImageHeight	= Image->Height;
						}
					}
				}
			}

			FreeDiskObject(Icon);
		}
	}

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExactClone,	TRUE,
	TAG_DONE))
	{
		struct Window		*PanelWindow;
		struct Region		*Clip = NULL,*Old;
		struct Rectangle	 ClipRect;
		LONG				 ClipWidth,
							 ClipHeight;

		if(!Ticks)
		{
			struct RastPort	This;
			ULONG			Width,MaxWidth,Len,i;
			STRPTR			String;

			InitRastPort(&This);
			SetFont(&This,UserTextFont);

			for(i = MaxWidth = 0 ; Table[i] ; i++)
			{
				Len = strlen(String = Table[i]);

				if(*String == '\b' || *String == '\a')
				{
					ULONG Style;

					if(*String == '\b')
						Style = FSF_BOLD;
					else
						Style = FSF_ITALIC;

					SetSoftStyle(&This,Style,FSF_BOLD | FSF_ITALIC);
					String++;
					Len--;
				}
				else
					SetSoftStyle(&This,0,FSF_BOLD | FSF_ITALIC);

				if(Len > 0)
				{
					Width = TextLength(&This,String,Len);

					if(Width > MaxWidth)
						MaxWidth = Width;
				}
			}

			ClipWidth	= MaxWidth;
			ClipHeight	= HEADING * This.TxHeight;

			Clip = NewRegion();
		}

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_LabelText,	TermName,
			TAG_DONE);
			{
				if(ImageBitMap)
				{
					LT_New(Handle,
						LA_Type,VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			FRAME_KIND,
							LAFR_InnerWidth,	ImageWidth,
							LAFR_InnerHeight,	ImageHeight,
							LAFR_DrawBox,		FALSE,
							LA_ID,				GAD_FRAME,
						TAG_DONE);

						LT_EndGroup(Handle);
					}
				}

				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BOX_KIND,
						LABX_FirstLine,	MSG_TERMINFO_INFOTEXT1_TXT,
						LABX_LastLine,	MSG_TERMINFO_INFOTEXT2_TXT,
						LABX_AlignText,	ALIGNTEXT_Centered,
						LABX_DrawBox,	FALSE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				if(!Ticks && Clip)
				{
					LT_New(Handle,
						LA_Type,VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			FRAME_KIND,
							LAFR_InnerWidth,	ClipWidth + 4,
							LAFR_InnerHeight,	ClipHeight + 2,
							LAFR_DrawBox,		FALSE,
							LA_ID,				GAD_SCROLL,
						TAG_DONE);

						LT_EndGroup(Handle);
					}
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
				LA_LabelID,	MSG_V36_1030,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BOX_KIND,
						LABX_FirstLine,	MSG_TERMINFO_INFOTEXT4_TXT,
						LABX_LastLine,	MSG_TERMINFO_INFOTEXT6_TXT,
						LABX_AlignText,	ALIGNTEXT_Centered,
						LABX_DrawBox,	FALSE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
					LA_LabelID,	MSG_V36_1031,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BOX_KIND,
						LABX_FirstLine,	MSG_TERMINFO_INFOTEXT10_TXT,
						LABX_LastLine,	MSG_TERMINFO_INFOTEXT12_TXT,
						LABX_AlignText,	ALIGNTEXT_Centered,
						LABX_DrawBox,	FALSE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
					LA_LabelID,	MSG_V36_1032,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BOX_KIND,
						LABX_FirstLine,	MSG_TERMINFO_INFOTEXT16_TXT,
						LABX_LastLine,	MSG_TERMINFO_INFOTEXT16_TXT,
						LABX_AlignText,	ALIGNTEXT_Centered,
						LABX_DrawBox,	FALSE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			if(!Ticks)
			{
				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		XBAR_KIND,
						LAXB_FullSize,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_V36_1033,
						LA_ID,			GAD_BUTTON,
						LABT_ReturnKey,	TRUE,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}
			}

			LT_EndGroup(Handle);
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_IDCMP,		Ticks ? (IDCMP_RAWKEY | IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_INTUITICKS) : (IDCMP_INTUITICKS),

			LAWN_HelpHook,	&GuideHook,
			LAWN_MaxPen,	-1,
			LAWN_Parent,	Window,
			WA_RMBTrap,		TRUE,
			WA_Activate,	TRUE,
			WA_ReportMouse,	TRUE,

			Ticks ? TAG_DONE : TAG_IGNORE,0,

			WA_DepthGadget,	TRUE,
			WA_DragBar,		TRUE,
			LAWN_TitleID,	MSG_V36_1034,
		TAG_DONE))
		{
			struct IntuiMessage	*Message;
			BOOL				 Done = FALSE;
			ULONG				 MsgClass,
								 MsgQualifier;
			UWORD				 MsgCode;
			struct Gadget		*MsgGadget;
			ULONG				 Signals;
			LONG				 TickCount = 0;
			LONG				 Top,Total,LastLine,IntroTicks = 15;
			struct RastPort		*RPort = PanelWindow->RPort;

			if(ImageBitMap)
			{
				LONG LocalLeft,LocalTop;

				LT_GetAttributes(Handle,GAD_FRAME,
					LA_Left,	&LocalLeft,
					LA_Top,		&LocalTop,
				TAG_DONE);

				BltBitMapRastPort(ImageBitMap,0,0,RPort,LocalLeft,LocalTop,ImageWidth,ImageHeight,MINTERM_COPY);
			}

			if(!Ticks)
			{
				LONG LeftEdge,TopEdge;

				LT_GetAttributes(Handle,GAD_SCROLL,
					LA_Left,	&LeftEdge,
					LA_Top,		&TopEdge,
				TAG_DONE);

				ClipRect.MinX = LeftEdge + 2;
				ClipRect.MinY = TopEdge + 2;
				ClipRect.MaxX = LeftEdge + 2 + ClipWidth - 1;
				ClipRect.MaxY = TopEdge + 2 + ClipHeight - 1;

				SetFont(RPort,UserTextFont);
				SetPens(RPort,Pens[TEXTPEN],Pens[BACKGROUNDPEN],JAM2);

				if(OrRectRegion(Clip,&ClipRect))
				{
					LONG i;

					Old = InstallClipRegion(PanelWindow->WLayer,Clip);

					Top = ClipRect.MinY;

					for(i = 0 ; i < HEADING ; i++)
					{
						PrintThisLine(RPort,&ClipRect,Top,Table[i]);

						Top += RPort->TxHeight;
					}

					LastLine = i;

					Total = 0;
				}
				else
				{
					DisposeRegion(Clip);
					Clip = NULL;
				}
			}

			while(Message = GT_GetIMsg(PanelWindow->UserPort))
				GT_ReplyIMsg(Message);

			if(!Ticks)
				PushWindow(PanelWindow);
			else
				Say(LocaleString(MSG_TERMINFO_WELCOME_TO_TERM_TXT));

			do
			{
				Signals = Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK | SIG_REXX);

				if(Signals & SIG_BREAK)
					break;

				if(Ticks)
				{
					if(Signals & SIG_REXX)
					{
						GotRexxMessage = TRUE;

						break;
					}
				}

				while(Message = (struct IntuiMessage *)GT_GetIMsg(PanelWindow->UserPort))
				{
					MsgClass		= Message->Class;
					MsgQualifier	= Message->Qualifier;
					MsgCode			= Message->Code;
					MsgGadget		= (struct Gadget *)Message->IAddress;

					GT_ReplyIMsg(Message);

					if(Ticks)
					{
						if(MsgClass == IDCMP_INTUITICKS && TickCount++ >= 50)
							Done = TRUE;

						if((MsgClass == IDCMP_RAWKEY || MsgClass == IDCMP_MOUSEBUTTONS) && !(MsgCode & IECODE_UP_PREFIX))
							Done = TRUE;

						if(MsgClass == IDCMP_MOUSEMOVE)
							Done = TRUE;
					}
					else
					{
						LT_HandleInput(Handle,MsgQualifier,&MsgClass,&MsgCode,&MsgGadget);

						if(MsgClass == IDCMP_GADGETUP)
							Done = TRUE;

						if(MsgClass == IDCMP_INTUITICKS && Clip)
						{
							if(IntroTicks > 0)
								IntroTicks--;
							else
							{
								ScrollRaster(RPort,0,1,ClipRect.MinX,ClipRect.MinY,ClipRect.MaxX,ClipRect.MaxY);

								BeginRefresh(Handle->Window);
								EndRefresh(Handle->Window,TRUE);

								Total++;

								PrintThisLine(RPort,&ClipRect,Top - Total,Table[LastLine]);

								if(Total == RPort->TxHeight)
								{
									Total = 0;

									LastLine++;

									if(!Table[LastLine])
										LastLine = 0;
								}
							}
						}
					}
				}
			}
			while(!Done);

			if(Clip)
			{
				InstallClipRegion(PanelWindow->WLayer,Old);

				DisposeRegion(Clip);
			}

			if(!Ticks)
				PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	if(ImageBitMap)
		LocalDeleteBitMap(ImageBitMap,ImageWidth,ImageHeight);

	return(GotRexxMessage);
}

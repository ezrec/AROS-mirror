/*
**	ScreenPanel.c
**
**	Editing panel for screen configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_SCREENMODE=1000,GAD_USE_PUBSCREEN,GAD_PUBSCREENNAME,GAD_FONT,
			GAD_COLOURMODE,GAD_SCREENTITLE,GAD_MAKESCREENPUBLIC,
			GAD_BLINKING,GAD_PALETTE,GAD_RED,GAD_GREEN,GAD_BLUE,GAD_RESET,
			GAD_STDPENS,GAD_EDITPENS,GAD_WINDOW_BORDER,GAD_SPLIT_STATUS,
			GAD_STATUS_LINE,GAD_PAGER,GAD_PAGEGROUP,

			GAD_USE,GAD_CANCEL,GAD_DEFAULT
		};

	/* CannotChangeColours(struct Configuration *LocalConfig,LONG OriginalColourMode):
	 *
	 *	This routine rolls the dice and determines whether the user should
	 *	be allowed to change the screen colours or rendering pens.
	 */

STATIC BOOL
CannotChangeColours(struct Configuration *LocalConfig,LONG OriginalColourMode)
{
	if(LocalConfig->ScreenConfig->ColourMode == OriginalColourMode && !LocalConfig->ScreenConfig->UseWorkbench && LocalConfig->ScreenConfig->ColourMode == Config->ScreenConfig->ColourMode && Screen)
	{
		LONG ThisDepth,RequestedDepth = 1;

		ThisDepth = GetBitMapDepth(Window->WScreen->RastPort.BitMap);

		switch(LocalConfig->ScreenConfig->ColourMode)
		{
			case COLOUR_AMIGA:

				RequestedDepth = 2;
				break;

			case COLOUR_EIGHT:

				RequestedDepth = 3;
				break;

			case COLOUR_SIXTEEN:

				RequestedDepth = 4;
				break;

			case COLOUR_MONO:

				RequestedDepth = 1;
				break;
		}

		return((BOOL)(ThisDepth < RequestedDepth));
	}
	else
		return(TRUE);
}

STATIC BOOL
ScreenPenPanel(struct Window *Window,UWORD *Array,UWORD *DefaultArray,LONG Depth)
{
	LayoutHandle *Handle;
	UWORD PrivateArray[16];
	BOOL Result;

	Result = FALSE;

	CopyMem(Array,PrivateArray,sizeof(PrivateArray));

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExactClone,	TRUE,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
				LA_LabelID,	MSG_PENPANEL_DRAWING_PENS_TXT,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_BACKGROUND_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[BACKGROUNDPEN],
						LA_Chars,			15,
						LA_ID,				100 + BACKGROUNDPEN,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_TEXT_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[TEXTPEN],
						LA_ID,				100 + TEXTPEN,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_IMPORTANT_TEXT_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[HIGHLIGHTTEXTPEN],
						LA_ID,				100 + HIGHLIGHTTEXTPEN,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_BRIGHT_EDGES_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[SHINEPEN],
						LA_ID,				100 + SHINEPEN,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_DARK_EDGES_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[SHADOWPEN],
						LA_ID,				100 + SHADOWPEN,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_FILLPEN_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[FILLPEN],
						LA_ID,				100 + FILLPEN,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_FILLTEXTPEN_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[FILLTEXTPEN],
						LA_ID,				100 + FILLTEXTPEN,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_BARFILLPEN_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[BARBLOCKPEN],
						LA_ID,				100 + BARBLOCKPEN,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelID,			MSG_PENPANEL_BARTEXTPEN_TXT,
						GTPA_Depth,			Depth,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&PrivateArray[BARDETAILPEN],
						LA_ID,				100 + BARDETAILPEN,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,XBAR_KIND,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_PENPANEL_STANDARDPENS_TXT,
						LA_ID,			GAD_DEFAULT,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullSize,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,LA_Type,HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_USE_GAD,
					LA_ID,			GAD_USE,
					LABT_ReturnKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
					LA_ID,			GAD_CANCEL,
					LABT_EscKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_PENPANEL_SCREEN_DRAWING_PENS_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage	*Message;
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			BOOL Done;
			LONG i;

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			GuideContext(CONTEXT_SCREEN);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_USE:

								CopyMem(PrivateArray,Array,sizeof(PrivateArray));

								Array[DETAILPEN]	= PrivateArray[BACKGROUNDPEN];
								Array[BLOCKPEN]		= PrivateArray[SHADOWPEN];
								Array[BARTRIMPEN]	= PrivateArray[BARDETAILPEN];

								Result = Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;

							case GAD_DEFAULT:

								for(i = TEXTPEN ; i <= BARBLOCKPEN ; i++)
									LT_SetAttributes(Handle,100 + i,GTPA_Color,DefaultArray[i],TAG_DONE);

								break;
						}
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	return(Result);
}

BOOL
ScreenPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[6] =
	{
		MSG_V36_0163,
		MSG_V36_0164,
		MSG_V36_0165,
		MSG_V36_0166,
		MSG_PENPANEL_DRAWING_PENS_TXT,
		-1
	};

	STATIC BYTE InitialPage = 0;

	LayoutHandle *Handle;
	CONST_STRPTR PageTitle[6];
	BOOL UseIt;

	UseIt = FALSE;

	LocalizeStringTable(PageTitle,PageTitleID);

		/* Make a backup of the current settings. We will modify only */
		/* the backup */

	SaveConfig(LocalConfig,PrivateConfig);

		/* If the settings aren't yet patched up for 96 bit mode, */
		/* do it now. */

	if(!PrivateConfig->ScreenConfig->UseColours96)
	{
		Colour12x96(PrivateConfig->ScreenConfig->Colours,PrivateConfig->ScreenConfig->Colours96,16);

		PrivateConfig->ScreenConfig->UseColours96 = TRUE;
	}

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExactClone,	TRUE,
		LAHN_ExitFlush,		FALSE,
	TAG_DONE))
	{
		UBYTE ModeName[DISPLAYNAMELEN + 1],FontName[MAX_FILENAME_LENGTH];
		struct Window *LocalWindow;
		ULONG DisplayMode;
		BOOL FixedColours;

		FixedColours = CannotChangeColours(PrivateConfig,LocalConfig->ScreenConfig->ColourMode);

			/* Get the desired screen display mode */

		DisplayMode = PrivateConfig->ScreenConfig->DisplayMode;

			/* Is the requested screen display mode available on this machine? */

		if(ModeNotAvailable(DisplayMode))
		{
			struct Screen *PubScreen = LockPubScreen(NULL);

				/* Lock the default public screen */

			if(PubScreen)
			{
				struct DimensionInfo DimensionInfo;

					/* Use the default display mode */

				DisplayMode = GetVPModeID(&PubScreen->ViewPort);

					/* Get the default screen size for this display mode */

				if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,DisplayMode))
				{
					LONG Width,Height;

					Width	= DimensionInfo.TxtOScan.MaxX - DimensionInfo.TxtOScan.MinX + 1;
					Height	= DimensionInfo.TxtOScan.MaxY - DimensionInfo.TxtOScan.MinY + 1;

						/* If the screen is set to a specific size, override */
						/* these settings with defaults. */

					if(Width != PrivateConfig->ScreenConfig->DisplayWidth && PrivateConfig->ScreenConfig->DisplayWidth > 0)
						PrivateConfig->ScreenConfig->DisplayWidth = Width;

					if(Height != PrivateConfig->ScreenConfig->DisplayHeight && PrivateConfig->ScreenConfig->DisplayHeight > 0)
						PrivateConfig->ScreenConfig->DisplayHeight = Height;
				}

				UnlockPubScreen(NULL,PubScreen);
			}
			else
				DisplayMode = DEFAULT_MONITOR_ID | HIRES_KEY;	/* Catch all... */
		}

			/* The name of the display mode chosen */

		GetModeName(DisplayMode,ModeName,sizeof(ModeName));

		BuildFontName(FontName,sizeof(FontName),PrivateConfig->ScreenConfig->FontName,PrivateConfig->ScreenConfig->FontHeight);

			/* If we cannot change the colours, or would */
			/* display the wrong colours when choosing the */
			/* rendering pens, don't display the last */
			/* two pages */

		if(FixedColours)
		{
			if(InitialPage > 2)
				InitialPage = 2;

			PageTitle[3] = NULL;
		}

		if(!Kick30)
		{
			if(InitialPage > 3)
				InitialPage = 3;

			PageTitle[4] = NULL;
		}

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		TAB_KIND,
					LA_ID,			GAD_PAGER,
					LA_LabelID,		MSG_XFER_PAGE_TXT,
					GTCY_Labels,	PageTitle,
					LACY_AutoPageID,GAD_PAGEGROUP,
					LACY_TabKey,	TRUE,
					LA_BYTE,		&InitialPage,
					LATB_FullWidth,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_ID,			GAD_PAGEGROUP,
				LAGR_ActivePage,InitialPage,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		TEXT_KIND,
							LA_LabelID,		MSG_SCREENPANEL_CURRENT_DISPLAY_MODE_GAD,
							LA_Chars,		DISPLAYNAMELEN,
							LA_ID,			GAD_SCREENMODE,
							GTTX_Text,		ModeName,
							GTTX_Border,	TRUE,
							LATX_Picker,	TRUE,
							LATX_LockSize,	TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		TEXT_KIND,
							LA_LabelID,		MSG_SCREENPANEL_SCREEN_FONT_GAD,
							LA_ID,			GAD_FONT,
							LA_Chars,		DISPLAYNAMELEN,
							GTTX_Text,		FontName,
							GTTX_Border,	TRUE,
							LATX_Picker,	TRUE,
							LATX_LockSize,	TRUE,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						if(Kick30)
						{
							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_SCREENPANEL_FASTER_LAYOUT_GAD,
								LA_BYTE,	&PrivateConfig->ScreenConfig->FasterLayout,
							TAG_DONE);
						}

						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,		MSG_SCREENPANEL_MAKE_SCREEN_PUBLIC_GAD,
							LA_BYTE,		&PrivateConfig->ScreenConfig->MakeScreenPublic,
							LA_ID,			GAD_MAKESCREENPUBLIC,
							GA_Disabled,	PrivateConfig->ScreenConfig->UseWorkbench,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,	CHECKBOX_KIND,
							LA_ID,		GAD_USE_PUBSCREEN,
							LA_LabelID,	MSG_SCREENPANEL_USE_WORKBENCH_GAD,
							LA_BYTE,	&PrivateConfig->ScreenConfig->UseWorkbench,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		STRING_KIND,
							LA_LabelID,		MSG_SCREENPANEL_PUBSCREEN_NAME_GAD,
							LA_ID,			GAD_PUBSCREENNAME,
							LA_STRPTR,		PrivateConfig->ScreenConfig->PubScreenName,
							LA_Chars,		14,
							GTST_MaxChars,	sizeof(PrivateConfig->ScreenConfig->PubScreenName) - 1,
							LAST_Picker,	TRUE,
							GA_Disabled,	!PrivateConfig->ScreenConfig->UseWorkbench,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,		MSG_SCREENPANEL_SCREEN_TITLE_GAD,
							LA_BYTE,		&PrivateConfig->ScreenConfig->TitleBar,
							LA_ID,			GAD_SCREENTITLE,
							GA_Disabled,	PrivateConfig->ScreenConfig->UseWorkbench,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,		MSG_SCREENPANEL_WINDOW_BORDER_TXT,
							LA_BYTE,		&PrivateConfig->ScreenConfig->ShareScreen,
							LA_ID,			GAD_WINDOW_BORDER,
							GA_Disabled,	PrivateConfig->ScreenConfig->UseWorkbench,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		XBAR_KIND,
						TAG_DONE);

						PrivateConfig->ScreenConfig->StatusLine &= 0xF;

						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,		MSG_SCREENPANEL_SEPARATE_STATUS_WINDOW_TXT,
							LA_BYTE,		&PrivateConfig->ScreenConfig->SplitStatus,
							LA_ID,			GAD_SPLIT_STATUS,
							GA_Disabled,	!PrivateConfig->ScreenConfig->StatusLine || (!PrivateConfig->ScreenConfig->ShareScreen && !PrivateConfig->ScreenConfig->UseWorkbench),
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		CYCLE_KIND,
							LA_LabelID,		MSG_SCREENPANEL_STATUS_LINE_GAD,
							LA_BYTE,		&PrivateConfig->ScreenConfig->StatusLine,
							LACY_FirstLabel,MSG_SCREENPANEL_STATUS_DISABLED_TXT,
							LACY_LastLabel,	MSG_SCREENPANEL_STATUS_COMPACT_TXT,
							LA_ID,			GAD_STATUS_LINE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		CYCLE_KIND,
							LA_LabelID,		MSG_SCREENPANEL_ONLINE_TIME_GAD,
							LA_BYTE,		&PrivateConfig->ScreenConfig->TimeMode,
							LACY_FirstLabel,MSG_SCREENPANEL_TIME_ONLINETIME_TXT,
							LACY_LastLabel,	MSG_SCREENPANEL_TIME_BOTH_TXT,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		MX_KIND,
						LA_LabelID,		MSG_SCREENPANEL_COLOUR_GAD,
						LA_WORD,		&PrivateConfig->ScreenConfig->ColourMode,
						LA_ID,			GAD_COLOURMODE,
						LAMX_FirstLabel,MSG_SCREENPANEL_COLOUR_AMIGA_TXT,
						LAMX_LastLabel,	MSG_SCREENPANEL_COLOUR_MONO_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_SCREENPANEL_BLINKING_GAD,
						LA_ID,			GAD_BLINKING,
						LA_BYTE,		&PrivateConfig->ScreenConfig->Blinking,
						GA_Disabled,	PrivateConfig->ScreenConfig->ColourMode == COLOUR_MONO,
						LA_ExtraSpace,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				if(!FixedColours)
				{
					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LONG Depth = 1;

						switch(PrivateConfig->ScreenConfig->ColourMode)
						{
							case COLOUR_MONO:

								Depth = 1;
								break;

							case COLOUR_AMIGA:

								Depth = 2;
								break;

							case COLOUR_EIGHT:

								Depth = 3;
								break;

							case COLOUR_SIXTEEN:

								Depth = 4;
								break;
						}

						if(PrivateConfig->ScreenConfig->UseWorkbench)
						{
							LONG i,MaxDepth = 4;

							for(i = 0 ; i < 4 ; i++)
							{
								if((1L << i) > Window->WScreen->ViewPort.ColorMap->Count)
								{
									MaxDepth = i;

									break;
								}
							}

							if(Depth > MaxDepth)
								Depth = MaxDepth;
						}

						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							STRPTR ColourFormat;
							LONG ColourMax;

							if(Kick30)
							{
								ColourMax		= 255;
								ColourFormat	= "%3ld";
							}
							else
							{
								ColourMax		= 15;
								ColourFormat	= "%2ld";
							}

							LT_New(Handle,
								LA_Type,			PALETTE_KIND,
								LA_LabelID,			MSG_SCREENPANEL_EDIT_SCREEN_PALETTE_GAD,
								LA_ID,				GAD_PALETTE,
								LA_Chars,			12,
								GTPA_Depth,			Depth,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,			SliderType,
								LA_LabelID,			MSG_SCREENPANEL_RED_GAD,
								LA_ID,				GAD_RED,
								GTSL_Min,			0,
								GTSL_Max,			ColourMax,
								GTSL_Level,			0,
								GTSL_LevelFormat,	ColourFormat,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,			SliderType,
								LA_LabelID,			MSG_SCREENPANEL_GREEN_GAD,
								LA_ID,				GAD_GREEN,
								GTSL_Min,			0,
								GTSL_Max,			ColourMax,
								GTSL_Level,			0,
								GTSL_LevelFormat,	ColourFormat,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,			SliderType,
								LA_LabelID,			MSG_SCREENPANEL_BLUE_GAD,
								LA_ID,				GAD_BLUE,
								GTSL_Min,			0,
								GTSL_Max,			ColourMax,
								GTSL_Level,			0,
								GTSL_LevelFormat,	ColourFormat,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,			XBAR_KIND,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	BUTTON_KIND,
								LA_LabelID,	MSG_SCREENPANEL_USE_DEFAULT_TXT,
								LA_ID,		GAD_RESET,
								LA_Chars,	16,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_EndGroup(Handle);
					}

					if(Kick30)
					{
						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	VERTICAL_KIND,
							TAG_DONE);
							{
								LT_New(Handle,
									LA_Type,		CHECKBOX_KIND,
									LA_LabelID,		MSG_PENPANEL_STANDARDPENS_TXT,
									LA_ID,			GAD_STDPENS,
									LA_BYTE,		&PrivateConfig->ScreenConfig->UsePens,
									GA_Disabled,	PrivateConfig->ScreenConfig->UseWorkbench,
								TAG_DONE);

								LT_EndGroup(Handle);
							}

							LT_New(Handle,
								LA_Type,	VERTICAL_KIND,
							TAG_DONE);
							{
								LT_New(Handle,
									LA_Type,		BUTTON_KIND,
									LA_LabelID,		MSG_PENPANEL_EDITPENS_TXT,
									LA_ID,			GAD_EDITPENS,
									GA_Disabled,	PrivateConfig->ScreenConfig->UsePens || PrivateConfig->ScreenConfig->UseWorkbench,
								TAG_DONE);

								LT_EndGroup(Handle);
							}

							LT_EndGroup(Handle);
						}
					}
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullSize,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,LA_Type,HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_USE_GAD,
					LA_ID,			GAD_USE,
					LABT_ReturnKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
					LA_ID,			GAD_CANCEL,
					LABT_EscKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LocalWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_SCREENPANEL_SCREEN_PREFERENCES_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage	*Message;
			struct ViewPort *LocalVPort;
			LONG ColourNumber,Count,i;
			struct Gadget *MsgGadget;
			WORD Red,Green,Blue;
			UWORD *DefaultPens = NULL;
			ULONG MsgClass;
			UWORD MsgCode;
			LONG Depth = 1;
			BOOL Done;

			ColourNumber = 0;
			LocalVPort = &LocalWindow->WScreen->ViewPort;

			switch(PrivateConfig->ScreenConfig->ColourMode)
			{
				case COLOUR_MONO:

					DefaultPens = MonoPens;
					Depth = 1;
					break;

				case COLOUR_AMIGA:

					DefaultPens = StandardPens;
					Depth = 2;
					break;

				case COLOUR_EIGHT:

					DefaultPens = ANSIPens;
					Depth = 3;
					break;

				case COLOUR_SIXTEEN:

					if(Kick30)
						DefaultPens = NewEGAPens;
					else
						DefaultPens = EGAPens;

					Depth = 4;
					break;
			}

			for(Count = 0, i = DETAILPEN ; i <= BARTRIMPEN ; i++)
			{
				if(PrivateConfig->ScreenConfig->PenArray[i])
					Count++;
			}

			if(!Count)
				CopyMem(DefaultPens,PrivateConfig->ScreenConfig->PenArray,16 * sizeof(UWORD));

			PrivateConfig->ScreenConfig->DisplayMode = DisplayMode;

			if(!FixedColours)
			{
				if(Kick30)
				{
					Red		= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 0] >> 24;
					Green	= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 1] >> 24;
					Blue	= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 2] >> 24;
				}
				else
				{
					Red		= (PrivateConfig->ScreenConfig->Colours[ColourNumber] >> 8) & 0xF;
					Green	= (PrivateConfig->ScreenConfig->Colours[ColourNumber] >> 4) & 0xF;
					Blue	= (PrivateConfig->ScreenConfig->Colours[ColourNumber]     ) & 0xF;
				}

				LT_SetAttributes(Handle,GAD_RED,	GTSL_Level,	Red,	TAG_DONE);
				LT_SetAttributes(Handle,GAD_GREEN,	GTSL_Level,	Green,	TAG_DONE);
				LT_SetAttributes(Handle,GAD_BLUE,	GTSL_Level,	Blue,	TAG_DONE);

				PaletteSetup(PrivateConfig);

				LoadColourTable(VPort,		NormalColourTable,NormalColours,PaletteSize);
				LoadColourTable(LocalVPort,	NormalColourTable,NormalColours,PaletteSize);
			}

			GuideContext(CONTEXT_SCREEN);

			PushWindow(LocalWindow);

			LT_ShowWindow(Handle,TRUE);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(LocalWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgCode		= Message->Code;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP || MsgClass == IDCMP_MOUSEMOVE || MsgClass == IDCMP_GADGETDOWN)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_RED:

								if(Kick30)
									Red = MsgCode;
								else
									Red = (MsgCode << 4) | MsgCode;

								PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 0] = SPREAD(Red);

								PaletteSetup(PrivateConfig);

								LoadColourTable(VPort,		NormalColourTable,NormalColours,PaletteSize);
								LoadColourTable(LocalVPort,	NormalColourTable,NormalColours,PaletteSize);

								break;

							case GAD_GREEN:

								if(Kick30)
									Green = MsgCode;
								else
									Green = (MsgCode << 4) | MsgCode;

								PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 1] = SPREAD(Green);

								PaletteSetup(PrivateConfig);

								LoadColourTable(VPort,		NormalColourTable,NormalColours,PaletteSize);
								LoadColourTable(LocalVPort,	NormalColourTable,NormalColours,PaletteSize);

								break;

							case GAD_BLUE:

								if(Kick30)
									Blue = MsgCode;
								else
									Blue = (MsgCode << 4) | MsgCode;

								PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 2] = SPREAD(Blue);

								PaletteSetup(PrivateConfig);

								LoadColourTable(VPort,		NormalColourTable,NormalColours,PaletteSize);
								LoadColourTable(LocalVPort,	NormalColourTable,NormalColours,PaletteSize);

								break;

							case GAD_STDPENS:

								LT_SetAttributes(Handle,GAD_EDITPENS,
									GA_Disabled,PrivateConfig->ScreenConfig->UsePens || PrivateConfig->ScreenConfig->UseWorkbench || FixedColours,
								TAG_DONE);

								break;

							case GAD_USE_PUBSCREEN:

								LT_SetAttributes(Handle,GAD_PUBSCREENNAME,		GA_Disabled,!PrivateConfig->ScreenConfig->UseWorkbench,TAG_DONE);
								LT_SetAttributes(Handle,GAD_MAKESCREENPUBLIC,	GA_Disabled,PrivateConfig->ScreenConfig->UseWorkbench,TAG_DONE);
								LT_SetAttributes(Handle,GAD_SCREENTITLE,		GA_Disabled,PrivateConfig->ScreenConfig->UseWorkbench,TAG_DONE);
								LT_SetAttributes(Handle,GAD_WINDOW_BORDER,		GA_Disabled,PrivateConfig->ScreenConfig->UseWorkbench,TAG_DONE);
								LT_SetAttributes(Handle,GAD_BLINKING,			GA_Disabled,PrivateConfig->ScreenConfig->ColourMode == COLOUR_MONO,TAG_DONE);
								LT_SetAttributes(Handle,GAD_SPLIT_STATUS,		GA_Disabled,!PrivateConfig->ScreenConfig->StatusLine || (!PrivateConfig->ScreenConfig->ShareScreen && !PrivateConfig->ScreenConfig->UseWorkbench),TAG_DONE);

								FixedColours = CannotChangeColours(PrivateConfig,LocalConfig->ScreenConfig->ColourMode);

								LT_SetAttributes(Handle,GAD_PALETTE,			GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_RED,				GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_GREEN,				GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_BLUE,				GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_RESET,				GA_Disabled,FixedColours,TAG_DONE);

								LT_SetAttributes(Handle,GAD_EDITPENS,
									GA_Disabled,	PrivateConfig->ScreenConfig->UsePens || PrivateConfig->ScreenConfig->UseWorkbench || FixedColours,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_STDPENS,
									GTCB_Checked,	PrivateConfig->ScreenConfig->UsePens = TRUE,
									GA_Disabled,	PrivateConfig->ScreenConfig->UseWorkbench || FixedColours,
								TAG_DONE);

								break;

							case GAD_WINDOW_BORDER:

								LT_SetAttributes(Handle,GAD_SPLIT_STATUS,GA_Disabled,!PrivateConfig->ScreenConfig->StatusLine || (!PrivateConfig->ScreenConfig->ShareScreen && !PrivateConfig->ScreenConfig->UseWorkbench),TAG_DONE);
								break;

							case GAD_COLOURMODE:

								if(!ModeOkay(PrivateConfig->ScreenConfig->DisplayMode) && PrivateConfig->ScreenConfig->ColourMode > COLOUR_AMIGA)
									LT_SetAttributes(Handle,GAD_COLOURMODE,GTCY_Active,COLOUR_AMIGA,TAG_DONE);

								FixedColours = CannotChangeColours(PrivateConfig,LocalConfig->ScreenConfig->ColourMode);

								LT_SetAttributes(Handle,GAD_PALETTE,	GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_RED,		GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_GREEN,		GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_BLUE,		GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_RESET,		GA_Disabled,FixedColours,TAG_DONE);
								LT_SetAttributes(Handle,GAD_BLINKING,	GA_Disabled,PrivateConfig->ScreenConfig->ColourMode == COLOUR_MONO,TAG_DONE);

								switch(PrivateConfig->ScreenConfig->ColourMode)
								{
									case COLOUR_MONO:

										DefaultPens = MonoPens;
										Depth = 1;
										break;

									case COLOUR_AMIGA:

										DefaultPens = StandardPens;
										Depth = 2;
										break;

									case COLOUR_EIGHT:

										DefaultPens = ANSIPens;
										Depth = 3;
										break;

									case COLOUR_SIXTEEN:

										DefaultPens = EGAPens;
										Depth = 4;
										break;
								}

								LT_SetAttributes(Handle,GAD_EDITPENS,
									GA_Disabled,	FixedColours,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_STDPENS,
									GA_Disabled,	FixedColours,
								TAG_DONE);

								Default2CurrentPalette(PrivateConfig);

								break;

							case GAD_EDITPENS:

								LT_LockWindow(LocalWindow);

								if(PrivateConfig->ScreenConfig->PenColourMode != PrivateConfig->ScreenConfig->ColourMode)
								{
									UWORD LocalPens[16];

									CopyMem(DefaultPens,LocalPens,16 * sizeof(UWORD));

									if(ScreenPenPanel(LocalWindow,LocalPens,DefaultPens,Depth))
									{
										CopyMem(LocalPens,PrivateConfig->ScreenConfig->PenArray,16 * sizeof(UWORD));

										PrivateConfig->ScreenConfig->PenColourMode = PrivateConfig->ScreenConfig->ColourMode;
									}
								}
								else
									ScreenPenPanel(LocalWindow,PrivateConfig->ScreenConfig->PenArray,DefaultPens,Depth);

								LT_UnlockWindow(LocalWindow);

								LT_ShowWindow(Handle,TRUE);
								break;

							case GAD_PALETTE:

								ColourNumber = MsgCode;

								if(Kick30)
								{
									Red		= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 0] >> 24;
									Green	= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 1] >> 24;
									Blue	= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 2] >> 24;
								}
								else
								{
									Red		= (PrivateConfig->ScreenConfig->Colours[ColourNumber] >> 8) & 0xF;
									Green	= (PrivateConfig->ScreenConfig->Colours[ColourNumber] >> 4) & 0xF;
									Blue	= (PrivateConfig->ScreenConfig->Colours[ColourNumber]     ) & 0xF;
								}

								LT_SetAttributes(Handle,GAD_RED,	GTSL_Level,	Red,	TAG_DONE);
								LT_SetAttributes(Handle,GAD_GREEN,	GTSL_Level,	Green,	TAG_DONE);
								LT_SetAttributes(Handle,GAD_BLUE,	GTSL_Level,	Blue,	TAG_DONE);

								break;

							case GAD_RESET:

								Default2CurrentPalette(PrivateConfig);

								if(Kick30)
								{
									Red		= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 0] >> 24;
									Green	= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 1] >> 24;
									Blue	= PrivateConfig->ScreenConfig->Colours96[ColourNumber * 3 + 2] >> 24;
								}
								else
								{
									Red		= (PrivateConfig->ScreenConfig->Colours[ColourNumber] >> 8) & 0xF;
									Green	= (PrivateConfig->ScreenConfig->Colours[ColourNumber] >> 4) & 0xF;
									Blue	= (PrivateConfig->ScreenConfig->Colours[ColourNumber]     ) & 0xF;
								}

								LT_SetAttributes(Handle,GAD_RED,	GTSL_Level,	Red,	TAG_DONE);
								LT_SetAttributes(Handle,GAD_GREEN,	GTSL_Level,	Green,	TAG_DONE);
								LT_SetAttributes(Handle,GAD_BLUE,	GTSL_Level,	Blue,	TAG_DONE);

								PaletteSetup(PrivateConfig);

								LoadColourTable(VPort,		NormalColourTable,NormalColours,PaletteSize);
								LoadColourTable(LocalVPort,	NormalColourTable,NormalColours,PaletteSize);

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								if(!FixedColours)
									PaletteSetup(PrivateConfig);
								else
								{
									if(PrivateConfig->ScreenConfig->PenColourMode != PrivateConfig->ScreenConfig->ColourMode)
										Default2CurrentPalette(PrivateConfig);
								}

								if(PrivateConfig->ScreenConfig->PenColourMode != PrivateConfig->ScreenConfig->ColourMode)
								{
									CopyMem(DefaultPens,PrivateConfig->ScreenConfig->PenArray,16 * sizeof(UWORD));

									PrivateConfig->ScreenConfig->PenColourMode = PrivateConfig->ScreenConfig->ColourMode;
								}

								UseIt = Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_FONT:

								if(OpenAnyFont(LocalWindow,PrivateConfig->ScreenConfig->FontName,&PrivateConfig->ScreenConfig->FontHeight))
								{
									BuildFontName(FontName,sizeof(FontName),PrivateConfig->ScreenConfig->FontName,PrivateConfig->ScreenConfig->FontHeight);

									LT_SetAttributes(Handle,GAD_FONT,
										GTTX_Text,	FontName,
									TAG_DONE);
								}

								break;

							case GAD_SCREENMODE:

								LT_LockWindow(LocalWindow);

								if(PickDisplayMode(LocalWindow,&PrivateConfig->ScreenConfig->DisplayMode,PrivateConfig))
								{
									GetModeName(PrivateConfig->ScreenConfig->DisplayMode,ModeName,sizeof(ModeName));

									LT_SetAttributes(Handle,GAD_SCREENMODE,GTTX_Text,ModeName,TAG_DONE);
								}

								LT_UnlockWindow(LocalWindow);

								LT_ShowWindow(Handle,TRUE);

								break;

							case GAD_STATUS_LINE:

								LT_SetAttributes(Handle,GAD_SPLIT_STATUS,GA_Disabled,!PrivateConfig->ScreenConfig->StatusLine || (!PrivateConfig->ScreenConfig->ShareScreen && !PrivateConfig->ScreenConfig->UseWorkbench),TAG_DONE);

								break;

							case GAD_PUBSCREENNAME:

								if(PrivateConfig->ScreenConfig->UseWorkbench)
								{
									UBYTE DummyBuffer[MAXPUBSCREENNAME + 1];
									UBYTE LocalBuffer[256];

									LT_LockWindow(LocalWindow);

									strcpy(DummyBuffer,LT_GetString(Handle,GAD_PUBSCREENNAME));
									strcpy(LocalBuffer,DummyBuffer);

									if(PickScreen(LocalWindow,DummyBuffer))
									{
										if(strcmp(DummyBuffer,LocalBuffer))
											LT_SetAttributes(Handle,GAD_PUBSCREENNAME,GTST_String,DummyBuffer,TAG_DONE);
									}

									LT_UnlockWindow(LocalWindow);

									LT_ShowWindow(Handle,TRUE);
								}

								break;
						}
					}
				}
			}
			while(!Done);

			PopWindow();

			if(((UseIt && Config != LocalConfig) || (!UseIt && Config == LocalConfig)) && !Config->ScreenConfig->UseWorkbench && Screen)
			{
				PaletteSetup(Config);

				LoadColourTable(VPort,		NormalColourTable,NormalColours,PaletteSize);
				LoadColourTable(LocalVPort,	NormalColourTable,NormalColours,PaletteSize);
			}
		}

		LT_DeleteHandle(Handle);
	}

	if(UseIt)
		SwapConfig(LocalConfig,PrivateConfig);
	else
		SaveConfig(LocalConfig,PrivateConfig);

	return(UseIt);
}

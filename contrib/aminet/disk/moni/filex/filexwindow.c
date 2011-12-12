#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <graphics/text.h>
#include <graphics/gfxmacros.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/diskfont.h>

#include <clib/alib_protos.h>
#endif

#include <string.h>
#include "filexstructs.h"
#include "filexstrings.h"
#include "allprotos.h"
#include "windowinfo.h"

struct MsgPort *GadgetReplyPort;

void DoGadgetReplyPortMsg( void )
{
	struct Message *Msg;

	while(( Msg = GetMsg( GadgetReplyPort )))
		FreeVec( Msg );
}

BOOL InitReplyPort( void )
{
	if(( GadgetReplyPort = CreateMsgPort()))
	{
		MyAddSignal( 1 << GadgetReplyPort->mp_SigBit, &DoGadgetReplyPortMsg );
		return( TRUE );
	}
	else
		return( FALSE );
}

void FreeReplyPort( void )
{
	struct Message *Msg;

	MyRemoveSignal( 1 << GadgetReplyPort->mp_SigBit );

	while(( Msg = GetMsg( GadgetReplyPort )))
		FreeVec( Msg );

	if( GadgetReplyPort )
		DeleteMsgPort( GadgetReplyPort );
}

/*
 * static void NewCalcCleanup( struct WindowInfo *wi )
 * 
 * Gibt den durch CalcSetup allokierten Speicher wieder frei.
 */

void NewCalcCleanup( struct WindowInfo *wi )
{
	if( wi->TF )
	{
		CloseFont( wi->TF );

		wi->TF = NULL;
	}

	if( wi )
		FreeMem( wi, sizeof( struct WindowInfo ));
}


/*
 * struct WindowInfo *NewCalcSetup( struct Screen *Screen )
 * 
 * Ermittelt Werte für die Berechnung.
 */

struct WindowInfo *NewCalcSetup( struct Screen *Screen )
{
	struct WindowInfo *wi;

	if(( wi = AllocMem( sizeof( struct WindowInfo ), MEMF_CLEAR )))
	{
		InitRastPort( &wi->RP );

		memcpy(&wi->TA, Screen->Font, sizeof(struct TextAttr));

		if( DiskfontBase )
			wi->TF = OpenDiskFont( &wi->TA );
		else
			wi->TF = OpenFont( &wi->TA );

		if( wi->TF )
		{
			WORD k, Width;
			UBYTE Char;

			SetFont( &wi->RP, wi->TF );

			wi->AverageCharWidth = 0;
			wi->MaxCharWidth = 0;
			wi->MaxNumericWidth = 0;

			for( k = 0; k < 256; k++ )
			{
				Char = k;

				Width = TextLength( &wi->RP, &Char, 1 );

				if( Width > wi->MaxCharWidth )
					wi->MaxCharWidth = Width;

				wi->AverageCharWidth += Width;
			}

			wi->AverageCharWidth /= 256;

			for( k = '0'; k <= '9'; k++ )
			{
				Char = k;

				if((Width = TextLength(&wi->RP,&Char,1)) > wi->MaxNumericWidth)
					wi->MaxNumericWidth = Width;
			}

			if((Width = TextLength(&wi->RP," ",1)) > wi->MaxNumericWidth)
				wi->MaxNumericWidth = Width;

			if((Width = TextLength(&wi->RP,",",1)) > wi->MaxNumericWidth)
				wi->MaxNumericWidth = Width;

			if((Width = TextLength(&wi->RP,".",1)) > wi->MaxNumericWidth)
				wi->MaxNumericWidth = Width;

			if(wi->AverageCharWidth > wi->MaxNumericWidth)
				wi->MaxNumericWidth = wi->AverageCharWidth;

			wi->MaxHexWidth = wi->MaxNumericWidth;

			for( k = 'a'; k <= 'f'; k++ )
			{
				Char = k;

				if((Width = TextLength(&wi->RP,&Char,1)) > wi->MaxHexWidth)
					wi->MaxHexWidth = Width;
			}

			for( k = 'A'; k <= 'F'; k++ )
			{
				Char = k;

				if((Width = TextLength(&wi->RP,&Char,1)) > wi->MaxHexWidth)
					wi->MaxHexWidth = Width;
			}
		}
		else
		{
			NewCalcCleanup( wi );
			wi = 0;
		}
	}

	return( wi );
}

/*
 * void SetzeMuster(struct Window *Wnd)
 *
 * Füllt den Windowhintergrund mit einem Muster.
 */

void SetzeMuster(struct Window *Wnd)
{
	if(( Wnd->BorderLeft < Wnd->Width - 1 - Wnd->BorderRight ) &&
		( Wnd->BorderTop < Wnd->Height - 1 - Wnd->BorderBottom ))
	{
		UWORD fuellmuster[] = { 0xaaaa, 0x5555 };

		SetAPen( Wnd->RPort, 2 );
		SetAfPt( Wnd->RPort, fuellmuster, 1 );

		RectFill( Wnd->RPort, Wnd->BorderLeft, Wnd->BorderTop, Wnd->Width - 1 - Wnd->BorderRight, Wnd->Height - 1 - Wnd->BorderBottom );

		SetAfPt( Wnd->RPort, 0, 0 );
		SetAPen( Wnd->RPort, 1 );
	}
}


/*
 * void MyFilledDrawBevelBox(struct RastPort *rp,long x,long y,long b,long h,BOOL rec)
 * 
 * Zeichnet eine mit Pen 0 ausgefüllte Bevelbox.
 */

void MyFilledDrawBevelBox(struct RastPort *rp,long x,long y,long b,long h,BOOL rec)
{
	DrawBevelBox( rp, x, y, b, h, GT_VisualInfo, VisualInfo, GTBB_Recessed, rec, TAG_DONE );

	SetAPen( rp, 0 );
	RectFill( rp, x + 2, y + 1, x + b - 3, y + h - 2 );
	SetAPen( rp, 1);
}


/*
 * long MyTextLength( struct RastPort *rp, char *ptr)
 * 
 * Ermittelt die Breite in Pixeln einer Zeichenkette(ohne den Unterstrich '_').
 */

long MyTextLength( struct RastPort *rp, CONST_STRPTR ptr)
{
	long len = 0;
	char text[40];

	while(*ptr)
		if(*ptr != '_') text[len++] = *ptr++;
		else ptr++;

	return(TextLength(rp, text, len));
}

static WORD SpaltenOffsets[ 10 ];

/*
 * static long GetWidth(struct MyNewGadget *NewGadget)
 * 
 * Ermittelt die Maximalbreite aller Gadgettitel
 */

static long GetWidth( struct MyNewGadget *NewGadget, struct WindowInfo *wi)
{
	WORD MaxWidth = 0, Width, SpaltenBreite = 0;
	WORD InhaltBreite, MaxInhaltBreite = 0;
	int k = 0;

	while(NewGadget->Typ)
	{
		if( NewGadget->Pos == GP_NEWCOLUMN )
		{
			SpaltenOffsets[ k++ ] = SpaltenBreite + MaxWidth + ( MaxWidth ? TEXTGADGETX : 0 );
			SpaltenOffsets[ k++ ] = MaxInhaltBreite + MaxWidth + ( MaxWidth ? TEXTGADGETX : 0 );

			SpaltenBreite += MaxWidth + INNERGADGETX + MaxInhaltBreite + ( MaxWidth ? TEXTGADGETX : 0 );

			MaxWidth = 0;
			MaxInhaltBreite = 0;
		}

		switch(NewGadget->Typ)
		{
			case INTEGER_KIND:
			case SLIDER_KIND:
			case HEX_KIND:
			case STRING_KIND:
				InhaltBreite = Max(NewGadget->MinHexs * wi->MaxNumericWidth + 12, NewGadget->MinChars * wi->AverageCharWidth + 12);
				Width = MyTextLength(&wi->RP, NewGadget->Title);
				break;

			case CYCLE_KIND:
				InhaltBreite = Max(NewGadget->MinHexs * wi->MaxNumericWidth + 12, NewGadget->MinChars * wi->AverageCharWidth + 12);
				Width = MyTextLength(&wi->RP, NewGadget->Title);
				break;

			case CHECKBOX_KIND:
				InhaltBreite = (wi->TF->tf_YSize + 3) * CHECKBOX_WIDTH / CHECKBOX_HEIGHT;
				Width = MyTextLength(&wi->RP, NewGadget->Title);
				break;

			case LISTVIEW_KIND:
				{
					long Dummy = MyTextLength(&wi->RP, NewGadget->Title);

					InhaltBreite = Max( Dummy, Max(NewGadget->MinHexs * wi->MaxNumericWidth + 12, NewGadget->MinChars * wi->AverageCharWidth + 12));
					Width = 0;
				}
				break;

			default:
				Width = InhaltBreite = 0;
				break;
		}

		if(Width > MaxWidth)
			MaxWidth = Width;

		if(InhaltBreite > MaxInhaltBreite )
			MaxInhaltBreite = InhaltBreite;

		NewGadget++;
	}

	SpaltenOffsets[ k++ ] = SpaltenBreite + MaxWidth + ( MaxWidth ? TEXTGADGETX : 0 );
	SpaltenOffsets[ k ] = SpaltenBreite + MaxInhaltBreite + MaxWidth + ( MaxWidth ? TEXTGADGETX : 0 );
	SpaltenBreite += MaxWidth + MaxInhaltBreite + ( MaxWidth ? TEXTGADGETX : 0 );

	return(SpaltenBreite);
}


/*
 * static long GetButtonWidth(struct MyNewGadget *NewGadget)
 * 
 * Ermittelt die Maximalbreite aller Buttons
 */

static long GetButtonWidth(struct MyNewGadget *NewGadget, WORD *ButtonCount, struct WindowInfo *wi)
{
	WORD MaxWidth = -100, Width;

	*ButtonCount = 0;

	while(NewGadget->Typ)
	{
		if( NewGadget->Typ == BUTTON_KIND )
		{
			( *ButtonCount )++;

			Width = MyTextLength(&wi->RP, NewGadget->Title);

			if(Width == 0)
				Width = -TEXTGADGETX;

			if(Width > MaxWidth)
				MaxWidth = Width;
		}

		NewGadget++;
	}

	return(MaxWidth + 16);
}


/*
 * static long GetHeight(struct MyNewGadget *NewGadget)
 * 
 * Ermittelt die Gesamthöhe aller Gadgets in der Bevelbox
 */

static long GetHeight(struct MyNewGadget *NewGadget, struct WindowInfo *wi )
{
	WORD Height = 0, MaxHeight = 0;

	while(NewGadget->Typ)
	{
		switch(NewGadget->Pos)
		{
			case GP_LEFTBOTTOM:
			case GP_RIGHTBOTTOM:
			case GP_MIDDLEBOTTOM:
				break;

			case GP_NEWCOLUMN:
				if( Height > MaxHeight )
					MaxHeight = Height;
				Height = 0;

			default:
				switch(NewGadget->Typ)
				{
					case INTEGER_KIND:
					case HEX_KIND:
					case STRING_KIND:
					case CYCLE_KIND:
					case BUTTON_KIND:
						Height += wi->TF->tf_YSize + 6 + INNERGADGETY;
						break;
					case SLIDER_KIND:
						Height += wi->TF->tf_YSize + 2 + INNERGADGETY;
						break;
					case CHECKBOX_KIND:
						Height += wi->TF->tf_YSize + 3 + INNERGADGETY;
						break;
					case LISTVIEW_KIND:
						Height += wi->TF->tf_YSize * ( LISTVIEWLINES + 1 ) + LISTVIEWTEXTGADGETY + LISTVIEWBORDERY + INNERGADGETY;
						if( !Kick30 ) Height += LISTVIEWBORDERY20;
						break;
				}
				break;
		}

		NewGadget++;
	}

	if( Height > MaxHeight )
		MaxHeight = Height;

	return(MaxHeight - INNERGADGETY);
}


/*
 * GetUnderlinedKey(UBYTE *String)
 * 
 * Liefert den Buchstaben nach dem '_' oder 0, falls kein '_' vorhanden.
 */

UBYTE GetUnderlinedKey(CONST_STRPTR String)
{
	while(*String && (*String != '_')) String++;

	if(*String && *(String+1)) return(*(String+1));
	else return(0);
}

/*
 * int NewOpenAWindow( struct WindowData *WD, CONST_STRPTR Title );
 */

int NewOpenAWindow( struct WindowData *WD, CONST_STRPTR Title )
{
	struct Gadget **Gadgets;

	if( !( Gadgets = WD->Gadgets = AllocVec( WD->GadgetZahl * 4+4, MEMF_CLEAR )))
		return( 5L );

	if(( WD->WI = NewCalcSetup( Scr )))
	{
		struct NewGadget ng;
		struct MyNewGadget *Gad;
		struct Gadget *g;
		WORD MaxWidth, Width, Height, ButtonCount, ButtonWidth, GadgetWidth;
		ULONG GadgetTyp;
		WORD SpaltenNummer = 0;

		static struct TagItem SliderTags[9] =
		{
			{GTSL_Min, 0},
			{GTSL_Max, 0},
			{GTSL_Level, 0},
			{GTSL_MaxLevelLen, 6},
			{GTSL_LevelFormat, (IPTR)"%4.ld"},
			{PGA_Freedom, LORIENT_HORIZ},
			{GA_RelVerify, TRUE},
			{GTSL_LevelPlace, PLACETEXT_RIGHT},
			{TAG_DONE}
		};
		
		static struct TagItem Tags[3] =
		{{GT_Underscore, '_'}, {GA_Disabled, 0}, {0, 0}};
		
		static struct TagItem StringTags[4] =
		{{GTST_String, 0}, {GTST_MaxChars, 0}, {GTST_EditHook, (LONG) 0}, {TAG_DONE}};
		
		static struct TagItem IntegerTags[2] =
		{{GTIN_Number, 0}, {TAG_DONE}};
		
		static struct TagItem CheckboxTags[3] =
		{{GTCB_Checked, 0}, {GTCB_Scaled, TRUE}, {TAG_DONE}};
		
		static struct TagItem ListviewTags[4] =
		{{GTLV_Labels, 0}, {GTLV_ShowSelected, 0}, {GTLV_Selected, 0}, {TAG_DONE}};

		static struct TagItem CycleTags[3] =
		{{GTCY_Labels, 0}, {GTCY_Active, 0}, {TAG_DONE}};

			/* Falls die Gadgets noch nicht lokalisiert wurden, */
			/* sie lokalisieren */

		if( !WD->SchonMalGeoeffnet )
		{
			Gad = WD->NG;

			while( Gad->Typ )
			{
				Gad->Title = (UBYTE *)GetStr( Gad->Title );
				Gad->Key = GetUnderlinedKey( Gad->Title );

				if( Gad->Typ == CYCLE_KIND )
				{
					CONST_STRPTR *String = ( CONST_STRPTR * ) Gad->CurrentValue;

					while( *String )
					{
						*String = GetStr( *String );
						String++;
					}
				}

				Gad++;
			}
		}

			/* Breite der Buttons und Anzahl ermitteln */

		ButtonWidth = GetButtonWidth(WD->NG, &ButtonCount, WD->WI);

			/* Breite des Fensters = */
			/* Maximale Gadgettextbreite */
			/* + maximale Gadgetbreite */
			/* + 2 * BORDERX Pixel für den gefüllten Freiraum */
			/* + 2 * BEVELX Pixel für die Bevelbox */
			/* + 2 * INNERX Pixel für den Freiraum Gadget - Bevelbox */

		Width = (MaxWidth = GetWidth(WD->NG,WD->WI)) + 2 * BORDERX + 2 * BEVELX + 2 * INNERX;

			/* Falls die Buttons zusammen zu Breit sind, Window/Gadgetbreite */
			/* vergrößern */

		if( ButtonCount * ButtonWidth + (ButtonCount - 1) * INNERGADGETX > Width - 2 * BORDERX )
		{
			Width = ButtonCount * ButtonWidth + (ButtonCount - 1) * INNERGADGETX + 2 * BORDERX;
		}

			/* Höhe des Fensters = */
			/* Gesamthöhe der Gadgets in der Bevelbox */
			/* + 2 * BORDERY für den gefüllten Freiraum Window - Bevelbox */ 
			/* + 2 * BEVELY Pixel für die Bevelboxhöhe */
			/* + 2 * INNERY für den Freiraum Bevelbox - Gadgets */
			/* + BUTTONY für den Freiraum Buttons - Bevelbox */
			/* + WD->WI->TF->tf_YSize + 6 für die Buttongadgets */

		Height = GetHeight(WD->NG,WD->WI) + 2 * BORDERY + 2 * BEVELY + 2 * INNERY + BUTTONY + WD->WI->TF->tf_YSize + 6;

		if( !WD->SchonMalGeoeffnet )
		{
			WD->Left = AktuDI->Wnd->LeftEdge + WNDXOPENOFF;
			WD->Top = AktuDI->Wnd->TopEdge + WNDYOPENOFF;
		}

		if((WD->Left + Width + Scr->WBorLeft + Scr->WBorRight) > Scr->Width) WD->Left = Scr->Width - Width;
		if((WD->Top + Height + Scr->RastPort.TxHeight + Scr->WBorTop + 1 + Scr->WBorBottom) > Scr->Height) WD->Top = Scr->Height - Height;

		if(! (g = CreateContext( &WD->GList )))
			return(1L);

		ng.ng_VisualInfo = VisualInfo;
		ng.ng_TextAttr = &WD->WI->TA;
		ng.ng_Flags = PLACETEXT_LEFT;
		ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BEVELY + INNERY + BORDERY;
		ng.ng_GadgetID = 0;

		Gad = WD->NG;

		while(Gad->Typ)
		{
			GadgetTyp = Gad->Typ;

			Tags[1].ti_Data = Gad->Disabled;

			switch(Gad->Pos)
			{
				case GP_LEFTBOTTOM:
					ng.ng_LeftEdge = Scr->WBorLeft + BORDERX;
					break;

				case GP_MIDDLEBOTTOM:
					ng.ng_LeftEdge = Scr->WBorLeft + ( Width - ButtonWidth ) / 2;
					break;

				case GP_RIGHTBOTTOM:
					ng.ng_LeftEdge = Scr->WBorLeft + Width - ButtonWidth - BORDERX;
					break;

				case GP_NEWCOLUMN:
					SpaltenNummer+=2;
					ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BEVELY + INNERY + BORDERY;

				default:
					ng.ng_LeftEdge = Scr->WBorLeft + BORDERX + BEVELX + INNERX + SpaltenOffsets[ SpaltenNummer ]; 
					break;
			}

			GadgetWidth = SpaltenOffsets[ SpaltenNummer + 1 ] - SpaltenOffsets[ SpaltenNummer ];

			switch(Gad->Typ)
			{
				case INTEGER_KIND:
					ng.ng_Width = GadgetWidth;
					ng.ng_Height = WD->WI->TF->tf_YSize + 6;
					ng.ng_Flags = PLACETEXT_LEFT;

					Tags[2].ti_Tag = TAG_MORE;
					Tags[2].ti_Data = (LONG)IntegerTags;

					IntegerTags[0].ti_Data = Gad->CurrentValue;

					break;

				case HEX_KIND:
				case STRING_KIND:
					ng.ng_Width = GadgetWidth;
					ng.ng_Height = WD->WI->TF->tf_YSize + 6;
					ng.ng_Flags = PLACETEXT_LEFT;

					Tags[2].ti_Tag = TAG_MORE;
					Tags[2].ti_Data = (LONG)StringTags;

					StringTags[0].ti_Data = Gad->CurrentValue;
					StringTags[1].ti_Data = Gad->Max;

					if(Gad->Typ == HEX_KIND)
						GadgetTyp = STRING_KIND;

					StringTags[2].ti_Tag = GTST_EditHook;
					StringTags[2].ti_Data = (LONG) &SearchHook;
					
					break;

				case SLIDER_KIND:
					ng.ng_Width = GadgetWidth - 4 * WD->WI->MaxNumericWidth - 7;
					ng.ng_Height = WD->WI->TF->tf_YSize + 2;
					ng.ng_Flags = PLACETEXT_LEFT;

					Tags[2].ti_Tag = TAG_MORE;
					Tags[2].ti_Data = (LONG)SliderTags;
	
					SliderTags[0].ti_Data = Gad->Min;
					SliderTags[1].ti_Data = Gad->Max;
					SliderTags[2].ti_Data = Gad->CurrentValue;

					break;

				case CHECKBOX_KIND:
					ng.ng_Width = (WD->WI->TF->tf_YSize + 3) * CHECKBOX_WIDTH / CHECKBOX_HEIGHT;
					ng.ng_Height = WD->WI->TF->tf_YSize + 3;
					ng.ng_Flags = PLACETEXT_LEFT;

					Tags[2].ti_Tag = TAG_MORE;
					Tags[2].ti_Data = (LONG)CheckboxTags;

					CheckboxTags[0].ti_Data = Gad->CurrentValue;

					Gad->CurrentValue = Gad->CurrentValue;

					break;

				case BUTTON_KIND:
					ng.ng_Width = ButtonWidth;
					ng.ng_Height = WD->WI->TF->tf_YSize + 6;
					ng.ng_TopEdge = Height + Scr->RastPort.TxHeight + Scr->WBorTop + 1 - ng.ng_Height - BORDERY;
					ng.ng_Flags = PLACETEXT_IN;
					Tags[2].ti_Tag = TAG_DONE;
					break;

				case LISTVIEW_KIND:
					ng.ng_Width = GadgetWidth;
					ng.ng_Height = ( WD->WI->TF->tf_YSize * LISTVIEWLINES ) + LISTVIEWBORDERY;
					if( !Kick30 ) ng.ng_Height += LISTVIEWBORDERY20;
					ng.ng_TopEdge += WD->WI->TF->tf_YSize + LISTVIEWTEXTGADGETY;
					ng.ng_Flags = PLACETEXT_ABOVE;

					Tags[2].ti_Tag = TAG_MORE;
					Tags[2].ti_Data = (LONG) ListviewTags;

					ListviewTags[0].ti_Data = Gad->CurrentValue;

					Gad->CurrentValue = 0;

					break;

				case CYCLE_KIND:
					ng.ng_Width = GadgetWidth;
					ng.ng_Height = WD->WI->TF->tf_YSize + 6;

					Tags[2].ti_Tag = TAG_MORE;
					Tags[2].ti_Data = (LONG) CycleTags;

					CycleTags[0].ti_Data = Gad->CurrentValue;
					CycleTags[1].ti_Data = Gad->Min;

					Gad->CurrentValue = Gad->Min;

					break;
			}
	
			ng.ng_GadgetText = Gad->Title;

			*Gadgets++ = g = CreateGadgetA(GadgetTyp, g, &ng, Tags);

			if(!g) return(2L);

			g->UserData = Gad;
	
			ng.ng_TopEdge += ng.ng_Height + INNERGADGETY;
			Gad++;
			ng.ng_GadgetID++;
		}

		*Gadgets = 0;

		if ( ! ( WD->Wnd = OpenWindowTags( NULL,
					WA_Left,	WD->Left,
					WA_Top,		WD->Top,
					WA_InnerWidth,	Width,
					WA_InnerHeight,	Height,
					WA_IDCMP,	BUTTONIDCMP|INTEGERIDCMP|TEXTIDCMP|SLIDERIDCMP|IDCMP_INTUITICKS|IDCMP_MOUSEBUTTONS|IDCMP_CLOSEWINDOW|IDCMP_VANILLAKEY|IDCMP_REFRESHWINDOW,
					WA_Flags,	WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH|WFLG_ACTIVATE|WFLG_RMBTRAP,
					WA_Title,	Title,
					WA_ScreenTitle,	"FileX '94",
					WA_PubScreen,	Scr,
					TAG_DONE )))
		return(3L);
	
		SetzeMuster(WD->Wnd);
	
		MyFilledDrawBevelBox((WD->Wnd)->RPort,	(WD->Wnd)->BorderLeft + BORDERX,
															(WD->Wnd)->BorderTop + BORDERY,
															(WD->Wnd)->Width - (WD->Wnd)->BorderRight - (WD->Wnd)->BorderLeft - BORDERX * 2,
															(WD->Wnd)->Height - (WD->Wnd)->BorderBottom - (WD->Wnd)->BorderTop - 2 * BORDERY - WD->WI->TF->tf_YSize - 6 - BUTTONY,
															TRUE);
	
		AddGList(WD->Wnd, WD->GList, 0, -1, 0);
	
		RefreshGadgets(WD->GList, WD->Wnd, 0);
		GT_RefreshWindow(WD->Wnd, NULL);

		MyAddWindow( WD );
		BlockMainWindow();

		WD->SchonMalGeoeffnet = TRUE;

		return(0);
	}
	else
		return(4L);
}

/*
 * void NewCloseAWindow( struct WindowData *WD )
 * 
 * Schließ ein Fenster, gib die Gadgets frei und merkt sich die letzte Pos.
 */

void NewCloseAWindow( struct WindowData *WD )
{
	ReleaseMainWindow();

	MyRemoveWindow( WD );

	if( WD->Wnd )
	{
		WD->Left = WD->Wnd->LeftEdge;
		WD->Top = WD->Wnd->TopEdge;
		CloseWindow( WD->Wnd );
		WD->Wnd = NULL;
	}

	if( WD->GList )
	{
		FreeGadgets( WD->GList );
		WD->GList = NULL;
	}

	if( WD->Gadgets )
	{
		FreeVec( WD->Gadgets );
		WD->Gadgets = NULL;
	}

	if( WD->WI )
	{	
		NewCalcCleanup( WD->WI );
		WD->WI = NULL;
	}
	
}


/*
 * void KeySelect(struct Gadget **GadgetList, struct IntuiMessage *Msg)
 * 
 * Wandelt Tastaturnachrichten in entsprechende andere Nachrichten.
 */

void KeySelect(struct Gadget **GadgetList, struct IntuiMessage *Msg)
{
	if(Msg->Class == IDCMP_VANILLAKEY)
	{
			/* Drücken der ESC-Taste ggf. in ein CLOSEWINDOW umwandeln */

		if(Msg->Code == '\033' && (Msg->IDCMPWindow -> Flags & WFLG_CLOSEGADGET))
			Msg->Class = IDCMP_CLOSEWINDOW;
		else
		{
			struct MyNewGadget *MyNewGadget;
			BOOL Shift, GotIt = FALSE;

				/* Run down all the gadgets. */

			while((!GotIt) && ( *GadgetList ))
			{
				/* Is there a gadget? */

				MyNewGadget = (*GadgetList)->UserData;

					/* Do the keys match? */

				if(ToUpper(MyNewGadget->Key) == ToUpper(Msg->Code))
				{
					GotIt = TRUE;

						/* Groß oder Kleinbuchtabe?
						 * Shift gedrückt?
						 */

					if(ToUpper(MyNewGadget->Key) == Msg->Code)
						Shift = TRUE;
					else
						Shift = FALSE;

						/* Update the object information. */

					switch(MyNewGadget->Typ)
					{
						case HEX_KIND:
						case STRING_KIND:
						case INTEGER_KIND:
							ActivateGadget( *GadgetList, Msg->IDCMPWindow, NULL);

							Msg->Class = IDCMP_GADGETDOWN;
							Msg->Code = 0;
							Msg->IAddress = *GadgetList;

							break;

						case BUTTON_KIND:
							Msg->Class = IDCMP_GADGETUP;
							Msg->Code = 0;
							Msg->IAddress = *GadgetList;

							break;

						case CHECKBOX_KIND:
							GT_SetGadgetAttrs( *GadgetList, Msg->IDCMPWindow, NULL,
								GTCB_Checked,	MyNewGadget->CurrentValue ? FALSE : TRUE,
							TAG_DONE);

							Msg->Class = IDCMP_GADGETUP;
							Msg->IAddress = *GadgetList;

							break;

						case SLIDER_KIND:
							if(!Shift)
							{
								if(MyNewGadget->CurrentValue < MyNewGadget->Max)
								{
									GT_SetGadgetAttrs( *GadgetList, Msg->IDCMPWindow, NULL,
										GTSL_Level,	MyNewGadget->CurrentValue + 1,
									TAG_DONE);

									Msg->Class = IDCMP_MOUSEMOVE;
									Msg->Code = MyNewGadget->CurrentValue + 1;
									Msg->IAddress = *GadgetList;
								}
							}
							else
							{
								if(MyNewGadget->CurrentValue > MyNewGadget->Min)
								{
									GT_SetGadgetAttrs(*GadgetList, Msg->IDCMPWindow, NULL,
										GTSL_Level,	MyNewGadget->CurrentValue - 1,
									TAG_DONE);

									Msg->Class = IDCMP_MOUSEMOVE;
									Msg->Code = MyNewGadget->CurrentValue - 1;
									Msg->IAddress = *GadgetList;
								}
							}

							break;

						case LISTVIEW_KIND:
							if(!Shift)
							{
								if(MyNewGadget->CurrentValue < MyNewGadget->Max)
								{
									GT_SetGadgetAttrs( *GadgetList, Msg->IDCMPWindow, NULL,
										GTLV_Selected, MyNewGadget->CurrentValue + 1,
										GTLV_Top, MyNewGadget->CurrentValue + 1,
										GTLV_MakeVisible, MyNewGadget->CurrentValue + 1,
									TAG_DONE);
						
									Msg->Class = IDCMP_GADGETUP;
									Msg->Code = MyNewGadget->CurrentValue + 1;
									Msg->IAddress = *GadgetList;
								}
							}
							else
							{
								if(MyNewGadget->CurrentValue > 0)
								{
									GT_SetGadgetAttrs( *GadgetList, Msg->IDCMPWindow, NULL,
										GTLV_Selected, MyNewGadget->CurrentValue - 1,
										GTLV_Top, MyNewGadget->CurrentValue - 1,
										GTLV_MakeVisible, MyNewGadget->CurrentValue - 1,
									TAG_DONE);
						
									Msg->Class = IDCMP_GADGETUP;
									Msg->Code = MyNewGadget->CurrentValue - 1;
									Msg->IAddress = *GadgetList;
								}
							}

							break;

						case CYCLE_KIND:
							if( !Shift )
							{
								GT_SetGadgetAttrs( *GadgetList, Msg->IDCMPWindow, NULL,
									GTCY_Active, Msg->Code = ( MyNewGadget->CurrentValue == MyNewGadget->Max ) ? 0 : MyNewGadget->CurrentValue + 1,
								TAG_DONE);
							}
							else
							{
								GT_SetGadgetAttrs( *GadgetList, Msg->IDCMPWindow, NULL,
									GTCY_Active, Msg->Code = MyNewGadget->CurrentValue ? MyNewGadget->CurrentValue - 1 : MyNewGadget->Max,
								TAG_DONE);
							}

							Msg->Class = IDCMP_GADGETUP;
							Msg->IAddress = *GadgetList;

							break;
					}
				}

				GadgetList++;
			}
		}
	}
}


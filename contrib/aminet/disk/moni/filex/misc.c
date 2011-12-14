#ifdef USE_PROTO_INCLUDES
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>

#endif

#include "filexstructs.h"
#include "allprotos.h"
#include "filexstrings.h"

static char *DisplayStrings[4] =
{
	MSG_GADGET_MISC_HEX,
	MSG_GADGET_MISC_ASCII,
	MSG_GADGET_MISC_HEXANDASCII,
	0
};

static char *SpaceStrings[5] =
{
	MSG_GADGET_MISC_BYTE,
	MSG_GADGET_MISC_WORD,
	MSG_GADGET_MISC_LONG,
	MSG_GADGET_MISC_NO,
	0
};

enum {
	GD_MISC_USEASL,
	GD_MISC_OVERWRITE,
	GD_MISC_ALTSPRUNGWEITE,
	GD_MISC_SCROLLRAND,
	GD_MISC_MAXUNDOLEVEL,
	GD_MISC_MAXUNDOMEM,
	GD_MISC_CLIPBOARDUNIT,
	GD_MISC_PROGRAMPRIO,
	GD_MISC_COMMANDSHELLWINDOW,
	GD_MISC_COMMANDWINDOW,
	GD_MISC_DISPLAY,
	GD_MISC_SPACE,
	GD_MISC_OK,
	GD_MISC_CANCEL
};

static struct MyNewGadget MiscNewGadgets[] =
{
	{CHECKBOX_KIND, 0, 0, 0, MSG_GADGET_MISC_USEASLLIBRARY, 0, 0, 0, 0, 0},
	{CHECKBOX_KIND, 0, 0, 0, MSG_GADGET_MISC_OVERWRITE, 0, 0, 0, 0, 0},
	{SLIDER_KIND, 0/*GP_NEWCOLUMN*/, 0, 0, MSG_GADGET_MISC_ALTJUMP, 0, 30, 0, 7, 0},
	{SLIDER_KIND, 0, 0, 0, MSG_GADGET_MISC_SCROLLBORDER, 0, 99, 0, 7, 0},
	{SLIDER_KIND, 0, 0, 0, MSG_GADGET_MISC_MAXUNDOLEVEL, 2, 9999, 0, 7, 0},
	{SLIDER_KIND, 0, 0, 0, MSG_GADGET_MISC_MAXUNDOMEM, 10, 9999, 0, 7, 0},
	{SLIDER_KIND, 0, 0, 0, MSG_GADGET_MISC_CLIPBOARDUNIT, 0, 255, 0, 7, 0},
	{SLIDER_KIND, 0, 0, 0, MSG_GADGET_MISC_PROGRAMPRIORITY, -128, 127, 0, 7, 0},
	{STRING_KIND, 0, 0, 0, MSG_GADGET_MISC_COMMANDSHELLWINDOW, 0, 256, 0, 0, 30},
	{STRING_KIND, 0, 0, 0, MSG_GADGET_MISC_COMMANDWINDOW, 0, 256, 0, 0, 30},
	{CYCLE_KIND, 0, 0, 0, MSG_GADGET_MISC_DISPLAY, 0, 0, 0, 0, 0},
	{CYCLE_KIND, 0, 0, 0, MSG_GADGET_MISC_SPACE, 0, 0, 0, 0, 0},

	{BUTTON_KIND, GP_LEFTBOTTOM, 0, 0, MSG_GADGET_OK, 0, 0, 0, 0, 0},
	{BUTTON_KIND, GP_RIGHTBOTTOM, 0, 0, MSG_GADGET_CANCEL, 0, 0, 0, 0, 0},
	{0}
};

static struct WindowData MiscWD =
{
	NULL,	NULL,	FALSE, NULL, NULL,
	0,0,
	&MiscNewGadgets[ 0 ], 14
};

static void DoMiscWndMsg( void )
{
	struct IntuiMessage	*m, Msg;
	struct Gadget *gad;

	BOOL wie = -1;	/* 0 = Cancel, 1 = Ok, -1 = Window bleibt offen */

/*	kprintf("DoMiscWnd: %8lx %8lx\n", MiscWD.Wnd, MiscWD.Wnd->UserPort );*/

	while(( m = GT_GetIMsg( MiscWD.Wnd->UserPort )))
	{
		CopyMem((char *)m, (char *)&Msg, (long)sizeof(struct IntuiMessage));

		GT_ReplyIMsg(m);

		KeySelect(MiscWD.Gadgets, &Msg);

		gad = (struct Gadget *) Msg.IAddress;

		switch( Msg.Class )
		{
/*			case	IDCMP_REFRESHWINDOW:
				GT_BeginRefresh( MiscWD.Wnd );
				GT_EndRefresh( MiscWD.Wnd, TRUE );
				break;*/

			case	IDCMP_VANILLAKEY:
				if( Msg.Code == 13)
				{
					wie = 1;
				}
				break;

			case	IDCMP_CLOSEWINDOW:
				wie = 0;
				break;

			case IDCMP_MOUSEMOVE:
				((struct MyNewGadget *)gad->UserData)->CurrentValue = Msg.Code;
				break;

			case	IDCMP_GADGETUP:
				switch(gad->GadgetID)
				{
					case GD_MISC_OK:
						wie = 1;
						break;
					case GD_MISC_CANCEL:
						wie = 0;
						break;
					case GD_MISC_OVERWRITE:
					case GD_MISC_USEASL:
						((struct MyNewGadget *)gad->UserData)->CurrentValue = !((struct MyNewGadget *)gad->UserData)->CurrentValue;
						break;
					case GD_MISC_DISPLAY:
					case GD_MISC_SPACE:
						((struct MyNewGadget *)gad->UserData)->CurrentValue = Msg.Code;
						break;
				}
				break;
		}
	}

	if( wie != -1)
	{
		BOOL RedrawDisplay = FALSE;

		if( wie == 1 )
		{
			altsprungweite = MiscNewGadgets[ GD_MISC_ALTSPRUNGWEITE ].CurrentValue;

			SetScrollRand(MiscNewGadgets[ GD_MISC_SCROLLRAND ].CurrentValue);
			SetUndoLevel(MiscNewGadgets[ GD_MISC_MAXUNDOLEVEL ].CurrentValue);
			SetMaxUndoMemSize(MiscNewGadgets[ GD_MISC_MAXUNDOMEM ].CurrentValue);
			SetTaskPri(FindTask(NULL), taskpri = MiscNewGadgets[ GD_MISC_PROGRAMPRIO ].CurrentValue);
			SetRequester((MiscWD.Gadgets[ GD_MISC_USEASL ]->Flags&GFLG_SELECTED)?TRUE:FALSE);
			SetClipboardUnit(MiscNewGadgets[ GD_MISC_CLIPBOARDUNIT ].CurrentValue);
			SetCommandWindow(GetString(MiscWD.Gadgets[ GD_MISC_COMMANDWINDOW ]));
			SetCommandShellWindow(GetString(MiscWD.Gadgets[ GD_MISC_COMMANDSHELLWINDOW ]));
			if(MiscNewGadgets[ GD_MISC_OVERWRITE ].CurrentValue)
				mainflags |= MF_OVERWRITE;
			else
				mainflags &= ~MF_OVERWRITE;
	
			if( AktuDD->DisplayForm != MiscNewGadgets[ GD_MISC_DISPLAY ].CurrentValue + 1 )
			{
				RedrawDisplay = TRUE;
				AktuDD->DisplayForm = MiscNewGadgets[ GD_MISC_DISPLAY ].CurrentValue + 1;
				
				if(( AktuDD->DisplayForm & DF_HEXASCII ) != DF_HEXASCII )
				{
					if( AktuDD->DisplayForm & DF_HEX )
						AktuDD->Flags |= DD_HEX;
					else
						AktuDD->Flags &= ~DD_HEX;
				}
			}
			
			if( 3 == MiscNewGadgets[ GD_MISC_SPACE ].CurrentValue )
				MiscNewGadgets[ GD_MISC_SPACE ].CurrentValue = 8;
	
			if( AktuDD->DisplaySpaces != MiscNewGadgets[ GD_MISC_SPACE ].CurrentValue )
			{
				AktuDD->DisplaySpaces = MiscNewGadgets[ GD_MISC_SPACE ].CurrentValue;
				RedrawDisplay = TRUE;
			}

			DefaultDisplayForm = AktuDD->DisplayForm;
			DefaultDisplaySpaces = AktuDD->DisplaySpaces;
		}
			
		MyRemoveSignal( 1L << MiscWD.Wnd->UserPort->mp_SigBit );
		NewCloseAWindow( &MiscWD );
	
		if( wie == 1 )
		{
			if( RedrawDisplay )
				MakeDisplay( AktuDI );
		}
	}
}


BOOL OpenMiscSettings( void )
{
	LONG err;

	if( MiscWD.Wnd )
	{
		ActivateWindow( MiscWD.Wnd );
		return( TRUE );
	}

		/* NewGadgets mit Werten belegen */

	MiscNewGadgets[ GD_MISC_ALTSPRUNGWEITE ].CurrentValue = altsprungweite;
	MiscNewGadgets[ GD_MISC_SCROLLRAND ].CurrentValue = realscrollrand;
	MiscNewGadgets[ GD_MISC_MAXUNDOLEVEL ].CurrentValue = UndoLevel;
	MiscNewGadgets[ GD_MISC_MAXUNDOMEM ].CurrentValue = maxundomemsize;
	MiscNewGadgets[ GD_MISC_CLIPBOARDUNIT ].CurrentValue = GetClipboardUnit();
	MiscNewGadgets[ GD_MISC_PROGRAMPRIO ].CurrentValue = taskpri;
	MiscNewGadgets[ GD_MISC_USEASL ].CurrentValue = UseAsl;
	MiscNewGadgets[ GD_MISC_OVERWRITE ].CurrentValue = (mainflags&MF_OVERWRITE) ? TRUE : FALSE;
	MiscNewGadgets[ GD_MISC_COMMANDSHELLWINDOW ].CurrentValue = (IPTR) arexxcommandshellwindow;
	MiscNewGadgets[ GD_MISC_COMMANDWINDOW ].CurrentValue = (IPTR) arexxcommandwindow;
	MiscNewGadgets[ GD_MISC_DISPLAY ].CurrentValue = (IPTR) DisplayStrings;
	MiscNewGadgets[ GD_MISC_SPACE ].CurrentValue = (IPTR) SpaceStrings;
	MiscNewGadgets[ GD_MISC_DISPLAY ].Max = 2;
	MiscNewGadgets[ GD_MISC_SPACE ].Max = 3;
	MiscNewGadgets[ GD_MISC_DISPLAY ].Min = AktuDD->DisplayForm - 1;

	if( AktuDD->DisplaySpaces <= 2 )
		MiscNewGadgets[ GD_MISC_SPACE ].Min = AktuDD->DisplaySpaces;
	else
		MiscNewGadgets[ GD_MISC_SPACE ].Min = 3;

	if((err = NewOpenAWindow( &MiscWD, GetStr(MSG_WINDOWTITLE_MISC ))))
	{
		MyRequest( MSG_INFO_GLOBAL_CANTOPENWINDOW, err );
		return( FALSE );
	}
	else
	{
		MyAddSignal( 1L << MiscWD.Wnd->UserPort->mp_SigBit, &DoMiscWndMsg );
	}
	
	return (TRUE);
}

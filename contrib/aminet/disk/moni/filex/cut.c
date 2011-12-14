#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>

#include <string.h>

#endif

#include "filexstructs.h"
#include "filexstrings.h"
#include "allprotos.h"

static UBYTE BlockFullName[256];		/* Blockverzeichnis- und -filename */

void PrintBlock( void )
{
	UBYTE *blockmem;
	ULONG blocklaenge;

	if(( blockmem = GetClip( &blocklaenge )))
	{
		MyPrint( blockmem, blocklaenge, AktuDD->DisplayTyp, AktuDI->Wnd );
		FreeMem( blockmem, blocklaenge );
	}
	else
		DisplayLocaleText( MSG_INFO_GLOBAL_BUFFEREMPTY );
}

/*
 * void Copy( struct DisplayData *DD )
 *
 * Kopiert den markierten Bereich ins Clipboard.
 */

void Copy( struct DisplayData *DD )
{
	if( DD->Flags & DD_MARK)
	{
		ULONG start, end;

		DD->Flags &= ~DD_MARK;
		MarkOff( DD );

		if( DD->CPos > DD->MPos)
		{
			start = DD->MPos;
			end = DD->CPos;
		}
		else
		{
			start = DD->CPos;
			end = DD->MPos;
		}
	
		SaveClip( DD->FD->Mem + start, end - start + 1 );
		CursorOn( DD );
	}
	else
		DisplayLocaleText( MSG_INFO_GLOBAL_NOBLOCKMARKED );
}

static void CutBlock( ULONG start, ULONG end, struct DisplayData *DD )
{
	struct Speicher s;

	if(start>end){long dummy=start;start=end;end=dummy;};

	s.len=end-start+1;

	if((s.mem = AllocMem(s.len,MEMF_ANY)))
	{
		memcpy(s.mem,DD->FD->Mem+start,s.len);
		memmove(DD->FD->Mem+start,DD->FD->Mem+end+1,DD->FD->Len-end);

		SaveClip( s.mem, s.len );

		DD->FD->Len -= s.len;

		CursorMarkOff( DD );

		AllDisplay( DD->FD, SCROLLERNEU );
		SetScrollerGadget(DD);

			/* Haben wir alles weggeschnitten? */

		if( DD->FD->Len == 0 )
			DD->CPos = 0;
		else
		{
			/* Falls Ende auch gleichzeitig Fileende ist, */
			/* Cursor auf Fileende-1 setzen */
	
			if( end != DD->FD->Len + s.len - 1 )
				DD->CPos = start;
			else
				DD->CPos = DD->FD->Len -1;
			
			CursorMarkOn( DD );
		}

				/* Eintrag in Undolist */

		AddUndoCut(start, s, DD->FD );

		UpdateAllStatusZeilen( DD->FD );
	}
	else
	{
		MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, s.len );
	}
}

void Cut( struct DisplayData *DD )
{
	if((DD->Flags & DD_MARK)&&(!(DD->FD->Typ == FD_GRAB)))
	{
		DD->Flags &= ~DD_MARK;
		MarkOff( DD );
		CutBlock( DD->CPos, DD->MPos, DD );
		CursorOn( DD );
		SetCursor( DD->CPos, DD );
	}
	else
	{
		if( DD->FD->Typ == FD_GRAB)
			DisplayLocaleText( MSG_INFO_GLOBAL_CUTTINGNOTPOSSIBLEINGRABMODE );
		else if( !( DD->Flags & DD_MARK))
			DisplayLocaleText( MSG_INFO_GLOBAL_NOBLOCKMARKED );
	}
}

/*
 * void CutBlockForce(ULONG start, ULONG end)
 *
 * Schneidet den Block von start bis end (einschließlich) raus
 * OHNE ihn irgendwo zu speichern
 */

void CutBlockForce( ULONG start, ULONG end, struct DisplayData *DD )
{
	ULONG blocklaenge;

	if(start>end){ULONG dummy=start;start=end;end=dummy;};

	blocklaenge=end-start+1;

	memmove(DD->FD->Mem+start,DD->FD->Mem+end+1,DD->FD->Len-end);

	DD->FD->Len-=blocklaenge;

	CursorMarkOff( DD );

	AllDisplay( DD->FD, SCROLLERNEU );
	SetScrollerGadget( DD );

		/* Haben wir alles weggeschnitten? */

	if( DD->FD->Len == 0 )
		DD->CPos = 0;
	else
	{
		/* Falls Ende auch gleichzeitig Fileende ist, */
		/* Cursor auf Fileende-1 setzen */

		if( end != DD->FD->Len + blocklaenge - 1 )
			DD->CPos = start;
		else
			DD->CPos = DD->FD->Len -1;
		
		CursorMarkOn( DD );
	}

	UpdateAllStatusZeilen( DD->FD );
}

/*
 * void Paste(long pos )
 *
 * Überschreibt den Bereich ab pos mit dem aktuellen Block im Blockspeicher.
 * Falls dieser nicht rein paßt, wird gefragt, ob dieser gekürzt reingeschrieben
 * werden soll.
 *
 * Neudarstellung des Fensterinhaltes.
 */

void Paste( struct DisplayData *DD )
{
	struct Speicher s;

	s.mem = GetClip( &s.len );

	if( s.len && s.mem )
	{
		struct Speicher ps;

		if( s.len > DD->FD->Len - DD->CPos )			/* Paßt Block rein? */
		{
			ps.len = DD->FD->Len - DD->CPos;		/* Nein => Länge kürzen */

				/* Garnichts paßt rein => Nichts machen */

			if( ps.len == 0 )
				return;

				/* Fragen: Trotzdem füllen? */

			if( FALSE == MyFullRequest( MSG_INFO_GLOBAL_BLOCKDOESNTFIT, MSG_GADGET_YESNO, s.len-ps.len))
				return;			/* Nein => Benden */
		}
		else
			ps.len = s.len;

		if(( ps.mem = AllocMem( ps.len, MEMF_ANY )))
		{
				/* Alten Bereich sichern */

			memcpy(ps.mem,DD->FD->Mem+DD->CPos,ps.len);

				/* Block in Speicher kopieren und Display neudarstellen */

			memcpy( DD->FD->Mem + DD->CPos, s.mem, ps.len );

				/* Eintrag in Undoliste */

			AddUndoPaste(DD->CPos,ps, s, DD->FD);

				/* Alles neudarstellen */

			AllDisplay( DD->FD, SCROLLERNEU );
		}
		else
		{
			MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, ps.len );
		}
	}
	else
		DisplayLocaleText( MSG_INFO_GLOBAL_BUFFEREMPTY );
}

/*
 * void InsertBlock( void )
 *
 * Fügt ab DD->CPos einen Block ein. Eintrag in die Undoliste.
 *
 * Neudarstellung des Fensterinhaltes.
 */

void InsertBlock( struct DisplayData *DD )
{
	struct Speicher s;

	s.mem = GetClip( &s.len );

	if( s.len && s.mem )
	{
			/* Prüfen, ob wir neuen Speicher brauchen */

		if( DD->FD->RLen - DD->FD->Len >= s.len )
		{
				/* Rest nach hinten verschieben */

			memmove(DD->FD->Mem+DD->CPos+s.len,DD->FD->Mem+DD->CPos,DD->FD->Len-DD->CPos);

				/* Block einfügen */

			memcpy(DD->FD->Mem+DD->CPos,s.mem,s.len);
			
			DD->FD->Len+=s.len;
		}
		else
		{
			UBYTE *neumem;
			long neulaenge=s.len+DD->FD->Len;

			if((neumem = AllocMem(neulaenge,MEMF_ANY)))
			{
					/* Den sich nicht ändernden Anfang kopieren */

				memcpy(neumem,DD->FD->Mem,DD->CPos);
				
					/* Block kopieren */

				memcpy(neumem+DD->CPos,s.mem,s.len);

					/* Den Rest kopieren */

				memcpy(neumem+DD->CPos+s.len,DD->FD->Mem+DD->CPos,DD->FD->Len-DD->CPos);

					/* Alten Speicher freigeben */

				if( DD->FD->Mem && DD->FD->RLen )
				{
					FreeMem( DD->FD->Mem, DD->FD->RLen );
					DD->FD->Mem = 0;
					DD->FD->RLen = DD->FD->Len = 0;
				}

					/* Neuen Speicher eintragen */

				DD->FD->Mem=neumem;
				DD->FD->RLen = DD->FD->Len = neulaenge;
			}
			else
			{
				MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, neulaenge );
				return;
			}
		}

		AddUndoInsert( DD->CPos, s, DD->FD);

		SetScrollerGadget(DD);
		AllDisplay( DD->FD, SCROLLERNEU );
		UpdateAllStatusZeilen(DD->FD);

		if( DD->FD->Len - s.len == 0 ) CursorMarkOn(DD);
	}
	else
		DisplayLocaleText( MSG_INFO_GLOBAL_BUFFEREMPTY );
}


/*
 * void InsertBlockForce(long pos, UBYTE *mem, ULONG len, struct DisplayData *DD )
 *
 * Fügt ab pos einen Block ein. Kein Eintrag in die Undoliste.
 *
 * Neudarstellung des Fensterinhaltes.
 */

void InsertBlockForce( long pos, UBYTE *mem, ULONG len, struct DisplayData *DD )
{
	if( len && mem )
	{
			/* Prüfen, ob wir neuen Speicher brauchen */

		if(DD->FD->RLen-DD->FD->Len >= len )
		{
				/* Rest nach hinten verschieben */

			memmove(DD->FD->Mem+pos+len,DD->FD->Mem+pos,DD->FD->Len-pos);

				/* Block einfügen */

			memcpy(DD->FD->Mem+pos,mem,len);
			
			DD->FD->Len+=len;
		}
		else
		{
			UBYTE *neumem;
			long neulaenge=len+DD->FD->Len;

			if((neumem = AllocMem(neulaenge,MEMF_ANY)))
			{
					/* Den sich nicht ändernden Anfang kopieren */

				memcpy(neumem,DD->FD->Mem,pos);
				
					/* Block kopieren */

				memcpy(neumem+pos,mem,len);

					/* Den Rest kopieren */

				memcpy(neumem+pos+len,DD->FD->Mem+pos,DD->FD->Len-pos);

					/* Alten Speicher freigeben */

				if( DD->FD->Mem && DD->FD->RLen )
				{
					FreeMem( DD->FD->Mem, DD->FD->RLen );
					DD->FD->Mem = 0;
					DD->FD->RLen = DD->FD->Len = 0;
				}

					/* Neuen Speicher eintragen */

				DD->FD->Mem=neumem;
				DD->FD->RLen = DD->FD->Len = neulaenge;
			}
			else
			{
				MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, neulaenge );
				return;
			}
		}

		SetScrollerGadget(DD);
		AllDisplay( DD->FD, SCROLLERNEU );
		UpdateAllStatusZeilen(DD->FD);

		if( DD->FD->Len - len == 0 ) CursorMarkOn(DD);
	}
	else
		DisplayLocaleText( MSG_INFO_GLOBAL_BUFFEREMPTY );
}

/*
 * void AppendBlock( void )
 *
 * Fügt Clipboard am Ende ein.
 *
 * Neudarstellung des Fensterinhaltes.
 */

void AppendBlock( struct DisplayData *DD )
{
	struct Speicher s;

	s.mem = GetClip( &s.len );

	if( s.len && s.mem )
	{
			/* Prüfen, ob wir neuen Speicher brauchen */

		if( DD->FD->RLen-DD->FD->Len >= s.len )
		{
				/* Block einfügen */

			memcpy(DD->FD->Mem+DD->FD->Len,s.mem,s.len);
			
			DD->FD->Len+=s.len;
		}
		else
		{
			UBYTE *neumem;
			long neulaenge=s.len+DD->FD->Len;

			if((neumem = AllocMem(neulaenge,MEMF_ANY)))
			{
					/* Den sich nicht ändernden Anfang kopieren */

				memcpy(neumem,DD->FD->Mem,DD->FD->Len);
				
					/* Block kopieren */

				memcpy(neumem+DD->FD->Len,s.mem,s.len);

					/* Alten Speicher freigeben */

				if( DD->FD->Mem && DD->FD->RLen )
				{
					FreeMem( DD->FD->Mem, DD->FD->RLen );
					DD->FD->Mem = 0;
					DD->FD->RLen = DD->FD->Len = 0;
				}

					/* Neuen Speicher eintragen */

				DD->FD->Mem=neumem;
				DD->FD->RLen=DD->FD->Len=neulaenge;
			}
			else
			{
				MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, neulaenge );
				return;
			}
		}

		AddUndoInsert(DD->FD->Len-s.len, s, DD->FD);

		SetScrollerGadget(DD);
		AllDisplay( DD->FD, SCROLLERNEU );
		UpdateAllStatusZeilen(DD->FD);

		if( DD->FD->Len - s.len == 0 ) CursorMarkOn(DD);
	}
	else
		 DisplayLocaleText( MSG_INFO_GLOBAL_BUFFEREMPTY );
}

/*
 * BOOL LoadBlock(char *fullname)
 *
 * Läd einen Block ein
 */

BOOL LoadBlock(char *fullname)
{
	BPTR fh;
	BOOL Success=FALSE;

	if((fh=Open(fullname,MODE_OLDFILE)))
	{
		ULONG blocklaenge;
		UBYTE *blockmem;

		blocklaenge=GetFileLaenge(fh);

		if(blocklaenge)
		{
		if((blockmem = AllocMem(blocklaenge,MEMF_ANY)))
		{
			Read(fh,blockmem,blocklaenge);

			SaveClip( blockmem, blocklaenge );

			FreeMem( blockmem, blocklaenge );
			Success = TRUE;
		}
		else
		{
			MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, blocklaenge );
		}
    	    	}
		Close(fh);
	}
	else
		MyRequest( MSG_INFO_GLOBAL_CANTOPENFILE, ( IPTR )fullname );

	return(Success);		
}

/*
 * BOOL LoadBlockFR(void)
 *
 * Öffnet einen FileRequester und läd einen Block ein
 */

BOOL LoadBlockFR(void)
{
	if( MyRequestFile( BlockFullName, GetStr( MSG_WINDOWTITLE_LOADBLOCK ), 0, FALSE ))
		return( LoadBlock( BlockFullName ));
	else
		return( FALSE );
}

/*
 * BOOL SaveBlock(char *Name)
 *
 * Speichert einen Block ab.
 */

BOOL SaveBlock( char *Name )
{
	BPTR fh;
	BOOL Success=FALSE;

	UBYTE *blockmem;
	ULONG blocklaenge;

	if(( blockmem = GetClip( &blocklaenge )))
	{
			/* Prüfen, ob File existiert. Falls ja, fragen, ob es überschrieben */
			/* werden soll */

		if(!(mainflags&MF_OVERWRITE))
			if(GetFileLaengeName(Name))
				if( 0 == MyFullRequest( MSG_INFO_GLOBAL_FILEALREADYEXISTS, MSG_GADGET_YESNO, ( IPTR )Name ))
					return(Success);

		if((fh=Open(Name,MODE_NEWFILE)))
		{
			if(blocklaenge!=Write(fh,blockmem,blocklaenge))
			{
					/* Falls nicht vollständig geschrieben, löschen */
	
				Close(fh);
				MyRequest( MSG_INFO_GLOBAL_CANTSAVEFILE, ( IPTR )Name );
				DeleteFile(Name);
			}
			else
			{
				Success = TRUE;
	
				Close( fh );
			}
		}
		else
			MyRequest( MSG_INFO_GLOBAL_CANTOPENFILE, ( IPTR )Name );

		FreeMem( blockmem, blocklaenge );
	}
	else
		DisplayLocaleText( MSG_INFO_GLOBAL_BUFFEREMPTY );

	return( Success );
}

/*
 * BOOL SaveBlockFR( void )
 *
 * Öffnet einen FileRequester und speichert einen Block ab.
 */

BOOL SaveBlockFR( void )
{
	if( MyRequestFile( BlockFullName, GetStr( MSG_WINDOWTITLE_SAVEBLOCK ), 0, FALSE ))
		return( SaveBlock( BlockFullName ));
	else
		return( FALSE );
}


/*
 * void Fill(ULONG start, ULONG end,UBYTE *was,ULONG laenge)
 * 
 * Füllt den bestimmten Bereich.
 */

void Fill( ULONG start, ULONG end, UBYTE *was, ULONG laenge, struct DisplayData *DD )
{
	struct Speicher f1, f2;

	if(laenge==0)return;

	if(start>end){ULONG dummy=start;start=end;end=dummy;}

	f1.len = f2.len = end-start+1;

	if((f1.mem = AllocMem(f1.len,MEMF_ANY)))
	{
	if((f2.mem = AllocMem(f1.len,MEMF_ANY)))
	{
		long pos=start;

			/* Alten Bereich sichern */

		memcpy(f1.mem,DD->FD->Mem+pos,f1.len);

			/* Bereich füllen */

		end++;

		while(start+laenge<=end)
		{
			memcpy(DD->FD->Mem+start,was,laenge);
			start+=laenge;
		}

		if(start<=end)
		{
			memcpy(DD->FD->Mem+start,was,end-start);
		}

		AllDisplay( DD->FD, SCROLLERNEU );

			/* Neuen Bereich sichern */

		memcpy(f2.mem,DD->FD->Mem+pos,f1.len);

			/* Eintrag in Undoliste */

		AddUndoPaste(pos,f1,f2, DD->FD);
	}
	else
	{
		MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, f1.len );
		FreeMem(f1.mem,f1.len);
	}
	}
	else
		MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, f1.len );
}

/****************************** FillWindow ******************************/

static UBYTE fillstring[256];
static ULONG fillmode = FM_STRING;

void SetFillString( char *String )
{
	strcpy( fillstring, String );
}

UBYTE *GetFillString( void )
{
	return( fillstring );
}

void SetFillMode( ULONG Mode )
{
	fillmode = Mode;
}

ULONG GetFillMode( void )
{
	return( fillmode );
}

enum {
GD_FILL_STRINGFILL,
GD_FILL_STRING,
GD_FILL_FILL,
GD_FILL_CANCEL
};

static struct MyNewGadget FillNewGadgets[] =
{
	{CHECKBOX_KIND, 0, 0, 0, MSG_GADGET_FILL_STRINGFILL, 0, 0, 0, 0, 0},
	{STRING_KIND, 0, 0, 0, MSG_GADGET_FILL_STRING, 0, 256, 0, 0, 30},

	{BUTTON_KIND, GP_LEFTBOTTOM, 0, 0, MSG_GADGET_FILL_FILL, 0, 0, 0, 0, 0},
	{BUTTON_KIND, GP_RIGHTBOTTOM, 0, 0, MSG_GADGET_CANCEL, 0, 0, 0, 0, 0},
	{0}
};

static struct WindowData FillWD =
{
	NULL,	NULL,	FALSE, NULL, NULL,
	0,0,
	&FillNewGadgets[ 0 ], 4
};

static void DoFillWndMsg( void )
{
	struct IntuiMessage	*m, Msg;
	struct Gadget *gad;

	int Select = -1;			/* Wie wurde Window verlassen ? (Nicht/Cancel/Fill) */

	while(( m = GT_GetIMsg( FillWD.Wnd->UserPort )))
	{
		CopyMem(( char * )m, ( char * )&Msg, (long)sizeof( struct IntuiMessage ));

		GT_ReplyIMsg( m );

		KeySelect(FillWD.Gadgets, &Msg);

		gad = (struct Gadget *) Msg.IAddress;

		switch( Msg.Class )
		{
			case	IDCMP_REFRESHWINDOW:
				GT_BeginRefresh( FillWD.Wnd );
				GT_EndRefresh( FillWD.Wnd, TRUE );
				break;

			case	IDCMP_VANILLAKEY:
				if( Msg.Code == 13)
					Select = 1;
				break;

			case	IDCMP_CLOSEWINDOW:
				Select = 0;
				break;

			case	IDCMP_GADGETUP:
				switch(gad->GadgetID)
				{
					case GD_FILL_STRING:
						if( FillNewGadgets[ GD_FILL_STRINGFILL ].CurrentValue == FALSE )
							if(-1==TestHexString(GetString(FillWD.Gadgets[GD_FILL_STRING])))
								ActivateGadget(FillWD.Gadgets[GD_FILL_STRING],FillWD.Wnd,0);
						break;

					case GD_FILL_STRINGFILL:
						((struct MyNewGadget *)gad->UserData)->CurrentValue = !((struct MyNewGadget *)gad->UserData)->CurrentValue;
						break;	

					case GD_FILL_FILL:
						Select = 1;
						break;

					case GD_FILL_CANCEL:
						Select = 0;
						break;
				}
				break;
		}
	}

	if( Select == 1 )		/* Falls nicht cancel, Werte merken */
	{
		strcpy( fillstring, GetString( FillWD.Gadgets[ GD_FILL_STRING ] ));

		if( FillNewGadgets[ GD_FILL_STRINGFILL ].CurrentValue )
			fillmode = FM_STRING;
		else
			fillmode = FM_NUMBER;
	}

	if( Select != -1 )
	{
		MyRemoveSignal( 1L << FillWD.Wnd->UserPort->mp_SigBit );
		NewCloseAWindow( &FillWD );
	}

	if( Select == 1 )
		FillDD( AktuDD );
}

void OpenFillWindow( void )
{
	ULONG err;

	if( FillWD.Wnd )
	{
		ActivateWindow( FillWD.Wnd );
		return;
	}

	FillNewGadgets[ GD_FILL_STRING ].CurrentValue = (IPTR) fillstring;
	FillNewGadgets[ GD_FILL_STRINGFILL ].CurrentValue =	( fillmode & FM_STRING ) ? TRUE : FALSE;

	if(( err = NewOpenAWindow( &FillWD, GetStr(MSG_WINDOWTITLE_FILL))))
		MyRequest( MSG_INFO_GLOBAL_CANTOPENWINDOW, err );
	else
	{
		MyAddSignal( 1L << FillWD.Wnd->UserPort->mp_SigBit, &DoFillWndMsg );
	}
}

void FillDD( struct DisplayData *DD )
{
	switch(GetFillMode())
	{
		case FM_STRING:
			Fill(DD->CPos,DD->MPos,GetFillString(),strlen( GetFillString()), DD);
			break;
		case FM_NUMBER:
			if(-1!=TestHexString(GetFillString()))
			{
				UBYTE hexfillstring[256];
				
				Fill(DD->CPos,DD->MPos,hexfillstring,ConvertHexstringToHexnumber(GetFillString(),hexfillstring), DD);
			}
			break;
	}
}

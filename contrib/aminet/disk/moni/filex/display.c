#ifdef USE_PROTO_INCLUDES
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <intuition/imageclass.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <clib/alib_protos.h>

#include <string.h>
#include <stdio.h>

#endif

#include "filexstructs.h"
#include "allprotos.h"
#include "filexstrings.h"

UBYTE CursorAPen = 3, CursorBPen = 2, CursorLinePen = 7;
UBYTE NormalAPen = 1, NormalBPen = 0;
UBYTE BlockAPen = 2, BlockBPen = 4;

void CalcStatusZeilenBreite( struct DisplayData *DD )
{
		/* Statuszeilenbreite neuberechnen */

	DD->StatusZeilenBreite = 9 * DD->DI->fbreite;

	if( DD->DisplayForm & DF_HEX )
		DD->StatusZeilenBreite += (2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DD->DI->fbreite + BOXADDX;
	if( DD->DisplayForm & DF_ASCII )
		DD->StatusZeilenBreite += DD->BPR * DD->DI->fbreite + BOXADDX;
}

void InvertStatusBalkenRahmen( struct DisplayData *DD, WORD off )
{
	SetDrMd( DD->Wnd->RPort, COMPLEMENT );
	Move( DD->Wnd->RPort, DD->sbx - BOXADDX/2, DD->sbby - BOXADDY/2 + off * DD->DI->fhoehe );
	Draw( DD->Wnd->RPort, DD->sbx + DD->StatusZeilenBreite + BOXADDX/2 - 1, DD->sbby - BOXADDY/2 + off * DD->DI->fhoehe );
	Draw( DD->Wnd->RPort, DD->sbx + DD->StatusZeilenBreite + BOXADDX/2 - 1, DD->sbby + off * DD->DI->fhoehe + DD->DI->fhoehe + BOXADDY/2 - 1);
	Draw( DD->Wnd->RPort, DD->sbx - BOXADDX/2, DD->sbby + off * DD->DI->fhoehe + DD->DI->fhoehe + BOXADDY/2 - 1);
	Draw( DD->Wnd->RPort, DD->sbx - BOXADDX/2, DD->sbby - BOXADDY/2 + off * DD->DI->fhoehe );
	SetDrMd( DD->Wnd->RPort, JAM2 );
}

struct DisplayData *GetroffenerView( WORD x, WORD y, struct DisplayInhalt *DI )
{
	struct DisplayData *DD;

	DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

	while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
	{
		if(( y >= DD->sbby - BOXADDY / 2 ) && ( y < DD->bby + DD->Zeilen * DI->fhoehe + BOXADDY/2 ))
			break;
		
		DD = ( struct DisplayData * )DD->Node.ln_Succ;
	}

	if( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
		return( DD );
	else
		return( NULL );
}

BOOL SetzeCursorMaus( WORD x, WORD y, struct DisplayData *SetDD, struct DisplayInhalt *DI )
{
	struct DisplayData *DD;
	BOOL Success = FALSE;
	BOOL Found = FALSE;

	DD = GetroffenerView(x,y,DI);

	if(DD && (( !SetDD ) || ( SetDD == DD )))
	{
		WORD xoff,yoff;

		if( y >= DD->bby )
		{
			yoff = ( y - DD->bby ) / DI->fhoehe;

			if( yoff < DD->Zeilen )
			{
				if( DD->DisplayForm & DF_ASCII )
				{
					if( x >= DD->abx )
					{
						xoff = ( x - DD->abx ) / DI->fbreite;
			
						if( xoff < DD->BPR )
						{
							Found = TRUE;
							Success = TRUE;
							SetCursor( yoff * DD->BPR + xoff + DD->SPos * DD->BPR, DD );

							if(( DD->Flags & DD_HEX ) && ( !( DD->Flags & DD_MARK )))
								WechselCursorBereich( DD );
						}
					}
				}
				if(( !Found ) && ( DD->DisplayForm & DF_HEX ))
				{
					if( x >= DD->hbx )
					{
						xoff = ( x - DD->hbx ) / DI->fbreite;
			
						if( xoff < 2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces ))
						{
							WORD Rest, HexBlockX;

							Found = TRUE;
							HexBlockX = 2 * ( 1 << DD->DisplaySpaces ) + 1;
			
							Rest = xoff % HexBlockX;
			
							if( Rest != HexBlockX - 1 )
							{
								Success = TRUE;
								SetCursor( yoff * DD->BPR + ( Rest + xoff / HexBlockX * ( HexBlockX - 1 )) / 2 + DD->SPos * DD->BPR, DD );

								if((!( DD->Flags & DD_HEX )) && ( !( DD->Flags & DD_MARK )))
									WechselCursorBereich( DD );
							}
						}
					}
				}
			}
		}
		
		ChangeAktuView( DD );
	}
	
	if( SetDD && ( !Found ))
	{
		WORD yoff;

		yoff = ( y - SetDD->bby ) / DI->fhoehe;

		MoveCursorY( yoff - ( SetDD->CPos / SetDD->BPR - SetDD->SPos ), SetDD );
	}

	return( Success );
}

BOOL StatusBalkenHit( WORD x, WORD y, struct DisplayInhalt *DI )
{
	struct DisplayData *DD;
	BOOL Found = FALSE;

	DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

	while(( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail ) && ( y >= DD->sbby - BOXADDY / 2 ))
	{
		if( y < DD->sbby + DI->fhoehe + BOXADDY/2 )
		{
			Found = TRUE;
			break;
		}
		
		DD = ( struct DisplayData * )DD->Node.ln_Succ;
	}

	if( Found )
		ChangeAktuView( DD );

	return( Found );
}

void MarkOn( struct DisplayData *DD )
{
	ExpandBlock( DD->MPos, DD->MPos, DD );
}

void MarkOff( struct DisplayData *DD )
{
/*	BlockKuerzen( DD->MPos - DD->SPos * DD->BPR, DD->CPos - DD->SPos * DD->BPR, DD);*/
}

void MySetWriteMask( struct RastPort *rp, ULONG msk )
{
/*	if( Kick30 ) SetWriteMask( rp, msk );*/
/*	else SetWrMsk( rp, msk );*/
}

static void DrawCursor( struct DisplayData *DD )
{
	int off,x,y;

	if( DD->FD->Len == 0 ) return;

/*	kprintf( "DrawCursor: Pos: %ld\n", DD->CPos );*/

	off = DD->CPos - DD->BPR * DD->SPos;

	if(( off >= 0 ) && ( off < DD->BPR * DD->Zeilen ))
	{
		x = off % DD->BPR;
		y = off / DD->BPR;
	
		if( DD->Flags & DD_HEX )			/* Im Hexfeld aktiv */
		{
			if( DD->DisplayForm & DF_ASCII )
			{
					/* Buchstaben in normalen Farben malen */

				SetAPen( DD->Wnd->RPort, NormalAPen );
				SetBPen( DD->Wnd->RPort, NormalBPen );
		
				Move( DD->Wnd->RPort, DD->abx + x * DD->DI->fbreite, DD->Wnd->RPort->Font->tf_Baseline + DD->bby + y * DD->DI->fhoehe );
				Text( DD->Wnd->RPort, &displaytab[ DD->DisplayTyp ][ DD->FD->Mem[ DD->CPos ]], 1);
				
					/* Linie in Cursorfarben drunter setzen */
		
				SetAPen( DD->Wnd->RPort, CursorLinePen );

				Move( DD->Wnd->RPort, DD->abx + x * DD->DI->fbreite, DD->bby + y * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
				Draw( DD->Wnd->RPort, DD->abx + x * DD->DI->fbreite + DD->DI->fbreite - 1, DD->bby + DD->DI->fhoehe * y + DD->DI->fhoehe - 1 );
			}

			if( DD->DisplayForm & DF_HEX )
			{
				char text[ 2 ];
			
				ByteToString( DD->FD->Mem[ DD->CPos ], text );
				
				x = 2 * x + ( x >> DD->DisplaySpaces );
					
				SetAPen( DD->Wnd->RPort, CursorAPen );
				SetBPen( DD->Wnd->RPort, CursorBPen );
				Move( DD->Wnd->RPort, DD->hbx + x * DD->DI->fbreite, DD->Wnd->RPort->Font->tf_Baseline + DD->bby + y * DD->DI->fhoehe );
				Text( DD->Wnd->RPort, text, 2 );
			}
		}
		else									/* Im ASCII-Feld aktiv */
		{
			if( DD->DisplayForm & DF_ASCII )
			{
				SetAPen( DD->Wnd->RPort, CursorAPen );
				SetBPen( DD->Wnd->RPort, CursorBPen );
			
				Move( DD->Wnd->RPort, DD->abx + x * DD->DI->fbreite, DD->Wnd->RPort->Font->tf_Baseline + DD->bby + y * DD->DI->fhoehe );
				Text( DD->Wnd->RPort, &displaytab[ DD->DisplayTyp ][ DD->FD->Mem[ DD->CPos ]], 1);
			}
	
			if( DD->DisplayForm & DF_HEX )
			{
				char text[ 2 ];
			
				ByteToString( DD->FD->Mem[ DD->CPos ], text );
				
				x = 2 * x + ( x >> DD->DisplaySpaces );
					
				SetAPen( DD->Wnd->RPort, NormalAPen );
				SetBPen( DD->Wnd->RPort, NormalBPen );
				Move( DD->Wnd->RPort, DD->hbx + x * DD->DI->fbreite, DD->Wnd->RPort->Font->tf_Baseline + DD->bby + y * DD->DI->fhoehe );
				Text( DD->Wnd->RPort, text, 2 );

				SetAPen( DD->Wnd->RPort, CursorLinePen );
				Move( DD->Wnd->RPort, DD->hbx + x * DD->DI->fbreite, DD->bby + y * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
				Draw( DD->Wnd->RPort, DD->hbx + x * DD->DI->fbreite + 2 * DD->DI->fbreite - 1, DD->bby + y * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
			}
		}
	}
}

/*
 * void CursorOn(void)
 *
 * Füllt ein Rechteck an der aktuellen Position in der Größe des Cursors
 *           und einen Strich im anderen Feld, je nachdem
 *           wie in cursorflags gesetzt
 * unter der Bed., daß die Länge der Daten > 0
 */

void CursorOn( struct DisplayData *DD )
{
/*	kprintf( "CursorOn:" );*/

	DrawCursor( DD );
}

/*
 * void CursorOff(void)
 *
 */

void CursorOff( struct DisplayData *DD )
{
	int off,x,y;

	if( DD->FD->Len == 0 ) return;

	off = DD->CPos - DD->BPR * DD->SPos;

	if(( off >= 0 ) && ( off < DD->BPR * DD->Zeilen ))
	{
		x = off % DD->BPR;
		y = off / DD->BPR;
	
		if( DD->DisplayForm & DF_ASCII )
		{
				/* Buchstaben in normalen Farben malen */

			SetAPen( DD->Wnd->RPort, NormalAPen );
			SetBPen( DD->Wnd->RPort, NormalBPen );
	
			Move( DD->Wnd->RPort, DD->abx + x * DD->DI->fbreite, DD->Wnd->RPort->Font->tf_Baseline + DD->bby + y * DD->DI->fhoehe );
			Text( DD->Wnd->RPort, &displaytab[ DD->DisplayTyp ][ DD->FD->Mem[ DD->CPos ]], 1);
		}

		if( DD->DisplayForm & DF_HEX )
		{
			char text[ 2 ];
		
			ByteToString( DD->FD->Mem[ DD->CPos ], text );
			
			x = 2 * x + ( x >> DD->DisplaySpaces );
				
			SetAPen( DD->Wnd->RPort, NormalAPen );
			SetBPen( DD->Wnd->RPort, NormalBPen );
			Move( DD->Wnd->RPort, DD->hbx + x * DD->DI->fbreite, DD->Wnd->RPort->Font->tf_Baseline + DD->bby + y * DD->DI->fhoehe );
			Text( DD->Wnd->RPort, text, 2 );
		}
	}
}

static void BlockZeichnen( long start, long end, BOOL ClearSpaces, struct DisplayData *DD )
{
	long startx, starty, endx, endy;

	if( start > end ) { long dummy = start; start = end; end = dummy; }

	if( start < 0 ) start = 0;
	if( end >= DD->BPR * DD->Zeilen ) end = DD->BPR * DD->Zeilen - 1;

/*	Printf( "BlockZeichnen( %ld, %ld, %ld)\n", start, end, ClearSpaces );*/

	startx = start % DD->BPR;
	starty = start / DD->BPR;
	endx = end % DD->BPR;
	endy = end / DD->BPR;

	MySetWriteMask( DD->Wnd->RPort, 2 );

		/* Gleiche Zeile ? */

	if( DD->DisplayForm & DF_HEX )
	{
		if( starty == endy )
		{
			RectFill( DD->Wnd->RPort,	DD->hbx + ( 2 * startx + ( startx >> DD->DisplaySpaces )) * DD->DI->fbreite,
								DD->bby + starty * DD->DI->fhoehe,
								DD->hbx + ( 2 * endx + ( endx >> DD->DisplaySpaces  )) * DD->DI->fbreite + DD->DI->fbreite * 2 - 1,
								DD->bby + starty * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
		}
		else
		{
				/* 3 Schritte: 1. Startzeile bis zum Ende markieren */
				/*             2. ggf. ZwischenZeilen markieren */
				/*             3. Endzeile von Anfang bis Endmarke markieren */
	
			RectFill(DD->Wnd->RPort,	DD->hbx + ( 2 * startx + ( startx >> DD->DisplaySpaces )) * DD->DI->fbreite,
								DD->bby + starty * DD->DI->fhoehe,
								DD->hbx + ( 2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DD->DI->fbreite - 1,
								DD->bby + starty * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
	
			if(( endy - starty ) > 1 )
			{
				RectFill(DD->Wnd->RPort,	DD->hbx,
									DD->bby + ( starty + 1 ) * DD->DI->fhoehe,
									DD->hbx + ( 2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DD->DI->fbreite - 1,
									DD->bby + ( endy - 1 ) * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
			}
	
			RectFill(DD->Wnd->RPort,	DD->hbx,
								DD->bby + endy * DD->DI->fhoehe,
								DD->hbx + ( 2 * endx + ( endx >> DD->DisplaySpaces  )) * DD->DI->fbreite + DD->DI->fbreite * 2 - 1,
								DD->bby + endy * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
	
		}

		if( ClearSpaces )
		{
				/* Prüfen, ob einer der Randpunkte an einer Hexfeldgrenze liegt */
		
			if(( startx > 0 ) && ( startx % ( 1 << DD->DisplaySpaces ) == 0 ))
			{
					/* Feld links daneben löschen */
		
				RectFill(DD->Wnd->RPort,	DD->hbx + ( 2 * startx + ( startx >> DD->DisplaySpaces )) * DD->DI->fbreite - DD->DI->fbreite,
									DD->bby + starty * DD->DI->fhoehe,
									DD->hbx + ( 2 * startx + ( startx >> DD->DisplaySpaces )) * DD->DI->fbreite - 1,
									DD->bby + starty * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
			}

			if(( endx < DD->BPR - 1 ) && ( endx % ( 1 << DD->DisplaySpaces ) == ( 1 << DD->DisplaySpaces ) - 1 ))
			{
					/* Feld rechts daneben löschen */
		
				RectFill(DD->Wnd->RPort,	DD->hbx + ( 2 + 2 * endx + ( endx >> DD->DisplaySpaces )) * DD->DI->fbreite,
									DD->bby + endy * DD->DI->fhoehe,
									DD->hbx + ( 2 + 2 * endx + ( endx >> DD->DisplaySpaces )) * DD->DI->fbreite + DD->DI->fbreite - 1,
									DD->bby + endy * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
			}
		}
	}

	if( DD->DisplayForm & DF_ASCII )
	{
	if( starty == endy )
	{
		RectFill( DD->Wnd->RPort,	DD->abx + startx * DD->DI->fbreite,
							DD->bby + starty * DD->DI->fhoehe,
							DD->abx + endx * DD->DI->fbreite + DD->DI->fbreite - 1,
							DD->bby + starty * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
	}
	else
	{
			/* 3 Schritte: 1. Startzeile bis zum Ende markieren */
			/*             2. ggf. ZwischenZeilen markieren */
			/*             3. Endzeile von Anfang bis Endmarke markieren */

		RectFill(DD->Wnd->RPort,	DD->abx + startx * DD->DI->fbreite,
							DD->bby + starty * DD->DI->fhoehe,
							DD->abx + DD->BPR * DD->DI->fbreite - 1,
							DD->bby + starty * DD->DI->fhoehe + DD->DI->fhoehe - 1 );

		if(( endy - starty ) > 1 )
		{
			RectFill(DD->Wnd->RPort,	DD->abx,
								DD->bby + ( starty + 1 ) * DD->DI->fhoehe,
								DD->abx + DD->BPR * DD->DI->fbreite - 1,
								DD->bby + ( endy - 1 ) * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
		}

		RectFill(DD->Wnd->RPort,	DD->abx,
							DD->bby + endy * DD->DI->fhoehe,
							DD->abx + endx * DD->DI->fbreite + DD->DI->fbreite - 1,
							DD->bby + endy * DD->DI->fhoehe + DD->DI->fhoehe - 1 );
	}
    	}
	
	MySetWriteMask( DD->Wnd->RPort, 0xff );
}

void BlockKuerzen( long start, long end, struct DisplayData *DD )
{
	SetAPen( DD->Wnd->RPort, 0 );

	BlockZeichnen( start, end, TRUE, DD );

	SetAPen( DD->Wnd->RPort, 1 );
}

void BlockErweitern( long start, long end, struct DisplayData *DD )
{
	SetAPen( DD->Wnd->RPort, 2 );

	BlockZeichnen( start, end, FALSE, DD );

	SetAPen( DD->Wnd->RPort, 1 );
}

void ChangeMark(long pos1,long pos2,long off1,long off2, struct DisplayData *DD )
{
	long m1,m2;					/* Hilfsmarken, rel. Pos vorher/nachher */


	m1=pos2-off2*DD->BPR;			/* vorher! */
	m2=pos1-off1*DD->BPR;			/* Bytes zum Ausschnittanfang (nachher)*/

/*kprintf("p1:%ld,p2:%ld,off1:%ld,Off2:%ld,m1:%ld, m2:%ld\n", pos1, pos2, off1, off2,m1, m2 );*/

	if((pos1==pos2)&&(off1==off2))return;

		/* Wechsel von Start und Ende? */

	if((((pos2>=DD->MPos)&&(pos1>=DD->MPos))||
		((pos2<=DD->MPos)&&(pos1<=DD->MPos))))
	{
		long start,end;

		if(pos2<DD->MPos)
		{
			start=pos2;
			end=DD->MPos;
		}
		else
		{
			start=DD->MPos;
			end=pos2;
		}

		if(off2!=off1)	/* Wurde gescrollt ? */
		{
		if(off2<off1)		/* Nach oben */
		{

					/* Prüfen, ob alte DD->CPos inner- oder außerhalb */

			if((pos1>=start)&&(pos1<=end))		/* Innerhalb */
			{
					/* Gleiche Position */

				if( pos1 == pos2 )
				{
					if( DD->MPos < pos2 )
						BlockErweitern( m1, m2, DD );
					else
						BlockKuerzen( m2, m1 - 1, DD );
				}
				else
				{
				/* Falls vorne eine Änderung nötig, ändern */

				if(m1!=m2)
				{
					if(m1>m2) BlockKuerzen( m2, m1 - 1, DD );
					else BlockErweitern( m1, m2, DD );
				}

				/* Falls noch nicht alles gefüllt ist */
				/* Block erweitern */

				if(end<(off1+DD->Zeilen)*DD->BPR)
				{
					start=end-off1*DD->BPR;
					end-=off2*DD->BPR;

					if(end>=DD->Zeilen*DD->BPR)end=DD->Zeilen*DD->BPR-1;
					if(pos2-off2*DD->BPR > start) start = pos2-off2*DD->BPR;

					BlockErweitern( start, end, DD );
				}
				}
			}
			else			/* Außerhalb */
			{
				if(m1!=m2)
				{
					if(m1<m2) BlockKuerzen( m1 + 1, m2, DD );
					else BlockErweitern( m2, m1, DD );
				}

				if(start>DD->BPR*off2)
				{
					end=start-1-off2*DD->BPR;
					start-=off1*DD->BPR;

					if(start<0)start=0;

					BlockKuerzen( start, end, DD );
				}
			}
		}
		else			/* Nach Unten gescrollt */
		{
			if((pos1>=start)&&(pos1<=end))	/* Innerhalb */
			{
						/* Keine Cursorverschiebung */

				if( pos1 == pos2 )
				{
					if( DD->MPos > pos2 )
						BlockErweitern( m1, m2, DD );
					else
						BlockKuerzen( m2, m1 + 1, DD );
				}
				else
				{

				if(m1!=m2)
				{
					if(m1<m2) BlockKuerzen( m1 + 1, m2, DD );
					else BlockErweitern( m2, m1, DD );
				}

				if(start>off1*DD->BPR)
				{
					end=start-off1*DD->BPR;
					start-=off2*DD->BPR;

					if(start<0)start=0;
					if(pos2-off2*DD->BPR < end) end = pos2-off2*DD->BPR;

					BlockErweitern( start, end, DD );
				}
				}
			}
			else
			{
					/* Außerhalb */

				if(m1!=m2)
				{
					if(m1>m2) BlockKuerzen( m2, m1 - 1, DD );
					else BlockErweitern( m1, m2, DD );
				}

				if(end<DD->BPR*(off2+DD->Zeilen))
				{
					start=end+1-off2*DD->BPR;
					end-=off1*DD->BPR;

					if(end>=DD->Zeilen*DD->BPR)end=DD->Zeilen*DD->BPR-1;

					BlockKuerzen( start, end, DD );
				}
			}
		}
		}
		else			/* Er wurde nicht gescrollt */
		{
			/* Falls Alte Pos innerhalb der neuen Markierung, Marke erweitern */
			/* sonst, Marke kürzen */

			if((pos1>=start)&&(pos1<=end))
			{
				if(pos1<pos2) start=pos1;
				else end=pos1;

				start-=off2*DD->BPR;
				end-=off2*DD->BPR;

				BlockErweitern( start, end, DD );
			}
			else
			{
				if(pos1>end)
				{
					start=end+1;
					end=pos1;
				}
				else
				{
					end=start-1;
					start=pos1;
				}

				start-=off2*DD->BPR;
				end-=off2*DD->BPR;

				BlockKuerzen( start, end, DD );
			}
		}
	}
	else	/* Wechsel von Start und End, alten Bereich löschen und neuen zeichnen */
	{
		BlockKuerzen( m2, DD->MPos - off1 * DD->BPR, DD );
		BlockErweitern( m1, DD->MPos - off2 * DD->BPR, DD );
	}
}


/*
 * void ByteToString(UBYTE num,UBYTE *string)
 *
 * Wandelt num in 2 hexadezimal Zeichen und fügt diese in string ein
 */

__inline void ByteToString(UBYTE num,UBYTE *string)
{
	*string++ = hexarray[ num >> 4 ];
	*string = hexarray[ num & 0xf ];
}

void MyText( struct RastPort *rp, WORD x, WORD y, char *text, LONG len )
{
	Move( &TempRP, 0, rp->Font->tf_Baseline );
	Text( &TempRP, text, len );
	ClipBlit( &TempRP, 0, 0, rp, x, y, len * rp->Font->tf_XSize, rp->Font->tf_YSize, 0xc0 );
}


/*
 * static void DisplayZeilen( long von, long bis, long pos, struct DisplayData *DD )
 *
 * Stellt die Daten in den Zeilen von-bis ab pos in einem View dar.
 */

void DisplayZeilen( long von, long bis, long pos, struct DisplayData *DD )
{
	UBYTE mtext[ 10 ];
	long ypos;
	ULONG ap;
	long k,t;
	long offset=0;

	if( DD->FD->Typ == FD_GRAB ) offset = ( long )DD->FD->Mem;

	mtext[8] = ':';

	for(t = von, ypos = DD->bby + DD->DI->fbase + t * DD->DI->fhoehe, ap = ( pos + t ) * DD->BPR;
			t < bis;
			t++, ypos += DD->DI->fhoehe, ap += DD->BPR)
	{
		{
			register UBYTE *ptr = mtext + 7;
			register ULONG aap = ap + offset;

			*ptr--=hexarray[ aap           & 0xf];
			*ptr--=hexarray[(aap = aap>>4) & 0xf];
			*ptr--=hexarray[(aap = aap>>4) & 0xf];
			*ptr--=hexarray[(aap = aap>>4) & 0xf];
			*ptr--=hexarray[(aap = aap>>4) & 0xf];
			*ptr--=hexarray[(aap = aap>>4) & 0xf];
			*ptr--=hexarray[(aap = aap>>4) & 0xf];
			*ptr  =hexarray[(      aap>>4) & 0xf];

			Move( DD->Wnd->RPort, DD->mbx, ypos);
			Text( DD->Wnd->RPort, mtext, 9);
		}

		if( DD->DisplayForm & DF_HEX )
		{
			register unsigned char *ptr = TextLineBuffer;
			register unsigned char *memptr = DD->FD->Mem + ap;

			for( k = 0; k < DD->BPR; k++ )
			{
				*ptr++ = hexarray[*memptr >>  4];
				*ptr++ = hexarray[*memptr++ & 0xf];

				if(( k >> DD->DisplaySpaces ) != (( k + 1 ) >> DD->DisplaySpaces)) *ptr++ = ' ';
			}

			if( ap + DD->BPR > DD->FD->Len )
			{
				ptr = TextLineBuffer + 2 * ( DD->FD->Len - ap ) + (( DD->FD->Len - ap ) >> DD->DisplaySpaces );

				while( ptr < ( unsigned char * )TextLineBuffer + 2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces ))
					*ptr++ = ' ';
			}

MyText( DD->Wnd->RPort, DD->hbx, ypos - DD->DI->fbase, TextLineBuffer, 2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces ));
/*Move( &TempRP, 0, DD->DI->fbase );*/
/*Text( &TempRP, TextLineBuffer, 2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces ));*/
/*ClipBlit( &TempRP, 0,0, DD->Wnd->RPort, DD->hbx, ypos - DD->DI->fbase, (2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DD->DI->fbreite, DD->DI->fhoehe, 0xc0 );*/
/*			Move( DD->Wnd->RPort, DD->hbx, ypos );*/
/*			Text( DD->Wnd->RPort, TextLineBuffer, 2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces ));*/
		}

		if( DD->DisplayForm & DF_ASCII )
		{
			register unsigned char *ptr = TextLineBuffer;
			register unsigned char *memptr = DD->FD->Mem+ap;


			for( k = 0; k < DD->BPR; k++ )
				*ptr++ = displaytab[ DD->DisplayTyp ][ *memptr++ ];
			
				/* Falls letzte Zeile */

			if( ap + DD->BPR > DD->FD->Len)
			{
				ptr = TextLineBuffer + DD->FD->Len - ap;

				while( ptr < ( unsigned char * )TextLineBuffer + DD->BPR)
					*ptr++ = ' ';
			}

MyText( DD->Wnd->RPort, DD->abx, ypos - DD->DI->fbase, TextLineBuffer, DD->BPR );
/*Move( &TempRP, 0, DD->DI->fbase );*/
/*Text( &TempRP, TextLineBuffer, DD->BPR );*/
/*ClipBlit( &TempRP, 0,0, DD->Wnd->RPort, DD->abx, ypos - DD->DI->fbase, DD->BPR * DD->DI->fbreite, DD->DI->fhoehe, 0xc0 );*/
/*			Move( DD->Wnd->RPort, DD->abx, ypos );*/
/*			Text( DD->Wnd->RPort, TextLineBuffer, DD->BPR );*/
		}
	}
}

void DrawAddresses( long von, long bis, struct DisplayData *DD )
{
	UBYTE mtext[ 10 ];
	long offset=0;
	long ypos;
	ULONG ap;

	if( DD->FD->Typ == FD_GRAB ) offset = ( long )DD->FD->Mem;

	ypos = DD->bby + DD->DI->fbase + von * DD->DI->fhoehe;
	ap = ( DD->SPos + von ) * DD->BPR;

	mtext[8] = ':';

	while( von <= bis )
	{
		register UBYTE *ptr = mtext + 7;
		register ULONG aap = ap + offset;

		*ptr--=hexarray[ aap           & 0xf];
		*ptr--=hexarray[(aap = aap>>4) & 0xf];
		*ptr--=hexarray[(aap = aap>>4) & 0xf];
		*ptr--=hexarray[(aap = aap>>4) & 0xf];
		*ptr--=hexarray[(aap = aap>>4) & 0xf];
		*ptr--=hexarray[(aap = aap>>4) & 0xf];
		*ptr--=hexarray[(aap = aap>>4) & 0xf];
		*ptr  =hexarray[(      aap>>4) & 0xf];

		Move( DD->Wnd->RPort, DD->mbx, ypos);
		Text( DD->Wnd->RPort, mtext, 9);
		
		von++;
		ypos += DD->DI->fhoehe;
		ap += DD->BPR;
	}
}

/*
 * void DisplayPart( long von, long bis, struct DisplayData *DD )
 *
 * Stellt die Daten von bis in einem View *neu* dar.
 * Aktuelle Farben werden berücksichtigt.
 */

void DisplayPart( long von, long bis, struct DisplayData *DD )
{
	long ypos;
	long k,t;
	long startzeile, endzeile;
	long HexZeilenBreite;

	HexZeilenBreite = 2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces );

	startzeile = von / DD->BPR;
	endzeile = bis / DD->BPR;

	ypos = DD->bby + DD->DI->fbase + startzeile * DD->DI->fhoehe;

#ifdef __AROS__
    // FIXME: stegerg: check why this is needed!
    SetDrMd(DD->Wnd->RPort, JAM2);
#endif
			

	if( startzeile == endzeile )
	{
		if( DD->DisplayForm & DF_HEX )
		{
			register unsigned char *ptr = TextLineBuffer;
			register unsigned char *memptr = DD->FD->Mem + ( DD->SPos + startzeile ) * DD->BPR ;
			long startoff;

			for( k = 0; k < DD->BPR; k++ )
			{
				*ptr++ = hexarray[*memptr >>  4];
				*ptr++ = hexarray[*memptr++ & 0xf];

				if(( k >> DD->DisplaySpaces ) != (( k + 1 ) >> DD->DisplaySpaces)) *ptr++ = ' ';
			}

			if( von % DD->BPR )
			{
				startoff = ( von % DD->BPR ) * 2 + (( von % DD->BPR - 1 ) >> DD->DisplaySpaces );
			}
			else
				startoff = 0;

			Move( DD->Wnd->RPort, DD->hbx + startoff * DD->DI->fbreite, ypos );
			Text( DD->Wnd->RPort, TextLineBuffer + startoff, ( bis % DD->BPR + 1 ) * 2 + (( bis % DD->BPR - 1 + 1 ) >> DD->DisplaySpaces ) - startoff);
		}

		if( DD->DisplayForm & DF_ASCII )
		{
			register unsigned char *ptr = TextLineBuffer;
			register unsigned char *memptr = DD->FD->Mem + ( DD->SPos + startzeile ) * DD->BPR;
			long startoff;

			for( k = 0; k < DD->BPR; k++ )
				*ptr++ = displaytab[ DD->DisplayTyp ][ *memptr++ ];
			
			startoff = von % DD->BPR;

			Move( DD->Wnd->RPort, DD->abx + startoff * DD->DI->fbreite, ypos );
			Text( DD->Wnd->RPort, TextLineBuffer + startoff, bis % DD->BPR + 1 - startoff );
		}
	}
	else
	{
			/* Startzeile neu darstellen */

		if( von % DD->BPR != 0 )
		{
			if( DD->DisplayForm & DF_HEX )
			{
				register unsigned char *ptr = TextLineBuffer;
				register unsigned char *memptr = DD->FD->Mem + ( DD->SPos + startzeile ) * DD->BPR ;
				long startoff;
	
				for( k = 0; k < DD->BPR; k++ )
				{
					*ptr++ = hexarray[*memptr >>  4];
					*ptr++ = hexarray[*memptr++ & 0xf];
	
					if(( k >> DD->DisplaySpaces ) != (( k + 1 ) >> DD->DisplaySpaces)) *ptr++ = ' ';
				}
	
				startoff = ( von % DD->BPR ) * 2 + (( von % DD->BPR - 1 ) >> DD->DisplaySpaces );

				Move( DD->Wnd->RPort, DD->hbx + startoff * DD->DI->fbreite, ypos );
				Text( DD->Wnd->RPort, TextLineBuffer + startoff, HexZeilenBreite - startoff );
			}
	
			if( DD->DisplayForm & DF_ASCII )
			{
				register unsigned char *ptr = TextLineBuffer;
				register unsigned char *memptr = DD->FD->Mem + ( DD->SPos + startzeile ) * DD->BPR;
				long startoff;
	
				for( k = 0; k < DD->BPR; k++ )
					*ptr++ = displaytab[ DD->DisplayTyp ][ *memptr++ ];
				
				startoff = von % DD->BPR;
			
				Move( DD->Wnd->RPort, DD->abx + startoff * DD->DI->fbreite, ypos );
				Text( DD->Wnd->RPort, TextLineBuffer + startoff, DD->BPR - startoff );
			}
	
			ypos += DD->DI->fhoehe;
		}
		else
		{
			startzeile--;
		}

		if( bis % DD->BPR == 15 )
			endzeile++;
	
			/* Ggf. Mittelteil neu darstellen */

		if( endzeile - startzeile > 1 )
		{
			char *mem = DD->FD->Mem + ( DD->SPos + startzeile + 1 ) * DD->BPR;

			for( t = startzeile + 1; t < endzeile;	t++ )
			{
				if( DD->DisplayForm & DF_HEX )
				{
					register unsigned char *ptr = TextLineBuffer;
					register unsigned char *memptr = mem;
		
					for( k = 0; k < DD->BPR; k++ )
					{
						*ptr++ = hexarray[*memptr >>  4];
						*ptr++ = hexarray[*memptr++ & 0xf];
		
						if(( k >> DD->DisplaySpaces ) != (( k + 1 ) >> DD->DisplaySpaces)) *ptr++ = ' ';
					}
					Move( DD->Wnd->RPort, DD->hbx, ypos );
					Text( DD->Wnd->RPort, TextLineBuffer, HexZeilenBreite );
				}
		
				if( DD->DisplayForm & DF_ASCII )
				{
					register unsigned char *ptr = TextLineBuffer;
					register unsigned char *memptr = mem;
		
					for( k = 0; k < DD->BPR; k++ )
						*ptr++ = displaytab[ DD->DisplayTyp ][ *memptr++ ];

					Move( DD->Wnd->RPort, DD->abx, ypos );
					Text( DD->Wnd->RPort, TextLineBuffer, DD->BPR );
				}

				ypos += DD->DI->fhoehe;
				mem += DD->BPR;
			}
		}
		
			/* Endzeile neu darstellen */

		if( bis % DD->BPR != 15 )
		{
			if( DD->DisplayForm & DF_HEX )
			{
				register unsigned char *ptr = TextLineBuffer;
				register unsigned char *memptr = DD->FD->Mem + ( DD->SPos + endzeile ) * DD->BPR ;
	
				for( k = 0; k < DD->BPR; k++ )
				{
					*ptr++ = hexarray[*memptr >>  4];
					*ptr++ = hexarray[*memptr++ & 0xf];
	
					if(( k >> DD->DisplaySpaces ) != (( k + 1 ) >> DD->DisplaySpaces)) *ptr++ = ' ';
				}
	
				Move( DD->Wnd->RPort, DD->hbx, ypos );
				Text( DD->Wnd->RPort, TextLineBuffer, ( bis % DD->BPR + 1 ) * 2 + (( bis % DD->BPR - 1 + 1 ) >> DD->DisplaySpaces ));
			}
	
			if( DD->DisplayForm & DF_ASCII )
			{
				register unsigned char *ptr = TextLineBuffer;
				register unsigned char *memptr = DD->FD->Mem + ( DD->SPos + endzeile ) * DD->BPR;
	
				for( k = 0; k < DD->BPR; k++ )
					*ptr++ = displaytab[ DD->DisplayTyp ][ *memptr++ ];

				Move( DD->Wnd->RPort, DD->abx, ypos );
				Text( DD->Wnd->RPort, TextLineBuffer, bis % DD->BPR + 1 );
			}
		}
	}
}

/*
 */

void MoveDisplay( long diff, struct DisplayData *DD )
{
	Printf("Diff = %ld\n", diff );

	if( diff > 0 )
	{
			/* Umkopieren des gleichbleibenden Bereichs */

		ClipBlit( DD->Wnd->RPort, DD->mbx, DD->bby + diff * DD->DI->fhoehe, DD->Wnd->RPort, DD->mbx, DD->bby, 9 * DD->DI->fbreite, ( DD->Zeilen - diff ) * DD->DI->fhoehe, 0xc0 );

		if( DD->DisplayForm & DF_HEX )
			ClipBlit( DD->Wnd->RPort, DD->hbx, DD->bby + diff * DD->DI->fhoehe, DD->Wnd->RPort, DD->hbx, DD->bby, (2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DD->DI->fbreite, ( DD->Zeilen - diff ) * DD->DI->fhoehe, 0xc0 );

		if( DD->DisplayForm & DF_ASCII )
			ClipBlit( DD->Wnd->RPort, DD->abx, DD->bby + diff * DD->DI->fhoehe, DD->Wnd->RPort, DD->abx, DD->bby, DD->BPR * DD->DI->fbreite, ( DD->Zeilen - diff ) * DD->DI->fhoehe, 0xc0 );
	}
	else
	{
		ClipBlit( DD->Wnd->RPort, DD->mbx, DD->bby, DD->Wnd->RPort, DD->mbx, DD->bby - diff * DD->DI->fhoehe, 9 * DD->DI->fbreite, ( DD->Zeilen + diff ) * DD->DI->fhoehe, 0xc0 );

		if( DD->DisplayForm & DF_HEX )
			ClipBlit( DD->Wnd->RPort, DD->hbx, DD->bby, DD->Wnd->RPort, DD->hbx, DD->bby - diff * DD->DI->fhoehe, (2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DD->DI->fbreite, ( DD->Zeilen + diff ) * DD->DI->fhoehe, 0xc0 );

		if( DD->DisplayForm & DF_ASCII )
			ClipBlit( DD->Wnd->RPort, DD->abx, DD->bby, DD->Wnd->RPort, DD->abx, DD->bby - diff * DD->DI->fhoehe, DD->BPR * DD->DI->fbreite, ( DD->Zeilen + diff ) * DD->DI->fhoehe, 0xc0 );
	}
}

/*
 * void RedrawDisplay( struct DisplayData *DD );
 */

void RedrawDisplay( struct DisplayData *DD )
{
	long MaxZeilen;

		/* Maximale Zeilen der Datei berechnen */

	MaxZeilen = ((( DD->FD->Len + DD->BPR - 1 ) / DD->BPR > DD->Zeilen) ? DD->Zeilen : ( DD->FD->Len + DD->BPR - 1 ) / DD->BPR );

	if( DD->FD->Len )
	{
		DrawAddresses( 0, ( DD->Zeilen <= MaxZeilen ) ? DD->Zeilen - 1 : MaxZeilen - 1, DD );

			/* Zeilen darstellen */

		if( DD->Flags & DD_MARK )
		{
			long markstart, markend;
			
			if( DD->MPos < DD->CPos )
			{
				markstart = DD->MPos;
				markend = DD->CPos;
			}
			else
			{
				markstart = DD->CPos;
				markend = DD->MPos;
			}
			
			markstart -= DD->SPos * DD->BPR;
			markend -= DD->SPos * DD->BPR;
			
			if( markstart > 0 )
			{
				SetAPen( DD->Wnd->RPort, NormalAPen );
				SetBPen( DD->Wnd->RPort, NormalBPen );
					
				DisplayPart( 0, markstart - 1, DD );
			}
			else
				markstart = 0;
							
			SetAPen( DD->Wnd->RPort, BlockAPen );
			SetBPen( DD->Wnd->RPort, BlockBPen );

			if( markend >= DD->BPR * DD->Zeilen )
			{
				DisplayPart( markstart, DD->BPR * DD->Zeilen - 1, DD );
			}
			else
			{
				DisplayPart( markstart, markend - 1, DD );

				{
					long end;
	
					end = DD->BPR * DD->Zeilen - 1;
					if( end > DD->FD->Len -  DD->BPR * DD->SPos )
						end = DD->FD->Len - DD->BPR * DD->SPos;

					SetAPen( DD->Wnd->RPort, NormalAPen );
					SetBPen( DD->Wnd->RPort, NormalBPen );
					
					DisplayPart( markend + 1, end, DD );
				}
			}
		}
		else
		{
			long end;

			SetAPen( DD->Wnd->RPort, NormalAPen );
			SetBPen( DD->Wnd->RPort, NormalBPen );
				
			end = DD->BPR * DD->Zeilen - 1;
				
			if( end > DD->FD->Len -  DD->BPR * DD->SPos )
				end = DD->FD->Len - DD->BPR * DD->SPos;

			DisplayPart( 0, end, DD );
		}
	}	
	
		/* Ungenutzte Zeilen löschen */

	if( DD->Zeilen != MaxZeilen )
	{
		if( DD->DisplayForm & DF_HEX )
			EraseRect( DD->Wnd->RPort, DD->hbx, DD->bby + MaxZeilen * DD->DI->fhoehe, DD->hbx + (2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DD->DI->fbreite - 1, DD->bby + DD->Zeilen * DD->DI->fhoehe - 1 );

		if( DD->DisplayForm & DF_ASCII )
			EraseRect( DD->Wnd->RPort, DD->abx, DD->bby + MaxZeilen * DD->DI->fhoehe, DD->abx + DD->BPR * DD->DI->fbreite - 1, DD->bby + DD->Zeilen * DD->DI->fhoehe - 1 );

		EraseRect( DD->Wnd->RPort, DD->mbx, DD->bby + MaxZeilen * DD->DI->fhoehe, DD->mbx + 9 * DD->DI->fbreite - 1, DD->bby + DD->Zeilen * DD->DI->fhoehe - 1 );
	}
}

/*
 * void RedrawPart( long von, long bis, struct DisplayData *DD )
 *
 * Stellt die Zeilen von bis neu dar.
 */

void RedrawPart( long von, long bis, struct DisplayData *DD )
{
	long bispos = bis * DD->BPR + DD->BPR - 1, vonpos = von * DD->BPR;

	Printf("RedrawPart: v=%ld b=%ld\n", von, bis );

	if( DD->FD->Len )
	{
		DrawAddresses( von, bis, DD );

			/* Zeilen darstellen */

		if( DD->Flags & DD_MARK )
		{
			long markstart, markend;
			
			if( DD->MPos < DD->CPos )
			{
				markstart = DD->MPos;
				markend = DD->CPos;
			}
			else
			{
				markstart = DD->CPos;
				markend = DD->MPos;
			}
			
			markstart -= DD->SPos * DD->BPR;
			markend -= DD->SPos * DD->BPR;
			
			Printf("Markstart:%ld, Markend:%ld,VP:%ld,BP:%ld\n", markstart, markend, vonpos, bispos );
			
			/* 5 Möglichkeiten:
			 * 1.Markbereich nicht in neuzuzeichnenden Bereich
			 * 2.Markbereich hört auf
			 * 3.Markbereich beginnt
			 * 4.Markbereich beginnt und hört auf
			 * 5.Markbereich beginnt nicht und hört auch nicht auf
			 */
			
			if(( markend < vonpos && markstart < vonpos ) ||
				( markend > bispos && markstart > bispos ))
			{
				Printf("Fall 1\n");
				SetAPen( DD->Wnd->RPort, NormalAPen );
				SetBPen( DD->Wnd->RPort, NormalBPen );
					
				DisplayPart( vonpos, bispos, DD );
			}
			else if( markstart <= vonpos && markend < bispos )
			{
				Printf("Fall 2\n");
				SetAPen( DD->Wnd->RPort, BlockAPen );
				SetBPen( DD->Wnd->RPort, BlockBPen );

				DisplayPart( vonpos, markend, DD );

				SetAPen( DD->Wnd->RPort, NormalAPen );
				SetBPen( DD->Wnd->RPort, NormalBPen );
					
				DisplayPart( markend + 1, bispos, DD );
			}
			else if( markstart > vonpos && markstart <= bispos && markend >= bispos )
			{
				Printf("Fall 3\n");
				SetAPen( DD->Wnd->RPort, NormalAPen );
				SetBPen( DD->Wnd->RPort, NormalBPen );
					
				DisplayPart( vonpos, markstart - 1, DD );

				SetAPen( DD->Wnd->RPort, BlockAPen );
				SetBPen( DD->Wnd->RPort, BlockBPen );

				DisplayPart( markstart, bispos, DD );
			}
			else if( markstart > vonpos && markend < bispos )
			{
				Printf("Fall 4\n");
				SetAPen( DD->Wnd->RPort, NormalAPen );
				SetBPen( DD->Wnd->RPort, NormalBPen );
					
				DisplayPart( vonpos, markstart - 1, DD );

				SetAPen( DD->Wnd->RPort, BlockAPen );
				SetBPen( DD->Wnd->RPort, BlockBPen );

				DisplayPart( markstart, markend, DD );

				SetAPen( DD->Wnd->RPort, NormalAPen );
				SetBPen( DD->Wnd->RPort, NormalBPen );
					
				DisplayPart( markend + 1, bispos, DD );
			}
			else if( markstart < vonpos && markend > bispos )
			{
				Printf("Fall 5\n");
				SetAPen( DD->Wnd->RPort, BlockAPen );
				SetBPen( DD->Wnd->RPort, BlockBPen );

				DisplayPart( vonpos, bispos, DD );
			}
			else
			{
				Printf("Nicht möglich!\n");
			}
		}
		else
		{
			SetAPen( DD->Wnd->RPort, NormalAPen );
			SetBPen( DD->Wnd->RPort, NormalBPen );
				
			DisplayPart( vonpos, bispos, DD );
		}
	}
	
/*	if( DD->Zeilen != MaxZeilen )
	{
		if( DD->DisplayForm & DF_HEX )
			EraseRect( DD->Wnd->RPort, DD->hbx, DD->bby + MaxZeilen * DD->DI->fhoehe, DD->hbx + (2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DD->DI->fbreite - 1, DD->bby + DD->Zeilen * DD->DI->fhoehe - 1 );

		if( DD->DisplayForm & DF_ASCII )
			EraseRect( DD->Wnd->RPort, DD->abx, DD->bby + MaxZeilen * DD->DI->fhoehe, DD->abx + DD->BPR * DD->DI->fbreite - 1, DD->bby + DD->Zeilen * DD->DI->fhoehe - 1 );

		EraseRect( DD->Wnd->RPort, DD->mbx, DD->bby + MaxZeilen * DD->DI->fhoehe, DD->mbx + 9 * DD->DI->fbreite - 1, DD->bby + DD->Zeilen * DD->DI->fhoehe - 1 );
	}*/
}

/*
 * void AllDisplayChar()
 *
 * Stellt ein Byte in allen Displays eines Files neu dar.
 */

void AllDisplayChar( struct FileData *FD, long Offset )
{
	struct DisplayData *DD;
	char text[ 2 ];
	int off, x, y;

	DD = GetDDFromFD( FD->DisplayList.lh_Head );

	while( DD != GetDDFromFD( &FD->DisplayList.lh_Tail ))
	{
		off = Offset - DD->BPR * DD->SPos;

		if(( off >= 0 ) && ( off < DD->BPR * DD->Zeilen ))
		{
			x = off % DD->BPR;
			y = off / DD->BPR;

			MySetWriteMask( DD->Wnd->RPort, 1 );

			if( DD->DisplayForm & DF_ASCII )
			{
				MyText( DD->Wnd->RPort, DD->abx + x * DD->DI->fbreite, DD->bby + y * DD->DI->fhoehe, &displaytab[ DD->DisplayTyp ][ DD->FD->Mem[ Offset ]], 1);
			}

			if( DD->DisplayForm & DF_HEX )
			{
				ByteToString( DD->FD->Mem[ Offset ], text );
	
				x = 2 * x + ( x >> DD->DisplaySpaces );
				MyText( DD->Wnd->RPort, DD->hbx + x * DD->DI->fbreite, DD->bby + y * DD->DI->fhoehe, text, 2 );
			}

			MySetWriteMask(DD->Wnd->RPort,0xff);
		}

		DD = GetDDFromFD( DD->FNode.ln_Succ );
	}
}

/*
 * void AllDisplay()
 *
 * Stellt alle Displays eines Files neu dar.
 */

void AllDisplay( struct FileData *FD, long Pos )
{
	struct DisplayData *DD;

	DD = GetDDFromFD( FD->DisplayList.lh_Head );

	while( DD != GetDDFromFD( &FD->DisplayList.lh_Tail ))
	{
		RedrawDisplay( DD );

		DD = GetDDFromFD( DD->FNode.ln_Succ );
	}
}

/*
 * void MakeDisplay( void )
 * 
 * Window nach dem Öffnen, einer Größenänderung oder einer DisplayFormänderung
 * neu darstellen.
 * 
 * Force = TRUE erzwingt einer Neudarstellung. Im anderen Fall wird
 * geprüft, ob Zeilen und Spaltenzahl gleich bleiben und ggf. wird
 * nicht geändert.
 */

void MakeDisplay( struct DisplayInhalt *DI )
{
	WORD MinWidth, MinHeight;
	BOOL TooSmallX = FALSE, TooSmallY = FALSE;

	struct DisplayData *DD;
	long Zeilen = 0;
	long Anzahl = 0;
	BYTE DisplayForm = 0;
	long YOff;

	SetWaitPointer( DI->Wnd );

		/* Alle Displaybereiche durchgehen */

	DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

	while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
	{
		Anzahl++;
		Zeilen += DD->Zeilen; 

		if( DisplayForm < DD->DisplayForm ) 
			DisplayForm = DD->DisplayForm;
		
		DD = ( struct DisplayData *)DD->Node.ln_Succ;
	}

		/* Minimale Breite und Höhe des Fensters bei eigestelltem Font */
		/* und eigestellten Displayoptionen berechnen */

	MinWidth = DI->Wnd->BorderLeft + DI->Wnd->BorderRight + BOXADDX + 9 * DI->fbreite;

	if( DisplayForm & DF_ASCII )
		MinWidth += DI->fbreite + BOXADDX;

	if( DisplayForm & DF_HEX )
		MinWidth += 2 * DI->fbreite + BOXADDX;

	MinHeight = DI->Wnd->BorderTop + DI->Wnd->BorderBottom;
	MinHeight += 2 * Anzahl * (DI->fhoehe + BOXADDY);

		/* Prüfen, ob das Window zu klein ist? */

	if( DI->Wnd->Height < MinHeight )
	{
			/* Alle Bereiche auf eine Zeile setzten */

		DD = ( struct DisplayData * )DI->DisplayList.lh_Head;
		while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
		{
			DD->Zeilen = 1;
		
			DD = ( struct DisplayData *)DD->Node.ln_Succ;
		}

		if( DI->Wnd->Height != Scr->Height )
			TooSmallY = TRUE;
	}
	else
	{
		WORD MaxZeilen;

		MaxZeilen = ( DI->Wnd->Height - MinHeight ) / DI->fhoehe + Anzahl;

		if( Zeilen != MaxZeilen )
		{
		if( Zeilen > MaxZeilen )
		{
			WORD Zuviel = Zeilen - MaxZeilen;

			DD = ( struct DisplayData * )DI->DisplayList.lh_TailPred;
			
			while( Zuviel && (DD != ( struct DisplayData *)&DI->DisplayList.lh_Head ))
			{
				if( DD->Zeilen - 1 > Zuviel )
				{
					DD->Zeilen -= Zuviel;
					Zuviel = 0;
				}
				else
				{
					Zuviel -= DD->Zeilen - 1;
					DD->Zeilen = 1;
				}

				DD = ( struct DisplayData * )DD->Node.ln_Pred;
			}
		}
		else
		{
			((struct DisplayData *)DI->DisplayList.lh_TailPred)->Zeilen += MaxZeilen - Zeilen;
		}
		}
	}

	if( DI->Wnd->Width < MinWidth )
	{
		DD->BPR = 1;

		if( DI->Wnd->Width != Scr->Width )
			TooSmallX = TRUE;
	}
	else
	{
		WORD TempBreite;

			/* Anzahl der Bytes pro Zeile für jeden Bereich berechen */
	
		TempBreite = DI->Wnd->Width - DI->Wnd->BorderLeft - DI->Wnd->BorderRight - DI->fbreite * 9 - BOXADDX;

		DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

		while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
		{
			DD->BPR = TempBreite;

			if( DD->DisplayForm & DF_HEX )
				DD->BPR -= BOXADDX;
			
			if( DD->DisplayForm & DF_ASCII )
				DD->BPR -= BOXADDX;
			
			DD->BPR /= DI->fbreite;
			
			if(( DD->DisplayForm & DF_ASCII ) && (DD->DisplayForm & DF_HEX ))
			{
				long Rest;
		
				Rest = DD->BPR % ( 3 * (1 << DD->DisplaySpaces) + 1 );
				DD->BPR = (1 << DD->DisplaySpaces) * (DD->BPR / ( 3 * (1 << DD->DisplaySpaces) + 1 ));
		
				DD->BPR += Rest / 3;
			}
			else if( DD->DisplayForm & DF_HEX )
			{
				long Rest;
		
				Rest = DD->BPR % ( 2 * (1 << DD->DisplaySpaces) + 1 );
				DD->BPR = (1 << DD->DisplaySpaces) * (DD->BPR / ( 2 * (1 << DD->DisplaySpaces) + 1 ));
		
				DD->BPR += Rest / 2;
			}

			CalcStatusZeilenBreite( DD );

			DD = ( struct DisplayData *)DD->Node.ln_Succ;
		}
	}

		/* Minimale Windowausmaße neu setzen */

	if( TooSmallX || TooSmallY )
	{
		ChangeWindowBox( DI->Wnd,	DI->Wnd->LeftEdge,
									DI->Wnd->TopEdge,
									TooSmallX ? MinWidth : DI->Wnd->Width,
									TooSmallY ? MinHeight : DI->Wnd->Height );
		Delay( 5 );
	}

	WindowLimits( DI->Wnd, MinWidth, MinHeight, 0, 0 );

	YOff = DI->Wnd->BorderTop + BOXADDY/2;

	DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

	while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
	{
			/* Abstand der einzelnen Displaybereiche zum linken Rand berechnen */
	
		DD->sbx = DI->Wnd->BorderLeft + BOXADDX / 2;
		DD->mbx = DI->Wnd->BorderLeft + BOXADDX / 2;
	
		if( DD->DisplayForm & DF_HEX)
			DD->hbx = DI->Wnd->BorderLeft + BOXADDX / 2 + BOXADDX + 9 * DI->fbreite;
	
		if( DD->DisplayForm & DF_ASCII )
		{
			if( DD->DisplayForm & DF_HEX)
				DD->abx = BOXADDX + ( 2 * DD->BPR + ((( DD->BPR - 1 ) >> DD->DisplaySpaces )) + 9 ) * DI->fbreite;
			else
				DD->abx = 9 * DI->fbreite;
	
			DD->abx += BOXADDX + DI->Wnd->BorderLeft + BOXADDX / 2;
		}
	
			/* Y-Abstand der Displaybereiche zum oberen Fensterrahmen berechenen */
	
		DD->sbby = YOff;
		DD->bby = DD->sbby + BOXADDY + DI->fhoehe;

		YOff += 2 * BOXADDY + ( DD->Zeilen + 1 ) * DI->fhoehe;

		DD = ( struct DisplayData *)DD->Node.ln_Succ;
	}

		/* Fensterinhalt löschen */

	EraseRect(	DI->Wnd->RPort,
					DI->Wnd->BorderLeft,
					DI->Wnd->BorderTop,
					DI->Wnd->Width - DI->Wnd->BorderRight - 1,
					DI->Wnd->Height - DI->Wnd->BorderBottom - 1 );


		/* Scrollrand für neue Fenstergröße berechnen */

	SetScrollRand( realscrollrand );

	DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

	while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
	{
		{
			long TempHoehe, TempTyp;
			struct DrawInfo *dri;
			Object *sizeimage;
			ULONG sizew = 18, sizeh = 10;
			ULONG voffset;

			dri = GetScreenDrawInfo(DI->Wnd->WScreen);
			if (dri)
			{
			    sizeimage = NewObject(NULL, SYSICLASS,
				    	SYSIA_Which, SIZEIMAGE,
					SYSIA_DrawInfo, (ULONG)dri,
					TAG_DONE);

			    if (sizeimage)
			    {
				GetAttr(IA_Width, sizeimage, &sizew);
				GetAttr(IA_Height, sizeimage, &sizeh);
			    }
			    FreeScreenDrawInfo(DI->Wnd->WScreen, dri);
			}

			voffset = sizew / 4;

			if( DD->Node.ln_Succ == ( struct Node * )&DI->DisplayList.lh_Tail )
			{
				TempHoehe = -DI->Wnd->Height + DI->Wnd->Height - sizeh - 1 - DD->sbby + 1;
				TempTyp = GA_RelHeight;
			}
			else
			{
				TempHoehe = (DD->Zeilen+1) * DI->fhoehe + 2 * BOXADDY - 2;
				TempTyp = GA_Height;
			}
			
			if( DD->PropGadget )
			{
				SetGadgetAttrs(DD->PropGadget,DI->Wnd,0,
									GA_Top,			DD->sbby - 1,
									TempTyp,			TempHoehe,
							TAG_DONE);
			}
			else
			{
				
				if(( DD->PropGadget = ( struct Gadget * ) NewObject( NULL, "propgclass",
						ICA_TARGET,		ICTARGET_IDCMP,
						GA_UserData,			DD,
						GA_Top,			DD->sbby - 1,
						GA_RelRight,	-sizew + voffset + 1,
						GA_Width,	sizew - voffset * 2,
						TempTyp,		TempHoehe,
						GA_RelVerify,	TRUE,
						GA_Immediate,	TRUE,
						PGA_Total,		1,
						PGA_Top,			0,
						PGA_Visible,	1,
						PGA_NewLook,	TRUE,
						PGA_Borderless,Kick30 ? TRUE : FALSE,
						GA_RightBorder, TRUE,
						TAG_END )))
					AddGadget( DI->Wnd, DD->PropGadget, 0);
			}
		}
			/* Statuszeilenbevelbox zeichnen */
	
		DrawBevelBox(DI->Wnd->RPort, DD->sbx - 4, DD->sbby - 2, DD->StatusZeilenBreite + BOXADDX, DI->fhoehe + 4, GT_VisualInfo, VisualInfo, TAG_DONE);

			/* Bevelboxes zeichen */
	
		DrawBevelBox(DI->Wnd->RPort, DD->mbx - 4, DD->bby - 2,  9 * DI->fbreite + 8, DD->Zeilen * DI->fhoehe + 4, GT_VisualInfo, VisualInfo, TAG_DONE);
		if( DD->DisplayForm & DF_HEX )
			DrawBevelBox(DI->Wnd->RPort, DD->hbx - 4, DD->bby - 2, (2 * DD->BPR + (( DD->BPR - 1 ) >> DD->DisplaySpaces )) * DI->fbreite + 8, DD->Zeilen * DI->fhoehe + 4, GT_VisualInfo, VisualInfo, TAG_DONE);
		if( DD->DisplayForm & DF_ASCII )
			DrawBevelBox(DI->Wnd->RPort, DD->abx - 4, DD->bby - 2, DD->BPR * DI->fbreite + 8, DD->Zeilen * DI->fhoehe + 4, GT_VisualInfo, VisualInfo, TAG_DONE);

		/* Scroller nach Position stellen und Gadget setzen */

		DD->SPos = DD->CPos / DD->BPR - DD->ScrollRand;
		if( DD->SPos < 0 ) DD->SPos = 0;
		SetScrollerGadget( DD );

			/* Displayinhalt neu darstellen */

		RedrawDisplay( DD );

			/* Cursor oder Markierung neu zeichnen */

		if( !( DD->Flags & DD_MARK ))
			CursorOn( DD );
		else
			MarkOn( DD );

		UpdateStatusZeile( DD );

		DD = ( struct DisplayData *)DD->Node.ln_Succ;
	}

	RefreshWindowFrame( DI->Wnd );

	ClrWaitPointer( DI->Wnd );
}

long GetFitHeight( struct DisplayInhalt *DI )
{
	struct DisplayData *DD;
	WORD Anzahl = 0, Zeilen = 0;

	DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

	while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
	{
		Anzahl++;
		Zeilen += DD->Zeilen;

		DD = ( struct DisplayData * )DD->Node.ln_Succ;
	}

	return(( Zeilen + Anzahl ) * DI->fhoehe + 2 * Anzahl * BOXADDY );
}

long GetFitWidth( struct DisplayInhalt *DI )
{
	struct DisplayData *DD;
	WORD MaxWidth = 0;

	DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

	while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
	{
		CalcStatusZeilenBreite( DD );

		if( MaxWidth < DD->StatusZeilenBreite )
			MaxWidth = DD->StatusZeilenBreite;

		DD = ( struct DisplayData * )DD->Node.ln_Succ;
	}

	return( MaxWidth + BOXADDX );
}

void AdjustWindowSize( struct DisplayInhalt *DI )
{
	if( DI->Wnd )
	{
		ChangeWindowBox( DI->Wnd,	DI->Wnd->LeftEdge,
											DI->Wnd->TopEdge,
											GetFitWidth( DI ) + DI->Wnd->BorderRight + DI->Wnd->BorderLeft,
											GetFitHeight(DI ) + DI->Wnd->BorderTop + DI->Wnd->BorderBottom );
	}
}

void SetStatusZeile( CONST_STRPTR String, struct DisplayData *DD )
{
	WORD StringLaenge;

	StringLaenge = strlen( String );

	if( StringLaenge > DD->StatusZeilenBreite / DD->DI->fbreite )
		StringLaenge = DD->StatusZeilenBreite / DD->DI->fbreite;

	if( DD->DI->AktuDD == DD )
		SetAPen( DD->Wnd->RPort, 3 );
	else	
		SetAPen( DD->Wnd->RPort, 0 );

	RectFill( DD->Wnd->RPort, DD->sbx - 2, DD->sbby - 1, DD->sbx - 1, DD->sbby + DD->DI->fhoehe + 1 - 1 );
	RectFill( DD->Wnd->RPort, DD->sbx - 2, DD->sbby - 1, DD->sbx + DD->StatusZeilenBreite + 2 - 1, DD->sbby - 1 );
	RectFill( DD->Wnd->RPort, DD->sbx - 2, DD->sbby + DD->DI->fhoehe + 1 - 1, DD->sbx + DD->StatusZeilenBreite + 2 - 1, DD->sbby + DD->DI->fhoehe + 1 - 1 );
	RectFill( DD->Wnd->RPort, DD->sbx - 2 + StringLaenge * DD->DI->fbreite, DD->sbby - 1, DD->sbx + DD->StatusZeilenBreite + 2 - 1, DD->sbby + DD->DI->fhoehe + 1 - 1 );
		
	if( DD->DI->AktuDD == DD )
	{
		SetAPen( DD->Wnd->RPort, 1 );
		SetBPen( DD->Wnd->RPort, 3 );
	}
	else	
	{
		SetAPen( DD->Wnd->RPort, 1 );
		SetBPen( DD->Wnd->RPort, 0 );
	}

	Move( DD->Wnd->RPort, DD->sbx, DD->sbby + DD->DI->fbase );
	Text( DD->Wnd->RPort, String, StringLaenge );

	SetBPen( DD->Wnd->RPort, 0 );
	SetAPen( DD->Wnd->RPort, 1 );
}

void UpdateStatusZeile( struct DisplayData *DD )
{
	UBYTE String[256];

	if( DD->FD->Typ == FD_GRAB )
	{
		char Buffer[ 3 ][ 10 ] = { "$", "$", "$" };
		stcl_h( Buffer[ 0 ] + 1, ( long )DD->FD->Mem );
		stcl_h( Buffer[ 1 ] + 1, ( long )DD->FD->Mem + DD->FD->Len );
		stcl_h( Buffer[ 2 ] + 1, ( long )DD->FD->Mem + DD->CPos );
		sprintf( String, GetStr( MSG_WINDOWTITLE_GRABMODE ), Buffer[ 2 ], Buffer[ 0 ], Buffer[ 1 ]);
	}
	else
	{
		char Buffer[ 2 ][ 10 ] = { "$", "$" };

		stcl_h( Buffer[ 0 ] + 1, DD->CPos );
		stcl_h( Buffer[ 1 ] + 1, DD->FD->Len );

		{
			long Changes;

			Changes = DD->FD->FullChanges + DD->FD->Changes + DD->FD->RedoChanges;

			if( Changes ) strcpy( String, "* " );
			else strcpy( String, "  " );
	
			sprintf( String + 2, GetStr( MSG_WINDOWTITLE_FILEMODE ), Buffer[ 0 ], Buffer[ 1 ], DD->FD->Name );

			if( !FileLoaded( DD->FD ))
				strcat( String, GetStr( MSG_INFO_GLOBAL_UNNAMED ));
		}
	}
	
	SetStatusZeile( String, DD );
}

void SetScrollRand( short new )
{
	struct DisplayData *DD;
	struct DisplayInhalt *DI;

	realscrollrand = new;

	DI = ( struct DisplayInhalt * )DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
		DD = ( struct DisplayData * ) DI->DisplayList.lh_Head;

		while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
		{
			if(( DD->Zeilen - 1 ) / 2 < realscrollrand ) DD->ScrollRand = ( DD->Zeilen - 1 ) / 2;
			else DD->ScrollRand = realscrollrand;

			DD = ( struct DisplayData *)DD->Node.ln_Succ;
		}

		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}
}
/*
 * void WechselCursorBereich(void)
 *
 * Wechselt den Cursor zwischen Ascii-aktiv und Hex-aktiv
 */

void WechselCursorBereich( struct DisplayData *DD )
{
	if( DD->FD->Len == 0 )
		return;

	CursorOff( DD );

	if( DD->Flags & DD_HEX )
		DD->Flags &= ~DD_HEX;
	else
		DD->Flags |= DD_HEX;

	CursorOn( DD );
}

BOOL MakeDisplayBigger( struct DisplayData *DD )
{
	struct DisplayData *DDT;
	BOOL Done = FALSE;

	DDT = ( struct DisplayData * )DD->Node.ln_Succ;

	while(( !Done ) && ( DDT != ( struct DisplayData * )&DD->DI->DisplayList.lh_Tail ))
	{
		if( DDT->Zeilen > 1 )
		{
			DDT->Zeilen--;
			DD->Zeilen++;
			Done = TRUE;
		}

		DDT = ( struct DisplayData * )DDT->Node.ln_Succ;
	}

	if( !Done )
	{
		DDT = ( struct DisplayData * )DD->Node.ln_Pred;

		while(( !Done ) && ( DDT != ( struct DisplayData * )&DD->DI->DisplayList.lh_Head ))
		{
			if( DDT->Zeilen > 1 )
			{
				DDT->Zeilen--;
				DD->Zeilen++;
				Done = TRUE;
			}

			DDT = ( struct DisplayData * )DDT->Node.ln_Pred;
		}
	}

	if( Done )
		MakeDisplay( DD->DI );

	return( Done );
}

void ChangeAktuView( struct DisplayData *DD )
{
	if( DD != AktuDD )
	{
		struct DisplayData *OldDD;
		
		OldDD = AktuDD;
		AktuDD = AktuDI->AktuDD = DD;
		
		UpdateStatusZeile( OldDD );
		UpdateStatusZeile( AktuDD );
		SetDisplayCheckMarks();
	}
}

void ActivateNextDisplay( void )
{
	struct DisplayData *DD;

	DD = ( struct DisplayData * )AktuDD->Node.ln_Succ;

	if( DD == ( struct DisplayData * )&AktuDD->DI->DisplayList.lh_Tail )
		DD = ( struct DisplayData * )AktuDD->DI->DisplayList.lh_Head;

	ChangeAktuView( DD );
}

void ActivatePreviousDisplay( void )
{
	struct DisplayData *DD;

	DD = ( struct DisplayData * )AktuDD->Node.ln_Pred;

	if( DD == ( struct DisplayData * )&AktuDD->DI->DisplayList.lh_Head )
		DD = ( struct DisplayData * )AktuDD->DI->DisplayList.lh_TailPred;

	ChangeAktuView( DD );
}

BOOL MakeDisplayMax( struct DisplayData *DD )
{
	struct DisplayData *DDT;
	WORD Zeilen = 0;

	DDT = ( struct DisplayData * )DD->DI->DisplayList.lh_Head;

	while( DDT != ( struct DisplayData * )&DD->DI->DisplayList.lh_Tail )
	{
		if( DD != DDT )
		{
			Zeilen += DDT->Zeilen - 1;
			DDT->Zeilen = 1;
		}

		DDT = ( struct DisplayData *)DDT->Node.ln_Succ;
	}

	if( Zeilen )
	{
		DD->Zeilen += Zeilen;
		MakeDisplay( DD->DI );
		return( TRUE );
	}
	else
		return( FALSE );
}

BOOL MakeDisplaySmaller( struct DisplayData *DD )
{
	struct DisplayData *DDT;
	BOOL Done = FALSE;

	if( DD->Zeilen == 1 )
		return( FALSE );

	DDT = ( struct DisplayData * )DD->Node.ln_Succ;

	if( DDT != ( struct DisplayData * )&DD->DI->DisplayList.lh_Tail )
	{
		DDT->Zeilen++;
		DD->Zeilen--;
	}
	else
	{
		DDT = ( struct DisplayData * )DD->Node.ln_Pred;

		if( DDT != ( struct DisplayData * )&DD->DI->DisplayList.lh_Head )
		{
			DDT->Zeilen++;
			DD->Zeilen--;
		}
		else
			return( FALSE );
	}

	MakeDisplay( DD->DI );

	return( Done );
}

BOOL NewDisplay( BOOL Split )
{
	struct DisplayData *New;
	struct FileData *FD;

	if( AktuDD->Zeilen > 4 )
	{
		if( !Split )
		{
			FD = AllocFD( FD_FILE );
			strcpy( FD->Name, AktuDD->FD->Name );
			*FilePart( FD->Name ) = 0;
		}
		else
			FD = AktuDD->FD;

		if(( New = AllocDD( AktuDD->DI, FD )))
		{
			if( Split )
			{
				New->CPos = AktuDD->CPos;
				New->SPos = AktuDD->SPos;
				New->DisplayForm = AktuDD->DisplayForm;
				New->DisplaySpaces = AktuDD->DisplaySpaces;
			}
	
			Remove( &New->Node );
			Insert( &AktuDI->DisplayList, &New->Node, &AktuDD->Node );
	
			New->Zeilen = ( AktuDD->Zeilen-1 ) / 2;
			AktuDD->Zeilen = ( AktuDD->Zeilen ) / 2;
	
			AktuDD = AktuDI->AktuDD = New;
	
			SetDisplayCheckMarks();
	
			MakeDisplay( AktuDI );
	
			return( TRUE );	
		}
		else
			if( FD != AktuDD->FD )
				FreeFD( FD );
	}
	else
	{
		if( MakeDisplayMax( AktuDD ))
			return( NewDisplay( Split ));
		return( FALSE );
	}
	
	return FALSE;
}

/*
 * BOOL CloseView( struct DisplayData *DD )
 *
 * Liefert TRUE, falls Fenster geschlossen wurde
 */

BOOL CloseView( struct DisplayData *DD, BOOL Force )
{
	struct DisplayInhalt *DI;
	BOOL LastOne = FALSE;

	if( Force || QuitView( 2, DD ))
	{
		DI = DD->DI;

		if( ElementZahl( &DD->DI->DisplayList ) == 1 )
			LastOne = TRUE;

			/* Falls es das letzte Display war, ganzes Fenster schließen */

		if( LastOne )
		{
			FreeDD( DD );

			LastOne = FALSE;

				/* Letztes Fenster? */

			if( ElementZahl( &DisplayInhaltList ) == 1 )
				LastOne = TRUE;

				/* Falls aktuelles Fenster, nächstes aktivieren */

			if( DI == AktuDI )
				ActivatePreviousWindow();

			CloseFileXWindow( DI );
			FreeDI( DI );

				/* Falls letztes Fenster, FileX beenden */

			if( LastOne )
				mainflags |= MF_ENDE;

			return( TRUE );
		}
		else
		{
				/* Falls aktuelles Display, nächstes aktivieren */

			if( DD == AktuDD )
			{
				if(( struct DisplayData * )DD->DI->DisplayList.lh_Head == DD )
					ActivateNextDisplay();
				else
					ActivatePreviousDisplay();
    	    	    	}
			FreeDD( DD );

			MakeDisplay( DI );
			return( FALSE );
		}
	}
	else
		return( FALSE );
}

BOOL CloseAktuView( void )
{
	return( CloseView( AktuDD, FALSE ));
}


BOOL CloseDIWindow( struct DisplayInhalt *DI, BOOL Force )
{
	struct DisplayData *DD, *NextDD;
	BOOL LastOne = FALSE;

	if( Force || QuitRequester( 1, DI ))
	{
			/* Letztes Fenster? */

		if( ElementZahl( &DisplayInhaltList ) == 1 )
			LastOne = TRUE;

		if( DI == AktuDI )
			ActivatePreviousWindow();

		DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

		while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
		{
			NextDD = ( struct DisplayData * )DD->Node.ln_Succ;
	
			FreeDD( DD );
	
			DD = NextDD;
		}

			/* Falls es das letzte Fenster war, Programm beenden */

		if( LastOne )
			mainflags |= MF_ENDE;

		CloseFileXWindow( DI );
		FreeDI( DI );

			/* Falls letztes Fenster, nichts */
			/* Programm muß beendet oder ein DummyDisplay muß geöffnet werden */

		return( TRUE );
	}
	else
		return( FALSE );
}

BOOL CloseAktuWindow( void )
{
	return( CloseDIWindow( AktuDI, FALSE ));
}



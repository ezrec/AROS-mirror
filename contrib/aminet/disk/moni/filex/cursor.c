#ifdef USE_PROTO_INCLUDES
#include <proto/graphics.h>
#include <proto/dos.h>
#endif

#include "filexstructs.h"
#include "allprotos.h"

void CursorMarkOn( struct DisplayData *DD )
{
	if( DD->Flags & DD_MARK )
		MarkOn( DD );
	else
		CursorOn( DD );
}

void CursorMarkOff( struct DisplayData *DD )
{
	if( DD->Flags & DD_MARK )
		MarkOff( DD );
	else
		CursorOff( DD );
}

void ExpandBlock( long x1, long x2, struct DisplayData *DD )
{
	long s1, s2;

	x1 -= DD->SPos * DD->BPR;
	x2 -= DD->SPos * DD->BPR;

	if( x1 < x2 )
	{
		s1 = x1;
		s2 = x2;
	}
	else
	{
		s1 = x2;
		s2 = x1;
	}

	SetAPen( DD->Wnd->RPort, BlockAPen );
	SetBPen( DD->Wnd->RPort, BlockBPen );
	DisplayPart( s1, s2, DD );
}

void ShrinkBlock( long x1, long x2, struct DisplayData *DD )
{
	long s1, s2;

	x1 -= DD->SPos * DD->BPR;
	x2 -= DD->SPos * DD->BPR;

	if( x1 < x2 )
	{
		s1 = x1;
		s2 = x2;
	}
	else
	{
		s1 = x2;
		s2 = x1;
	}

	SetAPen( DD->Wnd->RPort, NormalAPen );
	SetBPen( DD->Wnd->RPort, NormalBPen );
	DisplayPart( s1, s2, DD );
}

/*
 * void SetCursor( long Pos, struct DisplayData *DD )
 *
 * Setzt den Cursor an die Stelle Pos(in Bytes). Der Cursor muﬂ schon
 * irgendwo anders stehen.
 */

void SetCursor( long Pos, struct DisplayData *DD )
{
/*	Printf("SetCursor(%ld) %ld,\n", Pos, DD->CPos);*/

	if( DD->FD->Len == 0 ) return;

		/* HexEditPos auf obere 4Bits setzten */

	DD->Flags &= ~DD_HEXEDITPOS;

		/* Von Pos = 0 ins negative =>nichts machen */

	if(( DD->CPos == 0 ) && ( Pos < 0 )) return;

		/* Cursor nach hinten und stehen wir in der letzten Zeile? */

	if(( Pos > DD->CPos ) && (( DD->CPos / DD->BPR ) == ( DD->FD->Len / DD->BPR )))
	{
			/* Soll der Cursor an das Ende der Zeile? */
			/* Wenn ja, dann Cursor an das Ende des Files setzen */
			/* Wenn nein und der Cursor auﬂerhalb stehen soll, abbrechen */

		if( Pos % DD->BPR == DD->BPR - 1 )
			Pos = DD->FD->Len - 1;
		else
			if( Pos >= DD->FD->Len )
				return;
	}

		/* Falls der Cursor nicht im Filebereich, anpassen */

	if( Pos < 0 )
		Pos = ( Pos + DD->BPR * DD->Zeilen ) % DD->BPR;
	else
		if( Pos >= DD->FD->Len )
		{
			if( DD->FD->Len % DD->BPR <= Pos % DD->BPR )
				Pos = Pos % DD->BPR + ( DD->FD->Len / DD->BPR - 1 ) * DD->BPR;
			else
				Pos = Pos % DD->BPR + DD->FD->Len / DD->BPR * DD->BPR;
		}

		/* Gleiche Position und keine Verschiebung => abbrechen */

	if( Pos == DD->CPos )
	if((( Pos >= DD->SPos * DD->BPR + DD->ScrollRand * DD->BPR ) &&
			( Pos < ( DD->SPos + DD->Zeilen - DD->ScrollRand ) * DD->BPR )) ||
		( Pos < DD->ScrollRand * DD->BPR ) ||
		( Pos > ( DD->FD->Len - DD->ScrollRand * DD->BPR )))
			return;

		/* Im Markierungsmodus dieses Ex-Riesenbaby abarbeiten */
		/* Version 2.1: Total ge‰ndert und wieder l‰nger!! */

	if( DD->Flags & DD_MARK )
	{
		long PosZeile = Pos / DD->BPR;

			/* Falls neue CursorPos auﬂerhalb, in den neuen Bereich scrollen */

		if( PosZeile < DD->SPos + DD->ScrollRand )
		{
			long diff = PosZeile - ( DD->SPos + DD->ScrollRand );

			Printf("Diff = %ld\n", diff );
		
			if( -diff > DD->Zeilen )
			{
				DD->SPos += diff;
				DD->CPos = Pos;

				RedrawDisplay( DD );
			}
			else
			{
				long tp = Pos;

				if( tp < DD->SPos * DD->BPR )
					tp = DD->SPos * DD->BPR;

				if( tp >= DD->MPos )
				{
					if( tp != Pos ) tp--;
					ShrinkBlock( DD->CPos, tp + 1, DD );
				}
				else if( DD->CPos <= DD->MPos )
				{
					ExpandBlock( tp, DD->CPos - 1, DD );
				}
				else
				{
					ShrinkBlock( DD->CPos, DD->MPos + 1, DD );
					ExpandBlock( DD->MPos - 1, tp, DD );
				}

				DD->SPos += diff;
				DD->CPos = Pos;
					
				MoveDisplay( diff, DD );
				RedrawPart( 0, - diff - 1, DD );
				SetScrollerGadget( DD );
			}
		}
		else if( PosZeile >= DD->SPos + DD->Zeilen - DD->ScrollRand )
		{
			long diff = PosZeile - ( DD->SPos + DD->Zeilen - DD->ScrollRand - 1 );

			Printf("Diff = %ld\n", diff );

			if( diff > DD->Zeilen )
			{
				DD->SPos += diff;
				DD->CPos = Pos;

				RedrawDisplay( DD );
			}
			else
			{
				long tp = Pos;

				if( tp >= ( DD->SPos + DD->Zeilen ) * DD->BPR )
					tp = ( DD->SPos + DD->Zeilen ) * DD->BPR - 1;

				if( tp <= DD->MPos )
				{
					if( tp != Pos ) tp++;

					ShrinkBlock( DD->CPos, tp - 1, DD );
				}
				else if( DD->CPos >= DD->MPos )
				{
					ExpandBlock( tp, DD->CPos + 1, DD );
				}
				else
				{
					ShrinkBlock( DD->CPos, DD->MPos - 1, DD );
					ExpandBlock( DD->MPos + 1, tp, DD );
				}

				

				DD->SPos += diff;
				DD->CPos = Pos;

				MoveDisplay( diff, DD );
				RedrawPart( DD->Zeilen - diff, DD->Zeilen - 1, DD );
				SetScrollerGadget( DD );
			}
		}
		else
		{
				/* Es fand keine Verschiebung statt, wir m¸ssen aber */
				/* den Block erweitern oder k¸rzen */

			if( DD->CPos == DD->MPos )
			{
				if( Pos < DD->CPos )
				{
					ExpandBlock( DD->CPos - 1, Pos, DD );
				}
				else
				{
					ExpandBlock( DD->CPos + 1, Pos, DD );
				}
			}
			else if( DD->CPos > DD->MPos )
			{
				if( Pos > DD->CPos )
				{
					ExpandBlock( DD->CPos + 1, Pos, DD );
				}
				else if( Pos >= DD->MPos )
				{
					ShrinkBlock( DD->CPos, Pos + 1, DD );
				}
				else
				{
					ShrinkBlock( DD->MPos + 1, DD->CPos, DD );
					ExpandBlock( DD->MPos - 1, Pos, DD );
				}
			}
			else
			{
				if( Pos < DD->CPos )
				{
					ExpandBlock( DD->CPos - 1, Pos, DD );
				}
				else if( Pos <= DD->MPos )
				{
					ShrinkBlock( DD->CPos, Pos - 1, DD );
				}
				else
				{
					ShrinkBlock( DD->MPos - 1, DD->CPos, DD );
					ExpandBlock( DD->MPos + 1, Pos, DD );
				}
			}

			DD->CPos = Pos;
		}
	}
	else
	{
		long PosZeile = Pos / DD->BPR;

		CursorOff( DD );

			/* Falls eine Verschiebung ¸berhaupt mˆglich ist */

		if( DD->FD->Len > DD->Zeilen * DD->BPR )
		{
				/* Falls Cursor im unteren oder oberen DD->ScrollRand steht, */
				/*  Display verschieben */

			if( PosZeile < DD->SPos + DD->ScrollRand )
			{
				long diff = PosZeile - ( DD->SPos + DD->ScrollRand );

				if( DD->SPos + diff < 0 )
					diff = -DD->SPos;

Printf("Diff=%ld, PosZeile=%ld\n", diff, PosZeile );

				DD->SPos += diff;
				DD->CPos = Pos;

				if( diff )
				{
				if( -diff > DD->Zeilen )
				{
					RedrawDisplay( DD );
				}
				else
				{
					MoveDisplay( diff, DD );
					RedrawPart( 0, - diff - 1, DD );
					SetScrollerGadget( DD );
				}
				}
			}
			else
			if( Pos >= ( DD->SPos + DD->Zeilen - DD->ScrollRand ) * DD->BPR )
			{
				long diff = PosZeile - ( DD->SPos + DD->Zeilen - DD->ScrollRand - 1 );
				long ZeilenImFile = ( DD->FD->Len + DD->BPR - 1 ) / DD->BPR;
Printf("Diff=%ld, PosZeile=%ld\n", diff, PosZeile );

				if( DD->SPos + diff + DD->Zeilen >  ZeilenImFile )
				{
					if( ZeilenImFile < DD->Zeilen )
					{
						diff = 0;
					}
					else
					{
						diff = ZeilenImFile - ( DD->SPos + DD->Zeilen );
					}
				}

				DD->SPos += diff;
				DD->CPos = Pos;

				if( diff )
				{
				if( diff > DD->Zeilen )
				{
					RedrawDisplay( DD );
				}
				else
				{
					MoveDisplay( diff, DD );
					RedrawPart( DD->Zeilen - diff, DD->Zeilen - 1, DD );
					SetScrollerGadget( DD );
				}
				}
			}
		}

		DD->CPos = Pos;
		CursorOn( DD );
	}

	UpdateStatusZeile( DD );
}

void __inline MoveCursorX( WORD Steps, struct DisplayData *DD )
{
	SetCursor( DD->CPos + Steps, DD );
}

void __inline MoveCursorY( WORD Steps, struct DisplayData *DD )
{
	if( DD->CPos + DD->BPR * Steps < 0 ) SetCursor( DD->CPos % DD->BPR, DD );
	else SetCursor( DD->CPos + DD->BPR * Steps, DD );
}

void __inline SetCursorX( WORD Pos, struct DisplayData *DD )
{
	SetCursor( DD->CPos / DD->BPR + Pos, DD );
}

void __inline SetCursorY( WORD Pos, struct DisplayData *DD )
{
	SetCursor( DD->CPos % DD->BPR + Pos * DD->BPR, DD );
}

void __inline SetCursorStart( struct DisplayData *DD )
{
	SetCursor( 0, DD );
}

void __inline SetCursorEnd( struct DisplayData *DD )
{
	SetCursor( DD->FD->Len - 1, DD );
}

void __inline SetCursorStartOfLine( struct DisplayData *DD )
{
	SetCursor( DD->CPos / DD->BPR * DD->BPR, DD );
}

void __inline SetCursorEndOfLine( struct DisplayData *DD )
{
	SetCursor( DD->CPos / DD->BPR * DD->BPR + DD->BPR - 1, DD );
}

void __inline SetCursorAltLeft( struct DisplayData *DD )
{
	if( DD->DisplaySpaces != 8 )
		SetCursor((( DD->CPos - 1 ) & ( ~(( 1L << DD->DisplaySpaces ) - 1 ))), DD );
	else
		SetCursorStartOfLine( DD );
}

void __inline SetCursorAltRight( struct DisplayData *DD )
{
	if( DD->DisplaySpaces != 8 )
		SetCursor(( DD->CPos & ( ~(( 1L << DD->DisplaySpaces ) - 1 ))) + ( 1L << DD->DisplaySpaces ), DD );
	else
		SetCursorEndOfLine( DD );
}

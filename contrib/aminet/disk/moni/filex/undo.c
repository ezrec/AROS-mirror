#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#endif

#include <string.h>
#include "filexstructs.h"
#include "filexstrings.h"
#include "allprotos.h"

WORD UndoLevel = DEFAULT_UNDO_LEVEL;
WORD maxundomemsize = DEFAULT_UNDO_MEM;		/* In KB */

BOOL AllocUndostructs( struct FileData *FD )
{
	if(( FD->Undos = AllocMem( sizeof( struct Undostruct ) * UndoLevel, MEMF_CLEAR )))
	{
		FD->UndoStart = 0;
		FD->UndoTiefe = 0;
		FD->Changes = FD->RedoChanges = FD->FullChanges = 0;

/*kprintf("AllocUndostructs: UndoMem:%ld\n", FD->UndoMemSize );*/

		return( TRUE );
	}
	else
	{
		MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, sizeof( struct Undostruct ) * UndoLevel );
		return( FALSE );
	}
}

void FreeUndostructs( struct FileData *FD )
{
	if( FD->Undos )
	{
		int k;

		k = FD->UndoStart;

		while( FD->Undos[ k ].typ )
		{
			if( FD->Undos[ k ].typ & ( UT_INSERT | UT_PASTE | UT_CUT ))
			{
				FreeSpeicher( &FD->Undos[ k ].r, FD );
				FreeSpeicher( &FD->Undos[ k ].u, FD );
			}
	
			FD->UndoStart++;
		
			k++;
			if( k == UndoLevel ) k -= UndoLevel;
		}

		FreeMem( FD->Undos, sizeof( struct Undostruct ) * UndoLevel );
		FD->Undos = 0;
	}

/*if( FD->UndoMemSize )*/
/*kprintf("FreeUndostructs: UndoMem:%ld\n", FD->UndoMemSize );*/

		/* Sicherheitshalber alle Variablen zurücksetzten */

	FD->UndoMemSize = 0;
	FD->UndoStart = 0;
	FD->UndoTiefe = 0;
	FD->Changes = FD->RedoChanges = FD->FullChanges = 0;
}

/*genealogist*/

BOOL Undo( struct FileData *FD )
{
	struct Undostruct *u;
	struct DisplayData *DD = 0;

/*	kprintf("Undo: Changes: %ld RChanges: %ld FChanges: %ld\n", FD->Changes, FD->RedoChanges, FD->FullChanges );*/

	if( AktuDD->FD == FD )
		DD = AktuDD;

	if( FD->UndoTiefe == FD->UndoStart )
		return( FALSE );

	FD->UndoTiefe--;

	if( FD->UndoTiefe < 0 ) FD->UndoTiefe += UndoLevel;

	if( FD->Changes != 0 ) FD->Changes--;
	else FD->RedoChanges++;

	u = &FD->Undos[FD->UndoTiefe];

	switch( u->typ )
	{
		case	UT_ASCII:
			if( DD ) SetCursor( u->pos, DD);
			FD->Mem[u->pos]=u->wert;
			AllDisplayChar( FD, FD->Undos[ FD->UndoTiefe ].pos );
			UpdateAllStatusZeilen( FD );
			break;
		case	UT_HEX_L:
			if( DD )
			{
				SetCursor( u->pos, DD);
				DD->Flags |= DD_HEXEDITPOS;
			}
			FD->Mem[u->pos]=u->wert;
			AllDisplayChar( FD, FD->Undos[ FD->UndoTiefe ].pos );
			UpdateAllStatusZeilen( FD );
			break;
		case	UT_HEX_H:
			if( DD )
			{
				SetCursor(u->pos, DD);
				DD->Flags &= ~DD_HEXEDITPOS;
			}
			FD->Mem[u->pos]=u->wert;
			AllDisplayChar( FD, FD->Undos[ FD->UndoTiefe ].pos );
			UpdateAllStatusZeilen( FD );
			break;
		case	UT_INSERT:
			if( DD ) SetCursor(u->pos, DD);
			CutBlockForce(u->pos,u->pos+u->r.len-1, (struct DisplayData *)(((UBYTE *)FD->DisplayList.lh_Head)-sizeof(struct Node)));
			break;
		case	UT_CUT:
			InsertBlockForce(u->pos,u->u.mem, u->u.len, (struct DisplayData *)(((UBYTE *)FD->DisplayList.lh_Head)-sizeof(struct Node)) );
			if( DD ) SetCursor(u->pos, DD);
			break;
		case	UT_PASTE:
			memcpy(FD->Mem+u->pos,u->u.mem,u->u.len);
			AllDisplay( FD, SCROLLERNEU );
			UpdateAllStatusZeilen( FD );
			if( DD ) SetCursor(u->pos, DD);
			break;
	}

	return( TRUE );
}

BOOL Redo( struct FileData *FD )
{
	struct Undostruct *u;
	struct DisplayData *DD = 0;

/*	kprintf("Redo: Changes: %ld RChanges: %ld FChanges: %ld\n", FD->Changes, FD->RedoChanges, FD->FullChanges );*/

	if( AktuDD->FD == FD )
		DD = AktuDD;

	u = &FD->Undos[FD->UndoTiefe];

	if((!u->typ)||(FD->UndoTiefe==FD->UndoStart-1))
		return( FALSE );

	if(FD->RedoChanges!=0)FD->RedoChanges--;
	else	FD->Changes++;

	switch(u->typ)
	{
		case	UT_ASCII:
			if( DD ) SetCursor(u->pos, DD);
			FD->Mem[u->pos]=u->neuwert;
			AllDisplayChar( FD, FD->Undos[ FD->UndoTiefe ].pos );
			UpdateAllStatusZeilen( FD );
			if( DD ) SetCursor(u->pos+1, DD);
			break;

		case	UT_HEX_L:
			if( DD )
			{
				DD->Flags |= DD_HEXEDITPOS;
				SetCursor(u->pos, DD);
			}
			FD->Mem[u->pos]=u->wert;
			AllDisplayChar( FD, FD->Undos[ FD->UndoTiefe ].pos );
			UpdateAllStatusZeilen( FD );
			if( DD ) SetCursor(u->pos+1, DD);
			break;

		case	UT_HEX_H:
			if( DD )
			{
				DD->Flags &= ~DD_HEXEDITPOS;
				SetCursor(u->pos, DD);
			}
			FD->Mem[u->pos]=u->wert;
			AllDisplayChar( FD, FD->Undos[ FD->UndoTiefe ].pos );
			UpdateAllStatusZeilen( FD );
			break;

		case	UT_INSERT:
			InsertBlockForce(u->pos,u->r.mem,u->r.len, (struct DisplayData *)(((UBYTE *)FD->DisplayList.lh_Head)-sizeof(struct Node)) );
			if( DD ) SetCursor(u->pos, DD);
			break;

		case	UT_CUT:
			if( DD ) SetCursor(u->pos, DD);
			CutBlockForce(u->pos,u->pos+u->u.len-1, (struct DisplayData *)(((UBYTE *)FD->DisplayList.lh_Head)-sizeof(struct Node)));
			break;

		case	UT_PASTE:
			if( DD ) SetCursor(u->pos, DD);
			memcpy(FD->Mem+u->pos,u->r.mem,u->r.len);
			AllDisplay( FD, SCROLLERNEU );
			UpdateAllStatusZeilen( FD );
			break;
	}

	FD->UndoTiefe++;
	if(FD->UndoTiefe==UndoLevel)FD->UndoTiefe=0;

	return( TRUE );
}

void FreeUndo( struct Undostruct *u, struct FileData *FD )
{
		/* Eintrag löschen */

	if( u->typ & ( UT_INSERT | UT_PASTE | UT_CUT ))
	{
			/* Nun Speicher freigeben */

		FreeSpeicher( &u->r, FD );
		FreeSpeicher( &u->u, FD );
	}

	u->typ = 0;
}

static void AddUndo( struct FileData *FD )
{
	FD->UndoTiefe++;

/*	kprintf("undo steht auf: %ld Redo:%ld\n", FD->Undos[ FD->UndoTiefe ].typ, FD->RedoChanges );*/

	if(FD->UndoTiefe==UndoLevel)
		FD->UndoTiefe=0;

	if( FD->UndoTiefe == FD->UndoStart )
	{
		FreeUndo( &FD->Undos[ FD->UndoStart ], FD );

		FD->UndoStart++;
		if( FD->UndoStart == UndoLevel )
			FD->UndoStart=0;
	}

	FD->Changes++;

	if( FD->RedoChanges != 0 )
	{
		FD->FullChanges = FD->RedoChanges;
		FD->RedoChanges = 0;
	}
}

static void FreeOldUndo( struct FileData *FD )
{
	long k = FD->UndoTiefe;

	while( FD->Undos[ k ].typ )
	{
/*		kprintf("Undotyp[ %ld ] = %ld\n", k, FD->Undos[ k ].typ );*/
		FreeUndo( &FD->Undos[ k ], FD );
		k++;

		if( k == UndoLevel )
			k = 0;
	}
/*	kprintf("**********************\n");*/
}


void AddUndoChar(long pos,UBYTE wert,UBYTE neuwert, struct FileData *FD )
{
	FreeOldUndo( FD );

	FD->Undos[FD->UndoTiefe].pos=pos;
	FD->Undos[FD->UndoTiefe].wert=wert;
	FD->Undos[FD->UndoTiefe].neuwert=neuwert;
	FD->Undos[FD->UndoTiefe].typ=UT_ASCII;

	AddUndo( FD );
}

void AddUndoCharHex(long pos,UBYTE wert,UBYTE neuwert,BOOL low, struct FileData *FD)
{
	FreeOldUndo( FD );

	FD->Undos[FD->UndoTiefe].pos=pos;
	FD->Undos[FD->UndoTiefe].wert=wert;
	FD->Undos[FD->UndoTiefe].neuwert=neuwert;
	FD->Undos[FD->UndoTiefe].typ=(low==TRUE)?UT_HEX_L:UT_HEX_H;

	AddUndo( FD );
}

void AddUndoInsert(long pos,struct Speicher s, struct FileData *FD)
{
	FreeOldUndo( FD );

	FD->Undos[FD->UndoTiefe].pos=pos;
	FD->Undos[FD->UndoTiefe].typ=UT_INSERT;

	FD->Undos[FD->UndoTiefe].r = s;
	FD->Undos[FD->UndoTiefe].u.mem = 0;

	AddUndo( FD );

	FD->UndoMemSize += s.len;
/*kprintf("AddUndo: UndoMem:%ld\n", FD->UndoMemSize );*/
	PruefeUndomemsize( FD );
}

void AddUndoCut(long pos,struct Speicher s, struct FileData *FD)
{
	FreeOldUndo( FD );

	FD->Undos[FD->UndoTiefe].pos=pos;
	FD->Undos[FD->UndoTiefe].typ=UT_CUT;

	FD->Undos[FD->UndoTiefe].r.mem = 0;
	FD->Undos[FD->UndoTiefe].u = s;

	AddUndo( FD );

	FD->UndoMemSize += s.len;
/*kprintf("AddUndo: UndoMem:%ld\n", FD->UndoMemSize );*/
	PruefeUndomemsize( FD );
}

void AddUndoPaste(long pos,struct Speicher ps,struct Speicher s, struct FileData *FD)
{
	FreeOldUndo( FD );

	FD->Undos[FD->UndoTiefe].pos = pos;
	FD->Undos[FD->UndoTiefe].typ = UT_PASTE;

	FD->Undos[FD->UndoTiefe].r = s;
	FD->Undos[FD->UndoTiefe].u = ps;

	AddUndo( FD );

	FD->UndoMemSize += s.len + ps.len;
/*kprintf("AddUndo: UndoMem:%ld\n", FD->UndoMemSize );*/
	PruefeUndomemsize( FD );
}

void PruefeUndomemsizeRedo( struct FileData *FD )
{
	while( FD->UndoMemSize > maxundomemsize * 1024 )
	{
		long k = FD->UndoStart;

			/* Falls kein Eintrag mehr, abbrechen und
			 * wundern! Sollte nicht passieren! */

		if( !FD->Undos[ k ].typ )
			break;

			/* Letzes Element suchen */

		while( FD->Undos[ k ].typ )
		{
			k++;
			if( k == UndoLevel )
				k = 0;
		}

		k--;

		if( k < 0 )
			k += UndoLevel;

			/* k enthält die Nummer des Letzten Eintrags */

		FreeUndo( &FD->Undos[ k ], FD );
	}
}

void PruefeUndomemsize( struct FileData *FD )
{
	while( FD->UndoMemSize > maxundomemsize * 1024 )
	{
			/* Falls kein Eintrag mehr in Liste, abbrechen */

		if( !FD->Undos[ FD->UndoStart ].typ )
			break;

				/* Falls wir an aktueller Stelle ankommen, von hinten freigeben */

		if( FD->UndoStart == FD->UndoTiefe )
		{
			PruefeUndomemsizeRedo( FD );
			break;
		}

			/* Undoeintrag löschen */

		FreeUndo( &FD->Undos[ FD->UndoStart ], FD );

		FD->UndoStart++;

		if( FD->UndoStart == UndoLevel )
			FD->UndoStart = 0;
	}
}

void FreeSpeicher( struct Speicher *s, struct FileData *FD )
{
	if( s->len && s->mem )
	{
		FreeMem( s->mem, s->len );
		FD->UndoMemSize -= s->len;
/*kprintf("FreeSpeicher: UndoMem:%ld\n", FD->UndoMemSize );*/
	}
}

void SetMaxUndoMemSize(long new)
{
	if( maxundomemsize != new )
	{
		struct FileData *FD;

		maxundomemsize = new;

		FD = (struct FileData *)FileList.lh_Head;

		while( FD != ( struct FileData * )&FileList.lh_Tail )
		{
			PruefeUndomemsize( FD );

			FD = ( struct FileData *)FD->Node.ln_Succ;
		}
	}
}

/* SetUndoLevel
 *
 * Setzt die maximale Anzahl der Undoschritte in jedem File auf
 * einen neuen Wert. Redo wird dabei abgeschnitten.
 */

void SetUndoLevel( long neu )
{
	struct Undostruct *neuundos;
	struct FileData *FD;

	FD = (struct FileData *)FileList.lh_Head;

	if( neu != UndoLevel )
	while( FD != ( struct FileData * )&FileList.lh_Tail )
	{
		if( !( neuundos = AllocMem( sizeof( struct Undostruct ) * neu, MEMF_CLEAR | MEMF_ANY )))
		{
			MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, sizeof( struct Undostruct ) * MAX_UNDO_LEVEL );
			return;
		}

		if( FD->UndoStart != FD->UndoTiefe )
		{
			long undolaenge;

			if( FD->UndoStart > FD->UndoTiefe )
			{
				undolaenge = UndoLevel + FD->UndoTiefe - FD->UndoStart;
			}
			else
			{
				undolaenge = FD->UndoTiefe - FD->UndoStart;
			}

			if( neu >= undolaenge )
			{
				if( FD->UndoStart > FD->UndoTiefe )
				{
					memcpy( neuundos, FD->Undos + FD->UndoStart,(UndoLevel-FD->UndoStart)*sizeof(struct Undostruct));
					memcpy( neuundos + ( UndoLevel - FD->UndoStart ),FD->Undos,FD->UndoTiefe*sizeof(struct Undostruct));
				}
				else
					memcpy(neuundos,FD->Undos+FD->UndoStart,undolaenge*sizeof(struct Undostruct));

				FD->UndoTiefe=undolaenge;
				FD->UndoStart=0;
			}
			else
			{
				long k;

				for( k = 0; k < undolaenge - neu + 1; k++ )
				{
					FreeUndo( &FD->Undos[ FD->UndoStart ], FD );

					FD->UndoStart++;

					if( FD->UndoStart == UndoLevel )
						FD->UndoStart = 0;
				}

				if( FD->UndoStart > FD->UndoTiefe)
				{
					memcpy(neuundos,FD->Undos+FD->UndoStart,(UndoLevel-FD->UndoStart)*sizeof(struct Undostruct));
					memcpy(neuundos+(UndoLevel-FD->UndoStart),FD->Undos,FD->UndoTiefe*sizeof(struct Undostruct));
				}
				else
					memcpy(neuundos,FD->Undos+FD->UndoStart,neu*sizeof(struct Undostruct));

				FD->UndoTiefe=neu-1;
				FD->UndoStart=0;
			}
		}
	
		FreeMem( FD->Undos, UndoLevel * sizeof( struct Undostruct ));

		FD->Undos = neuundos;

		FD = ( struct FileData *)FD->Node.ln_Succ;
	}

	UndoLevel = neu;
}

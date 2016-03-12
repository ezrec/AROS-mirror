
/*-------------------------------------------------*/
/* fireworks.c - graphical MIDI note visualisation */
/*          ® 1998 by Christian Buchner            */
/*-------------------------------------------------*/

#include "fireworks.h"
#include "fireworks_protos.h"

#include "linear_protos.h"

extern UBYTE randomarray33[256];

/*-----------------------------------*/
/* Linear fireworks (straight lines) */
/*-----------------------------------*/

struct LinData
{
	struct Globals *glob;
	struct Prefs *pref;
	
	ULONG curtime;
	
	WORD centx, centy;
	WORD divx, divy;
	
	struct MinList CrackerList;
	
	WORD fxtab[128], fytab[128];
};

#define SCALE 1024

/* the firecrackers */

struct LinearCracker
{
	struct Node		cr_node;
	UBYTE			cr_chn;			/* 0 - 15  */
	UBYTE			cr_note;		/* 0 - 127 */
	UBYTE			cr_vel;			/* 0 - 127 */
	
	LONG			cr_starttime;		/* time stamp of note on */
	LONG			cr_stoptime;		/* time stamp of note off */
	UWORD			cr_savedlifetime;	/* lifetime when ray hit border */
	
	WORD			cr_fx;			/* -SCALE to SCALE equals -1 to -1 */
	WORD			cr_fy;			/* -SCALE to SCALE equals -1 to -1 */
	
	WORD			cr_sparkx;		/* for painting sparks */
	WORD			cr_sparky;
};

APTR Lin_InitFireworks(struct Globals *glob, struct Prefs *pref)
{
	struct LinData *ld = NULL;
	UWORD i;
	
	if ((ld = AllocVec(sizeof(struct LinData), MEMF_ANY|MEMF_CLEAR)))
	{
		ld->glob = glob;
		ld->pref = pref;
		
		NewList((struct List*)&ld->CrackerList);
		
		/* pre-calculate angles of all possible 128 notes */
		for (i = 0 ; i < 128 ; i++)
		{
			float angle = M_PI - (float)i/127 * M_PI;
			
			ld->fxtab[i] = SCALE * cos(angle);
			ld->fytab[i] = SCALE * sin(angle);
		}
		
		Lin_RethinkWindow((APTR)ld);
		
		GetTimeDelta();			/* initialize delta counter */
		ld->curtime = 0;		/* start at time counter 0 */
	}
	
	return((APTR)ld);
}

void Lin_RethinkWindow(APTR data)
{
	struct LinData *ld = (struct LinData*) data;
	struct Globals *glob = ld->glob;
	
	/* Calculate ray scaling factors */
	ld->divx  = SCALE * MAXCOORD / (glob->ww);
	ld->divy  = SCALE * MAXCOORD / (glob->wh);
	ld->centx = CENTER_X * (glob->ww) / MAXCOORD;
	ld->centy = CENTER_Y * (glob->wh) / MAXCOORD;
}

void Lin_TimePassed(APTR data)
{
	struct LinData *ld = (struct LinData*) data;
	
	ld->curtime += GetTimeDelta();
}

BOOL Lin_IsIdle(APTR data)
{
	struct LinData *ld = (struct LinData*) data;
	
	if (IsListEmpty(&ld->CrackerList))
		return(TRUE);
	else
		return(FALSE);
}

void Lin_DrawFireworks(APTR data, UWORD Mask)
{
	struct LinData *ld = (struct LinData*) data;
	struct LinearCracker *cr, *ncr;
	struct Globals *glob = ld->glob;
	struct Prefs *pref = ld->pref;
	
	UWORD lifetime;
	UWORD deathtime;
	WORD sx1, sy1;
	WORD sx2, sy2;
	LONG lastpen = -1, newpen;
	
	struct Layer *SaveLayer = glob->PaintRP.Layer;
		
	for ( cr = (struct LinearCracker*) ld->CrackerList.mlh_Head ;
		  (ncr = (struct LinearCracker*) cr->cr_node.ln_Succ );
		  cr = ncr )
	{
		/* get the ray's lifetime information */
		
		if (cr->cr_savedlifetime == 0xffff)
			if ((ld->curtime - cr->cr_starttime) < 0x10000)
				lifetime = (UWORD)(ld->curtime - cr->cr_starttime);
			else
				lifetime = 0xffff;
		else
			lifetime = cr->cr_savedlifetime;
		
		/* calculate ray starting points */
		sx1 = ld->centx + cr->cr_fx *  lifetime / ld->divx;
		sy1 = ld->centy - cr->cr_fy *  lifetime / ld->divy;
		
		if (cr->cr_savedlifetime == 0xffff)
		{
			if ((sx1 >= glob->ww) || (sy1 >= glob->wh) || (sx1 < 0) || (sy1 < 0))
			{
				/* Start of ray hit window borders */
				/* save the time this happened to prevent */
				/* the ray from growing longer than */
				/* graphic's clipping can handle safely */
				cr->cr_savedlifetime = lifetime;
			}
		}
		
		/* has ray already been stopped by a note off? */
		if (cr->cr_stoptime == -1)
		{
			/* no, still emmitting from the centre */
			sx2 = ld->centx;
			sy2 = ld->centy;
			deathtime = 0;
		}
		else
		{
			/* get the time when the note off occured */
			if ((ld->curtime - cr->cr_stoptime) < 0x10000)
				deathtime = (UWORD)(ld->curtime - cr->cr_stoptime);
			else
				deathtime = 0xffff;
			
			/* is the ray just a dot? */
			if (lifetime == deathtime)
			{
				sx2 = sx1;
				sy2 = sy1;
			}
			else
			{
				/* calculate ray ending points */
				sy2 = ld->centy - cr->cr_fy * deathtime / ld->divy;
				sx2 = ld->centx + cr->cr_fx * deathtime / ld->divx;
			}
		}
		
		/* see if the ray has disappeared or exceeded */
		/* its maximum time since the note was released */
		if ((sx2 >= glob->ww) || (sy2 >= glob->wh) || (sx2 < 0) || (sy2 < 0) ||
			deathtime  == 0xffff )
		{
			Remove((struct Node*)cr);
			FreePooled(glob->NotePool,cr,sizeof(struct LinearCracker));
		}
		else
		{
			/* display this channel ? */
			if ((1<<cr->cr_chn) & Mask)
			{
				/* change pen if necessary */
				newpen = glob->PenArray[Channelpens+cr->cr_chn];
				if (lastpen != newpen)
				{
					SetAPen(glob->Window->RPort, newpen);
					lastpen = newpen;
				}
				
				/* draw the ray */
				Move(glob->Window->RPort, sx1, sy1);
				Draw(glob->Window->RPort, sx2, sy2);
				
				if (pref->Flags & PREFF_DOUBLE)
				{
					WORD dx, dy;
					dx = sx2 - sx1; if (dx<0) dx=-dx;
					dy = sy2 - sy1; if (dy<0) dy=-dy;
					
					if (dy >= dx)
					{
						Move(glob->Window->RPort, sx1+1, sy1);
						Draw(glob->Window->RPort, sx2+1, sy2);
					}
					else
					{
						Move(glob->Window->RPort, sx1, sy1+1);
						Draw(glob->Window->RPort, sx2, sy2+1);
					}
				}
				if (pref->Flags & PREFF_SPARKS)
				{
					cr->cr_sparkx = sx1;
					cr->cr_sparky = sy1;
				}
			}
		}
	}
	
	if (pref->Flags & PREFF_SPARKS)
	{
		SetAPen(glob->Window->RPort, glob->PenArray[Sparkpen]);
		
		for ( cr = (struct LinearCracker*) ld->CrackerList.mlh_Head ;
			  (ncr = (struct LinearCracker*) cr->cr_node.ln_Succ );
			  cr = ncr )
		{
			/* display this channel ? */
			if ((1<<cr->cr_chn) & Mask)
			{
				if (cr->cr_savedlifetime == 0xffff)
				{
					UWORD margin = (pref->Flags & PREFF_DOUBLE) ? 2 : 1;
					UBYTE rnd = (ld->curtime>>6) + cr->cr_starttime + cr->cr_note;
					
					if (randomarray33[rnd])
					{
						sx1 = cr->cr_sparkx;
						sy1 = cr->cr_sparky;
						
						if ((sx1 >= margin) && (sy1 >= margin) &&
							(sx1 < (glob->ww - margin)) &&
							(sy1 < (glob->wh - margin)) )
							glob->PaintRP.Layer = NULL;
						else
							glob->PaintRP.Layer = SaveLayer;
						
						RectFill(glob->Window->RPort, sx1-margin, sy1-margin, sx1+margin, sy1+margin);
					}
				}
			}
		}
	}
	
	glob->PaintRP.Layer = SaveLayer;
}

void Lin_NoteOn(APTR data, UBYTE chn, UBYTE note, UBYTE vel, LONG reltime)
{
	struct LinData *ld = (struct LinData*) data;
	struct Globals *glob = ld->glob;
	struct Prefs *pref = ld->pref;
	struct LinearCracker *cr;
	
	if ((cr = AllocPooled(glob->NotePool, sizeof(struct LinearCracker))))
	{
		cr->cr_node.ln_Pri = chn;
		
		cr->cr_chn  = chn;
		cr->cr_note = note;
		cr->cr_vel  = vel;
		
		cr->cr_starttime = ld->curtime - reltime;
		cr->cr_stoptime  = -1;
		cr->cr_savedlifetime = 0xffff;
		
		cr->cr_fx = ld->fxtab[note] * cr->cr_vel / 127 * pref->Sensitivity / 100;
		cr->cr_fy = ld->fytab[note] * cr->cr_vel / 127 * pref->Sensitivity / 100;
		
		/* sorted by channel num to speed up painting */
		Enqueue((struct List*)&ld->CrackerList, (struct Node*)cr);
	}
}

void Lin_NoteOff(APTR data, UBYTE chn, UBYTE note, LONG reltime)
{
	struct LinData *ld = (struct LinData*) data;
	struct LinearCracker *cr, *ncr;
	
	for ( cr = (struct LinearCracker*) ld->CrackerList.mlh_Head ;
		  (ncr = (struct LinearCracker*) cr->cr_node.ln_Succ );
		  cr = ncr )
	{
		if ( (cr->cr_chn  == chn ) &&
			 (cr->cr_note == note) &&
			 (cr->cr_stoptime == -1) )
		{
			cr->cr_stoptime = ld->curtime - reltime;
			break;
		}
	}
}

void Lin_ReleaseNotes(APTR data, LONG reltime)
{
	struct LinData *ld = (struct LinData*) data;
	struct LinearCracker *cr, *ncr;
	
	for ( cr = (struct LinearCracker*) ld->CrackerList.mlh_Head ;
		 ( ncr = (struct LinearCracker*) cr->cr_node.ln_Succ );
		  cr = ncr )
	{
		if (cr->cr_stoptime == -1) cr->cr_stoptime = ld->curtime - reltime;
	}
}

void Lin_FreeNoteData(APTR data)
{
	struct LinData *ld = (struct LinData*) data;
	struct Globals *glob = ld->glob;
	struct LinearCracker *cr, *ncr;
	
	for ( cr = (struct LinearCracker*) ld->CrackerList.mlh_Head ;
		  (ncr = (struct LinearCracker*) cr->cr_node.ln_Succ) ;
		  cr = ncr )
	{
		Remove((struct Node*)cr);
		FreePooled(glob->NotePool,cr,sizeof(struct LinearCracker));
	}
}

void Lin_ExitFireworks(APTR data)
{
	struct LinData *ld = (struct LinData*) data;
	
	if (ld)
	{
		Lin_FreeNoteData(data);
		
		FreeVec(ld);
	}
}

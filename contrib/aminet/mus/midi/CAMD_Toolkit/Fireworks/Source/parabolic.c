
/*-------------------------------------------------*/
/* fireworks.c - graphical MIDI note visualisation */
/*          ® 1998 by Christian Buchner            */
/*-------------------------------------------------*/

#include "fireworks.h"
#include "fireworks_protos.h"
#include "fwmodes.h"

#include "parabolic_protos.h"

extern UBYTE randomarray33[256];
extern CHIP UWORD WaitPointer[];

/*------------------------------------------*/
/* Parabolic fireworks (gravity simulation) */
/*------------------------------------------*/

#define K 10
#define MAXLIFETIME (K*MAXCOORD)

struct ParData
{
	struct Globals *glob;
	struct Prefs *pref;
	
	ULONG curtime;
	
	WORD centx, centy;
	WORD divx, divy;
	UWORD winscale;
	
	struct MinList CrackerList;
	
	WORD fxtab[128], fytab[128];
	WORD  parabtable[MAXLIFETIME];	/* parabolic flight path */
	UWORD parabskip[MAXLIFETIME];	/* info for painting flight paths */
};

#define SCALE 1024

/* the firecrackers */

struct ParabolicCracker
{
	struct Node		cr_node;
	UBYTE			cr_chn;			/* 0 - 15  */
	UBYTE			cr_note;		/* 0 - 127 */
	UBYTE			cr_vel;			/* 0 - 127 */
	
	LONG			cr_starttime;		/* time stamp of note on */
	LONG			cr_stoptime;		/* time stamp of note off */
	
	WORD			cr_fx;			/* -SCALE to SCALE equals -1 to -1 */
	WORD			cr_fy;			/* -SCALE to SCALE equals -1 to -1 */
	
	UWORD			cr_offset;		/* offset to lifetime */
	WORD			cr_coordoffset;	/* offset to coordinate */
	
	UWORD			cr_multiplier;	/* time skip multiplier (0 to SCALE) */
	
	WORD			cr_sparkx;		/* for painting sparks */
	WORD			cr_sparky;
};

APTR Par_InitFireworks(struct Globals *glob, struct Prefs *pref)
{
	struct ParData *pd = NULL;
	UWORD i;
	UWORD lt;
	WORD val;
	
	if ((pd = AllocVec(sizeof(struct ParData), MEMF_ANY|MEMF_CLEAR)))
	{
		pd->glob = glob;
		pd->pref = pref;
		
		NewList((struct List*)&pd->CrackerList);
		
		BusyPointer(glob, pref);
		
		/* pre-calculate angles of all possible 128 notes */
		for (i = 0 ; i < 128 ; i++)
		{
			float angle = M_PI - (float)i/127 * M_PI;
			
			pd->fxtab[i] = SCALE * cos(angle);
			pd->fytab[i] = SCALE * sin(angle);
		}
		
		val = -(MAXLIFETIME/2);
		
		for (lt=0 ; lt <= MAXLIFETIME/2 ; lt++, val++)
		{
			pd->parabtable[lt] = pd->parabtable[MAXLIFETIME-1-lt] =
				(LONG)(val * val) / ((MAXLIFETIME*MAXLIFETIME/4) / (K * MAXCOORD / 4));
		}
		
		if (pref->FWMode == ParabolicMode2)
		{
			val = -(MAXLIFETIME/2);
			
			for (lt=0 ; lt <= MAXLIFETIME/2 ; )
			{
				UWORD s = (64.0 * pow(1+4*(float)(val*val)/(MAXLIFETIME*MAXLIFETIME/4),1.5));
				
				for (i=0; i<16; i++)
				{
					pd->parabskip[lt] = pd->parabskip[MAXLIFETIME-1-lt] = s;
					lt++; val++;
				}
			}
		}
		
		NormalPointer(glob, pref);
		
		Par_RethinkWindow((APTR)pd);
		
		GetTimeDelta();			/* initialize delta counter */
		pd->curtime = 0;		/* start at time counter 0 */
	}
	
	return((APTR)pd);
}

void Par_RethinkWindow(APTR data)
{
	struct ParData *pd = (struct ParData*) data;
	struct Globals *glob = pd->glob;
	
	/* Calculate ray scaling factors */
	pd->divx  = SCALE * MAXCOORD / (glob->ww);
	pd->divy  = SCALE * MAXCOORD / (glob->wh);
	pd->centx = CENTER_X * (glob->ww) / MAXCOORD;
	pd->centy = CENTER_Y * (glob->wh) / MAXCOORD;
	
	/* for min...max window size range from (3*SCALE)...SCALE */
	pd->winscale = SCALE * (3 * MAXCOORD) / (glob->ww + glob->wh + MAXCOORD);
}

void Par_TimePassed(APTR data)
{
	struct ParData *pd = (struct ParData*) data;
	
	pd->curtime += GetTimeDelta();
}

BOOL Par_IsIdle(APTR data)
{
	struct ParData *pd = (struct ParData*) data;
	
	if (IsListEmpty(&pd->CrackerList))
		return(TRUE);
	else
		return(FALSE);
}

void Par_DrawFireworks(APTR data, UWORD Mask)
{
	struct ParData *pd = (struct ParData*) data;
	struct ParabolicCracker *cr, *ncr;
	struct Globals *glob = pd->glob;
	struct Prefs *pref = pd->pref;
	
	UWORD lifetime;
	UWORD lt;
	WORD sx1, sy1;
	LONG lastpen = -1, newpen;
	UWORD margin = (pref->Flags & PREFF_DOUBLE) ? 1 : 0;
	
	struct Layer *SaveLayer = glob->PaintRP.Layer;
	glob->PaintRP.Layer = NULL;		/* we do our own clipping here */
	
	for ( cr = (struct ParabolicCracker*) pd->CrackerList.mlh_Head ;
		  (ncr = (struct ParabolicCracker*) cr->cr_node.ln_Succ );
		  cr = ncr )
	{
		/* get the pixel's lifetime information */
		
		if ((pd->curtime - cr->cr_starttime) < MAXLIFETIME)
			lifetime = (UWORD)(pd->curtime - cr->cr_starttime);
		else
			lifetime = MAXLIFETIME-1;
		
		lt = lifetime + cr->cr_offset;
		if (lt >= MAXLIFETIME)
		{
			lt = MAXLIFETIME-1;
			lifetime = lt - cr->cr_offset;
		}
		
		/* calculate pixel coordinate */
		sx1 = pd->centx + cr->cr_fx * lifetime                  / pd->divx;
		sy1 = pd->centy
			+ (pd->parabtable[lt] - cr->cr_coordoffset) * SCALE / pd->divy;
		
		/* see if the pixel has disappeared */
		/* or exceeded its maximum lifetime */
		if ( (sx1 >= (glob->ww-margin)) || (sy1 >= (glob->wh-margin)) ||
			 (sx1 < 0) || lt >= (MAXLIFETIME-1) )
		{
			Remove((struct Node*)cr);
			FreePooled(glob->NotePool,cr,sizeof(struct ParabolicCracker));
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
				
				if (sy1 >= 0)
				{
					/* draw the pixel */
					WritePixel(glob->Window->RPort, sx1, sy1);
					if (pref->Flags & PREFF_DOUBLE)
					{
						WritePixel(glob->Window->RPort, sx1+1, sy1  );
						WritePixel(glob->Window->RPort, sx1  , sy1+1);
						WritePixel(glob->Window->RPort, sx1+1, sy1+1);
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
		
		for ( cr = (struct ParabolicCracker*) pd->CrackerList.mlh_Head ;
			  (ncr = (struct ParabolicCracker*) cr->cr_node.ln_Succ) ;
			  cr = ncr )
		{
			/* display this channel ? */
			if ((1<<cr->cr_chn) & Mask)
			{
				UWORD margin = (pref->Flags & PREFF_DOUBLE) ? 2 : 1;
				UBYTE rnd = (pd->curtime>>6) + cr->cr_starttime + cr->cr_note;
				
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
	
	glob->PaintRP.Layer = SaveLayer;
}

void Par_NoteOn(APTR data, UBYTE chn, UBYTE note, UBYTE vel, LONG reltime)
{
	struct ParData *pd = (struct ParData*) data;
	struct Globals *glob = pd->glob;
	struct Prefs *pref = pd->pref;
	struct ParabolicCracker *cr;
	WORD absfx;

	if ((cr = AllocPooled(glob->NotePool, sizeof(struct ParabolicCracker))))
	{
		cr->cr_node.ln_Pri = chn;
		
		cr->cr_chn  = chn;
		cr->cr_note = note;
		cr->cr_vel  = vel;
		
		cr->cr_starttime = pd->curtime - reltime;
		cr->cr_stoptime  = -1;
		cr->cr_fx = pd->fxtab[note] * cr->cr_vel / 127 * pref->Sensitivity / 100;
		cr->cr_fy = pd->fytab[note] * cr->cr_vel / 127 * pref->Sensitivity / 100;
		
		cr->cr_offset      = (MAXLIFETIME/2) * (SCALE-cr->cr_fy) / SCALE;
		cr->cr_coordoffset = pd->parabtable[cr->cr_offset];
		
		if (pref->FWMode == ParabolicMode2)
		{
			absfx = cr->cr_fx;
			if (absfx < 0) absfx = -absfx;
			
			/* for absfx=0...1 -> range from (2*SCALE)...(SCALE/2) */
			cr->cr_multiplier = (SCALE*SCALE)/((UWORD)((SCALE/2)+3*absfx/2));
		}
		
		/* sorted by channel num to speed up painting */
		Enqueue((struct List*)&pd->CrackerList, (struct Node*)cr);
	}
}

void Par_NoteOff(APTR data, UBYTE chn, UBYTE note, LONG reltime)
{
	struct ParData *pd = (struct ParData*) data;
	struct ParabolicCracker *cr, *ncr;
	
	for ( cr = (struct ParabolicCracker*) pd->CrackerList.mlh_Head ;
		  (ncr = (struct ParabolicCracker*) cr->cr_node.ln_Succ) ;
		  cr = ncr )
	{
		if ( (cr->cr_chn  == chn ) &&
			 (cr->cr_note == note) &&
			 (cr->cr_stoptime == -1) )
		{
			cr->cr_stoptime = pd->curtime - reltime;
			break;
		}
	}
}

void Par_ReleaseNotes(APTR data, LONG reltime)
{
	struct ParData *pd = (struct ParData*) data;
	struct ParabolicCracker *cr, *ncr;
	
	for ( cr = (struct ParabolicCracker*) pd->CrackerList.mlh_Head ;
		  (ncr = (struct ParabolicCracker*) cr->cr_node.ln_Succ) ;
		  cr = ncr )
	{
		if (cr->cr_stoptime == -1) cr->cr_stoptime = pd->curtime - reltime;
	}
}

void Par_FreeNoteData(APTR data)
{
	struct ParData *pd = (struct ParData*) data;
	struct Globals *glob = pd->glob;
	struct ParabolicCracker *cr, *ncr;
	
	for ( cr = (struct ParabolicCracker*) pd->CrackerList.mlh_Head ;
		  (ncr = (struct ParabolicCracker*) cr->cr_node.ln_Succ) ;
		  cr = ncr )
	{
		Remove((struct Node*)cr);
		FreePooled(glob->NotePool,cr,sizeof(struct ParabolicCracker));
	}
}

void Par_ExitFireworks(APTR data)
{
	struct ParData *pd = (struct ParData*) data;
	
	if (pd)
	{
		Par_FreeNoteData(data);
		
		FreeVec(pd);
	}
}



/*-------------------------------------*/
/* Parabolic rays (gravity simulation) */
/*-------------------------------------*/

void Par_DrawFireworks2(APTR data, UWORD Mask)
{
	struct ParData *pd = (struct ParData*) data;
	struct ParabolicCracker *cr, *ncr;
	struct Globals *glob = pd->glob;
	struct Prefs *pref = pd->pref;
	
	UWORD lifetime;
	UWORD deathtime;
	UWORD lt, dt, t;
	WORD sx1, sy1;
	WORD sx2, sy2;
	WORD dx, dy;
	LONG lastpen = -1, newpen;
	UWORD margin = (pref->Flags & PREFF_DOUBLE) ? 1 : 0;
	
	struct Layer *SaveLayer = glob->PaintRP.Layer;
	glob->PaintRP.Layer = NULL;	/* most of the time we do our own clipping */

	for ( cr = (struct ParabolicCracker*) pd->CrackerList.mlh_Head ;
		  (ncr = (struct ParabolicCracker*) cr->cr_node.ln_Succ) ;
		  cr = ncr )
	{
		/* get the parabolic ray's lifetime information */
		
		if ((pd->curtime - cr->cr_starttime) < MAXLIFETIME)
			lifetime = (UWORD)(pd->curtime - cr->cr_starttime);
		else
			lifetime = MAXLIFETIME-1;
		
		lt = lifetime + cr->cr_offset;
		if (lt >= MAXLIFETIME)
		{
			lt = MAXLIFETIME-1;
			lifetime = lt - cr->cr_offset;
		}
		
		/* has ray already been stopped by a note off? */
		if (cr->cr_stoptime == -1)
		{
			deathtime = 0;
		}
		else
		{
			if ((pd->curtime - cr->cr_stoptime) < MAXLIFETIME)
				deathtime = (UWORD)(pd->curtime - cr->cr_stoptime);
			else
				deathtime = MAXLIFETIME-1;
		}
		
		dt = deathtime + cr->cr_offset;
		if (dt >= MAXLIFETIME)
		{
			dt = MAXLIFETIME-1;
			deathtime = dt - cr->cr_offset;
		}
		
		/* calculate parabolic ray coordinate */
		sx1 = pd->centx + cr->cr_fx * deathtime                 / pd->divx;
		sy1 = pd->centy
			+ (pd->parabtable[dt] - cr->cr_coordoffset) * SCALE / pd->divy;
		
		/* see if the parabolic ray has disappeared */
		/* or exceeded its maximum lifetime */
		if ( (sx1 >= (glob->ww-margin)) || (sy1 >= (glob->wh-margin)) ||
			 (sx1 < 0) || dt >= (MAXLIFETIME-1) )
		{
			Remove((struct Node*)cr);
			FreePooled(glob->NotePool,cr,sizeof(struct ParabolicCracker));
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
				
				if (lifetime == deathtime)
				{
					if (sy1 >= 0)
					{
						/* draw the pixel */
						WritePixel(glob->Window->RPort, sx1, sy1);
						if (pref->Flags & PREFF_DOUBLE)
						{
							WritePixel(glob->Window->RPort, sx1+1, sy1  );
							WritePixel(glob->Window->RPort, sx1  , sy1+1);
							WritePixel(glob->Window->RPort, sx1+1, sy1+1);
						}
					}
				}
				else
				{
					BOOL Invisible = FALSE;
					
					BOOL Clean = (sy1 >= 0) ? TRUE : FALSE;
					
					/* a wide flightpath and a large window will result */
					/* in the most detailed plot */
					UWORD scale = SCALE * pd->winscale      / SCALE
										* cr->cr_multiplier / SCALE;
					glob->PaintRP.Layer = SaveLayer;
					
					Move(glob->Window->RPort, sx1, sy1);
					
					for ( t=dt ; t<lt ; )
					{
						UWORD skip = pd->parabskip[t] * scale / SCALE;
						if (skip == 0) skip = 1;
						
						deathtime += skip;
						t         += skip;
						
						if (t>lt)
						{
							t=lt;
							deathtime=lifetime;
						}
						
						if (sy1 < 0) Invisible = TRUE;
						
						if ((sx1 >= (glob->ww)) || (sx1 < 0) || (sy1 > (glob->wh)))
							break;
						
						sx2 = sx1;
						sy2 = sy1;
						
						sx1 = pd->centx + cr->cr_fx * deathtime                / pd->divx;
						sy1 = pd->centy
							+ (pd->parabtable[t] - cr->cr_coordoffset) * SCALE / pd->divy;
						
						if (Invisible)
						{
							if (sy1 >= 0)
							{
								Move(glob->Window->RPort, sx2, sy2);
								Invisible = FALSE;
							}
						}
						
						if (!Invisible)
						{
							if ((sx1 >= 0) && (sx1 < (glob->ww - margin)) &&
								(sy1 >= 0) && (sy1 < (glob->wh - margin)) )
							{
								if (Clean)
									glob->PaintRP.Layer = NULL;
								else
									Clean = TRUE;
							}
							else
							{
								if (Clean)
								{
									glob->PaintRP.Layer = SaveLayer;
									Clean = FALSE;
								}
							}
							Draw(glob->Window->RPort, sx1, sy1);
							//WritePixel(glob->Window->RPort, sx1, sy1);
							
							if (pref->Flags & PREFF_DOUBLE)
							{
								dx = sx2 - sx1; if (dx<0) dx=-dx;
								dy = sy2 - sy1; if (dy<0) dy=-dy;
								
								if (dy >= dx)
								{
									Move(glob->Window->RPort, sx2+1, sy2);
									Draw(glob->Window->RPort, sx1+1, sy1);
								}
								else
								{
									Move(glob->Window->RPort, sx2, sy2+1);
									Draw(glob->Window->RPort, sx1, sy1+1);
								}
								
								Move(glob->Window->RPort, sx1, sy1);
							}
						}
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
		
		for ( cr = (struct ParabolicCracker*) pd->CrackerList.mlh_Head ;
			  (ncr = (struct ParabolicCracker*) cr->cr_node.ln_Succ) ;
			  cr = ncr )
		{
			/* display this channel ? */
			if ((1<<cr->cr_chn) & Mask)
			{
				UWORD margin = (pref->Flags & PREFF_DOUBLE) ? 2 : 1;
				UBYTE rnd = (pd->curtime>>6) + cr->cr_starttime + cr->cr_note;
				
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
	
	glob->PaintRP.Layer = SaveLayer;
}

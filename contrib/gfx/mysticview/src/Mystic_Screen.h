#ifndef MYSTIC_SCREEN_H
#define	MYSTIC_SCREEN_H 1

/*********************************************************************
----------------------------------------------------------------------

	MysticView 
	Screen

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/

#include "Mystic_Global.h"


/*--------------------------------------------------------------------

	structures

--------------------------------------------------------------------*/

struct mvscreen
{
	struct SignalSemaphore semaphore;

	APTR visualinfo;
	
	struct Screen *screen;			/* custom or public screen */
	ULONG screentype;				/* PUBLICSCREEN or CUSTOMSCREEN */
	
	APTR psm;						/* screen-pen manager */

	int aspectx;					/* screen's aspect ratios */
	int aspecty;

	ULONG modeID;

	struct TextFont *screenfont;	// screen's font
};


#include "Mystic_Settings.h"

/*--------------------------------------------------------------------

	prototypes

--------------------------------------------------------------------*/

extern struct mvscreen *CreateMVScreen(struct mainsettings *mvs);
extern void DeleteMVScreen(struct mvscreen *scr);
extern void GetVisibleSize(struct mvscreen *scr, int *width, int *height);


#endif

// VolumeGauge.h
// $Date$
// $Revision$


#ifndef	VOLUMEGAUGE_H_INCLUDED
#define	VOLUMEGAUGE_H_INCLUDED

#include <intuition/intuition.h>
#include <intuition/classes.h>

#include <defs.h>

//----------------------------------------------------------------------------

#define	d(x)	;
#define	d1(x)	;
#define	d2(x)	x;

extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//----------------------------------------------------------------------------

#define	VOLUMEGAUGE_Dummy	(TAG_USER+0xea00)

#define	VOLUMEGAUGE_Ticks	(VOLUMEGAUGE_Dummy+0)	// (I..)
#define	VOLUMEGAUGE_TickSize	(VOLUMEGAUGE_Dummy+1)	// (I..)
#define	VOLUMEGAUGE_Min		(VOLUMEGAUGE_Dummy+2)	// (IS.)
#define	VOLUMEGAUGE_Max		(VOLUMEGAUGE_Dummy+3)	// (IS.)
#define	VOLUMEGAUGE_Level	(VOLUMEGAUGE_Dummy+4)	// (IS.)
#define	VOLUMEGAUGE_ShortTicks	(VOLUMEGAUGE_Dummy+5)	// (I..)

//----------------------------------------------------------------------------

struct VolumeGaugeInst
{
	struct Window *BarWindow;
	short Left, Top, Width, Height;

	short LastLevel;
	long Level;
	long MinValue, MaxValue;

	long ScaleFactor;
	long Ticks;
	long ShortTicks;
	long TickSize;

	UWORD ActiveFillPen;
	UWORD ActiveShinePen;
	UWORD ActiveShadowPen;
	UWORD InactiveFillPen;
	UWORD InactiveShinePen;
	UWORD InactiveShadowPen;
};

//----------------------------------------------------------------------------

Class *VOLUMEGAUGE_GetClass(void);
BOOL freeVolumeGaugeGadClass(void);

//----------------------------------------------------------------------------

#endif	/* VOLUMEGAUGE_H_INCLUDED */

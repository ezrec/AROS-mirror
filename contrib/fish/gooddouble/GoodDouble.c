/*
 *		GoodDouble.c
 *
 *		by Piotr Obminski
 *
 *		July 1993
 *
 *		version 0.3
 */

#include <aros/oldprograms.h>
#include <exec/types.h>
#include <intuition/intuition.h>

#include "good_double.h"	/* only needed because of #define GDC_REGARGS */


#define left_button     0L
#define middle_button   1L
#define right_button    2L


/* ------------------------------------------------------------------
 * returns TRUE if the time you give it was inside the double-click
 * time-distance from the last time it got; and NO OTHER MOUSE BUTTON
 * WAS CLICKED IN BETWEEN. Otherwise return FALSE.
 *
 * This one understands all three mouse-buttons (the MIDDLE one too)
 * ------------------------------------------------------------------ */

#ifdef GDC_REGARGS
	#ifdef _DCC					/* i.e. DICE */
    		__regargs
	#endif
#endif
		BOOL
	GoodDouble( button, secs, mics )
#ifdef GDC_REGARGS
		register LONG button;
		register ULONG secs, mics;
#else
		LONG button;
		ULONG secs, mics;
#endif
{
    static ULONG OldLSeconds    = 0L;
    static ULONG OldLMicros     = 0L;

    static ULONG OldMSeconds    = 0L;
    static ULONG OldMMicros     = 0L;

    static ULONG OldRSeconds    = 0L;
    static ULONG OldRMicros     = 0L;

    register BOOL ret_val = FALSE;

    switch ( button ) {
        case left_button:
            if ( DoubleClick( OldLSeconds, OldLMicros, secs, mics ) ) {
                OldLSeconds = OldLMicros = 0L;
                ret_val = TRUE;
            } else {
                OldLSeconds = secs;
                OldLMicros  = mics;
            }
            OldMSeconds = OldMMicros = OldRSeconds = OldRMicros = 0L;
            break;
        case middle_button:
            if ( DoubleClick( OldMSeconds, OldMMicros, secs, mics ) ) {
                OldMSeconds = OldMMicros = 0L;
                ret_val = TRUE;
            } else {
                OldMSeconds = secs;
                OldMMicros  = mics;
            }
            OldLSeconds = OldLMicros = OldRSeconds = OldRMicros = 0L;
            break;
        case right_button:
            if ( DoubleClick( OldRSeconds, OldRMicros, secs, mics ) ) {
                OldRSeconds = OldRMicros = 0L;
                ret_val = TRUE;
            } else {
                OldRSeconds = secs;
                OldRMicros  = mics;
            }
            OldLSeconds = OldLMicros = OldMSeconds = OldMMicros = 0L;
            break;
    }
    return( ret_val );
}

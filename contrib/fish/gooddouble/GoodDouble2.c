/*
 *		GoodDouble2.c
 *
 * ( '2' stands here both for second version and for the two mouse-buttons )
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
 * Returns TRUE if the time you give it was inside the double-click
 * time distance from the last time it got; and NO OTHER MOUSE BUTTON
 * WAS CLICKED IN BETWEEN. Otherwise return FALSE.
 *
 * This one is smaller, but handles only the LEFT & RIGHT i.e.
 * the usually available mouse-buttons! If it gets 1 as its first
 * argument, it will return FALSE, but NOT RESET THE TIMES (seconds &
 * micros) for the LEFT & RIGHT mouse-buttons.
 * ----------------------------------------------------------------- */

#ifdef GDC_REGARGS
	#ifdef _DCC					/* i.e. DICE */
    		__regargs BOOL
		GoodDouble2( LONG button, ULONG secs, ULONG mics )
	#else
			BOOL
		GoodDouble2( register LONG button,
					register ULONG secs,
					register ULONG mics )
	#endif
#else
		BOOL
	GoodDouble2( LONG button, ULONG secs, ULONG mics )
#endif
{
    static ULONG OldLSeconds    = 0L;
    static ULONG OldLMicros     = 0L;
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
            OldRSeconds = OldRMicros = 0L;
            break;
        case right_button:
            if ( DoubleClick( OldRSeconds, OldRMicros, secs, mics ) ) {
                OldRSeconds = OldRMicros = 0L;
                ret_val = TRUE;
            } else {
                OldRSeconds = secs;
                OldRMicros  = mics;
            }
            OldLSeconds = OldLMicros = 0L;
            break;
    }
    return( ret_val );
}

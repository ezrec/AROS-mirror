/**********************************************************************/
/*                                                                    */
/*                              ARoach.h                              */
/*                                                                    */
/*     © Copyright Stefan Winterstein 1993. Freely Distributable.     */
/*                                                                    */
/**********************************************************************/


/* #define MWDEBUG	1	*//* enable Memlib functions */

#include <dos/dos.h>
#include <dos/rdargs.h>
#include <exec/memory.h>
#include <graphics/gels.h>
#include <graphics/layers.h>
#define INTUI_V36_NAMES_ONLY
#include <intuition/intuition.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

/*
 * Prototypes from amiga.lib (with stack parameters)
 * Declare before #including <exec.h>.
 */
extern ULONG RangeSeed;	/* from amiga.lib */
extern /*__stdargs*/ UWORD RangeRand(LONG);
extern /*__stdargs*/ void ArgArrayDone( void );
extern /*__stdargs*/ UBYTE **ArgArrayInit( long argc, UBYTE **argv );
extern /*__stdargs*/ LONG ArgInt( UBYTE **tt, STRPTR entry, long defaultval );
extern /*__stdargs*/ STRPTR ArgString( UBYTE **tt, STRPTR entry, STRPTR defaulstring );

#include <proto/dos.h>			/* -> will be autoopened */
#include <proto/exec.h>			/* #includes <clib/alib_protos> ! */
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/layers.h>

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef MWDEBUG
#include "sc:extras/memlib/memwatch.h"
#endif

#include "animtools.h"	/* from RKRM */

#define MAXNAMESIZE		81  /* for options etc */
#define MAXCMDLINE		512

#define ROACH_HEADINGS	24	/* number of orientations */
#define ROACH_ANGLE		15	/* angle between orientations */
#define ROACH_DEPTH     1	/* number of planes used */

typedef struct RoachMap {
	char *roachBits;
	/* pixmap not needed */
	int height;
	int width;
	float sine;
	float cosine;
} RoachMap;

typedef struct Roach {
    RoachMap *rm;	/* used to be "rp" */
	struct Bob *bob; /* new */
    int index;
    float x;
    float y;
    int intX;
    int intY;
    int hidden;
    int turnLeft;
    int steps;
} Roach;

/* Image data is in file "RoachData.c"  */


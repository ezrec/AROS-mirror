#include "ownincs/BBSconsole.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;


                /* ESC Functions for the BBS-ANSI emulator.. */


/*
 ESC	1/11	1bh   ^[	Escape, start escape seq, cancel any others
*/
VOID BBS_invokeESC(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
/*	args && argc could be set to NULL..!! */

	BBS_cancel(con);
	con->inESC = TRUE;
}


/*
 ESC [		CSI		Control Sequence Introducer
*/
VOID BBS_invokeCSI(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
/*	args && argc could be set to NULL..!! */

	BBS_cancel(con);
	con->inCSI = TRUE;
}


/*
	ESC c		RIS		Hard Terminal Reset
*/
VOID BBS_hardreset(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	BBS_reset(con);
}


/* end of source-code */

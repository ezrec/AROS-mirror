/******************************************************************************

    MODUL
	all.h

    DESCRIPTION
	Precompileable headerfile.

******************************************************************************/

#ifndef ALL_H
#define ALL_H

/**************************************
		Includes
**************************************/
#include <stdio.h>
#include <stdlib.h>
#include <lists.h>
#include <strings.h>
#include <ctype.h>
#include <stdarg.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <devices/inputevent.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/rastport.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/diskfont_protos.h>
#include <clib/console_protos.h>
#include <clib/alib_protos.h>	       /*HD added */
#include <clib/icon_protos.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/asl.h>
#include <clib/asl_protos.h>
#include <clib/reqtools_protos.h>
#include <libraries/reqtools.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/diskfont_pragmas.h>
#include <pragmas/console_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/reqtools_pragmas.h>

/* Memory-Debugging. */
/*#include "debug_mem.h" */

#include "global.h"


/**************************************
	    Globale Variable
**************************************/
extern struct DosLibrary * DOSBase;


/**************************************
	Defines und Strukturen
**************************************/
#ifndef NULL
#define NULL 0
#endif
#ifdef E
#undef E
#endif
#ifndef offsetof
#define offsetof(sname,fname)  ((long)((sname *)0)->fname)
#endif

/*
 *  note: if you want Local routines to silently be global, simply
 *  '#define Local' to nothing.
 */

#define Prototype extern
#define Local	  static

#ifndef _DCC
#define __stkargs
#endif

#define MAXLINELEN	256	    /* Max. length of a line */
#define MAXROWS 	256	    /* Max. number of rows a XDME-window can have */
#define PINGDEPTH	10	    /* Max. number of PING/PONGs */
#define MAX_FONT_SIZE	64	    /* Font may be at most this pixel high */

#define MAXTOGGLE   256
#define QMOVE	    (0x6B)

#define COLT(n)  (XTbase + ColumnPos[n])
#define ROWT(n)  (YTbase + RowPos[n])
#define COL(n)   (Xbase  + ColumnPos[n])
#define ROW(n)   (Ybase  + RowPos[n])

#define nomemory()  { SETF_MEMORYFAIL(Ep,1); SETF_ABORTCOMMAND(Ep,1); }

struct __XDMEArgs
{
    char ** files;		/* filenames */
    char  * startupfile;	/* ".edrc" */
    ULONG   newxdme;		/* FALSE */
    ULONG   iconify;		/* FALSE */
    ULONG   autoload;		/* FALSE */
    char  * publicscreenname;	/* NULL */
    char  * projectfilename;	/* "XDME_Project_File" */

    struct RDArgs * ra; 	/* internal */
};


/**************************************
	       Prototypes
**************************************/


#endif /* ALL_H */

/******************************************************************************
*****  ENDE all.h
******************************************************************************/


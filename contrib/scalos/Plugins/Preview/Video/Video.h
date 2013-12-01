// Video.h
// $Date$
// $Revision$

#ifndef VIDEO_PREFSPLUGIN_H
#define VIDEO_PREFSPLUGIN_H

//---------------------------------------------------------------

#include "plugin.h"

//---------------------------------------------------------------

#define Sizeof(array)		(sizeof(array) / sizeof((array)[0]))

//---------------------------------------------------------------

// Debugging...
#define	d(x)	;
#define	d1(x)	;
#define	d2(x)	x;

// aus debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//---------------------------------------------------------------

// defined in  mempools.lib

#ifndef __amigaos4__
extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);
#endif

//----------------------------------------------------------------------------

BOOL InitExtra(void);
void ExitExtra(void);
BOOL InitInstance(void);
void ExitInstance(void);

#endif /* VIDEO_PREFSPLUGIN_H */


// persist.h
// $Date$
// $Revision$


#ifndef PERSIST_H_INCLUDED
#define	PERSIST_H_INCLUDED

//----------------------------------------------------------------------------

#define	d(x)	;
#define	d1(x)	;
#define	d2(x)	x;

// aus debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//----------------------------------------------------------------------------

#include <intuition/classes.h>
#include <scalos/scalos.h>
#include <defs.h>

#define	PERSIST_CONFIGFILENAME	"ENV:Scalos/Persist.prefs"
#define	PERSIST_FILENAME	"ENVARC:Scalos/Persistant_Windows"
#define	TEMP_FILENAME		"t:PersistTemp"

//----------------------------------------------------------------------------

struct OpenNode
	{
	struct Node on_Node;
	BOOL on_Iconified;
	BOOL on_BrowserMode;
	const struct ScaWindowTask *on_WindowTask;
	char *on_Path;
	WORD on_Left;		// Window LeftEdge
	WORD on_Top;		// Window TopEdge
	WORD on_Width;		// Window Width
	WORD on_Height;		// Window Height
	};

struct ProcessTimerInfo
	{
	struct MsgPort *ppt_ioPort;
	T_TIMEREQUEST *ppt_TimeReq;
	BOOL ppt_timerOpen;
	BOOL ppt_timerPending;		// TRUE when spt_TimeReq is in use
	};

//----------------------------------------------------------------------------

#endif	/* PERSIST_H_INCLUDED */

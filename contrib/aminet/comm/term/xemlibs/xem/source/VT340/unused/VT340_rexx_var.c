#include "csource/lr-comm/minrexx.h"

APTR	InvokeRexx(VOID);
ULONG	upRexxPort(APTR, UBYTE *, struct rexxCommandList *, UBYTE *, ULONG (*uf)(APTR, APTR, struct RexxMsg *, struct rexxCommandList *, UBYTE *), APTR);
VOID	dnRexxPort(APTR);
VOID	dispRexxPort(APTR);
VOID	replyRexxCmd(APTR, struct RexxMsg *, ULONG, ULONG, UBYTE *);
struct RexxMsg *sendRexxCmd(APTR, UBYTE *, BOOL (*f)(VOID), STRPTR ,STRPTR ,STRPTR);
struct RexxMsg *syncRexxCmd(APTR, STRPTR, struct RexxMsg *);
struct RexxMsg *asyncRexxCmd(APTR, UBYTE *);
VOID	ShutRexx(APTR);

/*-----------------  ARexx Application Prototypes  ---------------*/

ULONG	VT340_RexxDispatcher(APTR, struct VT340Console *, struct RexxMsg *, struct rexxCommandList *, UBYTE *);
BOOL	VT340_reset(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_setclass(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_setid(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_newlinemode(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_insertmode(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_wrapmode(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_cursorvisible(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_smoothscroll(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_use24lines(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_destructivebs(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_destructiveff(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_destructivedel(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_adjustlines(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_swapbsdel(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_useisolatin1(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_savescreento(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_answerbackmsg(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_titlebar(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);
BOOL	VT340_emit(APTR, struct VT340Console *, struct RexxMsg *, UBYTE *);

struct rexxCommandList VT340_RexxCommandList[] = {
	{ "setclass",		(APTR)VT340_setclass },
	{ "setid",			(APTR)VT340_setid },
	{ "newlinemode",	(APTR)VT340_newlinemode },
	{ "insertmode",	(APTR)VT340_insertmode },
	{ "wrapmode",		(APTR)VT340_wrapmode },
	{ "cursorvisible",(APTR)VT340_cursorvisible },
	{ "smoothscroll",	(APTR)VT340_smoothscroll },
	{ "use24lines",	(APTR)VT340_use24lines },
	{ "adjustlines",	(APTR)VT340_adjustlines },
	{ "destructivebs",(APTR)VT340_destructivebs },
	{ "destructiveff",(APTR)VT340_destructiveff },
	{ "destructivedel",(APTR)VT340_destructivedel },
	{ "swapbsdel",		(APTR)VT340_swapbsdel },
	{ "useisolatin1",	(APTR)VT340_useisolatin1 },
	{ "savescreento",	(APTR)VT340_savescreento },
	{ "answerbackmsg",(APTR)VT340_answerbackmsg },
	{ "reset",			(APTR)VT340_reset },
	{ "titlebar",		(APTR)VT340_titlebar },
	{ "emit",			(APTR)VT340_emit },
	{ 0, 0 }
};

/* end of source-code */

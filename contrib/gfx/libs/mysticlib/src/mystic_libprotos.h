
#ifndef _MYSTIC_LIBPROTOS_H
#define _MYSTIC_LIBPROTOS_H 1

#include <intuition/screens.h>
#include <guigfx/guigfx.h>

/* mystic_lib.c */

struct Library * LIBENT LibInit(REG(d0) struct Library *LibBase, REG(a0) BPTR Segment, REG(a6) struct ExecBase *ExecBase);

struct Library * LIBENT LibOpen(
#ifdef __MORPHOS__
	void);
#else
	REG(a6) struct Library *LibBase);
#endif

#ifdef __MORPHOS__
BPTR LIBExpunge(void);
#endif
BPTR LIBENT LibExpunge(REG(a6) struct Library *LibBase);

BPTR LIBENT LibClose(
#ifdef __MORPHOS__
	void);
#else
	REG(a6) struct Library *LibBase);
#endif

LONG LibNull(void);


/* mystic_InitExit.c */

BOOL LIBENT MYSTIC_Init(void);
void LIBENT MYSTIC_Exit(void);


/* mystic_view.c */


APTR SAVE_DS ASM CreateView(REG(a0) struct Screen *, REG(a1) struct RastPort *, REG(a2) TAGLIST);
void SAVE_DS ASM DeleteView(REG(a0) APTR);
void SAVE_DS ASM SetViewAttrs(REG(a0) APTR, REG(a1) TAGLIST);
void SAVE_DS ASM SetViewPicture(REG(a0) APTR, REG(a1) PICTURE *);
BOOL SAVE_DS ASM DrawOn(REG(a0) APTR);
void SAVE_DS ASM DrawOff(REG(a0) APTR);
void SAVE_DS ASM RefreshView(REG(a0) APTR);
void SAVE_DS ASM GetViewAttrs(REG(a0) APTR mview, REG(a1) TAGLIST tags);
void SAVE_DS ASM SetViewRelative(REG(a0) APTR mview, REG(d0) LONG x, REG(d1) LONG y);
void SAVE_DS ASM SetViewStart(REG(a0) APTR mview, REG(d0) LONG startx, REG(d1) LONG starty);
void SAVE_DS ASM LockView(REG(a0) APTR mview, REG(d0) BOOL lock);

#endif

#ifndef FIREWORKS_H
#define FIREWORKS_H

#include <libcore/compiler.h>

#ifdef _AROS
#  define CHIP
#endif

#ifndef __SASC
#  define __stdargs
#endif

#include "fireworks_includes.h"

/* Some defines */

#define FPS 25				/* nominal frames per seconds */
#define MIN_FPS 2			/* when FPS drops below, the */
							/* watchdog timer intervenes */

#define NORMPRI -10			/* priority during normal operation */
#define HIGHPRI  10			/* priority in idle mode or when */
							/* watchdog timer intervenes */


#define MAXCOORD 1024

#define CENTER_X 512		/* window width  is 0...MAXCOORD */
#define CENTER_Y 980		/* window height is 0...MAXCOORD */


/* Library bases */

extern struct ExecBase			*SysBase;
extern struct DosLibrary		*DOSBase;
extern struct GfxBase			*GfxBase;
extern struct Library			*LayersBase;
extern struct IntuitionBase	*IntuitionBase;
extern struct Library 			*GadToolsBase;
extern struct UtilityBase			*UtilityBase;
extern struct Library			*DatatypesBase;
extern struct Library			*AslBase;
extern struct Library			*WorkbenchBase;
extern struct Library			*CamdBase;
extern struct Library			*TimerBase;
extern BOOL				 FPBase;	/* pseudo floating point base */


/* the main program's task */

extern struct Task *MyTask;


/* these flags serve for keeping track of asynchronous tasks */

typedef enum
{
	AboutTask  = 0x00000001,	/* these are the asynchronous messages */
	SaveTask   = 0x00000002,
	ErrTask    = 0x00000004,
	DropTask   = 0x00000008,
	LaunchTask = 0x00000010,
	CloseTask  = 0x00000020,
	
	PlayTask   = 0x00000100,	/* these are the file requesters */
	ImageTask  = 0x00000200,
	
	LinkTask   = 0x00001000,	/* the MIDI link requester */
	ScreenTask = 0x00002000,	/* the screenmode requester */

} TaskFlag;


/* Palette organisation */

enum
{
	Backgroundpen,
	Sparkpen,
	Channelpens,
	
	NUMPENS = Channelpens + 16
};


/* global preferences data */

struct Prefs
{
	UBYTE Header[10];
	UWORD Version;
	UBYTE Link[32];
	UWORD FWMode;
	UWORD Sensitivity;
	ULONG Flags;
	UWORD WinX;
	UWORD WinY;
	UWORD WinW;
	UWORD WinH;
	UBYTE Image[240];
	ULONG ScreenMode;
	UWORD ScreenWidth;
	UWORD ScreenHeight;
	UWORD ScreenDepth;
	ULONG OverscanType;
	BOOL AutoScroll;
	UBYTE MIDIFile[240];
};

#define PREFF_BACKDROP   (1L<< 0)
#define PREFF_TILE       (1L<< 1)
#define PREFF_FULLSCREEN (1L<< 2)
#define PREFF_DOUBLE     (1L<< 3)
#define PREFF_SPARKS     (1L<< 4)
#define PREFF_GM         (1L<< 5)
#define PREFF_GS         (1L<< 6)
#define PREFF_XG         (1L<< 7)
#define PREFF_MT32       (1L<< 8)
#define PREFF_ASLHOOKS   (1L<< 9)
#define PREFF_TIMESTAMP  (1L<<10)
#define PREFF_HDTITLEBAR (1L<<11)
#define PREFF_HDMOUSEPTR (1L<<12)


/* all data structures and pointers needed by this program */

struct Globals
{
	struct Screen *LockedScreen;
	struct Screen *OpenedScreen;
	
	struct Screen *Screen;
	APTR VisualInfo;
	struct Window *Window;
	struct Menu   *Menu;
	
	struct Window *ProcWindow;
	
	struct MsgPort *AppPort;
	struct AppWindow *AppWindow;
	
	struct timerequest *treq;
	
	WORD ww, wh;
	
	LONG PenArray[NUMPENS];
	
	Object *dto;
	struct BitMap *BGBitMap;
	struct BitMap *PaintBitMap;
	struct RastPort PaintRP;
	struct Layer_Info *LInfo;
	struct Layer *PaintLayer;
	
	struct MidiNode *midi;
	struct MidiLink *link;
	
	UBYTE WTitle[80];
	
	APTR NotePool;
	
	BOOL GUIRefresh;
	BOOL LinkRefresh;
	
	struct FileRequester *ImageFR;
	struct FileRequester *MIDIFR;
	struct ScreenModeRequester *ScreenModeRQ;
	
	ULONG TaskAlloc;
};

#endif	/* FIREWORKS_H */

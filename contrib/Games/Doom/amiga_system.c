#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <proto/exec.h>
//#include <powerup/clib/ppc_protos.h>

#include "doomdef.h"
#include "m_misc.h"
#include "i_system.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"
#include "m_argv.h"

extern void ppctimer (unsigned int *time);
extern double tb_scale_lo;
extern double tb_scale_hi;

extern byte *vid_mem;

void amiga_getevents (void);

#define MIN_ZONESIZE  (2*1024*1024)
#define MAX_ZONESIZE  (6*1024*1024)

/**********************************************************************/

extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

/**********************************************************************/
// Called by DoomMain.
void I_Init (void)
{
  if ((GfxBase = (struct GfxBase *) OpenLibrary ("graphics.library", 39)) == NULL)
    I_Error ("OpenLibrary(""graphics.library"", 39) failed");
  if ((IntuitionBase = (struct IntuitionBase *) OpenLibrary ("intuition.library", 39)) == NULL)
    I_Error ("OpenLibrary(""intuition.library"", 39) failed");

  I_InitSound ();
  I_InitMusic ();
  //  I_InitGraphics ();
}

/**********************************************************************/
// Called by startup code
// to get the ammount of memory to malloc
// for the zone management.
byte*	I_ZoneBase (int *size)
{
  byte *zone;
  ULONG memfree, largest;
  int p;

  memfree = AvailMem (MEMF_FAST);
  largest = AvailMem (MEMF_FAST | MEMF_LARGEST);
  printf ("Memfree = %d, largest = %d\n", memfree, largest);

  p = M_CheckParm ("-heapsize");
  if (p && p < myargc - 1) {

    *size = 1024 * atoi (myargv[p+1]);

  } else {

    if (largest > MAX_ZONESIZE + 65536 &&
        memfree > MAX_ZONESIZE + (2 * 1024 * 1024))
      *size = MAX_ZONESIZE;
    else if (memfree < largest + (2 * 1024 * 1024))
      *size = memfree - (2 * 1024 * 1024) - 65536;
    else
      *size = largest - 65536;

    if (*size < MIN_ZONESIZE)
      I_Error ("Unable to allocate at least %d fastmem for zone management\n"
               "while leaving 2Mb fastmem free\n"
               "Fastmem free = %d, largest block = %d",
               MIN_ZONESIZE, memfree, largest);
  }

  if ((zone = (byte *)malloc(*size)) == NULL)
    I_Error ("malloc() %d bytes for zone management failed", *size);

  printf ("I_ZoneBase(): Allocated %d bytes for zone management\n", *size);

  return zone;
}

/**********************************************************************/
// Called by D_DoomLoop,
// returns current time in tics.
int I_GetTime (void)
{
  unsigned int clock[2];
  double currtics;
  static double basetics=0.0;

  ppctimer (clock);
  if (basetics == 0.0)
    basetics = ((double) clock[0])*tb_scale_hi + ((double) clock[1])/tb_scale_lo;

  currtics = ((double) clock[0])*tb_scale_hi + ((double) clock[1])/tb_scale_lo;
  return (int) (currtics-basetics);
}

/**********************************************************************/
//
// Called by D_DoomLoop,
// called before processing any tics in a frame
// (just after displaying a frame).
// Time consuming syncronous operations
// are performed here (joystick reading).
// Can call D_PostEvent.
//
void I_StartFrame (void)
{
  amiga_getevents ();
}

/**********************************************************************/
//
// Called by D_DoomLoop,
// called before processing each tic in a frame.
// Quick syncronous operations are performed here.
// Can call D_PostEvent.
void I_StartTic (void)
{
}

/**********************************************************************/
// Asynchronous interrupt functions should maintain private queues
// that are read by the synchronous functions
// to be converted into events.

// Either returns a null ticcmd,
// or calls a loadable driver to build it.
// This ticcmd will then be modified by the gameloop
// for normal input.
ticcmd_t	emptycmd;
ticcmd_t* I_BaseTiccmd (void)
{
  return &emptycmd;
}

/**********************************************************************/
// Called by M_Responder when quit is selected.
// Clean exit, displays sell blurb.
void I_Quit (void)
{
  D_QuitNetGame ();
  I_ShutdownSound();
  I_ShutdownMusic();
  M_SaveDefaults ();
  I_ShutdownGraphics();

  if (GfxBase != NULL) {
    CloseLibrary ((struct Library *) GfxBase);
    GfxBase = NULL;
  }
  if (IntuitionBase != NULL) {
    CloseLibrary ((struct Library *) IntuitionBase);
    IntuitionBase = NULL;
  }

  if (vid_mem != NULL)
    PPCFreeVec (vid_mem);

  exit(0);
}

/**********************************************************************/
// Allocates from low memory under dos,
// just mallocs under unix
byte* I_AllocLow (int length)
{
  byte*	mem;

  if ((mem = (byte *)malloc (length)) == NULL)
    I_Error ("Out of memory allocating %d bytes", length);
  memset (mem,0,length);
  return mem;
}

/**********************************************************************/
void I_Tactile (int on, int off, int total)
{
  // UNUSED.
  on = off = total = 0;
}

/**********************************************************************/
void *I_malloc (size_t size)
{
  void *b;

  if ((b = malloc (size)) == NULL)
    I_Error ("Out of memory allocating %d bytes", size);
  return b;
}

/**********************************************************************/
void *I_calloc (size_t nelt, size_t esize)
{
  void *b;

  if ((b = calloc (nelt, esize)) == NULL)
    I_Error ("Out of memory allocating %d bytes", nelt * esize);
  return b;
}

/**********************************************************************/
void I_Error (char *error, ...)
{
  va_list	argptr;

  // Message first.
  va_start (argptr, error);
  fprintf (stderr, "Error: ");
  vfprintf (stderr, error, argptr);
  fprintf (stderr, "\n");
  va_end (argptr);

  fflush (stderr);

  // Shutdown. Here might be other errors.
  if (demorecording)
    G_CheckDemoStatus ();

  D_QuitNetGame ();
  I_ShutdownGraphics ();

  if (GfxBase != NULL) {
    CloseLibrary ((struct Library *) GfxBase);
    GfxBase = NULL;
  }
  if (IntuitionBase != NULL) {
    CloseLibrary ((struct Library *) IntuitionBase);
    IntuitionBase = NULL;
  }

  if (vid_mem != NULL)
    PPCFreeVec (vid_mem);

  exit (20);
}

/**********************************************************************/

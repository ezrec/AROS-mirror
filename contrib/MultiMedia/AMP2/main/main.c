/*
 *
 * main.c
 *
 */

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#if !defined(__AROS__)
#include <powerup/ppcproto/intuition.h>
#include <powerup/ppclib/interface.h>
#include <powerup/gcclib/powerup_protos.h>
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcinline/graphics.h>
#include <powerup/ppcproto/dos.h>
#include <powerup/ppcproto/asl.h>

#include <cybergraphx/cybergraphics.h>
#include <powerup/ppcinline/cybergraphics.h>

#include <cybergraphx/cgxvideo.h>
#include <powerup/ppcinline/cgxvideo.h>
#else

#include "aros-inc.h"

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <amiaudio.h>
#include "prefs.h"
#include "plugin.h"
#include "keyfile.h"
#include "main.h"
#include "buffer.h"
#include "../refresh/refresh.h"


#if !defined(__AROS__)
struct Library *GfxBase      = NULL;
#else
struct Device	 *TimerBase=NULL;
#endif

struct IntuitionBase *IntuitionBase = NULL;
struct Library *GadToolsBase = NULL;
struct Library *UtilityBase  = NULL;
struct Library *AslBase      = NULL;
struct Library *DiskfontBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *CGXVideoBase = NULL;

static char *filename = NULL;

void close_libraries()
{
  if (CGXVideoBase != NULL) {
    CloseLibrary(CGXVideoBase);
    CGXVideoBase = NULL;
  }
  if (CyberGfxBase != NULL) {
    CloseLibrary(CyberGfxBase);
    CyberGfxBase = NULL;
  }
  if (DiskfontBase != NULL) {
    CloseLibrary(DiskfontBase);
    DiskfontBase = NULL;
  }
  if (AslBase != NULL) {
    CloseLibrary(AslBase);
    AslBase = NULL;
  }
  if (UtilityBase != NULL) {
    CloseLibrary(UtilityBase);
    UtilityBase = NULL;
  }
  if (GfxBase != NULL) {
    CloseLibrary(GfxBase);
    GfxBase = NULL;
  }
  if (GadToolsBase != NULL) {
    CloseLibrary(GadToolsBase);
    GadToolsBase = NULL;
  }
  if (IntuitionBase != NULL) {
    CloseLibrary(IntuitionBase);
    IntuitionBase = NULL;
  }
}

void open_libraries()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
  if (IntuitionBase == NULL) {
    printf("Could not open intuition.library v39 or later!\n");
    close_libraries();
    exit(0);
  }

  GadToolsBase = OpenLibrary("gadtools.library", 39);
  if (GadToolsBase == NULL) {
    printf("Could not open gadtools.library v39 or later!\n");
    close_libraries();
    exit(0);
  }

  GfxBase = OpenLibrary("graphics.library", 39);
  if (GfxBase == NULL) {
    printf("Could not open graphics.library v39 or later!\n");
    close_libraries();
    exit(0);
  }

  UtilityBase = OpenLibrary("utility.library", 39);
  if (UtilityBase == NULL) {
    printf("Could not open utility.library v39 or later!\n");
    close_libraries();
    exit(0);
  }

  AslBase = OpenLibrary("asl.library", 39);
  if (AslBase == NULL) {
    printf("Could not open asl.library v39 or later!\n");
    close_libraries();
    exit(0);
  }

  DiskfontBase = OpenLibrary("diskfont.library", 39);
  if (DiskfontBase == NULL) {
    printf("Could not open diskfont.library v39 or later!\n");
    close_libraries();
    exit(0);
  }

  CyberGfxBase = OpenLibrary("cybergraphics.library", 40);
  CGXVideoBase = OpenLibrary("cgxvideo.library", 41);
}

#if !defined(__AROS__)
#define TEMPLATE "FILE/A,REQ/S,PAL/S,HIPAL/S,GRAY/S," \
                 "HALF/S,GRAYDEPTH/N,LOWCOLOR/S,HIGHCOLOR/S,TRUECOLOR/S,WINDOW/S,DEBUG/S," \
                 "VERBOSE/S,OVERLAY/S,STEREO/S,NOVIDEO/S,NOAUDIO/S,DEVICE/K,UNIT/N," \
                 "TRIPLE/S,FILTER/S,READALL/S,DIVISOR/N,LORES/S,OSD/S,14BIT/S,CALIBRATION/K," \
                 "56KHZ/S,SUBTITLE/K," \
                 "SPEEDTEST/K," \
                 "HAM/S,HAMDEPTH/N,HAMWIDTH/N,HAMHQ/S," \
                 "AHI/S"
#else
#define TEMPLATE "FILE/A,REQ/S,PAL/S,HIPAL/S,GRAY/S," \
                 "HALF/S,GRAYDEPTH/N,LOWCOLOR/S,HIGHCOLOR/S,TRUECOLOR/S,WINDOW/S,DEBUG/S," \
                 "VERBOSE/S,OVERLAY/S,STEREO/S,NOVIDEO/S,NOAUDIO/S,DEVICE/K,UNIT/N," \
                 "TRIPLE/S,FILTER/S,READALL/S,DIVISOR/N,LORES/S,OSD/S,14BIT/S,CALIBRATION/K," \
                 "56KHZ/S,SUBTITLE/K," \
                 "SPEEDTEST/K"
#endif

#define OPT_FILE          0
#define OPT_REQ           1
#define OPT_PAL           2
#define OPT_HIPAL         3
#define OPT_GRAY          4
#define OPT_HALF          5
#define OPT_GRAYDEPTH     6
#define OPT_LOWCOLOR      7
#define OPT_HIGHCOLOR     8
#define OPT_TRUECOLOR     9
#define OPT_WINDOW       10
#define OPT_DEBUG        11
#define OPT_VERBOSE      12
#define OPT_OVERLAY      13
#define OPT_STEREO       14
#define OPT_NOVIDEO      15
#define OPT_NOAUDIO      16
#define OPT_DEVICE       17
#define OPT_UNIT         18
#define OPT_TRIPLE       19
#define OPT_FILTER       20
#define OPT_READALL      21
#define OPT_DIVISOR      22
#define OPT_LORES        23
#define OPT_OSD          24
#define OPT_14BIT        25
#define OPT_CALIBRATION  26
#define OPT_56KHZ        27
#define OPT_SUBTITLE     28

#define OPT_SPEEDTEST    29

#if !defined(__AROS__)
#warning "TODO: AROS could probably support HAM?"
#define OPT_HAM          30
#define OPT_HAMDEPTH     31
#define OPT_HAMWIDTH     32
#define OPT_HAMHQ        33
/* On AROS AHI is the only Audio option! */
#define OPT_AHI          34  
#define OPT_COUNT        35
#else
#define OPT_COUNT        30
#endif



long opts[OPT_COUNT];
struct RDArgs *rdargs = NULL;

void handle_arguments(int argc, char *argv[])
{
  memset((char *)opts, 0, sizeof(opts));

  if ((rdargs = ReadArgs(TEMPLATE, opts, NULL))) {
    printf(", Copyright (c) 1998-2003 Mathias Roslund\n\n");

    /* Set prefs according to arguments */
    filename = strdup((char *)opts[OPT_FILE]);

    if (opts[OPT_REQ]) prefs.screenmode = PREFS_REQUESTER;
    if (opts[OPT_PAL]) prefs.screenmode = PREFS_PAL;
    if (opts[OPT_HIPAL]) prefs.screenmode = PREFS_HIPAL;

#if !defined(__AROS__)
    if (opts[OPT_HAM]) prefs.colormode = PREFS_HAM;
    if (opts[OPT_HAMDEPTH]) prefs.ham_depth = *((unsigned long *)opts[OPT_HAMDEPTH]);
    if (opts[OPT_HAMWIDTH]) prefs.ham_width = *((unsigned long *)opts[OPT_HAMWIDTH]);
    if (opts[OPT_HAMHQ]) prefs.ham_quality = PREFS_HIGH;
#endif

    if (opts[OPT_GRAY]) prefs.colormode = PREFS_GRAY;
    if (opts[OPT_HALF]) prefs.half = PREFS_ON;
    if (opts[OPT_GRAYDEPTH]) prefs.gray_depth = *((unsigned long *)opts[OPT_GRAYDEPTH]);
    if (opts[OPT_LOWCOLOR]) prefs.cgfx_depth = PREFS_LOWCOLOR;
    if (opts[OPT_HIGHCOLOR]) prefs.cgfx_depth = PREFS_HIGHCOLOR;
    if (opts[OPT_TRUECOLOR]) prefs.cgfx_depth = PREFS_TRUECOLOR;
    if (opts[OPT_WINDOW]) prefs.window = PREFS_ON;
    if (opts[OPT_DEBUG]) prefs.debug = PREFS_ON;
    if (opts[OPT_VERBOSE]) prefs.verbose = PREFS_ON;
    if (opts[OPT_OVERLAY]) prefs.overlay = PREFS_ON;
    if (opts[OPT_STEREO]) prefs.stereo = PREFS_ON;
    if (opts[OPT_NOVIDEO]) { prefs.no_video = PREFS_ON; prefs.no_audio = PREFS_OFF; }
    if (opts[OPT_NOAUDIO]) { prefs.no_audio = PREFS_ON; prefs.no_video = PREFS_OFF; }
    if (opts[OPT_DEVICE]) strcpy(prefs.device, (char *)opts[OPT_DEVICE]);
    if (opts[OPT_UNIT]) prefs.unit = *((int *)opts[OPT_UNIT]);

#if !defined(__AROS__)
    if (opts[OPT_AHI])
#endif
      prefs.ahi = PREFS_ON;

    if (opts[OPT_TRIPLE]) prefs.triple = PREFS_ON;
    if (opts[OPT_FILTER]) prefs.filter = PREFS_ON;
    if (opts[OPT_READALL]) prefs.readall = PREFS_ON;
    if (opts[OPT_DIVISOR]) prefs.divisor = *((unsigned long *)opts[OPT_DIVISOR]);
    if (opts[OPT_LORES]) prefs.lores = PREFS_ON;
    if (opts[OPT_OSD]) prefs.osd = PREFS_ON;
    if (opts[OPT_14BIT]) prefs.audiomode = PREFS_14BIT;
    if (opts[OPT_CALIBRATION]) strcpy(prefs.calibration, (char *)opts[OPT_CALIBRATION]);
    if (opts[OPT_56KHZ]) prefs.khz56 = PREFS_ON;
    if (opts[OPT_SUBTITLE]) strcpy(prefs.subtitle, (char *)opts[OPT_SUBTITLE]);

    if (opts[OPT_SPEEDTEST]) {
      char *s = (char *)opts[OPT_SPEEDTEST];
      int l = strlen(s);

      prefs.speedtest = 0x00;
      if (strncasecmp(s, "disp", 4) == 0) {
        prefs.speedtest = 0x01;
      } else if (strncasecmp(s, "null", 4) == 0) {
        prefs.speedtest = 0x01 | 0x02;
      } else if (strncasecmp(s, "conv", 4) == 0) {
        prefs.speedtest = 0x01 | 0x04;
      }
      if (prefs.speedtest && (l == 8)) {
        if (strncasecmp(&s[4], "skip", 4) == 0) {
          prefs.speedtest |= 0x08;
        }
      }
      if (prefs.speedtest == 0x00) {
        printf("illegal speedtest option, ignoring speedtest\n");
        prefs.speedtest = 0x00;
      }
    }

    /* Check the prefs to make sure nothing is wrong */
    check_prefs();
  } else {
    printf(", Copyright (c) 1998-2003 Mathias Roslund\n\nusage: %s <moviefile> <options>\n\n", argv[0]);

    printf("REQ/S         : use an ASL requester to get the screenmode (def: BestMode).\n");
    printf("PAL/S         : use PAL Lores (def: BestMode).\n");
    printf("HIPAL/S       : use PAL Hires Laced (def: BestMode).\n\n");

    printf("GRAY/S        : gray output (def: color).\n");

#if !defined(__AROS__)
    printf("HAM/S         : HAM output (def: color).\n\n");
    printf("HAMDEPTH/N    : HAM depth (6 or 8 bitplanes, def: 8).\n");
    printf("HAMWIDTH/N    : HAM width (1, 2 or 4 HAM pixels per RGB pixel, def: 2).\n");
    printf("HAMHQ/S       : HAM high quality (def: normal quality).\n");
#endif

    printf("HALF/S        : half height (PAL only) (def: off).\n\n");

    printf("GRAYDEPTH/N   : gray depth (4, 6 or 8 bitplanes, def: 8).\n");
    printf("LOWCOLOR/S    : 8bit CGFX (used for BestMode).\n");
    printf("HIGHCOLOR/S   : 16/15bit CGFX (used for BestMode).\n");
    printf("TRUECOLOR/S   : 24/32bit CGFX (used for BestMode).\n");
    printf("LORES/S       : decode at 1/4 of original resolution (def: off).\n\n");

    printf("WINDOW/S      : use a window on Workbench for playback (def: on).\n");
    printf("DEBUG/S       : use this if you have problems and send me the output (def: off).\n");
    printf("VERBOSE/S     : gives additional information while/after playing (def: off).\n");
    printf("OVERLAY/S     : enable CGFX overlay support (def: off).\n");
    printf("STEREO/S      : play and decode stereo audio (def: off).\n\n");

    printf("NOVIDEO/S     : no video playback, only audio.\n");
    printf("NOAUDIO/S     : no audio playback, only video.\n\n");

    printf("DEVICE/K      : device of the CD/DVD for VCD/CD-i/DVD playback (def: ata.device).\n");
    printf("UNIT/N        : unit of the CD/DVD for VCD/CD-i/DVD playback (def: 2).\n\n");

#if !defined(__AROS__)
    printf("AHI/S         : use AHI instead of audio.device (def: on).\n");
#endif

    printf("14BIT/S       : 14bit audio.device playback (def: 8bit).\n");
    printf("CALIBRATION/K : 14bit calibration file (def: no calibration).\n");
    printf("56KHZ/S       : allow up to 56khz audio.device playback (def: off).\n");
    printf("SUBTITLE/K    : subtitle file (def: no subtitle).\n");
    printf("TRIPLE/S      : enable triple buffering (def: off).\n");
    printf("FILTER/S      : turn on audio filter (def: off).\n");
    printf("READALL/S     : read the entire movie to memory before playing (def: off).\n");
    printf("DIVISOR/N     : audio frequency divisor (1, 2 or 4, def: 2).\n\n");

    exit(0);
  }
}

void free_arguments()
{
  if (rdargs) {
    FreeArgs((struct RDArgs *) rdargs);
  }
}

void amp_printf(const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void debug_printf(const char *fmt, ...)
{
  va_list args;

  if (prefs.debug == PREFS_ON) {
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}

void verbose_printf(const char *fmt, ...)
{
  va_list args;

  if (prefs.verbose == PREFS_ON) {
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}

int main(int argc, char *argv[])
{
  BPTR lock = NULL;

  /* Set default preferences */
  set_prefs();

  /* Get AMP directory */
  lock = GetProgramDir();
  NameFromLock(lock, prefs.cwd, 256);
  if (prefs.cwd[strlen(prefs.cwd) - 1] != ':') {
    strcat(prefs.cwd, "/");
  }

  /* Check keyfile */
  check_keyfile();
  amp_printf("\n%s", prefs.registered);

  /* Deal with commandline arguments */
  handle_arguments(argc, argv);

  /* Open libraries */
  open_libraries();
  refresh_init();

  if (audio_setup() != 0) {
    printf("audio setup failed\n");
  } else {
    if (prefs.speedtest & 0x04) {
      refresh_test(filename);
    } else {
      video_fopen();
      audio_fopen();

      start_plugin(filename);

      audio_fclose();
      video_fclose();
    }
  }

  /* Free some resources */
  free_arguments();
  refresh_exit();
  close_libraries();

  audio_cleanup();

  return 0;
}

/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996 by
 *
 *      Jan Hubicka          (hubicka@paru.cas.cz)
 *      Thomas Marsh         (tmarsh@austin.ibm.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#undef _EFENCE_

#ifdef _plan9_
#include <u.h>
#include <libc.h>
#include <ctype.h>

#else

#include <aconfig.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#ifdef __EMX__
#include <float.h>
#endif

#ifdef __EMX__
#include <sys/types.h>
#endif

#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef AROS
#include <signal.h>
#endif
#endif

#include <config.h>
#include <fconfig.h>
#ifndef _plan9_
#include <assert.h>
#endif
#include "help.h"
#include <filter.h>
#include <ui_helper.h>
#include <ui.h>
#include <param.h>
#include <version.h>
#include <timers.h>
#include <plane.h>
#include <xthread.h>

#ifdef DESTICKY
int euid, egid;
#endif
#define SIZE 1024
#define add_buffer(char) if(((end+1)%SIZE)!=begin) {keybuffer[end]=char;end=(end+1)%SIZE;}

#define textheight1 (driver->textheight)
#define textwidth1 (driver->textwidth)

#define FORMULA 0
#define HELP 1
#define DRIVER 2
#define MENU 3
#define FILTER 4
#define INPUT 5
#define XTEXTMAX 80
#define YTEXTMAX 80
#define ui_flush() (driver->flush?driver->flush(),1:0)
static int textstart = 1;
static char inputtext[XTEXTMAX][YTEXTMAX];
static int line;
static int nlines;
static int mlines;
void (*inputhandler) (void);

int   prog_argc;
char  **prog_argv;

extern struct ui_driver *drivers[];
extern int ndrivers;
static int width, height;

static int maxiter;
static xio_pathdata configfile;
static int autopilot;
static void ui_help (int);
static int ui_mouse (int, int, int, int);
static int keybuffer[SIZE];
static int introtext = 1;
static int begin = 0, end = 0;
static int resized;
static int callresize = 0;
char *interruptkeys = "z123456789rmfqe";
static int defformula[10];
static int defautopilot = 0;
static int defiters = -1;
static int defperiodicity = -1;
static tl_timer *maintimer;
static tl_timer *loopt;
static int defincoloring = -1;
static int maxframerate = 0;
static int letterspersec = 15;
static int defoutcoloring = -1;
static int defplane = -1;
static float defspeed = -1;
static char *defdriver = NULL;
static int todriver = 0;
static int defguessing = -1;
static int deflist;
static char *deflanguage = NULL;
static int printconfig;
static int printspeed;
static int fastmode = -1;
static int delaytime = 0;
static int defthreads = 0;
static char *loadfile = NULL;
static char *playfile = NULL;
static float defscreenwidth = 0.0, defscreenheight = 0.0, defpixelwidth = 0.0,
  defpixelheight = 0.0;
struct params global_params[] =
{
  {"-language", P_STRING, &deflanguage, "Select language catalog for tutorials"},
  {"-load", P_STRING, &loadfile, "Load possition"},
  {"-play", P_STRING, &playfile, "Play animation"},
  {"-delay", P_NUMBER, &delaytime, "Delay screen updates (milliseconds)."},
  {"-maxframerate", P_NUMBER, &maxframerate, "Maximal framerate (0 for unlimited - default)"},
  {"-letterspersec", P_NUMBER, &letterspersec, "Subtitles speed in tutorials (20 by default)"},
  {"", P_HELP, NULL, "Screen size options: \n\n  Knowedge of exact screen size makes random dot stereogram look better. \n  Also is used for choosing correct view area."},
  {"-screenwidth", P_FLOAT, &defscreenwidth, "exact size of screen in centimeters"},
  {"-screenheight", P_FLOAT, &defscreenheight, "exact size of screen in centimeters"},
  {"", P_HELP, NULL, "  Use this option in case you use some kind of virtual screen\n  or something similiar that confuses previous options"},
  {"-pixelwidth", P_FLOAT, &defpixelwidth, "exact size of one pixel in centimeters"},
  {"-pixelheight", P_FLOAT, &defpixelheight, "exact size of one pixel in centimeters"},
  {"", P_HELP, NULL, "Fractal formulas:"},
  {"-mandel", P_SWITCH, defformula, "Start with Mandelbrot set fractal(default)"},
  {"-mandel3", P_SWITCH, defformula + 1, "Start with Mandelbrot set power 3 fractal"},
  {"-mandel4", P_SWITCH, defformula + 2, "Start with Mandelbrot set power 4 fractal"},
  {"-mandel5", P_SWITCH, defformula + 3, "Start with Mandelbrot set power 5 fractal"},
  {"-mandel6", P_SWITCH, defformula + 4, "Start with Mandelbrot set power 6 fractal"},
  {"-octo", P_SWITCH, defformula + 5, "Start with Octal fractal"},
  {"-newton", P_SWITCH, defformula + 6, "Start with Newton fractal"},
  {"-barnsley", P_SWITCH, defformula + 7, "Start with Barnsley fractal"},
  {"-phoenix", P_SWITCH, defformula + 8, "Start with Phoenix fractal"},
  {"-magnet", P_SWITCH, defformula + 9, "Start with Magnet fractal"},
  {"", P_HELP, NULL, "Fractal oprions:"},
  {"-iter", P_NUMBER, &defiters, "Default number of iterations"},
  {"-incoloring", P_NUMBER, &defincoloring, "Set incoloring mode"},
  {"-outcoloring", P_NUMBER, &defoutcoloring, "Default outcoloringcoloring scheme"},
  {"-plane", P_NUMBER, &defplane, "Default plane"},
  {"-noperiodicity", P_SWITCH, &defperiodicity, "Disable periodicity checking"},
  {"-guessing", P_NUMBER, &defguessing, "Default solid guessing range"},
  {"-dynres", P_NUMBER, &fastmode, "Default dynamic resolution mode"},
  {"", P_HELP, NULL, "Parameters for ugly interface:"},
  {"-autopilot", P_SWITCH, &defautopilot, "Enable autopilot"},
  {"-speed", P_FLOAT, &defspeed, "Zooming speed (default is 1)"},
  {"-driver", P_STRING, &defdriver, "Select driver"},
  {"-list", P_SWITCH, &deflist, "List available drivers. Than exit"},
  {"-config", P_SWITCH, &printconfig, "Print configuration. Than exit"},
  {"-speedtest", P_SWITCH, &printspeed, "Test speed of caluclation loop. Than exit"},
#ifndef nthreads
  {"-threads", P_NUMBER, &defthreads, "Set number of threads (CPUs) to use"},
#else
  {"-threads", P_NUMBER, &defthreads, "Multiple CPUs supported - please recompile XaoS with threads enabled"},
#endif
  {NULL, 0, NULL, NULL}
};

static fractal_context *fcontext;
static struct palette *palette;
static struct image *image;
static uih_context *uih;
static struct ui_driver *driver;
static int ministatus = 0;
static char statustext[256];
static int menumax;
static void (*menucallback) (int);

static int numbertype = FORMULA;
static int helppage;
static void 
mousetype (int m)
{
  static int mouse;
  if (mouse != m)
    {
      mouse = m;
      if (driver->mousetype != NULL)
	driver->mousetype (m);
    }
}
static void 
ui_display (void)
{
  if (!uih->clearscreen)
    driver->display ();
  if (numbertype != FORMULA)
    uih_cycling_continue (uih);
  numbertype = FORMULA;
  if (ministatus)
    driver->print (0, 0, statustext);
  if (!(driver->flags & NOFLUSHDISPLAY))
    ui_flush ();
}
static float 
get_windowwidth (int width)
{
  if (defscreenwidth > 0.0 && driver->flags & RESOLUTION)
    return (defscreenwidth * width / driver->maxwidth);
  if (defscreenwidth > 0.0)
    return (defscreenwidth);
  if (defpixelwidth > 0.0)
    return (defpixelwidth * width);
  if (driver->flags & PIXELSIZE)
    return (driver->width * width);
  if (driver->flags & SCREENSIZE)
    return (driver->width);
  if (driver->flags & RESOLUTION)
    return (29.0 / driver->maxwidth * width);
  return (29.0);
}
static float 
get_windowheight (int height)
{
  if (defscreenheight > 0.0 && driver->flags & RESOLUTION)
    return (defscreenheight * height / driver->maxheight);
  if (defscreenheight > 0.0)
    return (defscreenheight);
  if (defpixelheight > 0.0)
    return (defpixelheight * height);
  if (driver->flags & PIXELSIZE)
    return (driver->height * height);
  if (driver->flags & SCREENSIZE)
    return (driver->height);
  if (driver->flags & RESOLUTION)
    return (21.0 / driver->maxheight * height);
  return (21.5);
}

extern int dynsize;
static void 
ui_outofmem (void)
{
  printf ("\n\nXaoS is out of memory.\n");
}

#define CHECKPROCESSEVENTS(b,k) assert(!((k)&~15)&&!((b)&~(BUTTON1|BUTTON2|BUTTON3)))
static int 
ui_passfunc (struct uih_context *c, int display, char *text, float percent)
{
  char str[80];
  int x, y, b, k;
  if (!uih->play)
    {
      if (display)
	ui_display ();
      sprintf (str, "%s %3.2f%%        ", text, percent);
    }
  driver->processevents (0, &x, &y, &b, &k);
  CHECKPROCESSEVENTS (b, k);
  if (k & 3)
    return (1);
  if (!c->interruptiblemode && !uih->play)
    {
      driver->print (0, height - textheight1, str);
      ui_flush ();
    }
  else
    {
      if (!(driver->flags & NOFLUSHDISPLAY))
	ui_flush ();
    }
  return (0);
}

static void 
ui_updatestatus (void)
{
  double times = (uih->fcontext->currentformula->v.rr) / (uih->fcontext->s.rr);
  double speed;

  if (!uih->clearscreen)
    driver->display ();
  if (numbertype != FORMULA)
    uih_cycling_continue (uih);
  numbertype = FORMULA;
  speed = uih_displayed (uih);

  sprintf (statustext, "%s %.2f times %2.2f frames/sec %c %i %i %i %i            ", times < 1 ? "unzoomed" : "zoomed", times < 1 ? 1.0 / times : times, speed, autopilot ? 'A' : ' ', uih->fcontext->coloringmode + 1, uih->fcontext->incoloringmode + 1, uih->fcontext->plane + 1, uih->fcontext->maxiter);
  if (ministatus)
    {
      driver->print (0, 0, statustext);
      ui_flush ();
    }
  else if (!(driver->flags & NOFLUSHDISPLAY))
    ui_flush ();
  STAT (printf ("framerate:%f\n", speed));
}
void 
ui_menu (char **text, char *label, int n, void (*callback) (int))
{
  int i;
  char s[50];
  if (driver->flags & PALETTE_REDISPLAYS)
    uih_cycling_stop (uih);
  numbertype = MENU;
  menumax = n;
  menucallback = callback;
  sprintf (s, "%-40s", label);
  driver->print (0, 0, s);
  for (i = 0; i < n; i++)
    {
      if (i < 10)
	sprintf (s, " %i - %-35s", i == 9 ? 0 : i + 1, text[i]);
      else
	sprintf (s, " %c - %-35s", 'A' + i - 10, text[i]);
      driver->print (0, (i + 1) * textheight1, s);
    }
  ui_flush ();
}
static void 
ui_menupress (int number)
{
  if (number >= menumax)
    return;
  uih->display = 1;
  numbertype = FORMULA;
  menucallback (number);
}

static void 
ui_fastmode (int mode)
{
  uih->fastmode = mode + 1;
}
static void 
ui_fastmode_menu (void)
{
  static char *fastmodes[] =
  {
    "dynamic resolution disabled",
    "use only during animation",
    "use also for new images",
  };
  ui_menu (fastmodes, "Please select drawing mode", 3, ui_fastmode);
}
static void 
ui_guessing (int mode)
{
  uih_setguessing (uih, mode + 1);
}
static int tutorialn;
static void 
ui_playtutorial (int n)
{
  char *name;
  xio_pathdata tmp;
  xio_file f = XIO_FAILED;
  static char *names[][10] =
  {
    {
      "cesky",
      "deutsch",
      "english",
      "francais",
    },
    {
      "fractal.xaf",
      "intro.xaf",
      "power.xaf",
      "newton.xaf",
      "barnsley.xaf",
      "phoenix.xaf",
      "octo.xaf",
      "magnet.xaf",
    },
    {
      "features.xaf",
      "outcolor.xaf",
      "incolor.xaf",
      "truecol.xaf",
      "filter.xaf",
      "plane.xaf",
      "anim.xaf",
      "pert.xaf",
      "palette.xaf",
      "other.xaf",
    },
    {
      "new30.xaf",
    }
  };
  name = names[tutorialn][n];
  if (!tutorialn)
    {
      uih_loadcatalog (uih, name);
      return;
    }
  f=xio_gettutorial(name,tmp);
  if (f == NULL)
    {
      uih->errstring = "Tutorial files not found. Reinstall XaoS";
      return;
    }
  driver->clrscr ();
  mousetype (WAITMOUSE);
  driver->print (0, 0, "Preparing first frame. Please wait");
  ui_flush ();
  uih_replayenable (uih, f, tmp);
}
static void 
ui_loadexample (void)
{
  xio_pathdata name;
  xio_file f=xio_getrandomexample(name);
  if (f == NULL)
    {
      uih->errstring = /*strerror (errno)*/"Could not open examples";
      return;
    }
  uih_load (uih, f, name);
}
static int 
ui_loadfile (xio_path name)
{
  xio_file f;
  f = xio_ropen (name);
  if (f == NULL)
    {
      return 0;
    }
  uih_load (uih, f, name);
  return 1;
}
static void 
ui_load2 (void)
{
  if (!ui_loadfile (inputtext[0]))
    {
      char s[256];
      sprintf (s, "%s.xpf", inputtext[0]);
      if (!ui_loadfile (s))
	{
	  sprintf (s, "fract%s.xpf", inputtext[0]);
	  if (!ui_loadfile (s))
	    {
#ifdef _plan9_
	      uih->errstring = "Load failed";
#else
	      uih->errstring = strerror (errno);
#endif
	      return;
	    }
	}
    }
}
static void 
ui_play2 (void)
{
  if (!uih_replayenable (uih,xio_ropen(inputtext[0]),inputtext[0]))
    {
      char s[256];
      uih->errstring=NULL;
      sprintf (s, "%s.xaf", inputtext[0]);
      if (!uih_replayenable (uih,xio_ropen(s),s))
	{
	  sprintf (s, "anim%s.xaf", inputtext[0]);
          uih->errstring=NULL;
	  if (!uih_replayenable (uih,xio_ropen(s),s)) return;
	}
    }
}
static void 
ui_input (int ml, int shift, void (*handler) (void))
{
  uih_stoptimers (uih);
  inputhandler = handler;
  numbertype = INPUT;
  mlines = ml;
  nlines = 1;
  inputtext[0][0] = 0;
  line = 0;
  textstart = shift;
}
static void 
ui_load (void)
{
  driver->print (0, 0, "Enter filename (or number)");
  ui_flush ();
  ui_input (0, 1, ui_load2);
}
static void 
ui_play (void)
{
  driver->print (0, 0, "Enter filename (or number)");
  ui_flush ();
  ui_input (0, 1, ui_play2);
}
static void 
ui_tutorial (int n)
{
  int num;
  static char *tutorials[][11] =
  {
    {
      "Cesky",
      "Deutsch",
      "English",
      "Francais",
      NULL
    },
    {
      "Whole story",
      "Introduction and the Mandelbrot set",
      "Higher power Mandelbrots",
      "Newton's method",
      "Barnsley's formula",
      "Phoenix",
      "Octo",
      "Magnet",
      NULL
    },
    {
      "All features",
      "Outcoloring modes",
      "Incoloring modes",
      "True-color coloring modes",
      "Filters",
      "Planes",
      "Animations and possition files",
      "Perturbation",
      "Random palettes",
      "Other noteworthly features"
    },
    {
      "Whats new in 3.0?"
    },
  };
  tutorialn = n;
  ui_display ();
  for (num = 0; tutorials[n][num] != NULL; num++);
  ui_menu (tutorials[n], "Please select chapter", num, ui_playtutorial);
}
static void 
ui_tutorial_menu (void)
{
  static char *tutorials[] =
  {
    "Select language for tutorials",
    "An introduction to fractals",
    "XaoS features overview",
    "Whats new?",
  };
  ui_display ();
  ui_menu (tutorials, "Please select tutorial", (int) (sizeof (tutorials) / sizeof (char *)), ui_tutorial);
}
static void 
ui_guessing_menu (void)
{
  static char *guessmodes[] =
  {
    "solid guessing disabled",
    "guess maximally 2x2 rectangles",
    "guess maximally 3x3 rectangles",
    "guess maximally 4x4 rectangles",
    "guess maximally 5x5 rectangles",
    "guess maximally 6x6 rectangles",
    "guess maximally 7x7 rectangles",
    "guess maximally 8x8 rectangles",
  };
  ui_menu (guessmodes, "Please select solid guessing mode", 8, ui_guessing);
}

static void 
ui_outcoloringmode (int mode)
{
  if (uih->fcontext->coloringmode == 10)
    {
      if (mode == TCOLOR)
	uih_setoutcoloringmode (uih, 0);
      else
	uih_setouttcolor (uih, mode);
    }
  else
    uih_setoutcoloringmode (uih, mode);
}
static void 
ui_outcoloringmode_menu (void)
{
  if (uih->fcontext->coloringmode != 10)
    ui_menu (outcolorname, "Please select outcoloring mode", OUTCOLORING, ui_outcoloringmode);
  else
    ui_menu (tcolorname, "Please select truecolor outcoloring mode", TCOLOR + 1, ui_outcoloringmode);
}
static void 
ui_incoloringmode (int mode)
{
  if (uih->fcontext->incoloringmode == 10)
    {
      if (mode == TCOLOR)
	uih_setincoloringmode (uih, 0);
      else
	uih_setintcolor (uih, mode);
    }
  else
    uih_setincoloringmode (uih, mode);
}
static void 
ui_incoloringmode_menu (void)
{
  if (uih->fcontext->incoloringmode != 10)
    ui_menu (incolorname, "Please select incoloring mode", INCOLORING, ui_incoloringmode);
  else
    ui_menu (tcolorname, "Please select truecolor incoloring mode", TCOLOR + 1, ui_incoloringmode);
}
static void 
ui_periodicity (void)
{
  ui_updatestatus ();
  uih_setperiodicity (uih, fcontext->periodicity ^ 1);
  if (fcontext->periodicity)
    driver->print (0, 0, "Periodicity checking enabled. ");
  else
    driver->print (0, 0, "Periodicity checking disabled.");
  ui_flush ();
}
xio_path 
ui_getfile (char *basename, char *extension)
{
  return(xio_getfilename(basename,extension));
}


static void 
ui_savepngfile (void)
{
  char str[256];
  xio_path name = ui_getfile ("fract", ".png");
  char *s;

  driver->print (0, 0, "Writing png image..  ");
  ui_flush ();
  s = uih_save (uih, name);
  ui_updatestatus ();
  if (s != NULL)
    driver->print (0, 0, s);
  else
    {
      sprintf (str, "File %s saved", name);
      driver->print (0, 0, str);
      ui_flush ();
    }
  uih->display = 0;
}
static void 
ui_savepos (int mode)
{
  char str[256];
  xio_path name;
  xio_file f;

  driver->print (0, 0, "Writing possition..  ");
  ui_flush ();
  if (mode == 0)
    name = ui_getfile ("fract", ".xpf");
  else
    name = configfile;
  f = xio_wopen (name);
  if (f == XIO_FAILED)
    {
      ui_updatestatus ();
      driver->print (0, 0, "Can not open file");
      ui_flush ();
      uih->display = 0;
      return;
    }
  uih_save_possition (uih, f, mode ? UIH_SAVEALL : UIH_SAVEPOS);
  ui_updatestatus ();
  if (uih->errstring != NULL)
    driver->print (0, 0, uih->errstring), uih->errstring = NULL;
  else
    {
      sprintf (str, "File %s saved", name);
      driver->print (0, 0, str);
    }
  ui_flush ();
  numbertype = FORMULA;
  uih->display = 0;
}
static void 
ui_saveanimation (void)
{
  char str[256];
  xio_path name;
  xio_file f;

  driver->print (0, 0, "Openning animation file..  ");
  ui_flush ();
  name = ui_getfile ("anim", ".xaf");
  f = xio_wopen (name);
  if (f == XIO_FAILED)
    {
      ui_updatestatus ();
      driver->print (0, 0, "Can not open file");
      uih->display = 0;
      ui_flush ();
      return;
    }
  uih_save_enable (uih, f, UIH_SAVEANIMATION);
  ui_updatestatus ();
  if (uih->errstring != NULL)
    driver->print (0, 0, uih->errstring), uih->errstring = NULL;
  else
    {
      sprintf (str, "Recording into %s enabled", name);
      driver->print (0, 0, str);
    }
  ui_flush ();
  numbertype = FORMULA;
  uih->display = 0;
}
static void 
ui_waitforcomplette (void)
{
  while (uih_needrecalculate (uih))
    {
      uih_stoptimers (uih);
      if (uih->display)
	ui_updatestatus ();
    }
}
static void 
ui_text (void)
{
  char text[XTEXTMAX * YTEXTMAX];
  int i;
  ui_waitforcomplette ();
  text[0] = 0;
  for (i = 0; i < nlines; i++)
    {
      strcat (text, inputtext[i]);
      if (i != nlines - 1)
	strcat (text, "\n");
    }
  uih_text (uih, text);
}
static void 
ui_save (int mode)
{
  if (uih->play) {
    switch (mode)
      {
      case 0:
	ui_savepngfile ();
	break;
      case 1:
	ui_savepos (0);
	break;
      case 2:
	ui_savepos (1);
	break;
      }
    return;
  }
  if (!uih->save)
    switch (mode)
      {
      case 0:
	ui_savepngfile ();
	break;
      case 1:
	ui_savepos (0);
	break;
      case 2:
	ui_saveanimation ();
	break;
      case 3:
	ui_load ();
	break;
      case 4:
	ui_play ();
	break;
      case 5:
	ui_loadexample ();
	break;
      case 6:
	ui_savepos (1);
	break;
      }
  else
    switch (mode)
      {
      case 0:
	ui_savepngfile ();
	break;
      case 1:
	uih_save_disable (uih);
	driver->print (0, 0, "Recording disabled");
	ui_flush ();
	break;
      case 2:
	uih_clearscreen (uih);
	break;
      case 3:
	ui_updatestatus ();
	driver->print (0, 0, "Enter text, <cr><.><cr> to save");
	ui_flush ();
	uih->display = 0;
	ui_input (1, 1, ui_text);
	break;
      }
}
static void 
ui_save_menu (void)
{
  static char *guessmodes[] =
  {
    "Save image into PNG file",
    "Save possition",
    "Record animation",
    "Load file",
    "Play animation",
    "Load random example",
    "Use current settings as default",
  };
  static char *guessmodes2[] =
  {
    "Save image into PNG file",
    "Finish animation recording",
    "Clear screen",
    "Display text",
  };
  static char *guessmodes3[] =
  {
    "Save image into PNG file",
    "Save possition",
    "Use current settings as default",
  };
  if (uih->play)
    ui_menu (guessmodes3, "Please select format", 3, ui_save);
  else
  if (!uih->save)
    ui_menu (guessmodes, "Please select format", 7, ui_save);
  else
    ui_menu (guessmodes2, "Please select format", 4, ui_save);
}


void 
ui_tbreak (void)		/*FIXME should go away. But is still used by drivers */
{
  /*uih_tbreak(uih); */
  uih_stoptimers (uih);
}

static void 
ui_plane (int mode)
{
  uih_setplane (uih, mode);
}
static void 
ui_driver_menu (void)
{
  int i;
  char s[80];
  if (driver->flags & PALETTE_REDISPLAYS)
    uih_cycling_stop (uih);
  if (ndrivers != 1)
    {
      driver->print (0, 0, "Please select driver:                        ");
      driver->print (0, textheight1, "Warning..Xaos Can crash during this operation");
      driver->print (0, 2 * textheight1, "Palette will be set to default               ");
      driver->print (0, 3 * textheight1, " 1 - abort action                            ");
      for (i = 0; i < ndrivers; i++)
	{
	  sprintf (s, " %i - %-40s", i + 2, drivers[i]->name);
	  driver->print (0, (i + 4) * textheight1, s);
	}
      numbertype = DRIVER;
    }
  else
    driver->print (0, 0, "Only one driver available-function disabled");
  ui_flush ();
}
static void 
ui_filter_menu (void)
{
  int i;
  char s[80];
  if (driver->flags & PALETTE_REDISPLAYS)
    uih_cycling_stop (uih);
  for (i = 0; i < uih_nfilters; i++)
    {
      sprintf (s, "%i %-23s - %s", i + 1, uih_filters[i]->name, uih->filter[i] != NULL ? "enabled" : "disabled");
      driver->print (0, i * textheight1, s);
      ui_flush ();
    }
  numbertype = FILTER;
}
static void 
ui_plane_menu (void)
{
  ui_menu (planename, "Please select display plane:", PLANES, ui_plane);
}

static void 
ui_mandelbrot (int mousex, int mousey)
{
  uih_setmandelbrot (uih, fcontext->mandelbrot ^ 1, mousex, mousey);
}

static void 
ui_autopilot (void)
{
  autopilot ^= 1;
  if (autopilot)
    uih_autopilot_on (uih);
  else
    uih_autopilot_off (uih);
}


static void 
ui_help (int number)
{
  int nlines = height / textheight1 - 7;
  int i, page;
  int npages = (UGLYTEXTSIZE + driver->helpsize + nlines - 1) / (nlines ? nlines : 1);
  char str[80];
  if (nlines <= 0)
    return;
  if (!number)
    helppage--;
  if (number == 1)
    helppage++;
  if (helppage < 0)
    helppage = 0;
  if (helppage >= npages)
    helppage = npages - 1;
  page = helppage;
  if (driver->flags & PALETTE_REDISPLAYS)
    uih_cycling_stop (uih);
  sprintf (str, "------> Ugly help <-----> %2i/%2i <------", page + 1, npages);
  driver->print (0, textheight1, "                                       ");
  driver->print (0, 0, str);
  for (i = 0; i < nlines; i++)
    {
      if (page * nlines + i < (int) UGLYTEXTSIZE)
	driver->print (0, (i + 2) * textheight1, helptext[page * nlines + i]);
      else
	{
	  if (page * nlines + i < (int) UGLYTEXTSIZE + driver->helpsize)
	    driver->print (0, (i + 2) * textheight1, driver->help[page * nlines + i - UGLYTEXTSIZE]);
	  else
	    driver->print (0, (i + 2) * textheight1, "~                                      ");
	}
    }
  driver->print (0, (nlines + 2) * textheight1, "                                       ");
  driver->print (0, (nlines + 3) * textheight1, "Hyperuglytext browser (tm) version 1.0 ");
  driver->print (0, (nlines + 4) * textheight1, "Press '1' for previous and '2' for next");
  driver->print (0, (nlines + 5) * textheight1, "             space for exit            ");
  ui_flush ();
  numbertype = HELP;
}


void 
ui_status (void)
{
  char str[6000];
  sprintf (str, "Fractal name:%s", fcontext->currentformula->name[!fcontext->mandelbrot]);
  driver->print (0, textheight1, str);
  sprintf (str, "Fractal type:%s", fcontext->mandelbrot ? "Mandelbrot" : "Julia");
  driver->print (0, 2 * textheight1, str);
  sprintf (str, "View:[%1.12f,%1.12f]", (double) fcontext->s.cr, (double) fcontext->s.ci);
  driver->print (0, 3 * textheight1, str);
  sprintf (str, "size:[%1.12f,%1.12f]", (double) fcontext->s.rr, (double) fcontext->s.ri);
  driver->print (0, 4 * textheight1, str);
  sprintf (str, "Rotation:%4.2f   Screen size:%4i:%-4i", (double) fcontext->angle, width, height);
  driver->print (0, 5 * textheight1, str);
  sprintf (str, "Iterations:%4i", fcontext->maxiter);
  driver->print (0, 6 * textheight1, str);
  sprintf (str, "Autopilot:%-4s    Plane:%s", uih->autopilot ? "On" : "Off", planename[fcontext->plane]);
  driver->print (0, 7 * textheight1, str);
  sprintf (str, "zoomspeed:%f   Palette size:%i", (float) uih->maxstep * 1000, uih->image->palette->size);
  driver->print (0, 8 * textheight1, str);
  sprintf (str, "incoloring:%s outcoloring:%s", incolorname[fcontext->incoloringmode], outcolorname[fcontext->coloringmode]);
  driver->print (0, 9 * textheight1, str);
  if (fcontext->mandelbrot)
    strcpy (str, "Parameter:none");
  else
    sprintf (str, "Parameter:[%f,%f]", (float) fcontext->pre, (float) fcontext->pim);
  driver->print (0, 10 * textheight1, str);
  ui_flush ();
}

void 
ui_message (void)		/*FIXME kept for compatibility */
{
  char s[80];
  if (uih->play)
    return;
  mousetype (WAITMOUSE);
  maxiter = fcontext->maxiter;
  sprintf (s, "Please wait while calculating %s", fcontext->currentformula->name[!fcontext->mandelbrot]);
  driver->print (0, 0, s);
  ui_status ();
}
void 
ui_message2 (struct uih_context *u)
{
  ui_message ();
}

#define ROTATESPEEDUP 30
static int 
ui_mouse (int mousex, int mousey, int mousebuttons, int iterchange)
{
  int inmovement = 0;
  static int dirty = 0;
  static int iterstep = 1;
  static int lastiter;
  char str[80];

  tl_update_time ();
  CHECKPROCESSEVENTS (mousebuttons, iterchange);
  if (uih->play)
    {
      if (iterchange == 2 && tl_lookup_timer (maintimer) > 2 * FRAMETIME)
	{
	  tl_reset_timer (maintimer);
	  uih->letterspersec++;
	  sprintf (str, "Letters per second:%i ", uih->letterspersec);
	  driver->print (0, 0, str);
	  ui_flush ();
	  return 1;
	}
      if (iterchange == 1 && tl_lookup_timer (maintimer) > 2 * FRAMETIME)
	{
	  tl_reset_timer (maintimer);
	  uih->letterspersec--;
	  sprintf (str, "Letters per second:%i ", uih->letterspersec);
	  driver->print (0, 0, str);
	  ui_flush ();
	  return 1;
	}
      uih_update (uih, mousex, mousey, mousebuttons);
      return 0;
    }
  if (numbertype == HELP)
    {
      if ((iterchange & 5) && !lastiter)
	{
	  ui_help (0);
	}
      if ((iterchange & 10) && !lastiter)
	{
	  ui_help (1);
	}
      lastiter = iterchange;
      return 0;
    }
  if (!uih->cycling)
    {
      if (uih->rotatemode == ROTATE_CONTINUOUS)
	{
	  if (iterchange == 2)
	    {
	      uih->rotationspeed += ROTATESPEEDUP * tl_lookup_timer (maintimer) / 1000000.0;
	      sprintf (str, "Rotation speed:%2.2f degrees per second ", (float) uih->rotationspeed);
	      driver->print (0, 0, str);
	      ui_flush ();
	    }
	  if (iterchange == 1)
	    {
	      uih->rotationspeed -= ROTATESPEEDUP * tl_lookup_timer (maintimer) / 1000000.0;
	      sprintf (str, "Rotation speed:%2.2f degrees per second ", (float) uih->rotationspeed);
	      driver->print (0, 0, str);
	      ui_flush ();
	    }
	  tl_reset_timer (maintimer);
	}
      else
	{
	  if (iterchange == 2 && tl_lookup_timer (maintimer) > FRAMETIME)
	    {
	      tl_reset_timer (maintimer);
	      maxiter += iterstep;
	      if (!(fcontext->maxiter % 10))
		iterstep = 10;
	      dirty = 1;
	      sprintf (str, "Iterations:%i ", maxiter);
	      driver->print (0, 0, str);
	      ui_flush ();
	      return 1;
	    }
	  if (iterchange == 1 && tl_lookup_timer (maintimer) > FRAMETIME)
	    {
	      tl_reset_timer (maintimer);
	      maxiter -= iterstep;
	      if (!(fcontext->maxiter % 10))
		iterstep = 10;
	      dirty = 1;
	      if (maxiter < 1)
		{
		  maxiter = 1;
		}
	      sprintf (str, "Iterations:%i ", maxiter);
	      driver->print (0, 0, str);
	      ui_flush ();
	      return 1;
	    }
	  if (!iterchange || iterchange == 3)
	    iterstep = 1;
	  if (iterchange & 3)
	    return 1;
	}
    }
  if (dirty)
    uih_setmaxiter (uih, maxiter), dirty = 0;
  if (numbertype == FORMULA)
    /*inmovement|= */ uih_update (uih, mousex, mousey, mousebuttons);
  if (uih->cycling)
    {
      if (iterchange & 1 && (tl_lookup_timer (maintimer) > FRAMETIME || mousebuttons))
	{
	  uih->cyclingspeed--;
	  if (!uih->cyclingspeed)
	    uih->cyclingspeed = -1;
	  tl_reset_timer (maintimer);
	  sprintf (str, "cyclingspeed:%i ", uih->cyclingspeed);
	  uih_setcycling (uih, uih->cyclingspeed);
	  driver->print (0, 0, str);
	  ui_flush ();
	}
      if (iterchange & 2 && (tl_lookup_timer (maintimer) > FRAMETIME || mousebuttons))
	{
	  uih->cyclingspeed++;
	  if (!uih->cyclingspeed)
	    uih->cyclingspeed = 1;
	  tl_reset_timer (maintimer);
	  sprintf (str, "cyclingspeed:%i ", uih->cyclingspeed);
	  uih_setcycling (uih, uih->cyclingspeed);
	  driver->print (0, 0, str);
	  ui_flush ();
	}
    }
  if (iterchange & 4 && (tl_lookup_timer (maintimer) > FRAMETIME || mousebuttons))
    {
      double mul1 = (double) tl_lookup_timer (maintimer) / FRAMETIME;
      double su = 1 + (SPEEDUP - 1) * mul1;
      if (su > 2 * SPEEDUP)
	su = SPEEDUP;
      tl_reset_timer (maintimer);
      uih->speedup *= su, uih->maxstep *= su;
      sprintf (str, "speed:%2.2f ", (double) uih->speedup * (1.0 / STEP));
      driver->print (0, 0, str);
      ui_flush ();
    }
  if (iterchange & 8 && (tl_lookup_timer (maintimer) > FRAMETIME || mousebuttons))
    {
      double mul1 = (double) tl_lookup_timer (maintimer) / FRAMETIME;
      double su = 1 + (SPEEDUP - 1) * mul1;
      if (su > 2 * SPEEDUP)
	su = SPEEDUP;
      tl_reset_timer (maintimer);
      uih->speedup /= su, uih->maxstep /= su;
      sprintf (str, "speed:%2.2f ", (double) uih->speedup * (1 / STEP));
      driver->print (0, 0, str);
      ui_flush ();
    }
  if (iterchange & 15)
    inmovement = 1;
  lastiter = iterchange;
  return (inmovement || (autopilot && numbertype == FORMULA) || inmovement);
}
void 
ui_call_resize (void)
{
  callresize = 1;
}
static int 
ui_alloccolor (struct palette *pal, int init, int r, int g, int b)
{
  int i;
  i = driver->set_color (r, g, b, init);
  if (i == -1)
    return (-1);
  if (init)
    pal->size = 0;
  pal->pixels[pal->size] = i;
  pal->rgb[i][0] = r;
  pal->rgb[i][1] = g;
  pal->rgb[i][2] = b;
  pal->size++;
  return (i);
}
static void 
ui_setpalette (struct palette *pal, int start, int end, rgb_t * rgb1)
{
  driver->set_range ((ui_palette) rgb1, start, end);
}
static void 
ui_flip (struct image *image)
{
  flipgeneric (image);
  driver->flip_buffers ();
}

void 
ui_resize (void)
{
  int w, h, scanline;
  char *b1, *b2;
  if (uih->incalculation)
    {
      resized = 1;
      uih_interrupt (uih);
      return;
    }
  uih_stoptimers (uih);
  uih_cycling_stop (uih);
  uih_savepalette (uih);
  driver->getsize (&w, &h);
  assert (w > 0 && w < 65000 && h > 0 && h < 65000);
  if (w != width || h != height || (driver->flags & UPDATE_AFTER_RESIZE))
    {
      driver->free_buffers (NULL, NULL);
      width = w;
      height = h;
      if (!(scanline = driver->alloc_buffers (&b1, &b2)))
	{
	  driver->uninit ();
	  printf ("Can not allocate buffers\n");
	  ui_outofmem ();
	  exit (-1);
	}
      destroy_image (image);
      destroypalette (palette);
      palette = createpalette (driver->palettestart, driver->paletteend, driver->imagetype, (driver->flags & RANDOM_PALETTE_SIZE) ? UNKNOWNENTRIES : 0, driver->maxentries, driver->set_color != NULL ? ui_alloccolor : NULL, driver->set_range != NULL ? ui_setpalette : NULL, NULL, NULL);
      if (!palette)
	{
	  driver->uninit ();
	  printf ("Can not create palette\n");
	  ui_outofmem ();
	  exit (-1);
	}
      image = create_image_cont (width, height, scanline, 2, (unsigned char *) b1, (unsigned char *) b2, palette, ui_flip, 0, get_windowwidth (width) / width, get_windowheight (height) / height);
      if (!image)
	{
	  driver->uninit ();
	  printf ("Can not create image\n");
	  ui_outofmem ();
	  exit (-1);
	}
      if (!uih_updateimage (uih, image))
	{
	  driver->uninit ();
	  printf ("Can not allocate tables\n");
	  ui_outofmem ();
	  exit (-1);
	}
      tl_process_group (syncgroup, NULL);
      tl_reset_timer (maintimer);
      uih_newimage (uih);
    }
  uih_restorepalette (uih);
  /*uih_mkdefaultpalette(uih); */
  uih->display = 1;;
  uih_cycling_continue (uih);
}
static void 
ui_driver (int d)
{
  int scanline;
  char *b1, *b2;
  struct ui_driver *driver1;
#ifdef DESTICKY
  seteuid (euid);		/* We need supervisor rights for a moment. */
  setegid (egid);
#endif
  if (d < 0)
    d = 0;
  if (d >= ndrivers)
    d = ndrivers - 1;
  uih_stoptimers (uih);
  driver1 = driver;
  uih_cycling_off (uih);
  uih_savepalette (uih);
  driver->free_buffers (NULL, NULL);
  driver->uninit ();
  driver = drivers[d];
  if (!driver->init ())
    {
      driver = driver1;
      printf ("Can not initialize new driver\n");
      if (!driver1->init ())
	{
	  printf ("Can not return back to previous driver\n");
	  exit (-1);
	}
      else
	driver = driver1;
    }
  driver->getsize (&width, &height);

  if (!(scanline = driver->alloc_buffers (&b1, &b2)))
    {
      driver->uninit ();
      printf ("Can not allocate buffers\n");
      ui_outofmem ();
      exit (-1);
    }
  destroy_image (image);
  destroypalette (palette);
  palette = createpalette (driver->palettestart, driver->paletteend, driver->imagetype, (driver->flags & RANDOM_PALETTE_SIZE) ? UNKNOWNENTRIES : 0, driver->maxentries, driver->set_color != NULL ? ui_alloccolor : NULL, driver->set_range != NULL ? ui_setpalette : NULL, NULL, NULL);
  if (!image)
    {
      driver->uninit ();
      printf ("Can not create palette\n");
      ui_outofmem ();
      exit (-1);
    }
  image = create_image_cont (width, height, scanline, 2, (unsigned char *) b1, (unsigned char *) b2, palette, ui_flip, 0, get_windowwidth (width) / width, get_windowheight (height) / height);
  if (!image)
    {
      driver->uninit ();
      printf ("Can not create image\n");
      ui_outofmem ();
      exit (-1);
    }
  if (!uih_updateimage (uih, image))
    {
      driver->uninit ();
      printf ("Can not allocate tables\n");
      ui_outofmem ();
      exit (-1);
    }
  tl_process_group (syncgroup, NULL);
  tl_reset_timer (maintimer);
  uih->display = 1;
  uih_newimage (uih);
  uih_restorepalette (uih);
  uih_do_fractal (uih);
  ui_updatestatus ();
#ifdef DESTICKY
  seteuid (getuid ());		/* Don't need supervisor rights anymore. */
  setegid (getgid ());
#endif
}
static void 
processbuffer (void)
{
  int k;
  for (; begin != end % SIZE;)
    {
      if (resized)
	resized = 0, ui_resize ();
      k = keybuffer[begin];
      begin = (begin + 1) % SIZE;
      ui_key (k);
    }
}
static void 
displaytext (void)
{
  int i;
  char str[XTEXTMAX + 2];
  for (i = 0; i < nlines + 2; i++)
    {
      memset (str, ' ', XTEXTMAX + 1);
      str[XTEXTMAX] = 0;
      if (line == i)
	sprintf (str, "%s_", inputtext[i]);
      else
	sprintf (str, "%s", inputtext[i]);
      str[strlen (str)] = ' ';
      driver->print (0, (i + textstart) * textheight1, str);
      ui_flush ();
    }
}
void 
ui_quit (int quit)
{
  if (!quit)
    {
      uih_cycling_off (uih);
      uih_freecontext (uih);
      tl_free_timer (maintimer);
      tl_free_timer (loopt);
      driver->free_buffers (NULL, NULL);
      driver->uninit ();
      destroy_image (image);
      destroypalette (palette);
      uih_freecatalog (uih);
      xth_uninit ();
      printf ("Thank you for using XaoS\n");
      exit (0);
    }
}
static void 
ui_various (int m)
{
  ui_updatestatus ();
  switch (m)
    {
    case 0:
      ui_periodicity ();
      break;
    case 1:
      ui_guessing_menu ();
      break;
    case 2:
      ui_fastmode_menu ();
      break;
    case 3:
      ui_driver_menu ();
      break;
    case 4:
      driver->driveropt ();
      break;
    }
}

static void 
ui_various_menu (void)
{
  static char *menu[] =
  {
    "Periodicity check",
    "Guessing options",
    "Dynamic resolution options",
    "Change driver",
    "Driver depended functions",
  };
  ui_menu (menu, "Please select format", sizeof (menu) / sizeof (char *) - (driver->driveropt == NULL), ui_various);
}

int 
ui_key (int key)
{
  int sym;
  int lasttype = numbertype;
  if (numbertype == INPUT)
    {				/*Text input routines */
      switch (key)
	{
	case 13:
	case '\n':
	  if (mlines)
	    {
	      if (inputtext[line][0] == '.' && inputtext[line][1] == 0)
		{
		  numbertype = FORMULA;
		  inputhandler ();
		  return 0;
		}
	      else
		{
		  line++;
		  if (line >= YTEXTMAX)
		    line = YTEXTMAX - 1;
		  else if (line > nlines)
		    nlines++;
		  inputtext[line][0] = 0;
		}
	    }
	  else
	    {
	      numbertype = FORMULA;
	      inputhandler ();
	      return 0;
	    }			/*mlines */
	  break;
	case 8:
	  {
	    int len = strlen (inputtext[line]);
	    if (len)
	      inputtext[line][len - 1] = 0;
	    else if (nlines && line)
	      {
		inputtext[line][0] = 0, line--;
		nlines--;
	      }
	  }
	  break;
	default:
	  {
	    int len = strlen (inputtext[line]);
	    if (isprint (key) && len < XTEXTMAX - 2)
	      inputtext[line][len] = key, inputtext[line][len + 1] = 0;
	  }
	  break;
	}			/*switch key */
      displaytext ();
      return 0;
    }				/*numbertype==INPUT */
  if (uih->play)
    {
      int number;
      switch (sym = tolower (key))
	{
	case 'q':
	  uih_replaydisable (uih);
	  break;
	case 's':
	  if(uih->incalculation) {add_buffer(key);} else
	  ui_save_menu();
	  break;
	case ' ':
	  if(uih->incalculation)
	  ui_updatestatus(); else {
	  uih_skipframe (uih);
	  driver->print (0, 0, "Skipping, please wait...");
	  }
	  break;
	case 'z':
	  if(uih->incalculation)
	  uih_interrupt (uih);
	  break;
	case 'l':
	  ministatus ^= 1;
	  ui_display ();
	  break;
        case '=':
	  if(uih->incalculation) {add_buffer(key);} else
	  callresize = 1;
	  break;
	default:
	  if (sym >= '0' && sym <= '9')
	    {
	      number = sym - '1';
	      if (number < 0)
		number = 9;
	      if(numbertype==MENU)
		{
		  ui_menupress (number);
		  break;
		}
	    }
	}
      return 0;
    }
  if (uih->incalculation)
    {
      int i;
      if (tolower (key) == ' ')
	ui_updatestatus ();
      else if (tolower (key) == 'z')
	uih_interrupt (uih);
      else
	{
	  add_buffer (key);
	  for (i = 0; i < (int) strlen (interruptkeys); i++)
	    {
	      if (tolower (key) == interruptkeys[i])
		uih_interrupt (uih);
	    }
	}
      return 1;
    }
  if (numbertype == MENU && tolower (key) >= 'a' && tolower (key) - 'a' < menumax - 10)
    {
      ui_menupress (tolower (key) - 'a' + 10);
    }
  else
    switch (sym = tolower (key))
      {
      case 'o':
	if (uih->fastrotate && uih->rotatemode == ROTATE_MOUSE)
	  uih_fastrotate (uih, 0);
	else if (!uih->fastrotate)
	  uih_fastrotate (uih, !uih->fastrotate),
	    uih_rotatemode (uih, ROTATE_CONTINUOUS);
	else
	  uih_rotatemode (uih, ROTATE_MOUSE);
	break;
      case 'y':
	if (uih->cycling)
	  uih_cycling_off (uih);
	else
	  {
	    if (!uih_cycling_on (uih))
	      driver->print (0, 0, "Inicialization of color cycling failed. Try to enable palette emulation filter");
	    ui_flush ();
	  }
	break;
      case 'j':
	if (uih->juliamode)
	  uih_disablejulia (uih);
	else
	  {
	    uih_enablejulia (uih);
	  }
	break;
      case 'v':
	ui_various_menu ();
	break;
      case '=':
	callresize = 1;
	break;
      case 'm':
	{
	  int mousex, mousey, b;
	  driver->getmouse (&mousex, &mousey, &b);
	  ui_mandelbrot (mousex, mousey);
	}
	break;
      case 'b':
	{
	  int mousex, mousey, b;
	  driver->getmouse (&mousex, &mousey, &b);
	  if (uih->fcontext->bre || uih->fcontext->bim)
	    uih_setperbutation (uih, 0.0, 0.0);
	  else
	    uih_perbutation (uih, mousex, mousey);
	}
	break;
      case 'i':
	ui_plane_menu ();
	break;
      case 'f':
	ui_incoloringmode_menu ();
	break;
      case 'c':
	ui_outcoloringmode_menu ();
	break;
      case 'h':
	if (numbertype == HELP)
	  ui_tutorial_menu ();
	else
	  ui_help (5);
	break;
      case 's':
	ui_save_menu ();
	break;
      case '?':
      case '/':
	ui_status ();
	break;
      case 'r':
	fcontext->version++;
	uih_newimage (uih);
	break;
      case 'p':
	uih_mkpalette (uih);
	if (driver->flags & UPDATE_AFTER_PALETTE)
	  {
	    uih->display = 1;
	  }
	break;
      case 'd':
	uih_mkdefaultpalette (uih);
	if (driver->flags & UPDATE_AFTER_PALETTE)
	  {
	    uih->display = 1;
	  }
	break;
      case 'l':
	ministatus ^= 1;
	ui_display ();
	break;
      case 'e':
	ui_filter_menu ();
	break;
      case 'a':
	ui_autopilot ();
	return (1);
	break;
      case 'q':
	if (numbertype == MENU && menucallback == ui_quit)
	  ui_quit (0);
	if (numbertype == FORMULA)
	  {
	    static char *quittext[] =
	    {"Yes I want quit",
	     "Not yet"};
	    ui_menu (quittext, "Quit ceremony:", 2, ui_quit);
	    break;
	  }
      case ' ':
	uih_display (uih);
	ui_updatestatus ();
	ui_flush ();
	break;
      default:
	{
	  int number;
	  if (sym >= '0' && sym <= '9')
	    {
	      number = sym - '1';
	      if (number < 0)
		number = 9;
	      switch (numbertype)
		{
		case FORMULA:
		  set_formula (fcontext, number);
		  uih_newimage (uih);
		  break;
		case HELP:
		  ui_help (number);
		  break;
		case MENU:
		  ui_menupress (number);
		  break;
		case DRIVER:
		  if (number)
		    todriver = number;
		  else
		    ui_display ();
		  return 2;
		  break;
		case FILTER:
		  if (number < uih_nfilters)
		    {
		      if (uih->filter[number] != NULL)
			uih_disablefilter (uih, number);
		      else
			uih_enablefilter (uih, number);
		    }
		  numbertype = FORMULA;
		  break;
		}
	    }
	}
	break;
      }
  if (numbertype != FORMULA && lasttype == FORMULA)
    uih_stopzooming (uih);
  processbuffer ();
  return 0;
}
static void 
displayui (void)
{
  int i, l = strlen (uih->text);
  int nlines = 0;
  int pos = 0;
  for (i = 0; i < l; i++)
    if (uih->text[i] == '\n')
      uih->text[i] = 0, nlines++;
  nlines++;
  for (i = 0; i < nlines; i++)
    {
      int x = 0, y = 0, l;
      l = strlen (uih->text + pos);
      switch (uih->xtextpos)
	{
	case 0:
	  x = 0;
	  break;
	case 1:
	  x = (uih->image->width - textwidth1 * l) / 2;
	  break;
	case 2:
	  x = uih->image->width - textwidth1 * l;
	  break;
	}
      switch (uih->ytextpos)
	{
	case 0:
	  y = i * textheight1;
	  break;
	case 1:
	  y = (uih->image->height - textheight1 * nlines) / 2 + i * textheight1;
	  break;
	case 2:
	  y = uih->image->height - textheight1 * (nlines - i);
	  break;
	}
      driver->print (x, y, uih->text + pos);
      pos += l + 1;
    }
  ui_flush ();
}
#ifdef _EFENCE_
extern int EF_ALIGNMENT;
extern int EF_PROTECT_BELOW;
extern int EF_PROTECT_FREE;
#endif
static void 
main_loop (void)
{
  int inmovement = 1;
  int x, y, b, k;
  int time;
  loopt = tl_create_timer ();
  while (1)
    {
      mousetype (uih->play ? REPLAYMOUSE : NORMALMOUSE);
      if (numbertype == FORMULA)
	{
	  if (uih_needrecalculate (uih))
	    uih_do_fractal (uih);
	  if (uih->display)
	    ui_updatestatus ();
	  if (uih->errstring != NULL)
	    {
	      driver->print (0, 0, uih->errstring);
	      ui_flush ();
#ifndef AROS
	      uih->errstring = NULL, sleep (1);
#else
#warning AROS has no sleep
	      uih->errstring = NULL;
#endif
	    }
	  if (uih->clearscreen)
	    driver->clrscr (), uih_cleared (uih);
	  if (uih->displaytext)
	    displayui (), uih_textdisplayed (uih);
	}
      else
	{
	  uih_stoptimers (uih);
	}
      if (introtext)
	{
	  introtext = 0;
	  /*driver->print(width/2-textwidth1*9,height-2*textheight1,"Welcome to XaoS"XaoS_VERSION); */
	  driver->print (width / 2 - textwidth1 * 29 / 2, height - textheight1, "Press 'H' twice for tutorials");
	  ui_flush ();
	}
      if ((time = tl_process_group (syncgroup, NULL)) != -1)
	{
	  if (!inmovement && !uih->inanimation)
	    {
	      if (time > 1000000 / 50)
		time = 1000000 / 50;
	      if (time > delaytime)
		{
		  tl_sleep (time - delaytime);
		}
	    }
	  inmovement = 1;
	}
      if (delaytime || maxframerate)
	{
	  tl_update_time ();
	  time = tl_lookup_timer (loopt);
	  tl_reset_timer (loopt);
	  time = 1000000 / maxframerate - time;
	  if (time < delaytime)
	    time = delaytime;
	  if (time)
	    tl_sleep (time);
	}
      processbuffer ();
      driver->processevents ((!inmovement && !uih->inanimation), &x, &y, &b, &k);
      inmovement = ui_mouse (x, y, b, k);
      if (todriver)
	ui_driver (todriver - 1), todriver = 0;
      if (callresize)
	ui_resize (), callresize = 0;
    }
}

#ifndef MAIN_FUNCTION
#define MAIN_FUNCTION main
#endif
int 
MAIN_FUNCTION (int argc, char **argv)
{
  int scanline;
  int i, formula = -1;
  char *buffer1, *buffer2;
#ifdef __alpha__
#ifdef __linux__
    extern void ieee_set_fp_control(unsigned long);
    ieee_set_fp_control(1UL);
#endif
#endif
  prog_argc= argc;
  prog_argv= argv;
  params_parser (argc, argv);
#ifdef MEM_DEBUG
  D_NORMALTEST;
#endif
#ifdef DESTICKY
  euid = geteuid ();
  egid = getegid ();
#endif
#ifdef _EFENCE_
  EF_ALIGNMENT = 1;
  EF_PROTECT_BELOW = 0;
  EF_PROTECT_FREE = 1;
#endif
#ifdef DEBUG
  printf ("Initialising driver\n");
#endif
#if !defined(_plan9_) && !defined(AROS)
  signal (SIGFPE, SIG_IGN);
#endif
  if (printconfig)
    {
#define tostring(s) #s
      printf ("XaoS configuration\n"
	      "Version:   %s\n"
	      "Type size: %i\n"
	      "Maxiter:   %i\n"
	      "MaxStep:   %f\n"
	      "integer size: %i\n"
	      "configfile: %s\n"

#ifndef _plan9_
#ifdef HAVE_ALLOCA
	      "using alloca\n"
#endif
#ifdef HAVE_LONG_DOUBLE
	      "using long double\n"
#endif
#ifdef USE_NCURSES
	      "using ncurses\n"
#endif
#ifdef const
	      "const disabled\n"
#endif
#ifdef inline
	      "inline disabled\n"
#endif
#ifdef HAVE_GETTIMEOFDAY
	      "using gettimeofday\n"
#endif
#ifdef HAVE_FTIME
	      "using ftime\n"
#endif
#ifdef MITSHM
	      "using mitshm\n"
#endif
#ifdef HAVE_MOUSEMASK
	      "using ncurses mouse\n"
#endif
#ifdef DEBUG
	      "debug enabled\n"
#endif
#ifdef NDEBUG
	      "assertions disabled\n"
#endif
#ifdef STATISTICS
	      "statistics enabled\n"
#endif
#endif
	      ,XaoS_VERSION, (int) sizeof (FPOINT_TYPE), (int) DEFAULT_MAX_ITER, MAXSTEP, (int) sizeof (int), CONFIGFILE);
    }
  if (deflist || printconfig)
    {
      printf ("Available drivers:\n");
      for (i = 0; i < ndrivers; i++)
	{
	  printf ("   %s\n", drivers[i]->name);
	}
      exit (0);
    }


#ifndef _plan9_
  xth_init (defthreads);
#endif
  if (defdriver != NULL)
    {
      for (i = 0; i < ndrivers; i++)
	{
	  int y;
	  for (y = 0; tolower (drivers[i]->name[y]) == tolower (defdriver[y]) && drivers[i]->name[y] != 0; y++);
	  if (drivers[i]->name[y] == 0)
	    {
	      driver = drivers[i];
	      if (driver->init ())
		break;
	      else
		{
		  printf ("Can not initialize %s driver\n", defdriver);
		  exit (-1);
		}
	    }
	}
      if (i == ndrivers)
	{
	  printf ("Unknown driver %s\n", defdriver);
	  exit (-1);
	}
    }
  else
    {
      for (i = 0; i < ndrivers; i++)
	{
	  driver = drivers[i];
	  if (driver->init ())
	    break;
	}
      if (i == ndrivers)
	{
	  printf ("Can not initialize driver\n");
	  exit (-1);
	}
    }
#ifdef DEBUG
  printf ("Getting size\n");
#endif
#ifdef DESTICKY
  seteuid (getuid ());		/* Don't need supervisor rights anymore. */
  setegid (getgid ());
#endif

  driver->getsize (&width, &height);
  if (!(scanline = driver->alloc_buffers (&buffer1, &buffer2)))
    {
      driver->uninit ();
      printf ("Can not alocate buffers\n");
      exit (-1);
    }
#ifdef _plan9_
  xth_init (defthreads);	/*plan0 requires to initialize tasks after graphics */
#endif
  mousetype (WAITMOUSE);
  driver->print (0, 0, "Initializing. Please wait");
  for (i = 0; i < nformulas; i++)
    if (defformula[i])
      formula = i;

  palette = createpalette (driver->palettestart, driver->paletteend, driver->imagetype, (driver->flags & RANDOM_PALETTE_SIZE) ? UNKNOWNENTRIES : 0, driver->maxentries, driver->set_color != NULL ? ui_alloccolor : NULL, driver->set_range != NULL ? ui_setpalette : NULL, NULL, NULL);
  image = create_image_cont (width, height, scanline, 2, (unsigned char *) buffer1, (unsigned char *) buffer2, palette, ui_flip, 0, get_windowwidth (width) / width, get_windowheight (height) / height);


  uih = uih_mkcontext (driver->flags, image, ui_passfunc, ui_message2);
  uih_loadcatalog (uih, "english");
  if (deflanguage != NULL)
    uih_loadcatalog (uih, deflanguage);
#ifdef HOMEDIR
  if (getenv ("HOME") != NULL)
    { char home[256],*env=getenv("HOME");
      int maxsize=255-strlen(CONFIGFILE)-1; /*Avoid buffer owerflow*/
      int i;
      for(i=0;i<maxsize&&env[i];i++) home[i]=env[i];
      env[i]=0;
      xio_addfname(configfile,home,CONFIGFILE);
    }
  else
#endif
    xio_addfname(configfile,XIO_EMPTYPATH,CONFIGFILE);

  ui_flush ();
  {
    xio_file f = xio_ropen (configfile);	/*load the configuration file */

    if (f != XIO_FAILED)
      {
	uih_load (uih, f, configfile);
	if (uih->errstring)
	  {
	    driver->free_buffers (NULL, NULL);
	    driver->uninit ();
	    fprintf (stderr, "Configuration file %s load failed:%s\n", configfile, uih->errstring);
	    fprintf (stderr, "Hint:try to remove it :)\n");
	    exit (1);
	  }
      }
  }
  fcontext = uih->fcontext;
  if (formula != -1)
    set_formula (fcontext, formula);
  srand (time (NULL));
  if (defiters > 0)
    fcontext->maxiter = defiters;
  if (defperiodicity >= 0)
    fcontext->periodicity = defperiodicity ^ 1;
  if (fastmode >= 0)
    uih->fastmode = fastmode;
  if (defguessing >= 0)
    fcontext->range = defguessing;
  if (defincoloring >= 0)
    fcontext->incoloringmode = defincoloring;
  if (defoutcoloring >= 0)
    fcontext->coloringmode = abs (defoutcoloring - 1) % OUTCOLORING;
  if (defplane >= 0)
    fcontext->plane = abs (defplane - 1) % PLANES;
  uih_letterspersec (uih, letterspersec);
  fcontext->version++;
  maintimer = tl_create_timer ();
  uih_newimage (uih);

  if (defautopilot)
    ui_autopilot ();
  if (defspeed >= 0)
    {
      if (defspeed >= 100)
	defspeed = 1.0;
      uih->speedup = uih->speedup * defspeed;
      uih->maxstep = uih->maxstep * defspeed;
    }
  tl_update_time ();
  tl_process_group (syncgroup, NULL);
  if (printspeed)
    {
      driver->uninit ();
      speed_test (uih->fcontext, image);
      return 0;
    }
  tl_reset_timer (maintimer);

  if (loadfile != NULL)
    {
      xio_file f = xio_ropen (loadfile);
      if (f == XIO_FAILED)
	{
	  driver->free_buffers (NULL, NULL);
	  driver->uninit ();
#ifndef AROS
	  perror (loadfile);
#else
#warning AROS has no perror
#endif
	  exit (1);
	}
      uih_load (uih, f, loadfile);
      if (uih->errstring)
	{
	  driver->free_buffers (NULL, NULL);
	  driver->uninit ();
	  fprintf (stderr, "Load failed:%s\n", uih->errstring);
	  exit (1);
	}
    }

  if (playfile != NULL)
    {
      xio_file f = xio_ropen (playfile);
      introtext = 0;
      if (f == NULL)
	{
	  driver->free_buffers (NULL, NULL);
	  driver->uninit ();
#ifndef AROS
	  perror (playfile);
#else
#warning AROS has no perror
#endif
	  exit (1);
	}
      uih_replayenable (uih, f, playfile);
      if (uih->errstring)
	{
	  driver->free_buffers (NULL, NULL);
	  driver->uninit ();
	  fprintf (stderr, "Play failed:%s\n", uih->errstring);
	  exit (1);
	}
      driver->print (0, 0, "Preparing first frame. Please wait");
      ui_flush ();
    }

  main_loop ();
  ui_quit (0);
  return (0);
}
fractal_context *
ui_getcontext (void)
{
  return (fcontext);
}

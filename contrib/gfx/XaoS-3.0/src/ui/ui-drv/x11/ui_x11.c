/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996,1997 by
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
#include "aconfig.h"
#ifdef X11_DRIVER
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include "xlib.h"
#ifdef MITSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif
#include <fconfig.h>
#include <ui.h>
xlibparam xparams =
{0, 0, 0, 0, NULL};
static int allocated;
Cursor normal, xwait, replay;

struct ui_driver x11_driver;
static xdisplay *d;
static char *size;
static int Xsync;
static int busy;
static int sharedcolormap;
#ifdef MITSHM
static int Completion;
#endif
#ifdef AMIGA
#define XFlush(x) while(0)
#endif

static void
x11_setpaletterange (ui_palette p, int s, int e)
{
  xsetpaletterange (d, p, s, e);
}
static int
x11_set_color (int r, int g, int b, int init)
{
  if (init)
    xfree_colors (d);
  return (xalloc_color (d, r * 256, g * 256, b * 256, init));
}

static void
x11_print (int x, int y, char *text)
{
  xmoveto (d, x, y + x11_driver.textheight - 2);
  xouttext (d, text);

}
static void
x11_flush (void)
{
  XFlush (d->display);
}

static void
x11_display (void)
{
  XFlush (d->display);
#ifdef MITSHM
  if (d->SharedMemFlag)
    busy++;
#endif
  if (Xsync)
    XSync (d->display, 0);
  draw_screen (d);
}

static void
x11_flip_buffers (void)
{
  xflip_buffers (d);
}
void
x11_free_buffers (char *b1, char *b2)
{
  if (allocated)
    {
      XSync (d->display, 0);
      allocated = 0;
      free_image (d);
    }
}
int
x11_alloc_buffers (char **b1, char **b2)
{
  if (!allocated)
    {
      XSync (d->display, 0);
      allocated = 1;
      if (!alloc_image (d))
	{
	  return (0);
	}
      xflip_buffers (d);
    }
  *b1 = d->vbuff;
  *b2 = d->back;
  return (d->linewidth);
}
static void
x11_getsize (int *w, int *h)
{
  XSync (d->display, 0);
  xupdate_size (d);
  *w = d->width;
  *h = d->height;
}

static void
x11_processevents (int wait, int *mx, int *my, int *mb, int *k)
{
  static int mousex = 0, mousey = 0;
  static int iflag = 0;
  static unsigned int mousebuttons = 0;
  static int resized;
  XEvent ev;

  if (busy >= 2 || wait || XPending (d->display))
    {
      do
	{
	  XNextEvent (d->display, &ev);
	  switch (ev.type)
	    {
	    case ClientMessage:
	      if (ev.xclient.format == 32 && ev.xclient.data.l[0] == wmDeleteWindow)
		ui_quit(0);
	      break;
	    case ButtonRelease:
	      mousex = ev.xbutton.x;
	      mousey = ev.xbutton.y;
	      switch (ev.xbutton.button)
		{
		case 1:
		  mousebuttons &= ~BUTTON1;
		  break;
		case 2:
		  mousebuttons &= ~BUTTON2;
		  break;
		case 3:
		  mousebuttons &= ~BUTTON3;
		  break;
		}
	      break;
	    case ButtonPress:
	      mousex = ev.xbutton.x;
	      mousey = ev.xbutton.y;
	      if (mousex < 0 || mousey < 0 || mousex > (int) d->width || mousey > (int) d->height)
		return;
	      switch (ev.xbutton.button)
		{
		case 1:
		  mousebuttons |= BUTTON1;
		  break;
		case 2:
		  mousebuttons |= BUTTON2;
		  break;
		case 3:
		  mousebuttons |= BUTTON3;
		  break;
		}
	      break;
	    case MotionNotify:
	      mousex = ev.xmotion.x;
	      mousey = ev.xmotion.y;
	      mousebuttons = ev.xmotion.state & (BUTTON1 | BUTTON2 | BUTTON3);
	      break;
	    case Expose:
	      if (resized)
		break;
#ifdef MITSHM
	      if (d->SharedMemFlag)
		busy++;
#endif
	      XSync (d->display, 0);
	      draw_screen (d);
	      ui_tbreak ();
	      break;
	    case ConfigureNotify:
	      {
		int oldw = d->width, oldh = d->height;
		XSync (d->display, 0);
		xupdate_size (d);
		if ((int) d->width != oldw || (int) d->height != oldh)
		  {
		    resized = 2;
		    ui_resize ();
		    resized = 0;
		  }
	      }
	      break;
	    case KeyRelease:
	      {
		KeySym ksym;
		switch (ksym = XLookupKeysym (&ev.xkey, 0))
		  {
		  case XK_Left:
		    iflag &= ~1;
		    break;
		  case XK_Right:
		    iflag &= ~2;
		    break;
		  case XK_Up:
		    iflag &= ~4;
		    break;
		  case XK_Down:
		    iflag &= ~8;
		    break;
		  }
	      }
	      break;
	    case KeyPress:
	      {
		KeySym ksym;
		switch (ksym = XLookupKeysym (&ev.xkey, 0))
		  {
		  case XK_Left:
		    iflag |= 1;
		    break;
		  case XK_Right:
		    iflag |= 2;
		    break;
		  case XK_Up:
		    iflag |= 4;
		    break;
		  case XK_Down:
		    iflag |= 8;
		    break;
		  default:
		    {
		      char *name, buff[256];
		      if (ksym == XK_F1)
			name = "h";
		      else if (ksym == XK_Escape)
			name = "q";
		      else
			{
			  name = buff;
			  buff[XLookupString (&ev.xkey, buff, 256, &ksym, NULL)] = 0;
			}
		      if (strlen (name) == 1)
			{
			  if (ui_key (*name) == 2)
			    return;
			}
		    }
		  }
	      }
	      break;
	    default:
#ifdef MITSHM
	      if (ev.xany.type == Completion)
		busy--;
#endif
	      break;
	    }
	}
      while (busy >= 2 || XEventsQueued (d->display, QueuedAlready));
    }
  *mx = mousex;
  *my = mousey;
  *mb = mousebuttons;
  *k = iflag;
}
/*static int defined; */
static void 
x11_cursor (int mode)
{
  /*if(defined)
     XUndefineCursor(d->display,d->window),defined=0; */
  switch (mode)
    {
    case NORMALMOUSE:
      XDefineCursor (d->display, d->window, normal);
      /*defined=1; */
      break;
    case WAITMOUSE:
      XDefineCursor (d->display, d->window, xwait);
      /*defined=1; */
      break;
    case REPLAYMOUSE:
      XDefineCursor (d->display, d->window, replay);
      /*defined=1; */
      break;
    }
  XFlush (d->display);
}
int
x11_init (void)
{
  if (xparams.fullscreen)
    sharedcolormap = 1;		/*private colormap is broken in fullscreen
				   mode (at least at my X) */
  xparams.privatecolormap = !sharedcolormap;
  if (xparams.display == NULL)
    {				/*solaris stuff */
      xparams.display = getenv ("DISPLAY");
    }
  if (size != NULL)
    {
      int x, y;
      sscanf (size, "%ix%i", &x, &y);
      if (x < 0)
	x = XSIZE;
      if (y < 0)
	y = YSIZE;
      d = xalloc_display ("XaoS", x, y, &xparams);
    }
  else
    d = xalloc_display ("XaoS", XSIZE, YSIZE, &xparams);
  if (d == NULL)
    return 0;
  /*normal=XCreateFontCursor(d->display,XC_arrow); */
  normal = XCreateFontCursor (d->display, XC_left_ptr);
  xwait = XCreateFontCursor (d->display, XC_watch);
  replay = XCreateFontCursor (d->display, XC_dot);
  if (d->truecolor || d->privatecolormap)
    x11_driver.flags &= ~RANDOM_PALETTE_SIZE;
  switch (d->visual->class)
    {
    case StaticGray:
      if (d->depth == 1)
	{
	  x11_driver.imagetype = UI_MBITMAP;
	}
      else
	{
	  /*Warning! this is untested. I believe it works */
	  /*x11_driver.set_color = x11_set_color; */
	  x11_driver.palettestart = 0;
	  x11_driver.paletteend = 256;
	  x11_driver.maxentries = 256;
	  x11_driver.imagetype = UI_GRAYSCALE;
	}
      break;
    case StaticColor:
    smallcolor:
      {
	int end = 256;
	int start = 0;
	int entries = d->visual->map_entries;
	if (d->visual->class == TrueColor)
	  {
	    entries = d->visual->red_mask |
	      d->visual->green_mask |
	      d->visual->blue_mask;
	  }
	x11_driver.imagetype = UI_FIXEDCOLOR;
	if (end > entries)
	  end = entries;
	if (end < 64)
	  start = 0;
	x11_driver.set_range = x11_setpaletterange;
	x11_driver.palettestart = start;
	x11_driver.paletteend = end;
	x11_driver.maxentries = end - start;
      }
      break;
    case PseudoColor:
      if (d->privatecolormap)
	{
	  int end = 256;
	  int start = 16;
	  if (end > d->visual->map_entries)
	    end = d->visual->map_entries;
	  if (end < 64)
	    start = 0;
	  x11_driver.set_range = x11_setpaletterange;
	  x11_driver.palettestart = start;
	  x11_driver.paletteend = end;
	  x11_driver.maxentries = end - start;
	}
      else
	{
	  int end = 256;
	  if (end > d->visual->map_entries)
	    end = d->visual->map_entries;
	  x11_driver.set_color = x11_set_color, x11_driver.flags |= RANDOM_PALETTE_SIZE;
	  x11_driver.palettestart = 0;
	  x11_driver.paletteend = end;
	  x11_driver.maxentries = end;
	}
      break;
    case TrueColor:
      switch (d->depth)
	{
	case 8:
	  goto smallcolor;
	case 15:
	  x11_driver.imagetype = UI_REALCOLOR;
	  break;
	case 16:
	  x11_driver.imagetype = UI_HICOLOR;
	  break;
	case 24:
	case 32:
	  if(d->visual->red_mask==0x0000ff00)
	       x11_driver.imagetype = UI_TRUECOLORMI; else
	       x11_driver.imagetype = UI_TRUECOLOR;
	}
    }
  x11_driver.maxwidth = XDisplayWidth (d->display, d->screen);
  x11_driver.maxheight = XDisplayHeight (d->display, d->screen);
  x11_driver.width = ((float) XDisplayWidthMM (d->display, d->screen)) / x11_driver.maxwidth / 10.0;
  x11_driver.height = ((float) XDisplayHeightMM (d->display, d->screen)) / x11_driver.maxheight / 10.0;
  x11_driver.textheight = xsetfont (d, "fixed");
  x11_driver.textwidth = d->font_struct->max_bounds.rbearing - d->font_struct->min_bounds.lbearing;
#ifdef MITSHM
  Completion = XShmGetEventBase (d->display) + ShmCompletion;
#endif
  if (d->privatecolormap)
    {
      x11_driver.flags |= PALETTE_ROTATION | ROTATE_INSIDE_CALCULATION;
    }
  return (1);
}
void
x11_uninitialise (void)
{
  xfree_colors (d);
  xfree_display (d);
}
void
x11_cls (void)
{
  xclear_screen (d);
}
void
x11_getmouse (int *x, int *y, int *b)
{
  int rootx, rooty;
  Window rootreturn, childreturn;
  XQueryPointer (d->display, d->window,
		 &rootreturn, &childreturn,
		 &rootx, &rooty, x,
		 y, (unsigned int *) b);
}
static char *helptext[] =
{
  "X11 DRIVER VERSION 3.0                 ",
  "======================                 ",
  "This was the first driver done for XaoS",
  "so it is fully featured but have       ",
  "following bugs/limitations:            ",
  " o XaoS makes some X servers too busy  ",
  "   In this case XaoS badly respond to  ",
  "   mouse and other aplications stops   ",
  "   This happends especially at old R4  ",
  "   servers. Use -sync to avoid this    ",
  "   problem.                            ",
  "   note that -sync do NOT enable an    ",
  "   synchronous mode in communication   ",
  "   just makes one additional XSync call",
  "   So slowdown is not so high as you   ",
  "   might expect                        ",
  " o supports only 8bpp pseudocolor,     ",
  "   15,16,24 and 32bpp truecolor,       ",
  "   staticolor modes.                   ",
  " o Palette rotating does not work for  ",
  "   8bpp pseudocolor w/o private palette",
  " o someone wants help?                 ",
  "                                       ",
  "And have following features:           ",
  " o runtime resizing of window          ",
  " o uses mitshm extension               ",
  "                                       ",
  "                                       ",
  "   X11 driver was done by Jan Hubicka  ",
  "           and Thomas Marsh            ",
  "              (C) 1997                 ",
};
#define UGLYTEXTSIZE (sizeof(helptext)/sizeof(char *))
static struct params params[] =
{
  {"-display", P_STRING, &xparams.display, "Select display"},
  {"-size", P_STRING, &size, "Select size of window (WIDTHxHEIGHT)."},
  {"-sync", P_SWITCH, &Xsync, "Generate sync signals before looking for events. This\n\t\t\thelps on old and buggy HP-UX X servers."},
  {"-shared", P_SWITCH, &sharedcolormap, "Use shared colormap on pseudocolor display."},
  {"-usedefault", P_SWITCH, &xparams.usedefault, "Use default visual if autodetection causes troubles."},
  {"-nomitshm", P_SWITCH, &xparams.nomitshm, "Disable MITSHM extension."},
  {"-fullscreen", P_SWITCH, &xparams.fullscreen, "Enable fullscreen mode."},
  {NULL, 0, NULL, NULL}
};

struct ui_driver x11_driver =
{
  "X11",
  x11_init,
  x11_getsize,
  x11_processevents,
  x11_getmouse,
  x11_uninitialise,
  NULL,
  NULL,
  x11_print,
  x11_display,
  x11_alloc_buffers,
  x11_free_buffers,
  x11_flip_buffers,
  x11_cls,
  x11_cursor,
  NULL,
  x11_flush,
  8,
  8,
  helptext,
  UGLYTEXTSIZE,
  params,
  RESOLUTION | PIXELSIZE | NOFLUSHDISPLAY /*| UPDATE_AFTER_RESIZE */ ,
  0.0, 0.0,
  0, 0,
  UI_C256,
  16, 254, 254 - 16
};

#endif

#include "aconfig.h"
#ifdef TEMPLATE_DRIVER
/*includes */
#include <ui.h>

static int
template_set_color (int r, int g, int b, int init)
{
  return ( /*pixel value or -1 for full palette */ -1);
}

static void
template_setpalette (ui_palette pal, int start, int end)
{
}

static void
template_print (int x, int y, char *text)
{
}

static void
template_display ()
{
}

static void
template_flip_buffers ()
{
}

void
template_free_buffers (char *b1, char *b2)
{
}

int
template_alloc_buffers (char **b1, char **b2)
{
  return 1;			/* bytes per scanline */
}

static void
template_getsize (int *w, int *h)
{
}

static void
template_processevents (int wait, int *mx, int *my, int *mb, int *k)
{
}

static int
template_init ()
{
  return ( /*1 for sucess 0 for fail */ 1);
}

static void
template_uninitialise ()
{
}

static void
template_getmouse (int *x, int *y, int *b)
{
}

static void
template_clearscreen ()
{
}

static void
template_mousetype (int type)
{
}

static char *helptext[] =
{
  "TEMPLATE DRIVER VERSION 1.0            ",
  "===========================            ",
  "blah....                               ",
  "                                       ",
  "                                       ",
};
#define UGLYTEXTSIZE (sizeof(helptext)/sizeof(char *))
static struct params params[] =
{
  {"-flag", P_SWITCH, &variable, "Example flag..."},
  {NULL, 0, NULL, NULL}
};

struct ui_driver template_driver =
{
  "Template",
  template_init,
  template_getsize,
  template_processevents,
  template_getmouse,
  template_uninitialise,
  template_set_color,		/*You should implement just one */
  template_set_palette,		/*of these and add NULL as second */
  template_print,
  template_display,
  template_alloc_buffers,
  template_free_buffers,
  template_flip_buffers,
  template_clearscreen,
  template_mousetype,		/*This should be NULL */
  NULL,				/*Driver depended action */
  NULL,				/*flush */
  256,				/*maximum palette size */
  8,				/*text width */
  8,				/*text height */
  helptext,
  UGLYTEXTSIZE,
  params,
  0,				/*flags...see ui.h */
  0.0, 0.0,			/*width/height of screen in centimeters */
  0, 0,				/*resolution of screen for windowed systems */
  UI_C256,			/*Image type */
  0, 255, 255			/*start, end of palette and maximum allocatable */
				/*entries */
};

#endif

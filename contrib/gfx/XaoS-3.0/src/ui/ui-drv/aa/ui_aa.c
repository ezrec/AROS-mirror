#include "aconfig.h"
#ifdef AA_DRIVER
#include <string.h>
#include <malloc.h>
#include <aalib.h>
#include <ui.h>
#include <unistd.h>
#include <archaccel.h>
struct ui_driver aalib_driver;
static aa_palette palette;
static aa_palette aapalette;
static int useaapalette;
#define c context
static aa_context *c;
static unsigned char *secondary;
static int mouse;
static int nomouse;



static void
aa_print (int x, int y, char *text)
{
  aa_puts (c, x / 2, y / 2, AA_SPECIAL, text);
}

static void
aa_display ()
{
  aa_renderpalette (c, useaapalette ? aapalette : palette, &aa_defrenderparams, 0, 0, aa_scrwidth (c), aa_scrheight (c));
}
static void
aa_set_palette (ui_palette pal, int start, int end)
{
  int i;
  for (i = start; i <= end; i++)
    aa_setpalette (palette, i, pal[i - start][0], pal[i - start][1], pal[i - start][2]);
  aa_display ();
  aa_flush (c);
}

static void
aa_flip_buffers ()
{
  unsigned char *tmp;
  tmp = secondary;
  secondary = c->imagebuffer;
  c->imagebuffer = tmp;
}

static void
aa_free_buffers (char *b1, char *b2)
{
  free (secondary);
}

static int
aa_alloc_buffers (char **b1, char **b2)
{
  secondary = malloc (aa_imgwidth (c) * aa_imgheight (c));
  *(unsigned char **)b2 = secondary;
  *(unsigned char **)b1 = c->imagebuffer;
  return aa_imgwidth (c);	/* bytes per scanline */
}

static void
aa_getsize (int *w, int *h)
{
  aa_resize (c);
  *w = aa_imgwidth (c);
  *h = aa_imgheight (c);
}

static int mousex, mousey, mouseb;
static void
aa_processevents (int wait, int *mx, int *my, int *mb, int *k)
{
  int ch;
  static int keys;
  do
    {
      if ((ch = aa_getevent (c, wait)) != AA_NONE && ch != AA_MOUSE && ch != AA_RESIZE && ch < 256)
	ui_key (ch);
      switch (ch)
	{
	case AA_BACKSPACE:
	  ui_key (8);
	  break;
	case AA_RESIZE:
	  ui_resize ();
	  break;
	case AA_MOUSE:
	  aa_getmouse (c, &mousex, &mousey, &mouseb);
	  break;
	case AA_LEFT:
	  if (c->kbddriver->flags & AA_SENDRELEASE)
	    keys |= 1;
	  else if (keys & 2)
	    keys &= ~2;
	  else
	    keys ^= 1;
	  break;
	case AA_LEFT | AA_RELEASE:
	  keys &= ~1;
	  break;
	case AA_RIGHT:
	  if (c->kbddriver->flags & AA_SENDRELEASE)
	    keys |= 2;
	  else if (keys & 1)
	    keys &= ~1;
	  else
	    keys ^= 2;
	  break;
	case AA_RIGHT | AA_RELEASE:
	  keys &= ~2;
	  break;
	case AA_UP:
	  if (c->kbddriver->flags & AA_SENDRELEASE)
	    keys |= 4;
	  else if (keys & 8)
	    keys &= ~8;
	  else
	    keys ^= 4;
	  break;
	case AA_UP | AA_RELEASE:
	  keys &= ~4;
	  break;
	case AA_DOWN:
	  if (c->kbddriver->flags & AA_SENDRELEASE)
	    keys |= 8;
	  else if (keys & 4)
	    keys &= ~4;
	  else
	    keys ^= 8;
	  break;
	case AA_DOWN | AA_RELEASE:
	  keys &= ~8;
	  break;
	}
      wait = 0;
    }
  while (ch != AA_NONE);
  *mx = mousex * 2;
  *my = mousey * 2;
  *k = keys;
  *mb = 0;
  if (mouseb & AA_BUTTON1)
    *mb |= BUTTON1;
  if (mouseb & AA_BUTTON2)
    *mb |= BUTTON2;
  if (mouseb & AA_BUTTON3)
    *mb |= BUTTON3;
  return;
}

#define NATTRS 7
static char *aadriver = NULL;
static char *kbddriver = NULL;
static char *mousedriver = NULL;
static char *deffont = NULL;
static int width, height, minwidth, minheight, maxwidth, maxheight, recwidth,
  recheight;
static int enable[NATTRS + 2], disable[NATTRS + 2];
static int extended, inverse, bright = 0, contrast = 0;
static float aa_gamma, dimmul, boldmul;
static int dithering[3];
static int randomval = 0;
static int masks[] =
{AA_NORMAL_MASK, AA_DIM_MASK, AA_BOLD_MASK, AA_BOLDFONT_MASK, AA_REVERSE_MASK, AA_ALL, AA_EIGHT};
static int
aa_initialize ()
{
  int i, y;
  aa_parseoptions (NULL, NULL, NULL, NULL);	/*parse environment first */

  if (deffont != NULL)
    {
      for (y = 0; aa_fonts[y] != NULL; y++)
	{
	  if (!strcmp (deffont, aa_fonts[y]->name) || !strcmp (deffont, aa_fonts[y]->shortname))
	    {
	      aa_defparams.font = aa_fonts[y];
	      break;
	    }
	}
    }

  if (extended)
    aa_defparams.supported |= AA_EXTENDED;

  for (i = 0; i < NATTRS; i++)
    {
      if (enable[i])
	aa_defparams.supported |= masks[i];
      if (disable[i])
	aa_defparams.supported &= ~masks[i];
    }

  for (i = 0; i < 3; i++)
    if (dithering[i])
      aa_defrenderparams.dither = i;
  if (randomval)
    aa_defrenderparams.randomval = randomval;
  if (bright)
    aa_defrenderparams.bright = bright;
  if (contrast)
    aa_defrenderparams.contrast = contrast;
  if (aa_gamma)
    aa_defrenderparams.gamma = aa_gamma;

  if (width)
    aa_defparams.width = width;
  if (height)
    aa_defparams.height = height;
  if (minwidth)
    aa_defparams.minwidth = minwidth;
  if (minheight)
    aa_defparams.minheight = minheight;
  if (maxwidth)
    aa_defparams.maxwidth = maxwidth;
  if (maxheight)
    aa_defparams.maxheight = maxheight;
  if (recwidth)
    aa_defparams.recwidth = recwidth;
  if (recheight)
    aa_defparams.recheight = recheight;

  if (aadriver != NULL)
    aa_recommendhidisplay (aadriver);
  if (kbddriver != NULL)
    aa_recommendhikbd (kbddriver);
  if (mousedriver != NULL)
    aa_recommendhimouse (mousedriver);
  if (dimmul)
    aa_defparams.dimmul = dimmul;
  if (boldmul)
    aa_defparams.boldmul = boldmul;
  if (inverse)
    aa_defrenderparams.inversion = 1;

  c = aa_autoinit (&aa_defparams);
  if (c == NULL)
    return 0;
  aa_autoinitkbd (c, AA_SENDRELEASE);
  if (!nomouse)
    mouse = aa_autoinitmouse (c, AA_MOUSEALLMASK);

  aalib_driver.width = aa_mmwidth (c) / 10.0;
  aalib_driver.height = aa_mmheight (c) / 10.0;
  return (1);
}

static void
aa_uninitialise ()
{
  aa_close (c);
}

static void
aa_get_mouse (int *x, int *y, int *b)
{
  if (mouse)
    aa_getmouse (c, &mousex, &mousey, &mouseb);
  *x = mousex * 2;
  *y = mousey * 2;
  *b = 0;
  if (mouseb & AA_BUTTON1)
    *b |= BUTTON1;
  if (mouseb & AA_BUTTON2)
    *b |= BUTTON2;
  if (mouseb & AA_BUTTON3)
    *b |= BUTTON3;
}

static void
aa_clearscreen ()
{
  memset (c->textbuffer, ' ', aa_scrwidth (c) * aa_scrheight (c));
  memset (c->attrbuffer, AA_NORMAL, aa_scrwidth (c) * aa_scrheight (c));
}

static void
aa_mousetype (int type)
{
#if (AA_LIB_VERSION>1||AA_LIB_MINNOR>0)
  if (type == REPLAYMOUSE)
    aa_hidemouse (c);
  else
    aa_showmouse (c);
#endif
}

static char *name[] =
{
  "normal characters     ",
  "half bright(dim)      ",
  "double bright(bold)   ",
  "bold font             ",
  "reversed              ",
  "reserved characters   ",
  "non ascii characters  ",
};

#if AA_LIB_VERSION==1 && AA_LIB_MINNOR==0
#define SUPPORTED c->driver->params.supported
#else
#define SUPPORTED c->driverparams.supported
#endif
static void
aa_atributes (int m)
{
  static int mask;
  char text[NATTRS + 1][50];
  char *menu[NATTRS + 1];
  int i;
  switch (m)
    {
    case -1:
      mask = c->params.supported;
      break;
    case 0:
      mask ^= AA_NORMAL_MASK;
      break;
    case 1:
      mask ^= AA_DIM_MASK;
      break;
    case 2:
      mask ^= AA_BOLD_MASK;
      break;
    case 3:
      mask ^= AA_BOLDFONT_MASK;
      break;
    case 4:
      mask ^= AA_REVERSE_MASK;
      break;
    case 5:
      mask ^= AA_ALL;
      break;
    case 6:
      mask ^= AA_EIGHT;
      break;
    case 7:
      return;
    }
  aa_setsupported (c, mask);
  for (i = 0; i < NATTRS; i++)
    {
      menu[i] = &text[i][0];
      sprintf (text[i], "%s %s", name[i], (SUPPORTED & masks[i]) ? (mask & masks[i]) ? "on" : "off" : "unsupported");
    }
  menu[i] = &text[i][0];
  sprintf (text[i], "Close menu");
  ui_menu (menu, "Available attributes:", NATTRS + 1, aa_atributes);
}

static void
aa_sfont (int i)
{
  aa_setfont (c, aa_fonts[i]);
}

static void
aa_font_menu (void)
{
  char *menu[100];
  int i;
  for (i = 0; aa_fonts[i] != NULL; i++)
    menu[i] = aa_fonts[i]->name;
  ui_menu (menu, "Please select font", i, aa_sfont);
}

static int driver;
static int font;
static int mask;

static void
aa_save5 (int m)
{
  struct aa_hardware_params p = aa_defparams;
  struct aa_context *sc;
  struct aa_savedata data;
  int i;
  char *name;
  char extension[100];
  if(aa_formats[driver]->flags&AA_USE_PAGES)
  strcpy(extension,"_0_0");
  else
  extension[0]=0;
  strcat(extension,aa_formats[driver]->extension);

#ifdef DJGPP
  name = ui_getfile ("fr", extension);
	  /*fit into ugly 8char. limit*/
#else
  name = ui_getfile ("fract", extension);
#endif

  if (name == NULL)
    {
      aa_print (0, 0, "Save driver initialization failed");
      aa_flush (c);
      sleep (3);
      return;
    }
  for (i = 0; name[i] != '.'&&name[i]!='_'; i++);
  name[i] = 0;
  strcat(name,"%c%e");

  p.minwidth = p.maxwidth = 0;
  p.minheight = p.maxheight = 0;
  p.width = aa_scrwidth (c);
  p.height = aa_scrheight (c);
  p.supported = mask;
  if (aa_formats[m]->font == NULL)
    p.font = aa_fonts[m];

  data.format = aa_formats[driver];
  data.name = name;
  sc = aa_init (&save_d, &p, &data);
  if (sc == NULL)
    {
      aa_print (0, 0, "Save driver initialization failed");
      aa_flush (c);
      sleep (3);
      return;
    }
  memcpy (sc->imagebuffer, c->imagebuffer, (aa_imgwidth (c) - 1) * aa_imgheight (c));
  aa_renderpalette (sc, useaapalette ? aapalette : palette, &aa_defrenderparams, 0, 0, aa_scrwidth (c), aa_scrheight (c));
  aa_flush (sc);
  aa_close (sc);
}

static void
aa_save4 (int m)
{
  char text[NATTRS + 1][50];
  char *menu[NATTRS + 1];
  int i;
  switch (m)
    {
    case -1:
      mask = aa_formats[driver]->supported & c->params.supported;
      if (!mask)
	mask = aa_formats[driver]->supported;
      break;
    case 0:
      mask ^= AA_NORMAL_MASK;
      break;
    case 1:
      mask ^= AA_DIM_MASK;
      break;
    case 2:
      mask ^= AA_BOLD_MASK;
      break;
    case 3:
      mask ^= AA_BOLDFONT_MASK;
      break;
    case 4:
      mask ^= AA_REVERSE_MASK;
      break;
    case 5:
      mask ^= AA_ALL;
      break;
    case 6:
      mask ^= AA_EIGHT;
      break;
    case 7:
      aa_save5 (font);
      return;
    }
  for (i = 0; i < NATTRS; i++)
    {
      menu[i] = &text[i][0];
      sprintf (text[i], "%s %s", name[i], (aa_formats[driver]->supported & masks[i]) ? (mask & masks[i]) ? "on" : "off" : "unsupported");
    }
  menu[i] = &text[i][0];
  sprintf (text[i], "save file");
  ui_menu (menu, "Available attributes:", NATTRS + 1, aa_save4);
}

static void
aa_save3 (int m)
{
  aa_display ();
  font = m;
  aa_save4 (-1);
}

static void
aa_save2 (int m)
{
  static char *menu[100];
  int i;

  driver = m;
  if (aa_formats[m]->font != NULL)
    aa_save3 (0);
  else
    {
      for (i = 0; aa_fonts[i] != NULL; i++)
	menu[i] = aa_fonts[i]->name;
      ui_menu (menu, "Please select font", i, aa_save3);
    }
}

static void
aa_save (void)
{
  char *menu[100];
  int i;
  for (i = 0; aa_formats[i] != NULL; i++)
    menu[i] = aa_formats[i]->formatname;
  ui_menu (menu, "Please select format", i, aa_save2);
}

static void
aa_dither (int m)
{
  aa_defrenderparams.dither = m;
}

static void
aa_dither_menu (void)
{
  char *menu[100];
  int i;

  aa_display ();
  for (i = 0; i < AA_DITHERTYPES; i++)
    menu[i] = aa_dithernames[i];
  ui_menu (menu, "Please select dithering mode", i, aa_dither);
}

static void
aa_spalette (int m)
{
  int i, s;
  if (!m)
    {
      useaapalette = 0;
      aa_display ();
      return;
    }
  useaapalette = 1;
  s = 1 << m;
  s--;
  for (i = 0; i < 255; i++)
    {
      aapalette[i] = (i & (s)) * 255 / s;
    }
}

static void
aa_palette_menu (void)
{
  static char *menu[] =
  {
    "Use XaoS palette",
    "Black and white stripes",
    "4 gray palette",
    "16 gray palette",
    "32 gray palette",
    "64 gray palette",
    "128 gray palette",
    "256 gray palette"
  };
  ui_menu (menu, "Please select AA palette", sizeof (menu) / sizeof (char *), aa_spalette);
}

static void
aa_actionpress (int m)
{
  switch (m)
    {
    case 0:
      aa_atributes (-1);
      break;
    case 1:
      aa_font_menu ();
      break;
    case 2:
      aa_defrenderparams.inversion ^= 1;
      break;
    case 3:
      aa_dither_menu ();
      break;
    case 4:
      aa_palette_menu ();
      break;
    case 5:
      aa_save ();
      break;
    }
}

static void
aa_action (void)
{
  static char *menu[] =
  {
    "Attributes",
    "Font",
    "Inversion",
    "Dithering mode",
    "Palette options",
    "Save text screen",
  };
  ui_menu (menu, "AA driver options", sizeof (menu) / sizeof (char *), aa_actionpress);
}
static void
aa_fflush (void)
{
  aa_flush (c);
}

static char *helptext[] =
{
  "AA DRIVER VERSION 1.0                  ",
  "=====================                  ",
  "                                       ",
  "AA driver is currently the most        ",
  "advanced and portable driver for XaoS  ",
  "It is based at AAlib - an hight quality",
  "ASCII-art library developed            ",
  "by AA-project                          ",
  "(see http://www.ta.jcu.cz/aa)          ",
  "                                       ",
  "It is fully featured XaoS driver for   ",
  "text mode displays. It supports 256    ",
  "colors and mouse where possible.       ",
  "                                       ",
  "It's advanced features is accesible via",
  "Driver depended action menu availabele ",
  "from varius stuff menu('V'). Menu      ",
  "contain following stuff:               ",
  " Attributes                            ",
  "  AA-lib may use character attributes  ",
  "  to reach fine qualitty of image.     ",
  "  By default it uses normal, dim and   ",
  "  bold characters where possible.      ",
  "  But you should also enable different ",
  "  ones like reversed or bold font      ",
  "  characters. You may also enable usage",
  "  of non ansii/reversed characters if  ",
  "  your device supports it.             ",
  " Font                                  ",
  "  AA-lib uses bitmap image of font to  ",
  "  prepare approximation table used for ",
  "  ascii art rendering. This bitmap is  ",
  "  expected to be same as one used by   ",
  "  your device. AAlib performs          ",
  "  detection where possible however some",
  "  devices (like UNIX text terminals or ",
  "  MDA does not support this.) AAlib has",
  "  few font images compiled in, so in   ",
  "  this cases you should try to use one ",
  "  of them to archieve best results.    ",
  " Inversion                             ",
  "  Some devices uses inversed video     ",
  "  use this to get correct colors at    ",
  "  such devices.                        ",
  " Dithering mode                        ",
  "  Dithering is an way to get more exact",
  "  color in approximation by combining  ",
  "  more characters. But this method     ",
  "  should bring ugly looking noise at   ",
  "  certain images. Use this menu to     ",
  "  disable or tune it.                  ",
  " Palette opions                        ",
  "  By default AA driver uses XaoS       ",
  "  palette to render images, but        ",
  "  it quite often looks ugly at text    ",
  "  displays. Here you can choose special",
  "  text palette instead. Note that with ",
  "  filters enabled results should be    ",
  "  rather ugly.                         ",
  " Save text screen                      ",
  "  Normal save function will generate   ",
  "  normal PNG image instad of nice      ",
  "  ascii-art. To save ascii art use this",
  "  function instead. It supports many   ",
  "  text file formats like HTML, ANSI,   ",
  "  MORE, etc... It will also ask you for",
  "  font and attributes(see above)       ",
  "                                       ",
  "                                       ",
  " An AA driver was done by Jan Hubicka  ",
  "                1997                   ",
};
#define UGLYTEXTSIZE (sizeof(helptext)/sizeof(char *))
static struct params params[] =
{
  {"-aadriver", P_STRING, &aadriver, "Select display driver used by aa-lib"},
  {"-kbddriver", P_STRING, &kbddriver, "Select keyboard driver used by aa-lib"},
  {"-mousedriver", P_STRING, &mousedriver, "Select keyboard driver used by aa-lib"},
  {"-font", P_STRING, &deffont, "Select font"},
  {"-width", P_NUMBER, &width, "Set width"},
  {"-height", P_NUMBER, &height, "Set height"},
  {"-minwidth", P_NUMBER, &minwidth, "Set minimal allowed width"},
  {"-minheight", P_NUMBER, &minheight, "Set minimal allowed height"},
  {"-maxwidth", P_NUMBER, &maxwidth, "Set maximal allowed width"},
  {"-maxheight", P_NUMBER, &maxheight, "Set maximal allowed height"},
  {"-recwidth", P_NUMBER, &recwidth, "Set recommended width"},
  {"-recheight", P_NUMBER, &recheight, "Set recommended height"},
  {"-normal", P_SWITCH, enable, "enable usage of narmal characters"},
  {"-nonormal", P_SWITCH, disable, "disable usage of narmal characters"},
  {"-dim", P_SWITCH, enable + 1, "enable usage of dim(half bright) characters"},
  {"-nodim", P_SWITCH, disable + 1, "disable usage of dim(half bright) characters"},
  {"-bold", P_SWITCH, enable + 2, "enable usage of bold(double bright) characters"},
  {"-nobold", P_SWITCH, disable + 2, "disable usage of bold(double bright) characters"},
  {"-boldfont", P_SWITCH, enable + 3, "enable usage of boldfont characters"},
  {"-noboldfont", P_SWITCH, disable + 3, "disable usage of boldfont characters"},
  {"-reverse", P_SWITCH, enable + 4, "enable usage of reversed characters"},
  {"-noreverse", P_SWITCH, disable + 4, "disable usage of reversed characters"},
  {"-all", P_SWITCH, enable + 5, "enable usage of reserved characters"},
  {"-eight", P_SWITCH, enable + 6, "enable usage of non ansii characters"},
{"-extended", P_SWITCH, &extended, "enable usage of extended character set"},
  {"-inverse", P_SWITCH, &inverse, "enable inverse"},
  {"-bright", P_NUMBER, &bright, "set bright (0-255)"},
  {"-contrast", P_NUMBER, &contrast, "set contrast (0-255)"},
  {"-gamma", P_FLOAT, &aa_gamma, "set famma (0-1)"},
  {"-nodither", P_SWITCH, dithering, "Disable dithering"},
  {"-floyd_steinberg", P_SWITCH, dithering + 2, "Enable floyd steinberg dithering"},
  {"-error_distribution", P_SWITCH, dithering + 1, "Enable error distribution dithering"},
  {"-random", P_NUMBER, &randomval, "Set random dithering value"},
  {"-dimmul", P_FLOAT, &dimmul, "Multiply factor for dim color (5.3)"},
  {"-boldmul", P_FLOAT, &boldmul, "Multiply factor for bold color (5.3)"},
  {"-nomouse", P_SWITCH, &nomouse, "Disable mouse"},
  {NULL, 0, NULL, NULL}
};

struct ui_driver aalib_driver =
{
  "aa",
  aa_initialize,
  aa_getsize,
  aa_processevents,
  aa_get_mouse,
  aa_uninitialise,
  NULL,				/*You should implement just one */
  aa_set_palette,		/*of these and add NULL as second */
  aa_print,
  aa_display,
  aa_alloc_buffers,
  aa_free_buffers,
  aa_flip_buffers,
  aa_clearscreen,
  aa_mousetype,
  aa_action,
  aa_fflush,
  2,				/*text width */
  2,				/*text height */
  helptext,
  UGLYTEXTSIZE,
  params,
  PALETTE_REDISPLAYS | SCREENSIZE | UPDATE_AFTER_RESIZE,	/*flags...see ui.h */
  0.0, 0.0,			/*width/height of screen in centimeters */
  0, 0,				/*resolution of screen for windowed systems */
  UI_C256,			/*Image type */
  0, 255, 255			/*start, end of palette and maximum allocatable */
				/*entries */
};

#endif

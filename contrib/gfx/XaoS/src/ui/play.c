#ifndef _plan9_
#ifndef AROS
#include <limits.h>
#include <malloc.h>
#endif
#include <string.h>
#else
#include <u.h>
#include <libc.h>
#endif
#include <fconfig.h>
#include <filter.h>
#include <fractal.h>
#include <ui_helper.h>
#include <catalog.h>
#include "play.h"
extern struct keyword *in_word_set (register char *str, register unsigned int len);
static int nonblockmode;
static catalog_t *catalog;	/*The message catalog should be "session wide" */

void
uih_freecatalog (uih_context * c)
{
  if (catalog != NULL)
    free_catalog (catalog),
      catalog = NULL;
}

int
uih_loadcatalog (uih_context * c, char *name)
{
  static int firsttime = 1;
  xio_file f = xio_getcatalog(name);
  if (f == XIO_FAILED)
    {
      if (firsttime)
	{
	  firsttime = 0;
	  return 0;
	}			/*Let XaoS work as stand alone executable */
      c->errstring = "Catalog file not found";
      return 0;
    }
  firsttime = 0;
  if (catalog != NULL)
    free_catalog (catalog);
  catalog = load_catalog (f, &c->errstring);
  return (catalog != NULL);
}
static void
handler (void *userdata)
{
  struct uih_context *uih = userdata;
  uih->playc->playframe++;
  uih->inanimation = 2;
  if (uih->playc->timerin)
    tl_remove_timer (uih->playc->timer);
  uih->playc->timerin = 0;
}
static void
handler1 (void *userdata)
{
  struct uih_context *uih = userdata;
  uih->playc->playframe++;
  uih->inanimation = 2;
  tl_update_time ();
  tl_reset_timer (uih->playc->timer);
  uih_setcomplettehandler (uih, NULL, NULL);
}
void
uih_skipframe (struct uih_context *uih)
{
  if (uih->play && uih->playc->timerin)
    handler (uih),tl_reset_timer(uih->playc->timer);
}
int
uih_replayenable (struct uih_context *uih, xio_file f, xio_path filename)
{
  struct uih_playcontext *p;
  if (uih->play)
    {
      uih->errstring = "Replay is arelady active";
      return 0;
    }
  if (f == XIO_FAILED)
    {
      uih->errstring = "File open failed";
      return 0;
    }
  p = calloc (sizeof (*p), 1);
  if (p == NULL)
    {
      uih->errstring = "Out of memory";
      return 0;
    }
  p->file = f;
  p->playframe = 1;
  p->timer = tl_create_timer ();
  p->frametime = 0;
  p->morph = 0;
  p->morphjulia = 0;
  tl_update_time ();
  tl_reset_timer (p->timer);
  tl_set_handler (p->timer, handler, uih);
  uih_stoptimers (uih);
  if (uih->stoppedtimers)
    tl_stop_timer (p->timer);
  uih->playc = p;
  uih->play = 1;
  uih->playc->line = 1;
  if (filename != NULL)
    {
      uih->playc->directory = xio_getdirectory(filename);
    }
  else
    {
      uih->playc->directory = xio_getdirectory(XIO_EMPTYPATH);
    }
  uih->playc->level = 0;
  uih_playupdate (uih);
  return 1;
}
void
uih_replaydisable (struct uih_context *uih)
{
  if (uih->play)
    {
      int i;
      uih->play = 0;
      tl_free_timer (uih->playc->timer);
      xio_close (uih->playc->file);
      for (i = 0; i < uih->playc->level; i++)
	xio_close (uih->playc->prevfiles[i]);
      uih->display = 1;
      uih->nonfractalscreen = 0;
      uih_setcomplettehandler (uih, NULL, NULL);
      free (uih->playc->directory);
      free (uih->playc);
    }
}
static char *errstring;
#define seterr(str) if(errstring==NULL) errstring=str;
#define FD uih->playc->file
static char token[1024];
static int first;
static int last;
static int parsenext;
static void
skipblank (struct uih_context *uih)
{
  int c;
  do
    {
      c = xio_getc (FD);
      if (c == '\n')
	uih->playc->line++;
      if (c == ';')
	while (c != '\n' && !xio_feof (FD))
	  {
	    c = xio_getc (FD);
	    if (c == '\n')
	      uih->playc->line++;
	  }
      while (xio_feof (FD) && uih->playc->level) {
	xio_close(FD);
	uih->playc->file = uih->playc->prevfiles[--uih->playc->level];
	uih->playc->line = 1;
      }
    }
  while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
  xio_ungetc (c, FD);
}
static int
gettoken (struct uih_context *uih)
{
  int c;
  int i = 0;
  skipblank (uih);
  if (first && (c = xio_getc (FD) != '('))
    {
      seterr ("Expected (");
      return 0;
    }
  if (first)
    skipblank (uih), first = 0;
  if (xio_feof (FD))
    {
      seterr ("Unexpected end of file 1");
      return 0;
    }
  if ((c = xio_getc (FD)) == '"')
    {
      token[i] = '"';
      i++;
      do
	{
	  c = xio_getc (FD);
	  if (c == XIO_EOF)
	    {
	      seterr ("Unexpected end of file 2");
	      return 0;
	    }
	  if (c == '\n')
	    uih->playc->line++;
	  if (c == '\\')
	    token[i] = xio_getc (FD);
	  else
	    token[i] = c;
	  i++;
	  if (i >= 1024)
	    {
	      seterr ("Token is too long");
	      i = 0;
	    }
	}
      while (c != '"');
    }
  else
    xio_ungetc (c, FD);
  do
    {
      c = xio_getc (FD);
      if (c == XIO_EOF)
	{
	  seterr ("Unexpected end of file 2");
	  return 0;
	}
      token[i] = c;
      i++;
      if (i >= 1024)
	{
	  seterr ("Token is too long");
	  i = 0;
	}
    }
  while (c != ' ' && c != '\t' && c != ')' && c != '\n');
  i--;
  token[i] = 0;
  skipblank (uih);
  if (c == ')')
    {
      last = 1;
      return i;
    }
  c = xio_getc (FD);
  while (xio_feof (FD) && uih->playc->level)
    uih->playc->file = uih->playc->prevfiles[--uih->playc->level], uih->playc->line = 1;
  if (c == XIO_EOF)
    {
      seterr ("Unexpected end of file 3");
      return 0;
    }
  if (c == ')')
    {
      last = 1;
      return i;
    }
  xio_ungetc (c, FD);
  return i;
}
static int
getint (struct uih_context *uih)
{
  int param;
  gettoken (uih);
  if (sscanf (token, "%i", &param) == 0)
    {
      seterr ("Integer parameter expected");
      return 0;
    }
  return (param);
}
static number_t
getfloat (struct uih_context *uih)
{
#ifdef HAVE_LONG_DOUBLE
  long double param;
#else
  double param;
#endif
  gettoken (uih);
#ifdef HAVE_LONG_DOUBLE
#ifndef USE_ATOLD
  if (sscanf (token, "%LG", &param) == 0)
#else
  param = _atold (token);
  if (0)
#endif
    {
#else
  if (sscanf (token, "%lG", &param) == 0)
    {
#endif
      seterr ("Floating point number expected");
      return 0;
    }
  return (param);
}
static char *
getkeystring (struct uih_context *uih)
{
  gettoken (uih);
  if (token[0] == '\'')
    return &token[1];
  return token;
}
static char *
getstring (struct uih_context *uih)
{
  int l;
  l = gettoken (uih);
  if (token[0] != '"' || token[l - 1] != '"' || l == 1)
    {
      seterr ("String parameter expected");
      return token;
    }
  token[l - 1] = 0;
  return token + 1;
}
static int
getnstring (struct uih_context *uih, char **str)
{
  char *text = getkeystring (uih);
  int i;
  for (i = 0;; i++)
    {
      if (str[i] == NULL)
	{
	  seterr ("Inexpected parameter");
	  return 0;
	}
      if (!strcmp (text, str[i]))
	return i;
    }
}
static int
getbool (struct uih_context *uih)
{
  if (gettoken (uih) != 2)
    {
      seterr ("Booelan type parameter expected");
      return 0;
    }
  if (token[0] != '#')
    {
      seterr ("Booelan type parameter expected");
      return 0;
    }
  if (token[1] == 't')
    {
      return 1;
    }
  if (token[1] == 'f')
    {
      return 0;
    }
  seterr ("Booelan type parameter expected");
  return 0;
}
void
uih_play_formula (struct uih_context *uih)
{
  char *fname = getkeystring (uih);
  int i;
  for (i = 0; i < nformulas; i++)
    {
      if (!strcmp (formulas[i].shortname, fname))
	{
	  set_formula (uih->fcontext, i);
	  uih_newimage (uih);
	  return;
	}
    }
  seterr ("Unknown formula type");
}
void
uih_play_setview (struct uih_context *uih)
{
  uih->fcontext->s.cr = getfloat (uih);
  uih->fcontext->s.ci = getfloat (uih);
  uih->fcontext->s.rr = getfloat (uih);
  uih->fcontext->s.ri = getfloat (uih);
  if (uih->fcontext->s.rr < 0 || uih->fcontext->s.ri < 0)
    {
      seterr ("view:Invalid viewpoint");
      uih->fcontext->s.rr = uih->fcontext->s.ri + 0.00001;
    }
  uih_newimage (uih);
}
void
uih_play_setview2 (struct uih_context *uih)
{
  uih->fcontext->s.cr = getfloat (uih);
  uih->fcontext->s.ci = getfloat (uih);
  uih->fcontext->s.rr = getfloat (uih);
  uih->fcontext->s.ri = getfloat (uih);
  if (uih->fcontext->s.rr < 0 || uih->fcontext->s.ri < 0)
    {
      seterr ("animateview:Invalid viewpoint");
      uih->fcontext->s.rr = uih->fcontext->s.ri + 0.00001;
    }
  uih_animate_image (uih);
}
void
uih_playmorph (struct uih_context *uih)
{
  uih->playc->source = uih->fcontext->s;
  uih->playc->destination.cr = getfloat (uih);
  uih->playc->destination.ci = getfloat (uih);
  uih->playc->destination.rr = getfloat (uih);
  uih->playc->destination.ri = getfloat (uih);
  if (uih->playc->destination.rr < 0 || uih->playc->destination.ri < 0)
    {
      seterr ("morphview:Invalid viewpoint");
      uih->playc->destination.rr = uih->playc->destination.ri = 0.0001;
    }
  uih->playc->morph = 1;
}
void
uih_playmove (struct uih_context *uih)
{
  uih->playc->source = uih->fcontext->s;
  uih->playc->destination.cr = getfloat (uih);
  uih->playc->destination.ci = getfloat (uih);
  uih->playc->destination.rr = uih->fcontext->s.rr;
  uih->playc->destination.ri = uih->fcontext->s.ri;
  if (uih->playc->destination.rr < 0 || uih->playc->destination.ri < 0)
    {
      seterr ("morphview:Invalid viewpoint");
      uih->playc->destination.rr = uih->playc->destination.ri = 0.0001;
    }
  uih->playc->morph = 1;
}
void
uih_playmorphjulia (struct uih_context *uih, number_t r, number_t i)
{
  uih->playc->sr = uih->fcontext->pre;
  uih->playc->si = uih->fcontext->pim;
  uih->playc->dr = r;
  uih->playc->di = i;
  uih->playc->morphjulia = 1;
}
void
uih_playmorphangle (struct uih_context *uih, number_t angle)
{
  uih->playc->morphangle = 1;
  uih->playc->srcangle = uih->fcontext->angle;
  uih->playc->destangle = angle;
}
void
uih_playautorotate (struct uih_context *uih, int mode)
{
  if (mode && uih->fastrotate)
    uih_rotatemode (uih, ROTATE_CONTINUOUS);
  else
    uih_rotatemode (uih, ROTATE_NONE);
}
void
uih_playfilter (struct uih_context *uih)
{
  char *fname = getkeystring (uih);
  int mode;
  int i;
  for (i = 0; i < uih_nfilters; i++)
    {
      if (!strcmp (uih_filters[i]->shortname, fname))
	{
	  mode = getbool (uih);
	  if (mode)
	    uih_enablefilter (uih, i);
	  else
	    uih_disablefilter (uih, i);
	  return;
	}
    }
  mode = getbool (uih);
  seterr ("Unknown filter");
}
void
uih_playpalette (struct uih_context *uih)
{
  int n1 = getint (uih);
  int n2 = getint (uih);
  int shift = getint (uih);
  if (n1 < 1 || n1 > PALGORITHMS)
    {
      seterr ("Unknown palette type");
    }
  if (uih->zengine->fractalc->palette == NULL)
    return;
  if (mkpalette (uih->zengine->fractalc->palette, n2, n1 - 1) != 0)
    {
      uih_newimage (uih);
    }
  uih->palettetype = n1;
  uih->palettechanged = 1;
  uih->paletteseed = n2;
  if (shiftpalette (uih->zengine->fractalc->palette, shift))
    {
      uih_newimage (uih);
    }
  uih->paletteshift = shift;
}
void
uih_playdefpalette (struct uih_context *uih, int shift)
{
  if (uih->zengine->fractalc->palette == NULL)
    return;
  if (mkdefaultpalette (uih->zengine->fractalc->palette) != 0)
    {
      uih_newimage (uih);
    }
  uih->palettetype = 0;
  uih->palettechanged = 1;
  if (shiftpalette (uih->zengine->fractalc->palette, shift))
    {
      uih_newimage (uih);
    }
  uih->paletteshift = shift;
}
void
uih_zoomcenter (struct uih_context *uih, number_t p1, number_t p2)
{
  uih->xcenter = p1;
  uih->ycenter = p2;
  uih->xcenterm = INT_MAX;
  uih->ycenterm = INT_MAX;
}
extern char *xtextposnames[];
extern char *ytextposnames[];
void
uih_playtextpos (struct uih_context *uih)
{
  int x, y;
  x = getnstring (uih, xtextposnames);
  y = getnstring (uih, ytextposnames);
  uih_settextpos (uih, x, y);
}
void
uih_playusleep (struct uih_context *uih, int time)
{
  parsenext = 0;
  uih->playc->frametime = time;
  if (time < tl_lookup_timer (uih->playc->timer) /*&&((!uih->step)||(!uih->zoomactive)) */ )
    {
      tl_slowdown_timer (uih->playc->timer, time);
      uih->playc->playframe++;
    }
  else
    {
      tl_set_interval (uih->playc->timer, time);
      if (!uih->playc->timerin)
	{
	  uih->playc->timerin = 1;
	  tl_add_timer (syncgroup, uih->playc->timer);
	}
      else
	printf ("Internal program error #12 %i\n", uih->playc->playframe);
    }
  uih->playc->starttime = tl_lookup_timer (uih->playc->timer);
}
void
uih_playtextsleep (struct uih_context *uih)
{
  uih_playusleep (uih, 500000 + 1000000 * (uih->nletters + uih->todisplayletters) / uih->letterspersec);
  uih->nletters = 0;
  uih->todisplayletters = 0;
}


void
uih_playwait (struct uih_context *uih)
{
  parsenext = 0;
  if (!uih->uncomplette && !uih->display && !uih->recalculatemode && !uih->displaytext && !uih->clearscreen)
    {
      uih->playc->playframe++;
    }
  else
    {
      uih_setcomplettehandler (uih, handler1, uih);
    }
}
void
uih_playjulia (struct uih_context *uih, int julia)
{
  julia = !julia;
  if (julia != uih->fcontext->mandelbrot)
    {
      uih->fcontext->mandelbrot = julia;
      uih->fcontext->version++;
      uih_newimage (uih);
    }
}
void
uih_playcalculate (struct uih_context *uih)
{
  uih_newimage (uih);
}
void
uih_playzoom (struct uih_context *uih)
{
  uih->zoomactive = 1;
}
void
uih_playunzoom (struct uih_context *uih)
{
  uih->zoomactive = -1;
}
void
uih_playstop (struct uih_context *uih)
{
  uih->zoomactive = 0;
}
void
uih_playmessage (struct uih_context *uih, char *name)
{
  char *message;
  if (catalog == NULL)
    {
      uih_text (uih, "No catalog file loaded");
      return;
    }
  message = find_text (catalog, name);
  if (message == NULL)
    {
      uih_text (uih, "Message not found in catalog file");
      return;
    }
  uih_text (uih, message);
}
void
uih_playinit (struct uih_context *uih)
{
  int i;
  uih->fcontext->version++;
  uih_newimage(uih);
  uih->step = 0;
  uih->speedup = STEP;
  uih->maxstep = MAXSTEP;
  uih_fastrotatedisable (uih);
  uih_disablejulia (uih);
  uih_cycling_off (uih);
  for (i = 0; i < uih_nfilters; i++)
    uih_disablefilter (uih, i);
  uih_setperbutation (uih, 0, 0);
  set_formula (uih->fcontext, 0);
  uih_setperiodicity (uih, 1);
  uih_setmaxiter (uih, 170);
  uih_setincoloringmode (uih, 0);
  uih_setoutcoloringmode (uih, 0);
  uih_setcycling (uih, 30);
  uih_display (uih);
  uih_setfastmode (uih, 2);
  uih_setintcolor (uih, 0);
  uih_setouttcolor (uih, 0);
  uih_setplane (uih, 0);
  uih_setguessing (uih, 3);
  uih_angle (uih, 0);
  uih_rotatemode (uih, 0);
  uih_rotationspeed (uih, 10);
  uih->xtextpos = 1;
  uih->ytextpos = 1;
  if (mkdefaultpalette (uih->zengine->fractalc->palette) != 0)
    {
      uih_newimage (uih);
    }
}
void 
uih_playload (struct uih_context *uih, xio_path file)
{
  xio_file f;
  xio_pathdata tmp;
  if (uih->playc->level == MAXLEVEL)
    {
      seterr ("Include level overflow");
      return;
    }


  xio_addfname(tmp,uih->playc->directory,file);
  f = xio_ropen (tmp);

  if (f == XIO_FAILED)
    {
      seterr ("File not found");
      return;
    }
  uih->playc->prevfiles[uih->playc->level] = uih->playc->file;
  uih->playc->level++;
  uih->playc->file = f;
  uih->playc->line = 1;
}
void
uih_playupdate (struct uih_context *uih)
{
  int len;
  struct keyword *k;
  static char errtext[1024];
  errstring = NULL;
  while (uih->play && uih->playc->playframe && errstring == NULL)
    {
      parsenext = 1;
      uih->playc->playframe--;
      if (uih->playc->morph)
	{
	  uih->fcontext->s = uih->playc->destination;
	  uih_animate_image (uih);
	  uih->playc->morph = 0;
	}
      if (uih->playc->morphangle)
	{
	  uih_angle (uih, uih->playc->destangle);
	  uih->playc->morphangle=0;
	}
      if (uih->playc->morphjulia)
	{
	  uih->fcontext->pre = uih->playc->dr;
	  uih->fcontext->pim = uih->playc->di;
	  uih_animate_image (uih);
	  uih->playc->morphjulia = 0;
	}
      while (!xio_feof (FD) && parsenext && errstring == NULL)
	{
	  first = 1;
	  last = 0;
	  len = gettoken (uih);
	  first = 0;
	  if ((k = in_word_set (token, len)) != NULL)
	    {
	      switch (k->type)
		{
		case GENERIC:
		  {
		    /*void (*callback)(struct uih_context *uih)=k->callback; */
		    ((void (*)(struct uih_context * uih)) k->callback) (uih);
		    break;
		  }
		  /*To avoid expensive creating of wrapper function or complicated
		   *parametter passing code, most of the functions fall into one of
		   *the following categories
		   */
		case PARAM_INTEGER:
		  {
		    ((void (*)(struct uih_context *, int)) k->callback) (uih, getint (uih));
		    break;
		  }
		case PARAM_FLOAT:
		  {
		    ((void (*)(struct uih_context *, number_t)) k->callback) (uih, getfloat (uih));
		    break;
		  }
		case PARAM_KEYSTRING:
		  {
		    ((void (*)(struct uih_context *, char *)) k->callback) (uih, getkeystring (uih));
		    break;
		  }
		case PARAM_STRING:
		  {
		    ((void (*)(struct uih_context *, char *)) k->callback) (uih, getstring (uih));
		    break;
		  }
		case PARAM_NSTRING:
		  {
		    ((void (*)(struct uih_context *, int)) k->callback) (uih, getnstring (uih, k->userdata));
		    break;
		  }
		case PARAM_BOOL:
		  {
		    ((void (*)(struct uih_context *, int)) k->callback) (uih, getbool (uih));
		    break;
		  }
		case PARAM_COORD:
		  {
		    number_t f1 = getfloat (uih);
		    number_t f2 = getfloat (uih);
		    ((void (*)(struct uih_context *, number_t, number_t)) k->callback) (uih, f1, f2);
		    break;
		  }
		}		/*switch */
	    }			/*if k==in_word_set */
	  else
	    {			/*Unknown token. Skip it */
	      /*while(!last&&errstring==NULL) gettoken(uih); */
	      /*For now it is better to display error :) */
	      seterr ("Unknown command");
	    }
	  if (!last)
	    {
	      seterr ("Too many parameters");
	    }
	}			/*while parsenext */
      if ((xio_feof (FD) && parsenext) || errstring)
	{
	  uih_replaydisable (uih);
	}
    }				/*while play&&playframe */
  if (errstring != NULL)
    {
      sprintf (errtext, "Line %i:%-200s", uih->playc->line, errstring);
      uih->errstring = errtext;
    }
}
void
uih_load (struct uih_context *uih, xio_file f, xio_path filename)
{
  nonblockmode = 1;
  uih_replayenable (uih, f, filename);
  uih_replaydisable (uih);
  nonblockmode = 0;
}

#ifndef _plan9_
#ifndef AROS
#include <malloc.h>
#endif
#include <ctype.h>
#include <stdlib.h>
#ifndef AROS
#include <limits.h>
#endif
#include <math.h>
#include <string.h>
#ifdef __EMX__
#include <float.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#endif
#else
#include <u.h>
#include <libc.h>
#include <ctype.h>
#endif
#include <fconfig.h>
#include <filter.h>
#include <ui_helper.h>
#include <plane.h>
#include <timers.h>
#include <zoom.h>
#include "autopilot.h"
#include <archaccel.h>


#define WAITTIME  200000
#define WAITTIME1 2000000
#define uih_palettechg(uih) if(!uih->recalculatemode&&uih->queue->palettechg!=NULL) uih->recalculatemode=UIH_PALETTEDRAW
#include <misc-f.h>

static struct filter *uih_getinstance (struct filteraction *a);
static void uih_destroyinstance (struct filter *f);
static int uih_require (struct filter *f, struct requirements *r);
static int uih_initialize (struct filter *f, struct initdata *i);

static struct filteraction ui_filter =
{
  "XaoS's user interface layer",
  "ui",
  0,
  uih_getinstance,
  uih_destroyinstance,
  NULL,
  uih_require,
  uih_initialize,
  convertupgeneric,
  convertdowngeneric,
  NULL
};

static uih_context *uih;
static int waitcount, waitcount1;

extern struct filteraction interlace_filter, stereogram_filter, subwindow_filter,
  smalliter_filter, julia_filter, blur_filter, edge_filter, edge2_filter,
  rotate_filter, starfield_filter, truecolor_filter, fixedcolor_filter,
  bitmap_filter, emboss_filter, palette_filter;

struct filteraction *uih_filters[MAXFILTERS] =
{
  &edge_filter,
  &edge2_filter,
  &starfield_filter,
  &stereogram_filter,
  &interlace_filter,
  &blur_filter,
  &emboss_filter,
  &palette_filter,
  &truecolor_filter,
  NULL
};

int uih_nfilters = 9;

static void
uih_invalidatepos (uih_context * uih)
{
  uih->xcenterm = INT_MAX;
  uih->xcenterm = INT_MAX;
}
static void
uih_finishpalette (struct uih_context *uih)
{
  if (uih->image->palette->flags & UNFINISHED)
    {
      if (uih->image->palette->allocfinished != NULL)
	uih->image->palette->allocfinished (uih->image->palette);
      uih->image->palette->flags &= ~UNFINISHED;
    }
}

static void
uih_getcoord (uih_context * uih, int x, int y, number_t * xr, number_t * yr)
{
  uih->uifilter->action->convertdown (uih->uifilter, &x, &y);
  *xr = (((number_t) (uih->fcontext->rs.nc + (x) * ((uih->fcontext->rs.mc - uih->fcontext->rs.nc) / (number_t) uih->zengine->image->width))));
  *yr = (((number_t) (uih->fcontext->rs.ni + (y) * ((uih->fcontext->rs.mi - uih->fcontext->rs.ni) / (number_t) uih->zengine->image->height))));
  rotateback (*(uih->fcontext), *xr, *yr);
}
int
uih_enablefilter (uih_context * c, int n)
{
  if (c->filter[n] == NULL)
    {
      struct filter *f, *f1;
      int i, wascycling = 0;
      if (uih->cycling)
	uih_cycling_off (c), wascycling = 1;
      f = uih_filters[n]->getinstance (uih_filters[n]);
      f1 = c->uifilter;
      if (c->fixedcolor != NULL)
	f1 = c->fixedcolor;
      for (i = MAXFILTERS - 1; i > n; i--)
	{
	  if (c->filter[i])
	    f1 = c->filter[i];
	}
      uih_newimage (c);
      insertfilter (f, f1);
      if (!initqueue (c->queue))
	{
	  removefilter (f);
	  f->action->destroyinstance (f);
	  if (!initqueue (c->queue))
	    {
	      printf ("Fatal error. Can not continue\n");
	      exit (-1);
	    }
	  if (wascycling)
	    uih_cycling_on (c);
	  return 0;
	}
      else
	c->filter[n] = f;
      if (wascycling)
	uih_cycling_on (c);
      return 1;
    }
  return 0;
}
int
uih_fastrotateenable (uih_context * c)
{
  int wascycling = 0;
  if (!c->fastrotate && !c->juliamode)
    {
      if (uih->cycling)
	uih_cycling_off (c), wascycling = 1;
      c->rotatef = rotate_filter.getinstance (&rotate_filter);
      if (c->rotatef == NULL)
	goto end;
      uih_newimage (c);
      addfilter (c->rotatef, c->zengine);
      if (!initqueue (c->queue))
	goto end2;
      if (wascycling)
	uih_cycling_on (c);
      c->fastrotate = 1;
      return 1;
    }
  return 0;
end2:
  removefilter (c->rotatef);
  initqueue (c->queue);
  c->rotatef->action->destroyinstance (c->rotatef);
end:
  if (wascycling)
    uih_cycling_on (c);
  return 0;
}
void
uih_fastrotatedisable (uih_context * c)
{
  if (c->fastrotate)
    {
      int wascycling = 0;
      if (uih->cycling)
	uih_cycling_off (c), wascycling = 1;
      c->fastrotate = 0;
      removefilter (c->rotatef);
      initqueue (c->queue);
      c->rotatef->action->destroyinstance (c->rotatef);
      c->rotatemode = ROTATE_NONE;
      uih_newimage (c);
      if (wascycling)
	uih_cycling_on (c);
    }
}
static int
uih_fixedcolorenable (uih_context * c)
{
#ifdef SCONVERTORS
  int wascycling = 0;
  if (c->fixedcolor == NULL)
    {
      struct filteraction *fa = NULL;
      switch (c->image->palette->type)
	{
#ifdef SFIXEDCOLOR
	case FIXEDCOLOR:
	  fa = &fixedcolor_filter;
	  break;
#endif
#ifdef SBITMAPS
	case LBITMAP:
	case MBITMAP:
	case LIBITMAP:
	case MIBITMAP:
	  fa = &bitmap_filter;
	  break;
#endif
	default:
	  printf ("Unsupported image type. Recompile XaoS\n");
	  return 0;
	}
      if (uih->cycling)
	uih_cycling_off (c), wascycling = 1;
      c->fixedcolor = fa->getinstance (fa);
      if (c->fixedcolor == NULL)
	goto end;
      uih_newimage (c);
      addfilter (c->fixedcolor, c->uifilter->previous);
      if (!initqueue (c->queue))
	goto end2;
      if (wascycling)
	uih_cycling_on (c);
      return 1;
    }
  return 0;
end2:
  removefilter (c->fixedcolor);
  c->fixedcolor->action->destroyinstance (c->fixedcolor);
  c->fixedcolor = NULL;
  initqueue (c->queue);
end:
  if (wascycling)
    uih_cycling_on (c);
  return 0;
#else
  printf ("Fixed color not supported, please recompile XaoS\n");
  return 0;
#endif
}
void
uih_fixedcolordisable (uih_context * c)
{
#ifdef SCONVERTORS
  if (c->fixedcolor != NULL)
    {
      int wascycling = 0;
      if (uih->cycling)
	uih_cycling_off (c), wascycling = 1;
      removefilter (c->fixedcolor);
      initqueue (c->queue);
      c->fixedcolor->action->destroyinstance (c->fixedcolor);
      c->fixedcolor = NULL;
      uih_newimage (c);
      if (wascycling)
	uih_cycling_on (c);
    }
#endif
}
int
uih_fastrotate (uih_context * c, int mode)
{
  if (mode)
    return (uih_fastrotateenable (c));
  uih_fastrotatedisable (c);
  return 1;
}
void
uih_angle (uih_context * c, number_t angle)
{
  if (angle != c->fcontext->angle)
    {
      if (!c->fastrotate)
	{
	  c->fcontext->version++;
	  uih_newimage (c);
	}
      c->fcontext->angle = angle;
      uih_animate_image (c);
    }
}
void
uih_rotatemode (uih_context * c, int mode)
{
  if (c->fastrotate)
    {
      if (c->rotatemode != mode)
	{
	  c->rotatemode = mode;
	  if (mode == ROTATE_CONTINUOUS)
	    tl_reset_timer (uih->doittimer);
	}
    }
}

int
uih_enablejulia (uih_context * c)
{
  int wascycling = 0;
  if (!c->juliamode&&c->fcontext->mandelbrot/*&&c->fcontext->currentformula->calculate_julia != NULL*/)
    {
      struct filter *addf = c->zengine;
      uih_newimage (c);
      if (c->fastrotate)
	addf = c->rotatef;
      if (uih->cycling)
	uih_cycling_off (c), wascycling = 1;
      if(c->fcontext->currentformula->calculate_julia == NULL) 
        c->julia = zoom_filter.getinstance (&zoom_filter);
      else
        c->julia = julia_filter.getinstance (&julia_filter);
      if (c->julia == NULL)
	goto end;

      c->subwindow = subwindow_filter.getinstance (&subwindow_filter);
      if (c->subwindow == NULL)
        goto end2;
      if(c->fcontext->currentformula->calculate_julia != NULL)  {
        c->smalliter = smalliter_filter.getinstance (&smalliter_filter);
        if (c->smalliter == NULL)
	  goto end3;
      } else c->smalliter=NULL;
      uih_fastrotatedisable (c);
      addfilter (c->subwindow, addf);
      if(c->fcontext->currentformula->calculate_julia != NULL)  {
        addfilter (c->smalliter, addf);
      }
      addfilter (c->julia, addf);
      subwindow_setsecond (c->subwindow, addf);
      if (!initqueue (c->queue))
	goto end4;
      if(c->fcontext->currentformula->calculate_julia == NULL) c->juliamode=2; else
         c->juliamode = 1;
      return 1;
    }
  return 0;
end4:;
  removefilter (c->subwindow);
  removefilter (c->julia);
  if(c->smalliter!=NULL) removefilter (c->smalliter);
  initqueue (c->queue);
end3:;
  c->smalliter->action->destroyinstance (c->smalliter);
end2:;
  c->subwindow->action->destroyinstance (c->subwindow);
end:;
  c->julia->action->destroyinstance (c->julia);
  if (wascycling)
    uih_cycling_on (c);
  return 0;
}
void
uih_disablejulia (uih_context * c)
{
  int wascycling = 0;
  if (uih->juliamode)
    {
      uih_newimage (c);
      uih->fcontext->version++;
      if (uih->cycling)
	uih_cycling_off (c), wascycling = 1;
      removefilter (c->subwindow);
      removefilter (c->julia);
      if(c->smalliter!=NULL) removefilter (c->smalliter);
      initqueue (c->queue);
      if(c->smalliter!=NULL) c->smalliter->action->destroyinstance (c->smalliter);
      c->subwindow->action->destroyinstance (c->subwindow);
      c->julia->action->destroyinstance (c->julia);
      if (wascycling)
	uih_cycling_on (c);
      c->juliamode = 0;
    }
}
int
uih_setjuliamode (uih_context * c, int mode)
{
  if (mode)
    return uih_enablejulia (c);
  uih_disablejulia (c);
  return 1;
}
void
uih_rotationspeed (uih_context * c, number_t speed)
{
  c->rotationspeed = speed;
}
void
uih_disablefilter (uih_context * c, int n)
{
  if (c->filter[n] != NULL)
    {
      int wascycling = 0;
      struct filter *f = c->filter[n];
      /*c->datalost = 1; */
      if (uih->cycling)
	uih_cycling_off (c), wascycling = 1;
      uih_newimage (c);
      removefilter (f);
      f->action->destroyinstance (f);
      if (!initqueue (c->queue))
	{
	  printf ("Fatal error. Can not continue\n");
	  exit (-1);
	}
      if (wascycling)
	uih_cycling_on (c);
    }
  c->filter[n] = NULL;
}
static void
uih_cyclinghandler (void *userdata, int n)
{
  struct uih_context *uih = userdata;
  int direct;
  if (uih->zengine->fractalc->palette != NULL &&
      uih->zengine->fractalc->palette->cyclecolors == NULL)
    return;
  direct = uih->direction * n;
  if (direct > 0)
    direct %= uih->zengine->fractalc->palette->size - 1;
  else
    direct = -((-direct) % (uih->zengine->fractalc->palette->size - 1));
  if (direct)
    {
      uih->paletteshift += direct;
      while (uih->paletteshift < 0)
	uih->paletteshift += uih->zengine->fractalc->palette->size - 1;
      uih->paletteshift = uih->paletteshift % (uih->zengine->fractalc->palette->size - 1);
      uih->zengine->fractalc->palette->cyclecolors (uih->zengine->fractalc->palette, direct);
      if (uih->flags & UPDATE_AFTER_PALETTE && (!uih->play || !uih->nonfractalscreen))
	uih->display = 1;
      uih_palettechg (uih);
    }
}
void
uih_cycling_off (struct uih_context *c)
{
  if (c->cycling)
    {
      tl_free_timer (c->cyclingtimer);
      c->cycling = 0;
    }
}
void
uih_display (struct uih_context *c)
{
  c->display = 1;
  c->nonfractalscreen = 0;
  if (c->clearscreen)
    c->clearscreen = 0;
  if (c->displaytext)
    c->displaytext = 0, free (c->text);
}
void
uih_cycling_stop (struct uih_context *c)
{
  if (c->cycling && !c->stopped)
    {
      tl_remove_timer (c->cyclingtimer);
      c->stopped = 1;
    }
}
void
uih_cycling_continue (struct uih_context *c)
{
  if (c->cycling && c->stopped)
    {
      c->stopped = 0;
      tl_add_timer (syncgroup, c->cyclingtimer);
    }
}
char *
uih_save (struct uih_context *c, xio_path filename)
{
  xio_file f;
  char *r;
  uih_cycling_stop (c);
  uih_stoptimers (c);
  f = xio_wbopen (filename);
  r = (char *) writepng (f, uih->queue->saveimage);
  uih_cycling_continue (c);
  uih_resumetimers(c);
  return (r);
}
void
uih_setcycling (struct uih_context *c, int speed)
{
  c->cyclingspeed = speed;
  if (c->cyclingspeed < 0)
    c->direction = -1;
  else
    c->direction = 1;
  if (c->cycling)
    tl_set_interval (c->cyclingtimer, 1000000 / c->cyclingspeed * c->direction);
}
int
uih_cycling_on (struct uih_context *c)
{
  if (uih->zengine->fractalc->palette != NULL &&
      uih->zengine->fractalc->palette->cyclecolors != NULL)
    {
      c->cycling = 1;
      tl_update_time ();
      c->cyclingtimer = tl_create_timer ();
      uih_setcycling (uih, c->cyclingspeed);
      tl_set_multihandler (c->cyclingtimer, uih_cyclinghandler, c);
      tl_add_timer (syncgroup, c->cyclingtimer);
    }
  else
    {
      return 0;
    }
  return 1;
}
int
uih_cycling (struct uih_context *uih, int mode)
{
  if (mode)
    return (uih_cycling_on (uih));
  uih_cycling_off (uih);
  return 1;
}



static void
uih_waitfunc (struct filter *f)
{
  int l;
  tl_process_group (syncgroup, NULL);
  l = tl_lookup_timer (uih->calculatetimer);
  if (uih->interrupt)
    f->interrupt = 1, uih->endtime = l;
  if (uih->interruptiblemode)
    {
      if (f->incalculation && !uih->starttime)
	uih->starttime = l;
      else if (uih->starttime && !f->incalculation && !uih->endtime)
	uih->endtime = l;
      if ((uih->maxtime && l > uih->maxtime && f->readyforinterrupt) || uih->interrupt)
	f->interrupt = 1, uih->endtime = l;
    }
  if ((l) > (waitcount + 1) * WAITTIME)
    {
      int display = 0;
      if (f->image == uih->image && !uih->interruptiblemode && l > (waitcount1 + 1) * WAITTIME1)
	{
	  if (!uih->play)
	    display = 1, uih_finishpalette (uih);
	  waitcount1++;
	}
      if (uih->passfunc != NULL)
	f->interrupt |= uih->passfunc (uih, display, f->pass, (float) (f->max ? f->pos * 100.0 / f->max : 100.0));
      waitcount++;
    }
}

void
uih_do_fractal (uih_context * c)
{
  int flags;
  int time;

  uih->interrupt = 0;
  uih = c;
  if (c->juliamode&&!c->fcontext->mandelbrot) {
     uih_disablejulia(c);
  }
  if ((c->juliamode==1&&c->fcontext->currentformula->calculate_julia==NULL)||
      (c->juliamode==2&&c->fcontext->currentformula->calculate_julia!=NULL)) {
     uih_disablejulia(c);
     uih_enablejulia(c);
     uih->fcontext->version++;
  }

  tl_update_time ();
  if (uih->recalculatemode < uih->fastmode)
    uih->interruptiblemode = 1;
  else
    uih->interruptiblemode = 0;
  if (!uih->interruptiblemode && uih->recalculatemode > UIH_ANIMATION)
    {
      if (uih->longwait != NULL)
	uih->longwait (uih);
      uih_stoptimers (uih);
    }

  tl_reset_timer (uih->calculatetimer);
  uih->starttime = 0;
  uih->endtime = 0;
  waitcount = tl_lookup_timer (uih->maintimer) / WAITTIME + 2;
  waitcount1 = tl_lookup_timer (uih->maintimer) / WAITTIME1 + 1;
  uih->incalculation = 1;

  if (!(uih->flags & ROTATE_INSIDE_CALCULATION))
    uih_cycling_stop (uih);

  time = tl_lookup_timer (uih->doittimer);
  if (uih->rotatemode == ROTATE_CONTINUOUS)
    {
      uih->fcontext->angle += uih->rotationspeed * time / 1000000.0;
    }

  tl_reset_timer (uih->doittimer);
  uih->indofractal=1;
  if (uih->recalculatemode < UIH_PALETTEDRAW)
    {
      if (uih->queue->palettechg != NULL)
	flags = uih->queue->palettechg->action->doit (uih->queue->palettechg, PALETTEONLY, 0);
      else
	flags = CHANGED;
    }
  else
    flags = uih->uifilter->previous->action->doit (uih->uifilter->previous, uih->interruptiblemode ? INTERRUPTIBLE : 0, time);
  uih->indofractal=0;

  if (!(uih->flags & ROTATE_INSIDE_CALCULATION))
    uih_cycling_continue (uih);

  uih->dirty = 0;
  if (uih->inanimation)
    uih->inanimation--;
  uih->ddatalost = 0;
  uih->recalculatemode = 0;

  if (flags & ANIMATION)
    uih->fastanimation = 1;
  else
    uih->fastanimation = 0;
  if (flags & (ANIMATION | UNCOMPLETTE) || (uih->rotatemode == ROTATE_CONTINUOUS))
    {
      tl_resume_timer (uih->doittimer);
      uih->uncomplette = 1, uih->inanimation = 2, uih->recalculatemode = UIH_ANIMATION;
    }
  else
    {
      tl_stop_timer (uih->doittimer);
      uih->uncomplette = 0;
    }
  if ((flags & CHANGED) && (!uih->play || !uih->nonfractalscreen))
    {
      uih->display = 1;
      if (flags & INEXACT)
	uih->dirty = 1;
    }
  else
    uih->incalculation = 0;
  uih_callcomplette (uih);
}
void
uih_callcomplette (uih_context * c)
{
  if (!c->uncomplette && !c->display && !c->recalculatemode && !c->inanimation /*&&!c->displaytext&&!c->clearscreen */  && c->complettehandler != NULL)
    {
      uih->complettehandler (uih->handlerdata);
    }
}
void
uih_setcomplettehandler (uih_context * c, void (h) (void *), void *d)
{
  uih->complettehandler = h;
  uih->handlerdata = d;
}

void
uih_letterspersec (uih_context * c, int n)
{
  if (n < 1)
    n = 1;
  c->letterspersec = n;
}
double
uih_displayed (uih_context * c)
{
  int drawingtime;
  uih_finishpalette (c);
  if(uih->indofractal) return 0; /*image is currently calculating*/
  uih->display = 0;
  tl_update_time ();
  uih_resumetimers (uih);
  c->nonfractalscreen = 0;
  c->nletters = 0;
  if (uih->incalculation)
    {
      uih->incalculation = 0;
      drawingtime = tl_lookup_timer (uih->calculatetimer);
      if (uih->lasttime == -1 || (drawingtime && uih->lasttime && (drawingtime / uih->lasttime < 0.2 || drawingtime / uih->lasttime > 4)))
	uih->lasttime = drawingtime;
      uih->lasttime = (uih->lasttime * 30 + drawingtime) / 31;
      uih->lastspeed = uih->lasttime ? 1000000.0 / uih->lasttime : 100.0;
      if (uih->interruptiblemode)
	{
	  int i;
	  int time1, time;
	  time1 = drawingtime;
	  time1 -= uih->endtime;
	  time = (drawingtime - uih->endtime) + uih->starttime;
	  if (uih->times[0][0] == -1)
	    {
	      for (i = 0; i < AVRGSIZE; i++)
		uih->times[0][i] = time, uih->times[1][i] = time1;
	      uih->count[0] = time * AVRGSIZE, uih->count[1] = time1 * AVRGSIZE;
	    }
	  uih->timespos = (uih->timespos + 1) % AVRGSIZE;
	  uih->count[0] += time - uih->times[0][uih->timespos];
	  uih->count[1] += time1 - uih->times[1][uih->timespos];
	  uih->times[0][uih->timespos] = time;
	  uih->times[1][uih->timespos] = time1;
	  uih->maxtime = (uih->count[0] * 5) / AVRGSIZE;
	  if (uih->step || uih->pressed || (uih->play && (uih->playc->morph || uih->playc->morphangle || uih->playc->morphjulia)) || (uih->rotatemode == ROTATE_CONTINUOUS) || uih->fastanimation)
	    {
	      if (uih->maxtime > 1000000 / 25)
		uih->maxtime = uih->count[0] * 3 / AVRGSIZE;
	      if (uih->maxtime > 1000000 / 15)
		uih->maxtime = 1000000 / 15;
	    }
	  else
	    {
	      uih->maxtime = 1000000 / 3;
	    }
	  if (uih->maxtime < 1000000 / 30)
	    uih->maxtime = 1000000 / 30;
	  uih->maxtime -= uih->count[1] / AVRGSIZE;
	  if (uih->maxtime < uih->starttime + 10000)
	    uih->maxtime = uih->starttime + 10000;
	}
    }
  uih_callcomplette (uih);
  return (uih->lastspeed);
}

void
uih_text (uih_context * c, char *text)
{
  int i, l;
  if (c->displaytext)
    free (c->text);
  c->text = strdup (text);
  l = strlen (text);
  c->todisplayletters = 0;
  for (i = 0; i < l; i++)
    {
      if (isalnum (text[i]))
	c->todisplayletters++;
      if (text[i] == '-')
	c->todisplayletters += 3;
      if (text[i] == '.')
	c->todisplayletters += 2;
    }
  c->step = 0;
  c->displaytext = 1;
  if (c->save)
    {
      if (c->savec->displaytext)
	free (c->text);
      c->savec->text = strdup (text);
      c->savec->displaytext = 1;
    }
}
void
uih_clearscreen (uih_context * c)
{
  c->clearscreen = 1;
  if (c->save)
    c->savec->clearscreen = 1;
  if (c->displaytext)
    c->displaytext = 0, free (c->text);
}
void
uih_settextpos (uih_context * c, int x, int y)
{
  c->xtextpos = x;
  c->ytextpos = y;
}


/*timming routines */

void
uih_tbreak (uih_context * c)
{
  c->tbreak = 1;
}

void
uih_stoptimers (uih_context * c)
{
  if (!c->stoppedtimers)
    {
      c->stoppedtimers = 1;
      c->display = 1;
      tl_stop_timer (c->maintimer);
      tl_stop_timer (c->doittimer);
      if (c->autopilot)
	tl_stop_timer (c->autopilottimer);
      if (c->play)
	{
	  tl_stop_timer (c->playc->timer);
	  if (c->cycling)
	    tl_stop_timer (c->cyclingtimer);
	}
      if (c->save)
	{
	  tl_stop_timer (c->savec->timer);
	  if (c->cycling)
	    tl_stop_timer (c->cyclingtimer);
	}
    }
}
void
uih_slowdowntimers (uih_context * c, int time)
{
  tl_slowdown_timer (c->maintimer, time);
  if (c->autopilot)
    tl_slowdown_timer (c->autopilottimer, time);
  if (c->play)
    {
      tl_slowdown_timer (c->playc->timer, time);
      if (c->cycling)
	tl_slowdown_timer (c->cyclingtimer, time);
    }
  if (c->save)
    {
      tl_slowdown_timer (c->savec->timer, time);
      if (c->cycling)
	tl_slowdown_timer (c->cyclingtimer, time);
    }
}
void
uih_resumetimers (uih_context * c)
{
  if (c->stoppedtimers)
    {
      c->stoppedtimers = 0;
      tl_resume_timer (c->maintimer);
      if (c->cycling)
	tl_resume_timer (c->cyclingtimer);
      if (c->autopilot)
	tl_resume_timer (c->autopilottimer);
      if (c->play)
	{
	  tl_resume_timer (c->playc->timer);
	}
      if (c->save)
	{
	  tl_resume_timer (c->savec->timer);
	}
    }
}

/*autopilot implementation */

static void
uih_changed (void)
{
  uih_newimage (uih);
}

static void
uih_autopilothandler (void *uih1, int n)
{
  struct uih_context *uih = uih1;
  do_autopilot (uih, &uih->autopilotx, &uih->autopiloty, &uih->autopilotbuttons, uih_changed, n);
}
static INLINE void
uih_zoom (uih_context * uih)
{
  uih->step += uih->speedup * 2 * uih->mul;
  if (uih->step > uih->maxstep)
    uih->step = uih->maxstep;
  else if (uih->step < -uih->maxstep)
    uih->step = -uih->maxstep;
}
static INLINE void
uih_unzoom (uih_context * uih)
{
  uih->step -= uih->speedup * 2 * uih->mul;
  if (uih->step > uih->maxstep)
    uih->step = uih->maxstep;
  else if (uih->step < -uih->maxstep)
    uih->step = -uih->maxstep;
}
static INLINE void
uih_slowdown (uih_context * uih)
{
  if (uih->step > 0)
    {
      if (uih->step < uih->speedup * uih->mul)
	uih->step = 0;
      else
	uih->step -= uih->speedup * uih->mul;
    }
  else if (uih->step < 0)
    {
      if (uih->step > -uih->speedup * uih->mul)
	uih->step = 0;
      else
	uih->step += uih->speedup * uih->mul;
    }
}
static INLINE void
uih_zoomupdate (uih_context * uih)
{
  number_t x;
  number_t y;
  number_t mmul = pow ((double) (1 - uih->step), (double) uih->mul);
  number_t mc = uih->fcontext->s.cr - uih->fcontext->s.rr / 2;
  number_t nc = uih->fcontext->s.cr + uih->fcontext->s.rr / 2;
  number_t mi = uih->fcontext->s.ci - uih->fcontext->s.ri / 2;
  number_t ni = uih->fcontext->s.ci + uih->fcontext->s.ri / 2;
  x = uih->xcenter, y = uih->ycenter;
  mc = x + (mc - x) * (mmul);
  nc = x + (nc - x) * (mmul);
  mi = y + (mi - y) * (mmul);
  ni = y + (ni - y) * (mmul);
  uih->fcontext->s.rr = nc - mc;
  uih->fcontext->s.ri = ni - mi;
  uih->fcontext->s.cr = (nc + mc) / 2;
  uih->fcontext->s.ci = (ni + mi) / 2;
  uih_animate_image (uih);
}


/*main uih loop */

int
uih_update (uih_context * c, int mousex, int mousey, int mousebuttons)
{
  int inmovement = 0;
  int slowdown = 1;
  int time;
  uih = c;

  if (mousebuttons == (BUTTON1 | BUTTON3))
    mousebuttons = BUTTON2;
  tl_process_group (syncgroup, NULL);	/*First we need to update timming */
  tl_update_time ();
  time = tl_lookup_timer (uih->maintimer);
  if(!uih->inanimation) time=0;
  if (time > 2000000)
    {
      uih_slowdowntimers (uih, time - 2000000);
      time = 2000000;
    }
  if (uih->inanimation)
    uih->inanimation--;
  tl_reset_timer (uih->maintimer);
  uih->mul = (double) time / FRAMETIME;
  if (uih->tbreak)
    uih->mul = 1, uih->tbreak--;
  if (uih->mul == 0)
    uih->mul = 0.00000001;
  if (uih->play)
    {
      uih_playupdate (uih);
      if (!uih->play)
	{
	  uih->inanimation = 2;
	  return 1;
	}
      if (!uih->juliamode)
	{
	  if (uih->step)
	    uih_zoomupdate (uih), inmovement = 1;
	  switch (uih->zoomactive)
	    {
	    case 1:
	      uih_zoom (uih), inmovement = 1;
	      break;
	    case -1:
	      uih_unzoom (uih), inmovement = 1;
	      break;
	    default:
	      uih_slowdown (uih);
	    }
	  if (uih->playc->morph)
	    {
	      number_t mmul = (tl_lookup_timer (uih->playc->timer) - uih->playc->starttime) / (number_t) (uih->playc->frametime - uih->playc->starttime);
	      number_t srr, drr;
	      number_t mmul1;
/*        printf("%i %i %i %f\n",tl_lookup_timer(uih->playc->timer),uih->playc->starttime,uih->playc->frametime,(double)mmul); */
	      if (uih->playc->source.rr * uih->fcontext->windowwidth > uih->playc->source.ri * uih->fcontext->windowheight)
		srr = uih->playc->source.rr;
	      else
		srr = uih->playc->source.ri;
	      if (uih->playc->destination.rr * uih->fcontext->windowwidth > uih->playc->destination.ri * uih->fcontext->windowheight)
		drr = uih->playc->destination.rr;
	      else
		drr = uih->playc->destination.ri;
	      if (srr == drr)
		mmul1 = mmul;
	      else
		mmul1 = (exp (log (srr) + ((log (drr) - log (srr)) * mmul)) - srr) / (drr - srr);
	      if (mmul1 > 1)
		mmul1 = 1;
	      if (mmul1 < 0)
		mmul1 = 0;
	      inmovement = 1;
	      uih->fcontext->s.rr = uih->playc->source.rr + (uih->playc->destination.rr - uih->playc->source.rr) * mmul1;
	      uih->fcontext->s.ri = uih->playc->source.ri + (uih->playc->destination.ri - uih->playc->source.ri) * mmul1;
	      uih->fcontext->s.cr = uih->playc->source.cr + (uih->playc->destination.cr - uih->playc->source.cr) * mmul1;
	      uih->fcontext->s.ci = uih->playc->source.ci + (uih->playc->destination.ci - uih->playc->source.ci) * mmul1;
	      uih_animate_image (uih);
	    }
	}
      else
	{
	  if (uih->playc->morphjulia)
	    {
	      number_t mmul = (tl_lookup_timer (uih->playc->timer) - uih->playc->starttime) / (number_t) (uih->playc->frametime - uih->playc->starttime);
	      uih->fcontext->pre = uih->playc->sr + (uih->playc->dr - uih->playc->sr) * mmul;
	      uih->fcontext->pim = uih->playc->si + (uih->playc->di - uih->playc->si) * mmul;
	      uih_animate_image (uih);
	      inmovement = 1;
	    }
	}
      if (uih->playc->morphangle)
	{
	  number_t mmul = (tl_lookup_timer (uih->playc->timer) - uih->playc->starttime) / (number_t) (uih->playc->frametime - uih->playc->starttime);
	  uih_angle (uih, uih->playc->srcangle + (uih->playc->destangle - uih->playc->srcangle) * mmul);
	  inmovement = 1;
	}
    }
  else
    {
      if (!uih->juliamode)
	{
	  if (uih->autopilot)
	    {			/*now handle autopilot */
	      tl_process_group (uih->autopilotgroup, NULL);
	      mousex = uih->autopilotx;
	      mousey = uih->autopiloty;
	      mousebuttons = uih->autopilotbuttons;
	      inmovement = 1;
	    }
	  if (uih->step)
	    {
	      number_t x;
	      number_t y;
	      if (mousex != uih->xcenterm || mousey != uih->ycenterm)
		{
		  uih->xcenterm = mousex;
		  uih->ycenterm = mousey;
		  uih_getcoord (uih, mousex, mousey, &x, &y);
		  uih->xcenter = x;
		  uih->ycenter = y;
		}
	      uih_zoomupdate (uih), inmovement = 1;
	    }
	  uih->zoomactive = 0;
	  if (uih->rotatemode != ROTATE_MOUSE)
	    switch (mousebuttons)
	      {			/*process buttons */
	      case BUTTON1:
		uih->zoomactive = 1;
		inmovement = 1;
		break;
	      case BUTTON3:
		uih->zoomactive = -1;
		inmovement = 1;
		break;
	      }
	  uih_saveframe (uih);
	  if (uih->rotatemode != ROTATE_MOUSE)
	    {
	      uih->rotatepressed = 0;
	      switch (mousebuttons)
		{		/*process buttons */
		case BUTTON1:
		  uih_zoom (uih), slowdown = 0;
		  break;
		case BUTTON3:
		  uih_unzoom (uih), slowdown = 0;
		  break;
		case BUTTON2:
		  {
		    number_t x, y;
		    uih_getcoord (uih, mousex, mousey, &x, &y);
		    if (uih->pressed && (uih->oldx != x || uih->oldy != y))
		      {
			uih->fcontext->s.cr -= x - uih->oldx;
			uih->fcontext->s.ci -= y - uih->oldy;
			uih_animate_image (uih);
			uih->moved = 1;
		      }
		    uih->pressed = 1;
		    uih->speed = 0;
		    update_view (uih->fcontext);
		    uih_getcoord (uih, mousex, mousey, &uih->oldx, &uih->oldy);
		  }
		  break;
		}
	    }
	  else
	    {
	      if (mousebuttons & BUTTON1)
		{
		  number_t x, y;
		  number_t angle;

		  x = (mousex - uih->image->width / 2) * uih->image->pixelwidth;
		  y = (mousey - uih->image->height / 2) * uih->image->pixelheight;
		  angle = -atan2 (x, y) * 180 / M_PI;
		  if (uih->rotatepressed)
		    {
		      uih_angle (uih, uih->fcontext->angle + angle - uih->oldangle);
		    }
		  uih->rotatepressed = 1;
		  uih->oldangle = angle;
		}
	      else
		uih->rotatepressed = 0;
	    }
	  if (!(mousebuttons & BUTTON2))
	    uih->pressed = 0;
	  if (slowdown)
	    uih_slowdown (uih);
	}
      else
	{
	  if (mousebuttons & BUTTON1)
	    {
	      number_t x, x1 = uih->fcontext->pre;
	      number_t y, y1 = uih->fcontext->pim;
	      uih->zoomactive = 0;
	      uih_getcoord (uih, mousex, mousey, &x, &y);
	      uih->fcontext->pre = x;
	      uih->fcontext->pim = y;
	      uih_saveframe (uih);
	      uih->pressed = 1;
	      recalculate (uih->fcontext->plane, &uih->fcontext->pre, &uih->fcontext->pim);
	      if (uih->fcontext->pre != x1 || uih->fcontext->pim != y1)
		{
		  uih_animate_image (uih);
		}
	    }
	  else
	    uih->pressed = 0;
	}
    }
  if (!inmovement)
    uih_tbreak (uih);
  if (uih->uncomplette)
    inmovement = 1;
  if (!uih->recalculatemode && !uih->display)
    uih_finishpalette (uih);
  if (!inmovement)
    uih_callcomplette (uih);
  if (uih->inanimation < inmovement * 2)
    uih->inanimation = inmovement * 2;
  return (inmovement * 2);
}

/*actions that can be used be user interface */

void
uih_autopilot_on (uih_context * c)
{
  if (!c->autopilot)
    {
      clean_autopilot (c);
      uih_autopilothandler (uih,1);
      tl_update_time ();
      uih_resumetimers (uih);
      c->autopilottimer = tl_create_timer ();
      c->autopilotgroup = tl_create_group ();
      tl_set_multihandler (c->autopilottimer, uih_autopilothandler, uih);
      tl_set_interval (c->autopilottimer, 1000000 / 25);
      tl_reset_timer (c->autopilottimer);
      tl_add_timer (c->autopilotgroup, c->autopilottimer);
      tl_update_time();
      c->autopilot = 1;
    }
}
void
uih_autopilot_off (uih_context * c)
{
  if (c->autopilot)
    {
      tl_remove_timer (c->autopilottimer);
      tl_free_timer (c->autopilottimer);
      tl_free_group (c->autopilotgroup);
      c->autopilot = 0;
    }
}
void
uih_mkdefaultpalette (uih_context * c)
{
  if (c->zengine->fractalc->palette == NULL)
    return;
  uih_cycling_stop (uih);
  if (mkdefaultpalette (c->zengine->fractalc->palette) != 0)
    {
      uih_newimage (c);
    }
  uih_palettechg (uih);
  uih->paletteshift = 0;
  uih->palettechanged = 1;
  uih->palettetype = 0;
  uih_finishpalette (uih);
  uih_cycling_continue (uih);
}
void
uih_mkpalette (uih_context * c)
{
  int seed;
  int alg = rand () % PALGORITHMS;
  if (c->zengine->fractalc->palette == NULL)
    return;
  uih_cycling_stop (uih);
  if (mkpalette (c->zengine->fractalc->palette, seed = rand (), alg) != 0)
    {
      uih_newimage (c);
    }
  uih_palettechg (uih);
  uih->paletteshift = 0;
  uih->paletteseed = seed;
  uih_finishpalette (uih);
  uih->palettechanged = 1;
  uih->palettetype = alg + 1;
  uih_cycling_continue (uih);
}
/*Basic inicialization routines */

static void
uih_alloctables (uih_context * c)
{
  c->zengine = zoom_filter.getinstance (&zoom_filter);
  if (c->zengine == NULL)
    return;
  c->fcontext = make_fractalc (0, c->image->pixelwidth * c->image->width, c->image->pixelheight * c->image->height);
  uih = c;
  c->uifilter = ui_filter.getinstance (&ui_filter);
  c->queue = create_queue (c->uifilter);
  insertfilter (c->zengine, c->uifilter);
}
static int
uih_initqueue (uih_context * c)
{
  return (initqueue (c->queue));
}
void
uih_setmaxstep (uih_context * c, number_t p)
{
  c->maxstep = p;
}

void
uih_setspeedup (uih_context * c, number_t p)
{
  c->speedup = p;
}

void
uih_setmaxiter (uih_context * c, int maxiter)
{
  if (maxiter < 1)
    maxiter = 1;
  if (maxiter > 2000000)
    maxiter = 2000000;
  if (c->fcontext->maxiter != (unsigned int) maxiter)
    {
      c->fcontext->maxiter = maxiter;
      c->fcontext->version++;
      uih_newimage (uih);
    }
}
void
uih_setincoloringmode (uih_context * c, int mode)
{
  if (mode < 0)
    mode = 0;
  if (mode > INCOLORING)
    mode = INCOLORING;
  if (c->fcontext->incoloringmode != mode)
    {
      c->fcontext->incoloringmode = mode;
      c->fcontext->version++;
      uih_newimage (uih);
    }
}
void
uih_setintcolor (uih_context * c, int mode)
{
  if (mode < 0)
    mode = 0;
  if (mode > TCOLOR)
    mode = TCOLOR;
  if (c->fcontext->intcolor != mode)
    {
      c->fcontext->intcolor = mode;
      if (c->fcontext->incoloringmode == 10)
	{
	  c->fcontext->version++;
	  uih_newimage (uih);
	}
    }
}
void
uih_setouttcolor (uih_context * c, int mode)
{
  if (mode < 0)
    mode = 0;
  if (mode > TCOLOR)
    mode = TCOLOR;
  if (c->fcontext->outtcolor != mode)
    {
      c->fcontext->outtcolor = mode;
      if (c->fcontext->coloringmode == 10)
	{
	  c->fcontext->version++;
	  uih_newimage (uih);
	}
    }
}
void
uih_setperbutation (uih_context * c, number_t zre, number_t zim)
{
  if (c->fcontext->bre != zre || c->fcontext->bim != zim)
    {
      c->fcontext->bre = zre;
      c->fcontext->bim = zim;
      if (c->fcontext->mandelbrot)
	{
	  c->fcontext->version++;
	  uih_newimage (uih);
	}
    }
}
void
uih_perbutation (uih_context * c, int mousex, int mousey)
{
  number_t r, i;
  uih_getcoord (c, mousex, mousey, &r, &i);
  uih_setperbutation (c, r, i);
}
void
uih_setjuliaseed (uih_context * c, number_t zre, number_t zim)
{
  if (c->fcontext->pre != zre || c->fcontext->pim != zim)
    {
      c->fcontext->pre = zre;
      c->fcontext->pim = zim;
      if (c->juliamode)
	{
	  uih_animate_image (uih);
	}
      else
	{
	  if (!c->fcontext->mandelbrot)
	    uih_newimage (uih);
	}
    }
}
void
uih_setfastmode (uih_context * c, int mode)
{
  if (mode < 0)
    mode = 0;
  c->fastmode = mode;
}
void
uih_setoutcoloringmode (uih_context * c, int mode)
{
  if (mode < 0)
    mode = 0;
  if (mode > OUTCOLORING)
    mode = OUTCOLORING - 1;
  if (c->fcontext->coloringmode != mode)
    {
      c->fcontext->coloringmode = mode;
      c->fcontext->version++;
      uih_newimage (uih);
    }
}
void
uih_setplane (uih_context * c, int mode)
{
  if (mode < 0)
    mode = 0;
  if (mode > PLANES)
    mode = PLANES - 1;
  uih_invalidatepos (uih);
  if (c->fcontext->plane != mode)
    {
      c->fcontext->plane = mode;
      c->fcontext->version++;
      uih_newimage (uih);
    }
}
void
uih_setmandelbrot (uih_context * c, int mode, int mousex, int mousey)
{
  if (mode < 0)
    mode = 0;
  if (mode > 1)
    mode = 1;
  if (c->fcontext->mandelbrot != mode)
    {
      c->fcontext->mandelbrot = mode;
      if (c->fcontext->mandelbrot == 0 && !c->juliamode)
	{
	  uih_getcoord (c, mousex, mousey, &c->fcontext->pre, &c->fcontext->pim);
	  recalculate (c->fcontext->plane, &c->fcontext->pre, &c->fcontext->pim);
	}
      else
	uih_disablejulia (c);
      c->fcontext->version++;
      uih_newimage (uih);
    }
}
void
uih_setguessing (uih_context * c, int range)
{
  c->fcontext->range = range;
}
void
uih_setperiodicity (uih_context * c, int periodicity)
{
  c->fcontext->periodicity = periodicity;
}

void
uih_interrupt (uih_context * c)
{
  if (c->incalculation)
    c->interrupt = 1;
}
void
uih_stopzooming (uih_context * c)
{
  c->speed = 0;
}
int
uih_updateimage (uih_context * c, struct image *image)
{
  c->image = image;
  c->queue->isinitialized = 0;
  uih->ddatalost = 1;
  fractalc_resize_to (c->fcontext, c->image->pixelwidth * c->image->width, c->image->pixelheight * c->image->height);
  uih->display = 1;
  uih->inanimation = 2;
  uih_newimage (uih);
  if (image->palette->type & (FIXEDCOLOR | BITMAPS))
    uih_fixedcolorenable (uih);
  else
    uih_fixedcolordisable (uih);
  return (uih_initqueue (uih));
}
struct uih_context *
uih_mkcontext (int flags, struct image *image, int (*passfunc) (struct uih_context *, int, char *, float), void (*longwait) (struct uih_context *))
{
  uih_context *uih;
  uih = (uih_context *) calloc (sizeof (*uih), 1);	/*setup parameters */
  uih->autopilot = 0;
  uih->flags = flags;
  uih->image = image;
  uih->speed = 0;
  uih->step = 0;
  uih->speedup = STEP;
  uih->maxstep = MAXSTEP;
  uih->lasttime = -1;
  uih->recalculatemode = UIH_NEW_IMAGE;
  uih->fastmode = 2;
  uih->direction = 1;
  uih->cyclingspeed = ROTATIONSPEED;
  uih->ddatalost = 1;
  uih->xtextpos = 1;
  uih->complettehandler = 0;
  uih->ytextpos = 1;
  uih->display = 0;
  uih->errstring = NULL;
  uih->rotatemode = 0;
  uih->rotationspeed = 10;
  uih->longwait = longwait;
  uih->passfunc = passfunc;
  uih->nletters = 0;
  uih->letterspersec = 15;
  uih->maintimer = tl_create_timer ();
  uih->calculatetimer = tl_create_timer ();
  uih->doittimer = tl_create_timer ();
  tl_update_time ();
  tl_reset_timer (uih->maintimer);
  tl_reset_timer (uih->calculatetimer);
  tl_stop_timer (uih->doittimer);
  tl_reset_timer (uih->doittimer);
  uih_alloctables (uih);
  uih_initqueue (uih);		/*FIXME return value should not be ignored */
  if (image->palette->type & (FIXEDCOLOR | BITMAPS))
    uih_fixedcolorenable (uih);
  uih_mkdefaultpalette (uih);
  uih_stoptimers (uih);
  clean_autopilot (uih);
  uih_newimage (uih);
  return (uih);
}
void
uih_savepalette (uih_context * c)
{
  if (uih->palette2 != NULL)
    destroypalette (uih->palette2);
  if (uih->zengine->fractalc->palette != NULL)
    uih->palette2 = clonepalette (uih->zengine->fractalc->palette);
}
void
uih_restorepalette (uih_context * uih)
{
  if (uih->palette2 != NULL)
    {
      if (uih->zengine->fractalc->palette != NULL)
	restorepalette (uih->zengine->fractalc->palette, uih->palette2);
      destroypalette (uih->palette2);
    }
  uih->palette2 = NULL;
  uih_finishpalette (uih);
}

void
uih_freecontext (uih_context * c)
{
  struct filter *f;
  while (c->queue->first)
    {
      f = c->queue->first;
      removefilter (c->queue->first);
      f->action->destroyinstance (f);
    }
  free (c->queue);
  free_fractalc (c->fcontext);
  free (c);
}
static struct filter *
uih_getinstance (struct filteraction *a)
{
  struct filter *f = createfilter (a);
  f->data = uih;
  f->name = "XaoS's user interface layer";
  return (f);
}
static void
uih_destroyinstance (struct filter *f)
{
  struct uih_context *c = f->data;
  if (c->autopilot)
    uih_autopilot_off (c);
  if (c->cycling)
    uih_cycling_off (c);
  tl_free_timer (uih->maintimer);
  tl_free_timer (uih->calculatetimer);
  tl_free_timer (uih->doittimer);
  if (c->displaytext)
    c->displaytext = 0, free (c->text);
  if (c->save)
    uih_save_disable (c);
  if (c->play)
    uih_replaydisable (c);
  free (f);
}
static int wascycling = 0;
static int
uih_require (struct filter *f, struct requirements *r)
{
  struct uih_context *uih;
  uih = f->data;
  if (uih->cycling)
    uih_cycling_off (uih), wascycling = 1;
  /*FIXME something should be done here :) */
  return (1);
}
static int
uih_initialize (struct filter *f, struct initdata *i)
{
  struct uih_context *uih;
  int returnval;
  uih = f->data;
  f->queue->saveimage = uih->image;
  i->fractalc = uih->fcontext;
  tl_update_time ();
  f->image = uih->image;
  f->wait_function = uih_waitfunc;
  uih->times[0][0] = -1;
  i->image = uih->image;
  f->fractalc = i->fractalc;
  f->image->palette->flags |= FINISHLATER;
  i->fractalc->palette = uih->image->palette;
  i->wait_function = uih_waitfunc;
  /*FIXME datalost should be handled in better way */
  if (uih->ddatalost)
    i->flags |= DATALOST;
  uih->tbreak = 2;
  uih_invalidatepos (uih);
  clean_autopilot (uih);
  returnval = f->previous->action->initialize (f->previous, i);
  if (wascycling)
    uih_cycling_on (uih), wascycling = 0;
  return returnval;
}

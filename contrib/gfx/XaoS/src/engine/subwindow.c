#ifndef _plan9_
#ifndef AROS
#include <malloc.h>
#endif
#include <stdio.h>		/*for NULL */
#include <string.h>		/*for memcpy */
#else
#include <u.h>
#include <libc.h>
#include <stdio.h>
#endif
#include <filter.h>
struct subdata
  {
    struct filter *second;
    struct image *image;
    pixel_t **currlines;
    int recal;
    int forpversion, forversion;
    number_t pre,pim;
  };
void
subwindow_setsecond (struct filter *f, struct filter *f1)
{
  struct subdata *s = f->data;
  s->second = f1;
}
void
myflip (struct image *image)
{
  struct subdata *s = image->data;
  flipgeneric (image);
  s->image->flip (s->image);
  s->currlines = s->image->currlines;
}
static int
requirement (struct filter *f, struct requirements *r)
{
  int val;
  r->nimages = 2;
  val = f->next->action->requirement (f->next, r);
  return (val);
}
static int
initialize (struct filter *f, struct initdata *i)
{
  struct subdata *s = f->data;
  int x;
  int val = 0;
  pixel_t **lines1, **lines2 = NULL;
  double size;
  int width, height;
  inhermisc (f, i);
  if (datalost (f, i))
    s->recal = 1;
  f->imageversion = i->image->version;
  if (f->childimage != NULL)
    destroy_image (f->childimage);
  s->image = f->image = i->image;
  s->currlines = f->image->currlines;
  s->forpversion = f->image->palette->version;
  s->forversion = f->fractalc->version;
  if (f->image->width * f->image->pixelwidth > f->image->height * f->image->pixelheight)
    size = f->image->width * f->image->pixelwidth / 2;
  else
    size = f->image->height * f->image->pixelheight / 2;
  width = size / f->image->pixelwidth;
  height = size / f->image->pixelheight;
  /*fractalc_resize_to(f->fractalc,size,size); */
  lines1 = malloc (sizeof (*lines1) * height);
  if (f->image->nimages == 2)
    lines2 = malloc (sizeof (*lines2) * height);
  if (lines1 == NULL)
    return 0;
  if (f->image->nimages == 2 && lines2 == NULL)
    {
      free (lines1);
      return 0;
    }
  for (x = 0; x < height; x++)
    {
      lines1[x] = i->image->currlines[x];
      if (f->image->nimages == 2)
	lines2[x] = i->image->oldlines[x];
    }
  if (f->image->nimages == 2)
    for (x = 0; x < f->image->height; x++)
      {
	memcpy (f->image->oldlines[x], f->image->currlines[x], f->image->width * f->image->bytesperpixel);
      }
  f->childimage = i->image = create_image_lines (width, height, f->image->nimages, lines1, lines2, i->image->palette, myflip, FREELINES, f->image->pixelwidth, f->image->pixelheight);
  if (i->image == NULL)
    {
      free (lines1);
      free (lines2);
      return 0;
    }
  f->childimage->data = s;
  x = f->previous->action->initialize (f->previous, i);
  if (!x)
    return 0;
  if (s->second != NULL)
    {
      i->image = f->image;
      val = s->second->action->initialize (s->second, i);
      if (!val)
	return 0;
    }
  return (x | val);

}
static struct filter *
getinstance (struct filteraction *a)
{
  struct filter *f = createfilter (a);
  struct subdata *s = calloc (sizeof (*s), 1);
  f->name = "Subwindow";
  f->data = s;
  s->second = NULL;
  return (f);
}
static void
destroyinstance (struct filter *f)
{
  free (f->data);
  if (f->childimage != NULL)
    destroy_image (f->childimage);
  free (f);
}
static int
doit (struct filter *f, int flags, int time)
{
  int val = 0,m,vold;
  vinfo vs;
  vrect rs;
  struct subdata *s = f->data;
  static int v;
  if (s->second != NULL && (s->recal || s->forpversion != f->image->palette->version || s->forversion != f->fractalc->version))
    { int x;
      if (s->recal)
	f->fractalc->version++;
      s->forpversion = f->image->palette->version;
      s->forversion = f->fractalc->version;
      s->recal = 1;
      val = (s->second->action->doit (s->second, flags, time));
      if (val & ANIMATION)
	return val;
      s->recal = 0;
      if (f->image->nimages == 2)
        for (x = 0; x < f->image->height; x++)
          {
	    memcpy (f->image->oldlines[x], f->image->currlines[x], f->image->width * f->image->bytesperpixel);
          }
    }
  if (s->currlines != f->image->currlines && f->childimage->nimages == 2)
    flipgeneric (f->childimage), s->currlines = f->image->currlines;
  /*FIXME: ugly hack for new julia mode*/
  v++;
  vs=f->fractalc->s;
  rs=f->fractalc->rs;
  f->fractalc->s=f->fractalc->currentformula->v;
  update_view(f->fractalc);
  m=f->fractalc->mandelbrot;
  vold=f->fractalc->version;
  if(s->pre!=f->fractalc->pre||
     s->pim!=f->fractalc->pim) {
    f->fractalc->version=v;
    s->pre=f->fractalc->pre;
    s->pim=f->fractalc->pim;
  }
  f->fractalc->mandelbrot=0;
  val=f->previous->action->doit (f->previous, flags, time) | val;
  f->fractalc->mandelbrot=m;
  f->fractalc->version=vold;
  f->fractalc->s=vs;
  f->fractalc->rs=rs;
  return val;
}
static void
myremove (struct filter *f)
{
  /*fractalc_resize_to(f->fractalc,f->queue->last->image->width*f->queue->last->image->pixelwidth,f->queue->last->image->height*f->queue->last->image->pixelheight); */
}
static void
convertdown (struct filter *f, int *x, int *y)
{
  struct subdata *s = f->data;
  if (s->second != NULL)
    s->second->action->convertdown (s->second, x, y);
  if (f->previous != NULL)
    f->previous->action->convertdown (f->previous, x, y);
}


struct filteraction subwindow_filter =
{
  "Subwindow",
  "Subwindow",
  0,
  getinstance,
  destroyinstance,
  doit,
  requirement,
  initialize,
  convertupgeneric,
  convertdown,
  myremove
};

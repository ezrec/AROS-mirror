#ifndef UNSUPPORTED
static void
do_starfield (void *data, struct taskinfo *task, int r1, int r2)
{
  struct filter *f = data;
  cpixel_t *dest;
  pixel8_t *src, *srcend;
  int color;
  int y, black = f->image->palette->pixels[0];
  mysrandom (rand ());
  for (y = r1; y < r2; y++)
    {
      src = f->childimage->currlines[y];
      srcend = f->childimage->currlines[y] + f->childimage->width;
      dest = (cpixel_t *) f->image->currlines[y];
      while (src < srcend)
	{
	  color = (myrandom () >> 7) & 15;
	  if (!*src || *src ** src ** src > (unsigned int)(myrandom () & (0xffffff)))
	    {
	      p_set (dest, f->image->palette->pixels[color]);
	    }
	  else
	    p_set (dest, black);
	  p_inc (dest, 1);
	  src++;
	}
    }
}
#endif
#undef do_starfield

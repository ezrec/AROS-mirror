#ifndef UNSUPPORTED
// static void calcline (realloc_t * ry) REGISTERS (3);
static void
calcline (realloc_t * ry)
{
  number_t y;
  int range = cfractalc.range;
  realloc_t *rx, *rend, *rend1, *ryl, *ryr;
  int distl, distr, distup, distdown;
  cpixel_t *vbuff, *vbuffu, *vbuffd;
  unsigned int inset = cpalette.pixels[0];
  int s2;
  unsigned int c;
#ifdef DRAW
  int s3;
#endif
  cppixel_t *vbuff1 = (cpixel_t **) cimage.currlines + (ry - czoomc.reallocy);
  assert (ry >= czoomc.reallocy);
  assert (ry < czoomc.reallocy + cimage.height);
  y = ry->possition;
  rend = ry - range - 1;
  if (czoomc.reallocy > rend)
    rend = czoomc.reallocy;
  for (ryl = ry - 1; rend <= ryl && ryl->dirty; ryl--);
  distl = ryl - ry;
  rend = ry + range;
  if (czoomc.reallocy + cimage.height < rend)
    rend = czoomc.reallocy + cimage.height;
  for (ryr = ry + 1; rend > ryr && ryr->dirty; ryr++);
  distr = ryr - ry;
  rend = czoomc.reallocy + cimage.height;
  if (ryr == czoomc.reallocy + cimage.height || ryl < czoomc.reallocy || ryr->dirty || ryl->dirty)
    {
      for (rx = czoomc.reallocx, vbuff = *vbuff1, rend1 = czoomc.reallocx + cimage.width;
	   rx < rend1; rx++)
	{
	  if (!rx->dirty)
	    {
	      STAT (tocalculate++);
#ifdef DRAW
	      s3 = mirrored;
#endif
	      p_set (vbuff, calculate (rx->possition, y, cfractalc.periodicity));
#ifdef DRAW
	      if (s3 == mirrored)
		vga_drawpixel (rx - czoomc.reallocx, ry - czoomc.reallocy, 254);
#endif
	    }
	  p_inc (vbuff, 1);
	}
    }
  else
    for (s2 = 0; s2 < 2; s2++)
      {
	distup = INT_MAX / 2;
	distdown = 0;
	for (rx = czoomc.reallocx + s2,
	     vbuff = p_add (*vbuff1, s2),
	     vbuffu = p_add (vbuff1[distl], s2),
	     vbuffd = p_add (vbuff1[distr], s2),
	     rend1 = czoomc.reallocx + cimage.width;
	     rx < rend1; rx += 2)
	  {
	    assert (rx < czoomc.reallocx + cimage.width);
	    assert (rx >= czoomc.reallocx);
	    if (!rx->dirty)
	      {
		STAT (tocalculate++);
		if (distdown <= 0)
		  {
		    for (ryr = rx + 1; ryr < rend1 && ryr->dirty; ryr++);
		    distdown = ryr - rx;
		    if (ryr == rend1)
		      distdown = INT_MAX;
		  }
		if (distdown < range &&
		    distup < range &&
		    (p_get (vbuffu) == p_get (vbuffd) &&
		     ((
			p_getp (vbuffu, distdown) == (c = p_getp (vbuffd, -distup)) &&
			c == p_getp (vbuffu, -distup) &&
			c == p_getp (vbuffd, distdown) &&
			c == p_get (vbuffd)) &&
		      (!s2 || rx == (rend1 - 1) ||
		       (
			 ((rx - 1)->dirty ||
			  p_getp (vbuff, -1) == c) &&
			 ((rx + 1)->dirty ||
			  p_getp (vbuff, +1) == c)
		       )))))
		  {
		    p_set (vbuff, c);
		    STAT (avoided++);
		  }
		else
		  {
#ifdef DRAW
		    s3 = mirrored;
#endif
		    if (cfractalc.periodicity &&
			distup < range &&
			distdown < range &&
			(p_get (vbuffu) != inset &&
			 p_get (vbuffd) != inset &&
			 ((
			    p_getp (vbuffu, -distup) != inset) &&
			  p_getp (vbuffu, +distdown) != inset &&
			  p_getp (vbuffd, -distup) != inset &&
			  p_getp (vbuffd, +distdown) != inset)
			)
		      )
		      p_set (vbuff, calculate (rx->possition, y, 0));
		    else
		      p_set (vbuff, calculate (rx->possition, y, cfractalc.periodicity));
#ifdef DRAW
		    if (s3 == mirrored)
		      vga_drawpixel (rx - czoomc.reallocx, ry - czoomc.reallocy, 255);
#endif
		  }
		distup = 0;
	      }
	    p_inc (vbuff, 2);
	    p_inc (vbuffu, 2);
	    p_inc (vbuffd, 2);
	    if (rx != rend1 - 1 && !(rx + 1)->dirty)
	      distup = -1;
	    distdown -= 2;
	    distup += 2;
	  }
      }
  ry->recalculate = 0;
  ry->dirty = 0;
}
// static void calccolumn (realloc_t * rx) REGISTERS (3);
static void
calccolumn (realloc_t * rx)
{
  number_t x;
  int range = cfractalc.range;
  realloc_t *ry, *rend, *rend1, *rxl, *rxr;
  int pos, distl, distr, distup, distdown;
  unsigned int c;
  unsigned int inset = cpalette.pixels[0];
  cppixel_t *vbuff;
  int s2;
#ifdef DRAW
  int s3;
#endif
  pos = rx - czoomc.reallocx;
  assert (pos >= 0);
  assert (pos < cimage.width);
  rend = rx - range + 1;
  if (czoomc.reallocx > rend)
    rend = czoomc.reallocx;
  for (rxl = rx - 1; rend <= rxl && rxl->dirty; rxl--);
  distl = rx - rxl;
  rend = rx + range;
  if (czoomc.reallocx + cimage.width < rend)
    rend = czoomc.reallocx + cimage.width;
  for (rxr = rx + 1; rxr < rend && rxr->dirty; rxr++);
  distr = rxr - rx;
  x = rx->possition;
  rend = czoomc.reallocx + cimage.width;
  if (rxr >= czoomc.reallocx + cimage.width || rxl < czoomc.reallocx || rxr->dirty || rxl->dirty)
    {
      for (ry = czoomc.reallocy, vbuff = (cppixel_t *) cimage.currlines, rend1 = czoomc.reallocy + cimage.height; ry < rend1; ry++, vbuff++)
	{
	  if (!ry->dirty)
	    {
	      STAT (tocalculate++);
#ifdef DRAW
	      s3 = mirrored;
#endif
	      p_setp ((*vbuff), pos, calculate (x, ry->possition, cfractalc.periodicity));
#ifdef DRAW
	      if (s3 == mirrored)
		vga_drawpixel (rx - czoomc.reallocx, ry - czoomc.reallocy, 32);
#endif
	    }
	}
    }
  else
    {
      distl = pos - distl;
      distr = pos + distr;
      assert (distl >= 0);
      assert (distr < cimage.width);
      for (s2 = 0; s2 < 2; s2++)
	{
	  distup = INT_MAX / 2;
	  distdown = 0;
	  for (ry = czoomc.reallocy + s2, vbuff = (cppixel_t *) cimage.currlines + s2, rend1 = czoomc.reallocy + cimage.height; ry < rend1; ry += 2)
	    {
	      /*if (ry->symto == -1) { */
	      assert (ry < czoomc.reallocy + cimage.height);
	      if (!ry->dirty)
		{
		  STAT (tocalculate++);
		  if (distdown <= 0)
		    {
		      for (rxr = ry + 1; rxr < rend1 && rxr->dirty; rxr++);
		      distdown = rxr - ry;
		      if (rxr == rend1)
			distdown = INT_MAX;
		    }
		  if (distup < range &&
		      distdown < range &&
		    (p_getp (vbuff[0], distl) == p_getp (vbuff[0], distr) &&
		     ((p_getp (vbuff[distdown], distl) == (c = p_getp (vbuff[-distup], distr)) &&
		       p_getp (vbuff[-distup], distl) == c &&
		       p_getp (vbuff[distdown], distr) == c &&
		       p_getp (vbuff[0], distl) == c) &&
		      (!s2 || (ry == rend1 - 1) ||
		       (((ry + 1)->dirty ||
			 p_getp (vbuff[1], pos) == c) &&
			((ry - 1)->dirty ||
			 p_getp (vbuff[-1], pos) == c)
		       )))))
		    {
		      STAT (avoided++);
		      p_setp (vbuff[0], pos, c);
		    }
		  else
		    {
#ifdef DRAW
		      s3 = mirrored;
#endif
		      if (cfractalc.periodicity &&
			  distup < range &&
			  distdown < range &&
			  (p_getp (vbuff[0], distl) != inset &&
			   p_getp (vbuff[0], distr) != inset &&
			   ((
			      p_getp (vbuff[distdown], distl) != inset) &&
			    p_getp (vbuff[-distup], distl) != inset &&
			    p_getp (vbuff[distdown], distr) != inset &&
			    p_getp (vbuff[-distup], distr) != inset)
			  )
			)
			p_setp (vbuff[0], pos, calculate (x, ry->possition, 0));
		      else
			p_setp (vbuff[0], pos, calculate (x, ry->possition, cfractalc.periodicity));
#ifdef DRAW
		      if (s3 == mirrored)
			vga_drawpixel (rx - czoomc.reallocx, ry - czoomc.reallocy, 255);
#endif
		    }
		  distup = 0;
		}
	      vbuff += 2;
	      if (ry != rend1 - 1 && !(ry + 1)->dirty)
		distup = -1;
	      distdown -= 2;
	      distup += 2;
	    }
	}
    }
  rx->recalculate = 0;
  rx->dirty = 0;
}

static /*INLINE */ void
dosymetry2 (void *data, struct taskinfo *task, int r1, int r2)
{
  cpixel_t **vbuff = (cpixel_t **) cimage.currlines;
  realloc_t *rx, *rend;
  cpixel_t **vend = (cpixel_t **) cimage.currlines + cimage.height;
  for (rx = czoomc.reallocx + r1, rend = czoomc.reallocx + r2; rx < rend; rx++)
    {
      assert (rx->symto >= 0 || rx->symto == -1);
      if (rx->symto >= 0)
	{
	  assert (rx->symto < cimage.width);
	  if (!czoomc.reallocx[rx->symto].dirty)
	    {
	      int pos = rx - czoomc.reallocx;
	      int pos1 = rx->symto;
	      vbuff = (cpixel_t **) cimage.currlines;
	      for (; vbuff < vend; vbuff++)
		p_copy (vbuff[0], pos, vbuff[0], pos1);
	      rx->dirty = 0;
	    }
	}
    }
}
/*
 * Fill - bitmap depended part.
 *
 * This function is called, when calculation was interrupted because of
 * timeout. It fills uncalculated rows by nearest one
 *
 * This function is very critical for time, since it takes about 17% of time
 * during zoom in 1027x768. So I tryed to optimize it.
 *
 * Following optimizations are done:
 *
 * 1) not exactly all lines are changed, just those where it is necessary -
 *    because after that uncalculated lines will be copied too, just
 *    caluclated lines are changed. This is done using vbuffs and height
 *    variables, that holds height and lines of subimage of original image
 *    that needs to be changed.
 * 2) There is onrolled versions for blocks - when resolution is really low,
 *    there is blocks of uncaluclated lines so I update up to 4 in one pass
 *    throught vbuffs array that is rather slow. In 8bpp mode I use memset for
 *    values higher than 4. In other modes larger blocks are filed using 4
 *    pixels passes.
 * 3) I tried to minimalize number of variables, since one i386 register is
 *    still missing. (Gcc uses stack accesses for vend - oops). I tried almost
 *    everything - register keyword, explicit register allocation, asm hacks
 *    saying that I need this variable but nothing helped. PLEASE FIX THIS
 *    IF YOU KNOW ANY WAY.
 *
 * This function is still very slow and ineffective especially in 8bpp mode
 * and I think it should help a much rewrite it into assembly (at least the
 * internal loops)
 *
 * Please if you like assembly coding and you want to do this. CONTACT
 * ME SOON.
 */
#ifndef __GNUC__
#undef bpp1
#endif
#ifndef __i386__
#undef bpp1
#endif

static /*INLINE */ void
fill2 (void *data, struct taskinfo *task, int rr1, int rr2)

{
  register cpixel_t **vbuff;
  register cpixel_t **vend;
  register int vsrc;
  register int pos;
  int height;
  pixel_t **vbuffs;
  realloc_t *rx, *r1, *r2, *rend, *rend2;
  register int n = 0;
  vbuffs = data;
  height = vbuffs - (pixel_t **) tmpdata;
  vbuffs = (pixel_t **) tmpdata;
  vbuff = (cpixel_t **) vbuffs;
  pos = 0;
  vsrc = 0;
  vend = vbuff + height;

  rx = czoomc.reallocx + rr1;
  while (rx > czoomc.reallocx && rx->dirty)
    rx--;
  for (rend = czoomc.reallocx + rr2, rend2 = czoomc.reallocx + cimage.width; rx < rend; rx++)
    {
      if (rx->dirty)
	{
	  incuncomplette ();
	  r1 = rx - 1;
	  for (r2 = rx + 1; r2->dirty && r2 < rend2; r2++);
	  if (r2 == rend2 && (rr2 != cimage.width || !rx))
	    return;
	  while (rx->dirty && rx < rend2)
	    {
	      n = r2 - rx;
	      assert (n > 0);
	      if (r2 < rend2 && (r1 < czoomc.reallocx || rx->possition - r1->possition > r2->possition - rx->possition))
		vsrc = (r2 - czoomc.reallocx), r1 = r2;
	      else
		vsrc = (r1 - czoomc.reallocx);
	      pos = (rx - czoomc.reallocx);
	      vbuff = (cpixel_t **) vbuffs;
	      switch (n)
		{
		case 1:
		  for (; vbuff < vend; vbuff++)
		    {
		      p_copy (vbuff[0], pos, vbuff[0], vsrc);
		    }
		  rx->dirty = 0;
		  rx->possition = r1->possition;
		  rx++;
		  break;
		case 2:
		  for (; vbuff < vend; vbuff++)
		    {
		      p_copy (vbuff[0], pos, vbuff[0], vsrc);
		      p_copy (vbuff[0], pos + 1, vbuff[0], vsrc);
		    }
		  rx->dirty = 0;
		  rx->possition = r1->possition;
		  (rx + 1)->dirty = 0;
		  (rx + 1)->possition = r1->possition;
		  rx += 2;
		  break;
		case 3:
		  for (; vbuff < vend; vbuff++)
		    {
		      p_copy (vbuff[0], pos, vbuff[0], vsrc);
		      p_copy (vbuff[0], pos + 1, vbuff[0], vsrc);
		      p_copy (vbuff[0], pos + 2, vbuff[0], vsrc);
		    }
		  rx->dirty = 0;
		  rx->possition = r1->possition;
		  (rx + 1)->dirty = 0;
		  (rx + 1)->possition = r1->possition;
		  (rx + 2)->dirty = 0;
		  (rx + 2)->possition = r1->possition;
		  rx += 3;
		  break;
		case 4:
#ifndef bpp1
		default:
#endif
		  for (; vbuff < vend; vbuff++)
		    {
		      p_copy (vbuff[0], pos, vbuff[0], vsrc);
		      p_copy (vbuff[0], pos + 1, vbuff[0], vsrc);
		      p_copy (vbuff[0], pos + 2, vbuff[0], vsrc);
		      p_copy (vbuff[0], pos + 3, vbuff[0], vsrc);
		    }
		  rx->dirty = 0;
		  rx->possition = r1->possition;
		  (rx + 1)->dirty = 0;
		  (rx + 1)->possition = r1->possition;
		  (rx + 2)->dirty = 0;
		  (rx + 2)->possition = r1->possition;
		  (rx + 3)->dirty = 0;
		  (rx + 3)->possition = r1->possition;
		  rx += 4;
		  break;
#ifdef bpp1
		default:
		  for (; vbuff < vend; vbuff++)
		    memset ((char *) *vbuff + pos, (int) vbuff[0][vsrc], (size_t) n);
		  for (; rx < r2; rx++)
		    rx->dirty = 0, rx->possition = r1->possition;
		  break;
#endif

		}		/*case */
	    }			/*while rx->dirty */
	}			/*if rx->dirty */
    }				/*for czoomc */
}
#endif
#undef fill2
#undef dosymetry2
#undef calcline
#undef calccolumn
#undef rend

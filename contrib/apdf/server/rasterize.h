//========================================================================
//
// rasterize.h
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#include <stdlib.h>
#include "AOutputDev.h"

#define IDB(x)	//x
#define DB(x)	//x

#if 0
#define CHECKBM(bm)	check_start = bm.data(); \
			check_stop = check_start + bm.modulo * bm.height * bm.bpp;
#define CHECK(p)	if ((Guchar*)(p) < check_start || (Guchar*)(p) >= check_stop) { \
				dprintf("#### Error: overflow line %d\n", __LINE__); \
				return; \
			}

Guchar *check_start;
Guchar *check_stop;
extern "C" void dprintf(const char *,...);
#else
#define CHECKBM(bm)
#define CHECK(p)
#endif

inline double length(double x, double y) { return sqrt(x * x + y * y); }
template<class T> inline T abs(T x) { return x < 0 ? -x : x; }
template<class T> inline T max(T x, T y) { return x < y ? y : x; }
template<class T> inline T min(T x, T y) { return x < y ? x : y; }
template<class T> inline void swap(T& x, T& y) { T t = x; x = y; y = t; }

template<class colorT> inline
void _rectfill(ABitmap& bm, int x0, int y0, int w, int h, colorT color) {
  CHECKBM(bm);

  int modulo = bm.modulo;
  colorT *dest = (colorT*) bm.data();

  x0 -= bm.xoffset;
  y0 -= bm.yoffset;

  if (x0 < 0) {
    w += y0;
    x0 = 0;
  }
  if (x0 + w >= bm.width)
    w = bm.width - x0;
  if (y0 < 0) {
    h += y0;
    y0 = 0;
  }
  if (y0 + h >= bm.height)
    h = bm.height - y0;

  dest += y0 * modulo + x0;
  modulo -= w;
  if (w > 0) {
    while (--h >= 0) {
      int k = w;
      while (--k >= 0) {
	CHECK(dest);
      	*dest++ = color;
      }
      dest += modulo;
    }
  }
}

template<class colorT> inline
void _fill(ABitmap& bm, int x0, int y0, region* area, colorT color) {
  CHECKBM(bm);

  int modulo = bm.modulo;
  colorT *dest = (colorT*) bm.data();

  x0 += NEARESTPEL(area->origin.x) - bm.xoffset;
  y0 += NEARESTPEL(area->origin.y) - bm.yoffset;

  int w = bm.width;
  int h = bm.height;

  //printf("fill (%d,%d,%d,%d)\n",area->xmin,area->ymin,w,h);
  for (edgelist *edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link) {
    pel *leftP = edge->xvalues;
    pel *rightP = edge->link->xvalues;
    int y1 = edge->ymin + y0;
    int y2 = min(edge->ymax + y0, h);
    //printf("edge: %d -> %d\n",y1,y2);
    if (y1 < 0) {
      leftP -= y1;
      rightP -= y1;
      y1 = 0;
    }
    colorT* p = dest + y1 * modulo;
    while (y1 < y2) {
      int x1 = max(*leftP++ + x0, 0);
      int x2 = min(*rightP++ + x0, w);
      //printf("\t %d -> %d\n",x1,x2);
      colorT* q = p + x1;
      while (x1 < x2) {
	CHECK(q);
	*q++ = color;
	++x1;
      }
      ++y1;
      p += modulo;
    }
  }
}


template<class colorT>
void AAfillrun(colorT* q, int x1, int x2, colorT c1, colorT c2) {
  if (x1 < x2) {
    q += x1 >> 1;
    if (x1 & 1) {
      CHECK(q);
      *q++ = c1;
      ++x1;
    }
    int x2odd = x2 & 1;
    x2 &= ~1;
    while (x1 < x2) {
      CHECK(q);
      *q++ = c2;
      x1 += 2;
    }
    if (x2odd) {
      CHECK(q);
      *q = c1;
    }
  }
}

template<class colorT> inline
void _AAfill(ABitmap& bm, int x0, int y0, region* area,
	     colorT c1, colorT c2, colorT c3, colorT c4) {
  CHECKBM(bm);
  edgelist *prev = area->anchor;
  if (!prev)
    return;
  int modulo = bm.modulo;
  colorT *dest = (colorT*) bm.data();
  int bw = bm.width << 1;
  int bh = bm.height << 1;
  int bx = bm.xoffset - x0 - NEARESTPEL(area->origin.x) << 1;
  int by = bm.yoffset - y0 - NEARESTPEL(area->origin.y) << 1;
  /*DB(printf("--------------AAFill--------------\n");
     for(edgelist* e=area->anchor;VALIDEDGE(e);e=e->link->link) {
	 printf("swath : y1=%d, y2=%d, top: %d -> %d, bottom : %d -> %d\n",
		e->ymin-by,e->ymax-by,
		*e->xvalues-bx,*e->link->xvalues-by,
		e->xvalues[e->ymax-e->ymin-1]-bx,
		e->link->xvalues[e->ymax-e->ymin-1]-by);
     })*/
  edgelist *prevprev = NULL;
  int prev_ymin = prev->ymin;
  for (edgelist *edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link) {
    pel *leftP = edge->xvalues;
    pel *rightP = edge->link->xvalues;
    int y1 = edge->ymin - by;
    int y2 = min(edge->ymax - by, bh);
    if (y1 != prev_ymin) {
      prevprev = prev;
      prev = edge;
      prev_ymin = y1;
    }
    //printf("edge: %d -> %d\n",y1,y2);
    if (y1 < 0) {
      leftP -= y1;
      rightP -= y1;
      y1 = 0;
    }
    if (y1 >= y2)
      continue;
    colorT* p = dest + (y1 >> 1) * modulo;
    if (y1 & 1) {
      int x1 = max(*leftP++ - bx, 0);
      int x2 = min(*rightP++ - bx, bw);
      if (prevprev == NULL || prevprev->ymax - by != y1)
	AAfillrun(p, x1, x2, c1, c2);
      else {
	colorT* q = p + (x1 >> 1);
	for (edgelist *e = prevprev; x1 < x2 && e != prev; e = e->link->link) {
	  int z1 = max(e->xvalues[e->ymax - e->ymin - 1] - bx, 0);
	  int z2 = min(e->link->xvalues[e->ymax - e->ymin - 1] - bx, bw);
	  if (z1 >= x2 || z2 <= x1 || z1 >= z2)
	    continue;
	  if (x1 < z1) {
	    if (x1 & 1) {
	      CHECK(q);
	      *q++ = c1;
	      ++x1;
	    }
	    int z1odd = z1 & 1;
	    z1 &= ~1;
	    while (x1 < z1) {
	      CHECK(q);
	      *q++ = c2;
	      x1 += 2;
	    }
	    if (z1odd) {
	      CHECK(q);
	      *q++ = c3;
	      x1 += 2;
	    }
	  } else if (x1 == z1 && (x1 & 1)) {
	    CHECK(q);
	    *q++ = c2;
	    ++x1;
	  }
	  int z2i = z2;
	  if (z2 > x2)
	    z2 = x2;
	  int z2odd = z2 & 1;
	  z2 &= ~1;
	  while (x1 < z2) {
	    CHECK(q);
	    *q++ = c4;
	    x1 += 2;
	  }
	  if (z2odd) {
	    if (z2i == x2) {
	      CHECK(q);
	      *q++ = c2;
	    } else {
	      CHECK(q);
	      *q++ = c3;
	    }
	    x1 += 2;
	  }
	}
	AAfillrun(p, x1, x2, c1, c2);
      }
      ++y1;
      p += modulo;
    }
    int oddy2 = y2 & 1;
    y2 &= ~1;
    while (y1 < y2) {
      int x1 = max(*leftP++ - bx, 0);
      int x2 = min(*rightP++ - bx, bw);
      int z1 = max(*leftP++ - bx, 0);
      int z2 = min(*rightP++ - bx, bw);
      //printf("\t %d -> %d\n",x1,x2);
      if (z1 >= x2 || z1 >= z2 || x1 >= x2) {
	if (z1 == x2)
	  x2 = z1 = z2;
	AAfillrun(p, x1, x2, c1, c2);
	AAfillrun(p, z1, z2, c1, c2);
      } else {
	// general case :
	//              ***********
	//     ****************************
	//    x1       z1         z2      x2
	// -> (1)  2   (3)   4    (3)  2  (1)

	if (x1 > z1) {  // We are only interested in the
	  int t = x1;   // average number of filled pixels
	  x1 = z1;      // in a 2x2 square, so we can
	  z1 = t;       // replace a situation like:
	}               //     *****          ***
	if (z2 > x2) {  //   *****     by   *******
	  int t = x2;
	  x2 = z2;
	  z2 = t;
	}
	// Here, we have  x1 <= z1 < z2 <= x2
	colorT* q = p + (x1 >> 1);
	if (x1 & 1) {
	  if (x1 < z1){
	    CHECK(q)    //  |  | ****
	    *q++ = c1;  //  | *|******
	  } else {
	    CHECK(q)
	    *q++ = c2;  //  | *|****
	    ++z1;       //  | *|******
	  }
	  ++x1;
	}
	int z1odd = z1 & 1;
	z1 &= ~1;
	while (x1 < z1) {
	  CHECK(q)
	  *q++ = c2;
	  x1 += 2;
	}
	if (z1odd) {
	  CHECK(q)
	  *q++ = c3;
	  x1 += 2;
	}
	if (x1 < z2) {
	  int z2odd = z2 & 1;
	  z2 &= ~1;
	  while (x1 < z2) {
	    CHECK(q)
	    *q++ = c4;
	    x1 += 2;
	  }
	  if (z2odd) {
	    x1 += 2;
	    if (z2 + 1 == x2) {
	      CHECK(q)
	      *q++ = c2;
	      x2 = x1;
	    } else {
	      CHECK(q)
	      *q++ = c3;
	    }
	  }
	  if (x1 < x2) {
	    int x2odd = x2 & 1;
	    x2 &= ~1;
	    while (x1 < x2) {
	      CHECK(q)
	      *q++ = c2;
	      x1 += 2;
	    }
	    if (x2odd) {
	      CHECK(q)
	      *q++ = c1;
	    }
	  }
	}
      }
      y1 += 2;
      p += modulo;
    }
    if (oddy2) {
      int x1 = max(*leftP++ - bx, 0);
      int x2 = min(*rightP++ - bx, bw);
      AAfillrun(p, x1, x2, c1, c2);
    }
  }
  //DB(printf("-----------------------------------\n");)
}


template<class colorT> inline
void _clippedHLine(ABitmap& bm, int y, int x1, int x2, colorT color, DashState& ds) {
  CHECKBM(bm)
  colorT *p = (colorT*)bm.data() + y * bm.modulo + x1;
  if (ds.isSolid()) {
    while (x1 <= x2) {
      CHECK(p)
      *p++ = color;
      ++x1;
    }
  } else {
    while (x1 < x2) {
      if (ds.isOn()) {
	CHECK(p)
	*p = color;
      }
      ++p;
      ds.advance(1 << DashState::fractBits);
      ++x1;
    }
    if (x1 == x2 && ds.isOn()) {
      CHECK(p)
      *p = color;
    }
  }
}

template<class colorT> inline
void _clippedVLine(ABitmap& bm, int x, int y1, int y2, colorT color, DashState& ds) {
  CHECKBM(bm)
  int m = bm.modulo;
  colorT *p = (colorT*)bm.data() + y1 * m + x;
  if (ds.isSolid()) {
    while (y1 <= y2) {
      CHECK(p)
      *p = color;
      p += m;
      ++y1;
    }
  } else {
    while (y1 < y2) {
      if (ds.isOn()) {
	CHECK(p)
	*p = color;
      }
      p += m;
      ds.advance(1 << DashState::fractBits);
      ++y1;
    }
    if (y1 == y2 && ds.isOn()) {
      CHECK(p)
      *p = color;
    }
  }
}

inline int RoundFP(int xy, int b) { return xy + (1 << b - 1) >> b; }
inline int TruncFP(int xy, int b) { return xy >> b; }

// called with y1 < y2
// use the exact algorithm of the type1 rasterizer, so that shapes filled
// and stroked have a boundary that matches the interior.
template<class colorT> inline
void _clippedLine(ABitmap& bm, fractpel x1, fractpel y1, fractpel x2, fractpel y2, colorT color, DashState& ds) {
  CHECKBM(bm)
  const int PREC = 8;

  x1 = TruncFP(x1, FRACTBITS - PREC);
  y1 = TruncFP(y1, FRACTBITS - PREC);
  x2 = TruncFP(x2, FRACTBITS - PREC);
  y2 = TruncFP(y2, FRACTBITS - PREC);
//dprintf("line x=%d..%d, y=%d..%d, bm %dx%d\n",x1,x2,y1,y2,bm.width,bm.height);

  int dx = x2 - x1;
  int dy = y2 - y1;

  int x = RoundFP(x1, PREC);
  int y = RoundFP(y1, PREC);
  int m = bm.modulo;
  colorT *p = (colorT*)bm.data() + y * m + x;
//dprintf("x=%d..%d, y=%d..%d\n",x,RoundFP(x2,PREC),y,RoundFP(y2,PREC));
  CHECK(p)
  CHECK((colorT*)bm.data() + RoundFP(y2, PREC) * m + RoundFP(x2, PREC))
  int county = RoundFP(y2, PREC) - y;
  int countx = RoundFP(x2, PREC) - x;
  int d;
  int pstep;
  if (dx < 0) {
    dx = -dx;
    countx = -countx;
    d = (dy * (x1 - (x << PREC) + (1 << (PREC - 1))) -
	dx * ((y << PREC) - y1 + (1 << (PREC - 1)))) >> PREC;
    pstep = -1;
  } else {
    d = (dy * ((x << PREC) - x1 + (1 << (PREC - 1))) -
	dx * ((y << PREC) - y1 + (1 << (PREC - 1)))) >> PREC;
    pstep = 1;
  }
//dprintf("d=%d dx=%d dy=%d pstep=%d count %d countx %d\n", d,dx,dy,pstep,count,countx);
  if (ds.isSolid()) {
    if (d < 0) {
      CHECK(p)
      *p = color;
    }
    while (--county >= 0) {
      if (d < 0) {
	--countx;
	p += pstep;
	d += dy;
	while (d < 0) {
	  CHECK(p)
	  *p = color;
	  --countx;
	  p += pstep;
	  d += dy;
	}
      }
      CHECK(p)
      *p = color;
      p += m;
      d -= dx;
    }
    if (d < 0) {
      --countx;
      p += pstep;
    }
    while (countx-- >= 0) {
      CHECK(p)
      *p = color;
      p += pstep;
    }
  } else {
    int xStep, yStep;
    double l = length(dx << DashState::fractBits, dy << DashState::fractBits);
    if (dx > dy) {
      xStep = int(l / dx);
      yStep = 0;

    } else {
      xStep = 0;
      yStep = int(l / dy);
    }
    if (d < 0) {
      CHECK(p)
      if (ds.isOn())
	*p = color;
    }
    while (--county >= 0) {
      if (d < 0) {
	--countx;
	p += pstep;
	ds.advance(xStep);
	d += dy;
	while (d < 0) {
	  CHECK(p)
	  if (ds.isOn())
	    *p = color;
	  --countx;
	  p += pstep;
	  ds.advance(xStep);
	  d += dy;
	}
      }
      CHECK(p)
      if(ds.isOn())
	*p = color;
      p += m;
      ds.advance(yStep);
      d -= dx;
    }
    if (d < 0) {
      --countx;
      p += pstep;
      ds.advance(yStep);
    }
    while (countx-- >= 0) {
      CHECK(p)
      if(ds.isOn())
	*p = color;
      p += pstep;
      ds.advance(xStep);
    }
  }
}

#if 0
template<class colorT>
inline void clippedLine(ABitmap& bm, int x1, int y1, int x2, int y2, colorT color) {
  int dx = x2 - x1;
  int adx = dx < 0 ? -dx : dx;
  int dy = y2 - y1;
  int ady = dy < 0 ? -dy : dy;

  if (adx > ady) {
    int m = bm.modulo;
    int sx = dx < 0 ? -1 : 1;
    colorT *p = (colorT*)bm.data() + /*y1 * m +*/ x1;
    int sy = (ady << 16) / adx;
    int ry = ((ady << 16) % adx) >> 1;
    if (dy < 0) {
      sy = -sy;
      ry = -ry;
    }
    y1 <<= 16;
    y1 += ry + (1 << 15);
    do {
      p[(y1 >> 16) * m] = color;
      y1 += sy;
      p += sx;
    } while (--adx >= 0);
  } else {
    int m = bm.modulo;
    colorT *p = (colorT*)bm.data() + y1 * m;
    if (dy < 0)
      m = -m;
    int sx = (adx << 16) / ady;
    int rx = ((adx << 16) % ady) >> 1;
    if (dx < 0) {
      sx = -sx;
      rx = -rx;
    }
    x1 <<= 16;
    x1 += rx + (1 << 15);
    do {
      p[x1 >> 16] = color;
      x1 += sx;
      p += m;
    } while (--ady >= 0);
  }
}
#endif

// copy bm2 to bm1 through area. 'area' and 'bm2' *must* be clipped to
// the boundary of bm1.
template<int BPP> inline
void _clippedCopy(ABitmap& bm1, ABitmap& bm2, region* area) {
  CHECKBM(bm1)
  int modulo1 = bm1.modulo * BPP;
  int modulo2 = bm2.modulo * BPP;
  int xoffs1 = bm1.xoffset - NEARESTPEL(area->origin.x);
  int yoffs1 = bm1.yoffset - NEARESTPEL(area->origin.y);
  int xoffs2 = bm2.xoffset - bm1.xoffset;
  int yoffs2 = bm2.yoffset - bm1.yoffset;
  Guchar *dest = (Guchar*) bm1.data();
  const Guchar *src = (const Guchar*) bm2.data();
  int width = bm1.width;
  int height = bm1.height;

  for (edgelist *edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link) {
    pel *leftP = edge->xvalues;
    pel *rightP = edge->link->xvalues;
    int y1 = edge->ymin - yoffs1;
    int y2 = edge->ymax - yoffs1;
    Guchar *p;
    const Guchar *q;

    if (y1 < 0) {
	leftP -= y1;
	rightP -= y1;
	y1 = 0;
    }
    if (y2 > height)
	y2 = height;

    p = dest + y1 * modulo1;
    q = src + (y1 - yoffs2) * modulo2;

    while (y1 < y2) {
      int x1 = *leftP++ - xoffs1;
      int x2 = *rightP++ - xoffs1;

      if (x1 < 0)
	  x1 = 0;
      if (x2 > width)
	  x2 = width;

      if (x1 < x2) {
	CHECK(p + x1 * BPP)
	CHECK(p + (x2 - 1) * BPP)
	memcpy(p + x1 * BPP,
	       q + (x1 - xoffs2) * BPP, (x2 - x1) * BPP);
      }

      ++y1;
      p += modulo1;
      q += modulo2;
    }
  }
}

template<class colorT> inline
void _drawPicLine(AOutputDev::DrawPicParams& params,
		  int y, int x1, int x2,
		  int xp1, int yp1, int xp2, int yp2) {
  int dxp, dyp;

  IDB(printf("drawPicLine(y=%d, %d -> %d, (%d,%d)->(%d,%d)\n", y, x1, x2, xp1, yp1, xp2, yp2));
  if (x1 >= x2) {
    dxp = 0;
    dyp = 0;
  } else {
    dxp = ((xp2 - xp1) << 16) / (x2 - x1);
    dyp = ((yp2 - yp1) << 16) / (x2 - x1);
  }

  int xp = xp1 << 16;
  int yp = yp1 << 16;

  if (x1 < 0) {
    xp -= dxp * x1;
    yp -= dyp * x1;
    x1 = 0;
  }
  if (x2 >= params.bm->width)
    x2 = params.bm->width - 1;

  int w = params.srcWidth;
  colorT *q = (colorT*)params.bm->data() + y * params.bm->modulo + x1;

  if (params.maskSrc) {
    const Guchar *p = params.maskSrc;
    colorT c(params.color);

    for (int x = x1; x <= x2; ++x) {
      if (p[(yp >> 16) * w + (xp >> 16)])
	*q = c;
      ++q;
      xp += dxp;
      yp += dyp;
    }

  } else {
    const colorT *p = (const colorT*)params.src;

    for (int x = x1; x <= x2; ++x) {
      *q++ = p[(yp >> 16) * w + (xp >> 16)];
      xp += dxp;
      yp += dyp;
    }
  }
}


//========================================================================
//
// AOutputDev.cc
//
// Copyright 1999-2004 Emmanuel Lesueur
//
// adapted from XOutputDev.cc
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#define DB(x) //x
#define IDB(x) //x
#define PATHDB(x) //x
#define T3DB(x) //x

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <setjmp.h>
#include "gmem.h"
#include "GString.h"
#include "Object.h"
#include "Stream.h"
#include "GfxState.h"
#include "GfxFont.h"
#include "Error.h"
#include "TextOutputDev.h"
#include "AOutputDev.h"
#include "FontOutputDev.h"

extern volatile int *abortFlagPtr;
extern volatile int *drawStatePtr;
extern int lastDrawState;

void (**openGfxLib(int, void**))(void);
void closeGfxLib(void**);

extern "C" void dprintf(const char *, ...);
//#define printf dprintf

//------------------------------------------------------------------------
// Constants and macros
//------------------------------------------------------------------------

inline int xoutRound(double x) { return int(x + 65536.5) - 65536; }

inline double length(double x, double y) { return sqrt(x * x + y * y); }

template<class T> inline T abs(T x) { return x < 0 ? -x : x; }
template<class T> inline T max(T x, T y) { return x < y ? y : x; }
template<class T> inline T min(T x, T y) { return x < y ? x : y; }
template<class T> inline void swap(T& x, T& y) { T t = x; x = y; y = t; }

template<class T> class ArrayPtr {
public:
  ArrayPtr(T *p = NULL) : ptr(p) {}
  ArrayPtr(ArrayPtr& p) : ptr(p.ptr) { p.ptr = NULL; }
  ~ArrayPtr() { delete [] ptr; }
  ArrayPtr& operator = (ArrayPtr& p) {
    if (ptr)
      delete [] ptr;
    ptr = p.ptr;
    p.ptr = NULL;
    return *this;
  }
  ArrayPtr& operator = (T *p) {
    if (ptr)
      delete [] ptr;
    ptr = p;
    return *this;
  }
  operator T* () const { return ptr; }
private:
  T *ptr;
};

//------------------------------------------------------------------------
// Parameters
//------------------------------------------------------------------------

extern int maxColors;


//------------------------------------------------------------------------
// ColorTable
//------------------------------------------------------------------------

//Gulong ColorTable::find(Gulong x) {
//  Gulong n = num;
//  entry *p = entries;
//  while (n > 0) {
//    Gulong k = n / 2;
//    entry *q = p + k;
//    if (x > q->val) {
//      p = q + 1;
//      n -= k + 1;
//    } else if(x != q->val)
//      n = k;
//    else
//      return q->color;
//  }
//  if (num == maxEntries) {
//    int k = p - entries;
//    maxEntries = (maxEntries * 3) / 2 + 16;
//    entries = (entry*) grealloc(entries, maxEntries * sizeof(entry));
//    p = entries + k;
//  }
//  if (p != entries + num)
//    memmove(p + 1, p, (entries + num - p) * sizeof(entry));
//  ++num;
//  Gulong c = colorAllocator->allocate(x);
//  p->color = c | x;
//  p->val = x;
//  pens[c] = p->color;
//  return p->color;
//}
//
//void ColorTable::allocate(int n, ColorEntry *p) {
//  colorAllocator->allocate(n, p);
//  for (; --n >= 0; ++p)
//    pens[p->index] =  (p->color.r & 0xff000000UL) |
//		     ((p->color.g & 0xff000000UL) >> 8) |
//		     ((p->color.b & 0xff000000UL) >> 16) |
//		     p->index;
//}



//------------------------------------------------------------------------
// TransformedGfxState
//------------------------------------------------------------------------

/*void TransformedGfxState::assign(const TransformedGfxState& s) {
  capStyle = s.capStyle;
  joinStyle = s.joinStyle;
  lineWidth = s.lineWidth;
  miterLimit = s.miterLimit;
  dashLength = s.dashLength;
  dashStart = s.dashStart;
  dashPattern = new int [dashLength];
  for (int k = 0; k < dashLength; ++k)
    dashPattern[k] = s.dashPattern[k];
}*/


//------------------------------------------------------------------------
// AOutputFont
//------------------------------------------------------------------------

#if 0
AOutputFont::AOutputFont(Ref *idA, double m11OrigA, double m12OrigA,
			 double m21OrigA, double m22OrigA,
			 double m11A, double m12A, double m21A, double m22A) {
  id = *idA;
  m11Orig = m11OrigA;
  m12Orig = m12OrigA;
  m21Orig = m21OrigA;
  m22Orig = m22OrigA;
  m11 = m11A;
  m12 = m12A;
  m21 = m21A;
  m22 = m22A;
}

AOutputFont::~AOutputFont() {
}
#else

AOutputFont::AOutputFont(BaseFont *base1,
			 double m11, double m12, double m21, double m22):
  OutputFont(base1, m11, m12, m21, m22)
{
  radius = int(sqrt(m21 * m21 + m22 * m22) * 1.5) + 2;
}


/*GBool AOutputFont::getMetrics(int c,
			      double *xMin, double *yMin,
			      double *xMax, double *yMax,
			      double *advanceX, double *advanceY) {
  if (region *glyph = getCharInterior(c)) {
    *xMin = glyph->xmin;
    *yMin = -glyph->ymax;
    *xMax = glyph->xmax;
    *yMax = -glyph->ymin;
    *advanceX = (glyph->ending.x - glyph->origin.x) / FRACTFLOAT;
    *advanceY = -(glyph->ending.y - glyph->origin.y) / FRACTFLOAT;
    return gTrue;
  }
  return gFalse;
}*/
#endif


//------------------------------------------------------------------------
// T3FontCache
//------------------------------------------------------------------------

struct T3FontCacheTag {
  Gushort code;
  Gushort mru;			// valid bit (0x8000) and MRU index
  ARegion *glyph;
};

class T3FontCache {
public:

  T3FontCache(Ref *fontID, double m11A, double m12A,
	      double m21A, double m22A,
	      int glyphXA, int glyphYA, int glyphWA, int glyphHA);
  ~T3FontCache();
  GBool matches(Ref *idA, double m11A, double m12A,
		double m21A, double m22A)
    { return fontID.num == idA->num && fontID.gen == idA->gen &&
	     m11 == m11A && m12 == m12A && m21 == m21A && m22 == m22A; }

  Ref fontID;			// PDF font ID
  double m11, m12, m21, m22;	// transform matrix
  int glyphX, glyphY;		// pixel offset of glyph pixmaps
  int glyphW, glyphH;		// size of glyph pixmaps, in pixels
  int cacheSets;		// number of sets in cache
  int cacheAssoc;		// cache associativity (glyphs per set)
  T3FontCacheTag *cacheTags;	// cache tags, i.e., char codes
};

T3FontCache::T3FontCache(Ref *fontIDA, double m11A, double m12A,
			 double m21A, double m22A,
			 int glyphXA, int glyphYA, int glyphWA, int glyphHA) {
  int i;

  fontID = *fontIDA;
  m11 = m11A;
  m12 = m12A;
  m21 = m21A;
  m22 = m22A;
  glyphX = glyphXA;
  glyphY = glyphYA;
  glyphW = glyphWA;
  glyphH = glyphHA;
  int glyphSize = glyphW * glyphH;
  cacheAssoc = 8;
  if (glyphSize <= 256) {
    cacheSets = 8;
  } else if (glyphSize <= 512) {
    cacheSets = 4;
  } else if (glyphSize <= 1024) {
    cacheSets = 2;
  } else {
    cacheSets = 1;
  }
  cacheTags = (T3FontCacheTag *)gmalloc(cacheSets * cacheAssoc *
					sizeof(T3FontCacheTag));
  for (i = 0; i < cacheSets * cacheAssoc; ++i) {
    cacheTags[i].mru = i & (cacheAssoc - 1);
    cacheTags[i].glyph = NULL;
  }
}

T3FontCache::~T3FontCache() {
  for (int i = 0; i < cacheSets * cacheAssoc; ++i) {
    delete cacheTags[i].glyph;
  }
  gfree(cacheTags);
}

struct T3GlyphStack {
  Gushort code;			// character code
  double x, y;			// position to draw the glyph

  //----- cache info
  T3FontCache *cache;		// font cache for the current font
  T3FontCacheTag *cacheTag;	// pointer to cache tag for the glyph
  Guchar *cacheData;		// pointer to cache data for the glyph

  //----- saved state
  //GfxRGB color;
  AGfxState *origState;
  ABitmap *bitmap;
  AGfxState state;
  Color origStrokeColor;
  Color origFillColor;
  double origCTM4, origCTM5;

  T3GlyphStack *next;		// next object on stack
};

//------------------------------------------------------------------------
// AOutputDev
//------------------------------------------------------------------------

/*int nop(void) { return 0; }

static void (*nullFuncTable[])() = {
  (void(*)())nop, (void(*)())nop, (void(*)())nop, (void(*)())nop,
  (void(*)())nop, (void(*)())nop, (void(*)())nop, (void(*)())nop,
  (void(*)())nop, (void(*)())nop, (void(*)())nop, (void(*)())nop,
  (void(*)())nop, (void(*)())nop,
};*/

//AOutputDev::AOutputDev(ColorAllocator* colorAllocator)
//  : colorTable(colorAllocator), xref(NULL), pageState(&bitmap), gfxState(&pageState) {
AOutputDev::AOutputDev(unsigned char* map, int size)
  : xref(NULL), pageState(&bitmap), gfxState(&pageState) {
  Gulong mask;
  int r, g, b, n, m, i;
  GBool ok;

  //funcTable = nullFuncTable;
  //libBase = NULL;
  //libBPP = 0;

  // set up the font
  gfxFont = NULL;
  font = NULL;

  // create text object
  text = new TextPage(gFalse);

  // initialize gfx state
  //textAALevel = 1;
  //strokeAALevel = 0;
  //fillAALevel = 0;
  //states = NULL;
  //clipRegion = NULL;
  //tempBitmap = NULL;
  textClipPath = NULL;
  //outlineAALevel = 0;

  if (size) {
    for (int k = 0; k < 256; ++k)
      colorMap[k] = map[k * size >> 8];
  }

  DB(printf("=== create AOutputDev %p\n", this));
  nT3Fonts = 0;
  t3GlyphStack = NULL;
}

AOutputDev::~AOutputDev() {
  DB(printf("=== delete AOutputDev %p\n", this));
  clearT3GlyphStack();
  for (int i = 0; i < nT3Fonts; ++i) {
    delete t3FontCache[i];
  }
  delete text;
  PixFmt::release(bitmap.pixfmt);
  //if (libBase)
  //  closeGfxLib(&libBase);
}

void AOutputDev::clearT3GlyphStack()
{
  for (T3GlyphStack *t3gs = t3GlyphStack; t3gs; ) {
    T3GlyphStack *next = t3gs->next;

    delete t3gs->bitmap;
    delete t3gs;

    t3gs = next;
  }

  t3GlyphStack = NULL;
}

void AOutputDev::init(unsigned char* p, int x, int y, int w, int h, int m, int b, int f) {
  PixFmt *fmt;
  /*if (PixFmt::FmtId(f) == PixFmt::lut8)
    fmt = PixFmt::get(&colorTable);
  else*/
  if (PixFmt::FmtId(f) == PixFmt::gray8)
    fmt = PixFmt::get(PixFmt::FmtId(f), colorMap);
  else
    fmt = PixFmt::get(PixFmt::FmtId(f));
  bitmap.init(p, x, y, w, h, m, fmt);
  //trueColor = fmt->is_truecolor();
  /*if (b != libBPP) {
    if (libBase)
      closeGfxLib(&libBase);
    funcTable = openGfxLib(b, &libBase);
    libBPP = b;
    if (!funcTable) {
      funcTable = nullFuncTable;
      libBPP = 0;
    }
  }*/
}

void AOutputDev::startDoc(XRef *xrefA) {
  xref = xrefA;
  for (int i = 0; i < nT3Fonts; ++i) {
    delete t3FontCache[i];
  }
  nT3Fonts = 0;
}

void AOutputDev::startPage(int pageNum, GfxState *state) {

  pageState.reset();
  gfxState = &pageState;
  //transformedState.reset();

  strokeColor = 0x000000UL;
  paperColor = 0xffffffUL;

  //if (libBase)
    pageState.fill(0, 0, 0xffff, 0xffff, paperColor);
  /*else {
    error(-1, "Can't load graphics module.");
    memset(bitmap.data, 0, bitmap.modulo * bitmap.height * bitmap.bpp);
  }*/

  fillColor = strokeColor;
  linkColor = Gulong(-1);

  // clear font
  gfxFont = NULL;
  font = NULL;

  // clear text object
  text->startPage(state);

  endPageCalled = gFalse;
}

void AOutputDev::endPage() {
  if (!endPageCalled) {
    pageState.reset();
    gfxState = &pageState;
    //colorTable.clear();
    text->coalesce(gTrue);
    APath::destroy(textClipPath);
    textClipPath = NULL;
    endPageCalled = gTrue;
    clearT3GlyphStack();
  }
}

#if 0
Gulong AOutputDev::getPixelColor(int x, int y) {
  x -= bitmap.xoffset;
  y -= bitmap.yoffset;
  if (x < 0)
    x = 0;
  else if (x >= bitmap.width)
    x = bitmap.width - 1;
  if (y < 0)
    y = 0;
  else if (y >= bitmap.height)
    y = bitmap.height - 1;
  if (x < bitmap.width && y < bitmap.height) {
    int offset = y * bitmap.modulo + x;
    Guchar *p = bitmap.data() + bitmap.bpp * offset;
    Gulong r;
    switch(bitmap.pixfmt) {
      case ABitmap::lut8:
	r = colorTable.getPenValue(*p);
	break;

      case ABitmap::rgb15:
	r = ((*(Gushort*)p & 0x7c00) << 17) |
	    ((*(Gushort*)p & 0x03e0) << 14) |
	    ((*(Gushort*)p & 0x001f) << 11);
	break;

      case ABitmap::bgr15:
	r = ((*(Gushort*)p & 0x7c00) << 1) |
	    ((*(Gushort*)p & 0x03e0) << 14) |
	    ((*(Gushort*)p & 0x001f) << 27);
	break;

      case ABitmap::rgb15pc:
	r = ((*(Gushort*)p & 0x007c) << 25) |
	    ((*(Gushort*)p & 0x0003) << 22) |
	    ((*(Gushort*)p & 0xe000) << 6) |
	    ((*(Gushort*)p & 0x1f00) << 3);
	break;

      case ABitmap::bgr15pc:
	r = ((*(Gushort*)p & 0x1f00) << 19) |
	    ((*(Gushort*)p & 0xe000) << 6) |
	    ((*(Gushort*)p & 0x0003) << 22) |
	    ((*(Gushort*)p & 0x007c) << 9);
	break;

      case ABitmap::rgb16:
	r = ((*(Gushort*)p & 0xf800) << 16) |
	    ((*(Gushort*)p & 0x07e0) << 13) |
	    ((*(Gushort*)p & 0x001f) << 11);
	break;

      case ABitmap::bgr16:
	r =  (*(Gushort*)p & 0xf800) |
	    ((*(Gushort*)p & 0x07e0) << 13) |
	    ((*(Gushort*)p & 0x001f) << 27);
	break;

      case ABitmap::rgb16pc:
	r = ((*(Gushort*)p & 0x00f8) << 24) |
	    ((*(Gushort*)p & 0x0007) << 21) |
	    ((*(Gushort*)p & 0xe000) << 5) |
	    ((*(Gushort*)p & 0x1f00) << 3);
	break;

      case ABitmap::bgr16pc:
	r = ((*(Gushort*)p & 0x1f00) << 19) |
	    ((*(Gushort*)p & 0xe000) << 5) |
	    ((*(Gushort*)p & 0x0007) << 21) |
	    ((*(Gushort*)p & 0x00f8) << 8);
	break;

      case ABitmap::rgb24:
	r = (p[0] << 24) | (p[1] << 16) | (p[2] << 8);
	break;

      case ABitmap::argb32:
	r = *(Gulong*)p << 8;
	break;

      case ABitmap::bgr24:
	r = (p[0] << 8) | (p[1] << 16) | (p[2] << 24);
	break;

      case ABitmap::bgra32:
	r = *(Gulong*)p;
	r = ((r & 0x0000ff00) << 16) |
	     (r & 0x00ff0000) |
	    ((r & 0xff000000) >> 16);
	break;

      case ABitmap::rgba32:
	r = *(Gulong*)p;
	break;
    }
  }
  return paperColor;
}
#endif

#if 0
void AOutputDev::drawLinkBorder(double x1, double y1, double x2, double y2,
				double w) {
  //~ w ignored.
  int u1, v1, u2, v2, u, v;
  const Color linkColor = 0x00ff0000;
  DB(printf("\ndrawlink(%f,%f,%f,%f,%f)\n",x1,y1,x2,y2,w);)

  //TransformedGfxState state1;
  //XSetLineAttributes(display, strokeGC, xoutRound(w),
  //                   LineSolid, CapRound, JoinRound);
  DashState ds(0,NULL,0);
  cvtUserToDev(x1, y1, &u, &v);
  cvtUserToDev(x2, y1, &u1, &v1);
  gfxState->line(u, v, u1, v1, linkColor, ds);
  cvtUserToDev(x2, y2, &u2, &v2);
  gfxState->line(u1, v1, u2, v2, linkColor, ds);
  cvtUserToDev(x1, y2, &u1, &v1);
  gfxState->line(u2, v2, u1, v1, linkColor, ds);
  gfxState->line(u1, v1, u, v, linkColor, ds);
}
#endif

void AOutputDev::saveState(GfxState *state) {
  DB(printf("\nsave state...\n"));
  gfxState->push();
  DB(printf("...save state\n");)
}

void AOutputDev::restoreState(GfxState *state) {
  DB(printf("\nrestore state...\n");)
  gfxState->pop();
  updateAll(state);
  DB(printf("...restore state\n");)
}

void AOutputDev::updateAll(GfxState *state) {
  updateLineAttrs(state, gTrue);
  updateFlatness(state);
  updateMiterLimit(state);
  updateFillColor(state);
  updateStrokeColor(state);
  updateFont(state);
}

void AOutputDev::updateCTM(GfxState *state, double m11, double m12,
			   double m21, double m22, double m31, double m32) {
  updateLineAttrs(state, gTrue);
}

void AOutputDev::updateLineDash(GfxState *state) {
  updateLineAttrs(state, gTrue);
}

void AOutputDev::updateFlatness(GfxState *state) {
  //flatness = state->getFlatness();
}

void AOutputDev::updateLineJoin(GfxState *state) {
  updateLineAttrs(state, gFalse);
}

void AOutputDev::updateLineCap(GfxState *state) {
  updateLineAttrs(state, gFalse);
}

void AOutputDev::updateMiterLimit(GfxState *state) {
  double ml = state->getMiterLimit();
  if (ml < 0.9999) {
    error(-1, "Bad miter limit (%f)", ml);
    ml = 1;
  }
  miterLimit = ml;
}

void AOutputDev::updateLineWidth(GfxState *state) {
  updateLineAttrs(state, gFalse);
}

void AOutputDev::updateLineAttrs(GfxState *state, GBool updateDash) {
  int cap = state->getLineCap();
  int join = state->getLineJoin();
  double lw = state->getTransformedLineWidth();

  if (cap < 0 || cap > 2)
    error(-1, "Bad line cap style (%d)", cap);

  if (join < 0 || join > 2)
    error(-1, "Bad line join style (%d)", join);

  capStyle = CapStyle(cap);
  joinStyle = JoinStyle(join);
  lineWidth = lw;

  if (updateDash) {
    double *dashPattern1;
    int dashLength1;
    double dashStart1;

    state->getLineDash(&dashPattern1, &dashLength1, &dashStart1);

    if (dashLength1 > 0) {
      int *dashPattern = new int [dashLength1];
      int dashStart = xoutRound(state->transformWidth(dashStart1) *
		      (1 << DashState::fract_bits));
      for (int i = 0; i < dashLength1; ++i) {
	dashPattern[i] = xoutRound(state->transformWidth(dashPattern1[i]) *
			(1 << DashState::fract_bits));
	if (dashPattern[i] == 0)
	  dashPattern[i] = 1;
      }
      dashState.init(dashLength1, dashPattern, dashStart);
    } else
      dashState.reset();
  }
}

void AOutputDev::updateFillColor(GfxState *state) {
  GfxRGB rgb;

  state->getFillRGB(&rgb);
  fillColor = makeColor(&rgb);
  //DB(printf("\nfill color=%d\n",fillColor);)
}

void AOutputDev::updateStrokeColor(GfxState *state) {
  GfxRGB rgb;

  state->getStrokeRGB(&rgb);
  strokeColor = makeColor(&rgb);
  //DB(printf("\nstroke color=%d\n",strokeColor);)
}

void AOutputDev::updateFont(GfxState *state) {
  double m11, m12, m21, m22;

  if (!(gfxFont = state->getFont()) ||
      gfxFont->getType() == fontType3) {
    font = NULL;
    return;
  }

  state->getFontTransMat(&m11, &m12, &m21, &m22);
  m11 *= state->getHorizScaling();
  m12 *= state->getHorizScaling(); /* m21 ???? */

  font = (AOutputFont *)baseFontCache->getFont(xref, gfxFont, m11, m12, m21, m22);
  DB(printf("font=%x\n",font));

  text->updateFont(state);
}


void AOutputDev::doStroke(APath *path) {
  gfxState->stroke(0, 0, path, strokeColor, lineWidth, capStyle, joinStyle, miterLimit, dashState);
}


void AOutputDev::doFill(APath *path, bool eo) {
  int x1, y1, x2, y2;
  if (path->is_rect(x1, y1, x2, y2)) {
    gfxState->fill(x1, y1, x2, y2, fillColor);
  } else {
    ARegion *region = path->make_region(eo);
    if (region) {
      gfxState->fill(0, 0, region, fillColor);
      delete region;
    }
  }
}


void AOutputDev::doClip(APath *path, bool eo) {
  int x1, y1, x2, y2;
  if (path->is_rect(x1, y1, x2, y2)) {
    gfxState->add_clip(x1, y1, x2, y2);
  } else {
    ARegion *region = path->make_region(eo);
    if (region) {
      gfxState->add_clip(0, 0, region);
    }
  }
}

void AOutputDev::pathOp(GfxState *state, int op) {
  PATHDB(printf("\npathOp(%lx)...\n",op);)

  GBool close = (op & (popClose | popFill | popEOFill | popClip | popEOClip)) != 0;

  APath *path = convertPath(state, close);

  if (path) {

    if (op & popFill)
      doFill(path, false);
    else if (op & popEOFill)
      doFill(path, true);

    if (op & popStroke)
      doStroke(path);

    if (op & popClip)
      doClip(path, false);
    else if (op & popEOClip)
      doClip(path, true);

    *drawStatePtr = ++lastDrawState;

    delete path;
  }

  PATHDB(printf("...pathop\n");)
}


APath *AOutputDev::convertPath(GfxState *state, GBool close) {
  APath::Part *res = NULL;
  double x = 0;
  double y = 0;

  PATHDB(printf("convertPath(%d)\n", close));

  // get path and number of subpaths
  GfxPath *path = state->getPath();
  int n = path->getNumSubpaths();

  // do each subpath
  for (int i = 0; i < n; ++i) {
    res = APath::join(res, convertSubpath(state, path->getSubpath(i), x, y, close));
  }

  return new APath(res);
}

APath::Part *AOutputDev::convertSubpath(GfxState *state, GfxSubpath *subpath,
    double& x, double& y, GBool close) {
  APath::Part *res = NULL;
  APath::Part *p1;
  int m, i;

  PATHDB(printf("convertSubpath(%d)\n", close));

  m = subpath->getNumPoints();
  if (m > 1) {
    double x0, y0, xa, ya;
    state->transform(subpath->getX(0), subpath->getY(0), &x0, &y0);
    res = APath::move(x0 - x, y0 - y);
    x = xa = x0;
    y = ya = y0;
    i = 1;
    while (i < m) {
      double x1, y1;
      state->transform(subpath->getX(i), subpath->getY(i), &x1, &y1);
      if (i >= 1 && subpath->getCurve(i)) {
	double x2, y2, x3, y3;
	state->transform(subpath->getX(i + 1), subpath->getY(i + 1), &x2, &y2);
	state->transform(subpath->getX(i + 2), subpath->getY(i + 2), &x3, &y3);
	res = APath::join(res, APath::bezier(x1 - x, y1 - y,
			   x2 - x, y2 - y,
			   x3 - x, y3 - y));
	i += 3;
	x = x3;
	y = y3;
      } else {
	res = APath::join(res, APath::line(x1 - x, y1 - y));
	++i;
	x = x1;
	y = y1;
      }
    }
    if (res && (close || subpath->isClosed())) {
      res = APath::close(res);
      x = xa;
      y = ya;
    }
  }
  return res;
}

void AOutputDev::endTextObject(GfxState *state) {
  if (textClipPath) {
    APath path(textClipPath);
    textClipPath = NULL;
    ARegion *region = path.make_region();
    if (region) {
      gfxState->add_clip(0, 0, region);
    }
  }
}

void AOutputDev::beginString(GfxState *state, GString *s) {
  //printf("(%s)\n",s->getCString());
  //text->beginWord(state, state->getCurX(), state->getCurY());
}

void AOutputDev::endString(GfxState *state) {
  //text->endWord();
  *drawStatePtr = ++lastDrawState;
}

#if 0
void AOutputDev::getAAColors(Gulong *colors) {
  Gulong c0, c1, c2, c3, c4;
  c0 = colors[0];
  c4 = colors[4];
  long r = ((c4>>2)&0x3fc00000UL) - ((c0>>2)&0x3fc00000UL);
  c1 = ((c0 & 0xff000000UL) + r) & 0xff000000UL;
  c2 = ((c0 & 0xff000000UL) + 2*r) & 0xff000000UL;
  c3 = ((c0 & 0xff000000UL) + 3*r) & 0xff000000UL;
  long g = ((c4>>2)&0x003fc000UL) - ((c0>>2)&0x003fc000UL);
  c1 |= ((c0 & 0x00ff0000UL) + g) & 0x00ff0000UL;
  c2 |= ((c0 & 0x00ff0000UL) + 2*g) & 0x00ff0000UL;
  c3 |= ((c0 & 0x00ff0000UL) + 3*g) & 0x00ff0000UL;
  long b = ((c4>>2)&0x00003fc0UL) - ((c0>>2)&0x00003fc0UL);
  c1 |= ((c0 & 0x0000ff00UL) + b) & 0x0000ff00UL;
  c2 |= ((c0 & 0x0000ff00UL) + 2*b) & 0x0000ff00UL;
  c3 |= ((c0 & 0x0000ff00UL) + 3*b) & 0x0000ff00UL;
  if (!trueColor) {
    c1 = colorTable.find(c1);
    c2 = colorTable.find(c2);
    c3 = colorTable.find(c3);
  }
  colors[1] = c1;
  colors[2] = c2;
  colors[3] = c3;
}
#endif

void AOutputDev::drawChar(GfxState *state, double x, double y,
			  double dx, double dy,
			  double originX, double originY,
			  CharCode code, int nBytes, Unicode *u, int uLen) {
  double x1, y1, dx1, dy1;
  //DB(printf("\ndrawchar(%d,%d) font %p render %d\n",code,*u,font,state->getRender());)

  text->addChar(state, x, y, dx, dy, code, nBytes, u, uLen);

  if (!font) {
    return;
  }

  x -= originX;
  y -= originY;
  state->transform(x, y, &x1, &y1);
  state->transformDelta(dx, dy, &dx1, &dy1);

  if (font->outside(*gfxState->bitmap, x1, y1, code, *u)) {
    if (state->getRender() & 4) {
      gfxState->add_clip(0, 0, 0, 0);
    }
  } else {
    bool txFill = false;
    bool txStroke = false;

    switch (state->getRender() & 3) {
    case 0: txFill = true; break;
    case 1: txStroke = true; break;
    case 2: txFill = true; txStroke = true; break;
    }

    if (txFill) {
      if (ARegion* region = font->getCharInterior(code, *u)) {
	int w = region->width;
	int h = region->height;

	if (w > 0 && h > 0)
	{
	  int x = xoutRound(x1);
	  int y = xoutRound(y1);

	  gfxState->fill(x, y, region, fillColor);
	}
      }
    }
    if (txStroke) {
      if (APath::Part *outline = font->getCharOutline(code, *u)) {
	APath path(outline);
	int x = xoutRound(x1);
	int y = xoutRound(y1);
	gfxState->stroke(x, y, &path, strokeColor, lineWidth, capStyle, joinStyle, miterLimit, dashState);
      }
    }
    if (state->getRender() & 4) {
      if (APath::Part *path = font->getCharOutline(code, *u)) {
	double dx, dy;
	APath::get_delta(path, dx, dy);
	if (textClipPath == NULL) {
	  APath::move(path, x1, y1);
	  textClipPath = path;
	} else {
	  APath::move(path, x1 - textClipLastX, y1 - textClipLastY);
	  textClipPath = APath::join(textClipPath, path);
	}
	textClipLastX = x1 + dx;
	textClipLastY = y1 + dy;
      }
    }
  }
}

GBool AOutputDev::beginType3Char(GfxState *state, double x, double y,
				 double dx, double dy,
				 CharCode code, Unicode *u, int uLen) {
  Ref *fontID;
  double *ctm, *bbox;
  //GfxRGB color;
  T3FontCache *t3Font;
  T3GlyphStack *t3gs;
  double x1, y1, xMin, yMin, xMax, yMax, xt, yt;
  int i, j;

  T3DB(printf("beginType3Char: code %d x %f y %f dx %f dy %f\n", code, x, y, dx, dy));

  //text->addChar(state, x, y, dx, dy, code, u, uLen);

  if (!gfxFont) {
    T3DB(printf("beginType3Char: no font\n"));
    return gFalse;
  }
  fontID = gfxFont->getID();
  ctm = state->getCTM();
  state->transform(0, 0, &xt, &yt);

  // is it the first (MRU) font in the cache?
  if (!(nT3Fonts > 0 &&
	t3FontCache[0]->matches(fontID, ctm[0], ctm[1], ctm[2], ctm[3]))) {

    // is the font elsewhere in the cache?
    for (i = 1; i < nT3Fonts; ++i) {
      if (t3FontCache[i]->matches(fontID, ctm[0], ctm[1], ctm[2], ctm[3])) {
	t3Font = t3FontCache[i];
	for (j = i; j > 0; --j) {
	  t3FontCache[j] = t3FontCache[j - 1];
	}
	t3FontCache[0] = t3Font;
	break;
      }
    }
    if (i >= nT3Fonts) {

      T3DB(printf("create new cache entry\n"));

      // create new entry in the font cache
      if (nT3Fonts == xOutT3FontCacheSize) {
	delete t3FontCache[nT3Fonts - 1];
	--nT3Fonts;
      }
      for (j = nT3Fonts; j > 0; --j) {
	t3FontCache[j] = t3FontCache[j - 1];
      }
      ++nT3Fonts;
      bbox = gfxFont->getFontBBox();
      if (bbox[0] == 0 && bbox[1] == 0 && bbox[2] == 0 && bbox[3] == 0) {
	// broken bounding box -- just take a guess
	xMin = xt - 5;
	xMax = xMin + 30;
	yMax = yt + 15;
	yMin = yMax - 45;
      } else {
	state->transform(bbox[0], bbox[1], &x1, &y1);
	xMin = xMax = x1;
	yMin = yMax = y1;
	state->transform(bbox[0], bbox[3], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
	state->transform(bbox[2], bbox[1], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
	state->transform(bbox[2], bbox[3], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
      }
      t3FontCache[0] = new T3FontCache(fontID, ctm[0], ctm[1], ctm[2], ctm[3],
				       (int)floor(xMin - xt),
				       (int)floor(yMin - yt),
				       (int)ceil(xMax) - (int)floor(xMin) + 3,
				       (int)ceil(yMax) - (int)floor(yMin) + 3);
    }
  }
  t3Font = t3FontCache[0];

  // is the glyph in the cache?
  i = (code & (t3Font->cacheSets - 1)) * t3Font->cacheAssoc;
  for (j = 0; j < t3Font->cacheAssoc; ++j) {
    if ((t3Font->cacheTags[i+j].mru & 0x8000) &&
	t3Font->cacheTags[i+j].code == code) {
      //state->getFillRGB(&color);
      /*if (reverseVideo) {
	color.r = 1 - color.r;
	color.g = 1 - color.g;
	color.b = 1 - color.b;
      }*/
      /*text->addChar(state, 0, 0,
		    t3Font->cacheTags[i+j].wx, t3Font->cacheTags[i+j].wy,
		    code, u, uLen);*/
      T3DB(printf("beginT3Char: glyph in cache (%p)\n", t3Font->cacheTags[i+j].glyph));
      if (t3Font->cacheTags[i+j].glyph)
	gfxState->fill(xoutRound(xt), xoutRound(yt), t3Font->cacheTags[i+j].glyph, fillColor);
#if 0
      drawType3Glyph(t3Font, &t3Font->cacheTags[i+j],
		     t3Font->cacheData + (i+j) * t3Font->glyphSize,
		     xt, yt, fillColor/*&color*/);
#endif
      return gTrue;
    }
  }

  // push a new Type 3 glyph record
  t3gs = new T3GlyphStack();
  t3gs->next = t3GlyphStack;
  t3GlyphStack = t3gs;
  t3GlyphStack->code = code;
  t3GlyphStack->x = xt;
  t3GlyphStack->y = yt;
  t3GlyphStack->cache = t3Font;
  t3GlyphStack->cacheTag = NULL;
  //t3GlyphStack->u = u;
  //t3GlyphStack->uLen = uLen;
  t3GlyphStack->bitmap = NULL;

  return gFalse;
}

void AOutputDev::endType3Char(GfxState *state) {
  T3GlyphStack *t3gs = t3GlyphStack;
  double *ctm;

  T3DB(printf("endType3Char: cacheTag %p\n", t3gs->cacheTag));

  if (t3gs->cacheTag) {
    ARegion *r = t3gs->cacheTag->glyph;
    if (r)
      r->clear();
    else
      r = t3gs->cacheTag->glyph = new ARegion;
    int w = t3gs->bitmap->width;
    int h = t3gs->bitmap->height;
    int dx = -t3gs->cache->glyphX;
    r->x0 = dx;
    r->y0 = -t3gs->cache->glyphY;
    r->width = w;
    r->height = h;
    T3DB(printf("endType3Char: %d×%d bitmap\n", w, h));
    if (r->alloc_rows(r->height)) {
      int y = -r->y0;
      int8u* p = t3gs->bitmap->data;
      while (h--) {
	ARegion::Span spans[32];
	int cur = 0;
	int x = 0;
	do {
	  while (x < w && p[x] == 0xff)
	    ++x;
	  if (x < w) {
	    int8u alpha = p[x];
	    int len = 1;
	    spans[cur].x = x + dx;
	    spans[cur].alpha = ~alpha;
	    while (len < 255 && ++x < w && p[x] == alpha)
	      ++len;
	    spans[cur].length = len;
	    ++cur;
	  }
	  if (cur == 32 || x == w) {
	    ARegion::Span *q = r->add_spans(y, cur);
	    if (q) {
	      memcpy(q, spans, cur * sizeof(*q));
	    }
	    cur = 0;
	  }
	} while (x < w);
	p += t3gs->bitmap->modulo;
	++y;
      }
    }
    ctm = state->getCTM();
    state->setCTM(ctm[0], ctm[1], ctm[2], ctm[3],
		  t3GlyphStack->origCTM4, t3GlyphStack->origCTM5);
    gfxState = t3gs->origState;
    fillColor = t3gs->origFillColor;
    strokeColor = t3gs->origStrokeColor;
    gfxState->fill(xoutRound(t3gs->x), xoutRound(t3gs->y), r, fillColor);
  }
  t3GlyphStack = t3gs->next;
  delete t3gs->bitmap;
  delete t3gs;
}

void AOutputDev::type3D0(GfxState *state, double wx, double wy) {
  //t3GlyphStack->wx = wx;
  //t3GlyphStack->wy = wy;
}

void AOutputDev::type3D1(GfxState *state, double wx, double wy,
			 double llx, double lly, double urx, double ury) {
  GfxColor fgColor;
  double *ctm;
  T3FontCache *t3Font;
  double xt, yt, xMin, xMax, yMin, yMax, x1, y1;
  int i, j;

  t3Font = t3GlyphStack->cache;
  //t3GlyphStack->wx = wx;
  //t3GlyphStack->wy = wy;

  // check for a valid bbox
  state->transform(0, 0, &xt, &yt);
  state->transform(llx, lly, &x1, &y1);
  xMin = xMax = x1;
  yMin = yMax = y1;
  state->transform(llx, ury, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  state->transform(urx, lly, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  state->transform(urx, ury, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  if (xMin - xt < t3Font->glyphX ||
      yMin - yt < t3Font->glyphY ||
      xMax - xt > t3Font->glyphX + t3Font->glyphW ||
      yMax - yt > t3Font->glyphY + t3Font->glyphH) {
/*printf("xmin %f xmax %f ymin %f ymax %f xt %f yt %f gx %f gy %f gw %f gh %f\n",
    xMin,yMin,xMax,yMax,xt,yt,t3Font->glyphX,t3Font->glyphY,t3Font->glyphW,t3Font->glyphH);*/
    error(-1, "Bad bounding box in Type 3 glyph");
    return;
  }


  // allocate a cache entry
  i = (t3GlyphStack->code & (t3Font->cacheSets - 1)) * t3Font->cacheAssoc;
  for (j = 0; j < t3Font->cacheAssoc; ++j) {
    if ((t3Font->cacheTags[i+j].mru & 0x7fff) == t3Font->cacheAssoc - 1) {
      t3Font->cacheTags[i+j].mru = 0x8000;
      t3Font->cacheTags[i+j].code = t3GlyphStack->code;
      t3GlyphStack->cacheTag = &t3Font->cacheTags[i+j];
      //t3GlyphStack->cacheData = t3Font->cacheData + (i+j) * t3Font->glyphSize;
    } else {
      ++t3Font->cacheTags[i+j].mru;
    }
  }
  //t3GlyphStack->cacheTag->wx = wx;
  //t3GlyphStack->cacheTag->wy = wy;

  // save state
  //t3GlyphStack->origBitmap = bitmap;
  //t3GlyphStack->origSplash = splash;
  t3GlyphStack->origState = gfxState;
  t3GlyphStack->origStrokeColor = strokeColor;
  t3GlyphStack->origFillColor = fillColor;
  ctm = state->getCTM();
  t3GlyphStack->origCTM4 = ctm[4];
  t3GlyphStack->origCTM5 = ctm[5];

  // create the temporary bitmap
  t3GlyphStack->bitmap = new ABitmap(t3Font->glyphW, t3Font->glyphH, PixFmt::get(PixFmt::red8));

  //fgColor.c[0] = 0;
  //state->setFillColorSpace(new GfxDeviceGrayColorSpace());
  //state->setFillColor(&fgColor);
  //state->setStrokeColorSpace(new GfxDeviceGrayColorSpace());
  //state->setStrokeColor(&fgColor);
  gfxState = &t3GlyphStack->state;
  t3GlyphStack->state.bitmap = t3GlyphStack->bitmap;
  //updateLineAttrs(state, gTrue);
  gfxState->fill(0, 0, 0xffff, 0xffff, 0xffffff);
  strokeColor = 0x000000;
  fillColor = 0x000000;
  //~ this should copy other state from t3GlyphStack->origSplash?
  state->setCTM(ctm[0], ctm[1], ctm[2], ctm[3],
		-t3Font->glyphX, -t3Font->glyphY);
}

#if 1

#if 0
void AOutputDev::drawPicSlice(DrawPicParams& params,int y1, int y2,
			      int xa1, int xa2, int xb1, int xb2,
			      int xpa1, int ypa1, int xpa2, int ypa2,
			      int xpb1, int ypb1, int xpb2, int ypb2) {

  IDB(printf("drawPicSlice(y = %d->%d, x1 = %d->%d,  x2 = %d->%d\n"
	     "(%d,%d)->(%d,%d), (%d,%d)->(%d,%d))\n",
     y1, y2, xa1, xa2, xb1, xb2, xpa1, ypa1, xpa2, ypa2, xpb1, ypb1, xpb2, ypb2));

  if (*abortFlagPtr)
    return;

  if (y2 == y1) // shouldn't happen
    return;

  int dxa = ((xa2 - xa1) <<16) / (y2 - y1);
  int dxb = ((xb2 - xb1) <<16) / (y2 - y1);
  int dxpa = ((xpa2 - xpa1) << 16) / (y2 - y1);
  int dypa = ((ypa2 - ypa1) << 16) / (y2 - y1);
  int dxpb = ((xpb2 - xpb1) << 16) / (y2 - y1);
  int dypb = ((ypb2 - ypb1) << 16) / (y2 - y1);
  int xa = xa1 << 16;
  int xb = xb1 << 16;
  int xpa = xpa1 << 16;
  int ypa = ypa1 << 16;
  int xpb = xpb1 << 16;
  int ypb = ypb1 << 16;

  if (xb1 < xa1 || xb2 < xa2) {
      int t;
      t = xa; xa = xb; xb = t;
      t = dxa; dxa = dxb; dxb = t;
      t = xpa; xpa = xpb; xpb = t;
      t = ypa; ypa = ypb; ypb = t;
      t = dxpa; dxpa = dxpb; dxpb = t;
      t = dypa; dypa = dypb; dypb = t;
  }

  for (int y = y1; y <= y2; ++y) {
    if (*abortFlagPtr)
      return;
    if (y >= 0 && y < params.bm->height)
	drawPicLine(params, y, xa >> 16, xb >> 16,
		    xpa >> 16, ypa >> 16, xpb >> 16, ypb >> 16);
    xa += dxa;
    xb += dxb;
    xpa += dxpa;
    ypa += dypa;
    xpb += dxpb;
    ypb += dypb;
  }
}
#endif

void AOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
			   int width, int height, GfxImageColorMap *colorMap,
			   int *maskColors, GBool inlineImg, GBool invert) {

  class ImageStreamPtr {
  public:
    ImageStreamPtr(ImageStream *s) : str(s) {}
    ~ImageStreamPtr() { delete str; }
    ImageStream *operator -> () const { return str; }
  private:
    ImageStream *str;
  };

  /*int x0, y0; // \ destination box, clipped
  int w0, h0; // /
  int x1, y1; // \ destination box, not clipped
  int w1, h1; // /
  int x2, y2; // \ visible box, relative to (x1,y1)
  int w2, h2; // /
  double xt, yt, wt, ht;
  int xStep;  // step in the destination area when moving from
	      // left to right in a row of the source picture
  int yStep;  // step in the destination area when moving from
	      // top to bottom in a column of the source picture
  GBool oblique;
  GfxRGB pixBuf;*/
  Guchar pixBuf2[gfxColorMaxComps];
  GBool dither;
  int nComps, nVals, nBits;
  int modulo;
  GBool isMask = colorMap == NULL;
  double *ctm, d;
  ABitmap& bitmap = *gfxState->bitmap;
  PixFmt *pixfmt = bitmap.pixfmt;

  // setup
  if (isMask) {
    nComps = 1;
    nVals = width;
    nBits = 1;
    dither = gFalse;
  } else {
    nComps = colorMap->getNumPixelComps();
    nVals = width * nComps;
    nBits = colorMap->getBits();
    dither = (nComps > 1 || nBits > 1) && !pixfmt->is_truecolor();
  }

  IDB(printf("width %d height %d comps %d bits %d\n", width, height, nComps, nBits));
  IDB(printf("bitmap: width %d height %d xoffset %d yoffset %d modulo %d bpp %d data %p\n",
	bitmap.width, bitmap.height, bitmap.xoffset, bitmap.yoffset, bitmap.modulo,
	bitmap.bpp, bitmap.data));

  // get CTM, check for singular matrix
  ctm = state->getCTM();
  d = ctm[0] * ctm[3] - ctm[1] * ctm[2];
  if (fabs(d) < 0.000001) {
    error(-1, "Singular CTM in drawImage");
    if (inlineImg) {
      str->reset();
      int j = height * ((nVals * nBits + 7) / 8);
      for (int i = 0; i < j; ++i) {
	str->getChar();
      }
      str->close();
    }
    return;
  }

  if (drawLevel == 0) {
    newDrawLevel = 1;
    if (inlineImg) {
      int j = height * ((nVals * nBits + 7) / 8);
      str->reset();
      for (int i = 0; i < j; ++i)
	str->getChar();
      str->close();
    }
    return;
  }

  // get image position in the destination bitmap

  int ulx1 = xoutRound(ctm[4]);
  int uly1 = xoutRound(ctm[5]);
  int urx1 = xoutRound(ctm[0] + ctm[4]);
  int ury1 = xoutRound(ctm[1] + ctm[5]);
  int llx1 = xoutRound(ctm[2] + ctm[4]);
  int lly1 = xoutRound(ctm[3] + ctm[5]);
  int lrx1 = xoutRound(ctm[0] + ctm[2] + ctm[4]);
  int lry1 = xoutRound(ctm[1] + ctm[3] + ctm[5]);

  IDB(printf("ul (%d,%d) ur (%d,%d) ll (%d,%d) lr (%d,%d)\n",
	ulx1, uly1, urx1, ury1, llx1, lly1, lrx1, lry1));

  // compute the bounding box in the destination bitmap

  int xmin, ymin, xmax, ymax;
  xmin = xmax = ulx1;
  ymin = ymax = uly1;
  if (urx1 < xmin) xmin = urx1; else if (urx1 > xmax) xmax = urx1;
  if (ury1 < ymin) ymin = ury1; else if (ury1 > ymax) ymax = ury1;
  if (llx1 < xmin) xmin = llx1; else if (llx1 > xmax) xmax = llx1;
  if (lly1 < ymin) ymin = lly1; else if (lly1 > ymax) ymax = lly1;
  if (lrx1 < xmin) xmin = lrx1; else if (lrx1 > xmax) xmax = lrx1;
  if (lry1 < ymin) ymin = lry1; else if (lry1 > ymax) ymax = lry1;

  IDB(printf("bbox: (%d,%d)-(%d,%d)\n", xmin, ymin, xmax, ymax));

  // check for images outside the bitmap.

  if (xmax < bitmap.xoffset || ymax < bitmap.yoffset ||
      xmin >= bitmap.xoffset + bitmap.width ||
      ymin >= bitmap.yoffset + bitmap.height) {
    if (inlineImg) {
      int j = height * ((nVals * nBits + 7) / 8);
      str->reset();
      for (int i = 0; i < j; ++i)
	str->getChar();
      str->close();
    }
    return;
  }

  // read the image

  ImageStreamPtr imgStr(new ImageStream(str, width, nComps, nBits));
  imgStr->reset();

  ArrayPtr<Color> mem(new Color [width * height]);
  Color *p = mem + width * (height - 1);
  IDB(printf("mem = %lx-%lx\n", (Color*)mem, mem + width * height));

  if (isMask) {
    //maskMem = new Guchar [width * height];
    //Guchar *p = maskMem;
    //IDB(printf("maskMem = %lx-%lx\n", (Guchar*)maskMem, (Guchar*)maskMem+width*height));

    Color front = fillColor;
    Color back = 0xff000000UL;

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
	imgStr->getPixel(pixBuf2);
	*p++ = pixBuf2[0] == invert ? front : back;
      }
      p -= width * 2;
    }

  } else {
    GfxRGB c;

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
	imgStr->getPixel(pixBuf2);
	colorMap->getRGB(pixBuf2, &c);
	Color color = makeColor(&c);
	if (maskColors) {
	  color |= 0xff000000UL;
	  for (int k = 0; k < nComps; ++k) {
	    if (pixBuf2[k] < maskColors[2*k] ||
		pixBuf2[k] > maskColors[2*k]) {
	      color &= ~0x00ffffffUL;
	      break;
	    }
	  }
	}
	*p++ = color;
      }
      p -= width * 2;
    }
  }

  // clip the bounding box.

  if (xmin < bitmap.xoffset) xmin = bitmap.xoffset;
  if (ymin < bitmap.yoffset) ymin = bitmap.yoffset;
  if (xmax >= bitmap.xoffset + bitmap.width) xmax = bitmap.width + bitmap.xoffset - 1;
  if (ymax >= bitmap.yoffset + bitmap.height) ymax = bitmap.height + bitmap.yoffset - 1;

  IDB(printf("clipped bbox: (%d,%d)-(%d,%d)\n", xmin, ymin, xmax, ymax));

  // compute the position and size of the clipped bounding box in the
  // source image.

  int x1 = xoutRound(width * (ctm[3] * (xmin - ctm[4]) - ctm[2] * (ymin - ctm[5])) / d);
  int y1 = xoutRound(height * (ctm[0] * (ymin - ctm[5]) - ctm[1] * (xmin - ctm[4])) / d);
  int dx1 = xoutRound(width * ctm[3] * (xmax - xmin + 1) / d);
  int dy1 = xoutRound(height * -ctm[1] * (xmax - xmin + 1) / d);
  int dx2 = xoutRound(width * -ctm[2] * (ymax - ymin + 1) / d);
  int dy2 = xoutRound(height * ctm[0] * (ymax - ymin + 1) / d);

  IDB(printf("(%d,%d) horiz (%d,%d) vert (%d,%d)\n",
	x1, y1, dx1, dy1, dx2, dy2));

  p = mem;
  int8u* q = bitmap.data + ((ymin - bitmap.yoffset) * bitmap.modulo +
      xmin - bitmap.xoffset) * bitmap.bpp;
  int dqx = bitmap.bpp;
  int dqy = (bitmap.modulo - xmax + xmin - 1) * bitmap.bpp;
  int w = abs(dx1 + dx2) / (xmax - xmin + 1);
  int h = abs(dy1 + dy2) / (ymax - ymin + 1);

  IDB(printf("w %d h %d dqx %d dqy %d p %p q %p\n", w, h, dqx, dqy, p, q));

  if (w == 0) w = 1;
  if (h == 0) h = 1;

  int total_weight = w * h * 255;

  x1 <<= 16;
  y1 <<= 16;
  dx1 <<= 16;
  dy1 <<= 16;
  if (dx1 < 0)
    dx1 = -((-dx1 + xmax - xmin) / (xmax - xmin + 1));
  else
    dx1 = (dx1 + xmax - xmin) / (xmax - xmin + 1);
  if (dy1 < 0)
    dy1 = -((-dy1 + xmax - xmin) / (xmax - xmin + 1));
  else
    dy1 = (dy1 + xmax - xmin) / (xmax - xmin + 1);
  dx2 <<= 16;
  dy2 <<= 16;
  if (dx2 < 0)
    dx2 = -((-dx2 + ymax - ymin) / (ymax - ymin + 1));
  else
    dx2 = (dx2 + ymax - ymin) / (ymax - ymin + 1);
  if (dy2 < 0)
    dy2 = -((-dy2 + ymax - ymin) / (ymax - ymin + 1));
  else
    dy2 = (dy2 + ymax - ymin) / (ymax - ymin + 1);

  for (int y = ymin; y <= ymax && !*abortFlagPtr; ++y) {

    int t = x1;
    int x3 = x1;
    int y3 = y1;

    for (int x = xmin; x <= xmax; ++x) {

      int y2 = y3 >> 16;
      int h2 = h;

      if (y2 + h2 > height) {
	h2 = height - y2;
      }

      if (y2 < 0) {
	h2 += y2;
	y2 = 0;
      }

      int x2 = x3 >> 16;
      int w2 = w;

      if (x2 + w2 > width) {
	w2 = width - x2;
      }

      if (x2 < 0) {
	w2 += x2;
	x2 = 0;
      }

      const Color *p2 = p + y2 * width + x2;

      int weight = 0;
      int r = 0, g = 0, b = 0;

      if (w2 > 0 && h2 > 0) {

	for (int j = 0; j < h2; ++j) {
	  for (int i = 0; i < w2; ++i) {
//if ((x >> 1) == 150 && (y >> 1) == 300) printf("(%d,%d)/(%d,%d) %08lx\n",x,y,i,j,p2[i]);
	    int a = 255 - (p2[i] >> 24);
	    r += ((p2[i] >> 16) & 0xff) * a;
	    g += ((p2[i] >> 8) & 0xff) * a;
	    b += (p2[i] & 0xff) * a;
	    weight += a;
	  }
	  p2 += width;
	}

	Color c = pixfmt->get_pixel(q);
/*if ((x >> 1) == 150 && (y >> 1) == 300)
  printf("c %08lx r %lx g %lx b %lx w %lx tw %lx\n", c, r,g,b,weight,total_weight);*/
	r += (total_weight - weight) * ((c >> 16) & 0xff);
	g += (total_weight - weight) * ((c >> 8) & 0xff);
	b += (total_weight - weight) * (c & 0xff);

	r /= total_weight;
	g /= total_weight;
	b /= total_weight;

/*if ((unsigned)r > 255 || (unsigned)g > 255 || (unsigned)b > 255)
  printf("r %x g %x b %x\n", r, g, b);*/
	pixfmt->put_pixel(q, (r << 16) | (g << 8) | b);

/*if ((x >> 1) == 150 && (y >> 1) == 300){
c=pixfmt->get_pixel(q);
  printf("r %x g %x b %x c %08lx\n", r, g, b,c);
}*/
      }

      q += dqx;
      x3 += dx1;
      y3 += dy1;
    }

    x1 = t;
    q += dqy;
    x1 += dx2;
    y1 += dy2;
  }

  *drawStatePtr = ++lastDrawState;

#if 0
  double d = ctm[0] * ctm[3] - ctm[1] * ctm[2];

  IDB(printf("image: xt=%f yt=%f wt=%f ht=%f\n",xt,yt,wt,ht);
      printf("bitmap: x=%d y=%d w=%d h=%d\n",bitmap.xoffset,
	     bitmap.yoffset,bitmap.width,bitmap.height);)

  x1 = xoutRound(xt) - bitmap.xoffset;
  y1 = xoutRound(yt) - bitmap.yoffset;

  state->transformDelta(1, 0, &xt, &yt);
  IDB(printf("horiz: %f,%f\n", xt, yt));
  if (fabs(xt) < 2 || fabs(yt) < 2) {
    // multiple of 90° rotation
    oblique = gFalse;
    w1 = w0 = xoutRound(fabs(wt));
    h1 = h0 = xoutRound(fabs(ht));
    x0 = wt >= 0 ? x1 : x1 - w0 + 1;
    y0 = ht >= 0 ? y1 : y1 - h0 + 1;

    // clip the image to the bitmap boundary
    if (x0 + w0 > bitmap.width)
      w2 = bitmap.width - x0;
    else
      w2 = w0;
    if (x0 < 0) {
      x2 = -x0;
      w2 += x0;
      x0 = 0;
    } else {
      x2 = 0;
    }
    if (y0 + h0 > bitmap.height)
      h2 = bitmap.height - y0;
    else
      h2 = h0;
    if (y0 < 0) {
      y2 = -y0;
      h2 += y0;
      y0 = 0;
    } else {
      y2 = 0;
    }
    w0 = w2;
    h0 = h2;

    if(bitmap.pixfmt->is_truecolor() || isMask) {
      modulo = bitmap.modulo;
    } else {
      modulo = w2;
    }

    // compute parameters
    //printf("horiz=%f,%f\n",xt,yt);
    if(fabs(xt) < fabs(yt)) {
      int t = w2;
      w2 = h2;
      h2 = t;
      t = x2;
      x2 = y2;
      y2 = t;
      t = w1;
      w1 = h1;
      h1 = t;
      if (ht > 0) {
	xStep = modulo;
      } else {
	xStep = -modulo;
	x2 = w1 - x2 - w2;
      }
      if (wt < 0) {
	yStep = 1;
      } else {
	yStep = -1;
	y2 = h1 - y2 - h2;
      }
    } else {
      // the picture is not rotated
      if (wt < 0) {
	xStep = -1;
	x2 = w1 - x2 - w2;
      } else {
	xStep = 1;
      }
      if (ht > 0) {
	yStep = -modulo;
	y2 = h1 - y2 - h2;
      } else {
	yStep = modulo;
      }
    }
  } else {
    oblique = gTrue;

    x2 = 0;
    y2 = 0;
    w2 = xoutRound(length(xt, yt));
    if (w2 > width) {
      w2 = width;
      h2 = height;
    } else {
      state->transformDelta(0, 1, &xt, &yt);
      h2 = xoutRound(length(xt, yt));
    }

    xStep = 1;
    yStep = -w2;
    w1 = w2;
    h1 = h2;
    modulo = w2;
  }

  IDB(printf("x0=%d y0=%d w0=%d h0=%d\n",x0,y0,w0,h0);
      printf("x1=%d y1=%d w1=%d h1=%d\n",x1,y1,w1,h1);
      printf("x2=%d y2=%d w2=%d h2=%d\n",x2,y2,w2,h2);
      printf("width=%d height=%d xstep=%d ystep=%d\n",width,height,xStep,yStep);
      printf("offs = %d (%d), oblique = %d\n",(xStep<0?-(w2-1)*xStep:0)+(yStep<0?-(h2-1)*yStep:0),w2*h2,oblique);)


  Guchar *q = NULL;
  ArrayPtr<Color> mem;
  ArrayPtr<Guchar> maskMem;

  IDB(printf("bitmap = %lx-%lx\n", bitmap.data, bitmap.data+bitmap.bpp*bitmap.modulo*bitmap.height);)
  if (isMask) {
    memset(q, 0, width * height);
  }

  // read the image

  ImageStreamPtr imgStr(new ImageStream(str, width, nComps, nBits));
  imgStr->reset();

  if (isMask) {
    maskMem = new Guchar [width * height];
    Guchar *p = maskMem;
    IDB(printf("maskMem = %lx-%lx\n", (Guchar*)maskMem, (Guchar*)maskMem+width*height));

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
	imgStr->getPixel(pixBuf2);
	*p++ = pixBuf2[0] == invert;
      }
    }

  } else {
    mem = new Color [width * height];
    Color *p = maskMem;
    IDB(printf("mem = %lx-%lx\n", (Guchar*)mem, mem+width*height));

    GfxRGB c;

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
	imgStr->getPixel(pixBuf2);
	colorMap->getRGB(pixBuf2, &c);
	Color color = pixfmt->rgb_to_int(colToByte(c.r), colToByte(c.g), colToByte(c.b)) | 0xff000000UL;
	if (maskColors) {
	  for (k = 0; k < nComps; ++k) {
	    if (pixBuf2[k] < maskColors[2*k] ||
		pixBuf2[k] > maskColors[2*k]) {
	      color &= ~0x00ffffffUL;
	      break;
	    }
	  }
	}
	*p++ = color;
      }
    }
  }

  if (xStep < 0)
    q -= (w2 - 1) * xStep;
  if (yStep < 0)
    q -= (h2 - 1) * yStep;

  IDB(printf("q = %lx, xStep = %d, yStep = %d\n", q, xStep, yStep);)

  // DDA parameters
  int dx, dy;
  if (w1 < 0)
    dx = -(((-w1 << 16) + width - 1) / width);
  else
    dx = ((w1 << 16) + width - 1) / width;
  if (h1 < 0)
    dy = -(((-h1 << 16) + height - 1) / height);
  else
    dy = ((h1 << 16) + height - 1) / height;

  x2 <<= 16;
  y2 <<= 16;
  w2 <<= 16;
  h2 <<= 16;

  int y3 = y2 + h2;
  int x3 = x2 + w2;
  int y6 = y2;

  // skip clipped lines
  int y = 0;
  int readLines = 0;
  while (y + dy <= y2) {
    imgStr->skipLine();
    y += dy;
    ++readLines;
  }

  if (isMask) {

    union {
      Gulong value;
      Guchar comp[4];
    } color;

    int n = oblique ? 1 : bitmap.bpp;
    color.value = oblique ? -1UL : pixfmt->color_to_int(fillColor);
    IDB(printf("n = %d\n", n);)

    while (y < y3 && !*abortFlagPtr) {
      int x = 0;
      int y5 = (min(y + dy, y3) >> 16) - (y6 >> 16);
      Guchar* t = q;
      int x6 = x2;
      for (int j = 0; j < width; ++j) {
	imgStr->getPixel(pixBuf2);
	if (x + dx > x2 && x < x3 && y5 > 0) {
	  int x5 = (min(x + dx, x3) >> 16) - (x6 >> 16);
	  if (x5 > 0) {
	    if (pixBuf2[0] == invert) {
	      Guchar* q = t;
	      for(int y4 = 0; y4 < y5; ++y4) {
		Guchar* r = q;
		for(int x4 = 0; x4 < x5; ++x4) {
		  switch(n) {
		    case 4: *r++ = color.comp[0];
		    case 3: *r++ = color.comp[1];
		    case 2: *r++ = color.comp[2];
		    case 1: *r++ = color.comp[3];
		  }
		  r += xStep - n;
		}
		q += yStep;
	      }
	    }
	  }
	  t += xStep * x5;
	  x += dx;
	  x6 = x;
	} else
	  x += dx;
      }
      q += yStep * y5;
      y += dy;
      y6 = y;
      ++readLines;
    }

  } else {

    int n = pixfmt->is_truecolor() ? bitmap.bpp : 4;
    IDB(printf("n = %d, pixfmt = %d\n", n, pixfmt->id());)

    // fill the bitmap with 00RRGGBB pixels
    while (y < y3 && !*abortFlagPtr) {
      int x = 0;
      int y5 = (min(y + dy, y3) >> 16) - (y6 >> 16);
      Guchar* t = q;
      int x6 = x2;
      for (int j = 0; j < width; ++j) {
	imgStr->getPixel(pixBuf2);
	if (x + dx > x2 && x < x3 && y5 > 0) {
	  int x5 = (min(x + dx, x3) >> 16) - (x6 >> 16);
	  if (x5 > 0) {
	    GfxRGB c;
	    colorMap->getRGB(pixBuf2, &c);
	    union {
	      Gulong value;
	      Guchar comp[4];
	    } color;
	    color.value = pixfmt->rgb_to_int(colToByte(c.r), colToByte(c.g), colToByte(c.b));
	    Guchar* q = t;
	    for (int y4 = 0; y4 < y5; ++y4) {
	      Guchar* r = q;
	      for (int x4 = 0; x4 < x5; ++x4) {
		switch(n) {
		  case 4: *r++ = color.comp[0];
		  case 3: *r++ = color.comp[1];
		  case 2: *r++ = color.comp[2];
		  case 1: *r++ = color.comp[3];
		}
		r += xStep - n;
	      }
	      q += yStep;
	    }
	  }
	  t += xStep * x5;
	  x += dx;
	  x6 = x;
	} else
	  x += dx;
      }
      q += yStep * y5;
      y += dy;
      y6 = y;
      ++readLines;
    }

  }

  if (inlineImg) {
    // skip remaining lines.
    while (readLines < height && !*abortFlagPtr) {
      imgStr->skipLine();
      ++readLines;
    }
    imgStr->close();
  }

  // for trueColor non rotated visuals, or masks, we are done.
  if (((isMask || bitmap.pixfmt->is_truecolor()) && !oblique) || *abortFlagPtr) {
    *drawStatePtr = ++lastDrawState;
    return;
  }

  w2 >>= 16;
  h2 >>= 16;

  ArrayPtr<ColorEntry> colorTable2;
  int newColors;
  Guchar mask;

  if (!bitmap.pixfmt->is_truecolor() && !isMask) {
    // compute color histogram
    const int max_colors = 4095;

    int colors;
    int bits = 5;
    mask = - (1 << 8 - bits);

    ColHist* ch = NULL;

    try {
      IDB(printf("getColorHist\n");)

      while(!(ch = getColorHist((const Gulong*)(const Guchar*)mem, w2 * h2, max_colors, colors, mask))) {
	mask = (mask << 1) & 0xff;
	--bits;
	if (*abortFlagPtr || bits == 0)
	  return;
      }

      // find the prefered colors
      newColors = maxColors < 2 ? 2 : maxColors;
      if (colors < newColors)
	newColors = colors;
      IDB(printf("ch = %x, newColors = %d, maxColors = %d\n", ch, newColors, maxColors);)

      Gulong mul = 0;
      for (Gulong k = 1 << 32 - bits; k != 0; k >>= bits)
	mul |= k;
      colorTable2 = new ColorEntry [newColors];
      medianCut(colorTable2, ch, colors, w2 * h2, newColors, mul, bits);

      if (*abortFlagPtr)
	return;

      // allocate those colors
      IDB(printf("allocating %d colors\n",newColors);)
      colorTable.allocate(newColors, colorTable2);
    }
    catch(...) {
      if (ch)
	freeColorHist(ch);
      throw;
    }
    freeColorHist(ch);
  } else
    newColors = 0;

  // allocate error diffusion accumulators
  ColorDiff32 errRight;
  ArrayPtr<ColorDiff32> errDown;

  HashTable* table = NULL;

  if (!bitmap.pixfmt->is_truecolor()) {
    table = createHashTable(colorTable2, newColors);
    if (!table)
      throw "No memory.";
  }

  try {

    if (oblique) {
      struct V {
	double xp;
	double yp;
	double xt;
	double yt;
      };
      int x[4], y[4], xp[4], yp[4];
      DrawPicParams params;
      V v[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

      params.bm = &bitmap;
      params.src = mem;
      params.maskSrc = maskMem;
      params.srcWidth = w2;
      params.srcHeight = h2;
      params.dither = dither;
      params.table = table;
      params.mask = mask;
      params.color = bitmap.pixfmt->color_to_int(fillColor);

      for (int k = 0; k < 4; ++k) {
	state->transform(v[k].xp, v[k].yp, &v[k].xt, &v[k].yt);
	IDB(printf("v[%d]=(%f,%f) (%f, %f)\n", k, v[k].xp, v[k].yp, v[k].xt, v[k].yt));
      }

      for (int k = 1; k < 4; ++k) {
	if (v[0].yt > v[k].yt) {
	  V t;
	  t = v[0];
	  v[0]           = v[k & 3];
	  v[k & 3]       = v[(k * 2) & 3];
	  v[(k * 2) & 3] = v[(k * 3) & 3];
	  v[(k * 3) & 3] = t;
	  k = 0;
	}
      }

      if (v[1].yt > v[3].yt) {
	V t = v[1]; v[1] = v[3]; v[3] = t;
      }

      for (int k = 0; k < 4; ++k) {
	x[k] = xoutRound(v[k].xt) - bitmap.xoffset;
	y[k] = xoutRound(v[k].yt) - bitmap.yoffset;
	xp[k] = xoutRound(v[k].xp * (w2 - 1));
	yp[k] = xoutRound(v[k].yp * (h2 - 1));
	IDB(printf("v[%d]=(%f,%f) (%f, %f) (%d,%d) (%d, %d)\n",
		   k, v[k].xp, v[k].yp, v[k].xt, v[k].yt, x[k], y[k], xp[k], yp[k]));
      }

      if (dither) {
	params.errX0 = min(x[1], x[3]);
	w0 = max(x[1], x[3]) - params.errX0 + 1;
	errDown = new ColorDiff32[w0];
	params.errDown = errDown;
	memset(errDown, 0, w0 * sizeof(ColorDiff32));
	IDB(printf("w0 = %d, errX0 = %d\n", w0, params.errX0));
      }

      int xa  = xoutRound(v[0].xt + (v[3].xt - v[0].xt) * (v[1].yt - v[0].yt) / (v[3].yt - v[0].yt)) - bitmap.xoffset;
      int xap = xoutRound((v[0].xp + (v[3].xp - v[0].xp) * (v[1].yt - v[0].yt) / (v[3].yt - v[0].yt)) * (w2 - 1));
      int yap = xoutRound((v[0].yp + (v[3].yp - v[0].yp) * (v[1].yt - v[0].yt) / (v[3].yt - v[0].yt)) * (h2 - 1));
      int xb  = xoutRound(v[1].xt + (v[2].xt - v[1].xt) * (v[3].yt - v[1].yt) / (v[2].yt - v[1].yt)) - bitmap.xoffset;
      int xbp = xoutRound((v[1].xp + (v[2].xp - v[1].xp) * (v[3].yt - v[1].yt) / (v[2].yt - v[1].yt)) * (w2 - 1));
      int ybp = xoutRound((v[1].yp + (v[2].yp - v[1].yp) * (v[3].yt - v[1].yt) / (v[2].yt - v[1].yt)) * (h2 - 1));

      DB(printf("xa = %d, xap = %d, yap = %d\n", xa, xap, yap));
      DB(printf("xb = %d, xbp = %d, ybp = %d\n", xb, xbp, ybp));

      drawPicSlice(params, y[0], y[1],
		   x[0], x[1], x[0], xa,
		   xp[0], yp[0], xp[1], yp[1],
		   xp[0], yp[0], xap, yap);
      *drawStatePtr = ++lastDrawState;
      drawPicSlice(params, y[1], y[3],
		   x[1], xb, xa, x[3],
		   xp[1], yp[1], xbp, ybp,
		   xap, yap, xp[3], yp[3]);
      *drawStatePtr = ++lastDrawState;
      drawPicSlice(params, y[3], y[2],
		   xb, x[2], x[3], x[2],
		   xbp, ybp, xp[2], yp[2],
		   xp[3], yp[3], xp[2], yp[2]);
    } else {

      if (dither) {
	errDown = new ColorDiff32 [w0];
	memset(errDown, 0, w0 * sizeof(ColorDiff32));
      }

      // first line
      IDB(printf("remapping\n");)
      const Gulong *p = (const Gulong *)(const Guchar*)mem;
      modulo = bitmap.modulo;
      Guchar* q = bitmap.data() + y0 * modulo + x0;
      IDB(printf("p = %lx, q = %lx, modulo = %d\n", p, q, modulo);)

      IDB(ColorDiff32 toterr;
	  ColorDiff32 minerr;
	  ColorDiff32 maxerr;
	  toterr.r = toterr.g = toterr.b = 0;
	  minerr.r = minerr.g = minerr.b = 0;
	  maxerr.r = maxerr.g = maxerr.b = 0;)

      // read image
      for (int y = 0; y < h0 && !*abortFlagPtr; ++y) {
	drawLUTLine(q, p, w0, dither, table, mask, errDown);
	q += modulo;
	p += w0;
	if ((y&63) == 63)
	    *drawStatePtr = ++lastDrawState;
      }

      IDB(printf("total error: %ld,%ld,%ld (%f,%f,%f)\n",
		 toterr.r,toterr.g,toterr.b,(double)toterr.r/(w0*h0),
		 (double)toterr.g/(w0*h0),(double)toterr.b/(w0*h0));)
      IDB(printf("min: %ld,%ld,%ld   max: %ld,%ld,%ld\n",
		 minerr.r,minerr.g,minerr.b,maxerr.r,maxerr.g,maxerr.b);)
    }

    *drawStatePtr = ++lastDrawState;
  }
  catch(...) {
    if (table)
      deleteHashTable(table);
    throw;
  }
  if (table)
    deleteHashTable(table);
#endif
}
#else
void AOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
			   int width, int height, GfxImageColorMap *colorMap,
			   int *maskColors, GBool inlineImg, GBool invert) {
  int nComps = colorMap->getNumPixelComps();
  int nVals = width * nComps;
  int nBits = colorMap->getBits();
  int j = height * ((nVals * nBits + 7) / 8);
  str->reset();
  for (int i = 0; i < j; ++i)
    str->getChar();
  str->close();
}
#endif

Color AOutputDev::makeColor(GfxRGB *rgb) {
  int r, g, b;

  r = colToByte(rgb->r);
  g = colToByte(rgb->g);
  b = colToByte(rgb->b);

  return (r << 16) | (g << 8) | b;
}

GBool AOutputDev::findText(Unicode *s, int len,
			   GBool startAtTop, GBool stopAtBottom,
			   GBool startAtLast, GBool stopAtLast,
			   int *xMin, int *yMin, int *xMax, int *yMax) {
  double xMin1, yMin1, xMax1, yMax1;

  xMin1 = (double)*xMin;
  yMin1 = (double)*yMin;
  xMax1 = (double)*xMax;
  yMax1 = (double)*yMax;
  if (text->findText(s, len, startAtTop, stopAtBottom, startAtLast, stopAtLast, gFalse, gFalse,
			  &xMin1, &yMin1, &xMax1, &yMax1)) {
    *xMin = xoutRound(xMin1);
    *xMax = xoutRound(xMax1);
    *yMin = xoutRound(yMin1);
    *yMax = xoutRound(yMax1);
    return gTrue;
  }
  return gFalse;
}

GString *AOutputDev::getText(int xMin, int yMin, int xMax, int yMax) {
  return text->getText((double)xMin, (double)yMin,
		       (double)xMax, (double)yMax);
}

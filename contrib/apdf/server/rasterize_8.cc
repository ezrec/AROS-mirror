//========================================================================
//
// rasterize_8.cc
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#define Object ZZObject
#include <exec/memory.h>
#ifdef __PPC__
#include <powerup/clib/ppc_protos.h>
#define CreatePool PPCCreatePool
#define DeletePool PPCDeletePool
#define AllocPooled PPCAllocPooled
#define FreePooled PPCFreePooled
#define AllocVec PPCAllocVec
#define FreeVec PPCFreeVec
#else
#include <proto/exec.h>
#endif
#undef Allocate
#undef Object
#include "rasterize.h"

#undef new
inline void* operator new (size_t, void *p) { return p; }

extern "C" inline void __builtin_delete(void*) {}


void fill(ABitmap& bm, int x0, int y0, region* area, Gulong color) {
  _fill(bm, x0, y0, area, Guchar(color));
}

void AAfill(ABitmap& bm, int x0, int y0, region* area,
	    Gulong c1, Gulong c2, Gulong c3, Gulong c4) {
  _AAfill(bm, x0, y0, area, Guchar(c1), Guchar(c2), Guchar(c3), Guchar(c4));
}

void rectfill(ABitmap& bm, int x0, int y0, int w, int h, Gulong color) {
  _rectfill(bm, x0, y0, w, h, Guchar(color));
}

void clippedHLine(ABitmap& bm, int y, int x1, int x2, Gulong color, DashState& ds) {
  _clippedHLine(bm, y, x1, x2, Guchar(color), ds);
}

void clippedVLine(ABitmap& bm, int x, int y1, int y2, Gulong color, DashState& ds) {
  _clippedVLine(bm, x, y1, y2, Guchar(color), ds);
}

void clippedLine(ABitmap& bm, fractpel x1, fractpel y1, fractpel x2, fractpel y2, Gulong color, DashState& ds) {
  _clippedLine(bm, x1, y1, x2, y2, Guchar(color), ds);
}

void clippedCopy(ABitmap& bm1, ABitmap& bm2, region* area) {
  _clippedCopy<1>(bm1, bm2, area);
}


// Color quantization code, to display true color embedded pictures
// on colormap screens.
//
// Uses Heckbert's algorithm, from Heckbert's seminal paper
//   Heckbert, Paul.  "Color Image Quantization for Frame Buffer Display",
//   Proc. SIGGRAPH '82, Computer Graphics v.16 #3 (July 1982), pp 297-304.
//
// Adapted from the code of the Independant JPEG Group.

typedef long LONG;

struct ColHist {
  Gulong color;
  int value;
};

// Hashtable for color histogram computation.
// Color format is 00RRGGBB.
class CHHashTable {
  enum { hashTableSize = 6553 };
  struct Node {
    Node *next;
    ColHist ch;
  };
  static bool equal(Gulong c1, Gulong c2) {
    return c1 == c2;
  }
  static int hash(Gulong c) {
    /*return (((c >> 24) & m) * 33023 +
	    ((c >> 16) & m) * 30013 +
	    ((c >> 8) & m) * 27011) % hashTableSize;*/
    return c % hashTableSize;
  }
public:
  class Iterator {
    friend class CHHashTable;
  public:
    bool operator == (const Iterator& i) const { return q == i.q; }
    bool operator != (const Iterator& i) const { return q != i.q; }
    ColHist& operator * () const { return q->ch; }
    Iterator& operator ++ () {
      q = q->next;
      if (!q) {
	while (++p != end && !*p);
	if (p != end)
	  q = *p;
      }
      return *this;
    }
  private:
    Node** p;
    Node* q;
    Node** end;
  };

  CHHashTable() : sz(0), nodes(NULL), num(0) {
    pool = CreatePool(MEMF_ANY, 32000, 16000);
    if (pool) {
      nodes = (Node**)AllocPooled(pool, hashTableSize * sizeof(Node*));
      if (nodes)
	memset(nodes, 0, hashTableSize * sizeof(Node*));
    }
  }

  ~CHHashTable() {
    DeletePool(pool);
  }

  bool ok() const { return nodes != NULL; }

  int size() const { return sz; }

  Iterator begin() {
    Iterator i;
    i.p = nodes;
    i.end = nodes + hashTableSize;
    while (!*i.p && ++i.p != i.end);
    if (i.p != i.end)
      i.q = *i.p;
    else
      i.q = NULL;
    return i;
  }

  Iterator end() {
      Iterator i;
      i.q = NULL;
      return i;
  }

  ColHist& get(Gulong c, Gulong mask) {
    c &= mask;
    int h = hash(c);
    for (Node *hn = nodes[h]; hn; hn = hn->next)
      if (equal(c, hn->ch.color))
	 return hn->ch;
    ++sz;

    if (num == 0) {
      num = 800;
      cur = (Node*)AllocPooled(pool, num * sizeof(Node));
      if (!cur) {
	num = 0;
	static ColHist ch;
	return ch;
      }
    }

    Node *hn = cur++;
    --num;

    hn->ch.color = c;
    hn->ch.value = 0;
    hn->next = nodes[h];
    nodes[h] = hn;
    return hn->ch;
  }

private:
  Node **nodes;
  int sz;
  void *pool;
  Node *cur;
  int num;
};

// Color histogram computation.
// Color input format is 00RRGGBB.
ColHist *getColorHist(const Gulong *p, int size,
		      int maxcols, int& hist_size,
		      Guchar mask) {

  IDB(printf("getColorHist(%x)\n",mask);)

  CHHashTable ht;
  ColHist* ch = NULL;

  if (ht.ok()) {
    Gulong longMask = Gulong(mask) * 0x00010101UL;
    while (--size >= 0) {
      ColHist& hn = ht.get(*p++, longMask);
      ++hn.value;
      if (ht.size() > maxcols)
	return NULL;
    }

    IDB(printf("-> %u colors\n",ht.size());)
    hist_size = ht.size();

    ch = (ColHist*)AllocVec(ht.size() * sizeof(*ch), MEMF_ANY);
    if (ch) {
      ColHist *q = ch;

      CHHashTable::Iterator e(ht.end());
      for(CHHashTable::Iterator i(ht.begin()); i != e; ++i)
	*q++ = *i;
    }
  }

  return ch;
}

void freeColorHist(ColHist* p) {
  FreeVec(p);
}

static int redcmp(const void *p1, const void *p2) {
  const ColHist *ch1 = (const ColHist *)p1;
  const ColHist *ch2 = (const ColHist *)p2;
  return (ch1->color & 0x00ff0000UL) - (ch2->color & 0x00ff0000UL);
}
static int greencmp(const void *p1, const void *p2) {
  const ColHist *ch1 = (const ColHist *)p1;
  const ColHist *ch2 = (const ColHist *)p2;
  return (ch1->color & 0x0000ff00UL) - (ch2->color & 0x0000ff00UL);
}
static int bluecmp(const void *p1, const void *p2) {
  const ColHist *ch1 = (const ColHist *)p1;
  const ColHist *ch2 = (const ColHist *)p2;
  return (ch1->color & 0x000000ffUL) - (ch2->color & 0x000000ffUL);
}

struct box {
  int index;
  int colors;
  long sum;
};

// Fill the colormap cm with newColors colors, choosen to
// match the color histogram as well as possible.
// Color histogram format: 00RRGGBB
bool medianCut(ColorEntry *cm, ColHist *ch, int colors,
	       long sum, int newColors, Gulong mul, int bits) {

  box* boxes = (box*)AllocVec(newColors * sizeof(box), MEMF_ANY);
  if (!boxes)
    return false;

/*
 * Start with 1 big box
 */
   int nBoxes = 1;
   boxes[0].index = 0;
   boxes[0].colors = colors;
   boxes[0].sum = sum;

/*
 * Split boxes
 */

  while (nBoxes < newColors /*&& !*abortFlagPtr*/) {

    IDB(printf("\n%d boxes:\n",nBoxes);
	for(int k=0;k<nBoxes;++k)
	    printf("\t%d\t%d\t%d\n",boxes[k].index,boxes[k].colors,boxes[k].sum);)

    int bi;
    Guchar minr, maxr, ming, maxg, minb, maxb;

    /*
     * Find the first splitable box. Boxes are sorted by sums.
     */
    for (bi = 0; bi < nBoxes && boxes[bi].colors < 2; ++bi);
    if (bi == nBoxes)
      break;

    IDB(printf("splitting box %d\n",bi);)

    int indx = boxes[bi].index;
    int clrs = boxes[bi].colors;
    long sm = boxes[bi].sum;

    /*
     * Find the boundaries of the box
     */

    minr = maxr = Guchar(ch[indx].color >> 16);
    ming = maxg = Guchar(ch[indx].color >> 8);
    minb = maxb = Guchar(ch[indx].color);

    for(int i = 1; i < clrs; ++i) {
      Gulong c = ch[indx + i].color;
      Guchar v = Guchar(c >> 16);
      if (v < minr) minr = v; else if (v > maxr) maxr = v;
      v = Guchar(c >> 8);
      if (v < ming) ming = v; else if (v > maxg) maxg = v;
      v = Guchar(c);
      if (v < minb) minb = v; else if (v > maxb) maxb = v;
    }
    IDB(printf("%02x..%02x,  %02x..%02x,  %02x..%02x\n",minr,maxr,ming,maxg,minb,maxb);)

    /*
     * Find the largest dimension, and sort the colors in the box by
     * this component.
     */
    int rl = 77 * (maxr - minr);
    int gl = 150 * (maxg - ming);
    int bl = 29 * (maxb - minb);

    if (rl >= gl && rl >= bl)
      qsort(&ch[indx], clrs, sizeof(ColHist), redcmp);
    else if (gl >= bl)
      qsort(&ch[indx], clrs, sizeof(ColHist), greencmp);
    else
      qsort(&ch[indx], clrs, sizeof(ColHist), bluecmp);

    /*
     * Split the box.
     */

    long lsum = ch[indx].value;
    long hsum = sm / 2;
    int i;
    for (i = 1; i < clrs - 1; i++) {
      if (lsum >= hsum)
	break;
      lsum += ch[indx + i].value;
    }
    if (i > 1 &&
	lsum - hsum > hsum - lsum + ch[indx + i - 1].value) {
      IDB(printf("[lsum=%d,  hsum=%d,  sm=%d,  i=%d]\n",lsum,hsum,sm-lsum,i);)
      --i;
      lsum -= ch[indx + i].value;
    }
    sm -= lsum;
    IDB(printf("lsum=%d,  hsum=%d,  sm=%d,  i=%d\n",lsum,hsum,sm,i);)

    /*
     * Add the two boxes in the right order
     */
    int i1, i2, clrs1, clrs2;
    long sum1, sum2;
    if (lsum > sm) {
      i1 = indx;
      clrs1 = i;
      sum1 = lsum;
      i2 = indx + i;
      clrs2 = clrs - i;
      sum2 = sm;
    } else {
      i1 = indx + i;
      clrs1 = clrs - i;
      sum1 = sm;
      i2 = indx;
      clrs2 = i;
      sum2 = lsum;
    }
    boxes[nBoxes].sum = 0;
    int a;
    for(a = bi + 1; boxes[a].sum > sum1; ++a);
    --a;
    if (a != bi)
      memmove(boxes + bi, boxes + bi + 1, (a - bi) * sizeof(box));
    boxes[a].index = i1;
    boxes[a].colors = clrs1;
    boxes[a].sum = sum1;
    for (++a; boxes[a].sum > sum2; ++a);
    if (a != nBoxes)
      memmove(boxes + a + 1, boxes + a, (nBoxes - a) * sizeof(box));
    boxes[a].index = i2;
    boxes[a].colors = clrs2;
    boxes[a].sum = sum2;
    ++nBoxes;
  }

/*
 * Find a representative color for each box.
 */

  IDB(printf("%d boxes:\n",nBoxes);)

  for (int i = 0; i < nBoxes; ++i) {
    Gulong s = 0, r = 0, g = 0, b = 0;
    ColHist* p = &ch[boxes[i].index];

    for (int j = 0; j < boxes[i].colors; ++j, ++p) {
      Gulong c = p->color;
      r += Guchar(c >> 16) * p->value;
      g += Guchar(c >> 8) * p->value;
      b += Guchar(c) * p->value;
      s += p->value;
    }
    r /= s;
    g /= s;
    b /= s;
    cm[i].color.r = (r >> 8 - bits) * mul;
    cm[i].color.g = (g >> 8 - bits) * mul;
    cm[i].color.b = (b >> 8 - bits) * mul;
    IDB(printf("\t%08lx %08lx %08lx\n",cm[i].color.r,cm[i].color.g,cm[i].color.b);)
  }

  FreeVec(boxes);

  return true;
}


// Hash table used to remap the picture.
// Input = ColorDiff32, output = color index.
class HashTable {
  enum { hashTableSize = 6553 };
  struct Node {
    Node* next;
    ColorDiff32 color;
    Color32 mappedColor;
    int index;
  };
  static int hash(const ColorDiff32& c, Guchar mask) {
    return Gulong(((c.r >> 8) & mask) * 33023 +
		  ((c.g >> 8) & mask) * 30013 +
		  ((c.b >> 8) & mask) * 27011) % hashTableSize;
  }
  static Gulong sq(LONG x) { return Gulong(x * x) >> 4; }

public:

  HashTable(void *p, ColorEntry *t, int n)
    : table(t), colors(n), pool(p), num(0) {
    memset(nodes, 0, sizeof(Node*) * hashTableSize);
  }

  ~HashTable() {}

  void destroy() { DeletePool(pool); }

  bool ok() const { return nodes != NULL; }

  int get(const ColorDiff32& c, ColorDiff32& err, Guchar mask);

private:
  Node *nodes[hashTableSize];
  int colors;
  ColorEntry* table;
  void *pool;
  Node *cur;
  int num;
};


int HashTable::get(const ColorDiff32& c, ColorDiff32& err, Guchar mask) {
  int h = hash(c, mask);
  for (Node *hn = nodes[h]; hn; hn = hn->next)
    if (((((hn->color.r ^ c.r) |
	   (hn->color.g ^ c.g) |
	   (hn->color.b ^ c.b)) >> 8) & mask) == 0) {
      err.r = c.r - hn->mappedColor.r;
      err.g = c.g - hn->mappedColor.g;
      err.b = c.b - hn->mappedColor.b;
      return hn->index;
    }

  if (num == 0) {
    num = 800;
    cur = (Node*)AllocPooled(pool, num * sizeof(Node));
    if (!cur) {
      num = 0;
      return 0;
    }
  }

  Node *hn = cur++;
  --num;

  ULONG d = 0xffffffffUL;
  ColorEntry *q = table;
  ColorEntry *e = q;
  for (int k = 0; k < colors; ++k, ++q) {
    ULONG d2 = sq(c.r - (q->color.r >> 16)) +
	       sq(c.g - (q->color.g >> 16)) +
	       sq(c.b - (q->color.b >> 16));
    if (d2 < d) {
      d = d2;
      e = q;
    }
  }
  hn->mappedColor.r = e->color.r >> 16;
  err.r = c.r - hn->mappedColor.r;
  hn->mappedColor.g = e->color.g >> 16;
  err.g = c.g - hn->mappedColor.g;
  hn->mappedColor.b = e->color.b >> 16;
  err.b = c.b - hn->mappedColor.b;
  hn->color = c;
  hn->index = e->index;
  hn->next = nodes[h];
  nodes[h] = hn;
  return e->index;
}



void drawPicLine(AOutputDev::DrawPicParams& params,
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
  Guchar* q = params.bm->data() + y * params.bm->modulo + x1;

  if (params.maskSrc) {
    const Guchar *p = params.maskSrc;
    Guchar c(params.color);

    for (int x = x1; x <= x2; ++x) {
      if (p[(yp >> 16) * w + (xp >> 16)])
	*q = c;
      ++q;
      xp += dxp;
      yp += dyp;
    }

  } else {
    const Gulong *p = (const Gulong *)params.src;
    int dither = params.dither;

    ColorDiff32 errRight, *errDown;
    HashTable& table = *params.table;
    Guchar mask = params.mask;

    errRight.r = errRight.g = errRight.b = 0;
    errDown = params.errDown + x1 - params.errX0;

    x2 -= x1;
    for (int x = 0; x <= x2; ++x) {
      ColorDiff32 color1;
      Gulong c = p[(yp >> 16) * w + (xp >> 16)];
      color1.r = LONG((c >> 8) & 0xff00);
      color1.g = LONG(c & 0xff00);
      color1.b = LONG((c << 8) & 0xff00);

      if (dither) {
	color1.r += errRight.r + errDown[x].r;
	if (color1.r > 0xffff)
	  color1.r = 0xffff;
	else if (color1.r < 0)
	  color1.r = 0;
	color1.g += errRight.g + errDown[x].g;
	if (color1.g > 0xffff)
	  color1.g = 0xffff;
	else if (color1.g < 0)
	  color1.g = 0;
	color1.b += errRight.b + errDown[x].b;
	if (color1.b > 0xffff)
	  color1.b = 0xffff;
	else if (color1.b < 0)
	  color1.b = 0;
      }

      ColorDiff32 err;
      *q++ = Guchar(table.get(color1, err, mask));
      if (dither) {
	errRight.r = errDown[x].r = err.r / 2;
	errRight.g = errDown[x].g = err.g / 2;
	errRight.b = errDown[x].b = err.b / 2;
      }

      xp += dxp;
      yp += dyp;
    }
  }
}

HashTable* createHashTable(ColorEntry* t, int n) {
  if (void *pool = CreatePool(MEMF_ANY, 32000, 16000)) {
    HashTable* r = (HashTable *)AllocPooled(pool, sizeof(HashTable));
    if (r) {
      new (r) HashTable(pool, t, n);
      if (r->ok())
	return r;
    }
    DeletePool(pool);
  }
  return NULL;
}

void deleteHashTable(HashTable* t) {
  t->destroy();
}


void drawLUTLine(Guchar* q, const Gulong *p, int w0, int dither,
		 HashTable *table, Gulong mask, ColorDiff32 *errDown) {

  // clear error accumulator
  ColorDiff32 errRight;
  errRight.r = errRight.g = errRight.b = 0;

  // for each column (line)...
  for (int x = 0; x < w0; ++x) {

    // draw image pixel
    ColorDiff32 color1;
    Gulong c = *p++;
    color1.r = LONG((c >> 8) & 0xff00);
    color1.g = LONG(c & 0xff00);
    color1.b = LONG((c << 8) & 0xff00);

    if (dither) {
      color1.r += errRight.r + errDown[x].r;
      if (color1.r > 0xffff)
	color1.r = 0xffff;
      else if (color1.r < 0)
	color1.r = 0;
      color1.g += errRight.g + errDown[x].g;
      if (color1.g > 0xffff)
	color1.g = 0xffff;
      else if (color1.g < 0)
	color1.g = 0;
      color1.b += errRight.b + errDown[x].b;
      if (color1.b > 0xffff)
	color1.b = 0xffff;
      else if (color1.b < 0)
	color1.b = 0;
    }
    ColorDiff32 err;
    *q++ = Guchar(table->get(color1, err, mask));
    if (dither) {
      errRight.r = errDown[x].r = err.r / 2;
      errRight.g = errDown[x].g = err.g / 2;
      errRight.b = errDown[x].b = err.b / 2;
    }
    IDB(toterr.r += err.r;
	toterr.g += err.g;
	toterr.b += err.b;
	if(err.r > maxerr.r)
	    maxerr.r = err.r;
	if(err.r < minerr.r)
	    minerr.r = err.r;
	if(err.g > maxerr.g)
	    maxerr.g = err.g;
	if(err.g < minerr.g)
	    minerr.g = err.g;
	if(err.b > maxerr.b)
	    maxerr.b = err.b;
	if(err.b < minerr.b)
	    minerr.b = err.b;)
  }

}

void (*funcTable[])() = {
  (void (*)())fill, (void (*)())AAfill, (void (*)())rectfill,
  (void (*)())clippedHLine, (void (*)())clippedVLine, (void (*)())clippedLine,
  (void (*)())clippedCopy, (void (*)())drawPicLine,
  (void (*)())getColorHist, (void (*)())freeColorHist, (void (*)())medianCut,
  (void (*)())createHashTable, (void (*)())deleteHashTable, (void (*)())drawLUTLine

};

#ifdef __PPC__
static const char verstring[]="$VER:ApdfGfx8_" PROC ".module 1.0 (27.12.2000)";
#else
extern const char LibName[];
extern const char LibId[];
const char LibName[]="ApdfGfx8_" PROC ".module";
const char LibId[]="ApdfGfx8_" PROC ".module 1.0 (27.12.2000)";
#endif


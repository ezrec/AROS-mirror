//========================================================================
//
// AOutputDev.h
//
// Copyright 1999-2002 Emmanuel Lesueur
//
// Adpated from XOutputDev.h
//
//========================================================================

#ifndef AOUTPUTDEV_H
#define AOUTPUTDEV_H

#ifdef __GNUC__
#pragma interface
#endif

#include <stddef.h>
#include <math.h>
#if defined(__SASC) && defined(_M68000)
#   include <m68881.h>
#endif

#include "config.h"
#include "OutputFontCache.h"
#include "OutputDev.h"
#include "gfx.h"
#include "FreeType.h"

class GString;
class GfxRGB;
class GfxFont;
class GfxSubpath;
class TextPage;
struct RGBColor;
class FontEncoding;
class AOutputFontCache;
class AOutputFont;
class AT1OutputFont;
class HashTable;
struct ColorDiff32;
struct ColHist;
class T3FontCache;
struct T3GlyphStack;


//------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------

static const int maxRGBCube=8;      // max size of RGB color cube

static const int numTmpPoints=256;  // number of XPoints in temporary array
static const int numTmpSubpaths=16; // number of elements in temporary arrays
				    //   for fill/clip

//------------------------------------------------------------------------
// Misc types
//------------------------------------------------------------------------

/*enum CapStyle {
  CapButt, CapRound, CapProjecting
};

enum JoinStyle {
  JoinMiter, JoinRound, JoinBevel
};*/

struct BoundingRect {
  short xMin, xMax;             // min/max x values
  short yMin, yMax;             // min/max y values
};

struct RGBColor {
  double r, g, b;
};

//struct Color32 {
//  Gulong r, g, b;
//};
//
//struct ColorEntry {
//  int index;
//  Color32 color;
//};

struct ColorDiff32 {
    long r, g, b;
};


//------------------------------------------------------------------------
// ColorAllocator
//------------------------------------------------------------------------

//class ColorAllocator {
//public:
//  ColorAllocator() {}
//  virtual ~ColorAllocator() {}
//  virtual Guchar allocate(Gulong) = 0;
//  virtual void allocate(int, ColorEntry *) = 0;
//private:
//  ColorAllocator(const ColorAllocator&);
//  ColorAllocator& operator = (const ColorAllocator&);
//};

//------------------------------------------------------------------------
// ColorTable
//------------------------------------------------------------------------

//class ColorTable {
//public:
//  ColorTable(ColorAllocator *colorAllocator1)
//    : colorAllocator(colorAllocator1), num(0),
//      entries(NULL), maxEntries(0) {}
//  ~ColorTable() {
//    gfree(entries);
//    delete colorAllocator;
//  }
//  Gulong find(Gulong);
//  Gulong getPenValue(Guchar c) { return pens[c]; }
//  void allocate(int n, ColorEntry *p);
//  void clear() { num = 0; }
//private:
//  ColorTable(const ColorTable&);
//  ColorTable& operator = (const ColorTable&);
//
//  Gulong num;
//  struct entry {
//    Gulong color; // RRGGBBnn
//    Gulong val;
//  };
//  entry* entries;
//  Gulong maxEntries;
//  ColorAllocator* colorAllocator;
//  Gulong pens[256];
//};

//------------------------------------------------------------------------
// TransformedGfxState
//------------------------------------------------------------------------

#if 0
class TransformedGfxState {
public:
  TransformedGfxState()
  : capStyle(CapButt), joinStyle(JoinMiter), lineWidth(0),
    miterLimit(10), dashLength(0), dashStart(0), dashPattern(NULL) {}
  ~TransformedGfxState() { delete [] dashPattern; }

  TransformedGfxState(const TransformedGfxState& s) {
    assign(s);
  }

  void reset() {
    capStyle = CapButt;
    joinStyle = JoinMiter;
    miterLimit = 10;
    lineWidth = 0;
    dashLength = 0;
    dashStart = 0;
    delete [] dashPattern;
    dashPattern = NULL;
  }

  TransformedGfxState& operator = (const TransformedGfxState& s) {
    reset();
    assign(s);
    return *this;
  }

  CapStyle getCapStyle() const { return capStyle; }
  JoinStyle getJoinStyle() const { return joinStyle; }
  double getMiterLimit() const { return miterLimit; }
  double getLineWidth() const { return lineWidth; }
  int getDashLength() const { return dashLength; }
  const int *getDashPattern() const { return dashPattern; }
  int getDashStart() const { return dashStart; }

  void setCapStyle(CapStyle capStyle1) { capStyle = capStyle1; }
  void setJoinStyle(JoinStyle joinStyle1) { joinStyle = joinStyle1; }
  void setMiterLimit(double miterLimit1) { miterLimit = miterLimit1; }
  void setLineWidth(double lineWidth1) { lineWidth = lineWidth1; }
  void setDashPattern(int dashLength1, int *dashPattern1, int dashStart1) {
    dashLength = dashLength1;
    delete [] dashPattern;
    dashPattern = dashPattern1;
    dashStart = dashStart1;
  }
private:
  void assign(const TransformedGfxState&);

  CapStyle capStyle;
  JoinStyle joinStyle;
  double miterLimit;
  double lineWidth;
  int dashLength;
  int dashStart;
  int *dashPattern;
};
#endif

//------------------------------------------------------------------------
// AOutputFont
//------------------------------------------------------------------------

class AOutputFont : public OutputFont {
public:

  // Constructor.
  AOutputFont(BaseFont *base,
	      double m11, double m12, double m21, double m22);

  // Get the character outline.
  virtual APath::Part *getCharOutline(CharCode c, Unicode u) = 0;

  // Get the character interior.
  virtual ARegion *getCharInterior(CharCode c, Unicode u) = 0;

  // Get character metrics
  /*virtual GBool getMetrics(int c,
			   double *xMin, double *yMin,
			   double *xMax, double *yMax,
			   double *advanceX, double *avanceY);*/

  bool outside(ABitmap& bitmap, double x1, double y1, CharCode c, Unicode u) const {
    int x = int(x1) - bitmap.xoffset;
    int y = int(y1) - bitmap.yoffset;
    return x - radius >= bitmap.width ||
	   x + radius < 0 ||
	   y - radius >= bitmap.height ||
	   y + radius < 0;
  }

protected:
  int radius;
};

//------------------------------------------------------------------------
// FTOutputFont
//------------------------------------------------------------------------

class FTOutputFont: public AOutputFont {
public:

  FTOutputFont(FTBaseFont *base,
	       double m11, double m12, double m21, double m22);

  virtual ~FTOutputFont();

  // Was font created successfully?
  virtual GBool isOk();

  // Get the character outline.
  virtual APath::Part *getCharOutline(CharCode c, Unicode u);

  // Get the character interior.
  virtual ARegion *getCharInterior(CharCode c, Unicode u);

private:
  int getCharIndex(CharCode c, Unicode u);

  struct Glyph {
    ARegion* region;
    APath* outline;
    Glyph() : region(NULL), outline(NULL) {}
    ~Glyph() {
      delete region;
      delete outline;
    }
  };

  Glyph *glyphs;
  FT_Size sizeObj;
  FT_Matrix matrix;
  GBool ok;
};

//------------------------------------------------------------------------
// AOutputDev
//------------------------------------------------------------------------

class AOutputDev: public OutputDev {
public:

  // Constructor.
  //AOutputDev(ColorAllocator*);
  AOutputDev(unsigned char *, int);

  // Destructor.
  virtual ~AOutputDev();

  //---- get info about output device

  // Does this device use upside-down coordinates?
  // (Upside-down means (0,0) is the top left corner of the page.)
  virtual GBool upsideDown() { return gTrue; }

  // Does this device use drawChar() or drawString()?
  virtual GBool useDrawChar() { return gTrue; }

  // Does this device use beginType3Char/endType3Char?  Otherwise,
  // text in Type 3 fonts will be drawn with drawChar/drawString.
  virtual GBool interpretType3Chars() { return gTrue; }

  // Does this device needs the fonts to be loaded ?
  virtual GBool loadFonts() { return gTrue; }

  //----- initialization and control

  // Start a page.
  virtual void startPage(int pageNum, GfxState *state);

  // End a page.
  virtual void endPage();

  //----- link borders
  //virtual void drawLinkBorder(double x1, double y1, double x2, double y2,
	//		      double w);

  //----- save/restore graphics state
  virtual void saveState(GfxState *state);
  virtual void restoreState(GfxState *state);

  //----- update graphics state
  virtual void updateAll(GfxState *state);
  virtual void updateCTM(GfxState *state, double m11, double m12,
			 double m21, double m22, double m31, double m32);
  virtual void updateLineDash(GfxState *state);
  virtual void updateFlatness(GfxState *state);
  virtual void updateLineJoin(GfxState *state);
  virtual void updateLineCap(GfxState *state);
  virtual void updateMiterLimit(GfxState *state);
  virtual void updateLineWidth(GfxState *state);
  virtual void updateFillColor(GfxState *state);
  virtual void updateStrokeColor(GfxState *state);

  //----- update text state
  virtual void updateFont(GfxState *state);

  //----- path painting
  virtual void pathOp(GfxState *state, int op);

  //----- text drawing
  virtual void endTextObject(GfxState *state);
  virtual void beginString(GfxState *state, GString *s);
  virtual void endString(GfxState *state);
  virtual void drawChar(GfxState *state, double x, double y,
			double dx, double dy,
			double originX, double originY,
			CharCode code, int nBytes, Unicode *u, int uLen);
  virtual GBool beginType3Char(GfxState *state, double x, double y,
			       double dx, double dy,
			       CharCode code, Unicode *u, int uLen);
  virtual void endType3Char(GfxState *state);

  //----- image drawing
  virtual void drawImage(GfxState *state, Object *ref, Stream *str,
			 int width, int height, GfxImageColorMap *colorMap,
			 int *maskColors, GBool inlineImg, GBool invert);

  //----- Type 3 font operators
  virtual void type3D0(GfxState *state, double wx, double wy);
  virtual void type3D1(GfxState *state, double wx, double wy,
		       double llx, double lly, double urx, double ury);

  //----- special access

  // Start a document.
  void startDoc(XRef *xrefA);

  // Setup the bitmap.
  void init(unsigned char* p, int x, int y, int w, int h, int m, int b, int f);

  // Uninstall the bitmap.
  void cleanup() {
    //bitmap.reset();
    //fullBitmap.reset();
  }

  // Set the AA levels.
  void setAALevels(int textAALevel1, int strokeAALevel1, int fillAALevel1) {
    /*textAALevel = textAALevel1;
    strokeAALevel = strokeAALevel1;
    fillAALevel = fillAALevel1;*/
  }

  // Set the draw level.
  void setDrawLevel(int l) { newDrawLevel = drawLevel = l; }

  // Get the draw level.
  int getDrawLevel() { return newDrawLevel; }

  // Find a string.  If <top> is true, starts looking at <xMin>,<yMin>;
  // otherwise starts looking at top of page.  If <bottom> is true,
  // stops looking at <xMax>,<yMax>; otherwise stops looking at bottom
  // of page.  If found, sets the text bounding rectange and returns
  // true; otherwise returns false.
  GBool findText(Unicode *s, int len,
		 GBool startAtTop, GBool stopAtBottom,
		 GBool startAtLast, GBool stopAtLast,
		 int *xMin, int *yMin, int *xMax, int *yMax);

  // Get the text which is inside the specified rectangle.
  GString *getText(int xMin, int yMin, int xMax, int yMax);

private:

  /*struct SaveState {
    SaveState *next;
    ABitmap bitmap;
    region *clipRegion;
    segment *clipOutline;
    int outlineAALevel;
    TransformedGfxState outlineState;
  };*/

public:
  struct DrawPicParams {
    ABitmap *bm;
    const Guchar *src;
    const Guchar *maskSrc;
    int srcWidth;
    int srcHeight;
    int dither;
    ColorDiff32 *errDown;
    int errX0;
    Guchar mask;
    HashTable* table;
    Gulong color;
  };

private:
  ABitmap bitmap;
  AGfxState pageState;
  AGfxState *gfxState;
  //int textAALevel;
  //int strokeAALevel;
  //int fillAALevel;
  Gulong strokeColor;
  Gulong fillColor;
  Gulong linkColor;
  Gulong paperColor;
  double miterLimit;
  double lineWidth;
  CapStyle capStyle;
  JoinStyle joinStyle;
  DashState dashState;
  //Gulong strokeAAColors[5];
  //Gulong fillAAColors[5];
  //ColorTable colorTable;
  int8u colorMap[256];
  //TransformedGfxState transformedState;
  //Gulong colorMask;
  //GBool trueColor;              // set if using a TrueColor visual
  //int rMul, gMul, bMul;         // RGB multipliers (for TrueColor)
  //int rShift, gShift, bShift;   // RGB shifts (for TrueColor)
  GfxFont *gfxFont;             // current PDF font
  AOutputFont *font;            // current output font
  TextPage *text;               // text from the current page
  //SaveState *states;
  //region *clipRegion;
  //segment *clipOutline;
  //int outlineAALevel;
  //TransformedGfxState outlineState;
  APath::Part *textClipPath;
  double textClipLastX;
  double textClipLastY;
  //void *tempBitmap;
  //ABitmap prevBitmap;
  int drawLevel;
  int newDrawLevel;
  XRef *xref;
  T3FontCache *			// Type 3 font cache
    t3FontCache[xOutT3FontCacheSize];
  int nT3Fonts;			// number of valid entries in t3FontCache
  T3GlyphStack *t3GlyphStack;	// Type 3 glyph context stack

  void (**funcTable)();
  GBool endPageCalled;
  //void *libBase;
  //int libBPP;

  void updateLineAttrs(GfxState *state, GBool updateDash);
  void doStroke(APath *path);
  void doFill(APath *path, bool eo);
  void doClip(APath *path, bool eo);
  APath *convertPath(GfxState *state, GBool);
  APath::Part *convertSubpath(GfxState *state, GfxSubpath *subpath, double&, double&, GBool);
  Color makeColor(GfxRGB *rgb);
  //void getAAColors(Gulong *colors);
  //void popClipState();
  //void flushClip();
  //void installClip();
  //void stroke(int x,int y, segment *path, Gulong color, Gulong* colors);
  //Gulong getPixelColor(int x,int y);
  void drawPicSlice(DrawPicParams& params,int y1, int y2,
		    int xa1, int xa2, int xb1, int xb2,
		    int xpa1, int ypa1, int xpa2, int ypa2,
		    int xpb1, int ypb1, int xpb2, int ypb2);

  void clearT3GlyphStack();

  /*void fill(ABitmap& bm, int x0, int y0, region* area, Gulong color) {
    (*(void(*)(ABitmap&, int, int, region*, Gulong))funcTable[0])(bm, x0, y0, area, color);
  }

  void AAfill(ABitmap& bm, int x0, int y0, region* area,
	      Gulong c1, Gulong c2, Gulong c3, Gulong c4) {
    (*(void(*)(ABitmap&, int, int, region*, Gulong, Gulong, Gulong, Gulong))funcTable[1])(bm, x0, y0, area, c1, c2, c3, c4);
  }

  void rectfill(ABitmap& bm, int x0, int y0, int w, int h, Gulong color) {
    (*(void(*)(ABitmap&, int, int, int, int, Gulong))funcTable[2])(bm, x0, y0, w, h, color);
  }

  void clippedHLine(ABitmap& bm, int y, int x1, int x2, Gulong color, DashState& ds) {
    (*(void(*)(ABitmap&, int, int, int, Gulong, DashState&))funcTable[3])(bm, y, x1, x2, color, ds);
  }

  void clippedVLine(ABitmap& bm, int x, int y1, int y2, Gulong color, DashState& ds) {
    (*(void(*)(ABitmap&, int, int, int, Gulong, DashState&))funcTable[4])(bm, x, y1, y2, color, ds);
  }

  void clippedLine(ABitmap& bm, fractpel x1, fractpel y1, fractpel x2, fractpel y2, Gulong color, DashState& ds) {
    (*(void(*)(ABitmap&, fractpel, fractpel, fractpel, fractpel, Gulong, DashState&))funcTable[5])(bm, x1, y1, x2, y2, color, ds);
  }

  void clippedCopy(ABitmap& bm1, ABitmap& bm2, region* area) {
    (*(void(*)(ABitmap&, ABitmap&, region*))funcTable[6])(bm1, bm2, area);
  }*/

  void drawPicLine(DrawPicParams& params, int y, int x1, int x2,
		   int xp1, int yp1, int xp2, int yp2) {
      (*(void(*)(DrawPicParams&, int, int, int, int, int, int, int))funcTable[7])
	(params, y, x1, x2, xp1, yp1, xp2, yp2);
  }

  ColHist *getColorHist(const Gulong *p, int size,
			int maxcols, int& hist_size,
			Guchar mask) {
    return (*(ColHist *(*)(const Gulong *, int, int, int&, Guchar mask))funcTable[8])
      (p, size, maxcols, hist_size, mask);
  }

  void freeColorHist(ColHist* p) {
    (*(void(*)(ColHist*))funcTable[9])(p);
  }

  bool medianCut(ColorEntry *cm, ColHist *ch, int colors,
		 long sum, int newColors, Gulong mul, int bits) {
    return (*(bool (*)(ColorEntry *, ColHist *, int, long, int, Gulong, int))
      funcTable[10])(cm, ch, colors, sum, newColors, mul, bits);
  }

  HashTable* createHashTable(ColorEntry* t, int n) {
    return (*(HashTable*(*)(ColorEntry*, int))funcTable[11])(t, n);
  }

  void deleteHashTable(HashTable* t) {
      (*(void(*)(HashTable*))funcTable[12])(t);
  }

  void drawLUTLine(Guchar* q, const Gulong *p, int w0, int dither,
		   HashTable *table, Gulong mask, ColorDiff32 *errDown) {
      (*(void(*)(Guchar*, const Gulong *, int, int,
		 HashTable *, Gulong, ColorDiff32 *))funcTable[13])
	(q, p, w0, dither, table, mask, errDown);
  }

};

#endif

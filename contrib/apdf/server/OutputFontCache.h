//========================================================================
//
// OutputFontCache.h
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#ifndef OUTPUTFONTCACHE_H
#define OUTPUTFONTCACHE_H

#ifdef __GNUC__
#pragma interface
#endif

#include "Object.h"
#include "GfxFont.h"

class OutputFont;

//------------------------------------------------------------------------
// BaseFont
//------------------------------------------------------------------------

class BaseFont {
  enum { cacheSize = 8 };
public:

  // Constructor.
  BaseFont(const char *filename);

  // Destructor.
  virtual ~BaseFont();

  // Was font created successfully?
  virtual GBool isOk() = 0;

  // Returns the font name, as specified internally by the font file.
  // Returns NULL if no name is available.
  char *getName() { return name.getCString(); }

  // Returns the custom font encoding, or NULL if the encoding is
  // not available.  If <taken> is set, the caller of this function
  // will be responsible for freeing the encoding object.
  //FontEncoding *getEncoding(GBool taken);

  // Build the map from character code to internal font index.
  //virtual void buildCharMap(FontEncoding *, short *map) = 0;

  // Get an output font with the given transformation matrix.
  OutputFont *get(double m11, double m12, double m21, double m22);

protected:

  // Create a font with the given transformation matrix.
  virtual OutputFont *create(double m11, double m12, double m21, double m22) = 0;

  //virtual void loadEncoding() = 0;

  void clearCache();
private:
  GBool hex;                   // subsetted font with hex char codes
			       //   (this flag is used for text output)
  OutputFont *
    fonts[cacheSize];          // font cache
  int nFonts;                  // number of fonts in cache.
protected:
  //FontEncoding *encoding;
  //GBool freeEnc;
  GString name;
};


//------------------------------------------------------------------------
// T3BaseFont
//------------------------------------------------------------------------
#if 0
class T3BaseFont : public BaseFont {
public:

  // Constructor.
  T3BaseFont(XRef *xrefA, Dict *fontDict);

  // Destructor.
  virtual ~T3BaseFont();

  // Was font created successfully?
  virtual GBool isOk() { return ok; }

  // Build the map from character code to internal font index.
  virtual void buildCharMap(FontEncoding *, short *map);

  // Get a character glyph.
  GBool getGlyph(int c, Object *result) {
    if (c >= 0 && c < numGlyphs) {
      glyphs[c].fetch(xref, result);
      return gTrue;
    } else {
      result->initNull();
      return gFalse;
    }
  }

protected:

  // Create a font with the given transformation matrix.
  virtual OutputFont *create(double m11, double m12, double m21, double m22) {
    return NULL;
  }

  virtual void loadEncoding() {}

private:
  GBool ok;
  Dict *dict;
  int numGlyphs;
  Object *glyphs;
  XRef *xref;
};
#endif

//------------------------------------------------------------------------
// OutputFont
//------------------------------------------------------------------------

class OutputFont {
public:

  // Constructor.
  OutputFont(BaseFont *base,
	     double m11, double m12, double m21, double m22);

  // Does this font match the ID, size, and angle?
  GBool matches(double m11, double m12, double m21, double m22)
    { return fabs(tm11-m11) + fabs(tm12-m12) + fabs(tm21-m21) +
	     fabs(tm22-m22) < 0.1; }

  // Was font created successfully?
  virtual GBool isOk() = 0;

  // Destructor.
  virtual ~OutputFont();

  // Get character metrics
  /*virtual GBool getMetrics(int c,
			   double *xMin, double *yMin,
			   double *xMax, double *yMax,
			   double *advanceX, double *avanceY) =0;*/

protected:
  BaseFont *base;              // base font
  double tm11, tm12,           // original transform matrix
	 tm21, tm22;
};

//------------------------------------------------------------------------
// BaseFontCache
//------------------------------------------------------------------------

class BaseFontCache {
  enum { cacheSize = 16 };
public:

  // Constructor.
  BaseFontCache();

  // Destructor.
  ~BaseFontCache();

  // Get a font.  This loads a new font if necessary.
  BaseFont *getFont(XRef *xref, GfxFont *gfxFont);
  OutputFont *getFont(XRef *xref, GfxFont *gfxFont,
      double m11, double m12, double m21, double m22);

  // Decrement the use count.
  void release(BaseFont *font);

  // Clear the cache.
  void clear();

private:
  BaseFont *tryFont(const char *, GfxFontType, GfxFont *);

  struct Entry {
    Ref id;
    BaseFont *baseFont;
    int count;
  };

  Entry *entries; // cache entries in reverse-LRU order
  int nValid;     // number of valid entries in entries[]
  int nTotal;     // size of then entries array
};

extern BaseFontCache *baseFontCache;

#endif


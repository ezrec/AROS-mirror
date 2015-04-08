//========================================================================
//
// FontCache.h
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef FONTCACHE_H
#define FONTCACHE_H

#ifdef __GNUC__
#pragma interface
#endif

#include "Object.h"
#include "GfxFont.h"

//------------------------------------------------------------------------
// GfxFontCache
//------------------------------------------------------------------------

class GfxFontCache {
  enum { cacheSize = 32 };
public:

  // Constructor.
  GfxFontCache();

  // Destructor.
  ~GfxFontCache() { clear(); }

  // Clear the cache.
  void clear();

  // Get a font.  This creates a new font if necessary.
  GfxFont *getGfxFont(XRef *xref, char *tag, Ref id1);

  // Decrement the use counter for that font.
  void release(GfxFont *gfxFont);

private:

  struct Entry {
    Ref id;
    GfxFont *gfxFont;
    int count;
  };

  Entry *entries; // cache entries in reverse-LRU order
  int nValid;     // number of valid entries in entries[]
  int nTotal;     // size of then entries array
};


extern GfxFontCache *gfxFontCache;

#endif


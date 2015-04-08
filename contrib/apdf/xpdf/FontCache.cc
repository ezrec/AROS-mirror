//========================================================================
//
// FontCache.cc
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include "FontCache.h"
#include "Error.h"

GfxFontCache *gfxFontCache;

//------------------------------------------------------------------------
// GfxFontCache
//------------------------------------------------------------------------

GfxFontCache::GfxFontCache() : nValid(0), nTotal(0), entries(NULL) {
}

void GfxFontCache::clear() {
  for (int i = 0; i < nValid; ++i)
    delete entries[i].gfxFont;
  delete [] entries;
  nValid = 0;
  nTotal = 0;
  entries = NULL;
}

GfxFont *GfxFontCache::getGfxFont(XRef *xref, char *tag, Ref id1) {
  Object ref;
  Object obj;
  GfxFont *gfxFont;

  for (int i = 0; i < nValid; ++i)
    if (entries[i].id.num == id1.num &&
	entries[i].id.gen == id1.gen) {
      if(i) {
	Entry e = entries[i];
	do {
	  entries[i] = entries[i - 1];
	} while (--i);
	entries[0] = e;
      }
      ++entries[0].count;
      return entries[0].gfxFont;
    }

  while (nValid >= cacheSize && entries[nValid - 1].count == 0)
    delete entries[--nValid].gfxFont;

  if (nValid == nTotal) {
    Entry *p = new Entry [nTotal + 8];
    for(int i = 0; i < nValid; ++i)
      p[i] = entries[i];
    delete [] entries;
    entries = p;
    nTotal += 8;
  }

  ref.initRef(id1.num, id1.gen);
  if (ref.fetch(xref, &obj)->isDict()) {
    gfxFont = GfxFont::makeFont(xref, tag, id1, obj.getDict());

    for(int i = nValid; i != 0; --i)
      entries[i] = entries[i - 1];

    ++nValid;
    entries[0].id = id1;
    entries[0].count = 1;
    entries[0].gfxFont = gfxFont;
  } else {
    error (-1, "Font object is not a dictionary");
    gfxFont = NULL;
  }
  obj.free();
  ref.free();
  return gfxFont;
}

void GfxFontCache::release(GfxFont *gfxFont) {
  for (int i = 0; i < nValid; ++i)
    if (entries[i].gfxFont == gfxFont) {
      --entries[i].count;
      break;
    }
}



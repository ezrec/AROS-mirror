//========================================================================
//
// FontCache.cc
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include "FontCache.h"
#include "server/Type1.h"
#include "server/TrueType.h"
#include "Error.h"
#include "FontFile.h"

BaseFontCache *baseFontCache;
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

GfxFont *GfxFontCache::getGfxFont(XRef *xref, Ref id1) {
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
    gfxFont = GfxFont::makeFont(xref, id1, obj.getDict());

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


//------------------------------------------------------------------------
// BaseFont
//------------------------------------------------------------------------

BaseFont::BaseFont(const char *filename1) :
  encoding(NULL), freeEnc(gTrue)
{
  int code;
  char *charName;

  nFonts = 0;
  hex = gFalse;

  // check for hex char names
  /*hex = gFalse;
  if (!gfxFont->is16Bit()) {
    for (code = 0; code < 256; ++code) {
      if ((charName = gfxFont->getCharName(code))) {
	if ((charName[0] == 'B' || charName[0] == 'C' ||
	     charName[0] == 'G') &&
	    strlen(charName) == 3 &&
	    ((charName[1] >= 'a' && charName[1] <= 'f') ||
	     (charName[1] >= 'A' && charName[1] <= 'F') ||
	     (charName[2] >= 'a' && charName[2] <= 'f') ||
	     (charName[2] >= 'A' && charName[2] <= 'F'))) {
	  hex = gTrue;
	  break;
	}
      }
    }
  } */
}

BaseFont::~BaseFont() {
  clearCache();
}

void BaseFont::clearCache() {
  for (int i = 0; i < nFonts; ++i)
    delete fonts[i];
  if (freeEnc)
    delete encoding;
  nFonts = 0;
  encoding = NULL;
}

OutputFont *BaseFont::get(double m11, double m12, double m21, double m22) {

  for (int i = 0; i < nFonts; ++i)
    if (fonts[i]->matches(m11, m12, m21, m22)) {
      OutputFont *font = fonts[i];
      if(i) {
	do {
	  fonts[i] = fonts[i - 1];
	} while (--i);
	fonts[0] = font;
      }
      return font;
    }

  if (nFonts == cacheSize)
    delete fonts[--nFonts];

  OutputFont* font = create(m11, m12, m21, m22);
  if (font && !font->isOk()) {
    delete font;
    font = NULL;
  }
  if (font) {
    for (int i = nFonts; --i >= 0; )
      fonts[i + 1] = fonts[i];
    fonts[0] = font;
    ++nFonts;
  } else
    error(-1, "Can't create instance of size %f of font \"%s\".",
	  sqrt(m21 * m21 + m22 * m22), getName());
  return font;
}

FontEncoding *BaseFont::getEncoding(GBool taken) {
  //~taken useless...
  if (!encoding) {
    encoding = new FontEncoding;
    loadEncoding();
  }
  FontEncoding *enc = encoding;
  if (enc) {
    if (taken)
      enc = enc->copy();
  }
  return enc;
}

//------------------------------------------------------------------------
// OutputFont
//------------------------------------------------------------------------

OutputFont::OutputFont(BaseFont *base1,
		       double m11, double m12, double m21, double m22):
  base(base1)
{
  tm11 = m11;
  tm12 = m12;
  tm21 = m21;
  tm22 = m22;
}

OutputFont::~OutputFont() {
}

//------------------------------------------------------------------------
// BaseFontCache
//------------------------------------------------------------------------

BaseFontCache::BaseFontCache() : nValid(0), nTotal(0), entries(NULL) {
}

BaseFontCache::~BaseFontCache() {
  for (int i = 0; i < nValid; ++i) {
    delete entries[i].baseFont;
    delete entries[i].name;
  }
  delete [] entries;
}

BaseFont *BaseFontCache::getFont(XRef *xref, const char *name, GfxFont *gfxFont,
				 EncodingID encID) {
  GfxFontType type = gfxFont->getType();
  Ref id1;
  gfxFont->getEmbeddedFontID(&id1);

  for (int i = 0; i < nValid; ++i)
    if (entries[i].id.gen == id1.gen &&
	entries[i].id.num == id1.num &&
	((name == NULL && entries[i].name == NULL) ||
	 (name != NULL && entries[i].name != NULL &&
	  !entries[i].name->cmp((char*)name)))) {
      if(i) {
	Entry e = entries[i];
	do {
	  entries[i] = entries[i - 1];
	} while (--i);
	entries[0] = e;
      }
      ++entries[0].count;
      return entries[0].baseFont;
    }

  while (nValid >= cacheSize && entries[nValid - 1].count == 0) {
    delete entries[--nValid].name;
    delete entries[nValid].baseFont;
  }

  if (nValid == nTotal) {
    Entry *p = new Entry [nTotal + 8];
    for(int i = 0; i < nValid; ++i)
      p[i] = entries[i];
    delete [] entries;
    entries = p;
    nTotal += 8;
  }

  BaseFont *base = NULL;

  // create the font file
  char tmpFileName[32];
  const char *fileName;
  char *buf = NULL;
  if (id1.num != -1) {
    int len;

    buf = gfxFont->readEmbFontFile(xref, &len);

    if (buf != NULL && type == fontType1C) {
      Type1CFontConverter cvt(buf, len);
      cvt.convert();
    }

    if (buf == NULL)
      return NULL;

    // build a 'magic' filename "\001::<buf>:<len>"
    // myfopen() will take the data from 'buf' instead of
    // opening a real file.

    char *t = tmpFileName;
    *t++ = '\001';
    *t++ = ':';
    *t++ = ':';
    unsigned x = unsigned(buf);
    while(x) {
	*t++ = char('0' + (x & 7));
	x >>= 3;
    }
    *t++ = ':';
    x = len;
    while(x) {
	*t++ = char('0' + (x & 7));
	x >>= 3;
    }
    *t = '\0';
    fileName = tmpFileName;
  } else {
    fileName = name;
  }
  if (type == fontTrueType)
    base = new TTBaseFont(fileName, encID);
  else
    base = new T1BaseFont(fileName, encID);

  gfree(buf);

  if (base && !base->isOk()) {
    delete base;
    base = NULL;
  }

  GString *n = name ? new GString((char*)name) : NULL;

  for(int i = nValid; i != 0; --i)
    entries[i] = entries[i - 1];
  ++nValid;
  entries[0].name = n;
  entries[0].id = id1;
  entries[0].count = 1;
  return entries[0].baseFont = base;
}

void BaseFontCache::release(BaseFont *baseFont) {
  for (int i = 0; i < nValid; ++i)
    if (entries[i].baseFont == baseFont) {
      --entries[i].count;
      break;
    }
}


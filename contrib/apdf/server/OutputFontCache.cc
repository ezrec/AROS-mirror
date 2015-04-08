//========================================================================
//
// OutputFontCache.cc
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include "FontMap.h"
#include "GlobalParams.h"
#include "server/OutputFontCache.h"
#include "server/FreeType.h"
#include "Error.h"

#define DEBUG_FONT(x)   ;


BaseFontCache *baseFontCache;

//------------------------------------------------------------------------
// BaseFont
//------------------------------------------------------------------------

BaseFont::BaseFont(const char *filename1)
{
  nFonts = 0;
}

BaseFont::~BaseFont() {
  clearCache();
}

void BaseFont::clearCache() {
  for (int i = 0; i < nFonts; ++i)
    delete fonts[i];
  nFonts = 0;
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
  clear();
}

void BaseFontCache::clear() {
  for (int i = 0; i < nValid; ++i) {
    delete entries[i].baseFont;
  }
  delete [] entries;

  nValid = 0;
  nTotal = 0;
  entries = NULL;
}

BaseFont *BaseFontCache::tryFont(const char *fileName, GfxFontType type, GfxFont *gfxFont) {
  BaseFont *base;

  DEBUG_FONT(printf("tryFont(%s,%d,%x)\n",
		    *fileName == 1 ? "<internal>" : fileName, type, gfxFont));
  if (gfxFont->isCIDFont()) {
    if (gfxFont->getType() == fontCIDType2) {
      base = new FTBaseFont(fileName, ((GfxCIDFont *)gfxFont)->getCIDToGID(),
	  ((GfxCIDFont *)gfxFont)->getCIDToGIDLen());
    } else { // fontCIDType0C
      base = new FTBaseFont(fileName);
    }
  } else {
    base = new FTBaseFont(fileName, ((Gfx8BitFont *)gfxFont)->getEncoding(),
	((Gfx8BitFont *)gfxFont)->getHasEncoding());
  }

  if (base && !base->isOk()) {
    delete base;
    base = NULL;
  }

  return base;
}

BaseFont *BaseFontCache::getFont(XRef *xref, GfxFont *gfxFont) {
  static char *type1Exts[] = { ".pfa", ".pfb", ".ps", "", NULL };
  static char *ttExts[] = { ".ttf", NULL };
  GfxFontType type = gfxFont->getType();
  Ref id1 = *gfxFont->getID();

  for (int i = 0; i < nValid; ++i) {
    if (entries[i].id.gen == id1.gen &&
	entries[i].id.num == id1.num) {
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
  }

  DEBUG_FONT(printf("getFont(%x,%s)\n", gfxFont,
		    gfxFont->getName() ? gfxFont->getName()->getCString() : "unnamed"));

  while (nValid >= cacheSize && entries[nValid - 1].count == 0) {
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
  GString *str = NULL;

  // create the font file
  char tmpFileName[32];
  const char *fileName=NULL;
  char *buf = NULL;
  if (gfxFont->getEmbeddedFontID(&id1)) {
    int len;

    buf = gfxFont->readEmbFontFile(xref, &len);

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
  } else if (gfxFont->getExtFontFile()) {
    fileName = gfxFont->getExtFontFile()->getCString();
  }
  if (fileName) {
    base = tryFont(fileName, type, gfxFont);
  }

  gfree(buf);

  if (!base) {

    GString *name2 = NULL;

    // look for font mapping
    if (gfxFont->isCIDFont()) {
      name2 = ((GfxCIDFont *)gfxFont)->getCollection();
    } else {
      name2 = gfxFont->getName();
    }
    DEBUG_FONT(printf("getFont: name \"%s\"\n",
		      name2 ? name2->getCString() : "NULL"));

    if (name2) {
      fileName = NULL;
      if (!docFontMap || !(fileName = docFontMap->find(name2->getCString(), type))) {
	fileName = globalFontMap.find(name2->getCString(), type);
      }
      DEBUG_FONT(printf("getFont: filename \"%s\"\n",
			fileName ? fileName : "NULL"));

      if (fileName) {
	GString name1(fileName);
	if (type == fontType1) {
	  str = globalParams->findFontFile(&name1, type1Exts);
	} else {
	  str = globalParams->findFontFile(&name1, ttExts);
	}
	if (str) {
	  DEBUG_FONT(printf("getFont: file \"%s\"\n",
			    str->getCString()));
	  base = tryFont(str->getCString(), type, gfxFont);
	}
      }
    }
  }

  if (!base && !gfxFont->isCIDFont()) {
    // do a property-based substitution
    int f = gfxFont->getFlags(), oldf = -1;
    int pass = 4;
    GfxFontType t;
    const char *substName = NULL;
    do {
      switch (pass) {
      case 1:
	f &= ~fontScript;
      case 2:
	f &= ~fontAllCap;
      case 3:
	f &= ~fontSmallCap;
      case 4:
	if (f != oldf) {
	  substName = ::getFont(f, t);
	  if (substName) {
	    GString name1(substName);
	    delete str;
	    str = NULL;
	    if (t == fontType1) {
	      str = globalParams->findFontFile(&name1, type1Exts);
	    } else {
	      str = globalParams->findFontFile(&name1, ttExts);
	    }
	    if (str) {
	      base = tryFont(str->getCString(), t, gfxFont);
	    }
	  }
	  oldf = f;
	}
	break;
      }
    } while (!base && --pass);
  }

  delete str;

  for(int i = nValid; i != 0; --i)
    entries[i] = entries[i - 1];
  ++nValid;
  entries[0].id = *gfxFont->getID();
  entries[0].count = 1;
  entries[0].baseFont = base;

  return base;
}

OutputFont *BaseFontCache::getFont(XRef *xref, GfxFont *gfxFont,
    double m11, double m12, double m21, double m22) {
  BaseFont *font = getFont(xref, gfxFont);
  if (font) {
    return font->get(m11, m12, m21, m22);
  }
  return NULL;
}


void BaseFontCache::release(BaseFont *baseFont) {
  for (int i = 0; i < nValid; ++i)
    if (entries[i].baseFont == baseFont) {
      --entries[i].count;
      break;
    }
}


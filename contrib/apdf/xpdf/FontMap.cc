//========================================================================
//
// FontMap.cc
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include "GString.h"
#include "gmem.h"
#include "config.h"
#include "Error.h"
#include "GfxFont.h"
#include "FontMap.h"

FontMap globalFontMap;
FontMap *docFontMap;

//------------------------------------------------------------------------
// Builtin font map
//------------------------------------------------------------------------
/*
struct FontMapEntry {
  const char *pdfFont;
  const char *font;
};

static FontMapEntry builtinFontMap[] = {
  {"Courier",               "Ghostscript:fonts/n022003l.pfb"},
  {"Courier-Bold",          "Ghostscript:fonts/n022004l.pfb"},
  {"Courier-BoldOblique",   "Ghostscript:fonts/n022024l.pfb"},
  {"Courier-Oblique",       "Ghostscript:fonts/n022023l.pfb"},
  {"Helvetica",             "Ghostscript:fonts/n019003l.pfb"},
  {"Helvetica-Bold",        "Ghostscript:fonts/n019004l.pfb"},
  {"Helvetica-BoldOblique", "Ghostscript:fonts/n019024l.pfb"},
  {"Helvetica-Oblique",     "Ghostscript:fonts/n019023l.pfb"},
  {"Symbol",                "Ghostscript:fonts/s050000l.pfb"},
  {"Times-Bold",            "Ghostscript:fonts/n021004l.pfb"},
  {"Times-BoldItalic",      "Ghostscript:fonts/n021024l.pfb"},
  {"Times-Italic",          "Ghostscript:fonts/n021023l.pfb"},
  {"Times-Roman",           "Ghostscript:fonts/n021003l.pfb"},
  {"ZapfDingbats",          "Ghostscript:fonts/d050000l.pfb"},
  {NULL}
};
static const int builtinFontMapSize=sizeof(builtinFontMap)/sizeof(builtinFontMap[0])-1;
*/

//------------------------------------------------------------------------
// Font substitutions
//------------------------------------------------------------------------

// index: {fixed:8, serif:4, sans-serif:0} + bold*2 + italic
static const char* fontSubst[] = {
  "Helvetica",
  "Helvetica-Oblique",
  "Helvetica-Bold",
  "Helvetica-BoldOblique",
  "Times-Roman",
  "Times-Italic",
  "Times-Bold",
  "Times-BoldItalic",
  "Courier",
  "Courier-Oblique",
  "Courier-Bold",
  "Courier-BoldOblique",
};
static const int fontSubstSize=sizeof(fontSubst)/sizeof(fontSubst[0]);

const char *getFont(int flags, GfxFontType& type) {

  type = fontType1;

  GString name("« ");

  if (flags & fontFixedWidth)
    name.append("fixed width, ");
  if (flags & fontSerif)
    name.append("serif, ");
  if (flags & fontScript)
    name.append("script, ");
  if (flags & fontAllCap)
    name.append("all caps, ");
  if (flags & fontSmallCap)
    name.append("small caps, ");
  if (name.getLength() > 2)
    name.del(name.getLength() - 2, 2);
  name.append(" »");

  const char *fontName;
  if (docFontMap && (fontName = docFontMap->find(name.getCString(), type)))
    return fontName;
  if (fontName = globalFontMap.find(name.getCString(), type))
    return fontName;
  /*for (FontMapEntry * p = builtinFontMap; p->pdfFont; ++p)
    if (!pdfFont->cmp((char*)p->pdfFont))
      return p->font;*/

  if (!(flags & (fontSmallCap | fontAllCap | fontScript))) {
    int index;
//~ Some non-symbolic fonts are tagged as symbolic.
//  if (flags & fontSymbol)
//    index = 12;
//  else
    if (flags & fontFixedWidth)
      index = 8;
    else if (flags & fontSerif)
      index = 4;
    else
      index = 0;
    if (flags & fontBold)
      index += 2;
    if (flags & fontItalic)
      index += 1;

    const char* defFontName = fontSubst[index];
    if (docFontMap && (fontName = docFontMap->find(defFontName, type)))
      return fontName;
    if (fontName = globalFontMap.find(defFontName, type))
      return fontName;
    return defFontName;
  }
  return NULL;
}

FontMap::FontMap() : beg(NULL), cur(NULL), end(NULL) {}

void FontMap::clear() {
  Entry *p = cur;
  if (p != beg)
    do {
      --p;
      delete p->pdfFont;
      delete p->font;
    } while (p != beg);
  gfree(beg);
  beg = NULL;
  cur = NULL;
  end = NULL;
}

const char *FontMap::find(const char *pdfFont, GfxFontType& type) {
  unsigned n = cur - beg;
  Entry *p = beg;
  while (n > 0) {
    unsigned k = n / 2;
    Entry *q = p + k;
    int cmp = strcmp(pdfFont, q->pdfFont->getCString());
    if (cmp > 0) {
      p = q + 1;
      n -= k + 1;
    } else if(cmp != 0)
      n = k;
    else {
      type = q->type;
      return q->font->getCString();
    }
  }
  return NULL;
}

void FontMap::add(const char *pdfFont, const char *font,
		  GfxFontType type, int flags) {
  unsigned n = cur - beg;
  Entry *p = beg;
  while (n > 0) {
    unsigned k = n / 2;
    Entry *q = p + k;
    int cmp = strcmp(pdfFont, q->pdfFont->getCString());
    if (cmp > 0) {
      p = q + 1;
      n -= k + 1;
    } else if(cmp != 0)
      n = k;
    else {
      p = q;
      break;
    }
  }
  if (cur == end) {
    unsigned sz = ((end - beg) * 3) / 2 + 16;
    ptrdiff_t k = cur - beg;
    ptrdiff_t l = p - beg;
    beg = (Entry*)grealloc(beg, sz * sizeof(*beg));
    end = beg + sz;
    cur = beg + k;
    p = beg + l;
  }

  GString *pdfFontStr = new GString((char*)pdfFont);
  GString *fontStr = new GString((char*)font);
  if (p != cur) {
    // move entries after the two preceding lines to avoid leaving
    // the object in an incoherent state in case of an exception.
    memmove(p + 1, p, (cur - p) * sizeof(*p));
  }
  p->pdfFont = pdfFontStr;
  p->font = fontStr;
  p->type = type;
  p->flags = flags;
  ++cur;
}

/*GBool FontMap::substitute(Gfx8BitFont *gfxFont) {
  GfxFontType type;
  const char *name = find(gfxFont->getName()->getCString(), type);
  if (name) {
    gfxFont->substitute(new GString((char*)name), type);
    return gTrue;
  } else
    return gFalse;
}*/

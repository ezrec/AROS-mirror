//========================================================================
//
// FontMap.h
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef FONTMAP_H
#define FONTMAP_H

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include "gtypes.h"
#include "GString.h"
#include "GfxFont.h"

class GfxFont;


extern const char *defFontsNames[];
static const int numDefFonts = 12;

const char *getFont(int flags, GfxFontType&);


class FontMap {
public:
  FontMap();
  ~FontMap() { clear(); }
  //const char *getFont(GfxFont *, GfxFontType&);
  void clear();
  void add(const char *pdfFont, const char *font, GfxFontType type, int flags);
  void add(const char *pdfFont, const char *font, GfxFontType type) {
    add(pdfFont, font, type, 0);
  }
  int size() const { return cur - beg; }
  //GBool substitute(Gfx8BitFont*);
  const char *find(const char *pdfFont, GfxFontType& type);
  void get(int n, const char *& pdfFont, const char *& font,
	   GfxFontType& type, int& flags) {
    Entry& e = beg[n];
    pdfFont = e.pdfFont->getCString();
    font = e.font->getCString();
    type = e.type;
    flags = e.flags;
  }
private:
  struct Entry {
    GString *pdfFont;
    GString *font;
    GfxFontType type;
    int flags;
  };
  Entry* beg;
  Entry* cur;
  Entry* end;
};

extern FontMap globalFontMap;
extern FontMap *docFontMap;

#endif


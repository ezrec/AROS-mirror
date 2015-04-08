//========================================================================
//
// FontOutputDev.cc
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
#include "GfxState.h"
#include "GfxFont.h"
#include "FontOutputDev.h"
//#include "XOutputFontInfo.h"


FontOutputDev::FontOutputDev() {}

FontOutputDev::~FontOutputDev() {}

void FontOutputDev::updateFont(GfxState *state) {
  if (GfxFont *gfxFont = state->getFont()) {
    if (gfxFont->getName()) {
      char *pdfFontName = gfxFont->getName()->getCString();
      GfxFontType type;
      if (fontMap.find(pdfFontName,type))
	return;
      GString prop;
      int flags = gfxFont->getFlags();
      if (flags & fontFixedWidth)
	prop.append(", fixed width");
      if (flags & fontSerif)
	prop.append(", serif");
      //~ lots of fonts are tagged as symbolic when they should not.
      //if (flags & fontSymbolic)
      //  prop.append(", symbolic");
      if (flags & fontScript)
	prop.append(", script");
      if (flags & fontAllCap)
	prop.append(", all caps");
      if (flags & fontSmallCap)
	prop.append(", small caps");
      if (flags & fontItalic)
	prop.append(", italic");
      if (flags & fontBold)
	prop.append(", bold");
      //~ this flag is not that meaningful.
      //if (flags & fontStdCharSet)
      //  prop.append(", std charset");
      if (prop.getLength() < 2)
	  prop.append("  ");
      Ref id;
      fontMap.add(pdfFontName, prop.getCString() + 2,
		  gfxFont->getType(), gfxFont->getEmbeddedFontID(&id));
    }
  }
}

#if 0
DefaultFontOutputDev::DefaultFontOutputDev() {
  beg = (Entry*)gmalloc((numDefFonts + fontMapSize) * sizeof(*beg));
  end = beg + numDefFonts + fontMapSize;
  Entry* p = beg;
  for (int k = 0; k < fontMapSize; ++k) {
    p->pdfFont = new GString(fontMap[k].pdfFont);
    FontMapEntry* q;
    for (q = userFontMap; q->pdfFont; ++q)
      if (!strcmp(q->pdfFont, p->pdfFont->getCString()))
	break;
    if (!q || !q->pdfFont)
      q = fontMap + k;
    p->font = q->font;
    p->type = fontUnknownType;
    p->flags = 0;
    cur = ++p;
  }
  for (int k = 0; k < numDefFonts; ++k) {
    p->pdfFont = new GString((char*)defFontsNames[k]);
    FontMapEntry* q;
    for (q = userFontMap; q->pdfFont; ++q) {
      if (!strcmp(q->pdfFont, p->pdfFont->getCString()))
	break;
    }
    if (q && q->pdfFont)
      p->font = q->font;
    else
      p->font = fontSubst[k].font;
    cur = ++p;
  }
}
#endif


//========================================================================
//
// Type3.cc - Type3 fonts handling
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#define DB(x) //x

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include "gmem.h"
#include "GString.h"
#include "Object.h"
#include "Stream.h"
#include "GfxState.h"
#include "GfxFont.h"
#include "FontFile.h"
#include "FontEncoding.h"
#include "FontCache.h"
#include "Error.h"

T3BaseFont::T3BaseFont(Dict *fontDict) : BaseFont("T3"), ok(gFalse) {
  Object obj1;

  numGlyphs = 0;
  glyphs = NULL;

  fontDict->lookup("CharProcs", &obj1);
  if (obj1.isDict()) {
    dict = obj1.getDict();
    dict->incRef();
    numGlyphs = dict->getLength();
    glyphs = new Object [numGlyphs];
    for (int i = 0; i < numGlyphs; ++i)
      dict->getValNF(i, &glyphs[i]);
    ok = gTrue;
  } else
    error(-1, "CharProcs is not a dictionary");
  obj1.free();
}

T3BaseFont::~T3BaseFont() {
  for (int i = 0; i < numGlyphs; ++i)
    glyphs[i].free();
  delete [] glyphs;
}

void T3BaseFont::buildCharMap(FontEncoding *enc, short *map) {
  for (int i = 0; i < numGlyphs; ++i) {
    int n = enc->getCharCode(dict->getKey(i));
    if (n >= 0 && n < 256)
      map[n] = i;
  }
  if (!dict->decRef())
    delete dict;
}


//========================================================================
//
// Type1.cc - Type1 fonts handling
//
// Copyright 1999-2002 Emmanuel Lesueur
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
#include <setjmp.h>
#include "gmem.h"
#include "GString.h"
#include "Object.h"
#include "Stream.h"
#include "GfxState.h"
#include "GfxFont.h"
#include "FontFile.h"
//#include "FontEncoding.h"
#include "Error.h"
#include "TextOutputDev.h"
#include "AOutputDev.h"
#include "FontOutputDev.h"
#include "Type1.h"


extern "C" {
  char* fontfcnA(char *env, int *mode, psfont *Font_Ptr);
  region *fontfcnB_ByIndex(int modflag, XYspace *S,
			   int charindex, int *mode,
			   psfont *Font_Ptr, int do_raster);
  int SearchDictName(psdict *,psobj *);
  char *t1_get_abort_message(int);
  jmp_buf stck_state;
  extern XYspace *IDENTITY;
}


//------------------------------------------------------------------------
// AT1BaseFont
//------------------------------------------------------------------------

T1BaseFont::T1BaseFont(const char *fileName, char **fontEnc/*, double **bboxA*/) :
  BaseFont(fileName), fontMatrix(NULL), vm_base(NULL), ok(gFalse)
{
  int first = 0x10000;
  int last = -1;

  memset(&type1Data, 0, sizeof(type1Data));

  if(!load(fileName))
    return;

  /*for (i = 0; i < 4; ++i) {
    bbox[i] = bboxA[i];
  }*/

  for (int i = 0; i < 256; ++i) {
    psobj name;
    map[i] = -1;
    if (name.data.nameP = *fontEnc++) {
      name.len = strlen(name.data.nameP);
      int n = SearchDictName(type1Data.CharStringsP, &name);
      if (n >= 0) {
	if (n < first)
	  first = n;
	if (n > last)
	  last = n;
	map[i] = short(n);
      }
    }
  }

  first_glyph = first;
  last_glyph = last;
  ok = first <= last;
}

T1BaseFont::~T1BaseFont() {
  clearCache();
  vm_freeall(vm_base);
  if (fontMatrix)
    KillSpace(fontMatrix);
}

GBool T1BaseFont::isOk() {
  return ok;
}

#if 0
void T1BaseFont::loadEncoding() {
  psobj *encobj = type1Data.fontInfoP[ENCODING].value.data.arrayP;
  for (int i = 0; i < 256; ++i, ++encobj) {
    if (strcmp(encobj->data.nameP, ".notdef")) {
      char *s = (char*)gmalloc(encobj->len + 1);
      if (encobj->len)
	memcpy(s, encobj->data.nameP, encobj->len);
      s[encobj->len]='\0';
      encoding->addChar(i, s);
    }
  }
}

void T1BaseFont::buildCharMap(FontEncoding *enc, short *map) {
  for (int i = 0; i < 256; ++i, ++map) {
    psobj name;
    if (name.data.nameP = enc->getCharName(i)) {
      name.len = strlen(name.data.nameP);
      int n = SearchDictName(type1Data.CharStringsP, &name);
      if (n >= 0)
	*map = short(n);
    }
  }
}
#endif

OutputFont* T1BaseFont::create(double m11, double m12, double m21, double m22) {
  return new T1OutputFont(this, m11, m12, m21, m22);
}

GBool T1BaseFont::load(const char *fileName) {
  int mode = 0;

  if (int err = setjmp(stck_state)) {
    error(-1, "T1 Error: %s\n", t1_get_abort_message(err));
    return gFalse;
  }

  // Load all PostScript information into memory
  if (!(vm_base = fontfcnA((char*)fileName, &mode, &type1Data))) {
    error(-1, "Can't load Type1 font \"%s\" (%d)", fileName, mode);
    return gFalse;
  }

  /* Now, that the font has been loaded into memory, try to find the
     FontMatrix in the font info dictionary. If it exists, load it into
     our local fontmatrix, otherwise use a default matrix which scales to
     1/1000 (since font outlines  are defined in a 1000 point space)
     and does no further transformations. */
  fontMatrix = (XYspace *) IDENTITY;
  if (type1Data.fontInfoP[FONTMATRIX].value.data.arrayP == NULL) {
    fontMatrix = (XYspace *)Scale(fontMatrix, 0.001, 0.001);
  } else {
    fontMatrix = (XYspace *)Transform(fontMatrix,
      (double)type1Data.fontInfoP[FONTMATRIX].value.data.arrayP[0].data.real,
      (double)type1Data.fontInfoP[FONTMATRIX].value.data.arrayP[1].data.real,
      (double)type1Data.fontInfoP[FONTMATRIX].value.data.arrayP[2].data.real,
      (double)type1Data.fontInfoP[FONTMATRIX].value.data.arrayP[3].data.real);
  }
  fontMatrix = (XYspace *)Permanent(fontMatrix);
  return gTrue;
}

//------------------------------------------------------------------------
// T1OutputFont
//------------------------------------------------------------------------

T1OutputFont::T1OutputFont(T1BaseFont *base1,
			   double m11, double m12,
			   double m21, double m22):
  AOutputFont(base1, m11, m12, m21, m22),
  charSpaceLocal(NULL), glyphs(NULL)
{
  if (int err = setjmp(stck_state)) {
    error(-1, "T1 Error: %s\n", t1_get_abort_message(err));
    return;
  }

  // setup charSpaceLocal for this font
  charSpaceLocal = (XYspace *) Permanent(Transform(
    base1->fontMatrix, m11, m12, m21, m22));

  // allocate the glyph array
  glyphs = new Glyph [base1->last_glyph - base1->first_glyph + 1];
}

T1OutputFont::~T1OutputFont() {
  delete [] glyphs;
  if (charSpaceLocal)
    KillSpace(charSpaceLocal);
}

GBool T1OutputFont::isOk() {
  return glyphs != NULL;
}

region *T1OutputFont::getCharInterior(CharCode c, Unicode) {
  T1BaseFont *t1base = (T1BaseFont *)base;
  if (c < 0 || c > 255)
    return NULL;
  c = t1base->map[c];
  if (c < 0)
    return NULL;
  int first = t1base->first_glyph;
  region *area = glyphs[c-first].region;
  if (area == NULL) {
    if (int err = setjmp(stck_state)) {
      error(-1, "T1 Error: %s\n", t1_get_abort_message(err));
      return NULL;
    }
    if (glyphs[c-first].outline)
      area = Interior(Dup(glyphs[c-first].outline), WINDINGRULE + CONTINUITY);
    else {
      int mode = 0;
      int modflag = 0;
      area = fontfcnB_ByIndex(modflag, charSpaceLocal, c, &mode,
			      &((T1BaseFont*)base)->type1Data, 1);
      if (mode != 0)
	return NULL;
    }
    glyphs[c-first].region = area;
  }
  return area;
}

segment *T1OutputFont::getCharOutline(CharCode c, Unicode) {
  T1BaseFont *t1base = (T1BaseFont *)base;
  if (c < 0 || c > 255)
    return NULL;
  c = t1base->map[c];
  if (c < 0)
    return NULL;
  int first = t1base->first_glyph;
  segment *outline = glyphs[c-first].outline;
  if (outline == NULL) {

    if (int err = setjmp(stck_state)) {
      error(-1, "T1 Error: %s\n", t1_get_abort_message(err));
      return NULL;
    }
    int mode = 0;
    int modflag = 0;
    outline = (segment*)fontfcnB_ByIndex(modflag, charSpaceLocal,
					 c, &mode,
					 &((T1BaseFont*)base)->type1Data, 0);
    if (mode != 0 || outline == NULL)
      return NULL;
    glyphs[c-first].outline = outline;
  }
  return outline;
}


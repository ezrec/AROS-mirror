//========================================================================
//
// TrueType.h
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#ifndef TRUETYPE_H
#define TRUETYPE_H

#ifdef __GNUC__
#pragma interface
#endif

#include "OutputFontCache.h"
#include "freetype/freetype.h"
#include "freetype/ftxpost.h"

class TTOutputFont;

enum TTFontIndexMode {
  ttFontModeUnicode,
  ttFontModeCharCode,
  ttFontModeCharCodeOffset,
  ttFontModeCodeMap,
  ttFontModeCIDToGIDMap
};

//------------------------------------------------------------------------
// TTBaseFont
//------------------------------------------------------------------------

class TTBaseFont : public BaseFont {
  friend class TTOutputFont;
public:

  TTBaseFont(const char *fileName, char **encoding, GBool hasEncoding);
  TTBaseFont(const char *fileName, Gushort *, int);

  virtual ~TTBaseFont();

  // Was font created successfully?
  virtual GBool isOk();

#if 0
  // Load the font's encoding
  virtual void loadEncoding();

  // Build the map from character code to internal font index.
  virtual void buildCharMap(FontEncoding *, short *map);
#endif

protected:
  // Get a font with the given transformation matrix.
  virtual OutputFont* create(double m11, double m12, double m21, double m22);

private:
  GBool load(const char *fileName);
  const char *cloneBuf(const char *, char *);

  TT_Face face;
  TT_Face_Properties props;
  TT_CharMap charMap;
  TTFontIndexMode mode;
  int charMapOffset;
  Guchar *codeMap;
  Gushort *cidToGID;
  int cidToGIDLen;
  //TT_Post post;
  //Guchar *glyphIndex;
  int numGlyphs;
  //FontEncoding *stdEnc;
  GBool ok;
  char* buf;
};

#endif


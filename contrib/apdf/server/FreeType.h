//========================================================================
//
// FreeType.h
//
// Copyright 2002 Emmanuel Lesueur
//
//========================================================================

#ifndef SERVER_FREETYPE_H
#define SERVER_FREETYPE_H

#ifdef __GNUC__
#pragma interface
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_RENDER_H
#include FT_OUTLINE_H

#include "OutputFontCache.h"
#include "CharTypes.h"

class FTOutputFont;

enum FTFontIndexMode {
  ftFontModeUnicode,
  ftFontModeCharCode,
  ftFontModeCharCodeOffset,
  ftFontModeCodeMap,
  ftFontModeCodeMapDirect,
  ftFontModeCIDToGIDMap,
  ftFontModeCFFCharset
};

//------------------------------------------------------------------------
// FTBaseFont
//------------------------------------------------------------------------

class FTBaseFont : public BaseFont {
  friend class FTOutputFont;
public:

  // 8-bit font, TrueType or Type 1/1C
  FTBaseFont(const char *fileName, char **encoding, GBool hasEncoding);

  // CID font, TrueType
  FTBaseFont(const char *fileName, Gushort *, int);

  // CID font, Type 0C
  FTBaseFont(const char *fileName);

  virtual ~FTBaseFont();

  // Was font created successfully?
  virtual GBool isOk();

protected:
  // Get a font with the given transformation matrix.
  virtual OutputFont* create(double m11, double m12, double m21, double m22);

private:
  GBool load(const char *fileName);
  int openFont(const char *fileName);
  const char *cloneBuf(const char *, char *);

  FT_Face face;
  FTFontIndexMode mode;
  int charMapOffset;
  Guint *codeMap;
  Gushort *cidToGID;
  int cidToGIDLen;
  GBool ok;
  char* buf;
  int bufLen;
  GString *tmpFileName;
};

#endif


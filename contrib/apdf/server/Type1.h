//========================================================================
//
// Type1.h
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#ifndef TYPE1_H
#define TYPE1_H

#ifdef __GNUC__
#pragma interface
#endif

#include "OutputFontCache.h"

#include "type1/objects.h"
#include "type1/spaces.h"
#include "type1/paths.h"
#include "type1/regions.h"
#include "type1/util.h"
#include "type1/fontfcn.h"

class T1OutputFont;

//------------------------------------------------------------------------
// T1BaseFont
//------------------------------------------------------------------------

class T1BaseFont : public BaseFont {
  friend class T1OutputFont;
public:

  // Constructor.
  T1BaseFont(const char *filename, char **encoding);

  // Destructor.
  virtual ~T1BaseFont();

  // Was font created successfully?
  virtual GBool isOk();

  // Load the font's encoding
  //virtual void loadEncoding();

  // Build the map from character code to internal font index.
  //virtual void buildCharMap(FontEncoding *, short *map);

protected:
  // Get a font with the given transformation matrix.
  virtual OutputFont* create(double m11, double m12, double m21, double m22);

private:
  GBool load(const char *fileName);

  char *vm_base;
  psfont type1Data;
  XYspace *fontMatrix;
  int first_glyph;
  int last_glyph;
  short map[256];
  GBool ok;
};


#endif


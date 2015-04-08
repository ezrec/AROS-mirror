//========================================================================
//
// FontOutputDev.h
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef FONTOUTPUTDEV_H
#define FONTOUTPUTDEV_H

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include "gtypes.h"
#include "OutputDev.h"
#include "FontMap.h"

class GfxState;

class FontOutputDev: public OutputDev {
public:

  FontOutputDev();

  // Destructor.
  virtual ~FontOutputDev();

  // Check if file was successfully created.
  virtual GBool isOk() { return gTrue; }

  //---- get info about output device

  // Does this device use upside-down coordinates?
  // (Upside-down means (0,0) is the top left corner of the page.)
  virtual GBool upsideDown() { return gTrue; }

  // Does this device use drawChar() or drawString()?
  virtual GBool useDrawChar() { return gTrue; }

  // Does this device use beginType3Char/endType3Char?  Otherwise,
  // text in Type 3 fonts will be drawn with drawChar/drawString.
  virtual GBool interpretType3Chars() { return gFalse; }

  //----- update text state
  virtual void updateFont(GfxState *state);

  //----- special access
  int size() const { return fontMap.size(); }

  void get(int n, const char*& pdfFont, const char*& font,
	   GfxFontType& type, int& flags) {
    fontMap.get(n, pdfFont, font, type, flags);
  }

protected:
  FontMap fontMap;
};

class DefaultFontOutputDev : public FontOutputDev {
public:
  DefaultFontOutputDev();
};

#endif


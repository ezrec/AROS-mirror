//========================================================================
//
// AROSSplashOutputDev.h
//
// Derived from XSplashOutputDev.h
// Copyright 2005 Vijay Kumar B <vijaykumar@bravegnu.org>
// Copyright 2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef AROSSPLASHOUTPUTDEV_H
#define AROSSPLASHOUTPUTDEV_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif
#include <exec/types.h>   

//externed

extern "C" {
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <libraries/mui.h>
#include <utility/hooks.h>
}
#include <proto/cybergraphics.h>
#include <aros/debug.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <clib/gadtools_protos.h>


#include "SplashTypes.h"
#include "SplashOutputDev.h"
#include "TextOutputDev.h"

//------------------------------------------------------------------------

#define xOutMaxRGBCube 6	// max size of RGB color cube

//------------------------------------------------------------------------
// AROSSplashOutputDev
//------------------------------------------------------------------------

class AROSSplashOutputDev: public SplashOutputDev {
public:

  AROSSplashOutputDev(struct RastPort *visualA,GBool reverseVideoA, 
		     SplashColor paperColorA,
		     GBool incrementalUpdateA,
		     void (*redrawCbkA)(void *data),
		     void *redrawCbkDataA);
  virtual ~AROSSplashOutputDev();

  //----- initialization and control

  // Start a page.
  virtual void startPage(int pageNum, GfxState *state);

  // End a page.
  virtual void endPage();

  // Dump page contents to display.
  virtual void dump();

  //----- update text state
  virtual void updateFont(GfxState *state);

  //----- text drawing
  virtual void drawChar(GfxState *state, double x, double y,
			double dx, double dy,
			double originX, double originY,
			CharCode code, int nBytes, Unicode *u, int uLen);
  virtual GBool beginType3Char(GfxState *state, double x, double y,
			       double dx, double dy,
			       CharCode code, int nBytes,
			       Unicode *u, int uLen);

  //----- special access

  // Clear out the document (used when displaying an empty window).
  void clear();

  // Copy the rectangle (srcX, srcY, width, height) to (destX, destY)
  // in destDC.
  void redraw(int srcX, int srcY,
	      int destX, int destY,
	      int width, int height,
	      int destW, int destH);

  // Find a string.  If <startAtTop> is true, starts looking at the
  // top of the page; else if <startAtLast> is true, starts looking
  // immediately after the last find result; else starts looking at
  // <xMin>,<yMin>.  If <stopAtBottom> is true, stops looking at the
  // bottom of the page; else if <stopAtLast> is true, stops looking
  // just before the last find result; else stops looking at
  // <xMax>,<yMax>.
  GBool findText(Unicode *s, int len,
		 GBool startAtTop, GBool stopAtBottom,
		 GBool startAtLast, GBool stopAtLast,
		 int *xMin, int *yMin,
		 int *xMax, int *yMax);

  // Get the text which is inside the specified rectangle.
  GString *getText(int xMin, int yMin, int xMax, int yMax);

private:

  GBool incrementalUpdate;      // incrementally update the display?
  void (*redrawCbk)(void *data);
  void *redrawCbkData;
  TextPage *text;               // text from the current page

  struct RastPort *visual;         // AROS visual pointer
};

#endif

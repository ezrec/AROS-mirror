//========================================================================
//
// AROSSplashOutputDev.cc
//
// Derived from XSplashOutputDev.cc.
// Copyright 2005 Vijay Kumar B. <vijaykumar@bravegnu.org>
// Copyright 2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmem.h"
#include "SplashTypes.h"
#include "SplashBitmap.h"
#include "Object.h"
#include "GfxState.h"
#include "TextOutputDev.h"
#include "AROSSplashOutputDev.h"
#include <cybergraphx/cybergraphics.h>
#include <proto/cybergraphics.h>


//------------------------------------------------------------------------
// Constants and macros
//------------------------------------------------------------------------

#define xoutRound(x) ((int)(x + 0.5))

//------------------------------------------------------------------------
// AROSSplashOutputDev
//------------------------------------------------------------------------

AROSSplashOutputDev::AROSSplashOutputDev(struct RastPort *visualA,
				       GBool reverseVideoA, 
				       SplashColor paperColorA,
				       GBool incrementalUpdateA,
				       void (*redrawCbkA)(void *data),
				       void *redrawCbkDataA):
  SplashOutputDev(splashModeRGB8, 4, reverseVideoA, paperColorA)
{
  Gulong mask;
  int r, g, b, n, m;
  GBool ok;
  incrementalUpdate = incrementalUpdateA;
  visual = visualA;
  redrawCbk = redrawCbkA;
  redrawCbkData = redrawCbkDataA;

  // create text xObject
  text = new TextPage(gFalse);
}

AROSSplashOutputDev::~AROSSplashOutputDev() {
  delete text;
}

void AROSSplashOutputDev::drawChar(GfxState *state, double x, double y,
				  double dx, double dy,
				  double originX, double originY,
				  CharCode code, int nBytes, 
				  Unicode *u, int uLen) {
  text->addChar(state, x, y, dx, dy, code, nBytes, u, uLen);
  SplashOutputDev::drawChar(state, x, y, dx, dy, originX, originY,
			    code, nBytes, u, uLen);
}

GBool AROSSplashOutputDev::beginType3Char(GfxState *state, double x, double y,
					 double dx, double dy,
					 CharCode code, int nBytes,
					 Unicode *u, int uLen) {
  text->addChar(state, x, y, dx, dy, code, nBytes, u, uLen);
  return SplashOutputDev::beginType3Char(state, x, y, dx, dy, code, u, uLen);
} 

void AROSSplashOutputDev::clear() {
  startDoc(NULL);
  startPage(0, NULL);
}

void AROSSplashOutputDev::startPage(int pageNum, GfxState *state) {
  SplashOutputDev::startPage(pageNum, state);
  text->startPage(state);
}

void AROSSplashOutputDev::endPage() {
  SplashOutputDev::endPage();
  if (!incrementalUpdate) {
    (*redrawCbk)(redrawCbkData);
  }
  text->coalesce(gTrue,gTrue);
}

void AROSSplashOutputDev::dump() {
  if (incrementalUpdate) {
    (*redrawCbk)(redrawCbkData);
  }
}

void AROSSplashOutputDev::updateFont(GfxState *state) {
  SplashOutputDev::updateFont(state);
  text->updateFont(state);
}

//draw to our bitmap?
void AROSSplashOutputDev::redraw(int srcX, int srcY, int destX, int destY,
				int width, int height, int destW, int destH) {

  register int x, y;
  register int w, h; 
  int bw, r, g, b;
  Gulong pixel;
  SplashColorPtr dataPtr;
  Guchar *p;
  unsigned char rgb;
  w = width;
  h = height;

  bw = getBitmap()->getRowSize();
  dataPtr = getBitmap()->getDataPtr();
  SetAPen(visual,0);
  RectFill(visual,0,0,destW-1,destH-1);
  WritePixelArray(dataPtr,srcX,srcY,bw,visual,destX,destY,width-2,height-1,RECTFMT_RGB);
}

GBool AROSSplashOutputDev::findText(Unicode *s, int len,
				 GBool startAtTop, GBool stopAtBottom,
				 GBool startAtLast, GBool stopAtLast,
				 int *xMin, int *yMin,
				 int *xMax, int *yMax) {
//   double xMin1, yMin1, xMax1, yMax1;
  
//   xMin1 = (double)*xMin;
//   yMin1 = (double)*yMin;
//   xMax1 = (double)*xMax;
//   yMax1 = (double)*yMax;
//   if (text->findText(s, len, startAtTop, stopAtBottom,
// 		     startAtLast, stopAtLast,
// 		     &xMin1, &yMin1, &xMax1, &yMax1)) {
//     *xMin = xoutRound(xMin1);
//     *xMax = xoutRound(xMax1);
//     *yMin = xoutRound(yMin1);
//     *yMax = xoutRound(yMax1);
//     return gTrue;
//   }
  return gFalse;
}

GString *AROSSplashOutputDev::getText(int xMin, int yMin, int xMax, int yMax) {
  return text->getText((double)xMin, (double)yMin,
		       (double)xMax, (double)yMax);
}

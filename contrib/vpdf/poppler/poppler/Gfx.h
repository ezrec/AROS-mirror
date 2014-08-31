//========================================================================
//
// Gfx.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2005 Jonathan Blandford <jrb@redhat.com>
// Copyright (C) 2007 Iñigo Martínez <inigomartinez@gmail.com>
// Copyright (C) 2008 Brad Hards <bradh@kde.org>
// Copyright (C) 2008, 2010 Carlos Garcia Campos <carlosgc@gnome.org>
// Copyright (C) 2009-2013 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2009, 2010, 2012, 2013 Thomas Freitag <Thomas.Freitag@alfa.de>
// Copyright (C) 2010 David Benjamin <davidben@mit.edu>
// Copyright (C) 2010 Christian Feuersänger <cfeuersaenger@googlemail.com>
// Copyright (C) 2013 Fabio D'Urso <fabiodurso@hotmail.it>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef GFX_H
#define GFX_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "poppler-config.h"
#include "goo/gtypes.h"
#include "goo/GooList.h"
#include "GfxState.h"
#include "Object.h"
#include "PopplerCache.h"

#include <vector>

class GooString;
class PDFDoc;
class XRef;
class Array;
class Stream;
class Parser;
class Dict;
class Function;
class OutputDev;
class GfxFontDict;
class GfxFont;
class GfxPattern;
class GfxTilingPattern;
class GfxShadingPattern;
class GfxShading;
class GfxFunctionShading;
class GfxAxialShading;
class GfxRadialShading;
class GfxGouraudTriangleShading;
class GfxPatchMeshShading;
struct GfxPatch;
class GfxState;
struct GfxColor;
class GfxColorSpace;
class Gfx;
class PDFRectangle;
class AnnotBorder;
class AnnotColor;
class Catalog;
struct MarkedContentStack;

//------------------------------------------------------------------------

enum GfxClipType {
  clipNone,
  clipNormal,
  clipEO
};

enum TchkType {
  tchkBool,			// boolean
  tchkInt,			// integer
  tchkNum,			// number (integer or real)
  tchkString,			// string
  tchkName,			// name
  tchkArray,			// array
  tchkProps,			// properties (dictionary or name)
  tchkSCN,			// scn/SCN args (number of name)
  tchkNone			// used to avoid empty initializer lists
};

#define maxArgs 33

struct Operator {
  char name[4];
  int numArgs;
  TchkType tchk[maxArgs];
  void (Gfx::*func)(PObject args[], int numArgs);
};

//------------------------------------------------------------------------

class GfxResources {
public:

  GfxResources(XRef *xref, Dict *resDict, GfxResources *nextA);
  ~GfxResources();

  GfxFont *lookupFont(char *name);
  GBool lookupXObject(char *name, PObject *obj);
  GBool lookupXObjectNF(char *name, PObject *obj);
  GBool lookupMarkedContentNF(char *name, PObject *obj);
  void lookupColorSpace(const char *name, PObject *obj);
  GfxPattern *lookupPattern(char *name, OutputDev *out);
  GfxShading *lookupShading(char *name, OutputDev *out);
  GBool lookupGState(char *name, PObject *obj);
  GBool lookupGStateNF(char *name, PObject *obj);

  GfxResources *getNext() { return next; }

private:

  GfxFontDict *fonts;
  PObject xObjDict;
  PObject colorSpaceDict;
  PObject patternDict;
  PObject shadingDict;
  PObject gStateDict;
  PopplerObjectCache gStateCache;
  PObject propertiesDict;
  GfxResources *next;
};

//------------------------------------------------------------------------
// Gfx
//------------------------------------------------------------------------

class Gfx {
public:

  // Constructor for regular output.
  Gfx(PDFDoc *docA, OutputDev *outA, int pageNum, Dict *resDict,
      double hDPI, double vDPI, PDFRectangle *box,
      PDFRectangle *cropBox, int rotate,
      GBool (*abortCheckCbkA)(void *data) = NULL,
      void *abortCheckCbkDataA = NULL, XRef *xrefA = NULL);

  // Constructor for a sub-page object.
  Gfx(PDFDoc *docA, OutputDev *outA, Dict *resDict,
      PDFRectangle *box, PDFRectangle *cropBox,
      GBool (*abortCheckCbkA)(void *data) = NULL,
      void *abortCheckCbkDataA = NULL, XRef *xrefA = NULL);

  ~Gfx();

  XRef *getXRef() { return xref; }

  // Interpret a stream or array of streams.
  void display(PObject *obj, GBool topLevel = gTrue);

  // Display an annotation, given its appearance (a Form XObject),
  // border style, and bounding box (in default user space).
  void drawAnnot(PObject *str, AnnotBorder *border, AnnotColor *aColor,
		 double xMin, double yMin, double xMax, double yMax, int rotate);

  // Save graphics state.
  void saveState();

  // Push a new state guard
  void pushStateGuard();

  // Restore graphics state.
  void restoreState();

  // Pop to state guard and pop guard
  void popStateGuard();

  // Get the current graphics state object.
  GfxState *getState() { return state; }

  GBool checkTransparencyGroup(Dict *resDict);

  void drawForm(PObject *str, Dict *resDict, double *matrix, double *bbox,
	       GBool transpGroup = gFalse, GBool softMask = gFalse,
	       GfxColorSpace *blendingColorSpace = NULL,
	       GBool isolated = gFalse, GBool knockout = gFalse,
	       GBool alpha = gFalse, Function *transferFunc = NULL,
	       GfxColor *backdropColor = NULL);

  void pushResources(Dict *resDict);
  void popResources();

private:

  PDFDoc *doc;
  XRef *xref;			// the xref table for this PDF file
  Catalog *catalog;		// the Catalog for this PDF file  
  OutputDev *out;		// output device
  GBool subPage;		// is this a sub-page object?
  GBool printCommands;		// print the drawing commands (for debugging)
  GBool profileCommands;	// profile the drawing commands (for debugging)
  GBool commandAborted;         // did the previous command abort the drawing?
  GfxResources *res;		// resource stack
  int updateLevel;

  GfxState *state;		// current graphics state
  int stackHeight;		// the height of the current graphics stack
  std::vector<int> stateGuards;   // a stack of state limits; to guard against unmatched pops
  GBool fontChanged;		// set if font or text matrix has changed
  GfxClipType clip;		// do a clip?
  int ignoreUndef;		// current BX/EX nesting level
  double baseMatrix[6];		// default matrix for most recent
				//   page/form/pattern
  int formDepth;
  GBool ocState;		// true if drawing is enabled, false if
				//   disabled

  MarkedContentStack *mcStack;	// current BMC/EMC stack

  Parser *parser;		// parser for page content stream(s)
  
  std::set<int> formsDrawing;	// the forms that are being drawn

  GBool				// callback to check for an abort
    (*abortCheckCbk)(void *data);
  void *abortCheckCbkData;

  static Operator opTab[];	// table of operators

  void go(GBool topLevel);
  void execOp(PObject *cmd, PObject args[], int numArgs);
  Operator *findOp(char *name);
  GBool checkArg(PObject *arg, TchkType type);
  Goffset getPos();

  int bottomGuard();

  // graphics state operators
  void opSave(PObject args[], int numArgs);
  void opRestore(PObject args[], int numArgs);
  void opConcat(PObject args[], int numArgs);
  void opSetDash(PObject args[], int numArgs);
  void opSetFlat(PObject args[], int numArgs);
  void opSetLineJoin(PObject args[], int numArgs);
  void opSetLineCap(PObject args[], int numArgs);
  void opSetMiterLimit(PObject args[], int numArgs);
  void opSetLineWidth(PObject args[], int numArgs);
  void opSetExtGState(PObject args[], int numArgs);
  void doSoftMask(PObject *str, GBool alpha,
		  GfxColorSpace *blendingColorSpace,
		  GBool isolated, GBool knockout,
		  Function *transferFunc, GfxColor *backdropColor);
  void opSetRenderingIntent(PObject args[], int numArgs);

  // color operators
  void opSetFillGray(PObject args[], int numArgs);
  void opSetStrokeGray(PObject args[], int numArgs);
  void opSetFillCMYKColor(PObject args[], int numArgs);
  void opSetStrokeCMYKColor(PObject args[], int numArgs);
  void opSetFillRGBColor(PObject args[], int numArgs);
  void opSetStrokeRGBColor(PObject args[], int numArgs);
  void opSetFillColorSpace(PObject args[], int numArgs);
  void opSetStrokeColorSpace(PObject args[], int numArgs);
  void opSetFillColor(PObject args[], int numArgs);
  void opSetStrokeColor(PObject args[], int numArgs);
  void opSetFillColorN(PObject args[], int numArgs);
  void opSetStrokeColorN(PObject args[], int numArgs);

  // path segment operators
  void opMoveTo(PObject args[], int numArgs);
  void opLineTo(PObject args[], int numArgs);
  void opCurveTo(PObject args[], int numArgs);
  void opCurveTo1(PObject args[], int numArgs);
  void opCurveTo2(PObject args[], int numArgs);
  void opRectangle(PObject args[], int numArgs);
  void opClosePath(PObject args[], int numArgs);

  // path painting operators
  void opEndPath(PObject args[], int numArgs);
  void opStroke(PObject args[], int numArgs);
  void opCloseStroke(PObject args[], int numArgs);
  void opFill(PObject args[], int numArgs);
  void opEOFill(PObject args[], int numArgs);
  void opFillStroke(PObject args[], int numArgs);
  void opCloseFillStroke(PObject args[], int numArgs);
  void opEOFillStroke(PObject args[], int numArgs);
  void opCloseEOFillStroke(PObject args[], int numArgs);
  void doPatternFill(GBool eoFill);
  void doPatternStroke();
  void doPatternText();
  void doPatternImageMask(PObject *ref, Stream *str, int width, int height,
			  GBool invert, GBool inlineImg);
  void doTilingPatternFill(GfxTilingPattern *tPat,
			   GBool stroke, GBool eoFill, GBool text);
  void doShadingPatternFill(GfxShadingPattern *sPat,
			    GBool stroke, GBool eoFill, GBool text);
  void opShFill(PObject args[], int numArgs);
  void doFunctionShFill(GfxFunctionShading *shading);
  void doFunctionShFill1(GfxFunctionShading *shading,
			 double x0, double y0,
			 double x1, double y1,
			 GfxColor *colors, int depth);
  void doAxialShFill(GfxAxialShading *shading);
  void doRadialShFill(GfxRadialShading *shading);
  void doGouraudTriangleShFill(GfxGouraudTriangleShading *shading);
  void gouraudFillTriangle(double x0, double y0, GfxColor *color0,
			   double x1, double y1, GfxColor *color1,
			   double x2, double y2, GfxColor *color2,
			   int nComps, int depth, GfxState::ReusablePathIterator *path);
  void gouraudFillTriangle(double x0, double y0, double color0,
			   double x1, double y1, double color1,
			   double x2, double y2, double color2,
			   double refineColorThreshold, int depth, GfxGouraudTriangleShading *shading, GfxState::ReusablePathIterator *path);
  void doPatchMeshShFill(GfxPatchMeshShading *shading);
  void fillPatch(GfxPatch *patch, int colorComps, int patchColorComps, double refineColorThreshold, int depth, GfxPatchMeshShading *shading);
  void doEndPath();

  // path clipping operators
  void opClip(PObject args[], int numArgs);
  void opEOClip(PObject args[], int numArgs);

  // text object operators
  void opBeginText(PObject args[], int numArgs);
  void opEndText(PObject args[], int numArgs);

  // text state operators
  void opSetCharSpacing(PObject args[], int numArgs);
  void opSetFont(PObject args[], int numArgs);
  void opSetTextLeading(PObject args[], int numArgs);
  void opSetTextRender(PObject args[], int numArgs);
  void opSetTextRise(PObject args[], int numArgs);
  void opSetWordSpacing(PObject args[], int numArgs);
  void opSetHorizScaling(PObject args[], int numArgs);

  // text positioning operators
  void opTextMove(PObject args[], int numArgs);
  void opTextMoveSet(PObject args[], int numArgs);
  void opSetTextMatrix(PObject args[], int numArgs);
  void opTextNextLine(PObject args[], int numArgs);

  // text string operators
  void opShowText(PObject args[], int numArgs);
  void opMoveShowText(PObject args[], int numArgs);
  void opMoveSetShowText(PObject args[], int numArgs);
  void opShowSpaceText(PObject args[], int numArgs);
  void doShowText(GooString *s);
  void doIncCharCount(GooString *s);

  // XObject operators
  void opXObject(PObject args[], int numArgs);
  void doImage(PObject *ref, Stream *str, GBool inlineImg);
  void doForm(PObject *str);

  // in-line image operators
  void opBeginImage(PObject args[], int numArgs);
  Stream *buildImageStream();
  void opImageData(PObject args[], int numArgs);
  void opEndImage(PObject args[], int numArgs);

  // type 3 font operators
  void opSetCharWidth(PObject args[], int numArgs);
  void opSetCacheDevice(PObject args[], int numArgs);

  // compatibility operators
  void opBeginIgnoreUndef(PObject args[], int numArgs);
  void opEndIgnoreUndef(PObject args[], int numArgs);

  // marked content operators
  void opBeginMarkedContent(PObject args[], int numArgs);
  void opEndMarkedContent(PObject args[], int numArgs);
  void opMarkPoint(PObject args[], int numArgs);
  GfxState *saveStateStack();
  void restoreStateStack(GfxState *oldState);
  GBool contentIsHidden();
  void pushMarkedContent();
  void popMarkedContent();
};

#endif

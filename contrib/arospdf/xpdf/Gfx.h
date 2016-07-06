//========================================================================
//
// Gfx.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef GFX_H
#define GFX_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"

class GString;
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
class AnnotBorderStyle;

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
  void (Gfx::*func)(xObject args[], int numArgs);
};

//------------------------------------------------------------------------

class GfxResources {
public:

  GfxResources(XRef *xref, Dict *resDict, GfxResources *nextA);
  ~GfxResources();

  GfxFont *lookupFont(char *name);
  GBool lookupXObject(char *name, xObject *obj);
  GBool lookupXObjectNF(char *name, xObject *obj);
  void lookupColorSpace(char *name, xObject *obj);
  GfxPattern *lookupPattern(char *name);
  GfxShading *lookupShading(char *name);
  GBool lookupGState(char *name, xObject *obj);

  GfxResources *getNext() { return next; }

private:

  GfxFontDict *fonts;
  xObject xObjDict;
  xObject colorSpaceDict;
  xObject patternDict;
  xObject shadingDict;
  xObject gStateDict;
  GfxResources *next;
};

//------------------------------------------------------------------------
// Gfx
//------------------------------------------------------------------------

class Gfx {
public:

  // Constructor for regular output.
  Gfx(XRef *xrefA, OutputDev *outA, int pageNum, Dict *resDict,
      double hDPI, double vDPI, PDFRectangle *box,
      PDFRectangle *cropBox, int rotate,
      GBool (*abortCheckCbkA)(void *data) = NULL,
      void *abortCheckCbkDataA = NULL);

  // Constructor for a sub-page xObject.
  Gfx(XRef *xrefA, OutputDev *outA, Dict *resDict,
      PDFRectangle *box, PDFRectangle *cropBox,
      GBool (*abortCheckCbkA)(void *data) = NULL,
      void *abortCheckCbkDataA = NULL);

  ~Gfx();

  // Interpret a stream or array of streams.
  void display(xObject *obj, GBool topLevel = gTrue);

  // Display an annotation, given its appearance (a Form XObject),
  // border style, and bounding box (in default user space).
  void drawAnnot(xObject *str, AnnotBorderStyle *borderStyle,
		 double xMin, double yMin, double xMax, double yMax);

  // Save graphics state.
  void saveState();

  // Restore graphics state.
  void restoreState();

  // Get the current graphics state xObject.
  GfxState *getState() { return state; }

private:

  XRef *xref;			// the xref table for this PDF file
  OutputDev *out;		// output device
  GBool subPage;		// is this a sub-page xObject?
  GBool printCommands;		// print the drawing commands (for debugging)
  GfxResources *res;		// resource stack
  int updateLevel;

  GfxState *state;		// current graphics state
  GBool fontChanged;		// set if font or text matrix has changed
  GfxClipType clip;		// do a clip?
  int ignoreUndef;		// current BX/EX nesting level
  double baseMatrix[6];		// default matrix for most recent
				//   page/form/pattern
  int formDepth;

  Parser *parser;		// parser for page content stream(s)

  GBool				// callback to check for an abort
    (*abortCheckCbk)(void *data);
  void *abortCheckCbkData;

  static Operator opTab[];	// table of operators

  void go(GBool topLevel);
  void execOp(xObject *cmd, xObject args[], int numArgs);
  Operator *findOp(char *name);
  GBool checkArg(xObject *arg, TchkType type);
  int getPos();

  // graphics state operators
  void opSave(xObject args[], int numArgs);
  void opRestore(xObject args[], int numArgs);
  void opConcat(xObject args[], int numArgs);
  void opSetDash(xObject args[], int numArgs);
  void opSetFlat(xObject args[], int numArgs);
  void opSetLineJoin(xObject args[], int numArgs);
  void opSetLineCap(xObject args[], int numArgs);
  void opSetMiterLimit(xObject args[], int numArgs);
  void opSetLineWidth(xObject args[], int numArgs);
  void opSetExtGState(xObject args[], int numArgs);
  void doSoftMask(xObject *str, GBool alpha,
		  GfxColorSpace *blendingColorSpace,
		  GBool isolated, GBool knockout,
		  Function *transferFunc, GfxColor *backdropColor);
  void opSetRenderingIntent(xObject args[], int numArgs);

  // color operators
  void opSetFillGray(xObject args[], int numArgs);
  void opSetStrokeGray(xObject args[], int numArgs);
  void opSetFillCMYKColor(xObject args[], int numArgs);
  void opSetStrokeCMYKColor(xObject args[], int numArgs);
  void opSetFillRGBColor(xObject args[], int numArgs);
  void opSetStrokeRGBColor(xObject args[], int numArgs);
  void opSetFillColorSpace(xObject args[], int numArgs);
  void opSetStrokeColorSpace(xObject args[], int numArgs);
  void opSetFillColor(xObject args[], int numArgs);
  void opSetStrokeColor(xObject args[], int numArgs);
  void opSetFillColorN(xObject args[], int numArgs);
  void opSetStrokeColorN(xObject args[], int numArgs);

  // path segment operators
  void opMoveTo(xObject args[], int numArgs);
  void opLineTo(xObject args[], int numArgs);
  void opCurveTo(xObject args[], int numArgs);
  void opCurveTo1(xObject args[], int numArgs);
  void opCurveTo2(xObject args[], int numArgs);
  void opRectangle(xObject args[], int numArgs);
  void opClosePath(xObject args[], int numArgs);

  // path painting operators
  void opEndPath(xObject args[], int numArgs);
  void opStroke(xObject args[], int numArgs);
  void opCloseStroke(xObject args[], int numArgs);
  void opFill(xObject args[], int numArgs);
  void opEOFill(xObject args[], int numArgs);
  void opFillStroke(xObject args[], int numArgs);
  void opCloseFillStroke(xObject args[], int numArgs);
  void opEOFillStroke(xObject args[], int numArgs);
  void opCloseEOFillStroke(xObject args[], int numArgs);
  void doPatternFill(GBool eoFill);
  void doPatternStroke();
  void doTilingPatternFill(GfxTilingPattern *tPat,
			   GBool stroke, GBool eoFill);
  void doShadingPatternFill(GfxShadingPattern *sPat,
			    GBool stroke, GBool eoFill);
  void opShFill(xObject args[], int numArgs);
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
			   int nComps, int depth);
  void doPatchMeshShFill(GfxPatchMeshShading *shading);
  void fillPatch(GfxPatch *patch, int nComps, int depth);
  void doEndPath();

  // path clipping operators
  void opClip(xObject args[], int numArgs);
  void opEOClip(xObject args[], int numArgs);

  // text xObject operators
  void opBeginText(xObject args[], int numArgs);
  void opEndText(xObject args[], int numArgs);

  // text state operators
  void opSetCharSpacing(xObject args[], int numArgs);
  void opSetFont(xObject args[], int numArgs);
  void opSetTextLeading(xObject args[], int numArgs);
  void opSetTextRender(xObject args[], int numArgs);
  void opSetTextRise(xObject args[], int numArgs);
  void opSetWordSpacing(xObject args[], int numArgs);
  void opSetHorizScaling(xObject args[], int numArgs);

  // text positioning operators
  void opTextMove(xObject args[], int numArgs);
  void opTextMoveSet(xObject args[], int numArgs);
  void opSetTextMatrix(xObject args[], int numArgs);
  void opTextNextLine(xObject args[], int numArgs);

  // text string operators
  void opShowText(xObject args[], int numArgs);
  void opMoveShowText(xObject args[], int numArgs);
  void opMoveSetShowText(xObject args[], int numArgs);
  void opShowSpaceText(xObject args[], int numArgs);
  void doShowText(GString *s);

  // XObject operators
  void opXObject(xObject args[], int numArgs);
  void doImage(xObject *ref, Stream *str, GBool inlineImg);
  void doForm(xObject *str);
  void doForm1(xObject *str, Dict *resDict, double *matrix, double *bbox,
	       GBool transpGroup = gFalse, GBool softMask = gFalse,
	       GfxColorSpace *blendingColorSpace = NULL,
	       GBool isolated = gFalse, GBool knockout = gFalse,
	       GBool alpha = gFalse, Function *transferFunc = NULL,
	       GfxColor *backdropColor = NULL);

  // in-line image operators
  void opBeginImage(xObject args[], int numArgs);
  Stream *buildImageStream();
  void opImageData(xObject args[], int numArgs);
  void opEndImage(xObject args[], int numArgs);

  // type 3 font operators
  void opSetCharWidth(xObject args[], int numArgs);
  void opSetCacheDevice(xObject args[], int numArgs);

  // compatibility operators
  void opBeginIgnoreUndef(xObject args[], int numArgs);
  void opEndIgnoreUndef(xObject args[], int numArgs);

  // marked content operators
  void opBeginMarkedContent(xObject args[], int numArgs);
  void opEndMarkedContent(xObject args[], int numArgs);
  void opMarkPoint(xObject args[], int numArgs);

  void pushResources(Dict *resDict);
  void popResources();
};

#endif

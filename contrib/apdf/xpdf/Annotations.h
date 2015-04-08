//========================================================================
//
// Annotations.h
//
// Copyright 2000-2005 Emmanuel Lesueur
//
//========================================================================

#ifndef ANNOTATIONS_H
#define ANNOTATIONS_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Object.h"
#include "Action.h"

class GfxColor;
class AcroForm;
class Field;
class Annot;

//------------------------------------------------------------------------
// Annot
//------------------------------------------------------------------------

const int annotInvisible = 1 << 0;
const int annotHidden    = 1 << 1;
const int annotPrint     = 1 << 2;
const int annotNoZoom    = 1 << 3;
const int annotNoRotate  = 1 << 4;
const int annotNoView    = 1 << 5;
const int annotReadOnly  = 1 << 6;

enum AnnotKind {
  annotText,
  annotLink,
  annotMovie,
  annotSound,
  annotFreeText,
  annotRubberStamp,
  annotLine,
  annotSquare,
  annotCircle,
  annotStrikeOut,
  annotHighlight,
  annotUnderline,
  annotInk,
  annotFileAttachment,
  annotPopup,
  annotTrapNet,
  annotWidget,
  annotUnknown
};

enum AnnotHighlight {
  highlightInvert,
  highlightNone,
  highlightOutline,
  highlightPush
};

/*enum AnnotBorder {
  borderNone,
  borderSolid,
  borderDashed,
  borderBeveled,
  borderInset,
  borderUnderlined
};*/

enum AnnotAppearance {
  appearanceNormal,
  appearanceRollover,
  appearanceDown
};

class AnnotState {
  friend class Annot;
public:

  AnnotState(): setFlags(0), clrFlags(0), additionalAction(NULL) {}
  ~AnnotState();

  // Import from an FDF dictionary.
  void import(Dict *);

private:
  AnnotState(const AnnotState&);
  AnnotState& operator = (const AnnotState&);

  int setFlags;
  int clrFlags;
  Object appearance[3];
  Object appearanceState;
  AdditionalAction *additionalAction;
};


class Annot {
public:

  // Constructor.
  Annot(XRef *, Object *);

  // Destructor.
  virtual ~Annot();

  // Check annotation type.
  virtual AnnotKind getKind() { return annotUnknown; }

  // Get the annotation description string.
  virtual char *getDescr() { return NULL; }

  // Was the Annot created successfully?
  GBool isOk() { return ok; }

  // Get annotation box.
  void getRect(double *xa1, double *ya1, double *xa2, double *ya2)
    { *xa1 = x1; *ya1 = y1; *xa2 = x2; *ya2 = y2; }

  // Check if point is inside the link rectangle.
  //GBool inRect(double x, double y)
  //  { return x1 <= x && x <= x2 && y1 <= y && y <= y2; }

  // Is the annotation invisible?
  GBool isInvisible() { return flags & annotInvisible; }

  // Is the annotation hidden?
  GBool isHidden() { return flags & annotHidden; }

  // Is the annotation read-only?
  GBool isReadOnly() { return flags & annotReadOnly; }

  // Get the highlight mode.
  AnnotHighlight getHighlightMode() { return highlightMode; }

  // Get the annotation appearance.
  Object *getAppearance(XRef *xref, AnnotAppearance a, Object *obj);
  Object *getAppearance(XRef *xref, Object *obj) {
    return getAppearance(xref, curAppearance, obj);
  }
  Dict *getAppearanceDict() {
    return appearance[appearanceNormal].isDict() ?
	      appearance[appearanceNormal].getDict() : NULL;
  }

  // Get the annotation border.
  Object *getBorder(Object *obj) { return border.copy(obj); }

  // Get the appearance state.
  char *getAppearanceState();

  // Get the additional action.
  AdditionalAction *getAdditionalAction() { return additionalAction; }

  // Set the additional action.
  //void setAdditionalAction(AdditionalAction *);

  // Add an action.
  void addAdditionalAction(ActionTrigger, Action *);

  // Set the appearance state.
  GBool setAppearance(AnnotAppearance a) {
    if (curAppearance == a)
      return gFalse;
    curAppearance = a;
    return gTrue;
  }
  void setAppearance(AnnotAppearance a, Object *);

  // Set the appearance state.
  void setAppearanceState(char *);

  // Change the hidden flag.
  void hide(GBool f) { if (f) flags |= annotHidden; else flags &= ~annotHidden; }

  // Check if an annotation has a specific appearance
  GBool hasAppearance(AnnotAppearance);

  // Build an appearance stream.
  Object *makeAppearanceStream(GString *, Object *);

  // Remove the border.
  void noBorder() { border.free(); border.initNull(); }

  // Get the annotation title.
  GString *getTitle() { return title; }

  // Save the annotation state.
  void saveState(AnnotState *);

  // Restore the annotation state.
  void restoreState(AnnotState *);

private:

  double x1, y1;                // lower left corner
  double x2, y2;                // upper right corner
  GfxColor *color;              // annotation color
  GString *title;               // annotation title
  int flags;
  AnnotHighlight highlightMode; // highlight mode
  Object appearance[3];         // appearances
  Object appearanceState;
  Object border;
  AnnotAppearance curAppearance;// current appearance
  AdditionalAction *additionalAction;
  //~ Dict *popup;
  //~ Dict *page;
  GBool ok;                     // is link valid?
};


//------------------------------------------------------------------------
// RawTextAnnot
//------------------------------------------------------------------------

class RawTextAnnot : public Annot {
public:

  // Constructor.
  RawTextAnnot(XRef *, Object *);

  // Destructor.
  virtual ~RawTextAnnot();

  // Accessors.
  virtual AnnotKind getKind() { return annotUnknown; }
  GString *getContents() { return contents; }

private:
  GString *contents;
  GBool open;
};

//------------------------------------------------------------------------
// TextAnnot
//------------------------------------------------------------------------

class TextAnnot : public RawTextAnnot {
public:

  // Constructor.
  TextAnnot(XRef *, Object *);

  // Accessors.
  virtual AnnotKind getKind() { return annotText; }
};


//------------------------------------------------------------------------
// LineAnnot
//------------------------------------------------------------------------

class LineAnnot : public RawTextAnnot {
public:

  // Constructor.
  LineAnnot(XRef *, Object *);

  // Accessor.
  virtual AnnotKind getKind() { return annotLine; }
};


//------------------------------------------------------------------------
// SquareAnnot
//------------------------------------------------------------------------

class SquareAnnot : public RawTextAnnot {
public:

  // Constructor.
  SquareAnnot(XRef *, Object *);

  // Accessor.
  virtual AnnotKind getKind() { return annotSquare; }
};


//------------------------------------------------------------------------
// CircleAnnot
//------------------------------------------------------------------------

class CircleAnnot : public RawTextAnnot {
public:

  // Constructor.
  CircleAnnot(XRef *, Object *);

  // Accessor.
  virtual AnnotKind getKind() { return annotCircle; }
};


//------------------------------------------------------------------------
// LinkAnnot
//------------------------------------------------------------------------

class LinkAnnot : public Annot {
public:

  // Constructor.
  LinkAnnot(XRef *, Object *);

  // Destructor.
  virtual ~LinkAnnot();

  // Accessors.
  virtual AnnotKind getKind() { return annotLink; }

  // Get the description string.
  virtual char *getDescr() { return action ? action->getDescr() : NULL; }

  // Get action.
  Action *getAction() { return action; }

private:
  Action *action;
};


//------------------------------------------------------------------------
// WidgetAnnot
//------------------------------------------------------------------------

class WidgetAnnot : public Annot {
public:

  WidgetAnnot(XRef *, Object *, Field *);

  // Destructor.
  virtual ~WidgetAnnot();

  // Accessors.
  virtual AnnotKind getKind() { return annotWidget; }

private:
  Field *field;
};


//------------------------------------------------------------------------
// Annots
//------------------------------------------------------------------------

class Annots {
public:

  // Read the array of annotations.
  Annots(XRef *xref, Object *annots, AcroForm *form);

  // Destructor.
  ~Annots();

  // Iterate through list of links.
  int getNumAnnots() { return numAnnots; }
  Annot *getAnnot(int i) { return annots[i]; }

  // If point <x>,<y> is on an annotation, return it,
  // else return NULL.
  Annot *find(double x, double y);

  // Return true if <x>,<y> is on an annotation.
  GBool onAnnot(double x, double y);

private:

  Annot **annots;
  int numAnnots;
};

#endif


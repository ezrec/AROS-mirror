//========================================================================
//
// Annotations.cc
//
// Copyright 2000-2005 Emmanuel Lesueur
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stddef.h>
#include <string.h>
#include "gmem.h"
#include "GString.h"
#include "Error.h"
#include "Object.h"
#include "Array.h"
#include "Dict.h"
#include "AcroForm.h"
#include "Annotations.h"
#include "Action.h"
#include "GfxState.h"


//------------------------------------------------------------------------
// AnnotState
//------------------------------------------------------------------------

AnnotState::~AnnotState() {
  for (int k = 0; k < 3; ++k)
    appearance[k].free();
  appearanceState.free();
  delete additionalAction;
}

void AnnotState::import(Dict *dict) { DEBUG_INFO
  Object obj, obj2;

  if (dict->lookup("F", &obj)->isInt()) {
    setFlags = obj.getInt();
    clrFlags = ~setFlags;
  } else {
    obj.free();
    if (dict->lookup("SetF", &obj)->isInt())
      setFlags |= obj.getInt();
    obj.free();
    if (dict->lookup("ClrF", &obj)->isInt())
      clrFlags |= obj.getInt();
  }
  obj.free();

  if (dict->lookup("AP", &obj)->isDict()) {
    if (obj.dictLookup("N", &obj2)->isStream()) {
      appearance[appearanceNormal].free();
      obj2.copy(&appearance[appearanceNormal]);
    }
    obj2.free();
    if (obj.dictLookup("R", &obj2)->isStream()) {
      appearance[appearanceRollover].free();
      obj2.copy(&appearance[appearanceRollover]);
    }
    obj2.free();
    if (obj.dictLookup("D", &obj2)->isStream()) {
      appearance[appearanceDown].free();
      obj2.copy(&appearance[appearanceDown]);
    }
    obj2.free();
  }
  obj.free();

  if (dict->lookup("AS", &obj)->isName()) {
    appearanceState.free();
    obj.copy(&appearanceState);
  }
  obj.free();

  if (dict->lookup("AA", &obj)->isDict()) {
    if (AdditionalAction *aa = new AdditionalAction(obj.getDict())) {
      delete additionalAction;
      additionalAction = aa;
    }
  }
  obj.free();

  if (dict->lookup("A", &obj)->isDict()) {
    if (Action *action = Action::makeAction(&obj)) {
      AdditionalAction *aa = new AdditionalAction(NULL);
      aa->setAction(trigUp, action);
      delete additionalAction;
      additionalAction = aa;
    }
  }
  obj.free();
}


//------------------------------------------------------------------------
// Annot
//------------------------------------------------------------------------

Annot::Annot(XRef *xref, Object *dict) { DEBUG_INFO
  Object obj1, obj2, obj3, obj4;
  double xa1, ya1, xa2, ya2;
  double borderW = 1;
  GString *borderStr = NULL;
  double r = 0, g = 0, b = 1;

  color = NULL;
  title = NULL;
  flags = 0;
  curAppearance = appearanceNormal;
  additionalAction = NULL;
  border.initNull();
  ok = gFalse;

  // get rectangle
  if (!dict->dictLookup("Rect", &obj1)->isArray()) {
    error(-1, "Annotation rectangle is wrong type");
    goto err2;
  }
  if (!obj1.arrayGet(0, &obj2)->isNum()) {
    error(-1, "Bad annotation rectangle");
    goto err1;
  }
  xa1 = obj2.getNum();
  obj2.free();
  if (!obj1.arrayGet(1, &obj2)->isNum()) {
    error(-1, "Bad annotation rectangle");
    goto err1;
  }
  ya1 = obj2.getNum();
  obj2.free();
  if (!obj1.arrayGet(2, &obj2)->isNum()) {
    error(-1, "Bad annotation rectangle");
    goto err1;
  }
  xa2 = obj2.getNum();
  obj2.free();
  if (!obj1.arrayGet(3, &obj2)->isNum()) {
    error(-1, "Bad annotation rectangle");
    goto err1;
  }
  ya2 = obj2.getNum();
  obj2.free();
  obj1.free();
  if (xa1 > xa2) {
    x1 = xa2;
    x2 = xa1;
  } else {
    x1 = xa1;
    x2 = xa2;
  }
  if (ya1 > ya2) {
    y1 = ya2;
    y2 = ya1;
  } else {
    y1 = ya1;
    y2 = ya2;
  }


  // get color
  /*if (!dict->dictLookup("C", &obj1)->isNull()) {
    if (obj1.isArray()) {
      color = new GfxColor;
      if (obj1.arrayGet(0, &obj2)->isNum())
	r = obj2.getNum();
      else
	error(-1, "Bad annotation color");
      obj2.free();
      if (obj1.arrayGet(1, &obj2)->isNum())
	g = obj2.getNum();
      else
	error(-1, "Bad annotation color");
      obj2.free();
      if (obj1.arrayGet(2, &obj2)->isNum())
	b = obj2.getNum();
      else
	error(-1, "Bad annotation color");
      obj2.free();
      color->setRGB(r, g, b);

      if (!dict->dictLookup("Subtype", &obj2)->isName("Link")) {
	r = 0; g = 0; b = 1;
      }
      obj2.free();

    } else
      error(-1, "Bad annotation color");
  }
  obj1.free(); */

  // get title.
  //~ For widgets, /T is the field name...
  if (getKind() != annotWidget &&
      !dict->dictLookup("T", &obj1)->isNull()) {
    if (obj1.isString())
      title = obj1.getString()->copy();
    else
      error(-1, "Bad annotation title");
  }
  obj1.free();

  // get flags
  if (!dict->dictLookup("F", &obj1)->isNull()) {
    if (obj1.isInt())
      flags = obj1.getInt();
    else
      error(-1, "Bad annotation flags");
  }
  obj1.free();

  // get highlight mode
  highlightMode = highlightInvert;
  if (!dict->dictLookup("H", &obj1)->isNull()) {
    if (obj1.isName("I"))
      highlightMode = highlightInvert;
    else if (obj1.isName("N"))
      highlightMode = highlightNone;
    else if (obj1.isName("O"))
      highlightMode = highlightOutline;
    else if (obj1.isName("P"))
      highlightMode = highlightPush;
    else
      error(-1, "Bad annotation highlight");
  }
  obj1.free();

  // get border style
  if (!dict->dictLookup("BS", &obj1)->isNull()) {
    if (obj1.isDict()) {
      if (!obj1.dictLookup("W", &obj2)->isNull()) {
	if (obj2.isNum())
	  borderW = obj2.getNum();
	else
	  error(-1, "Bad annotation border style");
      }
      obj2.free();

      if (borderW > 0) {
	char buf[128];
	GBool hasDashes = gFalse;
	int len;

	len = sprintf(buf, "q %f w 2 J %f %f %f RG", borderW, r, g, b);
	borderStr = new GString(buf, len);

	if (obj1.dictLookup("D", &obj2)->isArray()) {
	  int i;
	  int n = obj2.arrayGetLength();

	  hasDashes = gTrue;
	  borderStr->append('[');
	  for (i = 0; i < n; ++i) {
	    if (obj2.arrayGet(i, &obj3)->isNum()) {
	      len = sprintf(buf, " %f", obj3.getNum());
	      borderStr->append(buf, len);
	    } else
	      error(-1, "Bad border dashes array");
	    obj3.free();
	  }
	  borderStr->append("]0 d", 2);
	}
	obj2.free();

	xa1 = borderW / 2;
	ya2 = borderW / 2;
	xa2 = x2 - x1 - borderW / 2;
	ya1 = y2 - y1 - borderW / 2;

	len = 0;
	if (!obj1.dictLookup("S", &obj2)->isNull()) {
	  if (obj2.isName("D")) {
	    len = sprintf(buf, "%s %f %f %f %f re S",
			  hasDashes ? "" : " [3]0 d",
			  xa1, ya1, xa2 - xa1, ya2 - ya1);
	  } else if (obj2.isName("B")) {
	    len = sprintf(buf, " 0 G %f %f m %f %f l %f %f l S "
			       "0.7 G %f %f m %f %f l %f %f l S",
			       xa1, ya2, xa2, ya2, xa2, ya1,
			       xa2, ya1, xa1, ya1, xa1, ya2);
	  } else if (obj2.isName("I")) {
	    len = sprintf(buf, " 0 G %f %f m %f %f l %f %f l S "
			       "0.7 G %f %f m %f %f l %f %f l S",
			       xa2, ya1, xa1, ya1, xa1, ya2,
			       xa1, ya2, xa2, ya2, xa2, ya1);
	  } else if (obj2.isName("U")) {
	    len = sprintf(buf, " %f %f m %f %f l S",
			  xa1, ya2, xa2, ya2);
	  } else if (!obj2.isName("S"))
	    error(-1, "Bad annotation border style");
	}
	obj2.free();

	if (len == 0)
	  len = sprintf(buf, " %f %f %f %f re S",
			xa1, ya1, xa2 - xa1, ya2 - ya1);
	borderStr->append(buf, len);
      }
    } else
      error(-1, "Bad annotation border style");
  }
  obj1.free();

  // get border
  if (!borderStr && !dict->dictLookup("Border", &obj1)->isNull()) {
    if (obj1.isArray() && obj1.arrayGet(2, &obj2)->isNum()) {
      borderW = obj2.getNum();
      if (borderW > 0) {
	char buf[64];
	int len = sprintf(buf, "q %f w %f %f %f RG ", borderW, r, g, b);
	borderStr = new GString(buf, len);

	if (obj1.arrayGetLength() > 3 &&
	    obj1.arrayGet(3, &obj4)->isArray()) {
	  int i;
	  int n = obj4.arrayGetLength();

	  borderStr->append('[');
	  for (i = 0; i < n; ++i) {
	    if (obj4.arrayGet(i, &obj3)->isNum()) {
	      len = sprintf(buf, " %f", obj3.getNum());
	      borderStr->append(buf, len);
	    } else
	      error(-1, "Bad border dashes array");
	    obj3.free();
	  }
	  borderStr->append("]0 d ", 2);
	}
	obj4.free();

	xa1 = borderW / 2;
	ya1 = borderW / 2;
	xa2 = x2 - x1 - borderW / 2;
	ya2 = y2 - y1 - borderW / 2;

	len = sprintf(buf, "%f %f %f %f re S",
		      xa1, ya1, xa2 - xa1, ya2 - ya1);
	borderStr->append(buf, len);
      }
    } else
      error(-1, "Bad annotation border");
    obj2.free();
  }
  obj1.free();

  if (borderStr) {
    borderStr->append(" Q");
    makeAppearanceStream(borderStr, &border);
  }

  // get additional action
  if (!dict->dictLookup("AA", &obj1)->isNull()) {
    if (obj1.isDict())
      additionalAction = new AdditionalAction(obj1.getDict());
    else
      error(-1, "Bad annotation additional action");
  }
  obj1.free();

  // A and Dest keys replace the up entry of AA
  if (!dict->dictLookup("A", &obj1)->isNull()) {
    if (obj1.isDict()) {
      Action *action = Action::makeAction(&obj1);
      if (action)
	addAdditionalAction(trigUp, action);
    } else
      error(-1, "Bad annotation action");
  }
  obj1.free();

  if (!dict->dictLookup("Dest", &obj1)->isNull()) {
    Action *action = new ActionGoTo(new Destination(&obj1));
    if (action && action->isOk())
      addAdditionalAction(trigUp, action);
    else
      delete action;
  }
  obj1.free();

  // get appearance
  appearance[appearanceNormal].initNull();
  appearance[appearanceRollover].initNull();
  appearance[appearanceDown].initNull();
  if (dict->dictLookup("AP", &obj1)->isDict()) {
    obj1.dictLookupNF("N", &appearance[appearanceNormal]);
    obj1.dictLookupNF("R", &appearance[appearanceRollover]);
    obj1.dictLookupNF("D", &appearance[appearanceDown]);
  }
  obj1.free();

  // get appearance state
  if (!dict->dictLookup("AS", &appearanceState)->isNull()) {
    if (!appearanceState.isName()) {
      error(-1, "Bad annotation appearance state");
      appearanceState.free();
      appearanceState.initNull();
    }
  }

  //~ popup
  //~ page

  ok = gTrue;

  return;

 err1:
  obj2.free();
 err2:
  obj1.free();
}

Annot::~Annot() {
  delete color;
  delete title;
  delete additionalAction;
  appearanceState.free();
  appearance[0].free();
  appearance[1].free();
  appearance[2].free();
  border.free();
}

void Annot::saveState(AnnotState *state) { DEBUG_INFO
  state->setFlags = flags;
  state->clrFlags = ~flags;
  for (int k = 0; k < 3; ++k) {
    Object obj;
    obj = state->appearance[k];
    state->appearance[k] = appearance[k];
    appearance[k] = obj;
  }
  AdditionalAction *aa = state->additionalAction;
  state->additionalAction = additionalAction;
  additionalAction = aa;
  Object obj(state->appearanceState);
  state->appearanceState = appearanceState;
  appearanceState = obj;
}

void Annot::restoreState(AnnotState *state) { DEBUG_INFO
  flags |= state->setFlags;
  flags &= ~state->clrFlags;
  for (int k = 0; k < 3; ++k) {
    if (state->appearance[k].isStream()) {
      appearance[k].free();
      appearance[k] = state->appearance[k];
      state->appearance[k].clear();
    }
  }
  if (state->additionalAction) {
    delete additionalAction;
    additionalAction = state->additionalAction;
    state->additionalAction = NULL;
  }
  if (state->appearanceState.isName()) {
    appearanceState.free();
    appearanceState = state->appearanceState;
    state->appearanceState.clear();
  }
}

Object *Annot::makeAppearanceStream(GString *str, Object *stream) { DEBUG_INFO
  Object bDict, bbox, obj;
  Dict *dict;
  Array *array;

  bDict.initDict((XRef *)NULL);
  dict = bDict.getDict();
  bbox.initArray(NULL);
  array = bbox.getArray();
  obj.initInt(0);
  array->add(&obj);
  obj.initInt(0);
  array->add(&obj);
  obj.initReal(x2 - x1);
  array->add(&obj);
  obj.initReal(y2 - y1);
  array->add(&obj);
  dict->add(copyString("BBox"), &bbox);
  stream->free();
  stream->initStream(new StrStream(str, &bDict));
  return stream;
}

void Annot::setAppearance(AnnotAppearance a, Object *stream) { DEBUG_INFO
  appearance[a].free();
  stream->copy(&appearance[a]);
}

Object *Annot::getAppearance(XRef *xref, AnnotAppearance a, Object *obj) { DEBUG_INFO
  Object *obj1 = &appearance[a];
  if (obj1->isDict() && appearanceState.isName())
    obj1->dictLookup(appearanceState.getName(), obj);
  else if (obj1->isName() || obj1->isStream())
    obj1->copy(obj);
  else if (obj1->isRef())
    obj1->fetch(xref, obj);
  else {
    if(!obj1->isNull() && !obj1->isNone())
      error(-1, "Bad annotation appearance");
    obj->initNull();
  }
  return obj;
}

GBool Annot::hasAppearance(AnnotAppearance a) { DEBUG_INFO
  Object *obj1 = &appearance[a];
  Object obj;
  if (obj1->isDict() && appearanceState.isName())
    obj1->dictLookupNF(appearanceState.getName(), &obj);
  else if (obj1->isName() || obj1->isStream() || obj1->isRef())
    return gTrue;
  else
    return gFalse;
  GBool r = !obj.isNull() && !obj.isNone();
  obj.free();
  return r;
}

void Annot::setAppearanceState(char *state) {
  appearanceState.free();
  appearanceState.initName(state);
}

void Annot::addAdditionalAction(ActionTrigger t, Action *action) {
  if (!additionalAction)
    additionalAction = new AdditionalAction(NULL);
  additionalAction->setAction(t, action);
}


//------------------------------------------------------------------------
// RawTextAnnot
//------------------------------------------------------------------------

class ShowContentsAction : public Action {
public:
  ShowContentsAction(GString *contents1) : Action(NULL), contents(contents1) {}
  virtual GBool isOk() { return gTrue; }
  virtual ActionKind getKind() { return actionUnknown; }
protected:
  virtual Action *doExecute(ActionContext *context) {
    context->showText(contents);
    return NULL;
  }
private:
  GString *contents;
};


RawTextAnnot::RawTextAnnot(XRef *xref, Object *dict)
: Annot(xref, dict) { DEBUG_INFO
  Object obj;

  contents = NULL;
  open = gFalse;

  noBorder();

  if (dict->dictLookup("Contents", &obj)->isString())
    contents = obj.getString()->copy();
  obj.free();

  if (dict->dictLookup("Open", &obj)->isBool())
    open = obj.getBool();
  obj.free();

  if (contents)
    addAdditionalAction(trigDown, new ShowContentsAction(contents));
}

RawTextAnnot::~RawTextAnnot() {
  delete contents;
}


//------------------------------------------------------------------------
// TextAnnot
//------------------------------------------------------------------------

TextAnnot::TextAnnot(XRef *xref, Object *dict)
: RawTextAnnot(xref, dict) { DEBUG_INFO
  Object obj;
  GString *str;

  noBorder();

  if (getAppearance(xref, appearanceNormal, &obj)->isNull()) {
    double x1,y1,x2,y2;
    char buf[80];

    getRect(&x1, &y1, &x2, &y2);

    str = new GString("q 2 w 0 0 0 RG 1 1 0 rg ");
    sprintf(buf, "1 0 0 1 %f %f cm ", 1, y2 - y1 - 31);
    str->append(buf);
    str->append("5 30 m 20 30 l 20 0 l 0 0 l 0 25 l b "
		"5 30 m 5 25 l 0 25 l S "
		"6 8 m 16 8 l S "
		"4 13 m 16 13 l S "
		"4 18 m 16 18 l S "
		"Q");

    obj.free();
    setAppearance(appearanceNormal, makeAppearanceStream(str, &obj));
  }
  obj.free();
}

//------------------------------------------------------------------------
// LineAnnot
//------------------------------------------------------------------------

LineAnnot::LineAnnot(XRef *xref, Object *dict)
: RawTextAnnot(xref, dict) { DEBUG_INFO
  Object obj;

  noBorder();

  if (getAppearance(xref, appearanceNormal, &obj)->isNull()) {
    Object obj1, obj2, obj3;
    double m[4], borderW, x1, y1, x2, y2;
    char buf[64];
    int k;
    int len;
    GString *str;

    if (dict->dictLookup("L", &obj1)->isArray() && obj1.arrayGetLength() == 4) {
      for (k = 0; k < 4; ++k) {
	if (obj1.arrayGet(k, &obj2)->isNum())
	  m[k] = obj2.getNum();
	obj2.free();
      }
    } else {
      obj1.free();
      error(-1, "Bad line annotation");
      return;
    }
    obj1.free();

    str = new GString("q");

    if (!dict->dictLookup("BS", &obj1)->isNull()) {
      if (obj1.isDict()) {
	borderW = 1;
	if (!obj1.dictLookup("W", &obj2)->isNull()) {
	  if (obj2.isNum())
	    borderW = obj2.getNum();
	  else
	    error(-1, "Bad annotation border style");
	}
	obj2.free();

	if (borderW > 0) {
	  char buf[64];
	  GBool hasDashes = gFalse;

	  len = sprintf(buf, " %f w", borderW);
	  str->append(buf, len);

	  if (obj1.dictLookup("D", &obj2)->isArray()) {
	    int i;
	    int n = obj2.arrayGetLength();

	    hasDashes = gTrue;
	    str->append('[');
	    for (i = 0; i < n; ++i) {
	      if (obj2.arrayGet(i, &obj3)->isNum()) {
		len = sprintf(buf, " %f", obj3.getNum());
		str->append(buf, len);
	      } else
		error(-1, "Bad border dashes array");
	      obj3.free();
	    }
	    str->append("]0 d");
	  }
	  obj2.free();

	  if (obj1.dictLookup("S", &obj2)->isName("D") && !hasDashes)
	    str->append(" [3]0 d");
	  obj2.free();
	}
      } else
	error(-1, "Bad line annotation");
    }
    obj1.free();

    getRect(&x1, &y1, &x2, &y2);
    len = sprintf(buf, " %f %f m %f %f l S Q",
		  m[0] - x1, m[1] - y1, m[2] - x1, m[3] - y1);
    str->append(buf, len);

    setAppearance(appearanceNormal, makeAppearanceStream(str, &obj1));
    obj1.free();
  }
  obj.free();
}


//------------------------------------------------------------------------
// SquareAnnot
//------------------------------------------------------------------------

SquareAnnot::SquareAnnot(XRef *xref, Object *dict)
: RawTextAnnot(xref, dict) { DEBUG_INFO
  Object obj;

  noBorder();

  if (getAppearance(xref, appearanceNormal, &obj)->isNull()) {
    Object obj1, obj2, obj3;
    double m[4], borderW, x1, y1, x2, y2;
    char buf[64];
    int k;
    int len;
    GString *str = new GString("q");

    if (!dict->dictLookup("BS", &obj1)->isNull()) {
      if (obj1.isDict()) {
	borderW = 1;
	if (!obj1.dictLookup("W", &obj2)->isNull()) {
	  if (obj2.isNum())
	    borderW = obj2.getNum();
	  else
	    error(-1, "Bad annotation border style");
	}
	obj2.free();

	if (borderW > 0) {
	  char buf[64];
	  GBool hasDashes = gFalse;

	  len = sprintf(buf, " %f w", borderW);
	  str->append(buf, len);

	  if (obj1.dictLookup("D", &obj2)->isArray()) {
	    int i;
	    int n = obj2.arrayGetLength();

	    hasDashes = gTrue;
	    str->append('[');
	    for (i = 0; i < n; ++i) {
	      if (obj2.arrayGet(i, &obj3)->isNum()) {
		len = sprintf(buf, " %f", obj3.getNum());
		str->append(buf, len);
	      } else
		error(-1, "Bad border dashes array");
	      obj3.free();
	    }
	    str->append("]0 d");
	  }
	  obj2.free();

	  if (obj1.dictLookup("S", &obj2)->isName("D") && !hasDashes)
	    str->append(" [3]0 d");
	  obj2.free();
	}
      } else
	error(-1, "Bad line annotation");
    }
    obj1.free();

    getRect(&x1, &y1, &x2, &y2);
    len = sprintf(buf, " %f %f %f %f re S Q",
		  borderW / 2, borderW / 2,
		  x2 - x1 - borderW, y2 - y1 - borderW);
    str->append(buf, len);

    setAppearance(appearanceNormal, makeAppearanceStream(str, &obj1));
    obj1.free();
  }
  obj.free();
}


//------------------------------------------------------------------------
// CircleAnnot
//------------------------------------------------------------------------

CircleAnnot::CircleAnnot(XRef *xref, Object *dict)
: RawTextAnnot(xref, dict) { DEBUG_INFO
  Object obj;

  noBorder();

  if (getAppearance(xref, appearanceNormal, &obj)->isNull()) {
    Object obj1, obj2, obj3;
    double m[4], borderW, x1, y1, x2, y2, x3, y3;
    const double C = 0.552284749827;
    char buf[256];
    int k;
    int len;
    GString *str = new GString("q");

    if (!dict->dictLookup("BS", &obj1)->isNull()) {
      if (obj1.isDict()) {
	borderW = 1;
	if (!obj1.dictLookup("W", &obj2)->isNull()) {
	  if (obj2.isNum())
	    borderW = obj2.getNum();
	  else
	    error(-1, "Bad annotation border style");
	}
	obj2.free();

	if (borderW > 0) {
	  char buf[64];
	  GBool hasDashes = gFalse;

	  len = sprintf(buf, " %f w", borderW);
	  str->append(buf, len);

	  if (obj1.dictLookup("D", &obj2)->isArray()) {
	    int i;
	    int n = obj2.arrayGetLength();

	    hasDashes = gTrue;
	    str->append('[');
	    for (i = 0; i < n; ++i) {
	      if (obj2.arrayGet(i, &obj3)->isNum()) {
		len = sprintf(buf, " %f", obj3.getNum());
		str->append(buf, len);
	      } else
		error(-1, "Bad border dashes array");
	      obj3.free();
	    }
	    str->append("]0 d");
	  }
	  obj2.free();

	  if (obj1.dictLookup("S", &obj2)->isName("D") && !hasDashes)
	    str->append(" [3]0 d");
	  obj2.free();
	}
      } else
	error(-1, "Bad line annotation");
    }
    obj1.free();

    getRect(&x1, &y1, &x2, &y2);
    x2 -= x1;
    y2 -= y1;
    x1 = borderW / 2;
    y1 = borderW / 2;
    x2 -= x1;
    y2 -= y1;
    x3 = (x1 + x2) / 2;
    y3 = (y1 + y2) / 2;
    len = sprintf(buf, " %f %f m"
		       " %f %f %f %f %f %f c"
		       " %f %f %f %f %f %f c"
		       " %f %f %f %f %f %f c"
		       " %f %f %f %f %f %f c S Q",
		  x1, y3,
		  x1, y3 + (y2 - y3) * C, x3 + (x1 - x3) * C, y2, x3, y2,
		  x3 + (x2 - x3) * C, y2, x2, y3 + (y2 - y3) * C, x2, y3,
		  x2, y3 + (y1 - y3) * C, x3 + (x2 - x3) * C, y1, x3, y1,
		  x3 + (x1 - x3) * C, y1, x1, y3 + (y1 - y3) * C, x1, y3);
    str->append(buf, len);

    setAppearance(appearanceNormal, makeAppearanceStream(str, &obj1));
    obj1.free();
  }
  obj.free();
}


//------------------------------------------------------------------------
// LinkAnnot
//------------------------------------------------------------------------

LinkAnnot::LinkAnnot(XRef *xref, Object *dict)
: Annot(xref, dict) { DEBUG_INFO

  action = NULL;

  /*Object obj;

  if (dict->dictLookup("A", &obj)->isDict())
    action = Action::makeAction(&obj);
  obj.free();

  if (!action && !dict->dictLookup("Dest", &obj)->isNull())
    action = new ActionGoTo(new Destination(&obj));
  obj.free();*/

  AdditionalAction *aa = getAdditionalAction();
  if (aa)
    action = aa->getAction(trigUp);
}

LinkAnnot::~LinkAnnot() {
  //delete action;
}


//------------------------------------------------------------------------
// WidgetAnnot
//------------------------------------------------------------------------

class WidgetAction : public Action {
public:
  WidgetAction(Field *field1) : Action(NULL), field(field1) {}
  virtual GBool isOk() { return gTrue; }
  virtual ActionKind getKind() { return actionUnknown; }
protected:
  virtual Action *doExecute(ActionContext *context) {
    return field->down(context);
  }
private:
  Field *field;
};

WidgetAnnot::WidgetAnnot(XRef *xref, Object *dict, Field *field1)
: Annot(xref, dict) { DEBUG_INFO
  field = field1;
  field1->setAnnot(this);

  addAdditionalAction(trigDown, new WidgetAction(field1));
}

WidgetAnnot::~WidgetAnnot() {
  field->setAnnot(NULL);
}


//------------------------------------------------------------------------
// Annots
//------------------------------------------------------------------------

Annots::Annots(XRef *xref, Object *annotsObj, AcroForm *form) { DEBUG_INFO
  Annot *annot;
  Object obj1, obj2;
  int size;
  int i;

  annots = NULL;
  size = 0;
  numAnnots = 0;

  if (annotsObj->isArray()) {
    for (i = 0; i < annotsObj->arrayGetLength(); ++i) {
      if (annotsObj->arrayGetNF(i, &obj1)) {
	Ref ref;

	if (obj1.isRef()) {
	  ref = obj1.getRef();
	  Object obj3;
	  obj1.copy(&obj3);
	  obj1.free();
	  obj3.fetch(xref, &obj1);
	  obj3.free();
	} else {
	  ref.num = -1;
	  ref.gen = -1;
	}

	if (obj1.isDict()) {
	  obj1.dictLookup("Subtype", &obj2);

	  if (obj2.isName("Text"))
	    annot = new TextAnnot(xref, &obj1);
	  else if (obj2.isName("Link"))
	    annot = new LinkAnnot(xref, &obj1);
	  else if (obj2.isName("Text"))
	    annot = new TextAnnot(xref, &obj1);
	  else if (obj2.isName("Line"))
	    annot = new LineAnnot(xref, &obj1);
	  else if (obj2.isName("Square"))
	    annot = new SquareAnnot(xref, &obj1);
	  else if (obj2.isName("Circle"))
	    annot = new CircleAnnot(xref, &obj1);
	  else if (obj2.isName("Widget")) {
	    Field *field = form ? form->findField(ref.num, ref.gen) : NULL;
	    if (field)
	      annot = new WidgetAnnot(xref, &obj1, field);
	    else {
	      annot = NULL;
	      error(-1, "Can't associate Widget annotation to Form entry.");
	    }
//printf("Ref %d %d Widget %lx = Annot %lx ok = %d\n", ref.num, ref.gen, field, annot, annot->isOk());
	  } else {
	    annot = NULL;
	    if (obj2.isName())
	      error(-1, "Unsupported annotation type: %s\n", obj2.getName());
	    else
	      error(-1, "Bad annotation");
	  }
	  obj2.free();
	  if (annot && annot->isOk()) {
	    if (numAnnots >= size) {
	      size += 16;
	      annots = (Annot **)grealloc(annots, size * sizeof(Annot *));
	    }
	    annots[numAnnots++] = annot;
	  } else {
	    delete annot;
	  }
	} else
	  error(-1, "Bad annotation");
      }
      obj1.free();
    }
  }
}

Annots::~Annots() {
  int i;

  for (i = 0; i < numAnnots; ++i)
    delete annots[i];
  gfree(annots);
}

/*Annot *Annots::find(double x, double y) {
  int i;

  for (i = 0; i < numAnnots; ++i) {
    if (annots[i]->inRect(x, y))
      return annots[i];
  }
  return NULL;
}*/

/*GBool Annots::onAnnot(double x, double y) {
  int i;

  for (i = 0; i < numAnnots; ++i) {
    if (annots[i]->inRect(x, y))
      return gTrue;
  }
  return gFalse;
}*/


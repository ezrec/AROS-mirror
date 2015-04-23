//========================================================================
//
// AcroForm.cc
//
// Copyright 2000-2005 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
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
#include "FontCache.h"
#include "XRef.h"
#include "FDFForm.h"

//------------------------------------------------------------------------
// FieldAttrs
//------------------------------------------------------------------------

FieldAttrs::FieldAttrs(FieldAttrs *attrs, Dict *dict) { DEBUG_INFO
  Object obj;

  if (attrs) {
    attrs->value.copy(&value);
    attrs->defaultValue.copy(&defaultValue);
    flags = attrs->flags;
    attrs->defaultResources.copy(&defaultResources);
    defaultAppearance = attrs->defaultAppearance ?
			attrs->defaultAppearance->copy() : NULL;
    quadding = attrs->getQuadding();
    typeName = attrs->typeName ? attrs->typeName->copy() : NULL;
    attrs->opt.copy(&opt);
    topIndex = attrs->topIndex;
    maxLen = attrs->maxLen;
    name = attrs->name ? attrs->name->copy() : NULL;
    ref = attrs->ref;
  } else {
    value.initNull();
    defaultValue.initNull();
    flags = 0;
    defaultResources.initNull();
    defaultAppearance = NULL;
    quadding = 0;
    typeName = NULL;
    opt.initNull();
    topIndex = 0;
    maxLen = 65535;
    name = NULL;
    ref.num = -1;
    ref.gen = -1;
  }

  if (!dict)
    return;

  // value
  if (!dict->lookup("V", &obj)->isNull()) {
    value.free();
    obj.copy(&value);
  }
  obj.free();

  // default value
  if (!dict->lookup("DV", &obj)->isNull()) {
    defaultValue.free();
    obj.copy(&defaultValue);
  }
  obj.free();

  // flags
  if (dict->lookup("Ff", &obj)->isInt())
    flags = obj.getInt();
  obj.free();

  // default resources
  if (dict->lookup("DR", &obj)->isDict()) {
    defaultResources.free();
    obj.copy(&defaultResources);
  }
  obj.free();

  // default appearance string
  if (dict->lookup("DA", &obj)->isString()) {
    delete defaultAppearance;
    defaultAppearance = NULL; // in case an exception occurs.
    defaultAppearance = obj.getString()->copy();
  }
  obj.free();

  // highlight NOT INHERITABLE
  /*if (dict->lookup("H", &obj)) {
  ***
  }
  obj.free();*/

  // quadding
  if (dict->lookup("Q", &obj)->isInt())
    quadding = obj.getInt();
  obj.free();

  // original height
  /*if (dict->lookup("OH", &obj)->isInt()) {

  }
  obj.free();*/

  // type
  if (dict->lookup("FT", &obj)->isName()) {
    delete typeName;
    typeName = new GString(obj.getName());
  }
  obj.free();

  // choice options
  if (dict->lookup("Opt", &obj)->isArray()) {
    opt.free();
    obj.copy(&opt);
  }
  obj.free();

  // top index
  if (dict->lookup("TopIndex", &obj)->isInt())
    topIndex = obj.getInt();
  obj.free();

  // max length
  if (dict->lookup("MaxLen", &obj)->isInt())
    maxLen = obj.getInt();
  obj.free();

}

FieldAttrs::~FieldAttrs() {
  value.free();
  defaultValue.free();
  defaultResources.free();
  delete defaultAppearance;
  delete typeName;
  opt.free();
  delete name;
}


//------------------------------------------------------------------------
// SubmitFDFContext
//------------------------------------------------------------------------

SubmitFDFContext::SubmitFDFContext(const char *fileName) {
  file = fopen(fileName, "w");
  if (file) {
    fprintf(file,
	    "%%FDF-1.3\n"
	    "1 0 obj\n"
	    "<</FDF <</Fields [\n");
  }
}

SubmitFDFContext::~SubmitFDFContext() {
  if (file) {
    fprintf(file,
	    "] >> >>\n"
	    "endobj\n"
	    "trailer\n"
	    "<</Root 1 0 R>>\n"
	    "%%%%EOF\n");
    fclose(file);
  }
}

void SubmitFDFContext::beginField(GString *name) {
  if (file) {
    fprintf(file, "<<\n");
    if (name)
      fprintf(file, "/T (%s)\n", name->getCString());
  }
}

void SubmitFDFContext::endField() {
  if (file) {
    fprintf(file, ">>\n");
  }
}

void SubmitFDFContext::addValue(GString *value) {
  if (file) {
    int k;
    int len = value->getLength();
    const char *p = value->getCString();
    int column = 0;

    fprintf(file, "/V (");
    for (k = 0; k < len; ++k, ++p) {
      if (*p < ' ' || *p >= 127 || *p == '(' || *p == ')' || *p == '\\') {
	fprintf(file, "\\%03o", *p);
	column += 4;
      } else {
	fputc(*p, file);
	++column;
      }
      if (column >= 72) {
	column = 0;
	fprintf(file, "\\\n");
      }
    }
    fprintf(file, ")\n");
  }
}

void SubmitFDFContext::addNameValue(GString *value) {
  if (file) {
    int k;
    int len = value->getLength();
    const char *p = value->getCString();

    fprintf(file, "/V /");
    for (k = 0; k < len; ++k, ++p) {
	if (*p <= ' ' || *p >= 127 || *p == '%' || *p == '(' || *p == ')' ||
	    *p == '<' || *p == '>' || *p == '[' || *p == ']' || *p == '{' ||
	    *p == '}' || *p == '/' || *p == '#') {
	fprintf(file, "#%02x", *p);
      } else {
	fputc(*p, file);
      }
    }
    fprintf(file, "\n");
  }
}

void SubmitFDFContext::beginKids() {
  if (file) {
    fprintf(file, "/Kids [\n");
  }
}

void SubmitFDFContext::endKids() {
  if (file) {
    fprintf(file, "]\n");
  }
}


//------------------------------------------------------------------------
// SubmitHTMLGetContext
//------------------------------------------------------------------------

SubmitHTMLGetContext::SubmitHTMLGetContext() {
  str = NULL;
}

SubmitHTMLGetContext::~SubmitHTMLGetContext() {
  delete str;
}

void SubmitHTMLGetContext::beginField(GString *name) {
  if (str) {
    str->append('&');
  } else {
    str = new GString;
  }
  if (name) {
    str->append(name);
    str->append('=');
  }
}

void SubmitHTMLGetContext::addValue(GString *value) {
  if (str) {
    int k;
    int len = value->getLength();
    const char *p = value->getCString();

    for (k = 0; k < len; ++k, ++p) {
      if (*p == ' ') {
	str->append('+');
      } else if (*p == '\n') {
	str->append("%%0D%%0A");
      } else if (*p < ' ' || *p >= 127 || *p == '+') {
	char buf[4];
	sprintf(buf, "%%%02X", *p);
	str->append(buf);
      } else {
	str->append(*p);
      }
    }
  }
}

void SubmitHTMLGetContext::addNameValue(GString *value) {
  addValue(value);
}


//------------------------------------------------------------------------
// SubmitHTMLPostContext
//------------------------------------------------------------------------

SubmitHTMLPostContext::SubmitHTMLPostContext(const char *fileName) {
  file = fopen(fileName, "w");
}

SubmitHTMLPostContext::~SubmitHTMLPostContext() {
  if (file) {
    fclose(file);
  }
}

void SubmitHTMLPostContext::beginField(GString *name) {
  if (file && name) {
    fprintf(file, "%s=\"", name->getCString());
  }
}

void SubmitHTMLPostContext::addValue(GString *value) {
  if (file) {
    int k;
    int len = value->getLength();
    const char *p = value->getCString();

    for (k = 0; k < len; ++k, ++p) {
      if (*p < ' ' || *p >= 127 || *p == '\"') {
	fprintf(file, "#%02x", *p);
      } else {
	fputc(*p, file);
      }
    }
  }
}

void SubmitHTMLPostContext::addNameValue(GString *value) {
  addValue(value);
}


//------------------------------------------------------------------------
// Field
//------------------------------------------------------------------------

Field::Field(XRef *xref, Dict *dict, FieldAttrs *attrs) { DEBUG_INFO
  Object obj;

  name = NULL;
  userName = NULL;
  mappingName = NULL;
  next = NULL;
  kids = NULL;
  annot = NULL;

  flags = attrs->getFlags();
  ref = attrs->getRef();

  // name
  if (dict->lookup("T", &obj)->isString()) {
    GString *name1;
    GString *name2;
    name1 = obj.getString();
    name2 = attrs->getName();
    if (name2)
      name = name2->copy()->append('.')->append(name1);
    else
      name = name1->copy();
  }
  obj.free();

  // user name
  if (dict->lookup("TU", &obj)->isString())
    userName = obj.getString()->copy();
  obj.free();

  // mapping name
  if (dict->lookup("TM", &obj)->isString())
    mappingName = obj.getString()->copy();
  obj.free();

}

Field::~Field() {
  delete name;
  delete userName;
  delete mappingName;
}

void Field::setAnnot(WidgetAnnot *annot1) {
  if (annot1) {
    annot1->restoreState(&annotState);
  } else if (annot) {
    annot->saveState(&annotState);
  }
  annot = annot1;
}

GBool Field::import(Dict *dict) { DEBUG_INFO
  Object obj;
  Annot *annot;
  Field *field;

  if (dict->lookup("Ff", &obj)->isInt())
    flags = obj.getInt();
  else {
    obj.free();
    if (dict->lookup("SetFf", &obj)->isInt())
      flags |= obj.getInt();
    obj.free();
    if (dict->lookup("ClrFf", &obj)->isInt())
      flags &= ~obj.getInt();
  }
  obj.free();

  annotState.import(dict);

  annot = getAnnot();
  if (annot)
    annot->restoreState(&annotState);

  for (field = kids; field; field = field->next)
    if (!field->getName())
      field->import(dict);

  return gTrue;
}

void Field::resetFieldsTree() {
  Field *field;

  reset();
  for (field = kids; field; field = field->next)
    field->resetFieldsTree();
}

void Field::submit(SubmitContext *context) {
  if (!noExport() && exportFlag) {
    GBool hasValue;

    context->beginField(getName());
    hasValue = doSubmit(context);
    if (kids) {
      GBool hasKid = gFalse;
      for (Field *field = kids; field; field = field->next)
	if (!field->getName()) {
	  if (!hasValue) {
	    hasValue = field->doSubmit(context);
	  }
	} else if (field->exportFlag)
	  hasKid = gTrue;
      if (hasKid) {
	context->beginKids();
	for (Field *field = kids; field; field = field->next)
	  if (field->getName())
	    field->submit(context);
	context->endKids();
      }
    }
    context->endField();
  }
}

void Field::setExportFlags(GBool includeNoValue) {
  GBool hasExport = gFalse;
  for (Field *field = kids; field; field = field->next) {
    field->setExportFlags(includeNoValue);
    if (field->exportFlag)
      hasExport = gTrue;
  }
  exportFlag = hasExport || includeNoValue || hasValue();
}

GBool Field::doSubmit(SubmitContext *) {
  return gFalse;
}


//------------------------------------------------------------------------
// NodeField
//------------------------------------------------------------------------

NodeField::NodeField(XRef *xref, Dict *dict, FieldAttrs *attrs)
: Field(xref, dict, attrs) { DEBUG_INFO
}


//------------------------------------------------------------------------
// PushButtonField
//------------------------------------------------------------------------

PushButtonField::PushButtonField(XRef *xref, Dict *dict, FieldAttrs *attrs)
: Field(xref, dict, attrs) { DEBUG_INFO
}


//------------------------------------------------------------------------
// CheckboxField
//------------------------------------------------------------------------

CheckboxField::CheckboxField(XRef *xref, Dict *dict, FieldAttrs *attrs)
: Field(xref, dict, attrs) { DEBUG_INFO
  Object obj1, obj2, obj3;

  radio = NULL;

  attrs->getValue(&value);
  if (!value.isName()) {
    value.free();
    value.initName("Off");
  }
  attrs->getDefaultValue(&defaultValue);
  if (!defaultValue.isName()) {
    defaultValue.free();
    defaultValue.initName("Off");
  }
  if (dict->lookup("AP", &obj1)->isDict()) {
    if (obj1.dictLookup("N", &obj2)->isDict()) {
      if (obj2.dictLookupNF(value.getName(), &obj3)->isNull()) {
	value.free();
	value.initName("Off");
      }
      obj3.free();
      if (obj2.dictLookupNF(defaultValue.getName(), &obj3)->isNull()) {
	defaultValue.free();
	defaultValue.initName("Off");
      }
      obj3.free();
    }
    obj2.free();
  }
  obj1.free();
}

CheckboxField::~CheckboxField() {
  value.free();
  defaultValue.free();
}

void CheckboxField::reset() { DEBUG_INFO
  value.free();
  defaultValue.copy(&value);
  if (Annot *annot = getAnnot())
    annot->setAppearanceState(value.getName());
}

GBool CheckboxField::doSubmit(SubmitContext *context) {
  GString str(value.getName());
  context->addNameValue(&str);
  return gTrue;
}

GBool CheckboxField::import(Dict *dict) { DEBUG_INFO
  Object obj;

  Field::import(dict);
  if (dict->lookup("V", &obj)->isName()) {
    value.free();
    obj.copy(&value);
    if (Annot *annot = getAnnot())
      annot->setAppearanceState(value.getName());
  }
  obj.free();

  return gTrue;
}

void CheckboxField::toggle() { DEBUG_INFO
  if (Annot *annot1 = getAnnot()) {
    if (!strcmp(value.getName(), "Off")) {
      Dict *dict = annot1->getAppearanceDict();
      if (dict) {
	int num = dict->getLength();
	int k;
	for (k = 0; k < num; ++k)
	  if (strcmp(dict->getKey(k), "Off")) {
	    value.free();
	    value.initName(dict->getKey(k));
	    break;
	  }
      }
    } else {
      value.free();
      value.initName("Off");
    }
    annot1->setAppearanceState(value.getName());
  }
}

Action *CheckboxField::down(ActionContext *context) {
  if (radio)
    return radio->down(context, this);
  toggle();
  context->refresh(getAnnot());
  return NULL;
}


//------------------------------------------------------------------------
// RadioField
//------------------------------------------------------------------------

RadioField::RadioField(XRef *xref, Dict *dict, FieldAttrs *attrs)
: Field(xref, dict, attrs) { DEBUG_INFO
  Object obj;

  numChoices = 0;
  maxChoices = 0;
  choices = NULL;

  attrs->getValue(&value);
  if (!value.isName()) {
    value.free();
    value.initName("Off");
  }
  attrs->getDefaultValue(&defaultValue);
  if (!defaultValue.isName()) {
    defaultValue.free();
    defaultValue.initName("Off");
  }

  if (dict->lookup("Kids", &obj)->isArray()) {
    maxChoices = obj.arrayGetLength();
    choices = (CheckboxField **)gmalloc(maxChoices * sizeof(*choices));
  }
  obj.free();
}

RadioField::~RadioField() {
  gfree(choices);
  value.free();
  defaultValue.free();
}

void RadioField::reset() {
  value.free();
  defaultValue.copy(&value);
}

GBool RadioField::doSubmit(SubmitContext *context) {
  GString str(value.getName());
  context->addNameValue(&str);
  return gTrue;
}

GBool RadioField::import(Dict *dict) { DEBUG_INFO
  Object obj;

  Field::import(dict);
  if (dict->lookup("V", &obj)->isName()) {
    value.free();
    obj.copy(&value);
  }
  obj.free();

  return gTrue;
}

void RadioField::addChoice(CheckboxField *button) {
  if (button && numChoices < maxChoices) {
    choices[numChoices++] = button;
    button->setRadio(this);
  }
}

Action *RadioField::down(ActionContext *context, CheckboxField *box) {
  int k;
  if (isNoToggleToOff() && strcmp(box->getValue(), "Off"))
    return NULL;
  box->toggle();
  context->refresh(box->getAnnot());
  for (k = 0; k < numChoices; ++k)
    if (choices[k] != box && strcmp(choices[k]->getValue(), "Off")) {
      choices[k]->toggle();
      context->refresh(choices[k]->getAnnot());
    }
  value.free();
  value.initName(box->getValue());
  return NULL;
}


//------------------------------------------------------------------------
// TextBaseField
//------------------------------------------------------------------------

TextBaseField::TextBaseField(XRef *xrefA, Dict *dict, FieldAttrs *attrs)
: Field(xrefA, dict, attrs) { DEBUG_INFO
  Object obj;
  GString *da;
  int k;

  xref = xrefA;
  defaultAppearance = NULL;
  changed = gFalse;
  lines = 1;

  attrs->getDefaultResources(&defaultResources);
  da = attrs->getDefaultAppearance();
  if (da)
    defaultAppearance = da->copy();
  quadding = attrs->getQuadding();

  if (dict->lookup("OH", &obj)->isInt())
    origHeight = obj.getInt();
  else
    origHeight = -1;
  obj.free();
}

TextBaseField::~TextBaseField() {
  defaultResources.free();
  delete defaultAppearance;
}

void TextBaseField::setAnnot(WidgetAnnot *annot1) {
  Field::setAnnot(annot1);
  if (changed)
    rebuildAppearance();
}

GString *TextBaseField::makeAppStreamData() { DEBUG_INFO
  char c, *p;
  int k, l, lev;
  GString *str;
  GfxFont *gfxFont;
  //BaseFont *baseFont;

  // emit the prolog
  str = new GString("/Tx BMC q BT ");

  // add the DA entry
  if (defaultAppearance)
    str->append(defaultAppearance);

  // There should be a Tf operator.
  // If it has 0 as height argument, replace by OH.
  // Get the font from the name given to Tf and the
  // default resources dictionary.
  p = str->getCString();
//printf("orig appearance: \"%s\"\norigHeight=%d\n", p, origHeight);
  k = 0;
  lev = 0;
  gfxFont = NULL;
  while (p[k]) {
    // no need to check for k > 0 because chars can't be found
    // at the begining of the string.
    if (p[k] == '(' && p[k - 1] != '\\')
      ++lev;
    else if (p[k] == ')' && p[k - 1] != '\\')
      --lev;
    else if (lev == 0 && p[k] == 'T' && p[k + 1] == 'f' &&
	     isspace(p[k - 1])) {
//printf("found Tf\n");
      int m = k;
      double s1 = 0, s2 = 1;
      --k;
      do { --k; } while (isspace(p[k]));

      l = k;

      while(isdigit(p[k])) {
	s1 += (p[k] - '0') * s2;
	s2 *= 10;
	--k;
      }
      if (p[k] == '.') {
	s1 /= s2 * 10;
	s2 = 1;
	--k;
	while (isdigit(p[k])) {
	  s1 += (p[k] - '0') * s2;
	  s2 *= 10;
	  --k;
	}
      }

//printf("size=%g\n", s1);

      if (s1 < 0.05) {
	GString *size = GString::fromInt(int(0.75 * origHeight));
//printf("->size=%s\n",size->getCString());

	str->del(k + 1, l - k);
	str->insert(k + 1, size);
	p = str->getCString();
	m += size->getLength() + k - l;
	delete size;
      }

      while (isspace(p[k]))
	--k;

      l = k;
      while (p[k] != '/')
	--k;
      if (defaultResources.isDict()) {
	Object fontRes, obj;
	GString name(p + k + 1, l - k);
	if (defaultResources.dictLookup("Font", &fontRes)->isDict()) {
//printf("font=\"%s\"\n",name.getCString());
	  if (fontRes.dictLookupNF(name.getCString(), &obj)->isRef()) {
	      gfxFont = gfxFontCache->getGfxFont(xref, name.getCString(), obj.getRef());
	  } else {
	    error(-1, "Font resource entry is not indirect");
	  }
//printf("gfxFont=%x\n",gfxFont);
	  obj.free();
	} else {
	  error(-1, "Font resource is not a dictionary");
	}
	fontRes.free();
      } else {
	error(-1, "No default resources");
      }
      k = m;
    }
    ++k;
  }

  if (gfxFont) {
    double x1, y1, x2, y2;
    double *bBox;
    char buf[32];
    int len1;
    GString *str1;

    Annot *annot = getAnnot();
    if (annot)
      annot->getRect(&x1, &y1, &x2, &y2);
    else {
      y2 = origHeight;
      y1 = 0;
    }

    bBox = gfxFont->getFontBBox();
//printf("bbox %f %f %f %f origh %d a/d %f/%f\n",
//       bBox[0], bBox[1], bBox[2], bBox[3], origHeight,
//       gfxFont->getAscent(), gfxFont->getDescent());

    len1 = sprintf(buf, " 1 0 0 1 %f %f Tm %d TL ", -bBox[0] * origHeight,
		   (lines - gfxFont->getAscent()) * origHeight, origHeight);

    str->append(buf, len1);

    str1 = getValueAppearance();
    if (str1) {
      str->append(str1);
      delete str1;
    }
  }

  str->append(" ET Q EMC");
  return str;
}

void TextBaseField::buildAppearance() { DEBUG_INFO
  Annot *annot = getAnnot();

  if (annot) {
    Object bDict, bbox, obj;
    Dict *dict;
    Array *array;
    double x1, y1, x2, y2, d;

    // create the stream dictionary
    bDict.initDict((XRef *)NULL);
    dict = bDict.getDict();

    // add the BBox attribute
    annot->getRect(&x1, &y1, &x2, &y2);
//printf("rect=(%g,%g)-(%g,%g)\n",x1,y1,x2,y2);
    bbox.initArray(NULL);
    array = bbox.getArray();
    obj.initInt(0);
    array->add(&obj);
    obj.initInt(0);
    array->add(&obj);
    obj.initReal(fabs(x2 - x1));
    array->add(&obj);
    obj.initReal(fabs(y2 - y1));
    if (origHeight <= 0)
      origHeight = (int)obj.getReal();
    array->add(&obj);
    dict->add(copyString("BBox"), &bbox);

    // add the resources dictionary
    defaultResources.copy(&obj);
    dict->add(copyString("Resources"), &obj);

    // build the stream
    obj.initStream(new StrStream(makeAppStreamData(), &bDict));

    annot->setAppearance(appearanceNormal, &obj);
    obj.free();
  }
}


void TextBaseField::rebuildAppearance() { DEBUG_INFO
  GString *str;
  GString *str1;
  int c, k;
  int level, len;
  Object stream, stream1, dictObj, obj, obj2, obj3;
  Dict *dict, *dict1, *dict2;
  char *key;
  Annot *annot = getAnnot();

  if (annot) {
    changed = gFalse;
    annot->getAppearance(xref, appearanceNormal, &stream);

    if (stream.isStream()) {
      double x1, y1, x2, y2;
      Object obj, obj2;
      double m[4], d, h;

      annot->getRect(&x1, &y1, &x2, &y2);
//printf("rect=(%g,%g)-(%g,%g), OH=%d\n",x1,y1,x2,y2,origHeight);
      if (stream.streamGetDict()->lookup("Matrix", &obj)->isArray()) {
	for (k = 0; k < 4; ++k) {
	  obj.arrayGet(k, &obj2);
	  m[k] = obj2.getNum();
	  obj2.free();
	}
	h = (m[0] * (y2 - y1) - m[1] * (x2 - x1)) / (m[0] * m[3] - m[1] * m[2]);
      } else
	h = y2 - y1;
      obj.free();
      h = fabs(h);

      if (origHeight <= 0)
	origHeight = (int)h;
      else {
	lines = (int)(h / origHeight + 0.1);
	if (lines <= 1)
	  lines = 1;
      }
//printf("OH=%d, lines = %d\n",origHeight,lines);

      str = new GString;
      len = 0;

      stream.streamReset();
      while ((c = stream.streamGetChar()) != EOF) {
	str->append(c);
	++len;
	if (c == 'C' && len >= 7 && !strcmp(str->getCString() + len - 7, "/Tx BMC"))
	  break;
      }
      str1 = makeAppStreamData();
      if (c == EOF) {
	str->append(str1);
      } else {
	str->del(len - 7, 7);
	str->append(str1);
	str1->clear();
	len = 0;
	level = 1;
	while ((c = stream.streamGetChar()) != EOF) {
	  str1->append(c);
	  ++len;
	  if (c == 'C' && len >= 4) {
	    if (!strcmp(str1->getCString() + len - 4, " BMC"))
	      ++level;
	    else if (!strcmp(str1->getCString() + len - 3, "EMC") &&
		     --level == 0)
	      break;
	  } else if (c == '(') {
	    if (len < 2 || str1->getChar(len - 2) != '\\')
	      ++level;
	  } else if (c == ')') {
	    if (len < 2 || str1->getChar(len - 2) != '\\')
	      --level;
	  }
	}
	if (c == EOF)
	  error(-1, "Invalid appearance stream");
	else {
	  while ((c = stream.streamGetChar()) != EOF)
	    str->append(c);
	}
      }
      delete str1;


      if (defaultResources.isDict()) {
	Object resObj;
	Dict *resDict;


	// clone the stream dictionary, except the Resources entry
	dictObj.initDict((XRef *)NULL);
	dict = dictObj.getDict();
	dict1 = stream.streamGetDict();
	for (k = 0; k < dict1->getLength(); ++k) {
	  key = dict1->getKey(k);
	  if (strcmp(key, "Resources")) {
	    dict1->getValNF(k, &obj);
	    dict->add(copyString(key), &obj);
	  }
	}


	// build a Resources dictionary
	if (dict1->lookup("Resources", &obj)->isDict()) {
/*printf("streamRes=");
obj.print(stdout);
printf("\nDR=");
defaultResources.print(stdout);*/
	  // add defaultResources to the stream Resources
	  resObj.initDict((XRef *)NULL);
	  resDict = resObj.getDict();
	  dict1 = obj.getDict();
	  dict2 = defaultResources.getDict();
	  for (k = 0; k < dict1->getLength(); ++k) {
	    key = dict1->getKey(k);
	    dict1->getVal(k, &obj2);
	    // if a resource entry is in both dictionaries and
	    // is a dictionary, merge them.
	    // Otherwise, just use the stream one. Array entries
	    // should be merged too, but since the only documented
	    // one is ProcSet and is not used by Xpdf, don't bother.
	    if (dict2->lookup(key, &obj3)->isDict()) {
	      Object newEntry, obj4, obj5;
	      Dict *mergedDict;
	      Dict *origDict1 = obj2.getDict();
	      Dict *origDict2 = obj3.getDict();
	      int l;
	      char *key2;

	      newEntry.initDict((XRef *)NULL);
	      mergedDict = newEntry.getDict();
	      for (l = 0; l < origDict1->getLength(); ++l) {
		key2 = origDict1->getKey(l);
		origDict1->getValNF(l, &obj4);
		mergedDict->add(copyString(key2), &obj4);
	      }
	      for (l = 0; l < origDict2->getLength(); ++l) {
		key2 = origDict2->getKey(l);
		if (mergedDict->lookup(key2, &obj4)->isNull()) {
		  origDict2->getValNF(l, &obj5);
		  mergedDict->add(copyString(key2), &obj5);
		}
		obj4.free();
	      }
	      resDict->add(copyString(key), &newEntry);
	      obj2.free();
	    } else
	      resDict->add(copyString(key), &obj2);
	    obj3.free();
	  }

	  // add entries in DR that are not in the stream
	  for (k = 0; k < dict2->getLength(); ++k) {
	    key = dict2->getKey(k);
	    if (dict1->lookup(key, &obj3)->isNull()) {
	      dict2->getValNF(k, &obj2);
	      resDict->add(copyString(key), &obj2);
	    }
	    obj3.free();
	  }

	} else
	  defaultResources.copy(&resObj);
	dictObj.getDict()->add(copyString("Resources"), &resObj);
	obj.free();
/*printf("\nresult=");
dictObj.print(stdout);*/
      } else {
	dictObj.initDict(stream.streamGetDict());
	dictObj.getDict()->incRef();
      }
//printf("\n->stream=\"%s\"\n",str->getCString());
      stream1.initStream(new StrStream(str, &dictObj));
      annot->setAppearance(appearanceNormal, &stream1);
      stream1.free();
    } else
      buildAppearance();
  } else
    changed = gTrue;
  stream.free();
}


//------------------------------------------------------------------------
// ChoiceField
//------------------------------------------------------------------------

ChoiceField::ChoiceField(XRef *xrefA, Dict *dict, FieldAttrs *attrs)
: TextBaseField(xrefA, dict, attrs) { DEBUG_INFO
  Object obj, obj2, obj3;
  int k;

  value = NULL;
  defaultValue = NULL;

  attrs->getOpt(&opt);
  if (!opt.isArray())
    return;

  for (k = 0; k < opt.arrayGetLength(); ++k) {
    opt.arrayGet(k, &obj);
    if (!obj.isString() &&
	(!obj.isArray() || obj.arrayGetLength() != 2 ||
	 !obj.arrayGet(0, &obj2)->isString() ||
	 !obj.arrayGet(1, &obj3)->isString())) {
      error (-1, "Invalid option array for choice widget.");
      obj3.free();
      obj2.free();
      obj.free();
      opt.free();
      return;
    }
    obj3.free();
    obj2.free();
    obj.free();
  }

  attrs->getValue(&obj);
  if (obj.isName()) {
    value = new GString(obj.getName());
  } else if (obj.isString()) {
    value = obj.getString()->copy();
  }
  obj.free();

  attrs->getDefaultValue(&obj);
  if (obj.isName()) {
    defaultValue = new GString(obj.getName());
  } else if (obj.isString()) {
    defaultValue = obj.getString()->copy();
  }
  obj.free();

  topIndex = attrs->getTopIndex();
  if (topIndex < 0)
    topIndex = 0;
  else if (topIndex >= opt.arrayGetLength())
    topIndex = opt.arrayGetLength() - 1;

  buildAppearance();
}

ChoiceField::~ChoiceField() {
  opt.free();
  delete value;
  delete defaultValue;
}

void ChoiceField::reset() {
  delete value;
  value = NULL;
  if (defaultValue)
    value = defaultValue->copy();
  rebuildAppearance();
}

GBool ChoiceField::doSubmit(SubmitContext *context) {
  if (value) {
    context->addValue(value);
    return gTrue;
  }
  return gFalse;
}

GBool ChoiceField::hasValue() {
  return value != NULL;
}

GBool ChoiceField::import(Dict *dict) { DEBUG_INFO
  Object obj, obj2, obj3, obj4;
  int k;
  GBool ok;

  Field::import(dict);

  if (dict->lookup("V", &obj)->isString() || obj.isName()) {
    delete value;
    value = NULL; // int case of an exception
    if (obj.isName())
      value = new GString(obj.getName());
    else
      value = obj.getString()->copy();
  }
  obj.free();

  if (dict->lookup("Opt", &obj4)->isArray()) {
    ok = gTrue;
    for (k = 0; k < obj4.arrayGetLength(); ++k) {
      obj4.arrayGet(k, &obj);
      if (!obj.isString() &&
	  (!obj.isArray() || obj.arrayGetLength() != 2 ||
	   !obj.arrayGet(0, &obj2)->isString() ||
	   !obj.arrayGet(1, &obj3)->isString())) {
	error (-1, "Invalid option array for choice widget.");
	ok = gFalse;
      }
      obj3.free();
      obj2.free();
      obj.free();
    }
    if (ok) {
      opt.free();
      obj4.copy(&opt);
    }
  }
  obj4.free();

  rebuildAppearance();

  return gTrue;
}

Action *ChoiceField::down(ActionContext *context) {
  if (!isReadOnly())
    context->editChoice(this);
  return NULL;
}

int ChoiceField::getNumEntries() {
  return opt.arrayGetLength();
}

GString *ChoiceField::getEntry(int n) {
  Object obj, obj2;
  GString *str = NULL;
  if (opt.arrayGet(n, &obj)) {
    if (obj.isString()) {
      str = obj.getString()->copy();
    } else if (obj.isArray() && obj.arrayGetLength() == 2) {
      if (obj.arrayGet(0, &obj2)->isString()) {
	str = obj2.getString()->copy();
      }
      obj2.free();
    }
  }
  obj.free();
  return str;
}

/*void ChoiceField::setChoice(int num) { DEBUG_INFO
  if (num >= 0 && num < opt.arrayGetLength()) {
    Object obj, obj2, *p = &obj;
    opt.arrayGet(num, &obj);
    if (obj.isArray() && obj.arrayGetLength() == 2) {
      obj.arrayGet(0, &obj2);
      p = &obj2;
    }
    delete value;
    value = NULL; // exceptions
    value = p->getString()->copy();
    obj2.free();
    obj.free();
  }
}*/

/*void ChoiceField::setChoice(GString *str) {
  delete value;
  value = NULL; // exceptions
  value = str->copy();
}*/

void ChoiceField::update(ActionContext *context, GString *str) {
  delete value;
  value = str;
  rebuildAppearance();
  context->refresh(getAnnot());
}

GString *ChoiceField::getValueAppearance() {
  if (value && value->getLength() > 0) {
    char *str1 = value->getCString();
    int len = value->getLength();
    int k;
    Object obj, obj2;

    // If the value is the first entry of an element of /Opt that is an
    // array of two strings, the appearance is the second entry.
    for (k = 0; k < opt.arrayGetLength(); ++k) {
      if (opt.arrayGet(k, &obj)->isArray() && obj.arrayGetLength() == 2) {
	if (obj.arrayGet(0, &obj2)->isString() &&
	    obj2.getString()->cmp(str1) == 0) {
	  obj2.free();
	  if (obj.arrayGet(1, &obj2)->isString()) {
	    str1 = obj2.getString()->getCString();
	    len = obj2.getString()->getLength();
	    obj.free();
	    break;
	  }
	}
	obj2.free();
      }
      obj.free();
    }

    GString *str = new GString("(");

    for (k = 0; k < len; ++k) {
      char c = str1[k];
      if (c == '(' || c == ')')
	str->append('\\');
      str->append(c);
    }
    str->append(")Tj");

    obj2.free();

    return str;
  } else
    return NULL;
}


//------------------------------------------------------------------------
// TextField
//------------------------------------------------------------------------

TextField::TextField(XRef *xrefA, Dict *dict, FieldAttrs *attrs)
: TextBaseField(xrefA, dict, attrs) { DEBUG_INFO
  Object obj;

  attrs->getValue(&value);
  attrs->getDefaultValue(&defaultValue);
  maxLen = attrs->getMaxLen();

  if (!dict->lookup("AP", &obj)->isDict())
    rebuildAppearance();
  obj.free();
}


TextField::~TextField() {
  value.free();
  defaultValue.free();
}

void TextField::reset() {
  value.free();
  defaultValue.copy(&value);
  rebuildAppearance();
}

GBool TextField::doSubmit(SubmitContext *context) {
  if (value.isString()) {
    context->addValue(value.getString());
    return gTrue;
  }
  return gFalse;
}

GBool TextField::import(Dict *dict) { DEBUG_INFO
  Object obj;

  Field::import(dict);
  if (dict->lookup("V", &obj)->isString()) {
    value.free();
    obj.copy(&value);
    rebuildAppearance();
  }
  obj.free();

  return gTrue;
}

Action *TextField::down(ActionContext *context) {
  if (!isReadOnly())
    context->editText(this);
  return NULL;
}

void TextField::update(ActionContext *context, GString *str) {
  value.free();
  value.initString(str);
  rebuildAppearance();
  context->refresh(getAnnot());
}

GString *TextField::getValueAppearance() {
  if (value.isString() && value.getString()->getLength() > 0) {
    char *str1 = value.getString()->getCString();
    int len = value.getString()->getLength();
    GString *str = new GString("(");
    int k;

    for (k = 0; k < len; ++k) {
      char c = str1[k];
      if (c == '(' || c == ')' || c == '\\')
	str->append('\\');
      if (c == '\n')
	str->append(")Tj T*(");
      else
	str->append(c);
    }
    str->append(")Tj");
  } else
    return NULL;
}


//------------------------------------------------------------------------
// AcroForm
//------------------------------------------------------------------------

AcroForm::AcroForm(XRef *xref, Object *obj) { DEBUG_INFO
  Object obj2, obj3;
  int k, l;
  FieldAttrs *attrs = NULL;

  fields = NULL;
  rootFields = NULL;
  numRootFields = 0;
  numFields = 0;
  maxFields = 0;
  ok = gFalse;

  if (!obj->dictLookup("Fields", &obj2)->isArray()) {
    obj2.free();
    return;
  }

  attrs = new FieldAttrs(NULL, obj->getDict());
  rootFields = new Field * [obj2.arrayGetLength()];

  for (k = 0, l = 0; k < obj2.arrayGetLength(); ++k) {
    rootFields[l] = readFieldTree(xref, obj2.arrayGetNF(k, &obj3), attrs);
//printf("root[%d]=%lx\n",l,rootFields[l]);
    if (rootFields[l])
      ++l;
    obj3.free();
  }
  numRootFields = l;

//for(k=0;k<numFields;++k)
//    printf("%s\n",fields[k]->getName()?fields[k]->getName()->getCString():"??");

  delete attrs;

  obj2.free();

  ok = gTrue;
}

AcroForm::~AcroForm() {
  int k;

  for (k = 0; k < numFields; ++k)
    delete fields[k];

  delete rootFields;

  if (fields)
    gfree(fields);
}

Field *AcroForm::readFieldTree(XRef *xref, Object *ref, FieldAttrs *attrs1) { DEBUG_INFO
  Object obj, obj2;
  FieldAttrs *attrs;
  Field *field = NULL;
  RadioField *radioField = NULL;
  GString *type;
  Object dict;

  ref->fetch(xref, &dict);

  if (!ref->isRef() || !dict.isDict()) {
    error(-1, "Invalid AcroForm field.");
    dict.free();
    return NULL;
  }

  attrs = new FieldAttrs(attrs1, dict.getDict());
  attrs->setRef(ref->getRef());
  if (type = attrs->getTypeName()) {
    if (!type->cmp("Btn")) {
      if (attrs->getFlags() & fldPushButton) {
	field = new PushButtonField(xref, dict.getDict(), attrs);
      } else if (attrs->getFlags() & fldRadio) {
	field = radioField = new RadioField(xref, dict.getDict(), attrs);
      } else {
	field = new CheckboxField(xref, dict.getDict(), attrs);
      }
    } else if(!type->cmp("Ch")) {
      field = new ChoiceField(xref, dict.getDict(), attrs);
    } else if(!type->cmp("Tx")) {
      field = new TextField(xref, dict.getDict(), attrs);
    } else if(!type->cmp("Sig"))
      error(-1, "Signatures not supported.");
    else
      error(-1, "Unknown field type: %s", type->getCString());
  } else
    field = new NodeField(xref, dict.getDict(), attrs);

//printf("field=%lx, name='%s'\n",field,field && field->getName()?field->getName()->getCString():"");

  if (dict.dictLookup("Kids", &obj)->isArray()) {
    int k;
    FieldAttrs *attrs2 = new FieldAttrs(attrs, NULL);
//printf("kid[\n");
    if (radioField)
      attrs2->setFlags(attrs->getFlags() & ~fldRadio);
    if (field)
      attrs2->setName(field->getName());
    for (k = 0; k < obj.arrayGetLength(); ++k) {
      Field *field2 = readFieldTree(xref, obj.arrayGetNF(k, &obj2), attrs2);
      if (field2 && field)
//printf("added %lx to %lx\n",field2,field),
	field->addChild(field2);
      if (field2 && radioField && field2->getType() == fieldCheckbox) {
	radioField->addChoice((CheckboxField *)field2);
      }
      obj2.free();
    }
    delete attrs2;
//printf("]\n");
  }
  obj.free();

  if (field && !field->isOk()) {
    error(-1, "Invalid Widget.");
    delete field;
    field = NULL;
  }
  delete attrs;

  if (field)
    addField(field);

  dict.free();

//printf("-> %lx\n",field);
  return field;
}

void AcroForm::addField(Field *field) {
  if (numFields == maxFields) {
    maxFields += 16;
    fields = (Field**)grealloc(fields, maxFields * sizeof(*fields));
  }
  fields[numFields++] = field;
}

Field *AcroForm::findField(int num, int gen) {
  int k;

  for (k = 0; k < numFields; ++k)
    if (fields[k]->getRefNum() == num &&
	fields[k]->getRefGen() == gen)
      return fields[k];
  return NULL;
}

Field *AcroForm::findField(GString *name) {
  int k;

  for (k = 0; k < numFields; ++k)
    if (!name->cmp(fields[k]->getName()))
      return fields[k];
  return NULL;
}

GBool AcroForm::importFDF(Dict *dict) { DEBUG_INFO
  Object obj, obj2;
  int k;
  GBool ok = gTrue;

  if (dict->lookup("Fields", &obj)->isArray()) {
//printf("got fields\n");
    for (k = 0; k < obj.arrayGetLength(); ++k) {
      if (!importField(NULL, obj.arrayGet(k, &obj2)))
	ok = gFalse;
      obj2.free();
    }
  }
  obj.free();

  return ok;
}

GBool AcroForm::importField(GString *name, Object *dict) { DEBUG_INFO
  Object obj, obj2, obj3;
  GBool ok = gTrue;
  GString name2;
  Field *field;
  int k;

//printf("importField(%s)\n",name?name->getCString():"NULL");
  if (!dict->isDict()) {
    error(-1, "Field is not a dictionary.");
    return gFalse;
  }

  if (dict->dictLookup("T", &obj)->isString()) {
//printf("T=\"%s\"\n",obj.getString()->getCString());
    if (name) {
      name2.append(name);
      name2.append('.');
    }
    name2.append(obj.getString());
//printf("name2=\"%s\"\n",name2.getCString());

    if (field = findField(&name2)) {
//printf("field=%lx\n",field);
      field->import(dict->getDict());

      if (dict->dictLookup("Kids", &obj2)->isArray()) {
	for (k = 0; k < obj2.arrayGetLength(); ++k) {
	  if (!importField(&name2, obj2.arrayGet(k, &obj3)))
	    ok = gFalse;
	  obj3.free();
	}
      }
      obj2.free();
    }

  } else {
    error(-1, "Missing field name.");
    ok = gFalse;
  }
  obj.free();

  return ok;
}

GBool AcroForm::importFDF(GString *fileName) {
  Object catalog, obj;
  FDFForm *fdfForm = new FDFForm(fileName);
  GBool ok = gFalse;

  if (fdfForm && fdfForm->isOk()) {
    if (fdfForm->getCatalog(&catalog)->isDict()) {
      if (catalog.dictLookup("FDF", &obj)->isDict()) {
	importFDF(obj.getDict());
	ok = gTrue;
      }
      obj.free();
    }
    catalog.free();
  }
  delete fdfForm;

  return ok;
}

GBool AcroForm::exportFDF(GString *fileName) {
  int k;
  int num = getNumRootFields();

  SubmitFDFContext submitContext(fileName->getCString());

  for (k = 0; k < num; ++k)
    getRootField(k)->setExportFlags(gTrue);

  for (k = 0; k < num; ++k)
    getRootField(k)->submit(&submitContext);

  delete fileName;

  return gTrue; //~
}


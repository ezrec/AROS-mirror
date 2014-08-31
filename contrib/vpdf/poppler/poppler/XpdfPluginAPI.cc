//========================================================================
//
// XpdfPluginAPI.cc
//
// Copyright 2004 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2009 Kovid Goyal <kovid@kovidgoyal.net>
// Copyright (C) 2012 Albert Astals Cid <aacid@kde.org>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include "config.h"

#ifdef ENABLE_PLUGINS

#include "gmem.h"
#include "GlobalParams.h"
#include "Object.h"
#include "PDFDoc.h"
#include "XpdfPluginAPI.h"

//------------------------------------------------------------------------

//~ This should use a pool of Objects; change xpdfFreeObj to match.
static PObject *allocObj() {
  return (PObject *)gmalloc(sizeof(PObject));
}

//------------------------------------------------------------------------
// Document access functions
//------------------------------------------------------------------------

XpdfObject _xpdfGetInfoDict(XpdfDoc doc) {
  PObject *obj;

  obj = allocObj();
  return (XpdfObject)((PDFDoc *)doc)->getDocInfo(obj);
}

XpdfObject _xpdfGetCatalog(XpdfDoc doc) {
  PObject *obj;

  obj = allocObj();
  return (XpdfObject)((PDFDoc *)doc)->getXRef()->getCatalog(obj);
}

//------------------------------------------------------------------------
// PObject access functions.
//------------------------------------------------------------------------

XpdfBool _xpdfObjIsBool(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isBool();
}

XpdfBool _xpdfObjIsInt(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isInt();
}

XpdfBool _xpdfObjIsReal(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isReal();
}

XpdfBool _xpdfObjIsNumber(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isNum();
}

XpdfBool _xpdfObjIsString(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isString();
}

XpdfBool _xpdfObjIsName(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isName();
}

XpdfBool _xpdfObjIsNull(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isNull();
}

XpdfBool _xpdfObjIsArray(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isArray();
}

XpdfBool _xpdfObjIsDict(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isDict();
}

XpdfBool _xpdfObjIsStream(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isStream();
}

XpdfBool _xpdfObjIsRef(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->isRef();
}

XpdfBool _xpdfBoolValue(XpdfObject obj) {
  return (XpdfBool)((PObject *)obj)->getBool();
}

int _xpdfIntValue(XpdfObject obj) {
  if (!((PObject *)obj)->isInt()) {
    return 0;
  }
  return ((PObject *)obj)->getInt();
}

double _xpdfRealValue(XpdfObject obj) {
  if (!((PObject *)obj)->isReal()) {
    return 0;
  }
  return ((PObject *)obj)->getReal();
}

double _xpdfNumberValue(XpdfObject obj) {
  if (!((PObject *)obj)->isNum()) {
    return 0;
  }
  return ((PObject *)obj)->getNum();
}

int _xpdfStringLength(XpdfObject obj) {
  if (!((PObject *)obj)->isString()) {
    return 0;
  }
  return ((PObject *)obj)->getString()->getLength();
}

char *_xpdfStringValue(XpdfObject obj) {
  if (!((PObject *)obj)->isString()) {
    return 0;
  }
  return ((PObject *)obj)->getString()->getCString();
}

char *_xpdfNameValue(XpdfObject obj) {
  if (!((PObject *)obj)->isName()) {
    return NULL;
  }
  return ((PObject *)obj)->getName();
}

int _xpdfArrayLength(XpdfObject obj) {
  if (!((PObject *)obj)->isArray()) {
    return 0;
  }
  return ((PObject *)obj)->arrayGetLength();
}

XpdfObject _xpdfArrayGet(XpdfObject obj, int idx) {
  PObject *elem;

  elem = allocObj();
  if (!((PObject *)obj)->isArray()) {
    return (XpdfObject)elem->initNull();
  }
  return (XpdfObject)((PObject *)obj)->arrayGet(idx, elem);
}

XpdfObject _xpdfDictGet(XpdfObject obj, char *key) {
  PObject *elem;

  elem = allocObj();
  if (!((PObject *)obj)->isDict()) {
    return (XpdfObject)elem->initNull();
  }
  return (XpdfObject)((PObject *)obj)->dictLookup(key, elem);
}

void _xpdfFreeObj(XpdfObject obj) {
  ((PObject *)obj)->free();
  gfree(obj);
}

//------------------------------------------------------------------------
// Memory allocation functions
//------------------------------------------------------------------------

void *_xpdfMalloc(int size) {
  return gmalloc(size);
}

void *_xpdfRealloc(void *p, int size) {
  return grealloc(p, size);
}

void _xpdfFree(void *p) {
  gfree(p);
}

//------------------------------------------------------------------------
// Security handlers
//------------------------------------------------------------------------

void _xpdfRegisterSecurityHandler(XpdfSecurityHandler *handler) {
  if (handler->version <= xpdfPluginAPIVersion) {
    globalParams->addSecurityHandler(handler);
  }
}

//------------------------------------------------------------------------

XpdfPluginVecTable xpdfPluginVecTable = {
  xpdfPluginAPIVersion,
  &_xpdfGetInfoDict,
  &_xpdfGetCatalog,
  &_xpdfObjIsBool,
  &_xpdfObjIsInt,
  &_xpdfObjIsReal,
  &_xpdfObjIsString,
  &_xpdfObjIsName,
  &_xpdfObjIsNull,
  &_xpdfObjIsArray,
  &_xpdfObjIsDict,
  &_xpdfObjIsStream,
  &_xpdfObjIsRef,
  &_xpdfBoolValue,
  &_xpdfIntValue,
  &_xpdfRealValue,
  &_xpdfStringLength,
  &_xpdfStringValue,
  &_xpdfNameValue,
  &_xpdfArrayLength,
  &_xpdfArrayGet,
  &_xpdfDictGet,
  &_xpdfFreeObj,
  &_xpdfMalloc,
  &_xpdfRealloc,
  &_xpdfFree,
  &_xpdfRegisterSecurityHandler,
};

#endif // ENABLE_PLUGINS

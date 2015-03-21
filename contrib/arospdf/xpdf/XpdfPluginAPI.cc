//========================================================================
//
// XpdfPluginAPI.cc
//
// Copyright 2004 Glyph & Cog, LLC
//
//========================================================================

#include "aconf.h"

#ifdef ENABLE_PLUGINS

#include "gmem.h"
#include "GlobalParams.h"
#include "Object.h"
#include "PDFDoc.h"
#ifdef WIN32
#include "WinPDFCore.h"
#else
#include "XPDFCore.h"
#endif
#include "XpdfPluginAPI.h"

//------------------------------------------------------------------------

//~ This should use a pool of xObjects; change xpdfFreeObj to match.
static xObject *allocObj() {
  return (xObject *)gmalloc(sizeof(xObject));
}

//------------------------------------------------------------------------
// Document access functions
//------------------------------------------------------------------------

XpdfObject _xpdfGetInfoDict(XpdfDoc doc) {
  xObject *obj;

  obj = allocObj();
  return (XpdfObject)((PDFDoc *)doc)->getDocInfo(obj);
}

XpdfObject _xpdfGetCatalog(XpdfDoc doc) {
  xObject *obj;

  obj = allocObj();
  return (XpdfObject)((PDFDoc *)doc)->getXRef()->getCatalog(obj);
}

#ifdef _WIN32

HWND _xpdfWin32GetWindow(XpdfDoc doc) {
  WinPDFCore *core;

  if (!(core = (WinPDFCore *)((PDFDoc *)doc)->getGUIData())) {
    return NULL;
  }
  return core->getDrawFrame();
}

#else

Widget _xpdfXGetWindow(XpdfDoc doc) {
  XPDFCore *core;

  if (!(core = (XPDFCore *)((PDFDoc *)doc)->getGUIData())) {
    return NULL;
  }
  return core->getWidget();
}

#endif

//------------------------------------------------------------------------
// xObject access functions.
//------------------------------------------------------------------------

XpdfBool _xpdfObjIsBool(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isBool();
}

XpdfBool _xpdfObjIsInt(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isInt();
}

XpdfBool _xpdfObjIsReal(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isReal();
}

XpdfBool _xpdfObjIsNumber(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isNum();
}

XpdfBool _xpdfObjIsString(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isString();
}

XpdfBool _xpdfObjIsName(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isName();
}

XpdfBool _xpdfObjIsNull(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isNull();
}

XpdfBool _xpdfObjIsArray(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isArray();
}

XpdfBool _xpdfObjIsDict(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isDict();
}

XpdfBool _xpdfObjIsStream(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isStream();
}

XpdfBool _xpdfObjIsRef(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->isRef();
}

XpdfBool _xpdfBoolValue(XpdfObject obj) {
  return (XpdfBool)((xObject *)obj)->getBool();
}

int _xpdfIntValue(XpdfObject obj) {
  if (!((xObject *)obj)->isInt()) {
    return 0;
  }
  return ((xObject *)obj)->getInt();
}

double _xpdfRealValue(XpdfObject obj) {
  if (!((xObject *)obj)->isReal()) {
    return 0;
  }
  return ((xObject *)obj)->getReal();
}

double _xpdfNumberValue(XpdfObject obj) {
  if (!((xObject *)obj)->isNum()) {
    return 0;
  }
  return ((xObject *)obj)->getNum();
}

int _xpdfStringLength(XpdfObject obj) {
  if (!((xObject *)obj)->isString()) {
    return 0;
  }
  return ((xObject *)obj)->getString()->getLength();
}

char *_xpdfStringValue(XpdfObject obj) {
  if (!((xObject *)obj)->isString()) {
    return 0;
  }
  return ((xObject *)obj)->getString()->getCString();
}

char *_xpdfNameValue(XpdfObject obj) {
  if (!((xObject *)obj)->isName()) {
    return NULL;
  }
  return ((xObject *)obj)->getName();
}

int _xpdfArrayLength(XpdfObject obj) {
  if (!((xObject *)obj)->isArray()) {
    return 0;
  }
  return ((xObject *)obj)->arrayGetLength();
}

XpdfObject _xpdfArrayGet(XpdfObject obj, int idx) {
  xObject *elem;

  elem = allocObj();
  if (!((xObject *)obj)->isArray()) {
    return (XpdfObject)elem->initNull();
  }
  return (XpdfObject)((xObject *)obj)->arrayGet(idx, elem);
}

XpdfObject _xpdfDictGet(XpdfObject obj, char *key) {
  xObject *elem;

  elem = allocObj();
  if (!((xObject *)obj)->isDict()) {
    return (XpdfObject)elem->initNull();
  }
  return (XpdfObject)((xObject *)obj)->dictLookup(key, elem);
}

void _xpdfFreeObj(XpdfObject obj) {
  ((xObject *)obj)->free();
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
#ifdef _WIN32
  &_xpdfWin32GetWindow,
#else
  &_xpdfXGetWindow,
#endif
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

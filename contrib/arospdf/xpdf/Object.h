//========================================================================
//
// xObject.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef xObject_H
#define xObject_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <stdio.h>
#include <string.h>
#include "gtypes.h"
#include "gmem.h"
#include "GString.h"

class XRef;
class Array;
class Dict;
class Stream;

//------------------------------------------------------------------------
// Ref
//------------------------------------------------------------------------

struct Ref {
  int num;			// xObject number
  int gen;			// generation number
};

//------------------------------------------------------------------------
// xObject types
//------------------------------------------------------------------------

enum ObjType {
  // simple xObjects
  objBool,			// boolean
  objInt,			// integer
  objReal,			// real
  objString,			// string
  objName,			// name
  objNull,			// null

  // complex xObjects
  objArray,			// array
  objDict,			// dictionary
  objStream,			// stream
  objRef,			// indirect reference

  // special xObjects
  objCmd,			// command name
  objError,			// error return from Lexer
  objEOF,			// end of file return from Lexer
  objNone			// uninitialized xObject
};

#define numObjTypes 14		// total number of xObject types

//------------------------------------------------------------------------
// xObject
//------------------------------------------------------------------------

#ifdef DEBUG_MEM
#define initObj(t) ++numAlloc[type = t]
#else
#define initObj(t) type = t
#endif

class xObject {
public:

  // Default constructor.
  xObject():
    type(objNone) {}

  // Initialize an xObject.
  xObject *initBool(GBool boolnA)
    { initObj(objBool); booln = boolnA; return this; }
  xObject *initInt(int intgA)
    { initObj(objInt); intg = intgA; return this; }
  xObject *initReal(double realA)
    { initObj(objReal); real = realA; return this; }
  xObject *initString(GString *stringA)
    { initObj(objString); string = stringA; return this; }
  xObject *initName(char *nameA)
    { initObj(objName); name = copyString(nameA); return this; }
  xObject *initNull()
    { initObj(objNull); return this; }
  xObject *initArray(XRef *xref);
  xObject *initDict(XRef *xref);
  xObject *initDict(Dict *dictA);
  xObject *initStream(Stream *streamA);
  xObject *initRef(int numA, int genA)
    { initObj(objRef); ref.num = numA; ref.gen = genA; return this; }
  xObject *initCmd(char *cmdA)
    { initObj(objCmd); cmd = copyString(cmdA); return this; }
  xObject *initError()
    { initObj(objError); return this; }
  xObject *initEOF()
    { initObj(objEOF); return this; }

  // Copy an xObject.
  xObject *copy(xObject *obj);

  // If xObject is a Ref, fetch and return the referenced xObject.
  // Otherwise, return a copy of the xObject.
  xObject *fetch(XRef *xref, xObject *obj);

  // Free xObject contents.
  void free();

  // Type checking.
  ObjType getType() { return type; }
  GBool isBool() { return type == objBool; }
  GBool isInt() { return type == objInt; }
  GBool isReal() { return type == objReal; }
  GBool isNum() { return type == objInt || type == objReal; }
  GBool isString() { return type == objString; }
  GBool isName() { return type == objName; }
  GBool isNull() { return type == objNull; }
  GBool isArray() { return type == objArray; }
  GBool isDict() { return type == objDict; }
  GBool isStream() { return type == objStream; }
  GBool isRef() { return type == objRef; }
  GBool isCmd() { return type == objCmd; }
  GBool isError() { return type == objError; }
  GBool isEOF() { return type == objEOF; }
  GBool isNone() { return type == objNone; }

  // Special type checking.
  GBool isName(char *nameA)
    { return type == objName && !strcmp(name, nameA); }
  GBool isDict(char *dictType);
  GBool isStream(char *dictType);
  GBool isCmd(char *cmdA)
    { return type == objCmd && !strcmp(cmd, cmdA); }

  // Accessors.  NB: these assume xObject is of correct type.
  GBool getBool() { return booln; }
  int getInt() { return intg; }
  double getReal() { return real; }
  double getNum() { return type == objInt ? (double)intg : real; }
  GString *getString() { return string; }
  char *getName() { return name; }
  Array *getArray() { return array; }
  Dict *getDict() { return dict; }
  Stream *getStream() { return stream; }
  Ref getRef() { return ref; }
  int getRefNum() { return ref.num; }
  int getRefGen() { return ref.gen; }
  char *getCmd() { return cmd; }

  // Array accessors.
  int arrayGetLength();
  void arrayAdd(xObject *elem);
  xObject *arrayGet(int i, xObject *obj);
  xObject *arrayGetNF(int i, xObject *obj);

  // Dict accessors.
  int dictGetLength();
  void dictAdd(char *key, xObject *val);
  GBool dictIs(char *dictType);
  xObject *dictLookup(char *key, xObject *obj);
  xObject *dictLookupNF(char *key, xObject *obj);
  char *dictGetKey(int i);
  xObject *dictGetVal(int i, xObject *obj);
  xObject *dictGetValNF(int i, xObject *obj);

  // Stream accessors.
  GBool streamIs(char *dictType);
  void streamReset();
  void streamClose();
  int streamGetChar();
  int streamLookChar();
  char *streamGetLine(char *buf, int size);
  Guint streamGetPos();
  void streamSetPos(Guint pos, int dir = 0);
  Dict *streamGetDict();

  // Output.
  char *getTypeName();
  void print(FILE *f = stdout);

  // Memory testing.
  static void memCheck(FILE *f);

private:

  ObjType type;			// xObject type
  union {			// value for each type:
    GBool booln;		//   boolean
    int intg;			//   integer
    double real;		//   real
    GString *string;		//   string
    char *name;			//   name
    Array *array;		//   array
    Dict *dict;			//   dictionary
    Stream *stream;		//   stream
    Ref ref;			//   indirect reference
    char *cmd;			//   command
  };

#ifdef DEBUG_MEM
  static int			// number of each type of xObject
    numAlloc[numObjTypes];	//   currently allocated
#endif
};

//------------------------------------------------------------------------
// Array accessors.
//------------------------------------------------------------------------

#include "Array.h"

inline int xObject::arrayGetLength()
  { return array->getLength(); }

inline void xObject::arrayAdd(xObject *elem)
  { array->add(elem); }

inline xObject *xObject::arrayGet(int i, xObject *obj)
  { return array->get(i, obj); }

inline xObject *xObject::arrayGetNF(int i, xObject *obj)
  { return array->getNF(i, obj); }

//------------------------------------------------------------------------
// Dict accessors.
//------------------------------------------------------------------------

#include "Dict.h"

inline int xObject::dictGetLength()
  { return dict->getLength(); }

inline void xObject::dictAdd(char *key, xObject *val)
  { dict->add(key, val); }

inline GBool xObject::dictIs(char *dictType)
  { return dict->is(dictType); }

inline GBool xObject::isDict(char *dictType)
  { return type == objDict && dictIs(dictType); }

inline xObject *xObject::dictLookup(char *key, xObject *obj)
  { return dict->lookup(key, obj); }

inline xObject *xObject::dictLookupNF(char *key, xObject *obj)
  { return dict->lookupNF(key, obj); }

inline char *xObject::dictGetKey(int i)
  { return dict->getKey(i); }

inline xObject *xObject::dictGetVal(int i, xObject *obj)
  { return dict->getVal(i, obj); }

inline xObject *xObject::dictGetValNF(int i, xObject *obj)
  { return dict->getValNF(i, obj); }

//------------------------------------------------------------------------
// Stream accessors.
//------------------------------------------------------------------------

#include "Stream.h"

inline GBool xObject::streamIs(char *dictType)
  { return stream->getDict()->is(dictType); }

inline GBool xObject::isStream(char *dictType)
  { return type == objStream && streamIs(dictType); }

inline void xObject::streamReset()
  { stream->reset(); }

inline void xObject::streamClose()
  { stream->close(); }

inline int xObject::streamGetChar()
  { return stream->getChar(); }

inline int xObject::streamLookChar()
  { return stream->lookChar(); }

inline char *xObject::streamGetLine(char *buf, int size)
  { return stream->getLine(buf, size); }

inline Guint xObject::streamGetPos()
  { return stream->getPos(); }

inline void xObject::streamSetPos(Guint pos, int dir)
  { stream->setPos(pos, dir); }

inline Dict *xObject::streamGetDict()
  { return stream->getDict(); }

#endif

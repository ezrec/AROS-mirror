//========================================================================
//
// XRef.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "gmem.h"
#include "Object.h"
#include "Stream.h"
#include "Lexer.h"
#include "Parser.h"
#include "Dict.h"
#include "Error.h"
#include "ErrorCodes.h"
#include "XRef.h"
#include "PDFDoc.h"
#include "Hints.h"
#include "PartsVec.h"
#include "SecurityHandler.h"

//------------------------------------------------------------------------

#define xrefSearchSize 1024	// read this many bytes at end of file
				//   to look for 'startxref'

//------------------------------------------------------------------------
// Permission bits
//------------------------------------------------------------------------

#define permPrint    (1<<2)
#define permChange   (1<<3)
#define permCopy     (1<<4)
#define permNotes    (1<<5)
#define defPermFlags 0xfffc

//------------------------------------------------------------------------
// JoinStream
//------------------------------------------------------------------------

class JoinStream: public Stream {
public:

  JoinStream(Object *s1, Object *s2) {
    s1->copy(&str1);
    s2->copy(&str2);
    str = str1.getStream();
  }
  ~JoinStream() {
    str1.free();
    str2.free();
  }
  virtual StreamKind getKind() { return strWeird; }
  virtual void reset() {
    str1.streamReset();
    if (str2.isStream())
      str2.streamReset();
    str = str1.getStream();
  }
  virtual int getChar() {
    int c = str->getChar();
    if (c == EOF && str2.isStream() && str != str2.getStream()) {
      str = str2.getStream();
      c = str->getChar();
    }
    return c;
  }
  virtual int lookChar() {
    int c = str->lookChar();
    if (c == EOF && str2.isStream() && str != str2.getStream()) {
      c = str2.streamLookChar();
    }
    return c;
  }
  virtual int getPos() { return 0; }
  virtual void setPos(Guint pos1, int) {}
  virtual GBool isBinary(GBool last = gTrue) { return gTrue; }
  virtual BaseStream *getBaseStream() { return NULL; }
  virtual Dict *getDict() { return NULL; }

private:
  Stream *str;
  Object str1;
  Object str2;
};


//------------------------------------------------------------------------
// ObjectStream
//------------------------------------------------------------------------

class ObjectStream {
public:

  // Create an object stream, using object number <objStrNum>,
  // generation 0.
  ObjectStream(XRef *xref, int objStrNumA);

  ~ObjectStream();

  // Return the object number of this object stream.
  //int getObjStrNum() { return objStrNum; }

  // Get the <objIdx>th object from this stream, which should be
  // object number <objNum>, generation 0.
  Object *getObject(int objIdx, int objNum, Object *obj);

  // Get the <objIdx>th object from this stream.
  Object *getObject(int objIdx, Object *obj);

private:
  //int objStrNum;		// object number of the object stream
  int nObjects;			// number of objects in the stream
  Object *objs;			// the objects (length = nObjects)
  int *objNums;			// the object numbers (length = nObjects)
};

ObjectStream::ObjectStream(XRef *xref, int objStrNum) {
  DEBUG_INFO
  Stream *str;
  Parser *parser;
  int *offsets;
  Object objStr, obj1, obj2;
  int first, i;

  //objStrNum = objStrNumA;
  nObjects = 0;
  objs = NULL;
  objNums = NULL;

  if (!xref->fetch(objStrNum, 0, &objStr)->isStream()) {
    goto err1;
  }

  if (!objStr.streamGetDict()->lookup("N", &obj1)->isInt()) {
    obj1.free();
    goto err1;
  }
  nObjects = obj1.getInt();
  obj1.free();
  if (nObjects <= 0) {
    goto err1;
  }

  if (!objStr.streamGetDict()->lookup("First", &obj1)->isInt()) {
    obj1.free();
    goto err1;
  }
  first = obj1.getInt();
  obj1.free();
  if (first < 0) {
    goto err1;
  }

  objs = new Object[nObjects];
  objNums = (int *)gmallocn(nObjects, sizeof(int));
  offsets = (int *)gmallocn(nObjects, sizeof(int));

  // parse the header: object numbers and offsets
  objStr.streamReset();
  obj1.initNull();
  str = new EmbedStream(objStr.getStream(), &obj1, gTrue, first);
  parser = new Parser(xref, new Lexer(xref, str));
  for (i = 0; i < nObjects; ++i) {
    parser->getObj(&obj1);
    parser->getObj(&obj2);
    if (!obj1.isInt() || !obj2.isInt()) {
      obj1.free();
      obj2.free();
      delete parser;
      gfree(offsets);
      goto err1;
    }
    objNums[i] = obj1.getInt();
    offsets[i] = obj2.getInt();
    obj1.free();
    obj2.free();
    if (objNums[i] < 0 || offsets[i] < 0 ||
	(i > 0 && offsets[i] < offsets[i-1])) {
      delete parser;
      gfree(offsets);
      goto err1;
    }
  }
  while (str->getChar() != EOF) ;
  delete parser;

  // skip to the first object - this shouldn't be necessary because
  // the First key is supposed to be equal to offsets[0], but just in
  // case...
  for (i = first; i < offsets[0]; ++i) {
    objStr.getStream()->getChar();
  }

  // parse the objects
  for (i = 0; i < nObjects; ++i) {
    obj1.initNull();
    if (i == nObjects - 1) {
      str = new EmbedStream(objStr.getStream(), &obj1, gFalse, 0);
    } else {
      str = new EmbedStream(objStr.getStream(), &obj1, gTrue,
			    offsets[i+1] - offsets[i]);
    }
    parser = new Parser(xref, new Lexer(xref, str));
    parser->getObj(&objs[i]);
    while (str->getChar() != EOF) ;
    delete parser;
  }

  gfree(offsets);

 err1:
  objStr.free();
  return;
}

ObjectStream::~ObjectStream() {
  int i;

  if (objs) {
    for (i = 0; i < nObjects; ++i) {
      objs[i].free();
    }
    delete[] objs;
  }
  gfree(objNums);
}

Object *ObjectStream::getObject(int objIdx, int objNum, Object *obj) {
  if (objIdx < 0 || objIdx >= nObjects || objNum != objNums[objIdx]) {
    return obj->initNull();
  }
  return objs[objIdx].copy(obj);
}

Object *ObjectStream::getObject(int objIdx, Object *obj) {
  if (objIdx < 0 || objIdx >= nObjects) {
    return obj->initNull();
  }
  return objs[objIdx].copy(obj);
}

//------------------------------------------------------------------------
// ObjectStreamCache
//------------------------------------------------------------------------

class ObjectStreamCache
{
public:

  ObjectStreamCache();
  ~ObjectStreamCache();

  ObjectStream *getObjectStream(XRef *xref, int objStrNum);

private:
  static const int cacheSize = 16;
  struct Entry
  {
    Entry *next;
    int objStrNum;
    ObjectStream *objStr;
  };

  Entry *first;
  Entry entries[cacheSize];
};

ObjectStreamCache::ObjectStreamCache() {
  int k;
  Entry **p = &first;

  for (k = 0; k < cacheSize; ++k) {
    *p = &entries[k];
    p = &entries[k].next;
    entries[k].objStrNum = -1;
    entries[k].objStr = NULL;
  }

  *p = NULL;
}

ObjectStreamCache::~ObjectStreamCache() {
  int k;

  for (k = 0; k < cacheSize; ++k) {
    delete entries[k].objStr;
  }
}

ObjectStream *ObjectStreamCache::getObjectStream(XRef *xref, int objStrNum) {
  Entry **ptr = &first;
  Entry *last = NULL;
  Entry *entry;

  while ((entry = *ptr) != NULL && entry->objStrNum != objStrNum && entry->objStr) {
    ptr = &entry->next;
    last = entry;
  }

  if (entry == NULL || entry->objStr == NULL) {
    if (!entry) {
      entry = last;
      delete entry->objStr;
    }

    entry->objStrNum = objStrNum;
    entry->objStr = new ObjectStream(xref, objStrNum);
  } else if (entry != first) {
    *ptr = entry->next;
    entry->next = first;
    first = entry;
  }

  return entry->objStr;
}

//------------------------------------------------------------------------
// XRef
//------------------------------------------------------------------------

XRef::XRef(BaseStream *strA, PDFDoc *doc, GString *ownerPassword, GString *userPassword) {
  DEBUG_INFO
  Guint pos;
  Object obj;

  ok = gTrue;
  errCode = errNone;
  size = 0;
  entries = NULL;
  streamEnds = NULL;
  streamEndsLen = 0;

  pageOffsetHintTable = NULL;
  sharedObjectHintTable = NULL;
  outlinesHintTable = NULL;
  namedDestsHintTable = NULL;
  infoHintTable = NULL;
  formsHintTable = NULL;
  numFirstPageXRefs = -1;

  strCache = new ObjectStreamCache;

  encrypted = gFalse;
  permFlags = defPermFlags;
  ownerPasswordOk = gFalse;

  // read the trailer
  str = strA;

  if (doc) {
    doc->setXRef(this);
  }

  start = str->getStart();

  if (doc && getLinearizationInfo()) {
    str->preRead(0,endOfFirstPageOffset);
    pos = str->getPos();
    if (readXRef(&pos)) {
      if (readTrailerDict()) {
	Object hintStr;
	int i;
	Parser *parser;

	trailerDict.getDict()->setXRef(this);

	// check for encryption
	encrypted = gFalse;
	if (!checkEncrypted(doc, ownerPassword, userPassword)) {
	  ok = gFalse;
	  errCode = errEncrypted;
	  return;
	}

	str->preRead(hintTable1Offset, hintTable1Length);
	if (hintTable2Length)
	  str->preRead(hintTable2Offset, hintTable2Length);

	// hint streams should have the last object numbers,
	// but not all files get that right.
	for (i = size; --i > 0;)
	  if (entries[i].offset == hintTable1Offset)
	    break;

	if (fetch(i, entries[i].gen, &obj)->isStream())
	  obj.copy(&hintStr);
	obj.free();

	if (hintStr.isStream() && hintTable2Length) {
	  for (i = size; --i > 0;)
	    if (entries[i].offset == hintTable2Offset)
	      break;
	  if (fetch(i, entries[i].gen, &obj)->isStream()) {
	    Stream *hintStr2 = new JoinStream(&hintStr, &obj);
	    hintStr.free();
	    hintStr.initStream(hintStr2);
	  }
	  obj.free();
	}

	if (hintStr.isStream() && (hintStr.streamReset(), hintStr.streamLookChar()) != EOF) {
	  Dict *dict = hintStr.streamGetDict();
	  int offset = 0;

	  if (dict->lookup("P", &obj)->isInt())
	    offset = obj.getInt();
	  obj.free();

	  hintStr.streamReset();
	  for (i = 0; i < offset; ++i)
	    hintStr.streamGetChar();

	  pageOffsetHintTable = new PageOffsetHintTable(hintStr.getStream(), numPages,
							firstPageObjectNum,
							hintTable1Offset, hintTable1Length);

	  if (dict->lookup("S", &obj)->isInt()) {
	    int pos = obj.getInt();
	    hintStr.streamReset();
	    for (i = 0; i < pos; ++i)
	      hintStr.streamGetChar();
	  }
	  obj.free();
	  sharedObjectHintTable = new SharedObjectHintTable(hintStr.getStream(),
							    firstPageObjectNum, pageOffsetHintTable->getPageOffset(firstPageNum),
							    hintTable1Offset, hintTable1Length);

	  if (dict->lookup("O", &obj)->isInt()) {
	    pos = obj.getInt();
	    hintStr.streamReset();
	    for (i = 0; i < pos; ++i)
	      hintStr.streamGetChar();
	    outlinesHintTable = new GenericHintTable(hintStr.getStream());
	  }
	  obj.free();

	  if (dict->lookup("E", &obj)->isInt()) {
	    pos = obj.getInt();
	    hintStr.streamReset();
	    for (i = 0; i < pos; ++i)
	      hintStr.streamGetChar();
	    namedDestsHintTable = new GenericHintTable(hintStr.getStream());
	  }
	  obj.free();

	  if (dict->lookup("I", &obj)->isInt()) {
	    pos = obj.getInt();
	    hintStr.streamReset();
	    for (i = 0; i < pos; ++i)
	      hintStr.streamGetChar();
	    infoHintTable = new GenericHintTable(hintStr.getStream());
	  }
	  obj.free();

	  if (dict->lookup("V", &obj)->isInt()) {
	    pos = obj.getInt();
	    hintStr.streamReset();
	    for (i = 0; i < pos; ++i)
	      hintStr.streamGetChar();
	    formsHintTable = new FormsHintTable(hintStr.getStream());
	  }
	  obj.free();

	  hintStr.free();

	  str->setPos(start + mainXRefOffset);
	  str->preRead(start + mainXRefOffset, fileSize - mainXRefOffset);

	  parser = new Parser(NULL,
	      new Lexer(NULL,
		str->makeSubStream(start + mainXRefOffset, gFalse, 0, &obj)));
	  if (readXRefPart2(parser, 0, size - numFirstPageXRefs)) {
	    delete parser;
	    return;
	  }
	  delete parser;

	  delete pageOffsetHintTable;
	  delete sharedObjectHintTable;
	  delete outlinesHintTable;
	  delete namedDestsHintTable;
	  delete infoHintTable;
	  delete formsHintTable;
	  pageOffsetHintTable = NULL;
	  sharedObjectHintTable = NULL;
	  outlinesHintTable = NULL;
	  namedDestsHintTable = NULL;
	  infoHintTable = NULL;
	  formsHintTable = NULL;

	} else
	  error(-1, "Bad hint stream");

      } else {
	obj.free();
      }
      gfree(entries);
      entries = NULL;
      size = 0;
      trailerDict.free();
    }
    error(-1, "Bad linearized file");
  }
  str->removeParts();

  linearized = gFalse;

  pos = getStartXref();

  // if there was a problem with the 'startxref' position, try to
  // reconstruct the xref table
  if (pos == 0) {
    if (!(ok = constructXRef())) {
      errCode = errDamaged;
      return;
    }

  // read the xref table
  } else {
    while (readXRef(&pos)) ;

    // if there was a problem with the xref table,
    // try to reconstruct it
    if (!ok) {
      if (!(ok = constructXRef())) {
	errCode = errDamaged;
	return;
      }
    }
  }

  // get the root dictionary (catalog) object
  trailerDict.dictLookupNF("Root", &obj);
  if (obj.isRef()) {
    rootNum = obj.getRefNum();
    rootGen = obj.getRefGen();
    obj.free();
  } else {
    obj.free();
    if (!(ok = constructXRef())) {
      errCode = errDamaged;
      return;
    }
  }

  // now set the trailer dictionary's xref pointer so we can fetch
  // indirect objects from it
  trailerDict.getDict()->setXRef(this);

  // check for encryption
  if (doc) {
    encrypted = gFalse;
    if (!checkEncrypted(doc, ownerPassword, userPassword)) {
      ok = gFalse;
      errCode = errEncrypted;
      return;
    }
  }
}

#if 0
XRef::XRef(BaseStream *strA) {
  DEBUG_INFO
  Guint pos;
  Object obj;

  ok = gTrue;
  errCode = errNone;
  size = 0;
  entries = NULL;
  streamEnds = NULL;
  streamEndsLen = 0;

  pageOffsetHintTable = NULL;
  sharedObjectHintTable = NULL;
  outlinesHintTable = NULL;
  namedDestsHintTable = NULL;
  infoHintTable = NULL;
  formsHintTable = NULL;
  linearized = gFalse;
  encrypted = gFalse;

  strCache = new ObjectStreamCache;

  // read the trailer
  str = strA;

  start = str->getStart();
  pos = getStartXref();

  // if there was a problem with the 'startxref' position, try to
  // reconstruct the xref table
  if (pos == 0) {
    if (!(ok = constructXRef())) {
      errCode = errDamaged;
      return;
    }

  // read the xref table
  } else {
    while (readXRef(&pos)) ;

    // if there was a problem with the xref table,
    // try to reconstruct it
    if (!ok) {
      if (!(ok = constructXRef())) {
	errCode = errDamaged;
	return;
      }
    }
  }

  // get the root dictionary (catalog) object
  trailerDict.dictLookupNF("Root", &obj);
  if (obj.isRef()) {
    rootNum = obj.getRefNum();
    rootGen = obj.getRefGen();
    obj.free();
  } else {
    obj.free();
    if (!(ok = constructXRef())) {
      errCode = errDamaged;
      return;
    }
  }

  // now set the trailer dictionary's xref pointer so we can fetch
  // indirect objects from it
  trailerDict.getDict()->setXRef(this);
}
#endif

XRef::~XRef() {
  delete pageOffsetHintTable;
  delete sharedObjectHintTable;
  delete outlinesHintTable;
  delete namedDestsHintTable;
  delete infoHintTable;
  delete formsHintTable;
  delete strCache;
  gfree(entries);
  trailerDict.free();
  if (streamEnds) {
    gfree(streamEnds);
  }
}

// Read the 'startxref' position.
Guint XRef::getStartXref() {
  char buf[xrefSearchSize+1];
  char *p;
  int c, n, i;

  // read last xrefSearchSize bytes
  str->setPos(xrefSearchSize, -1);
  for (n = 0; n < xrefSearchSize; ++n) {
    if ((c = str->getChar()) == EOF) {
      break;
    }
    buf[n] = c;
  }
  buf[n] = '\0';

  // find startxref
  for (i = n - 9; i >= 0; --i) {
    if (!strncmp(&buf[i], "startxref", 9)) {
      break;
    }
  }
  if (i < 0) {
    return 0;
  }
  for (p = &buf[i+9]; isspace(*p); ++p) ;
  lastXRefPos = strToUnsigned(p);

  return lastXRefPos;
}

// Read one xref table section.  Also reads the associated trailer
// dictionary, and returns the prev pointer (if any).
GBool XRef::readXRef(Guint *pos) {
  Parser *parser;
  Object obj;
  GBool more;

  // start up a parser, parse one token
  obj.initNull();
  parser = new Parser(NULL,
	     new Lexer(NULL,
	       str->makeSubStream(start + *pos, gFalse, 0, &obj)));
  parser->getObj(&obj);

  // parse an old-style xref table
  if (obj.isCmd("xref")) {
    obj.free();
    more = readXRefTable(parser, pos);

  // parse an xref stream
  } else if (obj.isInt()) {
    obj.free();
    if (!parser->getObj(&obj)->isInt()) {
      goto err1;
    }
    obj.free();
    if (!parser->getObj(&obj)->isCmd("obj")) {
      goto err1;
    }
    obj.free();
    if (!parser->getObj(&obj)->isStream()) {
      goto err1;
    }
    more = readXRefStream(obj.getStream(), pos);
    obj.free();

  } else {
    goto err1;
  }

  delete parser;
  return more;

 err1:
  obj.free();
  delete parser;
  ok = gFalse;
  return gFalse;
}

GBool XRef::readXRefTable(Parser *parser, Guint *pos) {
  XRefEntry entry;
  GBool more;
  Object obj, obj2;
  Guint pos2;
  int first, n, newSize, i;

  while (1) {
    parser->getObj(&obj);
    if (obj.isCmd("trailer")) {
      obj.free();
      break;
    }
    if (!obj.isInt()) {
      goto err1;
    }
    first = obj.getInt();
    obj.free();
    if (!parser->getObj(&obj)->isInt()) {
      goto err1;
    }
    n = obj.getInt();
    obj.free();
    if (first < 0 || n < 0 || first + n < 0) {
      goto err1;
    }
    if (first + n > size) {
      for (newSize = size ? 2 * size : 1024;
	   first + n > newSize && newSize > 0;
	   newSize <<= 1) ;
      if (newSize < 0) {
	goto err1;
      }
      entries = (XRefEntry *)greallocn(entries, newSize, sizeof(XRefEntry));
      for (i = size; i < newSize; ++i) {
	entries[i].offset = 0xffffffff;
	entries[i].type = xrefEntryFree;
      }
      size = newSize;
    }
    if (numFirstPageXRefs == -1) {
      numFirstPageXRefs = n;
    }
    if (!readXRefPart(parser, first, n)) {
      goto err1;
    }
  }

  // read the trailer dictionary
  if (!parser->getObj(&obj)->isDict()) {
    goto err1;
  }

  // get the 'Prev' pointer
  obj.getDict()->lookupNF("Prev", &obj2);
  if (obj2.isInt()) {
    *pos = (Guint)obj2.getInt();
    more = gTrue;
  } else if (obj2.isRef()) {
    // certain buggy PDF generators generate "/Prev NNN 0 R" instead
    // of "/Prev NNN"
    *pos = (Guint)obj2.getRefNum();
    more = gTrue;
  } else {
    more = gFalse;
  }
  obj2.free();

  // save the first trailer dictionary
  if (trailerDict.isNone()) {
    obj.copy(&trailerDict);
  }

  // check for an 'XRefStm' key
  if (obj.getDict()->lookup("XRefStm", &obj2)->isInt()) {
    pos2 = (Guint)obj2.getInt();
    readXRef(&pos2);
    if (!ok) {
      obj2.free();
      goto err1;
    }
  }
  obj2.free();

  obj.free();
  return more;

 err1:
  obj.free();
  ok = gFalse;
  return gFalse;
}

GBool XRef::readXRefStream(Stream *xrefStr, Guint *pos) {
  Dict *dict;
  int w[3];
  GBool more;
  Object obj, obj2, idx;
  int newSize, first, n, i;

  dict = xrefStr->getDict();

  if (!dict->lookupNF("Size", &obj)->isInt()) {
    goto err1;
  }
  newSize = obj.getInt();
  obj.free();
  if (newSize < 0) {
    goto err1;
  }
  if (newSize > size) {
    entries = (XRefEntry *)greallocn(entries, newSize, sizeof(XRefEntry));
    for (i = size; i < newSize; ++i) {
      entries[i].offset = 0xffffffff;
      entries[i].type = xrefEntryFree;
    }
    size = newSize;
    if (numFirstPageXRefs == -1) {
      numFirstPageXRefs = size;
    }
  }

  if (!dict->lookupNF("W", &obj)->isArray() ||
      obj.arrayGetLength() < 3) {
    goto err1;
  }
  for (i = 0; i < 3; ++i) {
    if (!obj.arrayGet(i, &obj2)->isInt()) {
      obj2.free();
      goto err1;
    }
    w[i] = obj2.getInt();
    obj2.free();
    if (w[i] < 0 || w[i] > 4) {
      goto err1;
    }
  }
  obj.free();

  xrefStr->reset();
  dict->lookupNF("Index", &idx);
  if (idx.isArray()) {
    for (i = 0; i+1 < idx.arrayGetLength(); i += 2) {
      if (!idx.arrayGet(i, &obj)->isInt()) {
	idx.free();
	goto err1;
      }
      first = obj.getInt();
      obj.free();
      if (!idx.arrayGet(i+1, &obj)->isInt()) {
	idx.free();
	goto err1;
      }
      n = obj.getInt();
      obj.free();
      if (first < 0 || n < 0 ||
	  !readXRefStreamSection(xrefStr, w, first, n)) {
	idx.free();
	goto err0;
      }
    }
  } else {
    if (!readXRefStreamSection(xrefStr, w, 0, newSize)) {
      idx.free();
      goto err0;
    }
  }
  idx.free();

  dict->lookupNF("Prev", &obj);
  if (obj.isInt()) {
    *pos = (Guint)obj.getInt();
    more = gTrue;
  } else {
    more = gFalse;
  }
  obj.free();
  if (trailerDict.isNone()) {
    trailerDict.initDict(dict);
  }

  return more;

 err1:
  obj.free();
 err0:
  ok = gFalse;
  return gFalse;
}

GBool XRef::readXRefStreamSection(Stream *xrefStr, int *w, int first, int n) {
  Guint offset;
  int type, gen, c, newSize, i, j;

  if (first + n < 0) {
    return gFalse;
  }
  if (first + n > size) {
    for (newSize = size ? 2 * size : 1024;
	 first + n > newSize && newSize > 0;
	 newSize <<= 1) ;
    if (newSize < 0) {
      return gFalse;
    }
    entries = (XRefEntry *)greallocn(entries, newSize, sizeof(XRefEntry));
    for (i = size; i < newSize; ++i) {
      entries[i].offset = 0xffffffff;
      entries[i].type = xrefEntryFree;
    }
    size = newSize;
  }
  for (i = first; i < first + n; ++i) {
    if (w[0] == 0) {
      type = 1;
    } else {
      for (type = 0, j = 0; j < w[0]; ++j) {
	if ((c = xrefStr->getChar()) == EOF) {
	  return gFalse;
	}
	type = (type << 8) + c;
      }
    }
    for (offset = 0, j = 0; j < w[1]; ++j) {
      if ((c = xrefStr->getChar()) == EOF) {
	return gFalse;
      }
      offset = (offset << 8) + c;
    }
    for (gen = 0, j = 0; j < w[2]; ++j) {
      if ((c = xrefStr->getChar()) == EOF) {
	return gFalse;
      }
      gen = (gen << 8) + c;
    }
    if (entries[i].offset == 0xffffffff) {
      switch (type) {
      case 0:
	entries[i].offset = offset;
	entries[i].gen = gen;
	entries[i].type = xrefEntryFree;
	break;
      case 1:
	entries[i].offset = offset;
	entries[i].gen = gen;
	entries[i].type = xrefEntryUncompressed;
	break;
      case 2:
	entries[i].offset = offset;
	entries[i].gen = gen;
	entries[i].type = xrefEntryCompressed;
	break;
      default:
	return gFalse;
      }
    }
  }

  return gTrue;
}

GBool XRef::readXRefPart2(Parser *parser, int first, int n) {
  Object obj, obj1;
  //GBool more;
  Guint pos = str->getPos();
  Parser *parser2;

  // start up a parser, parse one token
  obj1.initNull();
  parser2 = new Parser(NULL,
	      new Lexer(NULL,
		str->makeSubStream(start + pos, gFalse, 0, &obj1)));

  if (!parser2->getObj(&obj)->isInt()) {
    goto err1;
  }
  obj.free();
  if (!parser2->getObj(&obj)->isInt()) {
    goto err1;
  }
  obj.free();
  if (parser2->getObj(&obj)->isCmd("obj")) {
    obj.free();
    if (!parser2->getObj(&obj)->isStream()) {
      goto err1;
    }
    delete parser2;
    obj1.free();
    /*more =*/ readXRefStream(obj.getStream(), &pos);
    obj.free();
  } else {
    obj.free();
    delete parser2;
    obj1.free();

    return readXRefPart(parser, first, n);
  }

  return gTrue/*more*/;

 err1:
  delete parser2;
  obj1.free();
  obj.free();
  ok = gFalse;
  return gFalse;
}

// Attempt to construct an xref table for a damaged file.
GBool XRef::constructXRef() {
  Parser *parser;
  Object newTrailerDict, obj;
  char buf[256];
  Guint pos;
  int num, gen;
  int newSize;
  int streamEndsSize;
  char *p;
  int i;
  GBool gotRoot;

  gfree(entries);
  size = 0;
  entries = NULL;

  error(0, "PDF file is damaged - attempting to reconstruct xref table...");
  gotRoot = gFalse;
  streamEndsLen = streamEndsSize = 0;

  str->reset();
  while (1) {
    pos = str->getPos();
    if (!str->getLine(buf, 256)) {
      break;
    }
    p = buf;

    // got trailer dictionary
    if (!strncmp(p, "trailer", 7)) {
      obj.initNull();
      parser = new Parser(NULL,
		 new Lexer(NULL,
		   str->makeSubStream(pos + 7, gFalse, 0, &obj)));
      parser->getObj(&newTrailerDict);
      if (newTrailerDict.isDict()) {
	newTrailerDict.dictLookupNF("Root", &obj);
	if (obj.isRef()) {
	  rootNum = obj.getRefNum();
	  rootGen = obj.getRefGen();
	  if (!trailerDict.isNone()) {
	    trailerDict.free();
	  }
	  newTrailerDict.copy(&trailerDict);
	  gotRoot = gTrue;
	}
	obj.free();
      }
      newTrailerDict.free();
      delete parser;

    // look for object
    } else if (isdigit(*p)) {
      num = atoi(p);
      if (num > 0) {
	do {
	  ++p;
	} while (*p && isdigit(*p));
	if (isspace(*p)) {
	  do {
	    ++p;
	  } while (*p && isspace(*p));
	  if (isdigit(*p)) {
	    gen = atoi(p);
	    do {
	      ++p;
	    } while (*p && isdigit(*p));
	    if (isspace(*p)) {
	      do {
		++p;
	      } while (*p && isspace(*p));
	      if (!strncmp(p, "obj", 3)) {
		if (num >= size) {
		  newSize = (num + 1 + 255) & ~255;
		  if (newSize < 0) {
		    error(-1, "Bad object number");
		    return gFalse;
		  }
		  entries = (XRefEntry *)
		      greallocn(entries, newSize, sizeof(XRefEntry));
		  for (i = size; i < newSize; ++i) {
		    entries[i].offset = 0xffffffff;
		    entries[i].type = xrefEntryFree;
		  }
		  size = newSize;
		}
		if (entries[num].type == xrefEntryFree ||
		    gen >= entries[num].gen) {
		  entries[num].offset = pos - start;
		  entries[num].gen = gen;
		  entries[num].type = xrefEntryUncompressed;
		}
	      }
	    }
	  }
	}
      }

    } else if (!strncmp(p, "endstream", 9)) {
      if (streamEndsLen == streamEndsSize) {
	streamEndsSize += 64;
	streamEnds = (Guint *)greallocn(streamEnds,
					streamEndsSize, sizeof(int));
      }
      streamEnds[streamEndsLen++] = pos;
    }
  }

  if (gotRoot)
    return gTrue;

  error(-1, "Couldn't find trailer dictionary");
  return gFalse;
}

GBool XRef::checkEncrypted(PDFDoc *doc, GString *ownerPassword, GString *userPassword) {
  Object encrypt;
  SecurityHandler *secHdlr;
  GBool ret;

  trailerDict.dictLookup("Encrypt", &encrypt);
  if (encrypt.isDict()) {
    if ((secHdlr = SecurityHandler::make(doc, &encrypt))) {
      if (secHdlr->checkEncryption(ownerPassword, userPassword)) {
	// authorization succeeded
       	setEncryption(secHdlr->getPermissionFlags(),
		      secHdlr->getOwnerPasswordOk(),
		      secHdlr->getFileKey(),
		      secHdlr->getFileKeyLength(),
		      secHdlr->getEncVersion());
	ret = gTrue;
      } else {
	// authorization failed
	ret = gFalse;
      }
      delete secHdlr;
    } else {
      // couldn't find the matching security handler
      ret = gFalse;
    }
  } else {
    // document is not encrypted
    ret = gTrue;
  }
  encrypt.free();

  return ret;
}

void XRef::setEncryption(int permFlagsA, GBool ownerPasswordOkA,
			 Guchar *fileKeyA, int keyLengthA, int encVersionA) {
  int i;

  encrypted = gTrue;
  permFlags = permFlagsA;
  ownerPasswordOk = ownerPasswordOkA;
  if (keyLengthA <= 16) {
    keyLength = keyLengthA;
  } else {
    keyLength = 16;
  }
  for (i = 0; i < keyLength; ++i) {
    fileKey[i] = fileKeyA[i];
  }
  encVersion = encVersionA;
}
  
GBool XRef::okToPrint(GBool ignoreOwnerPW) {
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permPrint);
}

GBool XRef::okToChange(GBool ignoreOwnerPW) {
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permChange);
}

GBool XRef::okToCopy(GBool ignoreOwnerPW) {
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permCopy);
}

GBool XRef::okToAddNotes(GBool ignoreOwnerPW) {
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permNotes);
}

Object *XRef::fetch(int num, int gen, Object *obj) {
  XRefEntry *e;
  Parser *parser;
  Object obj1, obj2, obj3;

//printf("fetch(%d,%d)\n",num,gen);
  // check for bogus ref - this can happen in corrupted PDF files
  if (num < 0 || num >= size) {
    goto err;
  }

  e = &entries[num];
//printf("-> %d %d %d\n", e->gen, e->offset, e->type);
  switch (e->type) {

  case xrefEntryUncompressed:
    if (e->gen != gen) {
      goto err;
    }
    obj1.initNull();
    parser = new Parser(this,
	       new Lexer(this,
		 str->makeSubStream(start + e->offset, gFalse, 0, &obj1)));
    parser->getObj(&obj1);
    parser->getObj(&obj2);
    parser->getObj(&obj3);
    if (!obj1.isInt() || obj1.getInt() != num ||
	!obj2.isInt() || obj2.getInt() != gen ||
	!obj3.isCmd("obj")) {
      obj1.free();
      obj2.free();
      obj3.free();
      delete parser;
      goto err;
    }
    parser->getObj(obj, encrypted ? fileKey : (Guchar *)NULL, keyLength,
		   num, gen);
    obj1.free();
    obj2.free();
    obj3.free();
    delete parser;
    break;

  case xrefEntryCompressed:
    if (gen != 0) {
      goto err;
    }
    strCache->getObjectStream(this, (int)e->offset)->getObject(e->gen, num, obj);
    break;

  default:
    goto err;
  }

  return obj;

 err:
  return obj->initNull();
}

Object *XRef::getDocInfo(Object *obj) {
  return trailerDict.dictLookup("Info", obj);
}

// Added for the pdftex project.
Object *XRef::getDocInfoNF(Object *obj) {
  return trailerDict.dictLookupNF("Info", obj);
}

GBool XRef::getStreamEnd(Guint streamStart, Guint *streamEnd) {
  int a, b, m;

  if (streamEndsLen == 0 ||
      streamStart > streamEnds[streamEndsLen - 1]) {
    return gFalse;
  }

  a = -1;
  b = streamEndsLen - 1;
  // invariant: streamEnds[a] < streamStart <= streamEnds[b]
  while (b - a > 1) {
    m = (a + b) / 2;
    if (streamStart <= streamEnds[m]) {
      b = m;
    } else {
      a = m;
    }
  }
  *streamEnd = streamEnds[b];
  return gTrue;
}

Guint XRef::strToUnsigned(char *s) {
  Guint x;
  char *p;
  int i;

#undef isdigit // workaround a bug in gcc 2.93's code generator
  x = 0;
  for (p = s, i = 0; *p && isdigit(*p) && i < 10; ++p, ++i) {
    x = 10 * x + (*p - '0');
  }
  return x;
}


GBool XRef::getLinearizationInfo() {
  DEBUG_INFO
  char buf[256];
  int pos = 0;
  int num, gen;
  char *p;
  Object obj;

  str->reset();

  // start up a parser, parse one token
  obj.initNull();
  // look for first object
  Parser parser(NULL, new Lexer(NULL, str->makeSubStream(start + pos, gFalse, 0, &obj)));

  parser.getObj(&obj);
  if (!obj.isInt()) {
    obj.free();
    return gFalse;
  }
  obj.free();

  parser.getObj(&obj);
  if (!obj.isInt()) {
    obj.free();
    return gFalse;
  }
  obj.free();

  parser.getObj(&obj);
  if (!obj.isCmd("obj")) {
    obj.free();
    return gFalse;
  }
  obj.free();

  // read first object
  Object linDict;
  GBool ok = gFalse;
  parser.getObj(&linDict);
  if (linDict.isDict()) {
    str->setPos(start + parser.getPos() - 4);

    linDict.dictLookupNF("Linearized", &obj);
    ok = obj.isNum() && (int)obj.getNum() == 1;
    obj.free();

    if (ok) {
      linDict.dictLookupNF("L", &obj);
      if (!obj.isInt()/* || obj.getInt() != file->size()*/) {
	ok = gFalse;
      } else {
	fileSize = obj.getInt();
      }
      obj.free();
      linDict.dictLookupNF("O", &obj);
      if (obj.isInt())
	firstPageObjectNum = obj.getInt();
      else
	ok = gFalse;
      obj.free();
      linDict.dictLookupNF("E", &obj);
      if (obj.isInt())
	endOfFirstPageOffset = obj.getInt();
      else
	ok = gFalse;
      obj.free();
      linDict.dictLookupNF("N", &obj);
      if (obj.isInt())
	numPages = obj.getInt();
      else
	ok = gFalse;
      obj.free();
      linDict.dictLookupNF("T", &obj);
      if (obj.isInt())
	mainXRefOffset = obj.getInt();
      else
	ok = gFalse;
      obj.free();
      linDict.dictLookupNF("P", &obj);
      if (obj.isInt())
	firstPageNum = obj.getInt();
      else
	firstPageNum = 0;
      obj.free();
      linDict.dictLookupNF("H", &obj);
      if (obj.isArray() && obj.arrayGetLength() >= 2) {
	Object obj2;
	if (obj.arrayGetNF(0,&obj2)->isInt())
	  hintTable1Offset = obj2.getInt();
	else
	  ok = gFalse;
	obj2.free();
	if (obj.arrayGetNF(1,&obj2)->isInt())
	  hintTable1Length = obj2.getInt();
	else
	  ok = gFalse;
	obj2.free();
	if (obj.arrayGetLength() == 4) {
	  if (obj.arrayGetNF(2,&obj2)->isInt())
	    hintTable2Offset = obj2.getInt();
	  else
	    ok = gFalse;
	  obj2.free();
	  if (obj.arrayGetNF(3,&obj2)->isInt())
	    hintTable2Length = obj2.getInt();
	  else
	    ok = gFalse;
	  obj2.free();
	} else {
	  hintTable2Offset = 0;
	  hintTable2Length = 0;
	}
      } else
	ok = gFalse;
      obj.free();
    }
  }
  linDict.free();
  /*printf("O=%d E=%x N=%d T=%x P=%d\nH=[%x %x %x %x] ok=%d\n",
       firstPageObjectNum,endOfFirstPageOffset,numPages,
       mainXRefOffset,firstPageNum,hintTable1Offset,hintTable1Length,
       hintTable2Offset,hintTable2Length,ok);*/
  return ok;
}

// read trailer dict
GBool XRef::readTrailerDict() {
  DEBUG_INFO
  Object obj;
  GBool r = gTrue;

  if (trailerDict.isDict()) {
    trailerDict.dictLookupNF("Size", &obj);
    if (obj.isInt())
      size = obj.getInt();
    else
      r = gFalse;
    obj.free();
    trailerDict.dictLookupNF("Root", &obj);
    if (obj.isRef()) {
      rootNum = obj.getRefNum();
      rootGen = obj.getRefGen();
    } else {
      r = gFalse;
    }
    obj.free();
  } else {
    r = gFalse;
  }
  return r;
}

// Read a part of an xref table
GBool XRef::readXRefPart(Parser *parser, int first, int n) {
  DEBUG_INFO
  XRefEntry entry;
  int i;
  Object obj;

  for (i = first; i < first + n; ++i) {
    if (!parser->getObj(&obj)->isInt()) {
      return gFalse;
    }
    entry.offset = (Guint)obj.getInt();
    obj.free();
    if (!parser->getObj(&obj)->isInt()) {
      return gFalse;
    }
    entry.gen = obj.getInt();
    obj.free();
    parser->getObj(&obj);
    if (obj.isCmd("n")) {
      entry.type = xrefEntryUncompressed;
    } else if (obj.isCmd("f")) {
      entry.type = xrefEntryFree;
    } else {
      return gFalse;
    }
    obj.free();
    if (entries[i].offset == 0xffffffff) {
//printf("entry[%d]= %d %d %d\n", i, entry.gen, entry.offset, entry.type);
      entries[i] = entry;
      // PDF files of patents from the IBM Intellectual Property
      // Network have a bug: the xref table claims to start at 1
      // instead of 0.
      if (i == 1 && first == 1 &&
	  entries[1].offset == 0 && entries[1].gen == 65535 &&
	  entries[1].type == xrefEntryFree) {
	i = first = 0;
	entries[0] = entries[1];
	entries[1].offset = 0xffffffff;
      }
    }
  }

  return gTrue;
}

void XRef::fetchPage(int n, Object *obj) {
  DEBUG_INFO
  int offset = pageOffsetHintTable->getPageOffset(n);
  int length = pageOffsetHintTable->getPageLength(n);
  int numRefs = pageOffsetHintTable->getPageNumRefs(n);
  int first = pageOffsetHintTable->getPageFirstObjectNum(n);
  int num = pageOffsetHintTable->getPageNumObjects(n);
  int maxSharedId = sharedObjectHintTable->getNumIds();
  PartsVec parts;

  parts.add(offset, length);

  for (int i = 0; i < numRefs; ++i) {
    int id = pageOffsetHintTable->getPageRefId(n, i);
    if (id < maxSharedId) {
      int offset2 = sharedObjectHintTable->getGroupOffset(id);
      int length2 = sharedObjectHintTable->getGroupLength(id);
      parts.add(offset2, length2);
    } else if (maxSharedId) {
      error(-1, "Bad shared object ID: page %d, ref %d id %d", n, i, id);
    }
  }

  num = parts.getNumBlocks();
  for (int i = 0; i < num; ++i) {
    str->preRead(parts.getBlockOffset(i), parts.getBlockSize(i));
  }

  fetch(first, 0, obj);
}

void XRef::preloadOutlines() {
  if (outlinesHintTable)
    str->preRead(outlinesHintTable->getOffset(), outlinesHintTable->getLength());
}

void XRef::preloadNamedDests() {
  if (namedDestsHintTable)
    str->preRead(namedDestsHintTable->getOffset(), namedDestsHintTable->getLength());
}

void XRef::preloadInfo() {
  if (infoHintTable)
    str->preRead(infoHintTable->getOffset(), infoHintTable->getLength());
}

void XRef::preloadForms() {
  DEBUG_INFO
  if (formsHintTable) {
    int numRefs = formsHintTable->getNumRefs();
    int maxSharedId = sharedObjectHintTable->getNumIds();
    PartsVec parts;
    parts.add(formsHintTable->getOffset(), formsHintTable->getLength());
    for (int i = 0; i < numRefs; ++i) {
      int id = formsHintTable->getRefId(i);
      if (id >= 0 && id < maxSharedId) {
	int offset2 = sharedObjectHintTable->getGroupOffset(id);
	int length2 = sharedObjectHintTable->getGroupLength(id);
	parts.add(offset2, length2);
      } else if (maxSharedId) {
	error(-1, "Bad shared object ID: form, ref %d id %d", i, id);
      }
    }
    int num = parts.getNumBlocks();
    for (int i = 0; i < num; ++i)
      str->preRead(parts.getBlockOffset(i), parts.getBlockSize(i));
  }
}


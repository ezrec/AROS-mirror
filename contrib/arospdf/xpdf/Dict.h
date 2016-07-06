//========================================================================
//
// Dict.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef DICT_H
#define DICT_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Object.h"

//------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------

struct DictEntry {
  char *key;
  xObject val;
};

class Dict {
public:

  // Constructor.
  Dict(XRef *xrefA);

  // Destructor.
  ~Dict();

  // Reference counting.
  int incRef() { return ++ref; }
  int decRef() { return --ref; }

  // Get number of entries.
  int getLength() { return length; }

  // Add an entry.  NB: does not copy key.
  void add(char *key, xObject *val);

  // Check if dictionary is of specified type.
  GBool is(char *type);

  // Look up an entry and return the value.  Returns a null xObject
  // if <key> is not in the dictionary.
  xObject *lookup(char *key, xObject *obj);
  xObject *lookupNF(char *key, xObject *obj);

  // Iterative accessors.
  char *getKey(int i);
  xObject *getVal(int i, xObject *obj);
  xObject *getValNF(int i, xObject *obj);

  // Set the xref pointer.  This is only used in one special case: the
  // trailer dictionary, which is read before the xref table is
  // parsed.
  void setXRef(XRef *xrefA) { xref = xrefA; }

private:

  XRef *xref;			// the xref table for this PDF file
  DictEntry *entries;		// array of entries
  int size;			// size of <entries> array
  int length;			// number of entries in dictionary
  int ref;			// reference count

  DictEntry *find(char *key);
};

#endif

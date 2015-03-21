//========================================================================
//
// Array.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef ARRAY_H
#define ARRAY_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Object.h"

class XRef;

//------------------------------------------------------------------------
// Array
//------------------------------------------------------------------------

class Array {
public:

  // Constructor.
  Array(XRef *xrefA);

  // Destructor.
  ~Array();

  // Reference counting.
  int incRef() { return ++ref; }
  int decRef() { return --ref; }

  // Get number of elements.
  int getLength() { return length; }

  // Add an element.
  void add(xObject *elem);

  // Accessors.
  xObject *get(int i, xObject *obj);
  xObject *getNF(int i, xObject *obj);

private:

  XRef *xref;			// the xref table for this PDF file
  xObject *elems;		// array of elements
  int size;			// size of <elems> array
  int length;			// number of elements in array
  int ref;			// reference count
};

#endif

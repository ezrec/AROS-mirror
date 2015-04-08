//========================================================================
//
// Array.cc
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
#include "gmem.h"
#include "Object.h"
#include "Array.h"

//------------------------------------------------------------------------
// Array
//------------------------------------------------------------------------

Array::Array(XRef *xrefA) {
  xref = xrefA;
  elems = NULL;
  size = length = 0;
  ref = 1;
}

Array::~Array() {
  int i;

  for (i = 0; i < length; ++i)
    elems[i].free();
  gfree(elems);
}

void Array::add(Object *elem) {
  if (length == size) {
    if (length == 0) {
      size = 8;
    } else {
      size *= 2;
    }
#ifdef DEBUG_MEM
    for (int k = 0; k < length; ++k)
      if (!elems[k].isNone() && !elems[k].isNull())
	objTracker.unTrack(&elems[k]);
#endif
    elems = (Object *)greallocn(elems, size, sizeof(Object));
#ifdef DEBUG_MEM
    for (int k = 0; k < length; ++k)
      if (!elems[k].isNone() && !elems[k].isNull())
	objTracker.track(&elems[k]);
#endif
  }
  elems[length] = *elem;
#ifdef DEBUG_MEM
  elem->clear();
#endif
  ++length;
}

Object *Array::get(int i, Object *obj) {
  if (i < 0 || i >= length) {
#ifdef DEBUG_MEM
    abort();
#else
    return obj->initNull();
#endif
  }
  return elems[i].fetch(xref, obj);
}

Object *Array::getNF(int i, Object *obj) {
  if (i < 0 || i >= length) {
#ifdef DEBUG_MEM
    abort();
#else
    return obj->initNull();
#endif
  }
  return elems[i].copy(obj);
}

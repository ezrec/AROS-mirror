//========================================================================
//
// Dict.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stddef.h>
#include <string.h>
#include "gmem.h"
#include "Object.h"
#include "XRef.h"
#include "Dict.h"

//------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------

Dict::Dict(XRef *xrefA) {
  xref = xrefA;
  entries = NULL;
  size = length = 0;
  ref = 1;
}

Dict::~Dict() {
  int i;

  for (i = 0; i < length; ++i) {
    gfree(entries[i].key);
    entries[i].val.free();
  }
  gfree(entries);
}

void Dict::add(char *key, xObject *val) {
  if (length == size) {
    if (length == 0) {
      size = 8;
    } else {
      size *= 2;
    }
    entries = (DictEntry *)greallocn(entries, size, sizeof(DictEntry));
  }
  entries[length].key = key;
  entries[length].val = *val;
  ++length;
}

inline DictEntry *Dict::find(char *key) {
  int i;

  for (i = 0; i < length; ++i) {
    if (!strcmp(key, entries[i].key))
      return &entries[i];
  }
  return NULL;
}

GBool Dict::is(char *type) {
  DictEntry *e;

  return (e = find("Type")) && e->val.isName(type);
}

xObject *Dict::lookup(char *key, xObject *obj) {
  DictEntry *e;

  return (e = find(key)) ? e->val.fetch(xref, obj) : obj->initNull();
}

xObject *Dict::lookupNF(char *key, xObject *obj) {
  DictEntry *e;

  return (e = find(key)) ? e->val.copy(obj) : obj->initNull();
}

char *Dict::getKey(int i) {
  return entries[i].key;
}

xObject *Dict::getVal(int i, xObject *obj) {
  return entries[i].val.fetch(xref, obj);
}

xObject *Dict::getValNF(int i, xObject *obj) {
  return entries[i].val.copy(obj);
}

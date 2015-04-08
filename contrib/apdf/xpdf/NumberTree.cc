//========================================================================
//
// NumberTree.cc
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
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
#include "NumberTree.h"


//------------------------------------------------------------------------
// NumberTreeNode
//------------------------------------------------------------------------

NumberTreeNode *NumberTreeNode::parse(Dict *dict, NumberTreeLeafParser *parser) {
  NumberTreeNode *r = NULL;
  Object obj1;
  if (dict->lookup("Kids", &obj1)->isArray()) {
    r = new NumberTreeIntermediate(obj1.getArray(), parser);
  } else {
    obj1.free();
    if (dict->lookup("Nums", &obj1)->isArray()) {
      r = new NumberTreeLeaves(obj1.getArray(), parser);
    } else {
      error(-1, "Bad number tree");
    }
  }
  obj1.free();
  if (r && !r->isOk()) {
    delete r;
    r = NULL;
  }
  if (r) {
    if (dict->lookup("Limits", &obj1)->isArray() &&
	obj1.arrayGetLength() == 2) {
      Object obj2;
      if (obj1.arrayGet(0, &obj2)->isInt())
	r->low = obj2.getInt();
      obj2.free();
      if (obj1.arrayGet(1, &obj2)->isInt())
	r->high = obj2.getInt();
      obj2.free();
    }
    obj1.free();
  }
  return r;
}


//------------------------------------------------------------------------
// NumberTreeLeaves
//------------------------------------------------------------------------

NumberTreeLeaves::NumberTreeLeaves(Array *array, NumberTreeLeafParser *parser) {
  ok = gTrue;
  numEntries = array->getLength() / 2;
  entries = new NumberTreeLeaf * [numEntries];
  for (int k = 0; k < numEntries; ++k) {
    int key;
    Object obj2;
    if (array->get(k * 2, &obj2)->isInt())
      key = obj2.getInt();
    else
      ok = gFalse;
    obj2.free();
    array->get(k * 2 + 1, &obj2);
    entries[k] = parser->parse(key, &obj2);
    if (entries[k] == NULL || !entries[k]->isOk()) {
      delete entries[k];
      entries[k] = NULL;
      ok = gFalse;
    }
    obj2.free();
  }
}

NumberTreeLeaves::~NumberTreeLeaves() {
  for (int k = 0; k < numEntries; ++k)
    delete entries[k];
  delete [] entries;
}

NumberTreeLeaf *NumberTreeLeaves::getLeaf(int key) {
  unsigned n = numEntries;
  NumberTreeLeaf **p = entries;
  if (n == 0)
    return NULL;
  do {
    unsigned k = n / 2;
    NumberTreeLeaf **q = p + k;
    if (key > (*q)->getKey()) {
      p = q + 1;
      n -= k + 1;
    } else if (key != (*q)->getKey())
      n = k;
    else
      return *q;
  } while (n != 0);
  if (p != entries)
    --p;
  return *p;
}


//------------------------------------------------------------------------
// NumberTreeIntermediate
//------------------------------------------------------------------------

NumberTreeIntermediate::NumberTreeIntermediate(Array *array, NumberTreeLeafParser *parser) {
  ok = gTrue;
  numKids = array->getLength();
  kids = new NumberTreeNode * [numKids];
  for (int k = 0; k < numKids; ++k) {
    Object obj2;
    if (array->get(k, &obj2)->isDict())
      kids[k] = parse(obj2.getDict(), parser);
    else {
      kids[k] = NULL;
      ok = gFalse;
    }
    obj2.free();
  }
}

NumberTreeIntermediate::~NumberTreeIntermediate() {
  for (int k = 0; k < numKids; ++k)
    delete kids[k];
  delete [] kids;
}

NumberTreeLeaf *NumberTreeIntermediate::getLeaf(int key) {
  for (int k = 0; k < numKids; ++k)
    if (key >= kids[k]->lowerBound() && key <= kids[k]->upperBound())
      return kids[k]->getLeaf(key);
  return NULL;
}


//------------------------------------------------------------------------
// NumberTree
//------------------------------------------------------------------------

NumberTree::NumberTree(Object *dict, NumberTreeLeafParser *parser) {
  root = NULL;
  if (!dict->isDict())
    error(-1, "Bad number tree");
  else
    root = NumberTreeNode::parse(dict->getDict(), parser);
}


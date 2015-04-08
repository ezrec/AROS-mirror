//========================================================================
//
// Outline2.cc
//
// Copyright 1999-2005 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stddef.h>
#include "gmem.h"
#include "Object.h"
#include "Dict.h"
#include "Error.h"
#include "Action.h"
#include "Outline2.h"

//------------------------------------------------------------------------
// Outline
//------------------------------------------------------------------------

Outline::~Outline() {
  delete action;
  delete children;
}

//------------------------------------------------------------------------
// OutlineTree
//------------------------------------------------------------------------

OutlineTree::~OutlineTree() {
  if (beg) {
    Outline **p = cur;
    while (p != beg)
      delete *--p;
    gfree(beg);
  }
}

void OutlineTree::addChild(Outline *outline) {
  DEBUG_INFO
  if (cur == end) {
    int sz = cur - beg;
    beg = (Outline **)greallocn(beg, sz + 16, sizeof(*beg));
    cur = beg + sz;
    end = beg + sz + 16;
  }
  *cur++ = outline;
}

OutlineTree *OutlineTree::read(Object *root) {
  DEBUG_INFO
  Object outline;
  OutlineTree *tree = NULL;
  if (root->dictLookup("First", &outline)->isDict()) {
    Object obj;
    GBool opened = gFalse;

    if (root->dictLookup("Count", &obj)->isInt() && obj.getInt() > 0)
      opened = gTrue;
    obj.free();
    tree = new OutlineTree(opened);

    while (gTrue) {
      Action *action = NULL;
      if (!outline.dictLookup("Dest", &obj)->isNull())
	action = new ActionGoTo(new Destination(&obj));
      else {
	obj.free();
	if (outline.dictLookup("A", &obj)->isDict())
	  action = Action::makeAction(&obj);
      }
      obj.free();
      outline.dictLookup("Title", &obj);
      if (obj.isString()) {
	tree->addChild(new Outline(obj.getString(), action, read(&outline)));
      } else {
	error(-1, "Bad outline title.");
	delete action;
      }
      obj.free();
      outline.dictLookup("Next", &obj);
      outline.free();
      if(obj.isDict()) {
	outline = obj;
	obj.clear();
      } else {
	obj.free();
	break;
      }
    }
  }
  outline.free();
  return tree;
}


//========================================================================
//
// Outline2.h
//
// Copyright 1999-2005 Emmanuel Lesueur
//
//========================================================================

#ifndef OUTLINE_H
#define OUTLINE_H

#ifdef __GNUC__
#pragma interface
#endif

#include "Object.h"

class GString;
class Array;
class Dict;
class OutlineTree;
class Action;

//------------------------------------------------------------------------
// Outline
//------------------------------------------------------------------------

class Outline {
public:
  Outline(GString *title1, Action *action1, OutlineTree *children1)
    : title(title1), action(action1), children(children1) {}
  ~Outline();

  GString *getTitle() { return &title; }
  Action *getAction() { return action; }
  OutlineTree *getChildren() { return children; }

private:
  GString title;
  Action *action;
  OutlineTree *children;
};


//------------------------------------------------------------------------
// OutlineTree
//------------------------------------------------------------------------

class OutlineTree {
public:
  OutlineTree(GBool opened1)
  : beg(NULL), cur(NULL), end(NULL), opened(opened1) {}

  ~OutlineTree();

  GBool isOpened() { return opened; }
  int getNumChildren() { return cur - beg; }
  Outline *getOutline(int num) { return beg[num]; }

  void addChild(Outline *);

  static OutlineTree *read(Object *root);

private:
  Outline **beg;
  Outline **cur;
  Outline **end;
  GBool opened;
};

#endif


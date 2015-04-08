//========================================================================
//
// NumberTree.h
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef NUMBERTREE_H
#define NUMBERTREE_H

#ifdef __GNUC__
#pragma interface
#endif

#include "Object.h"

class GString;
class Array;
class Dict;
class OutlineTree;

//------------------------------------------------------------------------
// NumberTreeLeaf
//------------------------------------------------------------------------

class NumberTreeLeaf {
public:
  NumberTreeLeaf(int key1) : key(key1) {}

  virtual ~NumberTreeLeaf() {}

  virtual GBool isOk() = 0;

  int getKey() const { return key; }
private:
  int key;
};


//------------------------------------------------------------------------
// NumberTreeLeafParser
//------------------------------------------------------------------------

class NumberTreeLeafParser {
public:
  virtual ~NumberTreeLeafParser() {}

  virtual NumberTreeLeaf *parse(int key, Object *) =0;
};


//------------------------------------------------------------------------
// NumberTreeNode
//------------------------------------------------------------------------

class NumberTreeNode {
public:
  NumberTreeNode() : low(-0x80000000), high(0x7fffffff) {}

  virtual ~NumberTreeNode() {}

  virtual GBool isOk() = 0;

  int lowerBound() const { return low; }
  int upperBound() const { return high; }

  virtual NumberTreeLeaf *getLeaf(int num) =0;

  static NumberTreeNode *parse(Dict *, NumberTreeLeafParser *);

private:
  int low, high;
};


//------------------------------------------------------------------------
// NumberTreeLeaves
//------------------------------------------------------------------------

class NumberTreeLeaves : public NumberTreeNode {
public:
  NumberTreeLeaves(Array *array, NumberTreeLeafParser *);

  virtual ~NumberTreeLeaves();

  virtual GBool isOk() { return ok; }

  virtual NumberTreeLeaf *getLeaf(int num);
private:
  int numEntries;
  NumberTreeLeaf **entries;
  GBool ok;
};


//------------------------------------------------------------------------
// NumberTreeIntermediate
//------------------------------------------------------------------------

class NumberTreeIntermediate : public NumberTreeNode {
public:
  NumberTreeIntermediate(Array *array, NumberTreeLeafParser *);

  virtual ~NumberTreeIntermediate();

  virtual GBool isOk() { return ok; }

  virtual NumberTreeLeaf *getLeaf(int num);

private:
  int numKids;
  NumberTreeNode **kids;
  GBool ok;
};


//------------------------------------------------------------------------
// NumberTree
//------------------------------------------------------------------------

class NumberTree {
public:
  NumberTree(Object *dict, NumberTreeLeafParser *);

  ~NumberTree() { delete root; }

  GBool isOk() const { return root != NULL; }

  NumberTreeLeaf *getLeaf(int num) { return root->getLeaf(num); }

private:
  NumberTreeNode *root;
};

#endif


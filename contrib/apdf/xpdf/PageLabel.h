//========================================================================
//
// PageLabel.h
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef PAGELABEL_H
#define PAGELABEL_H

#ifdef __GNUC__
#pragma interface
#endif

#include "NumberTree.h"

//------------------------------------------------------------------------
// PageLabelLeaf
//------------------------------------------------------------------------

class PageLabelLeaf : public NumberTreeLeaf {
public:
  enum Style {
    StNoNum, StDecimal, StUpperCaseRoman, StLowerCaseRoman,
    StUpperCaseAlpha, StLowerCaseAlpha
  };

  PageLabelLeaf(int key1, Style style1, GString *prefix1,
		int start1, PageLabelLeaf *next1)
    : NumberTreeLeaf(key1), style(style1), prefix(prefix1),
      start(start1), next(next1) {}

  virtual ~PageLabelLeaf();

  virtual GBool isOk() { return gTrue; }

  PageLabelLeaf *getNext() { return next; }
  Style getStyle() const { return style; }
  GString *getPrefix() const { return prefix; }
  int getStart() const { return start; }

private:
  Style style;
  GString *prefix;
  int start;
  PageLabelLeaf *next;
};

//------------------------------------------------------------------------
// PageLabelParser
//------------------------------------------------------------------------

class PageLabelParser : public NumberTreeLeafParser {
public:
  PageLabelParser() : labels(NULL) {}

  virtual ~PageLabelParser() {}

  virtual NumberTreeLeaf *parse(int key, Object *obj);

  PageLabelLeaf *getFirst() { return labels; }

private:
  PageLabelLeaf *labels;
};

//------------------------------------------------------------------------
// PageLabels
//------------------------------------------------------------------------

class PageLabels {
public:
  PageLabels(Object *dict);

  GBool isOk() const { return tree.isOk(); }

  GString *getPageLabel(int index);

  int getPageIndex(const char *label);

private:
  PageLabelParser parser;
  NumberTree tree;
};

#endif


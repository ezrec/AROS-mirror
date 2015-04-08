//========================================================================
//
// PageLabel.cc
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
#include "PageLabel.h"

extern GString *toLatin1(GString *);

//------------------------------------------------------------------------
// PageLabelLeaf
//------------------------------------------------------------------------

PageLabelLeaf::~PageLabelLeaf() {
  delete prefix;
}


//------------------------------------------------------------------------
// PageLabelParser
//------------------------------------------------------------------------

NumberTreeLeaf *PageLabelParser::parse(int n, Object *obj) {
  PageLabelLeaf *r = NULL;
  if (obj->isDict()) {
    PageLabelLeaf::Style style = PageLabelLeaf::StNoNum;
    GString *prefix = NULL;
    int start = 1;
    Object obj2;

    if (obj->dictLookup("S", &obj2)->isName()) {
      if (!strcmp(obj2.getName(), "D"))
	style = PageLabelLeaf::StDecimal;
      else if (!strcmp(obj2.getName(), "R"))
	style = PageLabelLeaf::StUpperCaseRoman;
      else if (!strcmp(obj2.getName(), "r"))
	style = PageLabelLeaf::StLowerCaseRoman;
      else if (!strcmp(obj2.getName(), "A"))
	style = PageLabelLeaf::StUpperCaseAlpha;
      else if (!strcmp(obj2.getName(), "a"))
	style = PageLabelLeaf::StLowerCaseAlpha;
    }
    obj2.free();
    if (obj->dictLookup("P", &obj2)->isString())
      prefix = toLatin1(obj2.getString());
    obj2.free();
    if (obj->dictLookup("St", &obj2)->isInt())
      start = obj2.getInt();
    obj2.free();

    r = new PageLabelLeaf(n, style, prefix, start, labels);
    labels = r;
  }

  return r;
}


//------------------------------------------------------------------------
// PageLabels
//------------------------------------------------------------------------

PageLabels::PageLabels(Object *dict) : tree(dict, &parser) {
}

static char *addRomanDigit(char *q, unsigned n, char d1, char d5, char d10) {
  n %= 10;
  if (n == 9) {
    *--q = d10;
    *--q = d1;
  } else if (n == 4) {
    *--q = d5;
    *--q = d1;
  } else {
    bool f = false;
    if (n >= 5) {
      f = true;
      n -= 5;
    }
    switch (n) {
      case 3: *--q = d1;
      case 2: *--q = d1;
      case 1: *--q = d1;
    }
    if (f)
      *--q = d5;
  }
  return q;
}

GString *PageLabels::getPageLabel(int index) {
  PageLabelLeaf *label = (PageLabelLeaf *)tree.getLeaf(index);
  GString *s = new GString;
  if (label) {
    int offset = 0;
    char buf[32];
    char *q = buf + sizeof(buf);
    int n = index - label->getKey() + label->getStart();
    GBool neg = gFalse;

    if (n < 0) {
      n = -n;
      neg = gTrue;
    }

    *--q = '\0';

    if (label->getPrefix())
      s->append(label->getPrefix());

    switch (label->getStyle()) {
      case PageLabelLeaf::StUpperCaseRoman:
	offset = 'A' - 'a';
	// fall through
      case PageLabelLeaf::StLowerCaseRoman:
	if (n > 0 && n < 4000) {
	  q = addRomanDigit(q, n, 'i' + offset, 'v' + offset, 'x' + offset);
	  n /= 10;
	  q = addRomanDigit(q, n, 'x' + offset, 'l' + offset, 'c' + offset);
	  n /= 10;
	  q = addRomanDigit(q, n, 'c' + offset, 'd' + offset, 'm' + offset);
	  n /= 10;
	  q = addRomanDigit(q, n, 'm' + offset, '?', '?');
	  break;
	}
	// fall through
      case PageLabelLeaf::StDecimal:
	do {
	  *--q = '0' + (n % 10);
	  n /= 10;
	} while (n != 0);
	break;


      case PageLabelLeaf::StUpperCaseAlpha:
	offset = 'A' - 'a';
	// fall through
      case PageLabelLeaf::StLowerCaseAlpha:
	do {
	  *--q = 'a' + (n % 26) + offset;
	  n /= 26;
	} while (n != 0);
	break;
    }

    if (neg)
      *--q = '-';

    s->append(q);
  }
  return s;
}

int PageLabels::getPageIndex(const char *label) {
  PageLabelLeaf *leaf;
  for (leaf = parser.getFirst(); leaf != NULL; leaf = leaf->getNext()) {
    GString *p = leaf->getPrefix();
    const char *l = label;
    int n = 0;
    if (p) {
      if (!p->cmpN((char*)l, p->getLength()))
	l += p->getLength();
      else
	continue;
    }
    switch (leaf->getStyle()) {
    case PageLabelLeaf::StNoNum:
      n = leaf->getStart();
      break;
    case PageLabelLeaf::StDecimal:
      while(*l >= '0' && *l <= '9') {
	n *= 10;
	n += *l - '0';
	++l;
      }
      break;
    case PageLabelLeaf::StUpperCaseRoman:
      while (*l && n >= 0) {
	switch (*l++) {
	case 'I':
	  if (*l == 'V') {
	    n += 4;
	    ++l;
	  } else if (*l == 'X') {
	    n += 9;
	    ++l;
	  } else
	    ++n;
	  break;
	case 'V':
	  n += 5;
	  break;
	case 'X':
	  if (*l == 'L') {
	    n += 40;
	    ++l;
	  } else if (*l == 'C') {
	    n += 90;
	    ++l;
	  } else
	    n += 10;
	  break;
	case 'L':
	  n += 50;
	  break;
	case 'C':
	  if (*l == 'D') {
	    n += 400;
	    ++l;
	  } else if (*l == 'M') {
	    n += 900;
	    ++l;
	  } else
	    n += 100;
	  break;
	case 'D':
	  n += 500;
	  break;
	case 'M':
	  n += 1000;
	  break;
	default:
	  n = -1;
	  break;
	}
      }
      break;
    case PageLabelLeaf::StLowerCaseRoman:
      while (*l && n >= 0) {
	switch (*l++) {
	case 'i':
	  if (*l == 'v') {
	    n += 4;
	    ++l;
	  } else if (*l == 'x') {
	    n += 9;
	    ++l;
	  } else
	    ++n;
	  break;
	case 'v':
	  n += 5;
	  break;
	case 'x':
	  if (*l == 'l') {
	    n += 40;
	    ++l;
	  } else if (*l == 'c') {
	    n += 90;
	    ++l;
	  } else
	    n += 10;
	  break;
	case 'l':
	  n += 50;
	  break;
	case 'c':
	  if (*l == 'd') {
	    n += 400;
	    ++l;
	  } else if (*l == 'm') {
	    n += 900;
	    ++l;
	  } else
	    n += 100;
	  break;
	case 'd':
	  n += 500;
	  break;
	case 'm':
	  n += 1000;
	  break;
	default:
	  n = -1;
	  break;
	}
      }
      break;
    case PageLabelLeaf::StUpperCaseAlpha:
      while(*l >= 'A' && *l <= 'Z') {
	n *= 26;
	n += *l - 'A';
	++l;
      }
      break;
    case PageLabelLeaf::StLowerCaseAlpha:
      while(*l >= 'a' && *l <= 'z') {
	n *= 26;
	n += *l - 'a';
	++l;
      }
      break;
    }
    if (*l == '\0')
      return n - leaf->getStart() + leaf->getKey();
  }
  return -1;
}


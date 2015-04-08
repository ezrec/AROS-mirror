//========================================================================
//
// Hints.h
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef HINTS_H
#define HINTS_H

#ifdef __GNUC__
#pragma interface
#endif

#include "gtypes.h"

class Stream;

//------------------------------------------------------------------------
// PageOffsetHintTable
//------------------------------------------------------------------------

class PageOffsetHintTable {

  struct SharedObjectRef {
    int id;
    int pos;
  };

  struct Data {
    int firstObjectNum;
    int numObjects;
    int pageOffset;
    int pageLength;
    int numSharedObjectsRefs;
    int contentsOffset;
    int contentsLength;
    SharedObjectRef *sharedObjectsRefs;
  };

public:

  PageOffsetHintTable(Stream *str, int numPages1,
		      int firstPageFirstObjectNum, 
		      int hintTable1Offset, int hintTable1Length);
  ~PageOffsetHintTable();

  int getPageFirstObjectNum(int k) const { return data[k].firstObjectNum; }
  int getPageNumObjects(int k) const { return data[k].numObjects; }
  int getPageOffset(int k) const { return data[k].pageOffset; }
  int getPageLength(int k) const { return data[k].pageLength; }
  int getPageNumRefs(int k) const { return data[k].numSharedObjectsRefs; }
  int getPageRefId(int k, int l) const { return data[k].sharedObjectsRefs[l].id; }
  int getPageRefPos(int k, int l) const { return data[k].sharedObjectsRefs[l].pos; }

private:
  int numPages;
  Data *data;
};


//------------------------------------------------------------------------
// SharedObjectHintTable
//------------------------------------------------------------------------

class SharedObjectHintTable {

  struct SharedObjectsGroup {
    int offset;
    int length;
    int firstObjectNum;
    int numObjects;
    Gulong MD5Sig[4];
  };

public:

  SharedObjectHintTable(Stream *str, int firstObjectNum, int firstPageOffset,
			int hintTable1Offset, int hintTable1Length);
  ~SharedObjectHintTable();

  int getNumIds() const { return size; }
  int getGroupOffset(int k) const { return sharedObjectsGroups[k].offset; }
  int getGroupLength(int k) const { return sharedObjectsGroups[k].length; }
  int getGroupFirstObject(int k) const { return sharedObjectsGroups[k].firstObjectNum; }
  int getGroupNumObjects(int k) const { return sharedObjectsGroups[k].numObjects; }

private:

  int size;
  SharedObjectsGroup *sharedObjectsGroups;
};

//------------------------------------------------------------------------
// GenericHintTable
//------------------------------------------------------------------------

class GenericHintTable {
public:

  GenericHintTable(Stream *str);

  int getFirstObjectNum() { return firstObjectNum; }
  int getNumObjects() { return numObjects; }
  int getOffset() { return offset; }
  int getLength() { return length; }

private:
  int firstObjectNum;
  int numObjects;
  int offset;
  int length;
};

//------------------------------------------------------------------------
// FormsHintTable
//------------------------------------------------------------------------

class FormsHintTable : public GenericHintTable {

  struct SharedObjectRef {
    int id;
  };

public:

  FormsHintTable(Stream *str);
  ~FormsHintTable();

  int getNumRefs() const { return numSharedObjectsRefs; }
  int getRefId(int l) const { return sharedObjectsRefs[l].id; }

private:

  int numSharedObjectsRefs;
  SharedObjectRef *sharedObjectsRefs;
};

#endif


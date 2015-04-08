//========================================================================
//
// PartsVec.cc
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#include <string.h>
#include "gmem.h"
#include "PartsVec.h"

#define DB(x) //x

PartsVec::PartsVec() {
  entries = NULL;
  numEntries = 0;
  maxEntries = 0;
}

PartsVec::~PartsVec() {
  gfree(entries);
}

bool PartsVec::getFirstMissingPart(int *offsetPtr, int *sizePtr) {

  int offset1 = *offsetPtr;
  int size1 = *sizePtr;
  unsigned i = 0;
  unsigned n = numEntries;
  if (size1 <= 0)
    return false;


  while (n) {
    unsigned k = n / 2;
    unsigned j = i + k;
    int offset2 = entries[j].offset;
    int size2 = entries[j].size;
    if (offset2 > offset1) {
      n = k;
    } else if (offset2 + size2 < offset1) {
      i = j + 1;
      n -= k + 1;
    } else {
      if (offset1 + size1 <= offset2 + size2)
	return false;
      size1 = offset1 + size1 - offset2 - size2;
      offset1 = offset2 + size2;
      ++j;
      if (j < numEntries && entries[j].offset < offset1 + size1)
	size1 = entries[j].offset - offset1;
      DB(printf("getmissingpart(%d,%d)=(%d,%d)\n", *offsetPtr, *sizePtr, offset1, size1);)
      *offsetPtr = offset1;
      *sizePtr = size1;
      return true;
    }
  }
  DB(printf("getmissingpart(%d,%d)=(%d,%d)\n", offset1, size1, offset1, size1);)
  return true;
}

void PartsVec::remove(int offset1, int size1) {
  DB(printf("remove(%d,%d)\n", offset1, size1);)
  unsigned i = 0;
  unsigned n = numEntries;

  while (n) {
    unsigned k = n / 2;
    unsigned j = i + k;
    int offset2 = entries[j].offset;
    int size2 = entries[j].size;
    if (offset2 >= offset1 + size1) {
      n = k;
    } else if (offset2 + size2 <= offset1) {
      i = j + 1;
      n -= k + 1;
    } else {
      if (offset1 > offset2 && offset1 + size1 < offset2 + size2) {
	entries[j].size = offset1 - offset2;
	insert_entry(j + 1, offset1 + size1,
		     offset2 + size2 - offset1 - size1);
      } else {
	unsigned l = j;
	while (j > 0 &&
	       entries[j - 1].offset + entries[j - 1].size > offset1)
	  --j;
	offset2 = entries[j].offset;
	size2 = entries[j].size;
	if (offset1 > offset2) {
	  entries[j].size = offset1 - offset2;
	  ++j;
	  if (j > l)
	    l = j;
	}
	while (l < numEntries &&
	       entries[l].offset + entries[l].size <= offset1 + size1)
	  ++l;
	if (l < numEntries && entries[l].offset < offset1 + size1) {
	  entries[l].size = entries[l].offset + entries[l].size - offset1 - size1;
	  entries[l].offset = offset1 + size1;
	}
	remove_entries(j, l);
      }
      DB(printf("partsVec(%d,%d)\n", numEntries, maxEntries);
	 for (int k = 0; k < numEntries; ++k)
	   printf("\t%d->%d\n", entries[k].offset, entries[k].offset + entries[k].size);)
      return;
    }
  }
  DB(printf("partsVec(%d,%d)\n", numEntries, maxEntries);
     for (int k = 0; k < numEntries; ++k)
       printf("\t%d->%d\n", entries[k].offset, entries[k].offset + entries[k].size);)
}

void PartsVec::add(int offset1, int size1) {
  DB(printf("add(%d,%d)\n", offset1, size1);)
  unsigned i = 0;
  unsigned n = numEntries;

  while (n) {
    unsigned k = n / 2;
    unsigned j = i + k;
    int offset2 = entries[j].offset;
    int size2 = entries[j].size;
    if (offset2 > offset1 + size1) {
      n = k;
    } else if (offset2 + size2 < offset1) {
      i = j + 1;
      n -= k + 1;
    } else {
      unsigned l = j + 1;

      while (j > 0 && entries[j - 1].offset + entries[j - 1].size >= offset1)
	--j;

      while (l < numEntries && entries[l].offset <= offset1 + size1)
	++l;

      if (entries[j].offset > offset1)
	entries[j].offset = offset1;
      entries[j].size = (entries[l - 1].offset + entries[l - 1].size > offset1 + size1 ?
			 entries[l - 1].offset + entries[l - 1].size : offset1 + size1) -
			 entries[j].offset;

      remove_entries(j + 1, l);

      DB(printf("partsVec(%d,%d)\n", numEntries, maxEntries);
	 for (int k = 0; k < numEntries; ++k)
	   printf("\t%d->%d\n", entries[k].offset, entries[k].offset + entries[k].size);)
      return;
    }
  }

  insert_entry(i, offset1, size1);

  DB(printf("partsVec(%d,%d)\n", numEntries, maxEntries);
     for (int k = 0; k < numEntries; ++k)
       printf("\t%d->%d\n", entries[k].offset, entries[k].offset + entries[k].size);)
}

void PartsVec::insert_entry(int pos, int offset, int size) {
  DB(printf("insert_entry(%d,%d,%d)\n", pos, offset, size);)
  if (numEntries == maxEntries) {
    maxEntries = maxEntries * 2 + 8;
    entries = (Entry*)grealloc(entries, maxEntries * sizeof(Entry));
  }
  if (pos < numEntries)
    memmove(entries + pos + 1, entries + pos, (numEntries - pos) * sizeof(Entry));
  ++numEntries;
  entries[pos].offset = offset;
  entries[pos].size = size;
}

void PartsVec::remove_entries(int i, int j) {
  DB(printf("remove_entry(%d,%d)\n", i, j);)
  if (i < j) {
    if (j < numEntries)
      memmove(entries + i, entries + j, (numEntries - j) * sizeof(Entry));
    numEntries -= j - i;
  }
}


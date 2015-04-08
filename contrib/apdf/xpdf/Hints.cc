//========================================================================
//
// Hints.cc
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "gmem.h"
#include "Object.h"
#include "XRef.h"
#include "Hints.h"
#include "Error.h"

#define HINTS_DEBUG 0

#if HINTS_DEBUG
#   define DB(x) x
#else
#   define DB(x)
#endif

extern "C" void dprintf(const char *, ...);

//------------------------------------------------------------------------
// BitStream
//------------------------------------------------------------------------

class BitStream {
public:
  BitStream(Stream *str1) : str(str1), bits(0), readBits(0) {}
  int getBits(int);
  void sync() {
    DB(printf("pos=%lx\n%08x:%d\n", str->getPos(), buf, bits);)
    readBits += (8 - (bits & 7)) & 7;
    bits &= ~7;
    buf &= (1 << bits) - 1;
  }
  int numReadChars() const { return readBits + 7 >> 3; }
private:
  BitStream(const BitStream&);
  BitStream& operator = (const BitStream&);

  Stream *str;
  unsigned buf;
  int bits;
  int readBits;
};

int BitStream::getBits(int n) {
  int r = 0;
  unsigned c = buf;
  int m = bits, n0 = n;
  readBits += n;
  if (n > m) {
    r = c;
    n -= m;
    c = str->getChar();
    c <<= 8;
    c |= Guchar(str->getChar());
    c <<= 8;
    c |= Guchar(str->getChar());
    c <<= 8;
    c |= Guchar(str->getChar());
    //DB(printf("%08lx",c);)
    DB(for(unsigned k=0x80000000;k;k>>=1)
       printf("%01x",(c&k)!=0);)
    m = 32;
    r <<= n;
  }
  r |= c >> m - n;
  m -= n;
  buf = c & ((1 << m) - 1);
  bits = m;
  return r;
}

//------------------------------------------------------------------------
// PageOffsetHintTable
//------------------------------------------------------------------------

PageOffsetHintTable::PageOffsetHintTable(Stream *str, int numPages1,
					 int firstPageFirstObjectNum,
					 int hintTable1Offset, int hintTable1Length) {
  BitStream bStr(str);

  int leastNumObjects = bStr.getBits(32);
  int firstPageObjectOffset = bStr.getBits(32);
  int maxObjectsBits = bStr.getBits(16);
  int leastPageLength = bStr.getBits(32);
  int maxPageLengthBits = bStr.getBits(16);
  int leastContentsOffset = bStr.getBits(32);
  int maxContentsOffsetBits = bStr.getBits(16);
  int leastContentsLength = bStr.getBits(32);
  int maxContentsLengthBits = bStr.getBits(16);
  int maxSharedObjectsRefsBits = bStr.getBits(16);
  int sharedObjectIdBits = bStr.getBits(16);
  int numeratorBits = bStr.getBits(16);
  int denominator = bStr.getBits(16);

#if HINTS_DEBUG
  printf("\nleastNumObjects=%d\n", leastNumObjects);
  printf("firstPageObjectOffset=0x%x\n", firstPageObjectOffset);
  printf("maxObjectsBits=%d\n", maxObjectsBits);
  printf("leastPageLength=0x%x\n", leastPageLength);
  printf("maxPageLengthBits=%d\n", maxPageLengthBits);
  printf("leastContentsOffset=0x%x\n", leastContentsOffset);
  printf("maxContentsOffsetBits=%d\n", maxContentsOffsetBits);
  printf("leastContentsLength=0x%x\n", leastContentsLength);
  printf("maxContentsLengthBits=%d\n", maxContentsLengthBits);
  printf("maxSharedObjectsRefsBits=%d\n", maxSharedObjectsRefsBits);
  printf("sharedObjectIdBits=%d\n", sharedObjectIdBits);
  printf("numeratorBits=%d\n", numeratorBits);
  printf("denominator=%d\n", denominator);
#endif

  numPages = numPages1;

  data = (Data*)gmalloc(numPages * sizeof(Data));

  for (int i = 0; i < numPages; ++i)
    data[i].sharedObjectsRefs = NULL;

  DB(printf("\n\nnumObjects\n");)
  for (int i = 0; i < numPages; ++i)
    data[i].numObjects = leastNumObjects + bStr.getBits(maxObjectsBits);

  data[0].firstObjectNum = firstPageFirstObjectNum;
  if (numPages > 1) {
    data[1].firstObjectNum = 1;
    int num = 1;
    for (int i = 2; i < numPages; ++i) {
      num += data[i - 1].numObjects;
      data[i].firstObjectNum = num;
    }
  }

  DB(printf("\n\npageLength\n");)

  int offset = firstPageObjectOffset;
  bStr.sync();
  for (int i = 0; i < numPages; ++i) {
    data[i].pageOffset = offset + (offset >= hintTable1Offset ? hintTable1Length : 0);
    data[i].pageLength = leastPageLength + bStr.getBits(maxPageLengthBits);
    offset += data[i].pageLength;
  }

  DB(printf("\n\nnumRefs\n");)
  bStr.sync();
  for (int i = 0; i < numPages; ++i) {
    data[i].numSharedObjectsRefs = bStr.getBits(maxSharedObjectsRefsBits);
    data[i].sharedObjectsRefs = (SharedObjectRef*)gmalloc(data[i].numSharedObjectsRefs * sizeof(SharedObjectRef));
  }

  DB(printf("\n\nsharedGroupNum\n");)
  bStr.sync();
  for (int i = 0; i < numPages; ++i) {
    int n = data[i].numSharedObjectsRefs;
    SharedObjectRef *p = data[i].sharedObjectsRefs;
    for (int j = 0; j < n; ++j, ++p)
      p->id = bStr.getBits(sharedObjectIdBits);
  }

  DB(printf("\n\nsharedGroupPos\n");)
  bStr.sync();
  for (int i = 0; i < numPages; ++i) {
    int n = data[i].numSharedObjectsRefs;
    SharedObjectRef *p = data[i].sharedObjectsRefs;
    for (int j = 0; j < n; ++j, ++p)
      p->pos = bStr.getBits(numeratorBits);
  }

  DB(printf("\ncontentsOffset\n");)
  bStr.sync();
  for (int i = 0; i < numPages; ++i) {
    int offset = leastContentsOffset + bStr.getBits(maxContentsOffsetBits);
    data[i].contentsOffset = offset + (offset >= hintTable1Offset ? hintTable1Length : 0);
  }

  DB(printf("\n\ncontentsLength\n");)
  bStr.sync();
  for (int i = 0; i < numPages; ++i)
    data[i].contentsLength = leastContentsLength + bStr.getBits(maxContentsLengthBits);

#if HINTS_DEBUG
  for (int i = 0; i < numPages; ++i) {
    printf("\n[%d].firstObjectNum=%d\n", i, data[i].firstObjectNum);
    printf("[%d].numObjects=%d\n", i, data[i].numObjects);
    printf("[%d].pageOffset=0x%x\n", i, data[i].pageOffset);
    printf("[%d].pageLength=0x%x\n", i, data[i].pageLength);
    printf("[%d].numSharedObjectsRefs=%d\n", i, data[i].numSharedObjectsRefs);
    printf("[%d].contentsOffset=0x%x\n", i, data[i].contentsOffset);
    printf("[%d].contentsLength=0x%x\n", i, data[i].contentsLength);
    int n = data[i].numSharedObjectsRefs;
    SharedObjectRef *p = data[i].sharedObjectsRefs;
    for (int j = 0; j < n; ++j, ++p)
      printf("\t%d/%d\n", p->id, p->pos);
  }
#endif
}

PageOffsetHintTable::~PageOffsetHintTable() {
  for (int i = 0; i < numPages; ++i)
    gfree(data[i].sharedObjectsRefs);
  gfree(data);
}


//------------------------------------------------------------------------
// SharedObjectsHintTable
//------------------------------------------------------------------------

SharedObjectHintTable::SharedObjectHintTable(Stream *str,
					     int firstObjectNum, int firstPageOffset,
					     int hintTable1Offset, int hintTable1Length) {
  BitStream bStr(str);

  int firstSharedObject = bStr.getBits(32);
  int firstSharedObjectOffset = bStr.getBits(32);
  int numFirstPageSharedObjects = bStr.getBits(32);
  int numSharedObjectsSectionSharedObjects = bStr.getBits(32) - numFirstPageSharedObjects;
  int objectsPerGroupBits = bStr.getBits(16);
  int leastGroupLength = bStr.getBits(32);
  int maxGroupLengthBits = bStr.getBits(16);

  if (numSharedObjectsSectionSharedObjects < 0) {
    error(-1, "Bad shared offset hint table.");
    sharedObjectsGroups = NULL;
    size = 0;
    return;
  }

#if HINTS_DEBUG
  printf("\nfirstSharedObject=%d\n", firstSharedObject);
  printf("firstSharedObjectOffset=0x%x\n", firstSharedObjectOffset);
  printf("numFirstPageSharedObjects=%d\n", numFirstPageSharedObjects);
  printf("numSharedObjectsSectionSharedObjects=%d\n", numSharedObjectsSectionSharedObjects);
  printf("objectsPerGroupBits=%d\n", objectsPerGroupBits);
  printf("leastGroupLength=0x%x\n", leastGroupLength);
  printf("maxGroupLengthBits=%d\n", maxGroupLengthBits);
#endif

  size = numFirstPageSharedObjects + numSharedObjectsSectionSharedObjects;
  sharedObjectsGroups = (SharedObjectsGroup*)gmalloc(size * sizeof(SharedObjectsGroup));

  DB(printf("\nnumObjects\n");)
  int num = firstObjectNum;
  for (int i = 0; i < numFirstPageSharedObjects; ++i) {
    sharedObjectsGroups[i].firstObjectNum = num;
    sharedObjectsGroups[i].numObjects = 1 + bStr.getBits(objectsPerGroupBits);
    num += sharedObjectsGroups[i].numObjects;
  }

  num = firstSharedObject;
  for (int i = 0; i < numSharedObjectsSectionSharedObjects; ++i) {
    sharedObjectsGroups[numFirstPageSharedObjects + i].firstObjectNum = num;
    sharedObjectsGroups[numFirstPageSharedObjects + i].numObjects = 1 + bStr.getBits(objectsPerGroupBits);
    num += sharedObjectsGroups[numFirstPageSharedObjects + i].numObjects;
  }

  DB(printf("\ngroupLength\n");)
  bStr.sync();

  int offset = firstPageOffset;
  if (offset > hintTable1Offset)
    offset -= hintTable1Length;

  for (int i = 0; i < numFirstPageSharedObjects; ++i) {
    sharedObjectsGroups[i].offset = offset + (offset >= hintTable1Offset ? hintTable1Length : 0);
    sharedObjectsGroups[i].length = leastGroupLength + bStr.getBits(maxGroupLengthBits);
    offset += sharedObjectsGroups[i].length;
  }

  offset = firstSharedObjectOffset;
  for (int i = 0; i < numSharedObjectsSectionSharedObjects; ++i) {
    sharedObjectsGroups[numFirstPageSharedObjects + i].offset = offset + (offset >= hintTable1Offset ? hintTable1Length : 0);
    sharedObjectsGroups[numFirstPageSharedObjects + i].length = leastGroupLength + bStr.getBits(maxGroupLengthBits);
    offset += sharedObjectsGroups[numFirstPageSharedObjects + i].length;
  }

  DB(printf("MD5Sig\n");)
  bStr.sync();
  for (int i = 0; i < numFirstPageSharedObjects; ++i)
    sharedObjectsGroups[i].MD5Sig[0] = bStr.getBits(1);

  for (int i = 0; i < numSharedObjectsSectionSharedObjects; ++i)
    sharedObjectsGroups[numFirstPageSharedObjects + i].MD5Sig[0] = bStr.getBits(1);

  bStr.sync();
  for (int i = 0; i < numFirstPageSharedObjects; ++i)
    if (sharedObjectsGroups[i].MD5Sig[0]) {
      sharedObjectsGroups[i].MD5Sig[0] = bStr.getBits(32);
      sharedObjectsGroups[i].MD5Sig[1] = bStr.getBits(32);
      sharedObjectsGroups[i].MD5Sig[2] = bStr.getBits(32);
      sharedObjectsGroups[i].MD5Sig[3] = bStr.getBits(32);
    }

  for (int i = 0; i < numSharedObjectsSectionSharedObjects; ++i)
    if (sharedObjectsGroups[numFirstPageSharedObjects + i].MD5Sig[0]) {
      sharedObjectsGroups[numFirstPageSharedObjects + i].MD5Sig[0] = bStr.getBits(32);
      sharedObjectsGroups[numFirstPageSharedObjects + i].MD5Sig[1] = bStr.getBits(32);
      sharedObjectsGroups[numFirstPageSharedObjects + i].MD5Sig[2] = bStr.getBits(32);
      sharedObjectsGroups[numFirstPageSharedObjects + i].MD5Sig[3] = bStr.getBits(32);
    }

#if HINTS_DEBUG
  for (int i = 0; i < numSharedObjectsSectionSharedObjects + numFirstPageSharedObjects; ++i) {
    printf("\n[%d].offset=0x%x\n", i, sharedObjectsGroups[i].offset);
    printf("[%d].length=0x%x\n", i, sharedObjectsGroups[i].length);
    printf("[%d].firstObjectNum=%d\n", i, sharedObjectsGroups[i].firstObjectNum);
    printf("[%d].numObjects=%d\n", i, sharedObjectsGroups[i].numObjects);
    printf("[%d].MD5Sig=%08x\n", i, sharedObjectsGroups[i].MD5Sig[0]);
  }
#endif
}


SharedObjectHintTable::~SharedObjectHintTable() {
  gfree(sharedObjectsGroups);
}

//------------------------------------------------------------------------
// GenericHintTable
//------------------------------------------------------------------------

GenericHintTable::GenericHintTable(Stream *str) {
  BitStream bStr(str);
  firstObjectNum = bStr.getBits(32);
  offset = bStr.getBits(32);
  numObjects = bStr.getBits(32);
  length = bStr.getBits(32);
  DB(printf("\nGenericHintTable: %d, %d, %x, %x\n", firstObjectNum, numObjects, offset, length);)
}

//------------------------------------------------------------------------
// FormsHintTable
//------------------------------------------------------------------------

FormsHintTable::FormsHintTable(Stream *str) : GenericHintTable(str) {
  BitStream bStr(str);

  int n = bStr.getBits(32);
  int sharedObjectIdBits = bStr.getBits(16);

#if HINTS_DEBUG
  printf("\nnumSharedRefs=%d\n", n);
  printf("sharedRefsBits=%d\n", sharedObjectIdBits);
#endif

  numSharedObjectsRefs = n;
  sharedObjectsRefs = (SharedObjectRef*)gmalloc(numSharedObjectsRefs * sizeof(SharedObjectRef));

  SharedObjectRef *p = sharedObjectsRefs;
  for (int j = 0; j < n; ++j, ++p)
    p->id = bStr.getBits(sharedObjectIdBits);

#if HINTS_DEBUG
  p = sharedObjectsRefs;
  for (int j = 0; j < n; ++j, ++p)
    printf("\t%d\n", p->id);
#endif
}

FormsHintTable::~FormsHintTable() {
  gfree(sharedObjectsRefs);
}



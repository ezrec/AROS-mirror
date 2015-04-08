//========================================================================
//
// PartsVec.h
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef PARTSVEC_H
#define PARTSVEC_H


class PartsVec {
public:

  PartsVec();
  ~PartsVec();

  void add(int offset,int size);
  void remove(int offset,int size);
  bool getFirstMissingPart(int *offset, int *size);

  int getNumBlocks() { return numEntries; }
  int getBlockOffset(int k) { return entries[k].offset; }
  int getBlockSize(int k) { return entries[k].size; }

private:
  void insert_entry(int pos, int offset, int size);
  void remove_entries(int pos1, int pos2);
  struct Entry {
    int offset;
    int size;
  };
  Entry* entries;
  int maxEntries;
  int numEntries;
};


#endif


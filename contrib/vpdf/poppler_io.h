#ifndef POPPLER_IO_H
#define POPPLER_IO_H

//========================================================================
//
// StdCachedFile.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2010 Michal Wozniak <wozniak_m@interia.pl>
//
//========================================================================

#include "CachedFile.h"

class StdCacheLoader : public CachedFileLoader {

public:

  size_t init(GooString *fname, CachedFile* cachedFile);
  int load(const std::vector<ByteRange> &ranges, CachedFileWriter *writer);

};

#endif

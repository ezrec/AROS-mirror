//========================================================================
//
// StdCachedFile.cc
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2010 Michal Wozniak <wozniak_m@interia.pl>
//
//========================================================================

#include <config.h>

#include "poppler_io.h"

#include <stdio.h>

size_t StdCacheLoader::init(GooString *fname, CachedFile *cachedFile)
{
  size_t read, size = 0;
  FILE *f = fopen(fname->getCString(), "r");
  if (f == NULL)
	return 0;

  char buf[CachedFileChunkSize];

  CachedFileWriter writer = CachedFileWriter (cachedFile, NULL);
  do {
	read = fread(buf, 1, CachedFileChunkSize, f);
    (writer.write) (buf, CachedFileChunkSize);
    size += read;
  }
  while (read == CachedFileChunkSize);

  fclose(f);
  return size;
}

int StdCacheLoader::load(const std::vector<ByteRange> &ranges, CachedFileWriter *writer)
{
  return 0;
}


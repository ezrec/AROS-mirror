//========================================================================
//
// gdebug.cc
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#ifdef DEBUG_MEM

#include "gdebug.h"
#include <stdio.h>
#include <string.h>

GDebug::Context *GDebug::Context::current;

struct GDebug::Tracker::Tracked {
  void *object;
  const char ** context;
};

struct GDebug::Tracker::TrackedArray {
  TrackedArray() : array(NULL), size(0), max(0) {}
  ~TrackedArray() {
    for (int k = 0; k < size; ++k)
      Context::free(array[k].context);
    delete [] array;
  }
  Tracked *array;
  int size;
  int max;
};

GDebug::Tracker::Tracker() : tracked(new TrackedArray [size]) {
}

GDebug::Tracker::~Tracker() {
  for (int i = 0; i < size; ++i) {
    for (int k = 0; k < tracked[i].size; ++k) {
      fprintf(stderr, "Object %lx not freed. Allocated at:\n", tracked[i].array[k].object);
      Context::print(tracked[i].array[k].context);
      fprintf(stderr, "\n");
      show(tracked[i].array[k].object);
    }
  }
  delete [] tracked;
}

void GDebug::Tracker::show(void*) {}

void GDebug::Tracker::track(void *object) {
  int hash = ((unsigned)object >> 4) % size;

  if (tracked[hash].size == tracked[hash].max) {
    int sz = tracked[hash].size * 2 + 16;
    Tracked *p = new Tracked[sz];
    if (tracked[hash].size)
      memcpy(p, tracked[hash].array, sizeof(*p) * tracked[hash].size);
    delete [] tracked[hash].array;
    tracked[hash].array = p;
    tracked[hash].max = sz;
  }

  tracked[hash].array[tracked[hash].size].object = object;
  tracked[hash].array[tracked[hash].size].context = Context::save();
  ++tracked[hash].size;
}

void GDebug::Tracker::unTrack(void *object) {
  int hash = ((unsigned)object >> 4) % size;

  for (int k = 0; k < tracked[hash].size; ++k) {
    if (tracked[hash].array[k].object == object) {
      delete [] tracked[hash].array[k].context;
      if (k < tracked[hash].size - 1)
	memmove(tracked[hash].array + k, tracked[hash].array + k + 1,
		(tracked[hash].size - k - 1) * sizeof(tracked[hash].array[0]));
      --tracked[hash].size;
      return;
    }
  }

  Context::traceBack();
  fprintf(stderr, "Object %lx has not been allocated !\n", object);
  throw "Internal Error";
}

const char **GDebug::Context::save() {
  int n = 0;
  const char **p;
  for (Context *c = current; c; c = c->prev)
    ++n;
  if (n) {
    p = new const char * [n + 1];
    if (p) {
      const char **q = p + n;
      *q = NULL;
      for (Context *c = current; c; c = c->prev)
	*--q = c->name;
    }
  } else
    p = NULL;
  return p;
}

void GDebug::Context::print(const char **context) {
  fprintf(stderr, "*");
  while (*context) {
    fprintf(stderr, " / %s", *context);
    ++context;
  }
}

void GDebug::Context::traceBack() {
  Context *context = current;
  while (context) {
    fprintf(stderr, "%s <- ", context->name);
    context = context->prev;
  }
  fprintf(stderr, "*");
}

#endif


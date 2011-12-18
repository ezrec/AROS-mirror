
/* gc is not ported to AROS, so here are some replacement-functions. */


#include <stdlib.h>
#include <gc.h>
#include "../common/nsmtracker.h"

void GC_amiga_set_toany(void){return;}

GC_API int GC_collect_a_little GC_PROTO((void)){
  return 0;
}


GC_API void GC_exclude_static_roots GC_PROTO((GC_PTR start, GC_PTR finish)){
  return;
}


GC_API GC_PTR GC_malloc GC_PROTO((size_t size_in_bytes)){
  return calloc(1,size_in_bytes);
}

GC_API GC_PTR GC_malloc_atomic GC_PROTO((size_t size_in_bytes)){
  return malloc(size_in_bytes);
}

GC_API GC_PTR GC_malloc_uncollectable GC_PROTO((size_t size_in_bytes)){
  return calloc(1,size_in_bytes);
}

GC_API GC_PTR GC_malloc_atomic_uncollectable GC_PROTO((size_t size_in_bytes)){
  return malloc(size_in_bytes);
}


GC_API void GC_free GC_PROTO((GC_PTR object_addr)){
  free(object_addr);
}

bool LoadMMP2(struct Tracker_Windows *window,char *filename){
  fprintf(stderr,"Octamed loading not included in AROS\n");
  return false;
}


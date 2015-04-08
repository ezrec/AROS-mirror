/*
 * gmem.h
 *
 * Memory routines with out-of-memory checking.
 *
 * Copyright 1996-2003 Glyph & Cog, LLC
 */

#ifndef GMEM_H
#define GMEM_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Same as malloc, but prints error message and exits if malloc()
 * returns NULL.
 */
extern void *gmalloc(int size);

/*
 * Same as realloc, but prints error message and exits if realloc()
 * returns NULL.  If <p> is NULL, calls malloc instead of realloc().
 */
extern void *grealloc(void *p, int size);

/*
 * These are similar to gmalloc and grealloc, but take an object count
 * and size.  The result is similar to allocating nObjs * objSize
 * bytes, but there is an additional error check that the total size
 * doesn't overflow an int.
 */
extern void *gmallocn(int nObjs, int objSize);
extern void *greallocn(void *p, int nObjs, int objSize);

/*
 * Same as free, but checks for and ignores NULL pointers.
 */
extern void gfree(void *p);

#ifdef DEBUG_MEM
/*
 * Report on unfreed memory.
 */
extern void gMemReport(FILE *f);
#else
#define gMemReport(f)
#endif

/*
 * Allocate memory and copy a string into it.
 */
extern char *copyString(char *s);

#ifdef __cplusplus
}
#endif

#ifdef DEBUGMEM
  extern const char* memalloc_file;
  extern int memalloc_line;
# ifdef __cplusplus
    class mdbg {
    public:
      mdbg(const char* f,int l) {
	  memalloc_file=f;
	  memalloc_line=l;
      }
      template<class T> T* operator << (T* p) {
	  return p;
      }
    };
#   define new         mdbg(__FILE__,__LINE__) << new
#   define delete      memalloc_file=__FILE__,memalloc_line=__LINE__,delete
# endif
# define malloc(x)     (memalloc_file=__FILE__,memalloc_line=__LINE__,malloc(x))
# define calloc(x,y)   (memalloc_file=__FILE__,memalloc_line=__LINE__,calloc(x,y))
# define realloc(x,y)  (memalloc_file=__FILE__,memalloc_line=__LINE__,realloc(x,y))
# define gmalloc(x)    (memalloc_file=__FILE__,memalloc_line=__LINE__,gmalloc(x))
# define grealloc(x,y) (memalloc_file=__FILE__,memalloc_line=__LINE__,grealloc(x,y))
//# define free(x)       (memalloc_file=__FILE__,memalloc_line=__LINE__,free(x))
# define gfree(x)      (memalloc_file=__FILE__,memalloc_line=__LINE__,gfree(x))
#endif

#endif

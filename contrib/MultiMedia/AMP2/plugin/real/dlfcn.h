/*
 * dlfcn.h
 */

#ifndef DLFCN_H
#define DLFCN_H

#define RTLD_LAZY   1  /* lazy function call binding */
#define RTLD_NOW    2  /* immediate function call binding */
#define RTLD_GLOBAL 4  /* allow symbols to be global */

extern void *dlopen(const char *file, int mode);
extern void *dlsym(void *handle, const char *name);
extern int dlclose(void *handle);
extern char *dlerror(void);

/* spcecial relocation structure */

typedef struct reloc_s {
  u8 *name;
  void *addr;
} reloc_t;

extern reloc_t reloc[];

#endif /* DLFCN_H */
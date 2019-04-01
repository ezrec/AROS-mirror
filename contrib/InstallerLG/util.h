//----------------------------------------------------------------------------
// util.h:
//
// Misc utility functions primarily used by the 'native' methods.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef UTIL_H_
#define UTIL_H_

#include "types.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//----------------------------------------------------------------------------
// Utility functions.
//----------------------------------------------------------------------------
void ror(entry_p *e);
entry_p local(entry_p e);
entry_p global(entry_p e);
entry_p custom(entry_p e);
int c_sane(entry_p c, size_t n);
int s_sane(entry_p c, size_t n);
void pretty_print(entry_p entry);
int get_numvar(entry_p c, char *v);
entry_p get_opt(entry_p c, opt_t t);
char *get_strvar(entry_p c, char *v);
char *get_optstr(entry_p c, opt_t t);
char *get_chlstr(entry_p c, bool p);
void set_numvar(entry_p c, char *v, int n);
void set_strvar(entry_p c, char *v, char *n);
char *get_buf(void);
size_t buf_size(void);
void *dbg_alloc(int line, const char *file, const char *func, void *mem);
entry_p native_exists(entry_p contxt, call_t f);

//----------------------------------------------------------------------------
// Utility macros.
//----------------------------------------------------------------------------
#define DCUR    contxt->resolved
#define DNUM    contxt->resolved->id
#define RCUR    if(contxt) return contxt->resolved; return NULL
#define RNUM(X) contxt->resolved->id = X; return contxt->resolved
#define RSTR(X) char *rstr = X; if(rstr) { free(contxt->resolved->name); contxt->resolved->name = rstr;} else { PANIC(contxt); contxt->resolved->name[0] = '\0'; }; return contxt->resolved
#define REST    if(contxt->resolved->name) { contxt->resolved->name[0] = '\0'; } return contxt->resolved
#define CARG(X) contxt->children[(X) - 1]
#define CSYM(X) contxt->symbols[(X) - 1]
#define DBG_ALLOC(M) dbg_alloc(__LINE__, __FILE__, __func__, M)
#define HERE printf("%s:%s:%d\n", __FILE__, __func__, __LINE__)

#ifdef __AROS__
#define B_TO_CSTR(S) AROS_BSTR_ADDR(S)
#else
#define B_TO_CSTR(S) (*((char *) BADDR(S)) ? (((char *) BADDR(S)) + 1) : ((char *) BADDR(S)))
#endif

#endif

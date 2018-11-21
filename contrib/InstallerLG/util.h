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

#include <stdlib.h>

//----------------------------------------------------------------------------
// Utility functions.
//----------------------------------------------------------------------------
void ror(entry_p *e);
entry_p local(entry_p e);
entry_p global(entry_p e);
int c_sane(entry_p c, size_t n);
int s_sane(entry_p c, size_t n);
void pretty_print(entry_p entry);
int get_numvar(entry_p c, char *v);
entry_p get_opt(entry_p c, opt_t t);
char *get_strvar(entry_p c, char *v);
char *get_optstr(entry_p c, opt_t);
char *get_chlstr(entry_p c);
void set_numvar(entry_p c, char *v, int n);
void set_strvar(entry_p c, char *v, char *n);
char *get_buf(void);
size_t buf_size(void);

//----------------------------------------------------------------------------
// Utility macros.
//----------------------------------------------------------------------------
#define DCUR    contxt->resolved
#define DNUM    contxt->resolved->id
#define RCUR    { return contxt ? contxt->resolved : NULL; }
#define RNUM(X) { contxt->resolved->id = X; return contxt->resolved; }
#define RSTR(X) { char *rstr = X; if(rstr) { free(contxt->resolved->name); contxt->resolved->name = rstr;} else { PANIC(contxt); contxt->resolved->name[0] = '\0'; }; return contxt->resolved; }
#define REST    { if(contxt->resolved->name) contxt->resolved->name[0] = '\0'; return contxt->resolved; }
#define CARG(X) (contxt->children[X - 1])
#define CSYM(X) (contxt->symbols[X - 1])

#ifdef __AROS__
#define B_TO_CSTR(S) AROS_BSTR_ADDR(S)
#else
#define B_TO_CSTR(S) (*((char *) BADDR(S)) ? (((char *) BADDR(S)) + 1) : ((char *) BADDR(S))) //(*S ? (((char *) S) + 1) : BADDR(S))
#endif

#endif

//----------------------------------------------------------------------------
// alloc.h:
//
// Functions for allocation of entry_t data and closely related functions.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef ALLOC_H_
#define ALLOC_H_

#include "types.h"

//----------------------------------------------------------------------------
// Allocation of entry_t types.
//----------------------------------------------------------------------------
entry_p new_contxt(void);
entry_p new_number(int num);
entry_p new_string(char *name);
entry_p new_symbol(char *name);
entry_p new_symref(char *name, int line);
entry_p new_cusref(char *name, int line, entry_p arg);
entry_p new_option(char *name, opt_t type, entry_p chl);
entry_p new_custom(char *name, int line, entry_p sym, entry_p chl);
entry_p new_native(char *name, int line, call_t call, entry_p chl, type_t type);

//----------------------------------------------------------------------------
// Utilities.
//----------------------------------------------------------------------------
void kill(entry_p entry);
entry_p append(entry_p **dst, entry_p ent);
entry_p merge(entry_p dst, entry_p src);
entry_p push(entry_p dst, entry_p src);
entry_p end(void);

#endif

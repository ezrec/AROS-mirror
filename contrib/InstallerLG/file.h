//----------------------------------------------------------------------------
// file.h:
//
// File operations
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef FILE_H_
#define FILE_H_

#include "types.h"

#include <stdbool.h>

entry_p m_copyfiles(entry_p contxt);
entry_p m_copylib(entry_p contxt);
entry_p m_delete(entry_p contxt);
entry_p m_exists(entry_p contxt);
entry_p m_expandpath(entry_p contxt);
entry_p m_fileonly(entry_p contxt);
entry_p m_foreach(entry_p contxt);
entry_p m_makeassign(entry_p contxt);
entry_p m_makedir(entry_p contxt);
entry_p m_protect(entry_p contxt);
entry_p m_startup(entry_p contxt);
entry_p m_textfile(entry_p contxt);
entry_p m_tooltype(entry_p contxt);
entry_p m_transcript(entry_p contxt);
entry_p m_rename(entry_p contxt);

int h_exists(const char *file);
int h_log(entry_p contxt, const char *fmt, ...);
bool h_confirm(entry_p contxt, const char *hlp, const char *msg, ...);

//----------------------------------------------------------------------------
// Exchange (GUI) datatypes
//----------------------------------------------------------------------------
struct pnode_t
{
    struct pnode_t *next;
    char *name;
    char *copy;
    long type;
};

typedef struct pnode_t * pnode_p;

#endif

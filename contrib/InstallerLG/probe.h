//----------------------------------------------------------------------------
// probe.h:
//
// Environment information retrieval
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef PROBE_H_
#define PROBE_H_

#include "types.h"

entry_p m_database(entry_p contxt);
entry_p m_earlier(entry_p contxt);
entry_p m_getassign(entry_p contxt);
entry_p m_getdevice(entry_p contxt);
entry_p m_getdiskspace(entry_p contxt);
entry_p m_getenv(entry_p contxt);
entry_p m_getsize(entry_p contxt);
entry_p m_getsum(entry_p contxt);
entry_p m_getversion(entry_p contxt);
entry_p m_iconinfo(entry_p contxt);

int h_getversion(entry_p contxt, const char *name);

#endif

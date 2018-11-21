//----------------------------------------------------------------------------
// external.h:
//
// Execution of external scripts / binaries
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include "types.h"

entry_p m_execute(entry_p contxt);
entry_p m_rexx(entry_p contxt);
entry_p m_run(entry_p contxt);

#endif

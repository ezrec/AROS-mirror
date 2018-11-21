//----------------------------------------------------------------------------
// exit.h:
//
// Interuption of program execution
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef EXIT_H_
#define EXIT_H_

#include "types.h"

entry_p m_abort(entry_p contxt);
entry_p m_exit(entry_p contxt);
entry_p m_onerror(entry_p contxt);
entry_p m_trap(entry_p contxt);
entry_p m_reboot(entry_p contxt);

#endif

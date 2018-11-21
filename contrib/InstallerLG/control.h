//----------------------------------------------------------------------------
// control.h:
//
// Control structures
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef CONTROL_H_
#define CONTROL_H_

#include "types.h"

entry_p m_if(entry_p contxt);
entry_p m_select(entry_p contxt);
entry_p m_until(entry_p contxt);
entry_p m_while(entry_p contxt);
entry_p m_trace(entry_p contxt);
entry_p m_retrace(entry_p contxt);

#endif

//----------------------------------------------------------------------------
// information.h:
//
// Functions for informing the user
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef INFORMATION_H_
#define INFORMATION_H_

#include "types.h"

entry_p m_complete(entry_p contxt);
entry_p m_debug(entry_p contxt);
entry_p m_message(entry_p contxt);
entry_p m_welcome(entry_p contxt);
entry_p m_working(entry_p contxt);
entry_p m_user(entry_p contxt);

#endif

//----------------------------------------------------------------------------
// bitwise.h:
//
// Bitwise operations
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef BITWISE_H_
#define BITWISE_H_

#include "types.h"

entry_p m_and(entry_p contxt);
entry_p m_bitand(entry_p contxt);
entry_p m_bitnot(entry_p contxt);
entry_p m_bitor(entry_p contxt);
entry_p m_bitxor(entry_p contxt);
entry_p m_not(entry_p contxt);
entry_p m_in(entry_p contxt);
entry_p m_or(entry_p contxt);
entry_p m_shiftleft(entry_p contxt);
entry_p m_shiftright(entry_p contxt);
entry_p m_xor(entry_p contxt);

#endif

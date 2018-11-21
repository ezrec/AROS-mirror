//----------------------------------------------------------------------------
// comparison.h:
//
// Comparison operators
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef COMPARISON_H_
#define COMPARISON_H_

#include "types.h"

entry_p m_eq(entry_p contxt);
entry_p m_gt(entry_p contxt);
entry_p m_gte(entry_p contxt);
entry_p m_lt(entry_p contxt);
entry_p m_lte(entry_p contxt);
entry_p m_neq(entry_p contxt);

#endif

//----------------------------------------------------------------------------
// prompt.h:
//
// User prompting
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef PROMPT_H_
#define PROMPT_H_

#include "types.h"

entry_p m_askbool(entry_p contxt);
entry_p m_askchoice(entry_p contxt);
entry_p m_askdir(entry_p contxt);
entry_p m_askdisk(entry_p contxt);
entry_p m_askfile(entry_p contxt);
entry_p m_asknumber(entry_p contxt);
entry_p m_askoptions(entry_p contxt);
entry_p m_askstring(entry_p contxt);

#endif

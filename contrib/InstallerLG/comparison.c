//----------------------------------------------------------------------------
// comparison.c:
//
// Comparison operators
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "comparison.h"
#include "error.h"
#include "eval.h"
#include "util.h"

#include <string.h>

//----------------------------------------------------------------------------
// < <= == >= >
//     helper function for m_eq, m_gt, m_gte, m_lt, m_lte, m_ne.
//----------------------------------------------------------------------------
static int h_cmp(entry_p a, entry_p b)
{
    // Resolve both arguments. We don't need
    // to check for failures; we will always
    // get something to compare.
    a = resolve(a);
    b = resolve(b);

    // If both arguments are strings then use
    // string comparison.
    if(a->type == STRING &&
       b->type == STRING)
    {
        return strcmp(a->name, b->name);
    }
    // Otherwise convert whatever we have to
    // numerical values and subtract.
    else
    {
        return num(a) - num(b);
    }
}

//----------------------------------------------------------------------------
// (= <expr1> <expr2>)
//     equality test (returns 0 or 1)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_eq(entry_p contxt)
{
    // We should have two arguments
    if(c_sane(contxt, 2))
    {
        // Turn the result of h_cmp
        // into 0 or 1.
        RNUM
        (
            !h_cmp(CARG(1), CARG(2)) ? 1 : 0
        );
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (< <expr1> <expr2>)
//     greater than test (returns 0 or 1)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_gt(entry_p contxt)
{
    // We should have two arguments
    if(c_sane(contxt, 2))
    {
        // Turn the result of h_cmp
        // into 0 or 1.
        RNUM
        (
            h_cmp(CARG(1), CARG(2)) > 0 ? 1 : 0
        );
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (>= <expr1> <expr2>)
//     greater than or equal test (returns 0 or 1)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_gte(entry_p contxt)
{
    // We should have two arguments
    if(c_sane(contxt, 2))
    {
        // Turn the result of h_cmp
        // into 0 or 1.
        RNUM
        (
            h_cmp(CARG(1), CARG(2)) >= 0 ? 1 : 0
        );
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (< <expr1> <expr2>)
//     less than test (returns 0 or 1)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_lt(entry_p contxt)
{
    // We should have two arguments
    if(c_sane(contxt, 2))
    {
        // Turn the result of h_cmp
        // into 0 or 1.
        RNUM
        (
            h_cmp(CARG(1), CARG(2)) < 0 ? 1 : 0
        );
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (<= <expr1> <expr2>)
//     less than or equal test (returns 0 or 1)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_lte(entry_p contxt)
{
    // We should have two arguments
    if(c_sane(contxt, 2))
    {
        // Turn the result of h_cmp
        // into 0 or 1.
        RNUM
        (
            h_cmp(CARG(1), CARG(2)) <= 0 ? 1 : 0
        );
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (<> <expr1> <expr2>)
//     inequality test (returns 0 or 1)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_neq(entry_p contxt)
{
    // We should have two arguments
    if(c_sane(contxt, 2))
    {
        // Turn the result of h_cmp
        // into 0 or 1.
        RNUM
        (
            h_cmp(CARG(1), CARG(2)) ? 1 : 0
        );
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
        RCUR;
    }
}

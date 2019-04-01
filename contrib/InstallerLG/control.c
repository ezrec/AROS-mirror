//----------------------------------------------------------------------------
// control.c:
//
// Control structures
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "control.h"
#include "error.h"
#include "eval.h"
#include "util.h"

//----------------------------------------------------------------------------
// (if <condition> <then-statement> [<else-statements>])
//     conditional
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_if(entry_p contxt)
{
    // Allow empty bodies. If empty, resolve the
    // conditional so that side effects, if any,
    // will come into being.
    if(c_sane(contxt, 1))
    {
        // Truth value of the condition.
        int val = tru(CARG(1));

        // Is the body non-empty?
        if(CARG(2) && CARG(2) != end())
        {
            // Select branch to execute.
            entry_p sel = val ? CARG(2) : CARG(3);

            // Is there a branch corresponding to the
            // resolved truth value?
            if(sel && sel != end())
            {
                // We execute the branch by resolving it.
                return resolve(sel);
            }
        }

        // We have nothing to execute.
        RNUM(0);
    }

    // The parser is broken.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (select <n> <item1> <item2> ...)
//     return n'th item
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_select(entry_p contxt)
{
    // We need atleast two arguments, the index
    // and the non-empty list of items.
    if(c_sane(contxt, 2))
    {
        int ndx = 0, sel = num(CARG(1));

        // Find the n:th (0-indexed) item, go one step
        // at a time in case no such item exist.
        while(CARG(2)->children[ndx] &&
              CARG(2)->children[ndx] != end())
        {
            // Return the resolved value of the found
            // item.
            if(ndx == sel)
            {
                return resolve(CARG(2)->children[ndx]);
            }

            ndx++;
        }

        // No such item, n > the number of items.
        ERR(ERR_NO_ITEM, str(CARG(1)));
        RNUM(0);
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (while ...) (m = 0) and (until ...) (m = 1)
//     helper / implementation
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
static entry_p h_whunt(entry_p contxt, int mode)
{
    // We always have two arguments, the expression
    // and the 'contents' of the loop, a CONTXT.
    if(c_sane(contxt, 2) &&
       CARG(2)->type == CONTXT)
    {
        // Set the return value of this function to zero.
        DNUM = 0;

        // Prepare to return the resolved value of this
        // function if the expression is false from the start.
        entry_p ret = contxt->resolved;

        // Use XOR to support both 'while' and 'until'. Break
        // the loop if something goes wrong inside.
        while((mode ^ tru(CARG(1))) && !DID_ERR)
        {
            // Save the return value of the last function
            // in the CONTXT
            ret = invoke(CARG(2));
        }

        // Return either zero, the value of the resolved
        // value of this function, or the return value of
        // the last function in the last iteration.
        return ret;
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (until <expr> <statements>)
//     do-until conditional structure (test end of loop)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_until(entry_p contxt)
{
    // Implemented in h_whunt.
    return h_whunt(contxt, 1);
}

//----------------------------------------------------------------------------
// (while <expr> <statements>)
//     do-while conditional structure (test top of loop)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_while(entry_p contxt)
{
    // Implemented in h_whunt.
    return h_whunt(contxt, 0);
}

//----------------------------------------------------------------------------
// (trace)
//     set a backtrace position
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_trace(entry_p contxt)
{
    // All we need is a context.
    if(contxt)
    {
        // We're not doing anything
        // except occupying space.
        RNUM(1);
    }

    // The parser is broken.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (retrace)
//     backtrace to the next to last backtrace position
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_retrace(entry_p contxt)
{
    // No children.
    if(contxt)
    {
        // Find nearest context / user defined
        // procedure.
        while(contxt->parent &&
              contxt->parent->type != CONTXT &&
              contxt->parent->type != CUSTOM)
        {
            // Climb one generation.
            contxt = contxt->parent;
        }

        // This shouldn't happen.
        if(!contxt->parent ||
           !contxt->parent->children)
        {
            // Nowhere to go.
            PANIC(contxt);
            RCUR;
        }

        // Iterator and stop.
        entry_p *chl = contxt->parent->children;
        entry_p top = *chl;

        // Locate ourselves.
        while(*chl != contxt)
        {
            chl++;
        }

        // Locate the first trace point,
        // unless we're first in line.
        if(*chl != top)
        {
            // Find first trace point.
            while(*(--chl) != top)
            {
                if((*chl)->call == m_trace)
                {
                    // Found it.
                    break;
                }
            }

            // Look for the second trace point,
            // unless we're at the top by now.
            if(*chl != top)
            {
                // Find the second point.
                while(*(--chl) != top)
                {
                    if((*chl)->call == m_trace)
                    {
                        // Found it.
                        break;
                    }
                }
            }
        }

        // Backtrack if we found two trace points.
        if((*chl)->call == m_trace)
        {
            // Expect failure.
            entry_p ret = end();
            entry_p *org = chl;

            // Frame counter.
            static int dep = 0;

            // Keep track of the recursion depth. Do not
            // invoke if we're beyond MAXDEP.
            if(dep++ < MAXDEP)
            {
                for(; !DID_ERR; chl = org)
                {
                    // As long as no one fails, resolve
                    // all children and save the return
                    // value of the last one.
                    while(*chl && *chl != end() && !DID_ERR)
                    {
                        // Resolve and proceed.
                        ret = resolve(*chl);
                        chl++;
                    }
                }

                // Leaving frame.
                dep--;

                // Return the last value.
                return ret;
            }

            // We risk running out of stack.
            ERR(ERR_MAX_DEPTH, contxt->name);
        }
    }
    else
    {
        // The parser is broken.
        PANIC(contxt);
    }

    // Nowhere to go or panic.
    RCUR;
}

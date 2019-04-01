//----------------------------------------------------------------------------
// symbol.c:
//
// Symbol manipulation
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "eval.h"
#include "symbol.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------
// (set <varname> <value> [<varname2> <value2> ...])
//      sets the variable `<varname>' to the indicated value.
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_set(entry_p contxt)
{
    // Symbol destination.
    entry_p glb = global(contxt);

    // We need atleast one symbol and one value.
    if(c_sane(contxt, 1) && s_sane(contxt, 1))
    {
        // Function argument tuples.
        entry_p *sym = contxt->symbols,
                *val = contxt->children;

        // The custom procedure contxt if
        // we're in one. If so we need to
        // handle local variables (args).
        entry_p cus = custom(contxt);

        // Iterate over all symbol -> value tuples
        while(*sym && *sym != end() &&
              *val && *val != end())
        {
            // We need to resolve the right hand side
            // before setting the symbol value.
            entry_p rhs = resolve(*val);
            entry_p dst = glb;

            // Are we within the contxt of a custom procedure?
            if(cus)
            {
                // If so determine if this particular symbol
                // is one of the local ones (arguments), if
                // it is, rather than the clobal context, the
                // local context should be the target..
                for(entry_p *cur = cus->symbols;
                    cur && *cur && *cur != end(); cur++)
                {
                    // As always with symbols, ignore the case.
                    if(!strcasecmp((*cur)->name, (*sym)->name))
                    {
                        // Set destination to the local context
                        // when we have a match.
                        dst = cus;
                    }
                }
            }

            // If we manage to resolve the right hand
            // side, create a copy of its contents.
            if(!DID_ERR)
            {
                entry_p res = DBG_ALLOC(malloc(sizeof(entry_t)));

                if(res)
                {
                    // Do a deep copy of the value.
                    memmove(res, rhs, sizeof(entry_t));

                    // Copy name string if such exists.
                    if(res->name)
                    {
                        res->name = DBG_ALLOC(strdup(res->name));

                        if(!res->name)
                        {
                            // Out of memory.
                            PANIC(contxt);
                            free(res);
                            break;
                        }
                    }

                    // In non strict mode we might have a
                    // DANGLE on the right hand side if a
                    // bogus resolve was done. To prevent
                    // leaks we need to typecast the rhs.
                    if(res->type == DANGLE)
                    {
                        // Typecast to string. The string
                        // will be empty. If evaluated as
                        // a number, it will be zero.
                        res->type = STRING;
                    }

                    // Reparent the value and free the old
                    // resolved value if any. Also, create
                    // a reference from the global context
                    // to the symbol.
                    res->parent = *sym;
                    kill((*sym)->resolved);
                    (*sym)->resolved = res;
                    push(dst, *sym);
                    (*sym)->parent = contxt;
                }
                else
                {
                    // Out of memory.
                    PANIC(contxt);
                    break;
                }

                // Do we have any more tuples?
                if(*(++sym) == *(++val))
                {
                    // We're at the end of the list.
                    return res;
                }
            }
            else
            {
                // Unresolvable rhs.
                break;
            }
        }
    }
    else
    {
        // Broken parser.
        PANIC(contxt);
    }

    // Unresolvable right hand
    // side or broken parser.
    return end();
}

//----------------------------------------------------------------------------
// (symbolset <symbolname> <expression>)
//     assign a value to a variable named by the string result of
//     `<symbolname>' (V42.9)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_symbolset(entry_p contxt)
{
    // Symbol destination.
    entry_p dst = global(contxt);

    // We need one or more tuples of symbol
    // name and value.
    if(c_sane(contxt, 2) && dst)
    {
        entry_p ret = DCUR;
        entry_p *cur = contxt->children;

        // Iterate over all tuples.
        while(*cur && *cur != end())
        {
            // Resolve symbol name and value.
            const char *lhs = str(*cur++);
            entry_p rhs = resolve(*cur++);

            // Could we resolve both lhs and rhs?
            if(!DID_ERR)
            {
                // Create a copy of the evaluated rhs.
                entry_p res = DBG_ALLOC(malloc(sizeof(entry_t)));

                if(res)
                {
                    entry_p *sym = contxt->symbols;
                    memmove(res, rhs, sizeof(entry_t));

                    // Do a deep copy if necessary.
                    if(res->name)
                    {
                        res->name = DBG_ALLOC(strdup(res->name));

                        if(!res->name)
                        {
                            // Out of memory.
                            PANIC(contxt);
                            free(res);
                            break;
                        }
                    }

                    // In non strict mode we might have a
                    // DANGLE on the right hand side if a
                    // bogus resolve was done. To prevent
                    // leaks we need to typecast the rhs.
                    if(res->type == DANGLE)
                    {
                        // Typecast to string. The string
                        // will be empty. If evaluated as
                        // a number, it will be zero.
                        res->type = STRING;
                    }

                    // Do we already have a symbol
                    // with this name?
                    while(*sym && *sym != end())
                    {
                        // If true, replace its resolved
                        // value with the copy of the rhs
                        if(!strcasecmp((*sym)->name, lhs))
                        {
                            kill((*sym)->resolved);
                            (*sym)->resolved = res;
                            push(dst, *sym);
                            res->parent = *sym;
                            ret = res;
                            break;
                        }

                        // Iterate over all symbols in
                        // this context.
                        sym++;
                    }

                    // Is this is a new symbol?
                    if(ret != res)
                    {
                        entry_p nsm = new_symbol(DBG_ALLOC(strdup(lhs)));

                        if(nsm)
                        {
                            res->parent = nsm;
                            nsm->resolved = res;

                            // Append the symbol to the current
                            // context and create a global ref.
                            if(append(&contxt->symbols, nsm))
                            {
                                push(dst, nsm);
                                nsm->parent = contxt;
                                ret = res;
                                continue;
                            }

                            // Out of memory.
                            kill(nsm);
                        }
                    }
                    else
                    {
                        // Symbol exists. Its new
                        // value has already been
                        // set above.
                        continue;
                    }

                    // Out of memory.
                    kill(res);
                    break;
                }
                else
                {
                    // Out of memory.
                    PANIC(contxt);
                    RCUR;
                }
            }
            else
            {
                // Could not resolve either the lhs
                // or the rhs. Error will be set by
                // resolve().
                RCUR;
            }
        }

        // Return the last rhs.
        return ret;
    }

    // Broken parser
    RCUR;
}

//----------------------------------------------------------------------------
// (symbolval <symbolname>)
//     returns the value of the symbol named by the string expression
//     `<smbolval>' (V42.9)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_symbolval(entry_p contxt)
{
    // We need one argument, the name
    // of the symbol.
    if(c_sane(contxt, 1))
    {
        static entry_t entry = { .type = SYMREF };
        entry_p ret;

        // Initialize and resolve dummy.
        entry.parent = contxt;
        entry.id = contxt->id;
        entry.name = str(CARG(1));

        ret = resolve(&entry);

        // Return the resolved value if
        // the symbol could be found.
        if(!DID_ERR)
        {
            return ret;
        }

        // Symbol not found.
        RNUM(0);
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
}

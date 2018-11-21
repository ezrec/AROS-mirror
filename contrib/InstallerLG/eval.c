//----------------------------------------------------------------------------
// eval.c:
//
// Functions for evaluation of entry_t objects.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "args.h"
#include "error.h"
#include "eval.h"
#include "exit.h"
#include "gui.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------
// Name:        find_symbol
// Description: Find the referent of a symbolic reference.
// Input:       entry_p entry:  A symbolic reference, SYMREF.
// Return:      entry_p:        A symbol, SYMBOL, matching the symbolic
//                              reference. NULL if no match is found.
//----------------------------------------------------------------------------
entry_p find_symbol(entry_p entry)
{
    // Local variables have priority. This
    // currently implies function arguments
    // only. We could enable local (set),
    // but this might break old scripts, so
    // let's not do it.
    entry_p con = local(entry);

    if(con)
    {
        do
        {
            // Current.
            entry_p *tmp;

            // Iterate over all symbols in the current
            // context.
            for(tmp = con->symbols;
                tmp && *tmp && *tmp != end();
                tmp++)
            {
                // Return value.
                entry_p ret = *tmp;

                // The current item might be a CUSTOM
                // Only match SYMBOL:s, return if we
                // find a match.
                if(ret->type == SYMBOL &&
                   !strcasecmp(ret->name, entry->name))
                {
                    // Rearrange symbols to make the
                    // next lookup (if any) faster.
                    // Don't do this on user defined
                    // procedures though, symbols in
                    // those are positional (args).
                    if(ret->parent->type != CUSTOM)
                    {
                        *tmp = *(con->symbols);
                        *(con->symbols) = ret;
                    }

                    // Symbol found.
                    return ret;
                }
            }

            // Nothing found in the current context.
            // Climb one scope higher and try again.
            con = local(con->parent);
        }
        while(con);

        // Only fail if we're in 'strict' mode.
        if(get_numvar(global(entry), "@strict"))
        {
            // We found nothing.
            ERR_C(entry, ERR_UNDEF_VAR, entry->name);
        }
    }
    else
    {
        // Bad input.
        PANIC(entry);
    }

    // A failure will be evaluated as
    // as a zero or an empty string.
    return end();
}

//----------------------------------------------------------------------------
// Name:        resolve
// Description: Derive primitive from a complex type, something that can be
//              directly evaluated as a string or numerical value.
// Input:       entry_p entry:  An entry_t pointer to an object of any type.
// Return:      entry_p:        Pointer to an entry_t primitive.
//----------------------------------------------------------------------------
entry_p resolve(entry_p entry)
{
    // Is there anything to resolve?
    if(entry)
    {
        switch(entry->type)
        {
            // Symbols are resolved from birth.
            case SYMBOL:
                return entry->resolved;

            // A symbolic reference is resolved by resolving
            // the symbol it refers to.
            case SYMREF:
                return resolve(find_symbol(entry));

            // A context is resolved by executing all functions in it.
            case CONTXT:
                return invoke(entry);

            // Functions are resolved by executing them.
            case CUSREF:
            case NATIVE:
                return entry->call(entry);

            // Dynamic options are treated like functions.
            case OPTION:
                return entry->id == OPT_DYNOPT ?
                       entry->call(entry) : entry;

            // We already have a primitive.
            case NUMBER:
            case STRING:
            case DANGLE:
                return entry;

            // We should never end up here.
            case CUSTOM:
                break;
        }
    }

    // Bad input.
    PANIC(entry);

    // Failure.
    return end();
}

//----------------------------------------------------------------------------
// Name:        opt_to_int
// Description: Convert option to numerical value. This will translate string
//              representations of user levels to the corresponding numerical
//              value.
// Input:       entry_p entry:  An entry_t pointer to an OPTION object.
// Return:      int:            An integer representation of the input.
//----------------------------------------------------------------------------
static int opt_to_int(entry_p entry)
{
    // Resolve once.
    char *opt = str(entry);

    // Special treatment of (confirm).
    if(entry->id == OPT_CONFIRM)
    {
        // Ignore case.
        if(!strcasecmp(opt, "novice"))
        {
            // Refer to Installer.guide.
            return 0;
        }

        // Ignore case.
        if(!strcasecmp(opt, "average"))
        {
            // Refer to Installer.guide.
            return 1;
        }

        // Ignore case.
        if(!strcasecmp(opt, "expert"))
        {
            // Refer to Installer.guide.
            return 2;
        }
    }

    // Fall through.
    return atoi(opt);
}

//----------------------------------------------------------------------------
// Name:        num
// Description: Get integer representation of an entry. This implies resolving
//              it, and, if necessary, converting it.
// Input:       entry_p entry:  An entry_t pointer to an object of any type.
// Return:      int:            An integer representation of the input.
//----------------------------------------------------------------------------
int num(entry_p entry)
{
    // Is there anything to resolve?
    if(entry)
    {
        switch(entry->type)
        {
            // Translate options.
            case OPTION:
                return opt_to_int(entry);

            // These are numeric values:
            case DANGLE:
            case NUMBER:
                return entry->id;

            // Recur.
            case SYMBOL:
                return num(entry->resolved);

            // Recur.
            case SYMREF:
                return num(find_symbol(entry));

            // Recur.
            case CUSREF:
            case NATIVE:
                return num(entry->call(entry));

            // Attempt to convert string.
            case STRING:
                return atoi(entry->name);

            // We should never end up here.
            case CONTXT:
            case CUSTOM:
                break;
        }
    }

    // Bad input.
    PANIC(entry);

    // Failure.
    return 0;
}

//----------------------------------------------------------------------------
// Name:        tru
// Description: Get truth value of an entry. This implies resolving it, and,
//              if necessary, converting it. Non empty strings and non zero
//              numerical values are considered true, everythings else false.
// Input:       entry_p entry:  An entry_t pointer to an object of any type.
// Return:      int:            The truth value of the input.
//----------------------------------------------------------------------------
int tru(entry_p entry)
{
    // Is there anything to resolve?
    if(entry)
    {
        // Attempt to resolve it.
        entry_p e = resolve(entry);

        // Evaluate on success.
        if(!DID_ERR())
        {
            // Only numerical values and strings
            // can be true.
            if((e->type == NUMBER && e->id) ||
               (e->type == STRING && *(e->name)))
            {
                return 1;
            }
        }
    }
    else
    {
        // Bad input.
        PANIC(entry);
    }

    // False.
    return 0;
}

//----------------------------------------------------------------------------
// Name:        str
// Description: Get string representation of an entry. This implies resolving
//              it, and, if necessary, converting it.
// Input:       entry_p entry:  An entry_t pointer to an object of any type.
// Return:      int:            String representation of the input.
//----------------------------------------------------------------------------
char *str(entry_p entry)
{
    // Is there anything to resolve?
    if(entry)
    {
        switch(entry->type)
        {
            // Special treatment of options
            // with a single string argument.
            // Let other options fall through.
            case OPTION:
                switch(entry->id)
                {
                    case OPT_APPEND:
                    case OPT_CONFIRM:
                    case OPT_DEFAULT:
                    case OPT_DEST:
                    case OPT_DISK:
                    case OPT_INCLUDE:
                    case OPT_NEWNAME:
                    case OPT_PATTERN:
                    case OPT_SETDEFAULTTOOL:
                    case OPT_SETSTACK:
                    case OPT_SOURCE:
                    case OPT_OVERRIDE:
                    case OPT_GETDEFAULTTOOL:
                    case OPT_GETSTACK:
                    case OPT_GETTOOLTYPE:
                        // All the options above have a single
                        // child.
                        return str
                        (
                            entry->children ?
                            entry->children[0] : NULL
                        );

                    case OPT_HELP:
                    case OPT_PROMPT:
                        // (help) and (prompt) may have multiple
                        // childred that must be concatenated.
                        free(entry->name);
                        entry->name = get_chlstr(entry);

                        // On OOM, fall through.
                        if(entry->name)
                        {
                            return entry->name;
                        }
                        else
                        {
                            // OOM.
                            PANIC(entry);
                        }
                }

            // Dangling entries and options
            // are considered empty strings
            // with the exceptions above.
            case DANGLE:
                return "";

            // Strings and function names can
            // be returned directly.
            case CUSTOM:
            case STRING:
                return entry->name;

            // Recur.
            case SYMBOL:
                return str(entry->resolved);

            // Recur.
            case SYMREF:
                return str(find_symbol(entry));

            // Recur.
            case CUSREF:
            case NATIVE:
                return str(entry->call(entry));

            // Conversion. Please note the use
            // of NUMLEN.
            case NUMBER:
                // Have we converted this number to a
                // string before?
                if(!entry->name)
                {
                    entry->name = malloc(NUMLEN);
                }

                // On OOM, fall through and PANIC below.
                if(entry->name)
                {
                    snprintf(entry->name, NUMLEN, "%d", entry->id);
                    return entry->name;
                }

            // We should never end up here.
            case CONTXT:
                break;
        }
    }

    // Bad input.
    PANIC(entry);

    // Failure.
    return "";
}

//----------------------------------------------------------------------------
// Name:        invoke
// Description: Evaluate all children of a CONTXT. In most cases this implies
//              executing all executable children and return the return value
//              of the last executed function. If any of the functions in the
//              CONTXT fails, the execution will be aborted.
// Input:       entry_p entry:  An entry_t pointer to a CONTXT object.
// Return:      entry_p:        The last resolved value in the entry CONTXT.
//----------------------------------------------------------------------------
entry_p invoke(entry_p entry)
{
    // Expect failure.
    entry_p r = end();

    if(entry)
    {
        // Iterator.
        entry_p *c = entry->children;

        // Empty procedures are allowed, there
        // might be no children at all.
        if(c)
        {
            // As long as no one fails, resolve
            // all children and save the return
            // value of the last one.
            while(*c && *c != end() && !DID_ERR())
            {
                // Resolve and proceed.
                r = resolve(*c);
                c++;
            }
        }

        // Return the last value.
        return r;
    }

    // Bad input.
    PANIC(entry);

    // Failure.
    return r;
}

//----------------------------------------------------------------------------
// Name:        run
// Description: Run script. Setup and teardown of everything, GUI included.
// Input:       entry_p entry:  The start symbol, refer to the parser.
// Return:      -
//----------------------------------------------------------------------------
void run(entry_p entry)
{
    locale_init();

    // Initialize GUI before starting
    // the execution.
    if(gui_init())
    {
        // Execute the script.
        entry_p status = invoke(entry);

        // Execute the (onerror) function
        // on failure.
        if(DID_ERR() && !DID_HALT())
        {
            status = m_onerror(entry);
        }

        // Output what we have unless we're
        // running from WB.
        if(arg_argc(-1))
        {
            printf("%s\n", str(status));
        }

        // GUI teardown.
        gui_exit();
    }

    // i18n teardown.
    locale_exit();

    // Free AST
    kill(entry);
}

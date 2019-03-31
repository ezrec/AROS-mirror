//----------------------------------------------------------------------------
// alloc.c:
//
// Functions for allocation of entry_t data and closely related functions.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "control.h"
#include "procedure.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------
// Name:        new_contxt
// Description: Allocate CONTXT.
// Input:       -
// Return:      entry_p:    A CONTXT on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_contxt(void)
{
    // We rely on everything being set to '0'
    entry_p entry = DBG_ALLOC(calloc(1, sizeof(entry_t)));

    if(entry)
    {
        // A context contains variables (symbols)
        // and functions (children).
        entry_p *symbols, *children;

        // Memory for children and symbols, this will
        // be grown if necessary. Start with VECLEN.
        symbols = DBG_ALLOC(calloc(VECLEN + 1, sizeof(entry_p)));
        children = DBG_ALLOC(calloc(VECLEN + 1, sizeof(entry_p)));

        if(symbols && children)
        {
            // Type + above.
            entry->type = CONTXT;
            entry->symbols = symbols;
            entry->children = children;

            // Set sentinel values.
            entry->symbols[VECLEN] = end();
            entry->children[VECLEN] = end();

            // Success.
            return entry;
        }

        // Out of memory.
        free(symbols);
        free(children);
    }

    // Out of memory.
    PANIC(NULL);
    free(entry);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_number
// Description: Allocate NUMBER.
// Input:       int num:    The initial value.
// Return:      entry_p:    A NUMBER on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_number(int num)
{
    // We rely on everything being set to '0'
    entry_p entry = DBG_ALLOC(calloc(1, sizeof(entry_t)));

    if(entry)
    {
        // The value of a NUMBER
        // equals its ID.
        entry->type = NUMBER;
        entry->id = num;

        // Success.
        return entry;
    }

    // Out of memory.
    PANIC(NULL);

    // Failure
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_string
// Description: Allocate STRING.
// Input:       char *name:    A pointer to a null terminated string. The
//                             string won't be copied and it will be free:d
//                             by kill(...) so it must be allocated by the
//                             calling function.
// Return:      entry_p:       A STRING on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_string(char *name)
{
    if(name)
    {
        // We rely on everything being set to '0'
        entry_p entry = DBG_ALLOC(calloc(1, sizeof (entry_t)));

        if(entry)
        {
            // The value of a string
            // equals its name.
            entry->type = STRING;
            entry->name = name;

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'name',
    // we need to free it here, or else it
    // will leak when OOM.
    free(name);

    // Out of memory / bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_symbol
// Description: Allocate SYMBOL.
// Input:       char *name: The name of the symbol. The string won't be
//                          copied and it will be free:d by kill(...) so it
//                          must be allocated by the calling function.
// Return:      entry_p:    A SYMBOL on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_symbol(char *name)
{
    // All symbols have a name.
    if(name)
    {
        // We rely on everything being set to '0'
        entry_p entry = DBG_ALLOC(calloc(1, sizeof(entry_t)));

        if(entry)
        {
            // The value of the symbol will
            // dangle until the first (set).
            entry->resolved = end();
            entry->type = SYMBOL;
            entry->name = name;

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'name',
    // we need to free it here, or else it
    // will leak when OOM.
    free(name);

    // Out of memory / bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_custom
// Description: Allocate CUSTOM, a user defined procedure / function.
// Input:       char *name:   The name of the function. This string won't be
//                            copied and it will be free:d by kill(...) so it
//                            must be allocated by the calling function.
//              int line:     The source code line number.
//              entry_p sym:  A CONTXT with symbols or NULL.
//              entry_p chl:  A CONTXT with children, functions.
// Return:      entry_p:      A CUSTOM on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_custom(char *name, int line, entry_p sym, entry_p chl)
{
    // Functions must have a name, but
    // they can have an empty body.
    if(name)
    {
        // We rely on everything being set to '0'
        entry_p entry = DBG_ALLOC(calloc(1, sizeof(entry_t)));

        if(entry)
        {
            // Iter.
            entry_p *cur;

            // Top level.
            entry->id = line;
            entry->name = name;
            entry->type = CUSTOM;

            // If we have symbols, move them to our
            // new CUSTOM, adopt them and clear the
            // 'resolved' status.
            if(sym && sym->symbols)
            {
                // Transfer and kill the input.
                entry->symbols = sym->symbols;
                sym->symbols = NULL;
                kill(sym);

                // Iter.
                cur = entry->symbols;

                // Adopt and reset symbols.
                while(*cur && *cur != end())
                {
                    (*cur)->parent = entry;
                    (*cur)->resolved = end();
                    cur++;
                }
            }

            // If we have children, adopt them.
            if(chl && chl->children)
            {
                // Transfer and kill the input.
                entry->children = chl->children;
                chl->children = NULL;
                kill(chl);

                // Iter.
                cur = entry->children;

                // Adopt all children.
                while(*cur && *cur != end())
                {
                    (*cur)->parent = entry;
                    cur++;
                }
            }

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'name',
    // 'chl' and 'sym', we need to free them
    // or else they will leak when  OOM.
    free(name);
    kill(chl);
    kill(sym);

    // Out of memory / bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_symref
// Description: Allocate SYMREF, a reference to a symbol / variable.
// Input:       char *name: The name of the referenced symbol. This string
//                          won't be copied and it will be free:d by kill(...)
//                          so it must be allocated by the calling function.
//              int line:   The source code line number.
// Return:      entry_p:    A SYMREF on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_symref(char *name, int line)
{
    // All references must have a name and only
    // real line numbers are tolerated. Line no
    // is used in error messages when refering
    // to a non existent symbol.
    if(name && (line > 0))
    {
        // We rely on everything being set to '0'
        entry_p entry = DBG_ALLOC(calloc(1, sizeof(entry_t)));

        if(entry)
        {
            entry->type = SYMREF;
            entry->name = name;
            entry->id = line;

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'name',
    // we need to free it here, or else it
    // will leak when OOM.
    free(name);

    // Out of memory / bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        move_contxt
// Description: Move children and symbols from one context to another.
//              The empty source context will be freed afterwards.
// Input:       entry_p dst:    The destination context.
//              entry_p src:    The source context.
// Return:      -
//----------------------------------------------------------------------------
static void move_contxt(entry_p dst, entry_p src)
{
    if(dst && src)
    {
        // Iter.
        entry_p *sym = dst->symbols = src->symbols,
                *chl = dst->children = src->children;

        // Reparent children.
        while(*chl && *chl != end())
        {
            (*chl)->parent = dst;
            chl++;
        }

        // Reparent symbols.
        while(*sym && *sym != end())
        {
            (*sym)->parent = dst;
            sym++;
        }

        // Free the source.
        src->children = NULL;
        src->symbols = NULL;
        kill(src);

        // Success.
        return;
    }

    // Invalid input.
    PANIC(NULL);
}

//----------------------------------------------------------------------------
// Name:        new_native
// Description: Allocate NATIVE, a native, non-user-defined function.
// Input:       char *name:     The name of the function. This string won't
//                              be copied and it will be free:d by kill(...)
//                              so it must be allocated by the calling
//                              function. It's used for decoration purposes
//                              only, it doesn't affect the execution.
//              int line:       The source code line number.
//              call_t call:    A function pointer, the code to be executed.
//              entry_p chl:    The context of the function, if any.
//              type_t type:    Default return value data type.
// Return:      entry_p:        A NATIVE on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_native(char *name, int line, call_t call, entry_p chl, type_t type)
{
    // Even though not strictly necessary, we require
    // a name and a line number.
    if(call && name && (line > 0))
    {
        // We rely on everything being set to '0'
        entry_p entry = DBG_ALLOC(calloc(1, sizeof (entry_t)));

        if(entry)
        {
            // Allocate default return value if we
            // have a sane return value data type.
            if(type == NUMBER)
            {
                entry->resolved = new_number(0);
            }
            else if(type == STRING)
            {
                entry->resolved = new_string(strdup(""));
            }
            else if(type == DANGLE)
            {
                entry->resolved = end();
            }

            // Do we have a valid default return type?
            if(entry->resolved)
            {
                // ID and name are for debugging purposes
                // only.
                entry->id = line;
                entry->call = call;
                entry->type = NATIVE;
                entry->name = name;

                // Adopt children and symbols if any.
                if(chl && chl->type == CONTXT)
                {
                    move_contxt(entry, chl);
                }

                // The function is the parent of the
                // return value.
                entry->resolved->parent = entry;
                return entry;
            }

            // Out of memory.
            PANIC(NULL);
            free(entry);
        }
    }

    // All or nothing. Since we own 'name' and,
    // 'chl' we need to free them, or else we
    // will leak when OOM.
    free(name);
    kill(chl);

    // Bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_option
// Description: Allocate OPTION
// Input:       char *name:     The name of the option. This string won't
//                              be copied and it will be free:d by kill(...)
//                              so it must be allocated by the calling
//                              function. It's used for decoration purposes
//                              only, it doesn't affect the execution.
//              opt_t type:     The option type.
//              entry_p chl:    An optional context containing children.
// Return:      entry_p:        An OPTION on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_option(char *name, opt_t type, entry_p chl)
{
    // Although not strictly necessary, we required
    // a name of the option. For debugging purposes.
    if(name)
    {
        // We rely on everything being set to '0'
        entry_p entry = DBG_ALLOC(calloc(1, sizeof (entry_t)));

        if(entry)
        {
            // Let the type be our ID.
            entry->id = (int) type;
            entry->type = OPTION;
            entry->name = name;

            // Adopt whatever is in the optional
            // CONTXT, if any.
            if(chl && chl->type == CONTXT)
            {
                // This is for options that contain
                // more information than just 1 / 0.
                // E.g delopts and command.
                move_contxt(entry, chl);
            }

            // If this is a dynamic option, it must
            // be resolved to be used.
            if(type == OPT_DYNOPT)
            {
                // Set callback for resolving. Only
                // (if) statements are allowed.
                entry->call = m_if;
            }

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'name' and,
    // 'chl' we need to free them, or else we
    // will leak when OOM.
    free(name);
    kill(chl);

    // Out of memory / invalid input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_cusref
// Description: Allocate CUSREF
// Input:       char *name:     The name of the user-defined function to be
//                              invoked. This string won't be copied and
//                              it will be free:d by kill(...) so it must
//                              be allocated by the calling function.
//              int line:       The source code line number.
//              entry_p arg:    An optional context with function arguments.
// Return:      entry_p:        a CUSREF on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_cusref(char *name, int line, entry_p arg)
{
    // A real line number is required, used
    // in error messages when invoking a non
    // existant procedure.
    if(name && (line > 0))
    {
        // We rely on everything being set to '0'
        entry_p entry = DBG_ALLOC(calloc(1, sizeof (entry_t)));

        if(entry)
        {
            // The m_gosub function is used
            // as a trampoline.
            entry->id = line;
            entry->name = name;
            entry->call = m_gosub;
            entry->type = CUSREF;

            // Adopt function arguments, if any.
            if(arg && arg->type == CONTXT)
            {
                move_contxt(entry, arg);
            }

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'name' and,
    // 'arg' we need to free them, or else we
    // will leak when OOM.
    free(name);
    kill(arg);

    // Out of memory / invalid input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        append
// Description: Append entry to array. Grow array if necessary.
// Input:       entry_p **dest:  The array.
//              entry_p entry:   The entry.
// Return:      entry_p:         On success, the entry. On failure, NULL.
//----------------------------------------------------------------------------
entry_p append(entry_p **dest, entry_p entry)
{
    if(entry && dest && *dest)
    {
        size_t num = 0;

        // Find the first 'free' slot,
        // if there is one.
        while((*dest)[num] &&
              (*dest)[num] != end())
        {
            num++;
        }

        // No free slot available. We need
        // to allocate more memory.
        if((*dest)[num])
        {
            // We rely on everything being set to '0'. Make the
            // new array twice as big.
            entry_p *new = DBG_ALLOC(calloc((num << 1) + 1, sizeof(entry_p)));

            // Move everything to the new array.
            if(new)
            {
                new[num << 1] = end();
                memcpy(new, *dest, num * sizeof(entry_p));
                free(*dest);
                *dest = new;
            }
            else
            {
                // Out of memory. This is a very rude
                // way of not leaking memory when OOM.
                // Simply overwrite previous elements
                // after killing them of.
                kill((*dest)[0]);
                (*dest)[0] = entry;

                PANIC(NULL);

                // Failure.
                return entry;
            }
        }

        // Success.
        (*dest)[num] = entry;
        return entry;
    }

    // Bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        merge
// Description: Move and append all children of a context to another one. The
//              empty context will be killed.
// Input:       entry_p dst:    The destination context.
//              entry_p src:    The source context.
// Return:      entry_p:        The destination context.
//----------------------------------------------------------------------------
entry_p merge(entry_p dst, entry_p src)
{
    // Sanity check.
    if(dst && dst->children &&
       src && src->children)
    {
        // Number of children.
        size_t num = 0;

        // Count the number of source children.
        for(size_t i = 0; src->children[i] &&
            src->children[i] != end(); i++)
        {
            num++;
        }

        // Add the number of current children.
        for(size_t i = 0; dst->children[i] &&
            dst->children[i] != end(); i++)
        {
            num++;
        }

        // We rely on everything being set to '0'.
        // Make the new array big enough to hold
        // both source and (current) destination.
        entry_p *new = DBG_ALLOC(calloc(num + 1,
                       sizeof(entry_p)));

        if(new)
        {
            // Start over.
            num = 0;

            // Copy current destination children.
            for(size_t i = 0; dst->children[i] &&
                dst->children[i] != end(); i++)
            {
                new[num] = dst->children[i];
                new[num++]->parent = dst;
            }

            // Append children of the source.
            for(size_t i = 0; src->children[i] &&
                src->children[i] != end(); i++)
            {
                new[num] = src->children[i];
                new[num++]->parent = dst;
            }

            // Replace the old with the new and
            // add array sentinel.
            free(dst->children);
            dst->children = new;
            new[num] = end();
        }
        else
        {
            // Out of memory.
            PANIC(NULL);
        }
    }
    else
    {
        // Bad input.
        PANIC(NULL);
    }

    // No matter how things went above, we
    // own 'src' and need to free it, else
    // we will leak on success and if OOM.
    kill(src);

    // Return destination.
    return dst;
}

//----------------------------------------------------------------------------
// Name:        push
// Description: Type aware 'append' working on 'entry_t' level. Takes care of
//              children and symbols, while avoiding duplicates of the latter.
// Input:       entry_p dst:    The destination.
//              entry_p src:    The source.
// Return:      entry_p:        When successful, the source entry. On failure,
//                              NULL.
//----------------------------------------------------------------------------
entry_p push(entry_p dst, entry_p src)
{
    if(dst && src)
    {
        // Assume we're dealing with a child.
        entry_p **dst_p = &dst->children;

        // Symbols and user-defined procedures
        // are treated as symbols.
        if((src->type == SYMBOL ||
            src->type == CUSTOM) &&
           (dst->type == CONTXT ||
            dst->type == CUSTOM))
        {
            // We can't have multiple references
            // with the same name.
            for(size_t ndx = 0; dst->symbols[ndx] &&
                dst->symbols[ndx] != end(); ndx++)
            {
                char *old = dst->symbols[ndx]->name,
                     *new = src->name;

                // If we have multiple references
                // with the same name. Just update
                // the reference, don't create a
                // new one. Multiple references
                // is not OK, multiple referents
                // is OK. This implies that you can
                // redefine procedures dynamically,
                // refer to @onerror for an example.
                // Needless to say, this is ofcourse
                // also true for normal variables.
                if(!strcasecmp(old, new))
                {
                    // Variables set without (set) own
                    // themselves (refer to init() and
                    // init_num / str) must be killed
                    // before the reference is updated
                    // or else we will leak memory.
                    if(dst->symbols[ndx]->parent == dst)
                    {
                        kill(dst->symbols[ndx]);
                    }

                    dst->symbols[ndx] = src;
                    return dst;
                }

                // Next symbols / user-defined
                // procedure.
            //    u++;
            }

            dst_p = &dst->symbols;
        }

        // Whether symbol or child, the procedure
        // is the same, just append and reparent.
        if(*dst_p && append(dst_p, src))
        {
            src->parent = dst;
            return dst;
        }
    }

    // All or nothing. Since we own 'src',
    // we need to free it, or else we will
    // leak when OOM.
    kill(src);

    // Out of memory / bad input.
    PANIC(NULL);

    // Failure.
    return dst;
}

//----------------------------------------------------------------------------
// Name:        kill
// Description: Free the memory occupied by 'entry' and all its children.
// Input:       entry_p:    The entry_p to be free:d.
// Return:      -
//----------------------------------------------------------------------------
void kill(entry_p entry)
{
    // DANGLE entries are static,
    // no need to free them.
    if(entry && entry->type != DANGLE)
    {
        // All entries might have a
        // name. Set to NULL to make
        // pretty_print:ing possible.
        free(entry->name);
        entry->name = NULL;

        // Free symbols, if any.
        if(entry->symbols)
        {
            // Iter.
            entry_p *chl = entry->symbols;

            while(*chl && *chl != end())
            {
                // Free only the ones we own.
                // References can be anywhere.
                if((*chl)->parent == entry)
                {
                    // Recur to free symbol.
                    kill(*chl);
                }

                // Next symbol.
                chl++;
            }

            // Free the array itself.
            free(entry->symbols);
            entry->symbols = NULL;
        }

        // Free children, if any.
        if(entry->children)
        {
            // Iter.
            entry_p *chl = entry->children;

            while(*chl && *chl != end())
            {
                // Free only the ones we own.
                // References can be anywhere.
                if((*chl)->parent == entry)
                {
                    // Recur to free child.
                    kill(*chl);
                }

                // Next child.
                chl++;
            }

            // Free the array itself.
            free(entry->children);
            entry->children = NULL;
        }

        // If we have any resolved entries that
        // we own, free them.
        if(entry->resolved &&
           entry->resolved->parent == entry)
        {
            kill(entry->resolved);
            entry->resolved = NULL;
        }

        // Nothing but this entry left.
        free(entry);
    }
}

//----------------------------------------------------------------------------
// Name:        end
// Description: Get entry_p sentinel.
// Input:       -
// Return:      entry_p:    An entry_p which can be used as sentinel.
//----------------------------------------------------------------------------
entry_p end(void)
{
    // Zero / empty string.
    static entry_t entry =
    {
        .type = DANGLE,
        .id = 0,
        .name = ""
    };

    return &entry;
}


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
    entry_p entry = calloc(1, sizeof(entry_t));

    if(entry)
    {
        // A context contains variables (symbols)
        // and functions (children).
        entry_p *symbols, *children;

        // Memory for children and symbols, this will
        // be grown if necessary. Start with VECLEN.
        symbols = calloc(VECLEN + 1, sizeof(entry_p));
        children = calloc(VECLEN + 1, sizeof(entry_p));

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
// Input:       int n:      The initial value.
// Return:      entry_p:    A NUMBER on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_number(int n)
{
    // We rely on everything being set to '0'
    entry_p entry = calloc(1, sizeof(entry_t));

    if(entry)
    {
        // The value of a NUMBER
        // equals its ID.
        entry->type = NUMBER;
        entry->id = n;

        // Success.
        return entry;
    }
    else
    {
        // Out of memory.
        PANIC(NULL);
    }

    // Failure
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_string
// Description: Allocate STRING.
// Input:       char *n:    A pointer to a null terminated string. The
//                          string won't be copied and it will be free:d
//                          by kill(...) so it must be allocated by the
//                          calling function.
// Return:      entry_p:    A STRING on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_string(char *n)
{
    if(n)
    {
        // We rely on everything being set to '0'
        entry_p entry = calloc(1, sizeof (entry_t));

        if(entry)
        {
            // The value of a string
            // equals its name.
            entry->type = STRING;
            entry->name = n;

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'n',
    // we need to free it here, or else
    // it will leak when OOM.
    free(n);

    // Out of memory / bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_symbol
// Description: Allocate SYMBOL.
// Input:       char *n:    The name of the symbol. The string won't be
//                          copied and it will be free:d by kill(...) so it
//                          must be allocated by the calling function.
// Return:      entry_p:    A SYMBOL on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_symbol(char *n)
{
    // All symbols have a name.
    if(n)
    {
        // We rely on everything being set to '0'
        entry_p entry = calloc(1, sizeof(entry_t));

        if(entry)
        {
            // The value of the symbol will
            // dangle until the first (set).
            entry->resolved = end();
            entry->type = SYMBOL;
            entry->name = n;

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'n',
    // we need to free it here, or else
    // it will leak when OOM.
    free(n);

    // Out of memory / bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_custom
// Description: Allocate CUSTOM, a user defined procedure / function.
// Input:       char *n:    The name of the function. This string won't be
//                          copied and it will be free:d by kill(...) so it
//                          must be allocated by the calling function.
//              int l:      The source code line number.
//              entry_p s:  A CONTXT with symbols or NULL.
//              entry_p c:  A CONTXT with children, functions.
// Return:      entry_p:    A CUSTOM on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_custom(char *n, int l, entry_p s, entry_p c)
{
    // Functions must have a name, but
    // they can have an empty body.
    if(n)
    {
        // We rely on everything being set to '0'
        entry_p entry = calloc(1, sizeof(entry_t));

        if(entry)
        {
            // Iter.
            entry_p *e;

            // Top level.
            entry->id = l;
            entry->name = n;
            entry->type = CUSTOM;

            // If we have symbols, move them to our
            // new CUSTOM, adopt them and clear the
            // 'resolved' status.
            if(s && s->symbols)
            {
                // Transfer and kill the input.
                entry->symbols = s->symbols;
                s->symbols = NULL;
                kill(s);

                // Iter.
                e = entry->symbols;

                // Adopt and reset symbols.
                while(*e && *e != end())
                {
                    (*e)->parent = entry;
                    (*e)->resolved = end();
                    e++;
                }
            }

            if(c)
            {
                // Transfer and kill the input.
                entry->children = c->children;
                c->children = NULL;
                kill(c);

                // Iter.
                e = entry->children;

                // Adopt all children.
                while(*e && *e != end())
                {
                    (*e)->parent = entry;
                    e++;
                }
            }

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'n',
    // 'c' and 's', we need to free them
    // here, or else they will leak when
    // OOM.
    free(n);
    kill(c);
    kill(s);

    // Out of memory / bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_symref
// Description: Allocate SYMREF, a reference to a symbol / variable.
// Input:       char *n:    The name of the referenced symbol. This string
//                          won't be copied and it will be free:d by
//                          kill(...) so it must be allocated by the calling
//                          function.
//              int l:      The source code line number.
// Return:      entry_p:    A SYMREF on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_symref(char *n, int l)
{
    // All references must have a name and only
    // real line numbers are tolerated. Line no
    // is used in error messages when refering
    // to a non existent symbol.
    if(n && (l > 0))
    {
        // We rely on everything being set to '0'
        entry_p entry = calloc(1, sizeof(entry_t));

        if(entry)
        {
            entry->type = SYMREF;
            entry->name = n;
            entry->id = l;

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'n',
    // we need to free it here, or else
    // it will leak when OOM.
    free(n);

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
        entry_p *s = dst->symbols = src->symbols,
                *c = dst->children = src->children;

        // Reparent children.
        while(*c && *c != end())
        {
            (*c)->parent = dst;
            c++;
        }

        // Reparent symbols.
        while(*s && *s != end())
        {
            (*s)->parent = dst;
            s++;
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
// Input:       char *n:        The name of the function. This string won't
//                              be copied and it will be free:d by kill(...)
//                              so it must be allocated by the calling
//                              function. It's used for decoration purposes
//                              only, it doesn't affect the execution.
//              int l:          The source code line number.
//              call_t call:    A function pointer, the code to be executed.
//              entry_p e:      The context of the function, if any.
//              type_t r:       Default return value data type.
// Return:      entry_p:        A NATIVE on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_native(char *n, int l, call_t call, entry_p e, type_t r)
{
    // Even though not strictly necessary, we require
    // a name and a line number.
    if(call && n && (l > 0))
    {
        // We rely on everything being set to '0'
        entry_p entry = calloc(1, sizeof (entry_t));

        if(entry)
        {
            // Allocate default return value if we
            // have a sane return value data type.
            if(r == NUMBER)
            {
                entry->resolved = new_number(0);
            }
            else if(r == STRING)
            {
                entry->resolved = new_string(strdup(""));
            }
            else if(r == DANGLE)
            {
                entry->resolved = end();
            }

            // Do we have a valid default return type?
            if(entry->resolved)
            {
                // ID and name are for debugging purposes
                // only.
                entry->id = l;
                entry->call = call;
                entry->type = NATIVE;
                entry->name = n;

                // Adopt children and symbols if any.
                if(e && e->type == CONTXT)
                {
                    move_contxt(entry, e);
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

    // All or nothing. Since we own 'n' and,
    // 'e' we need to free them, or else we
    // will leak when OOM.
    free(n);
    kill(e);

    // Bad input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_option
// Description: Allocate OPTION
// Input:       char *n:        The name of the option. This string won't
//                              be copied and it will be free:d by kill(...)
//                              so it must be allocated by the calling
//                              function. It's used for decoration purposes
//                              only, it doesn't affect the execution.
//              opt_t t:        The option type.
//              entry_p e:      An optional context containing children.
// Return:      entry_p:        An OPTION on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_option(char *n, opt_t t, entry_p e)
{
    // Although not strictly necessary, we required
    // a name of the option. For debugging purposes.
    if(n)
    {
        // We rely on everything being set to '0'
        entry_p entry = calloc(1, sizeof (entry_t));

        if(entry)
        {
            // Let the type be our ID.
            entry->id = (int) t;
            entry->type = OPTION;
            entry->name = n;

            // Adopt whatever is in the optional
            // CONTXT, if any.
            if(e && e->type == CONTXT)
            {
                // This is for options that contain
                // more information than just 1 / 0.
                // E.g delopts and command.
                move_contxt(entry, e);
            }

            // If this is a dynamic option, it must
            // be resolved to be used.
            if(t == OPT_DYNOPT)
            {
                // Set callback for resolving. Only
                // (if) statements are allowed.
                entry->call = m_if;
            }

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'n' and,
    // 'e' we need to free them, or else we
    // will leak when OOM.
    free(n);
    kill(e);

    // Out of memory /
    // invalid input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        new_cusref
// Description: Allocate CUSREF
// Input:       char *n:        The name of the user-defined function to be
//                              invoked. This string won't be copied and
//                              it will be free:d by kill(...) so it must
//                              be allocated by the calling function.
//              int l:          The source code line number.
//              entry_p e:      An optional context with function arguments.
// Return:      entry_p:        a CUSREF on success, NULL otherwise.
//----------------------------------------------------------------------------
entry_p new_cusref(char *n, int l, entry_p e)
{
    // A real line number is required, used
    // in error messages when invoking a non
    // existant procedure.
    if(n && (l > 0))
    {
        // We rely on everything being set to '0'
        entry_p entry = calloc(1, sizeof (entry_t));

        if(entry)
        {
            // The m_gosub function is used
            // as a trampoline.
            entry->id = l;
            entry->name = n;
            entry->call = m_gosub;
            entry->type = CUSREF;

            // Adopt function arguments, if any.
            if(e && e->type == CONTXT)
            {
                move_contxt(entry, e);
            }

            // Success.
            return entry;
        }
    }

    // All or nothing. Since we own 'n' and,
    // 'e' we need to free them, or else we
    // will leak when OOM.
    free(n);
    kill(e);

    // Out of memory /
    // invalid input.
    PANIC(NULL);

    // Failure.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        append
// Description: Append entry to array. Grow array if necessary.
// Input:       entry_p **dst:  The array.
//              entry_p e:      The entry.
// Return:      entry_p:        On success, the entry. On failure, NULL.
//----------------------------------------------------------------------------
entry_p append(entry_p **dst, entry_p e)
{
    if(dst && *dst)
    {
        size_t n = 0;

        // Find the first 'free' slot,
        // if there is one.
        while((*dst)[n] &&
              (*dst)[n] != end())
        {
            n++;
        }

        // No free slot available. We need
        // to allocate more memory.
        if((*dst)[n])
        {
            // We rely on everything being set to '0'. Make the
            // new array twice as big.
            entry_p *new = calloc((n << 1) + 1, sizeof(entry_p));

            // Move everything to the new array.
            if(new)
            {
                new[n << 1] = end();
                memcpy(new, *dst, n * sizeof(entry_p));
                free(*dst);
                *dst = new;
            }
            else
            {
                // Out of memory. This is a very rude
                // way of not leaking memory when OOM.
                // Simply overwrite previous elements
                // after killing them of.
                kill((*dst)[0]);
                (*dst)[0] = e;

                PANIC(NULL);

                // Failure.
                return e;
            }
        }

        // Success.
        (*dst)[n] = e;
        return e;
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
        entry_p *s = src->children;
        entry_p **d = &dst->children;

        // For all children in the source
        // context append to destination
        // and reparent.
        while(*s && *s != end())
        {
            // We might run out of memory
            // but in that case append( )
            // will PANIC.
            if(append(d, *s))
            {
                (*s)->parent = dst;
            }

            // Next child.
            s++;
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
            dst->type == CONTXT)
        {
            // Index.
            size_t u = 0;

            // We can't have multiple references
            // with the same name.
            while(dst->symbols[u] &&
                  dst->symbols[u] != end())
            {
                entry_p cur = dst->symbols[u];
                char *old = cur->name,
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
                    dst->symbols[u] = src;
                    return dst;
                }

                // Next symbols / user-defined
                // procedure.
                u++;
            }

            dst_p = &dst->symbols;
        }

        // Whether symbol or child, the procedure
        // is the same, just append and reparent.
        if(*dst_p)
        {
            if(append(dst_p, src))
            {
                src->parent = dst;
                return dst;
            }
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
            entry_p *e = entry->symbols;

            while(*e && *e != end())
            {
                // Free only the ones we own.
                // References can be anywhere.
                if((*e)->parent == entry)
                {
                    // Recur to free symbol.
                    kill(*e);
                }

                // Next symbol.
                e++;
            }

            // Free the array itself.
            free(entry->symbols);
            entry->symbols = NULL;
        }

        // Free children, if any.
        if(entry->children)
        {
            // Iter.
            entry_p *e = entry->children;

            while(*e && *e != end())
            {
                // Free only the ones we own.
                // References can be anywhere.
                if((*e)->parent == entry)
                {
                    // Recur to free child.
                    kill(*e);
                }

                // Next child.
                e++;
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
    static entry_t e =
    {
        .type = DANGLE,
        .id = 0,
        .name = ""
    };

    return &e;
}


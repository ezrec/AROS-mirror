//----------------------------------------------------------------------------
// util.c:
//
// Misc utility functions primarily used by the 'native' methods.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "eval.h"
#include "util.h"

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef AMIGA
#include <clib/debug_protos.h>
#define DBG(...) KPrintF((CONST_STRPTR)__VA_ARGS__)
#else
#define DBG(...) fprintf(stderr, __VA_ARGS__)
#endif

//----------------------------------------------------------------------------
// Name:        ror
// Description: Rotate array of entries to the right.
// Input:       entry_p *e: Array of entries.
// Return:      -
//----------------------------------------------------------------------------
void ror(entry_p *entry)
{
    // We have an array and it contains something.
    if(entry && *entry)
    {
        int lst = 0;

        // Let 'lst' be the index of the last entry.
        while(entry[lst] && entry[lst] != end())
        {
            lst++;
        }
        lst--;

        // Nothing to do if we have < 2 entries.
        if(lst > 0)
        {
            // Save the last entry.
            entry_p last = entry[lst];

            // Shift the rest to the right.
            while(lst)
            {
                entry[lst] = entry[lst - 1];
                lst--;
            }

            // Put the saved entry first.
            entry[0] = last;
        }
    }
}

//----------------------------------------------------------------------------
// Name:        local
// Description: Find the next context going upwards in the tree.
// Input:       entry_p entry:  The starting point.
// Return:      entry_p:        The closest context found, or
//                              NULL if no context was found.
//----------------------------------------------------------------------------
entry_p local(entry_p entry)
{
    // Go upwards until we find what we're
    // looking for, or hit the (broken) top.
    for(entry_p contxt = entry; contxt;
        contxt = contxt->parent)
    {
        if(contxt->type == CONTXT ||
           contxt->type == CUSTOM)
        {
            return contxt;
        }

        if(contxt->parent == contxt)
        {
            pretty_print(contxt);
            break;
        }
    }

    // Nothing:
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        global
// Description: Find the root context.
// Input:       entry_p entry:  The starting point.
// Return:      entry_p:        The root context, or NULL
//                              if no context was found.
//----------------------------------------------------------------------------
entry_p global(entry_p entry)
{
    // Go all the way up.
    for(entry_p contxt = local(entry); contxt;)
    {
        // Find the next context.
        entry_p next = local(contxt->parent);

        // If there is no higher context, we're
        // at the global level.
        if(!next)
        {
            return contxt;
        }

        // Next level.
        contxt = next;
    }

    // Nothing:
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        custom
// Description: Find CUSTOM parent if such exists.
// Input:       entry_p entry:  The starting point.
// Return:      entry_p:        The CUSTUM entry, or NULL
//                              if no CUSTOM was found.
//----------------------------------------------------------------------------
entry_p custom(entry_p entry)
{
    // Check current level then go all the way up.
    for(entry_p cur = entry; cur; cur = cur->parent)
    {
        // Is this a CUSTOM procedure?
        if(cur->type == CUSTOM)
        {
            // Found it.
            return cur;
        }
    }

    // Not inside CUSTOM.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        get_opt
// Description: Find option of a given type in a context.
// Input:       entry_p contxt:  The context to search in.
//              opt_t type:      The type of option to search for.
// Return:      entry_p:         An OPTION entry if found, NULL otherwise.
//----------------------------------------------------------------------------
entry_p get_opt(entry_p contxt, opt_t type)
{
    // We need a context.
    if(contxt && contxt != end() &&
       contxt->children)
    {
        // And we need children.
        entry_p *child = contxt->children;

        // Real option or (optional)?
        if(contxt->type != OPTION)
        {
            // Iterate over all options.
            while(*child && *child != end())
            {
                entry_p entry = *child;

                // ID == the type of option.
                if(entry->type == OPTION)
                {
                    // Dynamic options need to
                    // be resolved before eval.
                    if(entry->id == OPT_DYNOPT)
                    {
                        // Replace dummy with
                        // its resolved value.
                        entry = resolve(entry);
                    }

                    // Have we found the right
                    // type of option?
                    if(entry->id == (int32_t) type)
                    {
                        // We found it.
                        return entry;
                    }
                }

                // Nope, next.
                child++;
            }
        }
        // An (optional) string.
        else
        {
            // Iterate over all strings.
            while(*child && *child != end())
            {
                if((type == OPT_FAIL && !strcmp(str(*child), "fail")) ||
                   (type == OPT_FORCE && !strcmp(str(*child), "force")) ||
                   (type == OPT_NOFAIL && !strcmp(str(*child), "nofail")) ||
                   (type == OPT_ASKUSER && !strcmp(str(*child), "askuser")) ||
                   (type == OPT_OKNODELETE && !strcmp(str(*child), "oknodelete")))
                {
                    return *child;
                }

                // Nope, next.
                child++;
            }
        }
    }

    // If in non strict mode, allow the absense
    // of (prompt) and (help).
    if((type == OPT_PROMPT || type == OPT_HELP)
        && !get_numvar(contxt, "@strict"))
    {
        // This will be resolved as an
        // emtpy string.
        return end();
    }

    // Nothing found.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        c_sane
// Description: Context sanity check used by the NATIVE functions to verify
//              that we have atleast the number of children needed and that
//              these are valid. If this fails, it typically, but not always,
//              means that we have a parser problem.
// Input:       entry_p contxt:  The context.
//              size_t num:      The number of children necessary.
// Return:      int:             1 if context is valid, 0 otherwise.
//----------------------------------------------------------------------------
int c_sane(entry_p contxt, size_t num)
{
    // Assume success;
    int status = 1;

    // All contexts used by NATIVE functions must allocate an
    // array for children, it could be empty though.
    if(contxt && contxt->children)
    {
        // Expect atleast n children.
        for(size_t i = 0; i < num; i++)
        {
            // Assume failure;
            status = 0;

            // Make sure that something exists.
            if(contxt->children[i] == NULL)
            {
                DBG("contxt->children[%d] == NULL\n", (int) i);
                break;
            }

            // Make sure that it's not a sentinel.
            if(contxt->children[i] == end())
            {
                DBG("contxt->children[%d] == end()\n", (int) i);
                break;
            }

            // Make sure that it belongs to us.
            if(contxt->children[i]->parent != contxt)
            {
                DBG("contxt->children[%d]->parent != %p\n", (int) i,
                    (void *) contxt);
                break;
            }

            // Make sure that all NATIVE functions have
            // a default return value.
            if(contxt->type == NATIVE && !contxt->resolved)
            {
                DBG("contxt->type == NATIVE && !contxt->resolved\n");
                break;
            }

            // Make sure that all objects except CONTXTs
            // and NUMBERs have a name.
            if(!contxt->children[i]->name &&
                contxt->children[i]->type != CONTXT &&
                contxt->children[i]->type != NUMBER)
            {
                DBG("!contxt->children[i]->name && "
                    "contxt->children[i]->type != CONTXT && "
                    "contxt->children[i]->type != NUMBER\n");
                break;
            }

            // We're OK;
            status = 1;
        }
    }
    else
    {
        // Badly broken.
        status = 0;
        DBG("!contxt || !contxt->children\n");
    }

    return status;
}

//----------------------------------------------------------------------------
// Name:        s_sane
// Description: Context sanity check used by the NATIVE functions to verify
//              that we have atleast the number of symbols needed and that
//              these are valid. If this fails it typically, but not always,
//              means that we have a parser problem.
// Input:       entry_p contxt:  The context.
//              size_t num:      The number of symbols necessary.
// Return:      int:             1 if context is valid, 0 otherwise.
//----------------------------------------------------------------------------
int s_sane(entry_p contxt, size_t num)
{
    // Assume success;
    int status = 1;

    if(contxt && contxt->symbols)
    {
        // Expect atleast num symbols.
        for(size_t i = 0; i < num; i++)
        {
            // Assume failure;
            status = 0;

            // Make sure that somethings exists.
            if(contxt->symbols[i] == NULL)
            {
                DBG("contxt->symbols[%d] == NULL\n", (int) i);
                break;
            }

            // Make sure that it's not a sentinel.
            if(contxt->symbols[i] == end())
            {
                DBG("contxt->symbols[%d] == end()\n", (int) i);
                break;
            }

            // Make sure that it has a name.
            if(!contxt->symbols[i]->name)
            {
                DBG("!contxt->symbols[%d]->name\n", (int) i);
                break;
            }

            // Make sure that it belongs to us.
            if(contxt->symbols[i]->parent != contxt)
            {
                DBG("contxt->symbols[%d]->parent != %p\n", (int) i,
                    (void *) contxt);
                break;
            }

            // We're OK;
            status = 1;
        }
    }
    else
    {
        // Badly broken.
        status = 0;
        DBG("!contxt || !contxt->symbols\n");
    }

    return status;
}

//----------------------------------------------------------------------------
// Name:        set_numvar
// Description: Give an existing numerical variable a new value. Please note
//              that the variable must exist and that the current resolved
//              value must be a NUMBER, if not, this function will silently
//              fail.
// Input:       entry_p contxt:  The context.
//              char *var:       The name of the variable.
//              int val:         The new value of the variable.
// Return:      -
//----------------------------------------------------------------------------
void set_numvar(entry_p contxt, char *var, int val)
{
    // Dummy reference used for searching.
    static entry_t ref = { .type = SYMREF };

    // We need a name and a context.
    if(contxt && var)
    {
        // Name and reparent.
        ref.parent = contxt;
        ref.name = var;

        // Find whatever 'var' is.
        entry_p sym = find_symbol(&ref);

        // This should be a symbol. And it
        // should be a resolved numerical one.
        if(sym && sym->type == SYMBOL && sym->resolved &&
           sym->resolved->type == NUMBER)
        {
            // Success.
            sym->resolved->id = val;
            return;
        }
    }

    // Failure.
}

//----------------------------------------------------------------------------
// Name:        get_numvar
// Description: Get the value of an existing numerical variable. Please note
//              that the variable must exist and that the current resolved
//              value must be a NUMBER.
// Input:       entry_p contxt:  The context.
//              char *var:       The name of the variable.
// Return:      int:             The value of the variable or zero if the
//                               variable can't be found.
//----------------------------------------------------------------------------
int get_numvar(entry_p contxt, char *var)
{
    // We need a name and a context.
    if(contxt && var)
    {
        // Dummy reference used to find
        // the variable.
        entry_p sym;
        static entry_t ref = { .type = SYMREF };

        // Name and reparent dummy.
        ref.name = var;
        ref.parent = contxt;

        // Find whatever 'var' is.
        sym = find_symbol(&ref);

        // This should be a symbol. And it
        // should be a resolved numerical one.
        if(sym && sym->type == SYMBOL && sym->resolved &&
           sym->resolved->type == NUMBER)
        {
            // Success.
            return sym->resolved->id;
        }
    }

    // Failure.
    return 0;
}

//----------------------------------------------------------------------------
// Name:        get_strvar
// Description: Get the value of an existing string variable. Please note
//              that the variable must exist and that the current resolved
//              value must be a STRING.
// Input:       entry_p contxt:  The context.
//              char *var:       The name of the variable.
// Return:      char *:          The value of the variable or an empty string
//                               if the variable can't be found.
//----------------------------------------------------------------------------
char *get_strvar(entry_p contxt, char *var)
{
    // We need a name and a context.
    if(contxt && var)
    {
        // Dummy reference used to find
        // the variable.
        entry_p sym;
        static entry_t ref = { .type = SYMREF };

        // Name and reparent dummy.
        ref.name = var;
        ref.parent = contxt;

        // Find whatever 'v' is.
        sym = find_symbol(&ref);

        // This should be a symbol. And it
        // should be a resolved string.
        if(sym && sym->type == SYMBOL &&
           sym->resolved && sym->resolved->name &&
           sym->resolved->type == STRING)
        {
            // Success.
            return sym->resolved->name;
        }
    }

    // Failure.
    return "";
}

//----------------------------------------------------------------------------
// Name:        get_optstr
// Description: Concatenate all the strings in all the options of a given
//              type in contxt.
// Input:       entry_p contxt:    The context.
//              opt_t type:        The option type.
// Return:      char *:            A concatenation of all the strings found.
//----------------------------------------------------------------------------
char *get_optstr(entry_p contxt, opt_t type)
{
    size_t num = 0;
    entry_p *child = contxt->children;

    // Count the number of children
    // of the given option type.
    while(*child && *child != end())
    {
        if((*child)->type == OPTION &&
           (*child)->id == (int32_t) type)
        {
            num++;
        }

        // Next child.
        child++;
    }

    // Did we find the right type?
    if(num)
    {
        // References to string evaluations of all
        // options of the right type.
        char **str = DBG_ALLOC(calloc(num + 1, sizeof(char *)));

        if(str)
        {
            // Empty string.
            child = contxt->children;
            size_t len = 1;

            // For all children, evaluate them
            // once and save string pointers.
            for(size_t i = 0; i < num && *child
                && *child != end(); child++)
            {
                if((*child)->id == (int32_t) type &&
                   (*child)->type == OPTION)
                {
                    // Sum up the length.
                    char *cur = get_chlstr(*child, false);

                    if(cur)
                    {
                        str[i++] = cur;
                        len += strlen(cur);
                    }
                }
            }

            // Allocate memory to hold the
            // concatenation of all strings.
            char *ret = DBG_ALLOC(calloc(len, 1));

            if(ret)
            {
                // Concatenate substrings.
                for(size_t i = 0; str[i]; i++)
                {
                    strncat(ret, str[i], len - strlen(ret));
                }
            }

            // Free substrings.
            for(size_t i = 0; str[i]; i++)
            {
                free(str[i]);
            }

            free(str);

            // Done.
            return ret;
        }

        // Out of memory
    }

    // Not found / OOM.
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        get_chlst
// Description: Concatenate the string representations of all non context
//              children of a context.
// Input:       entry_p contxt:  The context.
//              bool pad:        Whitespace padding.
// Return:      char *:          The concatenation of the string representations
//                               of all non context children of 'contxt'.
//----------------------------------------------------------------------------
char *get_chlstr(entry_p contxt, bool pad)
{
    // Concatenation.
    char *ret = NULL;

    // We don't really need anything to
    // concatenate but we expect a sane
    // contxt.
    if(c_sane(contxt, 0))
    {
        size_t num = 0;
        entry_p *child = contxt->children;

        // Count the number of non context children.
        while(*child && *child != end())
        {
            num += ((*child)->type != CONTXT) ? 1 : 0;
            child++;
        }

        if(num)
        {
            // Allocate memory to hold a string pointer
            // for each child.
            char **stv = DBG_ALLOC(calloc(num + 1, sizeof(char *)));

            if(stv)
            {
                // Total length.
                size_t len = 0;

                // Save all string pointers so that we don't
                // evaluate children twice and thereby set of
                // side effects more than once.
                while(num > 0)
                {
                    entry_p cur = *(--child);

                    if(cur->type != CONTXT)
                    {
                        stv[--num] = str(cur);
                        len += strlen(stv[num]);

                        // Insert whitespace between strings?
                        if(pad)
                        {
                            // Make room for whitespace.
                            len++;
                        }
                    }
                }

                // If the total length is non zero, we will
                // concatenate all children.
                if(len)
                {
                    // Allocate memory to hold the full
                    // concatenated string.
                    ret = DBG_ALLOC(calloc(len + 1, 1));

                    if(ret)
                    {
                        // The concatenation. The 'stv'
                        // array is null terminated.
                        while(stv[num])
                        {
                            strncat(ret, stv[num], len + 1 - strlen(ret));
                            num++;

                            // Is padding enabled and is this not
                            // the final string?
                            if(pad && stv[num])
                            {
                                // Insert whitespace.
                                strncat(ret, " ", len + 1 - strlen(ret));
                            }
                        }
                    }
                }
                else
                {
                    // No data to concatenate.
                    ret = DBG_ALLOC(strdup(""));
                }

                // Free the references before
                // returning.
                free(stv);
            }
        }
        else
        {
            // No children to concatenate.
            ret = DBG_ALLOC(strdup(""));
        }
    }

    // We could be in any state
    // here, success or panic.
    return ret;
}

//----------------------------------------------------------------------------
// Name:        set_strvar
// Description: Give an existing string variable a new value. Please note
//              that the variable must exist and that the current resolved
//              value must be a STRING, if not, this function will silently
//              fail.
// Input:       entry_p contxt:  The context.
//              char *var:       The name of the variable.
//              char *val:       The new value of the variable.
// Return:      -
//----------------------------------------------------------------------------
void set_strvar(entry_p contxt, char *var, char *val)
{
    // We need a name and a context.
    if(contxt && var)
    {
        // Dummy reference used to find
        // the variable.
        entry_p sym;
        static entry_t ref = { .type = SYMREF };

        // Name and reparent dummy.
        ref.name = var;
        ref.parent = contxt;

        // Find whatever 'v' is.
        sym = find_symbol(&ref);

        // This should be a symbol. And it
        // should be a resolved string.
        if(sym && sym->type == SYMBOL &&
           sym->resolved &&
           sym->resolved->type == STRING)
        {
            // Taking ownership of 'val'.
            free(sym->resolved->name);
            sym->resolved->name = val;
        }
    }
}

//----------------------------------------------------------------------------
// Name:        pp_aux
// Description: Refer to pretty_print below.
// Input:       entry_p entry:  The tree to print.
//              int indent:     Indentation level.
// Return:      -
//----------------------------------------------------------------------------
static void pp_aux(entry_p entry, int indent)
{
    // Indentation galore.
    char ind[16] = "\t\t\t\t"
                   "\t\t\t\t"
                   "\t\t\t\t"
                   "\t\t\t\0";

    // Going backwards to go forward.
    char *type = ind + sizeof(ind) - 1 - indent;
    type = type < ind ? ind : type;

    // NULL is a valid value.
    if(entry)
    {
        // Data type descriptions.
        char *tps[] =
        {
            "NUMBER",
            "STRING",
            "SYMBOL",
            "SYMREF",
            "NATIVE",
            "OPTION",
            "CUSTOM",
            "CUSREF",
            "CONTXT",
            "STATUS",
            "DANGLE"
        };

        // All entries have a type, a parent and an ID.
        DBG("%s\n", tps[entry->type]);
        DBG("%sThis:%p\n", type, (void *) entry);
        DBG("%sParent:%p\n", type, (void *) entry->parent);
        DBG("%sId:\t%d\n", type, entry->id);

        // Most, but not all, have a name.
        if(entry->name)
        {
            DBG("%sName:\t%s\n", type, entry->name);
        }

         // Natives and cusrefs have callbacks.
        if(entry->call)
        {
            DBG("%sCall:\t%p\n", type, (void *) entry->call);
        }

        // Functions / symbols can be 'resolved'.
        if(entry->resolved)
        {
            DBG("%sRes:\t", type);

            // Pretty print the 'resolved' entry,
            // last / default return value and
            // values refered to by symbols.
            pp_aux(entry->resolved, indent + 1);
        }

        // Pretty print all children.
        if(entry->children)
        {
            entry_p *child = entry->children;

            while(*child && *child != end())
            {
                DBG("%sChl:\t", type);
                pp_aux(*child, indent + 1);
                child++;
            }
        }

        // Pretty print all symbols.
        if(entry->symbols)
        {
            entry_p *sym = entry->symbols;

            while(*sym && *sym != end())
            {
                DBG("%sSym:\t", type);
                pp_aux(*sym, indent + 1);
                sym++;
            }
        }
    }
    else
    {
        DBG("NULL\n\n");
    }
}

//----------------------------------------------------------------------------
// Name:        pretty_print
// Description: Pretty print the complete tree in 'entry'.
// Input:       entry_p entry:  The tree to print.
// Return:      -
//----------------------------------------------------------------------------
void pretty_print(entry_p entry)
{
    // Start with no indentation.
    pp_aux(entry, 0);
}

#define LG_BUFSIZ ((BUFSIZ > PATH_MAX ? BUFSIZ : PATH_MAX) + 1)

//----------------------------------------------------------------------------
// Name:        get_buf
// Description: Get pointer to temporary buffer.
// Input:       -
// Return:      char *: Buffer pointer.
//----------------------------------------------------------------------------
char *get_buf(void)
{
    static char buf[LG_BUFSIZ];
    return buf;
}

//----------------------------------------------------------------------------
// Name:        buf_size
// Description: Get size of temporary buffer.
// Input:       -
// Return:      size_t: Buffer size.
//----------------------------------------------------------------------------
size_t buf_size(void)
{
    return LG_BUFSIZ - 1;
}

//----------------------------------------------------------------------------
// Name:        dbg_alloc
// Description: Used by the DBG_ALLOX macro to provide more info when failing
//              to allocate memory. And to fail deliberately when testing.
// Input:       int line: Source code line.
//              const char *file: Source code file.
//              const char *func: Source code function.
//              void *mem: Pointer to allocated memory.
// Return:      void *: Pointer to allocated memory.
//----------------------------------------------------------------------------
void *dbg_alloc(int line, const char *file, const char *func, void *mem)
{
    // Fail deliberately if file or line defines are set.
    #if defined(FAIL_LINE) || defined(FAIL_FILE)
    const char *fail_file =
    #ifdef FAIL_FILE
    FAIL_FILE;
    #else
    NULL;
    #endif
    int fail_line =
    #ifdef FAIL_LINE
    FAIL_LINE;
    #else
    0;
    #endif

    // Do we have a line number restriction?
    if(!fail_line || (line == fail_line))
    {
        // Do we have a file restriction?
        if(!fail_file || (fail_file && !strcmp(file, fail_file)))
        {
            // Free memory and pass NULL to the calling function.
            free(mem);
            mem = NULL;
        }
    }
    #endif

    // Debug info.
    if(!mem)
    {
        DBG("Out of memory in %s (%s) line %d\n", func, file, line);
    }

    // Pass this on.
    return mem;
}

//----------------------------------------------------------------------------
// Name:        native_exists
// Description: Find first occurence of callback in AST.
// Input:       entry_p contxt:  CONTXT.
// Return:      entry_p:         NATIVE callback if found, NULL otherwise.
//----------------------------------------------------------------------------
entry_p native_exists(entry_p contxt, call_t func)
{
    entry_p entry = NULL;

    // NULL are valid values.
    if(contxt && contxt->children)
    {
        // Iterate over all children and
        // recur if needed.
        for(entry_p *c = contxt->children;
            *c && *c != end() && !entry; c++)
        {
            if((*c)->type == NATIVE &&
               (*c)->call == func)
            {
                // We found it.
                entry = *c;
            }
            else
            {
                // Recur.
                entry = native_exists(*c, func);
            }
        }
    }

    // NULL or callback.
    return entry;
}


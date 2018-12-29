//----------------------------------------------------------------------------
// strop.c:
//
// String operations
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "eval.h"
#include "strop.h"
#include "util.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef AMIGA
#include <dos/dos.h>
#include <proto/dos.h>
#endif

//----------------------------------------------------------------------------
// (cat <string1> <string2>...)
//     returns concatenation of strings
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_cat(entry_p contxt)
{
    // We need atleast one string.
    if(c_sane(contxt, 1))
    {
        // Start with a string length of 64.
        size_t n = 64;
        char *buf = calloc(n + 1, 1);

        if(buf)
        {
            size_t l = 0, i = 0;

            // Iterate over all arguments.
            while(contxt->children[i] &&
                  contxt->children[i] != end())
            {
                // Resolve and get a string representation
                // of the current argument.
                const char *s = str(contxt->children[i++]);
                size_t ln;

                // If we couldn't resolve the current argument,
                // return an empty string.
                if(DID_ERR())
                {
                    free(buf);
                    REST;
                }

                // Get length of the current argument.
                ln = strlen(s);

                // If the length is > 0, append to the result.
                if(ln)
                {
                    l += strlen(s);

                    // If we're about to exceed the current buffer,
                    // allocate a new one big enough.
                    if(l > n)
                    {
                        char *tmp;

                        // Double up until we have enough.
                        while(n && n < l)
                        {
                            n = n << 1;
                        }

                        tmp = calloc(n + 1, 1);

                        // Copy the contents to the new buffer
                        // and free the old one.
                        if(tmp && n)
                        {
                            memcpy(tmp, buf, l - ln + 1);
                            free(buf);
                            buf = tmp;
                        }
                        else
                        {
                            // Out of memory.
                            PANIC(contxt);
                            free(tmp);
                            free(buf);
                            REST;
                        }
                    }

                    // By now we're ready to append.
                    strncat(buf, s, n - strlen(buf));
                }
            }

            // Unless we're out of memory, buf will
            // will contain the concatenation of all
            // the children.
            RSTR(buf);
        }
    }
    // The parser isn't necessarily broken
    // if we end up here. We could alse be
    // out of memory.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// ("<fmt>" <expr1> <expr2>)
//     returns a formatted string
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_fmt(entry_p contxt)
{
    // The format string is in the name of this contxt. It will hold
    // a maximum of length / 2 of specifiers.
    char *ret = NULL, *fmt = contxt ? contxt->name : NULL;
    char **sct = fmt ? calloc((strlen(fmt) >> 1) + 1, sizeof(char *)) : NULL;

    if(sct)
    {
        size_t i = 0, j = 0, k = 0, l = 0;
        entry_p *arg = contxt->children;

        // Scan the format string.
        for(; fmt[i]; i++)
        {
            // If we have a specifier,
            if(fmt[i] == '%')
            {
                // that's not preceeded by an escape.
                if(!i || fmt[i - 1] != '\\')
                {
                    i++;

                    // If this is a specifier that we recognize,
                    // then allocate a new string with just this
                    // specifier, nothing else.
                    if(fmt[i] == 's' || (
                       fmt[i++] == 'l' &&
                       fmt[i] && fmt[i] == 'd'))
                    {
                        sct[k] = calloc(i - j + 2, 1);

                        if(sct[k])
                        {
                            memcpy(sct[k], fmt + j, i - j + 1);
                            j = i + 1;
                            k++;
                        }
                        else
                        {
                            // Out of memory
                            PANIC(contxt);
                        }
                    }
                    else
                    {
                        ERR(ERR_FMT_INVALID, contxt->name);
                        break;
                    }
                }
            }
        }

        // Iterate over all format specifiers and arguments and do
        // the appropriate conversions and formating.
        if(k)
        {
            for(k = 0; sct[k]; k++)
            {
                if(arg && *arg &&
                   *arg != end())
                {
                    size_t oln = strlen(sct[k]);
                    entry_p cur = resolve(*arg);

                    // Bail out if we didn't manage to
                    // resolve the current argument.
                    if(DID_ERR())
                    {
                        arg = NULL;
                        break;
                    }

                    // Format string.
                    if(sct[k][oln - 1] == 's' &&
                       cur->type == STRING)
                    {
                        size_t nln = oln + strlen(cur->name);
                        char *new = calloc(nln + 1, 1);

                        // Replace the current format string with
                        // the corresponding formated string.
                        if(new)
                        {
                            int n = snprintf(new, nln, sct[k], cur->name);

                            l += n > 0 ? (size_t) n : 0;
                            free(sct[k]);
                            sct[k] = new;
                        }
                        else
                        {
                            // Out of memory
                            PANIC(contxt);
                        }
                    }
                    // Format numeric value.
                    else if(sct[k][oln - 1] == 'd' &&
                            cur->type == NUMBER)
                    {
                        size_t nln = oln + NUMLEN;
                        char *new = calloc(nln + 1, 1);

                        // Replace the current format string with
                        // the corresponding formated string.
                        if(new)
                        {
                            int n = snprintf(new, nln, sct[k], cur->id);

                            l += n > 0 ? (size_t) n : 0;
                            free(sct[k]);
                            sct[k] = new;
                        }
                        else
                        {
                            // Out of memory
                            PANIC(contxt);
                        }
                    }
                    else
                    {
                        // Fail on argument -> specifier mismatch.
                        ERR(ERR_FMT_MISMATCH, contxt->name);
                    }

                    // Next specifier -> argument.
                    arg++;
                }
                else
                {
                    // Fail if the number of arguments and the number
                    // of specifiers don't match.
                    ERR(ERR_FMT_MISSING, contxt->name);
                    break;
                }
            }
        }

        // Concatenate all formated strings.
        if(k && l)
        {
            // Allocate memory to hold all of them.
            l += strlen(fmt + j) + 1;
            ret = calloc(l, 1);

            if(ret)
            {
                // All format strings.
                for(k = 0; sct[k]; k++)
                {
                    strncat(ret, sct[k], l - strlen(ret));
                }

                // Suffix.
                strncat(ret, fmt + j, l - strlen(ret));
            }
            else
            {
                // Out of memory
                PANIC(contxt);
            }
        }

        // Free all temporary format strings.
        for(k = 0; sct[k]; k++)
        {
            free(sct[k]);
        }

        free(sct);

        // Without format specifiers, the format string
        // is the return value.
        if(!k)
        {
            ret = strdup(fmt);
        }

        // Fail if the number of arguments and the number
        // of specifiers don't match, and we're in strict
        // mode.
        if(arg && *arg && *arg != end() &&
           get_numvar(contxt, "@strict"))
        {
            ERR(ERR_FMT_UNUSED, contxt->name);
        }
        else if(ret)
        {
            // Success.
            RSTR(ret);
        }
    }
    else
    {
        // Either something is terribly
        // broken or we're out of memory
        PANIC(contxt);
        free(sct);
        RCUR;
    }

    // Return empty string
    // on failure.
    free(ret);
    REST;
}

//----------------------------------------------------------------------------
// (pathonly <path>)
//     return dir part of path (see fileonly)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_pathonly(entry_p contxt)
{
    // We need one argument, a full path.
    if(c_sane(contxt, 1))
    {
        const char *s = str(CARG(1));
        size_t i = strlen(s);

        // Scan backwards.
        while(i--)
        {
            // If we find a delimiter, then
            // we have the path to the left
            // of it.
            if(s[i] == '/' ||
               s[i] == ':' )
            {
                // Get termination for free.
                char *r = calloc(i + 2, 1);

                if(r)
                {
                    // Copy full path.
                    memcpy(r, s, i + 1);

                    // Cut trailing '/' if preceeded
                    // by something absolute, dir or
                    // volume.
                    if(i > 1 &&
                       r[i] == '/' &&
                       r[i - 1] != '/' &&
                       r[i - 1] != ':')
                    {
                        r[i] = '\0';
                    }

                    RSTR(r);
                }
                else
                {
                    // Out of memory.
                    PANIC(contxt);
                }
            }
        }

        // Return empty string
        // on failure.
        REST;
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (patmatch <pattern> <string>)
//     Does <pattern> match <string> ? TRUE : FALSE
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_patmatch(entry_p contxt)
{
    // We need two arguments
    if(c_sane(contxt, 2))
    {
        #ifdef AMIGA
        // Use the global buffer.
        char *buf = get_buf(),
             *p = str(CARG(1)),
             *m = str(CARG(2));

        LONG w = ParsePattern(p, buf, buf_size());

        // Can we parse the pattern?
        if(w >= 0)
        {
            // Use pattern matching if we have one or more
            // wildcards, otherwise use plain strcmp().
            int r = w ? MatchPattern(buf, m) : !strcmp(p, m);
            RNUM(r ? 1 : 0);
        }
        else
        {
            // We probably had a buffer overflow.
            ERR(ERR_OVERFLOW, p);
            RNUM(0);
        }
        #else
        RNUM(0);
        #endif
    }
    else
    {
        // The parser is broken.
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (strlen <string>)
//     returns string length
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_strlen(entry_p contxt)
{
    // We need one argument, the string.
    if(c_sane(contxt, 1))
    {
        RNUM
        (
            (int) strlen(str(CARG(1)))
        );
    }
    else
    {
        // The parser is broken.
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (substr <string> <start> [<count>])
//     returns a substring of <string>
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_substr(entry_p contxt)
{
    if(c_sane(contxt, 2))
    {
        char *s = str(CARG(1));
        int i = num(CARG(2)),
            l = (int) strlen(s);

        // Is the number characters limited?
        if(CARG(3) && CARG(3) != end())
        {
            // Get the number of characters
            // to copy.
            int j = num(CARG(3));

            // Use the limitations used by the
            // CBM installer.
            if(i < l && j > 0 && i >= 0)
            {
                char *r = calloc((size_t) l + 1, 1);

                if(r)
                {
                    // Cap all values and do the
                    // actual copy.
                    l -= i;
                    l = l < j ? l : j;
                    memcpy(r, s + i, l);
                    RSTR(r);
                }
                else
                {
                    // Out of memory.
                    PANIC(contxt);
                    REST;
                }
            }
        }
        else
        // No, copy until the end of the string.
        {
            // Max cap
            if(i < l)
            {
                // Min cap
                if(i > 0)
                {
                    char *r = calloc((size_t) l + 1, 1);

                    if(r)
                    {
                        // All values are already
                        // capped, just copy.
                        memcpy(r, s + i, l - i);
                        RSTR(r);
                    }
                    else
                    {
                        // Out of memory.
                        PANIC(contxt);
                        REST;
                    }
                }

                // Return full string.
                return CARG(1);
            }
        }

        // Fall through. Return
        // empty string.
        REST;
    }

    // The parser isn't necessarily broken
    // if we end up here. We could also be
    // out of memory.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (tackon <path> <file>)
//     return properly concatenated file to path
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_tackon(entry_p contxt)
{
    // We need atleast two arguments, a path
    // and a file.
    if(c_sane(contxt, 2))
    {
        char *r = h_tackon(contxt, str(CARG(1)), str(CARG(2)));

        if(r)
        {
            RSTR(r);
        }

        // Return empty string
        // on failure.
        REST;
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
        RCUR;
    }
}

//----------------------------------------------------------------------------
// Name:        h_tackon
// Description: Concatenate directory and file strings
// Input:       entry_p contxt:     The execution context.
//              const char *p:      The directory.
//              const char *f:      The file.
// Return:      const char *:       The dir/file concatenation.
//----------------------------------------------------------------------------
char *h_tackon(entry_p contxt,
               const char *p,
               const char *f)
{
    // We need a path and a file.
    if(p && f)
    {
        size_t lp = strlen(p),
               lf = strlen(f);

        // No point doing this if both
        // strings are empty.
        if(lp || lf)
        {
            char *r = NULL;

            // If the path is empty, the result
            // equals the filename.
            if(!lp)
            {
                r = strdup(f);

                if(!r)
                {
                    // Out of memory.
                    PANIC(contxt);
                }

                return r;
            }

            // If the filename is empty, the result
            // equals the path.
            if(!lf)
            {
                r = strdup(p);

                if(!r)
                {
                    // Out of memory.
                    PANIC(contxt);
                }

                return r;
            }

            // If the filename ends with a delimiter,
            // it's not a valid filename.
            if(f[lf - 1] == '/' ||
               f[lf - 1] == ':')
            {
                 // Only fail if we're in 'strict' mode.
                if(get_numvar(contxt, "@strict"))
                {
                    ERR(ERR_NOT_A_FILE, f);
                    return NULL;
                }
            }

            // Allocate memory to hold path, filename,
            // delimiter and termination.
            size_t lt = lp + lf + 2;
            r = calloc(lt, 1);

            if(r)
            {
                // Copy the path.
                memcpy(r, p, lp);

                // Insert delimiter if none exist.
                if(p[lp - 1] != '/' &&
                   p[lp - 1] != ':')
                {
                   strncat(r, "/", lt - strlen(r));
                }

                // Concatenate the result.
                strncat(r, f, lt - strlen(r));
                return r;
            }
            else
            {
                // Out of memory.
                PANIC(contxt);
            }
        }
    }

    // Failure.
    return NULL;
}

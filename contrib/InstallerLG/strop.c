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
        size_t num = 64;
        char *buf = DBG_ALLOC(calloc(num + 1, 1));

        if(buf)
        {
            size_t len = 0, cur = 0;

            // Iterate over all arguments.
            while(contxt->children[cur] &&
                  contxt->children[cur] != end())
            {
                // Resolve and get a string representation
                // of the current argument.
                const char *arg = str(contxt->children[cur++]);
                size_t alen;

                // If we couldn't resolve the current argument,
                // return an empty string.
                if(DID_ERR)
                {
                    free(buf);
                    REST;
                }

                // Get length of the current argument.
                alen = strlen(arg);

                // If the length is > 0, append to the result.
                if(alen)
                {
                    len += strlen(arg);

                    // If we're about to exceed the current buffer,
                    // allocate a new one big enough.
                    if(len > num)
                    {
                        char *tmp;

                        // Double up until we have enough.
                        while(num && num < len)
                        {
                            num = num << 1;
                        }

                        tmp = DBG_ALLOC(calloc(num + 1, 1));

                        // Copy the contents to the new buffer
                        // and free the old one.
                        if(tmp && num)
                        {
                            memcpy(tmp, buf, len - alen + 1);
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
                    strncat(buf, arg, num - strlen(buf));
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
    char **sct = fmt ? DBG_ALLOC(calloc((strlen(fmt) >> 1) + 1, sizeof(char *))) : NULL;

    if(sct)
    {
        size_t ndx = 0, off = 0, num = 0, len = 0;
        entry_p *arg = contxt->children;

        // Scan the format string.
        for(; fmt[ndx]; ndx++)
        {
            // A specifier not preceeded by an escape?
            if(fmt[ndx] == '%' && (!ndx || fmt[ndx - 1] != '\\'))
            {
                // If this is a specifier that we recognize,
                // then allocate a new string with just this
                // specifier, nothing else.
                if(fmt[++ndx] == 's' || (
                   fmt[ndx++] == 'l' &&
                   fmt[ndx] == 'd'))
                {
                    sct[num] = DBG_ALLOC(calloc(ndx - off + 2, 1));

                    if(sct[num])
                    {
                        memcpy(sct[num], fmt + off, ndx - off + 1);
                        off = ndx + 1;
                        num++;
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

        // Iterate over all format specifiers and arguments and do
        // the appropriate conversions and formating.
        if(num)
        {
            for(num = 0; sct[num]; num++)
            {
                if(arg && *arg &&
                   *arg != end())
                {
                    size_t oln = strlen(sct[num]);
                    entry_p cur = resolve(*arg);

                    // Bail out if we didn't manage to
                    // resolve the current argument.
                    if(DID_ERR)
                    {
                        arg = NULL;
                        break;
                    }

                    // Format string.
                    if(sct[num][oln - 1] == 's' && cur->type == STRING)
                    {
                        size_t nln = oln + strlen(cur->name);
                        char *new = DBG_ALLOC(calloc(nln + 1, 1));

                        // Replace the current format string with
                        // the corresponding formated string.
                        if(new)
                        {
                            int sln = snprintf(new, nln, sct[num], cur->name);

                            len += sln > 0 ? (size_t) sln : 0;
                            free(sct[num]);
                            sct[num] = new;
                        }
                        else
                        {
                            // Out of memory
                            PANIC(contxt);
                            len = 0;
                        }
                    }
                    // Format numeric value.
                    else if(sct[num][oln - 1] == 'd' &&
                            cur->type == NUMBER)
                    {
                        size_t nln = oln + NUMLEN;
                        char *new = DBG_ALLOC(calloc(nln + 1, 1));

                        // Replace the current format string with
                        // the corresponding formated string.
                        if(new)
                        {
                            int sln = snprintf(new, nln, sct[num], cur->id);

                            len += sln > 0 ? (size_t) sln : 0;
                            free(sct[num]);
                            sct[num] = new;
                        }
                        else
                        {
                            // Out of memory
                            PANIC(contxt);
                            len = 0;
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
        if(num && len)
        {
            // Allocate memory to hold all of them.
            len += strlen(fmt + off) + 1;
            ret = DBG_ALLOC(calloc(len, 1));

            if(ret)
            {
                // All format strings.
                for(num = 0; sct[num]; num++)
                {
                    strncat(ret, sct[num], len - strlen(ret));
                }

                // Suffix.
                strncat(ret, fmt + off, len - strlen(ret));
            }
            else
            {
                // Out of memory
                PANIC(contxt);
            }
        }

        // Free all temporary format strings.
        for(num = 0; sct[num]; num++)
        {
            free(sct[num]);
        }

        // Free scatter list.
        free(sct);

        // Without format specifiers, the format string
        // is the return value.
        if(!num)
        {
            ret = DBG_ALLOC(strdup(fmt));
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
        const char *arg = str(CARG(1));
        size_t len = strlen(arg);

        // Scan backwards.
        while(len--)
        {
            // If we find a delimiter, then
            // we have the path to the left
            // of it.
            if(arg[len] == '/' ||
               arg[len] == ':' )
            {
                // Get termination for free.
                char *ret = DBG_ALLOC(calloc(len + 2, 1));

                if(ret)
                {
                    // Copy full path.
                    memcpy(ret, arg, len + 1);

                    // Cut trailing '/' if preceeded
                    // by something absolute, dir or
                    // volume.
                    if(len > 1 &&
                       ret[len] == '/' &&
                       ret[len - 1] != '/' &&
                       ret[len - 1] != ':')
                    {
                        ret[len] = '\0';
                    }

                    RSTR(ret);
                }

                // Out of memory.
                PANIC(contxt);
            }
        }

        // Return empty string
        // on failure.
        REST;
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
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
        }
        #endif
        // Problem or testing.
        RNUM(0);
    }

    // The parser is broken.
    PANIC(contxt);
    RCUR;
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

    // The parser is broken.
    PANIC(contxt);
    RCUR;
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
        char *arg = str(CARG(1));
        int off = num(CARG(2)),
            len = (int) strlen(arg);

        // Is the number characters limited?
        if(CARG(3) && CARG(3) != end())
        {
            // Get the number of characters
            // to copy.
            int chr = num(CARG(3));

            // Use the limitations used by the
            // CBM installer.
            if(off < len && chr > 0 && off >= 0)
            {
                char *ret = DBG_ALLOC(calloc((size_t) len + 1, 1));

                if(ret)
                {
                    // Cap all values and do the
                    // actual copy.
                    len -= off;
                    len = len < chr ? len : chr;
                    memcpy(ret, arg + off, len);
                    RSTR(ret);
                }

                // Out of memory.
                PANIC(contxt);
                REST;
            }
        }
        else
        // No, copy until the end of the string.
        {
            // Max cap.
            if(off < len)
            {
                // Min cap.
                if(off > 0)
                {
                    char *ret = DBG_ALLOC(calloc((size_t) len + 1, 1));

                    if(ret)
                    {
                        // All values are already
                        // capped, just copy.
                        memcpy(ret, arg + off, len - off);
                        RSTR(ret);
                    }

                    // Out of memory.
                    PANIC(contxt);
                    REST;
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
        char *ret = h_tackon(contxt, str(CARG(1)), str(CARG(2)));

        if(ret)
        {
            RSTR(ret);
        }

        // Return empty string
        // on failure.
        REST;
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// Name:        h_tackon
// Description: Concatenate directory and file strings
// Input:       entry_p contxt:     The execution context.
//              const char *pre:    The directory.
//              const char *suf:    The file.
// Return:      const char *:       The dir/file concatenation.
//----------------------------------------------------------------------------
char *h_tackon(entry_p contxt,
               const char *pre,
               const char *suf)
{
    // We need a path and a file.
    if(pre && suf)
    {
        size_t lep = strlen(pre),
               les = strlen(suf);

        // No point doing this if both
        // strings are empty.
        if(lep || les)
        {
            char *ret = NULL;

            // If the filename ends with a delimiter,
            // it's not a valid filename. Only fail if
            // we're running in 'strict' mode.
            if(les && (suf[les - 1] == '/' || suf[les - 1] == ':')
               && get_numvar(contxt, "@strict"))
            {
                ERR(ERR_NOT_A_FILE, suf);
                return NULL;
            }

            // Ignore the path part if the file part is
            // an absolute Amiga style path.
            if(strchr(suf, ':'))
            {
                // Empty path.
                lep = 0;
            }

            // If the path is empty, the result equals
            // the filename.
            if(!lep)
            {
                ret = DBG_ALLOC(strdup(suf));

                if(!ret)
                {
                    // Out of memory.
                    PANIC(contxt);
                }

                return ret;
            }

            // If the filename is empty, the result
            // equals the path.
            if(!les)
            {
                ret = DBG_ALLOC(strdup(pre));

                if(!ret)
                {
                    // Out of memory.
                    PANIC(contxt);
                }

                return ret;
            }

            // Allocate memory to hold path, filename,
            // delimiter and termination.
            size_t let = lep + les + 2;
            ret = DBG_ALLOC(calloc(let, 1));

            if(ret)
            {
                // Copy the path.
                memcpy(ret, pre, lep);

                // Insert delimiter if none exist.
                if(pre[lep - 1] != '/' &&
                   pre[lep - 1] != ':')
                {
                   strncat(ret, "/", let - strlen(ret));
                }

                // Concatenate the result.
                strncat(ret, suf, let - strlen(ret));
                return ret;
            }

            // Out of memory.
            PANIC(contxt);
        }
    }

    // Failure.
    return NULL;
}

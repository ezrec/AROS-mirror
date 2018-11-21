//----------------------------------------------------------------------------
// types.h:
//
// Datatypes and constants used everywhere.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

//----------------------------------------------------------------------------
// Misc constants.
//----------------------------------------------------------------------------
#define VECLEN 4        // Default array size
#define NUMLEN 16       // Max string length of numerical values
#define MAXDEP 64       // Max recursion depth

//----------------------------------------------------------------------------
// Data types.
//----------------------------------------------------------------------------
typedef enum
{
    NUMBER,
    STRING,
    SYMBOL,
    SYMREF,
    NATIVE,
    OPTION,
    CUSTOM,
    CUSREF,
    CONTXT,
    DANGLE
} type_t;

//----------------------------------------------------------------------------
// Option types, refer to new_option() and parser/opt.
//----------------------------------------------------------------------------
typedef enum
{
    OPT_ALL,
    OPT_APPEND,
    OPT_ASSIGNS,
    OPT_BACK,
    OPT_CHOICES,
    OPT_COMMAND,
    OPT_COMPRESSION,
    OPT_CONFIRM,
    OPT_DEFAULT,
    OPT_DELOPTS,
    OPT_DEST,
    OPT_DISK,
    OPT_FILES,
    OPT_FONTS,
    OPT_GETDEFAULTTOOL,
    OPT_GETPOSITION,
    OPT_GETSTACK,
    OPT_GETTOOLTYPE,
    OPT_HELP,
    OPT_INFOS,
    OPT_INCLUDE,
    OPT_NEWNAME,
    OPT_NEWPATH,
    OPT_NOGAUGE,
    OPT_NOPOSITION,
    OPT_NOREQ,
    OPT_PATTERN,
    OPT_PROMPT,
    OPT_QUIET,
    OPT_RANGE,
    OPT_SAFE,
    OPT_SETDEFAULTTOOL,
    OPT_SETPOSITION,
    OPT_SETSTACK,
    OPT_SETTOOLTYPE,
    OPT_SOURCE,
    OPT_SWAPCOLORS,
    OPT_OPTIONAL,
    OPT_RESIDENT,
    OPT_OVERRIDE,
    OPT_ASKUSER,
    OPT_FAIL,
    OPT_FORCE,
    OPT_NOFAIL,
    OPT_OKNODELETE,
    OPT_DYNOPT
} opt_t;

//----------------------------------------------------------------------------
// Types used by the parser and all native functions.
//----------------------------------------------------------------------------
typedef struct entry_s * entry_p;
typedef entry_p (*call_t) (entry_p);

struct entry_s
{
    type_t type;            // One of type_t above.
    int32_t id;             // Numerical ID. Refer to new_*.
    char *name;             // String repr. Refer to new_*().
    call_t call;            // NATIVE function.
    entry_p resolved;       // Resolved value. Refer to eval().
    entry_p *children;      // Subordinate native functions.
    entry_p *symbols;       // Variables / user defined functions.
    entry_p parent;         // Self descriptive.
};

typedef struct entry_s entry_t;

#ifndef AMIGA
# define LONG int
# define APTR void *
#endif

#endif

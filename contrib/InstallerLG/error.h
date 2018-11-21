//----------------------------------------------------------------------------
// error.h:
//
// Functions for setting, getting and communicating errors.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef ERROR_H_
#define ERROR_H_

#include "types.h"

//----------------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------------
#define HALT() error(NULL, 0, ERR_HALT, __func__)
#define RESET() error(NULL, 0, ERR_RESET, __func__)
#define PANIC(C) error(C, __LINE__, ERR_PANIC, __func__)
#define ERR_C(C,T,I) error((C), (C)->id, T, I)
#define ERR(T,I) ERR_C(contxt,T,I)
#define DID_ERR() (error(NULL, 0, ERR_NONE, NULL) != 0)
#define DID_HALT() (error(NULL, 0, ERR_NONE, NULL) == ERR_HALT)

//----------------------------------------------------------------------------
// Error types
//----------------------------------------------------------------------------
typedef enum
{
    ERR_NONE = 0,
    ERR_HALT,
    ERR_ABORT,
    ERR_RESET,
    ERR_PARSE,
    ERR_PANIC,
    ERR_OVERFLOW,
    ERR_READ,
    ERR_READ_FILE,
    ERR_READ_DIR,
    ERR_WRITE_FILE,
    ERR_WRITE_DIR,
    ERR_RENAME_FILE,
    ERR_DELETE_FILE,
    ERR_NO_VERSION,
    ERR_NOT_A_FILE,
    ERR_NOT_A_DIR,
    ERR_NO_SUCH_FILE_OR_DIR,
    ERR_GET_PERM,
    ERR_SET_PERM,
    ERR_EXEC,
    ERR_ASSIGN,
    ERR_UNDEF_FNC,
    ERR_UNDEF_VAR,
    ERR_MAX_DEPTH,
    ERR_MISSING_OPTION,
    ERR_NOTHING_TO_DO,
    ERR_DIV_BY_ZERO,
    ERR_FMT_INVALID,
    ERR_FMT_MISMATCH,
    ERR_FMT_MISSING,
    ERR_FMT_UNUSED,
    ERR_NO_ITEM,
    ERR_INVALID_APP,
    ERR_INVALID_VOLUME,
    ERR_INVALID_ASSIGN,
    ERR_OPTION_MUTEX
} error_t;

//----------------------------------------------------------------------------
// Don't use this function directly, use the macros above instead.
//----------------------------------------------------------------------------
int error(entry_p contxt, int id, error_t type, const char *info);

#endif

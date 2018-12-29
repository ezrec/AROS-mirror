//----------------------------------------------------------------------------
// error.c:
//
// Functions for setting, getting and communicating errors.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "error.h"
#include "gui.h"
#include "util.h"

#include <stdio.h>

//----------------------------------------------------------------------------
// Name:        error
// Description: Function used to set / get / communicate errors.
// Input:       int id:             A numerical identifier aiding debugging.
//                                  In most cases this will be a line number.
//              err_t:              Type of error to set, or ERR_NONE to get
//                                  status.
//              const char *info:   A message describing the problem in a way
//                                  that makes sense to the user.
// Return:      int:                The current state.
//----------------------------------------------------------------------------
int error(entry_p contxt, int id, err_t type, const char *info)
{
    // Last error.
    static err_t last;

    // Set or get state?
    if(type != ERR_NONE)
    {
        // Clear state?
        if(type == ERR_RESET)
        {
            last = ERR_NONE;
        }
        // Set state and show error message
        // if applicable.
        else
        {
            // Save value for future 'gets'.
            last = type;

            // If we have a 'real' error, show
            // it to the user.
            if(type > ERR_RESET)
            {
                static const char *des[] =
                {
                    NULL,
                    "Halt",
                    "Abort",
                    "Reset",
                    "Parse error",
                    "Internal error",
                    "Buffer overflow",
                    "Read error",
                    "Could not read from file",
                    "Could not read directory",
                    "Could not write to file",
                    "Could not create directory",
                    "Could not rename file",
                    "Could not delete file",
                    "Version string not found",
                    "Not a file",
                    "Not a directory",
                    "No such file or directory",
                    "Could not get file / dir permissions",
                    "Could not set file / dir permissions",
                    "Could not execute command",
                    "Could not create / remove assign",
                    "Undefined function",
                    "Undefined variable",
                    "Max recursion depth exceeded",
                    "Missing option",
                    "Nothing to do",
                    "Division by zero",
                    "Invalid format string",
                    "Format string type mismatch",
                    "Missing format string arguments",
                    "Unused format string arguments",
                    "No such item",
                    "Invalid application name",
                    "Invalid name of volume",
                    "Invalid name of assign",
                    "Options are mutually exclusive"
                };

                // Error window / console output.
                gui_error(id, des[type], info);

                // If this is a PANIC, or if we're in
                // debug mode, do a context dump.
                if(type == ERR_PANIC ||
                   get_numvar(contxt, "@debug"))
                {
                    pretty_print(contxt);
                }
            }
        }
    }

    // Current / last state.
    return last;
}

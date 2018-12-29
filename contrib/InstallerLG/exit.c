//----------------------------------------------------------------------------
// exit.c:
//
// Interuption of program execution
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "eval.h"
#include "exit.h"
#include "gui.h"
#include "procedure.h"
#include "resource.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

#ifdef AMIGA
#include <proto/exec.h>
#endif

//----------------------------------------------------------------------------
// (abort <string1> <string2> ...)
//     abandon installation
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_abort(entry_p contxt)
{
    // We need a sane context. Arguments
    // are optional though.
    if(c_sane(contxt, 0))
    {
        // Concatenate all children.
        char *msg = get_chlstr(contxt);

        // Did we manage to concatenate something?
        if(msg)
        {
            // If we could resolve all our children,
            // show the result of the concatenation
            // unless we have an empty string.
            if(*msg && !DID_ERR())
            {
                gui_abort(msg);
            }

            // Free the temporary buffer.
            free(msg);

            // Set abort state. Will make
            // invoke() halt.
            error(contxt, -3, ERR_ABORT, __func__);
            RNUM(0);
        }
    }

    // Broken parser / OOM
    PANIC(contxt);

    // Failure.
    RCUR;
}

//----------------------------------------------------------------------------
// (exit <string> <string> ... (quiet))
//     end installation after displaying strings (if provided)
//
// This causes normal termination of a script.  If strings are
// provided, they are displayed.  The 'done with installation'
// message is then displayed.  The 'onerror' statements are not
// executed.  If (quiet) is specified, the final report display
// is skipped.
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_exit(entry_p contxt)
{
    // All we need is a context.
    if(contxt)
    {
        // If we have any children, display them.
        if(contxt->children)
        {
            // Concatenate all children.
            char *msg = get_chlstr(contxt);

            // Did we manage to concatenate something?
            if(msg)
            {
                // If we could resolve all our children,
                // show the result of the concatenation
                // unless we have an empty string.
                if(*msg && !DID_ERR())
                {
                    gui_message(msg, 0);
                }

                // Free the temporary buffer.
                free(msg);
            }
            else
            {
                // Out of memory
                PANIC(contxt);

                // Failure.
                RCUR;
            }
        }

        // Show final message unless 'quiet' is set.
        if(!DID_ERR() &&
           !get_opt(contxt, OPT_QUIET))
        {
            // Get name and location of application.
            const char *app = get_strvar(contxt, "@app-name"),
                       *dst = get_strvar(contxt, "@default-dest");

            // Only display the 'the app can be found here' message
            // if we know the name and location of the application.
            if(*app && *dst)
            {
                snprintf(get_buf(), buf_size(),
                         tr(S_CBFI), tr(S_ICPL),
                         app, dst);

                // Display the full message.
                gui_message(get_buf(), 0);
            }
            else
            {
                // Display the bare minimum.
                gui_message(tr(S_ICPL), 0);
            }
        }

        // Make invoke() halt.
        HALT();
        RNUM(0);
    }
    else
    {
        // The parser is broken
        PANIC(contxt);

        // Failure.
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (onerror (<statements>))
//     general error trap
//
// ******************************************************
// In part implemented using m_procedure. This function
// just invokes the @onerror custom procedure inserted
// using (onerror) which is a special case of (procedure)
// ******************************************************
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_onerror(entry_p contxt)
{
    // A static reference. We might be out of
    // heap when this is invoked.
    static entry_t ref = { .type = CUSREF,
                           .name = "@onerror" };

    // We need nothing but a context.
    if(c_sane(contxt, 0))
    {
        // Reset error code otherwise
        // m_gosub / invoke will halt
        // immediately.
        RESET();

        // Connect reference to the current context.
        ref.parent = contxt;

        // Invoke @onerror by calling m_gosub just
        // like any non-native function call.
        return m_gosub(&ref);
    }
    else
    {
        // The parser is broken
        PANIC(contxt);

        // Failure.
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (trap <flags> <statements>)
//     trap errors.  flags: 1-abort, 2-nomem, 3-error, 4-dos, 5-badargs
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//
// Despite what the Installer.guide says, the implementaion of 'trap' in OS
// 3.9 seems like a stub, it doesn't work at all. Let's just leave this one
// empty.
//----------------------------------------------------------------------------
entry_p m_trap(entry_p contxt)
{
    // Flags and statement.
    if(c_sane(contxt, 2))
    {
        // Dummy.
        RNUM(1);
    }
    else
    {
        // The parser is broken
        PANIC(contxt);

        // Failure.
        RCUR;
    }
}

//----------------------------------------------------------------------------
// (reboot)                                                              (V44)
//     reboot the Amiga
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_reboot(entry_p contxt)
{
    // All we need is a context.
    if(contxt)
    {
        // Don't reboot in pretend mode.
        if(get_numvar(contxt, "@pretend"))
        {
            DNUM = 0;
        }
        else
        {
            #ifdef AMIGA
            // Hard reset.
            ColdReboot();
            #else
            DNUM = 1;
            #endif
        }
    }
    else
    {
        // Broken parser.
        PANIC(contxt);
    }

    // No reboot.
    RCUR;
}

//----------------------------------------------------------------------------
// wb.c:
//
// Workbench interaction
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "eval.h"
#include "file.h"
#include "util.h"
#include "wb.h"

#ifdef AMIGA
#include <proto/wb.h>
#endif

//----------------------------------------------------------------------------
// (openwbobject <argument> ...)                                         (V44)
//      open a workbench object which can be a disk, a drawer, a trashcan, a
//      tool or a project.
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_openwbobject(entry_p contxt)
{
    // We need a single argument.
    if(c_sane(contxt, 1))
    {
        entry_p prompt     = get_opt(CARG(2), OPT_PROMPT),
                help       = get_opt(CARG(2), OPT_HELP),
                confirm    = get_opt(CARG(2), OPT_CONFIRM),
                safe       = get_opt(CARG(2), OPT_SAFE);

        // A non safe operation in pretend mode
        // always succeeds.
        if(get_numvar(contxt, "@pretend") && !safe)
        {
            RNUM(1);
        }

        // Do we need confirmation?
        if(confirm)
        {
            // The default threshold is expert.
            int level = get_numvar(contxt, "@user-level"),
                th = 2;

            // If the (confirm ...) option contains
            // something that can be translated into
            // a new threshold value...
            if(confirm->children &&
               confirm->children[0] &&
               confirm->children[0] != end())
            {
                // ...then do so.
                th = num(confirm);
            }

            // If we are below the threshold value, or
            // user input has been short-circuited by
            // @yes, skip confirmation.
            if(level < th ||
               get_numvar(contxt, "@yes"))
            {
                confirm = NULL;
            }

            // Make sure that we have the prompt and
            // help texts that we need if 'confirm'
            // is set.
            if(!prompt || !help)
            {
                char * m = prompt ? "help" : "prompt";
                ERR(ERR_MISSING_OPTION, m);
                RNUM(0);
            }
        }

        if(!confirm || h_confirm(CARG(2), str(help), str(prompt)))
        {
            #ifdef AMIGA
            DNUM = OpenWorkbenchObjectA(str(CARG(1)), NULL) ? 1 : 0;
            #else
            // If not supported by workbench.library -1 is to be returned.
            DNUM = -1;
            #endif
        }
        else
        {
            DNUM = 0;
        }
    }
    else
    {
        // Broken parser.
        PANIC(contxt);
    }

    // Success, failure or panic.
    RCUR;
}

//----------------------------------------------------------------------------
// (showwbobject <argument>)                                             (V44)
//      scroll the view of an workbench drawer until the named icon is
//      visible. The drawer has to be opened before.
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//
// KNOWN BUG: This doesn't seem to work on AROS and MorphOS.
//----------------------------------------------------------------------------
entry_p m_showwbobject(entry_p contxt)
{
    // We need a single argument.
    if(c_sane(contxt, 1))
    {
        #ifdef AMIGA
        DNUM = MakeWorkbenchObjectVisibleA(str(CARG(1)), NULL) ? 1 : 0;
        #else
        // If not supported by workbench.library -1 is to be returned.
        DNUM = -1;
        #endif
    }
    else
    {
        // Broken parser.
        PANIC(contxt);
    }

    // Success, failure or panic.
    RCUR;
}

//----------------------------------------------------------------------------
// (closewbobject <argument>)                                            (V44)
//      close named workbench object. Only disks, trashcans and drawers can
//      be closed.
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//
// KNOWN BUG: This doesn't seem to work on AROS and MorphOS.
//----------------------------------------------------------------------------
entry_p m_closewbobject(entry_p contxt)
{
    // We need a single argument.
    if(c_sane(contxt, 1))
    {
        #ifdef AMIGA
        DNUM = CloseWorkbenchObjectA(str(CARG(1)), NULL) ? 1 : 0;
        #else
        // If not supported by workbench.library -1 is to be returned.
        DNUM = -1;
        #endif
    }
    else
    {
        // Broken parser.
        PANIC(contxt);
    }

    // Success, failure or panic.
    RCUR;
}

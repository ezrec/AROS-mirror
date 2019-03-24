//----------------------------------------------------------------------------
// external.c:
//
// Execution of external scripts / binaries
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "eval.h"
#include "external.h"
#include "file.h"
#include "gui.h"
#include "resource.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>

#ifdef AMIGA
#include <dos/dos.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#endif

#include <string.h>

//----------------------------------------------------------------------------
// h_run - m_run / m_execute / m_rexx helper
//----------------------------------------------------------------------------
static entry_p h_run(entry_p contxt, const char *pre, const char *dir)
{
    // We need atleast one argument
    if(c_sane(contxt, 1))
    {
        entry_p prompt   = get_opt(CARG(2), OPT_PROMPT),
                help     = get_opt(CARG(2), OPT_HELP),
                confirm  = get_opt(CARG(2), OPT_CONFIRM),
                safe     = get_opt(CARG(2), OPT_SAFE),
                back     = get_opt(CARG(2), OPT_BACK);

        DNUM = 0;

        // Find out if we need confirmation.
        if(confirm)
        {
            // The default threshold is expert.
            int level = get_numvar(contxt, "@user-level");
            int th = 2;

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

            // If we are below the threshold value,
            // don't care about getting confirmation
            // from the user.
            if(level < th)
            {
                confirm = NULL;
            }

            // Make sure that we have the prompt and
            // help texts that we need if 'confirm'
            // is set. It's not strictly necessary
            // if 'confirm' is not set, but it's not
            // valid code so lets fail anyway.
            if(!prompt || !help)
            {
                char *m = prompt ? "help" : "prompt";

                ERR(ERR_MISSING_OPTION, m);
                RCUR;
            }
        }

        // Did we need it? (confirmation)
        if(confirm)
        {
            inp_t rc = gui_confirm(str(prompt), str(help), back);

            // Is the back option available?
            if(back)
            {
                // Fake input?
                if(get_numvar(contxt, "@back"))
                {
                    rc = G_ABORT;
                }

                // On abort execute.
                if(rc == G_ABORT)
                {
                    return resolve(back);
                }
            }

            // FIXME
            if(rc == G_ABORT || rc == G_EXIT)
            {
                HALT();
            }

            // FIXME
            if(rc != G_TRUE)
            {
                RCUR;
            }
        }

        // Is this call safe to run or are we not
        // in pretend mode?
        if(safe || !get_numvar(contxt, "@pretend"))
        {
            // Command / script. Merge all.
            char *cmd = get_chlstr(contxt);

            if(cmd)
            {
                // Working dir.
                char *cwd = NULL;

                // DOS / Arexx script?
                if(pre)
                {
                    size_t cl = strlen(cmd) + strlen(pre) + 2;
                    char *t = DBG_ALLOC(malloc(cl));

                    if(t)
                    {
                        // Prepend prefix to command string.
                        snprintf(t, cl, "%s %s", pre, cmd);
                        free(cmd);
                        cmd = t;
                    }
                    else
                    {
                        // Out of memory
                        PANIC(contxt);
                        free(cmd);

                        // Failure.
                        RCUR;
                    }
                }

                // If we have a valid destination dir,
                // change to that directory. We're not
                // treating errors as such.
                if(dir && *dir && h_exists(dir))
                {
                    // Use the global buffer.
                    char *buf = get_buf();

                    // Try to get current working dir.
                    if(getcwd(buf, buf_size()) == buf)
                    {
                        // Try to change to the new dir
                        // and save the old one so that
                        // we can go back afterwards.
                        if(!chdir(dir))
                        {
                            cwd = buf;
                        }
                    }
                }

                #ifdef AMIGA
                // No input needed.
                BPTR inp = (BPTR) Open("NIL:", MODE_OLDFILE);

                // Can this fail?
                if(inp)
                {
                    // No output needed.
                    BPTR out = (BPTR) Open("NIL:", MODE_OLDFILE);

                    // Can this fail?
                    if(out)
                    {

                        // Execute whatever we have in cmd.
                        DNUM = SystemTags
                        (
                            cmd,
                            SYS_Input, inp,
                            SYS_Output, out,
                            TAG_END
                        );

                        // On error, get secondary status.
                        if(DNUM)
                        {
                            LONG ioe = IoErr();

                            // Expose IoErr to script.
                            set_numvar(contxt, "@ioerr", ioe);
                        }

                        // Not sure if we need to close NIL:
                        // but it doesn't hurt.
                        Close(out);
                    }
                    else
                    {
                        // Unknown error.
                        DNUM = -1;
                    }

                    // Not sure if we need to close NIL:
                    // but it doesn't hurt.
                    Close(inp);
                }
                else
                {
                    // Unknown error.
                    DNUM = -1;
                }
                #else
                // For testing purposes only.
                printf("%s%s", cmd, dir ? dir : "");
                #endif

                // Have we changed the working dir?
                if(cwd)
                {
                    // Go back to where we started.
                    chdir(cwd);
                }

                // OK == 0. Only fail in 'strict' mode.
                if(DNUM && get_numvar(contxt, "@strict"))
                {
                    ERR(ERR_EXEC, cmd);
                }

                // Free concatenation.
                free(cmd);
            }
            else
            {
                // Out of memory.
                PANIC(contxt);
                RCUR;
            }
        }

        // Write an explanation of what we just did /
        // tried to do to the log file.
        h_log(contxt, tr(S_XCTD), str(CARG(1)));
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Return whatever we have,
    // a success or a failure.
    RCUR;
}

//----------------------------------------------------------------------------
// (execute <arg> (help..) (prompt..) (confirm) (safe))
//     execute script file
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_execute(entry_p contxt)
{
    return h_run
    (
        contxt, "execute",
        get_strvar(contxt, "@execute-dir")
    );
}

//----------------------------------------------------------------------------
// (rexx <arg> (help..) (prompt..) (confirm..) (safe))
//     execute ARexx script
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_rexx(entry_p contxt)
{
    return h_run(contxt, "rx", NULL);
}

//----------------------------------------------------------------------------
// (run <arg> (help..) (prompt..) (confirm..) (safe))
//     execute program
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_run(entry_p contxt)
{
    return h_run
    (
        contxt, NULL,
        get_strvar(contxt, "@execute-dir")
    );
}

//----------------------------------------------------------------------------
// init.c:
//
// Initilization by means of prepending and appending code.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "args.h"
#include "control.h"
#include "exit.h"
#include "information.h"
#include "init.h"
#include "procedure.h"
#include "symbol.h"
#include "util.h"
#include "version.h"

#include <string.h>

#ifdef AMIGA
#include <proto/locale.h>
#endif

//----------------------------------------------------------------------------
// Name:        native_exists
// Description: Init helper; find first occurence of callback in AST.
// Input:       entry_p contxt:  CONTXT.
// Return:      entry_p:         NATIVE callback if found, NULL otherwise.
//----------------------------------------------------------------------------
static entry_p native_exists(entry_p contxt, call_t f)
{
    entry_p e = NULL;

    // NULL are valid values.
    if(contxt &&
       contxt->children)
    {
        // Iterate over all children and
        // recur if needed.
        for(entry_p *c = contxt->children;
            *c && *c != end() && !e; c++)
        {
            if((*c)->type == NATIVE &&
               (*c)->call == f)
            {
                // We found it.
                e = *c;
            }
            else
            {
                // Recur.
                e = native_exists(*c, f);
            }
        }
    }

    // NULL or m_welcome.
    return e;
}

//----------------------------------------------------------------------------
// Name:        init
// Description: Prepend / append startup and shutdown code.
// Input:       entry_p contxt:  The start symbol, refer to the parser.
// Return:      entry_p:         Start + startup / shutdown additions.
//----------------------------------------------------------------------------
entry_p init(entry_p contxt)
{
    // If we don't have a context with
    // children the parser is broken.
    if(contxt &&
       contxt->children)
    {
        // Is there a 'welcome' already?
        entry_p e = native_exists(contxt, m_welcome);

        // Get tooltype values / cli arguments.
        char *a_app = arg_get(ARG_APPNAME),
             *a_scr = arg_get(ARG_SCRIPT),
             *a_min = arg_get(ARG_MINUSER),
             *a_def = arg_get(ARG_DEFUSER),
             *a_log = arg_get(ARG_LOGFILE),
             *a_loc = arg_get(ARG_LANGUAGE);

        // Set default values.
        int defusr = 1, minusr = 0,
            nolog = arg_get(ARG_NOLOG) ? 1 : 0,
            nopretend = arg_get(ARG_NOPRETEND) ? 1 : 0;

        #ifdef AMIGA
        if(!a_loc)
        {
            // Open the current default locale.
            struct Locale *loc = OpenLocale(NULL);

            // Set the preferred installer language.
            if(loc && loc->loc_PrefLanguages[0])
            {
                a_loc = strdup(loc->loc_PrefLanguages[0]);
                CloseLocale(loc);
            }
        }
        else
        {
            // A copy is required. See the init
            // of @language below.
            a_loc = strdup(a_loc);
        }
        #endif

        // Use the defaults of the CBM installer.
        a_scr = a_scr ? a_scr : "";
        a_app = a_app ? a_app : "Test App";
        a_log = a_log ? a_log : "install_log_file";

        // Minimum user level setting?
        if(a_min)
        {
            // 'NOVICE' (0) is implicit.
            if(!strcasecmp("AVERAGE", a_min))
            {
                minusr = 1;
            }
            else if(!strcasecmp("EXPERT", a_min))
            {
                minusr = 2;
            }

            // Default user must be >= min user.
            if(minusr > defusr)
            {
                defusr = minusr;
            }
        }

        // Default user level setting?
        if(a_def)
        {
            // 'AVERAGE' (1) is implicit.
            if(!strcasecmp("NOVICE", a_def))
            {
                defusr = 0;
            }
            else if(!strcasecmp("EXPERT", a_def))
            {
                defusr = 2;
            }

            defusr = defusr > minusr ?
                     defusr : minusr;
        }

        #ifdef AMIGA
        // If no (welcome) is found insert a default one on top.
        // Only on Amiga, otherwise tests will break, they don't
        // expect any default (welcome).
        if(!e)
        {
            // The line numbers and naming are for debugging
            // purposes only.
            e = new_native
            (
                strdup("welcome"), __LINE__, m_welcome,
                push
                (
                    new_contxt(),
                    new_string(strdup("Welcome"))
                ),
                NUMBER
            );

            // Add to the root and reparent.
            if(e)
            {
                append(&contxt->children, e);
                e->parent = contxt;
            }

            // Rotate right to make it end up on top.
            ror(contxt->children);
        }
        #endif
        // Create default error handler, it simply returns '0'
        // without doing anything.
        e = new_native
        (
            strdup("onerror"), __LINE__, m_procedure,
            push
            (
                new_contxt(),
                new_custom
                (
                    strdup("@onerror"), __LINE__, NULL,
                    push
                    (
                        new_contxt(),
                        new_native
                        (
                            strdup("select"), __LINE__, m_select,
                            push(push
                            (
                                new_contxt(),
                                new_number(0)
                            ),
                                push
                                (
                                    new_contxt(),
                                    new_number(0)
                                )
                            ),
                            NUMBER
                        )
                    )
                )
            ),
            DANGLE
        );

        // Add to the root and reparent.
        if(e)
        {
            append(&contxt->children, e);
            e->parent = contxt;
        }

        // Rotate right to make it end up on top.
        ror(contxt->children);

        // Set default variables using (set) instead
        // of creating them directly. Hides all the
        // magic involved in symbol handling.
        e = new_native
        (
            // All the numerical values.
            strdup("set"), __LINE__, m_set,
            push(push(push(push(push(push(
            push(push(push(push(push(push(
            push(push(push(push(push(push(
            push(push(push(push(push(push(
            push(push(push(push
            (
                new_contxt(),
                new_symbol(strdup("@user-level"))),
                new_number(defusr)
                /*
                User level, 0 = novice, 1 = average,
                2 = expert.
                */
            ),
                new_symbol(strdup("@user-min"))),
                new_number(minusr)
                /*
                Minimum user level.
                */
            ),
                new_symbol(strdup("@pretend"))),
                new_number(0)
                /*
                Pretend mode, 1 = on, 0 = off.
                */
            ),
                new_symbol(strdup("@no-pretend"))),
                new_number(nopretend)
                /*
                Disallow pretend mode.
                */
            ),
                new_symbol(strdup("@installer-version"))),
                new_number((MAJOR << 16) | MINOR)
                /*
                The version of Installer.
                */
            ),
                new_symbol(strdup("@ioerr"))),
                new_number(0)
                /*
                The value of the last DOS error.
                */
            ),
                new_symbol(strdup("@log"))),
                new_number(0)
                /*
                Logging enabled = 1, disabled = 0.
                */
            ),
                new_symbol(strdup("@no-log"))),
                new_number(nolog)
                /*
                Disallow logging.
                */
            ),
                new_symbol(strdup("@yes"))),
                /*
                Mock user input. 1 = always yes.
                */
                new_number(0)
            ),
                new_symbol(strdup("@skip"))),
                new_number(0)
                /*
                Mock user input. 1 = always skip.
                */
            ),
                new_symbol(strdup("@abort"))),
                new_number(0)
                /*
                Mock user input. 1 = abort.
                */
            ),
                new_symbol(strdup("@each-type"))),
                new_number(0)
                /*
                When using (foreach), @each-name and @each-type
                will contain the filename and the object type.
                */
            ),
                new_symbol(strdup("@strict"))),
                #ifdef AMIGA
                new_number(0)
                #else
                new_number(1)
                #endif
                /*
                Toggle 'strict' mode.
                */
            ),
                new_symbol(strdup("@debug"))),
                new_number(0)
                /*
                Toggle 'debug' mode.
                */
            ),

            NUMBER
        );

        // Add to the root and reparent.
        if(e)
        {
            append(&contxt->children, e);
            e->parent = contxt;
        }

        // Rotate right to make it end up on top.
        // We need these to be set before any user
        // code is executed.
        ror(contxt->children);

        e = new_native
        (
            // All the string values.
            strdup("set"), __LINE__, m_set,
            push(push(push(push(push(push(push(push(
            push(push(push(push(push(push(push(push(
            push(push(push(push(push(push(push(push(
            push(push(push(push(push(push(push(push(
            push(push(push(push(push(push(push(push(
            push(push(push(push(push(push(push(push(
            push(push(push(push(push(push(push(push
            (
                new_contxt(),
                new_symbol(strdup("@abort-button"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@app-name"))),
                new_string(strdup(a_app))
                /*
                The `APPNAME' value given at startup.
                */
            ),
                new_symbol(strdup("@askoptions-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@askchoice-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@asknumber-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@askstring-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@askdisk-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@askfile-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@askdir-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@copylib-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@copyfiles-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@makedir-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@startup-help"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@default-dest"))),
                new_string(strdup("T:"))
                /*
                Suggested location for installing
                an application.
                */
            ),
                new_symbol(strdup("@error-msg"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@execute-dir"))),
                new_string(strdup(""))
                /*
                The Installer will change to this directory
                whenever (run) or (execute) are performed.
                */
            ),
                new_symbol(strdup("@icon"))),
                new_string(strdup(a_scr))
                /*
                Installer icon path.
                */
            ),
                new_symbol(strdup("@language"))),
                new_string(a_loc ? a_loc : strdup("english"))
                /*
                Default language.
                */
            ),
                new_symbol(strdup("@special-msg"))),
                new_string(strdup(""))
                /*
                NOT USED.
                */
            ),
                new_symbol(strdup("@log-file"))),
                new_string(strdup(a_log))
                /*
                The default log file.
                */
            ),
                new_symbol(strdup("@each-name"))),
                new_string(strdup(""))
                /*
                When using (foreach), @each-name and
                @each-type will contain the filename
                and the object type.
                */
            ),
                new_symbol(strdup("@user-startup"))),
                new_string(strdup("s:user-startup"))
                /*
                The default file used by (startup).
                */
            ),
                new_symbol(strdup("fail"))),
                new_string(strdup("fail"))
                /*
                Hack to deal with (optional) quirks.
                */
            ),
                new_symbol(strdup("nofail"))),
                new_string(strdup("nofail"))
                /*
                Hack to deal with (optional) quirks.
                */
            ),
                new_symbol(strdup("oknodelete"))),
                new_string(strdup("oknodelete"))
                /*
                Hack to deal with (optional) quirks.
                */
            ),
                new_symbol(strdup("force"))),
                new_string(strdup("force"))
                /*
                Hack to deal with (optional) quirks.
                */
            ),
                new_symbol(strdup("askuser"))),
                new_string(strdup("askuser"))
                /*
                Hack to deal with (optional) quirks.
                */
            ),
                new_symbol(strdup("@null"))),
                new_string(strdup("NULL"))
                /*
                Hack to deal with broken scripts.
                */
            ),
            STRING
        );

        // Add to the root and reparent.
        if(e)
        {
            append(&contxt->children, e);
            e->parent = contxt;
        }

        // Rotate right to make it end up on top.
        // We need these to be set before any user
        // code is executed.
        ror(contxt->children);

        // Create default (exit). Line numbers and
        // naming are for debugging purposes only.
        e = new_native
        (
            strdup("exit"), __LINE__,
            m_exit, NULL, NUMBER
        );

        #ifdef AMIGA
        // Insert only on Amiga, otherwise tests will
        // break, they don't expect any default (exit).

        // Add to the root and reparent.
        if(e)
        {
            append(&contxt->children, e);
            e->parent = contxt;
        }

        // No rotation. Default (exit) should be last.
        #else
        // We're not using this, kill it directly.
        kill(e);
        #endif
    }

    return contxt;
}


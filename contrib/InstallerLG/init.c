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
#include "gui.h"
#include "information.h"
#include "init.h"
#include "media.h"
#include "procedure.h"
#include "symbol.h"
#include "util.h"
#include "version.h"

#include <string.h>

#ifdef AMIGA
#include <proto/locale.h>
#ifdef __MORPHOS__
char *strcasestr(const char *, const char *);
#endif
#endif

//----------------------------------------------------------------------------
// Name:        init_num
// Description: Init helper; insert numeric variable in CONTXT. This is only
//              for use in init(), nothing is resolved and duplicates aren't
//              accounted for.
// Input:       entry_p contxt: CONTXT.
//              char *sym:      Name.
//              int num:        Value.
// Return:      -
//----------------------------------------------------------------------------
static void init_num(entry_p contxt, char *sym, int num)
{
    // Create SYMBOL VALUE tuple.
    entry_p var = new_symbol(DBG_ALLOC(strdup(sym))),
            val = new_number(num);

    // Unless we're OOM, init tuple.
    if(var && val)
    {
        var->parent = contxt;
        var->resolved = val;
        val->parent = var;

        // Insert result in CONTXT.
        append(&contxt->symbols, var);
    }
    else
    {
        // Don't leak on OOM.
        kill(var);
        kill(val);
    }
}

//----------------------------------------------------------------------------
// Name:        init_str
// Description: Init helper; insert string variable in CONTXT. This is only
//              for use in init(), nothing is resolved and duplicates aren't
//              accounted for.
// Input:       entry_p contxt: CONTXT.
//              char *sym:      Name.
//              char *str:      Value.
// Return:      -
//----------------------------------------------------------------------------
static void init_str(entry_p contxt, char *sym, char *str)
{
    // Create SYMBOL VALUE tuple.
    entry_p var = new_symbol(DBG_ALLOC(strdup(sym))),
            val = new_string(DBG_ALLOC(strdup(str)));

    // Unless we're OOM, init tuple.
    if(var && val)
    {
        var->parent = contxt;
        var->resolved = val;
        val->parent = var;

        // Insert result in CONTXT.
        append(&contxt->symbols, var);
    }
    else
    {
        // Don't leak on OOM.
        kill(var);
        kill(val);
    }
}

//----------------------------------------------------------------------------
// Name:        init_tooltypes
// Description: Init helper; Promote tooltypes to variables in CONTXT.
// Input:       entry_p contxt: CONTXT.
// Return:      -
//----------------------------------------------------------------------------
static void init_tooltypes(entry_p contxt)
{
    // Get tooltype values / cli arguments.
    char *a_app = arg_get(ARG_APPNAME),
         *a_scr = arg_get(ARG_SCRIPT),
         *a_min = arg_get(ARG_MINUSER),
         *a_def = arg_get(ARG_DEFUSER),
         *a_log = arg_get(ARG_LOGFILE),
         *a_lng = arg_get(ARG_LANGUAGE);

    // Modus.
    init_num(contxt, "@no-log", arg_get(ARG_NOLOG) ? 1 : 0);
    init_num(contxt, "@no-pretend", arg_get(ARG_NOPRETEND) ? 1 : 0);

    // File names.
    init_str(contxt, "@icon", a_scr ? a_scr : "");
    init_str(contxt, "@app-name", a_app ? a_app : "Test App");
    init_str(contxt, "@log-file", a_log ? a_log : "install_log_file");

    // Default and minimum user level.
    int l_def = 1, l_min = 0;

    // Minimum user level setting?
    if(a_min)
    {
        // NOVICE (0) is implicit.
        if(!strcasecmp("AVERAGE", a_min))
        {
            l_min = 1;
        }
        else
        if(!strcasecmp("EXPERT", a_min))
        {
            l_min = 2;
        }
    }

    // Default user level setting?
    if(a_def)
    {
        // AVERAGE (1) is implicit.
        if(!strcasecmp("NOVICE", a_def))
        {
            l_def = 0;
        }
        else
        if(!strcasecmp("EXPERT", a_def))
        {
            l_def = 2;
        }
    }

    // Set capped values, default must be >= minimum.
    init_num(contxt, "@user-level", l_def < l_min ? l_min : l_def);
    init_num(contxt, "@user-min", l_min);

    if(!a_lng)
    {
        #ifdef AMIGA
        // Open the current default locale.
        struct Locale *loc = OpenLocale(NULL);

        // Set the preferred installer language.
        if(loc && loc->loc_PrefLanguages[0])
        {
            init_str(contxt, "@language", loc->loc_PrefLanguages[0]);
            CloseLocale(loc);
        }
        #else
        init_str(contxt, "@language", "english");
        #endif
    }
    else
    {
        init_str(contxt, "@language", arg_get(ARG_LANGUAGE));
    }
}

//----------------------------------------------------------------------------
// Name:        init
// Description: Prepend / append startup and shutdown code.
// Input:       entry_p contxt:  The start symbol, refer to the parser.
// Return:      entry_p:         Start + startup / shutdown additions.
//----------------------------------------------------------------------------
entry_p init(entry_p contxt)
{
    // Sanity check.
    if(c_sane(contxt, 0) &&
       s_sane(contxt, 0))
    {
        // Is there a (welcome) already?
        entry_p entry = native_exists(contxt, m_welcome);

        // If not, insert a default (welcome).
        if(!entry)
        {
            // The line numbers and naming are for debugging
            // purposes only.
            entry = new_native
            (
                DBG_ALLOC(strdup("welcome")), __LINE__, m_welcome,
                push
                (
                    new_contxt(),
                    new_string(DBG_ALLOC(strdup("Welcome")))
                ),
                NUMBER
            );

            #ifdef AMIGA
            // Only on Amiga, otherwise tests will break,
            // they don't expect any default (welcome).

            // Add to the root and reparent.
            if(entry)
            {
                append(&contxt->children, entry);
                entry->parent = contxt;
            }

            // Rotate right to make it end up on top.
            ror(contxt->children);
            #else
            // We're not using this, kill it directly.
            kill(entry);
            #endif
        }

        // Create default error handler, it simply returns '0'
        // without doing anything.
        entry = new_native
        (
            DBG_ALLOC(strdup("onerror")), __LINE__, m_procedure,
            push
            (
                new_contxt(),
                new_custom
                (
                    DBG_ALLOC(strdup("@onerror")), __LINE__, NULL,
                    push
                    (
                        new_contxt(),
                        new_native
                        (
                            DBG_ALLOC(strdup("select")), __LINE__, m_select,
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

        // Unless we're out of memory.
        if(entry)
        {
            // Add to the root and reparent.
            append(&contxt->children, entry);
            entry->parent = contxt;

            // Rotate to put it on top.
            ror(contxt->children);
        }

        // Create default (exit). Line numbers and
        // naming are for debugging purposes only.
        entry = new_native
        (
            DBG_ALLOC(strdup("exit")), __LINE__,
            m_exit, NULL, NUMBER
        );

        #ifdef AMIGA
        // Insert only on Amiga, otherwise tests will
        // break, they don't expect any default (exit).

        // Add to the root and reparent.
        if(entry)
        {
            append(&contxt->children, entry);
            entry->parent = contxt;
        }

        // No rotation. Default (exit) should be last.
        #else
        // We're not using this, kill it directly.
        kill(entry);
        #endif

        // Get tooltype / cli arguments.
        init_tooltypes(contxt);

        // Set misc numerical values.
        init_num(contxt, "@pretend", 0);
        init_num(contxt, "@installer-version", (MAJOR << 16) | MINOR);
        init_num(contxt, "@ioerr", 0);
        init_num(contxt, "@log", 0);
        init_num(contxt, "@yes", 0);
        init_num(contxt, "@skip", 0);
        init_num(contxt, "@abort", 0);
        init_num(contxt, "@back", 0);
        init_num(contxt, "@each-type", 0);
        init_num(contxt, "@debug", 0);
        init_num(contxt, "@strict",
                         #ifdef AMIGA
                         0
                         #else
                         1
                         #endif
                         );

        // Set misc strings values.
        init_str(contxt, "@abort-button", "");
        init_str(contxt, "@askoptions-help", "");
        init_str(contxt, "@askchoice-help", "");
        init_str(contxt, "@asknumber-help", "");
        init_str(contxt, "@askstring-help", "");
        init_str(contxt, "@askdisk-help", "");
        init_str(contxt, "@askfile-help", "");
        init_str(contxt, "@askdir-help", "");
        init_str(contxt, "@copylib-help", "");
        init_str(contxt, "@copyfiles-help", "");
        init_str(contxt, "@makedir-help", "");
        init_str(contxt, "@startup-help", "");
        init_str(contxt, "@default-dest", "T:");
        init_str(contxt, "@error-msg", "");
        init_str(contxt, "@execute-dir", "");
        init_str(contxt, "@special-msg", "");
        init_str(contxt, "@each-name", "");
        init_str(contxt, "@user-startup", "s:user-startup");
        init_str(contxt, "fail", "fail");
        init_str(contxt, "nofail", "nofail");
        init_str(contxt, "oknodelete", "oknodelete");
        init_str(contxt, "force", "force");
        init_str(contxt, "askuser", "askuser");
        init_str(contxt, "@null", "NULL");
    }

    return contxt;
}

//----------------------------------------------------------------------------
// media.c:
//
// Multimedia features
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "eval.h"
#include "file.h"
#include "gui.h"
#include "media.h"
#include "util.h"

#include <string.h>
#if defined(__MORPHOS__) || defined(__VBCC__)
char *strcasestr(const char *, const char *);
#endif

#ifdef AMIGA
#include <datatypes/datatypesclass.h>
#else
// From datatypesclass.h 44.1 (17.4.1999)
#define STM_PAUSE          1
#define STM_PLAY           2
#define STM_CONTENTS       3
#define STM_INDEX          4
#define STM_RETRACE        5
#define STM_BROWSE_PREV    6
#define STM_BROWSE_NEXT    7
// Skip unused defines to avoid warnings.
#define STM_COMMAND        11
#define STM_REWIND         12
#define STM_FASTFORWARD    13
#define STM_STOP           14
// Skip unused defines to avoid warnings.
#define STM_LOCATE         16
#endif

//----------------------------------------------------------------------------
// (closemedia <media>)
//      close media file and remove it from memory.
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_closemedia(entry_p contxt)
{
    // We need 1 argument.
    if(c_sane(contxt, 1))
    {
        int mid = num(CARG(1));

        RNUM
        (
            gui_closemedia(mid) == G_TRUE ? 1 : 0
        );
    }

    // Broken parser.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// Name:        h_pos
// Description: Get (effect) and (showmedia) position as bitmask.
// Input:       const char *atr:    Attribute name.
// Return:      int:                G_* bitmask.
//----------------------------------------------------------------------------
static int h_pos(const char *atr)
{
    return (strcasestr(atr, "left") ? G_LEFT : 0) |
           (strcasestr(atr, "right") ? G_RIGHT : 0) |
           (strcasestr(atr, "upper") ? G_UPPER : 0) |
           (strcasestr(atr, "lower") ? G_LOWER : 0);
}

//----------------------------------------------------------------------------
// (effect <position> <effect> <color 1> <color 2>)
//      open the installer on its own screen
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_effect(entry_p contxt)
{
    // We need 4 arguments.
    if(c_sane(contxt, 4))
    {
        // Position and effect.
        char *est = str(CARG(2)),
             *eps = str(CARG(1));

        // Colors, type and position.
        int ic1 = num(CARG(3)),
            ic2 = num(CARG(4)),
            ief = h_pos(eps) | (
                  !strcasecmp(est, "radial") ? G_RADIAL :
                  !strcasecmp(est, "horizontal") ? G_HORIZONTAL : 0);

        // Known effect type?
        if(ief & G_EFFECT)
        {
            // Invalid initial values.
            static int oc1, oc2, oef = G_RADIAL | G_HORIZONTAL;

            // Only show something if this is the first
            // invocation or if the input has changed.
            if(ief != oef || ic1 != oc1 || ic2 != oc2)
            {
                // Show gradient.
                gui_effect(ief, ic1, ic2);

                // Save current values.
                oef = ief;
                oc1 = ic1;
                oc2 = ic2;
            }

            // Always.
            RNUM(1);
        }

        // Missing effect type.
        ERR(ERR_VAL_INVALID, est);
        RNUM(0);
    }

    // Broken parser.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (setmedia <media> <action> [parameter])
//      perform action on datatype
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_setmedia(entry_p contxt)
{
    // We need atleast 2 arguments.
    if(c_sane(contxt, 2))
    {
        // Action to perform.
        char *act = str(CARG(2));

        // Translate action to command.
        int cmd = !strcasecmp(act, "pause") ? STM_PAUSE :
                  !strcasecmp(act, "play") ? STM_PLAY :
                  !strcasecmp(act, "contents") ? STM_CONTENTS :
                  !strcasecmp(act, "index") ? STM_INDEX :
                  !strcasecmp(act, "retrace") ? STM_RETRACE :
                  !strcasecmp(act, "browser_prev") ? STM_BROWSE_PREV :
                  !strcasecmp(act, "browser_next") ? STM_BROWSE_NEXT :
                  !strcasecmp(act, "command") ? STM_COMMAND :
                  !strcasecmp(act, "rewind") ? STM_REWIND :
                  !strcasecmp(act, "fastforward") ? STM_FASTFORWARD :
                  !strcasecmp(act, "stop") ? STM_STOP :
                  !strcasecmp(act, "locate") ? STM_LOCATE : 0;

        // Valid action?
        if(cmd)
        {
            // Extra flags.
            char *par = NULL;

            // If the command requires an extra parameter,
            // resolved the next argument, if it exists.
            if((cmd == STM_COMMAND || cmd == STM_LOCATE) &&
                CARG(3) && CARG(3) != end())
            {
                // Resolve next.
                par = str(CARG(3));
            }

            // Media identifier.
            int mid = num(CARG(1));

            RNUM
            (
                // Invoke GUI to perform action.
                gui_setmedia(mid, cmd, par) == G_TRUE ? 1 : 0
            );
        }

        // Invalid action.
        ERR(ERR_VAL_INVALID, act);
        RNUM(0);
    }

    // Broken parser.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (showmedia <medianame> <filename> <position> <size> <borderflag> ...)
//      open datatype and present it to the user.
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_showmedia(entry_p contxt)
{
    // We need atleast 5 arguments.
    if(c_sane(contxt, 5))
    {
        // Get size.
        char *att = str(CARG(4));

        // Set size bitmask.
        int msk = h_pos(str(CARG(3))) | (num(CARG(5)) ? G_BORDER : 0) | (
                  !strcasecmp(att, "small") ? G_SMALL :
                  !strcasecmp(att, "small_medium") ? G_SMALL | G_LESS :
                  !strcasecmp(att, "small_large") ? G_SMALL | G_MORE :
                  !strcasecmp(att, "medium") ? G_MEDIUM :
                  !strcasecmp(att, "medium_small") ? G_MEDIUM | G_LESS :
                  !strcasecmp(att, "medium_large") ? G_MEDIUM | G_MORE :
                  !strcasecmp(att, "large") ? G_LARGE :
                  !strcasecmp(att, "large_small") ? G_LARGE | G_LESS :
                  !strcasecmp(att, "large_medium") ? G_LARGE | G_MORE : 0);

        // Get the rest of the flags.
        for(size_t i = 6; CARG(i) && CARG(i) != end(); i++)
        {
            // Get current flag.
            att = str(CARG(i));

            // Translate into bitmask.
            msk |= (!strcasecmp(att, "wordwrap") ? G_WORDWRAP :
                    !strcasecmp(att, "panel") ? G_PANEL :
                    !strcasecmp(att, "play") ? G_PLAY :
                    !strcasecmp(att, "repeat") ? G_REPEAT : 0);
        }

        // Invalid media ID.
        int mid = -1;

        if(gui_showmedia(&mid, str(CARG(2)), msk) != G_TRUE)
        {
            // Could not open file.
            RNUM(0);
        }

        // Symbol destination.
        entry_p dst = global(contxt);

        if(dst)
        {
            char *var = str(CARG(1));
            entry_p *sym = contxt->symbols;

            // Symbol exists already?
            while(*sym && *sym != end())
            {
                // If true, update current symbol.
                if(!strcasecmp((*sym)->name, var) &&
                   (*sym)->resolved)
                {
                    (*sym)->resolved->id = mid;

                    // Success.
                    RNUM(1);
                }

                // Next symbol.
                sym++;
            }

            // Create the new media ID.
            entry_p num = new_number(mid);

            if(num)
            {
                // Create new symbol with user defined name.
                entry_p nsm = new_symbol(DBG_ALLOC(strdup(var)));

                if(nsm)
                {
                    // Reparent value.
                    num->parent = nsm;
                    nsm->resolved = num;

                    // Append the symbol to the current
                    // context and create a global ref.
                    if(append(&contxt->symbols, nsm))
                    {
                        // Reparent symbol.
                        push(dst, nsm);
                        nsm->parent = contxt;

                        // Success.
                        RNUM(1);
                    }

                    // Out of memory.
                    kill(nsm);
                }
                else
                {
                    // Out of memory.
                    kill(num);
                }
            }
        }
    }

    // Broken parser / OOM.
    PANIC(contxt);
    RCUR;
}

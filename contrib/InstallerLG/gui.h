//----------------------------------------------------------------------------
// gui.h:
//
// MUI based GUI
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef GUI_H_
#define GUI_H_

#include "file.h"

#include <stdbool.h>

// Gui return codes.
typedef enum
{
    G_ERR,   // Panic
    G_TRUE,  // Yes / Ok / Next / Proceed
    G_FALSE, // No / Skip
    G_ABORT, // Abort / Back
    G_EXIT   // Quit
} inp_t;

// Gui effect bits.
enum
{
    G_UPPER_BIT,        // 1
    G_LOWER_BIT,        // 2
    G_LEFT_BIT,         // 4
    G_RIGHT_BIT,        // 8
    G_RADIAL_BIT,       // 16
    G_HORIZONTAL_BIT,   // 32
    G_SMALL_BIT,        // 64
    G_MEDIUM_BIT,       // 128
    G_LARGE_BIT,        // 256
    G_LESS_BIT,         // 512
    G_MORE_BIT,         // 1024
    G_BORDER_BIT,       // 2048
    G_WORDWRAP_BIT,     // 4096
    G_PANEL_BIT,        // 8192
    G_PLAY_BIT,         // 16384
    G_REPEAT_BIT        // 32768
};

// Gui effect masks.
#define G_UPPER (1 << G_UPPER_BIT)
#define G_LOWER (1 << G_LOWER_BIT)
#define G_LEFT (1 << G_LEFT_BIT)
#define G_RIGHT (1 << G_RIGHT_BIT)
#define G_RADIAL (1 << G_RADIAL_BIT)
#define G_HORIZONTAL (1 << G_HORIZONTAL_BIT)
#define G_EFFECT (G_RADIAL | G_HORIZONTAL)
#define G_HORIZ (G_LEFT | G_RIGHT)
#define G_VERT (G_UPPER | G_LOWER)
#define G_POSITION (G_LEFT | G_RIGHT | G_UPPER | G_LOWER)
#define G_SMALL (1 << G_SMALL_BIT)
#define G_MEDIUM (1 << G_MEDIUM_BIT)
#define G_LARGE (1 << G_LARGE_BIT)
#define G_LESS (1 << G_LESS_BIT)
#define G_MORE (1 << G_MORE_BIT)
#define G_SIZE (G_SMALL | G_MEDIUM | G_LARGE)
#define G_BORDER (1 << G_BORDER_BIT)
#define G_WORDWRAP (1 << G_WORDWRAP_BIT)
#define G_PANEL (1 << G_PANEL_BIT)
#define G_PLAY (1 << G_PLAY_BIT)
#define G_REPEAT (1 << G_REPEAT_BIT)

void gui_exit(void);
void gui_complete(int com);
void gui_abort(const char *msg);
void gui_error(int line, const char *type, const char *info);
void gui_copyfiles_end(void);
void gui_effect(int eff, int cl1, int cl2);
inp_t gui_closemedia(int mid);
inp_t gui_setmedia(int mid, int act, const char *par);
inp_t gui_showmedia(int *mid, const char* mda, int act);
inp_t gui_init(bool scr);
inp_t gui_message(const char *msg, bool bck);
inp_t gui_finish(const char *msg);
inp_t gui_working(const char *msg);
inp_t gui_bool(const char *msg, const char *hlp, const char *yes, const char *nay, bool bck);
inp_t gui_string(const char *msg, const char *hlp, const char *def, bool bck, const char **ret);
inp_t gui_choice(const char *msg, const char *hlp, const char **nms, int def, bool bck, int *ret);
inp_t gui_options(const char *msg, const char *hlp, const char **nms, int def, bool bck, int *ret);
inp_t gui_number(const char *msg, const char *hlp, int min, int max, int def, bool bck, int *ret);
inp_t gui_welcome(const char *msg, int *lvl, int *lgf, int *prt, int min, bool npr, bool nlg);
inp_t gui_askdir(const char *msg, const char *hlp, bool pth, bool dsk, bool asn, const char *def, bool bck, const char **ret);
inp_t gui_askfile(const char *msg, const char *hlp, bool pth, bool dsk, const char *def, bool bck, const char **ret);
inp_t gui_confirm(const char *msg, const char *hlp, bool bck);
inp_t gui_copyfiles_setcur(const char *cur, bool nga, bool bck);
inp_t gui_copyfiles_start(const char *msg, const char *hlp, pnode_p lst, bool cnf, bool bck);

#endif

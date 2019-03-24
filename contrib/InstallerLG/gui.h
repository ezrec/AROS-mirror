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

typedef enum
{
    G_ERR,   // Panic
    G_TRUE,  // Yes / Ok / Next / Proceed
    G_FALSE, // No / Skip
    G_ABORT, // Abort / Back
    G_EXIT   // Quit
} inp_t;

void gui_exit(void);
void gui_complete(int com);
void gui_abort(const char *msg);
void gui_error(int id, const char *type, const char *info);
void gui_copyfiles_end(void);

/*
    message
    asknumber
    askdir
    askfile
    askstring
    askchoice
    askoptions
    rename
    makedir
    askbool
    askdisk
    textfile
    delete
    execute
    copyfiles
    copylib
 */

inp_t gui_init(void);
inp_t gui_message(const char *msg, bool bck);
inp_t gui_finish(const char *msg);
inp_t gui_working(const char *msg);
inp_t gui_bool(const char *msg, const char *hlp, const char *yes, const char *no, bool bck);
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

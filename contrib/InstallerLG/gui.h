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

int gui_init(void);
void gui_exit(void);
int gui_welcome(const char *msg, int *lvl, int *lgf, int *prt, int min, int npr, int nlg);
int gui_message(const char *msg, int imm);
void gui_abort(const char *msg);
int gui_bool(const char *msg, const char *hlp, const char *yes, const char *no);
int gui_choice(const char *msg, const char *hlp, const char **nms, int def, int *hlt);
const char * gui_string(const char *msg, const char *hlp, const char *def, int *hlt);
int gui_number(const char *msg, const char *hlp, int min, int max, int def, int *hlt);
int gui_options(const char *msg, const char *hlp, const char **nms, int def, int *hlt);
int gui_error(int id, const char *type, const char *info);
const char *gui_askdir(const char *msg, const char *hlp, int pth, int dsk, int asn, const char *def);
const char *gui_askfile(const char *msg, const char *hlp, int pth, int dsk, const char *def);
int gui_copyfiles_start(const char *msg, const char *hlp, pnode_p lst, int cnf);
int gui_copyfiles_setcur(const char *cur, int nogauge);
void gui_copyfiles_end(void);
int gui_complete(int com);
int gui_confirm(const char *msg, const char *hlp);
void locale_init(void);
void locale_exit(void);

#endif

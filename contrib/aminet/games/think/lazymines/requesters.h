/*
 * requesters.h
 * ============
 * Some usefull functions for handling requesters.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#ifndef REQUESTERS_H
#define REQUESTERS_H

#include <exec/types.h>
#include <intuition/intuition.h>


/*
 * window_sleep
 * ------------
 * Puts a window to sleep.
 *
 * Arguments:
 *  win - Pointer to window to put to sleep.
 *  req - Pointer to requester to use.
 * Returnvalue:
 *  TRUE if the window was put to sleep, otherwise FALSE.
 */
BOOL
window_sleep (
   struct Window     *win,
   struct Requester  *req);

/*
 * window_wakeup
 * -------------
 * Wakes up a sleeping window.
 *
 * Arguments:
 *  win - Pointer to sleeping window.
 *  req - Pointer to requester that was used to put the window to sleep.
 * Returnvalue:
 *  none
 */
void
window_wakeup (
   struct Window     *win,
   struct Requester  *req);

void
error_requester (
   struct Window  *win,
   ULONG           msg,
   char           *more);

void
string_requester (
   struct Window  *win,
   APTR            vis_info,
   char           *win_title,
   char           *gad_title,
   char           *buffer,
   UBYTE           buf_size);

void
about_requester (
   struct Window  *win,
   struct Image   *img,
   STRPTR          name,
   STRPTR          version,
   STRPTR          msg_str);

void
request_optional_size (
   struct Window  *win);

void
request_autoopening (
   struct Window  *win);

#endif /* REQUESTERS_H */

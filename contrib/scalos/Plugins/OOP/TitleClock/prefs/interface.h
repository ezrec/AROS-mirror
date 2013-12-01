/*
 *  interface.h - Functions for creating the UI and handling IDCMP
 *
 * $Date$
 * $Revision$
 *
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include <intuition/intuition.h>
#include <stdlib.h>

extern struct Window    *tcp_Window;

void CloseMainWindow(void);
BOOL OpenMainWindow(void);
WORD HandleMainMessages(void);
void ShowExample(STRPTR format, UWORD gid, STRPTR buffer, size_t BuffLen);

#endif /* INTERFACE_H */


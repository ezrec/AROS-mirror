#ifndef INTUI_H
#define INTUI_H
/* intui.h: */

#include "acdrbase.h"

void Init_Intui(struct ACDRBase *);
void Close_Intui(struct ACDRBase *);
void Display_Error(struct ACDRBase *, char *, ...);
void Show_CDDA_Icon(struct ACDRBase *);
void Hide_CDDA_Icon(struct ACDRBase *);

extern struct MsgPort *g_app_port;
extern ULONG g_app_sigbit;
extern int g_retry_show_cdda_icon;
#endif


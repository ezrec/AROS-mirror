#ifndef INTUI_H
#define INTUI_H
/* intui.h: */

void Init_Intui(void);
void Close_Intui(void);
void Display_Error(char *, ...);
void Show_CDDA_Icon(void);
void Hide_CDDA_Icon(void);

extern struct MsgPort *g_app_port;
extern ULONG g_app_sigbit;
extern int g_retry_show_cdda_icon;
#endif


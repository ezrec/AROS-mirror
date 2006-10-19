#ifndef INTUI_H
#define INTUI_H
/* intui.h: */

void Init_Intui(void);
void Close_Intui(void);
#ifdef __MORPHOS__
void Display_Error_Tags(char *, APTR);
#define Display_Error(_p_msg, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	Display_Error_Tags(_p_msg, (CONST APTR )_tags);})
#else
void Display_Error(char *, ...);
#endif
void Show_CDDA_Icon(void);
void Hide_CDDA_Icon(void);

extern struct MsgPort *g_app_port;
extern ULONG g_app_sigbit;
extern int g_retry_show_cdda_icon;
#endif


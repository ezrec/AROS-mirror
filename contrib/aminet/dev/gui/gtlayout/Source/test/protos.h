#include <proto/gtlayout.h>

BOOL OpenLibs(void);
void CloseLibs(void);
void NewMessageDialog(void);
void UpdateMessageWindow(struct LayoutHandle *Handle);
int GetMessageCount(void);
void GlobalCleanup(void);

/* Tools.c */

VOID LimitedVSPrintf(LONG Size, STRPTR Buffer, STRPTR FormatString, va_list VarArgs);
VOID LimitedSPrintf(LONG Size, STRPTR Buffer, STRPTR FormatString, ...);
VOID LimitedStrcpy(LONG Size, STRPTR To, STRPTR From);
VOID StuffChar(struct FormatContext *Context, UBYTE Char);
VOID InitHook(struct Hook *Hook, HOOKFUNC Func, APTR Data);
STRPTR LocaleHookFunc(struct Hook *UnusedHook, APTR Unused, LONG ID, struct GlobalData * gd);
void Message(UBYTE *Msg,...);
STRPTR LocaleString(ULONG ID);

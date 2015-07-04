#ifndef FIREWORKS_PROTOS_H
#define FIREWORKS_PROTOS_H

#include "fireworks.h"

                             /* fireworks.c */

void BusyPointer(struct Globals *glob, struct Prefs *pref);
void NormalPointer(struct Globals *glob, struct Prefs *pref);

                               /* timer.c */

struct timerequest *OpenTimer(void);
void CloseTimer(struct timerequest *treq);
ULONG GetTimeDelta(void);
LONG HowOldIsTimestamp(ULONG timestamp);

                               /* async.c */

BOOL StartAsyncTask(struct Globals *glob, struct Prefs *pref, UBYTE *TaskName, TaskFlag flg, APTR AsyncFunction, APTR UserData, ULONG UDLength);
void AsyncMessageFunction(struct Globals *glob, struct Prefs *pref, APTR UserData);

BOOL AllocAsync(struct Globals *glob, TaskFlag flg);
void FreeAsync(struct Globals *glob, TaskFlag flg);
BOOL AskAsync(struct Globals *glob);
void WaitAsync(struct Globals *glob);

                              /* message.c */

struct MsgData
{
	UBYTE *Msg;
	UBYTE *Options;
	APTR Args;
};

LONG           MessageA(UBYTE *Msg, UBYTE *Options, APTR Args);
#define Message(Msg, Options, Args...) \
({ IPTR args[] = { AROS_PP_VARIADIC_CAST2IPTR(Args) }; MessageA(Msg, Options, args); })

#if !defined(__AROS__)
LONG __stdargs AsyncMessage(struct Globals *glob, TaskFlag flg, UBYTE *Msg, UBYTE *Options,...);
#else
#define AsyncMessage(glob, flg, msg, options...) \
({ \
    IPTR tmpargs[] = { AROS_PP_VARIADIC_CAST2IPTR(options) }; \
    APTR args = 0; \
    struct MsgData md; \
    if (sizeof(tmpargs) > 1) \
        args = &tmpargs[1]; \
    if (IntuitionBase && (WBMode || tmpargs[0])) \
    { \
            md.Msg = msg; \
            md.Options = (APTR)tmpargs[0]; \
            md.Args = args; \
            if (!StartAsyncTask(glob, NULL, "Fireworks messaging task", flg, &AsyncMessageFunction, &md, sizeof(struct MsgData))) \
            { \
                    MessageA(msg, (APTR)tmpargs[0], args); \
            } \
    } \
})
#endif

                              /* libopen.c */

BOOL OpenLibs(void);
void CloseLibs(void);

                               /* stubs.c */

void __stdargs _XCEXIT(LONG lcode);
#if defined(NO_INLINE_STDARG)
struct MidiNode *CreateMidi(Tag tag, ...);
BOOL SetMidiAttrs(struct MidiNode *mi, Tag tag, ...);
struct MidiLink *AddMidiLink(struct MidiNode *mi, LONG type, Tag tag, ...);
BOOL SetMidiLinkAttrs(struct MidiLink *mi, Tag tag, ...);

APTR GetVisualInfo(struct Screen *screen,Tag tag, ...);
struct Menu *CreateMenus(struct NewMenu *newmenu,Tag tag,...);
BOOL LayoutMenus(struct Menu *menu,APTR vi,Tag tag,...);
ULONG BestModeID(Tag tag,...);
#endif

#ifdef __AROS__
#include <proto/arossupport.h>
#else
void kprintf(UBYTE *fmt,...);
#endif
/*
APTR __stdargs SPrintf(char *, const char *, ...);
APTR __stdargs VSPrintf(char *, const char *, va_list);
*/

#endif /* FIREWORKS_PROTOS_H */

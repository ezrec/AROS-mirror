#include <aros/oldprograms.h>
#include <proto/alib.h>

#include <exec/memory.h>
#include <exec/devices.h>
#include <exec/libraries.h>
#include <libraries/dosextens.h>
#include <intuition/intuition.h>
#include <devices/inputevent.h>
#include <devices/serial.h>
#include <devices/timer.h>
#include <devices/keymap.h>
#include <hardware/cia.h>
//#include "xproto.h"
//#include <functions.h>
//#include <pragmas.h>
#include <string.h>

#define RAW 1
#define COOKED 0

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))
#define C2B(x) (((ULONG)(x)) >> 2)
#define WriteOut(x) Write(Output(), x, strlen(x))
#define ARGVAL() (*++(*argv) || (--argc && *++argv))

VOID *tt_fopen(char *, char *);
VOID tt_fclose(VOID *);
LONG tt_fread(char *, LONG, LONG, VOID *);
LONG tt_fwrite(char *, LONG, LONG, VOID *);
LONG tt_fseek(VOID *, LONG, LONG);
LONG tt_finfo(char *, LONG);
LONG tt_sread(char *, LONG, LONG);
LONG tt_swrite(char *, LONG);
LONG tt_sflush();
VOID tt_update(struct XPR_UPDATE *);
LONG tt_gets(char *, char *);
LONG tt_chkabort();

#if 0
#pragma regcall(tt_fopen(a0,a1))
#pragma regcall(tt_fclose(a0))
#pragma regcall(tt_fread(a0,d0,d1,a1))
#pragma regcall(tt_fwrite(a0,d0,d1,a1))
#pragma regcall(tt_fseek(a0,d0,d1))
#pragma regcall(tt_finfo(a0,d0))
#pragma regcall(tt_sread(a0,d0,d1))
#pragma regcall(tt_swrite(a0,d0))
#pragma regcall(tt_sflush())
#pragma regcall(tt_update(a0))
#pragma regcall(tt_chkabort())
#pragma regcall(tt_gets(a0,a1))
#endif

long dectoint(char *str);
char *inttodec(long num, char *buf, short width);
char *strlwr(char *str);
long SendPacket(struct MsgPort *port, struct MsgPort *replyport, long type, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6, long arg7);
void QueueCon(void);
short CheckCon(void);
long QueueTimer(long micros);
void AbortRequest(short *InProgress, struct IORequest *IOR);
short FinishedIO(short *InProgress, struct IORequest *IOR);
int QueueSerRead(char *buff, long length);
long SerRead(char *buff, long length, long micros);
void SerWrite(char *str, long n);
void SerFlush(void);
int CloseStuff(void);
struct TextFont *SmartOpenFont(struct TextAttr *ta);
int OpenStuff(void);
void RawCon(void);
void StdCon(void);
void tt_update(struct XPR_UPDATE *x);
long tt_gets(char *prompt, char *buffer);
long tt_swrite(char *s, long n);
long tt_sread(char *buff, long length, long micros);
long tt_sflush(void);
void *tt_fopen(char *s, char *t);
void tt_fclose(void *fp);
long tt_fread(char *buff, long size, long count, void *fp);
long tt_fwrite(char *buff, long size, long count, void *fp);
long tt_fseek(void *fp, long offset, long origin);
long tt_chkabort(void);
long tt_finfo(char *filename, long infotype);
int xpr_setup(struct XPR_IO *IO);
short DoKeys(short n);
void CloseProtocol(void);
char *InitProtocol(char *protocol);
struct KeyMap *FindKeymap(char *name);
struct TextFont *SetConFont(struct TextFont *Font);
void SelectProtocol(void);
void Transfer(short direction);
void _wb_parse(void);
void _abort(void);
int main(short argc, char **argv);

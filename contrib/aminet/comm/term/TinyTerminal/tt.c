#include "tt.h"

#define QUIT        1
#define DOWNLOAD    2
#define UPLOAD      3

struct GfxBase *GfxBase = NULL;
struct DiskFontBase *DiskfontBase = NULL;

struct Device *ConsoleDevice = NULL;
struct Library *XProtocolBase = NULL;
struct XPR_IO *XIO = NULL;
LONG XPRFlags = 0;

APTR OldWindowPtr;

BPTR filein;
BPTR fileout;
LONG confh;
struct StandardPacket *MyPacket = NULL;

struct MsgPort *ConPort;
struct MsgPort *ConSink = NULL;
struct MsgPort *SerSink = NULL;
struct MsgPort *TimerSink = NULL;
struct IOExtSer IOsr;
struct IOExtSer IOsw;
struct IOExtSer IOss;
struct IOStdReq IOcr;
struct timerequest IOtr;
LONG ConSig;
LONG SerSig;
LONG TimerSig;

struct KeyMap *Keymap = NULL;
struct InputEvent RKE = {
    NULL,
    IECLASS_RAWKEY,
    0,
    0,
    0
};

char *SerInBuf = NULL;
char *SerOutBuf;
char ConInBuf[64];
char CSBuf[64];
char Init[256];

WORD CSState = 0;
WORD CSPos = 0;

char *DeviceName = "serial.device";
LONG DeviceUnit = 0;
char *Protocol = NULL;
char *KeymapName = NULL;
char *FontName = NULL;
char *XferDir = NULL;

struct TextFont *Font = NULL;
struct TextFont *OldFont = NULL;

WORD IOswIP;
WORD IOsrIP;
WORD PIP;
WORD TIP;

LONG dectoint(REGISTER char *str)
{
    REGISTER long val = 0;
    REGISTER char c;
    while ((c = *str) >= '0' && c <= '9') {
        val = (((val<<2)+val)<<1) + c-'0';
        str++;
    }
    return(val);
}

char *inttodec(LONG num, char *buf, WORD width)
{
    REGISTER WORD i;
    for (i = 0; i < width; i++) {
        buf[i] = ' ';
    }
    buf[width - 1] = '0';
    buf[width] = '\0';
    i = width;
    while (num && --i >= 0) {
        buf[i] = '0' + (num % 10);
        num /= 10;
    }
    return buf;
}

char *strlwr(str)
char *str;
{
    REGISTER char *p = str;
    REGISTER char c;

    while (0 != (c = *p)) {
        if ('A' <= c && c <= 'Z') {
            *p = c + ('a' - 'A');
        }
        ++p;
    }
    return str;
}

LONG SendPacket(struct MsgPort *port,
                struct MsgPort *replyport,
                LONG type,
                LONG arg1, LONG arg2, LONG arg3, LONG arg4,
                LONG arg5, LONG arg6, LONG arg7)
{
    struct StandardPacket *Packet;
    struct MsgPort *rport = replyport;

    Packet = AllocMem(sizeof(struct StandardPacket), MEMF_PUBLIC|MEMF_CLEAR);
    if (!Packet) {
        return -1L;
    }

    if (!replyport) {
        rport = CreatePort(0L, 0L);
        if (!rport) {
            FreeMem(Packet, sizeof(struct StandardPacket));
            return -1L;
        }
    }
    Packet->sp_Msg.mn_Node.ln_Type = NT_MESSAGE;
    Packet->sp_Msg.mn_Node.ln_Name = (char *)&(Packet->sp_Pkt);
    Packet->sp_Msg.mn_ReplyPort = rport;
    Packet->sp_Pkt.dp_Link = &Packet->sp_Msg;
    Packet->sp_Pkt.dp_Port = rport;
    Packet->sp_Pkt.dp_Type = type;
    Packet->sp_Pkt.dp_Arg1 = arg1;
    Packet->sp_Pkt.dp_Arg2 = arg2;
    Packet->sp_Pkt.dp_Arg3 = arg3;
    Packet->sp_Pkt.dp_Arg4 = arg4;
    Packet->sp_Pkt.dp_Arg5 = arg5;
    Packet->sp_Pkt.dp_Arg6 = arg6;
    Packet->sp_Pkt.dp_Arg7 = arg7;
    PutMsg(port, (struct Message *)Packet);
    if (!replyport) {
        LONG result;

        do {
            WaitPort(rport);
        } while (!GetMsg(rport));
        DeletePort(rport);
        result = Packet->sp_Pkt.dp_Res1;
        FreeMem(Packet, sizeof(struct StandardPacket));
        return result;
    } else {
        return (LONG)Packet;
    }
}

VOID QueueCon()
{
    if (!PIP) {
        SendPacket(ConPort, ConSink, ACTION_READ,
          confh, (LONG)ConInBuf, 64, 0, 0, 0, 0);
        PIP = TRUE;
    }
}

WORD CheckCon()
{
    WORD chars = -1;
    struct StandardPacket *p;

    if (p = (struct StandardPacket *)GetMsg(ConSink)) {
        PIP = FALSE;
        chars = p->sp_Pkt.dp_Res1;
        FreeMem(p, sizeof(struct StandardPacket));
    }
    return chars;
}

LONG QueueTimer(LONG micros)
{
    LONG timeout = micros;

    if (!TIP) {
        if (!timeout) {
            return 0;
        }
        if (micros > 3000000) {
            timeout = 3000000;
        }
        IOtr.tr_time.tv_secs  = timeout / 1000000;
        IOtr.tr_time.tv_micro = timeout % 1000000;
        SendIO((struct IORequest *)&IOtr);
        TIP = TRUE;
        return micros - timeout;
    }
}

VOID AbortRequest(WORD *InProgress, struct IORequest *IOR)
{
    if (*InProgress) {
        AbortIO(IOR);
        WaitIO(IOR);
        *InProgress = FALSE;
    }
}

#define AbortTimer()    AbortRequest(&TIP, (struct IORequest *)&IOtr)
#define AbortSerRead()  AbortRequest(&IOsrIP, (struct IORequest *)&IOsr)
#define AbortSerWrite() AbortRequest(&IOsrIP, (struct IORequest *)&IOsr)

WORD FinishedIO(WORD *InProgress, struct IORequest *IOR)
{
    if (*InProgress && CheckIO(IOR)) {
        WaitIO(IOR);
        *InProgress = FALSE;
        return TRUE;
    } else {
        return FALSE;
    }
}

#define FinishedTimer()     FinishedIO(&TIP, (struct IORequest *)&IOtr)
#define FinishedSerRead()   FinishedIO(&IOsrIP, (struct IORequest *)&IOsr)
#define FinishedSerWrite()  FinishedIO(&IOswIP, (struct IORequest *)&IOsw)

void QueueSerRead(char *buff, LONG length)
{
    if (!IOsrIP) {
        IOsr.IOSer.io_Command = CMD_READ;
        IOsr.IOSer.io_Length = length;
        IOsr.IOSer.io_Data = (APTR)buff;
        SendIO((struct IORequest *)&IOsr);
        IOsrIP = TRUE;
    }
}

LONG SerRead(char *buff, LONG length, LONG micros)
{
    LONG pos = 0;
    WORD n;
    WORD carrier;

    if (!buff) {
        return -1;
    }

    micros = QueueTimer(micros);

    for (;;) {
        if (!IOsrIP) {
            IOss.IOSer.io_Command = SDCMD_QUERY;
            DoIO((struct IORequest *)&IOss);
            carrier = (IOss.io_Status & CIAF_COMCD) ? 0 : 1;
            if (n = IOss.IOSer.io_Actual) {
                IOsr.IOSer.io_Command = CMD_READ;
                IOsr.IOSer.io_Length = min(n, length);
                IOsr.IOSer.io_Data = (APTR)&buff[pos];
                DoIO((struct IORequest *)&IOsr);
                pos += IOsr.IOSer.io_Actual;
                length -= IOsr.IOSer.io_Actual;
            }
        }
        if (length == 0 || !TIP) {
            AbortTimer();
            return pos;
        }
        if (FinishedTimer()) {
            if (micros) {
                micros = QueueTimer(micros);
            } else {
                AbortSerRead();
                return pos;
            }
        }
        if (!carrier) {
            AbortTimer();
            AbortSerRead();
            return -1;
        }
        QueueSerRead((char *)&buff[pos], length);
        if (Wait(TimerSig | SerSig) & SerSig) {
            if (FinishedSerRead()) {
                pos += IOsr.IOSer.io_Actual;
                length -= IOsr.IOSer.io_Actual;
            }
        }
    }
}

VOID SerWrite(char *str, LONG n)
{
    LONG temp = n;
    char t[10];

    while (n) {
        if (IOswIP) {
            WaitIO((struct IORequest *)&IOsw);
            IOswIP = FALSE;
        }
        temp = min(n, 256);
        CopyMem(str, SerOutBuf, temp);
        IOsw.IOSer.io_Command = CMD_WRITE;
        IOsw.IOSer.io_Length = temp;
        IOsw.IOSer.io_Data = (APTR)SerOutBuf;
        SendIO((struct IORequest *)&IOsw);
        IOswIP = TRUE;
        n -= temp;
    }
}

VOID SerFlush()
{
    IOss.IOSer.io_Command = CMD_FLUSH;
    DoIO((struct IORequest *)&IOss);
}

CloseStuff()
{
    if (SerInBuf)               FreeMem(SerInBuf, 512);
    if (IOsw.IOSer.io_Message.mn_ReplyPort) {
        DeletePort(IOsw.IOSer.io_Message.mn_ReplyPort);
    }
    if (IOsr.IOSer.io_Device)   CloseDevice((struct IORequest *)&IOsr);
    if (IOtr.tr_node.io_Device) CloseDevice((struct IORequest *)&IOtr);
    if (IOcr.io_Device)         CloseDevice((struct IORequest *)&IOcr);
    if (ConSink)                DeletePort(ConSink);
    if (SerSink)                DeletePort(SerSink);
    if (TimerSink)              DeletePort(TimerSink);
    if (Font)                   CloseFont(Font);
    if (DiskfontBase)           CloseLibrary((struct Library *)DiskfontBase);
    if (GfxBase)                CloseLibrary((struct Library *)GfxBase);
    ((struct Process *)FindTask(NULL))->pr_WindowPtr = OldWindowPtr;
    exit(0);
}

struct TextFont *SmartOpenFont(ta)
struct TextAttr *ta;
{
    struct TextFont *MemF;
    struct TextFont *DskF = NULL;

    if ((MemF = OpenFont(ta)) && ta->ta_YSize == MemF->tf_YSize) {
        return MemF;
    }

    if (DiskfontBase) {
        DskF = OpenDiskFont(ta);
    }

    if (!MemF) {
        return DskF;
    }
    if (!DskF) {
        return MemF;
    }
    if (abs(ta->ta_YSize - MemF->tf_YSize) <=
        abs(ta->ta_YSize - DskF->tf_YSize)) {
        CloseFont(DskF);
        return MemF;
    }
    CloseFont(MemF);
    return DskF;
}

OpenStuff()
{
    OldWindowPtr = ((struct Process *)FindTask(NULL))->pr_WindowPtr;
    ((struct Process *)FindTask(NULL))->pr_WindowPtr = (APTR)-1;
    IOsr.IOSer.io_Device = NULL;
    IOsw.IOSer.io_Message.mn_ReplyPort = NULL;
    IOtr.tr_node.io_Device = NULL;
    IOcr.io_Device = NULL;

    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    if (!GfxBase) {
        Write(fileout, "Can't open graphics.library.\n", 29);
        CloseStuff();
    }

    if (FontName) {
        struct TextAttr ta;
        char temp[33];
        WORD pos = 0;

        DiskfontBase = (struct DiskFontBase *)OpenLibrary("diskfont.library", 0);
        strcpy(temp, FontName);
        ta.ta_Name = (UBYTE *)temp;
        while (temp[pos] && temp[pos] != '/') {
            ++pos;
        }
        if (temp[pos] == '\0') {     /* No size */
            ta.ta_YSize = 8;
        } else {
            temp[pos] = '\0';
            ta.ta_YSize = dectoint(&temp[pos + 1]);
        }
        if (pos > 5) {
            (VOID)strlwr(&temp[pos - 5]);
            if (strcmp(&temp[pos - 5], ".font")) {
                strcpy(&temp[pos], ".font");
            }
        } else {
            strcpy(&temp[pos], ".font");
        }
        ta.ta_Style = 0;
        ta.ta_Flags = 0;
        Font = SmartOpenFont(&ta);
        if (!Font) {
            Write(fileout, "Can't open font.\n", 17);
        }
    }

    if (!(ConSink = CreatePort(0, 0))) {
        Write(fileout, "Can't open console port.\n", 25);
        CloseStuff();
    }
    if (!(SerSink = CreatePort(0, 0))) {
        Write(fileout, "Can't open serial read port.\n", 29);
        CloseStuff();
    }
    if (!(TimerSink = CreatePort(0, 0))) {
        Write(fileout, "Can't open timer port.\n", 23);
        CloseStuff();
    }
    ConSig = 1L << ConSink->mp_SigBit;
    SerSig = 1L << SerSink->mp_SigBit;
    TimerSig = 1L << TimerSink->mp_SigBit;

    IOsr.io_SerFlags = SERF_SHARED | SERF_XDISABLED;
    IOsr.IOSer.io_Message.mn_ReplyPort = SerSink;

    if (OpenDevice(DeviceName, DeviceUnit, (struct IORequest *)&IOsr, NULL)) {
        IOsr.IOSer.io_Device = NULL;
        Write(fileout, "Can't open serial port for read.\n", 33);
        CloseStuff();
    }

    /*
     *        Assume a Getty is running, if the opencount is > 2 then
     *        assume collision and disallow
     */

    if (IOsr.IOSer.io_Device->dd_Library.lib_OpenCnt > 2) {
        Write(fileout, "Collision, serial port in use!\n", 31);
        CloseStuff();
    }

    CopyMem((char *)&IOsr, (char *)&IOsw, sizeof(struct IOExtSer));
    IOsw.IOSer.io_Message.mn_ReplyPort = CreatePort(0, 0);
    if (!IOsw.IOSer.io_Message.mn_ReplyPort) {
        Write(fileout, "Can't open serial write port.\n", 30);
        CloseStuff();
    }
    CopyMem((char *)&IOsw, (char *)&IOss, sizeof(struct IOExtSer));

    SerInBuf = AllocMem(512, MEMF_PUBLIC | MEMF_CLEAR);
    if (!SerInBuf) {
        Write(fileout, "No memory for serial buffers.\n", 30);
        CloseStuff();
    }
    SerOutBuf = &SerInBuf[256];

    if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)&IOtr, 0))  {
        Write(fileout, "Can't open timer device.\n", 25);
        CloseStuff();
    }

    IOtr.tr_node.io_Message.mn_ReplyPort = TimerSink;
    IOtr.tr_node.io_Command = TR_ADDREQUEST;
    IOtr.tr_node.io_Error = 0;

    if (OpenDevice("console.device", -1L, (struct IORequest *)&IOcr, NULL)) {
        Write(fileout, "Can't open console device.\n", 27);
        CloseStuff();
    }
    ConsoleDevice = IOcr.io_Device;
}

VOID RawCon()
{
    if (!filein) {
        filein = Open("*", MODE_OLDFILE);
        ConPort = ((struct FileHandle *)BADDR(filein))->fh_Type;
        confh = ((struct FileHandle *)BADDR(filein))->fh_Arg1;
        SendPacket(ConPort, NULL, ACTION_SCREEN_MODE, RAW, 0, 0, 0, 0, 0, 0);
        Write(fileout, "\2331{", 3);
    }
}

VOID StdCon()
{
    if (filein) {
        Write(fileout, "\2331}", 3);
        SendPacket(ConPort, NULL, ACTION_SCREEN_MODE, COOKED, 0, 0, 0, 0, 0, 0);
        Close(filein);
        filein = NULL;
        if (PIP) {
            WaitPort(ConSink);
            MyPacket = (struct StandardPacket *)GetMsg(ConSink);
            FreeMem(MyPacket, sizeof(struct StandardPacket));
        }
    }
}


VOID tt_update(struct XPR_UPDATE *x)
{
    char temp[10];
    STATIC char msg[51];
    STATIC char filename[51];
    STATIC LONG bytes;
    LONG update;

    int_start();

    update = x->xpru_updatemask;
    if (update & XPRU_MSG)       strncpy(msg, x->xpru_msg, 50);
    if (update & XPRU_ERRORMSG)  strncpy(msg, x->xpru_errormsg, 50);
    if (update & XPRU_FILENAME)  strncpy(filename, x->xpru_filename, 50);
    if (update & XPRU_BYTES)     bytes = x->xpru_bytes;
    Write(fileout, msg, strlen(msg));
    Write(fileout, "\233K\n", 3);
    Write(fileout, filename, strlen(filename));
    Write(fileout, "\233K\nBytes: ", 10);
    Write(fileout, inttodec(bytes, temp, 7), 7);
    Write(fileout, "\n", 1);
    if (update & (XPRU_EXPECTTIME | XPRU_ELAPSEDTIME)) {
        Write(fileout, "Expected time: ", 15);
        Write(fileout, x->xpru_expecttime, strlen(x->xpru_expecttime));
        Write(fileout, "  Elapsed time: ", 16);
        Write(fileout, x->xpru_elapsedtime, strlen(x->xpru_elapsedtime));
    }
    Write(fileout, "\r\2333A", 4);
    int_end();
}

LONG tt_gets(char *prompt, char *buffer)
{
    LONG length;
    int_start();
    StdCon();
    Write(fileout, prompt, strlen(prompt));
    length = Read(Input(), buffer, 256);
    if (length > 0) {
        buffer[length - 1] = '\0';
    }
    RawCon();
    int_end();
    return 0;
}

LONG tt_swrite(char *s, LONG n)
{
    char temp[10];
    int_start();
    SerWrite(s, n);
    int_end();
    return 0;
}

LONG tt_sread(char *buff, LONG length, LONG micros)
{
    LONG result;
    int_start();
    result = SerRead(buff, length, micros);
    int_end();
    return result;
}

LONG tt_sflush()
{
    int_start();
    SerFlush();
    while (SerRead(SerInBuf, 256, 0));  /* Clear serial buffer */
    int_end();
}

VOID *tt_fopen(s, t)
char *s, *t;
{
    BPTR file = NULL;
    int_start();
    switch (*t) {
        case 'r':
            file = Open(s, MODE_OLDFILE);
            break;
        case 'w':
            file = Open(s, MODE_NEWFILE);
            break;
        case 'a':
            file = Open(s, MODE_OLDFILE);
            if (!file) {
                file = Open(s, MODE_NEWFILE);
            }
            if (file) {
                Seek(file, 0, OFFSET_END);
            }
            break;
    }
    int_end();
    return (VOID *)file;
}

VOID tt_fclose(VOID *fp)
{
    int_start();
    Close((BPTR)fp);
    int_end();
}

LONG tt_fread(char *buff, LONG size, LONG count, VOID *fp)
{
    LONG result;
    int_start();
    result = Read((BPTR)fp, buff, size * count);
    if (result == -1) {
        result = 0;
    }
    result /= size;
    int_end();
    return result;
}

LONG tt_fwrite(char *buff, LONG size, LONG count, VOID *fp)
{
    LONG result;
    int_start();
    result = Write((BPTR)fp, buff, size * count);
    if (result == -1) {
        result = 0;
    }
    result /= size;
    int_end();
    return result;
}

LONG seekmode[3] = {
    OFFSET_BEGINNING,
    OFFSET_CURRENT,
    OFFSET_END
};

LONG tt_fseek(VOID *fp, LONG offset, LONG origin)
{
    int_start();
    Seek((BPTR)fp, offset, seekmode[origin]);
    int_end();
    return 0;
}

LONG tt_chkabort()
{
    LONG abort = 0;
    WORD n;
    int_start();
    if ((n = CheckCon()) != -1) {
        WORD i;
        for (i = 0; i < n; i++) {
            if (ConInBuf[i] == 27) {
                abort = 1;
                break;
            }
        }
        if (!abort) {
            QueueCon();
        }
    }
    int_end();
    return abort;
}

LONG tt_finfo(char *filename, LONG infotype)
{
    LONG result = 0;

    int_start();
    if (infotype == 1) {
        BPTR lock;
        struct FileInfoBlock *fib;
        if (lock = Lock(filename, ACCESS_READ)) {
            if (fib = AllocMem(sizeof(struct FileInfoBlock), MEMF_PUBLIC)) {
                if (Examine(lock, (BPTR)fib)) {
                    result = fib->fib_Size;
                }
                FreeMem(fib, sizeof(struct FileInfoBlock));
            }
            UnLock(lock);
        }
    } else if (infotype == 2) {
        result = 1;   /* Always binary */
    }
    int_end();
    return result;
}

int xpr_setup(struct XPR_IO *IO)
{
   IO->xpr_filename  = NULL;
   IO->xpr_fopen     = tt_fopen;
   IO->xpr_fclose    = tt_fclose;
   IO->xpr_fread     = tt_fread;
   IO->xpr_fwrite    = tt_fwrite;
   IO->xpr_sread     = tt_sread;
   IO->xpr_swrite    = tt_swrite;
   IO->xpr_sflush    = tt_sflush;
   IO->xpr_update    = tt_update;
   IO->xpr_chkabort  = tt_chkabort;
   IO->xpr_chkmisc   = NULL;
   IO->xpr_gets      = tt_gets;
   IO->xpr_setserial = NULL;
   IO->xpr_ffirst    = NULL;
   IO->xpr_fnext     = NULL;
   IO->xpr_finfo     = tt_finfo;
   IO->xpr_fseek     = tt_fseek;
   IO->xpr_extension = 0L;
   IO->xpr_options   = NULL;
   IO->xpr_data      = NULL;

   return;
}

WORD DoKeys(WORD n)
{
    WORD i;
    WORD chars = 0;

    for (i = 0; i < n; i++) {
        unsigned char ch = ConInBuf[i];
        if (CSState == 1) {
            if (ch == '[') {
                CSState = 2;
            } else {
                CSState = -2;
            }
        }
        if (ch == 0x9B) {
            CSState = 2;
        }
        if (ch == 0x1B) {
            CSState = 1;
        }
        if ((CSState == 3) && ((ch >= 0x40) && (ch <= 0x7E))) {
            CSState = -1;
            if (ch == '|') {    /* Raw Key Event */
                REGISTER char *p;
                LONG temp;
                CSBuf[CSPos] = 0;
                CSPos = 0;
                p = CSBuf;
                RKE.ie_Class = dectoint(p);     while (*p++ != ';');
                RKE.ie_SubClass = dectoint(p);  while (*p++ != ';');
                RKE.ie_Code = dectoint(p);      while (*p++ != ';');
                RKE.ie_Qualifier = dectoint(p); while (*p++ != ';');
                temp = dectoint(p) << 16;       while (*p++ != ';');
                temp |= dectoint(p);
                RKE.ie_EventAddress = (APTR)temp;
                temp = RawKeyConvert(&RKE, &ConInBuf[chars], 64 - chars, Keymap);
                  /* If it's a command key, dump all other chars, put
                     the command key in position 0 and return -1 */
                if (temp == 1 && RKE.ie_Qualifier & IEQUALIFIER_RCOMMAND) {
                    ConInBuf[0] = ConInBuf[chars];
                    return -1;
                }
                chars += temp;
            }
        }
        if (CSState == 3) {
            CSBuf[CSPos++] = ch;
        }
        if (CSState == 2) {
            CSState = 3;
            CSPos = 0;
        }
    }
    return chars;
}

VOID CloseProtocol()
{
    if (XIO) {
        XProtocolCleanup(XIO);
        XPRFlags = 0;
        FreeMem(XIO, sizeof(struct XPR_IO));
    }
    if (XProtocolBase)          CloseLibrary(XProtocolBase);
}

char *InitProtocol(char *protocol)
{
    char options[256];
    char temp[64];
    BPTR envfile;

    options[0] = '\0';
    strcpy(temp, "ENV:xpr");
    strcat(temp, protocol);
    if (envfile = Open(temp, MODE_OLDFILE)) {
        LONG length;
        length = Read(envfile, options, sizeof(options));
        if (length == -1) {
            options[0] = '\0';
        } else {
            REGISTER WORD i = 0;
            while (i < length) {
                if (options[i] == '\n') {
                    options[i] = '\0';
                    length = i;
                }
            }
        }
        Close(envfile);
    }
    strcpy(temp, "xpr");
    strcat(temp, protocol);
    strcat(temp, ".library");
    XProtocolBase = OpenLibrary(temp, 0);
    if (!XProtocolBase) {
        Write(fileout, "Can't open protocol ", 20);
        Write(fileout, protocol, strlen(protocol));
        Write(fileout, ".\n", 2);
        return NULL;
    }
    XIO = AllocMem(sizeof(struct XPR_IO), MEMF_PUBLIC | MEMF_CLEAR);
    if (!XIO) {
        return NULL;
    }
    xpr_setup(XIO);
    XIO->xpr_filename = options;
    XPRFlags = XProtocolSetup(XIO);
    if (!(XPRFlags & XPRS_SUCCESS)) {
        Write(fileout, "Can't open protocol ", 20);
        Write(fileout, protocol, strlen(protocol));
        Write(fileout, ".\n", 2);
        CloseProtocol();
        return NULL;
    }
}

struct KeyMap *FindKeymap(char *name)
{
    struct KeyMapResource *KMR;
    KMR = (struct KeyMapResource *)OpenResource("keymap.resource");
    if (KMR) {
        struct KeyMapNode *KMN;
        KMN = (struct KeyMapNode *)FindName(&KMR->kr_List, name);
        if (!KMN) {
            Write(fileout, "Keymap not loaded. Using default.\n", 34);
        } else {
            return &KMN->kn_KeyMap;
        }
    }
    return NULL;
}

struct TextFont *SetConFont(struct TextFont *Font)
{
    struct InfoData *id;
    struct TextFont *oldfont;
    struct Window *window;

    id = AllocMem(sizeof(struct InfoData), MEMF_PUBLIC | MEMF_CLEAR);
    SendPacket(ConPort, NULL, ACTION_DISK_INFO,
      C2B(id), 0, 0, 0, 0, 0, 0);
    window = (struct Window *)id->id_VolumeNode;
    oldfont = window->RPort->Font;
    SetFont(window->RPort, Font);
    Write(fileout, "\033c", 2);
    return oldfont;
}

VOID SelectProtocol()
{
    char protocol[256];

    tt_gets("Select protocol: ", protocol);
    if (strlen(protocol)) {
        CloseProtocol();
        InitProtocol(protocol);
    }
}

VOID Transfer(WORD direction)
{
    char filename[256];

    if (XProtocolBase) {
        tt_gets((direction ? "Download file: " : "Upload file: "), filename);
        if (strlen(filename)) {
            XIO->xpr_filename = filename;
            Write(fileout, "-- ", 3);
            Write(fileout, (direction ? "Download" : " Upload "), 8);
            Write(fileout, " -- ESC to abort --\n\2330 p", 24);
            QueueCon();
            if (direction) {
                XProtocolReceive(XIO);
            } else {
                XProtocolSend(XIO);
            }
            while (SerRead(SerInBuf, 256, 0));  /* Clear serial buffer */
            Write(fileout, "\7\n\n\n\n\n\233 p", 9);
        }
    } else {
        Write(fileout, "Transfer: No protocol selected.\n", 32);
    }
}

VOID _wb_parse() {}
VOID _abort() {}

main(WORD argc, char **argv)
{
    BOOL go;
    WORD eoa;
    BPTR Home = NULL;

    fileout = Output();
    strcpy(Init, "");
    eoa = 0;
    for (argc--, argv++; argc > 0; argc--, argv++) {
        if (**argv == '-' && !eoa) { /* Argument coming up */
            switch (*++(*argv)) {
                case 'S':       /* Serial device */
                    if (ARGVAL()) DeviceName = *argv;
                    break;
                case 'U':       /* Serial unit */
                    if (ARGVAL()) DeviceUnit = dectoint(*argv);
                    break;
                case 'P':       /* Protocol */
                    if (ARGVAL()) Protocol = *argv;
                    break;
                case 'K':       /* Keymap */
                    if (ARGVAL()) KeymapName = *argv;
                    break;
                case 'F':       /* Font */
                    if (ARGVAL()) FontName = *argv;
                    break;
                case 'D':       /* Transfer directory */
                    if (ARGVAL()) XferDir = *argv;
                    break;
                case '-':       /* End of args */
                    eoa = 1;
                    break;
            }
        } else if (!strcmp(*argv, "?")) {
            Write(fileout,
              "Usage: tt [-Sserial.device] [-Uunit] [-Pprotocol]\n"
              "          [-Kkeymap] [-Ffont/size] [-Dxferdirectory]\n"
              "          [modem startup text]\n", 134);
            return(0);
        } else {
            strcat(Init, *argv);
            strcat(Init, "\r");
        }
    }
    OpenStuff();
    RawCon();
    if (Protocol) {
        InitProtocol(Protocol);
    }
    if (KeymapName) {
        Keymap = FindKeymap(KeymapName);
    }
    if (Font) {
        OldFont = SetConFont(Font);
    }
    if (XferDir) {
        BPTR temp = Lock(XferDir, ACCESS_READ);
        if (!temp) {
            Write(fileout, "Can't find transfer directory.\n", 31);
        } else {
            Home = CurrentDir((struct FileLock *)temp);
        }    
    }
    IOswIP = FALSE;
    IOsrIP = FALSE;
    PIP = FALSE;
    TIP = FALSE;

    while (SerRead(SerInBuf, 256, 0));  /* Clear serial buffer */

    SerWrite(Init, strlen(Init));

    QueueSerRead(SerInBuf, 1);
    QueueCon();
    go = TRUE;
    while (go) {
        WORD chars;
        LONG signals;

        signals = Wait(ConSig | SerSig | SIGBREAKF_CTRL_C);
        if (signals & SIGBREAKF_CTRL_C) {
            go = FALSE;
        }

        if ((chars = CheckCon()) != -1) {
            chars = DoKeys(chars);
            if (chars == -1) {
                switch (ConInBuf[0]) {
                    case 'q':
                        go = FALSE;
                        break;
                    case 'p':
                        SelectProtocol();
                        break;
                    case 'u':
                        Transfer(0);
                        break;
                    case 'd':
                        Transfer(1);
                        break;
                }
            } else {
                if (XPRFlags & XPRS_USERMON) {
                    chars = XProtocolUserMon(XIO, ConInBuf, chars, 64);
                }
                if (chars) {
                    SerWrite(ConInBuf, chars);
                }
            }
        }
        if (FinishedSerRead()) {
            chars = IOsr.IOSer.io_Actual;
            chars += SerRead(&SerInBuf[chars], 256 - chars, 0);
            if (chars > 0) {
                if (XPRFlags & XPRS_HOSTMON) {
                    chars = XProtocolHostMon(XIO, SerInBuf, chars, 256);
                }
                if (chars) {
                    Write(fileout, SerInBuf, chars);
                }
            }
        }
        QueueCon();
        QueueSerRead(SerInBuf, 1);
    }
    if (Home) {
        UnLock(CurrentDir((struct FileLock *)Home));
    }
    if (OldFont) {
        (VOID)SetConFont(OldFont);
    }
    StdCon();
    AbortSerRead();
    AbortSerWrite();
    CloseProtocol();
    CloseStuff();
}

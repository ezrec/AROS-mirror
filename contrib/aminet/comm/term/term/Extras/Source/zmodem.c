/*
**  ZModem.c
**
**  Built-in ZModem file transfer protocol.
**
**  :ts=4
**
**  Based upon the zmodem.c code used by MacTerminal, written by
**
**      Erny Tontlinger
**      33, route d'Arlon
**      L-8410 Steinfort
**      Luxembourg
**
**  Which in turn is based upon work by
**
**      Chuck Forsberg
**      Omen Technology, Inc
**      Post Office Box 4681
**      Portland OR 97208
**      USA
**
**  All modifications for `term', great and small, are
**  copyright © 1990-1996 by Olaf `Olsen' Barthel
**      All Rights Reserved
*/

#ifdef BUILTIN_ZMODEM

#ifndef _GLOBAL_H
#include "Global.h"
#endif

#include <setjmp.h>

#ifdef FINE
#undef FINE
#endif /* FINE */

#ifdef XON
#undef XON
#endif /* XON */

#ifdef CAN
#undef CAN
#endif /* CAN */

#ifdef DEL
#undef DEL
#endif /* DEL */

enum
{
	GAD_NameBox=1000,
	GAD_BatchBox,
	GAD_MessageList,
	GAD_TransferBox,
	GAD_FileProgress,
	GAD_BatchProgress,
	GAD_Abort
};

enum
{
	NAMEBOX_Name,
	NAMEBOX_Size,
	NAMEBOX_Completion
};

enum
{
	BATCHBOX_Count,
	BATCHBOX_Size,
	BATCHBOX_Completion
};

enum
{
	TRANSFERBOX_Errors,
	TRANSFERBOX_Retries,
	TRANSFERBOX_CPS
};

enum
{
  FINE = 0,                     /* No error */
  NOTHING = 1000,               /* No data available */
  TIMEOUT,                      /* Timeout error */
  CANCEL,                       /* Cancel by button or menu */
  ABORT,                        /* Abort by ctrl-X received */
  ERROR,                        /* Other error */
  NO_CARRIER,                   /* Carrier lost */
  LINE_ERROR,
  PARITY_ERROR,
  TIMER_ERROR,
  BUFFER_OVERFLOW,
  NO_DSR,
  BREAK_DETECTED
};

STATIC BOOL ZEscapeCtl = FALSE;                 /* escape all control characters. */
STATIC BOOL ZOverwrite = FALSE;                 /* clobber existing files. */
STATIC LONG ZTimeout = 10;                      /* receive timeout (seconds) */
STATIC LONG ZBuffer = 0;                        /* receive buffer size (bytes) */
STATIC LONG ZRetries = 10;                      /* receive maximum retries */
STATIC LONG ZErrors = 10;                       /* maximum errors */
STATIC LONG ZPacket = 1024;                     /* transmit sub-packet length (bytes) */
STATIC LONG ZWindow = 0;                        /* transmit window size (bytes) */
STATIC LONG Zcrcq = 0;                          /* transmit ZCRCQ spacing (bytes) */
STATIC BOOL ZKeepPartial = TRUE;                /* keep partial files; do not delete them */
STATIC BOOL ZDeleteAfterSending = FALSE;        /* delete the file after sending */
STATIC BOOL ZProtectAfterSending = FALSE;       /* add archived bit after sending */
STATIC BOOL ZSkipIfArchived = FALSE;            /* skip file if archived bit is set */
STATIC BOOL ZSendFullPath = FALSE;              /* send complete file path */
STATIC BOOL ZUseFullPath = FALSE;               /* use complete file path */
STATIC BOOL ZTransferIfNewer = FALSE;           /* transfer file if newer than existing file */
STATIC BOOL ZTransferIfSize = FALSE;            /* transfer file if size different from existing file */

/* ----- Special characters -------------------------------------------- */

#define BS          0x08        /* Backspace */
#define LF          0x0A        /* Linefeed */
#define CR          0x0D        /* Carriage return */
#define DLE         0x10        /* Data link escape */
#define XON         0x11        /* Flow control */
#define XOFF        0x13        /* Flow control */
#define CAN         0x18        /* Cancel */
#define DEL         0x7F        /* Delete */

#define ZPAD        '*'         /* Padding character begins frames */
#define ZDLE        ('X'&0x1F)  /* Zmodem escape character ctrl-X */
#define ZBIN        'A'         /* Binary header indicator (CRC-16) */
#define ZHEX        'B'         /* HEX header indicator */

/* ----- Frame types --------------------------------------------------- */

#define ZRQINIT     0           /* Request receive init */
#define ZRINIT      1           /* Receive init */
#define ZSINIT      2           /* Send init sequence (optional) */
#define ZACK        3           /* ACK to above */
#define ZFILE       4           /* File name from sender */
#define ZSKIP       5           /* To sender: skip this file */
#define ZNAK        6           /* Last packet was garbled */
#define ZABORT      7           /* Abort batch transfers */
#define ZFIN        8           /* Finish session */
#define ZRPOS       9           /* Resume data trans at this position */
#define ZDATA       10          /* Data packet(s) follow */
#define ZEOF        11          /* End of file */
#define ZFERR       12          /* Fatal Read or Write error Detected */
#define ZCRC        13          /* Request for file CRC and response */
#define ZCHALLENGE  14          /* Receiver's Challenge */
#define ZCOMPL      15          /* Request is complete */
#define ZCAN        16          /* Other end canned session with 5 CANs */
#define ZFREECNT    17          /* Request for free bytes on filesystem */
#define ZCOMMAND    18          /* Command from sending program */
#define ZSTDERR     19          /* Output to standard error, data follows */

/* ----- ZDLE sequences ------------------------------------------------ */

#define ZCRCE       'h'         /* CRC next, frame ends, header follows */
#define ZCRCG       'i'         /* CRC next, frame continues nonstop */
#define ZCRCQ       'j'         /* CRC next, frame continues, ZACK expected */
#define ZCRCW       'k'         /* CRC next, ZACK expected, end of frame */
#define ZRUB0       'l'         /* Translate to DEL 0x7F */
#define ZRUB1       'm'         /* Translate to DEL 0xFF */

/* ----- Header structures --------------------------------------------- */

/* olsen: neat idea, but assumes that bit fields will be allocated
 *        from left to right. Luckily, that's just what SAS/C 6.50 does.
 */

typedef struct
{
  unsigned int pad:24;
  unsigned int command:8;       /* 0 or ZCOMMAND */
}
ZRQINITflags;

typedef struct
{
  unsigned int bufsize:16;      /* Receiver's buffer size (bytes swapped) */

  unsigned int pad:7;
  unsigned int canvhdr:1;       /* Variable headers OK */

  unsigned int esc8:1;          /* Rx expects 8th bit to be escaped */
  unsigned int escctl:1;        /* Rx expects ctl chars to be escaped */
  unsigned int canfc32:1;       /* Rx can use 32 bit CRC */
  unsigned int canlzw:1;        /* Rx can uncompress */
  unsigned int canrle:1;        /* Rx can decode RLE */
  unsigned int canbrk:1;        /* Rx can send a break signal */
  unsigned int canovio:1;       /* Rx can receive data during disk I/O */
  unsigned int canfdx:1;        /* Rx can send and receive true FDX */
}
ZRINITflags;

typedef struct
{
  unsigned int pad0:24;

  unsigned int esc8:1;          /* Tx expects 8th bit to be escaped */
  unsigned int escctl:1;        /* Tx expects ctl chars to be escaped */
  unsigned int pad1:6;
}
ZSINITflags;

typedef struct
{
  unsigned int pad:1;
  unsigned int sparse:1;        /* Encoding for sparse file operations */
  unsigned int canvhdr:6;       /* Variable headers OK */

  unsigned int transport:8;     /* Transport options */

  unsigned int skip:1;          /* Skip file if not present at rx */
  unsigned int manage:7;        /* Management options */

  unsigned int conv:8;          /* Conversion options */
}
ZFILEflags;

typedef struct
{
  unsigned int pad:31;
  unsigned int ack:1;           /* Acknowledge, then do command */
}
ZCOMMANDflags;

typedef union
{
  UBYTE b[4];
  ULONG position;               /* File position (low byte first) */
  ZRQINITflags zrqinit;
  ZRINITflags zrinit;
  ZSINITflags zsinit;
  ZFILEflags zfile;
  ZCOMMANDflags zcommand;
}
HEADER;

/* ----- Conversion options, ZFILE frame ------------------------------- */

#define ZCBIN       1           /* Binary transfer - inhibit conversion */
#define ZCNL        2           /* Convert NL to local eol convention */
#define ZCRESUM     3           /* Resume interrupted file transfer */

/* ----- Management options, ZFILE frame ------------------------------- */

#define ZMNEWL      1           /* Transfer if source newer or longer */
#define ZMCRC       2           /* Transfer if different file CRC or length */
#define ZMAPND      3           /* Append contents to existing file (if any) */
#define ZMCLOB      4           /* Replace existing file */
#define ZMNEW       5           /* Transfer if source newer */
#define ZMDIFF      6           /* Transfer if dates or lengths different */
#define ZMPROT      7           /* Protect destination file */

/* ----- Transport options, ZFILE frame -------------------------------- */

#define ZTLZW       1           /* Lempel-Ziv compression */
#define ZTRLE       3           /* Run Length encoding */

/* ----- Other constants ----------------------------------------------- */

#define ZATTNLEN    32          /* Max length of attention string */
#define ZMAXSPLEN   1024        /* Max subpacket length */

/* ----- Return values (internal) -------------------------------------- */

#define GOTOR   0x0100
#define GOTCRCE (GOTOR | ZCRCE) /* ZDLE-ZCRCE received */
#define GOTCRCG (GOTOR | ZCRCG) /* ZDLE-ZCRCG received */
#define GOTCRCQ (GOTOR | ZCRCQ) /* ZDLE-ZCRCQ received */
#define GOTCRCW (GOTOR | ZCRCW) /* ZDLE-ZCRCW received */

/* ----- Globals ------------------------------------------------------- */

UBYTE ZAutoReceiveString[] =
{
  ZPAD, ZDLE, ZHEX,
  '0', '0',                     /* Type: ZRQINIT */
  '0', '0', '0', '0', '0', '0', '0', '0',
  '0', '0', '0', '0',           /* CRC1 CRC2 */
  CR,
  0
};

UBYTE ZLastRx[] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define GARBAGE 1400+2400       /* Garbage count before header */

STATIC jmp_buf CancelEnv;       /* Long jump if cancel or abort */

STATIC BOOL Zctlesc;            /* TRUE: escape all control characters */
STATIC UBYTE *TxBuffer;         /* Transmit buffer */
STATIC UBYTE *TxPtr;            /* Pointer into transmit buffer */

/* Shared by transmit and receive */
STATIC BPTR FileHandle;         /* File handle */
STATIC UBYTE FileName[MAX_FILENAME_LENGTH];     /* Current file name */
STATIC ULONG Mark;              /* Current file position */
STATIC ULONG Length;            /* Length of file */
STATIC ULONG StartPos;          /* Start file position */
STATIC ULONG StartupTime;       /* Start time */
STATIC ULONG ZBytesOut, ZBytesIn; /* Number of bytes sent/received */
STATIC UBYTE *Buffer;           /* Block buffer + attention string + ... */
STATIC ULONG LastBPS;           /* Throughput (bytes/second) */

#define SERIALBUFFERSIZE (2 * ZPacket)
STATIC UBYTE *SerialBuffer;
STATIC UBYTE *SerialIndex;
STATIC LONG SerialCache;

/* Transmit only */
STATIC ULONG Txwindow;          /* Size of the transmitted window */
STATIC ULONG Txwspac;           /* Spacing between ZCRCQ requests */
STATIC UWORD Rxbuflen;          /* Receiver's max buffer length */
STATIC ULONG Lastsync;          /* Last offset to which we got a ZRPOS */
STATIC ULONG Lrxpos;            /* Receiver's last reported offset */
STATIC WORD BeenHereB4;         /* How many times ZRPOS same place */

/* Receive only */
STATIC UBYTE TryZhdrType;       /* Header type to send */
STATIC UWORD Count;             /* Bytes in block buffer */

#define Attn (Buffer+ZMAXSPLEN) /* Sender's attention string */
STATIC UWORD AttnLen;           /* Bytes in attention string */

#define BUFFERSIZE (ZMAXSPLEN+ZATTNLEN)
STATIC BOOL ConvertNL;          /* Convert newline to local eol */

STATIC UBYTE zc;
STATIC BPTR ReceiveDir;
STATIC BOOL IsBlockMapped;
STATIC LONG errors;
STATIC LayoutHandle *handle;
STATIC struct List *MessageList;
STATIC LONG MessageListLen;
STATIC BOOL Transmitting;
STATIC ULONG BytesThisBatch,FilesThisBatch;

/* ===== LOCAL PROTOTYPES=============================================== */
STATIC ULONG Unix2AmigaMode(ULONG Unix);
STATIC ULONG Amiga2UnixMode(ULONG Amiga);
STATIC LONG MapSerialError(LONG err);
STATIC VOID CloseUI(LayoutHandle *handle, LONG err);
STATIC VOID AddMessage(STRPTR Message, ...);
STATIC VOID ResetUI(LayoutHandle *handle);
STATIC LayoutHandle *OpenUI(STRPTR Title);
STATIC ULONG CalculateBPS(VOID);
STATIC VOID UpdateError(LONG error);
STATIC VOID NameProgress(STRPTR name);
STATIC BOOL CheckCancel(VOID);
STATIC VOID UpdateRetries(LONG retries);
STATIC VOID UpdateBatch(ULONG number, ULONG numberLeft, ULONG bytes, ULONG bytesLeft);
STATIC VOID UpdateProgress(LONG bytes, LONG maximum);
STATIC VOID ToOctal(ULONG v, UBYTE *s);
STATIC ULONG GetTime(VOID);
STATIC BOOL Escape(VOID);
STATIC LONG NewTxBuffer(VOID);
STATIC VOID DisposeTxBuffer(VOID);
STATIC VOID InitTxBuffer(VOID);
STATIC VOID TransmitTxBuffer(VOID);
STATIC VOID SendString(UBYTE *s, UWORD n);
STATIC VOID SendCancel(VOID);
STATIC UBYTE SendByte(UBYTE c);
STATIC VOID SendAttn(UBYTE *s);
STATIC UWORD __inline Swap2(UWORD n);
STATIC ULONG __inline Swap4(ULONG n);
STATIC VOID AdjustHeader(UBYTE type, HEADER *p);
STATIC UBYTE PutHex(UBYTE c);
STATIC UBYTE LocalSendLine(UBYTE c);
STATIC VOID SendHexHeader(UBYTE type, HEADER *hdr);
STATIC VOID SendBinaryHeader(UBYTE type, HEADER *hdr);
STATIC VOID SendData(UBYTE *buf, UWORD length, UBYTE frameend);
STATIC WORD ReadByte(UBYTE *c, ULONG timeout);
STATIC WORD WaitZPAD(ULONG timeout, UWORD n);
STATIC WORD ReadByte2(UBYTE *c, ULONG timeout);
STATIC WORD GetHex(UBYTE *c);
STATIC WORD ZdleRead(UBYTE *c);
STATIC WORD ReceiveBinaryHeader(UBYTE *type, UBYTE *hdr);
STATIC WORD ReceiveHexHeader(UBYTE *type, UBYTE *hdr);
STATIC WORD GetHeader(UBYTE *type, HEADER *hdr);
STATIC WORD ReceiveData(UBYTE *buf, UWORD length, UWORD *count, UBYTE *framend);
STATIC VOID ConvertTime(BOOL toAmiga, ULONG *t);
STATIC LONG LocalReadBuffer(LONG *count, UBYTE *buf);
STATIC LONG LocalReadBufferAndConvert(LONG *count, UBYTE *buf, UBYTE *lastc);
STATIC VOID SetFilePosition(ULONG position);
STATIC WORD GetInSync(BOOL flag, UBYTE *type);
STATIC WORD SendFileData(VOID);
STATIC WORD Invitation(VOID);
STATIC WORD SessionStartup(VOID);
STATIC WORD SendFileInfo(UBYTE *name, ULONG size, BOOL text, ULONG modif, ULONG mode, ULONG *startpos, ULONG TotalFiles, ULONG TotalBytes);
STATIC VOID SessionCleanup(VOID);
STATIC ULONG GetFreeSpace(VOID);
STATIC VOID AckOverAndOut(VOID);
STATIC WORD TryZReceive(HEADER *filehdr);
STATIC LONG ProcedeHeader(HEADER *filehdr, STRPTR name, ULONG *length, ULONG *modif, ULONG *mode, ULONG *filesleft, ULONG *bytesleft);
STATIC LONG WriteFile(VOID);
STATIC VOID CleanupFile(STRPTR name, ULONG modif, ULONG mode);
STATIC WORD ReceiveFile(HEADER *filehdr);
STATIC WORD ReceiveFiles(VOID);

/* ===== FILE MODE HANDLING ============================================ */
#define S_IRWXU 0000700         /* RWX mask for owner */
#define S_IRUSR 0000400         /* R for owner */
#define S_IWUSR 0000200         /* W for owner */
#define S_IXUSR 0000100         /* X for owner */

#define S_IRWXG 0000070         /* RWX mask for group */
#define S_IRGRP 0000040         /* R for group */
#define S_IWGRP 0000020         /* W for group */
#define S_IXGRP 0000010         /* X for group */

#define S_IRWXO 0000007         /* RWX mask for other */
#define S_IROTH 0000004         /* R for other */
#define S_IWOTH 0000002         /* W for other */
#define S_IXOTH 0000001         /* X for other */

#define S_IFMT  0170000         /* type of file */
#define S_IFREG 0100000         /* regular */

STATIC ULONG
Unix2AmigaMode (ULONG Unix)
{
  if (Unix & S_IFMT)
  {
    ULONG Amiga;

    Amiga = 0;

    if (Unix & S_IXOTH)
      Amiga |= FIBF_OTR_EXECUTE;

    if (Unix & S_IWOTH)
      Amiga |= FIBF_OTR_WRITE | FIBF_OTR_DELETE;

    if (Unix & S_IROTH)
      Amiga |= FIBF_OTR_READ;


    if (Unix & S_IXGRP)
      Amiga |= FIBF_GRP_EXECUTE;

    if (Unix & S_IWGRP)
      Amiga |= FIBF_GRP_WRITE | FIBF_GRP_DELETE;

    if (Unix & S_IRGRP)
      Amiga |= FIBF_GRP_READ;


    if (Unix & S_IXUSR)
      Amiga |= FIBF_EXECUTE;

    if (Unix & S_IWUSR)
      Amiga |= FIBF_WRITE | FIBF_DELETE;

    if (Unix & S_IRUSR)
      Amiga |= FIBF_READ;

    return (Amiga ^ (FIBF_EXECUTE | FIBF_WRITE | FIBF_DELETE | FIBF_READ));
  }
  else
    return (0);
}

STATIC ULONG
Amiga2UnixMode (ULONG Amiga)
{
  ULONG Unix;

  Amiga ^= FIBF_EXECUTE | FIBF_WRITE | FIBF_DELETE | FIBF_READ;

  Unix = S_IFREG;

  if (Amiga & FIBF_OTR_EXECUTE)
    Unix |= S_IXOTH;

  if (Amiga & (FIBF_OTR_WRITE | FIBF_OTR_DELETE))
    Unix |= S_IWOTH;

  if (Amiga & FIBF_OTR_READ)
    Unix |= S_IROTH;


  if (Amiga & FIBF_GRP_EXECUTE)
    Unix |= S_IXGRP;

  if (Amiga & (FIBF_GRP_WRITE | FIBF_GRP_DELETE))
    Unix |= S_IWGRP;

  if (Amiga & FIBF_GRP_READ)
    Unix |= S_IRGRP;


  if (Amiga & FIBF_EXECUTE)
    Unix |= S_IXUSR;

  if (Amiga & (FIBF_WRITE | FIBF_DELETE))
    Unix |= S_IWUSR;

  if (Amiga & FIBF_READ)
    Unix |= S_IRUSR;

  return (Unix);
}

/* ===== MISC ========================================================== */

STATIC LONG
MapSerialError (LONG err)
{
  switch (err)
  {
  case SerErr_LineErr:

    err = LINE_ERROR;
    break;

  case SerErr_ParityErr:

    err = PARITY_ERROR;
    break;

  case SerErr_TimerErr:

    err = TIMER_ERROR;
    break;

  case SerErr_BufOverflow:

    err = BUFFER_OVERFLOW;
    break;

  case SerErr_NoDSR:

    err = NO_DSR;
    break;

  case SerErr_DetectedBreak:

    err = BREAK_DETECTED;
    break;

  default:

    err = 0;
    break;
  }

  return (err);
}

STATIC VOID
CloseUI (LayoutHandle *handle, LONG err)
{
  if (err > 0)
  {
    LT_ShowWindow(handle,TRUE);
    DisplayBeep (handle->Window->WScreen);

    do
      WaitPort(handle->Window->UserPort);
    while(!CheckCancel());
  }

  LT_DeleteHandle(handle);

  DeleteList(MessageList);
  MessageList = NULL;
}

STATIC VOID
AddMessage(STRPTR Message,...)
{
	struct Node *Node;
	va_list VarArgs;
	ULONG Size;

	va_start(VarArgs,Message);
	Size = GetFormatLength(Message,VarArgs);

	LT_SetAttributes(handle,
		GTLV_Labels,~0,
	TAG_DONE);

	if(MessageListLen == 50)
	{
		FreeVecPooled(RemHead(MessageList));
		MessageListLen--;
	}

	if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + Count,MEMF_ANY))
	{
		VSPrintf(Node->ln_Name = (char *)(Node + 1),Message,VarArgs);
		AddTail(MessageList,Node);
		MessageListLen++;
	}

	va_end(VarArgs);

	LT_SetAttributes(handle,
		GTLV_Labels,MessageList,
	TAG_DONE);
}

STATIC VOID
ResetUI(LayoutHandle *handle)
{
	STRPTR nothing;
	LONG i;

	nothing = "-";

	for(i = 0 ; i < 4 ; i++)
	{
	  LT_SetAttributes(handle,GAD_NameBox,
	  	LABX_Index,		i,
	  	LABX_Text,		nothing,
	  TAG_DONE);

	  LT_SetAttributes(handle,GAD_BatchBox,
	  	LABX_Index,		i,
	  	LABX_Text,		nothing,
	  TAG_DONE);

	  LT_SetAttributes(handle,GAD_TransferBox,
	  	LABX_Index,		i,
	  	LABX_Text,		nothing,
	  TAG_DONE);
	}
}

STATIC LayoutHandle *
OpenUI (STRPTR Title)
{
	LayoutHandle *handle;

	if(!(MessageList = CreateList()))
		return(NULL);

	MessageListLen = 0;

	if(handle = LT_CreateHandleTagList(Window->WScreen,NULL))
	{
		LT_New(handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				STATIC STRPTR BoxLabels1[] =
				{
					"Name",
					"Size",
					"Completion",
					NULL
				};

				STATIC STRPTR BoxLabels2[] =
				{
					"Batch count",
					"Batch size",
					"Batch completion",
					NULL
				};

				STATIC STRPTR BoxLabels3[] =
				{
					"Number of errors",
					"Number of retries",
					"Bytes/second",
					NULL
				};

				LT_New(handle,
					LA_Type,		BOX_KIND,
					LA_ID,			GAD_NameBox,
					LA_Chars,		40,
					LABX_Labels,	BoxLabels1,
				TAG_DONE);

				LT_New(handle,
					LA_Type,		BOX_KIND,
					LA_ID,			GAD_BatchBox,
					LABX_Labels,	BoxLabels2,
				TAG_DONE);

				LT_New(handle,
					LA_Type,		LISTVIEW_KIND,
					LA_ID,			GAD_MessageList,
					LA_NoKey,		TRUE,
					LA_LabelText,	"Information",
					LA_LabelPlace,	PLACE_Left,
					LALV_MaxGrowY,	5,
					LALV_Lines,		2,
					GTLV_ReadOnly,	TRUE,
				TAG_DONE);

				LT_New(handle,
					LA_Type,		BOX_KIND,
					LA_ID,			GAD_TransferBox,
					LABX_Labels,	BoxLabels3,
				TAG_DONE);

				LT_New(handle,
					LA_Type,		GAUGE_KIND,
					LA_ID,			GAD_FileProgress,
					LA_LabelText,	"File progress",
				TAG_DONE);

				LT_New(handle,
					LA_Type,		GAUGE_KIND,
					LA_ID,			GAD_BatchProgress,
					LA_LabelText,	"Batch progress",
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_EndGroup(handle);
			}

			LT_New(handle,
				LA_Type,		VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullWidth,	TRUE,
				TAG_DONE);

				LT_EndGroup(handle);
			}

			LT_New(handle,
				LA_Type,		HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{

				LT_New(handle,
					LA_Type,		BLANK_KIND,
				TAG_DONE);

				LT_New(handle,
					LA_Type,		BUTTON_KIND,
					LA_ID,			GAD_Abort,
					LA_LabelText,	"Abort",
					LABT_ExtraFat,	TRUE,
					LABT_EscKey,	TRUE,
				TAG_DONE);

				LT_EndGroup(handle);
			}

			LT_EndGroup(handle);
		}

		if(LT_Build(handle,
			LAWN_TitleText,	Title,
			LAWN_Zoom,		TRUE,
			LAWN_Parent,	Window,
			WA_DepthGadget,	TRUE,
			WA_DragBar,		TRUE,
			WA_RMBTrap,		TRUE,
			WA_Activate,	TRUE,
		TAG_DONE))
		{
			ResetUI(handle);
			return(handle);
		}

		LT_DeleteHandle(handle);
	}

    DeleteList(MessageList);
    MessageList = NULL;

	return(NULL);
}

STATIC ULONG
CalculateBPS()
{
	ULONG Now;

	Now = GetTime();

	if(Now > StartupTime)
	{
		ULONG Bytes;

		Bytes = Transmitting ? ZBytesOut : ZBytesIn;
		Now -= StartupTime;

		LastBPS = Bytes / Now;
	}

	return(LastBPS);
}

STATIC VOID
UpdateError (LONG error)
{
  if (error > 0)
  {
    UBYTE LocalBuffer[256];
    STRPTR Message;

    switch (error)
    {
    case TIMEOUT:
      Message = "Timeout";
      break;
    case CANCEL:
      Message = "Local abort";
      break;
    case ABORT:
      Message = "Remote abort";
      break;
    case ERROR:
      Message = "Transmission error";
      break;
    case NO_CARRIER:
      Message = "Carrier lost";
      break;
    case LINE_ERROR:
      Message = "Hardware data overrun";
      break;
    case PARITY_ERROR:
      Message = "Parity error";
      break;
    case TIMER_ERROR:
      Message = "Timer error";
      break;
    case BUFFER_OVERFLOW:
      Message = "Buffer overflow";
      break;
    case NO_DSR:
      Message = "No DSR signal detected";
      break;
    case BREAK_DETECTED:
      Message = "Break signal detected";
      break;
    default:
      Fault (error, NULL, Message = LocalBuffer, sizeof (LocalBuffer));
      break;
    }

    AddMessage(Message);
  }
}

STATIC VOID
NameProgress (STRPTR name)
{
	LT_SetAttributes(handle,GAD_NameBox,
		LABX_Index,	NAMEBOX_Name,
		LABX_Text,	name,
	TAG_DONE);
}

STATIC BOOL
CheckCancel ()
{
  struct IntuiMessage *Message;
  ULONG MsgClass;
  UWORD MsgGadgetID;
  BOOL Result;

  Result = FALSE;

  while (Message = LT_GetIMsg (handle))
  {
    MsgClass = Message->Class;

    if(MsgClass == IDCMP_GADGETUP)
      MsgGadgetID = ((struct Gadget *)Message->IAddress)->GadgetID;

    LT_ReplyIMsg(Message);

    if(MsgClass == IDCMP_GADGETUP && MsgGadgetID == GAD_Abort)
      Result = TRUE;
  }

  return (Result);
}

STATIC VOID
UpdateRetries(LONG retries)
{
  UBYTE LocalBuffer[256];
  STRPTR String;

  if(retries > 0)
    LimitedSPrintf(sizeof(LocalBuffer),String = LocalBuffer,"%lD of %lD",retries,ZRetries);
  else
    String = "-";

  LT_SetAttributes(handle,GAD_TransferBox,
  	LABX_Index,		TRANSFERBOX_Retries,
  	LABX_Text,		String,
  TAG_DONE);
}

STATIC VOID
UpdateBatch(ULONG number,ULONG numberLeft,ULONG bytes,ULONG bytesLeft)
{
  if(numberLeft > 0 && bytesLeft > 0)
  {
    UBYTE LocalBuffer[256];
    ULONG Percent;
    ULONG bps;

    LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%lD of %lD",number + 1,number + numberLeft);

    LT_SetAttributes(handle,GAD_BatchBox,
  	  LABX_Index,		BATCHBOX_Count,
  	  LABX_Text,		LocalBuffer,
    TAG_DONE);

    LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%lD of %lD",bytes,bytes + bytesLeft);

    LT_SetAttributes(handle,GAD_BatchBox,
  	  LABX_Index,		BATCHBOX_Size,
  	  LABX_Text,		LocalBuffer,
    TAG_DONE);

    if((bps = CalculateBPS()) > 0)
    {
      ULONG time;
      STRPTR String;

      time = bytesLeft / bps;

      if(time < 1)
        String = "< 00:00:01 h";
      else
        LimitedSPrintf(sizeof(LocalBuffer),String = LocalBuffer,"  %2ld:%02ld:%02ld h",time / (60 * 60),(time / 60) % 60,time % 60);

      LT_SetAttributes(handle,GAD_BatchBox,
  		  LABX_Index,		BATCHBOX_Completion,
  		  LABX_Text,		String,
      TAG_DONE);

      LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%lD bytes/s",bps);

      LT_SetAttributes(handle,GAD_TransferBox,
  		  LABX_Index,		TRANSFERBOX_CPS,
  		  LABX_Text,		LocalBuffer,
      TAG_DONE);
    }

    Percent = (100 * number) / (number + numberLeft);

    if(Percent > 100)
      Percent = 100;

    LT_SetAttributes(handle,GAD_BatchProgress,
      GA_Disabled,  FALSE,
      LAGA_Percent, Percent,
    TAG_DONE);
  }
  else
  {
    LT_SetAttributes(handle,GAD_BatchProgress,
      GA_Disabled,TRUE,
    TAG_DONE);
  }
}

STATIC VOID
UpdateProgress (LONG bytes, LONG maximum)
{
  UBYTE LocalBuffer[256];
  ULONG Percent;

  if(bytes < maximum)
  {
    ULONG bps;

    if((bps = CalculateBPS()) > 0)
    {
      ULONG time,delta;
      STRPTR String;

      delta = maximum - bytes;

      time = delta / bps;

      if(time < 1)
        String = "< 00:00:01 h";
      else
        LimitedSPrintf(sizeof(LocalBuffer),String = LocalBuffer,"  %2ld:%02ld:%02ld h",time / (60 * 60),(time / 60) % 60,time % 60);

      LT_SetAttributes(handle,GAD_NameBox,
  		  LABX_Index,		NAMEBOX_Completion,
  		  LABX_Text,		String,
      TAG_DONE);

      LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%lD bytes/s",bps);

      LT_SetAttributes(handle,GAD_TransferBox,
  		  LABX_Index,		TRANSFERBOX_CPS,
  		  LABX_Text,		LocalBuffer,
      TAG_DONE);
    }
  }

  LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%lD of %lD",bytes,maximum);

  LT_SetAttributes(handle,GAD_NameBox,
  	LABX_Index,		NAMEBOX_Size,
  	LABX_Text,		LocalBuffer,
  TAG_DONE);

  if(maximum > 0)
    Percent = (100 * bytes) / maximum;
  else
    Percent = 0;

  LT_SetAttributes(handle,GAD_FileProgress,
  	LAGA_Percent,	Percent,
  TAG_DONE);
}

/* ===== CRC =========================================================== */
/* crctab calculated by Mark G. Mendel, Network Systems Corporation */
STATIC UWORD crctab[256] =
{
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

/*
 * updcrc macro derived from article Copyright (C) 1986 Stephen Satchell.
 *  NOTE: First argument must be in range 0 to 255.
 *        Second argument is referenced twice.
 *
 * Programmers may incorporate any or all code into their programs,
 * giving proper credit within the source. Publication of the
 * source routines is permitted so long as proper credit is given
 * to Stephen Satchell, Satchell Evaluations and Chuck Forsberg,
 * Omen Technology.
 */

#define updcrc(cp, crc) ( crctab[crc >> 8] ^ (((crc & 255) << 8) | (cp)))

/* ===== COMMON FUNCTIONS ============================================== */

STATIC VOID
ToOctal (ULONG v, UBYTE * s)
{
  UBYTE t[20], *u;

  u = t;

  do
  {
    *u++ = '0' + (v & 7);

    v >>= 3;
  }
  while (v != 0);

  while (u > t)
    *s++ = *(--u);

  *s = 0;
}

STATIC ULONG
GetTime ()
{
  struct timeval Now;

  GetSysTime (&Now);

  return (Now.tv_secs);
}

/* ----- Should we escape control characters? -------------------------- */

STATIC BOOL
Escape ()
{
  return ((BOOL) (ZEscapeCtl || Config->SerialConfig->BitsPerChar < 8));
}

/* ----- Allocate memory for transmit buffer --------------------------- */

STATIC LONG
NewTxBuffer (VOID)
{
  DB (kprintf ("»txbuffer size is %ld\«\n", block ? 2 * (ZMAXSPLEN + 6) : 32));

  TxBuffer = (UBYTE *) AllocVecPooled (3 * (ZMAXSPLEN + 6),MEMF_ANY);

  SerialBuffer = (UBYTE *) AllocVecPooled (SERIALBUFFERSIZE, MEMF_ANY);

  if (TxBuffer && SerialBuffer)
  {
    SerialIndex = SerialBuffer;
    SerialCache = 0;

    return (0);
  }
  else
    return (ERROR_NO_FREE_STORE);
}

/* ----- Dispose of transmit buffer ------------------------------------ */

STATIC VOID
DisposeTxBuffer ()
{
  FreeVecPooled (TxBuffer);
  TxBuffer = NULL;

  FreeVecPooled (SerialBuffer);
  SerialBuffer = NULL;
}

/* ----- Prepare new transmission -------------------------------------- */

STATIC VOID
InitTxBuffer ()
{
  TxPtr = TxBuffer;
}

/* ----- Start transmission, don't wait -------------------------------- */

STATIC VOID
TransmitTxBuffer ()
{
  if (TxPtr > TxBuffer)
  {
    ULONG Size;

    Size = (ULONG) TxPtr - (ULONG) TxBuffer;

    DoSerialWrite (TxBuffer, Size);
  }
}

/* ----- Send more bytes ----------------------------------------------- */

STATIC VOID
SendString (UBYTE * s, UWORD n)
{
  InitTxBuffer ();
  memcpy (TxPtr, s, n);
  TxPtr += n;
  TransmitTxBuffer ();
}

/* ----- Send cancel string -------------------------------------------- */

STATIC VOID
SendCancel ()
{
  STATIC UBYTE cs[] =
  {
    CAN, CAN, CAN, CAN, CAN, CAN, CAN, CAN,
    BS, BS, BS, BS, BS, BS, BS, BS, BS, BS
  };

  DoSerialWrite (cs, sizeof (cs));
}

/* ----- Send a byte --------------------------------------------------- */

STATIC UBYTE
SendByte (UBYTE c)
{
  *TxPtr++ = c;
  return c;
}

/* ----- Send attention string ----------------------------------------- */

STATIC VOID
SendAttn (UBYTE * s)
{
  while (*s)
  {
    switch (*s)
    {
    case 0xDD:                  /* Send break signal */

      DoSerialBreak ();
      break;

    case 0xDE:                  /* Sleep 1 second */

      DelayTime (1, 0);
      break;

    default:

      DoSerialWrite (s, 1);
      break;
    }

    ++s;
  }
}

/* ----- Swap 2 bytes in WORD ----------------------------------------- */

STATIC UWORD __inline
Swap2 (UWORD n)
{
  return ((UWORD) (((n & 0xFF) << 8) | (n >> 8)));
}

/* ----- Swap 4 bytes in long ------------------------------------------ */

STATIC ULONG __inline
Swap4 (ULONG n)
{
  return (((ULONG) Swap2 (n & 0xffff) << 16) | Swap2 (n >> 16));
}

/* ----- Adjust header ------------------------------------------------- */

STATIC VOID
AdjustHeader (UBYTE type, HEADER * p)
{
  switch (type)
  {
  case ZACK:
  case ZRPOS:
  case ZDATA:
  case ZEOF:
  case ZCRC:
  case ZCHALLENGE:
  case ZFREECNT:
  case ZCOMPL:

    p->position = Swap4 (p->position);
    break;

  case ZRINIT:

    p->zrinit.bufsize = Swap2 (p->zrinit.bufsize);
    break;
  }
}

/* ----- Send a byte as two hex digits --------------------------------- */

STATIC UBYTE
PutHex (UBYTE c)
{
  STATIC UBYTE digits[] =
  {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
  };

  SendByte (digits[(c >> 4) & 0xF]);
  SendByte (digits[c & 0xF]);

  return c;
}

/* ----- Send a byte with ZMODEM escape sequence encoding -------------- */

STATIC UBYTE
LocalSendLine (UBYTE c)
{
  UBYTE b = c;

  STATIC UBYTE lastsent = 0;    /* Last byte sent */

  if (c & 0x60)                 /* Non control character */
    SendByte (lastsent = c);
  else
  {
    switch (c)
    {
    case ZDLE:

      SendByte (ZDLE);
      SendByte (lastsent = c ^ 0x40);
      break;

    case CR:                    /* Escape CR followed by @ (Telenet net escape) */
    case 0x80 | CR:

      if (!Zctlesc && (lastsent & 0x7F) != '@')
      {
        SendByte (lastsent = c);
        break;
      }

      /* Fall thru */

    case DLE:
    case 0x80 | DLE:
    case XON:
    case 0x80 | XON:
    case XOFF:
    case 0x80 | XOFF:

      SendByte (ZDLE);
      c ^= 0x40;
      SendByte (lastsent = c);
      break;

    default:

      if (Zctlesc && !(c & 0x60))
      {
        SendByte (ZDLE);
        c ^= 0x40;
      }

      SendByte (lastsent = c);
    }
  }
  return b;
}

/* ----- Send ZMODEM HEX header ---------------------------------------- */

STATIC VOID
SendHexHeader (UBYTE type, HEADER * hdr)
{
  WORD i;
  UWORD crc;

  AdjustHeader (type, hdr);

  InitTxBuffer ();
  SendByte (ZPAD);              /* 1 */
  SendByte (ZPAD);              /* 1 */
  SendByte (ZDLE);              /* 1 */
  SendByte (ZHEX);              /* 1 */

  crc = updcrc (PutHex (type), 0);

  for (i = 0; i < 4; ++i)
    crc = updcrc (PutHex (hdr->b[i]), crc);     /* 8 */

  crc = updcrc (0, updcrc (0, crc));

  PutHex (crc >> 8);            /* 2 */
  PutHex (crc & 0xFF);          /* 2 */

  SendByte (CR);                /* 1 */
  SendByte (LF);                /* 1 */

  if (type != ZFIN && type != ZACK)
    SendByte (XON);             /* 1 */

  TransmitTxBuffer ();          /* Worst case:  19 bytes */
}

/* ----- Send ZMODEM binary header ------------------------------------- */

STATIC VOID
SendBinaryHeader (UBYTE type, HEADER * hdr)
{
  WORD i;
  UWORD crc;

  AdjustHeader (type, hdr);

  InitTxBuffer ();
  SendByte (ZPAD);              /* 1 */
  SendByte (ZDLE);              /* 1 */
  SendByte (ZBIN);              /* 1 */

  crc = updcrc (LocalSendLine (type), 0);

  for (i = 0; i < 4; ++i)
    crc = updcrc (LocalSendLine (hdr->b[i]), crc);      /* 8 */

  crc = updcrc (0, updcrc (0, crc));
  LocalSendLine (crc >> 8);     /* 2 */
  LocalSendLine (crc & 0xFF);   /* 2 */

  TransmitTxBuffer ();          /* Worst case:  15 bytes */
}

/* ----- Send binary array with ending ZDLE sequence ------------------- */

STATIC VOID
SendData (UBYTE * buf, UWORD length, UBYTE frameend)
{
  UWORD crc;

  InitTxBuffer ();
  for (crc = 0; length; ++buf, --length)
  {
    LocalSendLine (*buf);
    crc = updcrc (*buf, crc);
  }
  SendByte (ZDLE);
  SendByte (frameend);
  crc = updcrc (frameend, crc);
  crc = updcrc (0, updcrc (0, crc));
  LocalSendLine (crc >> 8);
  LocalSendLine (crc & 0xFF);

  if (frameend == ZCRCW)
    SendByte (XON);

  TransmitTxBuffer ();
}

/* ----- Read character with timeout ----------------------------------- */

STATIC WORD
ReadByte (UBYTE * c, ULONG timeout)
{
  STATIC WORD abort;            /* CAN counter */

  LONG err;

  DB (kprintf ("» ReadByte timeout=%ld\n", timeout));

  if (SerialCache > 0)
  {
    if (SetSignal (0, (1L << handle->Window->UserPort->mp_SigBit)) & (1L << handle->Window->UserPort->mp_SigBit))
    {
      if (CheckCancel ())
        longjmp (CancelEnv, CANCEL);
    }

    *c = *SerialIndex++;

    if (--SerialCache == 0)
    {
      DB (kprintf ("» Cache exhausted\n"));

      SerialIndex = SerialBuffer;

      if (CheckSerialRead ())
      {
        err = WaitSerialRead ();

        if (err)
        {
          if (err = MapSerialError (err))
            longjmp (CancelEnv, err);
        }

        SerialBuffer[0] = zc;

        if (SerialCache = GetSerialWaiting ())
        {
          if (SerialCache > SERIALBUFFERSIZE - 1)
            SerialCache = SERIALBUFFERSIZE - 1;

          err = DoSerialRead (&SerialBuffer[1], SerialCache);

          if (err)
          {
            if (err = MapSerialError (err))
              longjmp (CancelEnv, err);
          }
        }

        SerialCache++;

        StartSerialRead (&zc, 1);
      }

      DB (kprintf ("» Cache size = %ld\n", SerialCache));
    }
  }
  else
  {
    ULONG mask;

    if (!timeout)
      return (NOTHING);

    StartTime (timeout,0);

    for (;;)
    {
      mask = Wait (SIG_TIMER | SIG_SERIAL | (1L << handle->Window->UserPort->mp_SigBit));

      if (mask & (1L << handle->Window->UserPort->mp_SigBit))
      {
        if (CheckCancel ())
        {
          StopTime ();

          longjmp (CancelEnv, CANCEL);
        }
      }

      if (mask & SIG_SERIAL)
      {
        err = WaitSerialRead ();

        StopTime ();

        if (err)
        {
          if (err = MapSerialError (err))
            longjmp (CancelEnv, err);
        }

        *c = zc;

        if (SerialCache = GetSerialWaiting ())
        {
          if (SerialCache > SERIALBUFFERSIZE)
            SerialCache = SERIALBUFFERSIZE;

          err = DoSerialRead (SerialBuffer, SerialCache);

          if (err)
          {
            if (err = MapSerialError (err))
              longjmp (CancelEnv, err);
          }
        }

        StartSerialRead (&zc, 1);

        DB (kprintf ("» Cache size = %ld\n", SerialCache));

        break;
      }

      if (mask & SIG_TIMER)
      {
        WaitTime ();

#ifdef __AROS__
        if (GetSerialStatus () & SER_STATF_COMCD)
#else
        if (GetSerialStatus () & CIAF_COMCD)
#endif
          longjmp (CancelEnv, NO_CARRIER);
        else
          return (TIMEOUT);
      }
    }
  }

  if (*c == CAN)
  {
    if (++abort >= 5)
      longjmp (CancelEnv, ABORT);
  }
  else
    abort = 0;

  return FINE;
}

/* ----- Wait for ZPAD character --------------------------------------- */

STATIC WORD
WaitZPAD (
           ULONG timeout,       /* Timeout in ticks */
           UWORD n)             /* Max bytes before start of frame */
{
  UBYTE c;

  for (;;)
  {
    switch (ReadByte (&c, timeout))
    {
    case FINE:

      if ((c & 0x7F) == ZPAD)
        return FINE;

      if (--n == 0)
        return ERROR;

      break;

    case TIMEOUT:

      return TIMEOUT;
    }
  }
}

/* ----- Read character with timemout, eat parity, XON and XOFF -------- */

STATIC WORD
ReadByte2 (
            UBYTE * c,          /* Character (result) */
            ULONG timeout)      /* Timeout in ticks */
{
  for (;;)
  {
    if (ReadByte (c, timeout))
      return TIMEOUT;

    switch (*c &= 0x7F)
    {
    case XON:
    case XOFF:

      break;

    default:

      if (Zctlesc && !(*c & 0x60))
        break;

    case CR:
    case LF:
    case ZDLE:

      return FINE;
    }
  }
}

/* ----- Decode two lower case hex digits into an 8 bit byte value ----- */

STATIC WORD
GetHex (UBYTE * c)
{
  WORD i;
  UBYTE n;

  *c = 0;

  for (i = 0; i < 2; ++i)
  {
    *c <<= 4;

    if (ReadByte2 (&n, ZTimeout))
      return TIMEOUT;

    if (n >= '0' && n <= '9')
      n -= '0';
    else
    {
      if (n >= 'a' && n <= 'f')
        n -= 'a' - 10;
      else
        return ERROR;
    }

    *c += n;
  }

  return FINE;
}

/* ----- Read a byte, checking for ZMODEM escape encoding -------------- */

STATIC WORD
ZdleRead (UBYTE * c)
{
  do
  {
    if (ReadByte (c, ZTimeout))
      return TIMEOUT;

    if (*c & 0x60)              /* Non-control character */
      return FINE;

    switch (*c)                 /* Control character */
    {
    case ZDLE:
    case XON:
    case 0x80 | XON:
    case XOFF:
    case 0x80 | XOFF:

      break;                    /* Ignore XON/XOFF */

    default:

      if (!Zctlesc)
        return FINE;

      break;                    /* Ignore ctrl if escaping */
    }
  }
  while (*c != ZDLE);

  /* Previous character was ZDLE */

  for (;;)
  {
    if (ReadByte (c, ZTimeout))
      return TIMEOUT;

    switch (*c)
    {
    case ZCRCE:
    case ZCRCG:
    case ZCRCQ:
    case ZCRCW:

      return (WORD) (*c | GOTOR);       /* Frame end */

    case ZRUB0:

      *c = DEL;
      return FINE;

    case ZRUB1:

      *c = 0x80 | DEL;
      return FINE;

    case XON:
    case 0x80 | XON:
    case XOFF:
    case 0x80 | XOFF:

      break;                    /* Ignore XON/XOFF */

    default:
      if (Zctlesc && !(*c & 0x60))
        break;                  /* Ignore ctrl if escaping */

      if ((*c & 0x60) != 0x40)  /* Must be -10- ---- */
        return ERROR;

      *c ^= 0x40;               /* Invert bit 6 */

      return FINE;
    }
  }
}

/* ----- Receive a binary style header (type and position) ------------- */

STATIC WORD
ReceiveBinaryHeader (UBYTE * type,UBYTE * hdr)
{
  WORD n, err;
  UWORD crc;
  UBYTE c;

  /* Header type */

  if (err = ZdleRead (type))
    return err;                 /* TIMEOUT, ERROR, GOTxxxx */
  crc = updcrc (*type, 0);

  /* header info (4 bytes) */
  for (n = 0; n < 4; ++n, ++hdr)
  {
    if (err = ZdleRead (hdr))
      return err;               /* TIMEOUT, ERROR, GOTxxxx */
    crc = updcrc (*hdr, crc);
  }

  /* CRC (2 bytes) */
  if (err = ZdleRead (&c))
    return err;                 /* TIMEOUT, ERROR, GOTxxxx */
  crc = updcrc (c, crc);
  if (err = ZdleRead (&c))
    return err;                 /* TIMEOUT, ERROR, GOTxxxx */
  if (crc = updcrc (c, crc))
    return ERROR;               /* Bad CRC */

  return FINE;
}

/* ----- Receive a hex style header (type and position) ---------------- */

STATIC WORD
ReceiveHexHeader (UBYTE * type,UBYTE * hdr)
{
  WORD n, err;
  UWORD crc;
  UBYTE c;

  DB (kprintf ("»ReceiveHexHeader«\n"));

  /* Header type */

  if (err = GetHex (type))
    return err;
  DB (kprintf ("»header type is %ld«\n", *type));
  crc = updcrc (*type, 0);

  /* Header info (4 bytes) */

  for (n = 0; n < 4; ++n, ++hdr)
  {
    if (err = GetHex (hdr))
      return err;
    crc = updcrc (*hdr, crc);
  }

  DB (kprintf ("»header info is %08lx, crc %02lx«\n", *(ULONG *) & hdr[-4], crc));

  /* CRC (2 bytes) */

  if (err = GetHex (&c))
    return err;
  DB (kprintf ("»%lx", c));
  crc = updcrc (c, crc);
  if (err = GetHex (&c))
    return err;
  DB (kprintf ("%lx«\n", c));
  if (crc = updcrc (c, crc))
  {
    DB (kprintf ("»CRC %02lx is wrong«\n", crc));
    return ERROR;               /* Bad CRC */
  }

  /* Throw away possible CR/LF */

  if (ReadByte (&c, 1) == FINE && (c & 0x7F) == CR)
    ReadByte (&c, 1);

  return FINE;
}

/* ----- Read a ZMODEM header (either binary or hex) ------------------- */

STATIC WORD
GetHeader (UBYTE * type,HEADER * hdr)
{
  WORD err;
  WORD n = GARBAGE;
  UBYTE c;

  DB (kprintf ("»GetHeader«\n"));

  /* Wait for ZPAD character */

  if (err = WaitZPAD (ZTimeout, GARBAGE))
  {
    DB (kprintf ("»didn't get ZPAD«\n"));
    goto done;
  }

  /* Just got ZPAD character, wait for ZDLE character */

  do
  {
    if (err = ReadByte2 (&c, ZTimeout))
    {
      DB (kprintf ("»didn't get ZDLE«\n"));
      goto done;
    }
    switch (c)
    {
    case ZDLE:                  /* This is what we want */
    case ZPAD:                  /* May be more than one ZPAD */
    case ZPAD | 0x80:
      break;
    again:
    default:                    /* Garbage */
      if (--n == 0)
      {
        err = ERROR;
        goto done;
      }
      break;
    }
  }
  while (c != ZDLE);

  /* Just got ZPAD-ZDLE sequence */

  if (err = ReadByte2 (&c, ZTimeout))
  {
    DB (kprintf ("»didn't get header type«\n"));
    goto done;
  }
  DB (kprintf ("»got header type %ld«\n", c));
  switch (c)
  {
  case ZBIN:                    /* It's a binary header */
    err = ReceiveBinaryHeader (type, hdr->b);
    break;
  case ZHEX:                    /* It's a hex header */
    err = ReceiveHexHeader (type, hdr->b);
    break;
  default:                      /* Garbage */
    goto again;
  }
  AdjustHeader (*type, hdr);

  /* ZPAD ZDLE ZBIN/ZHEX ... */

done:
  DB (kprintf ("»GetHeader done, err %ld«\n", err));
  return err;
}

/* ----- Receive array with ending ZDLE sequence and CRC --------------- */

STATIC WORD
ReceiveData (
              UBYTE * buf,      /* Buffer for data */
              UWORD length,     /* Max size of buffer */
              UWORD * count,    /* Number of data bytes received */
              UBYTE * framend)  /* Frame end character received */
{
  UWORD crc = 0;
  UBYTE *max = buf + length;
  WORD err;
  UBYTE c;

  *framend = 0xFF;
  while (buf <= max)
  {
    switch (err = ZdleRead (&c))
    {
    case FINE:                  /* Data byte */
      crc = updcrc (c, crc);

      if (ConvertNL)
      {
        if (c != CR)
          *buf++ = c;
      }
      else
        *buf++ = c;

      break;
    case GOTCRCE:               /* Frame end, CRC follows */
    case GOTCRCG:
    case GOTCRCQ:
    case GOTCRCW:
      *count = length - (max - buf);
      crc = updcrc (*framend = c, crc);
      if (err = ZdleRead (&c))
        goto done;
      crc = updcrc (c, crc);
      if (err = ZdleRead (&c))
        goto done;
      if (crc = updcrc (c, crc))
        err = ERROR;            /* Bad CRC */
      else
        err = FINE;
      goto done;
    default:                    /* TIMEOUT, ERROR, ... */
      *count = length - (max - buf);
      goto done;
    }
  }
  *count = length - (max - buf);
  err = ERROR;                  /* Data subpacket too LONG */

done:
  return err;
}

/* ----- Convert between Amiga time and ZModem time ---------------- */

STATIC VOID
ConvertTime (BOOL toAmiga, ULONG * t)
{
  const ULONG UTC_Offset = 252482400;

  if (toAmiga)
  {
    if (*t < UTC_Offset)
      *t = 0;
    else
      *t = *t - UTC_Offset;
  }
  else
    *t = *t + UTC_Offset;
}

/* ===== TRANSMIT ====================================================== */

/* ----- Fill buffer --------------------------------------------------- */

STATIC LONG
LocalReadBuffer (LONG * count,UBYTE * buf)
{
  *count = Read (FileHandle, buf, *count);

  return (*count == 0);
}

STATIC LONG
LocalReadBufferAndConvert (LONG * count,UBYTE * buf,UBYTE * lastc)
{
  LONG last, c;
  LONG i;

  last = *lastc;

  for (i = 0; i < *count; i++)
  {
    if ((c = FGetC (FileHandle)) < 0)
    {
      *count = i;
      *lastc = last;
      return (TRUE);
    }

    if (c == LF && last != CR)
    {
      UnGetC (FileHandle, LF);
      c = CR;
    }

    *buf++ = c;
    last = c;
  }

  *lastc = last;

  return (FALSE);
}

/* ----- Set file position --------------------------------------------- */

STATIC VOID
SetFilePosition (ULONG position)
{
  Seek (FileHandle, position, OFFSET_BEGINNING);
}

/* ----- Respond to receiver's complaint, get back in sync ------------- */

STATIC WORD
GetInSync (
            BOOL flag,          /* TRUE: ZACK always returns */
            UBYTE * type)
{
  WORD retry;
  WORD err;
  HEADER hdr;

  for (retry = 0; retry <= ZRetries; ++retry)
  {
    AddMessage("Synchronizing...");
    UpdateRetries(retry);

    switch (GetHeader (type, &hdr))
    {
    case FINE:
      switch (*type)
      {
      case ZRPOS:
        SetFilePosition (hdr.position);
        Lrxpos = Mark = hdr.position;
        if (Lastsync == hdr.position &&
            ++BeenHereB4 > 4 && Count > 32)
          Count /= 2;
        Lastsync = hdr.position;
        err = FINE;
        goto done;
      case ZACK:
        Lrxpos = hdr.position;
        if (flag || Mark == hdr.position)
        {
          err = FINE;
          goto done;
        }
        break;                  /* Ignore this ZACK */
      case ZCAN:
      case ZABORT:
      case ZFIN:
        goto error;
      case ZRINIT:
      case ZSKIP:
        err = FINE;
        goto done;
      }
      break;
    case TIMEOUT:
      goto error;
    }
    hdr.position = 0;
    SendHexHeader (ZNAK, &hdr);
  }
error:
  err = ERROR;
done:
  return err;
}

/* ----- Send the data in the file ------------------------------------- */

STATIC WORD
SendFileData ()
{
  WORD junkcount;               /* Counts garbage chars received */
  ULONG txwcnt;                 /* Counter used to space ACK requests */
  WORD eofseen;                 /* End of file seen (file read error) */
  UBYTE type;                   /* Header type */
  HEADER hdr;                   /* Header */
  WORD err;                     /* Error code */
  WORD newcnt;                  /* Controls receiver's buffer */
  LONG n;                       /* Transmit buffer length */
  UBYTE e;                      /* Frame end character */
  UBYTE c;
  UBYTE lastc;

  Lastsync = (StartPos = Lrxpos = Mark) - 1;
  junkcount = 0;
  BeenHereB4 = 0;
  lastc = 0;

somemore:
  if (FALSE)
  {

  waitack:
    junkcount = 0;
    err = GetInSync (FALSE, &type);
  gotack:
    if (err)
      return ERROR;
    switch (type)
    {
    case ZSKIP:
    case ZRINIT:
      return FINE;
    case ZACK:
    case ZRPOS:
      break;
    default:
      return ERROR;
    }

    /* Check reverse channel (but don't wait) */

    if (CheckCancel ())
      longjmp (CancelEnv, CANCEL);
    while (ReadByte (&c, 0) == FINE)
    {
      switch (c)
      {
      case CAN:
      case ZPAD:
        err = GetInSync (TRUE, &type);
        goto gotack;
      case XOFF:                /* Wait a while for an XON */
      case XOFF | 0x80:
        ReadByte (&c, 100);
      }
    }

  }                             /* somemore */

  /* Send ZDATA header */

  hdr.position = Mark;
  SendBinaryHeader (ZDATA, &hdr);

  /* Send one or more data subpackets */

  newcnt = Rxbuflen;
  txwcnt = 0;
  do
  {
    UpdateProgress (Mark,Length);

    /* Send data subpacket */

    n = Count;

    if (ConvertNL)
      eofseen = LocalReadBufferAndConvert (&n, Buffer, &lastc);
    else
      eofseen = LocalReadBuffer (&n, Buffer);

    if (eofseen)
      e = ZCRCE;
    else if (junkcount > 3)
      e = ZCRCW;
    else if (Mark == Lastsync)
      e = ZCRCW;
    else if (Rxbuflen && (newcnt -= n) <= 0)
      e = ZCRCW;
    else if (Txwindow && (txwcnt += n) >= Txwspac)
    {
      txwcnt = 0;
      e = ZCRCQ;
    }
    else
      e = ZCRCG;

    SendData (Buffer, n, e);

    ZBytesOut += n;
    Mark += n;

    /* Wait for ZACK if necessary */

    if (e == ZCRCW)
      goto waitack;

    /* Check reverse channel (but don't wait) */

    if (CheckCancel ())
      longjmp (CancelEnv, CANCEL);
    while (ReadByte (&c, 0) == FINE)
    {
      switch (c)
      {
      case CAN:
      case ZPAD:
        err = GetInSync (TRUE, &type);
        if (!err && type == ZACK)
          break;
        /* zcrce - dinna wanna starta ping-pong game */
        SendData (Buffer, 0, ZCRCE);
        goto gotack;
      case XOFF:                /* Wait a while for an XON */
      case XOFF | 0x80:
        ReadByte (&c, 100);
      default:
        ++junkcount;
        break;
      }
    }

    /* Make sure to stay in transmit window */

    if (Txwindow)
    {
      while ((Mark - Lrxpos) >= Txwindow)
      {
        if (e != ZCRCQ)
          SendData (Buffer, 0, e = ZCRCQ);
        err = GetInSync (TRUE, &type);
        if (err || type != ZACK)
        {
          SendData (Buffer, 0, ZCRCE);
          goto gotack;
        }
      }
    }
  }
  while (!eofseen);

  /* Send ZEOF header, wait for ZRINIT header from receiver */

  for (;;)
  {
    hdr.position = Mark;
    SendBinaryHeader (ZEOF, &hdr);
    if (GetInSync (FALSE, &type))
      return ERROR;
    switch (type)
    {
    case ZRINIT:
    case ZSKIP:
      return FINE;
    case ZACK:
      break;
    case ZRPOS:
      goto somemore;
    default:
      return ERROR;
    }
  }
}

/* ----- Invite receiver ----------------------------------------------- */

STATIC WORD
Invitation ()
{
  HEADER hdr;
  WORD retry;
  WORD err;

  DB (kprintf ("»Invitation«\n"));

  for (retry = 0; retry <= ZRetries; ++retry)
  {
    UpdateRetries(retry);
    hdr.position = 0;
    SendHexHeader (ZRQINIT, &hdr);
    if ((err = WaitZPAD (ZTimeout, 10)) == FINE)
    {
      DB (kprintf ("»Got ZPad«\n"));
      return FINE;
    }
  }
  return TIMEOUT;
}

/* ----- Send invitation to receiver, get receiver's parameters -------- */

STATIC WORD
SessionStartup ()
{
  WORD retry;
  WORD err;
  UBYTE *message;
  UBYTE type;
  HEADER hdr;

  DB (kprintf ("»SessionStartup«\n"));

  StartupTime = GetTime ();
  ResetUI(handle);
  ZBytesIn = ZBytesOut = LastBPS = 0;
  message = NULL;
  for (retry = 0; retry <= ZRetries; ++retry)
  {
    if(message)
    {
      AddMessage(message);
      message = NULL;
    }

    UpdateRetries(retry);
    switch (GetHeader (&type, &hdr))
    {
    case FINE:
      DB (kprintf ("»FINE: type %ld«\n", type));

      switch (type)
      {
      case ZRINIT:              /* This is what we want */
        message = (UBYTE *) "ZRINIT";

        DB (kprintf ("»got ZRInit«\n"));

        /* Set transfer parameters */

        Rxbuflen = hdr.zrinit.bufsize;
        Txwindow = ZWindow;
        Txwspac = Zcrcq;
        Count = ZPacket;
#ifdef XXX
        if (Txwindow < 256)
          Txwindow = 256;
        Txwindow = (Txwindow / 64) * 64;
        Txwspac = Txwindow / 4;
        if (Count > Txwspac ||
            (!Count && Txwspac < ZMAXSPLEN))
          Count = Txwspac;
#endif
        if (!hdr.zrinit.canfdx)
          Txwindow = 0;
        if (Rxbuflen && Count > Rxbuflen)
          Count = Rxbuflen;

        /* No need to send an ZSINIT frame if ... */

        Zctlesc = hdr.zrinit.escctl || Escape ();
        if (!Escape () || hdr.zrinit.escctl)
        {
          err = FINE;
          goto done;
        }
        for (retry = 0; retry <= ZRetries; ++retry)
        {
          DB (kprintf ("»sending ZSInit\n«"));
          /* ZSINIT header */
          hdr.position = 0;
          hdr.zsinit.escctl = 1;
          SendBinaryHeader (ZSINIT, &hdr);

          /* Our attention string (empty string) */
          type = 0;
          SendData (&type, 1, ZCRCW);
          /* Wait for ZACK header from receiver */
          if (!GetHeader (&type, &hdr) && type == ZACK)
          {
            err = FINE;
            goto done;
          }
        }
        err = TIMEOUT;
        goto done;

      case ZCHALLENGE:          /* Echo challenge number */
        SendHexHeader (ZACK, &hdr);
        message = (UBYTE *) "ZCHALLENGE";
        break;
      case ZCOMMAND:            /* They didn't see out ZRQINIT */
        hdr.position = 0;
        SendHexHeader (ZRQINIT, &hdr);
        message = (UBYTE *) "ZRQINIT";
        break;
      case ZRQINIT:
        if (!hdr.zrqinit.command)
          break;                /* Ignore (our echo) */
        /* Fall thru */
      default:
        hdr.position = 0;
        SendHexHeader (ZNAK, &hdr);
        message = (UBYTE *) "->ZNAK";
        break;
      }
    case TIMEOUT:
      err = TIMEOUT;
      goto done;
    default:
      hdr.position = 0;
      SendHexHeader (ZNAK, &hdr);
      message = (UBYTE *) "->ZNAK";
      break;
    }
  }
  err = TIMEOUT;                /* Retry count exhausted */
done:
  DB (kprintf ("»Done«\n"));
  if(message)
    AddMessage(message);
  return err;
}

/* ----- Send file information ----------------------------------------- */

STATIC WORD
SendFileInfo (
               UBYTE * name,            /* File name */
               ULONG size,              /* File size */
               BOOL text,               /* Text file? */
               ULONG modif,             /* Modification date */
               ULONG mode,              /* File mode data */
               ULONG * startpos,        /* Start position */
               ULONG TotalFiles,        /* Number of files to send */
               ULONG TotalBytes)        /* Number of bytes to transmit */
{
  UBYTE *p;
  UBYTE s[30];
  WORD retry;
  WORD err;
  UBYTE *message;
  UBYTE buffer[128];
  UBYTE type;
  HEADER hdr;
  LONG len;

  if (!ZSendFullPath)
    name = FilePart (name);
  else
  {
    LONG i;

    /* Follow Unix file name conventions, i.e. strip the
       * leading device/volume name if any.
     */

    for (i = 0; i < strlen (name); i++)
    {
      if (name[i] == ':')
      {
        name = &name[i + 1];
        break;
      }
    }
  }

  /* File name as zero terminated ASCII string */

  p = buffer;
  strcpy (p, name);
  p += strlen (name) + 1;

  /* File size as decimal ASCII digits */

  LimitedSPrintf (sizeof (s), s, "%ld", size);
  len = strlen (s);

  memcpy (p, s, len);
  p += len;
  *p++ = ' ';

  /* Modification date: octal string = seconds since 1/1/1970 UTC */
  ConvertTime (FALSE, &modif);  /* -> ZModem time */

  ToOctal (modif, s);
  len = strlen (s);
  memcpy (p, s, len);
  p += len;
  *p++ = ' ';

  /* File mode (octal string) */

  ToOctal (mode, s);
  len = strlen (s);
  memcpy (p, s, len);
  p += len;
  *p++ = ' ';

  /* Serial number (octal string) */

  *p++ = '0';
  *p++ = ' ';

  /* Number of files remaining (decimal number) */

  LimitedSPrintf (sizeof (s), s, "%ld", TotalFiles);
  len = strlen (s);
  memcpy (p, s, len);
  p += len;
  *p++ = ' ';

  /* Number of bytes remaining (decimal number) */
  LimitedSPrintf (sizeof (s), s, "%ld", TotalBytes);
  len = strlen (s);
  memcpy (p, s, len);
  p += len;
  *p++ = 0;                     /* Final NULL */

  /* Send ZFILE header with ZModem conversion, management and transport
     options followed by a ZCRCW data subpacket containing the file name,
     file length, modification date. */

  message = NULL;
  for (retry = 0; retry <= ZRetries; ++retry)
  {
    UpdateRetries(retry);

    hdr.position = 0;
    hdr.zfile.conv = text ? 0 : ZCBIN;

    ConvertNL = text;

    if (ZOverwrite)
      hdr.zfile.manage = ZMCLOB;        /* Replace existing file */

    DB (kprintf ("»Sending ZFILE header (flags %08lx)«\n", hdr.position));
    SendBinaryHeader (ZFILE, &hdr);
    DB (kprintf ("»Sending data«\n"));
    SendData (buffer, p - buffer, ZCRCW);
    DB (kprintf ("»waiting for response«\n"));
  again:
    if(message)
    {
      AddMessage(message);
      message = NULL;
    }

    if (GetHeader (&type, &hdr))
    {                           /* Error */
      continue;
    }
    switch (type)
    {
    case ZRINIT:
      message = (UBYTE *) "ZRINIT";
      if (WaitZPAD (ZTimeout, 10) == FINE)
        goto again;
      break;
    case ZSKIP:
      message = (UBYTE *) "ZSKIP";
      /* The receiver may respond with a ZSKIP header, which
         makes the sender proceed to the next file (if any) in the
         batch. */
      *startpos = 0x7FFFFFFF;
      err = FINE;
      goto done;
    case ZRPOS:
      message = (UBYTE *) "ZRPOS";
      *startpos = hdr.position;
      err = FINE;
      goto done;
    }
  }
  err = TIMEOUT;
done:
  if(message)
    AddMessage(message);

  return err;
}

/* ----- Session cleanup ----------------------------------------------- */

STATIC VOID
SessionCleanup ()
{
  WORD retry;
  HEADER hdr;
  UBYTE type;
  STATIC UBYTE oo[] = "OO";     /* Over and out */

  for (retry = 0; retry <= ZRetries; ++retry)
  {
    hdr.position = 0;
    SendHexHeader (ZFIN, &hdr);
    if (GetHeader (&type, &hdr))
      continue;
    if (type == ZFIN)
    {
      SendString (oo, strlen (oo));
      return;
    }
  }
}

/* ----- Send a file --------------------------------------------------- */

LONG
ZTransmit (struct WBArg * ArgList, LONG NumArgs, BOOL TextMode)
{
  LONG err;
  ULONG modif;
  ULONG mode;
  ULONG TotalFiles, TotalBytes;
  BPTR OldCD, FileLock;
  LONG i;
  D_S (struct FileInfoBlock, FileInfo);

  Transmitting = TRUE;

  TotalFiles = TotalBytes = 0;
  BytesThisBatch = FilesThisBatch = 0;

  for (i = 0; i < NumArgs; i++)
  {
    OldCD = CurrentDir (ArgList[i].wa_Lock);

    if (!(FileLock = Lock (ArgList[i].wa_Name, SHARED_LOCK)))
      err = IoErr ();
    else
    {
      if (!Examine (FileLock, FileInfo))
        err = IoErr ();
      else
      {
        if (FileInfo->fib_DirEntryType > 0)
          err = ERROR_OBJECT_WRONG_TYPE;
        else
        {
          err = 0;

          if (!ZSkipIfArchived || !(FileInfo->fib_Protection & FIBF_ARCHIVE))
          {
            TotalFiles++;
            TotalBytes += FileInfo->fib_Size;
          }
        }
      }

      UnLock (FileLock);
    }

    CurrentDir (OldCD);

    if (err)
      return (err);
  }

  DB (kprintf ("»Buffer size is %ld«\n", ZMAXSPLEN));

  Buffer = (UBYTE *) AllocVecPooled (ZMAXSPLEN, MEMF_ANY);
  err = NewTxBuffer ();
  handle = OpenUI ("ZModem transmit");

  if (Buffer != NULL && err == 0 && handle != NULL)
  {
    Zctlesc = Escape ();

    ClearSerial ();

    if (SerialCache = GetSerialWaiting ())
    {
      if (SerialCache > SERIALBUFFERSIZE)
        SerialCache = SERIALBUFFERSIZE;

      DoSerialRead (SerialBuffer, SerialCache);
    }

    StartSerialRead (&zc, 1);

    for (i = 0; i < NumArgs; i++)
    {
      OldCD = CurrentDir (ArgList[i].wa_Lock);

      FileHandle = NULL;

      if (!(FileLock = Lock (ArgList[i].wa_Name, SHARED_LOCK)))
        err = IoErr ();
      else
      {
        if (!Examine (FileLock, FileInfo))
          err = IoErr ();
        else
        {
          if (FileInfo->fib_DirEntryType > 0)
            err = ERROR_OBJECT_WRONG_TYPE;
          else
          {
            LimitedStrcpy (sizeof (FileName), FileName, ArgList[i].wa_Name);

            if (!(FileHandle = Open (FileName, MODE_OLDFILE)))
              err = IoErr ();
            else
              err = 0;
          }
        }

        UnLock (FileLock);
      }

      CurrentDir (OldCD);

      if (ZSkipIfArchived && (FileInfo->fib_Protection & FIBF_ARCHIVE))
        continue;

      if (err != 0 || FileHandle == NULL)
        break;
      else
      {
        StartupTime = GetTime ();
        ZBytesIn = ZBytesOut = LastBPS = 0;
        Mark = StartPos = 0;
        ResetUI(handle);

        SetVBuf (FileHandle, NULL, BUF_FULL, 4096);

        Length = FileInfo->fib_Size;
        modif = (FileInfo->fib_Date.ds_Days * 24 * 60 + FileInfo->fib_Date.ds_Minute) * 60 + FileInfo->fib_Date.ds_Tick / TICKS_PER_SECOND;
        mode = Amiga2UnixMode (FileInfo->fib_Protection);

        UpdateProgress (0, Length);

        if (err = setjmp (CancelEnv))
        {                       /* Cancel or abort */
          if (err == CANCEL)
            SendCancel ();      /* Send cancel string */

          break;
        }

        if (i == 0)
        {
          if (!(err = Invitation ()))
            err = SessionStartup ();
        }
        else
          err = 0;

        if (!err)
        {
          if (!(err = SendFileInfo (FileName, Length, TextMode, modif, mode, &Mark, TotalFiles, TotalBytes)))
          {
            UpdateBatch(FilesThisBatch,TotalFiles,BytesThisBatch,TotalBytes);

            TotalFiles--;
            TotalBytes -= Length;

            BytesThisBatch += Length;
            FilesThisBatch++;

            if (Mark != 0x7FFFFFFF)
            {                   /* Not skip */
              SetFilePosition (Mark);

              NameProgress (FilePart (FileName));

              /* Send file data */
              err = SendFileData ();
            }
          }
        }

        Close (FileHandle);
        FileHandle = NULL;

        if (err == 0)
        {
          if (ZDeleteAfterSending)
            DeleteFile (FileName);
          else
          {
            if (ZProtectAfterSending)
              SetProtection (FileName, FileInfo->fib_Protection | FIBF_ARCHIVE);
          }
        }

        if (TotalFiles == 0)
          SessionCleanup ();

        if (err != 0)
          break;
      }
    }

    StopSerialRead ();
    RestartSerial ();
  }

  if (handle)
  {
    UpdateError (err);

    CloseUI (handle, err);
    handle = NULL;
  }
  if (FileHandle)
  {
    Close (FileHandle);
    FileHandle = NULL;
  }

  DisposeTxBuffer ();

  if (Buffer)
    FreeVecPooled (Buffer);

  return err;
}

/* ===== RECEIVE ======================================================= */

/* ----- Free bytes on the current file system ------------------------- */

STATIC ULONG
GetFreeSpace ()
{
  if (IsBlockMapped)
  {
    D_S (struct InfoData, InfoData);

    if (Info (ReceiveDir, InfoData))
    {
      if (InfoData->id_NumBlocks && InfoData->id_BytesPerBlock)
        return ((ULONG) ((InfoData->id_NumBlocks - InfoData->id_NumBlocksUsed) * InfoData->id_BytesPerBlock));
    }
  }

  return (0xFFFFFFFF);          /* many free bytes ... */
}

/* ----- Ack a ZFIN packet, let byegones be byegones ------------------- */

STATIC VOID
AckOverAndOut ()
{
  WORD retry;
  WORD err;
  HEADER hdr;
  UBYTE c;

  hdr.position = 0;
  for (retry = 0; retry < 3; ++retry)
  {
    SendHexHeader (ZFIN, &hdr);

    err = ReadByte (&c, ZTimeout);

    if (err == FINE && c == 'O')
    {
      ReadByte (&c, 60);
      return;                   /* Over and Out */
    }
  }
}

/* ----- Initialize for Zmodem receive attempt ------------------------- */

STATIC WORD
TryZReceive (
              HEADER * filehdr) /* Return sender's options */
{
  UBYTE *message;               /* Message to display */
  HEADER hdr;                   /* Header */
  UBYTE type;                   /* Header type received */
  WORD retry;                   /* Retry counter */
  WORD err;                     /* Error code */

  for (retry = 0, message = NULL; retry <= ZRetries;)
  {
    UpdateRetries(retry);

    hdr.position = 0;
    if (TryZhdrType == ZRINIT)
    {                           /* Capability flags */
      hdr.zrinit.bufsize = ZBuffer;
      hdr.zrinit.canfdx = 1;
      hdr.zrinit.canovio = 1;
      hdr.zrinit.escctl = Escape ();
    }
    SendHexHeader (TryZhdrType, &hdr);
    if (TryZhdrType == ZSKIP)   /* Don't skip too far */
      TryZhdrType = ZRINIT;
  again:
    if(message)
    {
      AddMessage(message);
      message = NULL;
    }
    ++retry;
    if (GetHeader (&type, &hdr) == FINE)
    {
      DB (kprintf ("»FINE: type %ld«\n", type));

      switch (type)
      {
      case ZRQINIT:
        continue;

      case ZFILE:
        *filehdr = hdr;         /* Return file options */
        TryZhdrType = ZRINIT;
        if (!ReceiveData (Buffer, ZMAXSPLEN, &Count, &type) &&
            type == ZCRCW)
        {
          err = FINE;
          goto done;            /* File name received */
        }
        SendHexHeader (ZNAK, &hdr);
        message = (UBYTE *) "ZFILE error";
        goto again;
      case ZSINIT:
        Zctlesc = hdr.zsinit.escctl;
        if (!ReceiveData (Attn, ZATTNLEN, &AttnLen, &type) &&
            type == ZCRCW)
        {
          hdr.position = 1;
          SendHexHeader (ZACK, &hdr);
          message = (UBYTE *) "ZSINIT ok";
          goto again;
        }
        SendHexHeader (ZNAK, &hdr);
        message = (UBYTE *) "ZSINIT error";
        goto again;
      case ZFREECNT:
        hdr.position = GetFreeSpace ();
        SendHexHeader (ZACK, &hdr);
        message = (UBYTE *) "ZFREECNT";
        goto again;
      case ZCOMMAND:
        if (!ReceiveData (Buffer, ZMAXSPLEN, &Count, &type) && type == ZCRCW)
        {
          STRPTR String;

          String = Buffer;

          while (*String == ' ' || *String == '\t')
            String++;

          if (!Strnicmp (String, "echo", 4))
            AddMessage(&String[4]);
          else
            AddMessage("Ignoring ZCOMMAND %s",String);

          do
          {
            hdr.position = 0;
            SendHexHeader (ZCOMPL, &hdr);

            if (GetHeader (&type, &hdr) == ZFIN)
            {
              AckOverAndOut ();
              Count = 0;        /* Nothing in Buffer */
              err = FINE;
              goto done;
            }
          }
          while (++errors < ZErrors);
        }
        else
        {
          SendHexHeader (ZNAK, &hdr);
          message = (UBYTE *) "ZCOMMAND error";
        }

        goto again;
      case ZCOMPL:
        message = (UBYTE *) "ZCOMPL";
        goto again;
      case ZFIN:
        message = (UBYTE *)"ZFIN";
        AckOverAndOut ();
        Count = 0;              /* Nothing in Buffer */
        err = FINE;
        goto done;
      }
    }
    else
      message = NULL;
  }
  err = TIMEOUT;
  Count = 0;                    /* Nothing in Buffer */
done:
  if(message)
     AddMessage(message);
  return err;
}

/* ----- Process incoming file information header ---------------------- */

STATIC LONG
ProcedeHeader (
                HEADER * filehdr,       /* Sender's options */
                STRPTR name,            /* File name extracted */
                ULONG * length,         /* File length extracted */
                ULONG * modif,          /* Modification time extracted */
                ULONG * mode,           /* File mode extracted */
                ULONG * filesleft,      /* Number of files left (0 == not known) */
                ULONG * bytesleft)      /* Number of bytes left (0 == not known) */
{
  WORD err;
  UBYTE *p, *max;
  UBYTE c;
  BOOL exists;                  /* File exists */
  LONG len;
  BPTR fileLock;
  D_S (struct FileInfoBlock, FileInfo);

  *name = 0;
  *length = 0;
  *modif = 0;
  *mode = 0;
  *filesleft = 0;
  *bytesleft = 0;

  FileHandle = NULL;

  /* Extract fields from file information header */

  max = (p = Buffer) + Count;
  err = 0;
  while (p < max)
  {
    c = *p++;
    switch (err)
    {
    case 0:                     /* File name (C-string) */
      if (c == '\0')
      {
        len = p - 1 - Buffer;
        memcpy (name, Buffer, len);
        name[len] = 0;
        ++err;                  /* Goto next state */
      }
      break;
    case 1:                     /* File length (decimal string, space) */
      if (c == ' ' || c == '\0')
        ++err;                  /* Goto next state */
      else
        *length = *length * 10 + (c & 0x0F);
      break;
    case 2:                     /* File modification time (octal string, space) */
      if (c == ' ' || c == '\0')
      {
        ConvertTime (TRUE, modif);      /* -> Amiga time */
        ++err;                  /* Goto next state */
      }
      else
        *modif = *modif * 8 + (c & 7);
      break;
    case 3:                     /* File modes (octal string, space) */
      if (c == ' ' || c == '\0')
      {
        *mode = Unix2AmigaMode (*mode);
        ++err;                  /* Goto next state */
      }
      else
        *mode = *mode * 8 + (c & 7);
      break;
    case 4:                     /* Serial number. */
      if (c == ' ' || c == '\0')
        err++;
      break;
    case 5:                     /* Number of files left */
      if (c == ' ' || c == '\0')
        err++;
      else
        *filesleft = *filesleft * 10 + (c & 15);
      break;
    case 6:                     /* Number of bytes left */
      if (c == ' ' || c == '\0')
        err++;
      else
        *bytesleft = *bytesleft * 10 + (c & 15);
      break;
    default:
      break;
    }
  }

  if (err < 3)                  /* Got no modification time...  */
    *modif = GetTime ();        /* ...so use actual time        */

  if (!ZUseFullPath)
    name = FilePart (name);

  /* See if file exists */

  if (fileLock = Lock (name, SHARED_LOCK))
  {
    if (!Examine (fileLock, FileInfo))
      memset (FileInfo, 0, sizeof (struct FileInfoBlock));

    UnLock (fileLock);

    exists = TRUE;
  }
  else
    exists = FALSE;

  /* Skip file if not present at rx */

  if (filehdr->zfile.skip && !exists)
    return 100;

  if (exists)
  {
    if (ZTransferIfNewer && err > 2)
    {
      struct DateStamp Stamp;

      Stamp.ds_Days = *modif / (24 * 60 * 60);
      Stamp.ds_Minute = (*modif % (24 * 60 * 60)) / 60;
      Stamp.ds_Tick = (*modif % 60) * TICKS_PER_SECOND;

      if (CompareDates (&Stamp, &FileInfo->fib_Date) < 0)
        exists = FALSE;
    }

    if (ZTransferIfSize && err > 1)
    {
      if (FileInfo->fib_Size != *length)
        exists = FALSE;
    }
  }

  /* Delete existing file */

  if (exists && filehdr->zfile.manage == ZMCLOB)
    exists = FALSE;

  /* Create new file */

  if (!exists)
    FileHandle = Open (name, MODE_NEWFILE);
  else
  {
    if (!(fileLock = Lock (name, EXCLUSIVE_LOCK)))
      FileHandle = Open (name, MODE_NEWFILE);
    else
    {
      if (!(FileHandle = OpenFromLock (fileLock)))
      {
        UnLock (fileLock);

        FileHandle = Open (name, MODE_READWRITE);
      }

      if (FileHandle)
      {
        if (Seek (FileHandle, 0, OFFSET_END) == -1)
        {
          LONG Error;

          Error = IoErr ();

          Close (FileHandle);

          SetIoErr (Error);

          FileHandle = NULL;
        }
      }
    }
  }

  if (FileHandle)
  {
    LimitedStrcpy (sizeof (FileName), FileName, name);

    return (FINE);
  }
  else
    return (IoErr ());
}

/* ----- Write buffer to file ------------------------------------------ */

STATIC LONG
WriteFile ()
{
  if (Write (FileHandle, Buffer, Count) == Count)
    return (0);
  else
    return (IoErr ());
}

/* ----- File was completely received ----------------------------------- */

STATIC VOID
CleanupFile (
              STRPTR name,      /* File name from ZModem header */
              ULONG modif,      /* File modification time */
              ULONG mode)       /* File mode data */
{
  struct DateStamp date;

  /* Set file creation/modification time and file attributes */

  date.ds_Days = modif / (24 * 60 * 60);
  date.ds_Minute = (modif % (24 * 60 * 60)) / 60;
  date.ds_Tick = (modif % 60) * TICKS_PER_SECOND;

  SetFileDate (name, &date);
  SetProtection (name, mode);
}

/* ----- Receive a file ------------------------------------------------ */

STATIC WORD
ReceiveFile (
              HEADER * filehdr) /* Sender's options */
{
  WORD err;             /* Error code */
  WORD retry;           /* Retry counter */
  UBYTE *message;       /* Message to display */
  UBYTE name[256];      /* File name from Z header */
  ULONG modif;          /* File modification date (Amiga time) */
  ULONG mode;           /* File mode data. */
  ULONG filesleft;      /* Number of files left. */
  ULONG bytesleft;      /* Number of bytes left. */
  HEADER hdr;           /* Header */
  UBYTE type;           /* Header type received */
  UBYTE end;            /* Data frame type */

  /* Get ready to receive new file */

  if (ProcedeHeader (filehdr, name, &Length, &modif, &mode, &filesleft, &bytesleft))
  {
    /* Error or skip, but that's fine! */
    TryZhdrType = ZSKIP;
    return FINE;
  }

  UpdateBatch(FilesThisBatch,filesleft,BytesThisBatch,bytesleft);

  Mark = StartPos = Seek (FileHandle, 0, OFFSET_CURRENT);

  NameProgress (name);
  Rxbuflen = ZBuffer;

  /* Receiving loop */

  retry = 0;
  message = NULL;

  for (;;)
  {
    UpdateRetries(retry);
    UpdateProgress (Mark,Length);

    hdr.position = Mark;
    SendHexHeader (ZRPOS, &hdr);
  nxthdr:
    if(message)
    {
      AddMessage("%s (position = %lD)",message,Mark);
      message = NULL;
    }
    switch (GetHeader (&type, &hdr))
    {
    case FINE:
      switch (type)
      {
      case ZNAK:
        ++retry;
        if (retry > ZRetries)
        {
          err = ERROR;
          goto done;
        }
        message = (UBYTE *) "hdr-ZNAK";
        break;
      case ZFILE:
        err = ReceiveData (Buffer, ZMAXSPLEN, &Count, &end);

        if (err != FINE)
          goto done;

        message = (UBYTE *) "hdr-ZFILE";
        break;
      case ZEOF:
        /* Ignore eof if it's at wrong place - force a
           timeout because the eof might have gone out before
           we sent our zrpos. */
        message = (UBYTE *) "hdr-ZEOF";
        if (hdr.position != Mark)
          goto nxthdr;          /* Ignore this eof */

        Close (FileHandle);
        FileHandle = NULL;

        BytesThisBatch += Mark;
        FilesThisBatch++;
        CleanupFile (name, modif, mode);
        err = FINE;             /* Normal eof */
        goto done;
      case ZSKIP:
        message = (UBYTE *) "hdr-ZSKIP";
        Close (FileHandle);
        FileHandle = NULL;
        err = FINE;             /* Sender skipped it */
        goto done;
      case ZDATA:
        message = (UBYTE *) "hdr-ZDATA";
        if (hdr.position != Mark)
        {
          ++retry;
          if (retry > ZRetries)
          {
            err = ERROR;
            goto done;
          }
          SendAttn (Attn);
          break;
        }
      moredata:
        UpdateProgress (Mark,Length);
        if(message)
        {
          AddMessage("%s (position = %lD)",message,Mark);
          message = NULL;
        }
        switch (ReceiveData (Buffer, ZMAXSPLEN, &Count, &end))
        {
        case TIMEOUT:
          ++retry;
          if (retry > ZRetries)
          {
            err = ERROR;
            goto done;
          }
          message = (UBYTE *) "data-TIMEOUT";
          break;
        case ERROR:
          ++retry;
          if (retry > ZRetries)
          {
            err = ERROR;
            goto done;
          }
          SendAttn (Attn);
          message = (UBYTE *) "data-ERROR";
          break;
        case FINE:
          retry = 0;
          WriteFile ();
          Mark += Count;
          ZBytesIn += Count;
          switch (end)
          {
          case ZCRCW:
            message = (UBYTE *) "data-ZCRCW";
            hdr.position = Mark;
            SendHexHeader (ZACK, &hdr);
            goto nxthdr;
          case ZCRCQ:
            message = (UBYTE *) "data-ZCRCQ";
            hdr.position = Mark;
            SendHexHeader (ZACK, &hdr);
            goto moredata;
          case ZCRCG:
            message = (UBYTE *) "data-ZCRCG";
            goto moredata;
          case ZCRCE:
            message = (UBYTE *) "data-ZCRCE";
            goto nxthdr;
          }
          break;
        }
        break;
      default:
        err = ERROR;
        goto done;
      }
      break;
    case TIMEOUT:
      ++retry;
      if (retry > ZRetries)
      {
        err = ERROR;
        goto done;
      }
      message = (UBYTE *) "hdr-TIMEOUT";
      break;
    case ERROR:
      ++retry;
      if (retry > ZRetries)
      {
        err = ERROR;
        goto done;
      }
      SendAttn (Attn);
      message = (UBYTE *) "hdr-ERROR";
      break;
    }
  }

done:
  if(message)
     AddMessage("%s (position = %lD)",message,Mark);
  return err;
}

/* ----- Receive one or more files ------------------------------------- */

STATIC WORD
ReceiveFiles ()
{
  WORD err;
  HEADER filehdr;               /* Sender's options */

  Mark = StartPos = 0;
  StartupTime = GetTime ();
  ZBytesIn = ZBytesOut = LastBPS = 0;
  Zctlesc = Escape ();
  TryZhdrType = ZRINIT;
  Attn[0] = 0;                  /* Clear sender's attention string */
  AttnLen = 0;

  for (;;)
  {
    StartupTime = GetTime ();
    ZBytesIn = ZBytesOut = LastBPS = 0;
    ResetUI(handle);

    errors = 0;
    err = TryZReceive (&filehdr);

    if (!Count)
    {
      if (err)
        UpdateError (err);

      return err;
    }

    ConvertNL = (BOOL) (filehdr.zfile.conv == ZCNL);

    err = ReceiveFile (&filehdr);

    UpdateError (err);

    if (err)
      return err;
  }
}

/* ----- ZModem receive ------------------------------------------------ */

LONG
ZReceive ()
{
  struct FileLock *FileLock;
  struct DosList *Entry;
  LONG err;
  BPTR OldCD;

  Transmitting = FALSE;
  BytesThisBatch = FilesThisBatch = 0;

  if (!(ReceiveDir = Lock (Config->PathConfig->BinaryDownloadPath, SHARED_LOCK)))
    return (IoErr ());

  FileLock = (struct FileLock *) BADDR (ReceiveDir);

  IsBlockMapped = FALSE;

  Entry = LockDosList (LDF_DEVICES | LDF_READ);

  while (Entry = NextDosEntry (Entry, LDF_DEVICES))
  {
    if (Entry->dol_Task == FileLock->fl_Task)
    {
      struct FileSysStartupMsg *Startup;

      Startup = (struct FileSysStartupMsg *) BADDR (Entry->dol_misc.dol_handler.dol_Startup);

      if (TypeOfMem (Startup))
      {
        struct DosEnvec *Environ;
        STRPTR Name;

        Name = (STRPTR) BADDR (Startup->fssm_Device);
        Environ = (struct DosEnvec *) BADDR (Startup->fssm_Environ);

        if (TypeOfMem (Name) && TypeOfMem (Environ))
        {
          struct IOStdReq Request;

          memset (&Request, 0, sizeof (Request));
          Request.io_Message.mn_Length = sizeof (Request);

          if (!OpenDevice (Name + 1, Startup->fssm_Unit, (struct IORequest *) &Request, Startup->fssm_Flags))
          {
            IsBlockMapped = TRUE;

            CloseDevice ((struct IORequest *) &Request);

            break;
          }
        }
      }
    }
  }

  UnLockDosList (LDF_DEVICES | LDF_READ);

  OldCD = CurrentDir (ReceiveDir);

  FileHandle = NULL;            /* No file open yet */

  DB (kprintf ("»Buffer size is %ld«\n", BUFFERSIZE));

  Buffer = (UBYTE *) AllocVecPooled (BUFFERSIZE, MEMF_ANY);
  err = NewTxBuffer ();
  handle = OpenUI ("ZModem receive");

  if (Buffer != NULL && err == 0 && handle != NULL)
  {
    ClearSerial ();

    if (SerialCache = GetSerialWaiting ())
    {
      if (SerialCache > SERIALBUFFERSIZE)
        SerialCache = SERIALBUFFERSIZE;

      DoSerialRead (SerialBuffer, SerialCache);
    }

    StartSerialRead (&zc, 1);

    if (err = setjmp (CancelEnv))       /* Prepare cancel or abort */
    {
      if (err == CANCEL)
        SendCancel ();          /* Send cancel string */
    }
    else
    {
      StartupTime = GetTime ();
      ZBytesIn = ZBytesOut = LastBPS = 0;
      Length = 0;
      ResetUI(handle);

      err = ReceiveFiles ();    /* Receive file(s) */
    }

    UpdateError (err);

    StopSerialRead ();
    RestartSerial ();
  }

  if (handle)
  {
    CloseUI (handle, err);
    handle = NULL;
  }

  if (FileHandle)
  {
    Close (FileHandle);
    FileHandle = NULL;

    if (!ZKeepPartial)
      DeleteFile (FileName);
  }

  DisposeTxBuffer ();

  if (Buffer)
    FreeVecPooled (Buffer);

  UnLock (CurrentDir (OldCD));

  return err;
}

/****************************************************************************/

STATIC VOID
InternalZModemUpload(BOOL TextMode)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	struct FileRequester *FileRequester;

	LimitedStrcpy(sizeof(LocalBuffer),LocalBuffer,Config->PathConfig->BinaryUploadPath);

	if(FileRequester = OpenSeveralFiles(Window,"ZModem transfer","Send",NULL,LocalBuffer,sizeof(LocalBuffer)))
	{
		ZTransmit(FileRequester->fr_ArgList,FileRequester->fr_NumArgs,TextMode);

		FreeAslRequest(FileRequester);
	}
}

VOID
InternalZModemTextUpload()
{
	InternalZModemUpload(TRUE);
}

VOID
InternalZModemBinaryUpload()
{
	InternalZModemUpload(FALSE);
}

STATIC STRPTR STACKARGS SAVE_DS
TimeoutDispFunc(struct Gadget *UnusedGadget,UWORD Level)
{
	STATIC UBYTE LocalBuffer[40];

	FormatTime(LocalBuffer,sizeof(LocalBuffer),Level / (60 * 60),(Level / 60) % 60,Level % 60);

	return(LocalBuffer);
}

STATIC STRPTR STACKARGS SAVE_DS
FrameDispFunc(struct Gadget *UnusedGadget,UWORD Level)
{
	if(Level > 0)
	{
		STATIC UBYTE LocalBuffer[40];

		LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,ConvNumber,Level);
		LimitedStrcat(sizeof(LocalBuffer),LocalBuffer," Bytes");

		return(LocalBuffer);
	}
	else
		return((STRPTR)"« Streaming »");
}

VOID
ZSettings()
{
	LayoutHandle *Handle;

	if(Handle = LT_CreateHandleTagList(Window->WScreen,NULL))
	{
		enum { GAD_Ok=1000 };

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Escape control characters",
					LA_BOOL,		&ZEscapeCtl,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Clobber existing files",
					LA_BOOL,		&ZOverwrite,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		SliderType,
					LA_LabelText,	"Receive timeout",
					LA_Chars,		20,
					GTSL_Min,		1,
					GTSL_Max,		12 * 60 * 60,
					GTSL_DispFunc,	TimeoutDispFunc,
					GTSL_LevelFormat,"%s h",
					LA_LONG,		&ZTimeout,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		SliderType,
					LA_LabelText,	"Retry limit",
					GTSL_Min,		0,
					GTSL_Max,		1000,
					LA_LONG,		&ZRetries,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		SliderType,
					LA_LabelText,	"Error limit",
					GTSL_Min,		0,
					GTSL_Max,		1000,
					LA_LONG,		&ZErrors,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		SliderType,
					LA_LabelText,	"Frame size",
					GTSL_Min,		0,
					GTSL_Max,		1024,
					LA_LONG,		&ZWindow,
					GTSL_DispFunc,	FrameDispFunc,
					GTSL_LevelFormat,"%s",
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Keep partial files",
					LA_BOOL,		&ZKeepPartial,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Delete files after sending",
					LA_BOOL,		&ZDeleteAfterSending,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Mark files as archived after sending",
					LA_BOOL,		&ZProtectAfterSending,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Do not send archived files",
					LA_BOOL,		&ZSkipIfArchived,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Send full path names",
					LA_BOOL,		&ZSendFullPath,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Use full received path names",
					LA_BOOL,		&ZUseFullPath,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Overwrite files if newer",
					LA_BOOL,		&ZTransferIfNewer,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelText,	"Overwrite files if size is different",
					LA_BOOL,		&ZTransferIfSize,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,HORIZONTAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullWidth,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,HORIZONTAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelText,	"Ok",
					LA_ID,			GAD_Ok,
					LABT_ExtraFat,	TRUE,
					LABT_ReturnKey,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LT_Build(Handle,
			LAWN_TitleText,	"Internal ZModem settings",
			LAWN_Zoom,		TRUE,
			LAWN_Parent,	Window,
			WA_DepthGadget,	TRUE,
			WA_DragBar,		TRUE,
			WA_RMBTrap,		TRUE,
			WA_Activate,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage *Message;
			ULONG MsgClass;
			struct Gadget *MsgGadget;
			BOOL Done;

			Done = FALSE;

			do
			{
				WaitPort(Handle->Window->UserPort);

				while(Message = LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP && MsgGadget->GadgetID == GAD_Ok)
						Done = TRUE;
				}
			}
			while(!Done);
		}

		LT_DeleteHandle(Handle);
	}
}

#endif	/* BUILTIN_ZMODEM */

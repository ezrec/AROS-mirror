
/* DLib.h */

#ifndef DLIB_H
#define DLIB_H

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/ports.h>

#ifdef DEBUG
#include <stdio.h>
/* #include "/include/syslog.h" */
#endif

#define NT_REQUEUE  0x40

struct DLinkChan {
	struct MsgPort	port;		/* recieve data, replies */
	struct MsgPort *DLinkPort;	/* dlink's master port */
	struct List	rdylist;	/* ready to be read */
	UWORD		chan;		/* chan # for open channels */
	UBYTE		eof;		/* channel remotely closed? */
	UBYTE		filler;		/* ????? */
	int		qlen;		/* allowed write queue size */
	int		queued;		/* current # packets queued */
};


#define DNCMD_OPEN      32  /*  Application open                */
#define DNCMD_SOPEN     33  /*  Server open                     */
#define DNCMD_CLOSE     34  /*  Close a channel                 */
#define DNCMD_EOF       35  /*  Will no longer write to channel */

#define DNCMD_WRITE     36  /*  Write data to a channel         */
#define DNCMD_QUIT      37  /*  Kill the DNET server            */
#define DNCMD_IOCTL     38
#define DNCMD_EXEC      39  /*  server asks driver to Execute() something */
#define DNCMD_INFO      64  /*  Ascii status message            */

/* cfg.c */
int OpenCfgFile ( void );
char *GetCfgLine ( char *what );
void CloseCfgFile ( void );
void GetOneCfg ( char *what );
int ExtractFieldVal ( char *str , char *field , short *pidx );

/* SafePutToPort.c */
BOOL SafePutToPort ( struct Message *message , STRPTR portname );

/* ioctl.c */
UBYTE DGetIoctl ( struct DLinkChan *chan , UWORD *pval , UBYTE *paux );
BOOL DIoctl ( struct DLinkChan *chan , UBYTE cmd , UWORD val , UBYTE aux );

/* write.c */
BOOL DQueueFull ( struct DLinkChan *chan );
LONG DWrite ( struct DLinkChan *chan , UBYTE *buf , LONG bytes );
void DQueue ( struct DLinkChan *chan , LONG len );

/* DOpen.c */
struct DLinkChan *DOpen ( UWORD portnum , int txpri , int rxpri );

/* DQuit.c */
BOOL DBreak ( void );
BOOL DQuit ( void );

/* Channel.c */
struct DLinkChan *MakeChannel ( void );
void DeleteChannel ( struct DLinkChan *chan );

/* DStat.c */
BOOL DStat ( UBYTE *buffer , ULONG size );

/* FixSignal.c */
void FixSignal ( struct DLinkChan *chan );

/* read.c */
LONG DNRead ( struct DLinkChan *chan , UBYTE *buf , LONG bytes );
LONG DRead ( struct DLinkChan *chan , UBYTE *buf , LONG bytes );

/* WaitMsg.c */
void WaitMsg ( struct IORequest *ior );

/* WaitQueue.c */
short WaitQueue ( struct DLinkChan *chan , struct IOStdReq *skipior );

/* accept.c */
struct DLinkChan *DAccept ( struct MsgPort *listenPort );
BOOL DNAccept ( struct MsgPort *listenPort );

/* DClose.c */
BOOL DClose ( struct DLinkChan *chan );

/* DExec.c */
BOOL DExec ( UBYTE *command );

/* eof.c */
BOOL DEof ( struct DLinkChan *chan );
int DCheckEof ( struct DLinkChan *chan );

/* listen.c */
struct MsgPort *DListen ( UWORD portNumber );
void DUnListen ( struct MsgPort *port );

/* nodes.c */
struct Node * GetHead( struct List *list );
struct Node * GetSucc( struct Node *node );
#endif


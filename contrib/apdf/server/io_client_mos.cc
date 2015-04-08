/*************************************************************************\
 *                                                                       *
 * io_client_68k.cc                                                      *
 *                                                                       *
 * Copyright 2000-2005 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include <string.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include "apdf/io_client.h"
#include "apdf/io_server.h"

#if 0
#   define DB(x)     x
#   include <stdio.h>
#else
#   define DB(x)
#endif

struct MyMessage1 {
    struct Message msg;
    char buf[1024];
};
struct MyMessage2 {
    struct Message msg;
    void *buf;
};

static struct MsgPort *port;
static struct MsgPort *replyPort;
static struct MyMessage1 *msg1;
static struct MyMessage2 *msg2;

IoHandle ioOpen(const char *name, size_t *size) {
    struct IoOpenMsg *msg = (struct IoOpenMsg *)msg1->buf;
    DB(printf("ioOpen(%s)\n", name);)
    msg->ioMsg.id = ioIdOpen;
    strncpy(msg->fileName, name, sizeof(msg->fileName) - 1);
    msg->fileName[sizeof(msg->fileName) - 1] = '\0';
    PutMsg(port, &msg1->msg);
    WaitPort(replyPort);
    GetMsg(replyPort);
    *size = msg->size;
    DB(printf("ioOpen = %lx, size = %ld\n", msg->ioMsg.handle, msg->size);)
    return msg->ioMsg.handle;
}

void ioClose(IoHandle handle) {
    struct IoMsg *msg = (struct IoMsg *)msg1->buf;
    DB(printf("ioClose(%lx)\n", handle);)
    msg->id = ioIdClose;
    msg->handle = handle;
    PutMsg(port, &msg1->msg);
    WaitPort(replyPort);
    GetMsg(replyPort);
}

void ioQuit(void) {
    struct IoMsg *msg = (struct IoMsg *)msg1->buf;
    DB(printf("ioQuit()\n");)
    msg->id = ioIdQuit;
    PutMsg(port, &msg1->msg);
    WaitPort(replyPort);
    GetMsg(replyPort);
}

void ioPreRead(IoHandle handle, size_t offset, size_t size) {
    struct IoPreReadMsg *msg = (struct IoPreReadMsg *)msg1->buf;
    DB(printf("ioPreRead(%lx, 0x%lx, 0x%lx)\n", handle, offset, size);)
    msg->ioMsg.id = ioIdPreRead;
    msg->ioMsg.handle = handle;
    msg->offset = offset;
    msg->size = size;
    PutMsg(port, &msg1->msg);
    WaitPort(replyPort);
    GetMsg(replyPort);
}

size_t ioRead(IoHandle handle, void *buf, size_t offset, size_t size) {
    struct IoReadMsg *msg = (struct IoReadMsg *)msg1->buf;
    void *buf1;
    DB(printf("ioRead(%lx, 0x%lx, 0x%lx)\n", handle, offset, size);)
    msg->ioMsg.id = ioIdRead;
    msg->ioMsg.handle = handle;
    msg->offset = offset;
    msg->size = size;
    msg2->buf = buf;
    PutMsg(port, &msg1->msg);
    PutMsg(port, &msg2->msg);
    WaitPort(replyPort);
    GetMsg(replyPort);
    WaitPort(replyPort);
    GetMsg(replyPort);
    return msg->size;
}

IoDir ioCurrentDir(IoDir dir) {
    struct IoCurrentDirMsg *msg = (struct IoCurrentDirMsg *)msg1->buf;
    DB(printf("ioCurrentDir(%lx)\n", dir);)
    msg->ioMsg.id = ioIdCurrentDir;
    msg->dir = dir;
    PutMsg(port, &msg1->msg);
    WaitPort(replyPort);
    GetMsg(replyPort);
    DB(printf("ioCurrentDir = %lx\n", msg->dir);)
    return msg->dir;
}

int initClientIo(IoServer server) {
    port = (struct MsgPort *)server;
    replyPort = CreateMsgPort();
    msg1 = (struct MyMessage1 *)AllocMem(sizeof(*msg1), MEMF_PUBLIC|MEMF_CLEAR);
    if (msg1)
	msg1->msg.mn_ReplyPort = replyPort;
    msg2 = (struct MyMessage2 *)AllocMem(sizeof(*msg2), MEMF_PUBLIC|MEMF_CLEAR);
    if (msg2)
	msg2->msg.mn_ReplyPort = replyPort;
    return replyPort && msg1 && msg2;
}

void cleanupClientIo(IoServer) {
    if (msg1)
	FreeMem(msg1, sizeof(*msg1));
    if (msg2)
	FreeMem(msg2, sizeof(*msg2));
    if (replyPort)
	DeleteMsgPort(replyPort);
}


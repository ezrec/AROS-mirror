/*************************************************************************\
 *                                                                       *
 * io_client_ppc.cc                                                      *
 *                                                                       *
 * Copyright 2000-2002 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include <string.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <powerup/ppclib/message.h>
#include <powerup/ppclib/tasks.h>
#include <powerup/ppclib/object.h>
#define strlen ZZstrlen
#include <powerup/gcclib/powerup_protos.h>
#undef strlen
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcproto/dos.h>
#include "apdf/io_client.h"
#include "apdf/io_server.h"

#if 0
#   define DB(x)     x
#   include <stdio.h>
#else
#   define DB(x)
#endif

static void *port;
static void *replyPort;
static void *msg1;
static void *msg2;
static void *data;
static void *data2;

IoHandle ioOpen(const char *name, size_t *size) {
    struct IoOpenMsg *msg = (struct IoOpenMsg *)data;
    DB(printf("ioOpen(%s)\n", name);)
    msg->ioMsg.id = ioIdOpen;
    strncpy(msg->fileName, name, sizeof(msg->fileName) - 1);
    msg->fileName[sizeof(msg->fileName) - 1] = '\0';
    PPCSendMessage(port, msg1, msg, sizeof(*msg), 0);
    PPCWaitPort(replyPort);
    PPCGetMessage(replyPort);
    *size = msg->size;
    DB(printf("ioOpen = %lx, size = %ld\n", msg->ioMsg.handle, msg->size);)
    return msg->ioMsg.handle;
}

void ioClose(IoHandle handle) {
    struct IoMsg *msg = (struct IoMsg *)data;
    DB(printf("ioClose(%lx)\n", handle);)
    msg->id = ioIdClose;
    msg->handle = handle;
    PPCSendMessage(port, msg1, msg, sizeof(*msg), 0);
    PPCWaitPort(replyPort);
    PPCGetMessage(replyPort);
}

void ioQuit(void) {
    struct IoMsg *msg = (struct IoMsg *)data;
    DB(printf("ioQuit()\n");)
    msg->id = ioIdQuit;
    PPCSendMessage(port, msg1, msg, sizeof(*msg), 0);
    PPCWaitPort(replyPort);
    PPCGetMessage(replyPort);
}

void ioPreRead(IoHandle handle, size_t offset, size_t size) {
    struct IoPreReadMsg *msg = (struct IoPreReadMsg *)data;
    DB(printf("ioPreRead(%lx, 0x%lx, 0x%lx)\n", handle, offset, size);)
    msg->ioMsg.id = ioIdPreRead;
    msg->ioMsg.handle = handle;
    msg->offset = offset;
    msg->size = size;
    PPCSendMessage(port, msg1, msg, sizeof(*msg), 0);
    PPCWaitPort(replyPort);
    PPCGetMessage(replyPort);
}

size_t ioRead(IoHandle handle, void *buf, size_t offset, size_t size) {
    struct IoReadMsg *msg = (struct IoReadMsg *)data;
    void *buf1;
    DB(printf("ioRead(%lx, 0x%lx, 0x%lx)\n", handle, offset, size);)
    if (size > 4096)
	buf1 = PPCAllocMem(size, MEMF_PUBLIC);
    else
	buf1 = data2;
    if (buf1) {
	msg->ioMsg.id = ioIdRead;
	msg->ioMsg.handle = handle;
	msg->offset = offset;
	msg->size = size;
	PPCSendMessage(port, msg1, msg, sizeof(*msg), 0);
	PPCSendMessage(port, msg2, buf1, size, 0);
	PPCWaitPort(replyPort);
	PPCGetMessage(replyPort);
	PPCWaitPort(replyPort);
	PPCGetMessage(replyPort);
	memcpy(buf, buf1, size);
	if (buf1 != data2)
	    PPCFreeMem(buf1, size);
	DB(printf("ioRead = %ld\n", msg->size);)
	return msg->size;
    }
    DB(printf("ioRead = -1\n");)
    return (size_t)-1;
}

IoDir ioCurrentDir(IoDir dir) {
    struct IoCurrentDirMsg *msg = (struct IoCurrentDirMsg *)data;
    DB(printf("ioCurrentDir(%lx)\n", dir);)
    msg->ioMsg.id = ioIdCurrentDir;
    msg->dir = dir;
    PPCSendMessage(port, msg1, msg, sizeof(*msg), 0);
    PPCWaitPort(replyPort);
    PPCGetMessage(replyPort);
    DB(printf("ioCurrentDir = %lx\n", msg->dir);)
    return msg->dir;
}

int initClientIo(IoServer server) {
    port = server;
    replyPort = PPCCreatePort(NULL);
    msg1 = PPCCreateMessage(replyPort, 1024);
    msg2 = PPCCreateMessage(replyPort, 4096);
    data = PPCAllocMem(1024, MEMF_PUBLIC);
    data2 = PPCAllocMem(4096, MEMF_PUBLIC);
    return replyPort && msg1 && msg2 && data && data2;
}

void cleanupClientIo(IoServer) {
    if (data)
	PPCFreeMem(data, 1024);
    if (data2)
	PPCFreeMem(data2, 4096);
    if (msg1)
	PPCDeleteMessage(msg1);
    if (msg2)
	PPCDeleteMessage(msg2);
    if (replyPort)
	PPCDeletePort(replyPort);
}


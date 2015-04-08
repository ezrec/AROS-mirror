/*************************************************************************\
 *                                                                       *
 * io_client.h                                                           *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef IO_CLIENT_H
#define IO_CLIENT_H

typedef void *IoHandle;
typedef void *IoServer;
typedef long IoDir;

IoDir ioCurrentDir(IoDir);
IoHandle ioOpen(const char *,size_t*);
void ioClose(IoHandle);
void ioPreRead(IoHandle, size_t offset, size_t size);
size_t ioRead(IoHandle, void *buf, size_t offset, size_t size);
void ioQuit(void);

IoServer initIo(void);
int initClientIo(IoServer);
void cleanupIo(IoServer);
void cleanupClientIo(IoServer);
void startServer(IoServer);

#endif


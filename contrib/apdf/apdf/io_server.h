/*************************************************************************\
 *                                                                       *
 * io_server.c                                                           *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef IO_SERVER_H
#define IO_SERVER_H

#include "io_client.h"

enum IoID {
    ioIdQuit, ioIdOpen, ioIdClose, ioIdRead, ioIdPreRead, ioIdAbort, ioIdCurrentDir
};


struct IoMsg {
    void *container;
    enum IoID id;
    IoHandle handle;
    int success;
};

struct IoOpenMsg {
    struct IoMsg ioMsg;
    int size;
    char fileName[256];
};

struct IoCloseMsg {
    struct IoMsg ioMsg;
};

struct IoPreReadMsg {
    struct IoMsg ioMsg;
    size_t offset;
    size_t size;
};

struct IoReadMsg {
    struct IoMsg ioMsg;
    size_t offset;
    size_t size;
};

struct IoCurrentDirMsg {
    struct IoMsg ioMsg;
    IoDir dir;
};


#endif

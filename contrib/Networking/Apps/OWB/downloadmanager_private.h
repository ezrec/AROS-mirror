#ifndef _DOWNLOADMANAGER_PRIVATE_H_
#define _DOWNLOADMANAGER_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <time.h>

struct Download {
    struct Node node;
    long long sizeTotal;
    long long sizeDownloaded;
    char *filename;
    char *filesize;
    int state;
    void *identifier;
    char progress[8];
    Object *manager;
    char *speed;
    time_t speedMeasureTime;
    long long speedSizeDownloaded;
};

struct DownloadManager_DATA {
    struct List downloads;
    Object *window;
    Object *list;
    struct Hook destroyHook;
    struct Hook displayHook;
    struct Hook cancelHook;
    struct Hook cancelAllHook;
    struct Hook clearFinishedHook;
    Object *preferences;
    Object *bt_cancel;
};

#endif

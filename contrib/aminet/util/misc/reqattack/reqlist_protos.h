#ifndef REQLIST_PROTOS_H
#define REQLIST_PROTOS_H

#ifndef REQLIST_H
#include "reqlist.h"
#endif

void InitReqList(void);
void CleanupReqList(void);
struct ReqNode *NewReqNode(void);
void FreeReqNode(struct ReqNode *node);
void AddReqNode(struct ReqNode *node);
void RemoveReqNode(struct ReqNode *node);
struct ReqNode *FindReqNode(struct Window *win);
BOOL ReqListEmpty(void);

#endif


#ifndef CONFIG_PROTOS_H
#define CONFIG_PROTOS_H

#ifndef REQLIST_H
#include "reqlist.h"
#endif

void InitConfig(void);
void CleanupConfig(void);
void ReadConfig(void);
void InitExecuteConfig(void);
void ExecuteConfig(struct ReqNode *reqnode);
void DoneExecuteConfig(void);
void HandleConfig(void);

#endif


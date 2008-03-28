#include "../common/nsmtracker.h"

extern void (*Ptask2MtaskCallBack)(void);

void Ptask2Mtask(void){
  (*Ptask2MtaskCallBack)();
}

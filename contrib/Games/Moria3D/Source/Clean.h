/****************************************************************************
*                                                                           *
* Clean.h                                                           970319  *
*                                                                           *
* Ensure that everything is cleaned up on exit                              *
*                                                                           *
****************************************************************************/

#ifndef CLEAN_H
#define CLEAN_H

#include "MyTypes.h"

typedef struct CleanNode * pCleanNode;
typedef struct CleanNode
    {
        pCleanNode   Next;
        pCleanNode   Prev;
        int32        Type;
        void       * Address;
        uint32       Size;
    } CleanNode;

#define CleanUpNone   0
#define CleanUpLib    1
#define CleanUpScreen 2
#define CleanUpWindow 3
#define CleanUpMem    4
#define CleanUpBitMap 5

extern void         InitCleanUp(void);
extern pCleanNode   EnterCleanNode(int32 Type, void * Address, uint32 Size);
extern void         RemoveCleanNode(pCleanNode Node);
extern void         CleanUpNodes(void);
extern void         CleanUpNode(pCleanNode);
extern void       * MemAlloc(int32 Size);
extern void         MemFree(void * Address);
extern void         CleanExit(char * ErrorStr);
extern void         CheckCleanUp(void);

#endif


#ifndef SETMAN_H
#define SETMAN_H 1

#include "amiga-align.h"

struct SetManPort {
    struct MsgPort sp_MsgPort;
    UBYTE sp_Version[4];
    struct SignalSemaphore sp_Semaphore;
    struct MinList sp_PatchList;
};

#define SETMANPORT_NAME  "SetMan"
#define SETMAN_VERSION   "2.0"

struct SetManNode {
    struct MinNode sn_Node;
    struct Library *sn_Library;
    LONG sn_Offset;
    struct Task *sn_Task;
    UBYTE sn_TaskName[22];
    UWORD sn_Jmp1;
    APTR sn_SetFunc;
    UWORD sn_Jmp2;
    APTR sn_PrevFunc;
    APTR sn_NextFunc;
};

#define JMPCODE 0x4ef9
#define BRACODE 0x6004
#define NOPCODE 0x4e71

#include "default-align.h"

#endif /* SETMAN_H */


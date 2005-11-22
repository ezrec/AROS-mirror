#ifndef _AROS_STUFF_H
#define _AROS_STUFF_H

#ifndef __AROS__
#define dl_OldName dl_Name
#endif

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/filehandler.h>
#include <devices/timer.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <utility/date.h>

#define AROS_BSTR_ADDR(s)        (((STRPTR)BADDR(s))+1)
#define AROS_BSTR_strlen(s)      (AROS_BSTR_ADDR(s)[-1])
#define AROS_BSTR_getchar(s,l)   (AROS_BSTR_ADDR(s)[l])

struct ASFSHandle {
        void *handle;   /* pointer to FileHandle or Lock */
        ULONG flags;
        struct ASFSDeviceInfo *device;
};

#define AHF_IS_LOCK (1<<0)
#define AHF_IS_FH   (1<<1)

struct ASFSDeviceInfo {
        struct MsgPort *taskmp;
        struct FileSysStartupMsg fssm;
        struct Globals *global;
        struct ASFSHandle rootfh;
};

#endif


#include <libcore/compiler.h>
#ifndef NULL
#define NULL ((void *)0)
#endif

#include "libdefs.h"

extern void AROS_SLIB_ENTRY(LC_BUILDNAME(OpenLib),LibHeader) (void);
extern void AROS_SLIB_ENTRY(LC_BUILDNAME(CloseLib),LibHeader) (void);
extern void AROS_SLIB_ENTRY(LC_BUILDNAME(ExpungeLib),LibHeader) (void);
extern void AROS_SLIB_ENTRY(LC_BUILDNAME(ExtFuncLib),LibHeader) (void);
extern void AROS_SLIB_ENTRY(XProtocolCleanup, BASENAME) (void); /* 5 */
extern void AROS_SLIB_ENTRY(XProtocolSetup, BASENAME) (void);
extern void AROS_SLIB_ENTRY(XProtocolSend, BASENAME) (void);
extern void AROS_SLIB_ENTRY(XProtocolReceive, BASENAME) (void);
extern void AROS_SLIB_ENTRY(XProtocolHostMon, BASENAME) (void);
extern void AROS_SLIB_ENTRY(XProtocolUserMon, BASENAME) (void);

void *const LIBFUNCTABLE[]=
{
    AROS_SLIB_ENTRY(LC_BUILDNAME(OpenLib),LibHeader),
    AROS_SLIB_ENTRY(LC_BUILDNAME(CloseLib),LibHeader),
    AROS_SLIB_ENTRY(LC_BUILDNAME(ExpungeLib),LibHeader),
    AROS_SLIB_ENTRY(LC_BUILDNAME(ExtFuncLib),LibHeader),
    AROS_SLIB_ENTRY(XProtocolCleanup, BASENAME), /* 5 */
    AROS_SLIB_ENTRY(XProtocolSetup, BASENAME),
    AROS_SLIB_ENTRY(XProtocolSend, BASENAME),
    AROS_SLIB_ENTRY(XProtocolReceive, BASENAME),
    AROS_SLIB_ENTRY(XProtocolHostMon, BASENAME),
    AROS_SLIB_ENTRY(XProtocolUserMon, BASENAME),
    (void *)-1L
};

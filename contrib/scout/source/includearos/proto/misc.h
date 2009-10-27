#ifndef  CLIB_MISC_PROTOS_H
#define  CLIB_MISC_PROTOS_H

#include <exec/types.h>

UBYTE *AllocMiscResource( ULONG unitNum, CONST_STRPTR name );
VOID FreeMiscResource( ULONG unitNum );

#endif

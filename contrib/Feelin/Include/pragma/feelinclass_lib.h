#ifndef _PRAGMA_FEELINCLASS_LIB_H
#define _PRAGMA_FEELINCLASS_LIB_H

/*
**    $VER: <pragma/feelinclass_lib.h> 8.0 (2004/12/18)
**
**    C pragmas definitions
**
**    © 2001-2004 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#ifndef CLIB_FEELINCLASS_PROTOS_H
#include <clib/feelinclass_protos.h>
#endif

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM)
#pragma amicall(FeelinClassBase,0x01e,F_Query(d0,a0))
#endif

#if defined(_DCC) || defined(__SASC)
#pragma libcall FeelinClassBase F_Query 1e 8002
#endif

#endif /* _PRAGMA_FEELINCLASS_LIB_H */

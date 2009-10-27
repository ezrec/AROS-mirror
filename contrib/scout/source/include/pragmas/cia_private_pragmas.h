#ifndef PRAGMAS_CIA_PRIVATE_PRAGMAS_H
#define PRAGMAS_CIA_PRIVATE_PRAGMAS_H

/*
**  $VER: cia_private_pragmas.h 40.1 (17.5.1996)
**
**  Direct ROM interface (pragma) definitions.
**
**  Copyright © 2001 Amiga, Inc.
**      All Rights Reserved
*/

#if defined(LATTICE) || defined(__SASC) || defined(_DCC)
#ifndef __CLIB_PRAGMA_LIBCALL
#define __CLIB_PRAGMA_LIBCALL
#endif /* __CLIB_PRAGMA_LIBCALL */
#else /* __MAXON__, __STORM__ or AZTEC_C */
#ifndef __CLIB_PRAGMA_AMICALL
#define __CLIB_PRAGMA_AMICALL
#endif /* __CLIB_PRAGMA_AMICALL */
#endif /* */

#if defined(__SASC) || defined(__STORM__)
#ifndef __CLIB_PRAGMA_TAGCALL
#define __CLIB_PRAGMA_TAGCALL
#endif /* __CLIB_PRAGMA_TAGCALL */
#endif /* __MAXON__, __STORM__ or AZTEC_C */

#ifndef CLIB_CIA_PROTOS_H
#include <clib/cia_protos.h>
#endif /* CLIB_CIA_PROTOS_H */

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
    #pragma amicall(CiaBase,0x006,AddICRVector(a6,d0,a1))
    #pragma amicall(CiaBase,0x00C,RemICRVector(a6,d0,a1))
    #pragma amicall(CiaBase,0x012,AbleICR(a6,d0))
    #pragma amicall(CiaBase,0x018,SetICR(a6,d0))
#endif
#if defined(_DCC) || defined(__SASC)
    #pragma  libcall CiaBase AddICRVector         006 90E03
    #pragma  libcall CiaBase RemICRVector         00C 90E03
    #pragma  libcall CiaBase AbleICR              012 0E02
    #pragma  libcall CiaBase SetICR               018 0E02
#endif

#endif /* PRAGMAS_CIA_PRIVATE_PRAGMAS_H */

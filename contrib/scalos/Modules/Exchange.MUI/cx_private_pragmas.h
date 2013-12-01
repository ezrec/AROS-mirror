// cx_private_pragmas.h
// $Date$
// $Revision$

#ifndef _INCLUDE_PRAGMA_COMMODITIES_PRIVATE_LIB_H
#define _INCLUDE_PRAGMA_COMMODITIES_PRIVATE_LIB_H

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
#pragma amicall(CxBase,0x0ba,CxCopyBrokerList(a0))
#pragma amicall(CxBase,0x0c0,CxFreeBrokerList(a0))
#pragma amicall(CxBase,0x0c6,CxBrokerCommand(a0,d0))
#endif
#if defined(_DCC) || defined(__SASC)
#pragma  libcall CxBase CxCopyBrokerList       0ba 801
#pragma  libcall CxBase CxFreeBrokerList       0c0 801
#pragma  libcall CxBase CxBrokerCommand        0c6 0802
#endif

#ifndef CLIB_COMMODITIES_PRIVATE_PROTOS_H
#include "cx_private_protos.h"
#endif

#endif	/*  _INCLUDE_PRAGMA_COMMODITIES_PRIVATE_LIB_H  */


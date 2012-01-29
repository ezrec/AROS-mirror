#ifndef _INCLUDE_PRAGMA_MUIBUILDER_LIB_H
#define _INCLUDE_PRAGMA_MUIBUILDER_LIB_H

#ifndef CLIB_MUIBUILDER_PROTOS_H
#include <clib/muibuilder_protos.h>
#endif

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
#pragma amicall(MUIBBase,0x01e,MB_Open())
#pragma amicall(MUIBBase,0x024,MB_Close())
#pragma amicall(MUIBBase,0x02a,MB_GetA(a1))
#pragma amicall(MUIBBase,0x030,MB_GetVarInfoA(d0,a1))
#pragma amicall(MUIBBase,0x036,MB_GetNextCode(a0,a1))
#pragma amicall(MUIBBase,0x03c,MB_GetNextNotify(a0,a1))
#endif
#if defined(_DCC) || defined(__SASC)
#pragma  libcall MUIBBase MB_Open                01e 00
#pragma  libcall MUIBBase MB_Close               024 00
#pragma  libcall MUIBBase MB_GetA                02a 901
#pragma  libcall MUIBBase MB_GetVarInfoA         030 9002
#pragma  libcall MUIBBase MB_GetNextCode         036 9802
#pragma  libcall MUIBBase MB_GetNextNotify       03c 9802
#endif
#ifdef __STORM__
#pragma tagcall(MUIBBase,0x02a,MB_Get(a1))
#pragma tagcall(MUIBBase,0x030,MB_GetVarInfo(d0,a1))
#endif
#ifdef __SASC_60
#pragma  tagcall MUIBBase MB_Get                 02a 901
#pragma  tagcall MUIBBase MB_GetVarInfo          030 9002
#endif

#endif	/*  _INCLUDE_PRAGMA_MUIBUILDER_LIB_H  */

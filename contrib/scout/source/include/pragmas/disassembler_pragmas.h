#ifndef _INCLUDE_PRAGMA_DISASSEMBLER_LIB_H
#define _INCLUDE_PRAGMA_DISASSEMBLER_LIB_H

#ifndef CLIB_DISASSEMBLER_PROTOS_H
#include <clib/disassembler_protos.h>
#endif

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
#pragma amicall(DisassemblerBase,0x042,Disassemble(a0))
#pragma amicall(DisassemblerBase,0x048,FindStartPosition(a0,d0,d1))
#endif
#if defined(_DCC) || defined(__SASC)
#pragma  libcall DisassemblerBase Disassemble          042 801
#pragma  libcall DisassemblerBase FindStartPosition    048 10803
#endif

#endif	/*  _INCLUDE_PRAGMA_DISASSEMBLER_LIB_H  */

/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_DISASSEMBLER_H
#define _PPCINLINE_DISASSEMBLER_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef DISASSEMBLER_BASE_NAME
#define DISASSEMBLER_BASE_NAME DisassemblerBase
#endif /* !DISASSEMBLER_BASE_NAME */

#define Disassemble(__p0) \
	LP1(66, APTR , Disassemble, \
		struct DisData *, __p0, a0, \
		, DISASSEMBLER_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define FindStartPosition(__p0, __p1, __p2) \
	LP3(72, APTR , FindStartPosition, \
		APTR , __p0, a0, \
		UWORD , __p1, d0, \
		UWORD , __p2, d1, \
		, DISASSEMBLER_BASE_NAME, 0, 0, 0, 0, 0, 0)

#endif /* !_PPCINLINE_DISASSEMBLER_H */

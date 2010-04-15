/* Automatically generated header! Do not edit! */

#ifndef _INLINE_DISASSEMBLER_H
#define _INLINE_DISASSEMBLER_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef DISASSEMBLER_BASE_NAME
#define DISASSEMBLER_BASE_NAME DisassemblerBase
#endif /* !DISASSEMBLER_BASE_NAME */

#define Disassemble(___data) \
	AROS_LC1(APTR, Disassemble, \
	AROS_LCA(struct DisData *, (___data), A0), \
	struct Library *, DISASSEMBLER_BASE_NAME, 11, /* s */)

#define FindStartPosition(___startpc, ___min, ___max) \
	AROS_LC3(APTR, FindStartPosition, \
	AROS_LCA(APTR, (___startpc), A0), \
	AROS_LCA(UWORD, (___min), D0), \
	AROS_LCA(UWORD, (___max), D1), \
	struct Library *, DISASSEMBLER_BASE_NAME, 12, /* s */)

#endif /* !_INLINE_DISASSEMBLER_H */

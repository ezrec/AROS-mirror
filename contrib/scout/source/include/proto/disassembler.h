/* Automatically generated header! Do not edit! */

#ifndef PROTO_DISASSEMBLER_H
#define PROTO_DISASSEMBLER_H

#ifndef __NOLIBBASE__
extern struct Library *
#ifdef __CONSTLIBBASEDECL__
__CONSTLIBBASEDECL__
#endif /* __CONSTLIBBASEDECL__ */
DisassemblerBase;
#endif /* !__NOLIBBASE__ */

#ifdef __amigaos4__
#include <interfaces/disassembler.h>
#ifdef __USE_INLINE__
#include <inline4/disassembler.h>
#endif /* __USE_INLINE__ */
#ifndef CLIB_DISASSEMBLER_PROTOS_H
#define CLIB_DISASSEMBLER_PROTOS_H
#endif /* CLIB_DISASSEMBLER_PROTOS_H */
#ifndef __NOGLOBALIFACE__
extern struct DisassemblerIFace *IDisassembler;
#endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
#include <clib/disassembler_protos.h>
#ifdef __GNUC__
#ifdef __AROS__
#ifndef NOLIBDEFINES
#ifndef DISASSEMBLER_NOLIBDEFINES
#include <defines/disassembler.h>
#endif /* DISASSEMBLER_NOLIBDEFINES */
#endif /* NOLIBDEFINES */
#else
#ifdef __PPC__
#ifndef _NO_PPCINLINE
#include <ppcinline/disassembler.h>
#endif /* _NO_PPCINLINE */
#else
#ifndef _NO_INLINE
#include <inline/disassembler.h>
#endif /* _NO_INLINE */
#endif /* __PPC__ */
#endif /* __AROS__ */
#else
#include <pragmas/disassembler_pragmas.h>
#endif /* __GNUC__ */
#endif /* __amigaos4__ */

#endif /* !PROTO_DISASSEMBLER_H */

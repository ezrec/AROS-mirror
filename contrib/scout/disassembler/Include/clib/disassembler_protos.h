#ifndef  CLIB_DISASSEMBLER_PROTOS_H
#define  CLIB_DISASSEMBLER_PROTOS_H

/*
**      $VER: disassembler_protos.h 41.0 (20.04.2010)
**      Includes Release 41.0
**
**      C prototypes.
**
**      © 1999 THOR-Software (Thomas Richter)
**	© 2010 Pavel Fedin
**
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LIBRARIES_DISASSEMBLER_H
#include <libraries/disassembler.h>
#endif

APTR Disassemble(struct DisData *ds);
APTR FindStartPosition(APTR startpc,UWORD min, UWORD max);

#endif


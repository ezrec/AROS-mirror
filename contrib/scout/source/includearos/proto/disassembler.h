#ifndef  CLIB_DISASSEMBLER_PROTOS_H
#define  CLIB_DISASSEMBLER_PROTOS_H

/*
**      $VER: disassembler_protos.h 40.2 (31.10.99)
**      Includes Release 40.2
**
**      C prototypes.
**
**      © 1999 THOR-Software (Thomas Richter)
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


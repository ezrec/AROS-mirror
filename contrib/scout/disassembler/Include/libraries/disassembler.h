/*************************************************************************
 ** disassembler.library                                                **
 **                                                                     **
 ** a simple system library for disassembling a86K binaries             **
 **                                                                     **
 ** © 1999 THOR-Software, Thomas Richter                                **
 ** No commercial use, reassembly, modification without prior, written  **
 ** permission of the authors.                                          **
 ** Including this library in any commercial software REQUIRES a        **
 ** written permission and the payment of a small fee.                  **
 **                                                                     **
 **---------------------------------------------------------------------**
 ** Definition of the library, and structures                           **
 **									**
 ** $VER: 40.2 (31.10.99) © THOR                       			**
 *************************************************************************/

#ifndef LIBRARIES_DISASSEMBLER_H
#define LIBRARIES_DISASSEMBLER_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

/* There's really nothing in this library base you need to care about */

struct DisassemblerBase {
        struct Library  dllib_Library;
        /* more below this point */
};

#define DISASSEMBLER_NAME "disassembler.library"


/* The one and only structure you need to care about. This one is
   passed in to Disassemble() */

struct DisData {
        APTR            ds_From;        /* base to start disassembly from */
        APTR            ds_UpTo;        /* where to stop it */
        APTR            ds_PC;          /* the disassembler prints a "*" here */
        void           *ds_PutProc;     /* to avoid casts, this is a void *. */
        APTR            ds_UserData;    /* passed to the PutProc in a1 and a3 */
        APTR            ds_UserBase;    /* passed to the PutProc in a4 */
        UWORD           ds_Truncate;    /* limit width of the output */
        UWORD           ds_reserved;    /* this is currently reserved */
};

/* PutProc is actually a function pointer, but due to different compilers
   and conventions defined here as a void *. It is called with the
   character to print in d0, the user data in a1 and a3 and the user
   base in a4. This is most useful for C code.

   The library functions are interrupt-callable, provided your PutProc
   is.
*/

#endif


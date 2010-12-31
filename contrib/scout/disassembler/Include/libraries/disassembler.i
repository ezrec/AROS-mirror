;*************************************************************************
;** disassembler.library                                                **
;**                                                                     **
;** An LGPL reimplementation of disassembler.library v40                **
;** © 2010 Pavel Fedin                                                  **
;**                                                                     **
;** disassembler.libary v40 is:                                         **
;** © 1999 THOR-Software, Thomas Richter                                **
;**---------------------------------------------------------------------**
;** Definition of the library, and structures                           **
;**									**
;** $VER: 41.0 (20.04.2010)                        			**
;*************************************************************************/

        IFND LIBRARIES_DISASSEMBLER_I

LIBRARIES_DISASSEMBLER_I SET 1

        IFND EXEC_TYPES_I
        INCLUDE "exec/types.i"
        ENDC ; EXEC_TYPES_I

        IFND EXEC_LIBRARIES_I
        INCLUDE "exec/libraries.i"
        ENDC ; EXEC_LIBRARIES_I

        ;* There's really nothing in this library base you need to care about *

        STRUCTURE	DisassemblerBase,LIB_SIZE
        LABEL		dllib_SIZE


DISASSEMBLERNAME        MACRO
                        DC.B 'disassembler.library',0
                        ENDM

;* The one and only structure you need to care about. This one is
;*  passed in to Disassemble() *

        STRUCTURE DisData,0
                APTR  ds_From         ;* base to start disassembly from *
                APTR  ds_UpTo         ;* where to stop it *
                APTR  ds_PC           ;* the disassembler prints a "*" here *
                APTR  ds_PutProc      ;* to avoid casts, this is a void *. *
                APTR  ds_UserData     ;* passed to the PutProc in a1 and a3 *
                APTR  ds_UserBase     ;* passed to the PutProc in a4 *
                UWORD ds_Truncate     ;* limit width of the output *
                UWORD ds_reserved     ;* this is currently reserved *
        LABEL ds_SIZE

;* PutProc is actually a function pointer, but due to different compilers
;* and conventions defined here as a void *. It is called with the
;* character to print in d0, the user data in a1 and a3 and the user
;* base in a4. This is most useful for C code.

;* The library functions are interrupt-callable, provided your PutProc
;* is.

        ENDC    ;LIBRARIES_DISASSEMBLER_I


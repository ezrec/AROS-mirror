;
; @(#) $Header$
;
; BGUI library
; stkext.asm
; Generic stack extension routines.
;
; (C) Janne Jalkanen 1998
; (C) Copyright 1998 Manuel Lemos.
; (C) Copyright 1996-1997 Ian J. Einman.
; (C) Copyright 1993-1996 Jaba Development.
; (C) Copyright 1993-1996 Jan van den Baard.
; All Rights Reserved.
;
; $Log$
; Revision 42.0  2000/05/09 22:10:18  mlemos
; Bumped to revision 42.0 before handing BGUI to AROS team
;
; Revision 41.11  2000/05/09 20:21:53  mlemos
; Bumped to revision 41.11
;
; Revision 41.1  2000/05/09 19:55:11  mlemos
; Merged with the branch Manuel_Lemos_fixes.
;
; Revision 1.1.2.5  1998/10/12 01:31:17  mlemos
; Fixed a remaining make-proto word in the previous log.
; Changed the make-proto declaration for EnsureStack function.
;
; Revision 1.1.2.4  1998/10/11 15:39:35  mlemos
; Split make-proto words in the log comment to not confuse make-proto program.
; Added make-proto declarations for EnsureStack and RevertStack to let them
; be called from C code.
;
; Revision 1.1.2.3  1998/10/01 23:03:12  mlemos
; Replaced a C like comment that was confusing make-proto by a semi-colon.
; Defined functions exported to C by make-proto as regargs.
;
; Revision 1.1.2.2  1998/10/01 04:37:01  mlemos
; Made the calls to exec.library be done by fetching the library base pointer
; from _SysBase(a4).
; Increased the stack margin size to 2048 bytes.
; Added code to pre-allocate and free the stack space for use by the
; input.device task.
; Removed the stack space copying code.
; Fixed the bug of post incrementing the register a5 after restoring pushed
; register from the previous stack space.
;
; Revision 1.1.2.1  1998/09/12 02:21:22  mlemos
; Initial revision.
;
;
        INCLUDE 'exec/exec_lib.i'
        INCLUDE 'exec/tasks.i'
        INCLUDE 'exec/memory.i'

        XDEF    _EnsureStack,_RevertStack
        XREF    _SysBase

STKMARGIN       EQU   2048      ; When should we start worrying about the stack?
STKSIZE         EQU   16384     ; Allocate this much stack space
CTLBLOCK        EQU   4*4       ; The control block, currently sizeof(StackSwapStruct)

callsys MACRO
        move.l  _SysBase(a4),a6
        jsr     _LVO\1(a6)
        ENDM

call    MACRO
        jsr     _LVO\1(a6)
        ENDM

        SECTION text,CODE

*******************************************************************************
*
*  EnsureStack allocates a handle, which must be returned back to RevertStack
*
*  Registers preserved: all, except d0 and sp
*
        XREF    _InputDevice
        XREF    _InputStack

;makeproto ASM APTR EnsureStack(VOID);

_EnsureStack:
        movem.l d1-d7/a0-a6,-(sp)   ; total of 14 regs = 56 bytes

; Figure out our address

        suba.l  a1,a1
        callsys FindTask            ; a2=FindTask(NULL)
        move.l  d0,a2

; Calculate current stack left and check if it is within
; allowed limits

        move.l  TC_SPLOWER(a2),d2   ; d2 = oldlower
        move.l  sp,d3
        sub.l   d2,d3               ; d3 = stkptr-oldlower

        moveq   #0,d0               ; d0 = NULL ; THis is the default return value
        cmp.l   #STKMARGIN,d3
        bhi     .stackok

; Check if this is input.device

        cmpa.l  _InputDevice(a4),a2
        bne     .allocmore

; if current stack space is already the pre-allocated stack, allocate a stack extension

        move.l  _InputStack(a4),d0
        moveq.l #CTLBLOCK,d1
        add.l   d0,d1
        cmp.l   d1,d2
        bne     .dontalloc

; Allocate the new stack and the control block (20 bytes extra)

.allocmore
        move.l  #STKSIZE+CTLBLOCK,d0
        move.l  #MEMF_CLEAR+MEMF_PUBLIC,d1
        call    AllocVec
        tst.l   d0
        beq     .memfailed

; Allocation was successfull, so we'll fill out the necessary data
; in the control block.

.dontalloc
        move.l  d0,a3               ; a3 = control block address
        add.l   #CTLBLOCK,d0
        move.l  d0,stk_Lower(a3)    ; start of real stack (ctrl block not included)

        add.l   #STKSIZE,d0
        move.l  d0,stk_Upper(a3)    ; end of stack

        move.l  TC_SPUPPER(a2),d1
        sub.l   sp,d1               ; d1 = stacksize = tc_SPUpper - stkptr

        sub.l   d1,d0               ; d0 = stk_Upper - stackoffset
        move.l  d0,stk_Pointer(a3)

; Copy the return address.

        move.l  d0,a1
        move.l  56(a7),(a1)         ; Copy return address to new stack

; Finally, do StackSwap()
        move.l  sp,a5               ; Old stack pointer so that we can easily
                                    ; restore registers on exit
        move.l  a3,a0
        call    StackSwap

        move.l  a3,d0               ; d0 = return value

        movem.l (a5),d1-d7/a0-a6   ; Restore registers and return
        rts

.memfailed

.stackok

; return
        movem.l (sp)+,d1-d7/a0-a6
        rts
 
*******************************************************************************
*
*   This function returns the stack back to free memory pool.
*   Safe to call with NULL args
*
*   Registers destroyed: d0/a0/a1/a7

;makeproto ASM VOID RevertStack( REG(a0) APTR);

_RevertStack
        move.l  a0,d0
        tst.l   d0
        beq     .noalloc

        movem.l d1-d7/a2-a6,-(sp)   ; 12 registers = 48 bytes

; Revert to original stack
        move.l  d0,a3               ; a3 = control block
        move.l  a3,a0

        callsys StackSwap

        move.l  stk_Pointer(a3),a5  ; a5 = our allocated stack pointer (now old)
        move.l  48(a5),a0           ; a0 = return address

        lea     48+12(sp),sp        ; forget all previous stuff
        move.l  a0,-(sp)            ; push real return address

        move.l  a3,a1               ; a1 = memory block area

        cmp.l   _InputStack(a4),a1
        beq     .nofree

        callsys FreeVec             ; free memory in a1
.nofree
        movem.l (a5),d1-d7/a2-a6   ; Return real registers
.noalloc
        rts

********************************************************************************
*
*   This function allocates memory for the input.device stack extension,
*   which is allocated only during library initialization.
*
;makeproto VOID InitInputStack(VOID);

        XDEF @InitInputStack
        XDEF @FreeInputStack

@InitInputStack
        movem.l a6,-(sp)

; Find input.device address

        lea     InputName(pc),a1
        callsys FindTask
        move.l  d0,_InputDevice(a4)  ; Save to globals.

; Allocate stack
        move.l  #STKSIZE+CTLBLOCK,d0
        move.l  #MEMF_CLEAR+MEMF_PUBLIC,d1
        call    AllocVec
        tst.l   d0
        move.l  d0,_InputStack(a4)

        movem.l (sp)+,a6
        rts

;makeproto VOID FreeInputStack(VOID);

@FreeInputStack
        movem.l a6,-(sp)

        move.l  _InputStack(a4),a1
        callsys FreeVec             ; NULL is safe, says autodoc

        movem.l (sp)+,a6
        rts

InputName
        dc.b    "input.device",0

        END

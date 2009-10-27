**
* Scout - The Amiga System Monitor
*
*------------------------------------------------------------------
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
* You must not use this source code to gain profit of any kind!
*
*------------------------------------------------------------------
*
* @author Andreas Gelhausen
* @author Richard Körber <rkoerber@gmx.de>
*



        include "classes/scout_asm.i"
        include "exec/funcdef.i"
        include "exec/exec_lib.i"
        include "lvo/mmu_lvo.i"
        include "utility/utility_lib.i"

        SECTION CODE

        xdef _MyCause
_MyCause:
        ;=================================
        ; causes software interrupt
        ;=================================
        ;  > A0: Passed to SoftInt
        ;  > A1: Int-Structure
        ;  < D0: Return Code
        movem.l a0-a6/d1-d7,-(sp)
        move.l  $4.w,a6
        move.l  IS_CODE(a1),a2
        move.l  IS_DATA(a1),a1
        jsr     (a2)
        movem.l (sp)+,a0-a6/d1-d7
        rts


        xdef _GetCACR
_GetCACR:
        ;=================================
        ;return current value of CACR in d0
        ;=================================

        movem.l a5/a6,-(SP)
        moveq.l #0,d0
        move.l  _AbsExecBase,a6
        move.w  AttnFlags(a6),d1
        btst    #AFB_68020,d1
        beq.s   _GetCACR_Exit

        lea     CACR_to_d0(PC),a5
        jsr     _LVOSupervisor(a6)
_GetCACR_Exit:
        movem.l (SP)+,a5/a6
        rts

CACR_to_d0:
        movec.l CACR,D0
        rte


        xdef _GetPCR
_GetPCR:
        ;=================================
        ;return current value of PCR in d0
        ;=================================

        movem.l a5/a6,-(SP)
        moveq.l #0,d0
        move.l  _AbsExecBase,a6
        move.w  AttnFlags(a6),d1
        btst    #AFB_68060,d1
        beq.s   _GetPCR_Exit

        lea     PCR_to_d0(PC),a5
        jsr     _LVOSupervisor(a6)
_GetPCR_Exit:
        movem.l (SP)+,a5/a6
        rts

PCR_to_d0:
        dc.w    $4E7A,$0808             ; movec.l PCR,D0
        rte



        xdef _GetVBR

_GetVBR:
        ;=================================
        ;return current value of VBR in d0
        ;=================================

        movem.l a5/a6,-(SP)
        moveq.l  #0,d0
        move.l  _AbsExecBase,a6
        move.w  AttnFlags(a6),d1
        btst    #AFB_68010,d1
        beq.s    _GetVBR_Exit

        lea     VBR_to_d0(PC),a5
        jsr     _LVOSupervisor(a6)

_GetVBR_Exit:
   movem.l      (SP)+,a5/a6
        rts

VBR_to_d0:
        movec.l VBR,D0
        rte


        xdef _ReadZeroPage
        xref _MMUBase

_ReadZeroPage:
        movem.l a5-a6,-(sp)
        move.l  _MMUBase,a6
        lea     _ReadPage(pc),a5
        jsr     _LVOWithoutMMU(a6)
        move.l  d0,(a1)
        move.l  d1,(a2)
        movem.l (sp)+,a5-a6
        rts

_ReadPage:
        move.l  $100(a0),d0     ;$100(a0),d0??
        move.l  $104(a0),d1     ;$104(a0),d1??
        rts


        xdef _InstallPatches
        xref _AbsExecBase

_InstallPatches:
        movem.l d0-d4/a0-a6,-(sp)
        move.l  _AbsExecBase,a6
        jsr     _LVODisable(a6)

        lea     _PatchesInstalled(pc),a0
        tst.b   (a0)
        bne.s   InstallPatches_Exit

        move.w  #-54,d0
        lea     _MySwitch(pc),a0
        exg.l   d0,a0
        move.l  a6,a1
        jsr     _LVOSetFunction(a6)
        lea     _OldSwitch(pc),a0
        move.l  d0,(a0)

        move.w  #_LVOAddTask,d0
        lea     _MyAdd(pc),a0
        exg.l   d0,a0
        move.l  a6,a1
        jsr     _LVOSetFunction(a6)
        lea     _OldAddTask(pc),a0
        move.l  d0,(a0)

        lea     _PatchesInstalled(pc),a0
        move.b  #$ff,(a0)

        jsr     _LVOCacheClearU(a6)

InstallPatches_Exit:
        jsr     _LVOEnable(a6)
        movem.l (sp)+,d0-d4/a0-a6
        rts


        xdef _UninstallPatches
        xref _AbsExecBase

_UninstallPatches:
        movem.l d0-d4/a0-a6,-(sp)
        move.l  _AbsExecBase,a6
        jsr     _LVODisable(a6)

        lea     _PatchesInstalled(pc),a0
        tst.b   (a0)
        beq.s   UninstallPatches_Exit

        move.w  #-54,d0
        lea     _OldSwitch(pc),a0
        move.l  (a0),a0
        exg.l   d0,a0
        move.l  a6,a1
        jsr     _LVOSetFunction(a6)

        move.w  #_LVOAddTask,d0
        lea     _OldAddTask(pc),a0
        move.l  (a0),a0
        exg.l   d0,a0
        move.l  a6,a1
        jsr     _LVOSetFunction(a6)

        lea     _PatchesInstalled(pc),a0
        move.b  #$00,(a0)

        jsr     _LVOCacheClearU(a6)

UninstallPatches_Exit:
        jsr     _LVOEnable(a6)
        movem.l (sp)+,d0-d4/a0-a6
        rts


_TaskNumber    EQU 512
_LVOAddTime    EQU -42
_LVOSubTime    EQU -48
_LVOCmpTime    EQU -54
_LVOReadEClock EQU -60
_LVOGetSysTime EQU -66


        xdef _MySwitch

_MySwitch:
        movem.l d0/a0,-(sp)
        ; für Hexaae
        ;bra     _OldSwitchFunc
        ; für Hexaae
        move.b  _SwitchState(pc),d0
        beq     _OldSwitchFunc

swstart:
        movem.l d1-d3/a1-a4/a6,-(sp)
        lea     _OldTimeVal(pc),a4
        move.l  _MyTVSecs(pc),(a4)+
        move.l  _MyTVMicro(pc),(a4)

        move.l  _AsmTimerBase(pc),a6
        lea     _MyTimeVal(pc),a0
        jsr     _LVOGetSysTime(a6)

        cmp.l   #42,-(a4)
        beq.s   swexit

        lea     _MyTimeVal(pc),a0
        lea     _OldTimeVal(pc),a1
        jsr     _LVOSubTime(a6)

        move.l  _AbsExecBase,a6
        move.l  ThisTask(a6),d0

        move.l  _TaskPtr1(pc),a4
        move.l  #_TaskNumber-1,d3
swloop:
        move.l  (a4),d1
        cmp.l   d0,d1
        beq.s   swfound
        lea     8(a4),a4
        dbf     d3,swloop

        move.l  _TaskPtr1(pc),a4
        move.l  #_TaskNumber-1,d3
swloop2:
        move.l  (a4),d1
        beq.s   swfound     ;Task noch nicht in der Tabelle
        lea     8(a4),a4
        dbf     d3,swloop2
        bra.s   swexit      ;Task konnte nicht mehr in die Tabelle
                            ; aufgenommen werden

swfound:
        move.l  d0,(a4)
        move.l  _MyTVMicro(pc),d0
        add.l   d0,4(a4)
        lea     _TotalMicros1(pc),a0
        add.l   d0,(a0)
        bra.s   swexit
swnext:
        lea     8(a4),a4
        dbf     d3,swloop

swexit:
        lea     _TotalMicros1(pc),a0
        move.l  (a0),d0
        andi.l  #$ff000000,d0
        tst.l   d0
        beq.s   swnoshift
        lsr.l   #8,d0
        lsr.l   #4,d0
        move.l  d0,(a0)

        move.l  _TaskPtr1(pc),a4
        move.l  #_TaskNumber-1,d3
swshift:
        cmp.l   #-2,(a4)
        beq.s   swshiftnext

        move.l  4(a4),d0
        lsr.l   #8,d0
        lsr.l   #4,d0
        move.l  d0,4(a4)
swshiftnext:
        lea     8(a4),a4
        dbf     d3,swshift

swnoshift:
        move.l  _AsmTimerBase(pc),a6
        lea     _MyTimeVal(pc),a0
        jsr     _LVOGetSysTime(a6)
        movem.l (sp)+,d1-d3/a1-a4/a6

_OldSwitchFunc:
        movem.l (sp)+,d0/a0
        move.l  _OldSwitch(pc),-(sp)
        rts


_MyTimeVal:
_MyTVSecs:     dc.l  42
_MyTVMicro:    dc.l  0

_OldTimeVal:
_OldTVSecs:    dc.l  0
_OldTVMicro:   dc.l  0


        xdef _SwitchState
_SwitchState:  dc.b  0,0


        xdef _MyAdd
_MyAdd:
addstart:
        ; für Hexaae
        ;bra     _OldAddTaskFunc
        ; für Hexaae
        movem.l d1/d4/d6/a0/a4/a6,-(sp)
        move.l  a1,d4
        bsr.s   _OldAddTaskFunc

        move.b  _SwitchState(pc),d1
        beq.s   addexit

;        move.l  _AbsExecBase,a6
;        jsr     _LVOForbid(a6)   ;Darf nicht verwendet werden! (Absturz)

        move.l  _TaskPtr1(pc),a4
        move.l  #_TaskNumber-1,d6
addloop:
        move.l  (a4),d1
        bne.s   addnext

addfound:
        move.l  #-2,(a4)    ;added task = -2
        move.l  d4,4(a4)    ;task address
        bra.s   addexit
addnext:
        lea     8(a4),a4
        dbf     d6,addloop

addexit:
;        jsr     _LVOPermit(a6)
        movem.l (sp)+,d1/d4/d6/a0/a4/a6
        rts

_OldAddTaskFunc:
        move.l  _OldAddTask(pc),-(sp)
        rts


        xdef _ClearTaskData

;-------------------------------------------------------------------
; Die Routine 'ClearTaskData' muß vor der Installierung der Patches
; aufgerufen werden!
;-------------------------------------------------------------------

_ClearTaskData:
   movem.l  d0-d3/a0-a4/a6,-(sp)
   move.b   _TaskPtrState(pc),d0
   bne.s    ctstart

   lea      _TaskPtr1(pc),a4
   jsr      _AllocTaskBuffer

ctstart:
   move.l   _AbsExecBase,a6
   jsr      _LVOForbid(a6)

   lea      _TaskPtr1(pc),a4     ;Pointer austauschen!
   move.l   (a4),d0
   move.l   4(a4),(a4)
   move.l   d0,4(a4)

   lea      _TotalMicros1(pc),a4 ;TotalMicros austauschen!
   move.l   (a4),d0
   move.l   4(a4),(a4)
   move.l   d0,4(a4)

   jsr      _LVOPermit(a6)

   moveq    #0,d0
   move.l   _TaskPtr1(pc),a0
   move.l   #_TaskNumber*2-1,d1
ctloop:
   move.l   d0,(a0)+
   dbf      d1,ctloop

   lea      _TotalMicros1(pc),a0
   move.l   #0,(a0)

ctexit:
   movem.l  (sp)+,d0-d3/a0-a4/a6
   rts


        xdef _GetTaskData

_GetTaskData:
   movem.l  d1-d3/a0,-(sp)
   move.l   d0,d2

   move.l   _TaskPtr2(pc),a0
   move.l   #_TaskNumber-1,d3
   moveq    #0,d0
gtloop:
   move.l   (a0),d1
   cmp.l    d1,d2
   bne.s    gtnext
   move.l   4(a0),d0
;   move.l   d0,d1
;   lsl.l    #3,d1
;   lsl.l    #1,d0
;   add.l    d1,d0
   bra.s    gtexit
gtnext:
   lea      8(a0),a0
   dbf      d3,gtloop
gtexit:
   movem.l  (sp)+,d1-d3/a0
   rts

_TaskPtr1:    dc.l  0     ;Die Reihenfolge dieser drei
_TaskPtr2:    dc.l  0     ; Variablen unbedingt so lassen!
_TaskPtrState: dc.b  0,0


   xdef _TotalMicros1
   xdef _TotalMicros2

_TotalMicros1: dc.l  0
_TotalMicros2: dc.l  0

   xdef _PatchesInstalled

_PatchesInstalled: dc.b 0,0


   xdef _OldSwitch

_OldSwitch:    dc.l  0


   xdef _OldAddTask

_OldAddTask:    dc.l  0


        xdef _GetAddedTask
_GetAddedTask:
   movem.l  d1/d6/a4,-(sp)
   move.l   _TaskPtr2(pc),a4
   move.l   #_TaskNumber-1,d6
   moveq    #0,d0
   move.l   #-2,d1
galoop:
   cmp.l    (a4),d1
   bne.s    ganext

gafound:
   move.l   4(a4),d0
   move.l   #0,(a4)
   move.l   #0,4(a4)
   bra.s    gaexit
ganext:
   lea      8(a4),a4
   dbf      d6,galoop

gaexit:
   movem.l  (sp)+,d1/d6/a4
   rts


   xdef _AsmTimerBase

_AsmTimerBase:
    dc.l  0


   xdef _CodeAddress

_CodeAddress:
    dc.l  0

;-----------------------------------------------------------------
; Ab hier kommen Dinge, die nicht für die Patches kopiert werden!
;-----------------------------------------------------------------

_AllocTaskBuffer:          ;a4: Adresse von TaskPtr1
   movem.l  d0-d4/a4/a6,-(sp)
   move.l   _AbsExecBase,a6
   move.l   #_TaskNumber*16+8,d0
   move.l   #$10001,d1
   jsr      _LVOAllocVec(a6)
   move.l   d0,d4
   beq.s    atbx

   jsr      _LVOForbid(a6)

   move.l   d4,a1
   jsr      _LVOFreeVec(a6)

   move.l   #_TaskNumber*8+4,d0
   move.l   #$50001,d1
   jsr      _LVOAllocVec(a6)
   move.l   d0,(a4)
   beq.s    atbx

   move.l   #_TaskNumber*8+4,d0
   move.l   #$50001,d1
   jsr      _LVOAllocVec(a6)
   move.l   d0,4(a4)
   beq.s    ftb1
   move.b   #1,8(a4)
atbx:
   jsr      _LVOPermit(a6)
   movem.l  (sp)+,d0-d4/a4/a6
   rts


   xdef _FreeTaskBuffer
_FreeTaskBuffer:
   movem.l  d0-d4/a4/a6,-(sp)
   move.l   _AbsExecBase,a6
   jsr      _LVOForbid(a6)

   lea      _TaskPtr1(pc),a4
   move.l   _TaskPtr2(pc),d0
   beq.s    ftb1
   move.l   d0,a1
   jsr      _LVOFreeVec(a6)
   move.l   #0,4(a4)
ftb1:
   move.l   _TaskPtr1(pc),d0
   beq.s    ftbx
   move.l   d0,a1
   jsr      _LVOFreeVec(a6)
   move.l   #0,(a4)
   move.b   #0,8(a4)
ftbx:
   jsr      _LVOPermit(a6)
   movem.l  (sp)+,d0-d4/a4/a6
   rts


        END


; $Id$

;    AHI - Hardware independent audio subsystem
;    Copyright (C) 1996-1999 Martin Blom <martin@blom.org>
;     
;    This library is free software; you can redistribute it and/or
;    modify it under the terms of the GNU Library General Public
;    License as published by the Free Software Foundation; either
;    version 2 of the License, or (at your option) any later version.
;     
;    This library is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;    Library General Public License for more details.
;     
;    You should have received a copy of the GNU Library General Public
;    License along with this library; if not, write to the
;    Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
;    MA 02139, USA.

	include	exec/exec.i
	include	lvo/exec_lib.i
	include	lvo/timer_lib.i
	include	lvo/utility_lib.i

	include	ahi_def.i

	XDEF	_PreTimer
	XDEF	_PostTimer
	XDEF	_DummyPreTimer
	XDEF	_DummyPostTimer

	XDEF	_UDivMod64

	XREF	_TimerBase
	XREF	_UtilityBase

	XDEF	_MixEntry

	section	.text,code

** Sprintf ********************************************************************

;
; Simple version of the C "sprintf" function.  Assumes C-style
; stack-based function conventions.
;
;   long eyecount;
;   eyecount=2;
;   sprintf(string,"%s have %ld eyes.","Fish",eyecount);
;
; would produce "Fish have 2 eyes." in the string buffer.
;
        XDEF _Sprintf
_Sprintf:       ; ( ostring, format, {values} )
	movem.l	a2/a3/a6,-(sp)

	move.l	4*4(sp),a3	;Get the output string pointer
	move.l	5*4(sp),a0	;Get the FormatString pointer
	lea.l	6*4(sp),a1	;Get the pointer to the DataStream
	lea.l	stuffChar(pc),a2
	move.l	4.w,a6
	jsr	_LVORawDoFmt(a6)

	movem.l	(sp)+,a2/a3/a6
	rts

;------ PutChProc function used by RawDoFmt -----------
stuffChar:
	move.b	d0,(a3)+	;Put data to output string
	rts


** PreTimer/PostTimer *********************************************************

;in:
* a2	ptr to AHIAudioCtrl
;out:
* d0	TRUE if too much CPU is used
* Z	Updated
_PreTimer:
	pushm	d1-d2/a0-a1/a6
	move.l	_TimerBase,a6
	lea	ahiac_Timer(a2),a0
	move.l	EntryTime+EV_LO(a0),d2
	call	ReadEClock
	move.l	EntryTime+EV_LO(a0),d1
	sub.l	d1,d2			; d2 = -(clocks since last entry)
	sub.l	ExitTime+EV_LO(a0),d1	; d1 = clocks since last exit
	add.l	d2,d1			; d1 = -(clocks spent mixing)
	beq	.ok
	neg.l	d1			; d1 = clocks spent mixing
	neg.l	d2			; d2 = clocks since last entry
	lsl.l	#8,d1
 IFGE	__CPU-68020
	divu.l	d2,d1
 ELSE
	move.l	d1,d0
	move.l	d2,d1
	move.l	_UtilityBase,a1
	jsr	_LVOUDivMod32(a1)
	move.l	d0,d1
 ENDC
 	move.w	d1,ahiac_UsedCPU(a2)
	cmp.b	ahiac_MaxCPU(a2),d1
	bls	.ok
	moveq	#TRUE,d0
	bra	.exit
.ok
	moveq	#FALSE,d0
.exit
	popm	d1-d2/a0-a1/a6
	rts


_PostTimer:
	pushm	d0-d1/a0-a1/a6
	move.l	_TimerBase,a6
	lea	ahiac_Timer+ExitTime(a2),a0
	call	ReadEClock
	popm	d0-d1/a0-a1/a6
	rts

_DummyPreTimer:
	moveq	#FALSE,d0
_DummyPostTimer:
	rts


** UDivMod64 ******************************************************************

;UDivMod64 -- unsigned 64 by 32 bit division
;             64 bit quotient, 32 bit remainder.
; (d1:d2)/d0 = d0:d2, d1 remainder.

_UDivMod64:
	movem.l	d3-d7,-(sp)
	move.l	d0,d7
	moveq	#0,d0
	move.l	#$80000000,d3
	move.l	#$00000000,d4
	moveq	#0,d5			;result
	moveq	#0,d6			;result

.2
	lsl.l	#1,d2
	roxl.l	#1,d1
	roxl.l	#1,d0
	sub.l	d7,d0
	bmi	.3
	or.l	d3,d5
	or.l	d4,d6
	skipw
.3
	add.l	d7,d0

	lsr.l	#1,d3
	roxr.l	#1,d4
	bcc	.2

	move.l	d5,d2
	move.l	d0,d1
	move.l	d6,d0
	movem.l	(sp)+,d3-d7
	rts


** Stubs **********************************************************************

_MixEntry:
	XREF	_MixM68K
	XREF	_MixPowerPC
	XREF	_PPCObject

	pushm	d0-d1/a0-a1
	tst.l	_PPCObject
	bne	.powerpc
	jsr	_MixM68K
	popm	d0-d1/a0-a1
	rts
.powerpc
	jsr	_MixPowerPC
	popm	d0-d1/a0-a1
	rts

** Debug functions ************************************************************


	XDEF	_KPrintF
	XDEF	_kprintf
	XDEF	_VKPrintF
	XDEF	_vkprintf
	XDEF	_KPutFmt
	XDEF	KPrintF
	XDEF	kprintf
	XDEF	KPutFmt
	XDEF	kprint_macro

kprint_macro:
	movem.l	d0-d1/a0-a1,-(sp)
	bsr	KPrintF
	movem.l	(sp)+,d0-d1/a0-a1
	rts


_KPrintF:
_kprintf:
	move.l	1*4(sp),a0
	lea	2*4(sp),a1
	bra	KPrintF

_VKPrintF:
_vkprintf:
_KPutFmt:
	move.l	1*4(sp),a0
	move.l	2*4(sp),a1

KPrintF:
kprintf:
KPutFmt:
	movem.l	a2-a3/a6,-(sp)
	move.l	4.w,a6
	lea.l	RawPutChar(pc),a2
	move.l	a6,a3
	jsr	_LVORawDoFmt(a6)
	movem.l	(sp)+,a2-a3/a6
	rts

RawPutChar:
	move.l	a3,a6
	jsr	-516(a6)	;_LVORawPutChar
	rts

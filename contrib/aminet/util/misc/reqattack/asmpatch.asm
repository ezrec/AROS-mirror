	incdir "stormc:asm-include/"

	include "exec/tasks.i"
	include "exec/memory.i"
	include "lvo/exec_lvo.i"
	include "lvo/intuition_lvo.i"

	code


;//***************************************************
;	XREF _New_EasyRequestArgs
	XREF _oldeasyrequestargs
	XDEF _Asm_EasyRequestArgs
	XDEF _Old_EasyRequestArgs
	xref _IntuitionBase
	xref _ReqToolsBase
MIN_STACK = 4096


_Asm_EasyRequestArgs:
	addq		#1,_patchentrycounter
	bsr.s		New_EasyRequestArgs
	subq		#1,_patchentrycounter
	rts

New_EasyRequestArgs:
	movem.l	d2/a0/a1/a4-a6,-(sp)

	lea	-(4+StackSwapStruct_SIZEOF)(sp),sp
	move.l	sp,a5

	move.l	a6,d2			;// backup IntuitionBase
	move.l	4.w,a6

	sub.l	a1,a1
	jsr	_LVOFindTask(a6)
	move.l	d0,a4

	move.l	sp,d0
	cmp.l	TC_SPLOWER(a4),d0
	bls.s	.with_stackswap
	cmp.l	TC_SPUPPER(a4),d0
	bhs.s	.with_stackswap
	sub.l	#MIN_STACK,d0
	cmp.l	TC_SPLOWER(a4),d0
	bls.s	.with_stackswap

.nomem:
	lea	4+StackSwapStruct_SIZEOF(sp),sp
	movem.l	(sp)+,d2/a0/a1/a4-a6
	bra.s	My_EasyRequestArgs

.with_stackswap:
	move.l	#MIN_STACK,d0
	move.l	#MEMF_PUBLIC+MEMF_REVERSE,d1
	jsr	_LVOAllocVec(a6)
	tst.l	d0
	beq.s	.nomem

	move.l	d0,(a5)
	move.l	d0,4+stk_Lower(a5)
	add.l	#MIN_STACK,d0
	move.l	d0,4+stk_Upper(a5)
	move.l	d0,4+stk_Pointer(a5)

	lea	4(a5),a0

	jsr	_LVOStackSwap(a6)

	exg.l	a6,d2			;// IntuitionBase

	movem.l	4+StackSwapStruct_SIZEOF(a5),d0/a0/a1

	bsr.s	My_EasyRequestArgs
	move.l	d0,a4			;// save result

	exg.l	a6,d2			;// ExecBase

	lea	4(a5),a0
	jsr	_LVOStackSwap(a6)

	move.l	(a5),a1
	jsr	_LVOFreeVec(a6)

	move.l	a4,d0
	lea	4+StackSwapStruct_SIZEOF(a5),sp
	movem.l	(sp)+,d2/a0/a1/a4-a6
	rts

My_EasyRequestArgs:
;// a0 = window
;// a1 = easyStruct
;// a2 = idcmpPtr
;// a3 = args
;// a6 = IntuitionBase

	move.l	a3,-(sp)

	moveq	#0,d0				;// idcmp
	cmp.l	#0,a2				;// idcmpPtr ?
	beq.s	.idcmpOK

	move.l	(a2),d0				;// idcmp = *idcmpPtr

.idcmpOK:
	jsr	_LVOBuildEasyRequestArgs(a6)
	cmp.l	#1,d0
	bls.s	.raus1

	move.l	d0,a3

.schleife1:
	move.l	a3,a0				;// window
	move.l	a2,a1				;// idcmpPtr
	moveq	#1,d0				;// waitInput

	jsr	_LVOSysReqHandler(a6)

	cmp.l	#-2,d0
	beq.s	.schleife1

	move.l	a3,a0
	move.l	d0,a3

	jsr	_LVOFreeSysRequest(a6)

	move.l	a3,d0

.raus1:
	move.l	(sp)+,a3
	rts

;//***************************************************

_Old_EasyRequestArgs:
	move.l	a6,-(sp)
	move.l	_IntuitionBase,a6
	movem.l	8(sp),a0-a3
	pea		.return1(pc)
	move.l	_oldeasyrequestargs,-(sp)
	rts

.return1:
	move.l	(sp)+,a6
	rts

;//***************************************************

;	XREF _New_AutoRequest
	XREF _oldautorequest
	XDEF _Asm_AutoRequest
	XDEF _Old_AutoRequest

_Asm_AutoRequest:
	addq		#1,_patchentrycounter
	bsr.s		New_AutoRequest
	subq		#1,_patchentrycounter
	rts

New_AutoRequest:
	movem.l	d0-d2/a0/a1/a4-a6,-(sp)

	lea	-(4+StackSwapStruct_SIZEOF)(sp),sp
	move.l	sp,a5

	move.l	4.w,a6
	sub.l	a1,a1
	jsr	_LVOFindTask(a6)
	move.l	d0,a4

	move.l	sp,d0
	cmp.l	TC_SPLOWER(a4),d0
	bls.s	.with_stackswap2
	cmp.l	TC_SPUPPER(a4),d0
	bhs.s	.with_stackswap2
	sub.l	#MIN_STACK,d0
	cmp.l	TC_SPLOWER(a4),d0
	bls.s	.with_stackswap2

.nomem2:
	lea	4+StackSwapStruct_SIZEOF(sp),sp
	movem.l	(sp)+,d0-d2/a0/a1/a4-a6
	bra.s	My_AutoRequest

.with_stackswap2:
	move.l	#MIN_STACK,d0
	move.l	#MEMF_PUBLIC+MEMF_REVERSE,d1
	jsr	_LVOAllocVec(a6)
	tst.l	d0
	beq.s	.nomem2

	move.l	d0,(a5)
	move.l	d0,4+stk_Lower(a5)
	add.l	#MIN_STACK,d0
	move.l	d0,4+stk_Upper(a5)
	move.l	d0,4+stk_Pointer(a5)

	lea	4(a5),a0

	jsr	_LVOStackSwap(a6)

	;// get IntuitionBase
	move.l	4+StackSwapStruct_SIZEOF+7*4(a5),a6

	movem.l	4+StackSwapStruct_SIZEOF(a5),d0-d2/a0/a1

	bsr.s	My_AutoRequest
	move.l	d0,a4			;// save result

	move.l	4.w,a6

	lea	4(a5),a0
	jsr	_LVOStackSwap(a6)

	move.l	(a5),a1
	jsr	_LVOFreeVec(a6)

	move.l	a4,d0
	lea	4+StackSwapStruct_SIZEOF+4(a5),sp
	movem.l	(sp)+,d1-d2/a0/a1/a4-a6
	rts

DAT2_idcmp 	= -4
DAT2_arg_pFlag	= -8
DAT2_len	= -8

My_AutoRequest:
;// a0 = window
;// a1 = body
;// a2 = posText
;// a3 = negText
;// d0 = pFlag
;// d1 = nFlag
;// d2 = width
;// d3 = height
;// a6 = IntuitionBase

	link	a5,#DAT2_len

	movem.l	d2/a3,-(sp)

	clr.l	DAT2_idcmp(a5)			;// idcmp=0

	move.l	d0,DAT2_arg_pFlag(a5)

	or.l	d1,d0				;// d0 = pFlag | nFlag
	move.l	d2,d1
	move.l	d3,d2

	jsr	_LVOBuildSysRequest(a6)
	cmp.l	#1,d0
	bls.s	.raus2

	move.l	d0,a3

.schleife2:
	move.l	a3,a0				;// window
	lea	DAT2_idcmp(a5),a1		;// idcmpPtr
	moveq	#1,d0				;// waitInput

	jsr	_LVOSysReqHandler(a6)

	cmp.l	#-2,d0
	beq.s	.schleife2

	move.l	a3,a0
	move.l	d0,a3

	jsr	_LVOFreeSysRequest(a6)

	move.l	a3,d0

	cmp.l	#-1,d0
	bne.s	.raus2

	moveq	#0,d0

	move.l	DAT2_idcmp(a5),d1
	and.l	DAT2_arg_pFlag(a5),d1		;// idcmp&posFlag ?
	beq.s	.raus2

	moveq	#1,d0

.raus2:
	movem.l	(sp)+,d2/a3
	unlk	a5
	rts


;//***************************************************

_Old_AutoRequest:
	move.l	a6,-(sp)
	move.l	_IntuitionBase,a6
	movem.l	8(sp),a0-a3
	movem.l	24(sp),d0/d1

 IFD GCC
 	movem.l	32(sp),d2/d3
 ELSE
	movem		32(sp),d2/d3
	ext.l		d2
	ext.l		d3
 ENDC

	pea		.return0(pc)
	move.l	_oldautorequest,-(sp)
	rts

.return0:
	move.l	(sp)+,a6
	rts

;//***************************************************

	XREF _New_BuildEasyRequestArgs
	XREF _oldbuildeasyrequestargs
	XDEF _Asm_BuildEasyRequestArgs
	XDEF _Old_BuildEasyRequestArgs

_Asm_BuildEasyRequestArgs:
	addq		#1,_patchentrycounter

	movem.l	d0/a0/a1/a4-a6,-(sp)

	move.l	4.w,a6

	lea		-(4+StackSwapStruct_SIZEOF)(sp),sp
	move.l	sp,a5
	move.l	#MIN_STACK,d0
	move.l	#MEMF_PUBLIC+MEMF_REVERSE,d1
	jsr		_LVOAllocVec(a6)
	tst.l		d0
	beq.s		.nomem3

	move.l	d0,(a5)
	move.l	d0,4+stk_Lower(a5)
	add.l		#MIN_STACK,d0
	move.l	d0,4+stk_Upper(a5)
	move.l	d0,4+stk_Pointer(a5)
	lea		4(a5),a0

	jsr		_LVOStackSwap(a6)

	movem.l	4+StackSwapStruct_SIZEOF(a5),d0/a0/a1

	movem.l	d0/a3,-(sp)
	movem.l	a0/a1,-(sp)
	jsr		_New_BuildEasyRequestArgs

	move.l	d0,a4

	lea		4(a5),a0
	jsr		_LVOStackSwap(a6)

	move.l	(a5),a1
	jsr		_LVOFreeVec(a6)

	move.l	a4,d0
	beq.s		.doold

	lea		4+StackSwapStruct_SIZEOF+3*4(a5),sp
	movem.l	(sp)+,a4-a6
	rts

.nomem3:
.doold:
	lea		4+StackSwapStruct_SIZEOF(a5),sp
	movem.l	(sp)+,d0/a0/a1/a4-a6

	pea		.return2(pc)
	move.l	_oldbuildeasyrequestargs,-(sp)
	rts

.return2:
	subq		#1,_patchentrycounter
	rts


;//***************************************************

_Old_BuildEasyRequestArgs:
	move.l	a6,-(sp)
	move.l	_IntuitionBase,a6

	movem.l	8(sp),a0/a1
	movem.l	16(sp),d0/a3
	pea		.return3(pc)
	move.l	_oldbuildeasyrequestargs,-(sp)
	rts

.return3:
	move.l	(sp)+,a6
	rts

;//***************************************************

	XREF _New_BuildSysRequest
	XREF _oldbuildsysrequest
	XDEF _Asm_BuildSysRequest
	XDEF _Old_BuildSysRequest

_Asm_BuildSysRequest:
	addq		#1,_patchentrycounter

	movem.l	d0/d1/a0/a1/a4-a6,-(sp)
	lea		-(4+StackSwapStruct_SIZEOF)(sp),sp
	move.l	sp,a5
	move.l	4.w,a6
	move.l	#MIN_STACK,d0
	move.l	#MEMF_PUBLIC+MEMF_REVERSE,d1
	jsr		_LVOAllocVec(a6)
	tst.l		d0
	beq.s		.nomem4

	move.l	d0,(a5)
	move.l	d0,4+stk_Lower(a5)
	add.l		#MIN_STACK,d0
	move.l	d0,4+stk_Upper(a5)
	move.l	d0,4+stk_Pointer(a5)
	lea		4(a5),a0

	jsr		_LVOStackSwap(a6)

	movem.l	4+StackSwapStruct_SIZEOF(a5),d0/d1/a0/a1

 IFD GCC
	ext.l		d1
	ext.l		d2
	movem.l	d1/d2,-(sp)
 ELSE
	movem		d1/d2,-(sp)
 ENDC

	move.l	d0,-(sp)
	movem.l	a0-a3,-(sp)
	jsr		_New_BuildSysRequest

	move.l	d0,a4

	lea		4(a5),a0
	jsr		_LVOStackSwap(a6)

	move.l	(a5),a1
	jsr		_LVOFreeVec(a6)

	move.l	a4,d0
	beq.s		.doold4

	lea		4+StackSwapStruct_SIZEOF+4*4(a5),sp
	movem.l	(sp)+,a4-a6
	rts

.nomem4:
.doold4:
	lea		4+StackSwapStruct_SIZEOF(a5),sp
	movem.l	(sp)+,d0/d1/a0/a1/a4-a6

	pea		.return4(pc)
	move.l	_oldbuildsysrequest,-(sp)
	rts

.return4:
	subq		#1,_patchentrycounter
	rts



;//***************************************************

_Old_BuildSysRequest:
	move.l	a6,-(sp)
	move.l	_IntuitionBase,a6
	movem.l	8(sp),a0-a3
	move.l	24(sp),d0

 IFD GCC
	movem.l	28(sp),d1/d2
 ELSE
	movem		28(sp),d1/d2
	ext.l		d1
	ext.l		d2
 ENDC

	pea		.return5(pc)
	move.l	_oldbuildsysrequest,-(sp)
	rts

.return5:
	move.l	(sp)+,a6
	rts

;//***************************************************

	XREF _New_FreeSysRequest
	XREF _oldfreesysrequest
	XDEF _Asm_FreeSysRequest
	XDEF _Old_FreeSysRequest

_Asm_FreeSysRequest:
	move.l	a0,-(sp)
	jsr		_New_FreeSysRequest
	addq.l	#4,sp

	;// New_FreeSysRequest returns 1 if requester was
	;// A ReqAttack requester, otherwise 0

	sub		d0,_patchentrycounter
	rts

;//***************************************************

_Old_FreeSysRequest:
	move.l	a6,-(sp)
	move.l	_IntuitionBase,a6
	move.l	8(sp),a0
	pea		.return6(pc)
	move.l	_oldfreesysrequest,-(sp)
	rts

.return6:
	move.l	(sp)+,a6
	rts

;//DOPUS5.LIBRARY PATCH*******************************************

;	XREF _olddopus
;	XREF _New_DOpus5Request
;	XDEF _Asm_DOpus
;	XDEF _Old_DOpus

;_Asm_DOpus:
;	addq		#1,_patchentrycounter
;	movem.l	d1-d7/a0-a6,-(sp) ;//registers to the stack

	;cmpi.l	#1,d0				;//is this a filerequester?
	;beq		_Old_DOpus		;//if yes -> call orig function
;	jsr		_New_DOpus5Request

;	movem.l	(sp)+,d1-d7/a0-a6 ;//restore registers
;	subq		#1,_patchentrycounter
;	rts

;_Old_DOpus:
;	movem.l	(sp)+,d1-d7/a0-a6 ;//restore registers
;	subq		#1,_patchentrycounter
;	move.l	_olddopus,-(sp);//run the original function
;	rts

;//REQTOOLS*******************************************

	XREF _oldreqtools
	XREF _New_ReqTools
	XDEF _Asm_ReqTools
	XDEF _Old_ReqTools

_Asm_ReqTools:
	addq		#1,_patchentrycounter
	movem.l	d1-d7/a0-a6,-(sp) ;//registers to the stack
	;//patch code
	jsr		_New_ReqTools
	cmpi.l	#-3,d0				;//structures alloced ?
	beq		_Old_ReqTools		;//if not -> call orig function

	movem.l	(sp)+,d1-d7/a0-a6 ;//restore registers
	subq		#1,_patchentrycounter
	rts

_Old_ReqTools:
	movem.l	(sp)+,d1-d7/a0-a6 ;//restore registers
	subq		#1,_patchentrycounter
	move.l	_oldreqtools,-(sp);//run the original function
	rts

;//***************************************************

	XREF _New_SysReqHandler
	XREF _oldsysreqhandler
	XDEF _Asm_SysReqHandler
	XDEF _Old_SysReqHandler

_Asm_SysReqHandler:
	addq		#1,_patchentrycounter

 IFD GCC
   move.l	d0,-(sp)
 ELSE
	move		d0,-(sp)
 ENDC

	movem.l	a0/a1,-(sp)
	jsr		_New_SysReqHandler

 IFD GCC
 	lea		3*4(sp),sp
 ELSE
	lea		2*4+2(sp),sp
 ENDC

	subq		#1,_patchentrycounter
	rts

;//***************************************************

_Old_SysReqHandler:
	move.l	a6,-(sp)
	move.l	_IntuitionBase,a6
	movem.l	8(sp),a0/a1

 IFD GCC
	move.l	16(sp),d0
 ELSE
	moveq		#0,d0
	move		16(sp),d0
 ENDC

	pea		.return7(pc)
	move.l	_oldsysreqhandler,-(sp)
	rts

.return7:
	move.l	(sp)+,a6
	rts

;//***************************************************

	XDEF	_patchentrycounter

_patchentrycounter:
	dc.w	0

	END


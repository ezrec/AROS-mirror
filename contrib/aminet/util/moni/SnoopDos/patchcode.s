*****************************************************************************
*
*	PATCHCODE.S						vi:ts=8
*
*	Copyright (c) Eddy Carroll, September 1994.
*
*	Patch code used by SnoopDos 3.0. This is copied into the
*	patch table when we first install the SnoopDos patches.
*
*****************************************************************************

	XDEF	_PatchCode_Start
	XDEF	_PatchCode_NormalReturn
	XDEF	_PatchCode_JumpOrigFunc
	XDEF	_PatchCode_End

	include "exec/execbase.i"
	include	"exec/tasks.i"

	SECTION	Text,CODE

;
;	This is the entry point for the patch. It checks to see if
;	the patch is currently enabled or not and if it isn't,
;	jumps to the original function instead. Normally, if the
;	patch isn't enabled, we will remove it completely, but if
;	something has patched it after we did, we can't remove it
;	so this code is needed.
;
;	Note that we do a slight trick to check if the patch is enabled
;	as quickly as possible. We'd like to say CMPI.W #0,enabled(pc)
;	or similar, but that won't work on a 68000. So instead, we ensure
;	that the enabled flag is always either $00000000 or $FFFFFFFF, and
;	then check A7 (which is guaranteed to never be either value)
;	against it.
;
_PatchCode_Start:

	cmp.l 	enabled(pc),a7		; Patch enabled?
	blo.b   callnewfunc  		; If so, call new function
	move.l  origfunc(pc),-(a7)	; Else call old function
	rts				;		

;----------------------------------------------------------------------------
;
;	Calling our new function. We do the following:
;
;	    - Check that we have enough stack to execute the patch.
;	    - Increment usage count to show someone is running the patch code
;	    - Call the new patch code. If the patch code decides not to monitor
;	      the call, it will adjust the return address on the stack to
;	      return to a different point.
;	    - Decrement usage count, and return to caller
;
;	If the stack return address was adjusted, it points to code that:
;
;	    - Decrements the usage count
;	    - Jumps to the original function
;
;	The reason we do it like this is so that when we are ignoring a
;	particular task, any calls made by the task will return to this
;	patch code rather than our C code. This allows the C code to be
;	unloaded even if there are still some calls that have not yet
;	returned (e.g. a RunCommand() call while booting.)
;
;	WARNING: If you make any changes here that will adjust the stack
;	contents on entry to the C code, you MUST update the MarkCallAddr
;	and CallAddr macros in PATCHES.C accordingly. Ditto for the
;	JumpOrigFunc[] macro.
;
callnewfunc:

	movem.l	a0/a1/d1/a5,-(a7)	; Save registers
;
;	Now some stack checking code. If the A7 lies in the range
;	SPLower to SPUpper for the current task, then we check if
;	there's enough room left on the stack to carry out the call.
;
;	If A7 doesn't seem to be on the task's stack, then it means
;	the task has allocated a different stack -- it's either a
;	CLI program or a SAS-compiled program that allocated a new
;	stack. In either case, we assume the program has allocated
;	sufficient stack space (the only things that will really cause
;	a problem are tasks with very small stacks, like device drivers).
;
	move.l	sysbase(pc),a5		; Get pointer to this task
	move.l	ThisTask(a5),a5		;
	cmp.l	TC_SPUPPER(a5),a7	; Check if A7 above upper stack bound?
	bhi.s	stack_okay		; If so, no point in checking further
	move.l	TC_SPLOWER(a5),a5	; Get lower bound
	cmp.l	a5,a7			; Check if A7 below lower stack bound?
	blo.s	stack_okay		; If so, no point in checking further
	add.l	stackneeded(pc),a5	; Calculate space needed
	cmp.l	a5,a7			; Does A7 remain within bounds?
	blo.s	stack_failed		; If not, skip this call

stack_okay:
	lea.l	usecount(pc),a5		;
	addq.w	#1,(a5)			; Increment usage count
	move.l	newfunc(pc),a5		;
	jsr	(a5)			; Call the C patch code

_PatchCode_NormalReturn:

	lea.l	usecount(pc),a5		; 
	subq.w	#1,(a5)			; Decrement usage count
	movem.l	(a7)+,a0/a1/d1/a5	; Restore registers
	move.l	d0,d1			; Copy D0 to D1 for compatibility
	rts				; And return to caller
;
;	We reach here by patching the return address directly on the stack.
;	If D0 is one of the parameters to the function, then the return
;	value from the function must be that parameter, so that D0 will
;	be setup correctly for calling the real function.
;
_PatchCode_JumpOrigFunc:

	lea.l	usecount(pc),a5		;
	subq.w	#1,(a5)			; Decrement usage count 

stack_failed:
	movem.l	(a7)+,a0/a1/d1/a5	; Restore registers
	move.l  origfunc(pc),-(a7)	; Jump to original function
	rts				;		
	
	cnop	0,4			; Keep remainder long word aligned

_PatchCode_End:
;
;	The following fields are also defined in the C Patch structure.
;	The two definitions must match exactly or Bad Things will happen.
;
;	They follow the above code directly, rather than appearing in a
;	separate section, because they are accessed using PC-relative
;	addressing.
;
origfunc	dc.l    0		; Pointer to original function
newfunc		dc.l	0		; Pointer to our replacement code
enabled		dc.l	0		; True if enabled
sysbase		dc.l	0		; Points to ExecBase
stackneeded	dc.l	0		; #bytes free stack required
usecount	dc.w	0		; Number of callers in function

	END

	incdir "stormc:asm-include/"

   INCLUDE "exec/types.i"
   INCLUDE "exec/initializers.i"
   INCLUDE "exec/libraries.i"
   INCLUDE "exec/lists.i"
   INCLUDE "exec/resident.i"

 	include "lvo/exec_lvo.i"
	include "lvo/intuition_lvo.i"

	code

	XDEF _Asm_RALibOpen
	XDEF _Asm_RALibClose
	XDEF _Asm_RALibExpunge

_Asm_RALibOpen:

	;//--- we have another guest;)

	addq.w	#1,LIB_OPENCNT(a6)

	;//--- unset the delayed expunge flag (which we don't really use)

	bclr	#LIBB_DELEXP,LIB_FLAGS(a6)

	;//--- setup the return value
	move.l	a6,d0
	rts

_Asm_RALibClose:

	;//--- we have one fewer guest :((

	subq.w	#1,LIB_OPENCNT(a6)

	;//--- unset the delayed expunge flag (which we don't really use)

	bclr	#LIBB_DELEXP,LIB_FLAGS(a6)

	move.l	#0,d0
	rts

_Asm_RALibExpunge:
	;//--- this routine is not really useful for us -ReqAttack remove the
	;//--- library while shutdown...

	;//--- unset the delayed expunge flag (which we don't really use)

	bclr	#LIBB_DELEXP,LIB_FLAGS(a6)

	move.l	#0,d0
	rts

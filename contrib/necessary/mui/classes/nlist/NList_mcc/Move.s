#NO_APP
gcc2_compiled.:
___gnu_compiled_c:
.text
	.even
.globl _NL_Move
_NL_Move:
	link a5,#0
	movel a5@(8),a1
	movel a5@(12),a0
	movel a5@(16),d0
	jle L2
	asrl #2,d0
	subql #1,d0
	.even
L4:
	movel a0@+,a1@+
	subql #1,d0
	jpl L4
L2:
	unlk a5
	rts
	.even
.globl _NL_MoveD
_NL_MoveD:
	link a5,#0
	movel a5@(8),a1
	movel a5@(12),a0
	movel a5@(16),d0
	jle L10
	asrl #2,d0
	subql #1,d0
	.even
L12:
	movel a0@-,a1@-
	subql #1,d0
	jpl L12
L10:
	unlk a5
	rts

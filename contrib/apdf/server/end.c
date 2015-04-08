
asm("
	.section \".init\",\"a\"
	.long	0,0
	");
asm("
	.section \".fini\",\"a\"
	.long	0,0
	");

asm("	.section \".ctors\"
	.globl	__ctrsend
__ctrsend:
	");

asm("	.section \".dtors\"
	.globl	__dtrsend
__dtrsend:
	");


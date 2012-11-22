	.text
	.align  16
	.globl  Kernel_KrnSti
	.type   Kernel_KrnSti, @function

Kernel_KrnSti:
	sti
	ret

	.size  Kernel_KrnSti, .-Kernel_KrnSti

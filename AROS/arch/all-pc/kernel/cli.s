	.text
	.align  16
	.globl  Kernel_KrnCli
	.type   Kernel_KrnCli, @function

Kernel_KrnCli:
	cli
	ret

	.size  Kernel_KrnCli, .-Kernel_KrnCli

	.text
	.align  16
	.globl  Kernel_KrnIsSuper
	.type   Kernel_KrnIsSuper, @function

Kernel_KrnIsSuper:
	mov	%cs, %ax
	and	$0x03, %eax		// Lower two bits are CPL
	xor	$0x03, %eax		// 0x03 will give zero, 0x00 will give nonzero
	ret

	.size  Kernel_KrnIsSuper, .-Kernel_KrnIsSuper

/*
	This code comes originally from the output of gcc
	(gcc -S) [ Dont forget to specify -O3 also !!!]

	Then it has been hand-optimized in the inner loop
	
	Hum it is a bit faster... but not enough to satisfy me :)

*/



	
.extern buffer
.text
	.align 4
	.globl refresh_prout
	
refresh_prout:
	pushl %ebp
	movl %esp,%ebp
	subl $72,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl $0,-8(%ebp)
	movl $0,-64(%ebp)
	movl $0,-68(%ebp)
	.align 4
.L167:
	movl $0,-4(%ebp)
	movl -68(%ebp),%esi
	movl %esi,-60(%ebp)
	.align 4
.L171:
	movl -60(%ebp),%esi
	movl ikse(%esi),%esi
	movl %esi,-24(%ebp)
	movl -60(%ebp),%esi
	movl ikse+4(%esi),%esi
	movl %esi,-40(%ebp)
	movl -60(%ebp),%esi
	movl ikse+168(%esi),%ecx
	movl igrek(%esi),%esi
	movl %esi,-32(%ebp)
	movl -60(%ebp),%esi
	movl igrek+4(%esi),%esi
	movl %esi,-48(%ebp)
	movl -60(%ebp),%esi
	movl igrek+164(%esi),%edx
	movl igrek+168(%esi),%ebx
	movl ikse+164(%esi),%eax
	subl -24(%ebp),%eax
	jns .L172
	addl $7,%eax
.L172:
	sarl $3,%eax
	movl %eax,-28(%ebp)
	movl %edx,%eax
	subl -32(%ebp),%eax
	jns .L173
	addl $7,%eax
.L173:
	sarl $3,%eax
	movl %eax,-36(%ebp)
	movl %ecx,%eax
	subl -40(%ebp),%eax
	jns .L174
	addl $7,%eax
.L174:
	sarl $3,%eax
	movl %eax,-44(%ebp)
	movl %ebx,%eax
	subl -48(%ebp),%eax
	jns .L175
	addl $7,%eax
.L175:
	sarl $3,%eax
	movl %eax,-52(%ebp)
	movl -4(%ebp),%eax
	sall $4,%eax
	addl buffer,%eax
	addl -64(%ebp),%eax
	movl %eax,-56(%ebp)
	movl $8,-12(%ebp)
	.align 4
.L179:
	movl -56(%ebp),%esi   // swapped ebx et esi
	addl $640,%esi
	movl %esi,-56(%ebp)  
	movl -24(%ebp),%ecx  // swapped ecx et edi
	movl -32(%ebp),%ebx
//	movl %esi,-72(%ebp)
	movl -40(%ebp),%eax
	subl %ecx,%eax
	sarl $3,%eax
	movl %eax,-16(%ebp)
	movl -48(%ebp),%eax
	subl %ebx,%eax
	sarl $3,%eax
	movl %eax,-20(%ebp)

	xorl	%eax,%eax
	movl texture1,%edx
	movl	-16(%ebp),%edi
	.align 4
.L183:
	movb %ch,%al
	addl %edi,%ecx
	movb %bh,%ah
	addl -20(%ebp),%ebx
	movw (%edx,%eax,2),%ax
	movw %ax,(%esi)

		movb %ch,%al
	addl %edi,%ecx
	movb %bh,%ah
	addl -20(%ebp),%ebx
	movw (%edx,%eax,2),%ax
	movw %ax,2(%esi)

		movb %ch,%al
	addl %edi,%ecx
	movb %bh,%ah
	addl -20(%ebp),%ebx
	movw (%edx,%eax,2),%ax
	movw %ax,4(%esi)

		movb %ch,%al
	addl %edi,%ecx
	movb %bh,%ah
	addl -20(%ebp),%ebx
	movw (%edx,%eax,2),%ax
	movw %ax,6(%esi)

		movb %ch,%al
	addl %edi,%ecx
	movb %bh,%ah
	addl -20(%ebp),%ebx
	movw (%edx,%eax,2),%ax
	movw %ax,8(%esi)

		movb %ch,%al
	addl %edi,%ecx
	movb %bh,%ah
	addl -20(%ebp),%ebx
	movw (%edx,%eax,2),%ax
	movw %ax,10(%esi)

		movb %ch,%al
	addl %edi,%ecx
	movb %bh,%ah
	addl -20(%ebp),%ebx
	movw (%edx,%eax,2),%ax
	movw %ax,12(%esi)

		movb %ch,%al
	addl %edi,%ecx
	movb %bh,%ah
	addl -20(%ebp),%ebx
	movw (%edx,%eax,2),%ax
	movw %ax,14(%esi)



	     // end of swap
	movl -28(%ebp),%esi
	addl %esi,-24(%ebp)
	movl -36(%ebp),%esi
	addl %esi,-32(%ebp)
	movl -44(%ebp),%esi
	addl %esi,-40(%ebp)
	movl -52(%ebp),%esi
	addl %esi,-48(%ebp)
	decl -12(%ebp)
	jne .L179
	addl $4,-60(%ebp)
	incl -4(%ebp)
	cmpl $39,-4(%ebp)
	jle .L171
	addl $5120,-64(%ebp)
	addl $164,-68(%ebp)
	incl -8(%ebp)
	cmpl $29,-8(%ebp)
	jle .L167
	leal -84(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret

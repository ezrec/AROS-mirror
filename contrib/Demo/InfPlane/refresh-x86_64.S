/*
	converted from refresh-i386.S
*/



	
.extern buffer
.text
	.align 4
	.globl refresh_prout
	
refresh_prout:
	pushq %rbp
	movq %rsp,%rbp
	subq $72,%rsp
	pushq %rdi
	pushq %rsi
	pushq %rbx
	movq $0,-8(%rbp)
	movq $0,-64(%rbp)
	movq $0,-68(%rbp)
	.align 4
.L167:
	movq $0,-4(%rbp)
	movq -68(%rbp),%rsi
	movq %rsi,-60(%rbp)
	.align 4
.L171:
	movq -60(%rbp),%rsi
	movq ikse(%rsi),%rsi
	movq %rsi,-24(%rbp)
	movq -60(%rbp),%rsi
	movq ikse+4(%rsi),%rsi
	movq %rsi,-40(%rbp)
	movq -60(%rbp),%rsi
	movq ikse+168(%rsi),%rcx
	movq igrek(%rsi),%rsi
	movq %rsi,-32(%rbp)
	movq -60(%rbp),%rsi
	movq igrek+4(%rsi),%rsi
	movq %rsi,-48(%rbp)
	movq -60(%rbp),%rsi
	movq igrek+164(%rsi),%rdx
	movq igrek+168(%rsi),%rbx
	movq ikse+164(%rsi),%rax
	subq -24(%rbp),%rax
	jns .L172
	addq $7,%rax
.L172:
	sarq $3,%rax
	movq %rax,-28(%rbp)
	movq %rdx,%rax
	subq -32(%rbp),%rax
	jns .L173
	addq $7,%rax
.L173:
	sarq $3,%rax
	movq %rax,-36(%rbp)
	movq %rcx,%rax
	subq -40(%rbp),%rax
	jns .L174
	addq $7,%rax
.L174:
	sarq $3,%rax
	movq %rax,-44(%rbp)
	movq %rbx,%rax
	subq -48(%rbp),%rax
	jns .L175
	addq $7,%rax
.L175:
	sarq $3,%rax
	movq %rax,-52(%rbp)
	movq -4(%rbp),%rax
	salq $4,%rax
	addq buffer,%rax
	addq -64(%rbp),%rax
	movq %rax,-56(%rbp)
	movq $8,-12(%rbp)
	.align 4
.L179:
	movq -56(%rbp),%rsi   // swapped rbx and rsi
	addq $640,%rsi
	movq %rsi,-56(%rbp)  
	movq -24(%rbp),%rcx  // swapped rcx and rdi
	movq -32(%rbp),%rbx
//	movq %rsi,-72(%rbp)
	movq -40(%rbp),%rax
	subq %rcx,%rax
	sarq $3,%rax
	movq %rax,-16(%rbp)
	movq -48(%rbp),%rax
	subq %rbx,%rax
	sarq $3,%rax
	movq %rax,-20(%rbp)

	xorq	%rax,%rax
	movq texture1,%rdx
	movq	-16(%rbp),%rdi
	.align 4
.L183:
	movb %ch,%al
	addq %rdi,%rcx
	movb %bh,%ah
	addq -20(%rbp),%rbx
	movw (%rdx,%rax,2),%ax
	movw %ax,(%rsi)

		movb %ch,%al
	addq %rdi,%rcx
	movb %bh,%ah
	addq -20(%rbp),%rbx
	movw (%rdx,%rax,2),%ax
	movw %ax,2(%rsi)

		movb %ch,%al
	addq %rdi,%rcx
	movb %bh,%ah
	addq -20(%rbp),%rbx
	movw (%rdx,%rax,2),%ax
	movw %ax,4(%rsi)

		movb %ch,%al
	addq %rdi,%rcx
	movb %bh,%ah
	addq -20(%rbp),%rbx
	movw (%rdx,%rax,2),%ax
	movw %ax,6(%rsi)

		movb %ch,%al
	addq %rdi,%rcx
	movb %bh,%ah
	addq -20(%rbp),%rbx
	movw (%rdx,%rax,2),%ax
	movw %ax,8(%rsi)

		movb %ch,%al
	addq %rdi,%rcx
	movb %bh,%ah
	addq -20(%rbp),%rbx
	movw (%rdx,%rax,2),%ax
	movw %ax,10(%rsi)

		movb %ch,%al
	addq %rdi,%rcx
	movb %bh,%ah
	addq -20(%rbp),%rbx
	movw (,%rax,2),%ax
	movw %ax,12(%rsi)

		movb %ch,%al
	addq %rdi,%rcx
	movb %bh,%ah
	addq -20(%rbp),%rbx
	movw (%rdx,%rax,2),%ax
	movw %ax,14(%rsi)



	     // end of swap
	movq -28(%rbp),%rsi
	addq %rsi,-24(%rbp)
	movq -36(%rbp),%rsi
	addq %rsi,-32(%rbp)
	movq -44(%rbp),%rsi
	addq %rsi,-40(%rbp)
	movq -52(%rbp),%rsi
	addq %rsi,-48(%rbp)
	decl -12(%rbp)
	jne .L179
	addq $4,-60(%rbp)
	incl -4(%rbp)
	cmpl $39,-4(%rbp)
	jle .L171
	addq $5120,-64(%rbp)
	addq $164,-68(%rbp)
	incl -8(%rbp)
	cmpl $29,-8(%rbp)
	jle .L167
	leaq -84(%rbp),%rsp
	popq %rbx
	popq %rsi
	popq %rdi
	leave
	ret

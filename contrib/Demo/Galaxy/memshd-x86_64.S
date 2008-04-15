
.text


/* memshset (char *dst, int center_shade,int fixed_shade, int length_2)
 dst is the *center* of the dst segment
 center shade is the initial color (at center) (!! 8:8 also)
 fixed_shade is the fixed point increment (8:8) 
 length_2 is the 1/2 length to set (to the left, and to the right)
*/


	
	
#define W 320   /*/ change this if you change W in main.c ... (dirty..) */

	
	
/*
	a memaddshadedset function
      (or something like that ) */
		
.globl memshset
.type   memshset,@function


memshset:
	pushq	%rbp
	movq	%rsp,%rbp
	
	pushq	%rdi		/* registres a sauvegarder (convention gcc) */
	pushq	%rsi
	pushq	%rbx

	movq	8(%rbp),%rdi      /* dst */
	movq	12(%rbp),%rax
	movq	16(%rbp),%rbx	  /* fixed point */
	movq	20(%rbp),%rcx	  /* half length */

	orq	%rcx,%rcx
	jz	2f

	movq	%rdi,%rsi
	subq	%rcx,%rdi	/* %rdi  left segment */
	addq	%rcx,%rsi       /* %rsi  right segment */
	incq	%rsi		/* dont overlap at middle */
	
	movq	%rcx,%rdx
	negq	%rdx
	decq	%rdx  /* for in-advance incq in loop */
1:
	addq	%rbx,%rax      /* increment color */
	incq	%rdx
	
	addb	%ah,(%rdi,%rcx) /* increment  left pixel */
	jc	3f              /* did overflow ? */
	
	addb	%ah,(%rsi,%rdx) /* increment right pixel */
	jc	4f

	decq	%rcx
	jnz	1b

2:	
	popq	%rbx
	popq	%rsi
	popq	%rdi

	leave
	ret
/* function goes on (several exit points): */
3:
	movb	$255,(%rdi,%rcx)
	addb	%ah,(%rsi,%rdx)        /* we recopy all the code.. to avoid jmps   (hmm some asm guru here :	 is it really efficient ?.. */
	jc	4f
	decq	%rcx
	jnz	1b
	
	popq	%rbx
	popq	%rsi
	popq	%rdi

	leave
	ret

4:
	movb	$255,(%rsi,%rdx)
	decq	%rcx
	jnz	1b
	
	popq	%rbx
	popq	%rsi
	popq	%rdi

	leave
	ret
/* function really stops here  */



/* draw a star (5 pixels) */
.globl starsh
.type   starsh,@function

starsh:	
	pushq	%rbp
	movq	%rsp,%rbp
	
	pushq	%rdi
	
	movq	8(%rbp),%rdi      /* dst */
	movq	12(%rbp),%rdx     /* inc */

	addb	%dl,(W)(%rdi)
	jnc	3f
	movb	$255,(W)(%rdi)

3:
	shrb	$1,%dl
	movb	(%rdi),%al
	movb	(W-1)(%rdi),%cl
	addb	%dl,%al
	jc	1f
	addb	%dl,%cl
	jc	2f
	movb	%al,(%rdi)
	movb	%cl,(W-1)(%rdi)
	jmp	31f
1:	
	addb	%dl,%cl
	jc	2f
	movb	$255,(%rdi)
	movb	%cl,(W-1)(%rdi)

	jmp 31f
	
2:	
	movb	$255,(%rdi)
	movb	$255,(W-1)(%rdi)


31:		
	movb	2*W(%rdi),%al
	movb	(W+1)(%rdi),%cl
	addb	%dl,%al
	jc	11f
	addb	%dl,%cl
	jc	21f
	movb	%al,2*W(%rdi)
	movb	%cl,(W+1)(%rdi)
	popq	%rdi
	leave
	ret

	
11:	
	addb	%dl,%cl
	jc	21f
	movb	$255,(2*W)(%rdi)
	movb	%cl,(W+1)(%rdi)
	popq	%rdi
	leave
	ret
	
21:	
	movb	$255,2*W(%rdi)
	movb	$255,(W+1)(%rdi)
	popq	%rdi
	leave
	ret



/* do the "motion blur" (actually the pixel fading) */
.globl  mblur
.type   mblur,@function
	
mblur:	
	pushq	%rbp
	movq	%rsp,%rbp
	
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	
	movq	8(%rbp),%rdi      /* dst */
	movq	12(%rbp),%rcx     /* number */
	movq	$0x7f7f7f7f,%rdx  /* clear 1 upper bits */
	movq	$0x1f1f1f1f,%rsi  /* clear 3 upper bits */
	shrq	$2,%rcx
	jz	2f

1:
	movq	(%rdi),%rax
	addq	$4,%rdi
	
	movq	%rax,%rbx
	shrq	$1,%rax

	
	shrq	$3,%rbx
	andq	%rdx,%rax
	
	andq	%rsi,%rbx
	addq	%rbx,%rax

	decq	%rcx
	movq	%rax,-4(%rdi)
	

	jnz	1b


2:
	popq	%rbx
	popq	%rsi
	popq	%rdi
	leave
	ret

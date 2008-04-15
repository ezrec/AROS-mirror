
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
	pushl	%ebp
	movl	%esp,%ebp
	
	pushl	%edi		/* registres a sauvegarder (convention gcc) */
	pushl	%esi
	pushl	%ebx

	movl	8(%ebp),%edi      /* dst */
	movl	12(%ebp),%eax
	movl	16(%ebp),%ebx	  /* fixed point */
	movl	20(%ebp),%ecx	  /* half length */

	orl	%ecx,%ecx
	jz	2f

	movl	%edi,%esi
	subl	%ecx,%edi	/* %edi  left segment */
	addl	%ecx,%esi       /* %esi  right segment */
	incl	%esi		/* dont overlap at middle */
	
	movl	%ecx,%edx
	negl	%edx
	decl	%edx  /* for in-advance incl in loop */
1:
	addl	%ebx,%eax      /* increment color */
	incl	%edx
	
	addb	%ah,(%edi,%ecx) /* increment  left pixel */
	jc	3f              /* did overflow ? */
	
	addb	%ah,(%esi,%edx) /* increment right pixel */
	jc	4f

	decl	%ecx
	jnz	1b

2:	
	popl	%ebx
	popl	%esi
	popl	%edi

	leave
	ret
/* function goes on (several exit points): */
3:
	movb	$255,(%edi,%ecx)
	addb	%ah,(%esi,%edx)        /* we recopy all the code.. to avoid jmps   (hmm some asm guru here :	 is it really efficient ?.. */
	jc	4f
	decl	%ecx
	jnz	1b
	
	popl	%ebx
	popl	%esi
	popl	%edi

	leave
	ret

4:
	movb	$255,(%esi,%edx)
	decl	%ecx
	jnz	1b
	
	popl	%ebx
	popl	%esi
	popl	%edi

	leave
	ret
/* function really stops here  */



/* draw a star (5 pixels) */
.globl starsh
.type   starsh,@function

starsh:	
	pushl	%ebp
	movl	%esp,%ebp
	
	pushl	%edi
	
	movl	8(%ebp),%edi      /* dst */
	movl	12(%ebp),%edx     /* inc */

	addb	%dl,(W)(%edi)
	jnc	3f
	movb	$255,(W)(%edi)

3:
	shrb	$1,%dl
	movb	(%edi),%al
	movb	(W-1)(%edi),%cl
	addb	%dl,%al
	jc	1f
	addb	%dl,%cl
	jc	2f
	movb	%al,(%edi)
	movb	%cl,(W-1)(%edi)
	jmp	31f
1:	
	addb	%dl,%cl
	jc	2f
	movb	$255,(%edi)
	movb	%cl,(W-1)(%edi)

	jmp 31f
	
2:	
	movb	$255,(%edi)
	movb	$255,(W-1)(%edi)


31:		
	movb	2*W(%edi),%al
	movb	(W+1)(%edi),%cl
	addb	%dl,%al
	jc	11f
	addb	%dl,%cl
	jc	21f
	movb	%al,2*W(%edi)
	movb	%cl,(W+1)(%edi)
	popl	%edi
	leave
	ret

	
11:	
	addb	%dl,%cl
	jc	21f
	movb	$255,(2*W)(%edi)
	movb	%cl,(W+1)(%edi)
	popl	%edi
	leave
	ret
	
21:	
	movb	$255,2*W(%edi)
	movb	$255,(W+1)(%edi)
	popl	%edi
	leave
	ret



/* do the "motion blur" (actually the pixel fading) */
.globl  mblur
.type   mblur,@function
	
mblur:	
	pushl	%ebp
	movl	%esp,%ebp
	
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	
	movl	8(%ebp),%edi      /* dst */
	movl	12(%ebp),%ecx     /* number */
	movl	$0x7f7f7f7f,%edx  /* clear 1 upper bits */
	movl	$0x1f1f1f1f,%esi  /* clear 3 upper bits */
	shrl	$2,%ecx
	jz	2f

1:
	movl	(%edi),%eax
	addl	$4,%edi
	
	movl	%eax,%ebx
	shrl	$1,%eax

	
	shrl	$3,%ebx
	andl	%edx,%eax
	
	andl	%esi,%ebx
	addl	%ebx,%eax

	decl	%ecx
	movl	%eax,-4(%edi)
	

	jnz	1b


2:
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret

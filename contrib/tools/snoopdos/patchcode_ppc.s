	.globl     PatchCode_Start
	.globl     PatchCode_End

	.section .text
	.align     4
PatchCode_Start:
	mflr	r0	/* Save lr */
	bcl	20,31,$+4	/* Fetch pc */
	mflr	r11
	mtlr	r0	/* No, restore lr */
	lwz	r12,128(r11)	/* Are we still active */
	cmpwi	r2,0
	beq	patch_active	/* Yes */
	lwz	r12,120(r11)	/* Get original function */
	mtctr	r12	/* Load the original function in ctr */
	bctr		/* Jump */

patch_active:
	addi	r11,r11,132	/* Atomic increase of usecount */
retry:	lwarx	r12,r0,r11
	addic	r12,r12,1
	stwcx.	r12,r0,r11
	bne-	retry

	stwu	r1,-16(r1)	/* Build a small stack frame */
	stw	r0,20(r1)	/* Save lr */
	mtctr	r0	/* Pass the originator in ctr */
	lwz	r12,124-132(r11)/* Get the new function */
	mtlr	r12	/* Load the new function in ctr */
	blrl		/* Jump */

	mflr	r11
	addi	r11,r11,56
retry2:	lwarx	r12,r0,r11
	addic	r12,r12,-1
	stwcx.	r12,r0,r11
	bne-	retry2
	lwz	r11,0(r1)
	lwz	r0,4(r11)
	mtlr	r0
	mr	r1,r11
	blr
PatchCode_End:


		.text

		.global	R_DrawColumn

		.extern	dc_yh
		.extern	dc_yl
		.extern	dc_source
		.extern	dc_colormap
		.extern	ylookup2
		.extern	dc_x
		.extern	dc_iscale
		.extern	dc_texturemid
		.extern	centery
		.extern	SCREENWIDTH

#void R_DrawColumn (void)

		.align	4

R_DrawColumn:	lis	r3,dc_yh@ha
		lwz	r3,dc_yh@l(r3)
		lis	r12,dc_yl@ha
		lwz	r12,dc_yl@l(r12)
		sub	r3,r3,r12
		addi	r3,r3,1			# count = dc_yh - dc_yl + 1

		lis	r4,dc_source@ha
		lwz	r4,dc_source@l(r4)	# source = dc_source

		lis	r6,dc_colormap@ha
		lwz	r6,dc_colormap@l(r6)	# colormap = dc_colormap

		lis	r5,ylookup2@ha
		lwz	r5,ylookup2@l(r5)
		slwi	r11,r12,2
		lwzx	r5,r5,r11
		lis	r11,dc_x@ha
		lwz	r11,dc_x@l(r11)
		add	r5,r5,r11	# dest = ylookup2[dc_yl] + dc_x

		lis	r8,dc_iscale@ha
		lwz	r8,dc_iscale@l(r8)

		lis	r7,dc_texturemid@ha
		lwz	r7,dc_texturemid@l(r7)
		lis	r11,centery@ha
		lwz	r11,centery@l(r11)
		sub	r12,r12,r11
		mullw	r12,r12,r8
		add	r7,r7,r12
		slwi	r7,r7,9		# frac = (dc_texturemid + (dc_yl-centery)*dc_iscale)<<9

		slwi	r8,r8,9		# fracstep = dc_iscale<<9

		lis	r12,SCREENWIDTH@ha
		lwz	r12,SCREENWIDTH@l(r12)
		b	dc_next

		.align	4

dc_loop:	srawi	r9,r7,25
		lbzx	r9,r4,r9
		lbzx	r9,r6,r9
		stb	r9,0(r5)	# *dest = colormap[source[frac>>25]]

		add	r5,r5,r12	# dest += SCREENWIDTH
		add	r7,r7,r8	# frac += fracstep
		addi	r3,r3,-1	# count -= 1

dc_next:	cmpwi	r3,0
		bgt	dc_loop
		blr


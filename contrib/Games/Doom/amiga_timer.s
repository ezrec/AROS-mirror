
		.extern	ppctimer

# return the timebase registers in the structure passed in

		.align	4

ppctimer:	mftbu	r4
		mftbl	r5
		mftbu	r6
		cmpw	r4,r6
		bne	ppctimer

		stw	r4,0(r3)
		stw	r5,4(r3)
		blr


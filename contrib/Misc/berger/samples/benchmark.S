	; a very simple benchmark test

	clr.l	d0
	clr.l	d1

l1:	subq	#1,d0
l2:	subq	#1,d1
	cmp	#0,d1
	bne	l2	
	cmp	#0,d0
	bne	l1

	rts

	END;

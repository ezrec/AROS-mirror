; Create an array of longwords from the available FALSE stack space.
; by Ed Mackey.

begin:
	move.l	(a5)+,d0  ;Size in longwords.
	lsl.l	#4,d0     ;convert to bytes
	suba.l	d0,a5     ;add to stack
	move.l	a5,-(a5)  ;leave pointer to it on stack
ende:

	END

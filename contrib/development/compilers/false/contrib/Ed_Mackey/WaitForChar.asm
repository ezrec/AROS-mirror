; Call dos.library's WaitForChar.  Have time to wait on FALSE stack on entry.
; Exits with "status" on FALSE stack.
; by Ed Mackey.

begin:
	move.l	d5,d1     ;stdin
	move.l	(a5)+,d2  ;timeout
	jsr	-$CC(a6)  ;_LVOWaitForChar(a6)
	move.l	d0,-(a5)  ;returns TRUE if a char is ready.
ende:

	END

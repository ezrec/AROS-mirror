
OPT MODULE

EXPORT PROC strchr(str,c)
	MOVEA.L str,A0
	MOVE.L c,D0
loop:
	MOVE.B (A0)+,D1
	BEQ.B fail
	CMP.B D0,D1
	BNE.B loop
-> ok:
	MOVE.L A0,D0
	SUBQ.L #1,D0
	RETURN D0
fail:
ENDPROC 0


OPT MODULE

EXPORT PROC stricmp( str1 : PTR TO CHAR , str2 : PTR TO CHAR )

	MOVE.L	str1 , A1
	MOVE.L	str2 , A2
loop_while:
	MOVE.B	(A1)+ , D1
	MOVE.B	(A2)+ , D2
	TST.B	D1
	BNE.B	test2
	TST.B	D2
	BEQ.B	end_eq
	RETURN -1
test2:
	TST.B	D2
	BEQ.B	end_greater
insidewhile:
		CMP.B	D1 , D2
		BEQ.B	loop_while
		CMP.B	#"a" , D1
		BCS.B	char1_ok
		CMP.B	#"z" , D1
		BHI.B	char1_ok
		BCLR	#5 , D1
char1_ok:
		CMP.B	#"a" , D2
		BCS.B	char2_ok
		CMP.B	#"z" , D2
		BHI.B	char2_ok
		BCLR	#5 , D2
char2_ok:
		CMP.B	D1 , D2
		BEQ.B	loop_while
		BHI.B	end_greater
end_less:
	RETURN -1
end_greater:
	RETURN 1
end_eq:
	RETURN 0

ENDPROC

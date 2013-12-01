OPT MODULE
OPT EXPORT
OPT PREPROCESS

EXPORT PROC getpath(currentdir:PTR TO LONG)
	DEF moins=0,str,pos,test

	test:=String(StrLen(currentdir))
	StrCopy(test,currentdir,ALL)
	IF test[StrLen(test)-1]<>":"
		IF test[StrLen(test)-1]="/" THEN SetStr(currentdir,StrLen(currentdir)-1)
		str:=String(StrLen(currentdir))
		pos:=String(StrLen(currentdir))
		moins:=0
		REPEAT
			MidStr(str,currentdir,0,StrLen(currentdir)-moins)
       			moins:=moins+1
		UNTIL StrCmp(RightStr(pos,str,1),'/',StrLen(str))=TRUE OR StrCmp(RightStr(pos,str,1),':',StrLen(str))=TRUE
		StrCopy(currentdir,str,ALL)
	ELSE
		StringF(currentdir,'')
	ENDIF
ENDPROC currentdir


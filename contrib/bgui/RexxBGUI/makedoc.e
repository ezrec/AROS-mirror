
CONST ZERO=0

ENUM ERR_NONE,ERR_ARGS,ERR_OPEN

RAISE ERR_ARGS IF ReadArgs()=NIL,
			ERR_OPEN IF Open()=ZERO

PROC main() HANDLE
	DEF rda=NIL,in=ZERO,out=ZERO,str[257]:STRING,c,q,tprefix[10]:STRING,n
	DEF a[2]:ARRAY OF LONG
	rda:=ReadArgs('FROM/A,TO/A',a,NIL)
	in:=Open(a[0],OLDFILE)
	out:=Open(a[1],NEWFILE)
	WHILE Fgets(in,str,256)
		IF StrCmp(str,'\t\t/* ',STRLEN)
			Fputs(out,'\n\e[1m')
			Fputs(out,str+STRLEN)
			Fputs(out,'\e[22m\n')
		ELSEIF StrCmp(str,'\t\t-> ',STRLEN)
			Fputs(out,str+STRLEN)
		ELSEIF StrCmp(str,'\t\t->\n')
			FputC(out,"\n")
		ELSEIF StrCmp(str,'-> ',STRLEN)
			Fputs(out,str+STRLEN)
		ELSEIF StrCmp(str,'->\n')
			FputC(out,"\n")
		ELSEIF StrCmp(str,'\tELSEIF stricmp(first,\a',STRLEN)
			q:=str+STRLEN
			StrCopy(tprefix,'')
			WHILE (c:=q[]++)<>"'" AND c DO StrAdd(tprefix,{c}+3,1)
			VfPrintf(out,'\n\e[1m\s_#? TAGS\e[22m\n\n',[tprefix])
		ELSEIF StrCmp(str,'\t\t\t\a',STRLEN)
			q:=str+STRLEN
			Fputs(out,tprefix)
			FputC(out,"_")
			n:=EstrLen(tprefix)+1
			WHILE (c:=q[]++)<>"'" AND c DO FputC(out,c) BUT n++
			WHILE n<30 DO FputC(out," ") BUT n++
			n:=0
			q:=str+InStr(str,'TAG_')+STRLEN
			WHILE (c:=q[]++)<>"," AND c DO FputC(out,c) BUT n++
			IF (q:=str+InStr(str,'->'))>str
				WHILE n<6 DO FputC(out," ") BUT n++
				q:=q+STRLEN
				WHILE (c:=q[]++)<>"\n" AND c DO FputC(out,c)
			ENDIF
			FputC(out,"\n")
		ENDIF
	ENDWHILE
EXCEPT DO
	Close(out)
	Close(in)
	IF rda THEN FreeArgs(rda)
	SELECT exception
		CASE ERR_ARGS; PrintFault(IoErr(),'MakeDoc cannot read args')
		CASE ERR_OPEN; PrintFault(IoErr(),'MakeDoc cannot open file')
	ENDSELECT
ENDPROC
